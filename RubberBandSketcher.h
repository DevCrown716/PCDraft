// RubberBandSketcher.h: interface for the CRubberBandSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUBBERBANDSKETCHER_H__C476E0B9_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_RUBBERBANDSKETCHER_H__C476E0B9_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RectSketcher.h"

class CRubberBandSketcher : public CRectSketcher  
{
public:
	CRubberBandSketcher() : CRectSketcher(kRectDiagonal) {}
	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC,const LongPoint& pt);

	CRect m_rc;
};

#endif // !defined(AFX_RUBBERBANDSKETCHER_H__C476E0B9_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
