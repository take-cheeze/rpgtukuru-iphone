/**
 * @file
 * @brief Game Battle
 * @author project.kuto
 */

#include <algorithm>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_error.h>

#include "game_battle.h"
#include "game_battle_map.h"
#include "game_battle_chara.h"
#include "game_battle_menu.h"
#include "game_message_window.h"
#include "game_skill_anime.h"
#include "game_inventory.h"

using namespace rpg2kLib::model;
using namespace rpg2kLib::structure;


GameBattle::GameBattle(kuto::Task* parent, GameSystem& gameSystem, const std::string& terrain, int enemyGroupId)
: kuto::Task(parent)
, gameSystem_(gameSystem)
, state_(kStateStart)
, stateCounter_(0)
, firstAttack_(false)
, enableEscape_(true)
, escapeSuccess_(false)
, escapeNum_(0)
, screenOffset_(0.f, 0.f)
, resultType_(kResultWin)
, turnNum_(1)
{
	map_ = GameBattleMap::createTask(this, gameSystem_, terrain);

	const Array1D& group = gameSystem_.getRpgLdb().getEnemyGroup()[enemyGroupId];
	const Array2D& enemyList = group[2];
	for(Array2D::Iterator it = enemyList.begin(); it != enemyList.end(); ++it) {
		const Array1D& info = it.second();

		GameBattleEnemy* enemy = GameBattleEnemy::createTask( this, gameSystem_, info[1] );
		enemy->setPosition(kuto::Vector2( (int)info[2], (int)info[3] ) );

		enemies_.push_back(enemy);
	}
/*
	for (int enemyIndex = 1; enemyIndex < group.placement.size(); enemyIndex++) {
		GameBattleEnemy* enemy = GameBattleEnemy::createTask(this, gameSystem_, group.placement[enemyIndex].enemyID);
		enemy->setPosition(kuto::Vector2(group.placement[enemyIndex].x, group.placement[enemyIndex].y));
		enemies_.push_back(enemy);
	}
 */

	menu_ = GameBattleMenu::createTask(this);
	messageWindow_ = GameMessageWindow::createTask(this, gameSystem_);
	messageWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	messageWindow_->setSize(kuto::Vector2(320.f, 80.f));
	messageWindow_->setEnableSkip(false);
	
	map_->pauseUpdate(true);
	menu_->pauseUpdate(true);
	messageWindow_->pauseUpdate(true);
}

GameBattle::~GameBattle()
{
}

void GameBattle::addPlayer(int playerId, GameCharaStatus& status)
{
	GameBattlePlayer* player = GameBattlePlayer::createTask(this, gameSystem_, playerId, status);
	players_.push_back(player);	
}

bool GameBattle::initialize()
{
	if (isInitializedChildren()) {
		map_->pauseUpdate(false);
		menu_->pauseUpdate(false);
		messageWindow_->pauseUpdate(false);
		setState(kStateStart);
		return true;
	}
	return false;
}

