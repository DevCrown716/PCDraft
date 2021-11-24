// RectSketcher.h: interface for the CRectSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTSKETCHER_H__C476E0AC_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_RECTSKETCHER_H__C476E0AC_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CRectSketcher : public CSketcher  
{
public:
	void MakeRect(::Gdiplus::RectF & rc);
	enum Mode { kRectCenter, kRectDiagonal };
	CRectSketcher(Mode nMode) : m_nMode(nMode) {}
protected:
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	LongPoint AdjustPosition(LongPoint pt);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	Mode		 m_nMode;
	LongPoint	 m_ptFrom;
	LongPoint	 m_ptTo;
};

#endif // !defined(AFX_RECTSKETCHER_H__C476E0AC_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
