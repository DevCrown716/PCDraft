// MDAngularDim.h: interface for the CMDAngularDim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDANGULARDIM_H__3F26AF65_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
#define AFX_MDANGULARDIM_H__3F26AF65_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDAssocDim.h"

struct AngDimPoints : public DimPoints
{
	AngDimPoints(const DimStandard* dimStd=NULL,const DimFormat* dimFmt=NULL) : DimPoints(dimStd,dimFmt) {}
	AngDimPoints(const LongPoint& from,const LongPoint& to,const LongPoint& center,const LongPoint anchor,const DimStandard* dimStd,const DimFormat* dimFmt): DimPoints(from,anchor,dimStd,dimFmt), lptTo(to), lptCenter(center) {}

	LongPoint lptTo;
	LongPoint lptCenter;

	LongPoint lptWit1Start;
	LongPoint lptWit1End;
	LongPoint lptWit2Start;
	LongPoint lptWit2End;

	int nDirection;
};

struct CAngDimTrackContext : public CTrackContext
{
//	LongPoint pt[4];
	~CAngDimTrackContext();
	AngDimPoints adp;
	bool bSmallerArc;
	CMDDimStr* pDimStr;
};

class CMDAngularDim : public CMDAssocDim  
{
	DECLARE_SERIAL(CMDAngularDim)
public:
	CMDAngularDim() {}
	CMDAngularDim(const DimStandard& DimStd,const DimFormat& DimFmt,BYTE nCap1=6,BYTE nCap2=6,bool bSmallerArc=true);

	void Serialize(CArchive& ar);

// extents
	LongRect CalculateBasicExtents() const;
// shape points
	int GetNumShapePoints() const { return 13; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint& pt);
// shape handles
	int GetNumShapeHandles() const { return 2; }
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void UpdateDependentShapes(CPCDraftView* pView);
// unglue
	bool CanUnglue() const { return true; }
	void Unglue(CMDShapeList* pList) const;
// operations
	void ReCalc(CMapper* pMapper);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
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
	virtual	void GetVertexesFromPt(CUIntArray& vertexes,CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);
	void OnUpdateLineLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex1,int nCauseVertex2);

	const DimPoints* GetDimPoints() const { return &m_adp; }
	void CalcPoints(CMapper* pMapper);
static void CalcPoints(bool bSmallerArc,AngDimPoints& adp,CMapper* pMapper);

   bool TestSelect(const pointd& pt, CMapper* pMapper) const; // Infinisys Ltd. 2021

protected:
	bool m_bSmallerArc;
	AngDimPoints m_adp;
//	LongPoint m_lpt[4];
};

#endif // !defined(AFX_MDANGULARDIM_H__3F26AF65_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
