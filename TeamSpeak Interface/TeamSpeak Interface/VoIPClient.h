#pragma once
#include <string.h>
#include <xstring>
#include <teamspeak/clientlib.h>

static class VoIPClient
{
public:
	VoIPClient();
	~VoIPClient();
	static bool VoIPClient::StartClient(char* username, char* ipAddr, int port, ClientUIFunctions callbacks, char* path);
	static bool VoIPClient::StopClient();

	static unsigned int VoIPClient::error;
	static uint64 VoIPClient::scHandlerID;
	static char* VoIPClient::version;
	static char* VoIPClient::identity;
};

