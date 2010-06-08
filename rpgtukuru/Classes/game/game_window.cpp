/**
 * @file
 * @brief Window
 * @author project.kuto
 */

#include "game_window.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "CRpgUtil.h"
#include "game_inventory.h"


GameWindow::GameWindow(kuto::Task* parent, const GameSystem& gameSystem)
: kuto::Task(parent)
, gameSystem_(gameSystem)
, position_(110.f, 150.f), size_(100.f, 60.f), fontSize_(12.f)
, priority_(0.f), rowHeight_(16.f), lineSpace_(0.f), messageAlign_(kAlignLeft)
, state_(kStateOpen), facePosition_(0)
, showFrame_(true), faceEnable_(false), faceRight_(false), faceReverse_(false)
{
}

bool GameWindow::initialize()
{
	return isInitializedChildren();
}

void GameWindow::update()
{
}

void GameWindow::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, priority_);
}

void GameWindow::renderFrame()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 windowSize(size_);
	const kuto::Vector2 windowPosition(position_);
	const kuto::Texture& systemTexture = gameSystem_.getSystemTexture();
	
	kuto::Vector2 scale = windowSize;
	kuto::Vector2 pos = windowPosition;
	kuto::Vector2 texcoord0(0.f, 0.f);
	kuto::Vector2 texcoord1(32.f / systemTexture.getWidth(), 32.f / systemTexture.getHeight());
	g->drawTexture(systemTexture, pos, scale, color, texcoord0, texcoord1);

	texcoord0.set(32.f / systemTexture.getWidth(), 0.f);
	texcoord1.set(64.f / systemTexture.getWidth(), 32.f / systemTexture.getHeight());
	kuto::Vector2 borderSize(8.f, 8.f);
	kuto::Vector2 borderCoord(8.f / systemTexture.getWidth(), 8.f / systemTexture.getHeight());
	g->drawTexture9Grid(systemTexture, pos, scale, color, texcoord0, texcoord1, borderSize, borderCoord);
}

void GameWindow::renderFace()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 size(48.f, 48.f);
	kuto::Vector2 pos(position_.x + 16.f, position_.y + 16.f);
	kuto::Vector2 sizeUV(48.f / faceTexture_.getWidth(), 48.f / faceTexture_.getHeight());
	kuto::Vector2 texcoord0;
	texcoord0.x = (facePosition_ % 4) * sizeUV.x;
	texcoord0.y = (facePosition_ / 4) * sizeUV.y;
	kuto::Vector2 texcoord1 = texcoord0 + sizeUV;
	g->drawTexture(faceTexture_, pos, size, color, texcoord0, texcoord1);	
}

void GameWindow::renderDownCursor()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 windowSize(size_);
	const kuto::Vector2 windowPosition(position_);
	const kuto::Texture& systemTexture = gameSystem_.getSystemTexture();
	
	kuto::Vector2 scale(16.f, 8.f);
	kuto::Vector2 pos(windowPosition.x + (windowSize.x * 0.5f) - 8.f, windowPosition.y + windowSize.y - 8.f);
	kuto::Vector2 texcoord0(40.f / systemTexture.getWidth(), 16.f / systemTexture.getHeight());
	kuto::Vector2 texcoord1(56.f / systemTexture.getWidth(), 24.f / systemTexture.getHeight());
	g->drawTexture(systemTexture, pos, scale, color, texcoord0, texcoord1);
}

void GameWindow::renderUpCursor()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 windowSize(size_);
	const kuto::Vector2 windowPosition(position_);
	const kuto::Texture& systemTexture = gameSystem_.getSystemTexture();
	
	kuto::Vector2 scale(16.f, 8.f);
	kuto::Vector2 pos(windowPosition.x + (windowSize.x * 0.5f) - 8.f, windowPosition.y);
	kuto::Vector2 texcoord0(40.f / systemTexture.getWidth(), 8.f / systemTexture.getHeight());
	kuto::Vector2 texcoord1(56.f / systemTexture.getWidth(), 16.f / systemTexture.getHeight());
	g->drawTexture(systemTexture, pos, scale, color, texcoord0, texcoord1);
}

void GameWindow::setFaceTexture(const std::string& filename, unsigned int position, bool right, bool reverse)
{
	if (filename.empty()) {
		faceEnable_ = false;
	} else {
		faceEnable_ = true;
		std::string faceTextureName = gameSystem_.getRootFolder();
		faceTextureName += "/FaceSet/";
		faceTextureName += filename;
		CRpgUtil::LoadImage(faceTexture_, faceTextureName, true);
		facePosition_ = position;
		faceRight_ = right;
		faceReverse_ = reverse;
	}
}

