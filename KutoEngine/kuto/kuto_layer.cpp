/**
 * @file
 * @brief Render Layer Class
 * @author project.kuto
 */

#include "kuto_font.h"
#include "kuto_graphics_device.h"
#include "kuto_irender.h"
#include "kuto_layer.h"

#include <algorithm>
#include <utility>

#include <typeinfo>


namespace kuto {

void Layer::renderBegin() const
{
}
void Layer::renderEnd() const
{
}

void Layer::render() const
{
	renderBegin();

	for(std::multimap<float, IRender*>::const_iterator it = renderObjects_.begin(); it != renderObjects_.end(); ++it) {
		if( it->second->clearRenderFlag() ) {
			it->second->render();
		}
	}

	renderEnd();
}
void Layer::addRender(float const priority, IRender* render)
{
	renderObjects_.insert( std::make_pair(priority, render) );
}
namespace { struct RenderMatch {
	IRender* match;
	RenderMatch(IRender* val) : match(val) {}
	bool operator ()(std::pair<float, IRender*> const& val) { return (val.second == match); }
}; /* struct RenderMatch */ } /* anonymous namespace */ 
void Layer::removeRender(IRender* render)
{
	while(true) {
		std::multimap<float, IRender*>::iterator it =
			std::find_if( renderObjects_.begin(), renderObjects_.end(), RenderMatch(render) );
		if( it == renderObjects_.end() ) break;
		renderObjects_.erase(it);
	}
}

void Layer2D::renderBegin() const
{
	GraphicsDevice* dev = GraphicsDevice::instance();

	Matrix matrix;
	matrix.ortho(0.0f, float(dev->getWidth()), 0.0f, float(dev->getHeight()), -1.0f, 1.0f);
	dev->setProjectionMatrix(matrix);
	Viewport vp(0, 0, dev->getWidth(), dev->getHeight());
	dev->setViewport(vp);
}

}
