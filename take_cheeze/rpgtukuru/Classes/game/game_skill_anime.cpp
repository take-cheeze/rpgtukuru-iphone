/**
 * @file
 * @brief Skill animation
 * @autor project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_skill_anime.h"
#include "game_battle_chara.h"
#include "game_image.h"

using namespace rpg2kLib::model;
using namespace rpg2kLib::structure;


GameSkillAnime::GameSkillAnime(kuto::Task* parent, const GameSystem& gameSystem, int animeId)
: kuto::Task(parent), gameSystem_(gameSystem), animeId_(animeId), counter_(0), played_(false), finished_(false)
{
	const Array1D& anime = gameSystem_.getRpgLdb().getBattleAnime()[animeId_];
	std::string filename = gameSystem_.getRootFolder();
	filename += "/Battle/" + anime[2].get_string();
	GameImage::LoadImage(texture_, filename, true);
}

bool GameSkillAnime::initialize()
{
	return isInitializedChildren();
}

void GameSkillAnime::update()
{
	if (!played_ || finished_)
		return;
	const Array1D& anime = gameSystem_.getRpgLdb().getBattleAnime()[animeId_];

	if ( counter_ > (int)( --anime[12].getArray2D().end() ).first()+1 ) finished_ = true;
	else counter_++;
}

void GameSkillAnime::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 8.f);
}

void GameSkillAnime::render(kuto::Graphics2D& g)
{
	if (!played_ || finished_ || counter_ == 0)
		return;

	const Array1D& anime = gameSystem_.getRpgLdb().getBattleAnime()[animeId_];
	const Array1D& animeFrame = anime[12].getArray2D()[counter_];
/*
	std::vector<const DataBase::TimingOfFlash*> flashes;
	for (u32 i = 0; i < anime.timingOfFlashes.size(); i++) {
		const DataBase::TimingOfFlash& flash = anime.timingOfFlashes[i];
		if (flash.frame == counter_) {
			flashes.push_back(&flash);
		}
	}
	for (u32 i = 0; i < flashes.size(); i++) {
		const DataBase::TimingOfFlash& flash = *flashes[i];
		kuto::Color color((float)flash.colorR / 31.f, (float)flash.colorG / 31.f, (float)flash.colorB / 31.f, (float)flash.strength / 31.f);
		if (flash.scope == DataBase::kFlashScopeTarget) {
			for (u32 iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				enemy->renderFlash(g, color);
			}
		} else if (flash.scope == DataBase::kFlashScopeScreen) {
			kuto::Vector2 pos(0.f, 0.f);
			kuto::Vector2 size(320.f, 160.f);
			g.fillRectangle(pos, size, color);
		}
	}
 */
	const Array2D& flashList = anime[6];
	for (Array2D::Iterator it = flashList.begin(); it != flashList.end(); ++it) {
		const Array1D& flash = it.second();

		static const float BASE_VAL = 31.f;
		kuto::Color color(
			(float)flash[4].get_int() / BASE_VAL,
			(float)flash[5].get_int() / BASE_VAL,
			(float)flash[6].get_int() / BASE_VAL,
			(float)flash[7].get_int() / BASE_VAL
		);
		switch( flash[3].get_int() ) {
		case DataBase::kFlashScopeTarget: {
			for (u32 iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				enemy->renderFlash(g, color);
			}
		} break;
		case DataBase::kFlashScopeScreen: {
			kuto::Vector2 pos(0.f, 0.f);
			kuto::Vector2 size(320.f, 160.f);
			g.fillRectangle(pos, size, color);
		} break;
		default: break;
		}
	}

	const Array2D& cellList = animeFrame[1];
	for (Array2D::Iterator it = cellList.begin(); it != cellList.end(); ++it) {
		if ( !it.second()[1].get_bool() ) continue;

		const Array1D& cell = it.second();
		const kuto::Vector2 cellSize(96.f * cell[5].get_int() * 0.01f, 96.f * cell[5].get_int() * 0.01f);

		if (anime[9].get_int() == DataBase::kAnimeScopeSingle) {
			for (u32 iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				kuto::Vector2 pos;
				pos.x = cell[3].get_int() + enemy->getPosition().x;
				pos.y = cell[4].get_int() + enemy->getPosition().y;
				switch ( anime[10].get_int() ) {
				case DataBase::kAnimeBaseLineTop:
					pos.y -= enemy->getScale().y * 0.5f;
					break;
				case DataBase::kAnimeBaseLineBottom:
					pos.y += enemy->getScale().y * 0.5f;
					break;
				}
				pos -= cellSize * 0.5f;
				kuto::Color color(cell[6].get_int() * 0.01f, cell[7].get_int() * 0.01f, cell[8].get_int() * 0.01f, (100 - cell[9].get_int()) * 0.01f);
				kuto::Vector2 texcoord1((cell[2].get_int() % 5) * 96.f / texture_.getWidth(), (cell[2].get_int() / 5) * 96.f / texture_.getHeight());
				kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
				g.drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
			}
		} else {
			kuto::Vector2 pos;
			pos.x = cell[3].get_int() + 160.f;
			pos.y = cell[4].get_int() + 80.f;
			switch ( anime[10].get_int() ) {
			case DataBase::kAnimeBaseLineTop:
				pos.y -= 80.f;
				break;
			case DataBase::kAnimeBaseLineBottom:
				pos.y += 80.f;
				break;
			}
			pos -= cellSize * 0.5f;
			kuto::Color color(cell[6].get_int() * 0.01f, cell[7].get_int() * 0.01f, cell[8].get_int() * 0.01f, (100 - cell[9].get_int()) * 0.01f);
			kuto::Vector2 texcoord1((cell[2].get_int() % 5) * 96.f / texture_.getWidth(), (cell[2].get_int() / 5) * 96.f / texture_.getHeight());
			kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
			g.drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
		}
	}
}

