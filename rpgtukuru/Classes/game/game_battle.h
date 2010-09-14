/**
 * @file
 * @brief Game Battle
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_static_vector.h>
#include "game_system.h"
#include "game_battle_info.h"

class GameBattleMap;
class GameBattleChara;
class GameBattlePlayer;
class GameBattleEnemy;
class GameBattleMenu;
class GameMessageWindow;
class GameSkillAnime;
class GameCharaStatus;


class GameBattle : public kuto::Task, public kuto::TaskCreatorParam3<GameBattle, rpg2k::model::Project&, const std::string&, int>
{
	friend class kuto::TaskCreatorParam3<GameBattle, rpg2k::model::Project&, const std::string&, int>;
public:
	typedef kuto::StaticVector<GameBattlePlayer*, 4> PlayerList;
	typedef kuto::StaticVector<GameBattleEnemy*, 8> EnemyList;
	typedef kuto::StaticVector<GameBattleChara*, 24> CharaList;
	typedef kuto::StaticVector<AttackResult, 24> AttackResultList;
	enum State {
		kStateStart,
		kStateFirstAttack,
		kStateMenu,
		kStateEscape,
		kStateAnimation,
		kStateResult,
		kStateLose,
		kStateEnd,
	};
	enum ResultType {
		kResultWin,
		kResultLose,
		kResultEscape,
	};

public:
	void addPlayer(int playerId, GameCharaStatus& status);
	void setFirstAttack(bool value) { firstAttack_ = value; }
	void setEnableEscape(bool value) { enableEscape_ = value; }
	const rpg2k::model::Project& getGameSystem() const { return gameSystem_; }
	rpg2k::model::Project& getGameSystem() { return gameSystem_; }
	PlayerList& getPlayers() { return players_; }
	EnemyList& getEnemies() { return enemies_; }
	State getState() const { return state_; }
	const kuto::Vector2& getScreenOffset() const { return screenOffset_; }
	void setScreenOffset(const kuto::Vector2& offset) { screenOffset_ = offset; }
	ResultType getResult() const { return resultType_; }

private:
	GameBattle(rpg2k::model::Project& gameSystem, const std::string& terrain, int enemyGroupId);

	virtual bool initialize();
	virtual void update();

	void setState(State newState);
	void setStartMessage();
	void setEscapeMessage();
	void setLoseMessage();
	void setResultMessage();
	void setAnimationMessage();
	void calcBattleOrder();

	void setAnimationMessageMagicSub(GameBattleChara* attacker, GameBattleChara* target);
	GameBattleChara* getTargetRandom(GameBattleChara* attacker);
	bool isWin();
	bool isLose();

private:
	rpg2k::model::Project&			gameSystem_;
	GameBattleMap*		map_;
	PlayerList			players_;
	EnemyList			enemies_;
	GameBattleMenu*		menu_;
	State				state_;
	int					stateCounter_;
	GameMessageWindow*	messageWindow_;
	CharaList			battleOrder_;
	int					currentAttacker_;
	AttackResultList	attackResults_;
	bool				firstAttack_;
	bool				enableEscape_;
	bool				escapeSuccess_;
	int					escapeNum_;
	CharaList			attackedTargets_;
	int					animationTargetIndex_;
	GameSkillAnime*		skillAnime_;
	kuto::Vector2		screenOffset_;
	ResultType			resultType_;
	int					turnNum_;
};	// class GameBattle

