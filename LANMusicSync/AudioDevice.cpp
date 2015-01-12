#include "AudioDevice.h"
#include "Debug.h"

IXAudio2* AudioDevice::XAudio2;
IXAudio2MasteringVoice* AudioDevice::XAudio2MasteringVoice;

bool AudioDevice::Init()
{
	// Set thread concurrency model
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Initialize the XAudio2 instance
	if (XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR) != S_OK)
	{
		ConsolePrintf(TEXT("Failed to initialize XAudio2."));
		return false;
	}

	// Initialize the mastering voice to handle all audio
	if (XAudio2->CreateMasteringVoice(&XAudio2MasteringVoice) != S_OK)
	{
		ConsolePrintf(TEXT("Failed to create XAudio2 mastering voice."));
		return false;
	}

	return true;
}
