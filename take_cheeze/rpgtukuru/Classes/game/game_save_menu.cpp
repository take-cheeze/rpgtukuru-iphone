/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_stringstream.h>
#include "game_save_menu.h"
#include "game_system.h"
#include "game_field.h"


GameSaveMenu::GameSaveMenu(GameField* gameField)
: GameSystemMenuBase(gameField)
{
	menu_.create(this, gameField->getGameSystem(), true);
}

GameSaveMenu::~GameSaveMenu()
{
}

bool GameSaveMenu::initialize()
{
	if (menu_.initialize()) {
		start();
		return true;
	}
	return false;
}

void GameSaveMenu::update()
{
	switch (state_) {
	case kStateTop:
		if (menu_.selected()) {
			GameSaveData saveData;
			saveData.save(gameField_);

			std::ostringstream ss;
			initStringStream(ss);

			ss.str("");
			ss << kuto::Directory::getHomeDirectory() << "/Documents/"
				<< kuto::File::getFileName( gameField_->getGameSystem().getRpgLdb().getRootFolder() );
			if ( !kuto::Directory::exists( ss.str().c_str() ) ) kuto::Directory::create( ss.str().c_str() );

			ss << "Save" << std::setw(2) << menu_.selectIndex() + 1 << ".lsdi";
			FILE* fp = fopen(ss.str().c_str(), "w");
			if (fp) {
				fwrite(&saveData, sizeof(saveData), 1, fp);
				fclose(fp);
			}
			state_ = kStateNone;
		} else if (menu_.canceled()) {
			state_ = kStateNone;
		}
		break;
	}
}

void GameSaveMenu::start()
{
	freeze(false);
	state_ = kStateTop;
	menu_.start();
}

void GameSaveMenu::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 20.f);
}

void GameSaveMenu::render(kuto::Graphics2D& g)
{
}

