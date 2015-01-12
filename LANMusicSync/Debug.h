#ifndef Debug_h__
#define Debug_h__

#include <windows.h>
#include <xaudio2.h>
#include <string>

// Outputs to the Visual Studio console
static void ConsolePrintf(LPTSTR fmt, ...)
{
	va_list vl;
	char szBuf[256];

	va_start(vl, fmt);
	vsprintf_s(szBuf, fmt, vl);
	va_end(vl);

	OutputDebugString(szBuf);
	OutputDebugString(TEXT("\r\n"));
}

#endif // Debug_h__
