/**
 * @file
 * @brief Load Binaly Core
 * @author project.kuto
 */

#include "kuto_load_binary_core.h"
#include "kuto_file.h"


namespace kuto {

LoadBinaryCore::LoadBinaryCore(const std::string& filename, const char* subname, bool readBytes)
: LoadCore(filename, subname), bytes_(NULL)
{
	if (readBytes)
		bytes_ = File::readBytes(filename_.c_str(), fileSize_);
}

LoadBinaryCore::~LoadBinaryCore() 
{
	releaseBytes();
}

void LoadBinaryCore::releaseBytes()
{
	if (bytes_)
		File::freeBytes(bytes_);
	bytes_ = NULL;
}

}	// namespace kuto
