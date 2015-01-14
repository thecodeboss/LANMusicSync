#include "WavFile.h"
#include "Debug.h"

// Prototypes
char * LoadEntireFile(std::string fileName, size_t& fileSize);

WavFile::~WavFile()
{
	delete[] m_RawData;
}

bool WavFile::LoadFile(std::string fileName)
{
	// @TODO:
	// Probably shouldn't load the entire file at once - this could be wasteful.
	m_RawFile = LoadEntireFile(fileName, m_FileSize);
	if (m_RawFile == nullptr) {
		ConsolePrintf("Failed to load wave file.");
		return false;
	}

	size_t bytesRead = 0;
	while (bytesRead + sizeof(WavChunk) <= m_FileSize) {
		WavChunk chunk;
		memcpy(&chunk, m_RawFile + bytesRead, sizeof(WavChunk));
		bytesRead += sizeof(WavChunk);
		switch (chunk.Type)
		{
		case WavChunk_t::RiffHeader:
			if (chunk.DataSize >= sizeof(WavHeader))
			{
				memcpy(&m_Format, m_RawFile + bytesRead, sizeof(WavHeader));
				// Ignore extra bytes in the header
				bytesRead += sizeof(WavHeader) + m_Format.NumExtraBytes;
			}
			break;
		case WavChunk_t::Data:
			m_RawData = new char[chunk.DataSize];
			memcpy(m_RawData, m_RawFile + bytesRead, chunk.DataSize);
			bytesRead += chunk.DataSize;
			break;
		default:
			bytesRead += chunk.DataSize;
			break;
		}
	}

	// Done with the file, delete it since we've copied all the wav file
	// data that we will need
	delete[] m_RawFile;
	m_RawFile = nullptr;

	return true;
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