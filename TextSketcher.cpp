// TextSketcher.cpp: implementation of the CTextSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "TextSketcher.h"
#include "MDText.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "DraftContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

bool CTextSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);

	TEXTMETRIC tm;
	{
		CDC cdc;
		cdc.CreateCompatibleDC(NULL);
		CDraftContext dc(&cdc,&m_pView->m_Mapper);
		dc.SelectFont(m_pView->GetDocument()->m_TextParams.m_lf);
		dc.GetTextMetrics(&tm);
		dc.UnselectFont();
	}
	if(Abs(m_ptTo.x-m_ptFrom.x)<5 || Abs(m_ptTo.y-m_ptFrom.y)<tm.tmHeight)
		m_ptTo=m_ptFrom;
	LongRect lrc;
	lrc.SetToBound(m_ptFrom,m_ptTo);
	lrc.Normalize();
	if(m_ptFrom==m_ptTo)
	{
		CMDText* pShape;
		pShape=new CMDText(CMDText::kTextAutoSize);
		lrc.Offset(-3,-6);
		for(int i=0;i<4;i++) pShape->SetShapePoint(i,lrc.GetCorner(i));
		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->SetFillModel(m_pView->GetDocument()->m_Fill); //uncommented as requested
		pShape->SetTextParams(m_pView->GetDocument()->m_TextParams);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		m_pView->ActivateTextShape(pShape);
		return true;
	}
	else
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDText* pShape;
		pShape=new CMDText(CMDText::kTextUserSize);
		for(int i=0;i<4;i++) pShape->SetShapePoint(i,lrc.GetCorner(i));
		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->SetFillModel(m_pView->GetDocument()->m_Fill); //uncommented as requested
		pShape->SetTextParams(m_pView->GetDocument()->m_TextParams);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		m_pView->ActivateTextShape(pShape);
		return true;
	}
	return false;
}
