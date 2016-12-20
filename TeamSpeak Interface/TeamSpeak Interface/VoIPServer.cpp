#include "VoIPServer.h"
#include "SharedAPI.h"

VoIPServer::VoIPServer()
{
}


VoIPServer::~VoIPServer()
{
}

///*
//* TeamSpeak 3 server sample
//*
//* Copyright (c) 2007-2014 TeamSpeak-Systems
//*/
//
#ifdef _WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <stdio.h>

#include <teamspeak/public_definitions.h>
#include <teamspeak/public_errors.h>
#include <teamspeak/serverlib_publicdefinitions.h>
#include <teamspeak/serverlib.h>

/*
* Callback when client has connected.
*
* Parameter:
*   serverID  - Virtual server ID
*   clientID  - ID of connected client
*   channelID - ID of channel the client joined
*/
void onClientConnected(uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError) {
	SharedAPI::Log((char*)((string)"Client  " + to_string(clientID) + " joined channel " + to_string(channelID) + " on virtual server " + to_string(serverID) + "\n").c_str());
}

/*
* Callback when client has disconnected.
*
* Parameter:
*   serverID  - Virtual server ID
*   clientID  - ID of disconnected client
*   channelID - ID of channel the client left
*/
void onClientDisconnected(uint64 serverID, anyID clientID, uint64 channelID) {
	SharedAPI::Log((char*)((string)"Client  " + to_string(clientID) + " left channel " + to_string(channelID) + " on virtual server " + to_string(serverID) + "\n").c_str());
}

/*
* Callback when client has moved.
*
* Parameter:
*   serverID     - Virtual server ID
*   clientID     - ID of moved client
*   oldChannelID - ID of old channel the client left
*   newChannelID - ID of new channel the client joined
*/
void onClientMoved(uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID) {
	SharedAPI::Log((char*)((string)"Client  " + to_string(clientID) + " moved from channel " + to_string(oldChannelID) + " to channel " + to_string(newChannelID) + " on virtual server " + to_string(serverID) + "\n").c_str());
}

/*
* Callback when channel has been created.
*
* Parameter:
*   serverID        - Virtual server ID
*   invokerClientID - ID of the client who created the channel
*   channelID       - ID of the created channel
*/
void onChannelCreated(uint64 serverID, anyID invokerClientID, uint64 channelID) {
	SharedAPI::Log((char*)((string)"Channel " + to_string(channelID) + " created by" + to_string(invokerClientID) + " on virtual server " + to_string(serverID) + "\n").c_str());
}

/*
* Callback when channel has been edited.
*
* Parameter:
*   serverID        - Virtual server ID
*   invokerClientID - ID of the client who edited the channel
*   channelID       - ID of the edited channel
*/
void onChannelEdited(uint64 serverID, anyID invokerClientID, uint64 channelID) {
	SharedAPI::Log((char*)((string)"Channel " + to_string(channelID) + " edited by" + to_string(invokerClientID) + " on virtual server " + to_string(serverID) + "\n").c_str());
}

/*
* Callback when channel has been deleted.
*
* Parameter:
*   serverID        - Virtual server ID
*   invokerClientID - ID of the client who deleted the channel
*   channelID       - ID of the deleted channel
*/
void onChannelDeleted(uint64 serverID, anyID invokerClientID, uint64 channelID) {
	SharedAPI::Log((char*)((string)"Channel " + to_string(channelID) + " deleted by" + to_string(invokerClientID) + " on virtual server " + to_string(serverID) + "\n").c_str());
}

/*
* Callback for user-defined logging.
*
* Parameter:
*   logMessage        - Log message text
*   logLevel          - Severity of log message
*   logChannel        - Custom text to categorize the message channel
*   logID             - Virtual server ID giving the virtual server source of the log event
*   logTime           - String with the date and time the log entry occured
*   completeLogString - Verbose log message including all previous parameters for convinience
*/
void onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
	/* Your custom error display here... */
	/* printf("LOG: %s\n", completeLogString); */
	if (logLevel == LogLevel_CRITICAL) {
		exit(1);  /* Your custom handling of critical errors */
	}
}

