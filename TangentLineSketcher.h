// TangentLineSketcher.h: interface for the CTangentLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANGENTLINESKETCHER_H__C1C727D3_54A0_11D3_B69F_A967EF18BE74__INCLUDED_)
#define AFX_TANGENTLINESKETCHER_H__C1C727D3_54A0_11D3_B69F_A967EF18BE74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"

class CMDShape;

class CTangentLineSketcher : public CSketcher  
{
public:
	bool ComputeTangent(LongPoint& lptFrom, LongPoint& lptTo);
	CTangentLineSketcher(CMDShape* pShape) : m_pShapeFrom(pShape) {}

	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	ContinueMode QueryContinue();
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);

protected:
	CMDShape* m_pShapeFrom;
	CMDShape* m_pShapeTo;
	LongPoint m_lpt;
};

#endif // !defined(AFX_TANGENTLINESKETCHER_H__C1C727D3_54A0_11D3_B69F_A967EF18BE74__INCLUDED_)
