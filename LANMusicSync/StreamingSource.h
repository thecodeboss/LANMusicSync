#ifndef StreamingSource_h__
#define StreamingSource_h__

#include "AudioSource.h"

class StreamingSource : public AudioSource {
	bool m_isServer;

	/*
	 * Used by the server to synchronize data with the clients.
	 * @param  numBuffers
	 */
	void synchronize(size_t numBuffers);

public:
	virtual Buffer getBuffer();
	virtual Buffer peekBuffer();
};

#endif // StreamingSource_h__