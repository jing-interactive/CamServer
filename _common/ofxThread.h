#ifndef _OFX_THREAD_H_
#define _OFX_THREAD_H_

#ifdef WIN32
	#include <windows.h>
	#include <process.h>
#else
    #include <pthread.h>
    #include <semaphore.h>
#endif
#include <stdio.h>

class ofxThread{

	public:
		ofxThread();
		virtual ~ofxThread();
		bool isThreadRunning();
		void startThread(bool _blocking = true, bool _verbose = true);
		bool lock();
		bool unlock();
		void stopThread();

	protected:

		//-------------------------------------------------
		//you need to overide this with the function you want to thread
		virtual void threadedFunction(){
			if(verbose)printf("ofxThread: overide threadedFunction with your own\n");
		}

		//-------------------------------------------------

		#ifdef WIN32
			static unsigned int __stdcall thread(void * objPtr){
				ofxThread* me	= (ofxThread*)objPtr;
				me->threadedFunction();
				return 0;
			}

		#else
			static void * thread(void * objPtr){
				ofxThread* me	= (ofxThread*)objPtr;
				me->threadedFunction();
				return 0;
			}
		#endif


	#ifdef WIN32
			HANDLE            myThread;
			CRITICAL_SECTION  critSec;  	//same as a mutex
	#else
			pthread_t        myThread;
			pthread_mutex_t  myMutex;
	#endif

	bool threadRunning;
	bool blocking;
	bool verbose;
};

#endif
