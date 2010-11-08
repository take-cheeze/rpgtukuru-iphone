/**
 * @file
 * @brief Load Core Interface
 * @author project.kuto
 */
#pragma once

#include <string>
#include "kuto_types.h"


namespace kuto {

class LoadCore
{
public:
	LoadCore(const std::string& filename, const char* subname = NULL);
	virtual ~LoadCore();

	void incRefCount() { refCount_++; }
	void decRefCount() { refCount_--; }
	int refCount() const { return refCount_; }
	const std::string& filename() const { return filename_; }
	const std::string& subname() const { return subname_; }
	u32 crc() const { return crc_; }
	u32 fileSize() const { return fileSize_; }

protected:
	std::string		filename_;
	std::string		subname_;
	u32				crc_;
	int				refCount_;
	u32				fileSize_;
};

}	// namespace kuto
