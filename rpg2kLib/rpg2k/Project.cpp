#include "Project.hpp"
#include "Debug.hpp"


namespace rpg2k
{
	namespace model
	{
		Project::Project(SystemString baseDir)
		: baseDir_(baseDir)
		, ldb_(baseDir), lmt_(baseDir)
		{
			init();
		}

		void Project::init()
		{
		// LcfSaveData
			lastSaveDataStamp_ = 0;
			lastSaveDataID_ = ID_MIN;

			lsd_.resize(SAVE_DATA_MAX+1);
			for(uint i = ID_MIN; i <= SAVE_DATA_MAX; i++) {
				lsd_[i].reset( new SaveData(baseDir_, i) );

				if( lsd_[i]->exists() ) {
					uint64_t cur = static_cast< std::vector< uint64_t > >( (*lsd_[i])[100].getArray1D()[1] ).at(0);
					if(cur > lastSaveDataStamp_) {
						lastSaveDataID_ = i;
						lastSaveDataStamp_ = cur;
					}

					cout
						<< "TimeStamp id: " << std::setw(2) << i << ", "
						<< std::hex << std::setw(16) << cur << ", "
						<< std::dec << std::setw(16) << cur << ", "
						<< endl;
				}
			}
		// set LcfSaveData buffer
			lsd_.front().reset(new SaveData);

			newGame();
		}

		Project::~Project()
		{
		}

		SaveData const& Project::getLSD() const
		{
			return *(lsd_.front());
		}
		SaveData& Project::getLSD()
		{
			return *(lsd_.front());
		}
		SaveData& Project::getLSD(uint id)
		{
			rpg2k_assert( rpg2k::within(ID_MIN, id, SAVE_DATA_MAX+1) );
			return *(lsd_[id]);
		}

		uint Project::getCurrentMapID()
		{
			return getLSD().eventState(EV_ID_PARTY).mapID();
		}

		MapUnit & Project::getLMU(uint id)
		{
			if( id >= lmu_.size() ) lmu_.resize(id + 1);
			if( !lmu_[id].get() ) lmu_[id].reset( new MapUnit(baseDir_, id) );
			return *(lmu_[id]);
		}

		bool Project::canTeleport()
		{
			return getLSD()[101].getArray1D().exists(121)
				? getLSD()[101].getArray1D()[121]
				: getLMT().canTeleport( getCurrentMapID() );
		}
		bool Project::canEscape()
		{
			return getLSD()[101].getArray1D().exists(122)
				? getLSD()[101].getArray1D()[122]
				: getLMT().canEscape( getCurrentMapID() );
		}
		bool Project::canSave()
		{
			return getLSD()[101].getArray1D().exists(123)
				? getLSD()[101].getArray1D()[123]
				: getLMT().canSave( getCurrentMapID() );
		}
		bool Project::canOpenMenu() const
		{
			return getLSD()[101].getArray1D().exists(124)
				? getLSD()[101].getArray1D()[124]
				: true;
		}

		void Project::loadLSD(uint id)
		{
			rpg2k_assert( rpg2k::within(ID_MIN, id, SAVE_DATA_MAX+1) );
			getLSD() = (*lsd_[id]);
		}

		void Project::saveLSD(uint id)
		{
			rpg2k_assert( rpg2k::within(ID_MIN, id, SAVE_DATA_MAX+1) );

			SaveData& lsd = getLSD();
		// set preview
			structure::Array1D& prev = lsd[100];
			prev.clear();
			std::vector< uint16_t >& mem = lsd.member();
			// reset the last time stamp and ID
			lastSaveDataStamp_ += 1000; // adding random natural number (because bigger is older)
			lastSaveDataID_ = id;
			prev[1] = std::vector< uint64_t >(1, lastSaveDataStamp_);
			// set front character status
			if( mem.size() ) {
				int frontCharID = mem[0];
				prev[11] = name(frontCharID);
				prev[12] = level(frontCharID);
				prev[13] = hp(frontCharID);
			}
			// set faceSet
			for(uint i = 0; i < mem.size(); i++) {
				prev[21 + 2*i] = faceSet(mem[i]);
				prev[22 + 2*i] = faceSetPos(mem[i]);
			}

			(*lsd_[id]) = lsd;
			lsd_[id]->save();
		}

