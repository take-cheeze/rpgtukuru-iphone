#ifndef _INC__RPG2K__MODEL__SAVE_DATA__HPP
#define _INC__RPG2K__MODEL__SAVE_DATA__HPP

#include "Model.hpp"


namespace rpg2k
{
	namespace model
	{
		class SaveData : public Base
		{
		public:
			struct Item { uint8_t num; uint8_t use; };
			typedef std::map<uint16_t, Item> ItemTable;
		private:
			unsigned id_;

			ItemTable item_;

			std::vector<int32_t> variable_;
			std::vector<uint8_t> switch_  ;

			std::vector<uint16_t> member_;

			std::vector< std::vector<uint8_t> > chipReplace_;

			unsigned currentEventID_;

			virtual void loadImpl();
			virtual void saveImpl();

			virtual char const* header() const { return "LcfSaveData"; }
			virtual char const* defaultName() const { return "Save00.lsd"; }
		public:
			SaveData();
			SaveData(SystemString const& dir, SystemString const& name);
			SaveData(SystemString const& dir, unsigned d);
			virtual ~SaveData();

			SaveData const& operator =(SaveData const& src);

			using Base::operator [];

			unsigned id() const { return id_; }

			std::vector<uint16_t> const& member() const { return member_; }
			std::vector<uint16_t>& member() { return member_; }
			unsigned member(unsigned index) const;
			unsigned memberNum() const { return member_.size(); }
			bool addMember(unsigned charID);
			bool removeMember(unsigned charID);

		// items
			ItemTable const& item() const { return item_; }
			ItemTable& item() { return item_; }
			unsigned itemNum(unsigned id) const;
			void setItemNum(unsigned id, unsigned val);
			void addItemNum(unsigned const id, int const val) { setItemNum( id, int(itemNum(id)) + val ); }
			unsigned itemUse(unsigned id) const;
			void setItemUse(unsigned id, unsigned val);
		// flag and vals
			bool flag(unsigned chipID) const;
			void setFlag(unsigned chipID, bool data);
			int32_t var(unsigned chipID) const;
			void    setVar(unsigned chipID, int32_t data);

			int money() const;
			void setMoney(int data);
			void addMoney(int val) { setMoney( money() + val ); }

			unsigned timerLeft() const { return 0; } // TODO

			unsigned replace(ChipSet::Type const type, unsigned const num) const { return chipReplace_[type][num]; }
			void replace(ChipSet::Type type, unsigned dstNum, unsigned srcNum);
			void resetReplace();

			structure::Array1D& system() { return (*this)[101]; }
			structure::Array1D const& system() const { return (*this)[101]; }

			structure::Array2D& picture() { return (*this)[103]; }
			structure::Array2D const& picture() const { return (*this)[103]; }

			void setCurrentEventID(unsigned const id) { currentEventID_ = id; }
			unsigned currentEventID() const { return currentEventID_; }

			structure::EventState& party()
			{
				return reinterpret_cast<structure::EventState&>( (*this)[104].toArray1D() );
			}
			structure::EventState const& party() const
			{
				return reinterpret_cast<structure::EventState const&>( (*this)[104].toArray1D() );
			}

			structure::Array2D& character() { return (*this)[108]; }
			structure::Array2D const& character() const { return (*this)[108]; }

			structure::Array2D& eventState() { return (*this)[111].toArray1D()[11]; }
			structure::Array2D const& eventState() const { return (*this)[111].toArray1D()[11]; }
			structure::EventState& eventState(unsigned id);
		};
	} // namespace model
} // namespace rpg2k

#endif
