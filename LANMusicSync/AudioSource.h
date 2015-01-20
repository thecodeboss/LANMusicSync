#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"
#include "WavFile.h"
#include "XAudio2VoiceCallback.h"

class AudioSource {
	HANDLE m_Mutex;
protected:
	std::deque<Buffer*> m_AudioData;
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
	virtual Buffer* PeekBuffer(int i);
	virtual void PutBuffer(Buffer* b);
	void LoadWavFile(WavFile* wavFile);
	WAVEFORMATEX * GetWavFormat();
	virtual bool Init(IXAudio2* XAudio2);
	std::string GetName();
	bool Start();
	bool Stop();
	bool Cleanup();
	size_t GetNumBuffers();
	void SetWavFormat(WavHeader * wavFormat);
	// @TODO: Need to straighten out with 'GetBufferForSend' and 'SentBuffer(i)'
	// or something similar.
};

#endif // AudioSource_h__
