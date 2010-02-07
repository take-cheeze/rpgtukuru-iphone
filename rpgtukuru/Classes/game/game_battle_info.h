/**
 * @file
 * @brief battle info
 * @author takuto
 */
#pragma once

#include <vector>
#include <kuto/kuto_types.h>

class GameBattleChara;


enum AttackType {
	kAttackTypeAttack,				///< 通常攻撃
	kAttackTypeSkill,				///< 特殊技能
	kAttackTypeDefence,				///< 防御
	kAttackTypeItem,				///< アイテム
	kAttackTypeDoubleAttack,		///< 連続攻撃
	kAttackTypeWaitAndSee,			///< 様子をみる
	kAttackTypeCharge,				///< 力を溜める
	kAttackTypeSuicideBombing,		///< 自爆する
	kAttackTypeEscape,				///< 逃げる
	kAttackTypeTransform,			///< 変身する
	kAttackTypeNone,				///< 何もしない
};


struct AttackInfo {
	GameBattleChara*		target;
	AttackType				type;
	int						id;
	
	AttackInfo() : target(NULL), type(kAttackTypeNone), id(0) {}
};


struct AttackResult {
	AttackInfo				attackInfo;
	int						hpDamage;
	int						mpDamage;
	int						attack;
	int						defence;
	int						magic;
	int						speed;
	struct {
		bool					miss		: 1;
		bool					critical	: 1;
		bool					cure		: 1;
		bool					absorption	: 1;
	};
	std::vector<int>		badConditions;
	
	AttackResult() : hpDamage(0), mpDamage(0), miss(false), critical(false), cure(false), absorption(false), attack(0), defence(0), magic(0), speed(0) {}
};

