/**
 * @file
 * @brief Game Battle
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_static_vector.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_vector2.h>

#include "game_battle_info.h"

namespace rpg2k { namespace model { class Project; } }

class GameBattleMap;
class GameBattleChara;
class GameBattlePlayer;
class GameBattleEnemy;
class GameBattleMenu;
class GameField;
class GameMessageWindow;
class GameSkillAnime;


class GameBattle : public kuto::Task, public kuto::TaskCreatorParam3<GameBattle, GameField&, const std::string&, int>
{
	friend class kuto::TaskCreatorParam3<GameBattle, GameField&, const std::string&, int>;
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
	void addPlayer(int playerId); // , GameCharaStatus& status);
	void setFirstAttack(bool value) { firstAttack_ = value; }
	void enableEscape(bool value = true) { enableEscape_ = value; }
	const rpg2k::model::Project& project() const { return project_; }
	rpg2k::model::Project& project() { return project_; }
	PlayerList& players() { return players_; }
	EnemyList& enemies() { return enemies_; }
	State state() const { return state_; }
	const kuto::Vector2& screenOffset() const { return screenOffset_; }
	void setScreenOffset(const kuto::Vector2& offset) { screenOffset_ = offset; }
	ResultType result() const { return resultType_; }
	GameField& field() { return field_; }

private:
	GameBattle(GameField& field, const std::string& terrain, int enemyGroupId);

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
	GameBattleChara* targetRandom(GameBattleChara* attacker);
	bool isWin() const;
	bool isLose() const;

private:
	GameField& field_;
	rpg2k::model::Project&			project_;
	GameBattleMap&		map_;
	PlayerList			players_;
	EnemyList			enemies_;
	GameBattleMenu&		menu_;
	State				state_;
	int					stateCounter_;
	GameMessageWindow&	messageWindow_;
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
