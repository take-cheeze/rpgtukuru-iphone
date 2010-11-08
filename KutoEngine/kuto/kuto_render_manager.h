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
#include "kuto_singleton.h"

#include <boost/smart_ptr.hpp>


namespace kuto {

class Graphics2D;

/// render manager class
class RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;
	friend class IRender;
private:
	RenderManager();
	~RenderManager();

protected:
	void addRender(IRender* render, Layer::Type layer, float priority);
	void removeRender(IRender* render);

public:
	void render();

	Layer* layer(u32 index) { return layers_[index]; }
	const Layer* layer(u32 index) const { return layers_[index]; }
	Layer::Type currentLayer() const { return currentLayer_; }
	Graphics2D* graphics2D() { return graphics2D_.get(); }

private:
	Array<Layer*, Layer::TYPE_END>		layers_;			///< レイヤー
	boost::scoped_ptr<Graphics2D>		graphics2D_;		///< Graphics2D
	Layer::Type							currentLayer_;		///< 現在のレイヤーIndex;
};	// class RenderManager

}	// namespace kuto
