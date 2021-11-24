// ZoomInSketcher.h: interface for the CZoomInSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZOOMINSKETCHER_H__7BC07A03_40D4_11D3_955B_008048DA0F68__INCLUDED_)
#define AFX_ZOOMINSKETCHER_H__7BC07A03_40D4_11D3_955B_008048DA0F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"

class CZoomInSketcher : public CSketcher  
{
public:
	CZoomInSketcher(int nZoomFactor);

	void OnDrawSketcher(CDC * pDC);
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnUpdatePt(const LongPoint& ptUpdate);

	LongPoint m_ptCenter;
	CRect m_Rect;
	CSize m_HalfSize;
	int m_nZoomFactor;
};

#endif // !defined(AFX_ZOOMINSKETCHER_H__7BC07A03_40D4_11D3_955B_008048DA0F68__INCLUDED_)
