#include "VoIPClient.h"
#include "SharedAPI.h"
#include <utility>
#include <string>

using namespace std;

VoIPClient::VoIPClient()
{
}


VoIPClient::~VoIPClient()
{
}

/*
* TeamSpeak 3 client minimal sample
*
* Copyright (c) 2007-2014 TeamSpeak-Systems
*/

#ifdef _WIN32
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
#include <teamspeak/clientlib_publicdefinitions.h>
#include <teamspeak/clientlib.h>

#ifdef _WIN32
#define SLEEP(x) Sleep(x)
#define strdup(x) _strdup(x)
#else
#define SLEEP(x) usleep(x*1000)
#endif

/*
* Callback for connection status change.
* Connection status switches through the states STATUS_DISCONNECTED, STATUS_CONNECTING, STATUS_CONNECTED and STATUS_CONNECTION_ESTABLISHED.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   newStatus                 - New connection status, see the enum ConnectStatus in clientlib_publicdefinitions.h
*   errorNumber               - Error code. Should be zero when connecting or actively disconnection.
*                               Contains error state when losing connection.
*/
void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	SharedAPI::Log((char*)((string)"Connect status changed : " + to_string(serverConnectionHandlerID) + " " + to_string(newStatus) + " " + to_string(errorNumber) + "\n").c_str());
	/* Failed to connect ? */
	if (newStatus == STATUS_DISCONNECTED && errorNumber == ERROR_failed_connection_initialisation) {
		SharedAPI::Log("Looks like there is no server running.\n");
	}
}

/*
* Callback for current channels being announced to the client after connecting to a server.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   channelID                 - ID of the announced channel
*   channelParentID           - ID of the parent channel
*/
void onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID) {
	/* Query channel name from channel ID */
	char* name;
	unsigned int error;

	SharedAPI::Log((char*)((string)"onNewChannelEvent: " + to_string(serverConnectionHandlerID) + " " + to_string(channelID) + " " + to_string(channelParentID) + "\n").c_str());
	if ((error = ts3client_getChannelVariableAsString(serverConnectionHandlerID, channelID, CHANNEL_NAME, &name)) == ERROR_ok) {
		SharedAPI::Log((char*)((string)"New channel: " + to_string(channelID) + " " + string(name) + "\n").c_str());
		ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
	}
	else {
		char* errormsg;
		if (ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			SharedAPI::Log((char*)((string)"Error getting channel name in onNewChannelEvent: " + string(errormsg) + "\n").c_str());
			ts3client_freeMemory(errormsg);
		}
	}
}

/*
* Callback for just created channels.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   channelID                 - ID of the announced channel
*   channelParentID           - ID of the parent channel
*   invokerID                 - ID of the client who created the channel
*   invokerName               - Name of the client who created the channel
*/
void onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	char* name;

	/* Query channel name from channel ID */
	if (ts3client_getChannelVariableAsString(serverConnectionHandlerID, channelID, CHANNEL_NAME, &name) != ERROR_ok)
		return;

	SharedAPI::Log((char*)((string)"New channel created: " + string(name) + "\n").c_str());
	ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
}

/*
* Callback when a channel was deleted.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   channelID                 - ID of the deleted channel
*   invokerID                 - ID of the client who deleted the channel
*   invokerName               - Name of the client who deleted the channel
*/
void onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	SharedAPI::Log((char*)((string)"Channel ID " + to_string(channelID) +  " deleted by " + string(invokerName) + " " + to_string(invokerID) + "\n").c_str());
}

/*
* Called when a client joins, leaves or moves to another channel.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   clientID                  - ID of the moved client
*   oldChannelID              - ID of the old channel left by the client
*   newChannelID              - ID of the new channel joined by the client
*   visibility                - Visibility of the moved client. See the enum Visibility in clientlib_publicdefinitions.h
*                               Values: ENTER_VISIBILITY, RETAIN_VISIBILITY, LEAVE_VISIBILITY
*/
void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	SharedAPI::Log((char*)((string)"ClientID " + to_string(clientID) + " moves from channel " + to_string(oldChannelID) + " to " + to_string(newChannelID) + "\n").c_str());
}

/*
* Callback for other clients in current and subscribed channels being announced to the client.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   clientID                  - ID of the announced client
*   oldChannelID              - ID of the subscribed channel where the client left visibility
*   newChannelID              - ID of the subscribed channel where the client entered visibility
*   visibility                - Visibility of the announced client. See the enum Visibility in clientlib_publicdefinitions.h
*                               Values: ENTER_VISIBILITY, RETAIN_VISIBILITY, LEAVE_VISIBILITY
*/
void onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility) {
	char* name;

	/* Query client nickname from ID */
	if (ts3client_getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &name) != ERROR_ok)
		return;

	SharedAPI::Log((char*)((string)"New client: " + string(name) + "\n").c_str());
	ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
}

/*
* Called when a client drops his connection.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   clientID                  - ID of the moved client
*   oldChannelID              - ID of the channel the leaving client was previously member of
*   newChannelID              - 0, as client is leaving
*   visibility                - Always LEAVE_VISIBILITY
*   timeoutMessage            - Optional message giving the reason for the timeout
*/
void onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage) {
	SharedAPI::Log((char*)((string)"ClientID " + to_string(clientID) + " timeouts with message " + string(timeoutMessage) + "\n").c_str());
}

