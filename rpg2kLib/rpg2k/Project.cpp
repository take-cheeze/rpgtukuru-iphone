#include <algorithm>

#include "Debug.hpp"
#include "Project.hpp"
#include "Stream.hpp"


namespace
{
	/*
	 * EXP caclulation routine from http://twitter.com/easyrpg
	 * (many rewrite with take-cheeze)
	 * The original code(writen with Pascal) is at
	 * http://code.google.com/p/turbu/source/browse/trunk/turbu/hero_data.pas
	 */
	int calcExp(int const level, int const basic, int const increase, int const correction)
	{
		int result = 0;

		#if (RPG2K_IS_PSP || RPG2K_IS_IPHONE)
			float standard = basic, additional = 1.5f + (increase * 0.01f);
			for(int i = 0; i < (level - 1); i++) {
				result += int(standard);
				standard *= additional;
				additional = (level * 0.002f + 0.8f) * (additional - 1.f) + 1.f;
			}
		#else
			double standard = basic, additional = 1.5 + (increase * 0.01);
			for(int i = 0; i < (level - 1); i++) {
				result += int(standard);
				standard *= additional;
				additional = (level * 0.002 + 0.8) * (additional - 1.0) + 1.0;
			}
		#endif
		result += correction * (level - 1);

		return result;
		// TODO:
		// if (result < 1000000) return result;
		// else return 1000000;
	}

