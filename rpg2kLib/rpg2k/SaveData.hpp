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
		private:
			uint id_;

			std::map< uint16_t, Item > item_;

			std::vector< int32_t > variable_;
			std::vector< uint8_t > switch_  ;

			std::vector< uint16_t > member_;

			std::vector< std::vector< uint8_t > > chipReplace_;

			std::map< uint, std::vector< uint16_t > > charSkill_;
		protected:
			virtual void load();

			virtual char const* getHeader() const { return "LcfSaveData"; }
			virtual char const* defaultName() const { return "Save00.lsd"; }
		public:
			SaveData();
			SaveData(SystemString const& dir, SystemString const& name);
			SaveData(SystemString const& dir, uint id);
			virtual ~SaveData();

			SaveData& operator =(SaveData const& src);

			virtual void save();

			using Base::operator [];

			uint getID() const { return id_; }

			std::vector< uint16_t >& member() { return member_; }
			uint member(uint index) const;
			uint memberNum() const { return member_.size(); }

			bool validPageMap   (structure::Array1D const& term) const;
			bool validPageBattle(structure::Array1D const& term) const;

		// items
			std::map< uint16_t, Item >& item() { return item_; }
			bool hasItem(uint id) const;
			uint getItemNum(uint id) const;
			void setItemNum(uint id, uint val);
			void addItemNum(uint id, int val) { setItemNum( id, int(getItemNum(id)) + val ); }
			uint getItemUse(uint id) const;
			void setItemUse(uint id, uint val);
		// flag and vals
			bool getFlag(uint id) const;
			void setFlag(uint id, bool data);
			int32_t getVar(uint id) const;
			void    setVar(uint id, int32_t data);

			int getMoney() const;
			void setMoney(int data);
			void addMoney(int val) { setMoney( getMoney() + val ); }

			uint timerLeft() const { return 0; } // TODO

			structure::Array2D& eventState() { return (*this)[111].getArray1D()[11]; }
			structure::EventState& eventState(uint id);

			structure::Array2D& character() { return (*this)[108]; }
			structure::Array2D const& character() const { return (*this)[108]; }
			std::vector< uint16_t >& skill(uint charID) { return charSkill_[charID]; } // .find( charID )->second; }

			uint getReplace(ChipSet::Type type, uint num) const { return chipReplace_[type][num]; }
			void replace(ChipSet::Type type, uint dstNum, uint srcNum);
			void resetReplace();
		};
	} // namespace model
} // namespace rpg2k

#endif
