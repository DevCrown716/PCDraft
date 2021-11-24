#ifndef __MDARCSHAPE_H__
#define __MDARCSHAPE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "MDFillShape.h"

struct CArcTrackContext : public CTrackContext
{
	LongPoint pt[4];
	int nArcDirection;
	LongPoint ptMouse;
	Angle rotAngle;
};

class CMDArcShape : public CMDFillShape
{
	DECLARE_SERIAL(CMDArcShape)
public:
	enum { kRadius, kElliptic, kThreePoint };

	CMDArcShape() : m_nMode(kRadius) {}
	CMDArcShape(int nMode) : m_nMode(nMode) {}

	void Serialize(CArchive& ar);

// extents
	LongRect CalculateBasicExtents() const;
// datum
	void CalcCenterDatum(LongPoint& lpt) { lpt=GetCenter(); }
// radial shape stubs
	bool IsRadial() const { return true; }
	LongPoint GetCenter() const;
	LongPoint GetRadii() const;
// shape points
	int GetNumShapePoints() const { return m_nMode==kElliptic?4:3; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint &lpt);
	LongPoint GetFirstShapePoint() const { return GetShapePoint((m_nMode==kThreePoint)?0:1); }
	LongPoint GetLastShapePoint() const { return GetShapePoint(2); }
// edges
	int	GetNumShapeEdges() const { return 1; }
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
	void DrawShapeOutlineForFill(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
	int  GetNumShapeHandles() const;
	LongPoint GetShapeHandle(int nIndex) const;
	int	TestHandles(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// area
	bool HaveArea() const { return true; }
	double GetRawArea(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
	void GetResizeValue(ResizeValue* pValues,int& nHandle);
	void SetResizeValue(ResizeValue* pValues,int nHandle=0);
// dxf
	void WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;
	static void GetArcInfo(int nMode, const LongPoint pt[], int nArcDirection, LongPoint* ptCenter, LongPoint* radii, Angle* nStartAngle, Angle* nEndAngle,Angle nRotAngle=0);
	static LongPoint SnapPtToEllipse(LongPoint ptCenter, LongPoint ptRadii, LongPoint pt);
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);
// Linked Vertex
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);

public:
	int m_nMode;
	int m_nArcDirection;
	LongPoint m_pt[4];	// elliptic uses 4 points
	// have no time to think deep.
	LongPoint m_ptCenterHandle;
};

#endif