		bool Project::isBelow(int chipID)
		{
			return !isAbove(chipID);
/*
			std::vector< uint8_t > const& lower = ldb_.lowerChipFlag(chipSetID());
			std::vector< uint8_t > const& upper = ldb_.upperChipFlag(chipSetID());

			int index = 0;

			if( rpg2k::within(chipID, 3000) ) index =  0 + chipID/1000;
			else if(chipID == 3028) index =  3 + 0;
			else if(chipID == 3078) index =  3 + 1;
			else if(chipID == 3128) index =  3 + 2;
			else if( rpg2k::within( 4000, chipID,  5000) ) {
				index = 6 + (chipID-4000)/50;
				if( (lower[index] & 0x30) == 0x30 ) {
					switch( (chipID-4000) % 50 ) {
						case 0x14: case 0x15: case 0x16: case 0x17:
						case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
						case 0x2a: case 0x2b:
						case 0x2d: case 0x2e:
							return false;
						default:
							return true;
					}
				}
			}
			else if( rpg2k::within( 5000, chipID,  5144) ) index = 18 + chipID-5000;
			else if( rpg2k::within(10000, chipID, 10144) )
				return (upper[chipID-10000] & 0x30) == 0x00;
			else rpg2k_assert(false);

			rpg2k_assert( rpg2k::within( index, lower.size() ) );
			return (lower[index] & 0x30) == 0x00;
 */
		}
		bool Project::isAbove(int chipID)
		{
			std::vector< uint8_t > const& lower = ldb_.lowerChipFlag(chipSetID());
			std::vector< uint8_t > const& upper = ldb_.upperChipFlag(chipSetID());

			uint index = 0;

			if( rpg2k::within(chipID, 3000) ) index =  0 + chipID/1000;
			else if(chipID == 3028) index =  3 + 0;
			else if(chipID == 3078) index =  3 + 1;
			else if(chipID == 3128) index =  3 + 2;
			else if( rpg2k::within( 4000, chipID,  5000) ) {
				index = 6 + (chipID-4000)/50;
				if( (lower[index] & 0x30) == 0x30 ) {
					switch((chipID-4000)%50) {
						case 0x14: case 0x15: case 0x16: case 0x17:
						case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
						case 0x2a: case 0x2b:
						case 0x2d: case 0x2e:
							return true;
						default:
							return false;
					}
				}
			}
			else if( rpg2k::within( 5000, chipID,  5144) ) index = 18 + getLSD().getReplace(ChipSet::LOWER, chipID-5000);
			else if( rpg2k::within(10000, chipID, 10144) )
				return ( upper[getLSD().getReplace(ChipSet::UPPER, chipID-10000)] & 0x30 ) == 0x10;
			else rpg2k_assert(false);

			rpg2k_assert( rpg2k::within( index, lower.size() ) );
			return (lower[index] & 0x30) == 0x10;
		}
		bool Project::isCounter(int chipID)
		{
			std::vector< uint8_t > const& upper = ldb_.upperChipFlag( chipSetID() );

			rpg2k_assert( rpg2k::within(10000, chipID, 10144) );
			return (upper[chipID-10000] & 0x40) != 0x00;
		}
		uint8_t Project::getPass(int chipID)
		{
			std::vector< uint8_t > const& lower = ldb_.lowerChipFlag(chipSetID());
			std::vector< uint8_t > const& upper = ldb_.upperChipFlag(chipSetID());

			uint index = 0;

			if( rpg2k::within(chipID, 3000) ) index = 0 + chipID/1000;
			else if(chipID == 3028) index = 3 + 0;
			else if(chipID == 3078) index = 3 + 1;
			else if(chipID == 3128) index = 3 + 2;
			else if( rpg2k::within( 4000, chipID,  5000) ) index =  6 + (chipID-4000)/50;
			else if( rpg2k::within( 5000, chipID,  5144) ) index = 18 +  getLSD().getReplace(ChipSet::LOWER, chipID-5000);
			else if( rpg2k::within(10000, chipID, 10144) ) return upper[getLSD().getReplace(ChipSet::UPPER, chipID-10000)]; // & 0x0f;
			else rpg2k_assert(false);

			return lower[index]; // & 0x0f;
		}
		int Project::getTerrainID(int chipID)
		{
			std::vector< uint16_t > const& data = ldb_.terrain( chipSetID() );

			uint index = 0;

			if( rpg2k::within(chipID, 3000) ) index = 0 + chipID/1000;
			else if(chipID == 3028) index = 3 + 0;
			else if(chipID == 3078) index = 3 + 1;
			else if(chipID == 3128) index = 3 + 2;
			else if( rpg2k::within(4000, chipID, 5000) ) index =  6 + (chipID-4000)/50;
			else if( rpg2k::within(5000, chipID, 5144) ) index = 18 + getLSD().getReplace(ChipSet::LOWER, chipID-5000);
			else rpg2k_assert(false);

			return data[index];
		}

