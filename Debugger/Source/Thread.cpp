//#include "stdafx.h"

#include "Thread.hpp"

#ifdef WIN32

// Windows has no usleep() function.  We can use Sleep() instead,
// but we have to include some other headers to get it.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>

#else

#include <unistd.h>

#endif

#ifdef __APPLE__
#include <sched.h>
#endif


namespace Iocaste
{
	namespace Debugger
	{


void Thread::nice(int msec)
{
	for (int i=0; i < msec; i++)
	{
	    #ifdef __APPLE__
            sched_yield();
	    #else
            pthread_yield();
        #endif
		Thread::usleep(1000);
	}
}

void Thread::usleep(int usec)
{
#ifdef WIN32
	// Stupid Windows has no usleep() function!
	Sleep(usec/1000);
#else
	// But on Linux we're ok.
	::usleep(usec);
#endif
}


	} // Debugger
} // Iocaste
