// MDMetaPolyShape.h: interface for the CMDMetaPolyShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDMETAPOLYSHAPE_H__ABF80826_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_)
#define AFX_MDMETAPOLYSHAPE_H__ABF80826_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDFillShape.h"
#include "Edge.h"

struct CMetaTrackContext : public CTrackContext
{
	LongRect lrc;
};

class CMDMetaPolyShape : public CMDFillShape  
{
	DECLARE_SERIAL(CMDMetaPolyShape)
public:
	CMDMetaPolyShape();
	CMDMetaPolyShape(CEdgeList* pList);
	virtual ~CMDMetaPolyShape();

	void Serialize(CArchive& ar);

// flags
	bool IsClosed() const;
// datum
	int DefaultDatumCode() { return kFreeDatum; }
	void CalcDefaultDatum(LongPoint& lpt);
	void CalcCenterDatum(LongPoint& lpt) { lpt=Middle(m_lrcExtents); }
//	extents
	LongRect CalculateBasicExtents() const;
// shape points
	LongPoint GetFirstShapePoint() const;
	LongPoint GetLastShapePoint() const;
// shape handles
	int GetNumShapeHandles() const { return 4; }
	LongPoint GetShapeHandle(int nIndex) const;
// edges
	int	GetNumShapeEdges() const;
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
//	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
//	int	TestHandles(const pointd& pt, CMapper* pMapper) const;
//	operations
	void ReverseEdges();
	void DoShapeOffset(const LongPoint& delta);
	void DoShapeRotate(Angle angle, short nAbout,const LongPoint& lptRef);
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
	int Snap(LongPoint* lptSnap,const LongRect& lrcProx, short nType) const;
// unglue
	bool CanUnglue()  const{ return true; }
	void Unglue(CMDShapeList* pList) const;
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

	bool AddMember(CMDShape* pShape);

protected:
	CEdgeList m_Edges;
};

#endif // !defined(AFX_MDMETAPOLYSHAPE_H__ABF80826_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_)
