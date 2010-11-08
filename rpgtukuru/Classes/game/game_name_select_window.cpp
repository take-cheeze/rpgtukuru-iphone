/**
 * @file
 * @brief Name Select
 * @author project.kuto
 */

#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_render_manager.h>

#include "game.h"
#include "game_name_select_window.h"


namespace {
	const char* const HIRAGANA = "あいうえおがぎぐげごかきくけこざじずぜぞさしすせそだぢづでどたちつてとばびぶべぼなにぬねのぱぴぷぺぽはひふへほぁぃぅぇぉまみむめもっゃゅょゎやゆよわんー〜・＝☆らりるれろ";
	const char* const KATAKANA = "アイウエオガギグゲゴカキクケコザジズゼゾサシスセソダヂヅデドタチツテトバビブベボナニヌネノパピプペポハヒフヘホァィゥェォマミミメモッャュョヮヤユヨワンー〜・＝☆ラリルレロ";
}


GameNameSelectWindow::GameNameSelectWindow(Game& g)
: GameSelectWindow(g)
, kanaType_(kHiragana)
{
	setAutoClose(false);
	setColumnSize(10);
	setKana(kHiragana);
	setPosition(kuto::Vector2(0.f, 80.f));
	setSize(kuto::Vector2(320.f, 160.f));
}

void GameNameSelectWindow::setKana(KanaType type)
{
	kanaType_ = type;
	const char* str = (kanaType_ == kHiragana)? HIRAGANA : KATAKANA;
	clearMessages();
	uint len = strlen(str);
	for (uint i = 0; i < len; i++) {
		char c = str[i];
		std::string mes;
		mes.push_back(c);
		if (c & 0x80) {
			if (c & 0x40) {
				mes.push_back(str[++i]);
				if (c & 0x20) {
					mes.push_back(str[++i]);
				}
			}
		}
		addLine(mes);
	}
	addLine("ヴ");
	addLine((kanaType_ == kHiragana)? "<カナ>" : "<かな>");
	addLine("");
	addLine("<決定>");
	addLine("");
}


void GameNameSelectWindow::update()
{
	int oldCursor = cursor();
	GameSelectWindow::update();
	if (oldCursor == kKanaButton && cursor() == kKanaButton+1)
		cursor_++;
	else if (cursor() == kKanaButton+1)
		cursor_--;
	if (oldCursor == kApplyButton && cursor() == kApplyButton+1)
		cursor_ -= 9;
	else if (oldCursor == kApplyButton+1)
		cursor_--;
	if (selected() && (cursor() == kKanaButton || cursor() == kKanaButton + 1)) {
		reset();
		setKana((KanaType)((kanaType_ + 1) % kKanaMax));
	}
}

void GameNameSelectWindow::render(kuto::Graphics2D& g) const
{
	if (showFrame_)
		renderFrame(g);

	if (showCursor_)
		renderSelectCursor(g);

	renderText(g);

	if (showCursor_) {
		int rowSize = maxRowSize();
		if (rowSize * columnSize_ + scrollPosition_ * columnSize_ < (int)messages_.size()) {
			renderDownCursor(g);
		}
		if (scrollPosition_ > 0) {
			renderUpCursor(g);
		}
	}
}


void GameNameSelectWindow::renderSelectCursor(kuto::Graphics2D& g) const
{
	const kuto::Texture& systemTexture = game_.systemTexture();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 windowSize(size_);
	kuto::Vector2 windowPosition(position_);
	kuto::Vector2 scale((windowSize.x - 8.f) / columnSize_, rowHeight_);
	kuto::Vector2 pos(windowPosition.x + 4.f + (cursor_ % columnSize_) * scale.x,
					windowPosition.y + 8.f + (cursor_ / columnSize_ - scrollPosition_) * (rowHeight_ + lineSpace_));
	if (cursor() == kKanaButton+1 || cursor() == kApplyButton+1) {
		pos.x -= scale.x;
	}
	if (cursor() >= kKanaButton) {
		scale.x *= 2.f;
	}
	kuto::Vector2 texcoord0, texcoord1;
	if ((cursorAnimationCounter_ / 6) % 2 == 0) {
		texcoord0.set(96.f / systemTexture.width(), 0.f);
		texcoord1.set(128.f / systemTexture.width(), 32.f / systemTexture.height());
	} else {
		texcoord0.set(64.f / systemTexture.width(), 0.f);
		texcoord1.set(96.f / systemTexture.width(), 32.f / systemTexture.height());
	}
	kuto::Vector2 borderSize(8.f, 8.f);
	kuto::Vector2 borderCoord(8.f / systemTexture.width(), 8.f / systemTexture.height());
	g.drawTexture9Grid(systemTexture, pos, scale, color, texcoord0, texcoord1, borderSize, borderCoord);
}