void GameBattle::update()
{
	switch (state_) {
	case kStateStart:
		stateCounter_++;
		if (stateCounter_ > 60) {
			setState(firstAttack_? kStateFirstAttack : kStateMenu);
		}
		break;
	case kStateFirstAttack:
		stateCounter_++;
		if (stateCounter_ > 60) {
			setState(kStateMenu);
		}
		break;
	case kStateMenu:
		if (menu_->decided()) {
			switch (menu_->getPartyCommand()) {
			case GameBattleMenu::kPartyCommandManual:
				setState(kStateAnimation);
				break;
			case GameBattleMenu::kPartyCommandAuto:
				for (u32 i = 0; i < players_.size(); i++) {
					players_[i]->setAttackInfoAuto(enemies_, players_, turnNum_);
				}
				setState(kStateAnimation);
				break;
			case GameBattleMenu::kPartyCommandEscape:
				setState(kStateEscape);
				break;
			}
			turnNum_++;
		}
		break;
	case kStateEscape:
		stateCounter_++;
		if (stateCounter_ > 60) {
			if (escapeSuccess_) {
				setState(kStateEnd);
			} else {
				for (u32 i = 0; i < players_.size(); i++) {
					AttackInfo info;
					info.target = NULL;
					info.type = kAttackTypeNone;
					info.id = 0;
					players_[i]->setAttackInfo(info);
				}
				setState(kStateAnimation);
			}
		}		
		break;
	case kStateAnimation:
		if (animationTargetIndex_ < 0) {
			if ((!skillAnime_ || skillAnime_->isFinished()) && !battleOrder_[currentAttacker_]->isAnimated()) {
				if (skillAnime_) {
					skillAnime_->release();
					skillAnime_ = NULL;
				}
				if (animationTargetIndex_ < 0) {
					animationTargetIndex_ = 0;
					if ( (uint)animationTargetIndex_ < attackedTargets_.size() ) {
						attackedTargets_[animationTargetIndex_]->playDamageAnime();
					}
				}
				messageWindow_->setLineLimit(messageWindow_->getLineLimit() + 1);
			}
		} else {
			if ( (uint)animationTargetIndex_ < attackedTargets_.size() ) {
				if (!attackedTargets_[animationTargetIndex_]->isAnimated()) {
					if (attackedTargets_[animationTargetIndex_]->getStatus().isDead()
					&& !attackedTargets_[animationTargetIndex_]->isExcluded()) {
						attackedTargets_[animationTargetIndex_]->playDeadAnime();
					} else {
						animationTargetIndex_++;
						if ( (uint)animationTargetIndex_ < attackedTargets_.size() ) {
							attackedTargets_[animationTargetIndex_]->playDamageAnime();
						}
					}
					messageWindow_->setLineLimit(messageWindow_->getLineLimit() + 1);
				}
			} else {
				if (isLose()) {
					setState(kStateLose);
				} else if (isWin()) {
					setState(kStateResult);
				} else {
					currentAttacker_++;
					while ( (uint)currentAttacker_ < battleOrder_.size() && battleOrder_[currentAttacker_]->isExcluded()) {
						currentAttacker_++;
					}
					if ( (uint)currentAttacker_ < battleOrder_.size() ) {
						stateCounter_ = 0;
						messageWindow_->reset();
						setAnimationMessage();
					} else {
						setState(kStateMenu);
					}
				}
			}
		}
		break;
	case kStateLose:
		stateCounter_++;
		if (messageWindow_->clicked()) {
			setState(kStateEnd);
		}
		break;
	case kStateResult:
		stateCounter_++;
		if (stateCounter_ > 20) {
			if ( (uint)messageWindow_->getLineLimit() < messageWindow_->getMessageSize()) {
				messageWindow_->setLineLimit(messageWindow_->getLineLimit() + 1);
				stateCounter_ = 0;
			} else {
				messageWindow_->setEnableClick(true);
				if (messageWindow_->clicked()) {
					setState(kStateEnd);
				}
			}
		}
		break;
	case kStateEnd:
		break;
	}
}

void GameBattle::draw()
{
}

