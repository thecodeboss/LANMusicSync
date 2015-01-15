#include "AudioSource.h"
#include "Debug.h"

AudioSource::AudioSource() : m_bActive(false), m_bPlaying(false), m_Source(nullptr), m_wavFile(nullptr) {}

Buffer* AudioSource::GetBuffer()
{
	Buffer* b = m_AudioData.front();
	m_AudioData.pop();
	return b;
}

Buffer* AudioSource::PeekBuffer()
{
	return m_AudioData.front();
}

void AudioSource::PutBuffer(Buffer* b)
{
	m_AudioData.push(b);
}

void AudioSource::LoadWavFile(WavFile* wavFile)
{
	// @TODO: The last buffer in the file may get cut off due to the logic used here
	size_t bytesRead = 0;
	char * filePtr = wavFile->GetRawData();
	while (wavFile->GetDataSize() - bytesRead >= BUFFER_SIZE) {
		Buffer* b = new Buffer(filePtr, filePtr + BUFFER_SIZE);
		m_AudioData.push(b);
		filePtr += BUFFER_SIZE;
		bytesRead += BUFFER_SIZE;
	}
	m_wavFile = wavFile;
}

bool AudioSource::isActive()
{
	return m_bActive;
}

bool AudioSource::Init(IXAudio2* XAudio2)
{
	WAVEFORMATEX * format = new WAVEFORMATEX();
	memcpy(format, m_wavFile->GetFormat(), sizeof(WavHeader));
	format->cbSize = 0; // Extra bits not in our header
	if (!XAudio2CheckedCall(XAudio2->CreateSourceVoice(&m_Source, format, 0, 2.0f, &m_Callback)))
	{
		ConsolePrintf(TEXT("Failed to create XAudio2 source voice."));
		return false;
	}

	m_bActive = true;

	return true;
}

std::string AudioSource::GetName()
{
	return "Default name";
}

bool AudioSource::Start()
{
	if (m_bPlaying) return false;
	m_bPlaying = true;

	if (m_Source == nullptr) return false;

	if (!XAudio2CheckedCall(m_Source->Start(0, 0)))
	{
		return false;
	}

	unsigned CurrentDiskReadBuffer = 0;
	unsigned CurrentPosition = 0;
	unsigned BufferSize = BUFFER_SIZE;
	while (m_bPlaying)
	{
		// Check with XAudio to see if there are any buffers available, and wait if needed
		XAUDIO2_VOICE_STATE state;
		while (m_Source->GetState(&state), state.BuffersQueued >= MAX_BUFFER_COUNT - 1)
		{
			WaitForSingleObject(m_Callback.hBufferEndEvent, INFINITE);
		}

		// Pointer the XAudio buffer at our buffer array
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = BufferSize;
		buf.pAudioData = &m_AudioData.front()->front();
		m_AudioData.pop();

		// Submit buffers
		if (!XAudio2CheckedCall(m_Source->SubmitSourceBuffer(&buf)))
		{
			ConsolePrintf(TEXT("XAudio2: Failed to submit source buffers."));
			break;
		}

		CurrentDiskReadBuffer++;
		CurrentDiskReadBuffer %= MAX_BUFFER_COUNT;
		CurrentPosition++;
	}

	// Wait for everything to finish
	XAUDIO2_VOICE_STATE state;
	while (m_Source->GetState(&state), state.BuffersQueued > 0)
	{
		WaitForSingleObjectEx(m_Callback.hBufferEndEvent, INFINITE, TRUE);
	}

	ConsolePrintf(TEXT("Finished playing procedural source voice (%s)"), GetName());

	return S_OK;
}

bool AudioSource::Stop()
{
	throw std::exception("The method or operation is not implemented.");
}

bool AudioSource::Cleanup()
{
	throw std::exception("The method or operation is not implemented.");
}
