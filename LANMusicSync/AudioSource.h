#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"

class AudioSource {
protected:
	std::queue<Buffer> m_AudioData;
public:
	virtual Buffer getBuffer();
	virtual Buffer peekBuffer();
	virtual void putBuffer(Buffer b);
	void loadWavFile(std::string fileName);
};

#endif // AudioSource_h__
