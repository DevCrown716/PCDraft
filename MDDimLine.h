// MDDimLine.h: interface for the CMDDimLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDDIMLINE_H__9B818B43_DA1B_11D3_9864_000000000000__INCLUDED_)
#define AFX_MDDIMLINE_H__9B818B43_DA1B_11D3_9864_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDLineShape.h"

class CMDDimStr;

class CMDDimLine : public CMDLineShape  
{
	DECLARE_SERIAL(CMDDimLine)
public:
	CMDDimLine();
	CMDDimLine(const LongPoint& lptFrom,const LongPoint& lptTo,short nTextDisplay=0);
	virtual ~CMDDimLine();

	void Serialize(CArchive& ar);

// extents
	void CalculateExtents();
// pen model
	void SetPenModel(const PenModel& pm);
	bool ModifyPenModel(const PenModel &pm, int nWhat);
// fill model
	void SetFillModel(const FillModel& fm);
// text params
	bool HaveTextParams(int nWhat) const { return (nWhat&tpUseFont) && m_pAttached; }
	const TextParams& GetTextParams() const;
	void ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC);
// hit-testing
	bool PointInFill(const pointd& pt, CMapper* pMapper) const;
// handles
	bool CanAddPt(CMDShape** ppNewShape) const { return false; }
// operations
	void MakeSelfContained(CPCDraftDoc* pDoc,bool CopyDIB=false);
	void UpdateDependentShapes(CPCDraftView* pView);
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


	short m_nTextDisplay;
};

#endif // !defined(AFX_MDDIMLINE_H__9B818B43_DA1B_11D3_9864_000000000000__INCLUDED_)
