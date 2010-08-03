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
	Matrix matrix;
	matrix.ortho(0.0f, 320.0f, 0.0f, 480.0f, -1.0f, 1.0f);
	GraphicsDevice::instance()->setProjectionMatrix(matrix);
	Viewport vp(0, 0, 320, 480);
	GraphicsDevice::instance()->setViewport(vp);
	// set viewport
	Layer::preRender();    	// call base class preRender
}


}

