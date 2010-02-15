/**
 * @file
 * @brief Character Status
 * @author project.kuto
 */

#include <algorithm>
#include <kuto/kuto_utility.h>
#include "game_chara_status.h"
#include "game_battle_info.h"
#include "game_config.h"

using namespace rpg2kLib;

static const int EXP_TABLE[] = {
	0,10,26,49,82,125,181,249,331,426,536,660,801,958,1129,1351,1526,1750,1996,2262,2548,2863,3203,3570,3972,4407,4879,5396,5958,6571,7239,7973,8771,9663,10635,11708,12896,14215,15673,17300,19117,21150,23431,25997,28895,32187,35923,40189,45076,50692,
	0,10,27,53,91,144,215,304,414,544,698,878,1080,1308,1565,1850,2167,2516,2899,3322,3785,4295,4849,5460,6128,6864,7670,8562,9539,10616,11809,13132,14596,16219,18031,20050,22306,24842,27692,30897,34524,38638,43313,48648,54757,61782,69876,79252,90161,102899,
	0,10,28,57,102,166,254,369,514,691,903,1155,1445,1776,2152,2579,3055,3586,4178,4836,5564,6371,7262,8253,9347,10564,11912,13414,15081,16938,19011,21327,23926,26841,30124,33826,38009,42755,48148,54300,61333,69414,78708,89453,101911,116422,133395,153331,176851,204750,
	0,10,29,61,113,190,299,445,634,874,1164,1512,1918,2394,2939,3563,4270,5066,5963,6968,8094,9357,10766,12343,14104,16076,18286,20766,23550,26684,30210,34199,38709,43823,49635,56265,63833,72510,82481,93977,107277,122717,140706,161750,186464,215620,250165,291300,340530,399760,
	0,10,30,66,125,218,351,536,780,1095,1486,1961,2529,3199,3977,4882,5914,7093,8433,9952,11668,13608,15794,18266,21055,24204,27764,31792,36359,41543,47441,54166,61845,70634,80725,92338,105747,121265,139287,160292,184855,213692,247682,287900,335710,392809,461335,544000,644275,766606,
};


GameCharaStatusBase::GameCharaStatusBase()
: level_(0), exp_(0), hp_(0), mp_(0), attack_(0), defence_(0), magic_(0), speed_(0)
, hitRatio_(0), criticalRatio_(0.f)
, strongGuard_(false), charged_(false)
{
	itemUp_.maxHP = 0;
	itemUp_.maxMP = 0;
	itemUp_.attack = 0;
	itemUp_.defence = 0;
	itemUp_.magic = 0;
	itemUp_.speed = 0;
	
	equip_.weapon = 0;
	equip_.shield = 0;
	equip_.protector = 0;
	equip_.helmet = 0;
	equip_.accessory = 0;
}


GameCharaStatus::GameCharaStatus()
: rpgLdb_(NULL)
{
}

void GameCharaStatus::setPlayerStatus(const DataBase& rpgLdb, int playerId, int level, const Status& itemUp, const Equip& equip)
{
	rpgLdb_ = &rpgLdb;
	charaType_ = kCharaTypePlayer;
	charaId_ = playerId;
	level_ = level;
	itemUp_ = itemUp;
	equip_ = equip;
	exp_ = getLevelExp(level_);

	calcStatus(true);
}

void GameCharaStatus::setEnemyStatus(const DataBase& rpgLdb, int enemyId, int level)
{
	rpgLdb_ = &rpgLdb;
	charaType_ = kCharaTypeEnemy;
	charaId_ = enemyId;
	level_ = level;
	
	calcStatus(true);
}