void GameWindow::renderTextLine(int line, int row, int columnMax, int count)
{
	int strPos = 0;
	std::string mes;
	for (unsigned int i = 0; i < messages_[line].str.size(); i++) {
		strPos++;
		if (count >= 0 && strPos > count) {
			break;
		}
		char c = messages_[line].str[i];
		if (c == 0x5c) {
			i++;
			c = tolower(messages_[line].str[i]);
			switch (c) {
			case 0x5c:
				mes.push_back(c);
				break;
			case 'c':
				i++;		// skip c
				i++;		// skip [
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case 's':
				i++;		// skip s
				i++;		// skip [
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case 'n':
				i++;		// skip n
				i++;		// skip [
				mes += gameSystem_.getPlayerInfo(atoi(messages_[line].str.c_str() + i)).name;
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case 'v':
				i++;		// skip v
				i++;		// skip [
				{
					char tempStr[32];
					sprintf(tempStr, "%d", gameSystem_.getVar(atoi(messages_[line].str.c_str() + i)));
					mes += tempStr;
				}
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case '$':
				{
					char tempStr[32];
					sprintf(tempStr, "%d", gameSystem_.getInventory()->getMoney());
					mes += tempStr;
				}
				break;
			case '!':
				break;
			case '.':
				break;
			case '|':
				break;
			case '>':
				break;
			case '<':
				break;
			case '_':
				mes.push_back(' ');
				break;
			}
		} else {
			mes.push_back(c);
			if (c & 0x80) {
				if (c & 0x40) {
					mes.push_back(messages_[line].str[++i]);
					if (c & 0x20) {
						mes.push_back(messages_[line].str[++i]);
					}
				}
			}
		}
	}
		
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color color(1.f, 1.f, 1.f, 1.f);
	switch (messages_[line].colorType) {
	case 1:
		color.set(0.5f, 0.5f, 0.5f, 1.f);
		break;
	}
	kuto::Vector2 windowSize(size_);
	kuto::Vector2 windowPosition(position_);
	if (faceEnable_) {
		windowPosition.x += 64.f;
		windowSize.x -= 64.f;
	}
	windowSize.x /= columnMax;
	windowPosition.x += (line % columnMax) * windowSize.x;

	kuto::Vector2 pos = windowPosition;
	
	pos.y += 8.f;
	pos.y += (rowHeight_ + lineSpace_) * row;
	pos.y += (rowHeight_ - fontSize_) * 0.5f;
	kuto::Vector2 scale = kuto::Font::instance()->getTextSize(mes.c_str(), fontSize_, kuto::Font::TYPE_NORMAL);
	switch (messageAlign_) {
	case kAlignLeft:
		pos.x = windowPosition.x + 8.f;
		break;
	case kAlignRight:
		pos.x = windowPosition.x + windowSize.x - scale.x - 8.f;
		break;
	case kAlignCenter:
		pos.x = windowPosition.x + (windowSize.x - scale.x) * 0.5f;
		break;
	}
	g->drawText(mes.c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
}

unsigned int GameWindow::getMessageLength() const
{
	unsigned int length = 0;
	for (unsigned int line = 0; line < messages_.size(); line++) {
		length += getMessageLineLength(line);
	}
	return length;
}

unsigned int GameWindow::getMessageLineLength(int line) const
{
	unsigned int length = 0;
	for (unsigned int i = 0; i < messages_[line].str.size(); i++) {
		char c = messages_[line].str[i];
		length++;
		if (c & 0x80) {
			if (c & 0x40) {
				i++;
				if (c & 0x20) {
					i++;
				}
			}
		} else if (c == 0x5c) {
			i++;
			c = tolower(messages_[line].str[i]);
			switch (c) {
			case 0x5c:
				break;
			case 'c':
				length--;
				i++;
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case 's':
				length--;
				i++;
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case 'n':
				length--;
				i++;
				length += gameSystem_.getPlayerInfo(atoi(messages_[line].str.c_str() + i)).name.size();
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case 'v':
				length--;
				i++;
				{
					char tempStr[32];
					sprintf(tempStr, "%d", gameSystem_.getVar(atoi(messages_[line].str.c_str() + i)));
					length += strlen(tempStr);
				}
				while (messages_[line].str[i] != ']') {
					i++;
				}
				break;
			case '$':
				length--;
				{
					char tempStr[32];
					sprintf(tempStr, "%d", gameSystem_.getInventory()->getMoney());
					length += strlen(tempStr);
				}
				break;
			case '!':
				length--;
				break;
			case '.':
				length--;
				break;
			case '|':
				length--;
				break;
			case '>':
				length--;
				break;
			case '<':
				length--;
				break;
			case '_':
				break;
			}
		}
	}
	return length;
}

