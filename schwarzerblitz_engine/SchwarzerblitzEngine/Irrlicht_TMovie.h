// class implemented on 06.12.2018 following the thread by Emil_halim » Tue Apr 12, 2005 8:06 pm
//http://irrlicht.sourceforge.net/forum/viewtopic.php?t=6389


#pragma once

#include <dshow.h>
#include <mmstream.h>
#include <amstream.h> 
#include <ddstream.h>
#include <irrlicht.h>

namespace fk_engine {

	static GUID MY_CLSID_AMMultiMediaStream = { 0x49C47CE5,0x9BA4,0x11D0,0x82,0x12,0x00,0xC0,0x4F,0xC3,0x2C,0x45 };
	static GUID MY_IID_IAMMultiMediaStream = { 0xBEBE595C,0x9A6F,0x11D0,0x8F,0xDE,0x00,0xC0,0x4F,0xD9,0x18,0x9D };
	static GUID MY_MSPID_PrimaryVideo = { 0xA35FF56A,0x9FDA,0x11D0,0x8F,0xDF,0x00,0xC0,0x4F,0xD9,0x18,0x9D };
	static GUID MY_IID_IDirectDrawMediaStream = { 0xF4104FCE,0x9A70,0x11D0,0x8F,0xDE,0x00,0xC0,0x4F,0xD9,0x18,0x9D };
	static GUID MY_MSPID_PrimaryAudio = { 0xA35FF56B,0x9FDA,0x11D0,0x8F,0xDF,0x00,0xC0,0x4F,0xD9,0x18,0x9D };

	class TMovie
	{
		IAMMultiMediaStream*     pAMStream;
		IMediaStream*            pPrimaryVidStream;
		IDirectDrawMediaStream*  pDDStream;
		IDirectDrawStreamSample* pSample;
		IDirectDrawSurface*      pSurface;
		RECT                     Movie_rect;
		LONG                     MoviePitch;
		void*                    MovieBuffer;
		DWORD                    time;
		DWORD                    oldtick;

	public:
		TMovie();
		~TMovie();
		void LoadMovie(const char* filename);
		void NextMovieFrame();
		int MovieWidth();
		int MovieHeight();
		void DrawMovie(int x, int y, irr::video::ITexture* Buf);
		void SetMovieFPS(int fps);
	};
}