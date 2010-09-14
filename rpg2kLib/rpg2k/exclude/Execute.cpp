#include <algorithm>

#include "Audio2D.hpp"
#include "Debug.hpp"
#include "Execute.hpp"
#include "Main.hpp"
#include "Project.hpp"


namespace rpg2k
{
	namespace model
	{
		Project& Execute::getProject() { return owner_.getProject(); }

		Execute::Execute(Main& m)
		: owner_(m)
		{
			#define PP_addEventCommand(code) \
				commandTable_.insert( std::make_pair( code, Command< code >(*this) ) )

				PP_addEventCommand(    0); // end of event
				PP_addEventCommand(   10); // end of junction

				PP_addEventCommand(10110); // show message
				PP_addEventCommand(20110);
				PP_addEventCommand(10120); // set message config
				PP_addEventCommand(10130); // set message face set
				PP_addEventCommand(10140); // select junction
				PP_addEventCommand(20140);
				PP_addEventCommand(20141);
				PP_addEventCommand(10150); // input number

				PP_addEventCommand(10210); // change switch
				PP_addEventCommand(10220); // change variable
				PP_addEventCommand(10230); // set timer

				PP_addEventCommand(10310); // set money
				PP_addEventCommand(10320); // set item
				PP_addEventCommand(10330); // exchange member

				PP_addEventCommand(10410); // change exp value
				PP_addEventCommand(10420); // change level
				PP_addEventCommand(10430); // change character parameter
				PP_addEventCommand(10440); // add or remove skill
				PP_addEventCommand(10450); // change equipment
				PP_addEventCommand(10460); // change HP
				PP_addEventCommand(10470); // change MP
				PP_addEventCommand(10480); // set condition
				PP_addEventCommand(10490); // full cure
				PP_addEventCommand(10500); // damage character

				PP_addEventCommand(10610); // change character name
				PP_addEventCommand(10620); // change character title
				PP_addEventCommand(10630); // change character graphic
				PP_addEventCommand(10640); // change face graphic
				PP_addEventCommand(10650); // change char graphic
				PP_addEventCommand(10660); // change system BGM
				PP_addEventCommand(10670); // change system SE
				PP_addEventCommand(10680); // change system graphic
				PP_addEventCommand(10690); // change screen refresh

				PP_addEventCommand(10710); // battle junction
				PP_addEventCommand(20710);
				PP_addEventCommand(20711);
				PP_addEventCommand(20712);
				PP_addEventCommand(20713);
				PP_addEventCommand(10720); // shop junction
				PP_addEventCommand(20720);
				PP_addEventCommand(20721);
				PP_addEventCommand(20722);
				PP_addEventCommand(10730); // hotel junction
				PP_addEventCommand(20730);
				PP_addEventCommand(20731);
				PP_addEventCommand(20732);
				PP_addEventCommand(10740); // input character name

				PP_addEventCommand(10810); // move to a place
				PP_addEventCommand(10820); // memorize current point
				PP_addEventCommand(10830); // move to the memorized point
				PP_addEventCommand(10840); // get on to the transport
				PP_addEventCommand(10850); // set transport posi
				PP_addEventCommand(10860); // set event position
				PP_addEventCommand(10870); // swap event position

				PP_addEventCommand(10910); // get terrain id
				PP_addEventCommand(10920); // get event id

				PP_addEventCommand(11010); // erase screen
				PP_addEventCommand(11020); // show screen
				PP_addEventCommand(11030); // set screen color tone
				PP_addEventCommand(11040); // flash screen
				PP_addEventCommand(11050); // shake screen
				PP_addEventCommand(11060); // scroll screen
				PP_addEventCommand(11070); // set weather effect

				PP_addEventCommand(11110); // show picture
				PP_addEventCommand(11120); // move picture
				PP_addEventCommand(11130); // erase picture

				PP_addEventCommand(11210); // battle anime at field

				PP_addEventCommand(11310); // transparent hero
				PP_addEventCommand(11320); // flash event
				PP_addEventCommand(11330); // set move
				PP_addEventCommand(11340); // process all moves
				PP_addEventCommand(11350); // dispose all moves

				PP_addEventCommand(11410); // wait

				PP_addEventCommand(11510); // play music
				PP_addEventCommand(11520); // fade out music
				PP_addEventCommand(11530); // memorize music
				PP_addEventCommand(11540); // play memorized music
				PP_addEventCommand(11550); // play sound
				PP_addEventCommand(11560); // play movie

				PP_addEventCommand(11610); // get key input

				PP_addEventCommand(11710); // change chip set
				PP_addEventCommand(11720); // change panorama
				PP_addEventCommand(11740); // change encount step
				PP_addEventCommand(11750); // replace chip

				PP_addEventCommand(11810); // add teleport link
				PP_addEventCommand(11820); // disable or enable teleport
				PP_addEventCommand(11830); // set escape link
				PP_addEventCommand(11840); // disable or enable escape

				PP_addEventCommand(11910); // call save screen
				PP_addEventCommand(11930); // disable or enable saving
				PP_addEventCommand(11950); // call menu
				PP_addEventCommand(11960); // disable or enable menu

				PP_addEventCommand(12010); // junction
				PP_addEventCommand(22010);
				PP_addEventCommand(22011);

				PP_addEventCommand(12110); // set label
				PP_addEventCommand(12120); // goto label

				PP_addEventCommand(12210); // loop
				PP_addEventCommand(22210);
				PP_addEventCommand(12220); // interrupt loop

				PP_addEventCommand(12310); // interrupt event
				PP_addEventCommand(12320); // erase event
				PP_addEventCommand(12330); // call event

				PP_addEventCommand(12410); // comment
				PP_addEventCommand(22410);
				PP_addEventCommand(12420); // gameover

				PP_addEventCommand(12510); // title
			#undef PP_addEventCommand

			#define PP_addEventCommand(code) \
				battleCommandTable_.insert( std::make_pair( code, Command< code >(*this) ) )

				PP_addEventCommand(    0); // end of event
				PP_addEventCommand(   10); // end of junction

				PP_addEventCommand(10110); // show message
				PP_addEventCommand(20110);
				PP_addEventCommand(10130); // set message face set

				PP_addEventCommand(10210); // change switch
				PP_addEventCommand(10220); // change variable

				PP_addEventCommand(10320); // set item
				PP_addEventCommand(10330); // exchange member

				PP_addEventCommand(10440); // add or remove skill
				PP_addEventCommand(10450); // change equipment
				PP_addEventCommand(10460); // change HP
				PP_addEventCommand(10470); // change MP
				PP_addEventCommand(10480); // set condition
				PP_addEventCommand(10490); // full cure

				PP_addEventCommand(11030); // set screen color tone
				PP_addEventCommand(11040); // flash screen
				PP_addEventCommand(11050); // shake screen

				PP_addEventCommand(11410); // wait

				PP_addEventCommand(11510); // play music
				PP_addEventCommand(11520); // fade out music
				PP_addEventCommand(11550); // play sound

				PP_addEventCommand(12410); // comment
				PP_addEventCommand(22410);
				PP_addEventCommand(12420); // gameover

				PP_addEventCommand(12510); // title
			// battle events
				PP_addEventCommand(13110); // change enemy HP
				PP_addEventCommand(13120); // change enemy MP
				PP_addEventCommand(13130); // change enemy condition
				PP_addEventCommand(13150); // appear enemy

				PP_addEventCommand(13210); // change backdrop
				PP_addEventCommand(13260); // show battle animation

				PP_addEventCommand(13310); // junction true  way
				PP_addEventCommand(23310); // junction false way
				PP_addEventCommand(23311); // end of junction

				PP_addEventCommand(13410); // interrupt battle
			#undef PP_addEventCommand
		}

