// RefPointSketcher.h: interface for the CRefPointSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFPOINTSKETCHER_H__1BD77C46_53E8_11D3_B69D_A4CA8465F274__INCLUDED_)
#define AFX_REFPOINTSKETCHER_H__1BD77C46_53E8_11D3_B69D_A4CA8465F274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"

class CRefPointSketcher : public CSketcher  
{
public:
	CRefPointSketcher() {}

	LongPoint m_lptRef;
protected:
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	CSketcher::ContinueMode QueryContinue();
	void OnDrawSketcher(CDC * pDC);
	void OnUpdatePt(const LongPoint& ptUpdate);
};

#endif // !defined(AFX_REFPOINTSKETCHER_H__1BD77C46_53E8_11D3_B69D_A4CA8465F274__INCLUDED_)
