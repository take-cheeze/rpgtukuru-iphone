/**
 * @file
 * @brief Render Layer
 * @author takuto
 */
#pragma once

#include "kuto_static_vector.h"
#include "kuto_render_object.h"
#include "kuto_math.h"
#include <algorithm>


namespace kuto {

/// Layer Types
enum LAYER_TYPE {
	LAYER_2D_OBJECT,
	LAYER_2D_DEBUG,
	LAYER_MAX
};

/// Layer Base Class
class Layer
{
public:
	enum {
		RENDER_OBJECT_MAX		= 512,
	};

public:
	virtual ~Layer() {}
	virtual void preRender() {
		std::sort(renderObjects_.begin(), renderObjects_.end(), std::greater<RenderObject>());
	}
	virtual void render() {
		for (u32 i = 0; i < renderObjects_.size(); i++)
			renderObjects_[i].render();
	}
	virtual void postRender() {
		renderObjects_.clear();
	}
	void addRenderObject(const RenderObject& value) { renderObjects_.push_back(value); }
	
protected:
	StaticVector<RenderObject, RENDER_OBJECT_MAX>	renderObjects_;
};	// class Layer

/// 2D Layer
class Layer2D : public Layer
{
public:
	virtual void preRender();
};	// class Layer2D

}	// namespace kuto
