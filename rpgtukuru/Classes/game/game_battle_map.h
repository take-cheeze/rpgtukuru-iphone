/**
 * @file
 * @brief Game Battle Map
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include "game_system.h"


class GameBattleMap : public kuto::IRender2D, public kuto::TaskCreatorParam2<GameBattleMap, const rpg2k::model::Project&, const std::string&>
{
	friend class kuto::TaskCreatorParam2<GameBattleMap, const rpg2k::model::Project&, const std::string&>;
public:
	virtual void render(kuto::Graphics2D* g) const;

private:
	GameBattleMap(const rpg2k::model::Project& gameSystem, const std::string& terrain);

	virtual bool initialize();
	virtual void update();

private:
	const rpg2k::model::Project&	gameSystem_;
	kuto::Texture		texture_;
	int					animationCounter_;
};
