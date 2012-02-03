#pragma once

#include <windows.h>
#include <assert.h>


//uncomment this if u have "FloWrite.h"
//#define USING_FLO_WRITE

#ifdef USING_FLO_WRITE
#include "FloWrite.h"
#else
#include <stdio.h>
#endif

#pragma comment(lib,"winmm.lib")

class MiniTimer
{
public:
	MiniTimer()
	{
		resetStartTime();
	}

	DWORD getTimeElapsedMS()//mil-seconds
	{
		return ::timeGetTime() - _start_time;
	}
	void resetStartTime()
	{
		_start_time = ::timeGetTime();
	}
	float getTimeElapsed()//seconds
	{
		return 0.001f*(::timeGetTime() - _start_time);
	}

	void profileFunction(char* funcName)
	{
#ifdef USING_FLO_WRITE
		FloWrite("%s[%s] : %d ms\n", getProperBlank(), funcName, getTimeElapsedMS());
#else
		printf("%s[%s] : %d ms\n", getProperBlank(), funcName, getTimeElapsedMS());	
#endif
		resetStartTime();
	}

	static char* getProperBlank()
	{
		return "--";
	}

private:
	DWORD _start_time;
};

