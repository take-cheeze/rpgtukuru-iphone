/**
 * @file
 * @brief Render Manager
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_irender.h"
#include "kuto_static_vector.h"
#include "kuto_layer.h"


namespace kuto {

class RenderObject
{
public:
	RenderObject() : render_(NULL), priority_(0.0f) {}
	RenderObject(IRender* render, float priority) : render_(render), priority_(priority) {}

	bool operator<(const RenderObject& rhs) const { return priority_ < rhs.priority_; }
	bool operator>(const RenderObject& rhs) const { return priority_ > rhs.priority_; }
	void render() { render_->render(); }

private:
	IRender*		render_;
	float			priority_;
};	// class RenderObject

}	// namespace kuto
