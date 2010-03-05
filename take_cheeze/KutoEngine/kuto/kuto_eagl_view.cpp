/**
 * @file
 * @brief OpenGL View
 * @author project.kuto
 */

#include <QtCore/QPointF>
#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLFramebufferObject>

#include <rpg2kLib/Defines.hpp>

#include "kuto_eagl_view.hpp"
#include "kuto_graphics_device.h"
#include "kuto_font.h"
#include "kuto_touch_pad.h"


#define USE_DEPTH_BUFFER 0

namespace kuto
{

GLView::GLView(QWidget* parent)
: QGLWidget(parent)
{
	QGLWidget::resize(rpg2kLib::SCREEN_W, rpg2kLib::SCREEN_H);
	QGLWidget::mapToParent( QPoint(0, 0) );
}
GLView::~GLView()
{
	if(! framebuffer_) delete  framebuffer_;
	if(!renderbuffer_) delete renderbuffer_;
	if(! depthbuffer_) delete  depthbuffer_;
}

void GLView::preUpdate()
{
	const_cast< QGLContext* >( context() )->makeCurrent();
}

void GLView::postUpdate()
{
	QGLContext* cont = const_cast< QGLContext* >( context() );
	cont->drawTexture( QPointF(0, 0), viewFramebuffer_ );
	cont->drawTexture( QPointF(0, 0), viewRenderbuffer_ );
	// cont->drawTexture( QPointF(0, 0), viewFramebuffer_ );
	cont->doneCurrent();
	cont->swapBuffers();
}

void GLView::initializeGL()
{
	backingWidth_  = rpg2kLib::SCREEN_W;
	backingHeight_ = rpg2kLib::SCREEN_H;

	framebuffer_ = new QGLFramebufferObject(rpg2kLib::SCREEN_W, rpg2kLib::SCREEN_H);
	viewFramebuffer_ = framebuffer_->texture();
	renderbuffer_ = new QGLFramebufferObject(rpg2kLib::SCREEN_W, rpg2kLib::SCREEN_H);
	viewRenderbuffer_ = renderbuffer_->texture();

	if(USE_DEPTH_BUFFER) {
		depthbuffer_ = new QGLFramebufferObject(rpg2kLib::SCREEN_W, rpg2kLib::SCREEN_H);
		depthRenderbuffer_ = depthbuffer_->texture();
	}

	kuto::GraphicsDevice::instance()->initialize(
		viewRenderbuffer_, viewFramebuffer_, depthRenderbuffer_,
		backingWidth_, backingHeight_
	);
}

};
