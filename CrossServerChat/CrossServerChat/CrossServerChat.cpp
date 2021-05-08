#pragma once
#include "CrossServerChat.h"

DECLARE_HOOK(AShooterGameMode_InitGame, void, AShooterGameMode*, FString*, FString*, FString*);

void Hook_AShooterGameMode_InitGame(AShooterGameMode* a_shooter_game_mode, FString* map_name, FString* options,
	FString* error_message)
{
	const auto& socketValue = Plugin::Get().config["Socket"];
	const auto& lunchMessage = Plugin::Get().config["LunchMessage"];
	const bool enable = lunchMessage["enable"];
	if (enable)
	{
		nlohmann::json item = nlohmann::json({});
		item["map"] = socketValue["map_name"].get<std::string>();
		item["message"] = lunchMessage["message"].get<std::string>();
		item["name"] = lunchMessage["name"].get<std::string>();
		item["steam_id"] = "";
		item["port"] = std::to_string(a_shooter_game_mode->RCONPortField());
		SocketClient::Get().sendInfo(item.dump(), 2);
	}
	AShooterGameMode_InitGame_original(a_shooter_game_mode, map_name, options, error_message);
}

DWORD WINAPI ProcessSendQueue(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);
	while (Plugin::Get().isLoad)
	{
		string value = Plugin::Get().sendQueue.popWait();
		if (value.size()>0)
		{
			try
			{
				SocketClient::Get().sendInfo(value, 1);
			}
			catch (const std::exception& e)
			{
				Log::GetLog()->error("Fail to send message:{};Exception:{}",value,string(e.what()));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
	}
	Log::GetLog()->info("Stop process Send thread");
	return 0;
}
DWORD WINAPI ProcessRecvQueue(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);
	while (Plugin::Get().isLoad)
	{
		Packet packet = Plugin::Get().recvQueue.popWait();
		if (packet.data.size() > 0)
		{
			try
			{
				//Log::GetLog()->debug("Recv message:" + UnicodeToAscii(Utf8ToUnicode(packet.data)));
				nlohmann::json message_entry = nlohmann::json::parse(packet.data);
				//FString message = FString(ArkApi::Tools::Utf8Decode(message_entry.value("message", "")));
				std::string message = message_entry.value("message", "");
				if (message.size()) {
					FString tribeName = FString(ArkApi::Tools::Utf8Decode(message_entry.value("tribeName", "")));
					FString name = FString(ArkApi::Tools::Utf8Decode(message_entry.value("name", "UnKnownName")));
					FString map = FString(ArkApi::Tools::Utf8Decode(message_entry.value("map", "UnKnownMap")));
					FString sendHead = tribeName.IsEmpty() ? FString::Format(*FString("{} <{}>"), *name, *map) : sendHead = FString::Format(*FString("{} [{}]<{}>"), *name, *tribeName, *map);
					FString steam_id = FString(message_entry.value("steam_id", "0"));
					//FString steamName = FString(ArkApi::Tools::Utf8Decode(message_entry.value("steamName", "")));
					Log::GetLog()->info("{}:{}", sendHead.ToString(), UnicodeToAscii(Utf8ToUnicode(message.c_str())));
					SendChatMessageToAll(sendHead, steam_id, ArkApi::Tools::Utf8Decode(message).c_str());
				}
			}
			catch (const std::exception& e)
			{
				Log::GetLog()->error("Fail to send message:{};Exception:{}", packet.data, string(e.what()));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
	}
	Log::GetLog()->info("Stop process recv queue thread");
	return 0;
}
DWORD WINAPI ProcessRecv(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);
	while (Plugin::Get().isLoad)
	{
		Packet packet;
		try
		{
			SocketClient::Get().getInfo(packet);
			if (packet.data.size()&& ArkApi::GetApiUtils().GetStatus() == ArkApi::ServerStatus::Ready) {
				Plugin::Get().recvQueue.push(packet);
			}
		}
		catch (const std::exception& e)
		{
			Log::GetLog()->error("Fail to process Recv:{}",string(e.what()));
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	Log::GetLog()->info("Stop process Recv thread");
	return 0;
}
bool ChatMessageCallback(AShooterPlayerController* _AShooterPlayerController, FString* Message, EChatSendMode::Type Mode, bool spam_check, bool command_executed)
{
	if (spam_check || command_executed) return false;

	if (_AShooterPlayerController && Message && Mode == EChatSendMode::Type::GlobalChat)
	{
		try
		{
			nlohmann::json item;
			uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(_AShooterPlayerController);
			uint64 playerID = ArkApi::GetApiUtils().GetPlayerID(_AShooterPlayerController);
			FString senderSteamName = ArkApi::GetApiUtils().GetSteamName(_AShooterPlayerController);
			const auto& socketValue = Plugin::Get().config["Socket"];
			FString name = ArkApi::GetApiUtils().GetCharacterName(_AShooterPlayerController);
			AShooterGameMode* game_mode = ArkApi::GetApiUtils().GetShooterGameMode();
			string tribeName = GetTribeName(_AShooterPlayerController);
			item["map"] = socketValue["map_name"].get<std::string>();
			item["steam_id"] = std::to_string(steam_id);
			item["message"] = Message->ToString();
			item["name"] = name.ToString();
			item["tribeName"] = tribeName;
			item["steamName"] = senderSteamName.ToString();
			Plugin::Get().sendQueue.push(item.dump());
		}
		catch (const std::exception& e)
		{
			Log::GetLog()->error("Fail process ChatMessageCallback:{}",string(e.what()));
		}
	}
	return false;
}
void loadConfig() {
	auto& plugin = Plugin::Get();
	try {
		const std::string path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/CrossServerChat/config.json";
		std::ifstream file{ path };
		if (!file.is_open()) {
			throw std::runtime_error("Failed to load config!");
		}
		file >> plugin.config;
		file.close();
	}
	catch (const std::exception& error) {
		Log::GetLog()->error(error.what());
		throw;
	}
}

void loadConfigRconCmd(RCONClientConnection* conn, RCONPacket* packet, UWorld* world) {
	if (Plugin::Get().isLoad)
	{
		FString response = "Already loaded config";
		conn->SendMessageW(packet->Id, 0, &response);
	}
	else {
		CrossServerChat::loadPlugin();
		FString response = "Loaded config";
		conn->SendMessageW(packet->Id, 0, &response);
	}
}

void unloadConfigRconCmd(RCONClientConnection* conn, RCONPacket* packet, UWorld* world) {
	CrossServerChat::unloadPlugin();
	FString response = "SuccessFul unloaded CrossServerChat";
	conn->SendMessageW(packet->Id, 0, &response);
}

void CrossServerChat::loadPlugin() {
	Log::Get().Init("CrossServerChat");
	loadConfig();
	auto& plugin = Plugin::Get();
	plugin.isLoad = true;
	plugin.processRecvThread = CreateThread(NULL, 0, ProcessRecv, NULL, 0, &plugin.processRecvThreadID);
	if (plugin.processRecvThread == NULL)
	{
		Log::GetLog()->error("Failed to create process recv thread.");
		throw;
	}
	plugin.processRecvSendThread = CreateThread(NULL, 0, ProcessRecvQueue, NULL, 0, &plugin.processRecvSendThreadID);
	if (plugin.processRecvSendThread == NULL)
	{
		Log::GetLog()->error("Failed to create process recv thread.");
		CloseHandle(Plugin::Get().processRecvThread);
		throw;
	}
	plugin.processSendThread = CreateThread(NULL, 0, ProcessSendQueue, NULL, 0, &plugin.processSendThreadID);
	if (plugin.processSendThread == NULL)
	{
		Log::GetLog()->error("Failed to create process send thread.");
		CloseHandle(Plugin::Get().processRecvThread);
		CloseHandle(Plugin::Get().processRecvSendThread);
		throw;
	}
	ArkApi::GetHooks().SetHook("AShooterGameMode.InitGame", &Hook_AShooterGameMode_InitGame, &AShooterGameMode_InitGame_original);
	ArkApi::GetCommands().AddOnChatMessageCallback("CrossServerChatCallback", &ChatMessageCallback);
	ArkApi::GetCommands().AddRconCommand("CrossServerChat.Unload", &unloadConfigRconCmd);
	ArkApi::GetCommands().AddRconCommand("CrossServerChat.load", &loadConfigRconCmd);
}

void CrossServerChat::unloadPlugin() {
	auto& plugin = Plugin::Get();
	ArkApi::GetHooks().DisableHook("AShooterGameMode.InitGame", &Hook_AShooterGameMode_InitGame);
	ArkApi::GetCommands().RemoveOnChatMessageCallback("CrossServerChatCallback");
	ArkApi::GetCommands().RemoveRconCommand("CrossServerChat.Unload");
	ArkApi::GetCommands().RemoveRconCommand("CrossServerChat.load");
	plugin.isLoad = false;
	plugin.sendQueue.notify();
	plugin.recvQueue.notify();
	SocketClient::Get().close();
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	FindClose(Plugin::Get().processSendThread);
	FindClose(Plugin::Get().processRecvThread);
	FindClose(Plugin::Get().processRecvSendThread);
}

void load() {
	CrossServerChat::loadPlugin();
}

void unload() {
	CrossServerChat::unloadPlugin();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD event, LPVOID lpReserved) {
	switch (event) {
		case DLL_PROCESS_ATTACH:
			load();
			break;
		case DLL_PROCESS_DETACH:
			unload();
			break;
	}
	return TRUE;
}