		bool Execute::isJunction(uint code) const
		{
			return false;
		}
		void Execute::refresh(
			std::vector< std::map< uint, std::multimap< uint, uint > > >& eventMap,
			std::map< uint, uint >& pageNo
		) {
			SaveData& lsd = getProject().getLSD();

			eventMap_ = &eventMap;
			pageNo_ = &pageNo;

			execInfo_.clear();
			execInfo_.resize( EV_TYPE_END, std::map< uint, ExecInfo >() );
		// get page number and map map events
			structure::Array2D& mapEv = getProject().getLMU()[81];
			for(structure::Array2D::Iterator it = mapEv.begin(); it != mapEv.end(); ++it) {
				if( !it.second().exists() ) continue;

				int eventID = it.first(), pageID = (*pageNo_)[eventID];
				if(pageID == INVALID_PAGE_ID) continue;
				structure::Array1D& page = mapEv[eventID][5].getArray2D()[pageID];

				ExecInfo info = {
					EV_MAP, eventID,
					0, false,
					EventStart::Type( page[33].get<int>() ), pageID, &( page[52].getEvent() )
				};

				execInfo_[EV_MAP].insert( std::make_pair(eventID, info) );
			}
		// map common events
			structure::Array2D& comEv = getProject().getLDB()[25];
			for(structure::Array2D::Iterator it = comEv.begin(); it != comEv.end(); ++it) {
				if( !it.second().exists() ) continue;

				int eventID = it.first();
				structure::Array1D& cur = it.second();
				if(
					( cur[11].get<int>() != EventStart::CALLED) &&
					( !cur[12].get<bool>() || ( cur[12].get<bool>() && lsd.getFlag(cur[13]) ) )
				) {
					ExecInfo info = {
						EV_MAP, eventID,
						0, false,
						EventStart::Type( cur[11].get<int>() ), 0, &( cur[22].getEvent() )
					};

					execInfo_[EV_COMMON].insert( std::make_pair(eventID, info) );
				}
			}
			nextEvent();
		}

		Execute::State Execute::execute(structure::Instruction const& inst)
		{
			rpg2k_assert( commandTable_.find( inst.code() ) != commandTable_.end() );
			return commandTable_.find( inst.code() )->second( inst.getString(), inst.getArgs() );
		}
		Execute::State Execute::executeBattle(structure::Instruction const& inst)
		{
			rpg2k_assert( battleCommandTable_.find( inst.code() ) != battleCommandTable_.end() );
			return battleCommandTable_.find( inst.code() )->second( inst.getString(), inst.getArgs() );
		}

