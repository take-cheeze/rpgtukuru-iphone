/**
 * @file
 * @brief Game Map
 * @author project.kuto
 */

#include "game_map.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>
#include "game_system.h"


GameMap::GameMap(kuto::Task* parent)
: kuto::Task(parent)
, mapId_(0), animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, enableScroll_(true), scrolled_(false), scrollRatio_(1.f)
, panoramaAutoScrollOffset_(0.f, 0.f)
{
}

bool GameMap::load(int mapIndex, rpg2k::model::DataBase& rpgLdb, const char* folder)
{
	mapId_ = mapIndex;
	rpgLdb_ = &rpgLdb;
/*
	if(!rpgLmu_.Init(mapIndex, *rpgLdb_, folder)){
		kuto_printf("error: cannot open Map%04d.lmu\n", mapIndex);
		return false;
	}
 */
	return true;
}

bool GameMap::initialize()
{
	return isInitializedChildren();
}

void GameMap::update()
{
	animationCounter_++;
	if (scrollRatio_ < 1.f) {
		scrollRatio_ = kuto::min(1.f, scrollRatio_ + scrollSpeed_);
		screenOffset_ = kuto::lerp(scrollBase_, scrollOffset_, scrollRatio_);
	}
	const kuto::Texture* panorama = NULL; // rpgLmu_.GetPanoramaTexture();
	if (panorama) {
		if (rpgLmu_[35].get<bool>()) panoramaAutoScrollOffset_.x += rpgLmu_[36].get<int>();
		if (rpgLmu_[37].get<bool>()) panoramaAutoScrollOffset_.y += rpgLmu_[38].get<int>();
	}
}

void GameMap::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 10.f);
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 8.f);
	renderCount_ = 0;
}

void GameMap::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
	if (renderCount_ == 0) {
		// Panorama
		const kuto::Texture* panorama = NULL; // rpgLmu_.GetPanoramaTexture();
		if (panorama) {
			kuto::Vector2 pos = screenOffset_;
			if (rpgLmu_[35].get<bool>()) pos.x += panoramaAutoScrollOffset_.x;
			if (rpgLmu_[37].get<bool>()) pos.y += panoramaAutoScrollOffset_.y;
			kuto::Vector2 scale(panorama->getOrgWidth(), panorama->getOrgHeight());
			scale *= screenScale_;
			g->drawTexture(*panorama, pos, scale, color, true);
		}
		// Map Chip
		drawLowerChips(false);
		drawUpperChips(false);
	} else {
		drawLowerChips(true);
		drawUpperChips(true);
#if 0
		const rpg2k::structure::Array1D& chipSet = rpgLdb_->chipSet()[rpgLmu_.GetChipSet()];
		for (int x = startX; x < rpgLmu_.getWidth(); x++) {
			float posx = x * size.x + screenOffset_.x;
			if (posx >= 320.f)
				break;
			for (int y = startY; y < rpgLmu_.getHeight(); y++) {
				float posy = y * size.y + screenOffset_.y;
				if (posy >= 240.f)
					break;
				kuto::Vector2 pos(posx, posy);
				int chipId = rpgLmu_.chipIDLw(x, y);
				int chipFlag = chipSet[4].getBinary()[chipId] & 0xFF;
				//int chipId = rpgLmu_.chipIDUp(x, y);
				//int chipFlag = chipSet[5].getBinary()[chipId] & 0xFF;
				char str[32];
				sprintf(str, "%02x", chipFlag);
				g->drawText(str, pos, color, 12.f, kuto::Font::TYPE_NORMAL);
			}
		}
#endif
	}
	renderCount_++;
}

