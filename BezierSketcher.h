// BezierSketcher.h: interface for the CBezierSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIERSKETCHER_H__C476E0B4_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_BEZIERSKETCHER_H__C476E0B4_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CBezierSketcher : public CSketcher  
{
public:
	enum { kMaxPoint=300 };
	CBezierSketcher() :  m_nCurPoint(0), m_bMouseDown(false), m_bEraser(false), m_bRemovePt(false) {}
	// CSketcher override
	ContinueMode QueryContinue();
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void HideEraser();
protected:
	int	m_nCurPoint;
	LongPoint m_pt[kMaxPoint+1];
	bool m_bMouseDown;
	bool m_bEraser;
	bool m_bRemovePt;
};

#endif // !defined(AFX_BEZIERSKETCHER_H__C476E0B4_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
