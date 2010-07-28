/**
 * @file
 * @brief Timer
 * @author project.kuto
 */

#include "kuto_timer.h"
// #include "kuto_define.h"

#if RPG2K_IS_IPHONE || RPG2K_IS_MAC_OS_X
	#include <mach/mach.h>
	#include <mach/mach_time.h>
#elif RPG2K_IS_WINDOWS
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <unistd.h>
#endif



namespace kuto {

u64 Timer::getTime()
{
#if RPG2K_IS_IPHONE || RPG2K_IS_MAC_OS_X
	return mach_absolute_time();
#elif RPG2K_IS_PSP
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec*1000000LL + tv.tv_usec)*100LL;
#elif RPG2K_IS_WINDOWS
	LARGE_INTEGER count;
	QueryPerformanceCounter( &count );
	return count.QuadPart;
#else
	timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	return (tv.tv_sec * 1000000000LL) + tv.tv_nsec;
#endif
}

u64 Timer::getElapsedTimeInNanoseconds(u64 startTime, u64 endTime)
{
#if RPG2K_IS_IPHONE
	u64 elapsed = endTime - startTime;
	static mach_timebase_info_data_t sTimebaseInfo = {0, 0};

	if ( sTimebaseInfo.denom == 0 ) {
		mach_timebase_info(&sTimebaseInfo);
	}
	return elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
#elif RPG2K_IS_WINDOWS
	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );
	return (endTime - startTime) * 1000000000LL / freq.QuadPart;
#else
	return endTime - startTime;
#endif
}

}	// namespace kuto
