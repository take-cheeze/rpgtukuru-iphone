/**
 * @file
 * @brief Game Battle Map
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include "game_system.h"


class GameBattleMap : public kuto::Task, public kuto::IRender
{
public:
	static GameBattleMap* createTask(kuto::Task* parent, const GameSystem& gameSystem, const std::string& terrain) { return new GameBattleMap(parent, gameSystem, terrain); }

	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	
private:
	GameBattleMap(kuto::Task* parent, const GameSystem& gameSystem, const std::string& terrain);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();
	
private:
	const GameSystem&	gameSystem_;
	kuto::Texture		texture_;
	int					animationCounter_;
};
