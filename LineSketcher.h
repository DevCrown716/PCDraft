// LineSketcher.h: interface for the CLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINESKETCHER_H__C476E0AA_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_LINESKETCHER_H__C476E0AA_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CLineSketcher : public CSketcher  
{
public:
	CLineSketcher() {}

	LongPoint m_ptFrom;
	LongPoint m_ptTo;
protected:
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	CSketcher::ContinueMode QueryContinue();
};

#endif // !defined(AFX_LINESKETCHER_H__C476E0AA_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
