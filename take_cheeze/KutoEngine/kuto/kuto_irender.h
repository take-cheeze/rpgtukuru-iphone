/**
 * @file
 * @brief Render Interface
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"



namespace kuto {

// class Graphics;
class Graphics2D;

class IRender
{
public:
	virtual ~IRender() {}
	virtual void render(Graphics2D& g) = 0;
	// virtual void render(Graphics& g) = 0;
};	// class IRender

}	// namespace kuto

