// ParallelPolySketcher.h: interface for the CParallelPolySketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARALLELPOLYSKETCHER_H__C476E0B7_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_PARALLELPOLYSKETCHER_H__C476E0B7_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PolySketcher.h"
#include "DSShape.h"

class CParallelPolySketcher : public CPolySketcher  
{
public:
	CParallelPolySketcher(const ParallelAttrs& Attrs,bool bClosed) : CPolySketcher(bClosed) { m_Attrs=Attrs; }

protected:
	// CSketcher override
	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);

protected:
	ParallelAttrs m_Attrs;
};

#endif // !defined(AFX_PARALLELPOLYSKETCHER_H__C476E0B7_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
