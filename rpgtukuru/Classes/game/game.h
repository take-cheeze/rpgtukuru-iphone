/**
 * @file
 * @brief Game Main
 * @author project.kuto
 */
#pragma once

#include <string>
#include <memory>
#include <kuto/kuto_error.h>

#include "game_config.h"
#include "game_system.h"
#include "game_texture_pool.h"

class GameField;
class GameTitle;
class GameOver;

/// Game Main Task
class Game : public kuto::Task, public kuto::TaskCreatorParam1<Game, GameConfig const&>
{
public:
	Game(const GameConfig& config);

private:
	virtual bool initialize();
	virtual void update();

public:
	GameField* getGameField() { return gameField_; }
	GameTitle* gameTitle() { return gameTitle_; }
	void gameOver();
	void returnTitle();

	GameTexturePool& getTexPool() { return texPool_; }

	GameConfig const& getConfig() const { return config_; }
	GameConfig& getConfig() { return config_; }

private:
	rpg2k::model::Project	gameSystem_;
	GameTexturePool 		texPool_;
	GameConfig				config_;
	GameField*				gameField_;
	GameTitle*				gameTitle_;
	GameOver*				gameOver_;
};	// class Game
