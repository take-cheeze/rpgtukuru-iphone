/**
 * @file
 * @brief Game Battle Character
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_static_vector.h>
#include "game_chara_status.h"
#include "game_system.h"
#include "game_battle_info.h"


class GameBattleChara : public kuto::Task
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
	rpg2kLib::model::DataBase::LimitActionType getLimitAction() const;
	void updateBadCondition();

protected:
	GameBattleChara(kuto::Task* parent, const GameSystem& gameSystem);
	virtual ~GameBattleChara() {}
	
	virtual bool initialize();
	
protected:
	const GameSystem&	gameSystem_;
	GameCharaStatus		status_;
	AttackInfo			attackInfo_;
	float				attackPriorityOffset_;
	bool				excluded_;
};

class GameBattleEnemy;
class GameBattlePlayer;
typedef kuto::StaticVector<GameBattlePlayer*, 4> GameBattlePlayerList;
typedef kuto::StaticVector<GameBattleEnemy*, 8> GameBattleEnemyList;

class GameBattleEnemy : public GameBattleChara, public kuto::IRender
{
public:
	enum AnimationState {
		kAnimationStateNone,
		kAnimationStateDamage,
		kAnimationStateDead,	
	};
	
public:
	static GameBattleEnemy* createTask(kuto::Task* parent, const GameSystem& gameSystem, int enemyId) { return new GameBattleEnemy(parent, gameSystem, enemyId); }
	
	virtual void render();
	void renderFlash(const kuto::Color& color);
	const kuto::Vector2& getPosition() const { return position_; }
	void setPosition(const kuto::Vector2& position) { position_ = position; }
	kuto::Vector2 getScale() const { return kuto::Vector2(texture_.getOrgWidth(), texture_.getOrgHeight()); }
	void setAttackInfoAuto(const GameBattlePlayerList& targets, const GameBattleEnemyList& party, int turnNum);
	virtual const std::string& getName() const { return gameSystem_.getRpgLdb().getEnemy()[enemyId_][1]; }
	virtual Type getType() const { return kTypeEnemy; }
	virtual void playDamageAnime();
	virtual void playDeadAnime();
	virtual bool isAnimated() const { return animationState_ != kAnimationStateNone; }
	int getResultExp() const;
	int getResultMoney() const;
	int getResultItem() const;
	
private:
	GameBattleEnemy(kuto::Task* parent, const GameSystem& gameSystem, int enemyId);

	virtual void update();
	virtual void draw();

private:
	kuto::Vector2		position_;
	kuto::Texture		texture_;
	AnimationState		animationState_;
	int					animationCounter_;
	int					enemyId_;
};


class GameBattlePlayer : public GameBattleChara
{
public:
	enum AnimationState {
		kAnimationStateNone,
		kAnimationStateDamage,
		kAnimationStateDead,	
	};
	
public:
	static GameBattlePlayer* createTask(kuto::Task* parent, const GameSystem& gameSystem, int playerId, GameCharaStatus& status) { return new GameBattlePlayer(parent, gameSystem, playerId, status); }
	
	const rpg2kLib::structure::Array1D& getPlayerInfo() const { return gameSystem_.getRpgLdb().getCharacter()[playerId_]; }
	void setAttackInfoAuto(const GameBattleEnemyList& targets, const GameBattlePlayerList& party, int turnNum);
	bool isExecAI() const;
	virtual const std::string& getName() const { return gameSystem_.getRpgLdb().getCharacter()[playerId_][1]; }
	virtual Type getType() const { return kTypePlayer; }
	virtual void playDamageAnime();
	virtual void playDeadAnime();
	virtual bool isAnimated() const { return animationState_ != kAnimationStateNone; }
	int getPlayerId() const { return playerId_; }
	
private:
	GameBattlePlayer(kuto::Task* parent, const GameSystem& gameSystem, int playerId, GameCharaStatus& status);

	virtual void update();

private:
	int					playerId_;
	AnimationState		animationState_;
	int					animationCounter_;
};

