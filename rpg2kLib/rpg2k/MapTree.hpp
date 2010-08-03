#ifndef _INC__RPG2K__MODEL__MAP_TREE_HPP
#define _INC__RPG2K__MODEL__MAP_TREE_HPP

#include "Model.hpp"

namespace rpg2k
{
	namespace model
	{
		class MapTree : public Base
		{
		private:
			// map< uint, bool > exists_;
			// BerEnum& getExist() { return getData()[1]; }
		protected:
			virtual void load();

			virtual char const* getHeader() const { return "LcfMapTree"; }
			virtual char const* defaultName() const { return "RPG_RT.lmt"; }

			using Base::operator [];
		public:
			MapTree(SystemString const& dir);
			MapTree(SystemString const& dir, SystemString const& name);
			virtual ~MapTree();

			structure::Array1D& operator [](uint mapID) const
			{
				return getData().front()->getArray2D()[mapID];
			}

			bool exists(uint mapID) const { return getData().front()->getArray2D().exists(mapID); }
			bool canTeleport(uint mapID) const;
			bool canEscape  (uint mapID) const;
			bool canSave    (uint mapID) const;

			structure::Array1D& getStartPoint() const { return *getData().back(); }
		};
	} // namespace model
} // namespace rpg2k

#endif
