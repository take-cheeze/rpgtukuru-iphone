#pragma once

extern "C"
{
	#if (RPG2K_IS_IPHONE || RPG2K_IS_MAC_OS_X)
		#include <OpenAL/al.h>
		#include <OpenAL/alc.h>
		#include <OpenAL/alext.h>
	#elif RPG2K_IS_PSP
		// TODO
	#else
		#include <AL/al.h>
		#include <AL/alc.h>
		#include <AL/alext.h>
	#endif
}
