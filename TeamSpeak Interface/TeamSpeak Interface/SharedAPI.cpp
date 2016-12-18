#include "SharedAPI.h"
#include "VoIPClient.h"

using namespace std;



SharedAPI::SharedAPI()
{
}


SharedAPI::~SharedAPI()
{
}

EXPORT bool StartClient(char* username, char* ipAddr, int port, char* path, ClientUIFunctions callbacks)
{
	return VoIPClient::StartClient(username, ipAddr, port, path, callbacks);
}

EXPORT bool StopClient()
{
	return VoIPClient::StopClient();
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
