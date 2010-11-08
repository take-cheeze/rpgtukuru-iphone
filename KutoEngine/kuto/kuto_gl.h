#pragma once

extern "C"
{
	#if RPG2K_IS_IPHONE
		#ifdef __OBJC__
			#import <UIKit/UIKit.h>
			#import <OpenGLES/EAGL.h>
		#endif
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
	#elif RPG2K_IS_MAC_OS_X
		#error TODO
	#elif RPG2K_IS_PSP
		#include <GLES/gl.h>
		#include <GLES/glut.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glut.h>
		#include <GL/glext.h>
	#endif
}