void GameCharaStatus::calcStatus(bool resetHpMp)
{
	if (charaType_ == kCharaTypePlayer) {
		const Array1D& player = rpgLdb_->getCharacter()[charaId_];

		vector< uint16_t >status = static_cast< Binary& >(player[31]);

		charaStatus_.maxHP += status[LV_MAX*0 + level_];
		charaStatus_.maxMP += status[LV_MAX*1 + level_];
		charaStatus_.attack  += status[LV_MAX*2 + level_];
		charaStatus_.defence += status[LV_MAX*3 + level_];
		charaStatus_.magic   += status[LV_MAX*4 + level_];
		charaStatus_.speed   += status[LV_MAX*5 + level_];

/*
		if (level_ > LV_MAX) {
			// 50以上は適当にLiner
			charaStatus_.maxHP += (player.status[49].maxHP - player.status[48].maxHP) * (level_ - LV_MAX) / 2;
			charaStatus_.maxMP += (player.status[49].maxMP - player.status[48].maxMP) * (level_ - LV_MAX) / 2;
			charaStatus_.attack  += (player.status[49].attack  - player.status[48].attack ) * (level_ - LV_MAX) / 2;
			charaStatus_.defence += (player.status[49].defence - player.status[48].defence) * (level_ - LV_MAX) / 2;
			charaStatus_.magic   += (player.status[49].magic   - player.status[48].magic  ) * (level_ - LV_MAX) / 2;
			charaStatus_.speed   += (player.status[49].speed   - player.status[48].speed  ) * (level_ - LV_MAX) / 2;
		}
 */

		hitRatio_ = 90;				// 素手＝90%。装備で変動。
		criticalRatio_ = static_cast< bool >(player[8])
			? (1.f / (float) static_cast< bool >(player[10]) ) : 0.f;
		strongGuard_ = player[24];

		baseStatus_ = charaStatus_;
		calcStatusWeapon(equip_.weapon, false);
		calcStatusWeapon(equip_.shield, player[21]);

		calcStatusArmour(equip_.protector);
		calcStatusArmour(equip_.helmet);
		calcStatusArmour(equip_.accessory);
		baseStatus_.maxHP += itemUp_.maxHP;
		baseStatus_.maxMP += itemUp_.maxMP;
		baseStatus_.attack += itemUp_.attack;
		baseStatus_.defence += itemUp_.defence;
		baseStatus_.magic += itemUp_.magic;
		baseStatus_.speed += itemUp_.speed;
		calcLearnedSkills();
	} else {
		const Array1D& enemy = rpgLdb_->getEnemy()[charaId_];

		charaStatus_.maxHP += static_cast< int >(enemy[4]);
		charaStatus_.maxMP += static_cast< int >(enemy[5]);
		charaStatus_.attack  += static_cast< int >(enemy[6]);
		charaStatus_.defence += static_cast< int >(enemy[7]);
		charaStatus_.magic   += static_cast< int >(enemy[8]);
		charaStatus_.speed   += static_cast< int >(enemy[9]);

		hitRatio_ = static_cast< bool >(enemy[26]) ? 70 : 90;
		criticalRatio_ = static_cast< bool >(enemy[21])
			? (1.f / (float) static_cast< int >(enemy[22]) )
			: 0.f
			;
		strongGuard_ = false;

		static const float EASY = 0.8f, HARD = 1.5f;
		switch(level_) {
			case GameConfig::kDifficultyEasy:
				charaStatus_.defence *= EASY;
				charaStatus_.speed   *= EASY;
				break;
			case GameConfig::kDifficultyHard:
				charaStatus_.maxHP *= HARD;
				charaStatus_.attack *= HARD;
				charaStatus_.magic  *= HARD;
				break;
			default: break;
		}

		baseStatus_ = charaStatus_;
	}
	
	if (resetHpMp) {
		hp_ = baseStatus_.maxHP;
		mp_ = baseStatus_.maxMP;
	} else {
		hp_ = kuto::max(0, kuto::min((int)baseStatus_.maxHP, hp_));
		mp_ = kuto::max(0, kuto::min((int)baseStatus_.maxMP, mp_));
	}
	attack_ = baseStatus_.attack;
	defence_ = baseStatus_.defence;
	magic_ = baseStatus_.magic;
	speed_ = baseStatus_.speed;
	baseHitRatio_ = hitRatio_;
}

void GameCharaStatus::calcStatusWeapon(int equipId, bool second)
{
	if (equipId > 0) {
		const Array1D& item = rpgLdb_->getItem()[equipId];

		baseStatus_.attack  += static_cast< int >(item[11]);
		baseStatus_.defence += static_cast< int >(item[12]);
		baseStatus_.magic   += static_cast< int >(item[13]);
		baseStatus_.speed   += static_cast< int >(item[14]);

		if (!second || equip_.weapon == 0) {
			hitRatio_ = item[17];
		} else {
			hitRatio_ = ( hitRatio_ * static_cast< int >(item[17]) ) / 100;
		}

		criticalRatio_ =
			kuto::max(
				0.f,
				kuto::min(
					1.f,
					criticalRatio_ + ( static_cast< int >(item[18]) / 100.f)
				)
			);
	}
}

