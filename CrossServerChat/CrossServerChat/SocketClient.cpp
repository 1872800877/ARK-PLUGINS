#include "SocketClient.h"
SocketClient::SocketClient() {
	_IsConnect = false;
}
SocketClient& SocketClient::Get() {
	static SocketClient instance;
	if (!instance._IsConnect) {
		if (instance.init()) {
			instance._IsConnect = true;
		}
	}
	return instance;
}
bool SocketClient::isConnect() {
	return _IsConnect;
}
bool SocketClient::init() {
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	const auto& socket = Plugin::Get().config["Socket"];
	string url = socket["url"];
	int port = socket["port"];
	SOCKADDR_IN socketAddr;
	socketAddr.sin_family = PF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr(url.c_str());
	socketAddr.sin_port = htons(port);
	int nNetTimeout = 1000;
	if (SOCKET_ERROR == setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int)))
	{
		Log::GetLog()->error("Set SO_SNDTIMEO error !\n");
		return false;
	}
	int cRes = connect(clientSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == cRes|| INVALID_SOCKET == cRes) {
		Log::GetLog()->error("Fail to connect: {}:{}", url,std::to_string(port));
		return false;
	}
	else {
		Log::GetLog()->info("Success to connect: {}:{}" ,url, std::to_string(port));
		return true;;
	}
}
void SocketClient::sendInfo(const std::string info,int type) {
	if (info.size()==0) {
		return;
	}
	const size_t len = info.length();
	DWORD64 packet = reinterpret_cast<DWORD64>(FMemory::Malloc(len + 10));
	*(int*)packet = len + 10;
	*(int*)(packet + 4) = type;
	memcpy_s((void*)(packet + 8), len, info.data(), len);
	*(char*)(len + packet + 8) = 0;
	*(char*)(len + packet + 9) = 0;
	int send_len = send(clientSocket, (char*)packet, len + 10, 0);
	if (send_len <=0 ) {
		Log::GetLog()->error("Fail to send message:"+ info);
		close();
	}
	FMemory::Free(reinterpret_cast<void*>(packet));
}
void SocketClient::getInfo(Packet &packet) {
	char recvBuf[1024];
	int size = 0;
	int packet_size = 1;
	char* packet_data = nullptr;
	while (size < packet_size)
	{
		int RecvSize = recv(clientSocket, recvBuf, 1024, 0);
		size += RecvSize;
		if (RecvSize <= 0) {
			Log::GetLog()->error("Recv message Fail");
			close();
			break;
		}
		if (!packet_data)
		{
			if (RecvSize <10)
			{
				Log::GetLog()->error("At last 10 Bytes in data packet");
				close();
				break;
			}
			memcpy_s(&packet_size,4, recvBuf, 4);
			if (packet_size==0|| packet_size> 2147483647)
			{
				Log::GetLog()->error("Data packet size excepotion："+ packet_size);
				close();
				break;
			}
			packet.size = packet_size;
			packet_data = (char*)FMemory::Malloc(packet_size);
		}
		memcpy_s(packet_data, RecvSize, recvBuf, RecvSize);
		memset(recvBuf, '\0', 1024);
	}
	if (packet_data) {
		int type = 0;
		memcpy_s(&type, 4, packet_data + 4, 4);
		packet.type = type;
		packet.data = packet_data + 8;
		int null1 = *(packet_data + packet.size - 2);
		int null2 = *(packet_data + packet.size - 1);
		FMemory::Free(packet_data);
		if (null1!= null2|| null1!=0)
		{
			packet.data.clear();
			packet.size = 0;
		}
	}
}
void SocketClient::close() {
	if (_IsConnect)
	{
		Log::GetLog()->info("Close Socket");
		_IsConnect = false;
		closesocket(clientSocket);
		WSACleanup();
	}
}
inline SocketClient::~SocketClient()
{
	close();
}