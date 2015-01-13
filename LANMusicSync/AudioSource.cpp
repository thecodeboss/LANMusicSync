#include "AudioSource.h"


Buffer AudioSource::getBuffer()
{
	Buffer b = m_AudioData.front();
	m_AudioData.pop();
	return b;
}

Buffer AudioSource::peekBuffer()
{
	return m_AudioData.front();
}

void AudioSource::putBuffer(Buffer b)
{
	m_AudioData.push(b);
}
