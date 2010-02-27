#include "kuto_gl.h"

class QGLContext;

class KutoEaglView // : UIView
{
private:
	GLint			backingWidth;		///< back buffer width
	GLint			backingHeight;		///< back buffer height
	QGLContext* context;
	// EAGLContext*	context;
	GLuint			viewRenderbuffer;
	GLuint			viewFramebuffer;
	GLuint			depthRenderbuffer;
public:
	void  preUpdate();
	void postUpdate();
};
