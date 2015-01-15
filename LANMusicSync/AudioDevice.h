#ifndef AudioDevice_h__
#define AudioDevice_h__

#include <windows.h>
#include <xaudio2.h>
#include "AudioSource.h"

DWORD WINAPI StreamProc(LPVOID pContext);

class AudioDevice {
	static IXAudio2* XAudio2;
	static IXAudio2MasteringVoice* XAudio2MasteringVoice;
	AudioSource* m_AudioSource;
public:
	bool Init();
	bool Play();
	void SetAudioSource(AudioSource* audioSource);

	DWORD WINAPI StreamThreadMain(AudioSource* source);
};

struct AudioStreamContext
{
	AudioDevice* audioDevice;
	AudioSource* Source;
	AudioStreamContext(AudioDevice* x, AudioSource* s) : audioDevice(x), Source(s) {}
};

#endif // AudioDevice_h__