void GameBattle::setState(State newState)
{
	state_ = newState;
	stateCounter_ = 0;
	
	menu_->freeze(true);
	messageWindow_->freeze(true);
	messageWindow_->setEnableClick(false);
	switch (state_) {
	case kStateStart:
		messageWindow_->reset();
		messageWindow_->freeze(false);
		setStartMessage();
		break;
	case kStateFirstAttack:
		messageWindow_->reset();
		messageWindow_->freeze(false);
		messageWindow_->clearMessages();
		messageWindow_->addMessage(gameSystem_.getRpgLdb().getVocabulary()[2]);
		break;
	case kStateMenu:
		for (u32 i = 0; i < players_.size(); i++) {
			if (!players_[i]->isExcluded())
				players_[i]->updateBadCondition();
		}
		for (u32 i = 0; i < enemies_.size(); i++) {
			if (!enemies_[i]->isExcluded())
				enemies_[i]->updateBadCondition();
		}
		menu_->reset();
		menu_->freeze(false);
		break;
	case kStateEscape:
		resultType_ = kResultEscape;
		messageWindow_->reset();
		messageWindow_->freeze(false);
		setEscapeMessage();
		firstAttack_ = false;
		break;
	case kStateAnimation:
		messageWindow_->reset();
		messageWindow_->freeze(false);
		for (u32 i = 0; i < players_.size(); i++) {
			if (players_[i]->isExecAI())
				players_[i]->setAttackInfoAuto(enemies_, players_, turnNum_);
		}
		if (!firstAttack_) {
			for (u32 i = 0; i < enemies_.size(); i++) {
				enemies_[i]->setAttackInfoAuto(players_, enemies_, turnNum_);
			}
		}
		calcBattleOrder();
		setAnimationMessage();
		messageWindow_->setUseAnimation(false);
		firstAttack_ = false;
		break;
	case kStateLose:
		resultType_ = kResultLose;
		messageWindow_->reset();
		messageWindow_->setEnableClick(true);
		messageWindow_->freeze(false);
		setLoseMessage();
		break;
	case kStateResult:
		resultType_ = kResultWin;
		messageWindow_->reset();
		messageWindow_->freeze(false);
		setResultMessage();
		break;
	case kStateEnd:
		messageWindow_->freeze(false);
		for (u32 playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
			GameCharaStatus& destStatus = gameSystem_.getPlayerStatus(players_[playerIndex]->getPlayerId());
			destStatus = players_[playerIndex]->getStatus();
			destStatus.resetBattle();
		}
		break;
	}
}

void GameBattle::setStartMessage()
{
	messageWindow_->clearMessages();
	for (u32 i = 0; i < enemies_.size(); i++) {
		messageWindow_->addMessage(enemies_[i]->getName() + gameSystem_.getRpgLdb().getVocabulary()[1]);
	}
}

void GameBattle::setEscapeMessage()
{
	messageWindow_->clearMessages();
	
	float playersSpeed = 0.f;
	float enemiesSpeed = 0.f;
	for (u32 i = 0; i < players_.size(); i++) {
		playersSpeed += players_[i]->getStatus().getSpeed();
	}
	playersSpeed /= players_.size();
	for (u32 i = 0; i < enemies_.size(); i++) {
		enemiesSpeed += enemies_[i]->getStatus().getSpeed();
	}
	enemiesSpeed /= enemies_.size();
	
	int escapeRatio = (int)((1.5f - (enemiesSpeed / playersSpeed)) * 100.f) + escapeNum_ * 10;
	escapeSuccess_ = kuto::random(100) < escapeRatio;
	if (gameSystem_.getConfig().alwaysEscape)
		escapeSuccess_ = true;
	if (firstAttack_)
		escapeSuccess_ = true;
	if (!enableEscape_)
		escapeSuccess_ = false;
	
	messageWindow_->
		addMessage( gameSystem_.getRpgLdb().getVocabulary()[escapeSuccess_ ? 0x03 : 0x04] );

	escapeNum_++;
}

void GameBattle::setLoseMessage()
{
	messageWindow_->clearMessages();
	messageWindow_->addMessage(gameSystem_.getRpgLdb().getVocabulary()[6]);
}

