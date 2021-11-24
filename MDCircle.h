// MDCircle.h: interface for the CMDCircle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDCIRCLE_H__CDB88B9C_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
#define AFX_MDCIRCLE_H__CDB88B9C_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDFillShape.h"
#include "lpoint.h"	// Added by ClassView

struct CCircleTrackContext : public CTrackContext
{
	LongPoint pt[3];
};

class CMDCircle : public CMDFillShape  
{
	DECLARE_SERIAL(CMDCircle)
public:
	enum Mode { kDiameter, kRadius, kThreePoint };

	CMDCircle();
	CMDCircle(int nType);

	void Serialize(CArchive& ar);

// flags
	bool IsClosed() const { return true; }
// datum
	void CalcCenterDatum(LongPoint& lpt) { lpt=GetCenter(); }
// extents
	LongRect CalculateBasicExtents() const;
// radial shape
	virtual bool IsRadial() const { return true; }
	virtual LongPoint GetCenter() const;
	virtual LongPoint GetRadii() const;
// shape points
	int GetNumShapePoints() const;
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
	int TestHandles(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
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
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);

protected:
	int	m_nType;
	LongPoint m_pt[3];

	friend class COffsetCircleSketcher;
};

#endif // !defined(AFX_MDCIRCLE_H__CDB88B9C_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