		int Project::currentPageID(uint eventID)
		{
			structure::Array1D* res = currentPage( getLMU()[81].getArray2D()[eventID][5] );
			return (res == NULL) ? INVALID_PAGE_ID : res->getIndex();
		}
		structure::Array1D* Project::currentPage(structure::Array2D const& pages) const
		{
			for(structure::Array2D::ReverseIterator it = pages.rbegin(); it != pages.rend(); ++it) {
				if(
					it.second().exists() &&
					getLSD().validPageMap( it.second()[2] )
				) return &it.second();
			}

			return NULL;
		}

		bool Project::equip(uint charID, uint itemID)
		{
			DataBase& ldb = getLDB();
			SaveData& lsd = getLSD();
			int itemNum = lsd.getItemNum(itemID);

			if(itemNum == 0) return false;

			uint type = ldb[13].getArray2D()[itemID][3];
			rpg2k_assert( rpg2k::within( uint(Item::WEAPON), type, uint(Item::ACCESSORY+1) ) );
			type -= Item::WEAPON;

			std::vector< uint16_t > vec = lsd.character()[charID][61].getBinary();
			vec[type] = itemID;
			lsd.character()[charID][61].getBinary() = vec;

			lsd.setItemNum(itemID, --itemNum); // decrement item stock

			return true;
		}
		void Project::unequip(uint charID, Equip::Type type)
		{
			SaveData& lsd = getLSD();

			std::vector< uint16_t > vec = lsd.character()[charID][61].getBinary();

			lsd.setItemNum( vec[type], lsd.getItemNum(vec[type]+1) );
			vec[type] = 0;

			lsd.character()[charID][61].getBinary() = vec;
		}

		RPG2kString const& Project::systemGraphic() const
		{
			structure::Array1D const& sys = getLSD()[101];

			if( sys.exists(21) ) return sys[21];
			else return getLDB()[22].getArray1D()[19];
		}
		Wallpaper::Type Project::wallpaperType() const
		{
			structure::Array1D const& sys = getLSD()[101];

			if( sys.exists(22) ) return (Wallpaper::Type)sys[22].get<int>();
			else return (Wallpaper::Type)getLDB()[22].getArray1D()[71].get<int>();
		}
		font::Face::Type Project::fontType() const
		{
			structure::Array1D const& sys = getLSD()[101];

			if( sys.exists(23) ) return (font::Face::Type)sys[23].get<int>();
			else return (font::Face::Type)getLDB()[22].getArray1D()[72].get<int>();
		}

