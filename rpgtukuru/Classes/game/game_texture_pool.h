#include <map>
#include <memory>
#include <string>

#include <boost/smart_ptr.hpp>

#include <kuto/kuto_texture.h>
#include <kuto/kuto_array.h>

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

	kuto::Texture& get(GameTexturePool::Type t, std::string const& name);
	kuto::Texture& getPicture(std::string const& name, bool const trans);

	void clear();

protected:
	std::auto_ptr< kuto::Texture > load(GameTexturePool::Type t, std::string const& name, bool const trans) const;

private:
	typedef std::map< std::string, boost::shared_ptr<kuto::Texture> > TexPoolType;

	rpg2k::model::Project const& proj_;
	kuto::Array<TexPoolType, TYPE_END> pool_;
	kuto::Array<TexPoolType, 2> picturePool_;

	static char const* DIR_NAME[];
	static bool const TRANS[];
}; // class GameTexturePool
