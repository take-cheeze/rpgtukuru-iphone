/**
 * @file
 * @brief Load Handle
 * @author project.kuto
 */
#pragma once

#include <string>
#include "kuto_types.h"


namespace kuto {

class LoadCore;


class LoadHandle
{
protected:
	LoadHandle();
	virtual ~LoadHandle();

public:
	bool load(const std::string& filename, const char* subname = NULL);
	void release();
	const std::string& filename() const;
	u32 crc() const;

private:
	virtual LoadCore* createCore(const std::string& filename, const char* subname) = 0;

protected:
	LoadCore*		core_;
};

}	// namespace kuto
