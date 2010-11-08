/**
 * @file
 * @brief Game Map
 * @author project.kuto
 */

#include "game.h"
#include "game_field.h"
#include "game_map.h"
#include "game_map_object.h"

#include <kuto/kuto_error.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_virtual_pad.h>

#include <rpg2k/Project.hpp>

#include <boost/array.hpp>

#include <utility>

using rpg2k::model::MapUnit;
using rpg2k::model::Project;
using rpg2k::model::SaveData;
using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;
using rpg2k::structure::EventState;


kuto::Point2 GameMap::directionAdd(rpg2k::EventDir::Type const t)
{
	switch(t) {
		case rpg2k::EventDir::DOWN : return kuto::Point2( 0,  1);
		case rpg2k::EventDir::LEFT : return kuto::Point2(-1,  0);
		case rpg2k::EventDir::RIGHT: return kuto::Point2( 1,  0);
		case rpg2k::EventDir::UP   : return kuto::Point2( 0, -1);
		default: kuto_assert(false); return kuto::Point2();
	}
}
kuto::Point2 GameMap::directionAdd(rpg2k::CharSet::Dir::Type const t)
{
	switch(t) {
		case rpg2k::CharSet::Dir::DOWN : return kuto::Point2( 0,  1);
		case rpg2k::CharSet::Dir::LEFT : return kuto::Point2(-1,  0);
		case rpg2k::CharSet::Dir::RIGHT: return kuto::Point2( 1,  0);
		case rpg2k::CharSet::Dir::UP   : return kuto::Point2( 0, -1);
		default: kuto_assert(false); return kuto::Point2();
	}
}

namespace
{
	typedef std::multimap<unsigned, unsigned>::const_iterator x_it;
	typedef std::vector< std::multimap<unsigned, unsigned> >::const_iterator y_it;
	typedef std::vector< std::vector< std::multimap<unsigned, unsigned> > >::const_iterator priority_it;

	bool eventExists(std::vector< std::multimap<unsigned, unsigned> > const& target, kuto::Point2 const& pos)
	{
		std::multimap<unsigned, unsigned> const& line = target[pos.y];
		return( line.find(pos.x) != line.end() );
	}

	kuto::Vector2 const
		SCREEN_SIZE(320.f, 240.f), CHIP_SIZE(16.f, 16.f), CHAR_SIZE(24.f, 32.f);
	kuto::Point2 const CHIP_NUM(SCREEN_SIZE.x / CHIP_SIZE.x, SCREEN_SIZE.y / CHIP_SIZE.y);
} // namespace

void GameMap::updateCache()
{
	cache_.project = &field_.project();
	cache_.ldb = &cache_.project->getLDB();
	cache_.lsd = &cache_.project->getLSD();
	cache_.lmu = &cache_.project->getLMU();
	unsigned const chipSetID = cache_.project->chipSetID();
	cache_.upperChipFlag = &cache_.ldb->upperChipFlag(chipSetID);
	cache_.lowerChipFlag = &cache_.ldb->lowerChipFlag(chipSetID);
	cache_.terrain = &cache_.ldb->terrain(chipSetID);
	cache_.moveWhenWait = cache_.lsd->system()[44];
	cache_.party = &cache_.lsd->party();
	cache_.mapSize.set( cache_.lmu->width(), cache_.lmu->height() );
	cache_.scrollFlag = (*cache_.lmu)[11];
	cache_.panorama = cache_.project->panorama();
}

GameMap::GameMap(GameField& field)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 9.f)
, field_(field)
, animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, enableScroll_(false), scrolled_(false), scrollRatio_(1.f)
, panoramaAutoScrollOffset_(0.f, 0.f)
, counter_(0), justMoved_(false)
, partyObj_( *addChild( kuto::TaskCreatorParam1<GameParty, GameMap&>::createTask(*this) ) )
{
	updateCache();
}

void GameMap::moveMap(unsigned const mapID, unsigned const x, unsigned const y)
{
	for(std::vector<GameMapObject*>::iterator i =objects_.begin(); i < objects_.end(); ++i) {
		if(*i) (*i)->release();
	}
	Array2D& events = field_.project().getLMU().event();
	objects_.clear();
	objects_.resize( events.rbegin()->first + 1, NULL );
	for(Array2D::Iterator it = events.begin(); it != events.end(); ++it) {
		if( !it->second->exists() ) continue;

		objects_[it->first] = addChild( GameMapObject::createTask( *this, it->first ) );
	}
	pageNo_.resize( objects_.size() );

	updateCache();

	justMoved_ = true;
}

rpg2k::structure::Array1D const& GameMap::page(unsigned const evID) const
{
	return objects_[evID]->page();
}

bool GameMap::initialize()
{
	return isInitializedChildren();
}

