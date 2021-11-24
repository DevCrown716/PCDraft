// MDRadialDim.h: interface for the CMDRadialDim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDRADIALDIM_H__1DA82653_EC51_11D2_B5B8_9ABF4588277F__INCLUDED_)
#define AFX_MDRADIALDIM_H__1DA82653_EC51_11D2_B5B8_9ABF4588277F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDAssocDim.h"
#include "lpoint.h"	// Added by ClassView

struct RadialDimPoints : public DimPoints
{
	RadialDimPoints(const DimStandard* dimStd=NULL,const DimFormat* dimFmt=NULL) : DimPoints(dimStd,dimFmt) {}
	RadialDimPoints(const LongPoint& center,Coord radius,const LongPoint anchor,const DimStandard* dimStd,const DimFormat* dimFmt): DimPoints(center,anchor,dimStd,dimFmt), Radius(radius) {}

	Coord Radius;
};

struct CRadialDimTrackContext : public CTrackContext
{
	~CRadialDimTrackContext();
	RadialDimPoints rdp;
//	LongPoint pt[2];
//	Coord radius;
	CMDDimStr* pDimStr;
};

class CMDRadialDim : public CMDAssocDim  
{
	DECLARE_SERIAL(CMDRadialDim)
public:
	CMDRadialDim() {};
	CMDRadialDim(const DimStandard& DimStd, const DimFormat& DimFmt, BYTE nCap1, BYTE nCap2, bool bDiameter, const LongPoint& lptCenter, Coord radius, const LongPoint& lptAnchor);

	void Serialize(CArchive& ar);

// extents
	LongRect CalculateBasicExtents() const;
// datum
	int DefaultDatumCode() { return kCenterDatum; }
	void CalcCenterDatum(LongPoint& lpt);
// shape points
	int GetNumShapePoints() const { return 8; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint& pt);
// shape handles
	int GetNumShapeHandles() const { return 2; }
	LongPoint GetShapeHandle(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
	void DoShapeRotate(Angle angle, short nAbout, const LongPoint& lptRef);
	void UpdateDependentShapes(CPCDraftView* pView);
	void ReCalc(CMapper* pMapper);
// unglue
	void Unglue(CMDShapeList* pList) const;
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
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
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);

	const DimPoints* GetDimPoints() const { return &m_rdp; }
	void CalcPoints(CMapper* pMapper);
static void CalcPoints(bool bDiameter,RadialDimPoints& rdp,CMapper* pMapper);
static void CalcRadius(RadialDimPoints& rdp,CMDShape* pShape);

public:
	bool m_bDiameter;

protected:
	RadialDimPoints m_rdp;
//	Coord m_Radius;
//	LongPoint m_lptCenter;
//	LongPoint m_lptAnchor;
};

#endif // !defined(AFX_MDRADIALDIM_H__1DA82653_EC51_11D2_B5B8_9ABF4588277F__INCLUDED_)
