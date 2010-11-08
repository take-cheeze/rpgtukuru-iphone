/**
 * @file
 * @brief Game Event Map Parts
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_event_map_chip.h"
#include "game_map.h"


GameEventMapChip::GameEventMapChip(rpg2k::model::DataBase& ldb, GameMap* map)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 9.f /* - 0.001f * position_.y - 0.01f * priority_ */)
, rpgLdb_(ldb), map_(map), partsIndex_(0)
, position_(0, 0), priority_(rpg2k::EventPriority::CHAR)
{
}

void GameEventMapChip::setPosition(const kuto::Point2& pos)
{
	position_ = pos;
	IRender2D::reset(kuto::Layer::OBJECT_2D, 9.f - 0.001f * position_.y - 0.01f * priority_);
}

void GameEventMapChip::update()
{
}

void GameEventMapChip::render(kuto::Graphics2D* g) const
{
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