void GameMap::update()
{
	updateCache();

	counter_++;
	justMoved_ = false;

	if( objects_.empty() ) return;

	Project& proj = *cache_.project;
	MapUnit& lmu = *cache_.lmu;
	SaveData& lsd = *cache_.lsd;

	eventMap_.clear();
	eventMap_.resize( rpg2k::EventPriority::END, std::vector< std::multimap<unsigned, unsigned> >( lmu.height() ) );
	std::fill_n( pageNo_.begin(), pageNo_.size(), 0 );

	while( !touchFromEvent_.empty() ) touchFromEvent_.pop();
	while( !touchFromParty_.empty() ) touchFromParty_.pop();
	while( !keyEnter_.empty() ) keyEnter_.pop();

	Array2D& eventStates = lsd.eventState();
	Array2D& mapEvents = lmu[81];
// mapping events
	for(Array2D::Iterator it = eventStates.begin(); it != eventStates.end(); ++it) {
		if( !it->second->exists() ) continue;

		int const evID = it->first;
		Array1D& event = mapEvents[evID];
		EventState& state = reinterpret_cast<EventState&>( *it->second );

		Array1D const* page = proj.currentPage(event[5]);
		objects_[evID]->pauseUpdate(page == NULL);
		if(page == NULL) continue;

		unsigned const pageID = page->index();
		objects_[evID]->setPage(*page, state);

		pageNo_[evID] = pageID;

		int x = state.exists(12)? state.x() : event[2];
		int y = state.exists(13)? state.y() : event[3];

		eventMap_[(*page)[34].to<int>()][y].insert( std::make_pair(x, evID) );
	}
// mapping non-events
	for(uint i = rpg2k::ID_PARTY; i <= rpg2k::ID_AIRSHIP; i++) {
		EventState& state = lsd.eventState(i);

		if( state.mapID() != cache_.party->mapID() ) {
			continue;
		}

		int x = state.x(), y = state.y();

		eventMap_[rpg2k::EventPriority::CHAR][y].insert( std::make_pair(x, i) );

		if(i == rpg2k::ID_PARTY) {
		}
	}

	animationCounter_++;
	if (scrollRatio_ < 1.f) {
		scrollRatio_ = kuto::min(1.f, scrollRatio_ + scrollSpeed_);
		screenOffset_ = kuto::lerp(scrollBase_, scrollOffset_, scrollRatio_);
	}
	if( !cache_.panorama.empty() ) {
		if(proj.getLMU()[35].to<bool>()) panoramaAutoScrollOffset_.x += proj.getLMU()[36].to<int>();
		if(proj.getLMU()[37].to<bool>()) panoramaAutoScrollOffset_.y += proj.getLMU()[38].to<int>();
	}

	if( kuto::VirtualPad::instance().press(kuto::VirtualPad::KEY_A) ) {
		// TODO: counter attribute
		kuto::Point2 const cur( party().x(), party().y() );
		kuto::Point2 const nxt = cur + directionAdd( party().eventDir() );
		#define PP_check(PRIORITY, POINT) \
			for( \
				x_it it = eventMap_[PRIORITY][POINT.y].find(POINT.x); \
				( it != eventMap_[PRIORITY][POINT.y].end() ) && ( it->first == unsigned(POINT.x) ); ++it \
			) { \
				if( \
					rpg2k::isEvent(it->second) && \
					page(it->second)[33].to<int>() == rpg2k::EventStart::KEY_ENTER \
				) keyEnter_.push(it->second); \
			}
		PP_check(rpg2k::EventPriority::BELOW, cur)
		PP_check(rpg2k::EventPriority::CHAR , nxt)
		PP_check(rpg2k::EventPriority::ABOVE, cur)
		#undef PP_check
	}
}

