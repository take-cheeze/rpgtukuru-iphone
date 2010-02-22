#include "kuto_gl.h"

class KutoEaglView // : UIView
{
private:
	GLint			backingWidth;		///< back buffer width
	GLint			backingHeight;		///< back buffer height
	EAGLContext*	context;
	GLuint			viewRenderbuffer;
	GLuint			viewFramebuffer;
	GLuint			depthRenderbuffer;
};

- (void)preUpdate;
- (void)postUpdate;

@end
