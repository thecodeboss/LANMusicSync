#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"
#include "WavFormat.h"

#ifdef WINDOWS
#include <windows.h>
#include "XAudio2VoiceCallback.h"
#endif

class AudioSource {

#ifdef WINDOWS
	HANDLE m_Mutex;
#endif

protected:

#ifdef WINDOWS
	IXAudio2SourceVoice* m_Source;
	XAudio2VoiceCallback m_Callback;
#endif

	std::deque<Buffer*> m_AudioData;
	bool m_bActive;
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
	std::string GetName();
	bool Start();
	bool Stop();
	bool Cleanup();
	size_t GetNumBuffers();
	void SetWavFormat(WavHeader * wavFormat);

	bool StreamFromFile(std::string fileName);

#ifdef WINDOWS
	DWORD WINAPI FileStreamThreadMain(std::string fileName);
	virtual bool Init(IXAudio2* XAudio2);
#endif
};

struct FileStreamContext
{
	AudioSource* Source;
	std::string FileName;
	FileStreamContext(AudioSource* s, std::string fileName) : Source(s), FileName(fileName) {}
};

#endif // AudioSource_h__