void GameMap::render(kuto::Graphics2D& g) const
{
	if(justMoved_) return;

	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(CHIP_SIZE * screenScale_);

	rpg2k::model::Project& proj = *cache_.project;
	rpg2k::model::MapUnit& lmu = *cache_.lmu;
// Panorama
	{
		if( !cache_.panorama.empty() ) {
			// TODO: draw only SCREEN_SIZE. some panorama is bigger than the SCREEN_SIZE.
			kuto::Texture& tex = field_.game().texPool().get(GameTexturePool::Panorama, cache_.panorama);
			kuto::Vector2 pos = screenOffset_;
			if (lmu[35].to<bool>()) pos.x += panoramaAutoScrollOffset_.x;
			if (lmu[37].to<bool>()) pos.y += panoramaAutoScrollOffset_.y;
			kuto::Vector2 const scale( tex.width(), tex.height() );
			g.drawTexture(tex, kuto::Vector2(0.f, 0.f), SCREEN_SIZE, color
			, pos / scale, (pos + SCREEN_SIZE) / scale, true);
		}
	}
// chip and events
	kuto::Point2 baseP = kuto::Point2( party().x(), party().y() ) - CHIP_NUM / 2;
	kuto::Point2 const mapS( lmu.width(), lmu.height() );
	kuto::Vector2 const baseVec(0.f, 0.f);


	#define PP_check(MASK, DIM) \
		if( !(cache_.scrollFlag & MASK) ) { \
			if( mapS.DIM <= (baseP.DIM + CHIP_NUM.DIM) ) { baseP.DIM = mapS.DIM - CHIP_NUM.DIM; } \
			else if(baseP.DIM < 0) { baseP.DIM = 0; } \
		} else { baseP.DIM += mapS.DIM; }
	PP_check(0x02, x) // horizontal
	PP_check(0x01, y) // vertical
	#undef PP_check

	kuto::Texture const& chipSetTex = field_.game().texPool().get(
		GameTexturePool::ChipSet, proj.chipSet()[2].to_string().toSystem() );

	kuto::Point2 it;
	for(it.y = 0; it.y < CHIP_NUM.y; it.y++) {
		bool aboveLw[32], aboveUp[32];
		int lw[32], up[32];

		// below chips
		for(it.x = 0; it.x < CHIP_NUM.x; it.x++) {
			kuto::Point2 const cur = (baseP + it) % mapS;
			up[it.x] = lmu.chipIDUp(cur.x, cur.y);
			lw[it.x] = lmu.chipIDLw(cur.x, cur.y);
			aboveUp[it.x] = isAbove(up[it.x]);
			aboveLw[it.x] = isAbove(lw[it.x]);

			kuto::Vector2 const itVec( float(it.x), float(it.y) );
			if( !aboveLw[it.x] ) { drawChip(g, chipSetTex, CHIP_SIZE*itVec + baseVec, lw[it.x]); }
			if( !aboveUp[it.x] ) { drawChip(g, chipSetTex, CHIP_SIZE*itVec + baseVec, up[it.x]); }
		}

		// event graphics
		for(priority_it prIt = eventMap_.begin(); prIt != eventMap_.end(); ++prIt) {
			std::multimap<unsigned, unsigned> const& base = (*prIt)[ (baseP.y + it.y) % mapS.y ];

			boost::array< std::pair<unsigned, unsigned>, 2 > range = {
				std::make_pair( baseP.x % mapS.x, (baseP.x + CHIP_NUM.x) % mapS.x ),
				std::make_pair(0, 0),
			};
			if(range[0].first > range[0].second) {
				range[1].second = range[0].second;
				range[0].second = mapS.x;
			}

			for(unsigned r = 0; r < range.size(); r++) {
				if( range[r].first == range[r].second ) continue;

				x_it x = base.begin();
				while( ( x != base.end() ) && ( x->first < unsigned(range[r].first) ) ) { ++x; }
				for(; x != base.end(); ++x) {
					if( range[r].second <= x->first ) break;

					static kuto::Vector2 CHAR_SET_OFFSET(
						(CHAR_SIZE.x-CHIP_SIZE.x)/2, CHAR_SIZE.y - CHIP_SIZE.y );

					if( rpg2k::isEvent(x->second) ) {
						GameMapObject& ob = *objects_[x->second];
						if( ob.isPauseUpdate() ) continue;
						EventState& state = ob.state();
						it.x = x->first - range[r].first;

						kuto::Vector2 const itVec( float(it.x), float(it.y) );
						if( !state.exists(73) || !state[73].to_string().empty() ) {
							// TODO: CharSet pattern, animation type
							drawChar(g, state.charSet(), state.charSetPos()
							, state.talkDir(), rpg2k::CharSet::Pat::MIDDLE
							, CHIP_SIZE*itVec + baseVec - CHAR_SET_OFFSET + ob.correction() );
						} else {
							drawChip(g, chipSetTex
							, CHIP_SIZE*itVec + baseVec + ob.correction()
							, 10000 + state.charSetPos() );
						}
					} else {
						EventState const& state = cache_.lsd->eventState(x->second);
						kuto::Vector2 const itVec( float(it.x), float(it.y) );

						if( state.exists(73) && !state[73].to_string().empty() ) {
							drawChar(g, state.charSet(), state.charSetPos()
							, state.talkDir(), rpg2k::CharSet::Pat::MIDDLE
							, CHIP_SIZE*itVec + baseVec - CHAR_SET_OFFSET + partyObj_.correction() );
						}
					}
				}
			}
		}
		// above chips
		for(it.x = 0; it.x < CHIP_NUM.x; it.x++) {
			kuto::Vector2 const itVec( float(it.x), float(it.y) );
			if( aboveLw[it.x] ) { drawChip( g, chipSetTex, CHIP_SIZE*itVec + baseVec, lw[it.x] ); }
			if( aboveUp[it.x] ) { drawChip( g, chipSetTex, CHIP_SIZE*itVec + baseVec, up[it.x] ); }
		}
	}
}

int GameMap::chipFlag(int x, int y, bool upper) const
{
	rpg2k::structure::Array1D& chipSet = cache_.project->chipSet();
	int chipId = upper? cache_.lmu->chipIDUp(x, y) : cache_.lmu->chipIDLw(x, y);
	return upper? chipSet[5].toBinary()[chipId] : chipSet[4].toBinary()[chipId];
}

bool GameMap::isCounter(int x, int y) const
{
	return field_.project().isCounter( cache_.lmu->chipIDUp(x, y) );
}


