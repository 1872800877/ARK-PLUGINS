#pragma once

#include "json.hpp"
#include "Utils.h"
#include "MyQueue.h"

class Plugin {
public:
	static Plugin& Get();
	bool isNum(std::string str);
	nlohmann::json config;
	HANDLE processRecvThread;
	DWORD processRecvThreadID;
	HANDLE processRecvSendThread;
	DWORD processRecvSendThreadID;
	HANDLE processSendThread;
	DWORD processSendThreadID;
	MyQueue<std::string> sendQueue;
	MyQueue<Packet> recvQueue;
	std::atomic<bool> isLoad{ false };
private:
	Plugin() {
	}
	~Plugin() = default;
};