void GameCharaStatus::calcStatusArmour(int equipId)
{
	if (equipId > 0) {
		const Array1D& item = rpgLdb_->getItem()[equipId];

		baseStatus_.attack  += static_cast< int >(item[11]);
		baseStatus_.defence += static_cast< int >(item[12]);
		baseStatus_.magic   += static_cast< int >(item[13]);
		baseStatus_.speed   += static_cast< int >(item[14]);
	}
}

void GameCharaStatus::calcLearnedSkills()
{
	if (charaType_ == kCharaTypePlayer) {
		const Array2D& skillList = rpgLdb_->getCharacter()[charaId_][63];

		for(Array2D::Iterator it = skillList.begin(); it != skillList.end(); ++it) {
			if ( static_cast< int >(it.second()[1]) <= level_ )
				 learnSkill( it.second()[2] );
			else
				forgetSkill( it.second()[2] );
		}
	}
}

void GameCharaStatus::addDamage(const AttackResult& result)
{
	if (result.miss) return;

	int op = result.cure? 1 : -1;

	hp_ = kuto::max(0, kuto::min((int)baseStatus_.maxHP, hp_ + result.hpDamage * op));
	mp_ = kuto::max(0, kuto::min((int)baseStatus_.maxMP, mp_ + result.mpDamage * op));

	attack_  = kuto::max(baseStatus_.attack  / 2, kuto::min(baseStatus_.attack  * 2, attack_  + result.attack  * op));
	defence_ = kuto::max(baseStatus_.defence / 2, kuto::min(baseStatus_.defence * 2, defence_ + result.defence * op));
	magic_   = kuto::max(baseStatus_.magic   / 2, kuto::min(baseStatus_.magic   * 2, magic_   + result.magic   * op));
	speed_   = kuto::max(baseStatus_.speed   / 2, kuto::min(baseStatus_.speed   * 2, speed_   + result.speed   * op));

	for (u32 i = 0; i < result.badConditions.size(); i++) {
		int index = getBadConditionIndex(result.badConditions[i]);

		if (index == -1)
			addBadCondition(BadCondition(result.badConditions[i], 0));
		else if (result.cure)
			removeBadCondition(index);
	}
	
	if (isDead()) {
		// 死んだ時は死亡以外の状態異常を消去
		for (BadConditionList::iterator it = badConditions_.begin(); it != badConditions_.end();) {
			if (it->id == 1)
				++it;
			else
				it = badConditions_.erase(it);
		}
		calcBadCondition();
	}
}

void GameCharaStatus::calcBadCondition()
{
	hitRatio_ = baseHitRatio_;
	for (u32 i = 0; i < badConditions_.size(); i++) {
		const Array1D& cond = rpgLdb_->getCondition()[badConditions_[i].id];

		if ( static_cast< bool >(cond[31]) ) attack_  = baseStatus_.attack  / 2;
		if ( static_cast< bool >(cond[32]) ) defence_ = baseStatus_.defence / 2;
		if ( static_cast< bool >(cond[33]) ) magic_   = baseStatus_.magic   / 2;
		if ( static_cast< bool >(cond[34]) ) speed_   = baseStatus_.speed   / 2;

		hitRatio_ = hitRatio_ * static_cast< int >(cond[35]) / 100;
	}
}

bool GameCharaStatus::isDead() const
{
	for (u32 i = 0; i < badConditions_.size(); i++) {
		if (badConditions_[i].id == 1)
			return true;
	}
	return false;
}

bool GameCharaStatus::isDoubleAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const Array2D& items = rpgLdb_->getItem();

		if (
			static_cast< bool >(items[equip_.weapon][22]) ||
			static_cast< bool >(items[equip_.shield][22])
		) {
			return true;
		}
	}
	return false;
}

bool GameCharaStatus::isFirstAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const Array2D& items = rpgLdb_->getItem();

		if (
			static_cast< bool >(items[equip_.weapon][21]) ||
			static_cast< bool >(items[equip_.shield][21])
		) {
			return true;
		}
	}
	return false;
}

