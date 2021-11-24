// DistributeSketcher.h: interface for the CDistributeSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISTRIBUTESKETCHER_H__09EB3C84_D706_11D2_B590_EA06B8112A7F__INCLUDED_)
#define AFX_DISTRIBUTESKETCHER_H__09EB3C84_D706_11D2_B590_EA06B8112A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDShape.h"
#include "LineSketcher.h"

class CDistributeSketcher : public CLineSketcher  
{
public:
	CDistributeSketcher(CMDShapeList* pShapeList,bool bByDatum);

	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);

protected:
	CMDShapeList* m_pShapeList;
	bool m_bByDatum;
	int m_nCount;
};

#endif // !defined(AFX_DISTRIBUTESKETCHER_H__09EB3C84_D706_11D2_B590_EA06B8112A7F__INCLUDED_)
