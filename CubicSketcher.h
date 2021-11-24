// CubicSketcher.h: interface for the CCubicSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUBICSKETCHER_H__C476E0B5_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_CUBICSKETCHER_H__C476E0B5_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CCubicSketcher : public CSketcher  
{
public:
	enum { kMaxPoint=300 };
	CCubicSketcher() :  m_nCurPoint(0), m_bRemovePt(false) {}
	// CSketcher override
	ContinueMode QueryContinue();
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	int	m_nCurPoint;
	LongPoint m_pt[kMaxPoint];
	bool m_bRemovePt;
};

#endif // !defined(AFX_CUBICSKETCHER_H__C476E0B5_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
