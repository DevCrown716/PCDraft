// MDCenterDim.cpp: implementation of the CMDCenterDim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDCenterDim.h"
#include "MDCircle.h"
#include "MDLineShape.h"

#include "DraftContext.h"
#include "PCDraftView.h"
#include "Mapper.h"
#include "phmath.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CENTERDIM_VERSION 2

IMPLEMENT_SERIAL(CMDCenterDim,CMDAssocDim,VERSIONABLE_SCHEMA|CENTERDIM_VERSION)

//////////////////////////////////////////////////////////////////////

CMDCenterDim::CMDCenterDim(const DimStandard& DimStd,const DimFormat& DimFmt,const LongPoint& lptCenter,const LongPoint radii) : CMDAssocDim(DimStd,DimFmt)
{
	m_lptCenter=lptCenter;
	m_Radii=radii;
	m_centerLen=0;
	m_centerGap=0;
	m_centerExt=0;
}

void CMDCenterDim::Serialize(CArchive& ar)
{
	CMDAssocDim::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_lptCenter;
		ar >> m_Radii;
      if(ar.m_nObjectSchema>1)
		{
			ar >> m_centerLen;
			ar >> m_centerGap;
			ar >> m_centerExt;
		}
	}
	else
	{
		ar << m_lptCenter;
		ar << m_Radii;
      ar << m_centerLen;
      ar << m_centerGap;
      ar << m_centerExt;
	}
}

LongRect CMDCenterDim::CalculateBasicExtents() const
{
	LongRect lrc(m_lptCenter-m_Radii,m_lptCenter+m_Radii);
	lrc.Inset(-m_centerExt,-m_centerExt);
	return lrc;
}

LongPoint CMDCenterDim::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex==0);
	return m_lptCenter;
}

void CMDCenterDim::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex==0);
	m_lptCenter=lpt;
}

LongPoint CMDCenterDim::GetShapeHandle(int nIndex) const
{
	LongPoint lpt=m_lptCenter;
	switch(nIndex)
	{
	case 1:
		lpt.y-=m_Radii.y;
		break;
	case 2:
		lpt.x+=m_Radii.x;
		break;
	case 3:
		lpt.y+=m_Radii.y;
		break;
	case 4:
		lpt.x-=m_Radii.x;
		break;
	}
	if(nIndex && m_RotAngle!=FixedZero)
	{
		TSinCos sinCos(m_RotAngle);
		lpt=mtRotPoint(lpt, sinCos, m_lptCenter);
	}
	return lpt;
}

