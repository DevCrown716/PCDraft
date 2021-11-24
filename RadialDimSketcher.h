// RadialDimSketcher.h: interface for the CRadialDimSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIALDIMSKETCHER_H__07EC3953_E9E8_11D2_B5B2_E66BFE7C387F__INCLUDED_)
#define AFX_RADIALDIMSKETCHER_H__07EC3953_E9E8_11D2_B5B2_E66BFE7C387F__INCLUDED_

#include "lpoint.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sketcher.h"

class CMDShape;
struct DimStandard;
struct DimFormat;
class CMDDimStr;

class CRadialDimSketcher : public CSketcher  
{
public:
	void UpdateDimStr(CDC* pDC);
	CRadialDimSketcher(DimStandard* pDimStd,DimFormat* pDimFmt,CMDShape* pShape,bool bDiameter=false) : m_pDimStd(pDimStd),m_pDimFmt(pDimFmt),m_pShape(pShape),m_bDiameter(bDiameter),m_bForceHairline(GetAsyncKeyState(VK_SHIFT)<0?true:false),m_bForceArrowsInside(GetAsyncKeyState(VK_OPTION)<0?true:false) {}

// CSketcher override 
	ContinueMode QueryContinue();
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	LongPoint AdjustPosition(LongPoint pt);
	void OnDrawSketcher(CDC * pDC);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);

protected:
	bool m_bDiameter;
	Coord m_Radius;
	CMDShape* m_pShape;
	LongPoint m_lptAnchor;
	DimStandard* m_pDimStd;
	DimFormat* m_pDimFmt;
	CMDDimStr* m_pDimStr;
	bool m_bForceHairline;
	bool m_bForceArrowsInside;
	bool m_bUpdateDimStr;
};

#endif // !defined(AFX_RADIALDIMSKETCHER_H__07EC3953_E9E8_11D2_B5B2_E66BFE7C387F__INCLUDED_)
