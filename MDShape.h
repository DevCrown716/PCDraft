#ifndef __MDSHAPE_H__
#define __MDSHAPE_H__

#include "dsshape.h"
#include "lpoint.h"	// Added by ClassView
#include "phmath.h"
#include "dxf.h"
#include "PackedDIB.h"
#include <gdiplus.h>

#if _MSC_VER >= 1000
#pragma once
#endif

class CMDShape;
typedef CTypedPtrList<CObList,CMDShape*> CMDShapeList;
typedef CTypedPtrArray<CObArray,CMDShape*> CMDShapeArrayBase;
// begin MD6.2.1 - Infinisys Ltd.
typedef CArray<unsigned __int64> UInt64ArrayBase;
class UInt64Array :
   public UInt64ArrayBase
{
public:

   int Find(unsigned __int64 ui)
   {
      for(int i = 0; i < GetCount(); i++)
      {
         if(ElementAt(i) == ui)
         {
            return i + 1;
         }
      }
      return 0;
   }
};
// end MD6.2.1 - Infinisys Ltd.
class CMDDimStr;
// begin MD6.2.1 - Infinisys Ltd.
class CMDShapeArray:
   public CMDShapeArrayBase
{
public:

   CMDShapeArray()
   {
      SetSize(0,2048);
   }

   POSITION GetTailPosition()
   {

      return (POSITION) GetCount();

   }

   POSITION GetHeadPosition()
   {

      if(GetCount() <= 0)
      {

         return (POSITION)NULL;

      }

      return (POSITION) 1;

   }

   CMDShape * GetPrev(POSITION & pos)
   {
      INT_PTR i = ((INT_PTR)pos) - 1;
      if(i <= 0)
      {
         pos = NULL;
      }
      else
      {
         ((INT_PTR &)pos)--;
      }
      return ElementAt(i);

   }
   CMDShape * GetNext(POSITION & pos)
   {
      INT_PTR i = ((INT_PTR)pos) - 1;
      if(i == GetUpperBound())
      {
         pos = NULL;
      }
      else
      {
         ((INT_PTR &)pos)++;
      }
      return ElementAt((INT_PTR)i);

   }

   void AddHead(CMDShape * pshape)
   {
      InsertAt(0,pshape);
   }
   void AddTail(CMDShape * pshape)
   {
      Add(pshape);
   }
   void AddTail(CMDShapeList * plist)
   {
      POSITION pos = plist->GetHeadPosition();
      while(pos != NULL)
      {
         Add(plist->GetNext(pos));
      }

   }
   void AddHead(CMDShapeList * plist)
   {
      POSITION pos = plist->GetTailPosition();
      while(pos != NULL)
      {
         AddHead(plist->GetPrev(pos));
      }

   }
   void InsertAfter(POSITION pos,CMDShape * pshape)
   {
      ASSERT(pos != 0);
      InsertAt((INT_PTR) pos,pshape);
   }
   void InsertBefore(POSITION pos,CMDShape * pshape)
   {
      ASSERT(pos != 0);
      InsertAt(((INT_PTR)pos)-1,pshape);
   }

   void RemoveAt(POSITION pos)
   {
      CMDShapeArrayBase::RemoveAt(((INT_PTR)pos) - 1);
   }
   CMDShape * GetTail()
   {
      return ElementAt(GetUpperBound());
   }
   CMDShape * RemoveTail()
   {
      CMDShape * pShape = ElementAt(GetUpperBound());
      CMDShapeArrayBase::RemoveAt(GetUpperBound());
      return pShape;
   }

   CMDShape * RemoveHead()
   {
      CMDShape * pShape = ElementAt(0);
      CMDShapeArrayBase::RemoveAt(0);
      return pShape;
   }

   POSITION Find(CMDShape * pshape)
   {
      for(INT_PTR i = 0; i < m_nSize; i++)
      {
         if(pshape == (CMDShape *) m_pData[i])
         {
            return (POSITION)(i + 1);
         }
      }
      return NULL;
   }


};
// end MD6.2.1 - Infinisys Ltd.

class CEdge;
class CBezierEdge;
class CMapper;
class CDraftContext;
class CPCDraftDoc;
class CVersionDoc;
class CPCDraftView;
class CMDReportRow;
struct PhIOContext;

// Infinisys Ltd
namespace format55
{
   struct PhIOContext;
}
// MD6.2.1 - Infinisys Ltd.
namespace formatMD60
{
   struct PhIOContext;
}

