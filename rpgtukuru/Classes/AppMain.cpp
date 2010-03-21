/**
 * @file
 * @brief Application main
 * @author project.kuto
 */

#include <kuto/kuto_define.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_load_manager.h>
#include <kuto/kuto_section_manager.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_timer.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_file.h>
#include "AppMain.h"
#include "game/game.h"
#include "test/test_font.h"
#include "test/test_map.h"
#include "test/test_battle.h"
#include "test/test_title.h"
#include "test/test_chara.h"


class MainTask : public kuto::Task
{
public:
	MainTask() : kuto::Task(NULL) {}
	virtual ~MainTask() {}
};


AppMain::AppMain()
: mainTask_(NULL)
{
}

AppMain::~AppMain()
{
	if (mainTask_ != NULL) {
		mainTask_->release();
		mainTask_->deleteChildren();
		delete mainTask_;
		kuto::TouchPad::destroyInstance();
		kuto::RenderManager::destroyInstance();
		kuto::GraphicsDevice::destroyInstance();
		kuto::SectionManager::destroyInstance();
	}
}

void AppMain::initialize()
{
	kuto::randomize();
	mainTask_ = new MainTask();
#if defined(RPG2K_IS_IPHONE)
	kuto::GraphicsDevice::createInstance();
#endif
	kuto::LoadManager::createTask(mainTask_);
	kuto::RenderManager::createInstance();
	kuto::SectionManager::createInstance();
	kuto::TouchPad::createInstance();
	kuto::VirtualPad::createTask(mainTask_);
	
	kuto::SectionManager::instance()->initialize(mainTask_);
	std::vector<std::string> directories = kuto::Directory::getDirectories("/User/Media/Photos/RPG2000/");
	for (u32 i = 0; i < directories.size(); i++) {
		if (directories[i] == "RTP")		// RTPフォルダは無視
			continue;
		std::string gameDir = "/User/Media/Photos/RPG2000/" + directories[i];
		kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandleParam1<Game, Game::Option>(directories[i].c_str(), Game::Option(gameDir)));
	}
	//kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandleParam1<Game, Game::Option>("Game", Game::Option("/User/Media/Photos/RPG2000/Project2")));
	//kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandleParam1<Game, Game::Option>("Game2", Game::Option("/User/Media/Photos/RPG2000/yoake")));
	kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandle<TestMap>("Test Map"));
	kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandle<TestBattle>("Test Battle"));
	//kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandle<TestChara>("Test Chara"));
	kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandle<TestTitle>("Test Title"));
	kuto::SectionManager::instance()->addSectionHandle(new kuto::SectionHandle<TestFont>("Test Font"));

#if !defined(RPG2K_IS_IPHONE)
	// test for drawing images
	kuto::SectionManager::instance()->beginSection("Test Title");
#endif
}

void AppMain::update()
{
	performanceInfo_.start();
	kuto::TouchPad::instance()->update();
	
	mainTask_->updateChildren();
	performanceInfo_.endUpdate();
	mainTask_->deleteChildren();
	mainTask_->drawChildren();
	performanceInfo_.endDraw();
	
	kuto::RenderManager::instance()->render();
	performanceInfo_.endRender();
	
	//performanceInfo_.draw();		// これを有効にすればFPSとか出るよ
}


