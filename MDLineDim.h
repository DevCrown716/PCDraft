// MDLineDim.h: interface for the CMDLineDim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDLINEDIM_H__3F26AF64_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
#define AFX_MDLINEDIM_H__3F26AF64_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDAssocDim.h"

struct LineDimPoints : public DimPoints
{
	LineDimPoints(const DimStandard* dimStd=NULL,const DimFormat* dimFmt=NULL) : DimPoints(dimStd,dimFmt) {}
	LineDimPoints(const LongPoint& from,const LongPoint& to,const LongPoint& anchor,const DimStandard* dimStd,const DimFormat* dimFmt) : DimPoints(from,anchor,dimStd,dimFmt), lptTo(to) {}

	LongPoint lptTo;

	LongPoint lptWit1Start;
	LongPoint lptWit1End;
	LongPoint lptWit2Start;
	LongPoint lptWit2End;
};

struct CLineDimTrackContext : public CTrackContext
{
	CLineDimTrackContext(const DimStandard* pDimStd=NULL,const DimFormat* pDimFmt=NULL) : ldp(pDimStd,pDimFmt), angle(0), pDimStr(NULL) {}
	~CLineDimTrackContext(); 

	LineDimPoints ldp;
	Angle angle;
	CMDDimStr* pDimStr;
};

class CMDLineDim : public CMDAssocDim  
{
	DECLARE_SERIAL(CMDLineDim)
public:
	virtual WrFixed GetDimValue();

	enum Mode { kHoriz, kVert, kSlope, kPerp };

	CMDLineDim() : m_nType(kHoriz) {}
	CMDLineDim(const DimStandard& DimStd,const DimFormat& DimFmt,BYTE nCap1=6,BYTE nCap2=6,int nType=kHoriz,Angle angle=0);

	void Serialize(CArchive& ar);

// extents
	LongRect CalculateBasicExtents() const;
// shape points
	int GetNumShapePoints() const { return 12; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint& lpt);
// shape handles
	int GetNumShapeHandles() const { return 2; }
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeRotate(Angle angle, short nAbout, const LongPoint& lptRef);
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
	void UpdateDependentShapes(CPCDraftView* pView);
	void ReCalc(CMapper* pMapper);
// unglue
	bool CanUnglue() const { return true; }
	void Unglue(CMDShapeList* pList) const;
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
	void OnUpdateLineLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex1,int nCauseVertex2);

	const DimPoints* GetDimPoints() const { return &m_ldp; }

	bool CalcPoints(CMapper* pMapper);
static bool CalcPoints(bool bHoriz,LineDimPoints& ldp,CMapper* pMapper);
static bool CalcPoints(int nType,Angle angle,LineDimPoints& ldp,CMapper* pMapper);

public:
	int m_nType;

protected:
	Angle m_Angle;
	LineDimPoints m_ldp;
//	LongPoint m_lpt[3];
};

#endif // !defined(AFX_MDLINEDIM_H__3F26AF64_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
