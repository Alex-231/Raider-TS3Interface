#include "SharedAPI.h"
#include "VoIPClient.h"
#include "VoIPServer.h"

using namespace std;



SharedAPI::SharedAPI()
{
}


SharedAPI::~SharedAPI()
{
}

CLIENT_EXPORT bool StartClient(char* username, char* ipAddr, int port, char* path, ClientUIFunctions callbacks)
{
	return VoIPClient::StartClient(username, ipAddr, port, path, callbacks);
}

CLIENT_EXPORT bool StopClient()
{
	return VoIPClient::StopClient();
}

SERVER_EXPORT bool StartServer(char* ipAddr, int port, char* serverName, ServerLibFunctions callbacks)
{
	return VoIPServer::StartServer(ipAddr, port, serverName, callbacks);
}

SERVER_EXPORT bool StopServer()
{
	return VoIPServer::StopServer();
}

void (*logCallback)(char* message);

void SharedAPI::Log(char* message)
{
	if (logCallback != NULL)
		logCallback(message);
}

EXPORT void SetupLogging(void(*_logCallback)(char* message))
{
	logCallback = _logCallback;
}
