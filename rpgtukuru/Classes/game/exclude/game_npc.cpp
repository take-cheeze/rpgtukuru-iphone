/**
 * @file
 * @brief NPC Class
 * @author project.kuto
 */

#include "game_npc.h"
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>
#include "game_field.h"
#include "game_player.h"


GameNpc::GameNpc(GameField* field, const EventPage& page)
: GameChara(field)
, eventPage_(NULL), moveWaitCounter_(0)
{
	setEventPage(page);
}

void GameNpc::setEventPage(const EventPage& page)
{
	eventPage_ = &page;
	priority_ = rpg2k::EventPriority::Type((*eventPage_)[34].get<int>());
	crossover_ = !(*eventPage_)[35].get<bool>();
	moveWaitMax_ = 300 / kuto::max(1, (*eventPage_)[37].get<int>());
}

void GameNpc::update()
{
	moveResult_ = kMoveResultNone;
	if (talking_) {
		moveWaitCounter_ = 0;
	} else if (isEnableRoute()) {
		controlRoute();
	} else {
		moveWaitCounter_++;
		if (moveWaitCounter_ > moveWaitMax_) {
			moveWaitCounter_ = 0;
			if (isEnableRoute()) {
				controlRoute();
			} else {
				switch ( (*eventPage_)[32].get<int>() ) {
				case 0:		controlWait();	break;
				case 1:		controlRandom();	break;
				case 2:		controlUpDown();	break;
				case 3:		controlLeftRight();	break;
				case 4:		controlApproach();	break;
				case 5:		controlEscape();	break;
				case 6:		controlRoute();	break;
				}
			}
		}
	}
	GameChara::update();
}

void GameNpc::controlWait()
{
}

void GameNpc::controlRandom()
{
	switch ( kuto::random(4) ) {
	case 0:		move(rpg2k::EventDir::UP);	break;
	case 1:		move(rpg2k::EventDir::DOWN);	break;
	case 2:		move(rpg2k::EventDir::LEFT);	break;
	case 3:		move(rpg2k::EventDir::RIGHT);	break;
	}
}

void GameNpc::controlUpDown()
{
	if (direction_ == rpg2k::EventDir::UP) {
		if (!move(rpg2k::EventDir::UP)) {
			move(rpg2k::EventDir::DOWN);
		}
	} else {
		if (!move(rpg2k::EventDir::DOWN)) {
			move(rpg2k::EventDir::UP);
		}
	}
}

void GameNpc::controlLeftRight()
{
	if (direction_ == rpg2k::EventDir::LEFT) {
		if (!move(rpg2k::EventDir::LEFT)) {
			move(rpg2k::EventDir::RIGHT);
		}
	} else {
		if (!move(rpg2k::EventDir::RIGHT)) {
			move(rpg2k::EventDir::LEFT);
		}
	}
}

