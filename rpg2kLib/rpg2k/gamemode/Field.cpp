#define USE_EVENT_COMMAND

#include "../Debug.hpp"
#include "../Execute.hpp"
#include "../Graphics2D.hpp"
#include "../KeyListener.hpp"
#include "../Main.hpp"
#include "../Project.hpp"


namespace rpg2k
{
	namespace gamemode
	{
		PP_defineDefaultGameMode(Shop)
		PP_defineDefaultGameMode(NameInput)
		PP_defineDefaultGameMode(Hotel)

		typedef std::multimap< uint, uint >::const_iterator x_it;
		typedef std::multimap< uint, uint >::value_type x_val;
		typedef std::map< uint, std::multimap< uint, uint > >::const_iterator y_it;
		typedef std::map< uint, std::multimap< uint, uint > >::value_type y_val;
		typedef std::vector< std::map< uint, std::multimap< uint, uint > > >::const_iterator priority_it;

		typedef std::map< int, int >::value_type page_val;

		static bool exists(std::map< uint, std::multimap< uint, uint > > const& data, uint x, uint y)
		{
			return ( data.find(y) != data.end() ) && ( data.find(y)->second.find(x) != data.find(y)->second.end() );
		}

		Field::Field(rpg2k::Main& m)
		: GameMode(m), touchFromParty_(), touchFromEvent_()
		{
		}

		void Field::reset()
		{
		}

		static int const speed[] = { 64, 32, 16, 8, 4, 2 };

		void Field::run(input::KeyListener& keyList)
		{
			if( keyList.cancel() && getOwner().getProject().canOpenMenu() ) {
				getOwner().callGameMode(GameMode::Menu);
			}

			for( ; !touchFromParty_.empty(); touchFromParty_.pop() );
			for( ; !touchFromEvent_.empty(); touchFromEvent_.pop() );

			eventMap_.clear();
			eventMap_.resize(EventPriority::END);
			pageNum_.clear();

			model::Project& proj = getOwner().getProject();
			model::SaveData& lsd = proj.getLSD();
			structure::Array2D& eventStates = lsd.eventState();
			structure::Array2D& mapEvents = getOwner().getProject().getLMU()[81];
		// mapping events
			for(structure::Array2D::Iterator it = eventStates.begin(); it != eventStates.end(); ++it) {
				if( !it.second().exists() ) continue;

				int eventID = it.first(), priority, pageID;
				structure::Array1D& event = mapEvents[eventID];
				structure::EventState& state = (structure::EventState&)it.second();

				structure::Array1D* page = proj.currentPage(event[5]);
				if( page != NULL ) {
					pageID = page->getIndex();
					priority = (*page)[34];
				} else {
					pageID = INVALID_PAGE_ID;
					priority = EventPriority::BELOW;
				}

				pageNum_.insert( page_val(eventID, pageID) );

				int x = state.exists(12) ? state.x() : event[2];
				int y = state.exists(13) ? state.y() : event[3];

				if( eventMap_[priority].find(y) == eventMap_[priority].end() ) {
					eventMap_[priority].insert( y_val( y, std::multimap< uint, uint >() ) );
				}
				eventMap_[priority][y].insert( x_val(x, eventID) );
			}
		// mapping non-events
			for(uint i = EV_ID_PARTY; i <= EV_ID_AIRSHIP; i++) {
				structure::EventState& state = lsd.eventState(i);

				if( state.mapID() != lsd.eventState(EV_ID_PARTY).mapID() ) continue;

				int x = state.x(), y = state.y();

				if( eventMap_[EventPriority::CHAR].find(x) == eventMap_[EventPriority::CHAR].end() ) {
					eventMap_[EventPriority::CHAR].insert( y_val( y, std::multimap< uint, uint >() ) );
				}
				eventMap_[EventPriority::CHAR][y].insert( x_val(x, i) );

				if(i == EV_ID_PARTY) {
					Key::Type cursor = keyList.getCursor();
					if( (cursor != Key::NONE) && judgeMove(i, toEventDir(cursor) ) ) {
						// do something
					}
				}
			}
		// process event
			model::Execute& exe = getOwner().getExecute();

			if(allEventExecuted_) {
				exe.refresh(eventMap_, pageNum_);
				allEventExecuted_ = false;
			}

			for(int step = 0; step < EV_STEP_MAX; step++) {
				const structure::Instruction* inst = exe.nextInst();

				if(inst == NULL) {
					allEventExecuted_ = true;
					break;
				} else switch( exe.execute( *inst ) ) {
					case model::Execute::FAILED:
						break;
					case model::Execute::SUCCESS:
						break;
					case model::Execute::NOT_IMPLEMENTED:
						break;
					case model::Execute::NEXT_FRAME: return;
					default:
						clog << "Undefined execute state." << endl;
						break;
				}
			}
		}

		static const Size2D CHIP_NUM = SCREEN_SIZE / CHIP_SIZE;

