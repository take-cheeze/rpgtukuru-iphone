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
			std::map< uint, std::vector< uint16_t > > charStatus_;

			std::map< uint, std::vector< uint16_t > > terrain_;
			std::map< uint, std::vector< std::vector< uint8_t > > > chipFlag_;

			std::map< uint, RPG2kString > vocabulary_; // maybe std::vector is better
		protected:
			virtual void load();

			virtual char const* getHeader() const { return "LcfDataBase"; }
			virtual char const* defaultName() const { return "RPG_RT.ldb"; }
		public:
			DataBase(SystemString const& dir);
			DataBase(SystemString const& dir, SystemString const& name);
			virtual ~DataBase();

			uint getBasicStatus(int charID, int level, Param::Type t) const;

			virtual void save();

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
			structure::Array2D& switchData() { return (*this)[23]; }
			structure::Array2D& variableData() { return (*this)[24]; }
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
			structure::Array2D const& switchData() const { return (*this)[23]; }
			structure::Array2D const& variableData() const { return (*this)[24]; }
			structure::Array2D const& commonEvent() const { return (*this)[25]; }

			RPG2kString const& vocabulary(uint index) const;

			std::vector< uint8_t >& chipFlag(uint id, ChipSet::Type t);
			std::vector< uint8_t >& lowerChipFlag(uint id) { return chipFlag(id, ChipSet::LOWER); }
			std::vector< uint8_t >& upperChipFlag(uint id) { return chipFlag(id, ChipSet::UPPER); }
			std::vector< uint16_t >& terrain(uint id);
		}; // class DataBase
	} // namespace model
} // namespace rpg2k

#endif // _INC__RPG2K__MODEL__DATA_BASE_HPP
