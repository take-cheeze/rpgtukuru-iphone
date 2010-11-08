#include "game_event_manager.h"
#include "game_field.h"
#include "game_map.h"
#include "game_map_object.h"

#include <kuto/kuto_point2.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_virtual_pad.h>

#include <rpg2k/Project.hpp>

using rpg2k::model::MapUnit;
using rpg2k::model::Project;
using rpg2k::model::SaveData;
using rpg2k::structure::Array1D;
using rpg2k::structure::EventState;


namespace
{
	unsigned SPEED_FREQ[] =
	{
		0x1 << 8,
		0x1 << 7,
		0x1 << 6,
		0x1 << 5,
		0x1 << 4,
		0x1 << 3,
		0x1 << 2,
		0x1 << 1,
	};
	struct EventSpeed { enum Type {
		OCTO = 1,
		QAUT = 2,
		HALF = 3,
		NORMAL = 4,
		TWICE = 5,
		FOUR_TIMES = 6,
	}; };
} // namespace

GameParty::GameParty(GameMap& map)
: GameMapObject(map, rpg2k::ID_PARTY)
{
	setState( map.field().project().getLSD().party() );
	setSpeed(EventSpeed::NORMAL);
}

void GameParty::update()
{
	if(moveCounter_) {
		--moveCounter_;
		return;
	}

	if( owner().field().eventManager().isWaiting() ) return;

	kuto::VirtualPad& vpad = kuto::VirtualPad::instance();
	if( ( owner().count() % SPEED_FREQ[speed()] ) == 0 ) {
		#define PP_move(DIR) \
		if( vpad.press(kuto::VirtualPad::KEY_##DIR) ) { \
			GameMapObject::move(rpg2k::EventDir::DIR); \
		}
		PP_move(UP)
		PP_move(DOWN)
		PP_move(LEFT)
		PP_move(RIGHT)
		#undef PP_move
	}
}

GameMapObject::GameMapObject(GameMap& map, unsigned id)
: moveCounter_(0)
, owner_(map), eventID_(id)
, page_(NULL), state_(NULL)
{
}
kuto::Vector2 GameMapObject::correction() const
{
	kuto::Point2 p = GameMap::directionAdd( state_->eventDir() );
	kuto::Vector2 ret( float(p.x), float(p.y) );
	return (-ret) * float( SPEED_FREQ[speed_] ) / float(moveCounter_) * 16.f;
}
void GameMapObject::setPage(rpg2k::structure::Array1D const& p, rpg2k::structure::EventState& st)
{
	if(page_ != &p) {
		page_ = &p;
		pageNo_ = p.index();
		moveType_ = p[31];
		speed_ = p[32];

		state_ = &st;

		Array1D::const_iterator it = p.find(21);
		if( it != p.end() ) {
			st[73] = it->second->to_string(); // charset
		}
		st[74] = p[22].to<int>(); // charset pos
	}
}
bool GameMapObject::move(rpg2k::EventDir::Type dir)
{
	bool ret = owner_.move(eventID_, dir);
	if(ret) {
		moveCounter_ = SPEED_FREQ[speed()];
	}
	return ret;
}
void GameMapObject::update()
{
	if(moveCounter_) {
		--moveCounter_;
		return;
	}

	Project& proj = owner_.field().project();
	SaveData& lsd = proj.getLSD();

	if(
		!lsd.eventState().exists(eventID_) ||
		( owner_.field().eventManager().isWaiting() && !owner_.moveWhenWait() )
	) return;

	if( ( owner().count() % SPEED_FREQ[speed()] ) == 0 ) {
		switch(moveType_) {
		case 0: // none
			return;
		case 1: // random
			move( rpg2k::EventDir::Type( ( kuto::random(4) + 1 ) * 2 ) );
			break;
		case 2: // vertical
			switch( state_->eventDir() ) {
				case rpg2k::CharSet::Dir::UP:
				case rpg2k::CharSet::Dir::DOWN:
					break;
				default:
					(*state_)[21] = int(rpg2k::CharSet::Dir::UP);
					break;
			}
			if( !move( rpg2k::toEventDir( state_->eventDir() ) ) ) {
				switch( state_->eventDir() ) {
					case rpg2k::CharSet::Dir::UP: (*state_)[21] = int(rpg2k::CharSet::Dir::DOWN);
					case rpg2k::CharSet::Dir::DOWN: (*state_)[21] = int(rpg2k::CharSet::Dir::UP);
					default: kuto_assert(false); break;
				}
			}
			break;
		case 3: // horizontal
			switch( state_->eventDir() ) {
				case rpg2k::CharSet::Dir::LEFT:
				case rpg2k::CharSet::Dir::RIGHT:
					break;
				default:
					(*state_)[21] = int(rpg2k::CharSet::Dir::RIGHT);
					break;
			}
			if( !move( rpg2k::toEventDir( state_->eventDir() ) ) ) {
				switch( state_->eventDir() ) {
					case rpg2k::CharSet::Dir::LEFT: (*state_)[21] = int(rpg2k::CharSet::Dir::RIGHT);
					case rpg2k::CharSet::Dir::RIGHT: (*state_)[21] = int(rpg2k::CharSet::Dir::LEFT);
					default: kuto_assert(false); break;
				}
			}
			break;
		case 4: // to party
		case 5: // from party
		{
			EventState& party = owner_.party();
			kuto::Point2 partyPos( party.x(), party.y() ), evPos( state_->x(), state_->y() );
			#define PP_distance(INDEX) \
				distance( partyPos, evPos + GameMap::directionAdd( rpg2k::EventDir::Type( (1 + INDEX) * 2 ) ) )
			unsigned range[4] = { PP_distance(0), PP_distance(1), PP_distance(2), PP_distance(3), };
			#undef PP_distance

			rpg2k::EventDir::Type t = rpg2k::EventDir::Type(
				(moveType_ == 4)? ( range - std::min_element(range, range + 4) + 1 ) * 2 :
				(moveType_ == 5)? ( range - std::max_element(range, range + 4) + 1 ) * 2 :
				0
			);
			move(t);
		} break;
		case 6: // manual
			// TODO
			break;
		}
	}
}

rpg2k::structure::EventState& GameMapObject::state()
{
	return *state_;
}
rpg2k::structure::Array1D const& GameMapObject::page() const
{
	return *page_;
}
rpg2k::structure::EventState const& GameMapObject::state() const
{
	return *state_;
}
