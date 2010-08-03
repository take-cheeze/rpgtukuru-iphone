#include "DataBase.hpp"


namespace rpg2k
{
	namespace model
	{
		DataBase::DataBase(SystemString const& dir)
		: Base(dir)
		{
			load();
		}
		DataBase::DataBase(SystemString const& dir, SystemString const& name)
		: Base(dir, name)
		{
			load();
		}
		DataBase::~DataBase()
		{
		}

		void DataBase::load()
		{
			Base::load();
		// load basic status
			structure::Array2D const& chars = (*this)[11];
			for(structure::Array2D::Iterator it = chars.begin(); it != chars.end(); ++it) {
				// if( !it.second().exists() ) continue;

				charStatus_.insert( std::map< uint, std::vector< uint16_t > >::value_type( it.first(), it.second()[31].get<Binary>() ) );
			}
		// load chip infos
			structure::Array2D const& chips = (*this)[20];
			for(structure::Array2D::Iterator it = chips.begin(); it != chips.end(); ++it) {
				if( !it.second().exists() ) continue;

				terrain_.insert( std::map< uint, std::vector< uint16_t > >::value_type( it.first(), it.second()[3].get<Binary>() ) );

				std::vector< std::vector< uint8_t > >& dst = chipFlag_[it.first()];
				 dst.push_back( it.second()[4].get<Binary>() );
				 dst.push_back( it.second()[5].get<Binary>() );
			}
		// copying vocabulary
			structure::Array1D const& vocSrc = (*this)[21];
			for(structure::Array1D::Iterator it = vocSrc.begin(); it != vocSrc.end(); ++it) {
				if( !it.second().exists() ) continue;

				vocabulary_.insert( std::make_pair( it.first(), it.second().get_string() ) );
			}
		}
		void DataBase::save()
		{
		// save basic status
			structure::Array2D& chars = (*this)[11];
			for(structure::Array2D::Iterator it = chars.begin(); it != chars.end(); ++it) {
				if( !it.second().exists() ) continue;

				rpg2k_assert( charStatus_.find( it.first() ) != charStatus_.end() );
				it.second()[31] = charStatus_.find( it.first() )->second;
			}
		// save chip info
			structure::Array2D const& chips = (*this)[20];
			for(structure::Array2D::Iterator it = chips.begin(); it != chips.end(); ++it) {
				if( !it.second().exists() ) continue;

				rpg2k_assert( terrain_.find( it.first() ) != terrain_.end() );
				it.second()[3] = terrain_.find( it.first() )->second;
				rpg2k_assert( chipFlag_.find( it.first() ) != chipFlag_.end() );
				it.second()[4] = chipFlag_.find( it.first() )->second[ChipSet::LOWER];
				it.second()[5] = chipFlag_.find( it.first() )->second[ChipSet::UPPER];
			}
		// saving vocabulary
			structure::Array1D& vocDst = (*this)[21];
			for(std::map< uint, RPG2kString >::const_iterator it = vocabulary_.begin(); it != vocabulary_.end(); ++it) {
				vocDst[it->first] = it->second;
			}

			Base::save();
		}

		uint DataBase::getBasicStatus(int charID, int level, Param::Type t) const
		{
			rpg2k_assert( charStatus_.find(charID) != charStatus_.end() );
			std::vector< uint16_t > const& data = charStatus_.find(charID)->second;
			rpg2k_assert( rpg2k::within( data.size() / Param::END * t + --level, data.size() ) );
			return data[data.size() / Param::END * t + --level];
		}
		std::vector< uint8_t >& DataBase::chipFlag(uint id, ChipSet::Type t)
		{
			rpg2k_assert( chipFlag_.find(id) != chipFlag_.end() );
			rpg2k_assert( rpg2k::within( uint(t), chipFlag_.find(id)->second.size() ) );
			return chipFlag_.find(id)->second[t];
		}
		std::vector< uint16_t >& DataBase::terrain(uint id)
		{
			rpg2k_assert( terrain_.find(id) != terrain_.end() );
			return terrain_.find(id)->second;
		}

		RPG2kString const& DataBase::vocabulary(uint index) const
		{
			rpg2k_assert( vocabulary_.find(index) != vocabulary_.end() );
			return vocabulary_.find(index)->second;
		}
	} // namespace model
} // namespace rpg2k
