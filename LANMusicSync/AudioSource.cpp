#include "AudioSource.h"
#include "Debug.h"

AudioSource::AudioSource() : m_bActive(false), m_bPlaying(false), m_Source(nullptr), m_wavFile(nullptr), m_SendBufferCount(0)
{
	m_Mutex = CreateMutex(
		nullptr,              // default security attributes
		FALSE,             // initially not owned
		nullptr);             // unnamed mutex
}

Buffer* AudioSource::PopFront()
{
	WaitForSingleObject(m_Mutex, INFINITE);
	Buffer* b = m_AudioData.front();
	m_AudioData.pop_front();
	m_SendBufferCount--;
	ReleaseMutex(m_Mutex);
	return b;
}

Buffer* AudioSource::GetBufferForSend()
{
	WaitForSingleObject(m_Mutex, INFINITE);
	if (m_SendBufferCount >= m_AudioData.size()) {
		ReleaseMutex(m_Mutex);
		return nullptr;
	}
	Buffer* b = m_AudioData.at(m_SendBufferCount);
	m_SendBufferCount++;
	ReleaseMutex(m_Mutex);
	return b;
}

void AudioSource::AppendBuffer(Buffer* b)
{
	WaitForSingleObject(m_Mutex, INFINITE);
	m_AudioData.push_back(b);
	ReleaseMutex(m_Mutex);
}

void AudioSource::LoadWavFile(WavFile* wavFile)
{
	// @TODO: The last buffer in the file may get cut off due to the logic used here
	size_t bytesRead = 0;
	char * filePtr = wavFile->GetRawData();
	while (wavFile->GetDataSize() - bytesRead >= BUFFER_SIZE) {
		Buffer* b = new Buffer(filePtr, filePtr + BUFFER_SIZE);
		m_AudioData.push_back(b);
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
	WAVEFORMATEX * format = GetWavFormat();
	if (!XAudio2CheckedCall(XAudio2->CreateSourceVoice(&m_Source, format, 0, 2.0f, &m_Callback)))
	{
		ConsolePrintf(TEXT("Failed to create XAudio2 source voice."));
		return false;
	}

	m_bActive = true;

	return true;
}

WAVEFORMATEX * AudioSource::GetWavFormat() {
	WAVEFORMATEX * format = new WAVEFORMATEX();
	memcpy(format, m_wavFile->GetFormat(), sizeof(WavHeader));
	format->cbSize = 0; // Extra bits not in our header
	return format;
}

void AudioSource::SetWavFormat(WavHeader * wavFormat)
{
	if (m_wavFile == nullptr) {
		m_wavFile = new WavFile();
	}
	m_wavFile->SetFormat(wavFormat);
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
			ConsolePrintf("Buffer completed.");
		}

		if (!GetNumBuffers()) continue;

		WaitForSingleObject(m_Mutex, INFINITE);

		// Pointer the XAudio buffer at our buffer array
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = BUFFER_SIZE;
		buf.pAudioData = &m_AudioData.front()->front();
		m_AudioData.pop_front();
		m_SendBufferCount--;

		ReleaseMutex(m_Mutex);

		// Submit buffers
		if (!XAudio2CheckedCall(m_Source->SubmitSourceBuffer(&buf)))
		{
			ConsolePrintf(TEXT("XAudio2: Failed to submit source buffers."));
			break;
		}

		ConsolePrintf(TEXT("Submitted a buffer for playback."));
	}

	// Wait for everything to finish
	XAUDIO2_VOICE_STATE state;
	while (m_Source->GetState(&state), state.BuffersQueued > 0)
	{
		WaitForSingleObjectEx(m_Callback.hBufferEndEvent, INFINITE, TRUE);
	}

	ConsolePrintf(TEXT("Finished playing source (%s)"), GetName());

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

size_t AudioSource::GetNumBuffers()
{
	size_t result = 0;
	WaitForSingleObject(m_Mutex, INFINITE);
	result = m_AudioData.size();
	ReleaseMutex(m_Mutex);
	return result;
}

DWORD WINAPI FileStreamProc(LPVOID pContext)
{
	if (!pContext) return -1;
	return ((FileStreamContext*)pContext)->Source->FileStreamThreadMain(((FileStreamContext*)pContext)->FileName);
}

bool AudioSource::StreamFromFile(std::string fileName)
{
	FileStreamContext* FileContext = new FileStreamContext(this, fileName);

	DWORD dwThreadId = 0;
	HANDLE StreamingVoiceThread = CreateThread(nullptr, 0, FileStreamProc, FileContext, 0, &dwThreadId);
	if(StreamingVoiceThread == nullptr)
	{
		return false;
	}

	return true;
}

DWORD WINAPI AudioSource::FileStreamThreadMain(std::string fileName)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// Do stuff
	ConsolePrintf("Found our way inside the file streaming thread!");

	CoUninitialize();

	return 0;
}
