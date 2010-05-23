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

void GameCharaStatus::setPlayerStatus(const CRpgLdb& rpgLdb, int playerId, int level, const CRpgLdb::Status& itemUp, const CRpgLdb::Equip& equip)
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

void GameCharaStatus::setEnemyStatus(const CRpgLdb& rpgLdb, int enemyId, int level)
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
		const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
		charaStatus_ = player.status[kuto::min(49, level_ - 1)];
		if (level_ > 50) {
			// 50以上は適当にLiner
			charaStatus_.maxHP += (player.status[49].maxHP - player.status[48].maxHP) * (level_ - 50) / 2;
			charaStatus_.maxMP += (player.status[49].maxMP - player.status[48].maxMP) * (level_ - 50) / 2;
			charaStatus_.attack += (player.status[49].attack - player.status[48].attack) * (level_ - 50) / 2;
			charaStatus_.defence += (player.status[49].defence - player.status[48].defence) * (level_ - 50) / 2;
			charaStatus_.magic += (player.status[49].magic - player.status[48].magic) * (level_ - 50) / 2;
			charaStatus_.speed += (player.status[49].speed - player.status[48].speed) * (level_ - 50) / 2;
		}
		hitRatio_ = 90;				// 素手＝90%。装備で変動。
		criticalRatio_ = player.criticalEnable? (1.f / (float)player.criticalRatio) : 0.f;
		strongGuard_ = player.strongGuard;
		
		baseStatus_ = charaStatus_;
		calcStatusWeapon(equip_.weapon, false);
		if (player.twinSword)
			calcStatusWeapon(equip_.shield, true);
		else
			calcStatusArmour(equip_.shield);
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
		const CRpgLdb::Enemy& enemy = rpgLdb_->saEnemy[charaId_];
		charaStatus_ = enemy.status;
		hitRatio_ = enemy.missRush? 70 : 90;
		criticalRatio_ = enemy.criticalEnable? (1.f / (float)enemy.criticalRatio) : 0.f;
		strongGuard_ = false;
		if (level_ == GameConfig::kDifficultyEasy) {			// Easy
			charaStatus_.defence = (u16)(charaStatus_.defence * 0.8f);
			charaStatus_.speed = (u16)(charaStatus_.speed * 0.8f);
		} else if (level_ == GameConfig::kDifficultyHard) {	// Hard
			charaStatus_.attack = (u16)(charaStatus_.attack * 1.5f);
			charaStatus_.magic = (u16)(charaStatus_.magic * 1.5f);		
			charaStatus_.maxHP = (u16)(charaStatus_.maxHP * 1.5f);		
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
		const CRpgLdb::Item& item = rpgLdb_->saItem[equipId];
		baseStatus_.attack += item.statusUpAttack;
		baseStatus_.defence += item.statusUpDefence;
		baseStatus_.magic += item.statusUpMagic;
		baseStatus_.speed += item.statusUpSpeed;
		if (!second || equip_.weapon == 0) {
			hitRatio_ = item.baseHitRatio;
		} else {
			hitRatio_ = (hitRatio_ + item.baseHitRatio) / 2;
		}
		criticalRatio_ = kuto::max(0.f, kuto::min(1.f, criticalRatio_ + (item.criticalUp / 100.f)));
	}
}

void GameCharaStatus::calcStatusArmour(int equipId)
{
	if (equipId > 0) {
		const CRpgLdb::Item& item = rpgLdb_->saItem[equipId];
		baseStatus_.attack += item.statusUpAttack;
		baseStatus_.defence += item.statusUpDefence;
		baseStatus_.magic += item.statusUpMagic;
		baseStatus_.speed += item.statusUpSpeed;
	}
}

void GameCharaStatus::calcLearnedSkills()
{
	if (charaType_ == kCharaTypePlayer) {
		const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
		for (u32 i = 1; i < player.learnSkill.size(); i++) {
			if (player.learnSkill[i].level <= level_) {
				learnSkill(player.learnSkill[i].skill);
			} else {
				forgetSkill(player.learnSkill[i].skill);
			}
		}
	}
}

