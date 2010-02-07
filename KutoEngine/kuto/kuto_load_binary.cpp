/**
 * @file
 * @brief Load Binaly
 * @author takuto
 */

#include "kuto_load_binary.h"
#include "kuto_load_binary_core.h"


namespace kuto {

LoadCore* LoadBinaryHandle::createCore(const std::string& filename, const char* subname)
{
	return new LoadBinaryCore(filename, subname);
}

char* LoadBinaryHandle::getData()
{
	if (core_)
		return static_cast<LoadBinaryCore*>(core_)->getBytes();
	return NULL;
}

}	// namespace kuto