/*
* Callback triggered when the specified client starts talking.
*
* Parameters:
*   serverID - ID of the virtual server sending the callback
*   clientID - ID of the client which started talking
*/
void onClientStartTalkingEvent(uint64 serverID, anyID clientID) {
	SharedAPI::Log((char*)((string)"onClientStartTalkingEvent serverID=" + to_string(serverID) + ", clientID=" + to_string(clientID) + "\n").c_str());
}

/*
* Callback triggered when the specified client stops talking.
*
* Parameters:
*   serverID - ID of the virtual server sending the callback
*   clientID - ID of the client which stopped talking
*/
void onClientStopTalkingEvent(uint64 serverID, anyID clientID) {
	SharedAPI::Log((char*)((string)"onClientStopTalkingEvent serverID=" + to_string(serverID) + ", clientID=" + to_string(clientID) + "\n").c_str());
}

/*
* Callback triggered when a license error occurs.
*
* Parameters:
*   serverID  - ID of the virtual server on which the license error occured. This virtual server will be automatically
*               shutdown. If the parameter is zero, all virtual servers are affected and have been shutdown.
*   errorCode - Code of the occured error. Use ts3server_getGlobalErrorMessage() to convert to a message string
*/
void onAccountingErrorEvent(uint64 serverID, unsigned int errorCode) {
	char* errorMessage = "";
	if (ts3server_getGlobalErrorMessage(errorCode, (char**)errorMessage) == ERROR_ok) {
		SharedAPI::Log((char*)((string)"onClientStopTalkingEvent serverID=" + to_string(serverID) + ", errorCode=" + to_string(errorCode) + ": " + string(errorMessage) + "\n").c_str());
		ts3server_freeMemory(errorMessage);
	}	

	/* Your custom handling here. In a real application, you wouldn't stop the whole process because the TS3 server part went down.
	* The whole idea of this callback is to let you gracefully handle the TS3 server failing to start and to continue your application. */
	//exit(1);
}


//Converted to fopen_s
#include <errno.h>
/*
* Read server key from file
*/
int readKeyPairFromFile(const char *fileName, char *keyPair) {
	FILE *file;
	errno_t err;

	err = fopen_s(&file, fileName, "r");
	if (file == NULL) {
		SharedAPI::Log((char*)((string)"Could not open file '" + string(fileName) + "' for reading keypair\n").c_str());
		return -1;
	}

	fgets(keyPair, BUFSIZ, file);
	if (ferror(file) != 0) {
		fclose(file);
		SharedAPI::Log((char*)((string)"Error reading keypair from file '" + string(fileName) + "'.\n").c_str());
		return -1;
	}
	fclose(file);

	SharedAPI::Log((char*)((string)"Read keypair '" + string(keyPair) + "' from file '" + string(fileName) + "'.\n").c_str());
	return 0;
}

//Converted to fopen_s
/*
* Write server key to file
*/
int writeKeyPairToFile(const char *fileName, const char* keyPair) {
	FILE *file;
	errno_t err;

	err = fopen_s(&file, fileName, "w");
	if (file == NULL) {
		SharedAPI::Log((char*)((string)"Could not open file '" + string(fileName) + "' for writing keypair\n").c_str());
		return -1;
	}

	fputs(keyPair, file);
	if (ferror(file) != 0) {
		fclose(file);
		SharedAPI::Log((char*)((string)"Error writing keypair to file '" + string(fileName) + "'.\n").c_str());
		return -1;
	}
	fclose(file);

	SharedAPI::Log((char*)((string)"Wrote keypair '" + string(keyPair) + "' to file '" + string(fileName) + "'.\n").c_str());
	return 0;
}

char *serverVersion;
uint64 serverID;
unsigned int serverError;
char buffer[BUFSIZ] = { 0 };
char filename[BUFSIZ];
char port_str[20];
char *keyPair;

