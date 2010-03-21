/**
 * @file
 * @brief Skill animation
 * @autor project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>
#include "game_event_picture.h"
#include "game_map.h"
#include "CRpgUtil.h"


GameEventPicture::GameEventPicture(kuto::Task* parent, const std::string& filename, const Info& info)
: kuto::Task(parent), infoBase_(info)
, effectCounter_(0), moveCounter_(0), moveCounterMax_(0)
{
	CRpgUtil::LoadImage(texture_, filename, info.useAlpha);
}

bool GameEventPicture::initialize()
{
	return isInitializedChildren();
}

void GameEventPicture::update()
{
	if (moveCounter_ > 0) {
		moveCounter_--;
		if (moveCounter_ == 0) {
			infoBase_ = infoMove_;
		}
	}
	effectCounter_++;
}

void GameEventPicture::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 1.f);
}

void GameEventPicture::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	float ratio = 1.f - (float)moveCounter_ / (float)moveCounterMax_;
	kuto::Vector2 pos = kuto::lerp(infoBase_.position, infoMove_.position, ratio);
	float scale = kuto::lerp(infoBase_.scale, infoMove_.scale, ratio);
	kuto::Vector2 size(texture_.getOrgWidth() * scale, texture_.getOrgHeight() * scale);
	kuto::Color color = kuto::lerp(infoBase_.color, infoMove_.color, ratio);
	// float saturation = kuto::lerp(infoBase_.saturation, infoMove_.saturation, ratio);
	if (infoBase_.effect == kEffectNone) {
		pos.x -= texture_.getOrgWidth() * 0.5f;
		pos.y -= texture_.getOrgHeight() * 0.5f;
		g->drawTexture(texture_, pos, size, color, true);
	} else if (infoBase_.effect == kEffectRoll) {
		float angle = (float)effectCounter_ * kuto::PI * 0.01f * infoBase_.effectSpeed;
		g->drawTextureRotate(texture_, pos, size, color, angle, true);
	} else {
		pos.x -= texture_.getOrgWidth() * 0.5f;
		pos.y -= texture_.getOrgHeight() * 0.5f;
		g->drawTexture(texture_, pos, size, color, true);
	}
}

void GameEventPicture::move(const Info& info, int count)
{
	moveCounter_ = count;
	moveCounterMax_ = count;
	infoMove_ = info;
}


