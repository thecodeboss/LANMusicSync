#include "AudioSource.h"


Buffer AudioSource::GetBuffer()
{
	Buffer b = m_AudioData.front();
	m_AudioData.pop();
	return b;
}

Buffer AudioSource::PeekBuffer()
{
	return m_AudioData.front();
}

void AudioSource::PutBuffer(Buffer b)
{
	m_AudioData.push(b);
}

void AudioSource::LoadWavFile(WavFile* wavFile)
{
	// @TODO: The last buffer in the file may get cut off due to the logic used here
	size_t bytesRead = 0;
	char * filePtr = wavFile->GetRawData();
	while (wavFile->GetDataSize() - bytesRead >= BUFFER_SIZE) {
		Buffer b(filePtr, filePtr + BUFFER_SIZE);
		m_AudioData.push(b);
		filePtr += BUFFER_SIZE;
		bytesRead += BUFFER_SIZE;
	}
}
