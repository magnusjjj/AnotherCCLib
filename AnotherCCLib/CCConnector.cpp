#include "CCConnector.h"
#include <mutex>
#include "CCLock.h"
#include <functional>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "CCMessages.h"
#include "CCEffect.h"

/// <summary>
/// Tries to open communication with the crowd control server locally.
/// </summary>
/// <returns>Whether or not it was successful in connecting.</returns>

CCConnector::CCConnector()
{
}

CCConnector::~CCConnector()
{
}

bool CCConnector::Connect()
{
	this->connection = new SimpleNullTerminatedTCPSocket();
	auto callback = std::bind(&CCConnector::OnTCPMessageCallbackReal, this, std::placeholders::_1);
	this->connection->SetMessageCallback(callback);
	return this->connection->Connect("localhost", "5420");
}

void CCConnector::OnTCPMessageCallbackReal(const char* message) {

	json data = json::parse(message);
	if (!data.is_object()){
		this->connection->_WARNING("Message from crowd control that wasn't an object: %s\n", message);
		return;
	}

	if (!data.contains("type") ) {
		this->connection->_WARNING("Message from crowd control that didn't contain a type field: %s\n", message);
		return;
	}

	switch ((uint8_t)data["type"]) {
		case CCRequestType::EffectStart:
			this->connection->_WARNING("A start of something nice: %s\n", message);
			for (const auto& n : effectmap) {
				const std::string code = data["code"];
				if (std::regex_match(code, *n.first)) {
					CCEffect* neweffect = effectmap[n.first]();
					neweffect->message = data;
					neweffect->connector = this;
					if (data.contains("duration"))
					{
						uint64_t duration = data["duration"];
						if (duration > 0) {
							neweffect->timeleft = duration;
							runningEffectsMutex.lock();
							runningeffects.push_back(neweffect);
							runningEffectsMutex.unlock();
						}
					}
					neweffect->Start();
				}
			}
			break;
		default:
			this->connection->_WARNING("Unhandled message from crowd control: %s\n", message);
			break;
	}
}

void CCConnector::AddEffect(std::regex* code_matcher, CCCallback2 factory)
{
	effectmap.emplace(code_matcher, factory);
}

void CCConnector::StartTimerThread()
{
	this->timerThread = new std::thread(&CCConnector::RunTimerThread, this);
}

void CCConnector::Send(const char* message) {
	this->connection->_MESSAGE("Sent.. %s", message);
	this->connection->Send(message);
}



void CCConnector::RunTimerThread() {
	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

	while (true) {
		this->timerPauseMutex.lock();
		if (this->timerPause) { // We snoozing bois. Sleep and stop executing until after it.
			this->timerPauseMutex.unlock();
			Sleep(100);
			continue;
		}
		else if (this->timerWasPaused) {
			this->timerWasPaused = false;
			lastTime = std::chrono::steady_clock::now();
		}

		std::chrono::steady_clock::time_point ticks = std::chrono::steady_clock::now();
		
		ULONGLONG msSinceLastTime = std::chrono::duration_cast<std::chrono::milliseconds>(ticks - lastTime).count();

		runningEffectsMutex.lock();

		runningeffects.remove_if([msSinceLastTime](CCEffect* e) {
			if (e->timeleft > msSinceLastTime)
			{
				e->timeleft = e->timeleft - msSinceLastTime;
				return false;
			}
			else {
				e->Stop();
				e->RespondFinished();
				return true; // TODO: MEMORY LEAK?
			}
		});

		runningEffectsMutex.unlock();

		lastTime = std::chrono::steady_clock::now();

		this->timerPauseMutex.unlock();
		Sleep(100);
	}
	
}

void CCConnector::PauseTimerThread(bool timerthreadshouldbeactive = false)
{
	std::lock_guard<std::mutex> lock(timerPauseMutex);
	if (timerthreadshouldbeactive) {
		this->timerPause = true;
		this->timerWasPaused = true;
	}
	else {
		this->timerPause = false;
	}

	std::lock_guard<std::mutex> lockrunning(runningEffectsMutex);
	

	if(timerthreadshouldbeactive){
		for (const auto& a : runningeffects) {
			a->RespondPause();
		}
	}
	else {
		for (const auto& a : runningeffects) {
			a->RespondUnpaused();
		}
	}
}