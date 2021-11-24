// MDBezierShape.h: interface for the CMDBezierShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDBEZIERSHAPE_H__CDB88B95_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
#define AFX_MDBEZIERSHAPE_H__CDB88B95_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDPolygonShape.h"

class CMDBezierShape : public CMDPolygonShape  
{
	DECLARE_SERIAL(CMDBezierShape)
public:
	CMDBezierShape() {}
	CMDBezierShape(int nNumPoints);

// edges
	int	GetNumShapeEdges() const;
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	int	 TestHandles(const pointd& pt, CMapper* pMapper) const;
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// unglue
	void Unglue(CMDShapeList* pList) const;
// handles
	bool CanAddPt(CMDShape** ppNewShape)  const{ return true;}
	int  AddPt(const pointd& lpt, CMapper* pMapper);
	bool CanRemovePt()  const{ return (m_nCount>4);}
	void RemovePt(int nHandle, CMDShape** ppNewShape);
// tracking
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext) {}
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
// area
	bool HaveArea() const { return true; }
	double GetRawArea(const LongRatio& ratio) const;
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);
};

#endif // !defined(AFX_MDBEZIERSHAPE_H__CDB88B95_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
