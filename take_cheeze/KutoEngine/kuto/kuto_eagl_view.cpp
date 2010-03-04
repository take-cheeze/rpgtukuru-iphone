/**
 * @file
 * @brief OpenGL View
 * @author project.kuto
 */

// #import <QuartzCore/QuartzCore.h>
// #import <OpenGLES/EAGLDrawable.h>
#include <QtOpenGL/QGLContext>

#include "kuto_eagl_view.hpp"
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_font.h>
#include <kuto/kuto_touch_pad.h>


#define USE_DEPTH_BUFFER 0

namespace kuto
{

GLView::GLView()
{
}
GLView::~GLView()
{
}

void GLView::preUpdate(const QGLContext* context)
{
}

void GLView::postUpdate(const QGLContext* context)
{
	context->swapBuffers();
}

};
