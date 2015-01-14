#ifndef WavFile_h__
#define WavFile_h__

#include <string>
#include <iostream>
#include <fstream>

// @TODO:
// There are assumptions made here about endians. Once more platforms are
// supported, I'll need to surround these with ifdefs.

enum WavChunk_t : unsigned {
	RiffHeader = 0x46464952,
	WavRiff = 0x45564157,
	Format = 0x020746d66,
	LabeledText = 0x478747C6,
	Instrumentation = 0x478747C6,
	Sample = 0x6C706D73,
	Fact = 0x47361666,
	Data = 0x61746164,
	Junk = 0x4b4e554a,
};

enum WavFormat_t : unsigned short {
	PulseCodeModulation = 0x01,
	IEEEFloatingPoint = 0x03,
	ALaw = 0x06,
	MuLaw = 0x07,
	IMAADPCM = 0x11,
	YamahaITUG723ADPCM = 0x16,
	GSM610 = 0x31,
	ITUG721ADPCM = 0x40,
	MPEG = 0x50,
	Extensible = 0xFFFE
};

#pragma pack(push, 1)
struct WavChunk {
	WavChunk_t Type;
	unsigned DataSize;
};

struct WavHeader {
	WavFormat_t Format;
	unsigned short NumChannels;
	unsigned SampleRate;
	unsigned AvgBitsPerSecond;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
};
#pragma pack(pop)

class WavFile {
	WavHeader m_Format;
	size_t m_FileSize;
	size_t m_DataSize;
	char * m_RawData;

public:
	virtual ~WavFile();
	WavFile();
	WavFile(std::string fileName);
	bool LoadFile(std::string fileName);
	size_t GetDataSize();
	char * GetRawData();
};

#endif // WavFile_h__
