// DocMapper.cpp: implementation of the CDocMapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "DocMapper.h"

#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "phmath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CDocMapper,CMapper)

//////////////////////////////////////////////////////////////////////


CDocMapper::CDocMapper() : 
   m_pDoc(NULL),
m_bZoomLineWeights(((CPCDraftApp*)AfxGetApp())->m_bZoomLineWeights) 
{

}


CDocMapper::CDocMapper(CPCDraftDoc* pDoc) :
   m_pDoc(pDoc),
   m_bZoomLineWeights(((CPCDraftApp*)AfxGetApp())->m_bZoomLineWeights) 
{

}



// Infinisys Ltd. 2021
double CDocMapper::NoZoomToZoom(double d) const
{
   ASSERT(m_pDoc);
   return m_pDoc->NoZoomToZoom(d);
}


Coord CDocMapper::NoZoomToZoom(const Coord& f) const
{
	ASSERT(m_pDoc);
	return m_pDoc->NoZoomToZoom(f);
}

long CDocMapper::NoZoomToZoom(long l) const
{
	ASSERT(m_pDoc);
	return m_pDoc->NoZoomToZoom(l);
}


// Infinisys Ltd. 2021
double CDocMapper::ZoomToNoZoom(double d) const
{
   ASSERT(m_pDoc);
   return m_pDoc->ZoomToNoZoom(d);
}


Coord CDocMapper::ZoomToNoZoom(const Coord& f) const
{
   ASSERT(m_pDoc);
   return m_pDoc->ZoomToNoZoom(f);
}

long CDocMapper::ZoomToNoZoom(long l) const
{
   ASSERT(m_pDoc);
   return m_pDoc->ZoomToNoZoom(l);
}

long CDocMapper::LPtoDP(const Coord& f) const
{
	ASSERT(m_pDoc);
	return m_pDoc->LPtoDP(f);
}

double CDocMapper::LPtoDPF(const Coord& f) const
{
   ASSERT(m_pDoc);
   return m_pDoc->LPtoDPF(f);
}

CPoint CDocMapper::LPtoDP(const LongPoint &lpt) const
{
	ASSERT(m_pDoc);
	return m_pDoc->LPtoDP(lpt);
}

pointd CDocMapper::LPtoDPF(const LongPoint &lpt) const
{
   ASSERT(m_pDoc);
   return m_pDoc->LPtoDPF(lpt);
}

CRect CDocMapper::LPtoDP(const LongRect &lrc) const
{
	ASSERT(m_pDoc);
	return m_pDoc->LPtoDP(lrc);
}


void CDocMapper::LPtoDP(pointd & p, const LongPoint& lpt) const
{
   ASSERT(m_pDoc);
   m_pDoc->LPtoDP(p, lpt);
}


Coord CDocMapper::DPtoLP(long l) const
{
	ASSERT(m_pDoc);
	return m_pDoc->DPtoLP(l);
}


Coord CDocMapper::DPtoLP(int l) const
{
   ASSERT(m_pDoc);
   return m_pDoc->DPtoLP((long)l);
}

Coord CDocMapper::DPtoLP(float f) const // Infinisys Ltd. GDI+
{
   ASSERT(m_pDoc);
   return m_pDoc->DPtoLP(f);
}


Coord CDocMapper::DPtoLP(double d) const // Infinisys Ltd. GDI+
{
   ASSERT(m_pDoc);
   return m_pDoc->DPtoLP(d);
}


LongPoint CDocMapper::DPtoLP(const CPoint &pt) const
{
	ASSERT(m_pDoc);
	return m_pDoc->DPtoLP(pt);
}

LongRect CDocMapper::DPtoLP(const CRect &rc) const
{
	ASSERT(m_pDoc);
	return m_pDoc->DPtoLP(rc);
}


LongPoint CDocMapper::DPtoLP(const pointd& lpt) const  // Infinisys Ltd. GDI+
{
   ASSERT(m_pDoc);
   return m_pDoc->DPtoLP(lpt);
}


Coord CDocMapper::Scalar2Long(double value) const
{
	if(m_pDoc->m_bEnglishUnits)
	{
		return value*m_pDoc->m_nDisplayRes;
	}
	else
	{
		return value*m_pDoc->m_nDisplayRes*mtRealToFix(0.1);
	}
}

short CDocMapper::ScreenPenW(short nPenW) const
{
	Coord penW=CMapper::PenW(nPenW);
	if(m_bZoomLineWeights) penW=m_pDoc->NoZoomToZoom(penW);
	nPenW=TruncToShort(penW);
	return nPenW?nPenW:1;
}

Coord CDocMapper::PenW(short nPenW) const
{
	Coord penW=CMapper::PenW(nPenW);
	return m_bZoomLineWeights?penW:m_pDoc->ZoomToNoZoom(penW);
}

float CDocMapper::fScreenPenW(float fPenW)
{
	return m_bZoomLineWeights? m_pDoc->fNoZoomToZoom(fPenW): fPenW;
}