		void Field::draw(Graphics2D& g)
		{
			model::Project& proj = getOwner().getProject();
			model::SaveData& lsd = proj.getLSD();
			model::MapUnit & lmu = proj.getLMU();
		// panorama
			RPG2kString panorama = proj.panorama();
			if( !panorama.empty() ) g.drawImage( g.getImage(Material::Panorama, panorama), Vector2D(0, 0) );
		// chip and events
			// mapping like [y][x]

			Vector2D baseP = lsd.eventState(EV_ID_PARTY).position() - (CHIP_NUM / 2);

			int scrll = lmu[11];
		// horizon
			if( !(scrll&0x02) ) {
				if( lmu[2].get<uint>() <= (baseP[0] + CHIP_NUM[0]) )
					baseP[0] = lmu[2].get<uint>() - CHIP_NUM[0];
				else if(baseP[0] < 0) baseP[0] = 0;
			}
		// vertical
			if( !(scrll&0x01) ) {
				if( lmu[3].get<uint>() <= (baseP[1] + CHIP_NUM[1]) )
					baseP[1] = lmu[3].get<uint>() - CHIP_NUM[1];
				else if(baseP[1] < 0) baseP[1] = 0;
			}

			Vector< 2, uint > it;
			for(it[1] = 0; it[1] < CHIP_NUM[1]; it[1]++) {
				bool aboveLw[ /* CHIP_NUM[0] */ 20 ], aboveUp[ /* CHIP_NUM[0] */ 20 ];
				int lw[ /* CHIP_NUM[0] */ 20 ], up[ /* CHIP_NUM[0] */ 20 ];

				// above chips
				for(it[0] = 0; it[0] < CHIP_NUM[0]; it[0]++) {
					up[ it[0] ] = lmu.chipIDUp(baseP + it);
					lw[ it[0] ] = lmu.chipIDLw(baseP + it);
					aboveUp[ it[0] ] = proj.isAbove(up[ it[0] ]);
					aboveLw[ it[0] ] = proj.isAbove(lw[ it[0] ]);

					if(!aboveLw[ it[0] ]) g.drawChipSet( CHIP_SIZE*it, lw[ it[0] ]);
					if(!aboveUp[ it[0] ]) g.drawChipSet( CHIP_SIZE*it, up[ it[0] ]);
				}

				// event graphics
				for(priority_it prIt = eventMap_.begin(); prIt != eventMap_.end(); ++prIt) {
					if(
						( prIt->find(baseP[1]+it[1]) == prIt->end() ) ||
						( prIt->find(baseP[1]+it[1])->second.size() == 0 )
					) continue;

					std::multimap< uint, uint > const& base = prIt->find(baseP[1]+it[1])->second;
					for(x_it x = base.begin(); x != base.end(); ++x) {
						if(
							(x->first < (uint)baseP[0]) ||
							( (baseP[0] + CHIP_NUM[0]) <= x->first )
						) continue;

						structure::EventState& state = lsd.eventState(x->second);
						it[0] = x->first - baseP[0];

						if( state.exists(73) ) {
							g.drawCharSet(
								state,
								it * CHIP_SIZE - Vector2D( (CHAR_SIZE[0]-CHIP_SIZE[0])/2, CHAR_SIZE[1]-CHIP_SIZE[1] )
							);
						} else g.drawChipSet( CHIP_SIZE*it, 10000+state.charSetPos() );
					}
				}
				// below chips
				for(it[0] = 0; it[0] < CHIP_NUM[0]; it[0]++) {
					if(aboveLw[ it[0] ]) g.drawChipSet( CHIP_SIZE*it, lw[ it[0] ] );
					if(aboveUp[ it[0] ]) g.drawChipSet( CHIP_SIZE*it, up[ it[0] ] );
				}
			}
		// picture
		// battle animation
			#if defined(RPG2000)
				drawPictures(g);
				drawBattles(g);
			#elif defined(RPG2000_VALUE) || defined(RPG2003)
				drawBattles(g);
				drawPictures(g);
			#endif
		// massage window
		// movie
		}
		void Field::drawPictures(Graphics2D& g)
		{
			structure::Array2D const& pictures = getOwner().getProject().getLSD()[103];
			for(structure::Array2D::Iterator it = pictures.begin(); it != pictures.end(); ++it) {
				structure::Array1D const& target = it.second();
				if(
					!target.exists() ||
					!target.exists(1) ||
					target[1].get_string().empty()
				) continue;

				Vector2D coord[4];
				Vector2D center( target[4].get<double>(), target[5].get<double>() );
				Image const& img = g.getPicture(target[1], target[9]);
				Size2D size = img.size() * target[33].get<int>() / 100;

				switch( target[15].get<int>() ) {
					default:
						coord[0] = center - size / 2;
						coord[1] = center + Size2D(+size[0], -size[1]) / 2;
						coord[2] = center + size / 2;
						coord[3] = center + Size2D(-size[0], +size[1]) / 2;
						break;
				}
				ColorRGBA tone;
				tone[3] = ( 100 - target[34].get<int>() ) / 100.0f;
				for(int i = 0; i < 3; i++)
					tone[i] = target[11 + i].get<double>() * target[14].get<double>() / 100.0f / 100.0f;

				g.drawImage(img, coord, tone);
			}
		}
		void Field::drawBattles(Graphics2D& g)
		{
		}
		void Field::gameModeChanged()
		{
			charMoveCount = 0;
			allEventExecuted_ = true;
		}

