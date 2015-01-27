#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"
#include "XAudio2VoiceCallback.h"
#include "WavFormat.h"

class AudioSource {
	HANDLE m_Mutex;
protected:
	std::deque<Buffer*> m_AudioData;
	bool m_bActive;
	IXAudio2SourceVoice* m_Source;
	XAudio2VoiceCallback m_Callback;
	WavHeader m_Format;
	bool m_bPlaying;
	size_t m_SendBufferCount;
public:
	AudioSource();
	virtual bool isActive();
	virtual Buffer* PopFront();
	virtual Buffer* GetBufferForSend();
	virtual void AppendBuffer(Buffer* b);
	WAVEFORMATEX * GetWavFormat();
	virtual bool Init(IXAudio2* XAudio2);
	std::string GetName();
	bool Start();
	bool Stop();
	bool Cleanup();
	size_t GetNumBuffers();
	void SetWavFormat(WavHeader * wavFormat);

	bool StreamFromFile(std::string fileName);
	DWORD WINAPI FileStreamThreadMain(std::string fileName);
};

struct FileStreamContext
{
	AudioSource* Source;
	std::string FileName;
	FileStreamContext(AudioSource* s, std::string fileName) : Source(s), FileName(fileName) {}
};

#endif // AudioSource_h__
