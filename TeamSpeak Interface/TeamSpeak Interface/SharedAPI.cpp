#include "SharedAPI.h"
#include "VoIPClient.h"



SharedAPI::SharedAPI()
{
}


SharedAPI::~SharedAPI()
{
}

EXPORT bool StartClient(char* username, char* ipAddr, int port, ClientUIFunctions callbacks, char* path)
{
	return VoIPClient::StartClient(username, ipAddr, port, callbacks, path);
}

EXPORT bool StopClient()
{
	return VoIPClient::StopClient();
}

void(*SharedAPI::logCallback)(char* message);
EXPORT void SetupLogging(void(*_logCallback)(char* message))
{
	SharedAPI::logCallback = _logCallback;
}
