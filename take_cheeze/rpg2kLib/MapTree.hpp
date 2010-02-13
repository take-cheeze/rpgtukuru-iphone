#ifndef _INC__RPG2K__MODEL__MAP_TREE_HPP
#define _INC__RPG2K__MODEL__MAP_TREE_HPP

#include "Model.hpp"

namespace rpg2kLib
{
	namespace model
	{

		class MapTree : protected Base
		{
		private:
			map< uint, bool > EXISTS;
		protected:
			void init();

			virtual string getHeader() const { return "LcfMapTree"; }
			virtual string defaultName() const { return "RPG_RT.lmt"; }

			using Base::operator [];

			BerEnum& getExist() { return getData()[1]; }
		public:
			MapTree(string dir, string name="");
			virtual ~MapTree();

			using Base::save;

			bool exists(uint mapID);

			Array1D& operator [](uint mapID)
			{
				// clog << "MapTree: ";

				return ( (Array2D&)getData()[0] )[mapID];
			}

			Array1D& getStartPoint() { return getData()[2]; }
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
