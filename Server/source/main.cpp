﻿#include "ofxThread.h"
#include "VideoApp.h"
#include "AppConfig.h"

#define VERSION "0.6.3"

void enableMemleakCheck(int breakpt = 0)
{
	#if defined _DEBUG && defined _MSC_VER
	#include <crtdbg.h>
	if (breakpt > 0)
		_CrtSetBreakAlloc(breakpt);
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	#endif
}

struct StartThread: public ofxThread
{
	void threadedFunction()
	{
		printf("CamServer %s  vinjn.z@gmail.com.\n\n", VERSION);
		for (int b=0;b<8;b++)
		{
			if (theApp.input_inited)//if VideoInput is already initialized
				break;				//exit the thread now
			for (int i=0;i<10;i++)
				printf("%c ", rand()%3+1);
			printf("\n");
			BEEP(sin(b/20.0f*3.14)*300,100);
		}
		printf("\n");
	}
};

struct ReadyThread: public ofxThread
{
	void threadedFunction()
	{
		for (int b=0;b<1;b++)
			BEEP(cos(b/20.0f*3.14)*100,100);
	}
};

void say_byebye()
{
	for (int b=8;b>1;b--)
	{
		for (int j=0;j<8-b;j++)
			printf("\t");
		printf("bye~\n");
		BEEP(sin(b/20.0f*3.14)*300,100);
	}
}

int main(int argc, char** argv )
{
	enableMemleakCheck();

	std::string input_src = theConfig.parse_args(argc, argv);
	argc = 2;
	argv[1] = (char*)input_src.c_str();

	StartThread start_thread;
	start_thread.startThread(false, false);

	if (theApp.init(argc, argv))
	{
		ReadyThread ready_thread;
		ready_thread.startThread(false, false);
		theApp.run();
	}
	say_byebye();

	return 0;
}