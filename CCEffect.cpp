#include "CCEffect.h"

void CCEffect::Respond(json response)
{
	this->connector->Send(response.dump().c_str());
}

void CCEffect::RespondSimple(CCEffectStatus status)
{
	json response = {
		{"id", (uint64_t)this->message["id"]},
		{"code", (std::string)this->message["code"]},
		{"status", status},
		{"type", CCResponseType::EffectRequest}
	};
	if (this->timeleft > 0) {
		response["timeRemaining"] = this->timeleft;
	}
	this->Respond(response);
}

void CCEffect::RespondOK()
{
	this->RespondSimple(CCEffectStatus::Success);
}

void CCEffect::RespondRetry() {
	this->RespondSimple(CCEffectStatus::Retry);
}

void CCEffect::RespondPause() {
	this->RespondSimple(CCEffectStatus::Paused);
}

void CCEffect::RespondUnpaused() {
	this->RespondSimple(CCEffectStatus::Resumed);
}

void CCEffect::RespondFinished() {
	this->RespondSimple(CCEffectStatus::Finished);
}