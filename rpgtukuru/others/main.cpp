/**
 * @file
 * @brief main loop
 * @author project.kuto
 */

#include <cstdlib>
#include <kuto/kuto_audio_device.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_memory.h>
#include "AppMain.h"

#include <rpg2k/Define.hpp>

#if RPG2K_IS_PSP

#include <pspkernel.h>

// PSP_MODULE_INFO("RPG_RT_EMU_2000", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

PSP_HEAP_SIZE_KB(5000);

#endif

namespace
{
	enum {
	#if RPG2K_IS_PSP
		SCREEN_HEIGHT = 240,
	#else
		SCREEN_HEIGHT = 480,
	#endif
	};

	AppMain* appMain_ = NULL;

	void update(float dt)
	{
		appMain_->update();
	}
}; // namespace

AppMain& AppMain::instance() { return *appMain_; }

#if RPG2K_IS_PSP
extern "C" int SDL_main(int argc, char* argv[])
#else
extern "C" int main(int argc, char* argv[])
#endif
{
	AppMain appMain;
	appMain_ = &appMain;
	appMain.initialize();
	kuto::GraphicsDevice::instance().initialize(argc, argv, 320, SCREEN_HEIGHT, "RPG Tukuru", update);

	glutMainLoop();

	return EXIT_SUCCESS;
}
