#include "Debug.hpp"
#include "MapUnit.hpp"


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
		MapUnit::MapUnit(SystemString const& dir, uint id)
		: Base( dir, SystemString() ), id_(id)
		{
			std::ostringstream ss;
			ss << "Map" << std::setfill('0') << std::setw(4) << id << ".lmu";
			setFileName( ss.str() );

			checkExists();

			load();
		}
		void MapUnit::load()
		{
			rpg2k_assert( rpg2k::within(ID_MIN, id_, MAP_UNIT_MAX+1) );

			Base::load();

			lower_ = (*this)[71].getBinary();
			upper_ = (*this)[72].getBinary();

			width_  = (*this)[2];
			height_ = (*this)[3];
		}

		MapUnit::~MapUnit()
		{
			debug::ANALYZE_RESULT << getHeader() << ":" << int(id_) << endl;
		}

		void MapUnit::save()
		{
			(*this)[71] = lower_;
			(*this)[72] = upper_;

			(*this)[2] = width_ ;
			(*this)[3] = height_;

			Base::save();
		}

		int MapUnit::chipIDLw(uint x, uint y) const
		{
			rpg2k_assert( rpg2k::within( x, getWidth () ) );
			rpg2k_assert( rpg2k::within( y, getHeight() ) );

			return lower_[getWidth()*y + x];
		}
		int MapUnit::chipIDUp(uint x, uint y) const
		{
			rpg2k_assert( rpg2k::within( x, getWidth () ) );
			rpg2k_assert( rpg2k::within( y, getHeight() ) );

			return upper_[getWidth()*y + x];
		}
	} // namespace model
} // namespace rpg2k
