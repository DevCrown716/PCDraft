#ifndef __MDRECTANGLE_H__
#define __MDRECTANGLE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "MDFillShape.h"

class CPCDraftView;

struct CRectTrackContext : public CTrackContext
{
	LongPoint pt[4];
};

class CMDRectangle : public CMDFillShape  
{
	DECLARE_SERIAL(CMDRectangle)
public:
	enum RectangleType
	{
		kRectCenter,
		kRectDiagonal
	};

	CMDRectangle();
	CMDRectangle(RectangleType nType);

	void Serialize(CArchive& ar);

// flags
	bool IsClosed() const { return true; }
// datum
	int DefaultDatumCode() { return m_nType==kRectCenter?kCenterDatum:0; }
// shape points
	int	GetNumShapePoints() const { return 4; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint &lpt);
// shape handles
	int GetNumShapeHandles() const { return 8; }
	LongPoint GetShapeHandle(int nIndex) const;
// edges
	int	GetNumShapeEdges() const { return 4; }
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
//	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
//	int	TestHandles(const pointd& pt, CMapper* pMapper) const;

// unglue
	bool CanUnglue() const { return true; }
	void Unglue(CMDShapeList* pList) const;
// handles
	bool CanAddPt(CMDShape **ppNewShape) const;
	bool CanRemovePt() const { return true;}
	void RemovePt(int nHandle, CMDShape** ppNewShape);
// operations
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
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
// report
	double ReportCalcWidth(const LongRatio& ratio) const;
	double ReportCalcHeight(const LongRatio& ratio) const;
	double ReportCalcLength(const LongRatio& ratio) const;
	double ReportCalcPerimeter(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
	void GetResizeValue(ResizeValue* pValues,int& nHandle);
	void SetResizeValue(ResizeValue* pValues,int nHandle=0);
// dxf
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
	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);

protected:
	RectangleType m_nType;
	LongPoint m_ptCorner[4];

	void AdjustScaleToRect(const LongRect& lrcFrom,LongRect& lrcTo);
};

#endif
