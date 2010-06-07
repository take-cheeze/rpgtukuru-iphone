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

bool GameMap::load(int mapIndex, CRpgLdb& rpgLdb, const char* folder)
{
	mapId_ = mapIndex;
	rpgLdb_ = &rpgLdb;
	if(!rpgLmu_.Init(mapIndex, *rpgLdb_, folder)){
		printf("error: cannot open Map%04d.lmu\n", mapIndex);
		return false;
	}
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
	const kuto::Texture* panorama = rpgLmu_.GetPanoramaTexture();
	if (panorama) {
		if (rpgLmu_.GetPanoramaInfo().scrollHorizontal)
			panoramaAutoScrollOffset_.x += rpgLmu_.GetPanoramaInfo().scrollSpeedHorizontal;
		if (rpgLmu_.GetPanoramaInfo().scrollVertical)
			panoramaAutoScrollOffset_.y += rpgLmu_.GetPanoramaInfo().scrollSpeedVertical;
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
		const kuto::Texture* panorama = rpgLmu_.GetPanoramaTexture();
		if (panorama) {
			kuto::Vector2 pos = screenOffset_;
			if (rpgLmu_.GetPanoramaInfo().scrollHorizontal)
				pos.x += panoramaAutoScrollOffset_.x;
			if (rpgLmu_.GetPanoramaInfo().scrollVertical)
				pos.y += panoramaAutoScrollOffset_.y;			
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
		const CRpgLdb::ChipSet& chipSet = rpgLdb_->saChipSet[rpgLmu_.GetChipSet()];
		for (int x = startX; x < rpgLmu_.GetWidth(); x++) {
			float posx = x * size.x + screenOffset_.x;
			if (posx >= 320.f)
				break;
			for (int y = startY; y < rpgLmu_.GetHeight(); y++) {
				float posy = y * size.y + screenOffset_.y;
				if (posy >= 240.f)
					break;
				kuto::Vector2 pos(posx, posy);
				int chipId = rpgLmu_.getLowerChipId(x, y);
				int chipFlag = chipSet.blockLower[chipId] & 0xFF;
				//int chipId = rpgLmu_.getUpperChipId(x, y);
				//int chipFlag = chipSet.blockUpper[chipId] & 0xFF;
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
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));
	if (rpgLmu_.GetChipSet() >= (int)rpgLdb_->saChipSet.GetSize())
		return;
	const CRpgLdb::ChipSet& chipSet = rpgLdb_->saChipSet[rpgLmu_.GetChipSet()];
	CRpgLmu::TextureInfoSet infoSet;
	std::vector<DefferdCommand> defferedRenders;
	for (int x = startX; x < rpgLmu_.GetWidth(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= 320.f)
			break;
		for (int y = startY; y < rpgLmu_.GetHeight(); y++) {
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 240.f)
				break;
			kuto::Vector2 pos(posx, posy);
			if (rpgLmu_.GetLowerChip(x, y, animationCounter_, infoSet)) {
				if (infoSet.size > 0) {
					int chipId = rpgLmu_.getLowerChipId(x, y);
					if (chipId >= (int)chipSet.blockLower.size())
						continue;
					if (((chipSet.blockLower[chipId] & CRpgLdb::FLAG_CHARACTER_UP) != 0) == high) {
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
		}
	}
	for (u32 i = 0; i < defferedRenders.size(); i++) {
		g->drawTexture(*defferedRenders[i].info.texture, defferedRenders[i].pos, size, color,
					   defferedRenders[i].info.texcoord[0], defferedRenders[i].info.texcoord[1]);
	}
}

void GameMap::drawUpperChips(bool high)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));
	if (rpgLmu_.GetChipSet() >= (int)rpgLdb_->saChipSet.GetSize())
		return;
	const CRpgLdb::ChipSet& chipSet = rpgLdb_->saChipSet[rpgLmu_.GetChipSet()];
	CRpgLmu::TextureInfo info;
	for (int x = startX; x < rpgLmu_.GetWidth(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= 320.f)
			break;
		for (int y = startY; y < rpgLmu_.GetHeight(); y++) {
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 240.f)
				break;
			kuto::Vector2 pos(posx, posy);
			if (rpgLmu_.GetUpperChip(x, y, info)) {
				uint chipId = rpgLmu_.getUpperChipId(x, y);
				if (chipId < chipSet.blockUpper.size()) {
					if (((chipSet.blockUpper[chipId] & CRpgLdb::FLAG_CHARACTER_UP) != 0) == high)
						g->drawTexture(*info.texture, pos, size, color, info.texcoord[0], info.texcoord[1]);
				}
			}
		}
	}
}

