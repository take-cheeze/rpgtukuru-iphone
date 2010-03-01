/**
 * @file
 * @brief Message Window
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_texture.h>
#include "game_window.h"


class GameMessageWindow : public GameWindow
{
public:
	static GameMessageWindow* createTask(kuto::Task* parent, const GameSystem& gameSystem) { return new GameMessageWindow(parent, gameSystem); }

private:
	GameMessageWindow(kuto::Task* parent, const GameSystem& gameSystem);
	
	virtual void update();
	void renderText(kuto::Graphics2D& g);

public:
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	bool clicked() const { return clicked_; }
	bool isEndAnimation() const { return animationEnd_; }
	void setUseAnimation(bool value) { useAnimation_ = value; }
	void setEnableSkip(bool value) { enableSkip_ = value; }
	void setEnableClick(bool value) { enableClick_ = value; }
	void reset();
	void setLineLimit(int value) { lineLimit_ = value; }
	int getLineLimit() const { return lineLimit_; }
	void addMessage(const std::string& message, int colorType = 0) { addMessageImpl(message, colorType); }
	
private:
	int				animationCounter_;
	int				lineLimit_;
	struct {
		bool		clicked_			: 1;
		bool		animationEnd_		: 1;
		bool		useAnimation_		: 1;
		bool		enableSkip_			: 1;
		bool		enableClick_		: 1;
	};
};
