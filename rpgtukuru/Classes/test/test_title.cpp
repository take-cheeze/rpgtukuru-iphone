/**
 * @file
 * @brief Title Test
 * @author project.kuto
 */

#include "test_title.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>
#include "CRpgUtil.h"


TestTitle::TestTitle(kuto::Task* parent)
: kuto::Task(parent)
, animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, drawTitle_(true), cursor_(0)
{
	kuto::VirtualPad::instance()->pauseDraw(false);
	const char* folder = "/User/Media/Photos/RPG2000/yoake";
	if (!rpgLdb_.Init(folder)) {
		kuto_printf("error: cannot open RPG_RT.ldb¥n");
		return;
	}
	
	std::string titleName = folder;
	titleName += "/Title/" + rpgLdb_.system.title;
	CRpgUtil::LoadImage(titleTex_, titleName, false);

	std::string gameoverName = folder;
	gameoverName += "/GameOver/" + rpgLdb_.system.gameover;
	CRpgUtil::LoadImage(gameoverTex_, gameoverName, false);

	std::string systemName = folder;
	systemName += "/System/" + rpgLdb_.system.system;
	CRpgUtil::LoadImage(systemTex_, systemName, true);
}

bool TestTitle::initialize()
{
	return isInitializedChildren();
}

void TestTitle::update()
{
#if 1
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	if (virtualPad->press(kuto::VirtualPad::KEY_START)) {
		// return debug menu
		this->release();
	}
	if (virtualPad->repeat(kuto::VirtualPad::KEY_UP)) {
		cursor_ = (cursor_ > 0)? cursor_ - 1 : 2;
	}
	if (virtualPad->repeat(kuto::VirtualPad::KEY_DOWN)) {
		cursor_ = (cursor_ < 2)? cursor_ + 1 : 0;
	}
	if (virtualPad->repeat(kuto::VirtualPad::KEY_A)) {
		drawTitle_ = !drawTitle_;
	}
#else
	kuto::TouchPad* touchPad = kuto::TouchPad::instance();
	if (touchPad->doubleClick(0)) {
		// return debug menu
		this->release();
	}
	if (touchPad->move(0) || touchPad->move(1)) {
		if (touchPad->on(0) && touchPad->on(1)) {
			// Multi touch -> scaling
			kuto::Vector2 offsetPrev = touchPad->prevPosition(0) - touchPad->prevPosition(1);
			kuto::Vector2 offset = touchPad->position(0) - touchPad->position(1);
			kuto::Vector2 center = (touchPad->position(0) + touchPad->position(1)) * 0.5f;
			kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
			kuto::Vector2 oldpoint = (center - screenOffset_) / size;
			
			float moveLength = offset.length() - offsetPrev.length();
			float moveRatio = moveLength * 0.01f * screenScale_.x;
			screenScale_.x = kuto::clamp(screenScale_.x + moveRatio, 0.01f, 10.f);
			screenScale_.y = kuto::clamp(screenScale_.y + moveRatio, 0.01f, 10.f);
			
			size.set(16.f * screenScale_.x, 16.f * screenScale_.y);
			kuto::Vector2 newcenter = oldpoint * size + screenOffset_;
			screenOffset_ -= newcenter - center;
		} else {
			// Single touch -> moving
			screenOffset_ += touchPad->moveOffset(0);
		}
	}
	if (touchPad->click(0)) {
		drawTitle_ = !drawTitle_;
	}
#endif
	animationCounter_++;
}

void TestTitle::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 0.f);
}

void TestTitle::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 windowSize(100.f, 60.f);
	const kuto::Vector2 windowPosition(160.f - windowSize.x * 0.5f, 180.f - windowSize.y * 0.5f);
	
	{
		kuto::Vector2 pos(screenOffset_);
		kuto::Texture& tex = drawTitle_? titleTex_ : gameoverTex_;
		kuto::Vector2 scale(tex.getOrgWidth(), tex.getOrgHeight());
		scale *= screenScale_;
		g->drawTexture(tex, pos, scale, color, true);
		
		scale = windowSize;
		pos = windowPosition;
		pos *= screenScale_;
		pos += screenOffset_;
		scale *= screenScale_;
		kuto::Vector2 texcoord0(0.f, 0.f);
		kuto::Vector2 texcoord1(32.f / systemTex_.getWidth(), 32.f / systemTex_.getHeight());
		g->drawTexture(systemTex_, pos, scale, color, texcoord0, texcoord1);
	
		texcoord0.set(32.f / systemTex_.getWidth(), 0.f);
		texcoord1.set(64.f / systemTex_.getWidth(), 32.f / systemTex_.getHeight());
		kuto::Vector2 borderSize(8.f, 8.f);
		kuto::Vector2 borderCoord(8.f / systemTex_.getWidth(), 8.f / systemTex_.getHeight());
		borderSize *= screenScale_;
		g->drawTexture9Grid(systemTex_, pos, scale, color, texcoord0, texcoord1, borderSize, borderCoord);
		
		scale.set(92.f, 16.f);
		pos.set(114.f, 154.f + cursor_ * 18.f);
		pos *= screenScale_;
		pos += screenOffset_;
		scale *= screenScale_;
		if ((animationCounter_ / 6) % 2 == 0) {
			texcoord0.set(64.f / systemTex_.getWidth(), 0.f);
			texcoord1.set(96.f / systemTex_.getWidth(), 32.f / systemTex_.getHeight());
		} else {
			texcoord0.set(96.f / systemTex_.getWidth(), 0.f);
			texcoord1.set(128.f / systemTex_.getWidth(), 32.f / systemTex_.getHeight());
		}
		borderSize *= screenScale_;
		g->drawTexture9Grid(systemTex_, pos, scale, color, texcoord0, texcoord1, borderSize, borderCoord);
	}
	{
		kuto::Vector2 scale = windowSize;
		kuto::Vector2 pos = windowPosition;
		pos *= screenScale_;
		pos += screenOffset_;
		scale *= screenScale_;
		const char* str = rpgLdb_.term.title.newGame.c_str();	// "New Game";
		float fontSize = 16.f * screenScale_.x;
		scale = kuto::Font::instance()->getTextSize(str, fontSize, kuto::Font::TYPE_NORMAL);
		pos.x = 160.f * screenScale_.x - scale.x * 0.5f;
		pos.x += screenOffset_.x;
		pos.y += 2.f * screenScale_.y;
		g->drawText(str, pos, color, fontSize, kuto::Font::TYPE_NORMAL);
		str = rpgLdb_.term.title.loadGame.c_str();	// "Continue";
		scale = kuto::Font::instance()->getTextSize(str, fontSize, kuto::Font::TYPE_NORMAL);
		pos.x = 160.f * screenScale_.x - scale.x * 0.5f;
		pos.x += screenOffset_.x;
		pos.y += fontSize + 2.f * screenScale_.y;
		g->drawText(str, pos, color, fontSize, kuto::Font::TYPE_NORMAL);
		str = rpgLdb_.term.title.endGame.c_str();	// "Shutdown";
		scale = kuto::Font::instance()->getTextSize(str, fontSize, kuto::Font::TYPE_NORMAL);
		pos.x = 160.f * screenScale_.x - scale.x * 0.5f;
		pos.x += screenOffset_.x;
		pos.y += fontSize + 2.f * screenScale_.y;
		g->drawText(str, pos, color, fontSize, kuto::Font::TYPE_NORMAL);
	}
}

