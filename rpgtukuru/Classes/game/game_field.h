/**
 * @file
 * @brief Game Field
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_array.h>
#include <kuto/kuto_static_vector.h>
#include <kuto/kuto_task.h>

#include "game_config.h"

class Game;
class GameBattle;
class GameDebugMenu;
class GameEventManager;
class GameFadeEffect;
class GameMap;
class GamePictureManager;
class GameSystemMenu;

namespace rpg2k { namespace model { class Project; } }


class GameField : public kuto::Task, public kuto::TaskCreatorParam2<GameField, Game&, unsigned>
{
	friend class kuto::TaskCreatorParam2<GameField, Game&, unsigned>;
public:
	// typedef kuto::StaticVector<GamePlayer*, 4> PlayerList;
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

public:
	const rpg2k::model::Project& project() const { return project_; }
	rpg2k::model::Project& project() { return project_; }
	GameMap& map() { return map_; }
	GameBattle& battle() { return *battle_; }

	void startBattle(const std::string& terrain, int enemyGroupId, bool firstAttack, bool enableEscape, bool loseGameOver);
	void endBattle();
	int battleResult() const { return battleResult_; }
	void setFadeInfo(int place, int type) { fadeInfos_[place] = type; }
	void changeMap(int mapId, int x, int y);
	void fadeOut(int type);
	void fadeIn(int type);
	void startSystemMenu();
	void endSystemMenu();

	Game& game() { return game_; }
	GameSystemMenu& systemMenu() { return systemMenu_; }
	GameEventManager& eventManager() { return eventManager_; }
	GamePictureManager& pictureManager() { return pictManager_; }

private:
	GameField(Game& parent, unsigned lsdID);

	virtual bool initialize();
	virtual void update();

private:
	Game&				game_;
	rpg2k::model::Project&			project_;
	GameMap&			map_;
	GameEventManager&	eventManager_;
	GameBattle*			battle_;
	GameFadeEffect&		fadeEffect_;
	GameFadeEffect&		fadeEffectScreen_;
	int					battleResult_;
	bool				battleLoseGameOver_;
	kuto::Array<int, kFadePlaceMax>		fadeInfos_;
	State				state_;
	// GamePlayer*			dummyLeader_;
	GameSystemMenu&		systemMenu_;
	GamePictureManager&	pictManager_;

	GameDebugMenu&		debugMenu_;
};	// class GameField
