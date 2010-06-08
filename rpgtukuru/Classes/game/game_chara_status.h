/**
 * @file
 * @brief Character Status
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_static_vector.h>
#include <kuto/kuto_static_bitarray.h>
#include "CRpgLdb.h"

struct AttackResult;


class GameCharaStatusBase
{
public:
	struct BadCondition
	{
		int			id;
		int			count;
		
		BadCondition() : id(-1), count(0) {}
		BadCondition(int id, int count) : id(id), count(count) {}
	};
	typedef kuto::StaticVector<BadCondition, 32> BadConditionList;
	enum CharaType {
		kCharaTypePlayer,
		kCharaTypeEnemy,
	};
	typedef kuto::StaticBitArray<5000> LearnedSkillArray;
public:
	GameCharaStatusBase();
	
protected:
	int					charaType_;			///< キャラタイプ
	int					charaId_;			///< キャラID
	int					level_;				///< レベル
	int					exp_;				///< 経験値
	int					hp_;				///< HP
	int					mp_;				///< MP
	int					attack_;			///< 攻撃力
	int					defence_;			///< 防御力
	int					magic_;				///< 精神力
	int					speed_;				///< 敏捷力
	int					hitRatio_;			///< 命中力
	float				criticalRatio_;		///< クリティカル発生率
	bool				strongGuard_;		///< 強力防御
	bool				charged_;			///< チャージ中フラグ
	BadConditionList	badConditions_;		///< 状態異常IDリスト
	CRpgLdb::Status		itemUp_;			///< アイテムでのUP分
	CRpgLdb::Equip		equip_;				///< 装備情報
	LearnedSkillArray	learnedSkills_;		///< 習得スキル
};

class GameCharaStatus : public GameCharaStatusBase
{
public:
	GameCharaStatus();

	void setPlayerStatus(const CRpgLdb& rpgLdb, int playerId, int level, const CRpgLdb::Status& itemUp, const CRpgLdb::Equip& equip);
	void setEnemyStatus(const CRpgLdb& rpgLdb, int enemyId, int level);
	
	void calcStatus(bool resetHpMp);
	void resetBattle();
	void fullCure();
	
	void addDamage(const AttackResult& result);
	void consumeMp(int value);
	void addExp(int value);
	void addItemUp(const CRpgLdb::Status& itemUp);
	void setEquip(const CRpgLdb::Equip& equip);
	
	int getLevel() const { return level_; }
	void setLevel(int value);
	void addLevel(int value) { setLevel(level_ + value); }
	int getExp() const { return exp_; }
	int getLevelExp(int level) const;
	int getNextLevelExp() const { return getLevelExp(level_ + 1); }
	int getHp() const { return hp_; }
	int getMp() const { return mp_; }
	int getAttack() const { return attack_; }
	int getDefence() const { return defence_; }
	int getMagic() const { return magic_; }
	int getSpeed() const { return speed_; }
	int getHitRatio() const { return hitRatio_; }
	float getCriticalRatio() const { return criticalRatio_; }
	bool isStrongGuard() const { return strongGuard_; }
	const CRpgLdb::Status& getBaseStatus() const { return baseStatus_; }
	int getBadConditionIndex(int id) const;
	const CRpgLdb::Equip& getEquip() const { return equip_; }
	bool isDead() const;
	bool isDoubleAttack() const;
	bool isFirstAttack() const;
	bool isWholeAttack() const;

	const BadConditionList& getBadConditions() const { return badConditions_; }
	BadConditionList& getBadConditions() { return badConditions_; }
	void addBadCondition(const BadCondition& value) { badConditions_.push_back(value); calcBadCondition(); }
	void removeBadCondition(int index) { badConditions_.erase(badConditions_.begin() + index); calcBadCondition(); }
	bool isCharged() const { return charged_; }
	void setCharged(bool value) { charged_ = false; }
	int getAttackAnime() const;
	
	int getCharaType() const { return charaType_; }
	int getCharaId() const { return charaId_; }
	
	void learnSkill(unsigned int skillId) { learnedSkills_.set(skillId); }
	void forgetSkill(unsigned int skillId) { learnedSkills_.reset(skillId); }
	bool isLearnedSkill(unsigned int skillId) const { return learnedSkills_.get(skillId); }
	
	bool applyItem(int itemId);
	bool applySkill(int skillId, GameCharaStatus* owner);
	
	void kill();

private:
	void calcStatusWeapon(int equipId, bool second);
	void calcStatusArmour(int equipId);
	void calcLearnedSkills();
	void calcBadCondition();
	
private:
	const CRpgLdb* 		rpgLdb_;			///< RpgLdb
	CRpgLdb::Status		charaStatus_;		///< 素ステータス
	CRpgLdb::Status		baseStatus_;		///< 装備後ステータス
	int					baseHitRatio_;		///< 命中力
};