// MD7.0- Infinisys Ltd.
namespace formatMD70
{
   struct PhIOContext;
}

#define pi 3.1415926535897932

const int kMoveHandle = -1;
const int kRotateHandle = -2;
const int kCenterHandle = -3;

#define	kNEndcaps			16
#define kNPenWidths			13
short	getEndcapIndex	(short	endcap,short	penWidth);
extern CDrawPrim::EndcapRec	gEndcaps[kNEndcaps * kNPenWidths];

#define		kNoValue			0x00
#define		kDoubleValue		0x01
#define		kLongValue			0x02
#define		kAngleValue			0x03
#define		kUnitlessValue		0x04

struct ResizeValue
{
	double	m_Double;
	long	m_Long;
	Angle	m_Angle;
	WrFixed	m_Unitless;
};

struct ResizeInfo
{
	int nTitle;
	int nParams;
	struct {
		int nName;
		int nType;
	} params[6];
};

struct CTrackContext
{
			 CTrackContext();
	virtual ~CTrackContext();

	int nWhichHandle;
	LPARAM lParam;
	CPCDraftView* pView;
	bool bSwappedVertexes;
	bool bShift;
	bool bOption;
};

enum
{
	kAboutCenter=0,
	kAboutDatum,
	kAboutRefPt
};

struct CRotateContext
{
	Angle m_Angle;
	short m_nAbout;
	LongPoint m_ptOrigin;
};

class CLayer;
class CLinkVertexList;
class CLinkVertexContext;
class CPackedItem;

class CMDShape : public CObject  
{
	DECLARE_SERIAL(CMDShape)
public:
	enum
	{
		kMaxAttrib = 5
	};

	CMDShape();
	virtual ~CMDShape() {};

	virtual void	Serialize(CArchive& ar);

// flags
	bool IsLocked() const { return m_bLocked; }
	void SetLocked(bool bLocked) { m_bLocked=bLocked; }
	virtual bool IsClosed() const { return false; }
// datum
	virtual int DefaultDatumCode() { return 0; }
	virtual void CalcDefaultDatum(LongPoint& lpt);
	virtual void CalcCenterDatum(LongPoint& lpt);
	virtual LongPoint GetShapeDatum() const;
	virtual void SetShapeDatum(short nDatum);
	virtual void SetShapeDatum(const LongPoint& lptDatum);
	void ValidateDatum();
// extents
	virtual LongRect CalculateBasicExtents() const;
	virtual void ExtendForPen(LongRect& lrcExtent) const;
	virtual void CalculateExtents();
	virtual const LongRect& GetExtents() const { return m_lrcExtents; }
	virtual void SetExtents(const LongRect& lrcExtents) { m_lrcExtents=lrcExtents; }
// pen model
	virtual const PenModel& GetPenModel() const { return m_PenModel; }
	virtual void SetPenModel(const PenModel& penModel) { ModifyPenModel(penModel,pmUseAll); }
	virtual bool ModifyPenModel(const PenModel &penModel, int nWhat);
	virtual void SetPenModelDirect(const PenModel& penModel) { m_PenModel=penModel; }
// fill model stubs
	virtual bool IsFilled() const { return false; }
	virtual const FillModel& GetFillModel() const;
	virtual void SetFillModel(const FillModel& fillModel) {}
	virtual bool GetFillRgn(CRgn* pRgn, CDraftContext* pDC) const { return false; }
   virtual void ModifyFillModel(const FillModel& fillModel,int nWhat);

// Opacity/Transparency - Infinisys Ltd
   virtual void SetFillOpacity(int iOpacity) {}
   virtual bool GetFillOpacity(int & iOpacity) const { return false; }
// parallel attrs stubs
	virtual bool IsParallel() const { return false; }
	virtual const ParallelAttrs& GetParallelAttrs() const;
	virtual void SetParallelAttrs(const ParallelAttrs& Attrs) {}
// text params stubs
	virtual bool HaveTextParams(int nWhat) const { return false; }
	virtual const TextParams& GetTextParams() const;
	virtual void SetTextParams(const TextParams& tp) {}
	virtual void ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC) {}
// radial shape stubs
	virtual bool IsRadial() const { return false; }
	virtual LongPoint GetCenter() const { return LongPoint::kZeroPt; }
	virtual LongPoint GetRadii() const { return LongPoint::kZeroPt; }
// shape points
	virtual int GetNumShapePoints() const { return 0; }
	virtual LongPoint GetShapePoint(int nIndex) const { return LongPoint::kZeroPt; }
	virtual void SetShapePoint(int nIndex, const LongPoint& pt) {}
	virtual int GetShapePointIndex(const LongPoint& pt);
	virtual LongPoint GetFirstShapePoint() const { return GetShapePoint(0); }
	virtual LongPoint GetLastShapePoint() const { return GetShapePoint(GetNumShapePoints()-1); }
// shape handles
	virtual int GetNumShapeHandles() const { return GetNumShapePoints(); }
	virtual LongPoint GetShapeHandle(int nIndex) const { return GetShapePoint(nIndex); }
// edges
	virtual	int	GetNumShapeEdges() const { return 0; }
	virtual	CEdge* GetShapeEdge(int nIndex) const { return NULL; }
	virtual CEdge* GetShapeEdgeFromPt(const pointd &pt,const CMapper* pMapper) const;
// drawing
	virtual void DrawShape(CDraftContext* pDC) const {}
	virtual void DrawFill(CDraftContext* pDC) const {}
	virtual void DrawHandles(CDraftContext* pDC) const;
	virtual void DrawShapeOutline(CDraftContext* pDC) const {}
	virtual void DrawShapeOutlineForFill(CDraftContext* pDC) const {}
// hit-testing
	virtual bool TestSelect(const pointd& pt, CMapper* pMapper) const;
	virtual bool PointInShape(const pointd& pt, CMapper* pMapper) const { return false; }
	virtual bool PointInFill(const pointd& pt, CMapper* pMapper) const { return false; }
	virtual	int	TestHandles(const pointd& pt, CMapper* pMapper) const;
// operations
	virtual void DoShapeOffset(const LongPoint& delta);
	virtual void DoShapeMove(const LongPoint& pos);
	virtual void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	virtual void DoShapeScale(const LongRatio& ratio,double factor=0);
	virtual void DoShapeRotate(Angle angle, short nAbout, const LongPoint& lptRef);
	virtual void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	virtual void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
	virtual void UpdateDependentShapes(CPCDraftView* pView);
	virtual void MakeSelfContained(CPCDraftDoc* pDoc,bool CopyDIB=false);
	virtual void ReCalc(CMapper* pMapper) {}

	CMDShape* CreateCopy()  const;
	virtual int Snap(LongPoint* lptSnap,const LongRect& lrcProx, short nType) const;

// rotation angle	
	Angle GetRotation() const { return m_RotAngle; }
	void SetRotation(Angle RotAngle) { m_RotAngle=RotAngle; }
// shape attributes
	void GetValues(CString* sValues) const;
	void SetValues(CString* sValues);
	void GetValues(CString& sValue1,CString& sValue2,CString& sValue3,CString& sValue4,CString& sValue5) const;
	void SetValues(CString& sValue1,CString& sValue2,CString& sValue3,CString& sValue4,CString& sValue5);
// smooth/unsmooth
	virtual bool CanSmooth() const { return false; }
	virtual void Smooth() {}
	virtual bool CanUnsmooth() const { return false; }
	virtual void Unsmooth() {}
// unglue
	virtual bool CanUnglue() const { return false; }
	virtual	void Unglue(CMDShapeList* pList) const {}
// handles
	virtual bool CanAddPt(CMDShape** ppNewShape) const { return false; }
	virtual int	AddPt(const pointd& pt,CMapper* pMapper) { return -1; }
	virtual bool CanRemovePt() const { return false;}
	virtual void RemovePt(int nHandle, CMDShape** ppNewShape) {}
//	tracking
	bool DoTrackShape(int nWhichHandle, CPoint pt, CPCDraftView* pView, LPCRECT lpcRect /*=NULL*/);
	virtual CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper) { return NULL; }
	virtual void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext) {}
	virtual void TrackShowSize(CTrackContext* pTrackContext) {}
	virtual void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext) {}
	virtual void TrackComplete(CTrackContext* pTrackContext) {}
// area
	virtual bool HaveArea() const { return false; }
	virtual double GetRawArea(const LongRatio& ratio) const { return 0; }
	virtual void RemoveArea(CPCDraftView* pView);
	bool IsAreaShown() const;
	void ShowArea(bool bShow,CPCDraftView* pView);
// attached
	virtual CMDShape* GetAttachedShape() const;
	void DeleteAttached();
	void Attach(CMDDimStr* pShape);
	CMDDimStr* Detach();
// report
	virtual double ReportCalcWidth(const LongRatio& ratio) const;
	virtual double ReportCalcHeight(const LongRatio& ratio) const;
	virtual double ReportCalcLength(const LongRatio& ratio) const;
	virtual double ReportCalcPerimeter(const LongRatio& ratio) const;
	virtual double ReportCalcXDim(const LongRatio& ratio) const;
	virtual double ReportCalcYDim(const LongRatio& ratio) const;
	virtual double ReportCalcArea(const LongRatio& ratio) const;
	virtual long ReportCalcFieldTotal(short nFieldType);
	virtual bool ReportFitsCriteria(CMDReportRow* pCriteria,CMDReportRow* pFunction);
// resize
	virtual const ResizeInfo* GetResizeInfo() const { return NULL; }
	virtual void GetResizeValue(ResizeValue* pValues,int& nHandle);
	virtual void SetResizeValue(ResizeValue* pValues,int nHandle=0);
	virtual	int	TestResizeHandles(const pointd& pt, CMapper* pMapper) const;
// dxf
	virtual void WriteDXF(CFile* pFile, CString& strLayerName, LongRectPtr pDocExtPtr, double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette* ptrPalette) const;
// MacDraft
	virtual int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
// MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);


   void SaveMacObjInfo(PhIOContext& ctx);
   // Infinisys Ltd
   void SaveMacObjInfo(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   void SaveMacObjInfo(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   void SaveMacObjInfo(::formatMD70::PhIOContext& ctx);

	virtual int GetObjectCount() { return 1;}

// Copiing
	// by TSM
	virtual void ProcessShapesForCopy(CVersionDoc *pDoc, int* used);
	virtual void ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int* used);
   virtual void ProcessShapesAfterSymLibraryCopy(CPackedItem* pitem,int *used);
	// end by TSM


   // Library - PCDraft 6 - Infinisys Ltd.
   virtual void ProcessShapesForLibraryArchiving(CVersionDoc *pDocNew, CVersionDoc *pDoc,int* used);
   virtual void ProcessShapesFromLibraryArchiving(CPCDraftDoc *pDoc,CPCDraftDoc *pDocLib);
   // end Library - PCDraft 6 - Infinisys Ltd.

// Linked Vertex
	virtual void AddToLinkContext(CLinkVertexContext* pContext);
	virtual void RecalcLinkedVertex(CLinkVertexContext* pContext);
	virtual void OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex);
	virtual void OnUpdateLineLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex1,int nCauseVertex2);
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);
	virtual bool DeleteLinks(CLinkVertexList* pList);
	virtual void AdjustLinks(CLinkVertexList* pList,CMDShape* pShapeToReplace);
	virtual bool IsLinked(CLinkVertexList& list,int nLinkVertex, CMDShape** pLinkedShape) const;
	virtual CMDShape* ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex);
	void ProcessDeletedShapes(CLinkVertexList* pList);
	
	static BOOL TestHandle(const pointd& hpt,const pointd& pt);
	static double ScaleDist(const LongRatio& ratio,WrFixed dist);
	static void ScaleLPoint(const LongRatio& ratio,const LongPoint& lpt,DoublePoint& pt);
protected:
	void DxfWritePtCord (CFile* pFile,CString& pLayerName, LongPointPtr pPtPtr, double * pUnitFacPtr, short pDashIdx ) const;
	short DxfGetColorIndex (const PenModel* pPenModelPtr, CPalette* ptrPalette) const;
	void DxfWriteVertex (CFile* pFile, CString& pLayerStr, LongPoint* pPtPtr, double* pUnitFacPtr, double* /*pBulgePtr*/, short /*pFlags*/, short pDashIdx) const;
	void DxfWriteSeqend (CFile* pFile, CString&	pLayerStr) const;
	void DxfWritePolyHead (CFile* pFile,CString& pLayerStr, const PenModel* pPenModelPtr, LongRect*, double pUnitFactor, short pClosed, CPalette *ptrPalette) const;
	void DxfDrawEndcap (CFile* pFile,CString& pLayerStr, LongPoint* pPtr1, LongPoint* pPtr2, short pEndCap, unsigned short pMap, PenModel* pPenModelPtr,bool pIsCurve, Coord outsideLineLen, LongRect* pDocExtPtr, double pUnitFactor, int nZoomFactor, CPalette *ptrPalette) const;
	void DxfArrowEM (CFile* pFile, CString& pLayerStr, LongPoint* pPtr1, LongPoint* pPtr2, CDrawPrim::EndcapRec* pEndCapPtr, unsigned short pMap, short penWidth, PenModel* pPenModelPtr, bool pIsCurve, Coord outsideLineLen, LongRect* pDocExtPtr, double pUnitFactor, int nZoomFactor, CPalette *ptrPalette) const;
	void DxfXEM (CFile* pFile, CString& pLayerStr, LongPoint* pPtr1, LongPoint* pPtr2, short pWidth, unsigned short pMap, short penWidth, PenModel* pPenModelPtr, Coord outsideLineLen, LongRect* pDocExtPtr, double pUnitFactor, CPalette *ptrPalette) const;
	void DxfSlashEM (CFile* pFile, CString& pLayerStr, LongPoint* pPtr1, LongPoint* pPtr2, short pWidth, unsigned short pMap, short penWidth, PenModel* pPenModelPtr, Coord outsideLineLen, LongRect* pDocExtPtr, double	pUnitFactor, CPalette *ptrPalette) const;
	void DxfDotEM (CFile* pFile, CString& pLayerStr, LongPoint* pPtr1, LongPoint* pPtr2, short pWidth, unsigned short pMap, PenModel* pPenModelPtr, Coord outsideLineLen, LongRect* pDocExtPtr, double pUnitFactor, CPalette *ptrPalette) const;
	void DxfWriteCircle (CFile* pFile, CString& pLayerStr, LongPoint* pCenFdPtr, Coord pRadius, PenModel* pPenModelPtr, LongRect* pDocExtPtr, double pUnitFactor, CPalette *ptrPalette) const;
	void DxfOutsideDimLine( CFile* pFile, CString& pLayerStr, LongPoint* pPtr1, LongPoint* pPtr2, unsigned short pMap, Coord outsideLineLen, PenModel* pPenModelPtr, LongRect* pDocExtPtr, double pUnitFactor, CPalette *ptrPalette) const;
	void DxfWriteALine(CFile* pFile, CString& pLayerStr, PenModel* pPenPtr, LongPointPtr pLPt1Ptr, LongPointPtr pLPt2Ptr, LongRectPtr pDocExtPtr, double vUnitFactor, CPalette *ptrPalette) const;
	void DxfLineAdjust (LongPointPtr p1, LongPointPtr p2, Coord adjust1, Coord adjust2) const;
	void DxfWriteOneBezierSeg(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, CBezierEdge* pEdge) const;
	void DxfMakeButtCaps (short penWidth, LongPoint p1, LongPoint p2, LongPoint *newP1, LongPoint *newP2) const;
	void DxfLineBorderPos (PenModel *penModel, bool isPrinting, LongPointPtr p1, LongPointPtr p2, LongPointPtr newP1, LongPointPtr newP2, int nZoomFactor) const;
	void DxfLineCapped (LongPointPtr p1, LongPointPtr p2, PenModel *penModel, short whichCap, LongPointPtr newP1, LongPointPtr newP2, int nZoomFactor) const;
	bool DxfEquivColIndex (CPalette* ptrPalette1, short colIndex1, short *colIndex2) const;
	short DxfWidthConversion(short nPenW) const;
	void DxfGetArcPts (	CArray <LongPoint, LongPoint> &pPtHdl, LongPoint &pCenter, Coord pHRad, Coord pVRad, Angle pStartAng,Angle pEndAng, Angle pRot, bool pIsFine, PenModel*	penModel) const;
	void DxfArcLongPoint (Angle	angle, LongPoint *arcPt, LongPoint	&pCenter, Coord pHRad, Coord pVRad, TSinCos& fSinCosRot, Angle fLongRot) const;

	void NormalizePoints(LongPoint* lpt);

protected:
	PenModel m_PenModel;
	Angle m_RotAngle;
	short m_nDatum;
	LongPoint m_lptDatum;
	LongRect m_lrcExtents;
	CString m_sValues[kMaxAttrib];
	bool m_bLocked;
	CMDDimStr* m_pAttached;
static CMDShapeList gDeletedShapes;
static CMap<CMDShape*,CMDShape*,CMDShape*,CMDShape*> gReplacedShapes;
};

#endif // !defined(AFX_MDSHAPE_H__64213821_1360_11D2_980B_000000000000__INCLUDED_)
