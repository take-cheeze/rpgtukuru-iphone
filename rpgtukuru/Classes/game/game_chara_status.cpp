/**
 * @file
 * @brief Character Status
 * @author project.kuto
 */

#include <algorithm>
#include <kuto/kuto_utility.h>

#include <rpg2k/Project.hpp>

#include "game_chara_status.h"
#include "game_battle_info.h"
#include "game_config.h"


using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;

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
, itemUp_(rpg2k::Param::END), equip_(rpg2k::Equip::END)
{
}


GameCharaStatus::GameCharaStatus()
: project_(NULL)
{
}

void GameCharaStatus::setPlayerStatus(const rpg2k::model::Project& proj, int playerId, int level, const std::vector< uint16_t >& itemUp, const std::vector< uint16_t >& equip)
{
	project_ = &proj;
	charaType_ = kCharaTypePlayer;
	charaId_ = playerId;
	level_ = level;
	itemUp_ = itemUp;
	equip_ = equip;
	exp_ = levelExp(level_);

	calcStatus(true);
}

void GameCharaStatus::setEnemyStatus(const rpg2k::model::Project& proj, int enemyId, int level)
{
	project_ = &proj;
	charaType_ = kCharaTypeEnemy;
	charaId_ = enemyId;
	level_ = level;

	calcStatus(true);
}

void GameCharaStatus::calcStatus(bool resetHpMp)
{
	if (charaType_ == kCharaTypePlayer) {
		const Array1D& player = project_->getLDB().character()[charaId_];
		// charaStatus_ = player.status[kuto::min(49, level_ - 1)];
		rpg2k::model::Project::Character const& character = project_->character(charaId_);
		for(int i = rpg2k::Param::BEGIN; i < rpg2k::Param::END; i++) {
			charaStatus_[i] = character.basicParam(level_, rpg2k::Param::Type(i));
		}
/*
		if (level_ > rpg2k::LV_MAX) {
			// 50以上は適当にLiner
			charaStatus_[rpg2k::Param::HP] += (player.status[49][rpg2k::Param::HP] - player.status[48][rpg2k::Param::HP]) * (level_ - 50) / 2;
			charaStatus_[rpg2k::Param::MP] += (player.status[49][rpg2k::Param::MP] - player.status[48][rpg2k::Param::MP]) * (level_ - 50) / 2;
			charaStatus_[rpg2k::Param::ATTACK] += (player.status[49][rpg2k::Param::ATTACK] - player.status[48][rpg2k::Param::ATTACK]) * (level_ - 50) / 2;
			charaStatus_[rpg2k::Param::GAURD] += (player.status[49][rpg2k::Param::GAURD] - player.status[48][rpg2k::Param::GAURD]) * (level_ - 50) / 2;
			charaStatus_[rpg2k::Param::MIND] += (player.status[49][rpg2k::Param::MIND] - player.status[48][rpg2k::Param::MIND]) * (level_ - 50) / 2;
			charaStatus_[rpg2k::Param::SPEED] += (player.status[49][rpg2k::Param::SPEED] - player.status[48][rpg2k::Param::SPEED]) * (level_ - 50) / 2;
		}
 */
		hitRatio_ = 90;				// 素手＝90%。装備で変動。
		criticalRatio_ = player[9].to<bool>()? (1.f / (float)player[19].to<int>()) : 0.f;
		strongGuard_ = player[24].to<bool>();

		baseStatus_ = charaStatus_;
		calcStatusWeapon(equip_[rpg2k::Equip::WEAPON], false);
		if (player[21].to<bool>()) calcStatusWeapon(equip_[rpg2k::Equip::SHIELD], true);
		else calcStatusArmour(equip_[rpg2k::Equip::SHIELD]);
		calcStatusArmour(equip_[rpg2k::Equip::ARMOR ]);
		calcStatusArmour(equip_[rpg2k::Equip::HELMET]);
		calcStatusArmour(equip_[rpg2k::Equip::OTHER ]);
		for(int i = rpg2k::Param::BEGIN;i < rpg2k::Param::END; i++) baseStatus_[i] += itemUp_[i];
		calcLearnedSkills();
	} else {
		const Array1D& enemy = project_->getLDB().enemy()[charaId_];
		// charaStatus_ = enemy.status;
		for(int i = rpg2k::Param::BEGIN; i < rpg2k::Param::END; i++) {
			charaStatus_[i] = enemy[4 + i].to<int>();
		}
		hitRatio_ = enemy[26].to<bool>()? 70 : 90;
		criticalRatio_ = enemy[21].to<bool>()? (1.f / (float)enemy[22].to<int>()) : 0.f;
		strongGuard_ = false;
		if (level_ == GameConfig::kDifficultyEasy) { // Easy
			charaStatus_[rpg2k::Param::GAURD] = (kuto::u16)(charaStatus_[rpg2k::Param::GAURD] * 0.8f);
			charaStatus_[rpg2k::Param::SPEED] = (kuto::u16)(charaStatus_[rpg2k::Param::SPEED] * 0.8f);
		} else if (level_ == GameConfig::kDifficultyHard) { // Hard
			charaStatus_[rpg2k::Param::ATTACK] = (kuto::u16)(charaStatus_[rpg2k::Param::ATTACK] * 1.5f);
			charaStatus_[rpg2k::Param::MIND] = (kuto::u16)(charaStatus_[rpg2k::Param::MIND] * 1.5f);
			charaStatus_[rpg2k::Param::HP] = (kuto::u16)(charaStatus_[rpg2k::Param::HP] * 1.5f);
		}
		baseStatus_ = charaStatus_;
	}

	if (resetHpMp) {
		hp_ = baseStatus_[rpg2k::Param::HP];
		mp_ = baseStatus_[rpg2k::Param::MP];
	} else {
		hp_ = kuto::max(0, kuto::min((int)baseStatus_[rpg2k::Param::HP], hp_));
		mp_ = kuto::max(0, kuto::min((int)baseStatus_[rpg2k::Param::MP], mp_));
	}
	attack_ = baseStatus_[rpg2k::Param::ATTACK];
	defence_ = baseStatus_[rpg2k::Param::GAURD];
	magic_ = baseStatus_[rpg2k::Param::MIND];
	speed_ = baseStatus_[rpg2k::Param::SPEED];
	baseHitRatio_ = hitRatio_;
}

