// ZoomSketcher.h: interface for the CZoomSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZOOMSKETCHER_H__6160B106_DFAB_11D2_9928_000000000000__INCLUDED_)
#define AFX_ZOOMSKETCHER_H__6160B106_DFAB_11D2_9928_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CZoomSketcher : public CSketcher  
{
public:
	void OnDrawSketcher(CDC * pDC);
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnUpdatePt(const LongPoint& ptUpdate);

	LongPoint m_ptFrom;
	LongPoint m_ptTo;
	bool m_bArea;
	short m_nZoom;
	double m_fZoom;
	CRect m_thinRect;
	CRect m_thickRect;
};

#endif // !defined(AFX_ZOOMSKETCHER_H__6160B106_DFAB_11D2_9928_000000000000__INCLUDED_)
