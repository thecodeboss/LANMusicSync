#ifndef Server_h__
#define Server_h__
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

class Server
{
	SOCKET m_ListenSocket;
	SOCKET m_ClientSocket;
public:
	int Start(char* port);
};

#endif // Server_h__