void GameCharaStatus::calcStatusWeapon(int equipId, bool second)
{
	if (equipId > 0) {
		const Array1D& item = project_->getLDB().item()[equipId];
		baseStatus_[rpg2k::Param::ATTACK] += item[11].to<int>();
		baseStatus_[rpg2k::Param::GAURD ] += item[12].to<int>();
		baseStatus_[rpg2k::Param::MIND  ] += item[13].to<int>();
		baseStatus_[rpg2k::Param::SPEED ] += item[14].to<int>();
		if (!second || equip_[rpg2k::Equip::WEAPON] == 0) {
			hitRatio_ = item[17].to<int>();
		} else {
			hitRatio_ = (hitRatio_ + item[17].to<int>()) / 2;
		}
		criticalRatio_ = kuto::max(0.f, kuto::min(1.f, criticalRatio_ + (item[18].to<int>() / 100.f)));
	}
}

void GameCharaStatus::calcStatusArmour(int equipId)
{
	if (equipId > 0) {
		const Array1D& item = project_->getLDB().item()[equipId];
		baseStatus_[rpg2k::Param::ATTACK] += item[11].to<int>();
		baseStatus_[rpg2k::Param::GAURD ] += item[12].to<int>();
		baseStatus_[rpg2k::Param::MIND  ] += item[13].to<int>();
		baseStatus_[rpg2k::Param::SPEED ] += item[14].to<int>();
	}
}

void GameCharaStatus::calcLearnedSkills()
{
	if (charaType_ == kCharaTypePlayer) {
		const Array1D& player = project_->getLDB().character()[charaId_];
		const Array2D& skillList = player[63];
		int curLevel = rpg2k::ID_MIN;
		for (Array2D::ConstIterator it = skillList.begin(); it != skillList.end(); ++it) {
			if( !it->second->exists() ) continue;

			if( it->second->exists(1) ) curLevel = (*it->second)[1];

			if (curLevel <= level_) learnSkill((*it->second)[2]);
			else forgetSkill((*it->second)[2]);
		}
	}
}

