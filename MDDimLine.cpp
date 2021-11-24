// MDDimLine.cpp: implementation of the CMDDimLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDDimLine.h"

#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "DraftContext.h"
#include "MDDimStr.h"
#include "phmath.h"

#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDDimLine,CMDLineShape,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMDDimLine::CMDDimLine():m_nTextDisplay(0)
{
}

CMDDimLine::CMDDimLine(const LongPoint& lptFrom,const LongPoint& lptTo,short nTextDisplay) : CMDLineShape(lptFrom,lptTo),m_nTextDisplay(nTextDisplay)
{
/*	m_pAttached=new CMDDimStr(true,this);
	m_pAttached->SetShapePoint(0,Average(lptFrom,lptTo));
*/
}

CMDDimLine::~CMDDimLine()
{
}

void CMDDimLine::Serialize(CArchive& ar)
{
	CMDLineShape::Serialize(ar);
}

void CMDDimLine::CalculateExtents()
{
	if(m_pAttached) m_pAttached->CalculateExtents();
	CMDLineShape::CalculateExtents();
}

void CMDDimLine::SetPenModel(const PenModel& pm)
{
	if(m_pAttached) m_pAttached->SetPenModel(pm);
	CMDLineShape::SetPenModel(pm);
}

bool CMDDimLine::ModifyPenModel(const PenModel &pm, int nWhat)
{
	if(m_pAttached) m_pAttached->ModifyPenModel(pm,nWhat);
	return CMDLineShape::ModifyPenModel(pm,nWhat);
}

void CMDDimLine::SetFillModel(const FillModel& fm)
{
	if(m_pAttached) m_pAttached->SetFillModel(fm);
}

const TextParams& CMDDimLine::GetTextParams() const
{
	if(m_pAttached) return m_pAttached->GetTextParams();
	return CMDLineShape::GetTextParams();
}

void CMDDimLine::ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC)
{
	if(m_pAttached) m_pAttached->ModifyTextParams(tp,nWhat,pDC);
}

bool CMDDimLine::PointInFill(const pointd& pt, CMapper* pMapper) const
{
	if(m_pAttached) return m_pAttached->PointInFill(pt,pMapper);
	return false;
}

void CMDDimLine::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	CMDShape::MakeSelfContained(pDoc,CopyDIB);
	if(m_pAttached) m_pAttached->MakeSelfContained(pDoc,CopyDIB);
}

void CMDDimLine::UpdateDependentShapes(CPCDraftView *pView)
{
	if(m_pAttached)
	{
		CPCDraftDoc* pDoc=pView->GetDocument();
		LongRatio ratio;
		pDoc->GetActiveScaleRatio(ratio);
		CDimUnit* pdu=pDoc->GetDimUnit();
		CString sValue=pdu->numUser2Str(pdu->numDB2User(ratio,Pythag(GetShapePoint(1)-GetShapePoint(0))),false);
		CDC* pDC=pView->GetDC();
		{
			CDraftContext dc(pDC,&pView->m_Mapper);
			m_pAttached->SetValues(&dc,sValue);
		}
		pView->ReleaseDC(pDC);

		short nDSAlign=kDSCenter;
		LongPoint lptAnchor,lptText;
		lptText=lptAnchor=Average(GetShapePoint(0),GetShapePoint(1));
		if(m_nTextDisplay>kHorizontal)
		{
			Angle angle=mtAngle(GetShapePoint(0),GetShapePoint(1));
			if(angle>90 && angle<=270) angle=mtFixAngle(angle-180);
			switch(m_nTextDisplay)
			{
			case kAbove:
				nDSAlign=kDSBottom;
				lptText.y-=4;
				break;
			case kBelow:
				nDSAlign=kDSTop;
				lptText.y+=4;
				break;
			}
			lptText=mtRotPoint(lptText,angle,lptAnchor);
			m_pAttached->SetRotation(angle);
			m_pAttached->CalcCorners();
		}
		m_pAttached->SetAlign(nDSAlign);
//		m_pAttached->SetShapePoint(0,lptText);
		m_pAttached->CalculateExtents();
	}
}

int CMDDimLine::SaveMacDraft(PhIOContext& ctx)
{
	PhDSDimShape ps;
	ps.fID=PhIDDimPt2Pt;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDShape::SaveMacDraft(ctx);

	ps.fConAngle=m_nConAngle;
	ps.fP1.fFlags=kPhRealVertex;
	ps.fP1.fLongPt=m_pt[0];
	ps.fP1.fUse=0;
	ps.fP2.fFlags=kPhRealVertex;
	ps.fP2.fLongPt=m_pt[1];
	ps.fP2.fUse=0;

	ps.fStatus=kPhDimVisible;
	if(GetAttachedShape()!=NULL) ps.fStatus|=kPhDimStrAttached;
	switch(m_nTextDisplay)
	{
	case kAbove:
		ps.fStatus|=kPhDimStrAbove;
	case kAlongLine:
		ps.fStatus|=kPhDimStrOnSlope;
		break;
	}
	ps.fDimStrHdl=(long)m_pAttached;

	uptr p((char*)&ps);
	p.swapDSShape();

	p.swaps(1);
	p.swapDBPoint(2);
	p.swaps(1);
	p.skip(12);
	p.swapl(1);

	ushort count=sizeof(PhDSDimShape)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSDimShape));
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDDimLine::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	CMDShape::ProcessShapesForCopy(pDoc, used);
	if(m_pAttached) m_pAttached->ProcessShapesForCopy(pDoc, used);
}

void CMDDimLine::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	CMDShape::ProcessShapesAfterCopy(pDoc, used);
	if(m_pAttached) m_pAttached->ProcessShapesAfterCopy(pDoc, used);
}

void CMDDimLine::ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used)
{
	CMDShape::ProcessShapesAfterSymLibraryCopy(pitem, used);
	if(m_pAttached) m_pAttached->ProcessShapesAfterSymLibraryCopy(pitem, used);
}