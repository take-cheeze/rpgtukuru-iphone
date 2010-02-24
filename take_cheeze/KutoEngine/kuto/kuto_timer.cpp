/**
 * @file
 * @brief Timer
 * @author project.kuto
 */

#include "kuto_timer.h"
#include "kuto_define.h"

#if defined(RPG2K_IS_IPHONE)
	#include <mach/mach.h>
	#include <mach/mach_time.h>
#else
	#include <sys/time.h>
#endif

#include <unistd.h>

using namespace std;

namespace kuto {

u64 Timer::getTime()
{
#if defined(RPG2K_IS_IPHONE)
	return mach_absolute_time();
#else
/*
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec*1000000LL + tv.tv_usec)*100LL;
 */

	timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	// cout << tv.tv_sec << " " << tv.tv_nsec << endl;
	return (tv.tv_sec * 1000000000LL) + tv.tv_nsec;
// returning nanoseconds
#endif
}

u64 Timer::getElapsedTimeInNanoseconds(u64 startTime, u64 endTime)
{
#if defined(RPG2K_IS_IPHONE)
	u64 elapsed = endTime - startTime;
	static mach_timebase_info_data_t sTimebaseInfo = {0, 0};

	if ( sTimebaseInfo.denom == 0 ) {
		mach_timebase_info(&sTimebaseInfo);
	}
	return elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
#else
	return endTime - startTime;
#endif
}

}	// namespace kuto
