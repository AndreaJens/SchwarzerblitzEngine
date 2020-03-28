// class implemented on 06.12.2018 fllowing the thread by Emil_halim » Tue Apr 12, 2005 8:06 pm
//http://irrlicht.sourceforge.net/forum/viewtopic.php?t=6389

#pragma once

#include "Irrlicht_TMovie.h"
#include <cmath>

//#pragma comment(lib, "strmiids")

namespace fk_engine {

	TMovie::TMovie()
	{
		CoInitialize(0);
		pAMStream = 0;
		pPrimaryVidStream = 0;
		pDDStream = 0;
		pSample = 0;
		pSurface = 0;
		time = 0;
	}

	TMovie::~TMovie()
	{
		pPrimaryVidStream->Release();
		pDDStream->Release();
		pSample->Release();
		pSurface->Release();
		pAMStream->Release();
		CoUninitialize();
	}

	void TMovie::LoadMovie(const char* filename)
	{
		WCHAR buf[512];
		MultiByteToWideChar(CP_ACP, 0, filename, -1, buf, 512);
		CoCreateInstance(MY_CLSID_AMMultiMediaStream, 0, 1, MY_IID_IAMMultiMediaStream, (void**)&pAMStream);
		pAMStream->Initialize((STREAM_TYPE)0, 0, NULL);
		pAMStream->AddMediaStream(0, &MY_MSPID_PrimaryVideo, 0, NULL);
		pAMStream->AddMediaStream(0, &MY_MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, NULL);
		pAMStream->OpenFile(buf, AMMSF_RENDERTOEXISTING); // 4); 
		pAMStream->GetMediaStream(MY_MSPID_PrimaryVideo, &pPrimaryVidStream);
		pPrimaryVidStream->QueryInterface(MY_IID_IDirectDrawMediaStream, (void**)&pDDStream);
		pDDStream->CreateSample(0, 0, 0, &pSample);
		pSample->GetSurface(&pSurface, &Movie_rect);
		pAMStream->SetState((STREAM_STATE)1);
	}

	void TMovie::NextMovieFrame()
	{
		if (GetTickCount() - oldtick < time)return;
		oldtick = GetTickCount();
		pSample->Update(0, NULL, NULL, 0);
	}

	int TMovie::MovieWidth() { return (Movie_rect.right - Movie_rect.left); }

	int TMovie::MovieHeight() { return (Movie_rect.bottom - Movie_rect.top); }

	void TMovie::DrawMovie(int x, int y, irr::video::ITexture* Buf)
	{
		void* pBits = Buf->lock();
		LONG  Pitch = Buf->getPitch();
		DDSURFACEDESC  ddsd;
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		pSurface->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		int wmin = (Pitch < ddsd.lPitch) ? Pitch : ddsd.lPitch;
		for (int h = 0; h < ddsd.dwHeight; h++)
			memcpy((BYTE*)pBits + ((y + h)*Pitch) + x * 4, (BYTE*)ddsd.lpSurface + h*ddsd.lPitch, wmin);
		pSurface->Unlock(NULL);
		Buf->unlock();
	}

	void TMovie::SetMovieFPS(int fps)
	{
		time = fps;
	}

};