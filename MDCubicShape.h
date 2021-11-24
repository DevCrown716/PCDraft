// MDCubicShape.h: interface for the CMDCubicShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDCUBICSHAPE_H__10B494B7_C9AA_11D2_B574_C0D96B5E0F7F__INCLUDED_)
#define AFX_MDCUBICSHAPE_H__10B494B7_C9AA_11D2_B574_C0D96B5E0F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDPolygonShape.h"

class CMDCubicShape : public CMDPolygonShape  
{
	DECLARE_SERIAL(CMDCubicShape)
public:
	CMDCubicShape() {}
	CMDCubicShape(int nNumPoints,bool bClosed);

// extents
	LongRect CalculateBasicExtents() const;
// edges
	int	GetNumShapeEdges() const;
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// unglue
	bool CanUnglue()  const{ return true; }
	void Unglue(CMDShapeList* pList) const;
// handles
	bool CanAddPt(CMDShape** ppNewShape)  const{ return true;}
	int  AddPt(const pointd& lpt, CMapper* pMapper);
// tracking
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

#endif // !defined(AFX_MDCUBICSHAPE_H__10B494B7_C9AA_11D2_B574_C0D96B5E0F7F__INCLUDED_)