		bool Execute::nextEvent()
		{
			typedef std::map< uint, Execute::ExecInfo >::const_iterator info_it;
			typedef std::multimap< uint, uint >::const_iterator x_it;
			typedef std::map< uint, std::multimap< uint, uint > >::const_iterator y_it;

			if( hasExec() ) {
				execInfo_[curExec().type].erase(curExec().id);
				popExec();
			}

			// Project& proj = getProject();

			std::map< uint, ExecInfo >& maps = execInfo_[EV_MAP];
			std::map< uint, ExecInfo >& coms = execInfo_[EV_COMMON];

			#define PP_iterate(target, eventType) \
				for(info_it it = target.begin(); it != target.end(); ++it) { \
					if(it->second.start == eventType) { \
						pushExec( it->second ); \
						return true; \
					} \
				}
			// auto start map event
			PP_iterate(maps, EventStart::AUTO)
			// parallel map event
			PP_iterate(maps, EventStart::PARALLEL)
			// auto start common event
			PP_iterate(coms, EventStart::AUTO)
			// parallel common event
			PP_iterate(coms, EventStart::PARALLEL)
			#undef PP_iterate
/*
			if( !owner_.touchFromEvent().empty() ) {
				stack< uint >& IDs = owner_.touchFromEvent();
				for( ; !IDs.empty(); IDs.pop() ) {
					if(maps[IDs.top()].start == EventStart::EVENT_TOUCH) {
						pushExec( maps[IDs.top()] );
						return true;
					}
				}
			}
			if( !owner_.touchFromParty().empty() ) {
				stack< uint >& IDs = owner_.touchFromParty();
				for( ; !IDs.empty(); IDs.pop() ) {
					if(maps[IDs.top()].start == EventStart::PARTY_TOUCH) {
						pushExec( maps[IDs.top()] );
						return true;
					}
				}
			}

			if( owner_.getKeyListener().enter() ) return false;

			structure::EventState& party = proj.getLSD().eventState(EV_ID_PARTY);
			EventDir dir = party.eventDir();
			uint curX = party.x(), curY = party.y();
			uint nxtX = gamemode::nextX(dir, curX), nxtY = gamemode::nextY(dir, curY);

			if( gamemode::exists( (*eventMap_)[EV_PR_CHAR], nxtX, nxtY ) ) {
				for(x_it it = (*eventMap_)[EV_PR_CHAR].find(nxtY)->second.find(nxtX); it->first == nxtX; ++it) {
					if(maps[it->second].start == EV_KEY_ENTER) {
						pushExec( maps[it->second] );
						return true;
					}
				}
			} else if( gamemode::exists( (*eventMap_)[EV_PR_BELOW], curX, curY ) ) {
				for(x_it it = (*eventMap_)[EV_PR_CHAR].find(curY)->second.find(curX); it->first == curX; ++it) {
					if(maps[it->second].start == EV_KEY_ENTER) {
						pushExec( maps[it->second] );
						return true;
					}
				}
			} else if( gamemode::exists( (*eventMap_)[EV_PR_ABOVE], curX, curY ) ) {
				for(x_it it = (*eventMap_)[EV_PR_CHAR].find(curY)->second.find(curX); it->first == curX; ++it) {
					if(maps[it->second].start == EV_KEY_ENTER) {
						pushExec( maps[it->second] );
						return true;
					}
				}
			}
 */
			return false;
		}

		const structure::Instruction* Execute::nextInst()
		{
			if(
				!hasExec() ||
				( ( curExec().counter >= curExec().event->size() ) && !nextEvent() )
			) return NULL;
			else return &(*curExec().event)[curExec().counter++];
		}
		const structure::Instruction* Execute::currentInst() const
		{
			if( !hasExec() ) return NULL;
			else return &(*curExec().event)[curExec().counter];
		}

		void Execute::skipToEndOfJunction(int nest, int code)
		{
			uint curNest = (nest == SKIP_CURRENT) ? currentInst()->nest() : nest;
			ExecInfo& exe = curExec();
			structure::Event const& event = *curExec().event;

			while(
				( event[exe.counter].nest() != curNest ) ||
				( event[exe.counter].code() != CODE__END_OF_JUNCTION)
			) exe.counter++;
		}
		bool Execute::skipToElse(int nest, int code)
		{
			uint curNest = (nest == SKIP_CURRENT) ? currentInst()->nest() : nest;
			uint curCode = (code == SKIP_CURRENT) ? currentInst()->code() : code;
			curCode = curCode % 10000 / 10;
			ExecInfo& exe = curExec();
			structure::Event const& event = *curExec().event;

			while(
				( event[exe.counter].nest() != curNest ) ||
				( ( event[exe.counter].code() % 10000 / 10) != curCode)
			) {
				if( event[exe.counter].code() == CODE__END_OF_JUNCTION ) return false;
				exe.counter++;
			}

			return true;
		}

		#define PP_codeDef(codeNo) \
			template< > Execute::State Execute::Command< codeNo >::operator() \
			(RPG2kString const& strArg, std::vector< int32_t > const& args) const
		#define PP_defaultThrow() default: rpg2k_assert(false)

		PP_codeDef(0) // end of event
		{
			getOwner().nextEvent();
			return SUCCESS;
		}
		PP_codeDef(10) // end of junction
		{
			return SUCCESS;
		}

