#ifndef CCEFFECT_H
#define CCEFFECT_H
#include <string>
#include "AnotherCCLib/nlohmann/json.hpp"
#include <iostream>
#include "CCConnector.h"
#include "CCMessages.h"

using json = nlohmann::json;

class CCEffect
{
public:
	CCConnector* connector;
	CCEffect* type;
	std::string code;
	json message;
	std::vector<std::string>* arguments;
	void Respond(json response);
	void RespondSimple(CCEffectStatus status);
	void RespondOK();
	void RespondRetry();
	void RespondPause();
	void RespondUnpaused();
	void RespondFinished();
	virtual void Start() = 0;
	virtual void Stop() = 0;
	uint64_t timeleft; // Time in ms
	
};

#endif