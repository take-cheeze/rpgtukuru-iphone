/**
 * @file
 * @brief main
 * @author project.kuto
 */

#include <QApplication>
#include <QWidget>
#include <AppDelegate.hpp>
#include <cstdlib>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow* mainWin = new MainWindow();

	mainWin->show();
	return app.exec();
}
