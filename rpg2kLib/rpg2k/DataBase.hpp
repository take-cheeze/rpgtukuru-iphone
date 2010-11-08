#ifndef _INC__RPG2K__MODEL__DATA_BASE_HPP
#define _INC__RPG2K__MODEL__DATA_BASE_HPP

#include "Model.hpp"

#include <map>
#include <vector>


namespace rpg2k
{
	namespace model
	{
		class DataBase : public Base
		{
		private:
			typedef std::map< unsigned, std::vector<uint16_t> > Terrain;
			Terrain terrain_;
			typedef std::map< unsigned, std::vector< std::vector<uint8_t> > > ChipFlag;
			ChipFlag chipFlag_;

			std::vector<RPG2kString> vocabulary_;

			virtual void loadImpl();
			virtual void saveImpl();

			virtual char const* header() const { return "LcfDataBase"; }
			virtual char const* defaultName() const { return "RPG_RT.ldb"; }
		public:
			DataBase(SystemString const& dir);
			DataBase(SystemString const& dir, SystemString const& name);
			virtual ~DataBase();

			structure::Array2D& character() { return (*this)[11]; }
			structure::Array2D& skill() { return (*this)[12]; }
			structure::Array2D& item() { return (*this)[13]; }
			structure::Array2D& enemy() { return (*this)[14]; }
			structure::Array2D& enemyGroup() { return (*this)[15]; }
			structure::Array2D& terrain() { return (*this)[16]; }
			structure::Array2D& attribute() { return (*this)[17]; }
			structure::Array2D& condition() { return (*this)[18]; }
			structure::Array2D& battleAnime() { return (*this)[19]; }
			structure::Array2D& chipSet() { return (*this)[20]; }
			structure::Array1D& system() { return (*this)[22]; }
			structure::Array2D& flag() { return (*this)[23]; }
			structure::Array2D& variable() { return (*this)[24]; }
			structure::Array2D& commonEvent() { return (*this)[25]; }

			structure::Array2D const& character() const { return (*this)[11]; }
			structure::Array2D const& skill() const { return (*this)[12]; }
			structure::Array2D const& item() const { return (*this)[13]; }
			structure::Array2D const& enemy() const { return (*this)[14]; }
			structure::Array2D const& enemyGroup() const { return (*this)[15]; }
			structure::Array2D const& terrain() const { return (*this)[16]; }
			structure::Array2D const& attribute() const { return (*this)[17]; }
			structure::Array2D const& condition() const { return (*this)[18]; }
			structure::Array2D const& battleAnime() const { return (*this)[19]; }
			structure::Array2D const& chipSet() const { return (*this)[20]; }
			structure::Array1D const& system() const { return (*this)[22]; }
			structure::Array2D const& flag() const { return (*this)[23]; }
			structure::Array2D const& variable() const { return (*this)[24]; }
			structure::Array2D const& commonEvent() const { return (*this)[25]; }

			RPG2kString const& vocabulary(unsigned index) const;

			std::vector<uint16_t> const& terrain(unsigned id) const;
			std::vector<uint8_t> const& chipFlag(unsigned id, ChipSet::Type t) const;
			std::vector<uint8_t> const& lowerChipFlag(unsigned id) const
			{
				return chipFlag(id, ChipSet::LOWER);
			}
			std::vector<uint8_t> const& upperChipFlag(unsigned id) const
			{
				return chipFlag(id, ChipSet::UPPER);
			}
		}; // class DataBase
	} // namespace model
} // namespace rpg2k

#endif // _INC__RPG2K__MODEL__DATA_BASE_HPP
