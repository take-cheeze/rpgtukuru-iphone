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


GameEventPicture::GameEventPicture(const std::string& filename, rpg2k::structure::Array1D& info)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 1.f /* priority_ */), info_(info)
, effectCounter_(0), moveCounter_(0), moveCounterMax_(0), priority_(1.f)
{
	if( !RPG2kUtil::LoadImage(texture_, filename, info[9].get<bool>()) ) kuto_assert(false);
}

void GameEventPicture::setPriority(float value)
{
	priority_ = value;
	IRender2D::reset(kuto::Layer::OBJECT_2D, priority_);
}

bool GameEventPicture::initialize()
{
	return isInitializedChildren();
}

void GameEventPicture::update()
{
	int& count = info_[51].get<int>();
	if (count > 0) {
		count--;
		if (count == 0) {
			// TODO: clear animation(moving) data
		} else {
			double const countDouble = count;
			// color
			for(int i = 0; i < 4; i++) {
				info_[11 + i] = double(info_[11 + i].get<double>() + (info_[41 + i].get<int>() - info_[11 + i].get<double>()) / countDouble);
			}
			// position
			info_[4] = double(info_[4].get<double>() + (info_[31].get<int>() - info_[4].get<double>()) / countDouble);
			info_[5] = double(info_[4].get<double>() + (info_[32].get<int>() - info_[5].get<double>()) / countDouble);
			// TODO: scale change
			// TODO: effect change
			// TODO: alpha change
		}
	}
	effectCounter_++;
}

void GameEventPicture::render(kuto::Graphics2D* g) const
{
	int const count = info_[51];
	kuto::Vector2 curP;
	kuto::Color color;
	float sat, scale;
	if( count ) {
		curP.set( info_[4].get<double>(), info_[5].get<double>() );
		scale = info_[33].get<int>();
		color.set(info_[11].get<double>(), info_[12].get<double>(), info_[13].get<double>(), info_[14].get<double>());
		sat = info_[14].get<double>();
	} else {
		curP.set( info_[4].get<double>(), info_[5].get<double>() );
		scale = info_[33].get<int>();
		color.set(info_[41].get<double>(), info_[42].get<double>(), info_[43].get<double>(), info_[44].get<double>());
		sat = info_[44].get<double>();
	}

	kuto::Vector2 texS( texture_.getOrgWidth(), texture_.getOrgHeight() );
	kuto::Vector2 pictS = texS * scale / 100.f;
	kuto::ColorHSV colorHSV = (color / 100.f).hsv();
	colorHSV.s *= sat / 100.f;
	color = colorHSV.rgb();

	kuto::Vector2 pos = curP - (texS / 2.f);

	switch(info_[15].get<int>()) {
	case kEffectNone:
		g->drawTexture(texture_, pos, pictS, color, true);
		break;
	case kEffectRoll: {
		float angle = float(effectCounter_) * kuto::PI * 0.01f * info_[16].get<double>();
		g->drawTextureRotate(texture_, curP, pictS, color, angle /* info_[52].get<double>() */, true);
	} break;
	case kEffectWave: // TODO: wave, sin?
		g->drawTexture(texture_, pos, pictS, color, true);
		break;
	default: kuto_assert(false);
	}
}

void GameEventPicture::move(rpg2k::structure::Instruction const& info, int count)
{
	// info_[51] = count;
	moveCounterMax_ = count;

	// infoMove_ = info;
}
