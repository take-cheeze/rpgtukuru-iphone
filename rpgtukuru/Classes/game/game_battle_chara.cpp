/**
 * @file
 * @brief Game Battle Character
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>
#include "game_battle_chara.h"
#include "game_battle.h"
// #include "CRpgUtil.h"


GameBattleChara::GameBattleChara(kuto::Task* parent, const rpg2k::model::Project& gameSystem)
: kuto::Task(parent), gameSystem_(gameSystem)
, status_(), attackPriorityOffset_(0.f), excluded_(false)
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
			const rpg2k::structure::Array1D& skill = gameSystem_.getLDB().skill()[attackInfo.id];
			int baseValue = skill[24].get<int>() + (status_.getAttack() * skill[21].get<int>() / 20) + (status_.getMagic() * skill[22].get<int>() / 40);
			baseValue += (int)(baseValue * (kuto::random(1.f) - 0.5f) * skill[23].get<int>() * 0.1f);
			if (skill[12].get<int>() < 2) {
				if (!skill[38].get<bool>()) {
					baseValue -= (target.getStatus().getDefence() * skill[21].get<int>() / 40) + (target.getStatus().getMagic() * skill[22].get<int>() / 80);
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
			if (skill[31].get<bool>()) result.hpDamage = baseValue;
			if (skill[32].get<bool>()) result.mpDamage = baseValue;
			if (skill[33].get<bool>()) result.attack = baseValue;
			if (skill[34].get<bool>()) result.magic = baseValue;
			if (skill[35].get<bool>()) result.defence = baseValue;
			if (skill[36].get<bool>()) result.speed = baseValue;
			result.absorption = skill[37].get<bool>();
			int hitRatio = skill[25];
			if (skill[7].get<int>() == 3) {
				hitRatio = (int)(100 - (100 - status_.getHitRatio()) *
					(1.f + ((float)target.getStatus().getSpeed() / (float)status_.getSpeed() - 1.f) / 2.f));
			}
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				rpg2k::Binary const& cond = skill[42];
				for (uint i = 0; i < cond.size(); i++) {
					if (cond[i]) result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
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
			const rpg2k::structure::Array1D& item = gameSystem_.getLDB().item()[attackInfo.id];
			switch (item[3].get<int>()) {
			case rpg2k::Item::MEDICINE:
				if ((item[38].get<bool>() && target.getStatus().getHp() > 0) || (!item[38].get<bool>() && target.getStatus().getHp() == 0)) {
					// do nothing
				} else {
					result.hpDamage = (item[33].get<int>() * target.getStatus().getBaseStatus()[rpg2k::Param::HP] / 100) + item[32].get<int>();
					result.mpDamage = (item[35].get<int>() * target.getStatus().getBaseStatus()[rpg2k::Param::MP] / 100) + item[34].get<int>();
					result.cure = true;
					rpg2k::Binary const& cond = item[64];
					for (uint i = 1; i < cond.size(); i++) {
						if (cond[i]) result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
					}
				}
				break;
			case rpg2k::Item::SPECIAL:
				{
					AttackInfo tempInfo = attackInfo;
					tempInfo.type = kAttackTypeSkill;
					tempInfo.id = item[53];
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
	if (isExcluded()) return false;
	for (uint i = 0; i < status_.getBadConditions().size(); i++) {
		if( gameSystem_.getLDB().condition()[status_.getBadConditions()[i].id][5].get<int>() == 1 ) return false;
	}
	return true;
}

int GameBattleChara::getWorstBadConditionId(bool doNotActionOnly) const
{
	int ret = 0;
	int pri = 0;
	for (uint i = 0; i < status_.getBadConditions().size(); i++) {
		const rpg2k::structure::Array1D& cond = gameSystem_.getLDB().condition()[status_.getBadConditions()[i].id];
		if ((!doNotActionOnly || cond[5].get<int>() == 1) && cond[4].get<int>() > pri) {
			ret = status_.getBadConditions()[i].id;
			pri = cond[5].get<int>();
		}
	}
	return ret;
}

/* DataBase::LimitActionType */ int GameBattleChara::getLimitAction() const
{
	/* DataBase::LimitActionType */ int ret = 0; // DataBase::kLimitActionNone;
	int pri = 0;
	for (uint i = 0; i < status_.getBadConditions().size(); i++) {
		const rpg2k::structure::Array1D& cond = gameSystem_.getLDB().condition()[status_.getBadConditions()[i].id];
		if ((cond[5].get<int>() == 2 /* DataBase::kLimitActionAttackEnemy */ || cond[5].get<int>() == 3 /* DataBase::kLimitActionAttackFriend */ ) && cond[4].get<int>() > pri) {
			ret = /* (DataBase::LimitActionType) */ cond[4];
			pri = cond[4];
		}
	}
	return ret;
}

