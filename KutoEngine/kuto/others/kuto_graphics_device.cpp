/**
 * @file
 * @brief Graphics Device
 * @author project.kuto
 */

#include <kuto/kuto_timer.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_key_pad.h>

#if RPG2K_IS_PSP
	#include <pspthreadman.h>
#endif


namespace kuto {

namespace
{
	TouchInfo gTouchInfo;
	KeyInfo gKeyInfo;
	int gClickCount = 0;
	int gDoubleClickCount = 0;
	int gWindowHandle = 0;

	const int CLICK_INTERVAL = 10;

	const int INTERVAL_MILLI_SECOND = 1000 / 60;
	const int INTERVAL_MICRO_SECOND = 1000000 / 60;

	namespace callback
	{
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

		VirtualPad::KEYS toVirtualKeySpecial(int key)
		{
			switch(key) {
				case GLUT_KEY_LEFT: return VirtualPad::KEY_LEFT;
				case GLUT_KEY_RIGHT: return VirtualPad::KEY_RIGHT;
				case GLUT_KEY_UP: return VirtualPad::KEY_UP;
				case GLUT_KEY_DOWN: return VirtualPad::KEY_DOWN;
				default: return VirtualPad::KEY_MAX;
			}
		}
		VirtualPad::KEYS toVirtualKey(int key)
		{
			switch(key) {
				#if RPG2K_IS_PSP
					case 'd': return VirtualPad::KEY_Y; // PSP_CTRL_TRIANGLE
					case 'o': return VirtualPad::KEY_A; // PSP_CTRL_CIRCLE
					case 'x': return VirtualPad::KEY_B; // PSP_CTRL_CROSS
					case 'q': return VirtualPad::KEY_X; // PSP_CTRL_SQUARE
					case 'a': return VirtualPad::KEY_START; // PSP_CTRL_START
				#else
					case 'y': return VirtualPad::KEY_Y;
					case 'x': return VirtualPad::KEY_X;
					case 'a': return VirtualPad::KEY_A;
					case 'b': return VirtualPad::KEY_B;
					case 0x1b: // ESC -> START button
						return VirtualPad::KEY_START;
				#endif
				default: return VirtualPad::KEY_MAX;
			}
		}

		void specialPress(int key, int x, int y)
		{
			gKeyInfo.reset();
			gKeyInfo.pressFlag_ = true; gKeyInfo.onFlag_ = true;
			gKeyInfo.key = toVirtualKeySpecial(key);
		}
		void specialRelease(int key, int x, int y)
		{
			gKeyInfo.reset();
			gKeyInfo.releaseFlag_ = true; gKeyInfo.onFlag_ = false;
			gKeyInfo.key = toVirtualKeySpecial(key);
		}
		void keyboardPress(unsigned char key, int x, int y)
		{
			gKeyInfo.reset();
			gKeyInfo.pressFlag_ = true; gKeyInfo.onFlag_ = true;
			gKeyInfo.key = toVirtualKey(key);
		}
		void keyboardRelease(unsigned char key, int x, int y)
		{
			gKeyInfo.reset();
			gKeyInfo.releaseFlag_ = true; gKeyInfo.onFlag_ = false;
			gKeyInfo.key = toVirtualKey(key);
		}

		void timer(int value)
		{
			glutPostRedisplay();
			#if !RPG2K_IS_PSP
				glutTimerFunc(INTERVAL_MILLI_SECOND, timer, 0);
			#endif
			TouchPad::instance()->setTouches(&gTouchInfo, 1);
			KeyPad::instance()->setKeys(&gKeyInfo, 1);
			gTouchInfo.pressFlag_ = false;
			gTouchInfo.releaseFlag_ = false;
			gTouchInfo.moveFlag_ = false;
			gTouchInfo.clickFlag_ = false;
			gTouchInfo.doubleClickFlag_ = false;
			gTouchInfo.prevPosition_ = gTouchInfo.position_;
			gKeyInfo.reset();
			gClickCount--;
			gDoubleClickCount--;
		}
		void display()
		{
		#if RPG2K_IS_PSP
			while(true) {
				timer(0);
				GraphicsDevice::instance()->callbackGultDisplay();
				sceKernelDelayThread(INTERVAL_MICRO_SECOND);
			}
		#else
			GraphicsDevice::instance()->callbackGultDisplay();
		#endif
		}
	}; // namespace callback
}; // namespace

bool GraphicsDevice::initialize(int argc, char *argv[], int w, int h, const char *title, UpdateFunc func)
{
	initialized_ = true;

	// GLの初期化
	::glutInit( &argc, argv );
	// 描画モード
	::glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA ); // wバッファ+RGBA
	// ウィンドウの作成
	::glutInitWindowPosition( 0, 0 );			// 表示位置, TODO: set window position_ to center of screen
	::glutInitWindowSize( w, h );				// サイズ
	width_ = w;
	height_ = h;
	gWindowHandle = ::glutCreateWindow( title );

	// コールバック関数の設定
	updateFunc_ = func;
	::glutDisplayFunc(callback::display);		// 描画関数
	::glutReshapeFunc(callback::resize);		// 画面が変形したとき
	::glutIdleFunc(callback::idle);				// ひまなとき
	::glutSpecialFunc(callback::specialPress);		// keyboard
	::glutSpecialUpFunc(callback::specialRelease);	//
	::glutKeyboardFunc(callback::keyboardPress);	//
	::glutKeyboardUpFunc(callback::keyboardRelease);//
	::glutMouseFunc(callback::mouse);			// mouse
	::glutMotionFunc(callback::mouseMotion);	//
	::glutPassiveMotionFunc(callback::mouseMotion);
	::glClearColor( 0.0, 0.0, 0.0, 0.0 );		// 背景色の設定
	#if !RPG2K_IS_PSP
		glutTimerFunc(INTERVAL_MILLI_SECOND, callback::timer, 0);
	#endif

	glClearColor( 0.0, 0.0, 0.0, 0.0 );	// 背景色の設定

	syncState();

	return true;
}

void GraphicsDevice::callbackGultDisplay()
{
	updateFunc_(1.f);
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
	::glBlendFunc(blendSrcFactor_, blendDestFactor_);
	enableTexture2D_ = false;
	bindTexture2D_ = 0;
	setGLEnable(GL_TEXTURE_2D, enableTexture2D_);
	::glBindTexture(GL_TEXTURE_2D, bindTexture2D_);
	vertexPointerInfo_.set(0, 0, 0, 0);
	texcoordPointerInfo_.set(0, 0, 0, 0);
	colorPointerInfo_.set(0, 0, 0, 0);
}

void GraphicsDevice::endRender()
{
	::glutSwapBuffers();
}

};	// namespace kuto
