#ifndef StreamingSource_h__
#define StreamingSource_h__

#include "AudioSource.h"
#include <queue>
#define BUFFER_SIZE 512
typedef DWORD Sample;

class StreamingSource : public AudioSource {
	std::queue<Sample[BUFFER_SIZE]> m_Buffer;
};

#endif // StreamingSource_h__