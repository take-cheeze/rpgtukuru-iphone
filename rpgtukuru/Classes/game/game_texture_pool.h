#pragma once

#include <map>
#include <memory>
#include <string>

#include <kuto/kuto_texture.h>
#include <kuto/kuto_array.h>

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
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


class GameTexturePool : boost::noncopyable
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

	kuto::Texture& get(GameTexturePool::Type t, rpg2k::SystemString const& name);
	kuto::Texture& picture(rpg2k::SystemString const& name, bool const trans);

	void clear();

protected:
	std::auto_ptr< kuto::Texture > load(GameTexturePool::Type t
	, std::string const& name, bool const trans) const;

private:
	rpg2k::model::Project const& project_;

	typedef boost::ptr_unordered_map<std::string, kuto::Texture> Pool;
	boost::array<Pool, TYPE_END> pool_;
	boost::array<Pool, 2> picturePool_;

	static boost::array<char const*, TYPE_END> const DIR_NAME;
	static boost::array<bool, TYPE_END> const TRANS;
}; // class GameTexturePool
