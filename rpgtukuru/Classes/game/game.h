/**
 * @file
 * @brief Game Main
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_error.h>
#include <kuto/kuto_task.h>

#include <rpg2k/Project.hpp>

#include "game_config.h"
#include "game_texture_pool.h"

class GameField;
class GameTitle;
class GameOver;

/// Game Main Task
class Game : public kuto::Task, public kuto::TaskCreatorParam1<Game, GameConfig const&>
{
	friend class kuto::TaskCreatorParam1<Game, GameConfig const&>;
private:
	Game(GameConfig const& config);

	virtual bool initialize();
	virtual void update();

public:
	GameField& field() { return *field_; }
	GameTitle& gameTitle() { return *title_; }
	void gameOver();
	void returnTitle();

	GameTexturePool& texPool() { return texPool_; }
	kuto::Texture& systemTexture();

	GameConfig const& config() const { return config_; }
	GameConfig& config() { return config_; }

	rpg2k::model::Project& project() { return project_; }

private:
	rpg2k::model::Project	project_;
	GameTexturePool 		texPool_;
	GameConfig				config_;
	GameField*				field_;
	GameTitle*				title_;
	GameOver*				gameOver_;
};	// class Game