/*
* This event is called when a client starts or stops talking.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   status                    - 1 if client starts talking, 0 if client stops talking
*   isReceivedWhisper         - 1 if this event was caused by whispering, 0 if caused by normal talking
*   clientID                  - ID of the client who announced the talk status change
*/
void onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	char* name;

	/* Query client nickname from ID */
	if (ts3client_getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &name) != ERROR_ok)
		return;
	if (status == STATUS_TALKING) {
		SharedAPI::Log((char*)((string)"Client " + string(name) + " starts talking.\n").c_str());
	}
	else {
		SharedAPI::Log((char*)((string)"Client " + string(name) + " stops talking.\n").c_str());
	}
	ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
}

void onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	SharedAPI::Log((char*)((string)"Error for server " + to_string(serverConnectionHandlerID) + ": " + string(errorMessage) + " " + string(extraMessage) + "\n").c_str());
}

unsigned int error;
uint64 scHandlerID;
char* version;
char* identity_;

bool VoIPClient::StartClient(char* username, char* ipAddr, int port, char* path, ClientUIFunctions callbacks)
{
	//Debug Logging
	SharedAPI::Log((char*)((string)"StartClient(" + string(username) + ", " + string(ipAddr) + ", " + to_string(port) + ", " + string(path) + ")").c_str());

	/* Callback function pointers */
	/* It is sufficient to only assign those callback functions you are using. When adding more callbacks, add those function pointers here. */
	callbacks.onConnectStatusChangeEvent = onConnectStatusChangeEvent;
	callbacks.onNewChannelEvent = onNewChannelEvent;
	callbacks.onNewChannelCreatedEvent = onNewChannelCreatedEvent;
	callbacks.onDelChannelEvent = onDelChannelEvent;
	callbacks.onClientMoveEvent = onClientMoveEvent;
	callbacks.onClientMoveSubscriptionEvent = onClientMoveSubscriptionEvent;
	callbacks.onClientMoveTimeoutEvent = onClientMoveTimeoutEvent;
	callbacks.onTalkStatusChangeEvent = onTalkStatusChangeEvent;
	callbacks.onServerErrorEvent = onServerErrorEvent;

	/* Initialize client lib with callbacks */
	/* Resource path points to the SDK\bin directory to locate the soundbackends*/
	error = ts3client_initClientLib(&callbacks, NULL, LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING, NULL, path);

	if (error != ERROR_ok) {
		char* errormsg;
		if (ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			SharedAPI::Log((char*)((string)"Error initialzing serverlib: " + string(errormsg)).c_str());
			ts3client_freeMemory(errormsg);
		}
		return false;
	}

	/* Spawn a new server connection handler using the default port and store the server ID */
	if ((error = ts3client_spawnNewServerConnectionHandler(0, &scHandlerID)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error spawning server connection handler: " + to_string(error)).c_str());
		return false;
	}

	/* Open default capture device */
	/* Passing empty string for mode and NULL or empty string for device will open the default device */
	if ((error = ts3client_openCaptureDevice(scHandlerID, "", NULL)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error opening capture device: " + to_string(error)).c_str());
	}

	/* Open default playback device */
	/* Passing empty string for mode and NULL or empty string for device will open the default device */
	if ((error = ts3client_openPlaybackDevice(scHandlerID, "", NULL)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error opening playback device: " + to_string(error)).c_str());
	}

	/* Create a new client identity */
	/* In your real application you should do this only once, store the assigned identity locally and then reuse it. */
	if ((error = ts3client_createIdentity(&identity_)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error creating identity: " + to_string(error)).c_str());
		return false;
	}

	/* Connect to server on localhost:9987 with nickname "client", no default channel, no default channel password and server password "secret" */
	if ((error = ts3client_startConnection(scHandlerID, identity_, ipAddr, port, username, NULL, "", "secret")) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error connecting to server: " + to_string(error)).c_str());
		return false;
	}

	ts3client_freeMemory(identity_);  /* Release dynamically allocated memory */
	identity_ = "";

	SharedAPI::Log("Client lib initialized and running");

	/* Query and print client lib version */
	if ((error = ts3client_getClientLibVersion(&version)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Failed to get clientlib version: " + to_string(error)).c_str());
		return false;
	}

	SharedAPI::Log((char*)((string)"Client lib version: " + string(version)).c_str());
	
	ts3client_freeMemory(version);  /* Release dynamically allocated memory */
	version = "";

	SLEEP(500);

	return true;
}

bool VoIPClient::StopClient()
{
	char* formattedMessage = "";
	/* Disconnect from server */
	if ((error = ts3client_stopConnection(scHandlerID, "leaving")) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error stopping connection: " + to_string(error)).c_str());
		return false;
	}

	SLEEP(200);

	/* Destroy server connection handler */
	if ((error = ts3client_destroyServerConnectionHandler(scHandlerID)) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Error destroying clientlib: " + to_string(error)).c_str());
		return false;
	}

	/* Shutdown client lib */
	if ((error = ts3client_destroyClientLib()) != ERROR_ok) {
		SharedAPI::Log((char*)((string)"Failed to destroy clientlib: " + to_string(error)).c_str());
		return false;
	}

	return true;
}
