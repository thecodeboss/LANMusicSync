#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Client.h"
#include "Server.h"
#include "AudioDevice.h"

// Format:
// ./LANMusicSync client 192.168.0.100 1234
// ./LANMusicSync server 1234

int main(int argc, char** argv) {
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Initialize audio
	AudioDevice* audioDevice = new AudioDevice();
	audioDevice->Init();

	// Set up the streaming audio source
	AudioSource * audioSource = new AudioSource();
	audioDevice->SetAudioSource(audioSource);

	// Run application
	if (argc >= 3) {
		if (!strcmp("client", argv[1])) {
			// Start a client
			Client c;
			c.setAudioDevice(audioDevice);
			c.Connect(argv[2], argv[3]);
		}
		else if (!strcmp("server", argv[1])) {
			// Load the audio file
			audioSource->StreamFromFile(argv[3]);

			// Start a server
			Server s;
			s.setAudioDevice(audioDevice);
			s.Start(argv[2]);
		}
	}

	delete audioDevice;
	return 0;
}
