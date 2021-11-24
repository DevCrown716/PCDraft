// Mapper.cpp: implementation of the CMapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "Mapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMapper,CObject)

//////////////////////////////////////////////////////////////////////

double CMapper::NoZoomToZoom(double d) const // Infinisys Ltd. 2021
{
   return d;
}

Coord CMapper::NoZoomToZoom(const Coord& f) const
{
	return f;
}

long CMapper::NoZoomToZoom(long l) const
{
   return l;
}

double CMapper::ZoomToNoZoom(double d) const // Infinisys Ltd. 2021
{
   return d;
}

Coord CMapper::ZoomToNoZoom(const Coord& f) const
{
   return f;
}

long CMapper::ZoomToNoZoom(long l) const
{
   return l;
}

long CMapper::LPtoDP(const Coord& f) const
{
	return f.GetLong();
}

double CMapper::LPtoDPF(const Coord& f) const
{
   return f.GetDouble();
}

pointd CMapper::LPtoDPF(const LongPoint &lpt) const
{
   pointd p;
   LPtoDP(p, lpt);
   return p;
}

void CMapper::LPtoDP(pointd & pt, const LongPoint &lpt) const
{
   pt.x = LPtoDPF(lpt.x);
   pt.y = LPtoDPF(lpt.y);
}

CPoint CMapper::LPtoDP(const LongPoint &lpt) const
{
	return CPoint(lpt.x.GetLong(),lpt.y.GetLong());
}

CRect CMapper::LPtoDP(const LongRect &lrc) const
{
	return CRect(lrc.left.GetLong(),lrc.top.GetLong(),lrc.right.GetLong(),lrc.bottom.GetLong());
}

Coord CMapper::DPtoLP(long l) const
{
	return WrFixedFromFixed(l<<16);
}
Coord CMapper::DPtoLP(int i) const
{
   return WrFixedFromFixed(i << 16);
}

Coord CMapper::DPtoLP(float f) const
{
   return (f/65536.0);
}

Coord CMapper::DPtoLP(double d) const
{
   return (d / 65536.0);
}

LongPoint CMapper::DPtoLP(const pointd& p) const
{
   return LongPoint(DPtoLP(p.x), DPtoLP(p.y));
}


LongPoint CMapper::DPtoLP(const CPoint &pt) const
{
	return pt;
}

LongRect CMapper::DPtoLP(const CRect &rc) const
{
	return rc;
}

Coord CMapper::Scalar2Long(double value) const
{
	return value;
}

short CMapper::ScreenPenW(short nPenW) const
{
	nPenW=TruncToShort(NoZoomToZoom(PenW(nPenW)));
	return nPenW?nPenW:1;
}

Coord CMapper::PenW(short nPenW) const
{
	switch(nPenW)
	{
	case 0x81:
		return WrFixedFromRatio(24,100); // 0.24 - hairline
	case 0xA1:
		return FixedHalf;
	case 0xC1:
		return WrFixedFromFixed(0x0000C000);
	default:
		return nPenW & 0x1F;
	}
}

float CMapper::fScreenPenW(float fPenW)
{
	return fPenW;
}