void GameCharaStatus::addDamage(const AttackResult& result)
{
	if (result.miss)
		return;
	int op = result.cure? 1 : -1;
	hp_ = kuto::max(0, kuto::min((int)baseStatus_.maxHP, hp_ + result.hpDamage * op));
	mp_ = kuto::max(0, kuto::min((int)baseStatus_.maxMP, mp_ + result.mpDamage * op));
	attack_ = kuto::max(baseStatus_.attack / 2, kuto::min(baseStatus_.attack * 2, attack_ + result.attack * op));
	defence_ = kuto::max(baseStatus_.defence / 2, kuto::min(baseStatus_.defence * 2, defence_ + result.defence * op));
	magic_ = kuto::max(baseStatus_.magic / 2, kuto::min(baseStatus_.magic * 2, magic_ + result.magic * op));
	speed_ = kuto::max(baseStatus_.speed / 2, kuto::min(baseStatus_.speed * 2, speed_ + result.speed * op));
	
	for (u32 i = 0; i < result.badConditions.size(); i++) {
		int index = getBadConditionIndex(result.badConditions[i]);
		if (result.cure) {
			if (index != -1) {
				removeBadCondition(index);
			}
		} else {
			if (index == -1) {
				addBadCondition(BadCondition(result.badConditions[i], 0));
			}
		}
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
		const CRpgLdb::Condition& cond = rpgLdb_->saCondition[badConditions_[i].id];
		if (cond.changeAttack)
			attack_ = baseStatus_.attack / 2;
		if (cond.changeDefence)
			defence_ = baseStatus_.defence / 2;
		if (cond.changeMagic)
			magic_ = baseStatus_.magic / 2;
		if (cond.changeSpeed)
			speed_ = baseStatus_.speed / 2;
		hitRatio_ = hitRatio_ * cond.changeHitRatio / 100;
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
		const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
		if (equip_.weapon > 0 && rpgLdb_->saItem[equip_.weapon].doubleAttack)
			return true;
		if (player.twinSword && equip_.shield > 0 && rpgLdb_->saItem[equip_.shield].doubleAttack)
			return true;
	}
	return false;
}

bool GameCharaStatus::isFirstAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
		if (equip_.weapon > 0 && rpgLdb_->saItem[equip_.weapon].firstAttack)
			return true;
		if (player.twinSword && equip_.shield > 0 && rpgLdb_->saItem[equip_.shield].firstAttack)
			return true;
	}
	return false;
}

bool GameCharaStatus::isWholeAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
		if (equip_.weapon > 0 && rpgLdb_->saItem[equip_.weapon].wholeAttack)
			return true;
		if (player.twinSword && equip_.shield > 0 && rpgLdb_->saItem[equip_.shield].wholeAttack)
			return true;
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
		if (rpgLdb_->saCondition[it->id].type == 0)
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
		if (equip_.weapon > 0) {
			const CRpgLdb::Item& item = rpgLdb_->saItem[equip_.weapon];
			return item.anime;
		}
		const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
		return player.bareHandsAnime;
	} else {
		return 1;
	}
}

