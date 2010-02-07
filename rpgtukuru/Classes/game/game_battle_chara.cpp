/**
 * @file
 * @brief Game Battle Character
 * @author takuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>
#include "game_battle_chara.h"
#include "game_battle.h"
#include "CRpgUtil.h"


GameBattleChara::GameBattleChara(kuto::Task* parent, const GameSystem& gameSystem)
: kuto::Task(parent), gameSystem_(gameSystem), attackPriorityOffset_(0.f), excluded_(false)
, status_()
{
}

bool GameBattleChara::initialize()
{
	return isInitializedChildren();
}

AttackResult GameBattleChara::getAttackResult(const GameBattleChara& target, const AttackInfo& attackInfo) const
{
	AttackResult result;
	result.attackInfo = attackInfo;
	switch (attackInfo.type) {
	case kAttackTypeAttack:
	case kAttackTypeDoubleAttack:
		{
			int atk = (int)(status_.getAttack() * 0.5f * (kuto::random(0.4f) + 0.8f));
			result.hpDamage = kuto::max(0, atk - target.getStatus().getDefence() / 4);
			if (status_.isCharged() || kuto::random(1.f) < status_.getCriticalRatio()) {
				// critical hit!
				result.critical = true;
				result.hpDamage *= 3;
			}
			if (target.getAttackInfo().type == kAttackTypeDefence) {
				if (status_.isStrongGuard())
					result.hpDamage /= 4;
				else
					result.hpDamage /= 2;
			}
			int hitRatio = (int)(100 - (100 - status_.getHitRatio()) *
				(1.f + ((float)target.getStatus().getSpeed() / (float)status_.getSpeed() - 1.f) / 2.f));
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				if (target.getStatus().getHp() - result.hpDamage <= 0) {
					result.badConditions.push_back(1);	// 戦闘不能状態に
				}
			}
		}
		break;
	case kAttackTypeSuicideBombing:
		{
			int atk = (int)(status_.getAttack() * (kuto::random(0.4f) + 0.8f));
			result.hpDamage = kuto::max(0, atk - target.getStatus().getDefence() / 2);
			if (target.getAttackInfo().type == kAttackTypeDefence) {
				if (status_.isStrongGuard())
					result.hpDamage /= 4;
				else
					result.hpDamage /= 2;
			}
			int hitRatio = (int)(100 - (100 - status_.getHitRatio()) *
				(1.f + ((float)target.getStatus().getSpeed() / (float)status_.getSpeed() - 1.f) / 2.f));
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				if (target.getStatus().getHp() - result.hpDamage <= 0) {
					result.badConditions.push_back(1);	// 戦闘不能状態に
				}
			}
		}
		break;
	case kAttackTypeSkill:
		{
			const CRpgLdb::Skill& skill = gameSystem_.getRpgLdb().saSkill[attackInfo.id];
			int baseValue = skill.baseEffect + (status_.getAttack() * skill.attackRatio / 20) + (status_.getMagic() * skill.magicRatio / 40);
			baseValue += (int)(baseValue * (kuto::random(1.f) - 0.5f) * skill.variance * 0.1f);
			if (skill.scope < CRpgLdb::kSkillScopeMyself) {
				if (!skill.statusDownIgnoreDefence) {
					baseValue -= (target.getStatus().getDefence() * skill.attackRatio / 40) + (target.getStatus().getMagic() * skill.magicRatio / 80);
				}
				if (target.getAttackInfo().type == kAttackTypeDefence) {
					if (status_.isStrongGuard())
						baseValue /= 4;
					else
						baseValue /= 2;
				}
			} else {
				result.cure = true;
			}
			baseValue = kuto::max(0, baseValue);
			if (skill.statusDownHP) {
				result.hpDamage = baseValue;
			}
			if (skill.statusDownMP) {
				result.mpDamage = baseValue;
			}
			if (skill.statusDownAttack) {
				result.attack = baseValue;
			}
			if (skill.statusDownMagic) {
				result.magic = baseValue;
			}
			if (skill.statusDownDefence) {
				result.defence = baseValue;
			}
			if (skill.statusDownSpeed) {
				result.speed = baseValue;
			}
			if (skill.statusDownAbsorption) {
				result.absorption = true;
			}
			int hitRatio = skill.baseSuccess;
			if (skill.messageMiss == 3) {
				hitRatio = (int)(100 - (100 - status_.getHitRatio()) *
					(1.f + ((float)target.getStatus().getSpeed() / (float)status_.getSpeed() - 1.f) / 2.f));
			}
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				for (u32 i = 0; i < skill.conditionChange.size(); i++) {
					if (skill.conditionChange[i]) {
						result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
					}
				}
				if (!result.cure) {
					if (target.getStatus().getHp() - result.hpDamage <= 0) {
						result.badConditions.push_back(1);	// 戦闘不能状態に
					}
				}
			}
		}
		break;
	case kAttackTypeItem:
		{
			const CRpgLdb::Item& item = gameSystem_.getRpgLdb().saItem[attackInfo.id];
			switch (item.type) {
			case CRpgLdb::kItemTypeMedicine:
				if ((item.deadOnly && target.getStatus().getHp() > 0) || (!item.deadOnly && target.getStatus().getHp() == 0)) {
					// do nothing
				} else {
					result.hpDamage = (item.cureHPRatio * target.getStatus().getBaseStatus().maxHP / 100) + item.cureHPValue;
					result.mpDamage = (item.cureMPRatio * target.getStatus().getBaseStatus().maxMP / 100) + item.cureMPValue;
					result.cure = true;
					for (u32 i = 1; i < item.conditionChange.size(); i++) {
						if (item.conditionChange[i]) {
							result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
						}
					}
				}
				break;
			case CRpgLdb::kItemTypeSpecial:
				{
					AttackInfo tempInfo = attackInfo;
					tempInfo.type = kAttackTypeSkill;
					tempInfo.id = item.skill;
					result = getAttackResult(target, tempInfo);
					result.attackInfo = attackInfo;
				}
				break;
			}
		}
		break;
	default:
		{
			result.hpDamage = kuto::max(0, status_.getAttack() / 2 - target.getStatus().getDefence() / 4);
			result.hpDamage = (int)(result.hpDamage * (kuto::random(0.4f) + 0.8f));
			int hitRatio = (int)(100 - (100 - status_.getHitRatio()) *
				(1.f + ((float)target.getStatus().getSpeed() / (float)status_.getSpeed() - 1.f) / 2.f));
			result.miss = kuto::random(100) >= hitRatio;
		}
		break;
	}
	return result;
}

float GameBattleChara::getAttackPriority() const
{
	if ((attackInfo_.type == kAttackTypeAttack || attackInfo_.type == kAttackTypeDoubleAttack) && status_.isFirstAttack())
		return 1000000.f;	// とりあえずでっかい値に
	return (float)status_.getSpeed() + attackPriorityOffset_;
}

bool GameBattleChara::isActive() const
{
	if (isExcluded())
		return false;
	for (u32 i = 0; i < status_.getBadConditions().size(); i++) {
		const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[status_.getBadConditions()[i].id];
		if (cond.limitAction == CRpgLdb::kLimitActionDoNotAction)
			return false;
	}
	return true;
}

int GameBattleChara::getWorstBadConditionId(bool doNotActionOnly) const
{
	int ret = 0;
	int pri = 0;
	for (u32 i = 0; i < status_.getBadConditions().size(); i++) {
		const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[status_.getBadConditions()[i].id];
		if ((!doNotActionOnly || cond.limitAction == CRpgLdb::kLimitActionDoNotAction) && cond.priority > pri) {
			ret = status_.getBadConditions()[i].id;
			pri = cond.priority;
		}
	}
	return ret;
}

CRpgLdb::LimitActionType GameBattleChara::getLimitAction() const
{
	CRpgLdb::LimitActionType ret = CRpgLdb::kLimitActionNone;
	int pri = 0;
	for (u32 i = 0; i < status_.getBadConditions().size(); i++) {
		const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[status_.getBadConditions()[i].id];
		if ((cond.limitAction == CRpgLdb::kLimitActionAttackEnemy || cond.limitAction == CRpgLdb::kLimitActionAttackFriend) && cond.priority > pri) {
			ret = (CRpgLdb::LimitActionType)cond.limitAction;
			pri = cond.priority;
		}
	}
	return ret;
}

void GameBattleChara::updateBadCondition()
{
	for (u32 i = 0; i < status_.getBadConditions().size(); ) {
		if (status_.getBadConditions()[i].id != 1) {
			const CRpgLdb::Condition& cond = gameSystem_.getRpgLdb().saCondition[status_.getBadConditions()[i].id];
			status_.getBadConditions()[i].count++;
			if (status_.getBadConditions()[i].count > cond.cureTurn) {
				if (cond.cureRatioNatural > kuto::random(100)) {
					status_.removeBadCondition(i);
					continue;
				}
			}
		}
		i++;
	}
	
}



GameBattleEnemy::GameBattleEnemy(kuto::Task* parent, const GameSystem& gameSystem, int enemyId)
: GameBattleChara(parent, gameSystem), enemyId_(enemyId)
, animationState_(kAnimationStateNone), animationCounter_(0), position_(0.f, 0.f)
{
	const CRpgLdb::Enemy& enemy = gameSystem_.getRpgLdb().saEnemy[enemyId_];
	std::string background = gameSystem_.getRootFolder();
	background += "/Monster/" + enemy.graphicName;
	CRpgUtil::LoadImage(texture_, background, true, enemy.graphicColor);
	
	status_.setEnemyStatus(gameSystem_.getRpgLdb(), enemyId, gameSystem_.getConfig().difficulty);
}

void GameBattleEnemy::update()
{
	animationCounter_++;
	switch (animationState_) {
	case kAnimationStateDamage:
		if (animationCounter_ >= 60) {
			animationState_ = kAnimationStateNone;
		}
		break;
	case kAnimationStateDead:
		if (animationCounter_ >= 60) {
			animationState_ = kAnimationStateNone;
			excluded_ = true;
		}
		break;
	}
}

void GameBattleEnemy::draw()
{
	if (!excluded_)
		kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 9.f);
}

void GameBattleEnemy::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(1.f, 1.f, 1.f, 1.f);
	switch (animationState_) {
	case kAnimationStateDamage:
		if (animationCounter_ >= 10 && animationCounter_ < 22 && ((animationCounter_ - 10) / 4) % 2 == 0) {
			color.a = 0.f;
		}
		break;
	case kAnimationStateDead:
		color.a = kuto::max(0.f, 1.f - (float)animationCounter_ / 50.f);
		break;
	}
	kuto::Vector2 scale(texture_.getOrgWidth(), texture_.getOrgHeight());
	kuto::Vector2 pos(position_.x - scale.x * 0.5f, position_.y - scale.y * 0.5f);
	pos += static_cast<GameBattle*>(getParent())->getScreenOffset();
	g->drawTexture(texture_, pos, scale, color, true);
}

void GameBattleEnemy::renderFlash(const kuto::Color& color)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Vector2 scale(texture_.getOrgWidth(), texture_.getOrgHeight());
	kuto::Vector2 pos(position_.x - scale.x * 0.5f, position_.y - scale.y * 0.5f);
	pos += static_cast<GameBattle*>(getParent())->getScreenOffset();
	g->fillRectangleMask(pos, scale, color, texture_, true);
}

void GameBattleEnemy::setAttackInfoAuto(const GameBattlePlayerList& targets, const GameBattleEnemyList& party, int turnNum)
{
	const CRpgLdb::Enemy& enemy = gameSystem_.getRpgLdb().saEnemy[enemyId_];
	AttackInfo info;
	std::vector<int> pattern;
	for (u32 i = 0; i < enemy.attackPattern.size(); i++) {
		switch (enemy.attackPattern[i].conditionType) {
		case 0:		// [常時]
			pattern.push_back(i);
			break;
		case 1:		// [スイッチ] S[A]がON
			if (gameSystem_.getSwitch(enemy.attackPattern[i].conditionSwitch))
				pattern.push_back(i);
			break;
		case 2:		// [ターン数] A×？+B ターン
			if (enemy.attackPattern[i].conditionNumberA > 0) {
				if (turnNum % enemy.attackPattern[i].conditionNumberA == enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			} else {
				if (turnNum == enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			}
			break;
		case 3:		// [グループ個体数] A〜B体
			{
				int partyNum = 0;
				for (u32 iParty = 0; iParty < party.size(); iParty++) {
					if (!party[iParty]->isExcluded())
						partyNum++;
				}
				if (partyNum >= enemy.attackPattern[i].conditionNumberA && partyNum <= enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			}
			break;
		case 4:		// [自分のHP] A〜B%
			{
				int hpRatio = status_.getHp() * 100 / status_.getBaseStatus().maxHP;
				if (hpRatio >= enemy.attackPattern[i].conditionNumberA && hpRatio <= enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			}
			break;
		case 5:		// [自分のMP] A〜B%
			{
				int mpRatio = status_.getMp() * 100 / status_.getBaseStatus().maxMP;
				if (mpRatio >= enemy.attackPattern[i].conditionNumberA && mpRatio <= enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			}
			break;
		case 6:		// [主人公平均Lv] A〜B
			{
				int level = 0;
				for (u32 iTarget = 0; iTarget < targets.size(); iTarget++)
					level += targets[iTarget]->getStatus().getLevel();
				level /= targets.size();
				if (level >= enemy.attackPattern[i].conditionNumberA && level <= enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			}
			break;
		case 7:		// [主人公消耗度] A〜B%
			{
				int hp = 0;
				for (u32 iTarget = 0; iTarget < targets.size(); iTarget++)
					hp += targets[iTarget]->getStatus().getHp() * 100 / targets[iTarget]->getStatus().getBaseStatus().maxHP;
				hp = 100 - hp / targets.size();
				if (hp >= enemy.attackPattern[i].conditionNumberA && hp <= enemy.attackPattern[i].conditionNumberB)
					pattern.push_back(i);
			}
			break;
		}
	}
	int priorityMax = 0;
	for (u32 i = 0; i < pattern.size(); i++) {
		priorityMax += enemy.attackPattern[pattern[i]].priority;
	}
	int attackIndex = -1;
	int priRange = kuto::random(priorityMax);
	for (u32 i = 0; i < pattern.size(); i++) {
		priRange -= enemy.attackPattern[pattern[i]].priority;
		if (priRange < 0) {
			attackIndex = i;
			break;
		}
	}
	if (attackIndex >= 0) {
		const CRpgLdb::AttackPattern& pt = enemy.attackPattern[pattern[attackIndex]];
		switch (pt.type) {
		case 0:		// 基本行動
			info.target = targets[rand() % targets.size()];
			switch (pt.base) {
			case 0:		// 通常攻撃
				info.type = kAttackTypeAttack;
				break;
			case 1:		// 連続攻撃
				info.type = kAttackTypeDoubleAttack;
				break;
			case 2:		// 防御
				info.type = kAttackTypeDefence;
				break;
			case 3:		// 様子をみる
				info.type = kAttackTypeWaitAndSee;
				break;
			case 4:		// 力を溜める
				info.type = kAttackTypeCharge;
				break;
			case 5:		// 自爆する
				info.type = kAttackTypeSuicideBombing;
				break;
			case 6:		// 逃げる
				info.type = kAttackTypeEscape;
				break;
			default:	// 何もしない
				info.type = kAttackTypeNone;
				break;
			}
			info.id = 0;
			break;
		case 1:		// 特殊技能
			{
				const CRpgLdb::Skill& skill = gameSystem_.getRpgLdb().saSkill[pt.skill];
				switch (skill.scope) {
				case CRpgLdb::kSkillScopeEnemySingle:
					info.target = targets[rand() % targets.size()];
					break;
				case CRpgLdb::kSkillScopeFriendSingle:
					info.target = party[rand() % party.size()];
					break;
				case CRpgLdb::kSkillScopeMyself:
					info.target = this;
					break;
				default:
					info.target = NULL;
				}
				info.type = kAttackTypeSkill;
				info.id = pt.skill;
			}
			break;
		case 2:		// 変身
			info.target = NULL;
			info.type = kAttackTypeTransform;
			info.id = pt.transform;
			break;
		}
	} else {
		info.target = targets[rand() % targets.size()];
		info.type = kAttackTypeAttack;
		info.id = 0;
	}
	setAttackInfo(info);
}

void GameBattleEnemy::playDamageAnime()
{
	animationState_ = kAnimationStateDamage;
	animationCounter_ = 0;
}

void GameBattleEnemy::playDeadAnime()
{
	animationState_ = kAnimationStateDead;
	animationCounter_ = 0;
}

int GameBattleEnemy::getResultExp() const
{
	const CRpgLdb::Enemy& enemy = gameSystem_.getRpgLdb().saEnemy[enemyId_];
	return enemy.resultExp;
}

int GameBattleEnemy::getResultMoney() const
{
	const CRpgLdb::Enemy& enemy = gameSystem_.getRpgLdb().saEnemy[enemyId_];
	return enemy.resultGold;
}

int GameBattleEnemy::getResultItem() const
{
	const CRpgLdb::Enemy& enemy = gameSystem_.getRpgLdb().saEnemy[enemyId_];
	if (enemy.resultItemDropRatio > 0 && kuto::random(100) < enemy.resultItemDropRatio)
		return enemy.resultItem;
	return 0;
}



GameBattlePlayer::GameBattlePlayer(kuto::Task* parent, const GameSystem& gameSystem, int playerId, GameCharaStatus& status)
: GameBattleChara(parent, gameSystem), playerId_(playerId)
, animationState_(kAnimationStateNone), animationCounter_(0)
{
	status_ = status;
	if (status_.isDead())	// 最初から死んでるし
		setExcluded(true);
}

void GameBattlePlayer::setAttackInfoAuto(const GameBattleEnemyList& targets, const GameBattlePlayerList& party, int turnNum)
{
	AttackInfo info;
	switch (getLimitAction()) {
	case CRpgLdb::kLimitActionNone:
		info.target = targets[rand() % targets.size()];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	case CRpgLdb::kLimitActionDoNotAction:
		info.target = NULL;
		info.type = kAttackTypeNone;
		info.id = 0;
		break;
	case CRpgLdb::kLimitActionAttackEnemy:
		info.target = targets[rand() % targets.size()];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	case CRpgLdb::kLimitActionAttackFriend:
		info.target = party[rand() % party.size()];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	}
	setAttackInfo(info);
}

void GameBattlePlayer::update()
{
	animationCounter_++;
	switch (animationState_) {
	case kAnimationStateDamage:
		if (animationCounter_ >= 60) {
			animationState_ = kAnimationStateNone;
		} else {
			kuto::Vector2 offset(0.f, 0.f);
			if (animationCounter_ >= 10 && animationCounter_ < 22) {
				int count = animationCounter_ - 10;
				if (count < 3)
					offset.x = 3.f * (count);
				else if (count < 6)
					offset.x = -3.f * (count - 3) + 9.f;
				else if (count < 9)
					offset.x = -3.f * (count - 6);
				else 
					offset.x = 3.f * (count - 9) - 9.f;
			}
			static_cast<GameBattle*>(getParent())->setScreenOffset(offset);
		}
		break;
	case kAnimationStateDead:
		if (animationCounter_ >= 60) {
			animationState_ = kAnimationStateNone;
			excluded_ = true;
		}
		break;
	}
}

bool GameBattlePlayer::isExecAI() const
{
	if (getPlayerInfo().execAI)
		return true;
	if (getLimitAction() != CRpgLdb::kLimitActionNone)
		return true;
	return false;
}

void GameBattlePlayer::playDamageAnime()
{
	animationState_ = kAnimationStateDamage;
	animationCounter_ = 0;
}

void GameBattlePlayer::playDeadAnime()
{
	animationState_ = kAnimationStateDead;
	animationCounter_ = 0;
}