		void Project::newGame()
		{
			SaveData& lsd = getLSD();
			DataBase& ldb = getLDB();
		// clear the save data buffer
			lsd.reset();
		// set party, boat, ship and airShip start point
			structure::Array1D const& startLMT = getLMT().getStartPoint();
			structure::Array1D const& sysLDB = getLDB()[22];
			for(uint i = 1; i <= (EV_ID_AIRSHIP-EV_ID_PARTY); i++) {
				structure::EventState& dst = lsd.eventState(i + EV_ID_PARTY);

				dst[11] = startLMT[10*i + 1].get<int>();
				dst[12] = startLMT[10*i + 2].get<int>();
				dst[13] = startLMT[10*i + 3].get<int>();
				dst[21] = (int)EventDir::DOWN;
				dst[22] = (int)EventDir::DOWN;
				dst[73] = sysLDB[10+i].get_string();
				if( sysLDB[10+i + 3].exists() ) dst[74] = sysLDB[10+i + 3].get<int>();
			}
		// set start member
			lsd.member() = sysLDB[22].getBinary();
		// set party's char graphic
			if( lsd.memberNum() ) {
				structure::Array1D const& frontChar = getLDB().character()[ lsd.member()[0] ];
				structure::EventState& party = lsd.eventState(EV_ID_PARTY);
				party[21] = int(EventDir::DOWN);
				party[22] = int(EventDir::DOWN);
				party[73] = frontChar[3].get_string();
				party[74] = frontChar[4].get<int>();
			}
		// move to start point
			move(startLMT[1].get<uint>(), startLMT[2].get<uint>(), startLMT[3].get<uint>());
		// set character paramaeter
			structure::Array2D const& charsLDB = ldb[11];
			structure::Array2D& charsLSD = lsd[108];
			for(structure::Array2D::Iterator it = charsLDB.begin(); it != charsLDB.end(); ++it) {
				if( !it.second().exists() ) continue;

				structure::Array1D const& charLDB = it.second();
				structure::Array1D& charLSD = charsLSD[ it.first() ];

				int level = charLDB[7].get<int>();
				charLSD[31] = level; // level
				charLSD[32] = 0; // charLDB[]; // experience

				charLSD[61] = charLDB[51].getBinary(); // equip

				charLSD[33] = 0;
				charLSD[34] = 0;
				charLSD[71] = ldb.getBasicStatus( it.first(), level, Param::HP ); // current HP
				charLSD[72] = ldb.getBasicStatus( it.first(), level, Param::MP ); // current MP
			}
		}

		void Project::move(uint mapID, int x, int y)
		{
			SaveData& lsd = getLSD();
		// set party position
			structure::EventState& party = lsd.eventState(EV_ID_PARTY);
			party[11] = mapID; party[12] = x; party[13] = y;

			/*
			 * the new party position must be set before this
			 * because getLMU() returns the current party mapID's LMU
			 */
			MapUnit& lmu = getLMU();
		// clear changed values
			lsd[111].getArray1D().clear();
		// reset chip replace
			lsd.resetReplace();
		// set map event position
			structure::Array2D const& mapEvent = lmu[81];
			structure::Array2D& states = lsd.eventState();
			for(structure::Array2D::Iterator it = mapEvent.begin(); it != mapEvent.end(); ++it) {
				if( !it.second().exists() ) continue;

				const structure::Array1D* page = currentPage( it.second()[5] );
				if(page == NULL) continue;

				structure::Array1D const& src = *page;
				structure::Array1D& dst = states[ it.first() ];
				dst[11] = 0; // mapID
				dst[12] = it.second()[2].get<int>(); // x
				dst[13] = it.second()[3].get<int>(); // y
				dst[73] = src[21].get_string(); // charSet
				dst[74] = src[22].get<int>(); // charSetPos
				dst[75] = src[23].get<int>(); // charSetPat
			}
		}

		RPG2kString Project::panorama()
		{
			if( getLSD()[111].getArray1D().exists(32) ) return getLSD()[111].getArray1D()[32];
			else if( getLMU()[31].get<bool>() ) return getLMU()[32];
			else return RPG2kString();
		}

		int Project::chipSetID()
		{
			return getLSD()[111].getArray1D().exists(5)
				? getLSD()[111].getArray1D()[5]
				: getLMU()[1];
		}

		RPG2kString Project::name(uint charID) const
		{
			return getLSD().character().exists(charID, 1)
				? getLSD().character()[charID][1]
				: getLDB().character()[charID][1];
		}
		RPG2kString Project::title(uint charID) const
		{
			return getLSD().character().exists(charID, 2)
				? getLSD().character()[charID][2]
				: getLDB().character()[charID][2];
		}
		RPG2kString Project::charSet(uint charID) const
		{
			return getLSD().character().exists(charID, 11)
				? getLSD().character()[charID][11]
				: getLDB().character()[charID][3];
		}
		int Project::charSetPos(uint charID) const
		{
			return getLSD().character().exists(charID, 12)
				? getLSD().character()[charID][12]
				: getLDB().character()[charID][4];
		}
		RPG2kString Project::faceSet(uint charID) const
		{
			return getLSD().character().exists(charID, 21)
				? getLSD().character()[charID][21]
				: getLDB().character()[charID][15];
		}
		int Project::faceSetPos(uint charID) const
		{
			return getLSD().character().exists(charID, 22)
				? getLSD().character()[charID][22]
				: getLDB().character()[charID][16];
		}