void CMDCenterDim::DrawShape(CDraftContext* pDC) const
{
	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDCenterDim::DrawShapeOutline(CDraftContext* pDC) const
{
	Coord halfCenterLen=m_centerLen >> 1;

	LongPoint lpt1,lpt2;
	TSinCos sinCos(m_RotAngle);
	// center cross
	if(m_centerLen > FixedZero)
	{
		lpt1 = m_lptCenter;
		lpt1.x -= halfCenterLen;
		lpt2 = m_lptCenter;
		lpt2.x += halfCenterLen;

		if(m_RotAngle != FixedZero)
		{
			lpt1 = mtRotPoint(lpt1, sinCos, m_lptCenter);
			lpt2 = mtRotPoint(lpt2, sinCos, m_lptCenter );
		}

		pDC->MoveTo(lpt1);
		pDC->OutsideLineTo(lpt2);
		pDC->ResetJoint();

		lpt1 = m_lptCenter;
		lpt1.y -= halfCenterLen;
		lpt2 = m_lptCenter;
		lpt2.y += halfCenterLen;

		if ( m_RotAngle != FixedZero )
		{
			lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
			lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
		}

		pDC->MoveTo(lpt1);
		pDC->OutsideLineTo(lpt2);
		pDC->ResetJoint();
	}

	// extension line - right
	lpt1 = lpt2 = m_lptCenter;
	lpt1.x += (halfCenterLen + m_centerGap);
	lpt2.x = m_lptCenter.x + m_Radii.x + m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pDC->MoveTo(lpt1);
	pDC->OutsideLineTo(lpt2);
	pDC->ResetJoint();

	// extension line - left
	lpt1 = lpt2 = m_lptCenter;
	lpt1.x -= (halfCenterLen + m_centerGap);
	lpt2.x = m_lptCenter.x - m_Radii.x - m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pDC->MoveTo(lpt1);
	pDC->OutsideLineTo(lpt2);
	pDC->ResetJoint();

	// extension line - top
	lpt1 = lpt2 = m_lptCenter;
	lpt1.y -= (halfCenterLen + m_centerGap);
	lpt2.y = m_lptCenter.y - m_Radii.y - m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pDC->MoveTo(lpt1);
	pDC->OutsideLineTo(lpt2);
	pDC->ResetJoint();

	// extension line - bottom
	lpt1 = lpt2 = m_lptCenter;
	lpt1.y += (halfCenterLen + m_centerGap);
	lpt2.y = m_lptCenter.y + m_Radii.y + m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pDC->MoveTo(lpt1);
	pDC->OutsideLineTo(lpt2);
	pDC->ResetJoint();
}

bool CMDCenterDim::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);

	Coord centerExt = pMapper->Scalar2Long(m_DimStd.CircleCenterExt);
	LongPoint lpt1,lpt2;
	TSinCos sinCos(m_RotAngle);


	lpt1 = lpt2 = m_lptCenter;
	lpt1.x = m_lptCenter.x - m_Radii.x - centerExt;
	lpt2.x = m_lptCenter.x + m_Radii.x + centerExt;
	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

   pointd pt1;
   pMapper->LPtoDP(pt1, lpt1);
	pointd pt2;
   pMapper->LPtoDP(pt2, lpt2);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;

	lpt1 = lpt2 = m_lptCenter;
	lpt1.y = m_lptCenter.y - m_Radii.y - centerExt;
	lpt2.y = m_lptCenter.y + m_Radii.y + centerExt;

	pMapper->LPtoDP(pt1, lpt1);
	pMapper->LPtoDP(pt2, lpt2);
	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	return guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
}

void CMDCenterDim::DoShapeScale(const LongRatio& ratio,double factor)
{
	if(factor) m_Radii*=factor;
	m_Radii*=ratio;
	CMDAssocDim::DoShapeScale(ratio,factor);
}

void CMDCenterDim::ReCalc(CMapper* pMapper)
{
	m_centerGap=pMapper->Scalar2Long(m_DimStd.CircleCenterGap);
	m_centerExt=pMapper->Scalar2Long(m_DimStd.CircleCenterExt);
	m_centerLen=pMapper->Scalar2Long(m_DimStd.CircleCenterLen);
}

void CMDCenterDim::Unglue(CMDShapeList* pList) const
{
	CMDShape* pShape;

	Coord halfCenterLen=m_centerLen >> 1;

	LongPoint lpt1,lpt2;
	TSinCos sinCos(m_RotAngle);
	// center cross
	if(m_centerLen > FixedZero)
	{
		lpt1 = m_lptCenter;
		lpt1.x -= halfCenterLen;
		lpt2 = m_lptCenter;
		lpt2.x += halfCenterLen;

		if(m_RotAngle != FixedZero)
		{
			lpt1 = mtRotPoint(lpt1, sinCos, m_lptCenter);
			lpt2 = mtRotPoint(lpt2, sinCos, m_lptCenter );
		}

		pShape=new CMDLineShape(lpt1,lpt2);
		pShape->SetPenModel(GetPenModel());
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);

		lpt1 = m_lptCenter;
		lpt1.y -= halfCenterLen;
		lpt2 = m_lptCenter;
		lpt2.y += halfCenterLen;

		if ( m_RotAngle != FixedZero )
		{
			lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
			lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
		}

		pShape=new CMDLineShape(lpt1,lpt2);
		pShape->SetPenModel(GetPenModel());
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}

	// extension line - right
	lpt1 = lpt2 = m_lptCenter;
	lpt1.x += (halfCenterLen + m_centerGap);
	lpt2.x = m_lptCenter.x + m_Radii.x + m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pShape=new CMDLineShape(lpt1,lpt2);
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);

	// extension line - left
	lpt1 = lpt2 = m_lptCenter;
	lpt1.x -= (halfCenterLen + m_centerGap);
	lpt2.x = m_lptCenter.x - m_Radii.x - m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pShape=new CMDLineShape(lpt1,lpt2);
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);

	// extension line - top
	lpt1 = lpt2 = m_lptCenter;
	lpt1.y -= (halfCenterLen + m_centerGap);
	lpt2.y = m_lptCenter.y - m_Radii.y - m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pShape=new CMDLineShape(lpt1,lpt2);
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);

	// extension line - bottom
	lpt1 = lpt2 = m_lptCenter;
	lpt1.y += (halfCenterLen + m_centerGap);
	lpt2.y = m_lptCenter.y + m_Radii.y + m_centerExt;

	if ( m_RotAngle != FixedZero )
	{
		lpt1 = mtRotPoint ( lpt1, sinCos , m_lptCenter );
		lpt2 = mtRotPoint ( lpt2, sinCos , m_lptCenter );
	}

	pShape=new CMDLineShape(lpt1,lpt2);
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);
}

