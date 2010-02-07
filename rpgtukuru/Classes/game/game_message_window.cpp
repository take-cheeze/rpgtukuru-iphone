/**
 * @file
 * @brief Message Window
 * @author takuto
 */

#include "game_message_window.h"
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>


GameMessageWindow::GameMessageWindow(kuto::Task* parent, const GameSystem& gameSystem)
: GameWindow(parent, gameSystem)
, animationCounter_(0), clicked_(false), animationEnd_(false), useAnimation_(true)
, enableSkip_(true), enableClick_(true), lineLimit_(-1)
{
}

void GameMessageWindow::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	switch (state_) {
	case kStateOpen:
		state_ = kStateLoop;
		break;
	case kStateLoop:
		if (virtualPad->press(kuto::VirtualPad::KEY_A)) {
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
		break;
	}
	animationCounter_++;

	if (animationCounter_ > getMessageLength()) {
		animationEnd_ = true;
	}
}

void GameMessageWindow::render()
{
	if (showFrame_)
		renderFrame();
	if (animationEnd_ && enableClick_) {
		if (animationCounter_ / 8 % 2 == 0)
			renderDownCursor();
	}
	if (faceEnable_)
		renderFace();
	renderText();
}

void GameMessageWindow::renderText()
{
	int rowSize = getMaxRowSize();
	bool unbreak = (animationEnd_ || !useAnimation_);
	int row = 0;
	int restCount = animationCounter_;
	int lineSize = lineLimit_ >= 0? kuto::min(lineLimit_, (int)messages_.size()) : messages_.size();
	for (int line = 0; line < lineSize; line++) {
		if (line >= kuto::max(lineSize - rowSize, 0)) {
			renderTextLine(line, row, 1, unbreak? kLineStringMax * 2 : restCount);
			row++;
		}
		restCount -= getMessageLineLength(line);
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

