#include "Debug.hpp"
#include "MapUnit.hpp"

#include <sstream>


namespace rpg2k
{
	namespace model
	{
		MapUnit::MapUnit()
		: Base("")
		{
			Base::reset();
		}
		MapUnit::MapUnit(SystemString const& dir, SystemString const& name)
		: Base(dir, name), id_(0)
		{
			load();
		}
		MapUnit::MapUnit(SystemString const& dir, unsigned id)
		: Base( dir, SystemString() ), id_(id)
		{
			std::ostringstream ss;
			ss << "Map" << std::setfill('0') << std::setw(4) << id << ".lmu";
			setFileName( ss.str() );

			checkExists();

			load();
		}
		void MapUnit::loadImpl()
		{
			rpg2k_assert( rpg2k::within<unsigned>(ID_MIN, id_, MAP_UNIT_MAX+1) );

			lower_ = (*this)[71].toBinary().convert<uint16_t>();
			upper_ = (*this)[72].toBinary().convert<uint16_t>();

			width_  = (*this)[2];
			height_ = (*this)[3];
		}

		MapUnit::~MapUnit()
		{
		#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ":" << int(id_) << endl;
		#endif
		}

		void MapUnit::saveImpl()
		{
			(*this)[71] = Binary(lower_);
			(*this)[72] = Binary(upper_);

			(*this)[2] = width_ ;
			(*this)[3] = height_;
		}

		int MapUnit::chipIDLw(unsigned const x, unsigned const y) const
		{
			rpg2k_assert( rpg2k::within( x, width () ) );
			rpg2k_assert( rpg2k::within( y, height() ) );

			return lower_[width()*y + x];
		}
		int MapUnit::chipIDUp(unsigned const x, unsigned const y) const
		{
			rpg2k_assert( rpg2k::within( x, width () ) );
			rpg2k_assert( rpg2k::within( y, height() ) );

			return upper_[width()*y + x];
		}
	} // namespace model
} // namespace rpg2k
