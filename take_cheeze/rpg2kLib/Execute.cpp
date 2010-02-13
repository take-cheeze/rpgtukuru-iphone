#define USE_EVENT_COMMAND

#include <algorithm>

#include "Encode.hpp"
#include "Execute.hpp"

using namespace rpg2kLib::encode;
using namespace rpg2kLib::model;

typedef multimap< uint, uint >::const_iterator x_it;
typedef map< uint, multimap< uint, uint > >::const_iterator y_it;

typedef map< uint, Execute::ExecInfo >::const_iterator info_it;

typedef Array1D::Iterator A1D_it;
typedef Array2D::Iterator A2D_it;

void Execute::refresh(
	vector< map< uint, multimap< uint, uint > > >& eventMap,
	map< uint, uint >& pageNo
) {
	SaveData& lsd = getProject().getLSD();

	EVENT_MAP = &eventMap;
	PAGE_NO = &pageNo;

	EXEC_INFO.clear();
	EXEC_INFO.resize( 5, map< uint, ExecInfo >() );
// get page number and map map events
	Array2D& mapEv = getProject().getLMU()[81];
	for(A2D_it it = mapEv.begin(); it != mapEv.end(); ++it) {
		int eventID = it.first(), pageNo = (*PAGE_NO)[eventID];
		if(pageNo == 0) continue;
		Array1D& page = ( (Array2D&) mapEv[eventID][5] )[pageNo];

		ExecInfo info =
		{
			EV_MAP, eventID,
			0, false,
			(EventStart) (int)page[33], pageNo, &( (Event&)page[52] )
		};

		EXEC_INFO[EV_MAP].insert( map< int, ExecInfo >::value_type(eventID, info) );
	}
// map common events
	Array2D& comEv = getProject().getLDB()[25];
	for(A2D_it it = comEv.begin(); it != comEv.end(); ++it) {
		int eventID = it.first();
		Array1D& cur = it.second();
		if(
			( (int)cur[11] != EV_CALLED) &&
			( !(bool)cur[12] || ( (bool)cur[12] && lsd.getFlag(cur[13]) ) )
		) {

			ExecInfo info =
			{
				EV_MAP, eventID,
				0, false,
				(EventStart) (int)cur[11], 0, &( (Event&)cur[22] )
			};

			clog << hex << (uint)info.event << dec << endl;

			EXEC_INFO[EV_COMMON].insert( map< int, ExecInfo >::value_type(eventID, info) );
		}
	}
	nextEvent(CUR_EXEC);
}

void Execute::nextEvent(ExecInfo& curState)
{
/*
	Project& proj = getProject();

	EXEC_INFO[curState.type].erase(curState.id);
	map< uint, ExecInfo >& maps = EXEC_INFO[EV_MAP], coms = EXEC_INFO[EV_COMMON];

	// auto start map event
	for(info_it it = maps.begin(); it != maps.end(); ++it)
		if(it->second.start == EV_AUTO) { curState = it->second; return; }
	// parallel map event
	for(info_it it = maps.begin(); it != maps.end(); ++it)
		if(it->second.start == EV_PARALLEL) { curState = it->second; return; }

	// auto start common event
	for(info_it it = coms.begin(); it != coms.end(); ++it)
		if(it->second.start == EV_AUTO) { curState = it->second; return; }
	// parallel common event
	for(info_it it = coms.begin(); it != coms.end(); ++it)
		if(it->second.start == EV_PARALLEL) { curState = it->second; return; }

	if( !OWNER.touchFromEvent().empty() ) {
		stack< uint >& IDs = OWNER.touchFromEvent();
		for( ; !IDs.empty(); IDs.pop() )
			if(maps[IDs.top()].start == EV_EVENT_TOUCH) {
				curState = maps[IDs.top()];
				return;
			}
	}
	if( !OWNER.touchFromParty().empty() ) {
		stack< uint >& IDs = OWNER.touchFromParty();
		for( ; !IDs.empty(); IDs.pop() )
			if(maps[IDs.top()].start == EV_PARTY_TOUCH) {
				curState = maps[IDs.top()];
				return;
			}
	}

	if( OWNER.getKeyListener().enter() ) {
		EventState& party = proj.getLSD().eventState(EV_ID_PARTY);
		EventDir dir = party.eventDir();
		uint curX = party.x(), curY = party.y();
		uint nxtX = gamemode::nextX(dir, curX), nxtY = gamemode::nextY(dir, curY);

		if( gamemode::exists( (*EVENT_MAP)[EV_PR_CHAR], nxtX, nxtY ) ) {
			for(
				x_it it = (*EVENT_MAP)[EV_PR_CHAR].find(nxtY)->second.find(nxtX);
				it->first == nxtX; ++it
			)
				if(maps[it->second].start == EV_KEY_ENTER) {
					curState = maps[it->second];
					return;
				}
		} else if( gamemode::exists( (*EVENT_MAP)[EV_PR_BELOW], curX, curY ) ) {
			for(
				x_it it = (*EVENT_MAP)[EV_PR_CHAR].find(curY)->second.find(curX);
				it->first == curX; ++it
			)
				if(maps[it->second].start == EV_KEY_ENTER) {
					curState = maps[it->second];
					return;
				}
		} else if( gamemode::exists( (*EVENT_MAP)[EV_PR_ABOVE], curX, curY ) ) {
			for(
				x_it it = (*EVENT_MAP)[EV_PR_CHAR].find(curY)->second.find(curX);
				it->first == curX; ++it
			)
				if(maps[it->second].start == EV_KEY_ENTER) {
					curState = maps[it->second];
					return;
				}
		}
	}

	throw "All event execute ended.";
 */
}

