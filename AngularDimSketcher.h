// AngularDimSketcher.h: interface for the CAngularDimSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANGULARDIMSKETCHER_H__07EC3954_E9E8_11D2_B5B2_E66BFE7C387F__INCLUDED_)
#define AFX_ANGULARDIMSKETCHER_H__07EC3954_E9E8_11D2_B5B2_E66BFE7C387F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"
#include "lpoint.h"

struct DimStandard;
struct DimFormat;
class CEdge;
class CMDDimStr;

class CAngularDimSketcher : public CSketcher  
{
public:
	CAngularDimSketcher() {}
	CAngularDimSketcher(DimStandard* pDimStd,DimFormat* pDimFmt,CEdge* pEdgeFrom);
	virtual ~CAngularDimSketcher();

	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	ContinueMode QueryContinue();
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	void UpdateDimStr();

protected:
	bool m_bAnchor;
	bool m_bSmallerArc;
	LongPoint m_lptAnchor;
	LongPoint m_lptCenter;
	LongPoint m_lptFrom;
	CEdge* m_pEdgeFrom;
	LongPoint m_lptTo;
	CEdge* m_pEdgeTo;
	DimStandard* m_pDimStd;
	DimFormat* m_pDimFmt;
	CMDDimStr* m_pDimStr;
	bool m_bForceHairline;
	bool m_bForceArrowsInside;
	bool m_bEdgeFromSwap;
	bool m_bEdgeToSwap;

static bool GetDimLineEndPt(const LongPoint& lptHit, const LongPoint& lptCenter, const LongPoint& lptFrom, const LongPoint& lptTo, LongPoint& lpt);
};

#endif // !defined(AFX_ANGULARDIMSKETCHER_H__07EC3954_E9E8_11D2_B5B2_E66BFE7C387F__INCLUDED_)
