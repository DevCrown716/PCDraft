// FreehandSketcher.h: interface for the CFreehandSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREEHANDSKETCHER_H__C476E0B0_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_FREEHANDSKETCHER_H__C476E0B0_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CFreehandSketcher : public CSketcher  
{
public:
	enum { kMaxPoint=32000 };
	CFreehandSketcher(bool bClosed) : m_bClosed(bClosed), m_nCurPoint(0), m_bEraser(false) {}
	// CSketcher override
	bool DoSketch(CPCDraftView* pView, CPoint& ptStart, LPRECT lpRectClip=NULL,UINT nSketchMode=0);
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	void OnQuickRedraw(CDC* pDC);
	ContinueMode QueryContinue();
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC *pDC, const LongPoint& pt);
	void HideEraser();
	void OnEraseSketcher(CDC * pDC);
protected:
	int	m_nCurPoint;
	LongPoint m_pt[kMaxPoint+1];
	//bool iCh[kMaxPoint+1];
	bool m_bClosed;
	bool m_bEraser;
	bool wasErased;
	LongPoint oldPoint;
};

#endif // !defined(AFX_FREEHANDSKETCHER_H__C476E0B0_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
