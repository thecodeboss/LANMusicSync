#include "Server.h"
#include "Buffer.h"
#include <queue>
#include <xutility>

// @TODO: Ifdef windows

int Server::Start( char* port )
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	m_ListenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections
	m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (m_ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind( m_ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(m_ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if ( listen( m_ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(m_ListenSocket);
		WSACleanup();
		return 1;
	}

	char recvbuf[BUFFER_SIZE];
	int recvbuflen = BUFFER_SIZE;

	m_ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	m_ClientSocket = accept(m_ListenSocket, NULL, NULL);
	if (m_ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(m_ListenSocket);
		WSACleanup();
		return 1;
	}

	while (m_AudioDevice->GetAudioSource()->GetNumBuffers() < 10) {
		// Wait for the file to be loaded
		Sleep(1);
	}

	// Create a copy of the first few buffers to send to the client, putting the wave
	// format information in the very first buffer
	size_t numBuffers = m_AudioDevice->GetAudioSource()->GetNumBuffers();
	std::queue<Buffer*> sendQueue;

	// Copy the wave format
	Buffer* format = new Buffer(sizeof(WAVEFORMATEX));
	memcpy(format->data(), m_AudioDevice->GetAudioSource()->GetWavFormat(), sizeof(WAVEFORMATEX));
	sendQueue.push(format);

	// Then put some buffers in the send queue
	for (unsigned i = 0; i < min(MAX_BUFFER_COUNT, numBuffers); i++) {
		sendQueue.push(m_AudioDevice->GetAudioSource()->GetBufferForSend());
	}
	Buffer* lastBufferSent = nullptr;

	// Start audio playback
	m_AudioDevice->Play();
	bool bEnd = false;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(m_ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			while (1) {
				lastBufferSent = sendQueue.front();
				int iSendResult = send(m_ClientSocket, (const char *)&lastBufferSent->front(), lastBufferSent->size(), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(m_ClientSocket);
					WSACleanup();
					return 1;
				}
				printf("Bytes sent: %d\n", iSendResult);

				sendQueue.pop();
				while (sendQueue.size() < MIN_BUFFER_COUNT && !bEnd) {
					Buffer* nextBuffer = m_AudioDevice->GetAudioSource()->GetBufferForSend();
					if (nextBuffer == nullptr) {
						// No buffers are ready, let's try waiting a bit for some.
						Sleep(200);
						nextBuffer = m_AudioDevice->GetAudioSource()->GetBufferForSend();
						if (nextBuffer == nullptr) {
							// Okay there probably aren't any more buffers to send, let's break.
							bEnd = true;
							break;
						}
					}
					sendQueue.push(nextBuffer);
				}

				if (bEnd && !sendQueue.size()) break;
			}
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(m_ClientSocket);
			WSACleanup();
			return 1;
		}

		if (bEnd) break;

	} while (iResult > 0);

	while (1) {
		Sleep(10);
	}

	return 0;
}

void Server::setAudioDevice(AudioDevice* audioDevice)
{
	m_AudioDevice = audioDevice;
}

