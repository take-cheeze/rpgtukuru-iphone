#pragma once

#include <QtOpenGL/QGLWidget>

class QEvent;
class QGLContext;
class QKeyEvent;
class QTimer;

class MainWindow : public QGLWidget
{
private:
	QTimer* timer_;
	QGLContext* context_;
	// const char* sectionName_;
public:
	MainWindow(QWidget* parent = NULL);
	virtual ~MainWindow();

	void startAnimation();
	void  stopAnimation();
protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual bool event(QEvent* e);

	virtual void keyPressEvent(QKeyEvent* e);
	virtual void keyReleaseEvent(QKeyEvent* e);
};
