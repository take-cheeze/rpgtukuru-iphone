/**
 * @file
 * @brief Window
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_static_vector.h>

#include <string>
#include <deque>

#include "game_system.h"


class GameWindow : public kuto::IRender2D
{
public:
	enum MessageAlign {
		kAlignLeft,
		kAlignRight,
		kAlignCenter,
	};
	enum State {
		kStateOpen,
		kStateLoop,
		kStateClose,
	};
	enum {
		kLineStringMax = 25,
		kLineStringFaceMax = 19,
	};
	struct MessageInfo {
		std::string		str;
		int				colorType;

		MessageInfo() : colorType(0) {}
		MessageInfo(const std::string& str, int colorType) : str(str), colorType(colorType) {}
	};
	typedef std::deque<MessageInfo> MessageList;

protected:
	GameWindow(const rpg2k::model::Project& gameSystem);

	virtual bool initialize();
	virtual void update();

	void renderFrame(kuto::Graphics2D* g) const;
	void renderFace(kuto::Graphics2D* g) const;
	void renderDownCursor(kuto::Graphics2D* g) const;
	void renderUpCursor(kuto::Graphics2D* g) const;
	void renderTextLine(kuto::Graphics2D* g, int line, int row, int columnMax, int count) const;

	void addMessageImpl(const std::string& message, int colorType = 0) { messages_.push_back(MessageInfo(message, colorType)); }

public:
	virtual void clearMessages() { messages_.clear(); }
	const MessageInfo& getMessage(int index) const { return messages_[index]; }
	uint getMessageSize() const { return messages_.size(); }
	uint getMessageLength() const;
	uint getMessageLineLength(int line) const;
	void setPosition(const kuto::Vector2& value) { position_ = value; }
	void setSize(const kuto::Vector2& value) { size_ = value; }
	void setFontSize(float value) { fontSize_ = value; }
	void setMessageAlign(MessageAlign align) { messageAlign_ = align; }
	void setPriority(float value); // { priority_ = value; }
	void setShowFrame(bool value) { showFrame_ = value; }
	State getState() const { return state_; }
	void open() { state_ = kStateOpen; }
	bool closed() const { return state_ == kStateClose; }
	void setFaceTexture(const std::string& filename, uint position, bool right, bool reverse);
	void setRowHeight(float value) { rowHeight_ = value; }
	void setLineSpace(float value) { lineSpace_ = value; }
	int getMaxRowSize() const { return (int)((size_.y - 16.f + lineSpace_) / (rowHeight_ + lineSpace_)); }

protected:
	const rpg2k::model::Project&						gameSystem_;
	kuto::Vector2							position_;
	kuto::Vector2							size_;
	MessageList								messages_;
	float									fontSize_;
	float									priority_;
	float									rowHeight_;
	float									lineSpace_;
	MessageAlign							messageAlign_;
	State									state_;
	kuto::Texture							faceTexture_;
	uint							facePosition_;
	bool		showFrame_			: 1;
	bool		faceEnable_			: 1;
	bool		faceRight_			: 1;
	bool		faceReverse_		: 1;
};
