#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_error.h>

#include <rpg2k/Stream.hpp>

class GameMap;
namespace rpg2k { namespace structure {
	class Array1D;
	class EventState;
} }
namespace kuto { class Vector2; }


class GameMapObject : public kuto::Task, public kuto::TaskCreatorParam2<GameMapObject, GameMap&, unsigned>
{
	friend class kuto::TaskCreatorParam2<GameMapObject, GameMap&, unsigned>;
protected:
	GameMapObject(GameMap& map, unsigned id);

public:
	void setPage(rpg2k::structure::Array1D const& p, rpg2k::structure::EventState& st);
	rpg2k::structure::Array1D const& page() const;
	rpg2k::structure::EventState& state();
	rpg2k::structure::EventState const& state() const;

	kuto::Vector2 correction() const;

private:
	virtual void update();

protected:
	GameMap& owner() { return owner_; }

	bool move(rpg2k::EventDir::Type dir);
	void setState(rpg2k::structure::EventState& st) { state_  = &st; }
	unsigned speed() const { return speed_; }
	void setSpeed(unsigned const sp) { speed_ = sp; }

	unsigned moveCounter_;

private:
	GameMap& owner_;
	unsigned const eventID_;

	rpg2k::structure::Array1D const* page_;
	unsigned pageNo_, moveType_, speed_;
	rpg2k::structure::EventState* state_;
}; // class GameMapObject

class GameParty : public GameMapObject, public kuto::TaskCreatorParam1<GameParty, GameMap&>
{
	friend class kuto::TaskCreatorParam1<GameParty, GameMap&>;
protected:
	GameParty(GameMap& map);

	using GameMapObject::page;
	using GameMapObject::setPage;

private:
	virtual void update();
}; // class GameParty
