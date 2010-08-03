/**
 * @file
 * @brief Fade Effect
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>
#include "game_fade_effect.h"


GameFadeEffect::GameFadeEffect(kuto::Task* parent)
: kuto::Task(parent), type_(kTypeFade), state_(kStateNone), counter_(0)
{
}

void GameFadeEffect::update()
{
	counter_++;
	switch (state_) {
	case kStateEncountFlash:
		if (counter_ > 20) {
			setState(kStateFadeOut);
		}
		break;
	case kStateFadeOut:
		if (counter_ > 30) {
			setState(kStateFadeOutEnd);
		}
		break;
	case kStateFadeIn:
		if (counter_ > 30) {
			setState(kStateFadeInEnd);
		}
		break;
	default: break;
	}
}

void GameFadeEffect::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, -1.f);
}

void GameFadeEffect::start(FadeType type, State state)
{
	type_ = type;
	state_ = state;
	counter_ = 0;

	blocks_.zeromemory();
	switch (type_) {
	case kTypeRandomBlock:
		for (uint i = 0; i < blocks_.size(); i++) {
			blocks_[i] = i % 30;
		}
		for (uint i = 0; i < blocks_.size(); i++) {
			int swapIndex = kuto::random((int)blocks_.size());
			int buf = blocks_[i];
			blocks_[i] = blocks_[swapIndex];
			blocks_[swapIndex] = buf;
		}
		break;
	case kTypeRandomBlockTopDown:
		for (uint i = 0; i < blocks_.size(); i++) {
			blocks_[i] = kuto::random(7) + (i / 40) - 5;
		}
		break;
	case kTypeRandomBlockBottomUp:
		for (uint i = 0; i < blocks_.size(); i++) {
			blocks_[i] = kuto::random(7) + (30 - i / 40) - 5;
		}
		break;
	default: break;
	}
}

void GameFadeEffect::render()
{
	switch (state_) {
	case kStateEncountFlash:
		renderBattleFlash();
		break;
	case kStateFadeOut:
		switch (type_) {
		case kTypeFade:
			renderFade((float)counter_ / 30.f);
			break;
		case kTypeRandomBlock:
		case kTypeRandomBlockTopDown:
		case kTypeRandomBlockBottomUp:
			renderRandomBlock((float)counter_ / 30.f);
			break;
		case kTypeBlind:
			renderBlind((float)counter_ / 30.f);
			break;
		case kTypeStripeVertical:
			renderStripeVertical((float)counter_ / 30.f);
			break;
		case kTypeStripeHorizontal:
			renderStripeHorizontal((float)counter_ / 30.f);
			break;
		case kTypeHoleShrink:
			renderHoleShrink((float)counter_ / 30.f);
			break;
		case kTypeHoleExpand:
			renderHoleExpand((float)counter_ / 30.f);
			break;
		case kTypeImmediate:
			setState(kStateFadeOutEnd);
			break;
		case kTypeNothing:
			break;
		default:
			renderFade((float)counter_ / 30.f);
			break;
		}
		break;
	case kStateFadeOutEnd:
		renderFade(1.f);
		break;
	case kStateFadeIn:
		switch (type_) {
		case kTypeFade:
			renderFade(1.f - (float)counter_ / 30.f);
			break;
		case kTypeRandomBlock:
		case kTypeRandomBlockTopDown:
		case kTypeRandomBlockBottomUp:
			renderRandomBlock(1.f - (float)counter_ / 30.f);
			break;
		case kTypeBlind:
			renderBlind(1.f - (float)counter_ / 30.f);
			break;
		case kTypeStripeVertical:
			renderStripeVertical(1.f - (float)counter_ / 30.f);
			break;
		case kTypeStripeHorizontal:
			renderStripeHorizontal(1.f - (float)counter_ / 30.f);
			break;
		case kTypeHoleShrink:
			renderHoleShrink(1.f - (float)counter_ / 30.f);
			break;
		case kTypeHoleExpand:
			renderHoleExpand(1.f - (float)counter_ / 30.f);
			break;
		case kTypeImmediate:
			setState(kStateFadeInEnd);
			break;
		case kTypeNothing:
			break;
		default:
			renderFade(1.f - (float)counter_ / 30.f);
			break;
		}
		break;
	default: break;
	}
}

void GameFadeEffect::renderBattleFlash()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	float alpha = (counter_ % 10) >= 5? (float)(10 - counter_ % 10) / 5.f : (float)(counter_ % 10) / 5.f;
	kuto::Color color(1.f, 1.f, 1.f, alpha);
	g->fillRectangle(kuto::Vector2(0, 0), kuto::Vector2(320, 240), color);
}

void GameFadeEffect::renderFade(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(0.f, 0.f, 0.f, ratio);
	g->fillRectangle(kuto::Vector2(0, 0), kuto::Vector2(320, 240), color);
}

void GameFadeEffect::renderRandomBlock(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(0.f, 0.f, 0.f, 1.f);
	for (uint i = 0; i < blocks_.size(); i++) {
		if (blocks_[i] <= ratio * 30.f) {
			kuto::Vector2 pos((i % 40) * 8.f, (i / 40) * 8.f);
			kuto::Vector2 size(8.f, 8.f);
			g->fillRectangle(pos, size, color);
		}
	}
}

void GameFadeEffect::renderBlind(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(0.f, 0.f, 0.f, 1.f);
	for (int i = 0; i < 30; i++) {
		kuto::Vector2 pos(0.f, i * 8.f);
		kuto::Vector2 size(320.f, ratio * 8.f);
		g->fillRectangle(pos, size, color);
	}
}

void GameFadeEffect::renderStripeVertical(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(0.f, 0.f, 0.f, 1.f);
	const kuto::Vector2 size(320.f, 4.f);
	for (int i = 0; i < (int)(ratio * 30) + 1; i++) {
		kuto::Vector2 pos(0.f, i * 8.f);
		g->fillRectangle(pos, size, color);
		pos.y = 240.f - i * 8.f - 4.f;
		g->fillRectangle(pos, size, color);
	}
}

void GameFadeEffect::renderStripeHorizontal(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(0.f, 0.f, 0.f, 1.f);
	const kuto::Vector2 size(6.f, 240.f);
	for (int i = 0; i < (int)(ratio * 30) + 1; i++) {
		kuto::Vector2 pos(i * 10.666666f, 0.f);
		g->fillRectangle(pos, size, color);
		pos.x = 320.f - i * 10.666666f - 6.f;
		g->fillRectangle(pos, size, color);
	}
}

void GameFadeEffect::renderHoleShrink(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(0.f, 0.f, 0.f, 1.f);
	kuto::Vector2 size(320.f, 240.f * ratio * 0.5f);
	kuto::Vector2 pos(0.f, 0.f);
	g->fillRectangle(pos, size, color);
	pos.y = 240.f - size.y;
	g->fillRectangle(pos, size, color);
	pos.y = size.y;
	size.set(320.f * ratio * 0.5f, 240.f - size.y * 2.f);
	g->fillRectangle(pos, size, color);
	pos.x = 320.f - size.x;
	g->fillRectangle(pos, size, color);
}

void GameFadeEffect::renderHoleExpand(float ratio)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(0.f, 0.f, 0.f, 1.f);
	const kuto::Vector2 size(320.f * ratio, 240.f * ratio);
	const kuto::Vector2 pos((320.f - size.x) * 0.5f, (240.f - size.y) * 0.5f);
	g->fillRectangle(pos, size, color);
}



