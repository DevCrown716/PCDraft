// ScaleMapper.h: interface for the CScaleMapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALEMAPPER_H__2B3F12A4_5FC7_11D3_B6C2_B5060148A774__INCLUDED_)
#define AFX_SCALEMAPPER_H__2B3F12A4_5FC7_11D3_B6C2_B5060148A774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mapper.h"

class CScaleMapper : public CMapper  
{
public:
	CScaleMapper(double factor=1) : m_Factor(factor) {}

	long NoZoomToZoom(long l) const;
	Coord NoZoomToZoom(const Coord& f) const;
	long LPtoDP(const Coord& f) const;
	CPoint LPtoDP(const LongPoint& lpt) const;
	CRect LPtoDP(const LongRect& lrc) const;
	Coord DPtoLP(long l) const;
	LongPoint DPtoLP(const CPoint& pt) const;
	LongRect DPtoLP(const CRect& rc) const;
	short ScreenPenW(short nPenW) const;

protected:
	double m_Factor;
};

#endif // !defined(AFX_SCALEMAPPER_H__2B3F12A4_5FC7_11D3_B6C2_B5060148A774__INCLUDED_)
