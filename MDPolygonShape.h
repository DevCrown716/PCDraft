// MDPolygonShape.h: interface for the CMDPolygonShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDPOLYGONSHAPE_H__CDB88B94_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
#define AFX_MDPOLYGONSHAPE_H__CDB88B94_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDFillShape.h"
#include "lpoint.h"	// Added by ClassView

struct CPolyTrackContext : public CTrackContext
{
	CArray<LongPoint,const LongPoint&> pt;
};

class CLinkVertexList;

class CMDPolygonShape : public CMDFillShape  
{
	DECLARE_SERIAL(CMDPolygonShape)
public:
	enum PolygonType
	{
		kFreehand,
		kPolygon,
		kFreeSpline,
		kPolySpline,
		kBezier,
		kCubic,
		kParallel
	};
	
	CMDPolygonShape() : m_nType(kPolygon) {}
	CMDPolygonShape(PolygonType nType,int nNumPoints,bool bClosed);

	void Serialize(CArchive& ar);

// flags
	bool IsClosed() const { return m_bClosed; }
// extents
	LongRect CalculateBasicExtents() const;
	void ExtendForPen(LongRect& lrcExtent) const;
// shape points
	int	 GetNumShapePoints() const { return m_nCount; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint &lpt);
// shape handles
	int GetNumShapeHandles() const;
	LongPoint GetShapeHandle(int nIndex) const;
// edges
	int	GetNumShapeEdges() const;
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
//	int	 TestHandles(const pointd& pt, CMapper* pMapper) const;
// smooth/unsmooth
	bool CanSmooth() const;
	void Smooth();
	bool CanUnsmooth() const;
	void Unsmooth();
// unglue
	bool CanUnglue()  const{ return true; }
	void Unglue(CMDShapeList* pList) const;
// handles
	bool CanAddPt(CMDShape** ppNewShape)  const{ return true;}
	int  AddPt(const pointd& lpt, CMapper* pMapper);
	bool CanRemovePt()  const{ return (m_nCount>2);}
	void RemovePt(int nHandle, CMDShape** ppNewShape);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// area
	bool HaveArea() const { return m_nType<kBezier || m_nType>kCubic; }
	double GetRawArea(const LongRatio& ratio) const;
// report
	double ReportCalcPerimeter(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
	void GetResizeValue(ResizeValue* pValues,int& nHandle);
	void SetResizeValue(ResizeValue* pValues,int nHandle=0);
	int	TestResizeHandles(const pointd& pt, CMapper* pMapper) const { return TestHandles(pt,pMapper); }
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
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);
	void UpdateVertexes(CLinkVertexList* pVertexList,int nCauseVertex);

	void SetClosed(bool closed);
	void SetNumShapePoints(int numPoints);
	void SetPolygonType(PolygonType type);
	PolygonType GetPolygonType() { return m_nType; }

protected:
	void TrackAnnot(CDraftContext* pDC, CPolyTrackContext* p);
	PolygonType m_nType;
	bool m_bClosed;
	int m_nCount;
	LongPointArray m_pt;

static LongPoint CuVertex(const LongPoint& lpt0,const LongPoint& lpt1,const LongPoint& lpt2);
static LongPoint BezCtrlPt(const LongPoint& lpt0,const LongPoint& lpt1);
static LongPoint BezVertex(const LongPoint& lpt0,const LongPoint& lpt1);
static double BezEdge(const LongRatio& ratio,const LongPoint& lpt1,const LongPoint& lpt2,const LongPoint& lpt3,const LongPoint& lpt4);
};

#endif // !defined(AFX_MDPOLYGONSHAPE_H__CDB88B94_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