void GameBattle::setResultMessage()
{
	const Array1D& voc = gameSystem_.getRpgLdb().getVocabulary();
	messageWindow_->clearMessages();
	messageWindow_->addMessage(voc[5]);
	int exp = 0;
	int money = 0;
	std::vector<int> items;
	for (u32 i = 0; i < enemies_.size(); i++) {
		if (enemies_[i]->getStatus().getHp() <= 0) {
			exp += enemies_[i]->getResultExp();
			money += enemies_[i]->getResultMoney();
			int item = enemies_[i]->getResultItem();
			if (item > 0)
				items.push_back(item);
		}
	}
	char temp[256];
	sprintf( temp, "%d%s", exp, voc[0x07].get_string().c_str() );
	messageWindow_->addMessage(temp);
	sprintf( temp, "%s%d%s%s",
		voc[0x08].get_string().c_str(), money,
		voc[0x5f].get_string().c_str(), voc[0x09].get_string().c_str()
	);
	messageWindow_->addMessage(temp);
	const Array2D& itemData = gameSystem_.getRpgLdb().getItem();
	for (u32 i = 0; i < items.size(); i++) {
		sprintf(temp, "%s%s",
			itemData[ items[i] ][1].get_string().c_str(),
			voc[0x0a].get_string().c_str()
		);
		messageWindow_->addMessage(temp);
	}
	
	for (u32 i = 0; i < players_.size(); i++) {
		int oldLevel = players_[i]->getStatus().getLevel();
		if (!players_[i]->isExcluded()) {
			players_[i]->getStatus().addExp(exp);
		}
		if (players_[i]->getStatus().getLevel() > oldLevel) {
			const Array1D& player = players_[i]->getPlayerInfo();
			sprintf(temp, "%sは%s%d%s",
				player[1].get_string().c_str(), voc[0x7b].get_string().c_str(),
				players_[i]->getStatus().getLevel(), voc[0x24].get_string().c_str()
			);
			messageWindow_->addMessage(temp);

			const Array2D& levelList = player[63];
			for( Array2D::Iterator it = levelList.begin(); it != levelList.end(); ++it ) {
				int level = it.second()[1];
				if (level > oldLevel && level <= players_[i]->getStatus().getLevel()) {
					const Array1D& skill = gameSystem_.getRpgLdb().getSkill()[it.second()[2].get_int()];
					sprintf(temp, "%s%s",
						skill[1].get_string().c_str(), voc[0x25].get_string().c_str()
					);
					messageWindow_->addMessage(temp);
				}
			}
/*
			for (u32 iLearn = 1; iLearn < player.learnSkill.size(); iLearn++) {
				const DataBase::LearnSkill& learnSkill = player.learnSkill[iLearn];
				if (learnSkill.level > oldLevel && learnSkill.level <= players_[i]->getStatus().getLevel()) {
					const Array1D& skill = gameSystem_.getRpgLdb().getSkill()[learnSkill.skill];
					sprintf(temp, "%s%s", skill.name.c_str(), term.battle.getSkill.c_str());
					messageWindow_->addMessage(temp);
				}
			}
 */
		}
	}
	gameSystem_.getInventory()->addMoney(money);
	for (u32 i = 0; i < items.size(); i++) {
		gameSystem_.getInventory()->setItemNum(items[i], 1);
	}
	messageWindow_->setLineLimit(1);
}

GameBattleChara* GameBattle::getTargetRandom(GameBattleChara* attacker)
{
	std::vector<GameBattleChara*> charaList;
	if (attacker->getType() == GameBattleChara::kTypePlayer) {
		for (u32 i = 0; i < enemies_.size(); i++) {
			if (!enemies_[i]->isExcluded())
				charaList.push_back(enemies_[i]);
		}
	} else {
		for (u32 i = 0; i < players_.size(); i++) {
			if (!players_[i]->isExcluded())
				charaList.push_back(players_[i]);
		}		
	}
	kuto_assert(!charaList.empty());
	return charaList[kuto::random((int)charaList.size())];
}