void GameMap::setPlayerPosition(const kuto::Vector2& pos)
{
	rpg2k::model::MapUnit& lmu = *cache_.lmu;
	kuto::Vector2 const mapSize( cache_.mapSize.x * 16.f, cache_.mapSize.y * 16.f );
	if (pos.x < (320.f) * 0.5f)
		screenOffsetBase_.x = 0.f;
	else if (pos.x > mapSize.x - (320.f) * 0.5f)
		screenOffsetBase_.x = (320.f) - mapSize.x;
	else
		screenOffsetBase_.x = (320.f) * 0.5f - pos.x;
	if (pos.y < (240.f) * 0.5f)
		screenOffsetBase_.y = 0.f;
	else if (pos.y > mapSize.y - (240.f) * 0.5f)
		screenOffsetBase_.y = (240.f) - mapSize.y;
	else
		screenOffsetBase_.y = (240.f) * 0.5f - pos.y;
	if (!enableScroll_ || isScrolling() || scrolled_)
		return;
	screenOffset_ = screenOffsetBase_;
}

int GameMap::terrainID(int x, int y) const
{
	return terrainID( cache_.lmu->chipIDLw(x, y) );
}

void GameMap::scroll(int x, int y, float speed)
{
	scrollOffset_.x = -x * 16.f;
	scrollOffset_.y = -y * 16.f;
	scrollBase_ = screenOffset_;
	scrollOffset_ += screenOffset_;
	scrollSpeed_ = speed / (std::abs(scrollOffset_.x - scrollBase_.x) + std::abs(scrollOffset_.y - scrollBase_.y));
	scrollRatio_ = 0.f;
	scrolled_ = true;
}

void GameMap::scrollBack(float speed)
{
	scrollOffset_ = screenOffsetBase_;
	scrollBase_ = screenOffset_;
	scrollSpeed_ = speed / (std::abs(scrollOffset_.x - scrollBase_.x) + std::abs(scrollOffset_.y - scrollBase_.y));
	scrollRatio_ = 0.f;
	scrolled_ = false;
}