bool GameCharaStatus::isWholeAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const Array2D& items = rpgLdb_->getItem();

		if (
			static_cast< bool >(items[equip_.weapon][23]) ||
			static_cast< bool >(items[equip_.shield][23])
		) {
			return true;
		}
	}

	return false;
}

void GameCharaStatus::consumeMp(int value)
{
	mp_ -= value;
	if (mp_ < 0)
		mp_ = 0;
}

int GameCharaStatus::getBadConditionIndex(int id) const
{
	for (u32 i = 0; i < badConditions_.size(); i++) {
		if (badConditions_[i].id == id)
			return i;
	}
	return -1;
}

void GameCharaStatus::resetBattle()
{
	attack_ = baseStatus_.attack;
	defence_ = baseStatus_.defence;
	magic_ = baseStatus_.magic;
	speed_ = baseStatus_.speed;
	for (BadConditionList::iterator it = badConditions_.begin(); it != badConditions_.end();) {
		if ( static_cast< int >(rpgLdb_->getCondition()[it->id][2]) == 0 )
			it = badConditions_.erase(it);
		else
			++it;
	}
	calcBadCondition();
}

void GameCharaStatus::fullCure()
{
	hp_ = baseStatus_.maxHP;
	mp_ = baseStatus_.maxMP;
	attack_ = baseStatus_.attack;
	defence_ = baseStatus_.defence;
	magic_ = baseStatus_.magic;
	speed_ = baseStatus_.speed;
	badConditions_.clear();
}

void GameCharaStatus::kill()
{
	hp_ = 0;
	badConditions_.clear();
	addBadCondition(BadCondition(1, 0));
}

int GameCharaStatus::getAttackAnime() const
{
	if (charaType_ == kCharaTypePlayer) {
		return (equip_.weapon > 0)
			? rpgLdb_->getItem()[equip_.weapon][20]
			: rpgLdb_->getCharacter()[charaId_][56]
			;
	} else {
		return 1;
	}
}

void GameCharaStatus::addExp(int value)
{
	exp_ += value;
	
	for (int i = 0; i < LV_MAX; i++) {
		if (exp_ >= getLevelExp(LV_MAX - i)) {
			if (level_ != LV_MAX - i)
				setLevel(LV_MAX - i);
			break;
		}
	}
}

void GameCharaStatus::setLevel(int value)
{
	level_ = kuto::max(1, value);
	
	int levelExp = getLevelExp(level_);
	int nextLevelExp = getLevelExp(level_ + 1);
	if (exp_ < levelExp)
		exp_ = levelExp;
	else if (exp_ >= nextLevelExp)
		exp_ = nextLevelExp - 1;
	
	calcStatus(false);
}

void GameCharaStatus::addItemUp(const Status& itemUp)
{
	itemUp_.maxHP += itemUp.maxHP;
	itemUp_.maxMP += itemUp.maxMP;
	itemUp_.attack += itemUp.attack;
	itemUp_.defence += itemUp.defence;
	itemUp_.magic += itemUp.magic;
	itemUp_.speed += itemUp.speed;
	calcStatus(false);
}

void GameCharaStatus::setEquip(const Equip& equip)
{
	equip_ = equip;
	calcStatus(false);
}

int GameCharaStatus::getLevelExp(int level) const
{
	if (charaType_ != kCharaTypePlayer) return 0;
	if (level < 1) return 0;

	const Array1D& player = rpgLdb_->getCharacter()[charaId_];

	int expGain = player[42];
	int expLow =
		EXP_TABLE[
			kuto::min(4, (expGain - 10) / 10) * LV_MAX +
			(kuto::min(LV_MAX, level) - 1)
		];
	int expHigh =
		EXP_TABLE[
			kuto::min(4, (expGain - 10) / 10 + 1) * LV_MAX +
			(kuto::min(LV_MAX, level) - 1)
		];
	float ratio = (float)(expGain % 10) / 10.f;
	int exp = (int)(
		kuto::lerp(
			(float)expLow,
			(float)expHigh, ratio) * (float) static_cast< int >(player[43]) * 0.1f) +
			static_cast< int >(player[41]
		);
	if (level > LV_MAX) {
		// if over level 50, liner curve.
		exp += exp * (level - LV_MAX) / 4;
	}
	return exp;
}

