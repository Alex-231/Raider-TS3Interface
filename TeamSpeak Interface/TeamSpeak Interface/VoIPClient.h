#pragma once
#include <string.h>
#include <xstring>
#include <teamspeak/clientlib.h>

#include <string>

using namespace std;

static class VoIPClient
{
public:
	VoIPClient();
	~VoIPClient();
	static bool VoIPClient::StartClient(char* username, char* ipAddr, int port, char* path, ClientUIFunctions callbacks);
	static bool VoIPClient::StopClient();
};

