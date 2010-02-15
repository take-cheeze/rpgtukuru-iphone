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


GameBattleChara::GameBattleChara(kuto::Task* parent, const GameSystem& gameSystem)
: kuto::Task(parent), gameSystem_(gameSystem), status_(), attackPriorityOffset_(0.f), excluded_(false)
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
			const Array1D& skill = gameSystem_.getRpgLdb().getSkill()[attackInfo.id];
			int phyRel = skill[21], magRel = skill[22];

			int baseValue = static_cast< int >(skill[24]) + (status_.getAttack() * phyRel / 20) + (status_.getMagic() * magRel / 40);
			baseValue += (int)(baseValue * (kuto::random(1.f) - 0.5f) * static_cast< int >(skill[23]) * 0.1f);

			if ( static_cast< int >(skill[12]) < DataBase::kSkillScopeMyself ) {
				if (!static_cast< bool >(skill[38]) ) {
					baseValue -= (target.getStatus().getDefence() * phyRel / 40) + (target.getStatus().getMagic() * magRel / 80);
				}
				if (target.getAttackInfo().type == kAttackTypeDefence) {
					baseValue /= (status_.isStrongGuard() ? 4 : 2);
				}
			} else {
				result.cure = true;
			}
			baseValue = kuto::max(0, baseValue);
			if ( static_cast< bool >(skill[31]) ) result.hpDamage = baseValue;
			if ( static_cast< bool >(skill[32]) ) result.mpDamage = baseValue;
			if ( static_cast< bool >(skill[33]) ) result.attack = baseValue;
			if ( static_cast< bool >(skill[34]) ) result.magic = baseValue;
			if ( static_cast< bool >(skill[35]) ) result.defence = baseValue;
			if ( static_cast< bool >(skill[36]) ) result.speed = baseValue;
			result.absorption = skill[37];

			int hitRatio = skill[25];
			if ( static_cast< int >(skill[7]) == 3 ) {
				hitRatio = (int)(100 - (100 - status_.getHitRatio()) *
					(1.f + ((float)target.getStatus().getSpeed() / (float)status_.getSpeed() - 1.f) / 2.f));
			}
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				uint condDataNum = skill[41].get_uint();
				const Binary& condData = skill[42];

				for (u32 i = 0; i < condDataNum; i++) {
					if (condData[i]) {
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
			const Array1D& item = gameSystem_.getRpgLdb().getItem()[attackInfo.id];

			switch ( static_cast< int >(item[3]) ) {
			case DataBase::kItemTypeMedicine: {
				bool forKnockout = item[38];

				if (
					// item isn't for dead char and HP != 0
					(!forKnockout && target.getStatus().getHp() != 0) ||
					// item is    for dead char and HP == 0
					( forKnockout && target.getStatus().getHp() == 0)
				) {
					result.hpDamage = ( static_cast< int >(item[33]) * target.getStatus().getBaseStatus().maxHP / 100) + static_cast< int >(item[32]);
					result.mpDamage = ( static_cast< int >(item[35]) * target.getStatus().getBaseStatus().maxMP / 100) + static_cast< int >(item[34]);
					result.cure = true;

					uint length = item[63].get_uint();
					vector< uint8_t > condEnable = static_cast< Binary& >(item[64]);
					for (u32 i = 0; i < length; i++) {
						if (condEnable[i]) {
							result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
						}
					}
				}
			} break;
			case DataBase::kItemTypeSpecial:
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
	if (isExcluded())
		return false;
	for (u32 i = 0; i < status_.getBadConditions().size(); i++) {
		const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[status_.getBadConditions()[i].id];
		if ( static_cast< int >(cond[5]) == DataBase::kLimitActionDoNotAction )
			return false;
	}
	return true;
}

int GameBattleChara::getWorstBadConditionId(bool doNotActionOnly) const
{
	int ret = 0;
	int pri = 0;
	for (u32 i = 0; i < status_.getBadConditions().size(); i++) {
		const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[status_.getBadConditions()[i].id];

		int curPri = cond[4];

		if (
			(
				!doNotActionOnly ||
				static_cast< int >(cond[5]) == DataBase::kLimitActionDoNotAction
			) && curPri > pri
		) {
			ret = status_.getBadConditions()[i].id;
			pri = curPri;
		}
	}
	return ret;
}

DataBase::LimitActionType GameBattleChara::getLimitAction() const
{
	DataBase::LimitActionType ret = DataBase::kLimitActionNone;
	int pri = 0;
	for (u32 i = 0; i < status_.getBadConditions().size(); i++) {
		const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[status_.getBadConditions()[i].id];

		int curPri = cond[4];
		DataBase::LimitActionType restrict =
			(DataBase::LimitActionType) static_cast< int >(cond[5]);

		if (
			(
				restrict == DataBase::kLimitActionAttackEnemy ||
				restrict == DataBase::kLimitActionAttackFriend
			) && curPri > pri
		) {
			ret = restrict;
			pri = curPri;
		}
	}
	return ret;
}

void GameBattleChara::updateBadCondition()
{
	for (u32 i = 0; i < status_.getBadConditions().size(); ) {
		if (status_.getBadConditions()[i].id != 1) {
			const Array1D& cond = gameSystem_.getRpgLdb().getCondition()[status_.getBadConditions()[i].id];

			status_.getBadConditions()[i].count++;
			if ( status_.getBadConditions()[i].count > static_cast< int >(cond[21]) ) {
				if ( static_cast< int >(cond[22]) > kuto::random(100) ) {
					status_.removeBadCondition(i);
					continue;
				}
			}
		}
		i++;
	}
	
}



GameBattleEnemy::GameBattleEnemy(kuto::Task* parent, const GameSystem& gameSystem, int enemyId)
: GameBattleChara(parent, gameSystem), position_(0.f, 0.f)
, animationState_(kAnimationStateNone), animationCounter_(0), enemyId_(enemyId)
{
/*
	const Array1D& enemy = gameSystem_.getRpgLdb().getEnemy()[enemyId_];
	std::string background = gameSystem_.getRootFolder();
	background += "/Monster/" + enemy[2];
	CRpgUtil::LoadImage(texture_, background, true, enemy.graphicColor);
 */
	
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
	const Array1D& enemy = gameSystem_.getRpgLdb().getEnemy()[enemyId_];
	const Array2D& attackPattern = enemy[42];
	AttackInfo info;
	std::vector<int> pattern;
	// for (u32 i = 0; i < attackPattern.size(); i++) {
	for(Array2D::Iterator it = attackPattern.begin(); it != attackPattern.end(); ++it) {
		const Array1D& pt = it.second();
		int checkVal = 0;

		switch ( static_cast< int >(pt[5]) ) {
		case 0:		// [常時]
			pattern.push_back( it.first() );
			continue;
		case 1:		// [スイッチ] S[A]がON
			if ( gameSystem_.getSwitch(pt[6]) ) pattern.push_back( it.first() );
			continue;
		case 2:		// [ターン数] A×？+B ターン
		{
			int interval;
			int valA = pt[6], valB = pt[7];

			if (valA > 0) interval = turnNum % valA;
			else interval = turnNum;

			if (interval == valB) pattern.push_back( it.first() );
		} continue;
		case 3:		// [グループ個体数] A〜B体
			for (u32 iParty = 0; iParty < party.size(); iParty++) {
				if ( !party[iParty]->isExcluded() )
					checkVal++;
			}
			break;
		case 4:		// [自分のHP] A〜B%
			checkVal = status_.getHp() * 100 / status_.getBaseStatus().maxHP;
			break;
		case 5:		// [自分のMP] A〜B%
			checkVal = status_.getMp() * 100 / status_.getBaseStatus().maxMP;
			break;
		case 6:		// [主人公平均Lv] A〜B
			for (u32 iTarget = 0; iTarget < targets.size(); iTarget++) {
				checkVal += targets[iTarget]->getStatus().getLevel();
			}
			checkVal /= targets.size();
			break;
		case 7:		// [主人公消耗度] A〜B%
			for (u32 iTarget = 0; iTarget < targets.size(); iTarget++)
				checkVal
					+= (targets[iTarget]->getStatus().getHp() * 100)
					/  targets[iTarget]->getStatus().getBaseStatus().maxHP;
			checkVal = 100 - checkVal / targets.size();
			break;
		}

		if (
			( static_cast< int >(pt[6]) <= checkVal ) &&
			( checkVal <= static_cast< int >(pt[7]) )
		) pattern.push_back( it.first() );
	}
	int priorityMax = 0;
	for (u32 i = 0; i < pattern.size(); i++) {
		priorityMax += static_cast< int >(attackPattern[ pattern[i] ][13] );
	}
	int attackIndex = -1;
	int priRange = kuto::random(priorityMax);
	for (u32 i = 0; i < pattern.size(); i++) {
		priRange -= static_cast< int >( attackPattern[ pattern[i] ][13] );
		if (priRange < 0) {
			attackIndex = i;
			break;
		}
	}
	if (attackIndex >= 0) {
		const Array1D& pt = static_cast< Array2D& >(enemy[42])[pattern[attackIndex]];
		switch ( static_cast< int >(pt[1]) ) {
		case 0:		// 基本行動
			info.target = targets[rand() % targets.size()];
			switch ( static_cast< int >(pt[2]) ) {
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
				info.type = kAttackTypeSkill;
				info.id = pt[3];
				const Array1D& skill = gameSystem_.getRpgLdb().getSkill()[info.id];
				switch ( static_cast< int >(skill[12]) ) {
				case DataBase::kSkillScopeEnemySingle:
					info.target = targets[rand() % targets.size()];
					break;
				case DataBase::kSkillScopeFriendSingle:
					info.target = party[rand() % party.size()];
					break;
				case DataBase::kSkillScopeMyself:
					info.target = this;
					break;
				default:
					info.target = NULL;
				}
			}
			break;
		case 2:		// 変身
			info.target = NULL;
			info.type = kAttackTypeTransform;
			info.id = pt[4];
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
	const Array1D& enemy = gameSystem_.getRpgLdb().getEnemy()[enemyId_];
	return enemy[11];
}

int GameBattleEnemy::getResultMoney() const
{
	const Array1D& enemy = gameSystem_.getRpgLdb().getEnemy()[enemyId_];
	return enemy[12];
}

int GameBattleEnemy::getResultItem() const
{
	const Array1D& enemy = gameSystem_.getRpgLdb().getEnemy()[enemyId_];
	if ( enemy.exists(13) && kuto::random(100) < static_cast< int >(enemy[14]) )
		return enemy[13];
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
	case DataBase::kLimitActionNone:
		info.target = targets[rand() % targets.size()];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	case DataBase::kLimitActionDoNotAction:
		info.target = NULL;
		info.type = kAttackTypeNone;
		info.id = 0;
		break;
	case DataBase::kLimitActionAttackEnemy:
		info.target = targets[rand() % targets.size()];
		info.type = status_.isDoubleAttack()? kAttackTypeDoubleAttack : kAttackTypeAttack;
		info.id = 0;
		break;
	case DataBase::kLimitActionAttackFriend:
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
	default: break;
	}
}

bool GameBattlePlayer::isExecAI() const
{
	if ( static_cast< bool >(getPlayerInfo()[23]) )
		return true;
	if (getLimitAction() != DataBase::kLimitActionNone)
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


