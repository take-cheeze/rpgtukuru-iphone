#include "Main.hpp"
#include "Material.hpp"


namespace rpg2k
{
	Material::Material(Main& m)
	: owner_(m)
	{
		#define PP_addElement(name) \
			directories_.insert( std::make_pair( Material::name, SystemString(#name) ) );
		PP_allMaterial(PP_addElement)
		#undef PP_addElement
		resources_.push_back( m.getGameDir() );
		resources_.push_back( m.getRTPDir () );
	}
	Material::~Material()
	{
	}
	SystemString const& Material::getName(Material::Type t) const
	{
		rpg2k_assert( directories_.find(t) != directories_.end() );
		return directories_.find(t)->second;
	}
	SystemString const& Material::getResource(Material::Resource t) const
	{
		rpg2k_assert( rpg2k::within( uint(t), resources_.size() ) );
		return resources_[t];
	}
	SystemString Material::getFileName(Material::Resource dir, Material::Type type, RPG2kString const& name, SystemString const& ext) const
	{
		return (getResource(dir) + PATH_SEPR + getName(type) + PATH_SEPR + name.toSystem() + "." + ext);
	}
} // namespace rpg2k
