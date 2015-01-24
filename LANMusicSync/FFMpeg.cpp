#include "FFMpeg.h"
#include <windows.h>

FFMpeg::FFMpeg()
{
	avcodec_register_all();
}