const Instruction& Execute::currentInst()
{
	if( (uint)CUR_EXEC.ip >= CUR_EXEC.event->length() ) nextEvent(CUR_EXEC);

	return (*CUR_EXEC.event)[CUR_EXEC.ip++];
}

void Execute::skipToEndOfJunction(uint eventCode)
{
}
void Execute::skipToElse(uint eventCode)
{
}

#define PP_codeDec(codeNo) \
	ExecuteState Execute::code##codeNo(const Instruction& inst)
#define PP_checkInvalidEnum() default: return EXE_FAILED

// end of event
PP_codeDec(0)
{
	nextEvent(CUR_EXEC);
	return EXE_SUCCESS;
}
// end of junction
PP_codeDec(10)
{
	return EXE_SUCCESS;
}

// show message
PP_codeDec(10110)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20110)
{
	return EXE_NOT_IMPLEMENTED;
}
// set message config
PP_codeDec(10120)
{
	
	return EXE_NOT_IMPLEMENTED;
}
// set message face set
PP_codeDec(10130)
{
/*
	Array1D& lsdSys = getProject().getLSD()[101];
	Array1D& ldbSys = getProject().getLDB()[22];

	

	return EXE_SUCCESS;
 */
	return EXE_NOT_IMPLEMENTED;
}
// select junction
PP_codeDec(10140)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20140)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20141)
{
	return EXE_NOT_IMPLEMENTED;
}
// input number
PP_codeDec(10150)
{
	return EXE_NOT_IMPLEMENTED;
}