bool GameCharaStatus::applyItem(int itemId)
{
	const Array1D& item = rpgLdb_->getItem()[itemId];

	switch ( static_cast< int >(item[3]) ) {
	case DataBase::kItemTypeMedicine: {
		bool forKnockout = item[58];
		if (
			( (forKnockout && isDead()) || (!forKnockout && !isDead()) ) &&
			(
				charaId_ > static_cast< int >(item[61]) ||
				static_cast< Binary& >(item[62])[charaId_]
			)
		) {
			hp_ =
				kuto::max(0,
					kuto::min( (int)baseStatus_.maxHP,
						hp_ +
						( static_cast< int >(item[33]) * baseStatus_.maxHP / 100 ) +
						static_cast< int >(item[32])
					)
				);
			mp_ =
				kuto::max(0,
					kuto::min( (int)baseStatus_.maxMP,
						mp_ +
						( static_cast< int >(item[35]) * baseStatus_.maxMP / 100 ) +
						static_cast< int >(item[34])
					)
				);
			uint condDataNum = item[63].get_uint();
			const Binary& condData = item[64];
			for (u32 i = 0; i < condDataNum; i++) {
				int index = getBadConditionIndex(i + 1);
				if (index >= 0 && condData[i]) {
					removeBadCondition(index);
				}
			}
			return true;
		}
	} break;
	case DataBase::kItemTypeBook:
		if (
			charaId_ > item[61].get_int() ||
			static_cast< Binary& >(item[62])[charaId_] ||
			!isLearnedSkill( item[53].get_uint() )
		) {
			learnSkill( item[53].get_uint() );
			return true;
		}
		break;
	case DataBase::kItemTypeSeed:
		if (
			charaId_ > static_cast< int >(item[61]) ||
			static_cast< Binary& >(item[62])[charaId_]
		) {
			Status itemUp;
			itemUp.maxHP = static_cast< int >(item[41]);
			itemUp.maxMP = static_cast< int >(item[42]);
			itemUp.attack  = static_cast< int >(item[43]);
			itemUp.defence = static_cast< int >(item[44]);
			itemUp.magic   = static_cast< int >(item[45]);
			itemUp.speed   = static_cast< int >(item[46]);
			addItemUp(itemUp);
			return true;
		}
		break;
	default: break;
	}

	return false;
}

bool GameCharaStatus::applySkill(int skillId, GameCharaStatus* owner)
{
	const Array1D& skill = rpgLdb_->getSkill()[skillId];
	switch ( static_cast< int >(skill[8]) ) {
	case DataBase::kSkillTypeNormal:
		{
			AttackResult result;
			int baseValue =
				static_cast< int >(skill[24]) +
				(owner->getAttack() * static_cast< int >(skill[21]) / 20) +
				(owner->getMagic () * static_cast< int >(skill[22]) / 40);
			baseValue += (int)(
				baseValue * (kuto::random(1.f) - 0.5f) *
				static_cast< int >(skill[23]) * 0.1f
			);
			baseValue = kuto::max(0, baseValue);

			result.cure = true;

			if ( static_cast< bool >(skill[31]) ) result.hpDamage = baseValue;
			if ( static_cast< bool >(skill[32]) ) result.mpDamage = baseValue;
			if ( static_cast< bool >(skill[33]) ) result.attack = baseValue;
			if ( static_cast< bool >(skill[34]) ) result.magic = baseValue;
			if ( static_cast< bool >(skill[35]) ) result.defence = baseValue;
			if ( static_cast< bool >(skill[36]) ) result.speed = baseValue;
			result.absorption = skill[37];

			int hitRatio = skill[25];
			if ( static_cast< int >(skill[7]) == 3 ) {
				hitRatio = (int)(100 - (100 - owner->getHitRatio()) *
					(1.f + ((float)getSpeed() / (float)owner->getSpeed() - 1.f) / 2.f));
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
					if (getHp() - result.hpDamage <= 0) {
						result.badConditions.push_back(1);	// 戦闘不能状態に
					}
				}
			}
			addDamage(result);
		}
		return true;
	}
	return false;
}
