/**
 * @file
 * @brief Render Interface
 * @author takuto
 */
#pragma once

#include "kuto_types.h"


namespace kuto {

class IRender
{
public:
	virtual ~IRender() {}
	virtual void render() = 0;
};	// class IRender

}	// namespace kuto