	using rpg2k::structure::Array1D;
	using rpg2k::structure::Array2D;
	using rpg2k::structure::EventState;
} // namespace

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
			lastSaveDataStamp_ = 0.0;
			lastSaveDataID_ = ID_MIN;

			lsd_.push_back( std::auto_ptr<SaveData>( new SaveData() ) ); // set LcfSaveData buffer
			for(unsigned i = ID_MIN; i <= SAVE_DATA_MAX; i++) {
				lsd_.push_back( std::auto_ptr<SaveData>( new SaveData(baseDir_, i) ) );

				if( lsd_.back().exists() ) {
					// TODO: caclating current time
					// Time Stamp Format: see http://support.microsoft.com/kb/210276
					double const cur = lsd_.back()[100].toArray1D()[1].to<double>();
					if(cur > lastSaveDataStamp_) {
						lastSaveDataID_ = i;
						lastSaveDataStamp_ = cur;
					}
				}
			}

			newGame();
		}

		SaveData& Project::getLSD(unsigned const id)
		{
			rpg2k_assert( rpg2k::within<unsigned>(ID_MIN, id, SAVE_DATA_MAX+1) );
			return lsd_[id];
		}

		unsigned Project::currentMapID()
		{
			return getLSD().eventState(ID_PARTY).mapID();
		}

		MapUnit& Project::getLMU(unsigned const id)
		{
			MapUnitTable::iterator const it = lmu_.find(id);
			if( it == lmu_.end() ) {
				return *lmu_.insert( id, std::auto_ptr<MapUnit>(
					new MapUnit( gameDir(), id ) ) ).first->second;
			} else return *it->second;
		}

		bool Project::canTeleport()
		{
			return getLSD()[101].toArray1D().exists(121)
				? getLSD()[101].toArray1D()[121]
				: getLMT().canTeleport( currentMapID() );
		}
		bool Project::canEscape()
		{
			return getLSD()[101].toArray1D().exists(122)
				? getLSD()[101].toArray1D()[122]
				: getLMT().canEscape( currentMapID() );
		}
		bool Project::canSave()
		{
			return getLSD()[101].toArray1D().exists(123)
				? getLSD()[101].toArray1D()[123]
				: getLMT().canSave( currentMapID() );
		}
		bool Project::canOpenMenu() const
		{
			return getLSD()[101].toArray1D().exists(124)
				? getLSD()[101].toArray1D()[124]
				: true;
		}

		void Project::loadLSD(unsigned const id)
		{
			rpg2k_assert( rpg2k::within<unsigned>(ID_MIN, id, SAVE_DATA_MAX+1) );
			getLSD() = lsd_[id];

			Array2D const& charsLDB = ldb_.character();
			Array2D& charsLSD = getLSD().character();
			charTable_.clear();
			for(Array2D::ConstIterator it = charsLDB.begin(); it != charsLDB.end(); ++it) {
				if( !it->second->exists() ) continue;

				charTable_.insert( it->first, std::auto_ptr<Character>(
					new Character( it->first, *it->second, charsLSD[it->first] ) ) );
			}
		}

		void Project::saveLSD(unsigned const id)
		{
			rpg2k_assert( rpg2k::within<unsigned>(ID_MIN, id, SAVE_DATA_MAX+1) );

			SaveData& lsd = getLSD();
		// set preview
			Array1D& prev = lsd[100];
			prev.clear();
			// reset the last time stamp and ID
			lastSaveDataStamp_ += 1.0; // TODO
			lastSaveDataID_ = id;
			prev[1] = lastSaveDataStamp_;
			{
				std::vector<uint16_t> const& mem = lsd.member();
				// set front character status
				if( !mem.empty() ) {
					Character const& c = character( mem.front() );
					prev[11] = c.name();
					prev[12] = c.level();
					prev[13] = c.hp();
				}
				// set faceSet
				for(unsigned i = 0; i < mem.size(); i++) {
					Character const& c = character( mem[i] );
					prev[21 + 2*i] = c.faceSet();
					prev[22 + 2*i] = c.faceSetPos();
				}
			}

			for(CharacterTable::iterator i = charTable_.begin(); i != charTable_.end(); ++i) {
				i->second->sync();
			}

			lsd_[id] = lsd;
			lsd_[id].save();
		}

		namespace
		{
			int chipID2chipIndex(SaveData& lsd, int const chipID) // only for lower chip
			{
				int index;

				if( rpg2k::within(chipID, 3000) ) index = 0 + chipID/1000;
				else if(chipID == 3028) index = 3 + 0;
				else if(chipID == 3078) index = 3 + 1;
				else if(chipID == 3128) index = 3 + 2;
				else if( rpg2k::within(4000, chipID, 5000) ) index =  6 + (chipID-4000)/50;
				else if( rpg2k::within(5000, chipID, 5144) ) index = 18 + lsd.replace(ChipSet::LOWER, chipID-5000);
				else rpg2k_assert(false);

				return index;
			}
			bool isUpperChip(int const chipID)
			{
				return rpg2k::within(10000, chipID, 10144);
			}
			int toUpperChipIndex(SaveData& lsd, int const chipID)
			{
				return lsd.replace(ChipSet::UPPER, chipID-10000);
			}
		} // namespace
		bool Project::isBelow(int const chipID)
		{
			return !isAbove(chipID);
		}
		bool Project::isAbove(int const chipID)
		{
			int flag;
			if( isUpperChip(chipID) ) {
				flag = getLDB().upperChipFlag( chipSetID() )[ toUpperChipIndex(getLSD(), chipID) ];
			} else {
				unsigned const index = chipID2chipIndex(getLSD(), chipID);
				if(
					rpg2k::within(4000, chipID, 5000) &&
					( (ldb_.lowerChipFlag(chipSetID())[index] & 0x30) == 0x30 )
				) switch( (chipID-4000) % 50 ) {
					case 0x14: case 0x15: case 0x16: case 0x17:
					case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
					case 0x2a: case 0x2b:
					case 0x2d: case 0x2e:
						return true;
					default:
						return false;
				}
				flag = ldb_.lowerChipFlag(chipSetID())[index];
			}
			return (flag & 0x30) == 0x10;
		}
		bool Project::isCounter(int chipID)
		{
			rpg2k_assert( isUpperChip(chipID) );
			return (ldb_.upperChipFlag( chipSetID() )[chipID-10000] & 0x40) != 0x00;
		}
		uint8_t Project::pass(int chipID)
		{
			if( isUpperChip(chipID) ) {
				return ldb_.upperChipFlag(chipSetID())[ toUpperChipIndex(getLSD(), chipID) ]; // & 0x0f;
			} else return ldb_.lowerChipFlag(chipSetID())[ chipID2chipIndex(getLSD(), chipID) ]; // & 0x0f;
		}
		int Project::terrainID(int chipID)
		{
			return ldb_.terrain( chipSetID() )[ chipID2chipIndex(getLSD(), chipID) ];
		}

		int Project::currentPageID(unsigned eventID)
		{
			Array1D const* res = currentPage( getLMU()[81].toArray2D()[eventID][5] );
			return (res == NULL) ? INVALID_PAGE_ID : res->index();
		}
		Array1D const* Project::currentPage(Array2D const& pages) const
		{
			for(Array2D::ConstRIterator it = pages.rbegin(); it != pages.rend(); ++it) {
				if(
					it->second->exists() &&
					validPageMap( (*it->second)[2] )
				) return it->second;
			}

			return NULL;
		}

		bool Project::equip(unsigned const charID, unsigned const itemID)
		{
			DataBase const& ldb = getLDB();
			SaveData& lsd = getLSD();
			int itemNum = lsd.itemNum(itemID);

			if(itemNum == 0) return false;

			unsigned type = ldb.item()[itemID][3];
			rpg2k_assert( rpg2k::within( unsigned(Item::WEAPON), type, unsigned(Item::ACCESSORY+1) ) );
			type -= Item::WEAPON;

			Character& c = this->character(charID);
			c.equip()[type] = itemID;

			lsd.setItemNum(itemID, --itemNum); // decrement item stock

			return true;
		}
		void Project::unequip(unsigned const charID, Equip::Type type)
		{
			SaveData& lsd = getLSD();

			std::vector<uint16_t> vec = lsd.character()[charID][61].toBinary();

			lsd.setItemNum( vec[type], lsd.itemNum(vec[type]+1) );
			vec[type] = 0;

			lsd.character()[charID][61].toBinary() = vec;
		}

		RPG2kString const& Project::systemGraphic() const
		{
			Array1D const& sys = getLSD()[101];

			if( sys.exists(21) ) return sys[21];
			else return getLDB()[22].toArray1D()[19];
		}
		Wallpaper::Type Project::wallpaperType() const
		{
			Array1D const& sys = getLSD()[101];

			if( sys.exists(22) ) return Wallpaper::Type( sys[22].to<int>() );
			else return Wallpaper::Type( getLDB()[22].toArray1D()[71].to<int>() );
		}
		Face::Type Project::fontType() const
		{
			Array1D const& sys = getLSD()[101];

			if( sys.exists(23) ) return Face::Type( sys[23].to<int>() );
			else return Face::Type( getLDB()[22].toArray1D()[72].to<int>() );
		}

		void Project::newGame()
		{
			SaveData& lsd = getLSD();
			DataBase& ldb = getLDB();
		// clear the save data buffer
			lsd.reset();
		// set party, boat, ship and airShip start point
			Array1D const& startLMT = getLMT().startPoint();
			Array1D const& sysLDB = getLDB()[22];
			for(unsigned i = 1; i <= (ID_AIRSHIP - ID_PARTY); i++) {
				EventState& dst = lsd.eventState(i + ID_PARTY);

				dst[11] = startLMT[10*i + 1].to<int>();
				dst[12] = startLMT[10*i + 2].to<int>();
				dst[13] = startLMT[10*i + 3].to<int>();
				dst[21] = int(CharSet::Dir::DOWN);
				dst[22] = int(CharSet::Dir::DOWN);
				dst[73] = sysLDB[10+i].to_string();
				if( sysLDB[10+i + 3].exists() ) dst[74] = sysLDB[10+i + 3].to<int>();
			}
		// move to start point
			move(startLMT[1], startLMT[2], startLMT[3]);
		// set character paramaeter
			Array2D const& charsLDB = ldb.character();
			Array2D& charsLSD = lsd.character();
			unsigned const conditioNum = ldb.condition().rbegin()->first + 1;
			for(Array2D::ConstIterator it = charsLDB.begin(); it != charsLDB.end(); ++it) {
				if( !it->second->exists() ) continue;

				Array1D const& charLDB = *it->second;
				Array1D& charLSD = charsLSD[ it->first ];

				int level = charLDB[7].to<int>();
				charLSD[31] = level; // level
				charLSD[33] = 0; // HP fix
				charLSD[34] = 0; // MP fix

				charLSD[61] = charLDB[51].toBinary(); // equip

				charTable_.insert( it->first, std::auto_ptr<Character>(
					new Character( it->first, charLDB, charLSD ) ) );
				Character& c = this->character(it->first);

				charLSD[32] = c.exp(level); // experience

				charLSD[71] = c.basicParam(level, Param::HP); // current HP
				charLSD[72] = c.basicParam(level, Param::MP); // current MP

				charLSD[81] = conditioNum;
				charLSD[82] = Binary( std::vector<uint16_t>(conditioNum) );
			}
		// set start member
			lsd.member() = sysLDB[22].toBinary();
		// set party's char graphic
			if( !lsd.member().empty() ) {
				Character const& frontChar = this->character( lsd.member().front() );
				EventState& party = lsd.eventState(ID_PARTY);
				party[21] = int(CharSet::Dir::DOWN);
				party[22] = int(CharSet::Dir::DOWN);
				party[73] = frontChar.charSet();
				party[74] = frontChar.charSetPos();
			}
		}

		void Project::move(unsigned const mapID, int const x, int const y)
		{
			rpg2k_assert(mapID != 0);

			SaveData& lsd = getLSD();
		// set party position
			EventState& party = lsd.eventState(ID_PARTY);
			party[11] = mapID; party[12] = x; party[13] = y;

			/*
			 * the new party position must be set before this
			 * because getLMU() returns the current party mapID's LMU
			 */
			MapUnit& lmu = getLMU();
		// clear changed values
			lsd[111].toArray1D().clear();
		// reset chip replace
			lsd.resetReplace();
		// set map event position
			Array2D const& mapEvent = lmu[81];
			Array2D& states = lsd.eventState();
			for(Array2D::ConstIterator it = mapEvent.begin(); it != mapEvent.end(); ++it) {
				if( !it->second->exists() ) continue;

				Array1D const* page = currentPage( (*it->second)[5] );
				if(page == NULL) continue;

				Array1D const& src = *page;
				Array1D& dst = states[it->first];

				dst[11] = 0; // mapID
				dst[12] = (*it->second)[2].to<int>(); // x
				dst[13] = (*it->second)[3].to<int>(); // y

				dst[21] = src[23].to<int>(); // direction
				dst[22] = src[23].to<int>();

				dst[73] = src[21].to_string(); // charSet
				dst[74] = src[22].to<int>(); // charSetPos
				dst[75] = src[24].to<int>(); // charSetPat
			}
		}

		RPG2kString Project::panorama()
		{
			if( getLSD()[111].toArray1D().exists(32) ) return getLSD()[111].toArray1D()[32];
			else if( getLMU()[31].to<bool>() ) return getLMU()[32];
			else return RPG2kString();
		}

		int Project::chipSetID()
		{
			return getLSD()[111].toArray1D().exists(5)
				? getLSD()[111].toArray1D()[5]
				: getLMU()[1];
		}

		Project::Character::Character(unsigned const charID
		, structure::Array1D const& ldb, structure::Array1D& lsd)
		: charID_(charID), ldb_(ldb), lsd_(lsd)
		, basicParam_(ldb_[31].toBinary().convert<uint16_t>())
		, skill_( lsd_[52].toBinary() )
		, condition_( lsd_[84].toBinary().convert<uint8_t>() )
		, conditionStep_( lsd_[82].toBinary().convert<uint16_t>() )
		, equip_( lsd_[61].toBinary() )
		{
		}
		void Project::Character::sync()
		{
			lsd_[51] = skill_.size();
			lsd_[52] = Binary(skill_);

			lsd_[61] = Binary(equip_);

			lsd_[81] = conditionStep_.size();
			lsd_[82] = Binary(conditionStep_);
			std::vector<uint16_t> const conditionClean( condition_.begin()
			, ++std::find( condition_.rbegin(), condition_.rend(), true ).base() );
			lsd_[83] = conditionClean.size();
			lsd_[84] = Binary(conditionClean);
		}

		bool Project::hasItem(unsigned const id) const
		{
			if( getLSD().item().find(id) != getLSD().item().end() ) return true;
			else {
				std::vector<uint16_t> const& mem = getLSD().member();
				for(unsigned i = 0; i < mem.size(); i++) {
					Character::Equip const& equip = this->character( mem[i] ).equip();

					for(int j = Equip::BEGIN; j < Equip::END; j++) if(equip[i] == id) return true;
				}
			}

			return false;
		}
		unsigned Project::equipNum(unsigned const itemID) const
		{
			unsigned ret = 0;
			std::vector<uint16_t> const& mem = getLSD().member();
			for(std::size_t i = 0; i < mem.size(); i++) {
				Character::Equip const& equip = this->character( mem[i] ).equip();
				ret += std::count( equip.begin(), equip.end(), itemID );
			}
			return ret;
		}

		bool Project::validPageMap(structure::Array1D const& term) const
		{
			int flags = term[1];

			std::vector<uint16_t> const& mem = getLSD().member();
			return !(
				( ( flags & (0x01 << 0) ) && !getLSD().flag(term[2]) ) ||
				( ( flags & (0x01 << 1) ) && !getLSD().flag(term[3]) ) ||
				( ( flags & (0x01 << 2) ) &&
					(getLSD().var(term[4].to<int>()) < term[5].to<int>() ) ) ||
				( ( flags & (0x01 << 3) ) && !hasItem(term[6]) ) ||
				( ( flags & (0x01 << 4) ) &&
					( std::find( mem.begin(), mem.end(), term[7].to<unsigned>() ) == mem.end() ) ) ||
				( ( flags & (0x01 << 5) ) && (getLSD().timerLeft() > term[8].to<unsigned>()) ) );
		}
		bool Project::validPageBattle(structure::Array1D const& term) const
		{
			int flags = term[1];

			return (
				( ( flags & (0x01 << 0) ) && !getLSD().flag(term[2].to<unsigned>()) ) ||
				( ( flags & (0x01 << 1) ) && !getLSD().flag(term[3].to<unsigned>()) ) ||
				( ( flags & (0x01 << 2) ) &&
					(getLSD().var (term[4].to<unsigned>()) < (int)term[5].to<unsigned>())
				) /* ||
			// turns
				( ( flags & (0x01 << 3) ) && ) ||
			// consume
				( ( flags & (0x01 << 4) ) && ) ||
			// enemy
				( ( flags & (0x01 << 5) ) && ) ||
			// party
				( ( flags & (0x01 << 6) ) && ) ||
				*/
			) ? false : true;
		}

		int Project::Character::basicParam(int const level, Param::Type const t) const
		{
			rpg2k_assert( rpg2k::within( basicParam_.size() / Param::END * t + level - 1, basicParam_.size() ) );
			return basicParam_[basicParam_.size() / Param::END * t + level - 1];
		}
		int Project::Character::param(Param::Type const t) const
		{
			switch(t) {
				case Param::HP: case Param::MP:
					return basicParam(level(), t) + lsd_[33 + t].to<int>();
				case Param::ATTACK: case Param::GAURD: case Param::MIND: case Param::SPEED:
					return basicParam(level(), t) + lsd_[41 + t - Param::ATTACK].to<int>();
				default: return 0;
			}
		}
		int Project::Character::exp(unsigned const level) const
		{
			return calcExp(level,
				ldb_[41].exists()? lsd_[41].to<int>() : EXP_DEF_VAL,
				ldb_[42].exists()? lsd_[42].to<int>() : EXP_DEF_VAL,
				ldb_[43] );
		}
		void Project::Character::setLevel(unsigned const nextLv)
		{
			if(nextLv > rpg2k::LV_MAX) { setLevel(rpg2k::LV_MAX); return; }

			unsigned const prevLv = level();
			lsd_[31] = nextLv;
			Array2D const& skillList = ldb_[63];
			unsigned currentLv = 1;
			if(prevLv > nextLv) {
				for(Array2D::ConstIterator it = skillList.begin(); it != skillList.end(); ++it) {
					if( (*it->second)[1].exists() ) { currentLv = (*it->second)[1]; }

					if(currentLv < nextLv) { continue; }
					else if(prevLv < currentLv) { break; }
					else {
						std::set<uint16_t>::iterator er = skill_.find( (*it->second)[2].to<int>() );
						if(er != skill_.end() ) { skill_.erase(er); }
					}
				}
			} else if(nextLv > prevLv) {
				for(Array2D::ConstIterator it = skillList.begin(); it != skillList.end(); ++it) {
					if( (*it->second)[1].exists() ) { currentLv = (*it->second)[1]; }

					if(currentLv < prevLv) { continue; }
					else if(nextLv < currentLv) { break; }
					else { skill_.insert( (*it->second)[2].to<int>() ); }
				}
			}
		}
		bool Project::Character::setExp(unsigned const val)
		{
			if(val > rpg2k::EXP_MAX) { return setExp(rpg2k::EXP_MAX); }

			lsd_[32] = val;
			if( this->canLevelUp() ) {
				this->addLevel();
				return true;
			} else {
				while( val < this->levelExp() ) { this->addLevel(-1); }
				return false;
			}
		}
		void Project::Character::cure()
		{
			setHP( param(Param::HP) );
			setMP( param(Param::MP) );

			condition_.clear();
			std::fill( conditionStep_.begin(), conditionStep_.end(), 0 );
		}

		Project::Character const& Project::character(unsigned const id) const
		{
			CharacterTable::const_iterator it = charTable_.find(id);
			rpg2k_assert( it != charTable_.end() );
			return *it->second;
		}
		Project::Character& Project::character(unsigned const id)
		{
			CharacterTable::iterator it = charTable_.find(id);
			rpg2k_assert( it != charTable_.end() );
			return *it->second;
		}

		int Project::paramWithEquip(unsigned charID, Param::Type t) const
		{
			Character const& c = this->character(charID);
			int ret = c.param(t);
			for(size_t i = 0; i < c.equip().size(); i++) {
			  ret += ldb_.item()[ c.equip()[i] ][11 + t - 2].to<int>();
			}
			return ret;
		}
		bool Project::processAction(unsigned const eventID, Action::Type const act, structure::StreamReader& r)
		{
			EventState& ev = getLSD().eventState(eventID);

			switch(act) {
				case Action::Move::UP   : ev[13] = ev[13].to<int>() - 1; break;
				case Action::Move::RIGHT: ev[12] = ev[12].to<int>() + 1; break;
				case Action::Move::DOWN : ev[13] = ev[13].to<int>() + 1; break;
				case Action::Move::LEFT : ev[12] = ev[12].to<int>() - 1; break;
				case Action::Move::RIGHT_UP:
					ev[12] = ev[12].to<int>() + 1;
					ev[13] = ev[13].to<int>() - 1;
					break;
				case Action::Move::RIGHT_DOWN:
					ev[12] = ev[12].to<int>() - 1;
					ev[13] = ev[13].to<int>() + 1;
					break;
				case Action::Move::LEFT_DOWN:
					ev[12] = ev[12].to<int>() + 1;
					ev[13] = ev[13].to<int>() + 1;
					break;
				case Action::Move::LEFT_UP:
					ev[12] = ev[12].to<int>() + 1;
					ev[13] = ev[13].to<int>() - 1;
					break;
				case Action::Move::RANDOM:
					return processAction( eventID, random( Action::Move::UP, Action::Move::LEFT + 1 ), r );
				case Action::Move::TO_PARTY:
					break;
				case Action::Move::FROM_PARTY:
					break;
				case Action::Move::A_STEP:
					return processAction( eventID, Action::Move::UP + ev.eventDir(), r );
				case Action::Face::UP   :
				case Action::Face::RIGHT:
				case Action::Face::DOWN :
				case Action::Face::LEFT :
					ev[21] = int(act - Action::Face::UP);
					ev[22] = int(act - Action::Face::UP);
					break;
				case Action::Turn::RIGHT_90:
					ev[21] = int( CharSet::Dir::Type( ev.eventDir() / CharSet::Dir::END ) );
					break;
				case Action::Turn::LEFT_90:
					ev[21] = int( CharSet::Dir::Type(
						( ev.eventDir() + CharSet::Dir::END - 1 ) / CharSet::Dir::END ) );
					break;
				case Action::Turn::OPPOSITE:
					ev[21] = int( CharSet::Dir::Type( ev.eventDir() + 1 / CharSet::Dir::END ) );
					break;
				case Action::Turn::RIGHT_OR_LEFT_90:
					return processAction( eventID,
						random(2)? Action::Turn::LEFT_90 : Action::Turn::RIGHT_90, r );
				case Action::Turn::RANDOM:
					ev[21] = ( random(4) + 1 ) * 2;
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
					getLSD().setFlag( r.ber(), true );
					break;
				case Action::SWITCH_OFF:
					getLSD().setFlag( r.ber(), false );
					break;
				case Action::CHANGE_CHAR_SET: {
					RPG2kString charSet( r.ber(), '\0' );
					for(RPG2kString::iterator it = charSet.begin(); it < charSet.end(); ++it) {
						*it = r.ber();
					}
					ev[73] = charSet;
					ev[74] = r.ber();
				} break;
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

			return true;
		}

		std::vector<unsigned> Project::sortLSD() const
		{
			std::map<double, unsigned> tmp;
			for(unsigned i = 0; i < SAVE_DATA_MAX; i++) {
				tmp.insert( std::make_pair( i + 1, lsd_[i + 1][100].toArray1D()[1].to<double const&>() ) );
			}
			std::vector<unsigned> ret;
			for(std::map<double, unsigned>::iterator i = tmp.begin(); i != tmp.end(); ++i) {
				ret.push_back(i->second);
			}

			return ret;
		}
	} // namespace model
} // namespace rpg2k
