#pragma once

class VoIPServer
{
public:
	VoIPServer();
	~VoIPServer();
	static bool VoIPServer::StartServer(char* ipAddr, int port, char* serverName, struct ServerLibFunctions callbacks);
	static bool VoIPServer::StopServer();
};