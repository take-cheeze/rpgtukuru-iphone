#pragma once

#include <iomanip>
#include <sstream>


namespace kuto
{

static const int KUTO_SS_BUFFER_SIZE = 256;

#define initStringStream(name) \
	char name##Buf[kuto::KUTO_SS_BUFFER_SIZE]; \
	name.rdbuf()->pubsetbuf(name##Buf, kuto::KUTO_SS_BUFFER_SIZE)

};