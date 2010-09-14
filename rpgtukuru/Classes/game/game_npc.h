/**
 * @file
 * @brief NPC Class
 * @author project.kuto
 */
#pragma once

#include "game_chara.h"
#include <rpg2k/MapUnit.hpp>


class GameNpc : public GameChara, public kuto::TaskCreatorParam2<GameNpc, GameField*, rpg2k::structure::Array1D const&>
{
	friend class kuto::TaskCreatorParam2<GameNpc, GameField*, rpg2k::structure::Array1D const&>;
public:
	typedef rpg2k::structure::Array1D EventPage;

	void setEventPage(const rpg2k::structure::Array1D& page);
private:
	GameNpc(GameField* field, const rpg2k::structure::Array1D& page);
	virtual void update();
	void controlWait();
	void controlRandom();
	void controlUpDown();
	void controlLeftRight();
private:
	const EventPage*	eventPage_;
	int					moveWaitCounter_;
};
