#include "AudioSource.h"
#include "Debug.h"
#include "FFMpeg.h"

#ifdef WINDOWS
AudioSource::AudioSource() : m_bActive(false), m_bPlaying(false), m_Source(nullptr), m_SendBufferCount(0)
{
	m_Mutex = CreateMutex(
		nullptr,              // default security attributes
		FALSE,             // initially not owned
		nullptr);             // unnamed mutex
}
#else
AudioSource::AudioSource() : m_bActive(false), m_bPlaying(false), m_SendBufferCount(0)
{
	// @TODO
}
#endif

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
	memcpy(format, &m_Format, sizeof(WavHeader));
	format->cbSize = 0; // Extra bits not in our header
	return format;
}

void AudioSource::SetWavFormat(WavHeader * wavFormat)
{
	memcpy(&m_Format, wavFormat, sizeof(WavHeader));
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

	std::deque<Buffer*> buffersForDelete;

	while (m_bPlaying)
	{
		// Check with XAudio to see if there are any buffers available, and wait if needed
		XAUDIO2_VOICE_STATE state;
		while (m_Source->GetState(&state), state.BuffersQueued >= MAX_BUFFER_COUNT - 1)
		{
			WaitForSingleObject(m_Callback.hBufferEndEvent, INFINITE);
		}

		if (!GetNumBuffers()) continue;

		if (buffersForDelete.size() > MAX_BUFFER_COUNT + 1) {
			delete buffersForDelete.front();
			buffersForDelete.pop_front();
		}

		WaitForSingleObject(m_Mutex, INFINITE);

		// Pointer the XAudio buffer at our buffer array
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = m_AudioData.front()->size();
		buf.pAudioData = &m_AudioData.front()->front();
		buffersForDelete.push_back(m_AudioData.front());
		m_AudioData.pop_front();
		m_SendBufferCount--;

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

	FFMpeg* ffmpeg = new FFMpeg();
	ffmpeg->LoadFile(fileName);

	Buffer *b = nullptr;
	bool bGotWavFormat = false;
	while (b = ffmpeg->GetFrame()) {
		if (b->size() % 4 != 0) continue;
		AppendBuffer(b);
		if (!bGotWavFormat) {
			SetWavFormat((WavHeader*)ffmpeg->GetWavFormat());
			bGotWavFormat = true;
		}

		// Don't read the whole file. Wait for data to be played first.
		while (GetNumBuffers() > 100) {
			Sleep(10);
		}
	}

	//delete ffmpeg;

	CoUninitialize();

	return 0;
}
