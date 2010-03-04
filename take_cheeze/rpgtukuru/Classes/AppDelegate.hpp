#pragma once

#include <QtOpenGL/QGLWidget>
#include <kuto/kuto_eagl_view.hpp>
#include <kuto/kuto_debug_menu_view.hpp>

class QEvent;
class QGLContext;
class QKeyEvent;
class QTimer;

class MainWindow : public QGLWidget
{
private:
	QTimer* timer_;
	kuto::GLView glView_;
	kuto::DebugMenuView debugView_;
	// const char* sectionName_;
public:
	MainWindow(QWidget* parent = NULL);
	virtual ~MainWindow();

	void startAnimation();
	void  stopAnimation();
	void setAnimationInterval(double interval);

	bool isMultipleTouchEnabled();
protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual bool event(QEvent* e);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);

	virtual void keyPressEvent(QKeyEvent* e);
	virtual void keyReleaseEvent(QKeyEvent* e);
};