void GameBattle::setAnimationMessageMagicSub(GameBattleChara* attacker, GameBattleChara* target)
{
	char temp[256];
	const Array1D& voc = gameSystem_.getRpgLdb().getVocabulary();
	const Array1D& skill = gameSystem_.getRpgLdb().getSkill()[attacker->getAttackInfo().id];
	AttackResult result = attacker->getAttackResult(*target);
	attackResults_.push_back(result);
	if (result.miss) {
		sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(voc[ 0x18 + static_cast< int >(skill[7]) ]).c_str());
		messageWindow_->addMessage(temp);
	} else {
		bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
		if ( static_cast< bool >(skill[31]) ) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), static_cast< string& >(voc[0x7c]).c_str() , result.hpDamage, static_cast< string& >(voc[0x1d]).c_str());
			} else {
				const char* damageMes = static_cast< string& >(voc[isTargetPlayer ? 0x16 : 0x14]).c_str();
				sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, damageMes);
			}
			messageWindow_->addMessage(temp);
		}
		if ( static_cast< bool >(skill[32]) ) {
			sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), static_cast< string& >(voc[0x7d]).c_str(), result.mpDamage, static_cast< string& >(voc[result.cure ? 0x1e : 0x1f]).c_str());
			messageWindow_->addMessage(temp);
		}
		for(uint i = 0; i < 4; i++) {
			if ( static_cast< bool >(skill[33+i]) ) {
				sprintf( temp, "%sの%sが%d%s",
					target->getName().c_str(),
					static_cast< string& >(voc[0x84+i]).c_str(),
					result.speed,
					static_cast< string& >(voc[result.cure ? 0x1e : 0x1f]).c_str()
				);
				messageWindow_->addMessage(temp);
			}
		}
		for (u32 i = 0; i < result.badConditions.size(); i++) {
			std::strcpy(temp, "");
			const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[result.badConditions[i]];
			if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
				if ( cond.exists(53) )
					sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[53]).c_str());
			} else {
				sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[(target->getType() == GameBattleChara::kTypePlayer) ? 51 : 52]).c_str());
			}
			if (std::strlen(temp) > 0)
				messageWindow_->addMessage(temp);
		}
	}
	target->getStatus().addDamage(result);
	attackedTargets_.push_back(target);
}

