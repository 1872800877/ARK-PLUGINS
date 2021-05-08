#pragma once
#include<iostream>
#include<winsock.h>
#include <Logger/Logger.h>
#include <functional>
#include "Plugin.h"
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
using namespace std;
class SocketClient
{
public:
	static SocketClient& Get();
	void sendInfo(std::string info,int type);
	void getInfo(Packet &packet);
	void close();
	bool isConnect();
	inline ~SocketClient();
private:
	SocketClient();
	SOCKET clientSocket;
	bool init();
	atomic<bool> _IsConnect{ true };
};