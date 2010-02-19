/**
 * @file
 * @brief Graphics Device
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_error.h"

#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) )
	#include <Carbon/TargetConditionals.h>
#endif
#if defined(TARGET_OS_IPHONE) && defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE==1)
	#include <OpenGLES/ES1/gl.h>
#else
	#include <GL/gl.h>
#endif


namespace kuto {

class GraphicsDevice
{
public:
	static GraphicsDevice* createInstance() { kuto_assert(!instance_); instance_ = new GraphicsDevice(); return instance_; }
	static void destroyInstance() { kuto_assert(instance_); delete instance_; instance_ = NULL; }
	static GraphicsDevice* instance() { kuto_assert(instance_); return instance_; }
	
private:
	GraphicsDevice();
	~GraphicsDevice();

public:
	bool initialize(GLuint viewRenderbuffer, GLuint viewFramebuffer, GLuint depthRenderbuffer, int width, int height);
	
	void setProjectionMatrix(const Matrix& matrix);
	void setModelMatrix(const Matrix& matrix);
	void setViewport(const Viewport& viewport);
	int getWidth() const { return width_; }
	int getHeight() const { return height_; }
	void beginRender();
	void endRender();
	
	void setGLClientState(GLenum state, bool enable) const {
		if (enable)
			glEnableClientState(state);
		else
			glDisableClientState(state);
	}
	void setGLEnable(GLenum state, bool enable) const {
		if (enable)
			glEnable(state);
		else
			glDisable(state);
	}
	void setVertexState(bool enableVertex, bool enableNormal, bool enableTexcoord, bool enableColor);
	void setBlendState(bool enableBlend, GLenum srcFactor, GLenum destFactor);
	void setTexture2D(bool enable, GLuint texture);
	void setColor(const Color& color);
	void setVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
	void setTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
	void setColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
	void drawArrays(GLenum mode, GLint first, GLsizei count) {
		glDrawArrays(mode, first,count);
	}
	
private:
	struct VertexPointerInfo
	{
		GLint			size;
		GLenum			type;
		GLsizei			stride;
		const GLvoid*	pointer;
		
		void set(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
			this->size = size;
			this->type = type;
			this->stride = stride;
			this->pointer = pointer;
		}
		bool equals(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) const {
			return this->size == size && this->type == type && this->stride == stride && this->pointer == pointer;
		}
	};	// struct VertexPointerInfo
private:
	int								width_;
	int								height_;
	static GraphicsDevice*			instance_;
		GLuint 							viewRenderbuffer_;
		GLuint							viewFramebuffer_;
		GLuint							depthRenderbuffer_;
	GLuint 							screenTexture_;

	struct {
		bool						enableVertex_	: 1;
		bool						enableNormal_	: 1;
		bool						enableTexcoord_	: 1;
		bool						enableColor_	: 1;
		bool						enableBlend_	: 1;
		bool						enableTexture2D_: 1;
	};
	GLenum							blendSrcFactor_;
	GLenum							blendDestFactor_;
	GLuint							bindTexture2D_;
	Color							color_;
	GLenum							matrixMode_;
	VertexPointerInfo				vertexPointerInfo_;
	VertexPointerInfo				texcoordPointerInfo_;
	VertexPointerInfo				colorPointerInfo_;
};
	
}	// namespace kuto

