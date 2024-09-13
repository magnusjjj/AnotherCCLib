#pragma once
#include <WinSock2.h>
#include <vector>
#include <string>
#include <functional>
#include <thread>

class SimpleNullTerminatedTCPSocket
{
	SOCKET m_socket = INVALID_SOCKET;
	bool running = false;
	std::vector<std::string> BufferSocketResponse(const char* buf, size_t buf_size);
	std::function<void(const char* message)> onmessagecallback = NULL;

	std::string socketBuffer = "";
	std::thread* runthread;
public:
	SimpleNullTerminatedTCPSocket();
	~SimpleNullTerminatedTCPSocket();
	void _ERROR(const char* fmt...);
	void _MESSAGE(const char* fmt...);
	void _WARNING(const char* fmt...);
	bool hasError;
	char error[100];
	bool connecting = false;
	void ResetError();
	bool Connect(const char* host, const char* port);
	void Send(const char* string);
	void Run();
	void SetMessageCallback(std::function<void(const char* message)> f);
};