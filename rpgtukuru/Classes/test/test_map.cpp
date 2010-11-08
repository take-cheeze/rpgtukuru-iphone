/**
 * @file
 * @brief Map Draw Test
 * @author project.kuto
 */

#include "test_map.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>


TestMap::TestMap()
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 0.f)
, rpgLdb_(GAME_DATA_PATH)
, rpgLmu_(GAME_DATA_PATH, kuto::random(126) + 1)
, rpgLmt_(GAME_DATA_PATH)
, animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
{
	kuto::VirtualPad::instance().pauseDraw(true);
}

bool TestMap::initialize()
{
	return isInitializedChildren();
}

void TestMap::update()
{
	kuto::TouchPad& touchPad = kuto::TouchPad::instance();
	if (touchPad.doubleClick(0)) {
		// return debug menu
		this->release();
	}
	if (touchPad.move(0) || touchPad.move(1)) {
		if (touchPad.on(0) && touchPad.on(1)) {
			// Multi touch -> scaling
			kuto::Vector2 offsetPrev = touchPad.prevPosition(0) - touchPad.prevPosition(1);
			kuto::Vector2 offset = touchPad.position(0) - touchPad.position(1);
			kuto::Vector2 center = (touchPad.position(0) + touchPad.position(1)) * 0.5f;
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
			screenOffset_ += touchPad.moveOffset(0);
		}
	}
	animationCounter_++;
}

struct DefferdCommand {
	kuto::Vector2			pos;
	// MapUnit::TextureInfo	info;
};

void TestMap::render(kuto::Graphics2D& g) const
{
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	// Panorama
	const kuto::Texture* panorama = NULL; // rpgLmu_.GetPanoramaTexture();
	if (panorama) {
		kuto::Vector2 pos(screenOffset_);
		kuto::Vector2 scale(panorama->orgWidth(), panorama->orgHeight());
		scale *= screenScale_;
		g.drawTexture(*panorama, pos, scale, color, true);
	}
	// Map Chip
/*
	MapUnit::TextureInfoSet infoSet;
	MapUnit::TextureInfo& info = infoSet.info[0];
 */
	const kuto::Vector2 size(16.f * screenScale_.x, 16.f * screenScale_.y);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));
	std::vector<DefferdCommand> defferedRenders;
	for (uint x = startX; x < rpgLmu_.width(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= 320.f) break;

		for (uint y = startY; y < rpgLmu_.height(); y++) {
/*
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 480.f) break;

			kuto::Vector2 pos(posx, posy);
			if (rpgLmu_.chipIDLw(x, y, animationCounter_, infoSet)) {
				if (infoSet.size > 0) {
					if (infoSet.size == 1) {
						if (infoSet.info[0].texture == rpgLmu_.GetChipSetTexture()) {
							g.drawTexture(*infoSet.info[0].texture, pos, size, color,
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
							g.drawTexture(*infoSet.info[i].texture, pos + offset, size * 0.5f, color,
									infoSet.info[i].texcoord[0], infoSet.info[i].texcoord[1]);
						}
					}
				}
			}
 */
		}
	}
/*
	for (uint i = 0; i < defferedRenders.size(); i++) {
		g.drawTexture(*defferedRenders[i].info.texture, defferedRenders[i].pos, size, color,
					   defferedRenders[i].info.texcoord[0], defferedRenders[i].info.texcoord[1]);
	}
 */
	for (uint x = startX; x < rpgLmu_.width(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= 320.f) break;

		for (uint y = startY; y < rpgLmu_.height(); y++) {
/*
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 480.f)
				break;
			kuto::Vector2 pos(posx, posy);
			if (rpgLmu_.chipIDLw(x, y, info)) {
				g.drawTexture(*info.texture, pos, size, color, info.texcoord[0], info.texcoord[1]);
			}
 */
		}
	}
}
