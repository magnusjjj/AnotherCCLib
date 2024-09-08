#include "CCEffect.h"
#include "CCMessages.h"

void CCEffect::Respond(json response)
{
	
}

void CCEffect::RespondOK()
{
	json response;
	response = {
		{"id", (uint64_t)this->message["id"]},
		{"code", (std::string)this->message["code"]},
		{"status", CCEffectStatus::Success},
		//{"message", ""}, //ommited for anything other than login
		{"type", CCResponseType::EffectRequest}
	};
	if (this->timeleft > 0) {
		response["timeRemaining"] = this->timeleft;
	}
	this->Respond(response);
}