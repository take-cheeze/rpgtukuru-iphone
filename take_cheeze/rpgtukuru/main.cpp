/**
 * @file
 * @brief main
 * @author project.kuto
 */

#include <qapplication.h>
#include <qwidget.h>
#include <AppDelegate.hpp>
#include <cstdlib>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow* mainWin = new MainWindow();

	mainWin->show();
	return app.exec();
}
