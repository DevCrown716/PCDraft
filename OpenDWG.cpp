// OpenDWG.cpp: implementation of the COpenDWG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "OpenDWG.h"
#include <io.h>
//#include <odio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenDWG::COpenDWG()
{
/*	adSetAd2OpenInitFileFn(OpenInitFile);
	adSetAd2ReadInitFileCharFn(ReadInitFileChar);
	adSetAd2ReadInitFileCharsFn(ReadInitFileChars);
	adSetAd2SeekInitFileFn(SeekInitFile);
	adSetAd2CloseInitFileFn(CloseInitFile);

	short initerror;
	if(!adInitAd2(NULL,&initerror))
	{
		TRACE("Cannot init AD\n");
	}
*/	
}

COpenDWG::~COpenDWG()
{
//	adCloseAd2();
}

unsigned char *COpenDWG::pInitData=0;
int COpenDWG::nPos=0;

void* COpenDWG::OpenInitFile(const char*)
{
	if(!pInitData)
	{
		HRSRC hResInfo=::FindResource(0,MAKEINTRESOURCE(IDR_ADINIT_DAT),"ADINIT");
		ASSERT(hResInfo);
		HGLOBAL hResData=::LoadResource(0,hResInfo);
		ASSERT(hResData);
		pInitData=(unsigned char*)::LockResource(hResData);
		ASSERT(pInitData);
	}
	nPos=0;
	return (void*)1;
}

unsigned char COpenDWG::ReadInitFileChar(void*)
{
	return pInitData[nPos++];
}

int COpenDWG::ReadInitFileChars(void *buf, unsigned int size, unsigned int num, void *f)
{
	memcpy(buf,pInitData+nPos,size*num);
	nPos+=size*num;
	return num;
}

//int COpenDWG::SeekInitFile(void *f, OdaLong offset, int whence)
//{
//	nPos=offset;
//	return 0;
//}

int COpenDWG::CloseInitFile(void *f)
{
	return 0;
}
