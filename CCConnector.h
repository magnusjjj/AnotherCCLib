#ifndef CCCONNECTOR_H
#define CCCONNECTOR_H


//#include "CCEffect.h"
#include <regex>
#include <WinSock2.h>
#include <map>
#include <list>
#include <functional>
#include <mutex>
#include "SimpleNullTerminatedTCPSocket.h"

class CCEffect;
typedef CCEffect* (*CCCallback2)();

class CCConnector
{
	SimpleNullTerminatedTCPSocket* connection = NULL;
	void OnTCPMessageCallbackReal(const char* message);
	std::map<std::regex*, CCCallback2> effectmap;
	std::list<CCEffect*> runningeffects;
	std::mutex runningEffectsMutex;


	// Timer thread variables
	void RunTimerThread();
	bool timerWasPaused = false;
	bool timerPause = false;
	std::mutex timerPauseMutex;
	std::thread* timerThread = NULL;
public:
	// The constructor and destructor are currently empty, but this is *required* for std::mutex not to give an error due to c++ creating a default copy constructor.. apparantly.
	// It was *not* a fun thing to try to find with the nonsensical error message and failing google :')
	CCConnector();
	~CCConnector();
	bool Connect();
	void AddEffect(std::regex* code_matcher, CCCallback2 callback);
	bool hasError = false;
	char error[100] = "";
	void StartTimerThread();
	void Send(const char* message);
	void PauseTimerThread(bool timerthreadshouldbeactive);
};

#endif // !CCCONNECTOR_H