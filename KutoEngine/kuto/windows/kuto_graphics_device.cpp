/**
 * @file
 * @brief Graphics Device
 * @author project.kuto
 */

#include <kuto/kuto_timer.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_touch_pad.h>


namespace kuto {

GraphicsDevice* GraphicsDevice::instance_ = NULL;

namespace
{

TouchInfo gTouchInfo;
int gClickCount = 0;
int gDoubleClickCount = 0;
static const int CLICK_INTERVAL = 10;

static const int INTERVAL_MILLI_SECOND = 1000 / 60;

namespace callback
{
	void display()
	{
		GraphicsDevice::instance()->callbackGultDisplay();
	}
	void resize(int width, int height)
	{
	}
	void idle()
	{
	}
	void mouse(int button, int state, int x, int y)
	{
		if (button ==  GLUT_LEFT_BUTTON) {
			if (state == GLUT_DOWN) {
				gTouchInfo.pressFlag_ = true;
				gClickCount = CLICK_INTERVAL;
				if (gDoubleClickCount > 0)
					gDoubleClickCount = CLICK_INTERVAL;
				gTouchInfo.onFlag_ = true;
			} else if (state == GLUT_UP) {
				gTouchInfo.releaseFlag_ = true;
				if (gDoubleClickCount > 0)
					gTouchInfo.doubleClickFlag_ = true;
				if (gClickCount > 0) {
					gTouchInfo.clickFlag_ = true;
					gDoubleClickCount = CLICK_INTERVAL;
				}
				gTouchInfo.onFlag_ = false;
			}
		}
		gTouchInfo.position_.x = (float)x;
		gTouchInfo.position_.y = (float)y;
	}
	void mouseMotion(int x, int y)
	{
		gTouchInfo.position_.x = (float)x;
		gTouchInfo.position_.y = (float)y;
		gTouchInfo.moveFlag_ = true;
	}
	void keyboard(int key, int x, int y)
	{
	}

	void timer(int value)
	{
		glutPostRedisplay();
		glutTimerFunc(INTERVAL_MILLI_SECOND, timer, 0);
		TouchPad::instance()->setTouches(&gTouchInfo, 1);
		gTouchInfo.pressFlag_ = false;
		gTouchInfo.releaseFlag_ = false;
		gTouchInfo.moveFlag_ = false;
		gTouchInfo.clickFlag_ = false;
		gTouchInfo.doubleClickFlag_ = false;
		gTouchInfo.prevPosition_ = gTouchInfo.position_;
		gClickCount--;
		gDoubleClickCount--;
	}

}; // namespace callback

}; // namespace

GraphicsDevice::GraphicsDevice()
: viewRenderbuffer_(0), viewFramebuffer_(0), depthRenderbuffer_(0)
, width_(0), height_(0)
{
}

GraphicsDevice::~GraphicsDevice()
{
}

bool GraphicsDevice::initialize(int argc, char *argv[], int w, int h, const char *title, UpdateFunc func)
{
	// GLの初期化
	glutInit( &argc, argv );
	// 描画モード
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA ); // wバッファ+RGBA
	// ウィンドウの作成
	glutInitWindowPosition( 0, 0 );		// 表示位置
	glutInitWindowSize( w, h );			// サイズ
	glutCreateWindow( title );

// コールバック関数の設定
	updateFunc_ = func;
	glutDisplayFunc(callback::display);	// 描画関数
	glutReshapeFunc(callback::resize);	// 画面が変形したとき
	glutIdleFunc(callback::idle);		    // ひまなとき
	// keyboard
	glutSpecialFunc(callback::keyboard);
	// mouse
	glutMouseFunc(callback::mouse);
	glutMotionFunc(callback::mouseMotion);
	glutPassiveMotionFunc(callback::mouseMotion);

	glutTimerFunc(INTERVAL_MILLI_SECOND, callback::timer, 0);


	glClearColor( 0.0, 0.0, 0.0, 0.0 );	// 背景色の設定
	return true;
}

void GraphicsDevice::callbackGultDisplay()
{
	updateFunc_(1.f);
}

void GraphicsDevice::setProjectionMatrix(const Matrix& matrix)
{
	if (matrixMode_ != GL_PROJECTION) {
		matrixMode_ = GL_PROJECTION;
		glMatrixMode(matrixMode_);
	}
	glLoadMatrixf(matrix.pointer());
}

