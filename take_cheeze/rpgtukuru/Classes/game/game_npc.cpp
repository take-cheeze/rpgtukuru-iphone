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


GameNpc::GameNpc(kuto::Task* parent, GameField* field, const EventPage& page)
: GameChara(parent, field)
, eventPage_(NULL), moveWaitCounter_(0)
{
	setEventPage(page);
}

void GameNpc::setEventPage(const EventPage& page)
{
	eventPage_ = &page;
	priority_ = (Priority)eventPage_->priority;
	crossover_ = !eventPage_->notCrossover;
	moveWaitMax_ = 300 / kuto::max(1, eventPage_->moveFrequency);
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
				switch (eventPage_->moveType) {
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
	switch (rand() % 4) {
	case 0:		move(GameChara::kDirUp);	break;
	case 1:		move(GameChara::kDirDown);	break;
	case 2:		move(GameChara::kDirLeft);	break;
	case 3:		move(GameChara::kDirRight);	break;
	}
}

void GameNpc::controlUpDown()
{
	if (direction_ == GameChara::kDirUp) {
		if (!move(GameChara::kDirUp)) {
			move(GameChara::kDirDown);
		}
	} else {
		if (!move(GameChara::kDirDown)) {
			move(GameChara::kDirUp);
		}
	}
}

void GameNpc::controlLeftRight()
{
	if (direction_ == GameChara::kDirLeft) {
		if (!move(GameChara::kDirLeft)) {
			move(GameChara::kDirRight);
		}
	} else {
		if (!move(GameChara::kDirRight)) {
			move(GameChara::kDirLeft);
		}
	}
}

