// ExtrudeSketcher.h: interface for the CExtrudeSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTRUDESKETCHER_H__9BEAE9C3_6A00_11D3_B6DE_E81AA3C4B974__INCLUDED_)
#define AFX_EXTRUDESKETCHER_H__9BEAE9C3_6A00_11D3_B6DE_E81AA3C4B974__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PolySketcher.h"
#include "DSShape.h"

class CMDShape;

class CExtrudeSketcher : public CPolySketcher  
{
public:
	CExtrudeSketcher(const ParallelAttrs& Attrs,bool bPoly,const LongPoint& lpt1,const LongPoint& lpt2, CMDShape* pShape) : CPolySketcher(false), m_Attrs(Attrs), m_bPoly(bPoly), m_lpt1(lpt1), m_lpt2(lpt2), m_pShape(pShape) {}

	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	ContinueMode QueryContinue();
	void OnUpdatePt(const LongPoint& ptUpdate);
	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);

protected:
	ParallelAttrs m_Attrs;
	bool m_bPoly;
	LongPoint m_lpt1,m_lpt2;
	CMDShape* m_pShape;
	Angle m_limitAngle;
};

#endif // !defined(AFX_EXTRUDESKETCHER_H__9BEAE9C3_6A00_11D3_B6DE_E81AA3C4B974__INCLUDED_)
