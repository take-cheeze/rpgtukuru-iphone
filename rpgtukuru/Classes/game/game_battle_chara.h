/**
 * @file
 * @brief Game Battle Character
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_static_vector.h>
#include "game_chara_status.h"
#include "game_system.h"
#include "game_battle_info.h"


class GameBattleChara
{
public:
	enum Type {
		kTypeNone,
		kTypePlayer,
		kTypeEnemy,
	};

public:
	const GameCharaStatus& getStatus() const { return status_; }
	GameCharaStatus& getStatus() { return status_; }
	virtual const std::string& getName() const = 0;
	const AttackInfo& getAttackInfo() const { return attackInfo_; }
	void setAttackInfo(const AttackInfo& value) { attackInfo_ = value; }
	AttackResult getAttackResult(const GameBattleChara& target) const { return getAttackResult(target, attackInfo_); }
	AttackResult getAttackResult(const GameBattleChara& target, const AttackInfo& attackInfo) const;
	virtual Type getType() const { return kTypeNone; }
	float getAttackPriority() const;
	void setAttackPriorityOffset(float value) { attackPriorityOffset_ = value; }
	bool isExcluded() const { return excluded_; }
	void setExcluded(bool value) { excluded_ = value; }
	virtual void playDamageAnime() = 0;
	virtual void playDeadAnime() = 0;
	virtual bool isAnimated() const = 0;
	bool isActive() const;
	int getWorstBadConditionId(bool doNotActionOnly) const;
	/* DataBase::LimitActionType */ int getLimitAction() const;
	void updateBadCondition();

protected:
	GameBattleChara(const rpg2k::model::Project& gameSystem);

protected:
	const rpg2k::model::Project&	gameSystem_;
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
	virtual void render(kuto::Graphics2D* g) const;
	void renderFlash(const kuto::Color& color);
	const kuto::Vector2& getPosition() const { return position_; }
	void setPosition(const kuto::Vector2& position) { position_ = position; }
	kuto::Vector2 getScale() const { return kuto::Vector2(texture_.getOrgWidth(), texture_.getOrgHeight()); }
	void setAttackInfoAuto(const GameBattlePlayerList& targets, const GameBattleEnemyList& party, int turnNum);
	virtual const std::string& getName() const { return gameSystem_.getLDB().enemy()[enemyId_][1]; }
	virtual Type getType() const { return kTypeEnemy; }
	virtual void playDamageAnime();
	virtual void playDeadAnime();
	virtual bool isAnimated() const { return animationState_ != kAnimationStateNone; }
	int getResultExp() const;
	int getResultMoney() const;
	int getResultItem() const;

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


class GameBattlePlayer : public kuto::Task, public GameBattleChara, public kuto::TaskCreatorParam3<GameBattlePlayer, const rpg2k::model::Project&, int, GameCharaStatus&>
{
	friend class kuto::TaskCreatorParam3<GameBattlePlayer, const rpg2k::model::Project&, int, GameCharaStatus&>;
public:
	enum AnimationState {
		kAnimationStateNone,
		kAnimationStateDamage,
		kAnimationStateDead,
	};

public:
	// const GamePlayerInfo& getPlayerInfo() const { return gameSystem_.getPlayerInfo(playerId_); }
	void setAttackInfoAuto(const GameBattleEnemyList& targets, const GameBattlePlayerList& party, int turnNum);
	bool isExecAI() const;
	virtual const std::string& getName() const { return gameSystem_.name(playerId_); } // .getPlayerInfo(playerId_).name; }
	virtual Type getType() const { return kTypePlayer; }
	virtual void playDamageAnime();
	virtual void playDeadAnime();
	virtual bool isAnimated() const { return animationState_ != kAnimationStateNone; }
	int getPlayerId() const { return playerId_; }

private:
	GameBattlePlayer(const rpg2k::model::Project& gameSystem, int playerId, GameCharaStatus& status);

	virtual void update();
	virtual bool initialize();

private:
	int					playerId_;
	AnimationState		animationState_;
	int					animationCounter_;
};
