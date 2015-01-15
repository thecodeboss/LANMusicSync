#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"
#include "WavFile.h"
#include "XAudio2VoiceCallback.h"

class AudioSource {
protected:
	std::queue<Buffer*> m_AudioData;
	bool m_bActive;
	IXAudio2SourceVoice* m_Source;
	XAudio2VoiceCallback m_Callback;
	WavFile* m_wavFile;
	bool m_bPlaying;
public:
	AudioSource();
	virtual bool isActive();
	virtual Buffer* GetBuffer();
	virtual Buffer* PeekBuffer();
	virtual void PutBuffer(Buffer* b);
	void LoadWavFile(WavFile* wavFile);
	virtual bool Init(IXAudio2* XAudio2);
	std::string GetName();
	bool Start();
	bool Stop();
	bool Cleanup();
};

#endif // AudioSource_h__
