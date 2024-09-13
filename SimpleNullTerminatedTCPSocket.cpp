#include "SimpleNullTerminatedTCPSocket.h"
#include <ws2tcpip.h>
#include "CCLock.h"
#include <cstdio>
#include <vector>
#include <regex>
#include <mutex>

#define DEFAULT_BUFLEN 512
#pragma comment(lib, "Ws2_32.lib")


SimpleNullTerminatedTCPSocket::SimpleNullTerminatedTCPSocket()
	: error{ "An error occurred" },
	m_socket(INVALID_SOCKET)
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };

	int err = WSAStartup(wVersionRequested, &wsaData);
}

SimpleNullTerminatedTCPSocket::~SimpleNullTerminatedTCPSocket()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	WSACleanup();
}

/// <summary>
/// Tries to open communication with the TCP server.
/// </summary>
/// <param name="host">Host to connect to.</param>
/// <param name="port">Port to connect to.</param>
/// <returns>Whether or not it was successful in connecting.</returns>

bool SimpleNullTerminatedTCPSocket::Connect(const char* host, const char* port)
{
	value_lock<bool> connect_lock(&connecting, true, false);

	try
	{
		int iFamily = AF_INET;
		int iType = SOCK_STREAM;
		int iProtocol = IPPROTO_TCP;

		struct addrinfo* result = NULL,
			* ptr = NULL,
			hints;

		ZeroMemory(&hints, sizeof(hints));

		int iResult = getaddrinfo("127.0.0.1", port, &hints, &result);

		if (iResult != 0)
		{
			hasError = true;
			snprintf(error, sizeof(error), "getaddrinfo error: %d", iResult);

			return false;
		}

		ptr = result;

		m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (m_socket == INVALID_SOCKET)
		{
			hasError = true;
			snprintf(error, sizeof(error), "Socket creation failed: %d", WSAGetLastError());

			return false;
		}

		iResult = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			hasError = true;

			m_socket = INVALID_SOCKET;
			snprintf(error, sizeof(error), "Error connecting to CrowdControl");

			return false;
		}

		ResetError();
		connecting = false;
		this->runthread = new std::thread(&SimpleNullTerminatedTCPSocket::Run, this);

		return true;
	}
	catch (std::exception e)
	{
		_ERROR("[Connector::Connect] %s", e.what());
		if (m_socket != INVALID_SOCKET)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	}

	return false;
}

void SimpleNullTerminatedTCPSocket::Send(const char* string) {
	send(this->m_socket, string, (int)strlen(string) + 1, 0);
}

void SimpleNullTerminatedTCPSocket::Run()
{
	value_lock<bool> run_lock(&running, true, false);
	while (true)
	{
		try
		{
			ResetError();
			int last_error = 0;
			int recvbuflen = DEFAULT_BUFLEN;
			char recvbuf[DEFAULT_BUFLEN];
			ZeroMemory(&recvbuf, sizeof(recvbuf));

			int iResult = recv(m_socket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{

				auto commands = BufferSocketResponse(recvbuf, iResult);


				for (auto c : commands)
				{
					if (c.length() == 0) continue;

					_MESSAGE("Got message: %s", c.c_str());
					if (this->onmessagecallback != NULL) {
						onmessagecallback(c.c_str());
					}
				}
			}

			else if (iResult == 0)
			{
				hasError = true;
				snprintf(error, sizeof(error), "Connection closed");
				m_socket = INVALID_SOCKET;
				break;
			}

			else
			{
				last_error = WSAGetLastError();
				if (last_error != (int)WSAEWOULDBLOCK)
				{
					hasError = true;
					snprintf(error, sizeof(error), "recv failed: %d\n", last_error);
					m_socket = INVALID_SOCKET;
					break;
				}
			}
		}
		catch (std::exception e)
		{
			_ERROR("[Connector::_Run] %s", e.what());
		}
	}
}

void SimpleNullTerminatedTCPSocket::SetMessageCallback(std::function<void(const char* message)> f)
{
	this->onmessagecallback = f;
}

void SimpleNullTerminatedTCPSocket::_ERROR(const char* fmt ...)
{
	va_list args;
	va_start(args, fmt);

	vfprintf(stderr, fmt, args);
	va_end(args);
}

void SimpleNullTerminatedTCPSocket::_MESSAGE(const char* fmt ...)
{
	va_list args;
	va_start(args, fmt);

	vfprintf(stderr, fmt, args);
	va_end(args);
}

void SimpleNullTerminatedTCPSocket::_WARNING(const char* fmt ...)
{
	va_list args;
	va_start(args, fmt);

	vfprintf(stderr, fmt, args);
	va_end(args);
}

void SimpleNullTerminatedTCPSocket::ResetError()
{
	hasError = false;
	ZeroMemory(&error, sizeof(error));
}

std::vector<std::string> SimpleNullTerminatedTCPSocket::BufferSocketResponse(const char* buf, size_t buf_size)
{
	socketBuffer.append(buf, buf_size);
	std::vector<std::string> buffer_array;

	size_t index = socketBuffer.find('\0');
	while (index != std::string::npos)
	{
		buffer_array.push_back(socketBuffer.substr(0, index));
		socketBuffer = socketBuffer.substr(index + 1);
		index = socketBuffer.find('\0');
	}

	return buffer_array;
}