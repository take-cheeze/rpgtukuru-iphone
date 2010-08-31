#include "KeyListener.hpp"
#include "Main.hpp"


namespace rpg2k
{
	GameMode::GameMode(Main& m)
	: owner_(m), cursorCount_(0)
	{
	}
	GameMode::~GameMode()
	{
	}
	int& GameMode::cursor() { return cursor_.back().point; }
	int& GameMode::cursor(uint index) { return cursor_.at(index).point; }
	int& GameMode::cursorMax() { return cursor_.back().max; }
	int& GameMode::cursorMax(uint index) { return cursor_.at(index).max; }
	bool GameMode::cursorCountLeft()
	{
		if( cursorCount_ != 0 ) { --cursorCount_; return true; }
		else return false;
	}
	Vector2D GameMode::cursorMove(int size)
	{
		int ret = size * cursorCount_ / CURSOR_SPEED;
		switch( getOwner().getKeyListener().lastCursor() ) {
			case Key::DOWN : return Vector2D(0, -ret);
			case Key::RIGHT: return Vector2D(-ret, 0);
			case Key::UP   : return Vector2D(0,  ret);
			case Key::LEFT : return Vector2D( ret, 0);
			default: return Vector2D(0, 0);
		}
	}
	void GameMode::addCursor(int max, bool stop)
	{
		CursorInfo info = { 0, max, stop };
		cursor_.push_back( info );
	}
	void GameMode::removeLastCursor() { cursor_.pop_back(); }
	void GameMode::clearCursor()
	{
		cursor_.clear(); cursorCount_ = 0;
	}

	// true if change happened
	bool GameMode::incCursor()
	{
		CursorInfo& info = cursor_.back();
		++info.point;
		if(info.point < info.max) { cursorCount_ = CURSOR_SPEED; return true; }
		else if(info.stop) { info.point = info.max - 1; return false; }
		else { info.point = 0; cursorCount_ = CURSOR_SPEED; return true; }
	}
	bool GameMode::incCursor(int index)
	{
		CursorInfo& info = cursor_.at(index);
		++info.point;
		if(info.point < info.max) { cursorCount_ = CURSOR_SPEED; return true; }
		else if(info.stop) { info.point = info.max - 1; return false; }
		else { info.point = 0; cursorCount_ = CURSOR_SPEED; return true; }
	}
	// true if change happened
	bool GameMode::decCursor()
	{
		CursorInfo& info = cursor_.back();
		--info.point;
		if(info.point > -1) { cursorCount_ = CURSOR_SPEED; return true; }
		else if(info.stop) { info.point = 0; return false; }
		else { info.point = info.max - 1; cursorCount_ = CURSOR_SPEED; return true; }
	}
	bool GameMode::decCursor(int index)
	{
		CursorInfo& info = cursor_.at(index);
		--info.point;
		if(info.point > -1) { cursorCount_ = CURSOR_SPEED; return true; }
		else if(info.stop) { info.point = 0; return false; }
		else { info.point = info.max - 1; cursorCount_ = CURSOR_SPEED; return true; }
	}
} // namespace rpg2k