		// EventDir::DOWN = 2, EventDir::LEFT = 4, EventDir::RIGHT = 6, EventDir::UP = 8,

		static int nextX(EventDir::Type dir, uint x)
		{
			switch(dir) {
				case EventDir::LEFT : return x-1;
				case EventDir::RIGHT: return x+1;
				case EventDir::DOWN : case EventDir::UP   : default: return x;
			}
		}
		static int nextY(EventDir::Type dir, uint y)
		{
			switch(dir) {
				case EventDir::DOWN : return y+1;
				case EventDir::UP   : return y-1;
				case EventDir::LEFT : case EventDir::RIGHT: default: return y;
			}
		}

		bool Field::judgeMove(uint eventID, EventDir::Type dir)
		{
			model::MapUnit& lmu = getOwner().getProject().getLMU();
			structure::Array2D& events = lmu[81];
			structure::Array1D& page = events[eventID][5].getArray2D()[ pageNum_[eventID] ];

			uint mapW = lmu[2], mapH = lmu[3];
			structure::EventState& state = getOwner().getProject().getLSD().eventState(eventID);
			state[21] = state[22] = (int) dir;

			uint curX = state.x(), curY = state.y();
			uint nxtX = nextX(dir, curX), nxtY = nextY(dir, curY);

			int scrll = lmu[11];
		// horizon
			if(scrll&0x02) {
				if(nxtX == (uint)-1) nxtX = mapW- 1;
				else if(mapW <= nxtX) nxtX = 0;
			} else if( (nxtX < 0) || (mapW <= nxtX) ) return false;
		// vertical
			if(scrll&0x01) {
				if(nxtY == (uint)-1) nxtY = mapH - 1;
				else if(mapH <= nxtY) nxtY = 0;
			} else if( (nxtY < 0) || (mapH <= nxtY) ) return false;

		// with map chip level
			if( canPassMap(dir, curX, curY, nxtX, nxtY) ) {
			// with event level
				// priority
				uint pr;
				if( (EV_ID_PARTY <= eventID) && (eventID <= EV_ID_AIRSHIP) ) pr = EventPriority::CHAR;
				else pr = page[34];
				// char
				if( (pr == EventPriority::CHAR) && gamemode::exists(eventMap_[pr], nxtX, nxtY) ) {
					// clog << "touch with event."<< endl;

					for(x_it it = eventMap_[pr][nxtY].find(nxtX); it->first == nxtX; ++it) {
						if(it->second == EV_ID_PARTY) touchFromEvent_.push(eventID);
						else if(eventID == EV_ID_PARTY) touchFromParty_.push(it->second);
					}
					return false;
				}
				// non piled event
				if( (eventID < EV_ID_PARTY) || (EV_ID_AIRSHIP < eventID) )
				for(int i = 0; i < EventPriority::END; i++) {
					if( gamemode::exists(eventMap_[i], nxtX, nxtY) )
					for(x_it it = eventMap_[i][nxtY].find(nxtX); it->first == nxtX; ++it) {
						if(
							events[it->second][5].getArray2D()[ pageNum_[it->second] ][35].get<bool>()
						) return false;
					}
				}
			// change pos
				// remove previous place
				std::multimap< uint, uint >::iterator it = eventMap_[pr][curY].find(curX);
				for(; it->second != eventID; ++it);
				eventMap_[pr][curY].erase(it);
				// reset next place
				if( eventMap_[pr].find(nxtY) == eventMap_[pr].end() )
					eventMap_[pr].insert( y_val( nxtY, std::multimap< uint, uint >() ) );
				eventMap_[pr][nxtY].insert( x_val(nxtX, eventID) );
				state[12] = nxtX; state[13] = nxtY;

				return true;
			}
			return false;
		}

		bool Field::canPassMap(EventDir::Type dir, int curX, int curY, int nxtX, int nxtY)
		{
			model::Project& proj = getOwner().getProject();
			model::MapUnit& lmu = proj.getLMU();

			int shiftNum = dir / 2 - 1;

			int curLw = lmu.chipIDLw(curX, curY), curUp = lmu.chipIDUp(curX, curY);
			int nxtLw = lmu.chipIDLw(nxtX, nxtY), nxtUp = lmu.chipIDUp(nxtX, nxtY);

			uint8_t curMsk = 0x01 << shiftNum, nxtMsk = 0x08 >> shiftNum;

		// with map chip level
			if(
				( proj.getPass(curUp) & curMsk ) &&
				( proj.getPass(nxtUp) & nxtMsk )
			) {
				bool curUpAb = proj.isAbove(curUp);
				bool nxtUpAb = proj.isAbove(nxtUp);

				if(
					( ( curUpAb && ( proj.getPass(curLw) & curMsk ) ) || !curUpAb ) &&
					( ( nxtUpAb && ( proj.getPass(nxtLw) & nxtMsk ) ) || !nxtUpAb )
				) return true;
			}

			return false;
		}
	} // namespace gamemode
} // namespace rpg2k
