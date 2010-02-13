#ifndef _INC__RPG2K__MODEL__MAP_UNIT_HPP
#define _INC__RPG2K__MODEL__MAP_UNIT_HPP

#include "Model.hpp"

namespace rpg2kLib
{
	namespace model
	{

		class MapUnit : protected Base
		{
		private:
			uint ID;

			vector< uint16_t > UPPER;
			vector< uint16_t > LOWER;

			uint WIDTH, HEIGHT;
		protected:
			void init();

			virtual string getHeader() const { return "LcfMapUnit"; }
			virtual string defaultName() const { return "Map0000.lmu"; }
		public:
			MapUnit() : Base("", "") {}
			MapUnit(string dir, string name="");
			MapUnit(string dir, uint id);
			virtual ~MapUnit();

			using Base::operator [];

			void save();

			uint getID() const { return ID; }

			int chipIDLw(uint x, uint y);
			int chipIDUp(uint x, uint y);

			uint getWidth () { return WIDTH ; }
			uint getHeight() { return HEIGHT; }
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
