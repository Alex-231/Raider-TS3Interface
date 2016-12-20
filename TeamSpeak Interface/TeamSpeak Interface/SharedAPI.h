#pragma once

#include <teamspeak/clientlib.h>
#include <teamspeak\serverlib.h>
#include <string>

using namespace std;

//Since both of these are the same, 
//there's no point in doing this, right?

//It looks nice so it's staying...

#define EXPORT __declspec(dllexport)
#define SERVER_EXPORT __declspec(dllexport)
#define CLIENT_EXPORT __declspec(dllexport)

extern "C"
{
	EXPORT void SetupLogging(void(*_logCallback)(char* message));
	//VoIPClient bool StartClient(char* username, char* ipAddr, void(*disconnectCallback)(int exitCode, char* exitDetails));
	CLIENT_EXPORT bool StartClient(char* username, char* ipAddr, int port, char* path, ClientUIFunctions callbacks);
	CLIENT_EXPORT bool StopClient();
	SERVER_EXPORT bool StartServer(char* ipAddr, int port, char* serverName, ServerLibFunctions callbacks);
	SERVER_EXPORT bool StopServer();
}

class SharedAPI
{
public:
	SharedAPI();
	~SharedAPI();

	static void Log(char* message);

	//void(*SharedAPI::logCallback)(string message);
};

