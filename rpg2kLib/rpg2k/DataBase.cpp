#include "Debug.hpp"
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
		#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ":" << endl;
		#endif
		}

		void DataBase::loadImpl()
		{
		// load chip infos
			structure::Array2D const& chips = (*this)[20];
			for(structure::Array2D::ConstIterator it = chips.begin(); it != chips.end(); ++it) {
				terrain_.insert( std::make_pair( it->first, (*it->second)[3].toBinary().convert<uint16_t>() ) );

				std::vector< std::vector<uint8_t> >& dst = chipFlag_[it->first];
				dst.push_back( (*it->second)[4].to<Binary>() );
				dst.push_back( (*it->second)[5].to<Binary>() );
			}
		// copying vocabulary
			structure::Array1D const& vocSrc = (*this)[21];
			for(structure::Array1D::ConstIterator it = vocSrc.begin(); it != vocSrc.end(); ++it) {
				if( it->first >= vocabulary_.size() ) {
					vocabulary_.resize(it->first + 1);
				}
				vocabulary_[it->first] = it->second->to_string();
			}
		}
		void DataBase::saveImpl()
		{
		// save chip info
			structure::Array2D& chips = (*this)[20];
			for(structure::Array2D::Iterator it = chips.begin(); it != chips.end(); ++it) {
				if( !it->second->exists() ) continue;

				(*it->second)[3] = Binary( terrain(it->first) );

				(*it->second)[4] = Binary( upperChipFlag(it->first) );
				(*it->second)[5] = Binary( lowerChipFlag(it->first) );
			}
		// saving vocabulary
			structure::Array1D& vocDst = (*this)[21];
			for(std::vector<RPG2kString>::const_iterator it = vocabulary_.begin()
			; it < vocabulary_.end(); ++it) {
				if( !it->empty() ) { vocDst[ it - vocabulary_.begin() ] = *it; }
			}
		}

		std::vector<uint8_t> const& DataBase::chipFlag(unsigned id, ChipSet::Type t) const
		{
			ChipFlag::const_iterator it = chipFlag_.find(id);
			rpg2k_assert( it != chipFlag_.end() );
			rpg2k_assert( rpg2k::within<unsigned>( t, it->second.size() ) );
			return it->second[t];
		}
		std::vector<uint16_t> const& DataBase::terrain(unsigned const id) const
		{
			Terrain::const_iterator it = terrain_.find(id);
			rpg2k_assert( it != terrain_.end() );
			return it->second;
		}

		RPG2kString const& DataBase::vocabulary(unsigned const index) const
		{
			rpg2k_assert( index < vocabulary_.size() );
			// rpg2k_assert( !vocabulary_[index].empty() );
			return vocabulary_[index];
		}
	} // namespace model
} // namespace rpg2k
