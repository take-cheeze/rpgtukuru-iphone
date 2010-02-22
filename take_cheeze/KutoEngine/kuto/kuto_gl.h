#include "kuto_define.h"

#if defined(RPG2K_IS_IPHONE)
	#import <UIKit/UIKit.h>
	#import <OpenGLES/EAGL.h>
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
