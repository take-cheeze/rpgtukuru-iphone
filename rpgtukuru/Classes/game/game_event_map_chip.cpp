/**
 * @file
 * @brief Game Event Map Parts
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_event_map_chip.h"
#include "game_map.h"


GameEventMapChip::GameEventMapChip(kuto::Task* parent, CRpgLdb& ldb, GameMap* map)
: kuto::Task(parent)
, rpgLdb_(ldb), map_(map), partsIndex_(0)
, position_(0, 0), priority_(CRpgMapEvent::kDrawPriorityNormal)
{
}

GameEventMapChip::~GameEventMapChip()
{
}

void GameEventMapChip::update()
{
	
}

void GameEventMapChip::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 9.f - 0.001f * position_.y - 0.01f * priority_);
}


void GameEventMapChip::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	
	CRpgLmu::TextureInfo info;
	kuto::Vector2 size(16.f, 16.f);
	kuto::Vector2 pos;
	pos.x = position_.x;
	pos.y = position_.y;
	pos *= size.x;
	pos += map_->getOffsetPosition();
	
	if (pos.x + size.x > 0.f && pos.x < 320.f && pos.y + size.y > 0.f && pos.y < 240.f) {
		int chipId = partsIndex_ + 10000;
		if (map_->getRpgLmu().GetUpperChip(chipId, info)) {
			g->drawTexture(*info.texture, pos, size, color, info.texcoord[0], info.texcoord[1]);
		}
	}
}

