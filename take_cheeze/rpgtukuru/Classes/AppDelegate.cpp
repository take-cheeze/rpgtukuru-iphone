#include "AppMain.h"
#include "AppDelegate.hpp"

#include <rpg2kLib/Defines.hpp>
#include <kuto/kuto_section_manager.h>
#include <kuto/kuto_timer.h>

#include <QEvent>
#include <QtOpenGL/QGLContext>
#include <QKeyEvent>
#include <QTimer>

#include <iomanip>

using namespace rpg2kLib;


static AppMain* sAppMain = NULL;

static double
	ACTIVE_INTERVAL = 1000.0 / 60.0,
	NON_ACTIVE_INTERVAL = 1000.0 / 5.0;

MainWindow::MainWindow(QWidget* parent)
: QGLWidget(parent)
// , SECTION_NAME(NULL)
{
	timer_ = new QTimer(this);
	resize(SCREEN_W, SCREEN_H);

// create app main
	if(sAppMain == NULL) {
		sAppMain = new AppMain();
		sAppMain->initialize();
	}
}
MainWindow::~MainWindow()
{
	stopAnimation();

	delete timer_;

	if (sAppMain != NULL) {
		delete sAppMain;
		sAppMain = NULL;
	}
}

void MainWindow::initializeGL()
{
/*
	CGRect rect = [[UIScreen mainScreen] bounds];

	// create a full-screen window
	window = [[UIWindow alloc] initWithFrame:rect];
	UINavigationController* naviController = [[UINavigationController alloc] init];

	// create the OpenGL view and add it to the window
	glView = [[KutoEaglView alloc] initWithFrame:rect];
	// debugView = [[KutoDebugMenuView alloc] initWithFrame:rect];
	[naviController pushViewController:debugView animated:NO];

	[window addSubview:naviController.view];	// set debug menu
	[window addSubview:glView];

	// show the window
	[window makeKeyAndVisible];

	//[application setStatusBarHidden:YES  animated:NO];	// info.plist

	self.animationInterval = 1.0 / 60.0;
	//[self startAnimation];
 */
	connect( timer_, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
	timer_->setInterval(ACTIVE_INTERVAL);
	timer_->start();
}
void MainWindow::paintGL()
{
	// std::cout << kuto::Timer::getTime() << std::endl;

	sAppMain->update();
	this->swapBuffers();

/*
	if (!kuto::SectionManager::instance()->getCurrentTask()) {
		if (SECTION_NAME) {
			kuto::SectionManager::instance()->beginSection(SECTION_NAME);
			SECTION_NAME = NULL;
		}
	}
 */
}
void MainWindow::resizeGL(int width, int height)
{
}

void MainWindow::startAnimation()
{
	timer_->start();
}
void MainWindow::stopAnimation()
{
	if( timer_->isActive() ) timer_->stop();
}

bool MainWindow::event(QEvent* e)
{
	// cout << "changeEvent() called. type: " << e->type() << endl;

	switch( e->type() ) {
		case QEvent::WindowActivate:
			timer_->setInterval(ACTIVE_INTERVAL);
			return true;
		case QEvent::WindowDeactivate:
			timer_->setInterval(NON_ACTIVE_INTERVAL);
			return true;
		default:
			return QGLWidget::event(e);
	}
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	// kuto::TouchInfo info;
	// info.onFlag_ = info.pressFlag_ = true;

	switch( e->key() ) {
		case Qt::Key_Up:
		case Qt::Key_K: case Qt::Key_Launch8:
			break;
		case Qt::Key_Right:
		case Qt::Key_L: case Qt::Key_Launch6:
			break;
		case Qt::Key_Left:
		case Qt::Key_H: case Qt::Key_Launch4:
			break;
		case Qt::Key_Down:
		case Qt::Key_J: case Qt::Key_Launch2:
			break;
		case Qt::Key_Enter:
		case Qt::Key_Space:
		case Qt::Key_Z:
			break;
		case Qt::Key_Escape:
		case Qt::Key_X: case Qt::Key_C:
		case Qt::Key_V: case Qt::Key_B:
		case Qt::Key_N:
		case Qt::Key_Launch0:
			break;
		case Qt::Key_Shift:
			break;
		default: break;
	}
}
void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
/*
	kuto::TouchInfo info;
	info.releaseFlag_ = true;
 */
}

/*
- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationInterval = interval;
    if (animationTimer) {
        [self stopAnimation];
    }
	[self startAnimation];
}

 */
