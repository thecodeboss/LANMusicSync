#ifndef Server_h__
#define Server_h__
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AudioDevice.h"

class Server
{
	AudioDevice* m_AudioDevice;
	SOCKET m_ListenSocket;
	SOCKET m_ClientSocket;
public:
	int Start(char* port);
	void setAudioDevice(AudioDevice* audioDevice);
};

#endif // Server_h__
