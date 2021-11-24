// PolySketcher.h: interface for the CPolySketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYSKETCHER_H__C476E0B1_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_POLYSKETCHER_H__C476E0B1_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CPolySketcher : public CSketcher  
{
public:
	enum { kMaxPoint=300 };
	CPolySketcher(bool bClosed) : m_bClosed(bClosed), m_nCurPoint(0), m_bRemovePt(false) {}
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	ContinueMode QueryContinue();
	LongPoint AdjustPosition(LongPoint ptTo);
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	bool IsNearPoints(const LongPoint& lpt1,const LongPoint& lpt2);
protected:
	int	m_nCurPoint;
	LongPoint m_pt[kMaxPoint+1];
	bool m_bClosed;
	bool m_bMoved;
	bool m_bRemovePt;
};

#endif // !defined(AFX_POLYSKETCHER_H__C476E0B1_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
