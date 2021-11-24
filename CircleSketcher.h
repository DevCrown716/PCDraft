// CircleSketcher.h: interface for the CCircleSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIRCLESKETCHER_H__C476E0B2_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_CIRCLESKETCHER_H__C476E0B2_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CCircleSketcher : public CSketcher  
{
public:
	enum Mode { kDiameter, kRadius, kThreePoint, kOffset };
	CCircleSketcher(Mode nMode) : m_nMode(nMode), m_nStage(0) {}
protected:
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	LongPoint AdjustPosition(LongPoint pt);
	ContinueMode QueryContinue();
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	Mode m_nMode;
	int	m_nStage;
	LongPoint m_pt[3];
};

#endif // !defined(AFX_CIRCLESKETCHER_H__C476E0B2_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
