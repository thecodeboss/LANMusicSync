#ifndef Debug_h__
#define Debug_h__

#ifdef _WINDOWS_
#include <windows.h>
#include <xaudio2.h>
#else
#include <cstdarg>
typedef const char * LPTSTR;
#endif

#include <string>

// Outputs to the Visual Studio console
static void ConsolePrintf(LPTSTR fmt, ...)
{
#if _DEBUG
	va_list vl;
	char szBuf[256];

	va_start(vl, fmt);
	vsprintf_s(szBuf, fmt, vl);
	va_end(vl);

	OutputDebugString(szBuf);
	OutputDebugString(TEXT("\r\n"));
#endif
}

#ifdef _WINDOWS_
static bool XAudio2CheckedCall(HRESULT ErrorCode)
{
	if (ErrorCode != S_OK)
	{
		switch (ErrorCode)
		{
		case XAUDIO2_E_INVALID_CALL:
			ConsolePrintf(TEXT("XAUDIO2_E_INVALID_CALL: API call or one of its arguments was illegal"));
			break;
		case XAUDIO2_E_XMA_DECODER_ERROR:
			ConsolePrintf(TEXT("XAUDIO2_E_XMA_DECODER_ERROR: The XMA hardware suffered an unrecoverable error"));
			break;
		case XAUDIO2_E_XAPO_CREATION_FAILED:
			ConsolePrintf(TEXT("XAUDIO2_E_XAPO_CREATION_FAILED: XAudio2 failed to initialize an XAPO effect"));
			break;
		case XAUDIO2_E_DEVICE_INVALIDATED:
			ConsolePrintf(TEXT("XAUDIO2_E_DEVICE_INVALIDATED: An audio device became unusable (unplugged, etc)"));
			break;
		default:
			ConsolePrintf(TEXT("XAudio2 API unknown error 0x%x"), ErrorCode);
			break;
		}
		return false;
	}
	return true;
}
#endif

#endif // Debug_h__
