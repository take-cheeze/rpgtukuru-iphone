#pragma once

#include <kuto/kuto_eagl_view.hpp>
#include <kuto/kuto_debug_menu_view.hpp>

#include <QtGui/QMainWindow>


class QEvent;
class QGLContext;
class QKeyEvent;
class QTimer;

class MainWindow : public QMainWindow
{
private:
	QTimer* timer_;
	kuto::GLView* glView_;
	kuto::DebugMenuView* debugView_;
public:
	MainWindow(QWidget* parent = NULL);
	virtual ~MainWindow();

	void startAnimation();
	void  stopAnimation();
	void setAnimationInterval(double interval);

	bool isMultipleTouchEnabled();
protected:
	virtual void update();

	virtual bool event(QEvent* e);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);

	virtual void keyPressEvent(QKeyEvent* e);
	virtual void keyReleaseEvent(QKeyEvent* e);
};
