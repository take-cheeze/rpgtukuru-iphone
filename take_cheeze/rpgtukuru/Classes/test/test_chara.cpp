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
// #include "CRpgUtil.h"
#include "game_player.h"
#include "game_map.h"

static const char* folder = "/User/Media/Photos/RPG2000/yoake";

TestChara::TestChara(kuto::Task* parent)
: kuto::Task(parent)
, rpgLdb_(folder)
, animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, drawFace_(false)
{
	kuto::VirtualPad::instance()->pauseDraw(false);
	
	//gameChara_ = GamePlayer::createTask(this, NULL);
	std::string walkTextureName = folder;
	walkTextureName += "/CharSet/";
	walkTextureName += rpgLdb_.getCharacter()[1][3].get_string();
	gameChara_->loadWalkTexture(walkTextureName.c_str(), rpgLdb_.getCharacter()[1][4].get_int());
	std::string faceTextureName = folder;
	faceTextureName += "/FaceSet/";
	faceTextureName += rpgLdb_.getCharacter()[1][15].get_string();
	gameChara_->loadFaceTexture(faceTextureName.c_str(), rpgLdb_.getCharacter()[1][16].get_int());
	
	gameMap_ = GameMap::createTask(this);
	int mapIndex = 1;
	gameMap_->load(mapIndex, rpgLdb_, folder);
}

bool TestChara::initialize()
{
	return isInitializedChildren();
}

void TestChara::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	if (virtualPad->press(kuto::VirtualPad::KEY_START)) {
		// return debug menu
		this->release();
	}
	animationCounter_++;
}

void TestChara::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 0.f);
}

void TestChara::render()
{
	//kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	if (drawFace_)
		gameChara_->renderFace(kuto::Vector2(0.f, 192.f));
}


