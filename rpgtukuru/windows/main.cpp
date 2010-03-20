/**
 * @file
 * @brief main loop
 * @author project.kuto
 */

#include <kuto/kuto_graphics_device.h>
#include "AppMain.h"


static AppMain appMain;

static void update(float dt)
{
	appMain.update();
}


int main(int argc, char* argv[])
{
	kuto::GraphicsDevice::createInstance();
	kuto::GraphicsDevice::instance()->initialize(argc, argv, 320.f, 480.f, "RPG Tukuru", update);
	appMain.initialize();
	glutMainLoop();
	return 0;
}

