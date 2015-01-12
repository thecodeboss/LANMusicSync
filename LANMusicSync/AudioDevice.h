#ifndef AudioDevice_h__
#define AudioDevice_h__

#include <windows.h>
#include <xaudio2.h>
#include "AudioSource.h"

class AudioDevice {
	static IXAudio2* XAudio2;
	static IXAudio2MasteringVoice* XAudio2MasteringVoice;
public:
	bool Init();
};

#endif // AudioDevice_h__