void GameBattle::setAnimationMessage()
{
	char temp[256];
	messageWindow_->clearMessages();
	messageWindow_->setLineLimit(1);
	attackResults_.clear();
	attackedTargets_.clear();
	skillAnime_ = NULL;
	animationTargetIndex_ = -1;
	if (battleOrder_.empty())
		return;
	GameBattleChara* attacker = battleOrder_[currentAttacker_];

	const Array1D& voc = gameSystem_.getRpgLdb().getVocabulary();

	if (!attacker->isActive()) {
		int limitBadConditionId = attacker->getWorstBadConditionId(true);
		if (limitBadConditionId > 0) {
			const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[limitBadConditionId];
			if ( cond.exists(51) ) {
				messageWindow_->addMessage( attacker->getName() + static_cast< string& >(cond[51]) );
			}
		}
		return;
	}
	
	switch (attacker->getAttackInfo().type) {
	case kAttackTypeAttack:
	case kAttackTypeDoubleAttack:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string& >(voc[0x0b]) );
			GameBattleChara* target = attacker->getAttackInfo().target;
			if (target->isExcluded()) {
				target = getTargetRandom(attacker);
			}
			int maxAttack = (attacker->getAttackInfo().type == kAttackTypeDoubleAttack)? 2 : 1;
			for (int numAttack = 0; numAttack < maxAttack; numAttack++) {
				AttackResult result = attacker->getAttackResult(*target);
				attackResults_.push_back(result);
				if (result.miss) {
					sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(voc[0x1b]).c_str());
					messageWindow_->addMessage(temp);
				} else {
					bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
					if (result.critical) {
						messageWindow_->addMessage(voc[isTargetPlayer ? 0x0c : 0x0d]);
					}
					if (result.hpDamage == 0) {
						const char* damageMes = static_cast< string& >(voc[isTargetPlayer ? 0x17 : 0x15]).c_str();
						sprintf(temp, "%s%s", target->getName().c_str(), damageMes);
					} else {
						const char* damageMes = static_cast< string& >(voc[isTargetPlayer ? 0x16 : 0x14]).c_str();
						sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, damageMes);
					}
					messageWindow_->addMessage(temp);
					for (u32 i = 0; i < result.badConditions.size(); i++) {
						std::strcpy(temp, "");
						const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[result.badConditions[i]];
						if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
							if ( cond.exists(53) )
								sprintf( temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[53]).c_str() );
						} else {
							sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[(target->getType() == GameBattleChara::kTypePlayer) ? 51 : 52]).c_str());
						}
						if (std::strlen(temp) > 0)
							messageWindow_->addMessage(temp);					
					}
				}
				target->getStatus().addDamage(result);
				attackedTargets_.push_back(target);
				if (target->getStatus().isDead())
					break;
			}
			attacker->getStatus().setCharged(false);
			skillAnime_ = GameSkillAnime::createTask(this, gameSystem_, attacker->getStatus().getAttackAnime());
		}
		break;
	case kAttackTypeSkill:
		{
			const Array1D& skill = gameSystem_.getRpgLdb().getSkill()[attacker->getAttackInfo().id];
			messageWindow_->addMessage(attacker->getName() + static_cast< string& >(skill[3]) );
			if ( skill.exists(4) )
				messageWindow_->addMessage(attacker->getName() + static_cast< string& >(skill[4]) );

			switch ( static_cast< int >(skill[12]) ) {
			case DataBase::kSkillScopeEnemySingle:
			case DataBase::kSkillScopeFriendSingle:
				{
					GameBattleChara* target = attacker->getAttackInfo().target;
					if (target->isExcluded()) {
						target = getTargetRandom(attacker);
					}
					setAnimationMessageMagicSub(attacker, target);
				}
				break;
			case DataBase::kSkillScopeMyself:
				setAnimationMessageMagicSub(attacker, attacker);
				break;
			case DataBase::kSkillScopeEnemyAll:
				{
					if (attacker->getType() == GameBattleChara::kTypePlayer) {
						for (u32 i = 0; i < enemies_.size(); i++) {
							if (!enemies_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, enemies_[i]);
						}
					} else {
						for (u32 i = 0; i < players_.size(); i++) {
							if (!players_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, players_[i]);
						}
					}
				}
				break;
			case DataBase::kSkillScopeFriendAll:
				{
					if (attacker->getType() == GameBattleChara::kTypePlayer) {
						for (u32 i = 0; i < players_.size(); i++) {
							if (!players_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, players_[i]);
						}
					} else {
						for (u32 i = 0; i < enemies_.size(); i++) {
							if (!enemies_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, enemies_[i]);
						}
					}
				}
				break;
			}
			attacker->getStatus().setCharged(false);
			skillAnime_ = GameSkillAnime::createTask(this, gameSystem_, skill[14]);
		}
		break;
	case kAttackTypeItem:
		{
			const Array1D& item = gameSystem_.getRpgLdb().getItem()[attacker->getAttackInfo().id];
			const Array1D& voc = gameSystem_.getRpgLdb().getVocabulary();

			GameBattleChara* target = attacker->getAttackInfo().target;
			AttackResult result = attacker->getAttackResult(*target);
			attackResults_.push_back(result);
			switch ( static_cast< int >(item[3]) ) {
			case DataBase::kItemTypeMedicine:
				sprintf(temp, "%sは%s%s", attacker->getName().c_str(), static_cast< string& >(item[1]).c_str(), static_cast< string& >(voc[0x1c]).c_str());
				messageWindow_->addMessage(temp);
				if (result.hpDamage) {
					sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), static_cast< string& >(voc[0x7c]).c_str(), result.hpDamage, static_cast< string& >(voc[0x1d]).c_str());
					messageWindow_->addMessage(temp);
				}
				if (result.mpDamage) {
					sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), static_cast< string& >(voc[0x7d]).c_str(), result.mpDamage, static_cast< string& >(voc[0x1e]).c_str());
					messageWindow_->addMessage(temp);
				}
				target->getStatus().addDamage(result);
				attackedTargets_.push_back(target);
				break;
			case DataBase::kItemTypeSpecial:
				sprintf(temp, "%sは%s%s", attacker->getName().c_str(), static_cast< string& >(item[1]).c_str(), static_cast< string& >(voc[0x1c]).c_str());
				messageWindow_->addMessage(temp);
				break;
			}
			attacker->getStatus().setCharged(false);
		}
		break;
	case kAttackTypeDefence:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string& >(voc[0x0e]) );
		}
		break;
	case kAttackTypeCharge:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string& >(voc[0x10]) );
			attacker->getStatus().setCharged(true);
		}
		break;
	case kAttackTypeWaitAndSee:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string& >(voc[0x0f]) );
		}
		break;
	case kAttackTypeSuicideBombing:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string& >(voc[0x11]) );
			attacker->setExcluded(true);
			
			for (u32 i = 0; i < players_.size(); i++) {
				if (players_[i]->isExcluded())
					continue;
				GameBattleChara* target = players_[i];
				AttackResult result = attacker->getAttackResult(*target);
				attackResults_.push_back(result);
				if (result.miss) {
					sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(voc[0x1b]).c_str());
					messageWindow_->addMessage(temp);
				} else {
					bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
					if (result.critical) {
						messageWindow_->addMessage(voc[isTargetPlayer ? 0x0c : 0x0d]);
					}
					if (result.hpDamage == 0) {
						const char* damageMes =
							static_cast< string& >(voc[isTargetPlayer ? 0x17 : 0x15]).c_str();
						sprintf(temp, "%s%s", target->getName().c_str(), damageMes);
					} else {
						const char* damageMes = static_cast< string& >(voc[isTargetPlayer ? 0x16 : 0x15]).c_str();
						sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, damageMes);
					}
					messageWindow_->addMessage(temp);
					for (u32 i = 0; i < result.badConditions.size(); i++) {
						std::strcpy(temp, "");
						const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[result.badConditions[i]];
						if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
							if ( !static_cast< string& >(cond[53]).empty() )
								sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[53]).c_str());
						} else {
							if (target->getType() == GameBattleChara::kTypePlayer)
								sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[51]).c_str());
							else
								sprintf(temp, "%s%s", target->getName().c_str(), static_cast< string& >(cond[52]).c_str());
						}
						if (std::strlen(temp) > 0)
							messageWindow_->addMessage(temp);
					}
				}
				target->getStatus().addDamage(result);
				attacker->getStatus().setCharged(false);
				attackedTargets_.push_back(target);
			}
		}
		break;
	case kAttackTypeEscape:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string >(voc[0x12]) );
			attacker->playDeadAnime();
			//attacker->setExcluded(true);
		}
		break;
	case kAttackTypeTransform:
		{
			messageWindow_->addMessage( attacker->getName() + static_cast< string >(voc[0x13]) );
		}
		break;
	default: break;
	}

	if (skillAnime_) {
		for (u32 i = 0; i < attackedTargets_.size(); i++) {
			if (attackedTargets_[i]->getType() == GameBattleChara::kTypeEnemy)
				skillAnime_->addEnemy(static_cast<GameBattleEnemy*>(attackedTargets_[i]));
		}
		skillAnime_->play();
	}
}


