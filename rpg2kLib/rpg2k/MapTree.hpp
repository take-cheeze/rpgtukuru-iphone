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
			virtual void loadImpl();
			virtual void saveImpl();

			virtual char const* header() const { return "LcfMapTree"; }
			virtual char const* defaultName() const { return "RPG_RT.lmt"; }
		protected:
			using Base::operator []; // since LMT's first element is Array2D
		public:
			MapTree(SystemString const& dir);
			MapTree(SystemString const& dir, SystemString const& name);
			virtual ~MapTree();

			structure::Array1D const& operator [](unsigned mapID) const;
			structure::Array1D& operator [](unsigned mapID);

			bool exists(unsigned mapID) const;

			bool canTeleport(unsigned mapID) const;
			bool canEscape  (unsigned mapID) const;
			bool canSave    (unsigned mapID) const;

			structure::Array1D const& startPoint() const { return data().back(); }
		};
	} // namespace model
} // namespace rpg2k

#endif
