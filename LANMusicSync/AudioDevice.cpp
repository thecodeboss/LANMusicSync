#include "AudioDevice.h"
#include "Debug.h"

IXAudio2* AudioDevice::XAudio2;
IXAudio2MasteringVoice* AudioDevice::XAudio2MasteringVoice;

DWORD WINAPI StreamProc( LPVOID pContext )
{
	if (!pContext) return -1;
	return ((AudioStreamContext*)pContext)->audioDevice->StreamThreadMain(((AudioStreamContext*)pContext)->Source);
}

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

void AudioDevice::SetAudioSource(AudioSource* audioSource)
{
	m_AudioSource = audioSource;
}

bool AudioDevice::Play()
{
	AudioStreamContext* StreamContext = new AudioStreamContext(this, m_AudioSource);

	DWORD dwThreadId = 0;
	HANDLE StreamingVoiceThread = CreateThread( NULL, 0, StreamProc, StreamContext, 0, &dwThreadId );
	if( StreamingVoiceThread == NULL )
	{
		return false;
	}

	return true;
}

DWORD WINAPI AudioDevice::StreamThreadMain( AudioSource* source )
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (!source->isActive())
	{
		source->Init(XAudio2);
	}

	ConsolePrintf("Starting XAudio2 source (%s)", source->GetName());
	source->Start();

	ConsolePrintf("Stopping XAudio2 source (%s)", source->GetName());
	source->Stop();

	ConsolePrintf("Cleaning up XAudio2 source (%s)", source->GetName());
	source->Cleanup();

	ConsolePrintf("XAudio2 source cleaned up (%s)", source->GetName());

	CoUninitialize();

	return 0;
}

AudioSource* AudioDevice::GetAudioSource()
{
	return m_AudioSource;
}
