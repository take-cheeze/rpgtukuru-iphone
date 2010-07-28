/**
 * @file
 * @brief error
 * @author project.kuto
 */

#include <cstdio>
#include <cstdarg>
#include <kuto/kuto_error.h>

#if RPG2K_IS_WINDOWS
	#include <windows.h>
#endif


namespace kuto {

void debug_printf(const char* str, ...)
{
	char temp[512];
	va_list args;
	va_start(args, str);
	vsprintf(temp, str, args);
	va_end(args);
#if RPG2K_IS_WINDOWS
	OutputDebugString(temp);
#else
	fputs(temp, stderr);
#endif
}
	
}	// namespace kuto

