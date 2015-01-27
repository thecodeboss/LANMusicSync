#ifndef WavFile_h__
#define WavFile_h__

// @TODO:
// There are assumptions made here about endians. Once more platforms are
// supported, I'll need to surround these with ifdefs.

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
struct WavHeader {
	WavFormat_t Format;
	unsigned short NumChannels;
	unsigned SampleRate;
	unsigned AvgBitsPerSecond;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
};
#pragma pack(pop)

#endif // WavFile_h__
