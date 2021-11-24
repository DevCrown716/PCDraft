// SpecialLineSketcher.h: interface for the CSpecialLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPECIALLINESKETCHER_H__C476E0BA_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_SPECIALLINESKETCHER_H__C476E0BA_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineSketcher.h"
#include "lpoint.h"	// Added by ClassView

class CEdge;

class CSpecialLineSketcher : public CLineSketcher  
{
public:
	enum { kPoint,kMidPoint,kEndPoint,kEdgePerp,kArcPerp,kTangent,kOffset };

	CSpecialLineSketcher(short nType) : m_nType(nType) {}
	CSpecialLineSketcher(const LongPoint& lpt) : m_lpt(lpt),m_nType(kPoint) {}
	CSpecialLineSketcher(CEdge* pEdge,short nType) : m_pEdge(pEdge),m_nType(nType) {}

	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	ContinueMode QueryContinue();
	void OnUpdatePt(const LongPoint& ptUpdate);
//protected:
	short m_nType;
	LongPoint m_lpt;
	Coord m_Radius;
	CEdge* m_pEdge;
};

#endif // !defined(AFX_SPECIALLINESKETCHER_H__C476E0BA_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
