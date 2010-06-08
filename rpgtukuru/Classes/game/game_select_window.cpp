/**
 * @file
 * @brief Select Window
 * @author project.kuto
 */

#include "game_select_window.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>


GameSelectWindow::GameSelectWindow(kuto::Task* parent, const GameSystem& gameSystem)
: GameWindow(parent, gameSystem)
, cursor_(0), columnSize_(1), cursorAnimationCounter_(0)
, scrollPosition_(0), cursorStart_(0)
, selected_(false), canceled_(false), pauseUpdateCursor_(false)
, enableCancel_(true), autoClose_(true), showCursor_(true), fullSelect_(false)
{
}

void GameSelectWindow::update()
{
	switch (state_) {
	case kStateOpen:
		state_ = kStateLoop;
		break;
	case kStateLoop:
		if (!pauseUpdateCursor_) {
			kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
			int rowSize = getMaxRowSize();
			
			if (virtualPad->repeat(kuto::VirtualPad::KEY_LEFT)) {
				cursor_ = cursor_ - 1;
				if (cursor_ < cursorStart_ || cursor_ % columnSize_ == columnSize_ - 1)
					cursor_ = kuto::max(cursorStart_, kuto::min((int)messages_.size() - 1, cursor_ + columnSize_));
			}
			if (virtualPad->repeat(kuto::VirtualPad::KEY_RIGHT)) {
				cursor_ = cursor_ + 1;
				if (cursor_ >= (int)messages_.size() || cursor_ % columnSize_ == 0)
					cursor_ = kuto::max(cursorStart_, kuto::min((int)messages_.size() - 1, cursor_ - columnSize_));
			}
			if (virtualPad->repeat(kuto::VirtualPad::KEY_UP)) {
				cursor_ = cursor_ - columnSize_;
				if (cursor_ < cursorStart_) {
					cursor_ = kuto::max(cursorStart_, kuto::min((int)messages_.size() - 1, cursor_ + (int)messages_.size()));
					scrollPosition_ = kuto::max(cursorStart_, cursor_ / columnSize_ - (rowSize - 1));
				} else {
					if (cursor_ / columnSize_ - scrollPosition_ < 0)
						scrollPosition_--;
				}
			}
			if (virtualPad->repeat(kuto::VirtualPad::KEY_DOWN)) {
				cursor_ = cursor_ + columnSize_;
				if (cursor_ >= (int)messages_.size()) {
					cursor_ = kuto::max(cursorStart_, kuto::min((int)messages_.size() - 1, cursor_ - (int)messages_.size()));
					scrollPosition_ = 0;
				} else {
					if (cursor_ / columnSize_ - scrollPosition_ >= rowSize)
						scrollPosition_++;
				}
			}
			if (virtualPad->repeat(kuto::VirtualPad::KEY_A)) {
				if (itemEnables_[cursor_]) {
					selected_ = true;
					if (autoClose_)
						state_ = kStateClose;
				}
			}
			if (enableCancel_ && virtualPad->repeat(kuto::VirtualPad::KEY_B)) {
				canceled_ = true;
				if (autoClose_)
					state_ = kStateClose;
			}
			cursorAnimationCounter_++;
		} else {
			cursorAnimationCounter_ = 0;
		}
		break;
	case kStateClose:
		break;
	}
}

void GameSelectWindow::render()
{
	if (showFrame_)
		renderFrame();
	if (faceEnable_)
		renderFace();
	
	if (showCursor_)
		renderSelectCursor();
	
	renderText();
	
	if (showCursor_) {
		int rowSize = getMaxRowSize();
		if (rowSize * columnSize_ + scrollPosition_ * columnSize_ < (int)messages_.size()) {
			renderDownCursor();
		}
		if (scrollPosition_ > 0) {
			renderUpCursor();
		}
	}
}

void GameSelectWindow::renderText()
{
	int rowSize = getMaxRowSize();
	int startIndex = kuto::max(0, scrollPosition_ * columnSize_);
	int row = 0;
	for (uint i = startIndex; i < messages_.size(); i++) {
		renderTextLine((int)i, row, (int)columnSize_, (int)kLineStringMax * 2);
		if (i % columnSize_ == (uint)(columnSize_ - 1))
			row++;
		if (row >= rowSize)
			break;
	}
}

void GameSelectWindow::renderSelectCursor()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Texture& systemTexture = gameSystem_.getSystemTexture();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 windowSize(size_);
	kuto::Vector2 windowPosition(position_);
	if (faceEnable_) {
		windowPosition.x += 64.f - 4.f;
		windowSize.x -= 64.f - 4.f;
	}
	kuto::Vector2 scale((windowSize.x - 8.f) / columnSize_, rowHeight_);
	kuto::Vector2 pos(windowPosition.x + 4.f + (cursor_ % columnSize_) * scale.x,
					windowPosition.y + 8.f + (cursor_ / columnSize_ - scrollPosition_) * (rowHeight_ + lineSpace_));
	if (fullSelect_) {
		scale.set(windowSize.x - 8.f, kuto::min(windowSize.y - 16.f, messages_.size() * (rowHeight_ + lineSpace_) - lineSpace_));
		pos.set(windowPosition.x + 4.f, windowPosition.y + 8.f);
	}
	kuto::Vector2 texcoord0, texcoord1;
	if ((cursorAnimationCounter_ / 6) % 2 == 0) {
		texcoord0.set(96.f / systemTexture.getWidth(), 0.f);
		texcoord1.set(128.f / systemTexture.getWidth(), 32.f / systemTexture.getHeight());
	} else {
		texcoord0.set(64.f / systemTexture.getWidth(), 0.f);
		texcoord1.set(96.f / systemTexture.getWidth(), 32.f / systemTexture.getHeight());
	}
	kuto::Vector2 borderSize(8.f, 8.f);
	kuto::Vector2 borderCoord(8.f / systemTexture.getWidth(), 8.f / systemTexture.getHeight());
	g->drawTexture9Grid(systemTexture, pos, scale, color, texcoord0, texcoord1, borderSize, borderCoord);
}

void GameSelectWindow::addMessage(const std::string& message, bool enable, int colorType)
{
	itemEnables_.push_back(enable);
	int realColorType = colorType;
	if (realColorType == -1)
		realColorType = enable? 0 : 1;
	addMessageImpl(message, realColorType);
}

