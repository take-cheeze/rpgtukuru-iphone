/**
 * @file
 * @brief Render Manager
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_irender.h"
#include "kuto_layer.h"
#include "kuto_array.h"
#include "kuto_scoped_ptr.h"


namespace kuto {

class Graphics2D;

/// render manager class
class RenderManager
{
public:
	static RenderManager* createInstance() { kuto_assert(!instance_); instance_ = new RenderManager(); return instance_; }
	static void destroyInstance() { kuto_assert(instance_); delete instance_; instance_ = NULL; }
	static RenderManager* instance() { kuto_assert(instance_); return instance_; }
	
private:
	RenderManager();
	~RenderManager();

public:
	void addRender(IRender* render, LAYER_TYPE layer, float priority);
	void render();
	Layer* getLayer(u32 index) { return layers_[index]; }
	const Layer* getLayer(u32 index) const { return layers_[index]; }
	LAYER_TYPE currentLayer() const { return currentLayer_; }
	Graphics2D* getGraphics2D() { return graphics2D_.get(); }

private:
	static RenderManager*			instance_;			///< シングルトンポインタ
	Array<Layer*, LAYER_MAX>		layers_;			///< レイヤー
	ScopedPtr<Graphics2D>			graphics2D_;		///< Graphics2D
	LAYER_TYPE						currentLayer_;		///< 現在のレイヤーIndex;
};	// class RenderManager

}	// namespace kuto
