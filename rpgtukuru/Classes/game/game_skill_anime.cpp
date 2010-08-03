/**
 * @file
 * @brief Skill animation
 * @autor project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_skill_anime.h"
#include "game_battle_chara.h"
// #include "CRpgUtil.h"


GameSkillAnime::GameSkillAnime(kuto::Task* parent, const rpg2k::model::Project& gameSystem, int animeId)
: kuto::Task(parent), gameSystem_(gameSystem), animeId_(animeId), counter_(0), played_(false), finished_(false)
, setPlayPosition_(false), deleteFinished_(false)
{
	const rpg2k::structure::Array1D& anime = gameSystem_.getLDB().battleAnime()[animeId_];
	std::string filename = gameSystem_.getGameDir();
	filename += "/Battle/" + anime[1].get_string().toSystem();
	bool res = CRpgUtil::LoadImage(texture_, filename, true); kuto_assert(res);
}

bool GameSkillAnime::initialize()
{
	return isInitializedChildren();
}

void GameSkillAnime::update()
{
	if (!played_ || finished_)
		return;
	const rpg2k::structure::Array1D& anime = gameSystem_.getLDB().battleAnime()[animeId_];
	counter_++;
	if (counter_ > (int)anime[12].getArray2D().rend().first()) {
		counter_ = anime[12].getArray2D().rend().first();
		finished_ = true;
		if (deleteFinished_)
			release();
	}
}

void GameSkillAnime::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 8.f);
}

void GameSkillAnime::render()
{
	if (!played_ || finished_ || counter_ == 0) return;

	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const rpg2k::structure::Array1D& anime = gameSystem_.getLDB().battleAnime()[animeId_];
	const rpg2k::structure::Array1D& animeFrame = anime[12].getArray2D()[counter_];

	const rpg2k::structure::Array2D& effectList = anime[6];
	for (rpg2k::structure::Array2D::Iterator it = effectList.begin(); it != effectList.end(); ++it) {
		if( it.second()[1].get<int>() != counter_ ) continue;

		kuto::Color color( (float)it.second()[4].get<int>(), (float)it.second()[5].get<int>(), (float)it.second()[6].get<int>(), (float)it.second()[7].get<int>() );
		color /= 31.f;

		switch (it.second()[3].get<int>() ) {
		case 0:
			for (uint iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				enemy->renderFlash(color);
			}
			break;
		case 1:
			kuto::Vector2 pos(0.f, 0.f);
			kuto::Vector2 size(320.f, 160.f);
			if (setPlayPosition_)
				size.y = 240.f;
			g->fillRectangle(pos, size, color);
			break;
		}
	}

	const rpg2k::structure::Array2D& cellList = animeFrame[1];
	for (rpg2k::structure::Array2D::Iterator it = cellList.begin(); it != cellList.end(); ++it) {
		const rpg2k::structure::Array1D& cell = it.second();
		if ( !cell.exists() || !cell[1].get<bool>() ) continue;

		const kuto::Vector2 cellSize = 96.f * cell[5].get<int>() * 0.01f;
		if (setPlayPosition_) {
			kuto::Vector2 pos;
			switch (anime[9].get<int>()) {
			case 0: // single
				pos.x = cell[3].get<int>() + playPosition_.x;
				pos.y = cell[4].get<int>() + playPosition_.y;
				break;
			case 1: // all
				pos.x = cell[3].get<int>() + 160.f;
				pos.y = cell[4].get<int>() + 80.f;
				break;
			}
			switch (anime[10].get<int>()) {
			case 0: // overhead
				pos.y -= 80.f;
				break;
			// case 1 break; // middle
			case 2: // foot
				pos.y += 80.f;
				break;
			}
			pos -= cellSize * 0.5f;
			kuto::Color color(cell[6].get<int>() * 0.01f, cell[7].get<int>() * 0.01f, cell[8].get<int>() * 0.01f, (100 - cell[10].get<int>()) * 0.01f);
			kuto::Vector2 texcoord1((cell[2].get<int>() % 5) * 96.f / texture_.getWidth(), (cell[2].get<int>() / 5) * 96.f / texture_.getHeight());
			kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
			g->drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
		} else {
			switch ( anime[9].get<int>() ) {
			case 1: { // all
				for (uint iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
					GameBattleEnemy* enemy = enemies_[iEnemy];
					kuto::Vector2 pos;
					pos.x = cell[3].get<int>() + enemy->getPosition().x;
					pos.y = cell[4].get<int>() + enemy->getPosition().y;
					switch ( anime[10].get<int>() ) {
					case 0: // overhead
						pos.y -= enemy->getScale().y * 0.5f;
						break;
					// case 1 break; // middle
					case 2: // foot
						pos.y += enemy->getScale().y * 0.5f;
						break;
					}
					pos -= cellSize * 0.5f;
					kuto::Color color(cell[6].get<int>() * 0.01f, cell[7].get<int>() * 0.01f, cell[8].get<int>() * 0.01f, (100 - cell[10].get<int>()) * 0.01f);
					kuto::Vector2 texcoord1((cell[2].get<int>() % 5) * 96.f / texture_.getWidth(), (cell[2].get<int>() / 5) * 96.f / texture_.getHeight());
					kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
					g->drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
				}
			} break;
			case 0: { // single
				kuto::Vector2 pos;
				pos.x = cell[3].get<int>() + 160.f;
				pos.y = cell[4].get<int>() + 80.f;
				switch ( anime[10].get<int>() ) {
				case 0: // overhead
					pos.y -= 80.f;
					break;
				// case 1 break; // middle
				case 2: // foot
					pos.y += 80.f;
					break;
				}
				pos -= cellSize * 0.5f;
				kuto::Color color(cell[6].get<int>() * 0.01f, cell[7].get<int>() * 0.01f, cell[8].get<int>() * 0.01f, (100 - cell[10].get<int>()) * 0.01f);
				kuto::Vector2 texcoord1((cell[2].get<int>() % 5) * 96.f / texture_.getWidth(), (cell[2].get<int>() / 5) * 96.f / texture_.getHeight());
				kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
				g->drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
			} break;
			}
		}
	}
}