bool GameMap::isEnableMove(int nowX, int nowY, int nextX, int nextY) const
{
	if (nextX < 0 || nextY < 0)
		return false;
	if (nextX >= rpgLmu_.GetWidth() || nextY >= rpgLmu_.GetHeight())
		return false;
	const CRpgLdb::ChipSet& chipSet = rpgLdb_->saChipSet[rpgLmu_.GetChipSet()];
	int nowFlag = 0, nextFlag = 0;
	if (nowX > nextX) {
		nowFlag = CRpgLdb::FLAG_MOVE_LEFT;
		nextFlag = CRpgLdb::FLAG_MOVE_RIGHT;
	} else if (nowX < nextX) {
		nowFlag = CRpgLdb::FLAG_MOVE_RIGHT;
		nextFlag = CRpgLdb::FLAG_MOVE_LEFT;
	} else if (nowY > nextY) {
		nowFlag = CRpgLdb::FLAG_MOVE_UP;
		nextFlag = CRpgLdb::FLAG_MOVE_DOWN;
	} else if (nowY < nextY) {
		nowFlag = CRpgLdb::FLAG_MOVE_DOWN;
		nextFlag = CRpgLdb::FLAG_MOVE_UP;
	}
	
	int upperChipNow = rpgLmu_.getUpperChipId(nowX, nowY);
	int upperChipNext = rpgLmu_.getUpperChipId(nextX, nextY);
	if (
		(upperChipNow != 0 && (chipSet.blockUpper[upperChipNow] & CRpgLdb::FLAG_CHARACTER_UP) == 0) ||
		(upperChipNext != 0 && (chipSet.blockUpper[upperChipNext] & CRpgLdb::FLAG_CHARACTER_UP) == 0)
	) {
		if (
			(upperChipNow != 0 && (chipSet.blockUpper[upperChipNow] & nowFlag) == 0) ||
			(upperChipNext != 0 && (chipSet.blockUpper[upperChipNext] & nextFlag) == 0)
		) {
			bool noReturn = false;
			if (nextY > 0 && (chipSet.blockUpper[upperChipNext] & CRpgLdb::FLAG_WALL) != 0) {
				int upperChipUp = rpgLmu_.getUpperChipId(nextX, nextY - 1);
				if (upperChipUp != upperChipNext)
					noReturn = true;
			}
			if ((chipSet.blockUpper[upperChipNow] & CRpgLdb::FLAG_WALL) != 0) {
				if (upperChipNow != upperChipNext)
					noReturn = true;
			}
			return noReturn;
		} else {
			return true;	
		}
	}
	
	int lowerChipNow = rpgLmu_.getLowerChipId(nowX, nowY);
	int lowerChipNext = rpgLmu_.getLowerChipId(nextX, nextY);
	if ((chipSet.blockLower[lowerChipNow] & nowFlag) == 0
	|| (chipSet.blockLower[lowerChipNext] & nextFlag) == 0) {
		bool noReturn = false;
		if (nextY > 0 && (chipSet.blockLower[lowerChipNext] & CRpgLdb::FLAG_WALL) != 0) {
			int lowerChipUp = rpgLmu_.getLowerChipId(nextX, nextY - 1);
			if (lowerChipUp != lowerChipNext)
				noReturn = true;
		}
		if ((chipSet.blockLower[lowerChipNow] & CRpgLdb::FLAG_WALL) != 0) {
			if (lowerChipNow != lowerChipNext)
				noReturn = true;
		}
		if (!noReturn)
			return false;
	}
	return true;
}

int GameMap::getChipFlag(int x, int y, bool upper) const
{
	const CRpgLdb::ChipSet& chipSet = rpgLdb_->saChipSet[rpgLmu_.GetChipSet()];
	int chipId = upper? rpgLmu_.getUpperChipId(x, y) : rpgLmu_.getLowerChipId(x, y);
	return upper? chipSet.blockUpper[chipId] : chipSet.blockLower[chipId];
}

bool GameMap::isCounter(int x, int y) const
{
	if (getChipFlag(x, y, false) & CRpgLdb::FLAG_COUNTER)
		return true;
	if (getChipFlag(x, y, true) & CRpgLdb::FLAG_COUNTER)
		return true;
	return false;
}


void GameMap::setPlayerPosition(const kuto::Vector2& pos)
{
	float mapWidth = rpgLmu_.GetWidth() * 16.f;
	float mapHeiht = rpgLmu_.GetHeight() * 16.f;
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
	const CRpgLdb::ChipSet& chipSet = rpgLdb_->saChipSet[rpgLmu_.GetChipSet()];
	uint lowerChip = rpgLmu_.getLowerChipId(x, y);
	return lowerChip < chipSet.randforms.size()? chipSet.randforms[lowerChip] : 1;
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

