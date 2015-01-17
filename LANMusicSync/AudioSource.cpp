#include "AudioSource.h"
#include "Debug.h"

AudioSource::AudioSource() : m_bActive(false), m_bPlaying(false), m_Source(nullptr), m_wavFile(nullptr)
{
	m_Mutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex
}


Buffer* AudioSource::GetBuffer()
{
	WaitForSingleObject(m_Mutex, INFINITE);
	Buffer* b = m_AudioData.front();
	m_AudioData.pop();
	ReleaseMutex(m_Mutex);
	return b;
}

Buffer* AudioSource::PeekBuffer()
{
	WaitForSingleObject(m_Mutex, INFINITE);
	Buffer* b = m_AudioData.front();
	ReleaseMutex(m_Mutex);
	return b;
}

void AudioSource::PutBuffer(Buffer* b)
{
	WaitForSingleObject(m_Mutex, INFINITE);
	m_AudioData.push(b);
	ReleaseMutex(m_Mutex);
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

	while (m_bPlaying)
	{
		// Check with XAudio to see if there are any buffers available, and wait if needed
		XAUDIO2_VOICE_STATE state;
		while (m_Source->GetState(&state), state.BuffersQueued >= MAX_BUFFER_COUNT - 1)
		{
			WaitForSingleObject(m_Callback.hBufferEndEvent, INFINITE);
		}

		if (!m_AudioData.size()) break;

		WaitForSingleObject(m_Mutex, INFINITE);

		// Pointer the XAudio buffer at our buffer array
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = BUFFER_SIZE;
		buf.pAudioData = &m_AudioData.front()->front();
		m_AudioData.pop();

		ReleaseMutex(m_Mutex);

		// Submit buffers
		if (!XAudio2CheckedCall(m_Source->SubmitSourceBuffer(&buf)))
		{
			ConsolePrintf(TEXT("XAudio2: Failed to submit source buffers."));
			break;
		}
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
	 // @TODO
	return true;
}

bool AudioSource::Cleanup()
{
	// @TODO
	return true;
}
