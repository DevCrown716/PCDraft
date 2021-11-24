// ConstrLineSketcher.h: interface for the CConstrLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTRLINESKETCHER_H__5CBCE666_CEF1_11D2_B582_AF564741237F__INCLUDED_)
#define AFX_CONSTRLINESKETCHER_H__5CBCE666_CEF1_11D2_B582_AF564741237F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineSketcher.h"

class CConstrLineSketcher : public CLineSketcher  
{
public:
	CConstrLineSketcher(int nConstraintAngle) : m_nConstraintAngle(nConstraintAngle) {}

protected:
	// CSketcher override
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void OnUpdatePt(const LongPoint& ptUpdate);

protected:
	int m_nConstraintAngle;
};

#endif // !defined(AFX_CONSTRLINESKETCHER_H__5CBCE666_CEF1_11D2_B582_AF564741237F__INCLUDED_)