bool GameMap::canPass(unsigned evID, rpg2k::EventDir::Type dir) const
{
	MapUnit& lmu = *cache_.lmu;

	EventState& state = objects_[evID]->state();
	state[21] = state[22] = (int) dir;

	kuto::Point2 const cur( state.x(), state.y() );
	kuto::Point2 nxt = cur + directionAdd(dir);

// horizontal
	if(cache_.scrollFlag & 0x02) {
		if(nxt.x < 0) nxt.x = cache_.mapSize.x- nxt.x;
		else if(cache_.mapSize.x <= nxt.x) nxt.x = 0;
	} else if( (nxt.x < 0) || (cache_.mapSize.x <= nxt.x) ) return false;
// vertical
	if(cache_.scrollFlag & 0x01) {
		if(nxt.y < 0) nxt.y = cache_.mapSize.y - nxt.y;
		else if(cache_.mapSize.y <= nxt.y) nxt.y = 0;
	} else if( (nxt.y < 0) || (cache_.mapSize.y <= nxt.y) ) return false;

	return( canPassMap(dir, cur, nxt) && canPassEvent(evID, dir, cur, nxt) );
}
void GameMap::move(unsigned const evID, kuto::Point2 const dst)
{
	EventState& state = objects_[evID]->state();
	kuto::Point2 const src( state.x(), state.y() );

	unsigned pr;
	if( !rpg2k::isEvent(evID) ) pr = rpg2k::EventPriority::CHAR;
	else pr = page(evID)[34].to<int>();
	// remove previous place
	std::multimap<unsigned, unsigned>::iterator it = eventMap_[pr][src.y].find(src.x);
	while(it->second != evID) ++it;
	eventMap_[pr][src.y].erase(it);
	// reset next place
	eventMap_[pr][dst.y].insert( std::make_pair(dst.x, evID) );
	state[12] = dst.x; state[13] = dst.y;
}
bool GameMap::move(unsigned const evID, rpg2k::EventDir::Type const dir)
{
	MapUnit& lmu = *cache_.lmu;

	EventState& state = objects_[evID]->state();
	state[21] = state[22] = (int) dir;

	kuto::Point2 const cur( state.x(), state.y() );
	kuto::Point2 nxt = cur + directionAdd(dir);

// horizontal
	if(cache_.scrollFlag & 0x02) {
		if(nxt.x == -1) nxt.x = cache_.mapSize.x- 1;
		else if(cache_.mapSize.x <= nxt.x) nxt.x = 0;
	} else if( (nxt.x < 0) || (cache_.mapSize.x <= nxt.x) ) return false;
// vertical
	if(cache_.scrollFlag & 0x01) {
		if(nxt.y == -1) nxt.y = cache_.mapSize.y - 1;
		else if(cache_.mapSize.y <= nxt.y) nxt.y = 0;
	} else if( (nxt.y < 0) || (cache_.mapSize.y <= nxt.y) ) return false;

	if( !canPassMap(dir, cur, nxt) || !canPassEvent(evID, dir, cur, nxt, true) ) return false;

	move(evID, nxt);

	return true;
}
bool GameMap::canPassMap(rpg2k::EventDir::Type const dir, kuto::Point2 const& cur, kuto::Point2 const& nxt) const
{
	MapUnit& lmu = *cache_.lmu;

	int shiftNum = dir / 2 - 1;

	int const curLw = lmu.chipIDLw(cur.x, cur.y);
	int const curUp = lmu.chipIDUp(cur.x, cur.y);
	int const nxtLw = lmu.chipIDLw(nxt.x, nxt.y);
	int const nxtUp = lmu.chipIDUp(nxt.x, nxt.y);

	uint8_t curMsk = 0x01 << shiftNum, nxtMsk = 0x08 >> shiftNum;

// with map chip level
	if( ( pass(curUp) & curMsk ) && ( pass(nxtUp) & nxtMsk ) ) {
		bool curUpAb = isAbove(curUp);
		bool nxtUpAb = isAbove(nxtUp);

		if(
			( ( curUpAb && ( pass(curLw) & curMsk ) ) || !curUpAb ) &&
			( ( nxtUpAb && ( pass(nxtLw) & nxtMsk ) ) || !nxtUpAb )
		) return true;
	}

	return false;
}
bool GameMap::canPassEvent(unsigned const evID, rpg2k::EventDir::Type const dir
, kuto::Point2 const& cur, kuto::Point2 const& nxt) const
{
// priority
	unsigned pr;
	if( !rpg2k::isEvent(evID) ) pr = rpg2k::EventPriority::CHAR;
	else pr = page(evID)[34].to<int>();
// char
	if( (pr == rpg2k::EventPriority::CHAR) && eventExists(eventMap_[pr], nxt) ) {
		return false;
	}
// non piled event
	if( rpg2k::isEvent(evID) )
	for(int i = rpg2k::EventPriority::BEGIN; i < rpg2k::EventPriority::END; i++) {
		for(
			x_it it = eventMap_[i][nxt.y].find(nxt.x);
			( it != eventMap_[i][nxt.y].end() ) && ( it->first == unsigned(nxt.x) ); ++it
		) {
			if( page(it->second)[35].to<bool>() ) return false;
		}
	}

	return true;
}
bool GameMap::canPassEvent(unsigned const evID, rpg2k::EventDir::Type const dir
, kuto::Point2 const& cur, kuto::Point2 const& nxt, bool const collision)
{
// priority
	unsigned pr;
	if( rpg2k::isEvent(evID) ) { pr = page(evID)[34].to<int>(); }
	else { pr = rpg2k::EventPriority::CHAR; }
// char
	if(pr == rpg2k::EventPriority::CHAR) {
		std::multimap<unsigned, unsigned> target = eventMap_[rpg2k::EventPriority::CHAR][nxt.y];
		if(collision) for(
			x_it it = target.find(nxt.x);
			( it != target.end() ) && ( it->first == unsigned(nxt.x) ); ++it
		) {
			Array1D const& page = this->page(it->second);
			if(
				(it->second == rpg2k::ID_PARTY) &&
				( page[33].to<int>() == rpg2k::EventStart::EVENT_TOUCH )
			) { touchFromEvent_.push(evID); }
			else if(
				(evID == rpg2k::ID_PARTY) &&
				( page[33].to<int>() == rpg2k::EventStart::PARTY_TOUCH )
			) { touchFromParty_.push(it->second); }
		}

		if( target.find(nxt.x) != target.end() ) { return false; }
	} else if( collision && (evID == rpg2k::ID_PARTY) ) {
		#define PP_check(PRIORITY) \
			for( \
				x_it it = eventMap_[PRIORITY][nxt.y].find(nxt.x); \
				( it != eventMap_[PRIORITY][nxt.y].end() ) && ( it->first == unsigned(nxt.x) ); ++it \
			) { \
				Array1D const& page = this->page(it->second); \
				if( \
					eventExists(eventMap_[PRIORITY], nxt) && \
					( page[33].to<int>() == rpg2k::EventStart::PARTY_TOUCH ) \
				) { touchFromParty_.push(it->second); } \
			}
		PP_check(rpg2k::EventPriority::BELOW)
		PP_check(rpg2k::EventPriority::ABOVE)
		#undef PP_check
	}
// non piled event
	if( rpg2k::isEvent(evID) ) {
		for(int i = rpg2k::EventPriority::BEGIN; i < rpg2k::EventPriority::END; i++) {
			for(
				x_it it = eventMap_[i][nxt.y].find(nxt.x);
				( it != eventMap_[i][nxt.y].end() ) && ( it->first == unsigned(nxt.x) ); ++it
			) { if( page(it->second)[35].to<bool>() ) return false; }
		}
	}

	return true;
}