void GameMap::drawLowerChips(bool high)
{
	// kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));
	if ( !rpgLdb_->chipSet().exists(rpgLmu_[1].get<int>()) ) return;

	// const rpg2k::structure::Array1D& chipSet = rpgLdb_->chipSet()[rpgLmu_[1].get<int>()];
	// MapUnit::TextureInfoSet infoSet;
	std::vector<DefferdCommand> defferedRenders;
	for (uint x = startX; x < rpgLmu_.getWidth(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= 320.f)
			break;
		for (uint y = startY; y < rpgLmu_.getHeight(); y++) {
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 240.f)
				break;
			kuto::Vector2 pos(posx, posy);
/*
			if (rpgLmu_.GetLowerChip(x, y, animationCounter_, infoSet)) {
				if (infoSet.size > 0) {
					int chipId = rpgLmu_.chipIDLw(x, y);
					if (chipId >= (int)chipSet[4].getBinary().size())
						continue;
					if (((chipSet[4].getBinary()[chipId] & DataBase::FLAG_CHARACTER_UP) != 0) == high) {
						if (infoSet.size == 1) {
							if (infoSet.info[0].texture == rpgLmu_.GetChipSetTexture()) {
								g->drawTexture(*infoSet.info[0].texture, pos, size, color,
									infoSet.info[0].texcoord[0], infoSet.info[0].texcoord[1]);
							} else {
								DefferdCommand com;
								com.info = infoSet.info[0];
								com.pos = pos;
								defferedRenders.push_back(com);
							}
						} else {
							for (int i = 0; i < infoSet.size; i++) {
								if (!infoSet.info[i].texture)
									continue;
								kuto::Vector2 offset((i % 2) * size.x * 0.5f, ((i % 4) / 2) * size.y * 0.5f);
								g->drawTexture(*infoSet.info[i].texture, pos + offset, size * 0.5f, color,
										infoSet.info[i].texcoord[0], infoSet.info[i].texcoord[1]);
							}
						}
					}
				}
			}
 */
		}
	}
	for (uint i = 0; i < defferedRenders.size(); i++) {
		// g->drawTexture(*defferedRenders[i].info.texture, defferedRenders[i].pos, size, color,
		// 			   defferedRenders[i].info.texcoord[0], defferedRenders[i].info.texcoord[1]);
	}
}

void GameMap::drawUpperChips(bool high)
{
	// kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));

	if ( rpgLdb_->chipSet().exists(rpgLmu_[1].get<int>()) ) return;

	// const rpg2k::structure::Array1D& chipSet = rpgLdb_->chipSet()[rpgLmu_[1].get<int>()];
	// MapUnit::TextureInfo info;
	for (uint x = startX; x < rpgLmu_.getWidth(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= 320.f)
			break;
		for (uint y = startY; y < rpgLmu_.getHeight(); y++) {
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 240.f)
				break;
			kuto::Vector2 pos(posx, posy);
/*
			if (rpgLmu_.GetUpperChip(x, y, info)) {
				uint chipId = rpgLmu_.chipIDUp(x, y);
				if (chipId < chipSet[5].getBinary().size()) {
					if (((chipSet[5].getBinary()[chipId] & DataBase::FLAG_CHARACTER_UP) != 0) == high)
						g->drawTexture(*info.texture, pos, size, color, info.texcoord[0], info.texcoord[1]);
				}
			}
 */
		}
	}
}

bool GameMap::isEnableMove(int nowX, int nowY, int nextX, int nextY) const
{
	if (nextX < 0 || nextY < 0)
		return false;
	if (uint(nextX) >= rpgLmu_.getWidth() || uint(nextY) >= rpgLmu_.getHeight())
		return false;
	const rpg2k::structure::Array1D& chipSet = rpgLdb_->chipSet()[rpgLmu_[1].get<int>()];
	int nowFlag = 0, nextFlag = 0;
	if (nowX > nextX) {
		 nowFlag = rpg2k::EventDir::LEFT;
		nextFlag = rpg2k::EventDir::RIGHT;
	} else if (nowX < nextX) {
		 nowFlag = rpg2k::EventDir::RIGHT;
		nextFlag = rpg2k::EventDir::LEFT;
	} else if (nowY > nextY) {
		 nowFlag = rpg2k::EventDir::UP;
		nextFlag = rpg2k::EventDir::DOWN;
	} else if (nowY < nextY) {
		 nowFlag = rpg2k::EventDir::DOWN;
		nextFlag = rpg2k::EventDir::UP;
	}

/*
	int upperChipNow = rpgLmu_.chipIDUp(nowX, nowY);
	int upperChipNext = rpgLmu_.chipIDUp(nextX, nextY);
	if (
		(upperChipNow != 0 && (chipSet[5].getBinary()[upperChipNow] & DataBase::FLAG_CHARACTER_UP) == 0) ||
		(upperChipNext != 0 && (chipSet[5].getBinary()[upperChipNext] & DataBase::FLAG_CHARACTER_UP) == 0)
	) {
		if (
			(upperChipNow != 0 && (chipSet[5].getBinary()[upperChipNow] & nowFlag) == 0) ||
			(upperChipNext != 0 && (chipSet[5].getBinary()[upperChipNext] & nextFlag) == 0)
		) {
			bool noReturn = false;
			if (nextY > 0 && (chipSet[5].getBinary()[upperChipNext] & DataBase::FLAG_WALL) != 0) {
				int upperChipUp = rpgLmu_.chipIDUp(nextX, nextY - 1);
				if (upperChipUp != upperChipNext)
					noReturn = true;
			}
			if ((chipSet[5].getBinary()[upperChipNow] & DataBase::FLAG_WALL) != 0) {
				if (upperChipNow != upperChipNext)
					noReturn = true;
			}
			return noReturn;
		} else {
			return true;
		}
	}
 */

	int lowerChipNow = rpgLmu_.chipIDLw(nowX, nowY);
	int lowerChipNext = rpgLmu_.chipIDLw(nextX, nextY);
	if ((chipSet[4].getBinary()[lowerChipNow] & nowFlag) == 0
	|| (chipSet[4].getBinary()[lowerChipNext] & nextFlag) == 0) {
		bool noReturn = false;
/*
		if (nextY > 0 && (chipSet[4].getBinary()[lowerChipNext] & DataBase::FLAG_WALL) != 0) {
			int lowerChipUp = rpgLmu_.chipIDLw(nextX, nextY - 1);
			if (lowerChipUp != lowerChipNext)
				noReturn = true;
		}
		if ((chipSet[4].getBinary()[lowerChipNow] & DataBase::FLAG_WALL) != 0) {
			if (lowerChipNow != lowerChipNext)
				noReturn = true;
		}
  */
		if (!noReturn)
			return false;
	}
	return true;
}

