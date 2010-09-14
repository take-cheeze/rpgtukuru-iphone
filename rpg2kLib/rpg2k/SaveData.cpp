#include <algorithm>

#include "Debug.hpp"
#include "SaveData.hpp"
#include "Structure.hpp"


namespace rpg2k
{
	namespace model
	{
		typedef std::map< uint16_t, SaveData::Item >::const_iterator item_it;

		SaveData::SaveData()
		: Base( SystemString(), SystemString() ), id_(-1)
		{
			Base::reset();

		// reset map chip info
			resetReplace();
		// char skill
			structure::Array2D const& chars = character();
			for(structure::Array2D::Iterator it = chars.begin(); it != chars.end(); ++it) {
				if( !it.second().exists() ) continue;

				charSkill_.insert( std::make_pair( uint(it.first()), std::vector< uint16_t >() ) );
			}
		}
		SaveData::SaveData(SystemString const& dir, SystemString const& name)
		: Base(dir, name), id_(0)
		{
			load();
		}
		SaveData::SaveData(SystemString const& dir, uint const id)
		: Base(dir, ""), id_(id)
		{
			std::ostringstream ss;
			ss << "Save" << std::setfill('0') << std::setw(2) << id << ".lsd";
			setFileName( ss.str() );

			checkExists();

			if( !exists() ) return;

			load();
		}
		SaveData::~SaveData()
		{
			debug::ANALYZE_RESULT << getHeader() << ": " << int(id_) << endl;
		}

		SaveData& SaveData::operator =(SaveData const& src)
		{
			this->getData() = src.getData();

			this->item_ = src.item_;

			this->variable_ = src.variable_;
			this->switch_   = src.switch_  ;

			this->member_ = src.member_;

			this->chipReplace_ = src.chipReplace_;

			this->charSkill_ = src.charSkill_;

			return *this;
		}

		void SaveData::loadImpl()
		{
			rpg2k_assert( rpg2k::within(ID_MIN, getID(), SAVE_DATA_MAX+1) );

			structure::Array1D& sys    = (*this)[101];
			structure::Array1D& status = (*this)[109];
			structure::Array1D& event  = (*this)[111];

		// item
			{
				int itemTypeNum = status[11];
				std::vector< uint16_t > id  = status[12].getBinary();
				std::vector< uint8_t  > num = status[13].getBinary();
				std::vector< uint8_t  > use = status[14].getBinary();

				for(int i = 0; i < itemTypeNum; i++) {
					if( !num[i] ) continue;

					Item info = { num[i], use[i] };
					item_.insert( std::make_pair(id[i], info) );
				}
			}
		// switch and variable
			switch_.resize(sys[31]);
			switch_ = sys[32].getBinary();
			variable_.resize(sys[33]);
			variable_ = sys[34].getBinary();
		// member
			member_.resize(status[1]);
			member_ = status[2].getBinary();
		// chip replace
			chipReplace_.resize(ChipSet::END);
			for(uint i = 0; i < ChipSet::END; i++) {
				chipReplace_[i] = event[21+i].getBinary();
			}
		// char skill
			structure::Array2D const& chars = character();
			for(structure::Array2D::Iterator it = chars.begin(); it != chars.end(); ++it) {
				if( !it.second().exists() ) continue;

				charSkill_.insert( std::make_pair(it.first(), it.second()[52].getBinary().convert<uint16_t>() ) );
				charEquip_.insert( std::make_pair(it.first(), it.second()[61].getBinary().convert<uint16_t>() ) );
			}
		}

		void SaveData::saveImpl()
		{
			structure::Array1D& status = (*this)[109];
			structure::Array1D& sys = (*this)[101];

		// item
			{
				int itemNum = item_.size();
				status[11] = itemNum;

				std::vector<uint16_t> id (itemNum);
				std::vector<uint8_t > num(itemNum);
				std::vector<uint8_t > use(itemNum);

				int i = 0;
				for(item_it it = item_.begin(); it != item_.end(); ++it) {
					id [i] = it->first;
					num[i] = it->second.num;
					use[i] = it->second.use;

					i++;
				}
				status[12] = id ;
				status[13] = num;
				status[14] = use;
			}
		// switch and variable
			sys[31] = switch_.size();
			sys[32] = switch_;
			sys[33] = variable_.size();
			sys[34] = variable_;
		// member
			(*this)[109].getArray1D()[1] = member_.size();
			(*this)[109].getArray1D()[2] = member_;
		// chip replace
			for(uint i = 0; i < ChipSet::END; i++) {
				rpg2k_assert( rpg2k::within( i, chipReplace_.size() ) );
				(*this)[111].getArray1D()[21+i] = chipReplace_[i];
			}
		// save char state
			structure::Array2D& chars = character();
			for(structure::Array2D::Iterator it = chars.begin(); it != chars.end(); ++it) {
				if( !it.second().exists() ) continue;

				std::vector< uint16_t > const& cur = skill( it.first() );
				it.second()[51] = cur.size();
				it.second()[52] = cur;

				it.second()[61] = charEquip_.find( it.first() )->second;
			}
		}

		bool SaveData::getFlag(uint const id) const
		{
			return ( id < switch_.size() ) ? switch_[id - ID_MIN] : SWITCH_DEF_VAL;
		}
		void SaveData::setFlag(uint id, bool data)
		{
			if( id >= switch_.size() ) switch_.resize(id, SWITCH_DEF_VAL);
			switch_[id - ID_MIN] = data;
		}

