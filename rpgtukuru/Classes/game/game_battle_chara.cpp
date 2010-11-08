/**
 * @file
 * @brief Game Battle Character
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>

#include <rpg2k/Project.hpp>

#include "game_battle.h"
#include "game_battle_chara.h"
#include "game_config.h"


using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;

GameBattleChara::GameBattleChara(const rpg2k::model::Project& gameSystem)
: project_(gameSystem)
, attackPriorityOffset_(0.f), excluded_(false)
{
}

bool GameBattleEnemy::initialize()
{
	return isInitializedChildren();
}
bool GameBattlePlayer::initialize()
{
	return isInitializedChildren();
}

AttackResult GameBattleChara::attackResult(const GameBattleChara& target, const AttackInfo& attackInfo) const
{
	AttackResult result;
	result.attackInfo = attackInfo;
	switch (attackInfo.type) {
	case kAttackTypeAttack:
	case kAttackTypeDoubleAttack:
		{
			int atk = (int)(status_.attack() * 0.5f * (kuto::random(0.4f) + 0.8f));
			result.hpDamage = kuto::max(0, atk - target.status().defence() / 4);
			if (status_.isCharged() || kuto::random(1.f) < status_.criticalRatio()) {
				// critical hit!
				result.critical = true;
				result.hpDamage *= 3;
			}
			if (target.attackInfo().type == kAttackTypeDefence) {
				if (status_.isStrongGuard())
					result.hpDamage /= 4;
				else
					result.hpDamage /= 2;
			}
			int hitRatio = (int)(100 - (100 - status_.hitRatio()) *
				(1.f + ((float)target.status().speed() / (float)status_.speed() - 1.f) / 2.f));
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				if (target.status().hp() - result.hpDamage <= 0) {
					result.badConditions.push_back(1);	// 戦闘不能状態に
				}
			}
		}
		break;
	case kAttackTypeSuicideBombing:
		{
			int atk = (int)(status_.attack() * (kuto::random(0.4f) + 0.8f));
			result.hpDamage = kuto::max(0, atk - target.status().defence() / 2);
			if (target.attackInfo().type == kAttackTypeDefence) {
				if (status_.isStrongGuard())
					result.hpDamage /= 4;
				else
					result.hpDamage /= 2;
			}
			int hitRatio = (int)(100 - (100 - status_.hitRatio()) *
				(1.f + ((float)target.status().speed() / (float)status_.speed() - 1.f) / 2.f));
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				if (target.status().hp() - result.hpDamage <= 0) {
					result.badConditions.push_back(1);	// 戦闘不能状態に
				}
			}
		}
		break;
	case kAttackTypeSkill:
		{
			const Array1D& skill = project_.getLDB().skill()[attackInfo.id];
			int baseValue = skill[24].to<int>() + (status_.attack() * skill[21].to<int>() / 20) + (status_.magic() * skill[22].to<int>() / 40);
			baseValue += (int)(baseValue * (kuto::random(1.f) - 0.5f) * skill[23].to<int>() * 0.1f);
			if (skill[12].to<int>() < 2) {
				if (!skill[38].to<bool>()) {
					baseValue -= (target.status().defence() * skill[21].to<int>() / 40) + (target.status().magic() * skill[22].to<int>() / 80);
				}
				if (target.attackInfo().type == kAttackTypeDefence) {
					if (status_.isStrongGuard())
						baseValue /= 4;
					else
						baseValue /= 2;
				}
			} else {
				result.cure = true;
			}
			baseValue = kuto::max(0, baseValue);
			if (skill[31].to<bool>()) result.hpDamage = baseValue;
			if (skill[32].to<bool>()) result.mpDamage = baseValue;
			if (skill[33].to<bool>()) result.attack = baseValue;
			if (skill[34].to<bool>()) result.magic = baseValue;
			if (skill[35].to<bool>()) result.defence = baseValue;
			if (skill[36].to<bool>()) result.speed = baseValue;
			result.absorption = skill[37].to<bool>();
			int hitRatio = skill[25];
			if (skill[7].to<int>() == 3) {
				hitRatio = (int)(100 - (100 - status_.hitRatio()) *
					(1.f + ((float)target.status().speed() / (float)status_.speed() - 1.f) / 2.f));
			}
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				rpg2k::Binary const& cond = skill[42];
				for (uint i = 0; i < cond.size(); i++) {
					if (cond[i]) result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
				}
				if (!result.cure) {
					if (target.status().hp() - result.hpDamage <= 0) {
						result.badConditions.push_back(1);	// 戦闘不能状態に
					}
				}
			}
		}
		break;
	case kAttackTypeItem:
		{
			const Array1D& item = project_.getLDB().item()[attackInfo.id];
			switch (item[3].to<int>()) {
			case rpg2k::Item::MEDICINE:
				if ((item[38].to<bool>() && target.status().hp() > 0) || (!item[38].to<bool>() && target.status().hp() == 0)) {
					// do nothing
				} else {
					result.hpDamage = (item[33].to<int>() * target.status().baseStatus()[rpg2k::Param::HP] / 100) + item[32].to<int>();
					result.mpDamage = (item[35].to<int>() * target.status().baseStatus()[rpg2k::Param::MP] / 100) + item[34].to<int>();
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
					result = attackResult(target, tempInfo);
					result.attackInfo = attackInfo;
				}
				break;
			}
		}
		break;
	default:
		{
			result.hpDamage = kuto::max(0, status_.attack() / 2 - target.status().defence() / 4);
			result.hpDamage = (int)(result.hpDamage * (kuto::random(0.4f) + 0.8f));
			int hitRatio = (int)(100 - (100 - status_.hitRatio()) *
				(1.f + ((float)target.status().speed() / (float)status_.speed() - 1.f) / 2.f));
			result.miss = kuto::random(100) >= hitRatio;
		}
		break;
	}
	return result;
}

float GameBattleChara::attackPriority() const
{
	if ((attackInfo_.type == kAttackTypeAttack || attackInfo_.type == kAttackTypeDoubleAttack) && status_.isFirstAttack())
		return 1000000.f;	// とりあえずでっかい値に
	return (float)status_.speed() + attackPriorityOffset_;
}

bool GameBattleChara::isActive() const
{
	if (isExcluded()) return false;
	for (uint i = 0; i < status_.badConditions().size(); i++) {
		if( project_.getLDB().condition()[status_.badConditions()[i].id][5].to<int>() == 1 ) return false;
	}
	return true;
}

int GameBattleChara::worstBadConditionID(bool doNotActionOnly) const
{
	int ret = 0;
	int pri = 0;
	for (uint i = 0; i < status_.badConditions().size(); i++) {
		const Array1D& cond = project_.getLDB().condition()[status_.badConditions()[i].id];
		if ((!doNotActionOnly || cond[5].to<int>() == 1) && cond[4].to<int>() > pri) {
			ret = status_.badConditions()[i].id;
			pri = cond[5].to<int>();
		}
	}
	return ret;
}

int GameBattleChara::actionLimit() const
{
	int ret = 0;
	int pri = 0;
	for (uint i = 0; i < status_.badConditions().size(); i++) {
		const Array1D& cond = project_.getLDB().condition()[status_.badConditions()[i].id];
		if ((cond[5].to<int>() == 2 || cond[5].to<int>() == 3) && cond[4].to<int>() > pri) {
			ret = cond[4];
			pri = cond[4];
		}
	}
	return ret;
}

void GameBattleChara::updateBadCondition()
{
	for (uint i = 0; i < status_.badConditions().size(); ) {
		if (status_.badConditions()[i].id != 1) {
			const Array1D& cond = project_.getLDB().condition()[status_.badConditions()[i].id];
			status_.badConditions()[i].count++;
			if (status_.badConditions()[i].count > cond[21].to<int>()) {
				if (cond[22].to<int>() > kuto::random(100)) {
					status_.removeBadCondition(i);
					continue;
				}
			}
		}
		i++;
	}

}



GameBattleEnemy::GameBattleEnemy(const rpg2k::model::Project& gameSystem, int enemyId)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 9.f), GameBattleChara(gameSystem)
, position_(0.f, 0.f), animationState_(kAnimationStateNone), animationCounter_(0)
, enemyId_(enemyId)
{
	const Array1D& enemy = project_.getLDB().enemy()[enemyId_];
	std::string background = project_.gameDir() + "/Monster/" + enemy[2].to_string().toSystem();
	bool res = RPG2kUtil::LoadImage(texture_, background, true); kuto_assert(res);
	status_.setEnemyStatus(project_, enemyId, GameConfig::kDifficultyNormal /* project_.config().difficulty */);
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

