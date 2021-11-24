// MDEllipse.h: interface for the CMDEllipse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDELLIPSE_H__CDB88B9D_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
#define AFX_MDELLIPSE_H__CDB88B9D_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDRectangle.h"

struct CEllipseTrackContext : public CRectTrackContext
{
	Angle rotAngle;
};

class CMDEllipse : public CMDRectangle  
{
	DECLARE_SERIAL(CMDEllipse)
public:
	CMDEllipse() {}
	CMDEllipse(RectangleType nType) : CMDRectangle(nType) {}

// datum
	int DefaultDatumCode() { return kCenterDatum; }
// radial shape
	bool IsRadial() const { return true; }
	LongPoint GetCenter() const;
	LongPoint GetRadii() const;
// edges
	int	GetNumShapeEdges() const { return 1; }
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// unglue
	bool CanUnglue() const { return false; }
// handles
	bool CanAddPt(CMDShape** ppNewShape) const { return false; }
	bool CanRemovePt() const { return false;}
// new tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
// area
	bool HaveArea() const { return true; }
	double GetRawArea(const LongRatio& ratio) const;
// report
	double ReportCalcPerimeter(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
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
};

#endif // !defined(AFX_MDELLIPSE_H__CDB88B9D_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