static bool sortBattleOrderFunc(const GameBattleChara* lhs, const GameBattleChara* rhs)
{
	return lhs->getAttackPriority() > rhs->getAttackPriority();
}

void GameBattle::calcBattleOrder()
{
	currentAttacker_ = 0;
	battleOrder_.clear();
	for (u32 i = 0; i < players_.size(); i++) {
		if (players_[i]->getAttackInfo().type != kAttackTypeNone && !players_[i]->isExcluded())
			battleOrder_.push_back(players_[i]);
	}
	if (!firstAttack_) {
		for (u32 i = 0; i < enemies_.size(); i++) {
			if (enemies_[i]->getAttackInfo().type != kAttackTypeNone && !enemies_[i]->isExcluded())
				battleOrder_.push_back(enemies_[i]);
		}
	}
	// sort
	for (u32 i = 0; i < battleOrder_.size(); i++) {
		battleOrder_[i]->setAttackPriorityOffset(kuto::random(0.1f));		// add random offset
	}
	std::sort(battleOrder_.begin(), battleOrder_.end(), sortBattleOrderFunc);
}

bool GameBattle::isWin()
{
	for (u32 i = 0; i < enemies_.size(); i++) {
		if (!enemies_[i]->isExcluded())
			return false;
	}
	return true;
}

bool GameBattle::isLose()
{
	for (u32 i = 0; i < players_.size(); i++) {
		if (!players_[i]->isExcluded())
			return false;
	}
	return true;
}





