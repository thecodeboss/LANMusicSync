#include "WavFile.h"
#include "Debug.h"

// Prototypes
char * LoadEntireFile(std::string fileName, size_t& fileSize);

WavFile::WavFile() : m_DataSize(0), m_FileSize(0), m_RawData(nullptr) {}

WavFile::WavFile(std::string fileName)
{
	LoadFile(fileName);
}

WavFile::~WavFile()
{
	delete[] m_RawData;
}

bool WavFile::LoadFile(std::string fileName)
{
	// @TODO:
	// Probably shouldn't load the entire file at once - this could be wasteful.
	char * RawFile = LoadEntireFile(fileName, m_FileSize);
	if (RawFile == nullptr) {
		ConsolePrintf("Failed to load wave file.");
		return false;
	}

	size_t bytesRead = 0;
	while (bytesRead + sizeof(WavChunk) <= m_FileSize) {
		WavChunk chunk;
		memcpy(&chunk, RawFile + bytesRead, sizeof(WavChunk));
		bytesRead += sizeof(WavChunk);
		switch (chunk.Type)
		{
		case WavChunk_t::RiffHeader:
			WavChunk_t RiffType;
			memcpy(&RiffType, RawFile + bytesRead, sizeof(WavChunk_t));
			bytesRead += sizeof(WavChunk_t);
			if (RiffType != WavChunk_t::WavRiff) {
				ConsolePrintf("Not a wave file.");
				return false;
			}
			break;
		case WavChunk_t::Format:
			if (chunk.DataSize >= sizeof(WavHeader))
			{
				memcpy(&m_Format, RawFile + bytesRead, sizeof(WavHeader));
				// There could be extra bits, but we skip them.
				bytesRead += chunk.DataSize;
			}
			break;
		case WavChunk_t::Data:
			m_RawData = new char[chunk.DataSize];
			m_DataSize = chunk.DataSize;
			memcpy(m_RawData, RawFile + bytesRead, chunk.DataSize);
			bytesRead += chunk.DataSize;
			break;
		default:
			bytesRead += chunk.DataSize;
			break;
		}
	}

	// Done with the file, delete it since we've copied all the wav file
	// data that we will need
	delete[] RawFile;
	RawFile = nullptr;

	return true;
}

size_t WavFile::GetDataSize()
{
	return m_DataSize;
}

char * WavFile::GetRawData()
{
	return m_RawData;
}

WavHeader* WavFile::GetFormat()
{
	return &m_Format;
}

void WavFile::SetFormat(WavHeader * wavFormat)
{
	memcpy(&m_Format, wavFormat, sizeof(WavHeader));
}

char * LoadEntireFile(std::string fileName, size_t& fileSize)
{
	std::streampos size = 0;
	char * memblock = nullptr;

	std::ifstream file(fileName, std::ios::in|std::ios::binary|std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		fileSize = static_cast<size_t>(size);
		memblock = new char [fileSize];
		file.seekg (0, std::ios::beg);
		file.read (memblock, size);
		file.close();
	}
	else ConsolePrintf("File could not be opened.");

	return memblock;
}