// change switch
PP_codeDec(10210)
{
	SaveData& lsd = getProject().getLSD();

	int start, end;

	switch(inst[0]) {
		case 0: start = end = inst[1];
			break;
		case 1: start = end = lsd.getVar(inst[1]);
			break;
		case 2: start = inst[1]; end = inst[2];
			break;
		PP_checkInvalidEnum();
	}
	switch(inst[3]) {
		case 0: for(;end >= start; end--) lsd.setFlag(end, true ); break;
		case 1: for(;end >= start; end--) lsd.setFlag(end, false); break;
		case 2:	for(;end >= start; end--)
			lsd.setFlag( end, !lsd.getFlag(end) );
			break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// change variable
PP_codeDec(10220)
{
	SaveData& lsd = getProject().getLSD();

	int start, end;

	switch(inst[0]) {
		case 0: start = end = inst[1]; break;
		case 2: start = end = lsd.getVar(inst[1]); break;
		case 1: start = inst[1]; end = inst[2]; break;
		PP_checkInvalidEnum();
	}

	int32_t op;
	int32_t C = inst[5], D = inst[6];
	switch(inst[4]) {
		case 0: op = C; break;
		case 1: op = lsd.getVar(C); break;
		case 2: op = lsd.getVar( lsd.getVar(C) ); break;
		case 3: op = random(C, D); break;
	// item
		case 4:
			switch(inst[6]) {
			// item number you have
				case 0: op = lsd.getItemNum(inst[5]); break;
			// item number your characters equiped
				case 1:
					op = 0; break;
				PP_checkInvalidEnum();
			}
			break;
	// char status
		case 5: {
			Array1D& target = lsd.charParam(C);
			switch(D) {
				case  0: case  1: op = target[31+D  ]; break;
				case  2: case  3: op = target[71+D-2]; break;
				case  4: case  5: op = target[33+D-4]; break;
				case  6: case  7: case  8: case  9: op = target[41+D-6]; break;
			// equipment
				case 10: case 11: case 12: case 13: case 14: {
					 vector< uint16_t > equip = static_cast< Binary& >(target[61]);
					 op = equip[D-10];
				} break;
				PP_checkInvalidEnum();
			}
		} break;
	// event info
		case 6: {
			EventState& state = lsd.eventState(C);
			switch(D) {
				case 0: case 1: case 2: op = state[11+D]; break;
				case 3:
				case 4:
				case 5:
					op = 0;
					break;
				PP_checkInvalidEnum();
			}
		} break;
	// others
		case 7:
			switch(D) {
				case 0: op = lsd.getMoney(); break;
				case 1:
					op = 0; break;
				case 2: op = lsd.member().size(); break;
				case 3: op = static_cast< Array1D& >(lsd[101])[131]; break;
				case 4: op = static_cast< Array1D& >(lsd[109])[32]; break;
				case 5: op = static_cast< Array1D& >(lsd[109])[34]; break;
				case 6: op = static_cast< Array1D& >(lsd[109])[33]; break;
				case 7: op = static_cast< Array1D& >(lsd[109])[35]; break;
				case 8:
				case 9:
					op = 0; break;
				PP_checkInvalidEnum();
			}
			break;
	// enemy status
		case 8:
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
					op = 0; break;
				PP_checkInvalidEnum();
			}
			break;
		PP_checkInvalidEnum();
	}

	switch(inst[3]) {
		case 0: for(;end >= start; end--) lsd.setVar(end, op); break;
		case 1: for(;end >= start; end--)
			lsd.setVar(end, lsd.getVar(end) + op); break;
		case 2: for(;end >= start; end--)
			lsd.setVar(end, lsd.getVar(end) - op); break;
		case 3: for(;end >= start; end--)
			lsd.setVar(end, lsd.getVar(end) * op); break;
		case 4: for(;end >= start; end--)
			lsd.setVar(end, lsd.getVar(end) / op); break;
		case 5: for(;end >= start; end--)
			lsd.setVar(end, lsd.getVar(end) % op); break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// set timer
PP_codeDec(10230)
{
	return EXE_NOT_IMPLEMENTED;
}

// set money
PP_codeDec(10310)
{
	SaveData& lsd = getProject().getLSD();

	int val;
	switch(inst[1]) {
		case 0: val = inst[2];
			break;
		case 1: val = lsd.getVar(inst[2]);
			break;
		PP_checkInvalidEnum();
	}
	switch(inst[0]) {
		case 0: /* val =  val; */ break;
		case 1:    val = -val;    break;
		PP_checkInvalidEnum();
	}

	lsd.setMoney( lsd.getMoney() + val );

	return EXE_SUCCESS;
}
// set item
PP_codeDec(10320)
{
	SaveData& lsd = getProject().getLSD();

	int id;
	switch(inst[1]) {
		case 0: id = inst[2]; break;
		case 1: id = lsd.getVar(inst[2]); break;
		PP_checkInvalidEnum();
	}

	int val;
	switch(inst[3]) {
		case 0: val = inst[4]; break;
		case 1: val = lsd.getVar(inst[4]); break;
		PP_checkInvalidEnum();
	}
	switch(inst[0]) {
		case 0: /* val =  val; */ break;
		case 1:    val = -val;    break;
		PP_checkInvalidEnum();
	}

	lsd.setItemNum( id, lsd.getItemNum(id)+val );

	return EXE_SUCCESS;
}
// exchange member
PP_codeDec(10330)
{
	SaveData& lsd = getProject().getLSD();
	vector< uint16_t >& mem = lsd.member();

	uint charID;
	switch(inst[1]) {
		case 0: charID = inst[2]; break;
		case 1: charID = lsd.getVar(inst[2]); break;
		PP_checkInvalidEnum();
	}

	switch(inst[0]) {
	// add member
		case 0:
			if( mem.size() < MEMBER_MAX ) mem.push_back(charID);
			break;
	// remove member
		case 1: {
			vector< uint16_t >::iterator it =
				find( mem.begin(), mem.end(), charID );
			if( it != mem.end() ) mem.erase(it);
		} break;
		PP_checkInvalidEnum();
	}

	return EXE_NOT_IMPLEMENTED;
}

// change exp value
PP_codeDec(10410)
{
	return EXE_NOT_IMPLEMENTED;
}
// change level
PP_codeDec(10420)
{
	return EXE_NOT_IMPLEMENTED;
}
// change character parameter
PP_codeDec(10430)
{
	SaveData& lsd = getProject().getLSD();
	Array2D& charDatas = lsd.charParam();
	vector< uint > charIDs;

	switch(inst[0]) {
		case 0: {
			vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(inst[1]); break;
		case 2: charIDs.push_back( lsd.getVar(inst[1]) ); break;
		PP_checkInvalidEnum();
	}

	int index, max, min;
	switch(inst[3]) {
		case 0:
			max = CHAR_HP_MAX; min = PARAM_MIN;
			index = 33 + inst[3];
			break;
		case 1:
			max =   PARAM_MAX; min =    MP_MIN;
			index = 33 + inst[3];
			break;
		case 2: case 3: case 4: case 5:
			max =   PARAM_MAX; min = PARAM_MIN;
			index = 41 + inst[3] - 2;
			break;
		PP_checkInvalidEnum();
	}

	int val = inst[5];
	if(val == 0) val = 1;
	else switch(inst[4]) {
		case 0: break;
		case 1: val = lsd.getVar(val); break;
		PP_checkInvalidEnum();
	}

	switch(inst[2]) {
		case 0: /* val =  val; */ break;
		case 1:    val = -val;    break;
		PP_checkInvalidEnum();
	}

	for( vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		Element& e = charDatas[*it][index];

		int result = e + val;
	// check maximum
		if(result > max) result = e;
	// check minimum
		else if(result < min) result = min;

		e = result;
	}

	return EXE_SUCCESS;
}
// add or remove skill
PP_codeDec(10440)
{
	return EXE_NOT_IMPLEMENTED;
}
// change equipment
PP_codeDec(10450)
{
	Project& proj = getProject();
	SaveData& lsd = getProject().getLSD();
	vector< uint > charIDs;

	switch(inst[0]) {
		case 0: {
			vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(inst[1]); break;
		case 2: charIDs.push_back( lsd.getVar(inst[1]) ); break;
		PP_checkInvalidEnum();
	}

	switch(inst[2]) {
	// equip
		case 0: {
			uint itemID;
			switch(inst[4]) {
				case 0: itemID = (inst[3] == 0) ? 1 : inst[3]; break;
				case 1: itemID = lsd.getVar(inst[3]); break;
				PP_checkInvalidEnum();
			}

			for(
				vector< uint >::const_iterator it = charIDs.begin();
				it != charIDs.end(); it++
			) {
				if( !proj.equip(*it, itemID) ) break;
			}
		} break;
	// unequip
		case 1: {
			uint start, end;
			switch(inst[3]) {
				case 0: case 1: case 2: case 3: case 4:
					start = end = inst[3]; break;
				case 5:
					start = 0; end = EQUIP_NUM-1; break;
				PP_checkInvalidEnum();
			}
			for(
				vector< uint >::const_iterator it = charIDs.begin();
				it != charIDs.end(); ++it
			) {
				proj.unequip( *it, static_cast< EquipType >(end) );
			}
		} break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// change HP
PP_codeDec(10460)
{
	SaveData& lsd = getProject().getLSD();
	Array2D& charDatas = lsd.charParam();
	vector< uint > charIDs;
// get target
	switch(inst[0]) {
		case 0: {
			vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(inst[1]); break;
		case 2: charIDs.push_back( lsd.getVar(inst[1]) ); break;
		PP_checkInvalidEnum();
	}
// get value
	int val = inst[4];
	if(val == 0) val = 1;
	else switch(inst[3]) {
		case 0: break;
		case 1: val = lsd.getVar(val); break;
		PP_checkInvalidEnum();
	}

	switch(inst[2]) {
		case 0: /* val =  val; */ break;
		case 1:    val = -val;    break;
		PP_checkInvalidEnum();
	}
// set value
	for( vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		Element& e = charDatas[*it][71];

		int result = e + val;
	// check maximum
		if(result > charDatas[*it][33]) {
			result = e;
	// check minimum and knock out
		} else if(result <= CHAR_HP_MIN) {
			switch(inst[5]) {
				case 0: result = 1;
				case 1: result = 0;
				PP_checkInvalidEnum();
			}
		}

		e = result;
	}

	return EXE_SUCCESS;
}
// change MP
PP_codeDec(10470)
{
	SaveData& lsd = getProject().getLSD();
	Array2D& charDatas = lsd.charParam();
	vector< uint > charIDs;
// get target
	switch(inst[0]) {
		case 0: {
			vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(inst[1]); break;
		case 2: charIDs.push_back( lsd.getVar(inst[1]) ); break;
		PP_checkInvalidEnum();
	}
// get value
	int val = inst[4];
	if(val == 0) val = 1;
	else switch(inst[3]) {
		case 0: break;
		case 1: val = lsd.getVar(val); break;
		PP_checkInvalidEnum();
	}

	switch(inst[2]) {
		case 0: /* val =  val; */ break;
		case 1:    val = -val;    break;
		PP_checkInvalidEnum();
	}
// set value
	for( vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		Element& e = charDatas[*it][72];

		int result = e + val;
	// check maximum
		if(result > charDatas[*it][34]) result = e;
	// check minimum
		else if(result < MP_MIN) result = MP_MIN;

		e = result;
	}

	return EXE_SUCCESS;
}
// set condition
PP_codeDec(10480)
{
/*
	SaveData& lsd = getProject().getLSD();
	Array2D& charDatas = lsd.charParam();
	vector< uint > charIDs;

	switch(inst[0]) {
		case 0: {
			Array< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.length(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(inst[1]); break;
		case 2: charIDs.push_back( lsd.getVar(inst[1]) ); break;
		PP_checkInvalidEnum();
	}

	for( vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		charDatas[*it][71] = (int)charDatas[*it][33];
	}

	return EXE_SUCCESS;
 */
	return EXE_NOT_IMPLEMENTED;
}
// full cure
PP_codeDec(10490)
{
	SaveData& lsd = getProject().getLSD();
	Array2D& charDatas = lsd.charParam();
	vector< uint > charIDs;

	switch(inst[0]) {
		case 0: {
			vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(inst[1]); break;
		case 2: charIDs.push_back( lsd.getVar(inst[1]) ); break;
		PP_checkInvalidEnum();
	}

	for( vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		charDatas[*it][71] = (int)charDatas[*it][33];
		charDatas[*it][72] = (int)charDatas[*it][34];
	}

	return EXE_SUCCESS;
}
// damage character
PP_codeDec(10500)
{
	return EXE_NOT_IMPLEMENTED;
}

// change character name
PP_codeDec(10610)
{
	getProject().getLSD().charParam(inst[0])[1] = static_cast< string >(inst);

	return EXE_SUCCESS;
}
// change character title
PP_codeDec(10620)
{
	getProject().getLSD().charParam(inst[0])[2] = static_cast< string >(inst);

	return EXE_SUCCESS;
}
// change character graphic
PP_codeDec(10630)
{
	Array1D& charData = getProject().getLSD().charParam(inst[0]);

	charData[11] = static_cast< string >(inst);
	charData[12] = inst[1];

/*
	switch(inst[2]) {
		case 0: break;
		case 1:
			charData[] = (bool)true;
		PP_checkInvalidEnum();
	}
 */

	return EXE_SUCCESS;
}
// change face graphic
PP_codeDec(10640)
{
	Array1D& charData = getProject().getLSD().charParam(inst[0]);

	charData[21] = static_cast< string >(inst);
	charData[22] = inst[1];

	return EXE_SUCCESS;
}
// change transport graphic
PP_codeDec(10650)
{
	switch(inst[0]) {
		case 0: case 1: case 2: {
			EventState& info = getProject().getLSD()[ 105+inst[0] ];
			info[73] = static_cast< string >(inst);
			info[74] = inst[1];
		} break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// change system BGM
PP_codeDec(10660)
{
	return EXE_NOT_IMPLEMENTED;
}
// change system SE
PP_codeDec(10670)
{
	return EXE_NOT_IMPLEMENTED;
}
// change system graphic
PP_codeDec(10680)
{
	Array1D& lsdSys = getProject().getLSD()[101];
	Array1D& ldbSys = getProject().getLDB()[22];

	if( ldbSys[19] != static_cast< string >(inst) ) {
		lsdSys[21] = static_cast< string >(inst);
	}

	if(ldbSys[71] != inst[0]) lsdSys[22] = inst[0];
	if(ldbSys[72] != inst[1]) lsdSys[23] = inst[1];

	return EXE_SUCCESS;
}
// change screen refresh
PP_codeDec(10690)
{
	Array1D& lsdSys = getProject().getLSD()[101];
	Array1D& ldbSys = getProject().getLDB()[22];

	if(ldbSys[ 61+inst[0] ] != inst[1]) {
		static_cast< Binary& >(lsdSys[ 111+inst[0] ])[0] = inst[1];
	}

	return EXE_SUCCESS;
}

// battle junction
PP_codeDec(10710)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20710)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20711)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20712)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20713)
{
	return EXE_NOT_IMPLEMENTED;
}
// shop junction
PP_codeDec(10720)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20720)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20721)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20722)
{
	return EXE_NOT_IMPLEMENTED;
}
// hotel junction
PP_codeDec(10730)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20730)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20731)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(20732)
{
	return EXE_NOT_IMPLEMENTED;
}
// input character name
PP_codeDec(10740)
{
	return EXE_NOT_IMPLEMENTED;
}

// move to a place
PP_codeDec(10810)
{
	return EXE_NOT_IMPLEMENTED;
}
// memorize current point
PP_codeDec(10820)
{
	SaveData& lsd = getProject().getLSD();
	EventState& state = lsd.eventState(EV_ID_PARTY);

	lsd.setVar( inst[0], state.mapID() );
	lsd.setVar( inst[1], state.x    () );
	lsd.setVar( inst[2], state.y    () );

	return EXE_SUCCESS;
}
// move to the memorized point
PP_codeDec(10830)
{
	return EXE_NOT_IMPLEMENTED;
}
// get on to the transport
PP_codeDec(10840)
{
	return EXE_NOT_IMPLEMENTED;
}
// set transport position
PP_codeDec(10850)
{
	SaveData& lsd = getProject().getLSD();

	list< uint > point;
	switch(inst[1]) {
		case 0:
			for(uint i = 0; i < 3; i++) point.push_back(inst[i+1]);
			break;
		case 1:
			for(uint i = 0; i < 3; i++) point.push_back( lsd.getVar(inst[i+1]) );
			break;
		PP_checkInvalidEnum();
	}

	switch(inst[0]) {
		case 0: case 1: case 2: {
			EventState& state = lsd.eventState(EV_ID_BOAT + inst[0]);
			list< uint >::const_iterator it = point.begin();

			state[11] = *it;
			state[12] = *(++it);
			state[13] = *(++it);
		} break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// set event position
PP_codeDec(10860)
{
	SaveData& lsd = getProject().getLSD();
	EventState& state = lsd.eventState(inst[0]);

	uint x, y;
	switch(inst[1]) {
		case 0:
			x = inst[2];
			y = inst[3];
			break;
		case 1:
			x = lsd.getVar(inst[2]);
			y = lsd.getVar(inst[3]);
			break;
		PP_checkInvalidEnum();
	}

	state[12] = x;
	state[13] = y;

	return EXE_SUCCESS;
}
// exchange event position
PP_codeDec(10870)
{
	SaveData& lsd = getProject().getLSD();

	EventState& stateA = lsd.eventState(inst[0]);
	EventState& stateB = lsd.eventState(inst[1]);

	uint xBuff = stateA.x(), yBuff = stateA.y();

	stateA[12] = stateB.x();
	stateA[13] = stateB.y();

	stateB[12] = xBuff;
	stateB[13] = yBuff;

	return EXE_SUCCESS;
}

// get terrain id
PP_codeDec(10910)
{
	Project& proj = getProject();
	MapUnit & lmu = proj.getLMU();
	SaveData& lsd = proj.getLSD();

	uint x, y;
	switch(inst[0]) {
		case 0:
			x = inst[1];
			y = inst[2];
			break;
		case 1:
			x = lsd.getVar(inst[2]);
			y = lsd.getVar(inst[3]);
			break;
		PP_checkInvalidEnum();
	}

	lsd.setVar(
		inst[3],
		proj.getTerrainID( lmu.chipIDLw(x, y) )
	);

	return EXE_SUCCESS;
}
// get event id
PP_codeDec(10920)
{
	SaveData& lsd = getProject().getLSD();
	Array2D& states = lsd.eventState();

	uint x, y;
	switch(inst[0]) {
		case 0:
			x = inst[1];
			y = inst[2];
			break;
		case 1:
			x = lsd.getVar(inst[1]);
			y = lsd.getVar(inst[2]);
			break;
		PP_checkInvalidEnum();
	}

	uint result = 0;
	for(Array2D::Iterator it = states.begin(); it != states.end(); ++it) {
		if( ( it.second()[12] == x ) && ( it.second()[13] == y ) ) {
			result = it.first();
		}
	}

	lsd.setVar(inst[3], result);

	return EXE_SUCCESS;
}

// erase screen
PP_codeDec(11010)
{
	return EXE_NOT_IMPLEMENTED;
}
// show screen
PP_codeDec(11020)
{
	return EXE_NOT_IMPLEMENTED;
}
// set screen color tone
PP_codeDec(11030)
{
	return EXE_NOT_IMPLEMENTED;
}
// flash screen
PP_codeDec(11040)
{
	return EXE_NOT_IMPLEMENTED;
}
// shake screen
PP_codeDec(11050)
{
	return EXE_NOT_IMPLEMENTED;
}
// scroll screen
PP_codeDec(11060)
{
	return EXE_NOT_IMPLEMENTED;
}
// set weather effect
PP_codeDec(11070)
{
	return EXE_NOT_IMPLEMENTED;
}


// show picture
PP_codeDec(11110)
{
	return EXE_NOT_IMPLEMENTED;
}
// move picture
PP_codeDec(11120)
{
	return EXE_NOT_IMPLEMENTED;
}
// erase picture
PP_codeDec(11130)
{
	return EXE_NOT_IMPLEMENTED;
}

// battle anime at field
PP_codeDec(11210)
{
	return EXE_NOT_IMPLEMENTED;
}

// transparent hero
PP_codeDec(11310)
{
	return EXE_NOT_IMPLEMENTED;
}
// flash event
PP_codeDec(11320)
{
	return EXE_NOT_IMPLEMENTED;
}
// set move
PP_codeDec(11330)
{
	return EXE_NOT_IMPLEMENTED;
}
// process all moves
PP_codeDec(11340)
{
	return EXE_NOT_IMPLEMENTED;
}
// dispose all moves
PP_codeDec(11350)
{
	return EXE_NOT_IMPLEMENTED;
}

// wait
PP_codeDec(11410)
{
	return EXE_NOT_IMPLEMENTED;
}

// play music
PP_codeDec(11510)
{
	return EXE_NOT_IMPLEMENTED;
}
// fade out music
PP_codeDec(11520)
{
	return EXE_NOT_IMPLEMENTED;
}
// memorize music
PP_codeDec(11530)
{
	return EXE_NOT_IMPLEMENTED;
}
// play memorized music
PP_codeDec(11540)
{
	return EXE_NOT_IMPLEMENTED;
}
// play sound
PP_codeDec(11550)
{
	return EXE_NOT_IMPLEMENTED;
}
// play movie
PP_codeDec(11560)
{
	return EXE_NOT_IMPLEMENTED;
}

// get key input
PP_codeDec(11610)
{
	return EXE_NOT_IMPLEMENTED;
}

// change chip set
PP_codeDec(11710)
{
	if(getProject().getLMU()[1] != inst[0]) {
		static_cast< Array1D& >( getProject().getLSD()[111] )[5] = inst[0];
	}

	return EXE_SUCCESS;
}
// change panorama
PP_codeDec(11720)
{
/*
	Array1D& state = getProject().getLSD()[111];
	MapUnit& lmu = getProject().getLMU();

	[32]: string panorama;
	[33]: bool horizontalScroll;
	[34]: bool verticalScroll;
	[35]: bool horizontalScrollAuto;
	[36]: int horizontalScrollSpeed;
	[37]: bool verticalScrollAuto;
	[38]: int verticalScrollSpeed;

	return EXE_SUCCESS;
 */
	return EXE_NOT_IMPLEMENTED;
}
// change encount step
PP_codeDec(11740)
{
	if(getProject().getLMT()[44] != inst[0]) {
		static_cast< Array1D& >( getProject().getLSD()[111] )[3] = inst[0];
	}

	return EXE_SUCCESS;
}
// replace chip
PP_codeDec(11750)
{
	getProject().getLSD().replace( (ChipSetType)inst[0], inst[1], inst[2] );

	return EXE_SUCCESS;
}

// add teleport link
PP_codeDec(11810)
{
	Array2D& points = getProject().getLSD()[110];

	switch(inst[0]) {
	// add teleport link
		case 0: {
			Array1D& point =  points[ inst[1] ];
			point[1] = inst[1];
			point[2] = inst[2];
			point[3] = inst[3];
			switch(inst[4]) {
				case 0:
					point[4] = false;
					break;
				case 1:
					point[4] = true;
					point[5] = inst[5];
					break;
				PP_checkInvalidEnum();
			}
		} break;
	// remove teleport link
		case 1: points.remove(inst[1]); break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// disable or enable teleport
PP_codeDec(11820)
{
	static_cast< Array1D& >( getProject().getLSD()[101] )[121] = (bool)inst[0];

	return EXE_SUCCESS;
}
// set escape link
PP_codeDec(11830)
{
	Array1D& point =
		static_cast< Array2D& >( getProject().getLSD()[110] )[0];

	point[1] = inst[0];
	point[2] = inst[1];
	point[3] = inst[2];
	switch(inst[3]) {
		case 0:
			point[4] = false;
			break;
		case 1:
			point[4] = true;
			point[5] = inst[4];
			break;
		PP_checkInvalidEnum();
	}

	return EXE_SUCCESS;
}
// disable or enable escape
PP_codeDec(11840)
{
	static_cast< Array1D& >( getProject().getLSD()[101] )[122] = (bool)inst[0];

	return EXE_SUCCESS;
}
// call save screen
PP_codeDec(11910)
{
	// sgetOwner().getOwner().setGameMode(MODE_Save);

	return EXE_NEXT_FRAME;
}
// disable or enable saving
PP_codeDec(11930)
{
	static_cast< Array1D& >( getProject().getLSD()[101] )[123] = (bool)inst[0];

	return EXE_SUCCESS;
}
// call menu
PP_codeDec(11950)
{
	// getOwner().getOwner().setGameMode(MODE_Menu);

	return EXE_NEXT_FRAME;
}
// disable or enable menu
PP_codeDec(11960)
{
	static_cast< Array1D& >( getProject().getLSD()[101] )[124] = (bool)inst[0];

	return EXE_SUCCESS;
}

// junction
PP_codeDec(12010)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(22010)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(22011)
{
	return EXE_NOT_IMPLEMENTED;
}

// set label
PP_codeDec(12110)
{
	return EXE_NOT_IMPLEMENTED;
}
// goto label
PP_codeDec(12120)
{
	return EXE_NOT_IMPLEMENTED;
}

// loop
PP_codeDec(12210)
{
	return EXE_NOT_IMPLEMENTED;
}
PP_codeDec(22210)
{
	return EXE_NOT_IMPLEMENTED;
}
// interrupt loop
PP_codeDec(12220)
{
	return EXE_NOT_IMPLEMENTED;
}

// interrupt event
PP_codeDec(12310)
{
	return EXE_NOT_IMPLEMENTED;
}
// erase event
PP_codeDec(12320)
{
	return EXE_NOT_IMPLEMENTED;
}
// call event
PP_codeDec(12330)
{
	return EXE_NOT_IMPLEMENTED;
}

// comment
PP_codeDec(12410)
{
	clog << Encode::getInstance().toSystem(inst) << endl;
	return EXE_SUCCESS;
}
PP_codeDec(22410)
{
	clog << Encode::getInstance().toSystem(inst) << endl;
	return EXE_SUCCESS;
}
// gameover
PP_codeDec(12420)
{
	// getOwner().getOwner().setGameMode(MODE_GameOver);
	return EXE_NEXT_FRAME;
}

// title
PP_codeDec(12510)
{
	// getOwner().getOwner().gotoTitle();

	return EXE_NEXT_FRAME;
}

// battle events

// change enemy HP
PP_codeDec(13110)
{
	return EXE_NOT_IMPLEMENTED;
}
// change enemy MP
PP_codeDec(13120)
{
	return EXE_NOT_IMPLEMENTED;
}
// change enemy condition
PP_codeDec(13130)
{
	return EXE_NOT_IMPLEMENTED;
}
// appear enemy
PP_codeDec(13150)
{
	return EXE_NOT_IMPLEMENTED;
}

// change backdrop
PP_codeDec(13210)
{
	return EXE_NOT_IMPLEMENTED;
}
// show battle animation
PP_codeDec(13260)
{
	return EXE_NOT_IMPLEMENTED;
}

// junction true  way
PP_codeDec(13310)
{
	return EXE_NOT_IMPLEMENTED;
}
// junction false way
PP_codeDec(23310)
{
	return EXE_NOT_IMPLEMENTED;
}
// end of junction
PP_codeDec(23311)
{
	return EXE_NOT_IMPLEMENTED;
}

// interrupt battle
PP_codeDec(13410)
{
	return EXE_NOT_IMPLEMENTED;
}

#undef PP_codeDec
#undef PP_checkInvalidEnum
