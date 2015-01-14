#include "StreamingSource.h"


Buffer StreamingSource::getBuffer()
{
	// Do network stuff to populate buffer

	// Then parent handles getBuffer as usual for playback
	return AudioSource::getBuffer();
}

Buffer StreamingSource::peekBuffer()
{
	return Buffer();
}

void StreamingSource::synchronize(size_t numBuffers)
{

}
