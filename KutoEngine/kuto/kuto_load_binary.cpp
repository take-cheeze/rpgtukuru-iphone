/**
 * @file
 * @brief Load Binaly
 * @author project.kuto
 */

#include "kuto_load_binary.h"
#include "kuto_load_binary_core.h"


namespace kuto {

LoadCore* LoadBinaryHandle::createCore(const std::string& filename, const char* subname)
{
	return new LoadBinaryCore(filename, subname);
}

char* LoadBinaryHandle::data()
{
	if (core_)
		return static_cast<LoadBinaryCore*>(core_)->bytes();
	return NULL;
}

}	// namespace kuto

