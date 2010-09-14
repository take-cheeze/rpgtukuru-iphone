/**
 * @file
 * @brief Map Draw Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <rpg2k/DataBase.hpp>
#include <rpg2k/MapUnit.hpp>
#include <rpg2k/MapTree.hpp>


class TestMap : public kuto::IRender2D, public kuto::TaskCreator<TestMap>
{
	friend class kuto::TaskCreator<TestMap>;
private:
	TestMap();

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;

private:
	rpg2k::model::DataBase				rpgLdb_;
	rpg2k::model::MapUnit				rpgLmu_;
	rpg2k::model::MapTree				rpgLmt_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
};

