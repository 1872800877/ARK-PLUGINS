#pragma once
#include <windows.h>
#include <fstream>
#include <iostream>
#include <API/ARK/Ark.h>
#include <Logger/Logger.h>
#include "SocketClient.h"
#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")

namespace CrossServerChat{
	void loadPlugin();
	void unloadPlugin();
}