const std::string& GameBattleEnemy::name() const
{
	return project_.getLDB().enemy()[enemyId_][1];
}

void GameBattleEnemy::render(kuto::Graphics2D& g) const
{
	if (excluded_) return;

	const Array1D& enemy = project_.getLDB().enemy()[enemyId_];
	kuto::ColorHSV hsv = kuto::Color(1.f, 1.f, 1.f, 1.f).hsv();
	hsv.h += enemy[3].to<int>();
	if (hsv.h < 0)
		hsv.h += 360;
	else if (hsv.h >= 360)
		hsv.h -= 360;
	kuto::Color color = hsv.rgb();

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
	kuto::Vector2 scale(texture_.orgWidth(), texture_.orgHeight());
	kuto::Vector2 pos(position_.x - scale.x * 0.5f, position_.y - scale.y * 0.5f);
	pos += static_cast<GameBattle const*>(parent())->screenOffset();
	g.drawTexture(texture_, pos, scale, color, true);
}

void GameBattleEnemy::renderFlash(kuto::Graphics2D& g, const kuto::Color& color) const
{
	kuto::Vector2 scale(texture_.orgWidth(), texture_.orgHeight());
	kuto::Vector2 pos(position_.x - scale.x * 0.5f, position_.y - scale.y * 0.5f);
	pos += static_cast<GameBattle const*>(parent())->screenOffset();
	g.fillRectangleMask(pos, scale, color, texture_, true);
}

