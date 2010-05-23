/**
 * @file
 * @brief Game Field
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_static_vector.h>
#include "game_system.h"

class GameMap;
class GameCollision;
class GamePlayer;
class GameEventManager;
class GameBattle;
class Game;
class GameFadeEffect;
class GameSystemMenu;
class GameDebugMenu;


class GameField : public kuto::Task
{
public:
	typedef kuto::StaticVector<GamePlayer*, 4> PlayerList;
	enum FadePlace {
		kFadePlaceMapHide,
		kFadePlaceMapShow,
		kFadePlaceBattleStartHide,
		kFadePlaceBattleStartShow,
		kFadePlaceBattleEndHide,
		kFadePlaceBattleEndShow,
		kFadePlaceMax		
	};
	enum State {
		kStateField,
		kStateBattleStart,
		kStateBattle,
		kStateBattleEnd,
		kStateChangeMap,
		kStateSystemMenuStart,
		kStateSystemMenu,
		kStateSystemMenuEnd,
	};
	struct MapChangeInfo {
		int		mapId;
		int		x;
		int		y;
		int		dir;
	};

public:
	static GameField* createTask(Game* parent, GameSystem& gameSystem, int saveId) { return new GameField(parent, gameSystem, saveId); }

	const GameSystem& getGameSystem() const { return gameSystem_; }
	GameSystem& getGameSystem() { return gameSystem_; }
	GamePlayer* getPlayerLeader() { return gamePlayers_.empty()? dummyLeader_ : gamePlayers_[0]; }
	GamePlayer* getPlayerFromId(int playerId);
	PlayerList& getPlayers() { return gamePlayers_; }
	void addPlayer(int playerId);
	void removePlayer(int playerId);
	GameMap* getMap() { return gameMap_; }
	GameCollision* getCollision() { return gameCollision_; }
	GameBattle* getGameBattle() { return gameBattle_; }
	
	void startBattle(const std::string& terrain, int enemyGroupId, bool firstAttack, bool enableEscape, bool loseGameOver);
	void endBattle();
	int getBattleResult() const { return battleResult_; }
	void gameOver();
	void returnTitle();
	void setFadeInfo(int place, int type) { fadeInfos_[place] = type; }
	void changeMap(int mapId, int x, int y, int dir);
	void fadeOut(int type);
	void fadeIn(int type);
	void startSystemMenu();
	void endSystemMenu();
	
private:
	GameField(Game* parent, GameSystem& gameSystem, int saveId);
	virtual ~GameField();
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

private:
	Game*				game_;
	GameSystem&			gameSystem_;
	GameMap*			gameMap_;
	PlayerList			gamePlayers_;
	GameEventManager*	gameEventManager_;
	GameCollision*		gameCollision_;
	GameBattle*			gameBattle_;
	GameFadeEffect*		fadeEffect_;
	GameFadeEffect*		fadeEffectScreen_;
	int					battleResult_;
	bool				battleLoseGameOver_;
	kuto::Array<int, kFadePlaceMax>		fadeInfos_;
	State				state_;
	MapChangeInfo		mapChangeInfo_;
	GamePlayer*			dummyLeader_;
	GameSystemMenu*		systemMenu_;
	
	GameDebugMenu*		debugMenu_;
};	// class GameField

