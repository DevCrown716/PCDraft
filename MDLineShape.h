// MDLineShape.h: interface for the CMDLineShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDLINESHAPE_H__E2778594_141A_11D2_980C_000000000000__INCLUDED_)
#define AFX_MDLINESHAPE_H__E2778594_141A_11D2_980C_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MDShape.h"
#include "DrawPrim.h"

class CPCDraftView;

struct CLineTrackContext : public CTrackContext
{
	LongPoint pt[2];
};


class CMDLineShape : public CMDShape  
{
	DECLARE_SERIAL(CMDLineShape)
public:
	CMDLineShape() : m_nConAngle(0) {}
	CMDLineShape(const LongPoint& lptFrom,const LongPoint& lptTo,int nConAngle=0);

	void Serialize(CArchive& ar);

//	shape points
	int GetNumShapePoints() const { return 2; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint &lpt);
// edges
	int	GetNumShapeEdges() const { return 1; }
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// handles
	bool CanAddPt(CMDShape** ppNewShape) const;
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// report
	double ReportCalcLength(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
	void GetResizeValue(ResizeValue* pValues,int& nHandle);
	void SetResizeValue(ResizeValue* pValues,int nHandle=0);
//dxf
	void WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;
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
	int m_nConAngle;
	LongPoint m_pt[2];
};

#include "MDFiloPoly.h"


#endif // !defined(AFX_MDLINESHAPE_H__E2778594_141A_11D2_980C_000000000000__INCLUDED_)
