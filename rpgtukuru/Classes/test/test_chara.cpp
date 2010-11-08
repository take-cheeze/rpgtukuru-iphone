/**
 * @file
 * @brief Chara Test
 * @author project.kuto
 */

#include "test_chara.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>
#include "game_map.h"


TestChara::TestChara()
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 0.f)
, rpgLdb_("/User/Media/Photos/RPG2000/Project2")
, animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, drawFace_(false)
{
	kuto::VirtualPad::instance().pauseDraw(false);
/*
	gameChara_ = NULL;
	addChild(kuto::TaskCreatorParam3<GamePlayer, GameField&, int, GameCharaStatus&>::createTask(NULL, 1, charaStatus_));
	std::string walkTextureName = rpgLdb_.character()[1][3].to_string().toSystem();
	gameChara_->loadWalkTexture(walkTextureName, rpgLdb_.character()[1][4].to<int>());
	std::string faceTextureName = rpgLdb_.character()[1][15].to_string().toSystem();
	gameChara_->loadFaceTexture(faceTextureName, rpgLdb_.character()[1][16].to<int>());
 */
	// gameMap_ = addChild(GameMap::createTask(*this));
	// int mapIndex = 1;
	// gameMap_->load(mapIndex);
}

bool TestChara::initialize()
{
	return isInitializedChildren();
}

void TestChara::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	if (virtualPad.press(kuto::VirtualPad::KEY_START)) {
		// return debug menu
		this->release();
	}
	animationCounter_++;
}

void TestChara::render(kuto::Graphics2D& g) const
{
/*
	if (drawFace_)
		gameChara_->renderFace(g, kuto::Vector2(0.f, 192.f));
 */
}