void GameCharaStatus::addExp(int value)
{
	exp_ += value;
	
	for (u32 i = 0; i < 50; i++) {
		if (exp_ >= getLevelExp(50 - i)) {
			if (level_ != (int)(50 - i))
				setLevel(50 - i);
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

void GameCharaStatus::addItemUp(const CRpgLdb::Status& itemUp)
{
	itemUp_.maxHP += itemUp.maxHP;
	itemUp_.maxMP += itemUp.maxMP;
	itemUp_.attack += itemUp.attack;
	itemUp_.defence += itemUp.defence;
	itemUp_.magic += itemUp.magic;
	itemUp_.speed += itemUp.speed;
	calcStatus(false);
}

void GameCharaStatus::setEquip(const CRpgLdb::Equip& equip)
{
	equip_ = equip;
	calcStatus(false);
}

int GameCharaStatus::getLevelExp(int level) const
{
	if (charaType_ != kCharaTypePlayer)
		return 0;
	if (level < 1)
		return 0;
	const CRpgLdb::Player& player = rpgLdb_->saPlayer[charaId_];
	int expLow = EXP_TABLE[kuto::min(4, (player.expGain - 10) / 10) * 50 + (kuto::min(50, level) - 1)];
	int expHigh = EXP_TABLE[kuto::min(4, (player.expGain - 10) / 10 + 1) * 50 + (kuto::min(50, level) - 1)];
	float ratio = (float)(player.expGain % 10) / 10.f;
	int exp = (int)(kuto::lerp((float)expLow, (float)expHigh, ratio) * (float)player.expBase * 0.1f) + player.expOffset;
	
	if (level > 50) {
		// if over level 50, liner curve.
		exp += exp * (level - 50) / 4;
	}
	return exp;
}

bool GameCharaStatus::applyItem(int itemId)
{
	const CRpgLdb::Item& item = rpgLdb_->saItem[itemId];
	switch (item.type) {
	case CRpgLdb::kItemTypeMedicine:
		if ((item.deadOnly && !isDead()) || (!item.deadOnly && isDead()))
			return false;
		if (charaId_ < (int)item.equipPlayer.size() && !item.equipPlayer[charaId_])
			return false;
		hp_ = kuto::max(0, kuto::min((int)baseStatus_.maxHP, hp_ + (item.cureHPRatio * baseStatus_.maxHP / 100) + item.cureHPValue));
		mp_ = kuto::max(0, kuto::min((int)baseStatus_.maxMP, mp_ + (item.cureMPRatio * baseStatus_.maxMP / 100) + item.cureMPValue));
		for (u32 i = 0; i < item.conditionChange.size(); i++) {
			int index = getBadConditionIndex(i + 1);
			if (index >= 0 && item.conditionChange[i]) {
				removeBadCondition(index);
			}
		}
		return true;
	case CRpgLdb::kItemTypeBook:
		if (charaId_ < (int)item.equipPlayer.size() && !item.equipPlayer[charaId_])
			return false;
		if (isLearnedSkill(item.skill))
			return false;
		learnSkill(item.skill);
		return true;
	case CRpgLdb::kItemTypeSeed:
		if (charaId_ < (int)item.equipPlayer.size() && !item.equipPlayer[charaId_])
			return false;
		{
			CRpgLdb::Status itemUp;
			itemUp.maxHP = item.statusChangeHP;
			itemUp.maxMP = item.statusChangeMP;
			itemUp.attack = item.statusChangeAttack;
			itemUp.defence = item.statusChangeDefence;
			itemUp.magic = item.statusChangeMagic;
			itemUp.speed = item.statusChangeSpeed;
			addItemUp(itemUp);
		}
		return true;
	}
	return false;
}

bool GameCharaStatus::applySkill(int skillId, GameCharaStatus* owner)
{
	const CRpgLdb::Skill& skill = rpgLdb_->saSkill[skillId];
	switch (skill.type) {
	case CRpgLdb::kSkillTypeNormal:
		{
			AttackResult result;
			int baseValue = skill.baseEffect + (owner->getAttack() * skill.attackRatio / 20) + (owner->getMagic() * skill.magicRatio / 40);
			baseValue += (int)(baseValue * (kuto::random(1.f) - 0.5f) * skill.variance * 0.1f);
			result.cure = true;
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
				hitRatio = (int)(100 - (100 - owner->getHitRatio()) *
					(1.f + ((float)getSpeed() / (float)owner->getSpeed() - 1.f) / 2.f));
			}
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				for (u32 i = 0; i < skill.conditionChange.size(); i++) {
					if (skill.conditionChange[i]) {
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
