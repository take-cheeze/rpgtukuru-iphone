/**
 * @file
 * @brief NPC Class
 * @author takuto
 */
#pragma once

#include "game_chara.h"
#include "CRpgLmu.h"


class GameNpc : public GameChara
{
public:
	typedef CRpgLmu::MAPEVENT::MAPEVENT_PAGE EventPage;
	
	static GameNpc* createTask(kuto::Task* parent, GameField* field, const EventPage& page) { return new GameNpc(parent, field, page); }
	void setEventPage(const EventPage& page);
	
private:
	GameNpc(kuto::Task* parent, GameField* field, const EventPage& page);
	virtual void update();
	void controlWait();
	void controlRandom();
	void controlUpDown();
	void controlLeftRight();
	
private:
	const EventPage*	eventPage_;
	int					moveWaitCounter_;
};
