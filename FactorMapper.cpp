// FactorMapper.cpp: implementation of the CFactorMapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "FactorMapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

// Infinisys Ltd. 2021
double CFactorMapper::NoZoomToZoom(double d) const
{
   return d * m_Factor;
}

Coord CFactorMapper::NoZoomToZoom(const Coord& f) const
{
	return f*m_Factor;
}

long CFactorMapper::NoZoomToZoom(long l) const
{
	return long(l*m_Factor);
}

// Infinisys Ltd. 2021
double CFactorMapper::ZoomToNoZoom(double d) const
{
   return d / m_Factor;
}


Coord CFactorMapper::ZoomToNoZoom(const Coord& f) const
{
   return f / m_Factor;
}

long CFactorMapper::ZoomToNoZoom(long l) const
{
   return long(l/m_Factor);
}

long CFactorMapper::LPtoDP(const Coord& f) const
{
	return long(f*m_Factor);
}

double CFactorMapper::LPtoDPF(const Coord& f) const
{
   return double(f*m_Factor);
}

CPoint CFactorMapper::LPtoDP(const LongPoint &lpt) const
{
	return CPoint(LPtoDP(lpt.x-m_lptOrg.x),LPtoDP(lpt.y-m_lptOrg.y));
}

void CFactorMapper::LPtoDP(pointd & pt, const LongPoint &lpt) const
{
   pt.x = LPtoDPF(lpt.x - m_lptOrg.x);
   pt.y = LPtoDPF(lpt.y - m_lptOrg.y);

}

CRect CFactorMapper::LPtoDP(const LongRect &lrc) const
{
	return CRect(LPtoDP(lrc.GetCorner(0)),LPtoDP(lrc.GetCorner(2)));
}

Coord CFactorMapper::DPtoLP(long l) const
{
	return WrFixedFromRawDoubleTruncated(l*65536.0/m_Factor);
}

Coord CFactorMapper::DPtoLP(int i) const
{
   return WrFixedFromRawDoubleTruncated(i*65536.0 / m_Factor);
}


Coord CFactorMapper::DPtoLP(float f) const
{
   return f / m_Factor;
}

Coord CFactorMapper::DPtoLP(double d) const
{
   return d / m_Factor;
}


LongPoint CFactorMapper::DPtoLP(const pointd &pt) const
{
   return (m_lptOrg + LongPoint(DPtoLP(pt.x), DPtoLP(pt.y)));
}


LongPoint CFactorMapper::DPtoLP(const CPoint &pt) const
{
	return (m_lptOrg+LongPoint(DPtoLP(pt.x),DPtoLP(pt.y)));
}

LongRect CFactorMapper::DPtoLP(const CRect &rc) const
{
	return LongRect(DPtoLP(rc.TopLeft()),DPtoLP(rc.BottomRight()));
}


Coord CFactorMapper::Scalar2Long(double value) const
{
   if (m_pDoc->m_bEnglishUnits)
   {
      return value * m_pDoc->m_nDisplayRes;
   }
   else
   {
      return value * m_pDoc->m_nDisplayRes*mtRealToFix(0.1);
   }
}
