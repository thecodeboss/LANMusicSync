#ifndef AudioDevice_h__
#define AudioDevice_h__

#include <windows.h>
#include <xaudio2.h>
#include "AudioSource.h"

#ifdef WINDOWS
	DWORD WINAPI StreamProc(LPVOID pContext);
#endif

class AudioDevice {
#ifdef WINDOWS
	static IXAudio2* XAudio2;
	static IXAudio2MasteringVoice* XAudio2MasteringVoice;
#endif
	AudioSource* m_AudioSource;
public:
	bool Init();
	bool Play();
	void SetAudioSource(AudioSource* audioSource);
	AudioSource* GetAudioSource();

#ifdef WINDOWS
	DWORD WINAPI StreamThreadMain(AudioSource* source);
#endif
};

struct AudioStreamContext
{
	AudioDevice* audioDevice;
	AudioSource* Source;
	AudioStreamContext(AudioDevice* x, AudioSource* s) : audioDevice(x), Source(s) {}
};

#endif // AudioDevice_h__
