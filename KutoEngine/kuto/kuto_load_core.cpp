/**
 * @file
 * @brief Load Core Interface
 * @author takuto
 */

#include "kuto_load_core.h"
#include "kuto_utility.h"


namespace kuto {

LoadCore::LoadCore(const std::string& filename, const char* subname)
: filename_(filename), refCount_(1), fileSize_(0)
{
	if (subname)
		subname_ = subname;
	crc_ = crc32(filename_ + subname_);
}

LoadCore::~LoadCore()
{
}


}	// namespace kuto
