/**
 * @file
 * @brief Render Layer Class
 * @author project.kuto
 */

#include "kuto_layer.h"
#include "kuto_graphics_device.h"
#include "kuto_font.h"


namespace kuto {

void Layer2D::preRender()
{
	GraphicsDevice* dev = GraphicsDevice::instance();

	Matrix matrix;
	matrix.ortho(0.0f, (float)dev->getWidth(), 0.0f, (float)dev->getHeight(), -1.0f, 1.0f);
	GraphicsDevice::instance()->setProjectionMatrix(matrix);
	Viewport vp(0, 0, dev->getWidth(), dev->getHeight());
	dev->setViewport(vp);
	// set viewport
	Layer::preRender();    	// call base class preRender
}


}

