/**
 * @file
 * @brief OpenGL View
 * @author project.kuto
 */

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

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