void GameBattleChara::updateBadCondition()
{
	for (uint i = 0; i < status_.getBadConditions().size(); ) {
		if (status_.getBadConditions()[i].id != 1) {
			const rpg2k::structure::Array1D& cond = gameSystem_.getLDB().condition()[status_.getBadConditions()[i].id];
			status_.getBadConditions()[i].count++;
			if (status_.getBadConditions()[i].count > cond[21].get<int>()) {
				if (cond[22].get<int>() > kuto::random(100)) {
					status_.removeBadCondition(i);
					continue;
				}
			}
		}
		i++;
	}

}



GameBattleEnemy::GameBattleEnemy(kuto::Task* parent, const rpg2k::model::Project& gameSystem, int enemyId)
: GameBattleChara(parent, gameSystem), position_(0.f, 0.f)
, animationState_(kAnimationStateNone), animationCounter_(0)
, enemyId_(enemyId)
{
	const rpg2k::structure::Array1D& enemy = gameSystem_.getLDB().enemy()[enemyId_];
	std::string background = gameSystem_.getGameDir() + "/Monster/" + enemy[2].get_string().toSystem();
	bool res = CRpgUtil::LoadImage(texture_, background, true, enemy[3].get<int>()); kuto_assert(res);
	status_.setEnemyStatus(gameSystem_.getLDB(), enemyId, GameConfig::kDifficultyNormal /* gameSystem_.getConfig().difficulty */);
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
	default: break;
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
	default: break;
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
	const rpg2k::structure::Array1D& enemy = gameSystem_.getLDB().enemy()[enemyId_];
	const rpg2k::structure::Array2D& attackPattern = enemy[42];
	AttackInfo info;
	std::vector<int> pattern;
	for (rpg2k::structure::Array2D::Iterator it = attackPattern.begin(); it != attackPattern.end(); ++it) {
		switch (it.second()[5].get<int>()) {
		case 0:		// [常時]
			pattern.push_back(it.first());
			break;
		case 1:		// [スイッチ] S[A]がON
			if (gameSystem_.getLSD().getFlag(it.second()[8].get<int>()))
				pattern.push_back(it.first());
			break;
		case 2:		// [ターン数] A×？+B ターン
			if (it.second()[6].get<int>() > 0) {
				if (turnNum % it.second()[6].get<int>() == it.second()[7].get<int>())
					pattern.push_back(it.first());
			} else {
				if (turnNum == it.second()[7].get<int>())
					pattern.push_back(it.first());
			}
			break;
		case 3:		// [グループ個体数] A〜B体
			{
				int partyNum = 0;
				for (uint iParty = 0; iParty < party.size(); iParty++) {
					if (!party[iParty]->isExcluded())
						partyNum++;
				}
				if (partyNum >= it.second()[6].get<int>() && partyNum <= it.second()[7].get<int>())
					pattern.push_back(it.first());
			}
			break;
		case 4:		// [自分のHP] A〜B%
			{
				int hpRatio = status_.getHp() * 100 / status_.getBaseStatus()[rpg2k::Param::HP];
				if (hpRatio >= it.second()[6].get<int>() && hpRatio <= it.second()[7].get<int>())
					pattern.push_back(it.first());
			}
			break;
		case 5:		// [自分のMP] A〜B%
			{
				int mpRatio = status_.getMp() * 100 / status_.getBaseStatus()[rpg2k::Param::MP];
				if (mpRatio >= it.second()[6].get<int>() && mpRatio <= it.second()[7].get<int>())
					pattern.push_back(it.first());
			}
			break;
		case 6:		// [主人公平均Lv] A〜B
			{
				int level = 0;
				for (uint iTarget = 0; iTarget < targets.size(); iTarget++)
					level += targets[iTarget]->getStatus().getLevel();
				level /= targets.size();
				if (level >= it.second()[6].get<int>() && level <= it.second()[7].get<int>())
					pattern.push_back(it.first());
			}
			break;
		case 7:		// [主人公消耗度] A〜B%
			{
				int hp = 0;
				for (uint iTarget = 0; iTarget < targets.size(); iTarget++)
					hp += targets[iTarget]->getStatus().getHp() * 100 / targets[iTarget]->getStatus().getBaseStatus()[rpg2k::Param::HP];
				hp = 100 - hp / targets.size();
				if (hp >= it.second()[6].get<int>() && hp <= it.second()[7].get<int>())
					pattern.push_back(it.first());
			}
			break;
		}
	}
	int priorityMax = 0;
	for (uint i = 0; i < pattern.size(); i++) {
		priorityMax += attackPattern[pattern[i]][13].get<int>();
	}
	int attackIndex = -1;
	int priRange = kuto::random(priorityMax);
	for (uint i = 0; i < pattern.size(); i++) {
		priRange -= attackPattern[pattern[i]][13].get<int>();
		if (priRange < 0) {
			attackIndex = i;
			break;
		}
	}
	if (attackIndex >= 0) {
		const rpg2k::structure::Array1D& pt = attackPattern[pattern[attackIndex]];
		switch (pt[1].get<int>()) {
		case 0:		// 基本行動
			info.target = targets[kuto::random(targets.size())];
			switch (pt[2].get<int>()) {
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
				const rpg2k::structure::Array1D& skill = gameSystem_.getLDB().skill()[pt[3].get<int>()];
				switch (skill[12].get<int>()) {
				case 0: // enemy single
					info.target = targets[kuto::random(targets.size())];
					break;
				case 2: // user
					info.target = this;
					break;
				case 3: // party single
					info.target = party[kuto::random(party.size())];
					break;
				default:
					info.target = NULL;
				}
				info.type = kAttackTypeSkill;
				info.id = pt[3].get<int>();
			}
			break;
		case 2:		// 変身
			info.target = NULL;
			info.type = kAttackTypeTransform;
			info.id = pt[4].get<int>();
			break;
		}
	} else {
		info.target = targets[kuto::random(targets.size())];
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
	return gameSystem_.getLDB().enemy()[enemyId_][11];
}

int GameBattleEnemy::getResultMoney() const
{
	return gameSystem_.getLDB().enemy()[enemyId_][12];
}

int GameBattleEnemy::getResultItem() const
{
	const rpg2k::structure::Array1D& enemy = gameSystem_.getLDB().enemy()[enemyId_];
	if (kuto::random(100) < enemy[14].get<int>()) return enemy[13];
	return 0;
}


GameBattlePlayer::GameBattlePlayer(kuto::Task* parent, const rpg2k::model::Project& gameSystem, int playerId, GameCharaStatus& status)
: GameBattleChara(parent, gameSystem), playerId_(playerId)
, animationState_(kAnimationStateNone), animationCounter_(0)
{
	status_ = status;
	if (status_.isDead()) setExcluded(true); // 最初から死んでるし
}

void GameBattlePlayer::setAttackInfoAuto(const GameBattleEnemyList& targets, const GameBattlePlayerList& party, int turnNum)
{
	AttackInfo info;
	switch (getLimitAction()) {
	case 0:
		info.target = targets[kuto::random(targets.size())];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	case 1:
		info.target = NULL;
		info.type = kAttackTypeNone;
		info.id = 0;
		break;
	case 2:
		info.target = targets[kuto::random(targets.size())];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	case 3:
		info.target = party[kuto::random(party.size())];
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
	default: break;
	}
}

bool GameBattlePlayer::isExecAI() const
{
	return gameSystem_.getLDB().character()[getPlayerId()][23].get<bool>() || (getLimitAction() != 0 /* DataBase::kLimitActionNone */ );
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
