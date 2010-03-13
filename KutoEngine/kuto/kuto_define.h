#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__MACOS_CLASSIC__) )
	#include <TargetConditionals.h>

	#if defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE==1)
		#define RPG2K_IS_IPHONE
	#endif
	#if defined(TARGET_IPHONE_SIMULATOR) && (TARGET_IPHONE_SIMULATOR==1)
		#define RPG2K_IS_IPHONE_SIMULATOR
	#endif
#endif

#if defined(PSP)
	#define RPG2K_IS_PSP
#endif

#if defined(WIN32) || defined(WIN64)
	#define RPG2K_IS_WINDOWS
#endif
