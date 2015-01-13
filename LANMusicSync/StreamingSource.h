#ifndef StreamingSource_h__
#define StreamingSource_h__

#include "AudioSource.h"

class StreamingSource : public AudioSource {
	bool m_isServer;
public:
	virtual Buffer getBuffer();
	virtual Buffer peekBuffer();
};

#endif // StreamingSource_h__