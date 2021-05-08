#include "Utils.h"

#include "Plugin.h"
#include <API/ARK/Ark.h>


int GetTribeId(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe)
			return tribe->TribeIDField();
	}

	return -1;
}

FString GetTribeRank(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe) {
			uint64 playerId = ArkApi::IApiUtils::GetPlayerID(playerController);

			FString rankName;
			tribe->GetRankNameForPlayerID(&rankName, playerId);
			Log::GetLog()->debug("Utils::GetTribeRank() -> rankName={}", rankName.ToString());
			
			return rankName;
		}
	}

	return FString("");
}
void SendChatMessageToAll(const FString& sender_name, const FString& steam_id, const FString& msg)
{
	FChatMessage chat_message = FChatMessage();
	
	chat_message.SenderName = sender_name;
	chat_message.Message = msg;
	//chat_message.UserId = steam_id;

	const auto& player_controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
	for (TWeakObjectPtr<APlayerController> player_controller : player_controllers)
	{
		AShooterPlayerController* shooter_pc = static_cast<AShooterPlayerController*>(player_controller.Get());
		if (shooter_pc)
		{
			shooter_pc->ClientChatMessage(chat_message);
		}
	}
}
void SendChatMessageToAll(
	const unsigned int senderId,
	const FString& senderName,
	const FString& senderSteamName,
	const FString& senderTribeName,
	const FString& message,
	UTexture2D* senderIcon)
{
	FChatMessage chat_message = FChatMessage();
	chat_message.SenderId = senderId;
	chat_message.SenderName = senderName;
	chat_message.SenderSteamName = senderSteamName;
	chat_message.SenderTribeName = senderTribeName;
	chat_message.Message = message;
	chat_message.SenderIcon = senderIcon;

	auto world = ArkApi::GetApiUtils().GetWorld();
	if (!world) return;

	auto playerControllers = world->PlayerControllerListField();
	for (auto playerController : playerControllers)
	{
		AShooterPlayerController* shooter_pc = static_cast<AShooterPlayerController*>(playerController.Get());
		if (shooter_pc)
		{
			shooter_pc->ClientChatMessage(chat_message);
		}
	}
}
std::string GetPlayerName(AShooterPlayerController* playerController)
{
	std::string playerName;

	auto playerState = playerController->PlayerStateField();
	if (playerState) playerName = ArkApi::Tools::Utf8Encode(*playerState->PlayerNameField());

	return playerName;
}

std::wstring GetPlayerCharacterName(AShooterPlayerController* playerController)
{
	auto characterName = ArkApi::GetApiUtils().GetCharacterName(playerController);

	return *characterName;
}
std::string GetTribeName(AShooterPlayerController* playerController)
{
	std::string tribeName;

	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState)
	{
		auto tribeData = playerState->MyTribeDataField();
		tribeName = tribeData->TribeNameField().ToString();
	}

	return tribeName;
}
std::wstring AsciiToUnicode(const std::string& str) {
	// 预算-缓冲区中宽字节的长度    
	int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	// 给指向缓冲区的指针变量分配内存    
	wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
	// 开始向缓冲区转换字节    
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pUnicode, unicodeLen);
	std::wstring ret_str = pUnicode;
	free(pUnicode);
	return ret_str;
}
std::string UnicodeToUtf8(const std::wstring& wstr) {
	// 预算-缓冲区中多字节的长度    
	int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	// 给指向缓冲区的指针变量分配内存    
	char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
	// 开始向缓冲区转换字节    
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
	std::string ret_str = pAssii;
	free(pAssii);
	return ret_str;
}
std::string UnicodeToAscii(const std::wstring& wstr) {
	// 预算-缓冲区中多字节的长度    
	int ansiiLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	// 给指向缓冲区的指针变量分配内存    
	char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
	// 开始向缓冲区转换字节    
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
	std::string ret_str = pAssii;
	free(pAssii);
	return ret_str;
}
std::wstring Utf8ToUnicode(const std::string& str) {
	// 预算-缓冲区中宽字节的长度    
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	// 给指向缓冲区的指针变量分配内存    
	wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
	// 开始向缓冲区转换字节    
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pUnicode, unicodeLen);
	std::wstring ret_str = pUnicode;
	free(pUnicode);
	return ret_str;
}
std::wstring FromUTF16(const std::u16string& s) {
	return std::wstring(s.begin(), s.end());
}

std::u16string ToUTF16(const std::wstring& s) {
	return std::u16string(s.begin(), s.end());
}