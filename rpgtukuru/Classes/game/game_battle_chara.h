/**
 * @file
 * @brief Game Battle Character
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_static_vector.h>
#include <kuto/kuto_texture.h>

#include "game_chara_status.h"
#include "game_battle_info.h"

namespace rpg2k { namespace model { class Project; } }


class GameBattleChara
{
public:
	enum Type {
		kTypeNone,
		kTypePlayer,
		kTypeEnemy,
	};

public:
	const GameCharaStatus& status() const { return status_; }
	GameCharaStatus& status() { return status_; }
	virtual const std::string& name() const = 0;
	const AttackInfo& attackInfo() const { return attackInfo_; }
	void setAttackInfo(const AttackInfo& value) { attackInfo_ = value; }
	AttackResult attackResult(const GameBattleChara& target) const { return attackResult(target, attackInfo_); }
	AttackResult attackResult(const GameBattleChara& target, const AttackInfo& attackInfo) const;
	virtual Type type() const { return kTypeNone; }
	float attackPriority() const;
	void setAttackPriorityOffset(float value) { attackPriorityOffset_ = value; }
	bool isExcluded() const { return excluded_; }
	void setExcluded(bool value) { excluded_ = value; }
	virtual void playDamageAnime() = 0;
	virtual void playDeadAnime() = 0;
	virtual bool isAnimated() const = 0;
	bool isActive() const;
	int worstBadConditionID(bool doNotActionOnly) const;
	int actionLimit() const;
	void updateBadCondition();

protected:
	GameBattleChara(const rpg2k::model::Project& gameSystem);

protected:
	const rpg2k::model::Project&	project_;
	GameCharaStatus		status_;
	AttackInfo			attackInfo_;
	float				attackPriorityOffset_;
	bool				excluded_;
};

class GameBattleEnemy;
class GameBattlePlayer;
typedef kuto::StaticVector<GameBattlePlayer*, 4> GameBattlePlayerList;
typedef kuto::StaticVector<GameBattleEnemy*, 8> GameBattleEnemyList;

class GameBattleEnemy : public kuto::IRender2D, public GameBattleChara, public kuto::TaskCreatorParam2<GameBattleEnemy, rpg2k::model::Project const&, int>
{
	friend class kuto::TaskCreatorParam2<GameBattleEnemy, rpg2k::model::Project const&, int>;
public:
	enum AnimationState {
		kAnimationStateNone,
		kAnimationStateDamage,
		kAnimationStateDead,
	};

public:
	virtual void render(kuto::Graphics2D& g) const;
	void renderFlash(kuto::Graphics2D& g, const kuto::Color& color) const;
	const kuto::Vector2& position() const { return position_; }
	void setPosition(const kuto::Vector2& position) { position_ = position; }
	kuto::Vector2 scale() const { return kuto::Vector2(texture_.orgWidth(), texture_.orgHeight()); }
	void setAttackInfoAuto(const GameBattlePlayerList& targets, const GameBattleEnemyList& party, int turnNum);
	virtual const std::string& name() const;
	virtual Type type() const { return kTypeEnemy; }
	virtual void playDamageAnime();
	virtual void playDeadAnime();
	virtual bool isAnimated() const { return animationState_ != kAnimationStateNone; }
	int resultExp() const;
	int resultMoney() const;
	int resultItem() const;

private:
	GameBattleEnemy(const rpg2k::model::Project& gameSystem, int enemyId);

	virtual void update();
	virtual bool initialize();

private:
	kuto::Vector2		position_;
	kuto::Texture		texture_;
	AnimationState		animationState_;
	int					animationCounter_;
	int					enemyId_;
};


class GameBattlePlayer : public kuto::Task, public GameBattleChara
, public kuto::TaskCreatorParam2<GameBattlePlayer, const rpg2k::model::Project&, int>
{
	friend class kuto::TaskCreatorParam2<GameBattlePlayer, const rpg2k::model::Project&, int>;
public:
	enum AnimationState {
		kAnimationStateNone,
		kAnimationStateDamage,
		kAnimationStateDead,
	};

public:
	void setAttackInfoAuto(const GameBattleEnemyList& targets, const GameBattlePlayerList& party, int turnNum);
	bool isExecAI() const;
	virtual const std::string& name() const;
	virtual Type type() const { return kTypePlayer; }
	virtual void playDamageAnime();
	virtual void playDeadAnime();
	virtual bool isAnimated() const { return animationState_ != kAnimationStateNone; }
	int playerID() const { return playerId_; }

private:
	GameBattlePlayer(const rpg2k::model::Project& gameSystem, int playerId);

	virtual void update();
	virtual bool initialize();

private:
	int					playerId_;
	AnimationState		animationState_;
	int					animationCounter_;
};