void GameBattleEnemy::setAttackInfoAuto(const GameBattlePlayerList& targets
, const GameBattleEnemyList& party, int turnNum)
{
	const Array1D& enemy = project_.getLDB().enemy()[enemyId_];
	const Array2D& attackPattern = enemy[42];
	AttackInfo info;
	std::vector<int> pattern;
	for (Array2D::ConstIterator it = attackPattern.begin()
	; it != attackPattern.end(); ++it)
	{
		switch ((*it->second)[5].to<int>()) {
		case 0:		// [常時]
			pattern.push_back(it->first);
			break;
		case 1:		// [スイッチ] S[A]がON
			if (project_.getLSD().flag((*it->second)[8].to<int>()))
				pattern.push_back(it->first);
			break;
		case 2:		// [ターン数] A×？+B ターン
			if ((*it->second)[6].to<int>() > 0) {
				if (turnNum % (*it->second)[6].to<int>() == (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			} else {
				if (turnNum == (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			}
			break;
		case 3:		// [グループ個体数] A〜B体
			{
				int partyNum = 0;
				for (uint iParty = 0; iParty < party.size(); iParty++) {
					if (!party[iParty]->isExcluded())
						partyNum++;
				}
				if (partyNum >= (*it->second)[6].to<int>() && partyNum <= (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			}
			break;
		case 4:		// [自分のHP] A〜B%
			{
				int hpRatio = status_.hp() * 100 / status_.baseStatus()[rpg2k::Param::HP];
				if (hpRatio >= (*it->second)[6].to<int>() && hpRatio <= (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			}
			break;
		case 5:		// [自分のMP] A〜B%
			{
				int mpRatio = status_.mp() * 100 / status_.baseStatus()[rpg2k::Param::MP];
				if (mpRatio >= (*it->second)[6].to<int>() && mpRatio <= (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			}
			break;
		case 6:		// [主人公平均Lv] A〜B
			{
				int level = 0;
				for (uint iTarget = 0; iTarget < targets.size(); iTarget++)
					level += targets[iTarget]->status().level();
				level /= targets.size();
				if (level >= (*it->second)[6].to<int>() && level <= (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			}
			break;
		case 7:		// [主人公消耗度] A〜B%
			{
				int hp = 0;
				for (uint iTarget = 0; iTarget < targets.size(); iTarget++)
					hp += targets[iTarget]->status().hp() * 100 / targets[iTarget]->status().baseStatus()[rpg2k::Param::HP];
				hp = 100 - hp / targets.size();
				if (hp >= (*it->second)[6].to<int>() && hp <= (*it->second)[7].to<int>())
					pattern.push_back(it->first);
			}
			break;
		}
	}
	int priorityMax = 0;
	for (uint i = 0; i < pattern.size(); i++) {
		priorityMax += attackPattern[ pattern[i] ][13].to<int>();
	}
	int attackIndex = -1;
	int priRange = kuto::random(priorityMax);
	for (uint i = 0; i < pattern.size(); i++) {
		priRange -= attackPattern[ pattern[i] ][13].to<int>();
		if (priRange < 0) {
			attackIndex = i;
			break;
		}
	}
	if (attackIndex >= 0) {
		const Array1D& pt = attackPattern[ pattern[attackIndex] ];
		switch (pt[1].to<int>()) {
		case 0:		// 基本行動
			info.target = targets[kuto::random(targets.size())];
			switch (pt[2].to<int>()) {
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
				const Array1D& skill = project_.getLDB().skill()[pt[3].to<int>()];
				switch (skill[12].to<int>()) {
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
				info.id = pt[3].to<int>();
			}
			break;
		case 2:		// 変身
			info.target = NULL;
			info.type = kAttackTypeTransform;
			info.id = pt[4].to<int>();
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

int GameBattleEnemy::resultExp() const
{
	return project_.getLDB().enemy()[enemyId_][11];
}

int GameBattleEnemy::resultMoney() const
{
	return project_.getLDB().enemy()[enemyId_][12];
}

int GameBattleEnemy::resultItem() const
{
	const Array1D& enemy = project_.getLDB().enemy()[enemyId_];
	if (kuto::random(100) < enemy[14].to<int>()) return enemy[13];
	return 0;
}


GameBattlePlayer::GameBattlePlayer(const rpg2k::model::Project& gameSystem, int playerId)
: GameBattleChara(gameSystem), playerId_(playerId)
, animationState_(kAnimationStateNone), animationCounter_(0)
{
	// status_ = status;
	if (status_.isDead()) setExcluded(true); // 最初から死んでるし
}

void GameBattlePlayer::setAttackInfoAuto(const GameBattleEnemyList& targets, const GameBattlePlayerList& party, int turnNum)
{
	AttackInfo info;
	switch (actionLimit()) {
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
			static_cast<GameBattle*>(parent())->setScreenOffset(offset);
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
	return project_.getLDB().character()[playerID()][23].to<bool>() || (actionLimit() != 0);
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

const std::string& GameBattlePlayer::name() const
{
	return project_.character(playerId_).name();
}
