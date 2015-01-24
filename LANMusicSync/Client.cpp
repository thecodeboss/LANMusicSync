#include "Client.h"
#include "Debug.h"
#include "Buffer.h"

int Client::Connect( char* server, char* port )
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(server, port, &hints, &result);
	if (getaddrinfo(server, port, &hints, &result) != 0) {
		ConsolePrintf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	m_ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr=result;

	// Create a SOCKET for connecting to server
	m_ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	if (m_ConnectSocket == INVALID_SOCKET) {
		ConsolePrintf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	iResult = connect( m_ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		ConsolePrintf("connect failed: %d\n", WSAGetLastError());
		closesocket(m_ConnectSocket);
		m_ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (m_ConnectSocket == INVALID_SOCKET) {
		ConsolePrintf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	int ConnectID = 13;
	iResult = send(m_ConnectSocket, (char*)&ConnectID, (int) sizeof(int), 0);
	if (iResult == SOCKET_ERROR) {
		ConsolePrintf("send failed: %d\n", WSAGetLastError());
		closesocket(m_ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(m_ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		ConsolePrintf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(m_ConnectSocket);
		WSACleanup();
		return 1;
	}

	bool bWavFormatReceived = false;
	bool bPlaying = false;

	// Receive data until the server closes the connection
	int recvbuflen = BUFFER_SIZE * 16;
	unsigned char tempBuffer[BUFFER_SIZE * 16];
	do {
		
		iResult = recv(m_ConnectSocket, (char*)&tempBuffer[0], recvbuflen, MSG_WAITALL);
		Buffer* recvbuf = new Buffer(tempBuffer, tempBuffer + iResult);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());

		if (!bWavFormatReceived) {
			// Then the first buffer is the wave format
			WavHeader test = *(WavHeader *)&recvbuf->front();
			m_AudioDevice->GetAudioSource()->SetWavFormat(&test);
			bWavFormatReceived = true;
			continue;
		}

		m_AudioDevice->GetAudioSource()->AppendBuffer(recvbuf);
		ConsolePrintf(TEXT("Received a buffer."));
		// If we aren't playing and we now have enough buffers to being, start playing.
		// Otherwise, keep receiving more buffers.
		if (!bPlaying && m_AudioDevice->GetAudioSource()->GetNumBuffers() >= MIN_BUFFER_COUNT) {
			// Start audio playback
			m_AudioDevice->Play();
			bPlaying = true;
		}
	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(m_ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		ConsolePrintf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(m_ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(m_ConnectSocket);
	WSACleanup();

	return 0;
}

void Client::setAudioDevice(AudioDevice* audioDevice)
{
	m_AudioDevice = audioDevice;
}
