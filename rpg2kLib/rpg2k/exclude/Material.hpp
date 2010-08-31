#ifndef _INC_RPG2K__MATERIAL_HPP
#define _INC_RPG2K__MATERIAL_HPP

#include "Define.hpp"

#include <map>
#include <vector>


namespace rpg2k
{
	#define PP_nonTransMaterial(func) \
		func(Backdrop) \
		func(FaceSet) \
		func(GameOver) \
		func(Panorama) \
		func(Title)
	#define PP_transMaterial(func) \
		func(Battle) \
		func(CharSet) \
		func(ChipSet) \
		func(Monster) \
		func(System)
	#define PP_imageMaterial(func) \
		PP_transMaterial(func) PP_nonTransMaterial(func) \
		func(Picture)

	#define PP_allMaterial(func) \
		PP_imageMaterial(func) \
		func(Movie) \
		func(Music) \
		func(Sound)

	class Main;

	class Material
	{
	public:
		enum Type
		{
			#define PP_enum(name) name,
			PP_allMaterial(PP_enum)
			#undef PP_enum
			TYPE_END,
		};
		enum Resource { GAME = 0, RTP, RES_END, };
	private:
		Main& owner_;
		// enum2dirName table
		std::map< Material::Type, SystemString > directories_;
		std::vector< SystemString > resources_;
	public:
		Material(Main& m);
		~Material();

		SystemString const& getName(Material::Type t) const;
		SystemString const& getResource(Material::Resource t) const;
		SystemString getFileName(Material::Resource dir, Material::Type type, RPG2kString const& name, SystemString const& ext) const;
	}; // class Material
} // namespace rpg2k

#endif
