// ParallelLineSketcher.h: interface for the CParallelLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARALLELLINESKETCHER_H__C476E0B6_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_PARALLELLINESKETCHER_H__C476E0B6_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineSketcher.h"
#include "DSShape.h"	// Added by ClassView

class CParallelLineSketcher : public CLineSketcher  
{
public:
	CParallelLineSketcher(const ParallelAttrs& Attrs) { m_Attrs=Attrs; }
	// CSketcher override
	void OnDrawSketcher(CDC * pDC);
	LongPoint AdjustPosition(LongPoint pt);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);

protected:
	ParallelAttrs m_Attrs;
};

#endif // !defined(AFX_PARALLELLINESKETCHER_H__C476E0B6_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