		PP_codeDef(10110) // show message
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20110)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10120) // set message config
		{
			structure::Array1D& lsdSys = getOwner().getProject().getLSD()[101];

			lsdSys[41] = args[0];
			lsdSys[42] = args[1];
			lsdSys[43] = bool(args[3]);
			lsdSys[44] = bool(args[4]);

			return SUCCESS;
		}
		PP_codeDef(10130) // set message face set
		{
			structure::Array1D& lsdSys = getOwner().getProject().getLSD()[101];

			lsdSys[51] = strArg;
			lsdSys[52] = args[0];
			lsdSys[53] = args[1];
			lsdSys[54] = bool(args[2]);

			return SUCCESS;
		}
		PP_codeDef(10140) // select junction
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20140)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20141)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10150) // input number
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(10210) // change switch
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			int start, end;
			switch(args[0]) {
				case 0: start = end = args[1];
					break;
				case 1: start = end = lsd.getVar(args[1]);
					break;
				case 2: start = args[1]; end = args[2];
					break;
				PP_defaultThrow();
			}
			switch(args[3]) {
				case 0: for(;end >= start; end--) lsd.setFlag(end, true );
					break;
				case 1: for(;end >= start; end--) lsd.setFlag(end, false);
					break;
				case 2:	for(;end >= start; end--) lsd.setFlag( end, !lsd.getFlag(end) );
					break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(10220) // change variable
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			int start, end;
			switch(args[0]) {
				case 0: start = end = args[1]; break;
				case 2: start = end = lsd.getVar(args[1]); break;
				case 1: start = args[1]; end = args[2]; break;
				PP_defaultThrow();
			}

			int32_t op;
			int32_t C = args[5], D = args[6];
			switch(args[4]) {
				case 0: op = C; break;
				case 1: op = lsd.getVar(C); break;
				case 2: op = lsd.getVar( lsd.getVar(C) ); break;
				case 3: op = random(C, D); break;
				case 4: // item
					switch(args[6]) {
					// item number you have
						case 0: op = lsd.getItemNum(args[5]); break;
					// item number your characters equiped
						case 1:
							return NOT_IMPLEMENTED;
						PP_defaultThrow();
					}
					break;
				case 5: { // char status
					structure::Array1D& target = lsd.character()[C];
					switch(D) {
						case  0: case  1: op = target[31+D  ]; break;
						case  2: case  3: op = target[71+D-2]; break;
						case  4: case  5: op = target[33+D-4]; break;
						case  6: case  7: case  8: case  9: op = target[41+D-6]; break;
					// equipment
						case 10: case 11: case 12: case 13: case 14: {
							 std::vector< uint16_t > equip = target[61].getBinary();
							 op = equip[D-10];
						} break;
						PP_defaultThrow();
					}
				} break;
				case 6: { // event info
					structure::EventState& state = lsd.eventState(C);
					switch(D) {
						case 0: case 1: case 2: op = state[11+D]; break;
						case 3:
						case 4:
						case 5:
							return NOT_IMPLEMENTED;
						PP_defaultThrow();
					}
				} break;
				case 7: // others
					switch(D) {
						case 0: op = lsd.getMoney(); break;
						case 1:
							return NOT_IMPLEMENTED;
						case 2: op = lsd.member().size(); break;
						case 3: op = lsd[101].getArray1D()[131]; break;
						case 4: op = lsd[109].getArray1D()[32]; break;
						case 5: op = lsd[109].getArray1D()[34]; break;
						case 6: op = lsd[109].getArray1D()[33]; break;
						case 7: op = lsd[109].getArray1D()[35]; break;
						case 8:
						case 9:
							return NOT_IMPLEMENTED;
						PP_defaultThrow();
					}
					break;
				case 8: // enemy status
					switch(D) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:
							return NOT_IMPLEMENTED;
						PP_defaultThrow();
					}
					break;
				PP_defaultThrow();
			}

			switch(args[3]) {
				case 0: for(;end >= start; end--) lsd.setVar(end, op); break;
				case 1: for(;end >= start; end--) lsd.setVar(end, lsd.getVar(end) + op); break;
				case 2: for(;end >= start; end--) lsd.setVar(end, lsd.getVar(end) - op); break;
				case 3: for(;end >= start; end--) lsd.setVar(end, lsd.getVar(end) * op); break;
				case 4: for(;end >= start; end--) lsd.setVar(end, lsd.getVar(end) / op); break;
				case 5: for(;end >= start; end--) lsd.setVar(end, lsd.getVar(end) % op); break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(10230) // set timer
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(10310) // set money
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			int val;
			switch(args[1]) {
				case 0: val = args[2];
					break;
				case 1: val = lsd.getVar(args[2]);
					break;
				PP_defaultThrow();
			}
			switch(args[0]) {
				case 0: val =  val; break;
				case 1: val = -val; break;
				PP_defaultThrow();
			}

			lsd.setMoney( lsd.getMoney() + val );

			return SUCCESS;
		}
		PP_codeDef(10320) // set item
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			int id;
			switch(args[1]) {
				case 0: id = args[2]; break;
				case 1: id = lsd.getVar(args[2]); break;
				PP_defaultThrow();
			}

			int val;
			switch(args[3]) {
				case 0: val = args[4]; break;
				case 1: val = lsd.getVar(args[4]); break;
				PP_defaultThrow();
			}
			switch(args[0]) {
				case 0: val =  val; break;
				case 1: val = -val; break;
				PP_defaultThrow();
			}

			lsd.setItemNum( id, lsd.getItemNum(id)+val );

			return SUCCESS;
		}
		PP_codeDef(10330) // exchange member
		{
			SaveData& lsd = getOwner().getProject().getLSD();
			std::vector< uint16_t >& mem = lsd.member();

			uint charID;
			switch(args[1]) {
				case 0: charID = args[2]; break;
				case 1: charID = lsd.getVar(args[2]); break;
				PP_defaultThrow();
			}

			switch(args[0]) {
			// add member
				case 0:
					if( mem.size() < MEMBER_MAX ) mem.push_back(charID);
					break;
			// remove member
				case 1: {
					std::vector< uint16_t >::iterator it = find( mem.begin(), mem.end(), charID );
					if( it != mem.end() ) mem.erase(it);
				} break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}

		PP_codeDef(10410) // change exp value
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array2D& charDatas = lsd.character();
		// target
			std::vector< uint16_t > target;
			switch( args[0] ) {
				case 0: // all party member
					target = lsd.member();
					break;
				case 1: // immediate
					target.push_back( args[1] );
					break;
				case 2: // V[a]
					target.push_back( lsd.getVar( args[1] ) );
					break;
				PP_defaultThrow();
			}
		// change
			int val;
			switch( args[3] ) {
				case 0: // immediate
					val = args[4];
					break;
				case 1: // V[b]
					val = lsd.getVar( args[4] );
					break;
				PP_defaultThrow();
			}
			switch( args[2] ) {
				case 0: val =  val; break; // increase
				case 1: val = -val; break; // decrease
				PP_defaultThrow();
			}

			for(std::vector< uint16_t >::const_iterator it = target.begin(); it  != target.end(); ++it) {
				charDatas[*it][32] = charDatas[*it][32].get<int>() + val;
				if( proj.canLevelUp(*it) && args[5] ) {
					return NOT_IMPLEMENTED;
				}
			}

			return SUCCESS;
		}
		PP_codeDef(10420) // change level
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array2D& charDatas = lsd.character();
		// target
			std::vector< uint16_t > target;
			switch( args[0] ) {
				case 0: target = lsd.member(); break;
				case 1: target.push_back( args[1] ); break;
				case 2: target.push_back( lsd.getVar( args[1] ) ); break;
				PP_defaultThrow();
			}
		// change
			int val;
			switch( args[3] ) {
				case 0: val = args[4]; break;
				case 1: val = lsd.getVar( args[4] ); break;
				PP_defaultThrow();
			}
			switch( args[2] ) {
				case 0: val =  val; break; // increase
				case 1: val = -val; break; // decrease
				PP_defaultThrow();
			}

			for(std::vector< uint16_t >::const_iterator it = target.begin(); it  != target.end(); ++it) {
				int tmp = charDatas[*it][32].get<int>() + val;

				// check range
				if(tmp < LV_MIN) tmp = LV_MIN;
				else if(tmp > LV_MAX) tmp = LV_MAX;

				if( ( tmp != charDatas[*it][32].get<int>() ) && args[5] ) {
					charDatas[*it][32] = tmp;
					return NOT_IMPLEMENTED;
				}
			}

			return SUCCESS;
		}
		PP_codeDef(10430) // change character parameter
		{
			SaveData& lsd = getOwner().getProject().getLSD();
			structure::Array2D& charDatas = lsd.character();

			std::vector< uint16_t > target;
			switch(args[0]) {
				case 0: target = lsd.member(); break;
				case 1: target.push_back(args[1]); break;
				case 2: target.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}

			int index, max, min;
			switch(args[3]) {
				case Param::HP:
					max = CHAR_HP_MAX; min = PARAM_MIN;
					index = 33 + args[3];
					break;
				case Param::MP:
					max =   PARAM_MAX; min =    MP_MIN;
					index = 33 + args[3];
					break;
				case Param::ATTACK: case Param::GAURD: case Param::MIND: case Param::SPEED:
					max =   PARAM_MAX; min = PARAM_MIN;
					index = 41 + args[3] - Param::ATTACK;
					break;
				PP_defaultThrow();
			}

			int val = args[5];
			if(val == 0) val = 1;
			else switch(args[4]) {
				case 0: break;
				case 1: val = lsd.getVar(val); break;
				PP_defaultThrow();
			}
			switch(args[2]) {
				case 0: val =  val; break;
				case 1: val = -val; break;
				PP_defaultThrow();
			}

			for( std::vector< uint16_t >::const_iterator it = target.begin(); it != target.end(); it++ ) {
				charDatas[*it][index] = charDatas[*it][index].get<int>() + val;
			}

			return SUCCESS;
		}
		PP_codeDef(10440) // add or remove skill
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			std::vector< uint16_t > target;
			switch(args[0]) {
				case 0: target = lsd.member(); break;
				case 1: target.push_back(args[1]); break;
				case 2: target.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}

			int val;
			switch(args[3]) {
				case 0: val = args[4]; break;
				case 1: val = lsd.getVar( args[4] ); break;
				PP_defaultThrow();
			}
			if( val == 0 ) val = ID_MIN;

			for(std::vector< uint16_t >::const_iterator it = target.begin(); it != target.end(); ++it) {
				std::vector< uint16_t >& skill = lsd.skill( *it );
				switch(args[2]) {
					case 0:
						if( std::find( skill.begin(), skill.end(), val ) == skill.end() ) {
							skill.push_back(val);
						}
						break;
					case 1: {
						std::vector< uint16_t >::iterator pos = std::find( skill.begin(), skill.end(), val );
						if( pos != skill.end() ) skill.erase(pos);
					} break;
					PP_defaultThrow();
				}
			}

			return SUCCESS;
		}
		PP_codeDef(10450) // change equipment
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = getOwner().getProject().getLSD();

			std::vector< uint16_t > target;
			switch(args[0]) {
				case 0: target = lsd.member(); break;
				case 1: target.push_back(args[1]); break;
				case 2: target.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}

			switch(args[2]) {
			// equip
				case 0: {
					uint itemID;
					switch(args[4]) {
						case 0: itemID = (args[3] == 0) ? 1 : args[3]; break;
						case 1: itemID = lsd.getVar(args[3]); break;
						PP_defaultThrow();
					}

					for(std::vector< uint16_t >::const_iterator it = target.begin(); it != target.end(); it++) {
						if( !proj.equip(*it, itemID) ) break;
					}
				} break;
			// unequip
				case 1: {
					uint start, end;
					switch(args[3]) {
						case Equip::WEAPON: case Equip::SHIELD: case Equip::ARMOR: case Equip::HELMET: case Equip::OTHER:
							start = end = args[3]; break;
						case Equip::END: // all
							start = 0; end = Equip::END-1; break;
						PP_defaultThrow();
					}
					for(std::vector< uint16_t >::const_iterator it = target.begin(); it != target.end(); ++it) {
						proj.unequip( *it, static_cast< Equip::Type >(end) );
					}
				} break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(10460) // change HP
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array2D& charDatas = lsd.character();
			std::vector< uint > charIDs;
		// get target
			switch(args[0]) {
				case 0: {
					std::vector< uint16_t >& member = lsd.member();
					for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
				} break;
				case 1: charIDs.push_back(args[1]); break;
				case 2: charIDs.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}
		// get value
			int val;
			switch(args[3]) {
				case 0: val = args[4]; break;
				case 1: val = lsd.getVar(args[4]); break;
				PP_defaultThrow();
			}
			if(val == 0) val = ID_MIN;

			switch(args[2]) {
				case 0: val =  val; break;
				case 1: val = -val; break;
				PP_defaultThrow();
			}
		// set value
			for( std::vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
				structure::Element& e = charDatas[*it][71];

				int result = e.get<int>() + val;
			// check maximum
				if( result > proj.param(*it, Param::HP) ) {
					result = e;
			// check minimum and knock out
				} else if(result <= CHAR_HP_MIN) {
					switch(args[5]) {
						case 0: result = 1;
						case 1: result = 0;
						PP_defaultThrow();
					}
				}

				e = result;
			}

			return SUCCESS;
		}
		PP_codeDef(10470) // change MP
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array2D& charDatas = lsd.character();
			std::vector< uint > charIDs;
		// get target
			switch(args[0]) {
				case 0: {
					std::vector< uint16_t >& member = lsd.member();
					for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
				} break;
				case 1: charIDs.push_back(args[1]); break;
				case 2: charIDs.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}
		// get value
			int val = args[4];
			if(val == 0) val = 1;
			else switch(args[3]) {
				case 0: break;
				case 1: val = lsd.getVar(val); break;
				PP_defaultThrow();
			}

			switch(args[2]) {
				case 0: val =  val; break;
				case 1: val = -val; break;
				PP_defaultThrow();
			}
		// set value
			for( std::vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
				structure::Element& e = charDatas[*it][72];

				int result = e.get<int>() + val;
			// check maximum
				if( result > proj.param(*it, Param::MP) ) result = e;
			// check minimum
				else if(result < MP_MIN) result = MP_MIN;

				e = result;
			}

			return SUCCESS;
		}
		PP_codeDef(10480) // set condition
		{
		/*
			SaveData& lsd = getOwner().getProject().getLSD();
			structure::Array2D& charDatas = lsd.character();
			std::vector< uint > charIDs;

			switch(args[0]) {
				case 0: {
					Array< uint16_t >& member = lsd.member();
					for( uint i = 0; i < member.length(); i++ ) charIDs.push_back(member[i]);
				} break;
				case 1: charIDs.push_back(args[1]); break;
				case 2: charIDs.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}

			for( std::vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
				charDatas[*it][71] = (int)charDatas[*it][33];
			}

			return SUCCESS;
		 */
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10490) // full cure
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array2D& charDatas = lsd.character();
			std::vector< uint > charIDs;

			switch(args[0]) {
				case 0: {
					std::vector< uint16_t >& member = lsd.member();
					for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
				} break;
				case 1: charIDs.push_back(args[1]); break;
				case 2: charIDs.push_back( lsd.getVar(args[1]) ); break;
				PP_defaultThrow();
			}

			for( std::vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
				charDatas[*it][71] = proj.param(*it, Param::HP);
				charDatas[*it][72] = proj.param(*it, Param::MP);
			}

			return SUCCESS;
		}
		PP_codeDef(10500) // damage character
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(10610) // change character name
		{
			getOwner().getProject().getLSD().character()[ args[0] ][1] = strArg;

			return SUCCESS;
		}
		PP_codeDef(10620) // change character title
		{
			getOwner().getProject().getLSD().character()[ args[0] ][2] = strArg;

			return SUCCESS;
		}
		PP_codeDef(10630) // change character graphic
		{
			structure::Array1D& charData = getOwner().getProject().getLSD().character()[ args[0] ];

			charData[11] = strArg;
			charData[12] = args[1];

			return NOT_IMPLEMENTED;
			switch(args[2]) {
				case 0:
					break;
				case 1:
					break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(10640) // change face graphic
		{
			structure::Array1D& charData = getOwner().getProject().getLSD().character()[ args[0] ];

			charData[21] = strArg;
			charData[22] = args[1];

			return SUCCESS;
		}
		PP_codeDef(10650) // change transport graphic
		{
			structure::EventState& info = getOwner().getProject().getLSD()[ 105+args[0] ];
			info[73] = strArg;
			info[74] = args[1];

			return SUCCESS;
		}
		PP_codeDef(10660) // change system BGM
		{
			int index;
			switch(args[0]) {
				case 0: case 1: case 2:
					index = 72 + args[0];
					break;
				case 3: case 4: case 5: case 6:
					index = 79 + args[0];
					break;
				PP_defaultThrow();
			}
			structure::Music& dst = getOwner().getProject().getLSD()[101].getArray1D()[index];
			dst[1] = strArg;
			dst[2] = args[1];
			dst[3] = args[2];
			dst[4] = args[3];
			dst[5] = args[4];

			return SUCCESS;
		}
		PP_codeDef(10670) // change system SE
		{
			structure::Sound& dst = getOwner().getProject().getLSD()[101].getArray1D()[ 91 + args[0] ];
			dst[1] = strArg;
			dst[3] = args[1];
			dst[4] = args[2];
			dst[5] = args[3];

			return SUCCESS;
		}
		PP_codeDef(10680) // change system graphic
		{
			structure::Array1D& lsdSys = getOwner().getProject().getLSD()[101];
			structure::Array1D& ldbSys = getOwner().getProject().getLDB()[22];

			if( ldbSys[19] != strArg ) lsdSys[21] = strArg;

			if( ldbSys[71].get<int>() != args[0] ) lsdSys[22] = args[0];
			if( ldbSys[72].get<int>() != args[1] ) lsdSys[23] = args[1];

			return SUCCESS;
		}
		PP_codeDef(10690) // change screen refresh
		{
			structure::Array1D& lsdSys = getOwner().getProject().getLSD()[101];
			structure::Array1D& ldbSys = getOwner().getProject().getLDB()[22];

			if( ldbSys[ 61+args[0] ].get<int>() != args[1]) {
				lsdSys[ 111+args[0] ].getBinary()[0] = args[1];
			}

			return SUCCESS;
		}

	/*
	 * junction event checks the rpg2k::Main::getExecute().currentInst()
	 * so no need to pass values
	 *
	 * todo: skipping junction after returnGameMode()
	 */
		PP_codeDef(10710) // battle junction
		{
			getOwner().getOwner().callGameMode(GameMode::Battle);
			return NEXT_FRAME;
		}
		PP_codeDef(20710)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20711)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20712)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20713)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10720) // shop junction
		{
			getOwner().getOwner().callGameMode(GameMode::Shop);
			return NEXT_FRAME;
		}
		PP_codeDef(20720)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20721)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20722)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10730) // hotel junction
		{
			getOwner().getOwner().callGameMode(GameMode::Hotel);
			return NEXT_FRAME;
		}
		PP_codeDef(20730)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20731)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(20732)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10740) // input character name
		{
			getOwner().getOwner().callGameMode(GameMode::NameInput);
			return NEXT_FRAME;
		}

		PP_codeDef(10810) // move to a place
		{
			Project& proj = getOwner().getProject();
			proj.move( args[0], args[1], args[2] );

			// set char direction
			std::vector< int > arg;
			int act;
			switch( args[3] ) {
				case 1: act = Action::Face::UP   ; break; // up
				case 2: act = Action::Face::RIGHT; break; // right
				case 3: act = Action::Face::DOWN ; break; // down
				case 4: act = Action::Face::LEFT ; break; // left
				PP_defaultThrow();
			}
			if(args[3] != 0) proj.processAction(EV_ID_PARTY, act, arg);

			return SUCCESS;
		}
		PP_codeDef(10820) // memorize current point
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::EventState& party = lsd.eventState(EV_ID_PARTY);
			lsd.setVar( args[0], party.mapID() );
			lsd.setVar( args[1], party.x() );
			lsd.setVar( args[2], party.y() );

			return SUCCESS;
		}
		PP_codeDef(10830) // move to the memorized point
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			proj.move( lsd.getVar(args[0]), lsd.getVar(args[1]), lsd.getVar(args[2]) );

			return SUCCESS;
		}
		PP_codeDef(10840) // get on to the transport
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(10850) // set transport position
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			std::deque< uint > point;
			switch(args[1]) {
				case 0:
					for(uint i = 0; i < 3; i++) point.push_back(args[i+1]);
					break;
				case 1:
					for(uint i = 0; i < 3; i++) point.push_back( lsd.getVar(args[i+1]) );
					break;
				PP_defaultThrow();
			}

			switch(args[0]) {
				case 0: case 1: case 2: {
					structure::EventState& state = lsd.eventState(EV_ID_BOAT + args[0]);
					std::deque< uint >::const_iterator it = point.begin();

					state[11] = *it;
					state[12] = *(++it);
					state[13] = *(++it);
				} break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(10860) // set event position
		{
			SaveData& lsd = getOwner().getProject().getLSD();
			structure::EventState& state = lsd.eventState(args[0]);

			uint x, y;
			switch(args[1]) {
				case 0:
					x = args[2];
					y = args[3];
					break;
				case 1:
					x = lsd.getVar(args[2]);
					y = lsd.getVar(args[3]);
					break;
				PP_defaultThrow();
			}

			state[12] = x;
			state[13] = y;

			return SUCCESS;
		}
		PP_codeDef(10870) // swap event position
		{
			SaveData& lsd = getOwner().getProject().getLSD();

			structure::EventState& stateA = lsd.eventState(args[0]);
			structure::EventState& stateB = lsd.eventState(args[1]);

			uint xBuff = stateA.x(), yBuff = stateA.y();

			stateA[12] = stateB.x();
			stateA[13] = stateB.y();

			stateB[12] = xBuff;
			stateB[13] = yBuff;

			return SUCCESS;
		}

		PP_codeDef(10910) // get terrain id
		{
			Project& proj = getOwner().getProject();
			MapUnit & lmu = proj.getLMU();
			SaveData& lsd = proj.getLSD();

			uint x, y;
			switch(args[0]) {
				case 0:
					x = args[1];
					y = args[2];
					break;
				case 1:
					x = lsd.getVar(args[2]);
					y = lsd.getVar(args[3]);
					break;
				PP_defaultThrow();
			}

			lsd.setVar(
				args[3],
				proj.getTerrainID( lmu.chipIDLw(x, y) )
			);

			return SUCCESS;
		}
		PP_codeDef(10920) // get event id
		{
			SaveData& lsd = getOwner().getProject().getLSD();
			structure::Array2D& states = lsd.eventState();

			int x, y;
			switch(args[0]) {
				case 0:
					x = args[1];
					y = args[2];
					break;
				case 1:
					x = lsd.getVar(args[1]);
					y = lsd.getVar(args[2]);
					break;
				PP_defaultThrow();
			}

			uint result = 0;
			for(structure::Array2D::Iterator it = states.begin(); it != states.end(); ++it) {
				if(
					it.second().exists() &&
					( it.second()[12].get<int>() == x ) &&
					( it.second()[13].get<int>() == y )
				) result = it.first();
			}

			lsd.setVar(args[3], result);

			return SUCCESS;
		}

		PP_codeDef(11010) // erase screen
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11020) // show screen
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11030) // set screen color tone
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11040) // flash screen
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11050) // shake screen
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11060) // scroll screen
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11070) // set weather effect
		{
			return NOT_IMPLEMENTED;
		}


		PP_codeDef(11110) // show picture
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11120) // move picture
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11130) // erase picture
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(11210) // battle anime at field
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(11310) // transparent party
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11320) // flash event
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11330) // set move
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11340) // process all moves
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11350) // dispose all moves
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(11410) // wait
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(11510) // play music
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Music& mus = lsd[101].getArray1D()[75];

			mus.clear();
			mus[1] = strArg;
			if(args[0] != FADE_IN_DEF) mus[2] = args[0];
			if(args[1] != VOLUME_DEF) mus[3] = args[1];
			if(args[2] != TEMPO_DEF) mus[4] = args[2];
			if(args[3] != BALANCE_DEF) mus[5] = args[3];

			getOwner().getOwner().getAudio2D().playMusic(mus);

			return SUCCESS;
		}
		PP_codeDef(11520) // fade out music
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11530) // memorize music
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array1D& sys = lsd[101];

			sys[78] = sys[75].getMusic();

			return SUCCESS;
		}
		PP_codeDef(11540) // play memorized music
		{
			Project& proj = getOwner().getProject();
			SaveData& lsd = proj.getLSD();
			structure::Array1D& sys = lsd[101];

			sys[75] = sys[78].getMusic();

			getOwner().getOwner().getAudio2D().playMusic( sys[75] );

			return SUCCESS;
		}
		PP_codeDef(11550) // play sound
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11560) // play movie
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(11610) // get key input
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(11710) // change chip set
		{
			if( getOwner().getProject().getLMU()[1].get<int>() != args[0] ) {
				getOwner().getProject().getLSD()[111].getArray1D()[5] = args[0];
			}

			return SUCCESS;
		}
		PP_codeDef(11720) // change panorama
		{
		/*
			structure::Array1D& state = getOwner().getProject().getLSD()[111];
			MapUnit& lmu = getOwner().getProject().getLMU();

			[32]: string panorama;
			[33]: bool horizontalScroll;
			[34]: bool verticalScroll;
			[35]: bool horizontalScrollAuto;
			[36]: int horizontalScrollSpeed;
			[37]: bool verticalScrollAuto;
			[38]: int verticalScrollSpeed;

			return SUCCESS;
		 */
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(11740) // change encount step
		{
			Project& proj = getOwner().getProject();
			if( proj.getLMT( proj.getCurrentMapID() )[44].get<int>() != args[0] ) {
				proj.getLSD()[111].getArray1D()[3] = args[0];
			}

			return SUCCESS;
		}
		PP_codeDef(11750) // replace chip
		{
			getOwner().getProject().getLSD().replace( (ChipSet::Type)args[0], args[1], args[2] );

			return SUCCESS;
		}

		PP_codeDef(11810) // add teleport link
		{
			structure::Array2D& points = getOwner().getProject().getLSD()[110];

			switch(args[0]) {
			// add teleport link
				case 0: {
					structure::Array1D& point =  points[ args[1] ];
					point[1] = args[1];
					point[2] = args[2];
					point[3] = args[3];
					switch(args[4]) {
						case 0:
							point[4] = false;
							break;
						case 1:
							point[4] = true;
							point[5] = args[5];
							break;
					}
				} break;
			// remove teleport link
				case 1: points.remove(args[1]); break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(11820) // disable or enable teleport
		{
			getOwner().getProject().getLSD()[101].getArray1D()[121] = (bool)args[0];

			return SUCCESS;
		}
		PP_codeDef(11830) // set escape link
		{
			structure::Array1D& point = getOwner().getProject().getLSD()[110].getArray2D()[0];

			point[1] = args[0];
			point[2] = args[1];
			point[3] = args[2];
			switch(args[3]) {
				case 0:
					point[4] = false;
					break;
				case 1:
					point[4] = true;
					point[5] = args[4];
					break;
				PP_defaultThrow();
			}

			return SUCCESS;
		}
		PP_codeDef(11840) // disable or enable escape
		{
			getOwner().getProject().getLSD()[101].getArray1D()[122] = (bool)args[0];

			return SUCCESS;
		}
		PP_codeDef(11910) // call save screen
		{
			// getOwner().getOwner().setGameMode(MODE_Save);

			return NEXT_FRAME;
		}
		PP_codeDef(11930) // disable or enable saving
		{
			getOwner().getProject().getLSD()[101].getArray1D()[123] = (bool)args[0];

			return SUCCESS;
		}
		PP_codeDef(11950) // call menu
		{
			// getOwner().getOwner().setGameMode(MODE_Menu);

			return NEXT_FRAME;
		}
		PP_codeDef(11960) // disable or enable menu
		{
			getOwner().getProject().getLSD()[101].getArray1D()[124] = (bool)args[0];

			return SUCCESS;
		}

		PP_codeDef(12010) // junction
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(22010)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(22011)
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(12110) // set label
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(12120) // goto label
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(12210) // loop
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(22210)
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(12220) // interrupt loop
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(12310) // interrupt event
		{
			getOwner().popExec();
			return SUCCESS;
		}
		PP_codeDef(12320) // erase event
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(12330) // call event
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(12410) // comment
		{
			clog << strArg.toSystem() << endl;
			return SUCCESS;
		}
		PP_codeDef(22410)
		{
			clog << strArg.toSystem() << endl;
			return SUCCESS;
		}
		PP_codeDef(12420) // gameover
		{
			getOwner().getOwner().callGameMode(GameMode::GameOver);
			return NEXT_FRAME;
		}

		PP_codeDef(12510) // title
		{
			getOwner().getOwner().gotoTitle();
			return NEXT_FRAME;
		}

	// battle events
		PP_codeDef(13110) // change enemy HP
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(13120) // change enemy MP
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(13130) // change enemy condition
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(13150) // appear enemy
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(13210) // change backdrop
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(13260) // show battle animation
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(13310) // junction true  way
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(23310) // junction false way
		{
			return NOT_IMPLEMENTED;
		}
		PP_codeDef(23311) // end of junction
		{
			return NOT_IMPLEMENTED;
		}

		PP_codeDef(13410) // interrupt battle
		{
			return NOT_IMPLEMENTED;
		}

		#undef PP_codeDef
	} // namespace model
} // namespace rpg2k
