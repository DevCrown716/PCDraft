// RndRectSketcher.h: interface for the CRndRectSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RNDRECTSKETCHER_H__C476E0AD_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_RNDRECTSKETCHER_H__C476E0AD_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RectSketcher.h"

class CRndRectSketcher : public CRectSketcher  
{
public:
	enum Mode { kCornerElliptical, kCornerCircle, kCornerProportional };
	CRndRectSketcher(Mode nMode) :
		CRectSketcher(CRectSketcher::kRectDiagonal), m_nMode(nMode) {}
	// CSketcher override
	void OnDrawSketcher(CDC * pDC);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	Mode m_nMode;
	LongPoint m_lptCorner;
};

#endif // !defined(AFX_RNDRECTSKETCHER_H__C476E0AD_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
