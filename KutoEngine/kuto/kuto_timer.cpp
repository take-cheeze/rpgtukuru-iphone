/**
 * @file
 * @brief Timer
 * @author project.kuto
 */

#include "kuto_define.h"
#include "kuto_timer.h"
#if defined(RPG2K_IS_IPHONE)
	#include <mach/mach.h>
	#include <mach/mach_time.h>
	#include <unistd.h>
#elif defined(RPG2K_IS_WINDOWS)
	#include <windows.h>
#endif


namespace kuto {

#if defined(RPG2K_IS_IPHONE)
u64 Timer::getTime()
{
	return mach_absolute_time();
}

u64 Timer::getElapsedTimeInNanoseconds(u64 startTime, u64 endTime)
{
	u64 elapsed = endTime - startTime;
    static mach_timebase_info_data_t sTimebaseInfo = {0, 0};
	if ( sTimebaseInfo.denom == 0 ) {
    	mach_timebase_info(&sTimebaseInfo);
    }
    return elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
}
#elif defined(RPG2K_IS_WINDOWS)
u64 Timer::getTime()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

u64 Timer::getElapsedTimeInNanoseconds(u64 startTime, u64 endTime)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	u64 elapsed = endTime - startTime;
    return elapsed * 1000 * 1000 * 1000 / frequency.QuadPart;
}
#endif

}	// namespace kuto
