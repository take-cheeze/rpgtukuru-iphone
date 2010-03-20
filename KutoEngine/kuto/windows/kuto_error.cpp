/**
 * @file
 * @brief error
 * @author project.kuto
 */

#include <stdio.h>
#include <stdarg.h>
#include <kuto/kuto_error.h>
#include <windows.h>


namespace kuto {

void debug_printf(const char* str, ...)
{
	static char temp[512];
	va_list args;
	va_start(args, str);
	vsprintf(temp, str, args);
	va_end(args);
	OutputDebugString(temp);
}
	
}	// namespace kuto

