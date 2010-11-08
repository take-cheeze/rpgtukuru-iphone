#include "Debug.hpp"
#include "MapTree.hpp"


namespace rpg2k
{
	namespace model
	{
		MapTree::MapTree(SystemString const& dir)
		: Base(dir)
		{
			load();
		}
		MapTree::MapTree(SystemString const& dir, SystemString const& name)
		: Base(dir, name)
		{
			load();
		}

		MapTree::~MapTree()
		{
		#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ":" << endl;
		#endif
		}

		void MapTree::loadImpl()
		{
		}
		void MapTree::saveImpl()
		{
		}

		structure::Array1D const& MapTree::operator [](unsigned mapID) const
		{
			return data().front().toArray2D()[mapID];
		}
		structure::Array1D& MapTree::operator [](unsigned mapID)
		{
			return data().front().toArray2D()[mapID];
		}

		bool MapTree::exists(unsigned const mapID) const
		{
			return data().front().toArray2D().exists(mapID);
		}

		bool MapTree::canTeleport(unsigned const mapID) const
		{
			switch( (*this)[mapID][31].to<int>() ) {
				case 0: return canTeleport( (*this)[mapID][2] );
				case 1: return true ;
				case 2: return false;
				default: rpg2k_assert(false); return false;
			}
		}
		bool MapTree::canEscape(unsigned const mapID) const
		{
			switch( (*this)[mapID][32].to<int>() ) {
				case 0: return canTeleport( (*this)[mapID][2] );
				case 1: return true ;
				case 2: return false;
				default: rpg2k_assert(false); return false;
			}
		}
		bool MapTree::canSave(unsigned mapID) const
		{
			switch( (*this)[mapID][33].to<int>() ) {
				case 0: return canTeleport( (*this)[mapID][2] );
				case 1: return true ;
				case 2: return false;
				default: rpg2k_assert(false); return false;
			}
		}
	} // namespace model
} // namespace rpg2k
