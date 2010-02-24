#include "AppMain.h"
#include "AppDelegate.hpp"

#include <rpg2kLib/Defines.hpp>
#include <kuto/kuto_section_manager.h>
#include <kuto/kuto_timer.h>

#include <QEvent>
#include <QTimer>
// #include <QTimerEvent>
#include <QWindowStateChangeEvent>

#include <iomanip>

using namespace rpg2kLib;

static AppMain* sAppMain = NULL;

static double
	ACTIVE_INTERVAL = 1000.0 / 60.0,
	NON_ACTIVE_INTERVAL = 1000.0 / 5.0;

MainWindow::MainWindow(QWidget* parent)
	: QGLWidget(parent), SECTION_NAME(NULL)
{
	TIMER = new QTimer(this);
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

	delete TIMER;

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
	connect( TIMER, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
	TIMER->setInterval(ACTIVE_INTERVAL);
	TIMER->start();
}
void MainWindow::paintGL()
{
	// cout << kuto::Timer::getTime() << endl;

	sAppMain->update();
	swapBuffers();

	if (!kuto::SectionManager::instance()->getCurrentTask()) {
		if (SECTION_NAME) {
			kuto::SectionManager::instance()->beginSection(SECTION_NAME);
			SECTION_NAME = NULL;
		}
	}
}
void MainWindow::resizeGL(int, int)
{
}

void MainWindow::startAnimation()
{
	TIMER->start();
}
void MainWindow::stopAnimation()
{
	if( TIMER->isActive() ) TIMER->stop();
}

bool MainWindow::event(QEvent* e)
{
	// cout << "changeEvent() called. type: " << e->type() << endl;

	switch( e->type() ) {
		case QEvent::WindowActivate:
			TIMER->setInterval(ACTIVE_INTERVAL);
			return true;
		case QEvent::WindowDeactivate:
			TIMER->setInterval(NON_ACTIVE_INTERVAL);
			return true;
		default:
			return QGLWidget::event(e);
	}
}

/*
- (void)setAnimationTimer:(NSTimer *)newTimer {
	if (animationTimer)
		[animationTimer invalidate];
    animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationInterval = interval;
    if (animationTimer) {
        [self stopAnimation];
    }
	[self startAnimation];
}

 */
