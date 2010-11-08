/**
 * @file
 * @brief Character Status
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_static_vector.h>
#include <kuto/kuto_static_bitarray.h>

struct AttackResult;
namespace rpg2k { namespace model { class Project; } }


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
	std::vector< uint16_t >		itemUp_;			///< アイテムでのUP分
	std::vector< uint16_t >		equip_;				///< 装備情報
	LearnedSkillArray	learnedSkills_;		///< 習得スキル
};

class GameCharaStatus : public GameCharaStatusBase
{
public:
	GameCharaStatus();

	void setPlayerStatus(rpg2k::model::Project const& proj, int playerId, int level, const std::vector< uint16_t >& itemUp, const std::vector< uint16_t >& equip);
	void setEnemyStatus(rpg2k::model::Project const& proj, int enemyId, int level);

	void calcStatus(bool resetHpMp);
	void resetBattle();
	void fullCure();

	void addDamage(const AttackResult& result);
	void consumeMp(int value);
	void addExp(int value);
	void addItemUp(const std::vector< uint16_t >& itemUp);
	void setEquip(const std::vector< uint16_t >& equip);

	int level() const { return level_; }
	void setLevel(int value);
	void addLevel(int value) { setLevel(level_ + value); }
	int exp() const { return exp_; }
	int levelExp(int level) const;
	int nextLevelExp() const { return levelExp(level_ + 1); }
	int hp() const { return hp_; }
	int mp() const { return mp_; }
	int attack() const { return attack_; }
	int defence() const { return defence_; }
	int magic() const { return magic_; }
	int speed() const { return speed_; }
	int hitRatio() const { return hitRatio_; }
	float criticalRatio() const { return criticalRatio_; }
	bool isStrongGuard() const { return strongGuard_; }
	const std::vector< uint16_t >& baseStatus() const { return baseStatus_; }
	int badConditionIndex(int id) const;
	const std::vector< uint16_t >& equip() const { return equip_; }
	bool isDead() const;
	bool isDoubleAttack() const;
	bool isFirstAttack() const;
	bool isWholeAttack() const;

	const BadConditionList& badConditions() const { return badConditions_; }
	BadConditionList& badConditions() { return badConditions_; }
	void addBadCondition(const BadCondition& value) { badConditions_.push_back(value); calcBadCondition(); }
	void removeBadCondition(int index) { badConditions_.erase(badConditions_.begin() + index); calcBadCondition(); }
	bool isCharged() const { return charged_; }
	void setCharged(bool value) { charged_ = false; }
	int attackAnime() const;

	int charaType() const { return charaType_; }
	int charaID() const { return charaId_; }

	void learnSkill(uint skillId) { learnedSkills_.set(skillId); }
	void forgetSkill(uint skillId) { learnedSkills_.reset(skillId); }
	bool isLearnedSkill(uint skillId) const { return learnedSkills_.get(skillId); }

	bool applyItem(int itemId);
	bool applySkill(int skillId, GameCharaStatus* owner);

	void kill();

private:
	void calcStatusWeapon(int equipId, bool second);
	void calcStatusArmour(int equipId);
	void calcLearnedSkills();
	void calcBadCondition();

private:
	const rpg2k::model::Project* 		project_;			///< RpgLdb
	std::vector< uint16_t >		charaStatus_;		///< 素ステータス
	std::vector< uint16_t >		baseStatus_;		///< 装備後ステータス
	int					baseHitRatio_;		///< 命中力
};
