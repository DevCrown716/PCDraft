// OriginSketcher.h: interface for the COriginSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORIGINSKETCHER_H__E9D666B3_5BA2_11D3_955E_008048DA0F68__INCLUDED_)
#define AFX_ORIGINSKETCHER_H__E9D666B3_5BA2_11D3_955E_008048DA0F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"

class COriginSketcher : public CSketcher  
{
public:
	COriginSketcher();
	virtual ~COriginSketcher();

	virtual void OnDrawSketcher(CDC * pDC);
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnUpdatePt(const LongPoint& ptUpdate);

	LongPoint m_ptOrigin;
};

#endif // !defined(AFX_ORIGINSKETCHER_H__E9D666B3_5BA2_11D3_955E_008048DA0F68__INCLUDED_)
