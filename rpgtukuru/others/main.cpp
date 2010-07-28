/**
 * @file
 * @brief main loop
 * @author project.kuto
 */

#include <cstdlib>
#include <kuto/kuto_graphics_device.h>
#include "AppMain.h"

#if RPG2K_IS_PSP

#include <pspkernel.h>

PSP_MODULE_INFO("RPG_RT_EMU_2000", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

//PSP_HEAP_SIZE_KB(5000);

int psp_exit_callback(int arg1, int arg2, void *common)
{
	exit(0);
	return 0;
}

int psp_callback_thread(SceSize args, void *argp)
{
	int cbid;
	cbid = sceKernelCreateCallback("Exit Callback", 
				       psp_exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int psp_setup_callbacks(void)
{
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", 
				     psp_callback_thread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	return thid;
}

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
#if RPG2K_IS_PSP
	pspDebugScreenInit();
	psp_setup_callbacks();

	/* Register sceKernelExitGame() to be called when we exit */
	atexit(sceKernelExitGame);
#endif

	kuto::GraphicsDevice::createInstance();
	kuto::GraphicsDevice::instance()->initialize(argc, argv, 320, 480, "RPG Tukuru", update);
	appMain.initialize();

	glutMainLoop();

	return EXIT_SUCCESS;
}

