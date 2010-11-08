/**
 * @file
 * @brief Message Window
 * @author project.kuto
 */

#include "game_message_window.h"
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>


GameMessageWindow::GameMessageWindow(Game& g)
: GameWindow(g)
, animationCounter_(0), lineLimit_(-1)
, clicked_(false), animationEnd_(false), useAnimation_(true)
, enableSkip_(false), enableClick_(true)
{
}

void GameMessageWindow::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	switch (state_) {
	case kStateOpen:
		state_ = kStateLoop;
		break;
	case kStateLoop:
		if (virtualPad.press(kuto::VirtualPad::KEY_A)) {
			if (!animationEnd_) {
				if (enableSkip_)
					animationEnd_ = true;
			} else {
				if (enableClick_) {
					clicked_ = true;
					state_ = kStateClose;
				}
			}
		}
		break;
	case kStateClose:
		renderMoneyWindow_ = false;
		break;
	}
	animationCounter_++;

	if (animationCounter_ > (int)messageLength()) {
		animationEnd_ = true;
	}
}

void GameMessageWindow::render(kuto::Graphics2D& g) const
{
	if (showFrame_) {
		renderFrame(g);
	}
	if (animationEnd_ && enableClick_ && (animationCounter_ / 8 % 2 == 0)) {
		renderDownCursor(g);
	}
	if (faceEnable_) {
		renderFace(g);
	}
	renderText(g);

	if(renderMoneyWindow_) {
		// TODO: render money window
	}
}

void GameMessageWindow::renderText(kuto::Graphics2D& g) const
{
	int rowSize = maxRowSize();
	bool unbreak = (animationEnd_ || !useAnimation_);
	int row = 0;
	int restCount = animationCounter_;
	int lineSize = lineLimit_ >= 0? kuto::min(lineLimit_, (int)messages_.size()) : messages_.size();
	for (int line = 0; line < lineSize; line++) {
		if (line >= kuto::max(lineSize - rowSize, 0)) {
			renderTextLine(g, line, row, 1, unbreak? kLineStringMax * 2 : restCount);
			row++;
		}
		restCount -= messageLineLength(line);
		if (!unbreak && restCount <= 0)
			break;
	}
}

void GameMessageWindow::reset()
{
	animationEnd_ = false;
	clicked_ = false;
	animationCounter_ = 0;
	lineLimit_ = -1;
}
