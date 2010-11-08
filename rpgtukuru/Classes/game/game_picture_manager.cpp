#include "game.h"
#include "game_field.h"
#include "game_picture_manager.h"

#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>

#include <rpg2k/Project.hpp>


GamePictureManager::GamePictureManager(GameField& f)
: field_(f)
{
	for(int i = 0; i < rpg2k::PICTURE_NUM; i++) {
		pictureList_[i] = addChildBack( Picture::createTask(f, rpg2k::ID_MIN + i) );
	}
}
void GamePictureManager::update()
{
}
bool GamePictureManager::isMoving(unsigned const id) const
{
	return pictureList_[rpg2k::ID_MIN + id]->isMoving();
}

bool GamePictureManager::Picture::isMoving() const
{
	return field_.project().getLSD().picture()[id_][51].to<int>() > 0;
}

GamePictureManager::Picture::Picture(GameField& f, unsigned const id)
: IRender2D(kuto::Layer::OBJECT_2D, 1.f + 0.1f * id)
, field_(f), id_(id)
, effectCounter_(0)
{
}

void GamePictureManager::Picture::update()
{
	rpg2k::structure::Array1D& pict = field_.project().getLSD().picture()[id_];
	int& count = pict[51].to<int>();
	if (count > 0) {
		count--;
		if (count == 0) {
			// TODO: clear animation(moving) data
		} else {
			double const countDouble = count;
			// color
			for(int i = 0; i < 4; i++) {
				pict[11 + i] = double(pict[11 + i].to<double>() + (pict[41 + i].to<int>() - pict[11 + i].to<double>()) / countDouble);
			}
			// position
			pict[4] = double(pict[4].to<double>() + (pict[31].to<int>() - pict[4].to<double>()) / countDouble);
			pict[5] = double(pict[4].to<double>() + (pict[32].to<int>() - pict[5].to<double>()) / countDouble);
			// TODO: scale change
			// TODO: effect change
			// TODO: alpha change
		}
	}

	effectCounter_++;
}

void GamePictureManager::Picture::render(kuto::Graphics2D& g) const
{
	rpg2k::structure::Array1D const& pict = field_.project().getLSD().picture()[id_];
	if( !pict[1].exists() ) return;

	int const& count = pict[51];
	kuto::Vector2 curP;
	kuto::Color color;
	float sat, scale;
	if( count ) {
		curP.set( pict[4].to<double>(), pict[5].to<double>() );
		scale = pict[33].to<int>();
		color.set(pict[11].to<double>(), pict[12].to<double>(), pict[13].to<double>(), pict[14].to<double>());
		sat = pict[14].to<double>();
	} else {
		curP.set( pict[4].to<double>(), pict[5].to<double>() );
		scale = pict[33].to<int>();
		color.set(pict[41].to<double>(), pict[42].to<double>(), pict[43].to<double>(), pict[44].to<double>());
		sat = pict[44].to<double>();
	}

	kuto::Texture& tex = field_.game().texPool().picture( pict[1].to_string().toSystem(), pict[9].to<bool>() );
	kuto::Vector2 texS( tex.orgWidth(), tex.orgHeight() );
	kuto::Vector2 pictS = texS * scale / 100.f;
	kuto::ColorHSV colorHSV = (color / 100.f).hsv();
	colorHSV.s *= sat / 100.f;
	color = colorHSV.rgb();

	kuto::Vector2 pos = curP - (texS / 2.f);

	switch(pict[15].to<int>()) {
	case kEffectNone:
		g.drawTexture(tex, pos, pictS, color, true);
		break;
	case kEffectRoll: {
		float angle = float(effectCounter_) * kuto::PI * 0.01f * pict[16].to<double>();
		g.drawTextureRotate(tex, curP, pictS, color, angle /* pict[52].to<double>() */, true);
	} break;
	case kEffectWave: // TODO: wave, sin?
		g.drawTexture(tex, pos, pictS, color, true);
		break;
	default: kuto_assert(false);
	}
}
