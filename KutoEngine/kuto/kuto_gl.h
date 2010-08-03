#pragma once

// #include "kuto_define.h"

extern "C"
{
	#if RPG2K_IS_IPHONE || RPG2K_IS_MAC_OS_X
		#ifdef __OBJC__
			#import <UIKit/UIKit.h>
			#import <OpenGLES/EAGL.h>
		#endif
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
	#elif RPG2K_IS_PSP
		#include <GLES/gl.h>
		// #include <GLES/glu.h>
		#include <GLES/glut.h>
		// #include <GLES/glext.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glut.h>
		#include <GL/glext.h>
	#endif
}
