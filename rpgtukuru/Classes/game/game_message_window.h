/**
 * @file
 * @brief Message Window
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_texture.h>
#include "game_window.h"


class GameMessageWindow : public GameWindow, public kuto::TaskCreatorParam1<GameMessageWindow, Game&>
{
	friend class kuto::TaskCreatorParam1<GameMessageWindow, Game&>;
private:
	GameMessageWindow(Game& g);

	virtual void update();
	void renderText(kuto::Graphics2D& g) const;

public:
	virtual void render(kuto::Graphics2D& g) const;
	bool clicked() const { return clicked_; }
	bool isEndAnimation() const { return animationEnd_; }
	void useAnimation(bool value = true) { useAnimation_ = value; }
	void enableSkip(bool value = true) { enableSkip_ = value; }
	void enableClick(bool value = true) { enableClick_ = value; }
	void reset();
	void setLineLimit(int value) { lineLimit_ = value; }
	int lineLimit() const { return lineLimit_; }
	void addLine(const std::string& message, int colorType = 0) { addMessageImpl(message, colorType); }

private:
	int				animationCounter_;
	int				lineLimit_;
	struct {
		bool		clicked_			: 1;
		bool		animationEnd_		: 1;
		bool		useAnimation_		: 1;
		bool		enableSkip_			: 1;
		bool		enableClick_		: 1;
		bool		renderMoneyWindow_	: 1;
	};
};