bool VoIPServer::StartServer(char* ipAddr, int port, char* serverName, ServerLibFunctions callbacks) {

	SharedAPI::Log((char*)((string)"StartServer(" + string(ipAddr) + ", " + to_string(port) + ", " + string(serverName) + ")").c_str());

	/* Now assign the used callback function pointers */
	callbacks.onClientConnected = onClientConnected;
	callbacks.onClientDisconnected = onClientDisconnected;
	callbacks.onClientMoved = onClientMoved;
	callbacks.onChannelCreated = onChannelCreated;
	callbacks.onChannelEdited = onChannelEdited;
	callbacks.onChannelDeleted = onChannelDeleted;
	callbacks.onUserLoggingMessageEvent = onUserLoggingMessageEvent;
	callbacks.onClientStartTalkingEvent = onClientStartTalkingEvent;
	callbacks.onClientStopTalkingEvent = onClientStopTalkingEvent;
	callbacks.onAccountingErrorEvent = onAccountingErrorEvent;

	/* Initialize server lib with callbacks */
	if ((serverError = ts3server_initServerLib(&callbacks, LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING, NULL)) != ERROR_ok) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(serverError, &errormsg) == ERROR_ok) {
			SharedAPI::Log((char*)((string)"Error initialzing serverlib: " + string(errormsg) + "\n").c_str());
			ts3server_freeMemory(errormsg);
		}
		return false;
	}

	/* Query and print server lib version */
	if ((serverError = ts3server_getServerLibVersion(&serverVersion)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error querying server lib version:" + to_string(serverError) + "\n").c_str());
		return false;
	}
	SharedAPI::Log((char*)((string)"Server lib version: " + string(serverVersion) + "\n").c_str());

	ts3server_freeMemory(serverVersion);  /* Release dynamically allocated memory */

									/* Attempt to load keypair from file */
									/* Assemble filename: keypair_<port>.txt */
	strcpy_s(filename, "keypair_");
	sprintf_s(port_str, "%d", 9987);  // Default port
	strcat_s(filename, port_str);
	strcat_s(filename, ".txt");

	/* Try reading keyPair from file */
	if (readKeyPairFromFile(filename, buffer) == 0) {
		keyPair = buffer;  /* Id read from file */
	}
	else {
		keyPair = "";  /* No Id saved, start virtual server with empty keyPair string */
	}

	/* Create virtual server with max 32 slots */

	/* Create the virtual server with specified port, name, keyPair and max clients */
	SharedAPI::Log((char*)((string)"Create virtual server using keypair '" + string(keyPair) + "'\n").c_str());
	if ((serverError = ts3server_createVirtualServer(port, ipAddr, serverName, keyPair, 32, &serverID)) != ERROR_ok) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(serverError, &errormsg) == ERROR_ok) {
			SharedAPI::Log((char*)((string)"Error creating virtual server: " + string(errormsg) + " (" + to_string(serverError) + ")\n").c_str());
			ts3server_freeMemory(errormsg);
		}
		return false;
	}

	/* If we didn't load the keyPair before, query it from virtual server and save to file */
	if (!*buffer) {
		if ((serverError = ts3server_getVirtualServerKeyPair(serverID, &keyPair)) != ERROR_ok) {
			char* errormsg;
			if (ts3server_getGlobalErrorMessage(serverError, &errormsg) == ERROR_ok) {
				SharedAPI::Log((char*)((string)"Error querying keyPair: " + string(errormsg) + "\n\n").c_str());
				ts3server_freeMemory(errormsg);
			}
			return false;
		}

		/* Save keyPair to file "keypair_<port>.txt"*/
		if (writeKeyPairToFile(filename, keyPair) != 0) {
			ts3server_freeMemory(keyPair);
			return false;
		}
		ts3server_freeMemory(keyPair);
	}

	/* Set welcome message */
	if ((serverError = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_WELCOMEMESSAGE, "Hello TeamSpeak 3")) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error setting server welcomemessage: " + to_string(serverError) + "\n").c_str());
		return false;
	}

	/* Set server password */
	if ((serverError = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_PASSWORD, "secret")) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error setting server password: " + to_string(serverError) + "\n").c_str());
		return false;
	}

	/* Flush above two changes to server */
	if ((serverError = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error flushing server variables: " + to_string(serverError) + "\n").c_str());
		return false;
	}

	return true;
}

bool VoIPServer::StopServer() {

	SharedAPI::Log("StopServer()");

	/* Stop virtual server */
	if ((serverError = ts3server_stopVirtualServer(serverID)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error stopping virtual server: " + to_string(serverError) + "\n").c_str());
		return false;
	}

	/* Shutdown server lib */
	if ((serverError = ts3server_destroyServerLib()) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error destroying server lib: " + to_string(serverError) + "\n").c_str());
		return false;
	}

	return true;
}

