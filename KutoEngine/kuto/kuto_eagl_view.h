/**
 * @file
 * @brief OpenGL View
 * @author project.kuto
 */

#include "kuto_gl.h"


@interface KutoEaglView : UIView {
@private
	GLint			backingWidth;		///< back buffer width
	GLint			backingHeight;		///< back buffer height
	EAGLContext*	context;
	GLuint			viewRenderbuffer;
	GLuint			viewFramebuffer;
	GLuint			depthRenderbuffer;
}

- (void)preUpdate;
- (void)postUpdate;

@end