void GameMap::drawChar(kuto::Graphics2D& g
, rpg2k::RPG2kString const& name
, unsigned const pos
, rpg2k::CharSet::Dir::Type const dir
, rpg2k::CharSet::Pat::Type const pat
, kuto::Vector2 const& dstP) const
{
	kuto::Texture const& tex = field_.game().texPool().get(
		GameTexturePool::CharSet, name.toSystem() );
	kuto::Vector2 const srcP(
		float( (pos % 4) * rpg2k::CharSet::Pat::END + pat ),
		float( (pos / 4) * rpg2k::CharSet::Dir::END + dir ) );
	kuto::Vector2 const scale( tex.width(), tex.height() );
	kuto::Color const color(1.f, 1.f, 1.f, 1.f);

	g.drawTexture(tex, dstP, CHAR_SIZE, color
	, srcP * CHAR_SIZE / scale, (srcP+1.f) * CHAR_SIZE / scale);
}
void GameMap::drawChip(kuto::Graphics2D& g
, kuto::Texture const& src, kuto::Vector2 const& dstP, unsigned const chipID) const
{
	rpg2k_assert( src.isValid() );

	SaveData& lsd = field_.project().getLSD();
	kuto::Vector2 srcP;

	if(chipID == 10000) { return; // skip unvisible upper chip
	} else if( rpg2k::within(chipID, 3000u) ) {
		rpg2k_assert( rpg2k::within( chipID / 1000, 3) );
		rpg2k_assert( rpg2k::within( chipID % 1000 / 50, 0x10 ) );
		rpg2k_assert( rpg2k::within( chipID % 1000 % 50, 0x2f ) );

		drawBlockA_B( g, src, dstP
		, chipID / 1000, chipID % 1000 / 50,  chipID % 1000 % 50, counter_ % 12 / 4);
		return;
	} else if( rpg2k::within(3000u, chipID, 4000u) ) {
		rpg2k_assert( ( (chipID-3000)%50 ) == 28 );

		srcP.set( (chipID-3000)/50+3, 4 + counter_ % 12 / 3 );
	} else if( rpg2k::within(4000u, chipID, 5000u) ) {
		rpg2k_assert( rpg2k::within( (chipID-4000) / 50, 12 ) );
		rpg2k_assert( rpg2k::within( (chipID-4000) % 50, 0x2f ) );

		drawBlockD( g, src, dstP, (chipID-4000) / 50, (chipID-4000) % 50 );
		return;
	} else if( rpg2k::within(5000u, chipID, 5144u) ) {
		int num = lsd.replace(rpg2k::ChipSet::LOWER, chipID -  5000);
		srcP.set(
			float( (num%6) + (num<96? 0 :  6) + 12 ),
			float( (num/6) - (num<96? 0 : 16) ) );
	} else if( rpg2k::within(10000u, chipID, 10144u) ) {
		int num = lsd.replace(rpg2k::ChipSet::UPPER, chipID - 10000);
		srcP.set(
			float( (num%6) + (num<48? 0 :  6) + (12+6) ),
			float( (num/6) + (num<48? 8 : -8) ) );
	} else rpg2k_assert(false);

	kuto::Vector2 const scale( src.width(), src.height() );
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	g.drawTexture(src, dstP, CHIP_SIZE, color, srcP * CHIP_SIZE / scale, (srcP+1.f) * CHIP_SIZE / scale);
}
// SEG[0] = (0,0), SEG[1] = (8,0),
// SEG[3] = (0,8), SEG[2] = (8,8)
static kuto::Vector2 const SEG_P[4] =
{
	kuto::Vector2(0.f, 0.f),
	kuto::Vector2(8.f, 0.f),
	kuto::Vector2(8.f, 8.f),
	kuto::Vector2(0.f, 8.f),
};
void GameMap::drawBlockA_B(kuto::Graphics2D& g
, kuto::Texture const& src, kuto::Vector2 const& dstP
, unsigned const x, unsigned const y, unsigned const z, unsigned const anime) const
{
	kuto_assert( rpg2k::within(anime, 3u) );

	enum Pattern { A = 0, B = 1, C = 2, D = 3, N = 4, };
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 const scale( src.width(), src.height() );
// Ocean
	{
		Pattern ocean[4];
		Pattern OFF = (x == 2)? D : A, ON = (x == 2)? C : B;
		for(int i = 0; i < 4; i++) ocean[i] = y&(0x01<<i)? ON : OFF;
		// OCN_SEG[0] = (0,0), OCN_SEG[1] = (8,0),
		// OCN_SEG[2] = (0,8), OCN_SEG[3] = (8,8)
		static kuto::Vector2 const OCN_SEG_P[4] = {
			kuto::Vector2(0.f, 0.f), kuto::Vector2(8.f, 0.f),
			kuto::Vector2(0.f, 8.f), kuto::Vector2(8.f, 8.f),
		};
		kuto::Vector2 const oceanP = CHIP_SIZE * kuto::Vector2(anime, 4);
		for(int i = 0; i < 4; i++) {
			g.drawTexture(src, dstP + OCN_SEG_P[i], CHIP_SIZE * 0.5f, color
			, (oceanP + kuto::Vector2(0.f, 16.f) * ocean[i] + OCN_SEG_P[i]) / scale
			, (oceanP + kuto::Vector2(0.f, 16.f) * ocean[i] + OCN_SEG_P[i] + CHIP_SIZE * 0.5f) / scale );
		}
	}
// Coast
	{
		Pattern coast[4];
		if(z < 0x10) {
			for(int i = 0; i < 4; i++) coast[i] = z&(0x1<<i)? D : N;
		} else if(z < 0x14) {
			coast[0] = C; coast[1] = z&0x1? D : N;
			coast[3] = C; coast[2] = z&0x2? D : N;
		} else if(z < 0x18) {
			coast[0] = C; coast[1] = C;
			coast[3] = z&0x2? D : N; coast[2] = z&0x1? D : N;
		} else if(z<0x1c) {
			coast[0] = z&0x2? D : N; coast[1] = B;
			coast[3] = z&0x1? D : N; coast[2] = B;
		} else if(z<0x20) {
			coast[0] = z&0x1? D : N; coast[1] = z&0x2? D : N;
			coast[3] = C; coast[2] = C;
		} else if(z == 0x20) {
			coast[0] = coast[1] =
			coast[3] = coast[2] = B;
		} else if(z == 0x21) {
			coast[0] = coast[1] =
			coast[3] = coast[2] = C;
		} else if(z<0x24) {
			coast[0] = A; coast[1] = C;
			coast[3] = B; coast[2] = z&0x1? D : N;
		} else if(z<0x26) {
			coast[0] = C; coast[1] = A;
			coast[3] = z&0x1? D : N; coast[2] = B;
		} else if(z<0x28) {
			coast[0] = z&0x1? D : N; coast[1] = B;
			coast[3] = C; coast[2] = A;
		} else if(z<0x2a) {
			coast[0] = B; coast[1] = z&0x1? D : N;
			coast[3] = A; coast[2] = C;
		} else switch(z) {
			case 0x2a:
				coast[0] = A; coast[1] = A;
				coast[3] = B; coast[2] = B;
				break;
			case 0x2b:
				coast[0] = A; coast[1] = C;
				coast[3] = A; coast[2] = C;
				break;
			case 0x2c:
				coast[0] = B; coast[1] = B;
				coast[3] = A; coast[2] = A;
				break;
			case 0x2d:
				coast[0] = C; coast[1] = A;
				coast[3] = C; coast[2] = A;
				break;
			case 0x2e:
				coast[0] = coast[1] =
				coast[3] = coast[2] = A;
				break;
			default: rpg2k_assert(false); break;
		}
		kuto::Vector2 const coastP = CHIP_SIZE * kuto::Vector2( ( (x==1)? 3 : 0 ) + anime, 0.f );
		for(int i = 0; i < 4; i++) {
			if( coast[i] == N ) continue;

			g.drawTexture(src, dstP + SEG_P[i], CHIP_SIZE * 0.5f, color
			, (coastP + kuto::Vector2(0.f, 16.f) * coast[i] + SEG_P[i]) / scale
			, (coastP + kuto::Vector2(0.f, 16.f) * coast[i] + SEG_P[i] + CHIP_SIZE * 0.5f) / scale );
		}
	}
}
void GameMap::drawBlockD(kuto::Graphics2D& g, kuto::Texture const& src
, kuto::Vector2 const& dstP, unsigned const x, unsigned const y) const
{
	enum Pattern {
		A , B , C ,
		D7, D8, D9,
		D4, D5, D6,
		D1, D2, D3,
	} pat[4] = { D5, D5, D5, D5 };

	if(y < 0x10) { for(int i = 0; i < 4; i++) pat[i] = y&(0x01<<i)? C : D5;
	} else if(y < 0x14) {
		pat[0] = D4; pat[1] = y&0x01? C : D5;
		pat[3] = D4; pat[2] = y&0x02? C : D5;
	} else if(y < 0x18) {
		pat[0] = D8; pat[1] = D8;
		pat[3] = y&0x02? C : D5; pat[2] = y&0x01? C : D5;
	} else if(y < 0x1C) {
		pat[0] = y&0x01? C : D5; pat[1] = D6;
		pat[3] = y&0x02? C : D5; pat[2] = D6;
	} else if(y < 0x20) {
		pat[0] = y&0x01? C : D5; pat[1] = y&0x02? C : D5;
		pat[3] = D2; pat[2] = D2;
	} else if(y == 0x20) {
		pat[0] = D4; pat[1] = D6;
		pat[3] = D4; pat[2] = D6;
	} else if(y == 0x21) {
		pat[0] = D8; pat[1] = D8;
		pat[3] = D2; pat[2] = D2;
	} else if(y < 0x24) {
		pat[0] = D7; pat[1] = D7;
		pat[3] = D7; pat[2] = y&0x01? C : D7;
	} else if(y < 0x26) {
		pat[0] = D9; pat[1] = D9;
		pat[3] = y&0x01? C : D9; pat[2] = D9;
	} else if(y < 0x28) {
		pat[0] = y&0x01? C : D3; pat[1] = D3;
		pat[3] = D3; pat[2] = D3;
	} else if(y < 0x2a) {
		pat[0] = D1; pat[1] = y&0x01? C : D1;
		pat[3] = D1; pat[2] = D1;
	} else if(y == 0x2a) {
		pat[0] = D7; pat[1] = D9;
		pat[3] = D7; pat[2] = D9;
	} else if(y == 0x2b) {
		pat[0] = D7; pat[1] = D7;
		pat[3] = D1; pat[2] = D1;
	} else if(y == 0x2c) {
		pat[0] = D1; pat[1] = D3;
		pat[3] = D1; pat[2] = D3;
	} else if(y == 0x2d) {
		pat[0] = D9; pat[1] = D9;
		pat[3] = D3; pat[2] = D3;
	} else if(y == 0x2e) {
		pat[0] = D7; pat[1] = D9;
		pat[3] = D1; pat[2] = D3;
	} else rpg2k_assert(false);

	kuto::Vector2 const baseP = CHIP_SIZE * kuto::Vector2(
		(3*(x%2) + (x<4? 0 :  6)),
		(4*(x/2) + (x<4? 8 : -8)) );
	static kuto::Vector2 const PAT_P[3 * 4] = {
		kuto::Vector2( 0.f,  0.f), kuto::Vector2(16.f,  0.f), kuto::Vector2(32.f,  0.f),
		kuto::Vector2( 0.f, 16.f), kuto::Vector2(16.f, 16.f), kuto::Vector2(32.f, 16.f),
		kuto::Vector2( 0.f, 32.f), kuto::Vector2(16.f, 32.f), kuto::Vector2(32.f, 32.f),
		kuto::Vector2( 0.f, 48.f), kuto::Vector2(16.f, 48.f), kuto::Vector2(32.f, 48.f),
	};
	kuto::Color const color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 const scale( src.width(), src.height() );
	for(int i = 0; i < 4; i++) {
		g.drawTexture(src, dstP+SEG_P[i], CHIP_SIZE/2, color
		, (baseP + PAT_P[ pat[i] ] + SEG_P[i]) / scale
		, (baseP + PAT_P[ pat[i] ] + SEG_P[i] + CHIP_SIZE * 0.5f) / scale );
	}
}

