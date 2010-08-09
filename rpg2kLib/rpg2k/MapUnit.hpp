#ifndef _INC__RPG2K__MODEL__MAP_UNIT_HPP
#define _INC__RPG2K__MODEL__MAP_UNIT_HPP

#include "Model.hpp"

namespace rpg2k
{
	namespace model
	{
		class MapUnit : public Base
		{
		private:
			uint id_;

			std::vector< uint16_t > upper_;
			std::vector< uint16_t > lower_;

			uint width_, height_;
		protected:
			virtual void load();

			virtual char const* getHeader() const { return "LcfMapUnit"; }
			virtual char const* defaultName() const { return "Map0000.lmu"; }
		public:
			MapUnit();
			MapUnit(SystemString const& dir, SystemString const& name);
			MapUnit(SystemString const& dir, uint id);
			virtual ~MapUnit();

			virtual void save();

			uint getID() const { return id_; }

			int chipIDLw(uint x, uint y) const;
			int chipIDUp(uint x, uint y) const;
			int chipIDLw(Vector2D const& pos) const { return chipIDLw(pos[0], pos[1]); }
			int chipIDUp(Vector2D const& pos) const { return chipIDUp(pos[0], pos[1]); }

			uint getWidth () const { return  width_; }
			uint getHeight() const { return height_; }
		};
	} // namespace model
} // namespace rpg2k

#endif
