/**
 * @file
 * @brief Load Binaly Core
 * @author project.kuto
 */
#pragma once

#include "kuto_load_core.h"


namespace kuto {

class LoadBinaryCore : public LoadCore
{
public:
	LoadBinaryCore(const std::string& filename, const char* subname, bool readBytes = true);
	virtual ~LoadBinaryCore();
	char* getBytes() { return bytes_; }
	void releaseBytes();

private:
	char*		bytes_;
};

}	// namespace kuto
