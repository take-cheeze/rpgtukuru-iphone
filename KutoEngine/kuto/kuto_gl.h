#include "kuto_define.h"

#if defined(RPG2K_IS_IPHONE)
	#ifdef __OBJC__
		#import <UIKit/UIKit.h>
		#import <OpenGLES/EAGL.h>
	#endif
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#elif defined(RPG2K_IS_WINDOWS)
	#include <glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif