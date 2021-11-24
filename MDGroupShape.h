// MDGroupShape.h: interface for the CMDGroupShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDGROUPSHAPE_H__CDB88B93_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
#define AFX_MDGROUPSHAPE_H__CDB88B93_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDShape.h"

struct CGroupTrackContext : public CTrackContext
{
	LongRect lrc;
	LongPoint ptLastDelta;
};

class CMDGroupShape : public CMDShape
{
	DECLARE_SERIAL(CMDGroupShape)
public:
	CMDGroupShape() {}
	CMDGroupShape(CMDShape* pShape);
	CMDGroupShape(CMDShapeList* members);
	virtual ~CMDGroupShape();

	void Serialize(CArchive& ar);
	CMDShape* MemberFromPoint(const pointd& pt,CMapper* pMapper,bool bNoGroup=true);
	bool IsMember(CMDShape *pShape, bool bInsideGroups=false);

// datum
	int DefaultDatumCode() { return kFreeDatum; }
	void CalcDefaultDatum(LongPoint& lpt);
	void CalcCenterDatum(LongPoint& lpt) { lpt=Middle(m_lrcExtents); }
// extents
	LongRect CalculateBasicExtents() const;
	void ExtendForPen(LongRect& lrcExtent) const {}
	bool RecalcExtents(CMDShape* pShape);
// pen model
	bool ModifyPenModel(const PenModel &penModel, int nWhat);
// fill model stubs
	void SetFillModel(const FillModel& fm);
// Fill Model - Infinisys Ltd
   virtual const FillModel& GetFillModel() const;
   virtual void CalcCommonBestEffortFillModel();
// Pen Model - Infinisys Ltd
   virtual const PenModel& GetPenModel() const;
   virtual void CalcCommonBestEffortPenModel();
   virtual void ModifyFillModel(const FillModel& fillModel,int nWhat);
   virtual void SetFillOpacity(int iOpacity);
   virtual bool GetFillOpacity(int & iOpacity) const;

// shape handles
	int GetNumShapeHandles() const { return 8; }
	LongPoint GetShapeHandle(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
//	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
	bool PointInFill(const pointd& pt, CMapper* pMapper) const;
//	int	TestHandles(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeOffset(const LongPoint& delta);
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
	void DoShapeRotate(Angle angle, short nAbout,const LongPoint& lptRef);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
	void UpdateDependentShapes(CPCDraftView *pView);
	void MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB = false);
	void ReCalc(CMapper* pMapper);

	int Snap(LongPoint* lptSnap,const LongRect& lrcProx, short nType) const;

	void Ungroup(CMDShapeList* pList);
// area
	virtual void RemoveArea(CPCDraftView* pView);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// report
	double ReportCalcArea(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
	LongRect CalcResizeExtent();
	void GetResizeValue(ResizeValue* pValues,int& nHandle);
	void SetResizeValue(ResizeValue* pValues,int nHandle=0);
// dxf
	void WriteDXF(CFile* pFile, CString& strLayerName, LongRectPtr pDocExtPtr, double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);


// Linked Vertex
	virtual void AddToLinkContext(CLinkVertexContext* pContext);
	virtual void RecalcLinkedVertex(CLinkVertexContext* pContext);
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);
	virtual CMDShape* ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex);
	virtual bool DeleteLinks(CLinkVertexList* pList);
	virtual void AdjustLinks(CLinkVertexList* pList,CMDShape* pShapeToReplace);
	virtual bool IsLinked(CLinkVertexList& list,int nLinkVertex, CMDShape** pLinkedShape) const;

	int GetObjectCount();
	void AddMember(CMDShape* pShape);
	int GetMembersCount() { return (int) m_Members.GetCount(); }
	// by TSM
	virtual void ProcessShapesForCopy(CVersionDoc *pDoc, int* used);
	virtual void ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int* used);
   virtual void ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used);
	// end by TSM

   // Library - PCDraft 6 - Infinisys Ltd.
   virtual void ProcessShapesForLibraryArchiving(CVersionDoc *pDocNew,CVersionDoc *pDoc,int* used);
   virtual void ProcessShapesFromLibraryArchiving(CPCDraftDoc *pDoc,CPCDraftDoc *pDocLib);
   // end Library - PCDraft 6 - Infinisys Ltd.

protected:
	CMDShapeList m_Members;
   // Infinisys Ltd
   FillModel      m_fmCommonBestEffort;
};

#endif // !defined(AFX_MDGROUPSHAPE_H__CDB88B93_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
