// MDAssocDim.h: interface for the CMDAssocDim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDASSOCDIM_H__3F26AF63_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
#define AFX_MDASSOCDIM_H__3F26AF63_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DimStructs.h"
#include "MDShape.h"

class CPCDraftView;
class CMDDimStr;

struct DimPoints
{
   DimPoints(const DimStandard* dimStd=NULL,const DimFormat* dimFmt=NULL): pDimStd(dimStd),pDimFmt(dimFmt),nDSAlign(0),angText(0) {}
   DimPoints(const LongPoint& from,const LongPoint& anchor,const DimStandard* dimStd,const DimFormat* dimFmt): lptFrom(from),lptAnchor(anchor),pDimStd(dimStd),pDimFmt(dimFmt),nDSAlign(0),angText(0) {}

	const DimStandard* pDimStd;
	const DimFormat* pDimFmt;

	LongPoint lptFrom;
	LongPoint lptAnchor;

	LongPoint lptDimStart;
	LongPoint lptDimEnd;
	LongPoint lptExtent;
	LongPoint lptLeader;

	LongPoint lptText;
	short nDSAlign;
	Angle angText;
};

class CMDAssocDim : public CMDShape //CMDFillShape  
{
	DECLARE_SERIAL(CMDAssocDim)
public:


   CMDAssocDim();
	CMDAssocDim(const DimStandard& DimStd,const DimFormat& DimFmt,BYTE nCap1=6,BYTE nCap2=6);
	virtual ~CMDAssocDim();

	void Serialize(CArchive& ar);

   void SaveMacDimLabel(PhIOContext& ctx);
   // Infinisys Ltd
   void SaveMacDimLabel(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   void SaveMacDimLabel(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   void SaveMacDimLabel(::formatMD70::PhIOContext& ctx);


// extents
	void ExtendForPen(LongRect& lrcExtent) const;
	void CalculateExtents();
// pen model
	void SetPenModel(const PenModel& pm);
	bool ModifyPenModel(const PenModel &pm, int nWhat);
	void SetPenModelDirect(const PenModel& penModel);
// fill model
	void SetFillModel(const FillModel& fm);
// text params
	bool HaveTextParams(int nWhat) const { return (nWhat&tpUseFont) && m_pDimStr; }
	const TextParams& GetTextParams() const;
	void ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC);
// drawing
	void DrawHandles(CDraftContext* pDC) const;
// hit-testing
	bool PointInFill(const pointd& pt, CMapper* pMapper) const;
	int	TestHandles(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeOffset(const LongPoint& delta);
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void MakeSelfContained(CPCDraftDoc* pDoc,bool CopyDIB = false);
	void UpdateDependentShapes(CPCDraftView* pView);
// unglue
	bool CanUnglue() const { return true; }
// report
	double ReportCalcWidth(const LongRatio& ratio) const { return 0; }
	double ReportCalcHeight(const LongRatio& ratio) const { return 0; }
	double ReportCalcLength(const LongRatio& ratio) const { return 0; }
	double ReportCalcPerimeter(const LongRatio& ratio) const { return 0; }
	double ReportCalcXDim(const LongRatio& ratio) const { return 0; }
	double ReportCalcYDim(const LongRatio& ratio) const { return 0; }
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

// Copiing
	// by TSM
	virtual void ProcessShapesForCopy(CVersionDoc *pDoc, int* used);
	virtual void ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int* used);
   virtual void ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used);
	// end by TSM

	void DrawDimStr(CDraftContext* pDC) const;
	void SetDimStr(CMDDimStr* pDimStr);
	virtual const DimPoints* GetDimPoints() const { return NULL; }

	DimStandard m_DimStd;
	DimFormat m_DimFmt;
	CMDDimStr* m_pDimStr;
	BYTE m_nDimCap1,m_nDimCap2;
};

#endif // !defined(AFX_MDASSOCDIM_H__3F26AF63_E93F_11D2_B5B0_8BF94E6E637F__INCLUDED_)
