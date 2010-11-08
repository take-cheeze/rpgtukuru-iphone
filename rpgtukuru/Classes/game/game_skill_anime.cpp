/**
 * @file
 * @brief Skill animation
 * @autor project.kuto
 */

#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_render_manager.h>

#include "game_battle_chara.h"
#include "game_skill_anime.h"

#include <rpg2k/Project.hpp>


GameSkillAnime::GameSkillAnime(const rpg2k::model::Project& gameSystem, int animeId)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 8.f), project_(gameSystem)
, animeId_(animeId), counter_(0), played_(false), finished_(false)
, setPlayPosition_(false), deleteFinished_(false)
{
	const rpg2k::structure::Array1D& anime = project_.getLDB().battleAnime()[animeId_];
	bool res = RPG2kUtil::LoadImage(texture_, std::string( project_.gameDir() ).append("/Battle/").append( anime[2].to_string().toSystem() ), true); kuto_assert(res);
}

bool GameSkillAnime::initialize()
{
	return isInitializedChildren();
}

void GameSkillAnime::update()
{
	if (!played_ || finished_)
		return;
	const rpg2k::structure::Array1D& anime = project_.getLDB().battleAnime()[animeId_];
	counter_++;
	if (counter_ > (int)anime[12].toArray2D().rend()->first) {
		counter_ = anime[12].toArray2D().rend()->first;
		finished_ = true;
		if (deleteFinished_)
			release();
	}
}

void GameSkillAnime::render(kuto::Graphics2D& g) const
{
	if (!played_ || finished_ || counter_ == 0) return;

	const rpg2k::structure::Array1D& anime = project_.getLDB().battleAnime()[animeId_];
	const rpg2k::structure::Array1D& animeFrame = anime[12].toArray2D()[counter_];

	const rpg2k::structure::Array2D& effectList = anime[6];
	for (rpg2k::structure::Array2D::ConstIterator it = effectList.begin(); it != effectList.end(); ++it) {
		if( (*it->second)[1].to<int>() != counter_ ) continue;

		kuto::Color color( (float)(*it->second)[4].to<int>(), (float)(*it->second)[5].to<int>(), (float)(*it->second)[6].to<int>(), (float)(*it->second)[7].to<int>() );
		color /= 31.f;

		switch ((*it->second)[3].to<int>() ) {
		case 0:
			for (uint iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				enemy->renderFlash(g, color);
			}
			break;
		case 1:
			kuto::Vector2 pos(0.f, 0.f);
			kuto::Vector2 size(320.f, 160.f);
			if (setPlayPosition_)
				size.y = 240.f;
			g.fillRectangle(pos, size, color);
			break;
		}
	}

	const rpg2k::structure::Array2D& cellList = animeFrame[1];
	for (rpg2k::structure::Array2D::ConstIterator it = cellList.begin(); it != cellList.end(); ++it) {
		const rpg2k::structure::Array1D& cell = *it->second;
		if ( !cell.exists() || !cell[1].to<bool>() ) continue;

		const kuto::Vector2 cellSize = 96.f * cell[5].to<int>() * 0.01f;
		if (setPlayPosition_) {
			kuto::Vector2 pos;
			switch (anime[9].to<int>()) {
			case 0: // single
				pos.x = cell[3].to<int>() + playPosition_.x;
				pos.y = cell[4].to<int>() + playPosition_.y;
				break;
			case 1: // all
				pos.x = cell[3].to<int>() + 160.f;
				pos.y = cell[4].to<int>() + 80.f;
				break;
			}
			switch (anime[10].to<int>()) {
			case 0: // overhead
				pos.y -= 80.f;
				break;
			// case 1 break; // middle
			case 2: // foot
				pos.y += 80.f;
				break;
			}
			pos -= cellSize * 0.5f;
			kuto::Color color(cell[6].to<int>() * 0.01f, cell[7].to<int>() * 0.01f, cell[8].to<int>() * 0.01f, (100 - cell[10].to<int>()) * 0.01f);
			kuto::Vector2 texcoord1((cell[2].to<int>() % 5) * 96.f / texture_.width(), (cell[2].to<int>() / 5) * 96.f / texture_.height());
			kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.width(), texcoord1.y + 96.f / texture_.height());
			g.drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
		} else {
			switch ( anime[9].to<int>() ) {
			case 1: { // all
				for (uint iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
					GameBattleEnemy* enemy = enemies_[iEnemy];
					kuto::Vector2 pos;
					pos.x = cell[3].to<int>() + enemy->position().x;
					pos.y = cell[4].to<int>() + enemy->position().y;
					switch ( anime[10].to<int>() ) {
					case 0: // overhead
						pos.y -= enemy->scale().y * 0.5f;
						break;
					// case 1 break; // middle
					case 2: // foot
						pos.y += enemy->scale().y * 0.5f;
						break;
					}
					pos -= cellSize * 0.5f;
					kuto::Color color(cell[6].to<int>() * 0.01f, cell[7].to<int>() * 0.01f, cell[8].to<int>() * 0.01f, (100 - cell[10].to<int>()) * 0.01f);
					kuto::Vector2 texcoord1((cell[2].to<int>() % 5) * 96.f / texture_.width(), (cell[2].to<int>() / 5) * 96.f / texture_.height());
					kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.width(), texcoord1.y + 96.f / texture_.height());
					g.drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
				}
			} break;
			case 0: { // single
				kuto::Vector2 pos;
				pos.x = cell[3].to<int>() + 160.f;
				pos.y = cell[4].to<int>() + 80.f;
				switch ( anime[10].to<int>() ) {
				case 0: // overhead
					pos.y -= 80.f;
					break;
				// case 1 break; // middle
				case 2: // foot
					pos.y += 80.f;
					break;
				}
				pos -= cellSize * 0.5f;
				kuto::Color color(cell[6].to<int>() * 0.01f, cell[7].to<int>() * 0.01f, cell[8].to<int>() * 0.01f, (100 - cell[10].to<int>()) * 0.01f);
				kuto::Vector2 texcoord1((cell[2].to<int>() % 5) * 96.f / texture_.width(), (cell[2].to<int>() / 5) * 96.f / texture_.height());
				kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.width(), texcoord1.y + 96.f / texture_.height());
				g.drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
			} break;
			}
		}
	}
}
