// RotateSketcher.h: interface for the CRotateSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTATESKETCHER_H__5BE1CDA6_D3A6_11D2_B589_A7A676B1297F__INCLUDED_)
#define AFX_ROTATESKETCHER_H__5BE1CDA6_D3A6_11D2_B589_A7A676B1297F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"

class CRotateSketcher : public CSketcher  
{
public:
	CRotateSketcher(Angle angle=0,bool bAboutCenter=true, LongPoint* pExtentCenter=NULL);

protected:
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	CSketcher::ContinueMode QueryContinue();
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	void OnDrawSketcher(CDC * pDC);

	short m_nMode;
	bool m_bAboutCenter;
	LongPoint m_ptCenter,m_pt;
	short m_nDegree,m_nMinute,m_nSecond;
	Angle m_startAngle;
	LongPoint* m_pExtentCenter;
};

#endif // !defined(AFX_ROTATESKETCHER_H__5BE1CDA6_D3A6_11D2_B589_A7A676B1297F__INCLUDED_)
