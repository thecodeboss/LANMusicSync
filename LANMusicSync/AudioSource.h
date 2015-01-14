#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"
#include "WavFile.h"

class AudioSource {
protected:
	std::queue<Buffer> m_AudioData;
public:
	virtual Buffer GetBuffer();
	virtual Buffer PeekBuffer();
	virtual void PutBuffer(Buffer b);
	void LoadWavFile(WavFile* wavFile);
};

#endif // AudioSource_h__
