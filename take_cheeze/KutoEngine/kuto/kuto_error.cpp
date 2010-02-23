/**
 * @file
 * @brief error
 * @author project.kuto
 */

#include "stdarg.h"
#include <iostream>
#include <cstdio>
// #import <Foundation/Foundation.h>
#include "kuto_error.h"

using namespace std;

namespace kuto {

void debug_printf(const char* str, ...)
{
	static char temp[512];
	va_list args;
	va_start(args, str);
	vsprintf(temp, str, args);
	va_end(args);

	clog << temp << endl;
	// NSString* nsStr = [[NSString alloc] initWithUTF8String:temp];
	// NSLog(nsStr);
	// [nsStr release];
}
	
}	// namespace kuto
