#pragma once

#include <sstream>

#include <API/ARK/Ark.h>

#include <API/UE/UE.h>

int GetTribeId(AShooterPlayerController* playerController);
FString GetTribeRank(AShooterPlayerController* playerController);
void SendChatMessageToAll(const FString&,const FString&, const FString&);
void SendChatMessageToAll(const unsigned int senderId,const FString& senderName,const FString& senderSteamName,const FString& senderTribeName,const FString& message,UTexture2D* senderIcon);
std::string GetTribeName(AShooterPlayerController* playerController);
std::string GetPlayerName(AShooterPlayerController* playerController);
std::wstring GetPlayerCharacterName(AShooterPlayerController* playerController);
std::wstring AsciiToUnicode(const std::string& str);
std::string UnicodeToUtf8(const std::wstring& wstr);
std::string UnicodeToAscii(const std::wstring& wstr);
std::wstring Utf8ToUnicode(const std::string& str);
std::wstring FromUTF16(const std::u16string& s);
std::u16string ToUTF16(const std::wstring& s);
struct Packet
{
	int size;
	int type;
	std::string data;
	Packet() {
		size = 0;
		type = 0;
		data = "";
	}
};