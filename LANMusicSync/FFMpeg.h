#ifndef FFMpeg_h__
#define FFMpeg_h__

extern "C" 
{
	#include "libavcodec/avcodec.h"
}
#pragma comment(lib, "avcodec.lib")

class FFMpeg {
public:
	FFMpeg();
};

#endif // FFMpeg_h__