CTrackContext* CMDCenterDim::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CCenterDimTrackContext* p=new CCenterDimTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->pt=m_lptCenter;
	p->radii=m_Radii;
	if(nWhichHandle==kRotateHandle)
	{
		CRotateContext* pCtx=(CRotateContext*)lParam;
		LongPoint lptRot;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			lptRot=m_lptCenter;
			break;
		case kAboutDatum:
			lptRot=GetShapeDatum();
			break;
		case kAboutRefPt:
			lptRot=pCtx->m_ptOrigin;
			break;
		default:
			ASSERT(0);
		}
	}
	return p;
}

void CMDCenterDim::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	int nHandle=pTrackContext->nWhichHandle;
	if(nHandle && !pTrackContext->pView->IsLinkedShape(this,kCenterHandle))
	{
		CCenterDimTrackContext* p=(CCenterDimTrackContext*)pTrackContext;
		if(nHandle==kMoveHandle) p->pt+=pt;
		else
		{
			if(nHandle%2) p->radii.y=Abs(p->pt.y-pt.y);
			else p->radii.x=Abs(p->pt.x-pt.x);
		}
	}
}

void CMDCenterDim::TrackComplete(CTrackContext* pTrackContext)
{
	CCenterDimTrackContext* p=(CCenterDimTrackContext*)pTrackContext;
	m_lptCenter=p->pt;
	m_Radii=p->radii;
	CalculateExtents();
}

void CMDCenterDim::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CCenterDimTrackContext* p=(CCenterDimTrackContext*)pTrackContext;
	switch(p->nWhichHandle)
	{
	case 0:
		break;

	case kRotateHandle:
		break;

	default:
		pDC->Ellipse(p->pt,p->radii,m_RotAngle);
		break;
	}
}

int CMDCenterDim::SaveMacDraft(PhIOContext& ctx)
{
	PhDSCenterDim ps;
	ps.fID=PhIDCenterDim;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDAssocDim::SaveMacDraft(ctx);

	ps.centerPt.fUse=0;
	ps.centerPt.fFlags=kPhRealVertex;
	ps.centerPt.fLongPt=m_lptCenter;
	ps.hRad=m_Radii.x;
	ps.vRad=m_Radii.y;

	uptr p((char*)&ps);
	p.swapDSShape();

	p.swaps(2); // fm
	p.swaps(3);	p.skip(12);
	p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
	p.swaps(1); p.swapl(10); // dim std
	p.swaplpt(8); 
	p.skip(kDimStrSize+kDimTolStrSize+kDimTolStrSize);

	p.swapDBPoint(1);
	p.swapl(2);

	short count=sizeof(PhDSCenterDim);
	uptr(&count).swaps(1);
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSCenterDim));

	SaveMacObjInfo(ctx);
	return 1;
}

void CMDCenterDim::OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex)
{
	if(nVertex==kCenterHandle)
	{
		if(pCauseShape->IsRadial())
		{
			m_lptCenter=pCauseShape->GetCenter();
			m_Radii=pCauseShape->GetRadii();
			if(!pCauseShape->IsKindOf(RUNTIME_CLASS(CMDCircle))) SetRotation(pCauseShape->GetRotation());
			CalculateExtents();
		}
	}
	else CMDAssocDim::OnUpdateLinkedVertex(pView,nVertex,pCauseShape,nCauseVertex);
}
