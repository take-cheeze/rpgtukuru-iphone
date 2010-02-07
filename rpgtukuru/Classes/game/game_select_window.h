/**
 * @file
 * @brief Select Window
 * @author takuto
 */
#pragma once

#include "game_window.h"


class GameSelectWindow : public GameWindow
{
public:
	static GameSelectWindow* createTask(kuto::Task* parent, const GameSystem& gameSystem) { return new GameSelectWindow(parent, gameSystem); }

protected:
	GameSelectWindow(kuto::Task* parent, const GameSystem& gameSystem);
	
	virtual void update();

	void renderText();
	void renderSelectCursor();

public:
	virtual void render();
	
	bool selected() const { return selected_; }
	bool canceled() const { return canceled_; }
	int cursor() const { return cursor_; }
	void setCursor(int value) { cursor_ = value; }
	void setPauseUpdateCursor(bool value) { pauseUpdateCursor_ = value; }
	void reset() { selected_ = false; canceled_ = false; }
	void resetCursor() { cursor_ = 0; scrollPosition_ = 0; }
	void setColumnSize(int value) { columnSize_ = value; }
	void setEnableCancel(bool value) { enableCancel_ = value; }
	void setAutoClose(bool value) { autoClose_ = value; }
	void setShowCursor(bool value) { showCursor_ = value; }
	void setFullSelect(bool value) { fullSelect_ = value; }
	bool isFullSelect() const { return fullSelect_; }
	void addMessage(const std::string& message, bool enable = true, int colorType = -1);

protected:
	int			cursor_;
	int			columnSize_;
	int			cursorAnimationCounter_;
	int			scrollPosition_;
	struct {
		bool		selected_			: 1;
		bool		canceled_			: 1;
		bool		pauseUpdateCursor_	: 1;
		bool		enableCancel_		: 1;
		bool		autoClose_			: 1;
		bool		showCursor_			: 1;
		bool		fullSelect_			: 1;
	};
	std::vector<bool>	itemEnables_;
};
