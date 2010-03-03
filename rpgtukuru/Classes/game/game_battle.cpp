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


GameBattle::GameBattle(kuto::Task* parent, GameSystem& gameSystem, const std::string& terrain, int enemyGroupId)
: kuto::Task(parent)
, gameSystem_(gameSystem), state_(kStateStart), stateCounter_(0), escapeNum_(0), escapeSuccess_(false)
, screenOffset_(0.f, 0.f), resultType_(kResultWin), firstAttack_(false), enableEscape_(true)
, turnNum_(1)
{
	map_ = GameBattleMap::createTask(this, gameSystem_, terrain);
	
	const CRpgLdb::EnemyGroup& group = gameSystem_.getRpgLdb().saEnemyGroup[enemyGroupId];
	for (int enemyIndex = 1; enemyIndex < group.placement.size(); enemyIndex++) {
		GameBattleEnemy* enemy = GameBattleEnemy::createTask(this, gameSystem_, group.placement[enemyIndex].enemyID);
		enemy->setPosition(kuto::Vector2(group.placement[enemyIndex].x, group.placement[enemyIndex].y));
		enemies_.push_back(enemy);
	}
	
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
					if (animationTargetIndex_ < attackedTargets_.size()) {
						attackedTargets_[animationTargetIndex_]->playDamageAnime();
					}
				}
				messageWindow_->setLineLimit(messageWindow_->getLineLimit() + 1);
			}
		} else {
			if (animationTargetIndex_ < attackedTargets_.size()) {
				if (!attackedTargets_[animationTargetIndex_]->isAnimated()) {
					if (attackedTargets_[animationTargetIndex_]->getStatus().isDead()
					&& !attackedTargets_[animationTargetIndex_]->isExcluded()) {
						attackedTargets_[animationTargetIndex_]->playDeadAnime();
					} else {
						animationTargetIndex_++;
						if (animationTargetIndex_ < attackedTargets_.size()) {
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
					while (currentAttacker_ < battleOrder_.size() && battleOrder_[currentAttacker_]->isExcluded()) {
						currentAttacker_++;
					}
					if (currentAttacker_ < battleOrder_.size()) {
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
			if (messageWindow_->getLineLimit() < messageWindow_->getMessageSize()) {
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
		messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.firstAttack);
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
		messageWindow_->addMessage(enemies_[i]->getName() + gameSystem_.getRpgLdb().term.battle.battleStart);
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
	
	if (escapeSuccess_)
		messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.escapeSuccess);
	else
		messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.escapeMiss);

	escapeNum_++;
}

void GameBattle::setLoseMessage()
{
	messageWindow_->clearMessages();
	messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.lose);
}

void GameBattle::setResultMessage()
{
	const CRpgLdb::Term& term = gameSystem_.getRpgLdb().term;
	messageWindow_->clearMessages();
	messageWindow_->addMessage(term.battle.win);
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
	sprintf(temp, "%d%s", exp, term.battle.getExp.c_str());
	messageWindow_->addMessage(temp);
	sprintf(temp, "%s%d%s%s", term.battle.getMoney[0].c_str(), money, term.shopParam.money.c_str(), term.battle.getMoney[1].c_str());
	messageWindow_->addMessage(temp);
	for (u32 i = 0; i < items.size(); i++) {
		sprintf(temp, "%s%s", gameSystem_.getRpgLdb().saItem[items[i]].name.c_str(), term.battle.getItem.c_str());
		messageWindow_->addMessage(temp);
	}
	
	for (u32 i = 0; i < players_.size(); i++) {
		int oldLevel = players_[i]->getStatus().getLevel();
		if (!players_[i]->isExcluded()) {
			players_[i]->getStatus().addExp(exp);
		}
		if (players_[i]->getStatus().getLevel() > oldLevel) {
			const GamePlayerInfo& player = players_[i]->getPlayerInfo();
			sprintf(temp, "%sは%s%d%s", player.name.c_str(), term.param.level.c_str(),
				players_[i]->getStatus().getLevel(), term.battle.levelUp.c_str());
			messageWindow_->addMessage(temp);
			for (u32 iLearn = 1; iLearn < player.baseInfo->learnSkill.size(); iLearn++) {
				const CRpgLdb::LearnSkill& learnSkill = player.baseInfo->learnSkill[iLearn];
				if (learnSkill.level > oldLevel && learnSkill.level <= players_[i]->getStatus().getLevel()) {
					const CRpgLdb::Skill& skill = gameSystem_.getRpgLdb().saSkill[learnSkill.skill];
					sprintf(temp, "%s%s", skill.name.c_str(), term.battle.getSkill.c_str());
					messageWindow_->addMessage(temp);
				}
			}
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
	const CRpgLdb::Term& term = gameSystem_.getRpgLdb().term;
	const CRpgLdb::Skill& skill = gameSystem_.getRpgLdb().saSkill[attacker->getAttackInfo().id];
	AttackResult result = attacker->getAttackResult(*target);
	attackResults_.push_back(result);
	if (result.miss) {
		switch (skill.messageMiss) {
		case 3:
			sprintf(temp, "%s%s", target->getName().c_str(), term.battle.attackMiss.c_str());
			break;
		default:
			sprintf(temp, "%s%s", target->getName().c_str(), term.battle.skillMiss[skill.messageMiss].c_str());
			break;
		}
		messageWindow_->addMessage(temp);
	} else {
		bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
		if (skill.statusDownHP) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.hp.c_str() , result.hpDamage, term.battle.paramCure.c_str());
				messageWindow_->addMessage(temp);
			} else {
				const char* damageMes = isTargetPlayer? 
					term.battle.friendDamage.c_str() : term.battle.enemyDamage.c_str();
				sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, damageMes);
				messageWindow_->addMessage(temp);
			}
		}
		if (skill.statusDownMP) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.mp.c_str(), result.mpDamage, term.battle.paramUp.c_str());
				messageWindow_->addMessage(temp);				
			} else {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.mp.c_str(), result.mpDamage, term.battle.paramDown.c_str());
				messageWindow_->addMessage(temp);
			}
		}
		if (skill.statusDownAttack) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.attack.c_str(), result.attack, term.battle.paramUp.c_str());
				messageWindow_->addMessage(temp);				
			} else {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.attack.c_str(), result.attack, term.battle.paramDown.c_str());
				messageWindow_->addMessage(temp);
			}
		}
		if (skill.statusDownDefence) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.defence.c_str(), result.defence, term.battle.paramUp.c_str());
				messageWindow_->addMessage(temp);				
			} else {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.defence.c_str(), result.defence, term.battle.paramDown.c_str());
				messageWindow_->addMessage(temp);
			}
		}
		if (skill.statusDownMagic) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.magic.c_str(), result.magic, term.battle.paramUp.c_str());
				messageWindow_->addMessage(temp);				
			} else {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.magic.c_str(), result.magic, term.battle.paramDown.c_str());
				messageWindow_->addMessage(temp);
			}
		}
		if (skill.statusDownSpeed) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.speed.c_str(), result.speed, term.battle.paramUp.c_str());
				messageWindow_->addMessage(temp);				
			} else {
				sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.speed.c_str(), result.speed, term.battle.paramDown.c_str());
				messageWindow_->addMessage(temp);
			}
		}
		for (u32 i = 0; i < result.badConditions.size(); i++) {
			std::strcpy(temp, "");
			const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[result.badConditions[i]];
			if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
				if (!cond.alreadyMessage.empty())
					sprintf(temp, "%s%s", target->getName().c_str(), cond.alreadyMessage.c_str());
			} else {
				if (target->getType() == GameBattleChara::kTypePlayer)
					sprintf(temp, "%s%s", target->getName().c_str(), cond.friendMessage.c_str());
				else
					sprintf(temp, "%s%s", target->getName().c_str(), cond.enemyMessage.c_str());
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
	
	if (!attacker->isActive()) {
		int limitBadConditionId = attacker->getWorstBadConditionId(true);
		if (limitBadConditionId > 0) {
			const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[limitBadConditionId];
			if (!cond.myTurnMessage.empty()) {
				messageWindow_->addMessage(attacker->getName() + cond.myTurnMessage);
			}
		}
		return;
	}
	
	switch (attacker->getAttackInfo().type) {
	case kAttackTypeAttack:
	case kAttackTypeDoubleAttack:
		{
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.normalAttack);
			GameBattleChara* target = attacker->getAttackInfo().target;
			if (target->isExcluded()) {
				target = getTargetRandom(attacker);
			}
			int maxAttack = (attacker->getAttackInfo().type == kAttackTypeDoubleAttack)? 2 : 1;
			for (int numAttack = 0; numAttack < maxAttack; numAttack++) {
				AttackResult result = attacker->getAttackResult(*target);
				attackResults_.push_back(result);
				if (result.miss) {
					sprintf(temp, "%s%s", target->getName().c_str(), gameSystem_.getRpgLdb().term.battle.attackMiss.c_str());
					messageWindow_->addMessage(temp);
				} else {
					bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
					if (result.critical) {
						if (attacker->getType() == GameBattleChara::kTypePlayer)
							messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.criticalFriend);
						else
							messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.criticalEnemy);
					}
					if (result.hpDamage == 0) {
						const char* damageMes = isTargetPlayer? 
							gameSystem_.getRpgLdb().term.battle.friendNoDamage.c_str() : gameSystem_.getRpgLdb().term.battle.enemyNoDamage.c_str();
						sprintf(temp, "%s%s", target->getName().c_str(), damageMes);
					} else {
						const char* damageMes = isTargetPlayer? 
							gameSystem_.getRpgLdb().term.battle.friendDamage.c_str() : gameSystem_.getRpgLdb().term.battle.enemyDamage.c_str();
						sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, damageMes);
					}
					messageWindow_->addMessage(temp);
					for (u32 i = 0; i < result.badConditions.size(); i++) {
						std::strcpy(temp, "");
						const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[result.badConditions[i]];
						if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
							if (!cond.alreadyMessage.empty())
								sprintf(temp, "%s%s", target->getName().c_str(), cond.alreadyMessage.c_str());
						} else {
							if (target->getType() == GameBattleChara::kTypePlayer)
								sprintf(temp, "%s%s", target->getName().c_str(), cond.friendMessage.c_str());
							else
								sprintf(temp, "%s%s", target->getName().c_str(), cond.enemyMessage.c_str());
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
			const CRpgLdb::Skill& skill = gameSystem_.getRpgLdb().saSkill[attacker->getAttackInfo().id];
			messageWindow_->addMessage(attacker->getName() + skill.message1);
			if (!skill.message2.empty())
				messageWindow_->addMessage(attacker->getName() + skill.message2);
			switch (skill.scope) {
			case CRpgLdb::kSkillScopeEnemySingle:
			case CRpgLdb::kSkillScopeFriendSingle:
				{
					GameBattleChara* target = attacker->getAttackInfo().target;
					if (target->isExcluded()) {
						target = getTargetRandom(attacker);
					}
					setAnimationMessageMagicSub(attacker, target);
				}
				break;
			case CRpgLdb::kSkillScopeMyself:
				setAnimationMessageMagicSub(attacker, attacker);
				break;
			case CRpgLdb::kSkillScopeEnemyAll:
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
			case CRpgLdb::kSkillScopeFriendAll:
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
			skillAnime_ = GameSkillAnime::createTask(this, gameSystem_, skill.anime);
		}
		break;
	case kAttackTypeItem:
		{
			const CRpgLdb::Item& item = gameSystem_.getRpgLdb().saItem[attacker->getAttackInfo().id];
			const CRpgLdb::Term& term = gameSystem_.getRpgLdb().term;
			GameBattleChara* target = attacker->getAttackInfo().target;
			AttackResult result = attacker->getAttackResult(*target);
			attackResults_.push_back(result);
			switch (item.type) {
			case CRpgLdb::kItemTypeMedicine:
				sprintf(temp, "%sは%s%s", attacker->getName().c_str(), item.name.c_str(), term.battle.useItem.c_str());
				messageWindow_->addMessage(temp);
				if (result.hpDamage != 0) {
					sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.hp.c_str() , result.hpDamage, term.battle.paramCure.c_str());
					messageWindow_->addMessage(temp);
				}
				if (result.mpDamage != 0) {
					sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), term.param.mp.c_str(), result.mpDamage, term.battle.paramUp.c_str());
					messageWindow_->addMessage(temp);				
				}
				target->getStatus().addDamage(result);
				attackedTargets_.push_back(target);
				break;
			case CRpgLdb::kItemTypeSpecial:
				sprintf(temp, "%sは%s%s", attacker->getName().c_str(), item.name.c_str(), term.battle.useItem.c_str());
				messageWindow_->addMessage(temp);
				break;
			}
			attacker->getStatus().setCharged(false);
		}
		break;
	case kAttackTypeDefence:
		{
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.guard);
		}
		break;
	case kAttackTypeCharge:
		{
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.charge);
			attacker->getStatus().setCharged(true);
		}
		break;
	case kAttackTypeWaitAndSee:
		{
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.seek);
		}
		break;
	case kAttackTypeSuicideBombing:
		{
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.suicideBombing);
			attacker->setExcluded(true);
			
			for (u32 i = 0; i < players_.size(); i++) {
				if (players_[i]->isExcluded())
					continue;
				GameBattleChara* target = players_[i];
				AttackResult result = attacker->getAttackResult(*target);
				attackResults_.push_back(result);
				if (result.miss) {
					sprintf(temp, "%s%s", target->getName().c_str(), gameSystem_.getRpgLdb().term.battle.attackMiss.c_str());
					messageWindow_->addMessage(temp);
				} else {
					bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
					if (result.critical) {
						if (attacker->getType() == GameBattleChara::kTypePlayer)
							messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.criticalFriend);
						else
							messageWindow_->addMessage(gameSystem_.getRpgLdb().term.battle.criticalEnemy);
					}
					if (result.hpDamage == 0) {
						const char* damageMes = isTargetPlayer? 
							gameSystem_.getRpgLdb().term.battle.friendNoDamage.c_str() : gameSystem_.getRpgLdb().term.battle.enemyNoDamage.c_str();
						sprintf(temp, "%s%s", target->getName().c_str(), damageMes);
					} else {
						const char* damageMes = isTargetPlayer? 
							gameSystem_.getRpgLdb().term.battle.friendDamage.c_str() : gameSystem_.getRpgLdb().term.battle.enemyDamage.c_str();
						sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, damageMes);
					}
					messageWindow_->addMessage(temp);
					for (u32 i = 0; i < result.badConditions.size(); i++) {
						std::strcpy(temp, "");
						const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[result.badConditions[i]];
						if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
							if (!cond.alreadyMessage.empty())
								sprintf(temp, "%s%s", target->getName().c_str(), cond.alreadyMessage.c_str());
						} else {
							if (target->getType() == GameBattleChara::kTypePlayer)
								sprintf(temp, "%s%s", target->getName().c_str(), cond.friendMessage.c_str());
							else
								sprintf(temp, "%s%s", target->getName().c_str(), cond.enemyMessage.c_str());
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
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.escapeEnemy);
			attacker->playDeadAnime();
			//attacker->setExcluded(true);
		}
		break;
	case kAttackTypeTransform:
		{
			messageWindow_->addMessage(attacker->getName() + gameSystem_.getRpgLdb().term.battle.transform);
		}
		break;
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





