/**
 * @file
 * @brief Game Main
 * @author project.kuto
 */
#pragma once

#include <string>
#include <kuto/kuto_error.h>
#include "game_texture_pool.h"
#include "game_system.h"

class GameField;
class GameTitle;
class GameOver;


/// Game Main Task
class Game : public kuto::Task
{
public:
	struct Option {
		std::string		projectName;

		Option(const std::string& projectName) : projectName(projectName) {}
	};
	static Game* createTask(Task* parent, const Option& option) { if (!instance_) instance_ = new Game(parent, option); return instance_; }
	static Game* instance() { kuto_assert(instance_); return instance_; }

private:
	Game(kuto::Task* parent, const Option& option);
	virtual ~Game();

	virtual bool initialize();
	virtual void update();

public:
	GameField* getGameField() { return gameField_; }
	GameTitle* gameTitle() { return gameTitle_; }
	void gameOver();
	void returnTitle();

	GameTexturePool& getTexPool() { return texPool_; }

private:
	static Game*		instance_;
	rpg2k::model::Project			gameSystem_;
	GameTexturePool texPool_;
	GameField*			gameField_;
	GameTitle*			gameTitle_;
	GameOver*			gameOver_;
};	// class Game