		int Project::level(uint charID) const
		{
			return getLSD().character().exists(charID, 31)
				? getLSD().character()[charID][31]
				: getLDB().character()[charID][7];
		}
		int Project::conditionID(uint charID) const
		{
			return CONDITION_NORMAL;
/*
			return getLSD().character().exists(charID, )
				? getLSD().character()[charID][]
				: getLDB().character()[charID][];
 */
		}
		RPG2kString Project::condition(uint charID) const
		{
			int id = conditionID(charID);

			if( id == CONDITION_NORMAL ) return getLDB()[21].getArray1D()[126];
			else return getLDB()[18].getArray2D()[id][1];
		}
		int Project::conditionColor(uint charID) const
		{
			int id = conditionID(charID);

			if( id == CONDITION_NORMAL ) return font::FNT_NORMAL;
			else return getLDB()[18].getArray2D()[id][3];
		}
		int Project::hp(uint charID) const
		{
			return getLSD().character()[charID][71];
		}
		int Project::mp(uint charID) const
		{
			return getLSD().character()[charID][72];
		}
		int Project::param(uint charID, Param::Type t) const
		{
			structure::Array1D const& curChar = getLSD().character()[charID];
			switch(t) {
				case Param::HP: case Param::MP:
					return getLDB().getBasicStatus(charID, curChar[31], t) + curChar[33 + t].get<int>();
				case Param::ATTACK: case Param::GAURD: case Param::MIND: case Param::SPEED:
					return getLDB().getBasicStatus(charID, curChar[31], t) + curChar[41 + t - Param::ATTACK].get<int>();
				default: return 0;
			}
		}
		int Project::exp(uint charID) const
		{
			return getLSD().character()[charID][32];
		}
		int Project::nextLevelExp(uint charID) const
		{
			return 0;
/*
			if( getLSD().character().exists(charID, 22) ) return getLSD().character()[charID][22];
			else return getLDB().character()[charID][16];
 */
		}
		bool Project::checkLevel(uint charID)
		{
			return false;
		}
		void Project::processAction(uint eventID, Action::Type act, std::vector< int > const& arg)
		{
			switch(act) {
				case Action::Move::UP:
					break;
				case Action::Move::RIGHT:
					break;
				case Action::Move::DOWN:
					break;
				case Action::Move::LEFT:
					break;
				case Action::Move::RIGHT_UP:
					break;
				case Action::Move::RIGHT_DOWN:
					break;
				case Action::Move::LEFT_DOWN:
					break;
				case Action::Move::LEFT_UP:
					break;
				case Action::Move::RANDOM:
					break;
				case Action::Move::TO_PARTY:
					break;
				case Action::Move::FROM_PARTY:
					break;
				case Action::Move::A_STEP:
					break;
				case Action::Face::UP:
					break;
				case Action::Face::RIGHT:
					break;
				case Action::Face::DOWN:
					break;
				case Action::Face::LEFT:
					break;
				case Action::Turn::RIGHT_90:
					break;
				case Action::Turn::LEFT_90:
					break;
				case Action::Turn::OPPOSITE:
					break;
				case Action::Turn::RIGHT_OR_LEFT_90:
					break;
				case Action::Turn::RANDOM:
					break;
				case Action::Turn::TO_PARTY:
					break;
				case Action::Turn::OPPOSITE_OF_PARTY:
					break;
				case Action::HALT:
					break;
				case Action::BEGIN_JUMP:
					break;
				case Action::END_JUMP:
					break;
				case Action::FIX_DIR:
					break;
				case Action::UNFIX_DIR:
					break;
				case Action::SPEED_UP:
					break;
				case Action::SPEED_DOWN:
					break;
				case Action::FREQ_UP:
					break;
				case Action::FREQ_DOWN:
					break;
				case Action::SWITCH_ON:
					break;
				case Action::SWITCH_OFF:
					break;
				case Action::CHANGE_CHAR_SET:
					break;
				case Action::PLAY_SOUND:
					break;
				case Action::BEGIN_SLIP:
					break;
				case Action::END_SLIP:
					break;
				case Action::BEGIN_ANIME:
					break;
				case Action::END_ANIME:
					break;
				case Action::TRANS_UP:
					break;
				case Action::TRANS_DOWN:
					break;
				default: rpg2k_assert(false);
			}
		}
	} // namespace model
} // namespace rpg2k
