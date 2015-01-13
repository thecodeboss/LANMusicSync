#ifndef AudioSource_h__
#define AudioSource_h__

#include <queue>
#define BUFFER_SIZE 512
typedef unsigned long Sample;
typedef std::vector<Sample> Buffer;

class AudioSource {
	std::queue<Buffer> m_AudioData;
public:
	virtual Buffer getBuffer();
	virtual Buffer peekBuffer();
	virtual void putBuffer(Buffer b);
};

#endif // AudioSource_h__
