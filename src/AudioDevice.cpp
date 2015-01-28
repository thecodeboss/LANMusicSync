#include "AudioDevice.h"
#include "Debug.h"

IXAudio2* AudioDevice::XAudio2;
IXAudio2MasteringVoice* AudioDevice::XAudio2MasteringVoice;

#ifdef WINDOWS
DWORD WINAPI StreamProc( LPVOID pContext )
{
	if (!pContext) return -1;
	return ((AudioStreamContext*)pContext)->audioDevice->StreamThreadMain(((AudioStreamContext*)pContext)->Source);
}
#endif

bool AudioDevice::Init()
{
#ifdef WINDOWS
	// Set thread concurrency model
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Initialize the XAudio2 instance
	if (XAudio2Create(&XAudio2, XAUDIO2_DEBUG_ENGINE, XAUDIO2_DEFAULT_PROCESSOR) != S_OK)
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
#endif

	return true;
}

void AudioDevice::SetAudioSource(AudioSource* audioSource)
{
	m_AudioSource = audioSource;
}

bool AudioDevice::Play()
{
	AudioStreamContext* StreamContext = new AudioStreamContext(this, m_AudioSource);

#ifdef WINDOWS
	DWORD dwThreadId = 0;
	HANDLE StreamingVoiceThread = CreateThread( NULL, 0, StreamProc, StreamContext, 0, &dwThreadId );
	if( StreamingVoiceThread == NULL )
	{
		return false;
	}
#else
	return false;
#endif

	return true;
}

#ifdef WINDOWS
DWORD WINAPI AudioDevice::StreamThreadMain( AudioSource* source )
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (!source->isActive())
	{
		source->Init(XAudio2);
	}

	ConsolePrintf("Starting audio source (%s)", source->GetName().c_str());
	source->Start();

	ConsolePrintf("Stopping audio source (%s)", source->GetName().c_str());
	source->Stop();

	ConsolePrintf("Cleaning up audio source (%s)", source->GetName().c_str());
	source->Cleanup();

	ConsolePrintf("Audio source cleaned up (%s)", source->GetName().c_str());

	CoUninitialize();

	return 0;
}
#endif

AudioSource* AudioDevice::GetAudioSource()
{
	return m_AudioSource;
}
