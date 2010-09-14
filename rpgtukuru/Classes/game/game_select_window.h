/**
 * @file
 * @brief Select Window
 * @author project.kuto
 */
#pragma once

#include "game_window.h"


class GameSelectWindow : public GameWindow, public kuto::TaskCreatorParam1<GameSelectWindow, const rpg2k::model::Project&>
{
	friend class kuto::TaskCreatorParam1<GameSelectWindow, const rpg2k::model::Project&>;
protected:
	GameSelectWindow(const rpg2k::model::Project& gameSystem);

	virtual void update();

	void renderText(kuto::Graphics2D* g) const;
	void renderSelectCursor(kuto::Graphics2D* g) const;

public:
	virtual void render(kuto::Graphics2D* g) const;

	bool selected() const { return selected_; }
	bool canceled() const { return canceled_; }
	int cursor() const { return cursor_; }
	void setCursor(int value) { cursor_ = value; }
	void setPauseUpdateCursor(bool value) { pauseUpdateCursor_ = value; }
	void reset() { selected_ = false; canceled_ = false; cursorStart_ = 0; }
	void resetCursor() { cursor_ = 0; scrollPosition_ = 0; }
	void setColumnSize(int value) { columnSize_ = value; }
	void setEnableCancel(bool value) { enableCancel_ = value; }
	void setAutoClose(bool value) { autoClose_ = value; }
	void setShowCursor(bool value) { showCursor_ = value; }
	void setFullSelect(bool value) { fullSelect_ = value; }
	bool isFullSelect() const { return fullSelect_; }
	void addLine(const std::string& message, bool enable = true, int colorType = -1);
	void setCursorStart(int value) { cursorStart_ = value; cursor_ = cursorStart_; }
	virtual void clearMessages() { GameWindow::clearMessages(); itemEnables_.clear(); }

protected:
	int			cursor_;
	int			columnSize_;
	int			cursorAnimationCounter_;
	int			scrollPosition_;
	int			cursorStart_;
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
