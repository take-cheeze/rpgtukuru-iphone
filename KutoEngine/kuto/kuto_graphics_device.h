/**
 * @file
 * @brief Graphics Device
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_error.h"
#include "kuto_gl.h"
#include "kuto_singleton.h"


namespace kuto {

class GraphicsDevice : public Singleton<GraphicsDevice>
{
	friend class Singleton<GraphicsDevice>;
public:
protected:
	GraphicsDevice();
	~GraphicsDevice();

public:
#if RPG2K_IS_IPHONE
	bool initialize(GLuint viewRenderbuffer, GLuint viewFramebuffer, GLuint depthRenderbuffer, int width, int height);
#else // using GLUT
	typedef void (*UpdateFunc)(float);
	bool initialize(int& argc, char *argv[], int w, int h, const char *title, UpdateFunc func);
	void callbackGultDisplay();

	UpdateFunc		updateFunc_;
#endif

	void setProjectionMatrix(const Matrix& matrix);
	void setModelMatrix(const Matrix& matrix);
	void setViewport(const Viewport& viewport);
	int width() const { return width_; }
	int height() const { return height_; }
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
		glDrawArrays(mode, first, count);
	}

	void setTitle(std::string const& title);

	void syncState();

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
	GLuint 							viewRenderbuffer_;
	GLuint							viewFramebuffer_;
	GLuint							depthRenderbuffer_;
	int								width_;
	int								height_;
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
