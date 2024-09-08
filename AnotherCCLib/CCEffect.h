#ifndef CCEFFECT_H
#define CCEFFECT_H
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include "CCConnector.h"

using json = nlohmann::json;

class CCEffect
{
public:
	CCConnector* connector;
	CCEffect* type;
	std::string code;
	json message;
	void Respond(json response);
	void RespondOK();
	virtual void Start() = 0;
	virtual void Stop() = 0;
	uint64_t timeleft; // Time in ms
	
};

#endif