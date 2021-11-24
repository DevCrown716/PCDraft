#ifndef __MDFILLSHAPE_H__
#define __MDFILLSHAPE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "MDShape.h"

class CMDFillShape : public CMDShape
{
	DECLARE_SERIAL(CMDFillShape)
public:
	CMDFillShape();

	void Serialize(CArchive& ar);

//	fill model
	bool IsFilled() const { return (m_FillModel.m_nFillType!=kNoFill); }
	const FillModel& GetFillModel() const { return m_FillModel; }
	void SetFillModel(const FillModel& fillModel) { m_FillModel=fillModel; }
	bool GetFillRgn(CRgn * pRgn, CDraftContext* pDC) const;
   // drawing
	void DrawFill(CDraftContext* pDC) const;
	void DrawShapeOutlineForFill(CDraftContext* pDC) const { DrawShapeOutline(pDC); }
// hit-testing
	bool PointInFill(const pointd& pt, CMapper* pMapper) const;
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);

   // Opacity/Transparency - Infinisys Ltd
   virtual void SetFillOpacity(int iOpacity);
   virtual bool GetFillOpacity(int & iOpacity) const;

	void MakeSelfContained(CPCDraftDoc* pDoc,bool CopyDIB=false);
// Copiing
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
	FillModel   m_FillModel;
};

#endif
