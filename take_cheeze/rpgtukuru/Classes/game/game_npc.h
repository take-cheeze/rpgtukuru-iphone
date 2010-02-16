/**
 * @file
 * @brief NPC Class
 * @author project.kuto
 */
#pragma once

#include "game_chara.h"
#include <rpg2kLib/Project.hpp>


class GameNpc : public GameChara
{
public:
	// typedef MapUnit::MAPEVENT::MAPEVENT_PAGE EventPage;
	
	// static GameNpc* createTask(kuto::Task* parent, GameField* field, const EventPage& page) { return new GameNpc(parent, field, page); }
	// void setEventPage(const EventPage& page);
	
private:
	// GameNpc(kuto::Task* parent, GameField* field, const EventPage& page);
	virtual void update();
	void controlWait();
	void controlRandom();
	void controlUpDown();
	void controlLeftRight();
	
private:
	// const EventPage*	eventPage_;
	int					moveWaitCounter_;
};