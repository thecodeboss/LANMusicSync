#ifndef XAudio2VoiceCallback_h__
#define XAudio2VoiceCallback_h__

#ifdef WINDOWS
#include <xaudio2.h>

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	HANDLE hBufferEndEvent;

	XAudio2VoiceCallback() : hBufferEndEvent( CreateEventEx( NULL, FALSE, FALSE, NULL ) ) {}
	virtual ~XAudio2VoiceCallback() { CloseHandle( hBufferEndEvent ); }

	//overrides
	STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 bytesRequired )
	{
	}
	STDMETHOD_( void, OnVoiceProcessingPassEnd )()
	{
	}
	STDMETHOD_( void, OnStreamEnd )()
	{
	}
	STDMETHOD_( void, OnBufferStart )( void* pContext )
	{
	}
	STDMETHOD_( void, OnBufferEnd )( void* pContext )
	{
		SetEvent( hBufferEndEvent );
	}
	STDMETHOD_( void, OnLoopEnd )( void* pContext )
	{
	}
	STDMETHOD_( void, OnVoiceError )( void* pContext, HRESULT error )
	{
	}
};
#endif

#endif // XAudio2VoiceCallback_h__
