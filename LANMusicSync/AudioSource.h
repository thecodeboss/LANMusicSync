#ifndef AudioSource_h__
#define AudioSource_h__

#include <queue>
#define BUFFER_SIZE 512
typedef unsigned long Sample_t;

struct Buffer {
	unsigned long BufferSize;
	std::vector<Sample_t> Buffer;
};

class AudioSource {
	std::queue<Buffer> m_AudioData;
public:
	virtual Buffer getBuffer();
	virtual Buffer peekBuffer();
};

#endif // AudioSource_h__
