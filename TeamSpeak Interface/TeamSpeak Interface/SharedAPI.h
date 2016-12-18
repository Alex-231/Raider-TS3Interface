#pragma once

#include <teamspeak/clientlib.h>
#include <string>

using namespace std;

//Since both of these are the same, 
//there's no point in doing this, right?

//It looks nice so it's staying...

#define EXPORT __declspec(dllexport)

extern "C"
{
	EXPORT void SetupLogging(void(*_logCallback)(char* message));
	//VoIPClient bool StartClient(char* username, char* ipAddr, void(*disconnectCallback)(int exitCode, char* exitDetails));
	EXPORT bool StartClient(char* username, char* ipAddr, int port, char* path, ClientUIFunctions callbacks);
	EXPORT bool StopClient();
	EXPORT bool StartServer();
}

class SharedAPI
{
public:
	SharedAPI();
	~SharedAPI();

	static void Log(char* message);

	//void(*SharedAPI::logCallback)(string message);
};

