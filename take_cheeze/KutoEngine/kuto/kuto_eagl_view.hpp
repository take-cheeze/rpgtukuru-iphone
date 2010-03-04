#include "kuto_gl.h"

class QGLContext;

namespace kuto
{

class GLView // : UIView
{
private:
	GLint			backingWidth;		///< back buffer width
	GLint			backingHeight;		///< back buffer height
	// QGLContext* context;
	GLuint			viewRenderbuffer;
	GLuint			viewFramebuffer;
	GLuint			depthRenderbuffer;
public:
	GLView();
	virtual ~GLView();

	void  preUpdate(const QGLContext* context);
	void postUpdate(const QGLContext* context);
};

};