		int32_t SaveData::getVar(uint const id) const
		{
			return ( id < variable_.size() ) ? variable_[id - ID_MIN] : VAR_DEF_VAL;
		}
		void SaveData::setVar(uint const id, int32_t const data)
		{
			if( id >= variable_.size() ) variable_.resize(id, VAR_DEF_VAL);
			variable_[id - ID_MIN] = data;
		}

		int SaveData::getMoney() const
		{
			return (*this)[109].getArray1D()[21];
		}
		void SaveData::setMoney(int const data)
		{
			if(data < MONEY_MIN) (*this)[109].getArray1D()[21] = MONEY_MIN;
			else if(MONEY_MAX < data) (*this)[109].getArray1D()[21] = MONEY_MAX;
			else (*this)[109].getArray1D()[21] = data;
		}

		uint SaveData::getItemNum(uint const id) const
		{
			return ( item_.find(id) == item_.end() ) ? 0 : item_.find(id)->second.num;
		}
		void SaveData::setItemNum(uint const id, uint const val)
		{
			uint validVal = val;
			if(validVal < ITEM_MIN) validVal = MONEY_MIN;
			else if(ITEM_MAX < validVal) validVal = MONEY_MAX;

			if( item_.find(id) == item_.end() ) {
				Item const i = { validVal, 0 };
				item_.insert( std::make_pair(id, i) );
			} else item_[id].num = validVal;

			if( validVal == 0 ) item_.erase( item_.find(id) );
		}

		uint SaveData::getItemUse(uint const id) const
		{
			return ( item_.find(id) == item_.end() ) ? 0 : item_.find(id)->second.use;
		}
		void SaveData::setItemUse(uint const id, uint const val)
		{
			if( item_.find(id) != item_.end() ) item_[id].use = val;
		}

		bool SaveData::hasItem(uint const id) const
		{
			if( item_.find(id) != item_.end() ) return true;
			else {
				for(uint i = 0; i < member_.size(); i++) {
					rpg2k_assert( charEquip_.find( member_[i] ) != charEquip_.end() );
					std::vector< uint16_t > const& equip = charEquip_.find( member_[i] )->second;

					for(int j = 0; j < Equip::END; j++) if(equip[i] == id) return true;
				}
			}

			return false;
		}
		uint SaveData::getEquipNum(uint const itemID) const
		{
			uint ret = 0;
			for(std::size_t i = 0; i < member_.size(); i++) {
				std::vector< uint16_t > const& equip = charEquip_.find( member_[i] )->second;
				std::vector< uint16_t >::const_iterator it = equip.begin();

				while( ( it = std::find(it, equip.end(), itemID) ) != equip.end() ) ret++;
			}
			return ret;
		}

		bool SaveData::validPageMap(structure::Array1D const& term) const
		{
			int flags = term[1];

			return (
				( ( flags & (0x01 << 0) ) && !getFlag(term[2].get<uint>()) ) ||
				( ( flags & (0x01 << 1) ) && !getFlag(term[3].get<uint>()) ) ||
				( ( flags & (0x01 << 2) ) &&
					(this->getVar (term[4].get<uint>()) < term[5].get<int>() )
				) || (
					( flags & (0x01 << 3) ) && !hasItem(term[6].get<uint>()) ) ||
				(
					( flags & (0x01 << 4) ) &&
					( find( member_.begin(), member_.end(), uint(term[7].get<uint>()) ) == member_.end() )
				) || (
					( flags & (0x01 << 5) ) && (timerLeft() > (uint)term[8].get<uint>()) )
			) ? false : true;
		}
		bool SaveData::validPageBattle(structure::Array1D const& term) const
		{
			int flags = term[1];

			return (
				( ( flags & (0x01 << 0) ) && !this->getFlag(term[2].get<uint>()) ) ||
				( ( flags & (0x01 << 1) ) && !this->getFlag(term[3].get<uint>()) ) ||
				( ( flags & (0x01 << 2) ) && (this->getVar(term[4].get<uint>()) < (int)term[5].get<uint>()) ) /* ||
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

		uint SaveData::member(uint const index) const
		{
			rpg2k_assert( rpg2k::within( index, member_.size() ) );
			return member_[index];
		}

		structure::EventState& SaveData::eventState(uint const id)
		{
			switch(id) {
				case EV_ID_PARTY: case EV_ID_BOAT: case EV_ID_SHIP: case EV_ID_AIRSHIP:
					 return (*this)[ 104 + (id-EV_ID_PARTY) ];
				case EV_ID_THIS: rpg2k_assert("Event THIS Not supported."); // TODO
				default:
					return reinterpret_cast< structure::EventState& >( (*this)[111].getArray1D()[11].getArray2D()[id] );
			}
		}

		void SaveData::replace(ChipSet::Type const type, uint dstNo, uint const srcNo)
		{
			rpg2k_assert( rpg2k::within(dstNo, CHIP_REPLACE_MAX) );
			rpg2k_assert( rpg2k::within(srcNo, CHIP_REPLACE_MAX) );

			uint srcVal = chipReplace_[type][srcNo];
			uint dstVal = chipReplace_[type][dstNo];
			chipReplace_[type][dstNo] = srcVal;
			chipReplace_[type][srcNo] = dstVal;
		}
		void SaveData::resetReplace()
		{
			chipReplace_.clear();
			chipReplace_.resize(ChipSet::END);
			for(uint i = 0; i < ChipSet::END; i++) {
				chipReplace_[i].resize(CHIP_REPLACE_MAX);
				for(uint j = 0; j < CHIP_REPLACE_MAX; j++) chipReplace_[i][j] = j;
			}
		}
	} // namespace model
} // namespace rpg2k