int GameMap::getChipFlag(int x, int y, bool upper) const
{
	const rpg2k::structure::Array1D& chipSet = rpgLdb_->chipSet()[rpgLmu_[1].get<int>()];
	int chipId = upper? rpgLmu_.chipIDUp(x, y) : rpgLmu_.chipIDLw(x, y);
	return upper? chipSet[5].getBinary()[chipId] : chipSet[4].getBinary()[chipId];
}

bool GameMap::isCounter(int x, int y) const
{
/*
	if (getChipFlag(x, y, false) & DataBase::FLAG_COUNTER)
		return true;
	if (getChipFlag(x, y, true) & DataBase::FLAG_COUNTER)
		return true;
 */
	return false;
}


void GameMap::setPlayerPosition(const kuto::Vector2& pos)
{
	float mapWidth = rpgLmu_.getWidth () * 16.f;
	float mapHeiht = rpgLmu_.getHeight() * 16.f;
	if (pos.x < (320.f) * 0.5f)
		screenOffsetBase_.x = 0.f;
	else if (pos.x > mapWidth - (320.f) * 0.5f)
		screenOffsetBase_.x = (320.f) - mapWidth;
	else
		screenOffsetBase_.x = (320.f) * 0.5f - pos.x;
	if (pos.y < (240.f) * 0.5f)
		screenOffsetBase_.y = 0.f;
	else if (pos.y > mapHeiht - (240.f) * 0.5f)
		screenOffsetBase_.y = (240.f) - mapHeiht;
	else
		screenOffsetBase_.y = (240.f) * 0.5f - pos.y;
	if (!enableScroll_ || isScrolling() || scrolled_)
		return;
	screenOffset_ = screenOffsetBase_;
}

int GameMap::getTerrainId(int x, int y) const
{
	// const rpg2k::structure::Array1D& chipSet = rpgLdb_->chipSet()[rpgLmu_[1].get<int>()];
	// uint lowerChip = rpgLmu_.chipIDLw(x, y);
	return 1; // lowerChip < chipSet.randforms.size()? chipSet.randforms[lowerChip] : 1;
}

void GameMap::scroll(int x, int y, float speed)
{
	scrollOffset_.x = -x * 16.f;
	scrollOffset_.y = -y * 16.f;
	scrollBase_ = screenOffset_;
	scrollOffset_ += screenOffset_;
	scrollSpeed_ = speed / (std::abs(scrollOffset_.x - scrollBase_.x) + std::abs(scrollOffset_.y - scrollBase_.y));
	scrollRatio_ = 0.f;
	scrolled_ = true;
}

void GameMap::scrollBack(float speed)
{
	scrollOffset_ = screenOffsetBase_;
	scrollBase_ = screenOffset_;
	scrollSpeed_ = speed / (std::abs(scrollOffset_.x - scrollBase_.x) + std::abs(scrollOffset_.y - scrollBase_.y));
	scrollRatio_ = 0.f;
	scrolled_ = false;
}

