/**
 * @file
 * @brief Game System Menu Base Class
 * @author takuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include "game_system.h"

class GameField;


class GameSystemMenuBase : public kuto::Task
{
public:
	enum State {
		kStateNone,
		kStateSystemMenuEnd,
		kStateCustom,
	};
	
protected:
	GameSystemMenuBase(GameField* gameField);
	virtual ~GameSystemMenuBase();

public:
	virtual void start() = 0;
	bool isEnd() const { return state_ == kStateNone || state_ == kStateSystemMenuEnd; }
	bool isSystemMenuEnd() const { return state_ == kStateSystemMenuEnd; }
	
protected:
	GameField*			gameField_;
	int					state_;
};
