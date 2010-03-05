#include "kuto_gl.h"

#include <QtOpenGL/QGLWidget>

class QGLFramebufferObject;

namespace kuto
{

class GLView : public QGLWidget
{
private:
	GLint			backingWidth_;		///< back buffer width
	GLint			backingHeight_;		///< back buffer height
	// QGLContext* context_;
	QGLFramebufferObject* framebuffer_;
	GLuint			viewFramebuffer_;
	QGLFramebufferObject* renderbuffer_;
	GLuint			viewRenderbuffer_;
	QGLFramebufferObject* depthbuffer_;
	GLuint			depthRenderbuffer_;
public:
	GLView(QWidget* parent);
	virtual ~GLView();

	void  preUpdate();
	void postUpdate();
protected:
	virtual void initializeGL();
/*
	virtual void paintGL();
	virtual void resizeGL(int width, int height);
 */
};

};
