// MDParallelPolygon.h: interface for the CMDParallelPolygon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDPARALLELPOLYGON_H__ABF80825_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_)
#define AFX_MDPARALLELPOLYGON_H__ABF80825_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDPolygonShape.h"

class CMDParallelPolygon : public CMDPolygonShape  
{
	DECLARE_SERIAL(CMDParallelPolygon)
public:
	CMDParallelPolygon() {}
	CMDParallelPolygon(int nNumPoints,bool bClosed,const ParallelAttrs& Attrs) : CMDPolygonShape(kParallel,nNumPoints,bClosed) { m_Attrs=Attrs; }

	virtual void Serialize(CArchive& ar);

// extents
	LongRect CalculateBasicExtents() const;
// parallel attrs
	bool IsParallel() const { return true; }
	const ParallelAttrs& GetParallelAttrs() const { return m_Attrs; }
	void SetParallelAttrs(const ParallelAttrs& Attrs) { m_Attrs=Attrs; }
// edges
	int	GetNumShapeEdges() const { return 0; }
	CEdge* GetShapeEdgeFromPt(const pointd &pt,const CMapper* pMapper) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
	void DrawShapeOutlineForFill(CDraftContext* pDC) const { CMDPolygonShape::DrawShapeOutline(pDC); }
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeScale(const LongRatio& ratio,double factor=0);
// unglue
	bool CanUnglue() const { return true; }
	void Unglue(CMDShapeList* pList) const;
// handles
	void RemovePt(int nHandle, CMDShape** ppNewShape);
// tracking
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
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
	CMDShape* ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex);
// Copiing
	// by TSM
	virtual void ProcessShapesForCopy(CVersionDoc *pDoc, int* used);
	virtual void ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int* used);
   virtual void ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used);
	// end by TSM

	void MakeSelfContained(CPCDraftDoc* pDoc,bool CopyDIB = false);
protected:
	ParallelAttrs m_Attrs;

	void CalcPoints(LongPointArray *pt) const;
};

#endif // !defined(AFX_MDPARALLELPOLYGON_H__ABF80825_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_)
