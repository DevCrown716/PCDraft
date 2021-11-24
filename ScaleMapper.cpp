// ScaleMapper.cpp: implementation of the CScaleMapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "ScaleMapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

Coord CScaleMapper::NoZoomToZoom(const Coord& f) const
{
	return f*m_Factor;
}

long CScaleMapper::NoZoomToZoom(long l) const
{
	return long(l*m_Factor);
}

long CScaleMapper::LPtoDP(const Coord& f) const
{
	return long(f*m_Factor);
}

CPoint CScaleMapper::LPtoDP(const LongPoint &lpt) const
{
	return CPoint(LPtoDP(lpt.x),LPtoDP(lpt.y));
}

CRect CScaleMapper::LPtoDP(const LongRect &lrc) const
{
	return CRect(LPtoDP(lrc.GetCorner(0)),LPtoDP(lrc.GetCorner(2)));
}

Coord CScaleMapper::DPtoLP(long l) const
{
	return WrFixedFromRawDoubleTruncated(l*65536.0/m_Factor);
}

LongPoint CScaleMapper::DPtoLP(const CPoint &pt) const
{
	return LongPoint(DPtoLP(pt.x),DPtoLP(pt.y));
}

LongRect CScaleMapper::DPtoLP(const CRect &rc) const
{
	return LongRect(DPtoLP(rc.TopLeft()),DPtoLP(rc.BottomRight()));
}

short CScaleMapper::ScreenPenW(short nPenW) const
{
	nPenW &= 0x7F;
	return (short)NoZoomToZoom(nPenW);
}
