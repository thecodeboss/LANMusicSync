#ifndef FFMpeg_h__
#define FFMpeg_h__
#include <string>
#include <windows.h>
#include "Buffer.h"
#include "WavFormat.h"

extern "C" 
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/avutil.h"
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

class FFMpeg {
	AVFrame* frame;
	AVFormatContext* formatContext;
	AVStream* audioStream;
	AVCodecContext* codecContext;
	AVPacket readingPacket;

	WavHeader WavFormat;
	bool bWavFormatReady;
public:
	FFMpeg();
	void LoadFile( std::string fileName );
	Buffer* GetFrame();
	~FFMpeg();
	WAVEFORMATEX* GetWavFormat();
	void setupWavFormat();
};

#endif // FFMpeg_h__
