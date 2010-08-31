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


GameBattle::GameBattle(kuto::Task* parent, rpg2k::model::Project& gameSystem, const std::string& terrain, int enemyGroupId)
: kuto::Task(parent)
, gameSystem_(gameSystem), state_(kStateStart), stateCounter_(0)
, firstAttack_(false)
, enableEscape_(true), escapeSuccess_(false), escapeNum_(0)
, screenOffset_(0.f, 0.f), resultType_(kResultWin)
, turnNum_(1)
{
	map_ = GameBattleMap::createTask(this, gameSystem_, terrain);

	const rpg2k::structure::Array1D& group = gameSystem_.getLDB().enemyGroup()[enemyGroupId];
	const rpg2k::structure::Array2D& enemyEnum = group[2];
	for (rpg2k::structure::Array2D::Iterator it = enemyEnum.begin(); it != enemyEnum.end(); ++it) {
		if( !it.second().exists() ) continue;

		GameBattleEnemy* enemy = GameBattleEnemy::createTask(this, gameSystem_, it.second()[1].get<int>());
		enemy->setPosition(kuto::Vector2(it.second()[2].get<int>(), it.second()[3].get<int>()));
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
				for (uint i = 0; i < players_.size(); i++) {
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
				for (uint i = 0; i < players_.size(); i++) {
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
					if (animationTargetIndex_ < (int)attackedTargets_.size()) {
						attackedTargets_[animationTargetIndex_]->playDamageAnime();
					}
				}
				messageWindow_->setLineLimit(messageWindow_->getLineLimit() + 1);
			}
		} else {
			if (animationTargetIndex_ < (int)attackedTargets_.size()) {
				if (!attackedTargets_[animationTargetIndex_]->isAnimated()) {
					if (attackedTargets_[animationTargetIndex_]->getStatus().isDead()
					&& !attackedTargets_[animationTargetIndex_]->isExcluded()) {
						attackedTargets_[animationTargetIndex_]->playDeadAnime();
					} else {
						animationTargetIndex_++;
						if (animationTargetIndex_ < (int)attackedTargets_.size()) {
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
					while (currentAttacker_ < (int)battleOrder_.size() && battleOrder_[currentAttacker_]->isExcluded()) {
						currentAttacker_++;
					}
					if (currentAttacker_ < (int)battleOrder_.size()) {
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
			if (messageWindow_->getLineLimit() < (int)messageWindow_->getMessageSize()) {
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
		messageWindow_->addLine(gameSystem_.getLDB().vocabulary(2).toSystem());
		break;
	case kStateMenu:
		for (uint i = 0; i < players_.size(); i++) {
			if (!players_[i]->isExcluded())
				players_[i]->updateBadCondition();
		}
		for (uint i = 0; i < enemies_.size(); i++) {
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
		for (uint i = 0; i < players_.size(); i++) {
			if (players_[i]->isExecAI())
				players_[i]->setAttackInfoAuto(enemies_, players_, turnNum_);
		}
		if (!firstAttack_) {
			for (uint i = 0; i < enemies_.size(); i++) {
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
/*
		for (uint playerIndex = 0; playerIndex < players_.size(); playerIndex++) {
			GameCharaStatus& destStatus = gameSystem_.getPlayerStatus(players_[playerIndex]->getPlayerId());
			destStatus = players_[playerIndex]->getStatus();
			destStatus.resetBattle();
		}
 */
		break;
	}
}

void GameBattle::setStartMessage()
{
	messageWindow_->clearMessages();
	for (uint i = 0; i < enemies_.size(); i++) {
		messageWindow_->addLine(enemies_[i]->getName() + gameSystem_.getLDB().vocabulary(1).toSystem());
	}
}

void GameBattle::setEscapeMessage()
{
	messageWindow_->clearMessages();

	float playersSpeed = 0.f;
	float enemiesSpeed = 0.f;
	for (uint i = 0; i < players_.size(); i++) {
		playersSpeed += players_[i]->getStatus().getSpeed();
	}
	playersSpeed /= players_.size();
	for (uint i = 0; i < enemies_.size(); i++) {
		enemiesSpeed += enemies_[i]->getStatus().getSpeed();
	}
	enemiesSpeed /= enemies_.size();

	int escapeRatio = (int)((1.5f - (enemiesSpeed / playersSpeed)) * 100.f) + escapeNum_ * 10;
	escapeSuccess_ = kuto::random(100) < escapeRatio;
	// if (gameSystem_.getConfig().alwaysEscape)
	//	escapeSuccess_ = true;
	if (firstAttack_)
		escapeSuccess_ = true;
	if (!enableEscape_)
		escapeSuccess_ = false;

	if (escapeSuccess_)
		messageWindow_->addLine(gameSystem_.getLDB().vocabulary(3).toSystem());
	else
		messageWindow_->addLine(gameSystem_.getLDB().vocabulary(4).toSystem());

	escapeNum_++;
}

void GameBattle::setLoseMessage()
{
	messageWindow_->clearMessages();
	messageWindow_->addLine(gameSystem_.getLDB().vocabulary(6).toSystem());
}

void GameBattle::setResultMessage()
{
	rpg2k::model::DataBase const& ldb = gameSystem_.getLDB();

	messageWindow_->clearMessages();
	messageWindow_->addLine(ldb.vocabulary(5).toSystem());
	int exp = 0;
	int money = 0;
	std::vector< int > items;
	for (uint i = 0; i < enemies_.size(); i++) {
		if (enemies_[i]->getStatus().getHp() <= 0) {
			exp += enemies_[i]->getResultExp();
			money += enemies_[i]->getResultMoney();
			int item = enemies_[i]->getResultItem();
			if (item > 0)
				items.push_back(item);
		}
	}
	char temp[256];
	sprintf(temp, "%d%s", exp, ldb.vocabulary(7).toSystem().c_str());
	messageWindow_->addLine(temp);
	sprintf(temp, "%s%d%s%s", ldb.vocabulary(8).toSystem().c_str(), money, ldb.vocabulary(95).toSystem().c_str(), ldb.vocabulary(9).toSystem().c_str());
	messageWindow_->addLine(temp);
	for (uint i = 0; i < items.size(); i++) {
		sprintf(temp, "%s%s", ldb.item()[items[i]][1].get_string().toSystem().c_str(), ldb.vocabulary(10).toSystem().c_str());
		messageWindow_->addLine(temp);
	}

	for (uint i = 0; i < players_.size(); i++) {
		int oldLevel = players_[i]->getStatus().getLevel();
		uint playerID = players_[i]->getPlayerId();
		if (!players_[i]->isExcluded()) {
			players_[i]->getStatus().addExp(exp);
		}
		if (players_[i]->getStatus().getLevel() > oldLevel) {
			// const GamePlayerInfo& player = players_[i]->getPlayerInfo();
			sprintf(temp, "%sは%s%d%s", gameSystem_.name(playerID).c_str(), ldb.vocabulary(123).toSystem().c_str(),
				gameSystem_.level(playerID), ldb.vocabulary(36).toSystem().c_str());
			messageWindow_->addLine(temp);
			rpg2k::structure::Array2D const& skillInfo = gameSystem_.getLDB().skill()[playerID][63];

			int lv = 1;
			for (rpg2k::structure::Array2D::Iterator it = skillInfo.begin(); it != skillInfo.end(); ++it) {
				if( !it.second().exists() ) continue;

				if( it.second()[1].exists() ) lv = it.second()[1];
				if (lv > oldLevel && lv <= players_[i]->getStatus().getLevel()) {
					messageWindow_->addLine(ldb.skill()[it.second()[2].get<int>()][1].get_string().toSystem() + ldb.vocabulary(37).toSystem());
				}
			}
		}
	}
	gameSystem_.getLSD().setMoney( gameSystem_.getLSD().getMoney() + money );
	for (uint i = 0; i < items.size(); i++) {
		gameSystem_.getLSD().setItemNum(items[i], 1);
	}
	messageWindow_->setLineLimit(1);
}

GameBattleChara* GameBattle::getTargetRandom(GameBattleChara* attacker)
{
	std::vector<GameBattleChara*> charaList;
	if (attacker->getType() == GameBattleChara::kTypePlayer) {
		for (uint i = 0; i < enemies_.size(); i++) {
			if (!enemies_[i]->isExcluded()) charaList.push_back(enemies_[i]);
		}
	} else {
		for (uint i = 0; i < players_.size(); i++) {
			if (!players_[i]->isExcluded()) charaList.push_back(players_[i]);
		}
	}
	kuto_assert(!charaList.empty());
	return charaList[kuto::random((int)charaList.size())];
}

void GameBattle::setAnimationMessageMagicSub(GameBattleChara* attacker, GameBattleChara* target)
{
	char temp[256];
	rpg2k::model::DataBase const& ldb = gameSystem_.getLDB();
	const rpg2k::structure::Array1D& skill = ldb.skill()[attacker->getAttackInfo().id];
	AttackResult result = attacker->getAttackResult(*target);
	attackResults_.push_back(result);
	const char* name = target->getName().c_str();
	if (result.miss) {
		messageWindow_->addLine(name + ldb.vocabulary(24 + skill[7].get<int>()));
	} else {
		bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
		if (skill[31].get<bool>()) {
			if (result.cure) {
				sprintf(temp, "%sの%sが%d%s", name, ldb.vocabulary(124).toSystem().c_str() , result.hpDamage, ldb.vocabulary(29).toSystem().c_str());
				messageWindow_->addLine(temp);
			} else {
				sprintf(temp, "%s%s%d%s", name, isTargetPlayer? "は":"に", result.hpDamage, ldb.vocabulary(isTargetPlayer ? 22 : 20).c_str());
				messageWindow_->addLine(temp);
			}
		}
		if (skill[32].get<bool>()) {
			sprintf(temp, "%sの%sが%d%s", name, ldb.vocabulary(125).toSystem().c_str(), result.mpDamage, ldb.vocabulary(result.cure ? 30 : 31).c_str());
			messageWindow_->addLine(temp);
		}
		for(uint i = rpg2k::Param::ATTACK; i <= rpg2k::Param::SPEED; i++) {
			if ( skill[31 + i].get<bool>() ) {
				sprintf(temp, "%sの%sが%d%s",
					name, ldb.vocabulary(132 + i - rpg2k::Param::ATTACK).c_str(),
					result.mpDamage, ldb.vocabulary(result.cure ? 30 : 31).c_str()
				);
				messageWindow_->addLine(temp);
			}
		}
		for (uint i = 0; i < result.badConditions.size(); i++) {
			const rpg2k::structure::Array1D& cond = ldb.condition()[result.badConditions[i]];
			if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
				if (cond.exists(53)) messageWindow_->addLine(name + cond[53].get_string().toSystem());
			} else {
				if (target->getType() == GameBattleChara::kTypePlayer) messageWindow_->addLine(name + cond[51].get_string().toSystem());
				else messageWindow_->addLine(name + cond[52].get_string().toSystem());
			}
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
	rpg2k::model::DataBase const& ldb = gameSystem_.getLDB();

	if (!attacker->isActive()) {
		int limitBadConditionId = attacker->getWorstBadConditionId(true);
		if (limitBadConditionId > 0) {
			const rpg2k::structure::Array1D& cond = ldb.condition()[limitBadConditionId];
			if (cond.exists(54)) {
				messageWindow_->addLine(attacker->getName() + cond[54].get_string().toSystem());
			}
		}
		return;
	}

	switch (attacker->getAttackInfo().type) {
	case kAttackTypeAttack:
	case kAttackTypeDoubleAttack:
		{
			messageWindow_->addLine(attacker->getName() + ldb.vocabulary(11).toSystem());
			GameBattleChara* target = attacker->getAttackInfo().target;
			if (target->isExcluded()) {
				target = getTargetRandom(attacker);
			}
			int maxAttack = (attacker->getAttackInfo().type == kAttackTypeDoubleAttack)? 2 : 1;
			for (int numAttack = 0; numAttack < maxAttack; numAttack++) {
				AttackResult result = attacker->getAttackResult(*target);
				attackResults_.push_back(result);
				if (result.miss) {
					messageWindow_->addLine(target->getName() + ldb.vocabulary(27).toSystem());
				} else {
					bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
					if (result.critical) {
						messageWindow_->addLine(ldb.vocabulary(isTargetPlayer ? 12 : 13));
					}
					if (result.hpDamage == 0) {
						sprintf(temp, "%s%s", target->getName().c_str(), ldb.vocabulary(isTargetPlayer ? 23 : 21).c_str());
					} else {
						sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, ldb.vocabulary(isTargetPlayer ? 22 : 20).c_str());
					}
					messageWindow_->addLine(temp);
					for (uint i = 0; i < result.badConditions.size(); i++) {
						const rpg2k::structure::Array1D& cond = ldb.condition()[result.badConditions[i]];
						if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
							if (cond.exists(53)) messageWindow_->addLine(target->getName() + cond[53].get_string().toSystem());
						} else {
							messageWindow_->addLine(target->getName() + cond[isTargetPlayer ? 51 : 52].get_string().toSystem());
						}
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
			const rpg2k::structure::Array1D& skill = ldb.skill()[attacker->getAttackInfo().id];
			messageWindow_->addLine(attacker->getName() + skill[3].get_string().toSystem());
			if (!skill.exists(4))
				messageWindow_->addLine(attacker->getName() + skill[4].get_string().toSystem());
			switch (skill[12].get<int>()) {
			case 0: // enemy single
			case 1: // enemy all
				{
					GameBattleChara* target = attacker->getAttackInfo().target;
					if (target->isExcluded()) {
						target = getTargetRandom(attacker);
					}
					setAnimationMessageMagicSub(attacker, target);
				}
				break;
			case 2: // user
				setAnimationMessageMagicSub(attacker, attacker);
				break;
			case 3: // party single
				{
					if (attacker->getType() == GameBattleChara::kTypePlayer) {
						for (uint i = 0; i < enemies_.size(); i++) {
							if (!enemies_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, enemies_[i]);
						}
					} else {
						for (uint i = 0; i < players_.size(); i++) {
							if (!players_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, players_[i]);
						}
					}
				}
				break;
			case 4: // party all
				{
					if (attacker->getType() == GameBattleChara::kTypePlayer) {
						for (uint i = 0; i < players_.size(); i++) {
							if (!players_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, players_[i]);
						}
					} else {
						for (uint i = 0; i < enemies_.size(); i++) {
							if (!enemies_[i]->isExcluded())
								setAnimationMessageMagicSub(attacker, enemies_[i]);
						}
					}
				}
				break;
			}
			attacker->getStatus().setCharged(false);
			skillAnime_ = GameSkillAnime::createTask(this, gameSystem_, skill[14].get<int>());
		}
		break;
	case kAttackTypeItem:
		{
			const rpg2k::structure::Array1D& item = ldb.item()[attacker->getAttackInfo().id];
			GameBattleChara* target = attacker->getAttackInfo().target;
			AttackResult result = attacker->getAttackResult(*target);
			attackResults_.push_back(result);
			switch (item[3].get<int>()) {
			case rpg2k::Item::MEDICINE:
				sprintf(temp, "%sは%s%s", attacker->getName().c_str(), item[1].get_string().toSystem().c_str(), ldb.vocabulary(28).toSystem().c_str());
				messageWindow_->addLine(temp);
				if (result.hpDamage != 0) {
					sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), ldb.vocabulary(124).toSystem().c_str() , result.hpDamage, ldb.vocabulary(29).toSystem().c_str());
					messageWindow_->addLine(temp);
				}
				if (result.mpDamage != 0) {
					sprintf(temp, "%sの%sが%d%s", target->getName().c_str(), ldb.vocabulary(125).toSystem().c_str(), result.mpDamage, ldb.vocabulary(30).toSystem().c_str());
					messageWindow_->addLine(temp);
				}
				target->getStatus().addDamage(result);
				attackedTargets_.push_back(target);
				break;
			case rpg2k::Item::SPECIAL:
				sprintf(temp, "%sは%s%s", attacker->getName().c_str(), item[1].get_string().toSystem().c_str(), ldb.vocabulary(28).toSystem().c_str());
				messageWindow_->addLine(temp);
				break;
			}
			attacker->getStatus().setCharged(false);
		}
		break;
	case kAttackTypeDefence:
		messageWindow_->addLine(attacker->getName() + ldb.vocabulary(14).toSystem());
		break;
	case kAttackTypeWaitAndSee:
		messageWindow_->addLine(attacker->getName() + ldb.vocabulary(15).toSystem());
		break;
	case kAttackTypeCharge:
		messageWindow_->addLine(attacker->getName() + ldb.vocabulary(16).toSystem());
		attacker->getStatus().setCharged(true);
		break;
	case kAttackTypeSuicideBombing:
		{
			messageWindow_->addLine(attacker->getName() + ldb.vocabulary(17).toSystem());
			attacker->setExcluded(true);

			for (uint i = 0; i < players_.size(); i++) {
				if (players_[i]->isExcluded())
					continue;
				GameBattleChara* target = players_[i];
				AttackResult result = attacker->getAttackResult(*target);
				attackResults_.push_back(result);
				if (result.miss) {
					messageWindow_->addLine(target->getName() + ldb.vocabulary(27).toSystem());
				} else {
					bool isTargetPlayer = (target->getType() == GameBattleChara::kTypePlayer);
					if (result.critical) messageWindow_->addLine(ldb.vocabulary(isTargetPlayer ? 12 : 13));

					if (result.hpDamage == 0) sprintf(temp, "%s%s", target->getName().c_str(), ldb.vocabulary(isTargetPlayer ? 23 : 21).c_str());
					else sprintf(temp, "%s%s%d%s", target->getName().c_str(), isTargetPlayer? "は":"に", result.hpDamage, ldb.vocabulary(isTargetPlayer ? 22 : 20).c_str());
					messageWindow_->addLine(temp);

					for (uint i = 0; i < result.badConditions.size(); i++) {
						const rpg2k::structure::Array1D& cond = ldb.condition()[result.badConditions[i]];
						if (target->getStatus().getBadConditionIndex(result.badConditions[i]) >= 0) {
							if (cond.exists(53)) messageWindow_->addLine(target->getName() + cond[53].get_string().toSystem());
						} else {
							messageWindow_->addLine(target->getName() + cond[isTargetPlayer ? 51 : 52].get_string().toSystem());
						}
					}
				}
				target->getStatus().addDamage(result);
				attacker->getStatus().setCharged(false);
				attackedTargets_.push_back(target);
			}
		}
		break;
	case kAttackTypeEscape:
		messageWindow_->addLine(attacker->getName() + ldb.vocabulary(18).toSystem());
		attacker->playDeadAnime();
		//attacker->setExcluded(true);
		break;
	case kAttackTypeTransform:
		messageWindow_->addLine(attacker->getName() + ldb.vocabulary(19).toSystem());
		break;
	default: break;
	}

	if (skillAnime_) {
		for (uint i = 0; i < attackedTargets_.size(); i++) {
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
	for (uint i = 0; i < players_.size(); i++) {
		if (players_[i]->getAttackInfo().type != kAttackTypeNone && !players_[i]->isExcluded())
			battleOrder_.push_back(players_[i]);
	}
	if (!firstAttack_) {
		for (uint i = 0; i < enemies_.size(); i++) {
			if (enemies_[i]->getAttackInfo().type != kAttackTypeNone && !enemies_[i]->isExcluded())
				battleOrder_.push_back(enemies_[i]);
		}
	}
	// sort
	for (uint i = 0; i < battleOrder_.size(); i++) {
		battleOrder_[i]->setAttackPriorityOffset(kuto::random(0.1f));		// add random offset
	}
	std::sort(battleOrder_.begin(), battleOrder_.end(), sortBattleOrderFunc);
}

bool GameBattle::isWin()
{
	for (uint i = 0; i < enemies_.size(); i++) {
		if (!enemies_[i]->isExcluded())
			return false;
	}
	return true;
}

bool GameBattle::isLose()
{
	for (uint i = 0; i < players_.size(); i++) {
		if (!players_[i]->isExcluded())
			return false;
	}
	return true;
}





