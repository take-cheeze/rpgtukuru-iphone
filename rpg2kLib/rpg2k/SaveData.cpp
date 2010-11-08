#include "Debug.hpp"
#include "SaveData.hpp"
#include "Structure.hpp"

#include <algorithm>
#include <sstream>


namespace rpg2k
{
	namespace model
	{
		SaveData::SaveData()
		: Base( SystemString(), SystemString() ), id_(-1)
		{
			Base::reset();

		// reset map chip info
			resetReplace();
		}
		SaveData::SaveData(SystemString const& dir, SystemString const& name)
		: Base(dir, name), id_(0)
		{
			load();
		}
		SaveData::SaveData(SystemString const& dir, unsigned const id)
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
		#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ": " << int(id_) << endl;
		#endif
		}

		SaveData const& SaveData::operator =(SaveData const& src)
		{
			this->data() = src.data();

			this->item_ = src.item_;

			this->variable_ = src.variable_;
			this->switch_   = src.switch_  ;

			this->member_ = src.member_;

			this->chipReplace_ = src.chipReplace_;

			return *this;
		}

		void SaveData::loadImpl()
		{
			structure::Array1D& sys    = (*this)[101];
			structure::Array1D& status = (*this)[109];
			structure::Array1D& event  = (*this)[111];

		// item
			{
				int itemTypeNum = status[11];
				std::vector<uint16_t> id  = status[12].toBinary();
				std::vector<uint8_t > num = status[13].toBinary();
				std::vector<uint8_t > use = status[14].toBinary();

				for(int i = 0; i < itemTypeNum; i++) {
					if( !num[i] ) continue;

					Item info = { num[i], use[i] };
					item_.insert( std::make_pair(id[i], info) );
				}
			}
		// switch and variable
			switch_.resize(sys[31]);
			switch_ = sys[32].toBinary();
			variable_.resize(sys[33]);
			variable_ = sys[34].toBinary();
		// member
			member_.resize(status[1]);
			member_ = status[2].toBinary();
		// chip replace
			chipReplace_.resize(ChipSet::END);
			for(unsigned i = 0; i < ChipSet::END; i++) {
				chipReplace_[i] = event[21+i].toBinary();
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
				for(ItemTable::const_iterator it = item_.begin(); it != item_.end(); ++it) {
					id [i] = it->first;
					num[i] = it->second.num;
					use[i] = it->second.use;

					i++;
				}
				status[12] = Binary(id );
				status[13] = Binary(num);
				status[14] = Binary(use);
			}
		// switch and variable
			sys[31] = switch_.size();
			sys[32] = Binary(switch_);
			sys[33] = variable_.size();
			sys[34] = Binary(variable_);
		// member
			(*this)[109].toArray1D()[1] = member_.size();
			(*this)[109].toArray1D()[2] = Binary(member_);
		// chip replace
			for(unsigned i = ChipSet::BEGIN; i < ChipSet::END; i++) {
				(*this)[111].toArray1D()[21+i] = Binary(chipReplace_[i]);
			}
		}

		bool SaveData::addMember(unsigned const charID)
		{
			if( (member_.size() > rpg2k::MEMBER_MAX)
			|| std::find( member_.begin(), member_.end(), charID ) == member_.end() ) return false;
			else {
				member_.push_back(charID);
				return true;
			}
		}
		bool SaveData::removeMember(unsigned const charID)
		{
			std::vector<uint16_t>::iterator it = std::find( member_.begin(), member_.end(), charID );
			if( it != member_.end() ) {
				member_.erase(it);
				return true;
			} else return false;
		}

		bool SaveData::flag(unsigned const id) const
		{
			return ( id < switch_.size() ) ? switch_[id - ID_MIN] : SWITCH_DEF_VAL;
		}
		void SaveData::setFlag(unsigned id, bool data)
		{
			if( id >= switch_.size() ) switch_.resize(id, SWITCH_DEF_VAL);
			switch_[id - ID_MIN] = data;
		}

		int32_t SaveData::var(unsigned const id) const
		{
			return ( id < variable_.size() ) ? variable_[id - ID_MIN] : VAR_DEF_VAL;
		}
		void SaveData::setVar(unsigned const id, int32_t const data)
		{
			if( id >= variable_.size() ) variable_.resize(id, VAR_DEF_VAL);
			variable_[id - ID_MIN] = data;
		}

		int SaveData::money() const
		{
			return (*this)[109].toArray1D()[21];
		}
		void SaveData::setMoney(int const data)
		{
			if(data < MONEY_MIN) (*this)[109].toArray1D()[21] = int(MONEY_MIN);
			else if(MONEY_MAX < data) (*this)[109].toArray1D()[21] = int(MONEY_MAX);
			else (*this)[109].toArray1D()[21] = data;
		}

		unsigned SaveData::itemNum(unsigned const id) const
		{
			ItemTable::const_iterator it = item_.find(id);
			return ( it == item_.end() )? ITEM_MIN : it->second.num;
		}
		void SaveData::setItemNum(unsigned const id, unsigned const val)
		{
			unsigned validVal = val;
			if(validVal < ITEM_MIN) validVal = ITEM_MIN;
			else if(ITEM_MAX < validVal) validVal = ITEM_MAX;

			if( item_.find(id) == item_.end() ) {
				Item const i = { validVal, 0 };
				item_.insert( std::make_pair(id, i) );
			} else item_[id].num = validVal;

			if( validVal == 0 ) item_.erase(id);
		}

		unsigned SaveData::itemUse(unsigned const id) const
		{
			ItemTable::const_iterator it = item_.find(id);
			return ( it == item_.end() ) ? ITEM_MIN : it->second.use;
		}
		void SaveData::setItemUse(unsigned const id, unsigned const val)
		{
			ItemTable::iterator it = item_.find(id);
			if( it != item_.end() ) it->second.use = val;
		}

		unsigned SaveData::member(unsigned const index) const
		{
			rpg2k_assert( rpg2k::within<unsigned>( index, member_.size() ) );
			return member_[index];
		}

		structure::EventState& SaveData::eventState(unsigned id)
		{
			switch(id) {
				case ID_PARTY: case ID_BOAT: case ID_SHIP: case ID_AIRSHIP:
					 return (*this)[ 104 + (id-ID_PARTY) ];
				case ID_THIS: id = currentEventID(); // TODO
				default:
					return reinterpret_cast< structure::EventState& >(
						(*this)[111].toArray1D()[11].toArray2D()[id] );
			}
		}

		void SaveData::replace(ChipSet::Type const type, unsigned dstNo, unsigned const srcNo)
		{
			rpg2k_assert( rpg2k::within<unsigned>(dstNo, CHIP_REPLACE_MAX) );
			rpg2k_assert( rpg2k::within<unsigned>(srcNo, CHIP_REPLACE_MAX) );

			unsigned srcVal = chipReplace_[type][srcNo];
			unsigned dstVal = chipReplace_[type][dstNo];
			chipReplace_[type][dstNo] = srcVal;
			chipReplace_[type][srcNo] = dstVal;
		}
		void SaveData::resetReplace()
		{
			chipReplace_.clear();
			chipReplace_.resize(ChipSet::END);
			for(unsigned i = ChipSet::BEGIN; i < ChipSet::END; i++) {
				chipReplace_[i].resize(CHIP_REPLACE_MAX);
				for(unsigned j = 0; j < CHIP_REPLACE_MAX; j++) chipReplace_[i][j] = j;
			}
		}
	} // namespace model
} // namespace rpg2k
