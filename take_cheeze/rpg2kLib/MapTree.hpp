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
			// map< uint, bool > EXISTS;
			// BerEnum& getExist() { return getData()[1]; }
		protected:
			void init();

			virtual string getHeader() const { return "LcfMapTree"; }
			virtual string defaultName() const { return "RPG_RT.lmt"; }

			using Base::operator [];
		public:
			MapTree(string dir);
			MapTree(string dir, string name);
			virtual ~MapTree();

			using Base::save;

			Array1D& operator [](uint mapID) const
			{
				return static_cast< Array2D& >( getData()[0] )[mapID];
			}

			bool exists(uint mapID) const { return static_cast< Array2D& >( getData()[0] ).exists(mapID); }
			bool canTeleport(uint mapID) const;
			bool canEscape  (uint mapID) const;
			bool canSave    (uint mapID) const;

			Array1D& getStartPoint() { return getData()[2]; }
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