namespace
{
	int chipID2chipIndex(SaveData& lsd, unsigned const chipID) // only for lower chip
	{
		int index;

		if( rpg2k::within(chipID, 3000u) ) index = 0 + chipID/1000;
		else if(chipID == 3028) index = 3 + 0;
		else if(chipID == 3078) index = 3 + 1;
		else if(chipID == 3128) index = 3 + 2;
		else if( rpg2k::within(4000u, chipID, 5000u) ) index =  6 + (chipID-4000)/50;
		else if( rpg2k::within(5000u, chipID, 5144u) )
			index = 18 + lsd.replace(rpg2k::ChipSet::LOWER, chipID-5000);
		else kuto_assert(false);

		return index;
	}
	bool isUpperChip(int const chipID)
	{
		return rpg2k::within(10000, chipID, 10144);
	}
	int toUpperChipIndex(SaveData& lsd, int const chipID)
	{
		return lsd.replace(rpg2k::ChipSet::UPPER, chipID-10000);
	}
} // namespace
bool GameMap::isAbove(int const chipID) const
{
	int flag;
	if( isUpperChip(chipID) ) {
		flag = (*cache_.upperChipFlag)[ toUpperChipIndex(*cache_.lsd, chipID) ];
	} else {
		uint const index = chipID2chipIndex(*cache_.lsd, chipID);
		if(
			rpg2k::within(4000, chipID, 5000) &&
			( ((*cache_.lowerChipFlag)[index] & 0x30) == 0x30 )
		) switch( (chipID-4000) % 50 ) {
			case 0x14: case 0x15: case 0x16: case 0x17:
			case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
			case 0x2a: case 0x2b:
			case 0x2d: case 0x2e:
				return true;
			default:
				return false;
		}
		flag = (*cache_.lowerChipFlag)[index];
	}
	return (flag & 0x30) == 0x10;
}
bool GameMap::isCounter(int const chipID) const
{
	rpg2k_assert( isUpperChip(chipID) );
	return ( (*cache_.upperChipFlag)[chipID-10000] & 0x40 ) != 0x00;
}
uint8_t GameMap::pass(int const chipID) const
{
	if( isUpperChip(chipID) ) {
		return (*cache_.upperChipFlag)[ toUpperChipIndex(*cache_.lsd, chipID) ]; // & 0x0f;
	} else return (*cache_.lowerChipFlag)[ chipID2chipIndex(*cache_.lsd, chipID) ]; // & 0x0f;
}
int GameMap::terrainID(int const chipID) const
{
	rpg2k_assert( !isUpperChip(chipID) );
	return (*cache_.terrain)[ chipID2chipIndex(*cache_.lsd, chipID) ];
}
