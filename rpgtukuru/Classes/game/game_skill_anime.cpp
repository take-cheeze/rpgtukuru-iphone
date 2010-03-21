/**
 * @file
 * @brief Skill animation
 * @autor project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_skill_anime.h"
#include "game_battle_chara.h"
#include "CRpgUtil.h"


GameSkillAnime::GameSkillAnime(kuto::Task* parent, const GameSystem& gameSystem, int animeId)
: kuto::Task(parent), gameSystem_(gameSystem), animeId_(animeId), counter_(0), played_(false), finished_(false)
{
	const CRpgLdb::BattleAnime& anime = gameSystem_.getRpgLdb().saBattleAnime[animeId_];
	std::string filename = gameSystem_.getRootFolder();
	filename += "/Battle/" + anime.filename;
	CRpgUtil::LoadImage(texture_, filename, true);
	
}

bool GameSkillAnime::initialize()
{
	return isInitializedChildren();
}

void GameSkillAnime::update()
{
	if (!played_ || finished_)
		return;
	const CRpgLdb::BattleAnime& anime = gameSystem_.getRpgLdb().saBattleAnime[animeId_];
	counter_++;
	if (counter_ >= (int)anime.animeFrames.size()) {
		counter_ = anime.animeFrames.size() - 1;
		finished_ = true;
	}
}

void GameSkillAnime::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 8.f);
}

void GameSkillAnime::render()
{
	if (!played_ || finished_ || counter_ == 0)
		return;
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const CRpgLdb::BattleAnime& anime = gameSystem_.getRpgLdb().saBattleAnime[animeId_];
	const CRpgLdb::AnimeFrame& animeFrame = anime.animeFrames[counter_];
	std::vector<const CRpgLdb::TimingOfFlash*> flashes;
	for (u32 i = 0; i < anime.timingOfFlashes.size(); i++) {
		const CRpgLdb::TimingOfFlash& flash = anime.timingOfFlashes[i];
		if (flash.frame == counter_) {
			flashes.push_back(&flash);
		}
	}
	for (u32 i = 0; i < flashes.size(); i++) {
		const CRpgLdb::TimingOfFlash& flash = *flashes[i];
		kuto::Color color((float)flash.colorR / 31.f, (float)flash.colorG / 31.f, (float)flash.colorB / 31.f, (float)flash.strength / 31.f);
		if (flash.scope == CRpgLdb::kFlashScopeTarget) {
			for (u32 iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				enemy->renderFlash(color);
			}
		} else if (flash.scope == CRpgLdb::kFlashScopeScreen) {
			kuto::Vector2 pos(0.f, 0.f);
			kuto::Vector2 size(320.f, 160.f);
			g->fillRectangle(pos, size, color);
		}
	}
	
	for (u32 i = 0; i < animeFrame.cells.GetSize(); i++) {
		const CRpgLdb::AnimeCell& cell = animeFrame.cells[i];
		if (!cell.visible)
			continue;
		const kuto::Vector2 cellSize(96.f * cell.scale * 0.01f, 96.f * cell.scale * 0.01f);
		if (anime.scope == CRpgLdb::kAnimeScopeSingle) {
			for (u32 iEnemy = 0; iEnemy < enemies_.size(); iEnemy++) {
				GameBattleEnemy* enemy = enemies_[iEnemy];
				kuto::Vector2 pos;
				pos.x = cell.x + enemy->getPosition().x;
				pos.y = cell.y + enemy->getPosition().y;
				switch (anime.baseLine) {
				case CRpgLdb::kAnimeBaseLineTop:
					pos.y -= enemy->getScale().y * 0.5f;
					break;
				case CRpgLdb::kAnimeBaseLineBottom:
					pos.y += enemy->getScale().y * 0.5f;
					break;
				}
				pos -= cellSize * 0.5f;
				kuto::Color color(cell.colorR * 0.01f, cell.colorG * 0.01f, cell.colorB * 0.01f, (100 - cell.colorA) * 0.01f);
				kuto::Vector2 texcoord1((cell.pattern % 5) * 96.f / texture_.getWidth(), (cell.pattern / 5) * 96.f / texture_.getHeight());
				kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
				g->drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
			}
		} else {
			kuto::Vector2 pos;
			pos.x = cell.x + 160.f;
			pos.y = cell.y + 80.f;
			switch (anime.baseLine) {
			case CRpgLdb::kAnimeBaseLineTop:
				pos.y -= 80.f;
				break;
			case CRpgLdb::kAnimeBaseLineBottom:
				pos.y += 80.f;
				break;
			}
			pos -= cellSize * 0.5f;
			kuto::Color color(cell.colorR * 0.01f, cell.colorG * 0.01f, cell.colorB * 0.01f, (100 - cell.colorA) * 0.01f);
			kuto::Vector2 texcoord1((cell.pattern % 5) * 96.f / texture_.getWidth(), (cell.pattern / 5) * 96.f / texture_.getHeight());
			kuto::Vector2 texcoord2(texcoord1.x + 96.f / texture_.getWidth(), texcoord1.y + 96.f / texture_.getHeight());
			g->drawTexture(texture_, pos, cellSize, color, texcoord1, texcoord2);
		}
	}
}

