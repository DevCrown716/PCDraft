// LineDimSketcher.h: interface for the CLineDimSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEDIMSKETCHER_H__1CAFF2C3_CC88_11D2_A05C_00A0C9A0C319__INCLUDED_)
#define AFX_LINEDIMSKETCHER_H__1CAFF2C3_CC88_11D2_A05C_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"
#include "MDLineDim.h"

class CMDDimStr;

class CLineDimSketcher : public CSketcher  
{
public:
	enum Mode { kHoriz, kVert, kSlope, kPerp };

	CLineDimSketcher() : m_nType(kHoriz) {}
	CLineDimSketcher(DimStandard* pDimStd,DimFormat* pDimFmt,int nMode,int nType, CEdge* pEdge=NULL);

	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	ContinueMode QueryContinue();
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);

	void DrawMarker(CDC* pDC,const pointd& pt);

protected:
	int m_nMode;
	int m_nType;
	bool m_bNextDim;
	bool m_bAnchor;
	LineDimPoints m_ldp;
	Angle m_Angle;
	CMDDimStr* m_pDimStr;
	LongPoint m_lptPrevAnchor;
	bool m_bForceHairline;
	bool m_bForceArrowsInside;
	CEdge* m_pEdge;
};

#endif // !defined(AFX_LINEDIMSKETCHER_H__1CAFF2C3_CC88_11D2_A05C_00A0C9A0C319__INCLUDED_)
