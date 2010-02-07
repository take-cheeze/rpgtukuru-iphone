/**
 * @file
 * @brief Load Binaly
 * @author takuto
 */
#pragma once

#include "kuto_load_handle.h"


namespace kuto {

class LoadBinaryHandle : public LoadHandle
{
public:
	char* getData();
	
private:
	virtual LoadCore* createCore(const std::string& filename, const char* subname);
};

}	// namespace kuto