void GameCharaStatus::addDamage(const AttackResult& result)
{
	if (result.miss) return;

	int op = result.cure? 1 : -1;
	hp_ = kuto::max(0, kuto::min((int)baseStatus_[rpg2k::Param::HP], hp_ + result.hpDamage * op));
	mp_ = kuto::max(0, kuto::min((int)baseStatus_[rpg2k::Param::MP], mp_ + result.mpDamage * op));
	attack_ = kuto::max(baseStatus_[rpg2k::Param::ATTACK] / 2, kuto::min(baseStatus_[rpg2k::Param::ATTACK] * 2, attack_ + result.attack * op));
	defence_ = kuto::max(baseStatus_[rpg2k::Param::GAURD] / 2, kuto::min(baseStatus_[rpg2k::Param::GAURD] * 2, defence_ + result.defence * op));
	magic_ = kuto::max(baseStatus_[rpg2k::Param::MIND] / 2, kuto::min(baseStatus_[rpg2k::Param::MIND] * 2, magic_ + result.magic * op));
	speed_ = kuto::max(baseStatus_[rpg2k::Param::SPEED] / 2, kuto::min(baseStatus_[rpg2k::Param::SPEED] * 2, speed_ + result.speed * op));

	for (uint i = 0; i < result.badConditions.size(); i++) {
		int index = badConditionIndex(result.badConditions[i]);
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
	for (uint i = 0; i < badConditions_.size(); i++) {
		const Array1D& cond = project_->getLDB().condition()[badConditions_[i].id];
		if (cond[31].to<bool>())  attack_ = baseStatus_[rpg2k::Param::ATTACK] / 2;
		if (cond[32].to<bool>()) defence_ = baseStatus_[rpg2k::Param::GAURD ] / 2;
		if (cond[33].to<bool>())   magic_ = baseStatus_[rpg2k::Param::MIND  ] / 2;
		if (cond[34].to<bool>())   speed_ = baseStatus_[rpg2k::Param::SPEED ] / 2;
		hitRatio_ = hitRatio_ * cond[35].to<int>() / 100;
	}
}

bool GameCharaStatus::isDead() const
{
	for (uint i = 0; i < badConditions_.size(); i++) {
		if (badConditions_[i].id == 1)
			return true;
	}
	return false;
}

bool GameCharaStatus::isDoubleAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const Array1D& player = project_->getLDB().character()[charaId_];
		if (equip_[rpg2k::Equip::WEAPON] && project_->getLDB().item()[equip_[rpg2k::Equip::WEAPON]][22].to<bool>())
			return true;
		if (player[21].to<bool>() && equip_[rpg2k::Equip::SHIELD] && project_->getLDB().item()[equip_[rpg2k::Equip::SHIELD]][22].to<bool>())
			return true;
	}
	return false;
}

bool GameCharaStatus::isFirstAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const Array1D& player = project_->getLDB().character()[charaId_];
		if (equip_[rpg2k::Equip::WEAPON] && project_->getLDB().item()[equip_[rpg2k::Equip::WEAPON]][21].to<bool>())
			return true;
		if (player[21].to<bool>() && equip_[rpg2k::Equip::SHIELD] && project_->getLDB().item()[equip_[rpg2k::Equip::SHIELD]][21].to<bool>())
			return true;
	}
	return false;
}

