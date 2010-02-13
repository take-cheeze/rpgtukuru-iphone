#ifndef _INC__RPG2K__MODEL__SAVE_DATA_HPP
#define _INC__RPG2K__MODEL__SAVE_DATA_HPP

#include "Model.hpp"

namespace rpg2kLib
{
	namespace model
	{

		class SaveData : public Base
		{
		private:
			uint ID;

			struct Item { uint8_t num; uint8_t use; };
			map< uint16_t, Item > ITEM;

			vector< int32_t > VARIABLE;
			vector< uint8_t > SWITCH  ;

			vector< uint16_t > MEMBER;

			vector< vector< uint8_t > > CHIP_REPLACE;
		protected:
			void init();

			virtual string getHeader() const { return "LcfSaveData"; }
			virtual string defaultName() const { return "Save00.lsd"; }
		public:
			SaveData() : Base("", "") {}
			SaveData(string dir, string name="");
			SaveData(string dir, uint id);
			virtual ~SaveData();

			SaveData& operator =(SaveData& src);

			virtual void save();

			using Base::operator [];

			uint getID() const { return ID; }

			vector< uint16_t >& member() { return MEMBER; }
			uint member(uint index);
			uint memberNum() { return MEMBER.size(); }

			bool validPageMap   (Array1D& term);
			bool validPageBattle(Array1D& term);

		// items
			bool hasItem(uint id);
			uint getItemNum(uint id);
			void setItemNum(uint id, uint val);
			uint getItemUse(uint id);
			void setItemUse(uint id, uint val);
		// flag and vals
			bool getFlag(uint id);
			void setFlag(uint id, bool data);
			int32_t getVar(uint id);
			void    setVar(uint id, int32_t data);

			int getMoney();
			void setMoney(int data);

			uint timerLeft() { return 0; }

			Array2D& eventState()
			{
				return static_cast< Array1D& >( (*this)[111] )[11];
			}
			EventState& eventState(uint id);

			Array2D& charParam() { return (*this)[108]; }
			Array1D& charParam(uint id)
			{
				return static_cast< Array1D& >( (*this)[108] )[id];
			}

			uint getReplace(ChipSetType type, uint num) { return CHIP_REPLACE[type][num]; }
			void replace(ChipSetType type, uint dstNum, uint srcNum);
			void resetReplace();
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
