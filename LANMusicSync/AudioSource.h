#ifndef AudioSource_h__
#define AudioSource_h__

#include "Buffer.h"

class AudioSource {
	std::queue<Buffer> m_AudioData;
public:
	virtual Buffer getBuffer();
	virtual Buffer peekBuffer();
	virtual void putBuffer(Buffer b);
};

#endif // AudioSource_h__
