#ifndef Client_h__
#define Client_h__
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

class Client
{
	SOCKET m_ConnectSocket;
public:
	int Connect(char* server, char* port);
};

#endif // Client_h__
