// MDCenterDim.h: interface for the CMDCenterDim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDCENTERDIM_H__3F26AF66_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
#define AFX_MDCENTERDIM_H__3F26AF66_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_

#include "lpoint.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDAssocDim.h"

struct CCenterDimTrackContext : public CTrackContext
{
	LongPoint pt;
	LongPoint radii;
};

class CMDCenterDim : public CMDAssocDim  
{
	DECLARE_SERIAL(CMDCenterDim)
public:
	CMDCenterDim() {}
	CMDCenterDim(const DimStandard& DimStd,const DimFormat& DimFmt,const LongPoint& lptCenter,const LongPoint radii);

	void Serialize(CArchive& ar);

// extents
	LongRect CalculateBasicExtents() const;
// datum
	void CalcCenterDatum(LongPoint& lpt) { lpt=m_lptCenter; }
// shape points
	int GetNumShapePoints() const { return 1; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint& pt);
// shape handles
	int GetNumShapeHandles() const { return 5; }
	LongPoint GetShapeHandle(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void ReCalc(CMapper* pMapper);
	void DoShapeScale(const LongRatio& ratio,double factor);
// unglue
	void Unglue(CMDShapeList* pList) const;
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);

// Linked Vertex
	void OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex);

protected:
	LongPoint m_lptCenter;
	LongPoint m_Radii;

	Coord m_centerLen, m_centerGap, m_centerExt;
};

#endif // !defined(AFX_MDCENTERDIM_H__3F26AF66_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
