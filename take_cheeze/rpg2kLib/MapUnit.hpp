#ifndef _INC__RPG2K__MODEL__MAP_UNIT_HPP
#define _INC__RPG2K__MODEL__MAP_UNIT_HPP

#include "Model.hpp"

namespace rpg2kLib
{
	namespace model
	{

		class MapUnit : protected Base
		{
		private:
			uint ID;

			std::vector< uint16_t > UPPER;
			std::vector< uint16_t > LOWER;

			uint WIDTH, HEIGHT;
		protected:
			void init();

			virtual std::string getHeader() const { return "LcfMapUnit"; }
			virtual std::string defaultName() const { return "Map0000.lmu"; }
		public:
			MapUnit() : Base("") {}
			MapUnit(std::string dir, std::string name);
			MapUnit(std::string dir, uint id);
			virtual ~MapUnit();

			using Base::operator [];

			void save();

			uint getID() const { return ID; }

			int chipIDLw(uint x, uint y) const;
			int chipIDUp(uint x, uint y) const;

			uint getWidth () const { return WIDTH ; }
			uint getHeight() const { return HEIGHT; }

/*
			struct TextureInfo
			{
				kuto::Vector2			texcoord[2];
				const kuto::Texture*	texture;
			};
			struct TextureInfoSet
			{
				TextureInfo		info[8];
				int				size;
			};
 */
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
