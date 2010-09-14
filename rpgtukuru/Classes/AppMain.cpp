/**
 * @file
 * @brief Application main
 * @author project.kuto
 */

#include <kuto/kuto_debug_menu.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_performance_info.h>
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_section_manager.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_virtual_pad.h>

#include "AppMain.h"
#include "game/game.h"

#include "test/test_font.h"
#include "test/test_map.h"
#include "test/test_battle.h"
#include "test/test_title.h"
#include "test/test_chara.h"


AppMain::AppMain()
{
	virtualPad_ = addChild( std::auto_ptr<kuto::VirtualPad>( new kuto::VirtualPad() ) );
	sectionManager_ = addChild( std::auto_ptr<kuto::SectionManager>( new kuto::SectionManager() ) );
	performanceInfo_ = addChild( std::auto_ptr<kuto::PerformanceInfo>( new kuto::PerformanceInfo() ) );

	#if !RPG2K_DEBUG
		performanceInfo_->pauseDraw(true); // これを有効にすればFPSとか出るよ
	#endif
}

AppMain::~AppMain()
{
	this->release();
	this->deleteReleasedChildren();
}

bool AppMain::initialize()
{
	kuto::randomize();
	typedef std::auto_ptr<kuto::SectionHandleBase> SectionPointer;

	const char* rpgRootDir = GAME_FIND_PATH;
	std::vector<std::string> directories = kuto::Directory::getDirectories(rpgRootDir);
	for (std::vector<std::string>::const_iterator i = directories.begin(); i < directories.end(); i++) {
		if (
			!kuto::File::exists( (*i + "/RPG_RT.ldb").c_str() ) || // needs LcfDataBase
			!kuto::File::exists( (*i + "/RPG_RT.lmt").c_str() ) || // needs LcfMapTree
			(*i == "RTP")		// RTPフォルダは無視
		) continue;
		std::string gameDir = rpgRootDir + *i;
	 	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandleParam1<Game, GameConfig>(i->c_str(), GameConfig(gameDir))) );
	}
	//sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandleParam1<Game, GameConfig>("Game", Game::Option("/User/Media/Photos/RPG2000/Project2"))) );
	//sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandleParam1<Game, GameConfig>("Game2", Game::Option("/User/Media/Photos/RPG2000/yoake"))) );
	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandle<TestMap>("Test Map")) );
	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandle<TestBattle>("Test Battle")) );
	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandle<TestChara>("Test Chara")) );
	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandle<TestTitle>("Test Title")) );
	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandle<TestFont>("Test Font")) );

#if !RPG2K_IS_IPHONE
	sectionManager_->addSectionHandle( SectionPointer(new kuto::SectionHandle<kuto::DebugMenu>("Debug Menu")) );
	sectionManager_->beginSection("Debug Menu");
#endif

	return true;
}

void AppMain::update()
{
	performanceInfo_->start();
		performanceInfo_->startUpdate();
			this->updateChildren();
		performanceInfo_->endUpdate();
		this->deleteReleasedChildren();
		performanceInfo_->startDraw();
			this->drawChildren();
		performanceInfo_->endDraw();

		if( !performanceInfo_->clearDelayFlag() ) {
			performanceInfo_->startRender();
				kuto::RenderManager::instance()->render();
			performanceInfo_->endRender();
		}
	performanceInfo_->end();

	performanceInfo_->calculate();

#if !RPG2K_IS_IPHONE
	if (!sectionManager_->getCurrentTask()) {
		sectionManager_->beginSection("Debug Menu");
	}
#endif
}
