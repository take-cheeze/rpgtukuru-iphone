#pragma once

#include <Qt/qgl.h>

class QTimer;

class MainWindow : public QGLWidget
{
private:
	QTimer* TIMER;
	const char* SECTION_NAME;
public:
	MainWindow(QWidget* parent = NULL);
	virtual ~MainWindow();

	void startAnimation();
	void  stopAnimation();
protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);
};
