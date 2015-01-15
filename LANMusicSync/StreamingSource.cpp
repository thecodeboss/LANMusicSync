#include "StreamingSource.h"


Buffer* StreamingSource::GetBuffer()
{
	// Do network stuff to populate buffer

	// Then parent handles getBuffer as usual for playback
	return AudioSource::GetBuffer();
}

Buffer* StreamingSource::PeekBuffer()
{
	return new Buffer();
}

void StreamingSource::Synchronize(size_t numBuffers)
{

}
