/**
 * @file
 * @brief error
 * @author project.kuto
 */

#include <stdarg.h>
#include <kuto/kuto_error.h>
#import <Foundation/Foundation.h>


namespace kuto {

void debug_printf(const char* str, ...)
{
	static char temp[512];
	va_list args;
	va_start(args, str);
	vsprintf(temp, str, args);
	va_end(args);
	NSString* nsStr = [[NSString alloc] initWithUTF8String:temp];
	NSLog(nsStr);
	[nsStr release];
}
	
}	// namespace kuto