void GraphicsDevice::setModelMatrix(const Matrix& matrix)
{
	if (matrixMode_ != GL_MODELVIEW) {
		matrixMode_ = GL_MODELVIEW;
		glMatrixMode(matrixMode_);
	}
	glLoadMatrixf(matrix.pointer());
}

void GraphicsDevice::setViewport(const Viewport& viewport)
{
    glViewport((GLint)viewport.x, (GLint)viewport.y, (GLsizei)viewport.width, (GLsizei)viewport.height);
}

void GraphicsDevice::beginRender()
{
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	matrixMode_ = GL_MODELVIEW;
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	enableVertex_ = true; setGLClientState(GL_VERTEX_ARRAY, enableVertex_);
	enableNormal_ = false; setGLClientState(GL_NORMAL_ARRAY, enableNormal_);
	enableTexcoord_ = true; setGLClientState(GL_TEXTURE_COORD_ARRAY, enableTexcoord_);
	enableColor_ = false; setGLClientState(GL_COLOR_ARRAY, enableColor_);
	enableBlend_ = false; setGLEnable(GL_BLEND, enableBlend_);
	blendSrcFactor_ = GL_SRC_ALPHA;
	blendDestFactor_ = GL_ONE_MINUS_SRC_ALPHA;
	glBlendFunc(blendSrcFactor_, blendDestFactor_);
	enableTexture2D_ = false;
	bindTexture2D_ = 0;
	setGLEnable(GL_TEXTURE_2D, enableTexture2D_);
	glBindTexture(GL_TEXTURE_2D, bindTexture2D_);
	vertexPointerInfo_.set(0, 0, 0, 0);
	texcoordPointerInfo_.set(0, 0, 0, 0);
	colorPointerInfo_.set(0, 0, 0, 0);
}

void GraphicsDevice::endRender()
{
	glutSwapBuffers();
}

void GraphicsDevice::setVertexState(bool enableVertex, bool enableNormal, bool enableTexcoord, bool enableColor)
{
	if (enableVertex_ != enableVertex) {
		enableVertex_ = enableVertex;
		setGLClientState(GL_VERTEX_ARRAY, enableVertex_);
	}
	if (enableNormal_ != enableNormal) {
		enableNormal_ = enableNormal;
		setGLClientState(GL_NORMAL_ARRAY, enableNormal_);
	}
	if (enableTexcoord_ != enableTexcoord) {
		enableTexcoord_ = enableTexcoord;
		setGLClientState(GL_TEXTURE_COORD_ARRAY, enableTexcoord_);
	}
	if (enableColor_ != enableColor) {
		enableColor_ = enableColor;
		setGLClientState(GL_COLOR_ARRAY, enableColor_);
	}
}

void GraphicsDevice::setBlendState(bool enableBlend, GLenum srcFactor, GLenum destFactor)
{
	if (enableBlend_ != enableBlend) {
		enableBlend_ = enableBlend;
		setGLEnable(GL_BLEND, enableBlend_);
	}
	if (blendSrcFactor_ != srcFactor || blendDestFactor_ != destFactor) {
		blendSrcFactor_ = srcFactor;
		blendDestFactor_ = destFactor;
		glBlendFunc(blendSrcFactor_, blendDestFactor_);
	}
}

void GraphicsDevice::setTexture2D(bool enable, GLuint texture)
{
	if (enableTexture2D_ != enable) {
		enableTexture2D_ = enable;
		setGLEnable(GL_TEXTURE_2D, enableTexture2D_);
	}
	if (bindTexture2D_ != texture) {
		bindTexture2D_ = texture;
		glBindTexture(GL_TEXTURE_2D, bindTexture2D_);
	}
}

void GraphicsDevice::setColor(const Color& color)
{
	if (color_ != color) {
		color_ = color;
		glColor4f(color_.r, color_.g, color_.b, color_.a);
	}
}

void GraphicsDevice::setVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	if (!vertexPointerInfo_.equals(size, type, stride, pointer)) {
		vertexPointerInfo_.set(size, type, stride, pointer);
		glVertexPointer(size, type, stride, pointer);
	}
}

void GraphicsDevice::setTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	if (!texcoordPointerInfo_.equals(size, type, stride, pointer)) {
		texcoordPointerInfo_.set(size, type, stride, pointer);
		glTexCoordPointer(size, type, stride, pointer);
	}
}

void GraphicsDevice::setColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	if (!colorPointerInfo_.equals(size, type, stride, pointer)) {
		colorPointerInfo_.set(size, type, stride, pointer);
		glColorPointer(size, type, stride, pointer);
	}
}


};	// namespace kuto
