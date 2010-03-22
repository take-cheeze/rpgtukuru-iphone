/**
 * @file
 * @brief Graphics Device
 * @author project.kuto
 */

#include <kuto/kuto_timer.h>
#include <kuto/kuto_graphics_device.h>


namespace kuto {

GraphicsDevice* GraphicsDevice::instance_ = NULL;

namespace
{

u64 startTime;
u64 lastUpdateTime;
/*
float g_ftime = 0.0f;           // アプリケーションの起動時間
float g_fLastUpdateTime = 0.0f; // 最終更新時間
 */

static const double INTERVAL_MILLI_SECOND = 1000.0 / 60.0;

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
	}
	void mouseMotion(int x, int y)
	{
	}
	void keyboard(int key, int x, int y)
	{
	}
/*
	void timer(int value)
	{
		g_ftime += 0.02f;
		glutPostRedisplay();
		glutTimerFunc(INTERVAL_MILLI_SECOND, timer, 0);
	}
 */
}; // namespace callback

}; // namespace

GraphicsDevice::GraphicsDevice()
: viewRenderbuffer_(NULL), viewFramebuffer_(NULL), depthRenderbuffer_(NULL)
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
	// 時間管理
	startTime = Timer::getTime();
/*
	g_ftime = g_fLastUpdateTime = 0.0f;
	glutTimerFunc(INTERVAL_MILLI_SECOND, callback::timer, 0);
 */

	//g_MouseInfo.raw = g_MouseInfo.trg = g_MouseInfo.tmp = 0;
	//g_MouseInfo.pos[0] = 0;
	//g_MouseInfo.pos[1] = 0;

	// 一度だけすればいい設定
	glClearColor( 0.0, 0.0, 0.0, 0.0 );	// 背景色の設定
	return true;
}

void GraphicsDevice::callbackGultDisplay()
{
	updateFunc_(1.f);

	lastUpdateTime = Timer::getTime();
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
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
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
