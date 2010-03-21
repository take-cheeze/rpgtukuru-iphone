/**
 * @file
 * @brief main loop
 * @author project.kuto
 */

#include <cstdlib>
#include <kuto/kuto_graphics_device.h>
#include "AppMain.h"


#if defined(RPG2K_IS_PSP)

#include <pspkernel.h> 

PSP_MODULE_INFO("RPG_RT_EMU_2000", 0, 0, 0); 

#endif

namespace
{
	AppMain appMain;

	static void update(float dt)
	{
		appMain.update();
	}
}; // namespace

int main(int argc, char* argv[])
{
	kuto::GraphicsDevice::createInstance();
	kuto::GraphicsDevice::instance()->initialize(argc, argv, 320.f, 480.f, "RPG Tukuru", update);
	appMain.initialize();

	glutMainLoop();

	return EXIT_SUCCESS;
}

