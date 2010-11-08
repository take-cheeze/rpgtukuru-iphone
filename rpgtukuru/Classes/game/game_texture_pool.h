#pragma once

#include <map>
#include <memory>
#include <string>

#include <kuto/kuto_texture.h>
#include <kuto/kuto_array.h>

#include <boost/array.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>

namespace rpg2k { namespace model { class Project; } }

#define PP_nonTransMaterial(func) \
	func(Backdrop) \
	func(FaceSet) \
	func(GameOver) \
	func(Panorama) \
	func(Title) \

#define PP_transMaterial(func) \
	func(Battle) \
	func(CharSet) \
	func(ChipSet) \
	func(Monster) \
	func(System) \

#define PP_imageMaterial(func) \
	PP_transMaterial(func) PP_nonTransMaterial(func) \
	func(Picture) \


class GameTexturePool
{
public:
	enum Type
	{
		#define PP_enum(name) name,
		PP_imageMaterial(PP_enum)
		#undef PP_enum
		TYPE_END,
	};

public:
	GameTexturePool(rpg2k::model::Project const& p);
	~GameTexturePool();

	kuto::Texture& get(GameTexturePool::Type t, rpg2k::SystemString const& name);
	kuto::Texture& picture(rpg2k::SystemString const& name, bool const trans);

	void clear();

protected:
	std::auto_ptr< kuto::Texture > load(GameTexturePool::Type t, std::string const& name, bool const trans) const;

private:
	typedef boost::ptr_unordered_map<std::string, kuto::Texture> TexPoolType;

	rpg2k::model::Project const& proj_;
	boost::array<TexPoolType, TYPE_END> pool_;
	boost::array<TexPoolType, 2> picturePool_;

	static char const* DIR_NAME[];
	static bool const TRANS[];
}; // class GameTexturePool
