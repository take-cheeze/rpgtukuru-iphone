#include "AppMain.h"
#include "AppDelegate.hpp"

#include <rpg2kLib/Defines.hpp>
#include <kuto/kuto_section_manager.h>
#include <kuto/kuto_timer.h>
#include <kuto/kuto_touch_pad.h>

#include <QtGui/QKeyEvent>
#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>


static AppMain* sAppMain = NULL;

static const int SCREEN_H = 480, SCREEN_W = 320;
static const double ACTIVE_INTERVAL = 1000.0 / 60.0, NON_ACTIVE_INTERVAL = 1000.0 / 5.0;

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
	glView_ = new kuto::GLView(this);
	debugView_ = new kuto::DebugMenuView(this);

	timer_ = new QTimer(this);
	this->resize(SCREEN_W, SCREEN_H);

// create app main
	if(sAppMain == NULL) {
		sAppMain = new AppMain();
		sAppMain->initialize();
	}

	connect( timer_, SIGNAL( timeout() ), this, SLOT( update() ) );
	timer_->setInterval(ACTIVE_INTERVAL);
	timer_->start();
}
MainWindow::~MainWindow()
{
	stopAnimation();

	delete timer_;

	delete glView_;
	delete debugView_;

	if (sAppMain != NULL) {
		delete sAppMain;
		sAppMain = NULL;
	}
}

void MainWindow::update()
{
	if (!kuto::SectionManager::instance()->getCurrentTask()) {
		if ( debugView_->selectedSection() ) {
			kuto::SectionManager::instance()->beginSection( debugView_->selectedSection() );
			debugView_->selectedSection() = NULL;
		}
	} else {
		glView_-> preUpdate();
		sAppMain->update();
		glView_->postUpdate();
	}
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
			return QMainWindow::event(e);
	}
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
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
}

bool MainWindow::isMultipleTouchEnabled() { return true; }

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	kuto::TouchInfo info;

	info.position_.set( event->x(), event->y() );
	info.prevPosition_.set( event->x(), event->y() );

	info.onFlag_ = true;
	info.moveFlag_ = true;

	kuto::TouchPad::instance()->setTouches(&info, 1);
}
void MainWindow::mousePressEvent(QMouseEvent* event)
{
	kuto::TouchInfo info;

	info.position_.set( event->x(), event->y() );
	info.prevPosition_.set( event->x(), event->y() );

	info.onFlag_ = true;
	info.pressFlag_ = true;

	kuto::TouchPad::instance()->setTouches(&info, 1);
}
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	kuto::TouchInfo info;

	info.releaseFlag_ = true;
/*
	if (touch.tapCount >= 1) {
		info.clickFlag_ = true;
		if (touch.tapCount == 2)
			info.doubleClickFlag_ = true;
	}
 */

	info.onFlag_ = true;
	info.pressFlag_ = true;

	kuto::TouchPad::instance()->setTouches(&info, 1);
}

void MainWindow::setAnimationInterval(double interval)
{
	timer_->setInterval(interval*1000.0);
}
