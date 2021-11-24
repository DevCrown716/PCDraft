// MacGWorld.cpp: implementation of the CMacGWorld class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//namespace QTML {
//#include <QTML.h>
//#include <QuickDraw.h>
//#include <ImageCompression.h>
//#include "MacGWorld.h"
//
//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif
//
////////////////////////////////////////////////////////////////////////
//// Construction/Destruction
////////////////////////////////////////////////////////////////////////
//
//CMacGWorld::CMacGWorld()
//{
//	m_bDrawing=false;
//	m_GWorld=NULL;
//	m_rcBound.left=m_rcBound.top=m_rcBound.right=m_rcBound.bottom=0;
//}
//
//CMacGWorld::CMacGWorld(const Rect* rcBound,int nPixelDepth,CTabHandle table)
//{
//	m_bDrawing=false;
//	m_GWorld=NULL;
//	SetGWorld(rcBound,nPixelDepth,table);
//	BeginDrawing();
//}
//
//CMacGWorld::~CMacGWorld()
//{
//	CleanUp();
//}
//
//void CMacGWorld::CleanUp()
//{
//	if(m_GWorld)
//	{
//		EndDrawing();
//		DisposeGWorld(m_GWorld);
//		m_GWorld=NULL;
//	}
//}
//
//void CMacGWorld::SetGWorld(const Rect* rcBound,int nPixelDepth,CTabHandle table)
//{
//	CleanUp();
//	AdjustPixelDepth(nPixelDepth,table);
//	OSErr err=QTNewGWorld(&m_GWorld,nPixelDepth,rcBound,table,NULL,(GWorldFlags)0);
//	ASSERT(err==noErr/* && GetDIB()*/);
//	m_rcBound=*rcBound;
//}
//
//void CMacGWorld::BeginDrawing()
//{
//	if(!m_bDrawing)
//	{
//		ASSERT(m_GWorld!=NULL);
//		LockPixels(GetPixMapHandle());
//		QTML::GetGWorld(&m_OldPort,&m_OldDevice);
//		QTML::SetGWorld(m_GWorld,NULL);
//		m_bDrawing=true;
//	}
//}
//
//void CMacGWorld::EndDrawing()
//{
//	if(m_bDrawing)
//	{
//		ASSERT(m_GWorld!=NULL);
//		QTML::SetGWorld(m_OldPort,m_OldDevice);
//		UnlockPixels(GetPixMapHandle());
//		m_bDrawing=false;
//	}
//}
//
//PicHandle CMacGWorld::GetPicture(const Rect* pRc)
//{
//	BeginDrawing();
//	if(NULL==pRc){
//		pRc=&m_rcBound;
//	}
//	Rect rcPicture=*pRc;
//	MacOffsetRect(&rcPicture,-rcPicture.left,-rcPicture.top);
//	PicHandle hPict=OpenPicture(&rcPicture);
//	BitMap* pBitMap=GetBitMapHandle();
//	QTML::CopyBits(pBitMap,pBitMap,pRc,&rcPicture,ditherCopy,0);
//	ClosePicture();
//	EndDrawing();
//	return hPict;
//}
//
//void CMacGWorld::AdjustPixelDepth(int& nDepth,CTabHandle& table)
//{
//	if(nDepth==-1) // current screen colors
//	{
//		PixMapHandle map=GetMaxMap();
//		nDepth=(*map)->pixelSize;
//		table=(*map)->pmTable;
//	}
//	switch(nDepth)
//	{
//	case k16BE555PixelFormat:
//		nDepth=k16LE555PixelFormat;
//		break;
//	case k24RGBPixelFormat:
//		nDepth=k24BGRPixelFormat;
//		break;
//	case k32ARGBPixelFormat:
//	case k32ABGRPixelFormat:
//	case k32RGBAPixelFormat:
//		nDepth=k32BGRAPixelFormat;
//		break;
//	}
//}
//
//PixMapHandle CMacGWorld::GetMaxMap()
//{
//	Rect				maxRect;
//	GDHandle			maxDevice;
//	PixMapHandle		map;
//	
//	maxRect.top=(short)0x8001;
//	maxRect.left=(short)0x8001;
//	maxRect.bottom=(short)0x7FFF;
//	maxRect.right=(short)0x7FFF;
//	maxDevice=GetMaxDevice(&maxRect);
//	map=(*maxDevice)->gdPMap;
//
//	return map;
//}
//
//}