bool GameCharaStatus::isWholeAttack() const
{
	if (charaType_ == kCharaTypePlayer) {
		const Array1D& player = project_->getLDB().character()[charaId_];
		if (equip_[rpg2k::Equip::WEAPON] && project_->getLDB().item()[equip_[rpg2k::Equip::WEAPON]][23].to<bool>())
			return true;
		if (player[21].to<bool>() && equip_[rpg2k::Equip::SHIELD] && project_->getLDB().item()[equip_[rpg2k::Equip::SHIELD]][23].to<bool>())
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

int GameCharaStatus::badConditionIndex(int id) const
{
	for (uint i = 0; i < badConditions_.size(); i++) {
		if (badConditions_[i].id == id)
			return i;
	}
	return -1;
}

void GameCharaStatus::resetBattle()
{
	attack_ = baseStatus_[rpg2k::Param::ATTACK];
	defence_ = baseStatus_[rpg2k::Param::GAURD];
	magic_ = baseStatus_[rpg2k::Param::MIND];
	speed_ = baseStatus_[rpg2k::Param::SPEED];
	for (BadConditionList::iterator it = badConditions_.begin(); it != badConditions_.end();) {
		if (project_->getLDB().condition()[it->id][2].to<int>() == 0)
			it = badConditions_.erase(it);
		else
			++it;
	}
	calcBadCondition();
}

void GameCharaStatus::fullCure()
{
	hp_ = baseStatus_[rpg2k::Param::HP];
	mp_ = baseStatus_[rpg2k::Param::MP];
	attack_ = baseStatus_[rpg2k::Param::ATTACK];
	defence_ = baseStatus_[rpg2k::Param::GAURD];
	magic_ = baseStatus_[rpg2k::Param::MIND];
	speed_ = baseStatus_[rpg2k::Param::SPEED];
	badConditions_.clear();
}

void GameCharaStatus::kill()
{
	hp_ = 0;
	badConditions_.clear();
	addBadCondition(BadCondition(1, 0));
}

int GameCharaStatus::attackAnime() const
{
	if (charaType_ == kCharaTypePlayer) {
		return equip_[rpg2k::Equip::WEAPON]
			? project_->getLDB().item()[equip_[rpg2k::Equip::WEAPON]][20] : project_->getLDB().character()[charaId_][56];
	} else {
		return 1;
	}
}

void GameCharaStatus::addExp(int value)
{
	exp_ += value;

	for (uint i = 0; i < 50; i++) {
		if (exp_ >= levelExp(50 - i)) {
			if (level_ != (int)(50 - i))
				setLevel(50 - i);
			break;
		}
	}
}

void GameCharaStatus::setLevel(int value)
{
	level_ = kuto::max(1, value);

	int levelExp = this->levelExp(level_);
	int nextLevelExp = this->levelExp(level_ + 1);
	if (exp_ < levelExp)
		exp_ = levelExp;
	else if (exp_ >= nextLevelExp)
		exp_ = nextLevelExp - 1;

	calcStatus(false);
}

void GameCharaStatus::addItemUp(const std::vector< uint16_t >& itemUp)
{
	itemUp_ = itemUp;;
	calcStatus(false);
}

void GameCharaStatus::setEquip(const std::vector< uint16_t >& equip)
{
	equip_ = equip;
	calcStatus(false);
}

int GameCharaStatus::levelExp(int level) const
{
	if (charaType_ != kCharaTypePlayer)
		return 0;
	if (level < 1)
		return 0;
	const Array1D& player = project_->getLDB().character()[charaId_];
	int expLow  = EXP_TABLE[kuto::min(4, (player[42].to<int>() - 10) / 10) * 50 + (kuto::min(50, level) - 1)];
	int expHigh = EXP_TABLE[kuto::min(4, (player[42].to<int>() - 10) / 10 + 1) * 50 + (kuto::min(50, level) - 1)];
	float ratio = (float)(player[42].to<int>() % 10) / 10.f;
	int exp = (int)(kuto::lerp((float)expLow, (float)expHigh, ratio) * (float)player[41].to<int>() * 0.1f) + player[43].to<int>();

	if (level > 50) {
		// if over level 50, liner curve.
		exp += exp * (level - 50) / 4;
	}
	return exp;
}

bool GameCharaStatus::applyItem(int const itemId)
{
	Array1D const& item = project_->getLDB().item()[itemId];
	switch (item[3].to<int>()) {
	case rpg2k::Item::MEDICINE:
		{
			if ((item[38].to<bool>() && !isDead()) || (!item[38].to<bool>() && isDead()))
				return false;
			if (charaId_ < (int)item[62].toBinary().size() && !item[62].toBinary()[charaId_])
				return false;
			hp_ = kuto::max(0, kuto::min((int)baseStatus_[rpg2k::Param::HP], hp_ + (item[33].to<int>() * baseStatus_[rpg2k::Param::HP] / 100) + item[32].to<int>()));
			mp_ = kuto::max(0, kuto::min((int)baseStatus_[rpg2k::Param::MP], mp_ + (item[35].to<int>() * baseStatus_[rpg2k::Param::MP] / 100) + item[34].to<int>()));
			rpg2k::Binary const& cond = item[64];
			for (uint i = 0; i < cond.size(); i++) {
				int index = badConditionIndex(i + 1);
				if (index >= 0 && cond[i]) {
					removeBadCondition(index);
				}
			}
		}
		return true;
	case rpg2k::Item::BOOK:
		if (charaId_ < (int)item[62].toBinary().size() && !item[62].toBinary()[charaId_])
			return false;
		if (isLearnedSkill(item[53].to<int>()))
			return false;
		learnSkill(item[53].to<int>());
		return true;
	case rpg2k::Item::SEED:
		if (charaId_ < (int)item[62].toBinary().size() && !item[62].toBinary()[charaId_])
			return false;
		else {
			std::vector< uint16_t > param;
			for(int i = rpg2k::Param::BEGIN; i < rpg2k::Param::END; i++) param.push_back( item[31 + i].to<int>() );
			addItemUp(param);
			return true;
		}
	}
	return false;
}

bool GameCharaStatus::applySkill(int skillId, GameCharaStatus* owner)
{
	const Array1D& skill = project_->getLDB().skill()[skillId];
	switch (skill[8].to<int>()) {
	case rpg2k::Skill::NORMAL:
		{
			AttackResult result;
			int baseValue = skill[24].to<int>() + (owner->attack() * skill[21].to<int>() / 20) + (owner->magic() * skill[22].to<int>() / 40);
			baseValue += (int)(baseValue * (kuto::random(1.f) - 0.5f) * skill[25].to<int>() * 0.1f);
			result.cure = true;
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
				hitRatio = (int)(100 - (100 - owner->hitRatio()) *
					(1.f + ((float)speed() / (float)owner->speed() - 1.f) / 2.f));
			}
			result.miss = kuto::random(100) >= hitRatio;
			if (!result.miss) {
				rpg2k::Binary const& cond = skill[42];
				for (uint i = 0; i < cond.size(); i++) {
					if (cond[i]) result.badConditions.push_back(i + 1);		// conditionは0〜格納されてる模様なので+1
				}
				if (!result.cure) {
					if (hp() - result.hpDamage <= 0) {
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
