// MDGroupShape.cpp: implementation of the CMDGroupShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDGroupShape.h"
#include "MDAssocDim.h"
#include "LinkVertex.h"

#include "gu.h"
#include "DraftContext.h"

#include "PCDraftView.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDGroupShape,CMDShape,1)

//////////////////////////////////////////////////////////////////////

CMDGroupShape::CMDGroupShape(CMDShape* pShape)
{
   if (pShape == this)
   {

      ASSERT(FALSE);
   }
	m_Members.AddTail(pShape);
}

CMDGroupShape::CMDGroupShape(CMDShapeList* members)
{
	m_Members.AddTail(members);
}

CMDGroupShape::~CMDGroupShape()
{
//	while(m_Members.GetCount())
	//	delete m_Members.RemoveHead();
}

void CMDGroupShape::Serialize(CArchive& ar)
{
	CMDShape::Serialize(ar);
	m_Members.Serialize(ar);
}

void CMDGroupShape::CalcDefaultDatum(LongPoint &lpt)
{
	POSITION pos=m_Members.GetTailPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetPrev(pos);
		if(!pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
		{
			lpt=pShape->GetShapeDatum();
			return;
		}
	}
	lpt=GetExtents().GetCorner(0);
}

LongRect CMDGroupShape::CalculateBasicExtents() const
{
	LongBoundRect lrcExtents;
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		LongRect lrc=pShape->CalculateBasicExtents();
		pShape->ExtendForPen(lrc);
		lrcExtents|=lrc;
	}
	return lrcExtents;
}

bool CMDGroupShape::RecalcExtents(CMDShape* pShape)
{
	bool bRecalc=false;
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pMemberShape=m_Members.GetNext(pos);
		if(pMemberShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
			bRecalc=((CMDGroupShape*)pMemberShape)->RecalcExtents(pShape);
		else bRecalc=(pMemberShape==pShape);
		if(bRecalc) break;
	}
	if(bRecalc) CalculateExtents();
	return bRecalc;
}

void CMDGroupShape::SetFillModel(const FillModel& fm)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		if(!pShape->IsLocked()) pShape->SetFillModel(fm);
	}
}


// Infinisys Ltd
void CMDGroupShape::ModifyFillModel(const FillModel& fillModel,int nWhat)
{
   POSITION pos=m_Members.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Members.GetNext(pos);
      if(!pShape->IsLocked()) pShape->ModifyFillModel(fillModel, nWhat);
   }
}


// Infinisys Ltd
void CMDGroupShape::SetFillOpacity(int iOpacity)
{
   POSITION pos=m_Members.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Members.GetNext(pos);
      if(!pShape->IsLocked()) pShape->SetFillOpacity(iOpacity);
   }
}



// Infinisys Ltd
bool CMDGroupShape::GetFillOpacity(int & iOpacity) const
{
   bool bOk = false;
   bool bFirst = true;
   iOpacity = -1;
   int iOpacityShape;
   POSITION pos=m_Members.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Members.GetNext(pos);
      if(!pShape->IsLocked())
      {
         if(!pShape->GetFillOpacity(iOpacityShape))
         {
            bOk = false;
            break;
         }
         else if(bFirst)
         {
            bOk = true;
            bFirst = false;
            iOpacity = iOpacityShape;
         }
         else if(iOpacity != iOpacityShape)
         {
            bOk = false;
            break;
         }
      }
   }
   return bOk;
}

// Infinisys Ltd
const FillModel & CMDGroupShape::GetFillModel() const
{

   ((CMDGroupShape *) this)->CalcCommonBestEffortFillModel();

   return m_fmCommonBestEffort;

}

// Infinisys Ltd
void CMDGroupShape::CalcCommonBestEffortFillModel()
{
   {
      bool bOk = false;
      bool bFirst = true;
      FillModel fmCommon;
      POSITION pos=m_Members.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Members.GetNext(pos);
         if(!pShape->IsLocked())
         {
            const FillModel & fm = pShape->GetFillModel();
            if(bFirst)
            {
               bOk = true;
               bFirst = false;
               fmCommon = fm;
            }
            else if(fmCommon.m_crColor != fm.m_crColor
               || fmCommon.m_nFillType != fm.m_nFillType
               || fmCommon.m_nIndex != fm.m_nIndex)
            {
               bOk = false;
               break;
            }
         }
      }
      if(bOk)
      {
         m_fmCommonBestEffort = fmCommon;
      }
      else
      {
         m_fmCommonBestEffort = FillModel();
      }

   }
   {
      bool bOk = false;
      bool bFirst = true;
      int iOpacity = -1;
      int iOpacityShape;
      POSITION pos=m_Members.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Members.GetNext(pos);
         if(!pShape->IsLocked())
         {
            if(!pShape->GetFillOpacity(iOpacityShape))
            {
               bOk = false;
               break;
            }
            else if(bFirst)
            {
               bOk = true;
               bFirst = false;
               iOpacity = iOpacityShape;
            }
            else if(iOpacity != iOpacityShape)
            {
               bOk = false;
               break;
            }
         }
      }
      m_fmCommonBestEffort.m_iFillOpacity = bOk ? iOpacity : -1;
   }
}

// Infinisys Ltd
const PenModel & CMDGroupShape::GetPenModel() const
{

   ((CMDGroupShape *) this)->CalcCommonBestEffortPenModel();

   return m_PenModel;

}


enum enum_pen_attribute
{

	e_pen_attribute_style = 1 << 16,
	e_pen_attribute_width = 1 << 17,

};


enum enum_fill_attribute
{
   
   e_fill_attribute_type = 1 << 16,
   e_fill_attribute_color = 1 << 17,
   e_fill_attribute_opacity = 1 << 18,

};


// Infinisys Ltd
void CMDGroupShape::CalcCommonBestEffortPenModel()
{
	
   PenModel pmCommon;
   FillModel fmCommon;

   {

      bool bOk = false;

      bool bFirst = true;

      int nPenWhat = 0xffffffff;
      int nFillWhat = 0xffffffff;

      POSITION pos=m_Members.GetHeadPosition();

      while(pos)
      {

         CMDShape* pShape=m_Members.GetNext(pos);

         if(!pShape->IsLocked())
         {

            const PenModel & pm = pShape->GetPenModel();

            const FillModel & fm = pShape->GetFillModel();

            if(bFirst)
            {
               bOk = true;
               bFirst = false;
               pmCommon = pm;
               fmCommon = fm;
            }
            else
            {
               if(pmCommon.m_crColor != pm.m_crColor)
               {
                  nPenWhat &= ~kPenColor;
               }
               if(pmCommon.m_iPenOpacity != pm.m_iPenOpacity)
               {
                  nPenWhat &= ~kPenColor;
               }
               if (pmCommon.m_nStyle != pm.m_nStyle)
               {
                  nPenWhat &= ~e_pen_attribute_style;
               }
               if (pmCommon.m_nWidth != pm.m_nWidth)
               {
                  nPenWhat &= ~e_pen_attribute_width;
               }
               if (fmCommon.m_nFillType != fm.m_nFillType)
               {
                  nPenWhat &= ~e_fill_attribute_type;
               }
               if (fmCommon.m_crColor != fm.m_crColor)
               {
                  nPenWhat &= ~e_fill_attribute_color;
               }
               if (fmCommon.m_iFillOpacity != fm.m_iFillOpacity)
               {
                  nPenWhat &= ~e_fill_attribute_opacity;
               }

            }

         }

      }

      m_PenModel = PenModel();
      
      if(nPenWhat & kPenColor)
      {

         m_PenModel.m_crColor = pmCommon.m_crColor;

      }

      if(nPenWhat & kPenColor)
      {

         m_PenModel.m_iPenOpacity = pmCommon.m_iPenOpacity;

      }

      if (nPenWhat & e_pen_attribute_style)
      {

         m_PenModel.m_nStyle = pmCommon.m_nStyle;

      }

      if (nPenWhat & e_pen_attribute_width)
      {

         m_PenModel.m_nWidth = pmCommon.m_nWidth;

      }

      if (nFillWhat & e_fill_attribute_type)
      {

         m_fmCommonBestEffort.m_nFillType = fmCommon.m_nFillType;

      }

      if (nFillWhat & e_fill_attribute_color)
      {

         m_fmCommonBestEffort.m_crColor = fmCommon.m_crColor;

      }

      if (nFillWhat & e_fill_attribute_opacity)
      {

         m_fmCommonBestEffort.m_iFillOpacity = fmCommon.m_iFillOpacity;

      }

   }

}


bool CMDGroupShape::ModifyPenModel(const PenModel &penModel, int nWhat)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		if(!pShape->IsLocked()) pShape->ModifyPenModel(penModel,nWhat);
	}
	CalculateExtents();
	return true;
}

LongPoint CMDGroupShape::GetShapeHandle(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<8);
	if(nIndex<4) return m_lrcExtents.GetCorner(nIndex);
	return Average(m_lrcExtents.GetCorner(nIndex%4),m_lrcExtents.GetCorner((nIndex+1)%4));
}

void CMDGroupShape::DrawShape(CDraftContext* pDC) const
{
	POSITION pos=m_Members.GetTailPosition();
   int iGroupShape = 0;
   while (pos)
   {
      iGroupShape++;
      m_Members.GetPrev(pos)->DrawShape(pDC);

   }
}
/*
void CMDGroupShape::DrawHandles(CDraftContext* pDC) const
{
	int i;
	pointd pt;
	LongPoint lpt,lpt1;
	for(i=0;i<4;i++)
	{
		lpt=m_lrcExtents.GetCorner(i);
		pDC->DrawHandle(lpt,m_bLocked);

		lpt1=Average(lpt,m_lrcExtents.GetCorner((i+1)%4));
		pDC->DrawHandle(lpt1,m_bLocked);
	}
}
*/
void CMDGroupShape::DrawShapeOutline(CDraftContext* pDC) const
{
	POSITION pos=m_Members.GetTailPosition();
	while(pos) m_Members.GetPrev(pos)->DrawShapeOutline(pDC);
}

bool CMDGroupShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		if(pShape->PointInShape(pt,pMapper)) return true;
	}
	return false;
}

bool CMDGroupShape::PointInFill(const pointd& pt, CMapper* pMapper) const
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		if(pShape->PointInFill(pt,pMapper)) return true;
	}
	return false;
}

void CMDGroupShape::DoShapeOffset(const LongPoint& delta)
{
	CMDShape::DoShapeOffset(delta);
	POSITION pos=m_Members.GetHeadPosition();
	while(pos) m_Members.GetNext(pos)->DoShapeOffset(delta);
}

void CMDGroupShape::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	bool bResult=false;
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		POSITION opos=pos;
		CMDShape* pShape=m_Members.GetNext(pos);
		CMDShape* pNewShape=NULL;
		pShape->DoShapeScale(lrcFrom,lrcTo,&pNewShape);
		if(pNewShape)
		{
			m_Members.InsertAfter(opos,pNewShape);
			m_Members.RemoveAt(opos);
			gDeletedShapes.AddTail(pShape); // to be deleted
		}
	}
	CMDShape::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
//	CalculateExtents();
}

void CMDGroupShape::DoShapeScale(const LongRatio& ratio,double factor)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		m_Members.GetNext(pos)->DoShapeScale(ratio,factor);
	}
	CMDShape::DoShapeScale(ratio,factor);
//	CalculateExtents();
}

void CMDGroupShape::DoShapeRotate(Angle angle, short nAbout, const LongPoint &lptRef)
{
	LongPoint lptRot;
	switch(nAbout)
	{
	case kAboutCenter:
		lptRot=Middle(GetExtents());
		break;
	case kAboutDatum:
		lptRot=GetShapeDatum();
		break;
	case kAboutRefPt:
		lptRot=lptRef;
		break;
	default:
		ASSERT(0);
	}
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		pShape->DoShapeRotate(angle,kAboutRefPt,lptRot);
	}
	CMDShape::DoShapeRotate(angle,nAbout,lptRef);
	ValidateDatum();
}

void CMDGroupShape::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos) m_Members.GetNext(pos)->DoShapeMirror(lptRef1,lptRef2);
	CMDShape::DoShapeMirror(lptRef1,lptRef2);
}

void CMDGroupShape::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	LongPoint lptCenterMul2;
	if(pLpt) lptCenterMul2=*pLpt;
	else
	{
		lptCenterMul2.x=m_lrcExtents.left+m_lrcExtents.right;
		lptCenterMul2.y=m_lrcExtents.top+m_lrcExtents.bottom;
	}
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pMember=m_Members.GetNext(pos);
		pMember->DoShapeFlip(bVertical,&lptCenterMul2);
		pMember->CalculateExtents();
	}
	CMDShape::DoShapeFlip(bVertical,&lptCenterMul2);
}

void CMDGroupShape::UpdateDependentShapes(CPCDraftView *pView)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos) m_Members.GetNext(pos)->UpdateDependentShapes(pView);
}

void CMDGroupShape::ReCalc(CMapper* pMapper)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos) m_Members.GetNext(pos)->ReCalc(pMapper);
	CalculateExtents();
}

int CMDGroupShape::Snap(LongPoint* lptSnap,const LongRect& lrcProx, short nType) const
{
	if(nType!=0)
	{
		POSITION pos=m_Members.GetHeadPosition();
		while(pos)
		{
			CMDShape* pShape=m_Members.GetNext(pos);
			if(pShape->Snap(lptSnap,lrcProx,nType)>=0) return 0;
		}
	}
	return -1;
}

void CMDGroupShape::Ungroup(CMDShapeList* pList)
{
	pList->AddTail(&m_Members);
	m_Members.RemoveAll();
}

CTrackContext* CMDGroupShape::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CGroupTrackContext* p=new CGroupTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->lrc=m_lrcExtents;
	p->ptLastDelta=LongPoint::kZeroPt;
	return p;
}

void CMDGroupShape::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CGroupTrackContext* p=(CGroupTrackContext*)pTrackContext;
	if(p->nWhichHandle==kMoveHandle) p->lrc.Offset(p->ptLastDelta=pt);
	else
	{
		if(p->nWhichHandle<4 && !p->bOption)
		{
			LongPoint size=m_lrcExtents.GetSize();
			LongPoint delta=pt-m_lrcExtents.GetCorner(p->nWhichHandle^2);
			bool bSignX=(delta.x<FixedZero);
			bool bSignY=(delta.y<FixedZero);
			delta=Abs(delta);
			double xRatio=delta.x.GetRawDouble()/size.x.GetRawDouble();
			double yRatio=delta.y.GetRawDouble()/size.y.GetRawDouble();
			if(xRatio>yRatio) delta.y=delta.x*(size.y.GetRawDouble()/size.x.GetRawDouble());
			else delta.x=delta.y*(size.x.GetRawDouble()/size.y.GetRawDouble());
			if(((bSignX!=bSignY) && (p->nWhichHandle&1)) || ((bSignX==bSignY) && !(p->nWhichHandle&1)))
			{
				if(bSignX) delta.x=-delta.x;
				if(bSignY) delta.y=-delta.y;
			}
			else
			{
				switch(p->nWhichHandle)
				{
				case 0:
					delta.y=-delta.y;
					delta.x=-delta.x;
					break;
				case 1:
					delta.y=-delta.y;
					break;
				case 2:
					break;
				case 3:
					delta.x=-delta.x;
					break;
				}
			}

			switch(p->nWhichHandle)
			{
			case 0:
				p->lrc.left=m_lrcExtents.right+delta.x;
				p->lrc.top=m_lrcExtents.bottom+delta.y;
				break;
			case 1:
				p->lrc.right=m_lrcExtents.left+delta.x;
				p->lrc.top=m_lrcExtents.bottom+delta.y;
				break;
			case 2:
				p->lrc.right=m_lrcExtents.left+delta.x;
				p->lrc.bottom=m_lrcExtents.top+delta.y;
				break;
			case 3:
				p->lrc.left=m_lrcExtents.right+delta.x;
				p->lrc.bottom=m_lrcExtents.top+delta.y;
				break;
			}
		}
		else
		{
			switch(p->nWhichHandle)
			{
			case 0:
				p->lrc.left=pt.x;
				p->lrc.top=pt.y;
				break;
			case 1:
				p->lrc.right=pt.x;
				p->lrc.top=pt.y;
				break;
			case 2:
				p->lrc.right=pt.x;
				p->lrc.bottom=pt.y;
				break;
			case 3:
				p->lrc.left=pt.x;
				p->lrc.bottom=pt.y;
				break;
			case 4:
				p->lrc.top=pt.y;
				break;
			case 5:
				p->lrc.right=pt.x;
				break;
			case 6:
				p->lrc.bottom=pt.y;
				break;
			case 7:
				p->lrc.left=pt.x;
				break;
			}
		}
	}
}

void CMDGroupShape::TrackShowSize(CTrackContext* pTrackContext)
{
	CGroupTrackContext* p=(CGroupTrackContext*)pTrackContext;
	p->pView->SetShowSize(0,1,Width(p->lrc));
	p->pView->SetShowSize(1,2,Height(p->lrc));
}

void CMDGroupShape::TrackComplete(CTrackContext* pTrackContext)
{
	CGroupTrackContext* p=(CGroupTrackContext*)pTrackContext;
	if(p->nWhichHandle>=0)
	{
		CMDShape* pNewShape;
		DoShapeScale(m_lrcExtents,p->lrc,&pNewShape);
		ProcessDeletedShapes(&p->pView->GetActiveLinkVertexList());
		ReCalc(&p->pView->m_Mapper);
		UpdateDependentShapes(p->pView);
	}
}

void CMDGroupShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CGroupTrackContext* p=(CGroupTrackContext*)pTrackContext;
	if(p->nWhichHandle==kRotateHandle)
	{
		CRotateContext rotContext;
		CRotateContext* pCtx=(CRotateContext*)p->lParam;
		rotContext.m_Angle=pCtx->m_Angle;
		rotContext.m_nAbout=kAboutRefPt;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			rotContext.m_ptOrigin=Middle(GetExtents());
			break;
		case kAboutDatum:
			rotContext.m_ptOrigin=GetShapeDatum();
			break;
		case kAboutRefPt:
			rotContext.m_ptOrigin=pCtx->m_ptOrigin;
			break;
		default:
			ASSERT(0);
		}

		POSITION pos=m_Members.GetTailPosition();
		while(pos)
		{
			CMDShape* pShape=m_Members.GetPrev(pos);
			CTrackContext* pTrackContext=pShape->TrackCreateContext(-2, (LPARAM)&rotContext, &p->pView->m_Mapper);
			if(pTrackContext)
			{
				pTrackContext->pView=p->pView;
				pShape->TrackWireFrame(pDC, pTrackContext);
				delete pTrackContext;
			}
		}
	}
	else if(p->nWhichHandle==kMoveHandle)
	{
		POSITION pos=m_Members.GetTailPosition();
		while(pos)
		{
			CMDShape* pShape=m_Members.GetPrev(pos);
			CTrackContext* pTrackContext=pShape->TrackCreateContext(kMoveHandle, 0, &p->pView->m_Mapper);
			if(pTrackContext)
			{
				pTrackContext->pView=p->pView;
				pShape->TrackFeedback(p->ptLastDelta, pTrackContext);
				pShape->TrackWireFrame(pDC, pTrackContext);
				delete pTrackContext;
			}
		}
	}
	else
	{
		pDC->MoveTo(p->lrc.GetCorner(3));
		for(int i=0;i<4;i++)
			pDC->LineTo(p->lrc.GetCorner(i));
	}
}

double CMDGroupShape::ReportCalcArea(const LongRatio& ratio) const
{
	double area=0;
	POSITION pos=m_Members.GetHeadPosition();
	while(pos) area+=m_Members.GetNext(pos)->ReportCalcArea(ratio);
	return area;
}

const ResizeInfo* CMDGroupShape::GetResizeInfo() const
{
static ResizeInfo groupResize =
{
	IDS_RGROUP, 3, {IDS_PERCENT,kLongValue,IDS_WIDTH,kUnitlessValue, IDS_HEIGHT,kUnitlessValue}
};
	return &groupResize;
}

LongRect CMDGroupShape::CalcResizeExtent()
{
	LongBoundRect lrcExtents;
	POSITION pos=m_Members.GetHeadPosition();
	bool bDimsOnly=true;
	while(pos)
	{
		CMDShape* pMember=m_Members.GetNext(pos);
		LongRect lrcShape=pMember->IsKindOf(RUNTIME_CLASS(CMDGroupShape))?((CMDGroupShape*)pMember)->CalcResizeExtent():pMember->CalculateBasicExtents();
		if(!pMember->IsKindOf(RUNTIME_CLASS(CMDAssocDim)))
		{
			if(bDimsOnly)
			{
				lrcExtents.Set(lrcShape.left,lrcShape.top,lrcShape.right,lrcShape.bottom);
				bDimsOnly=false;
			}
			else lrcExtents|=lrcShape;
		}
		else
		{
			if(bDimsOnly)
				lrcExtents|=lrcShape;
		}
	}
	return lrcExtents;
}

void CMDGroupShape::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDShape::GetResizeValue(pValues,nHandle);
	pValues[0].m_Long=100;
	LongPoint lpt=CalcResizeExtent().GetSize();
	pValues[1].m_Unitless=lpt.x;
	pValues[2].m_Unitless=lpt.y;
}

void CMDGroupShape::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	CMDShape::SetResizeValue(pValues);
	LongRect lrcExtents=CalcResizeExtent();
	LongPoint lpt=lrcExtents.GetSize();
	if(nHandle<0 || nHandle>7) nHandle=2;
	if(nHandle>3)
	{
		if(nHandle==4 || nHandle==6)
		{
			if(pValues[0].m_Long==100) lpt.y=pValues[2].m_Unitless;
			else lpt.y*=(pValues[0].m_Long/100.0);
		}
		else
		{
			if(pValues[0].m_Long==100) lpt.x=pValues[1].m_Unitless;
			else lpt.x*=(pValues[0].m_Long/100.0);
		}
		if(nHandle==5 || nHandle==6) nHandle=0;
		else nHandle=2;
	}
	else
	{
		if(pValues[0].m_Long==100)
		{
			lpt.x=pValues[1].m_Unitless;
			lpt.y=pValues[2].m_Unitless;
		}
		else lpt*=(pValues[0].m_Long/100.0);
		if(nHandle<2) nHandle+=2;
		else nHandle-=2;
	}

	LongPoint lptRef=lrcExtents.GetCorner(nHandle);
	LongRect lrcTo;
	switch(nHandle)
	{
	case 0:
		lrcTo.left=lptRef.x;
		lrcTo.top=lptRef.y;
		lrcTo.right=lptRef.x+lpt.x;
		lrcTo.bottom=lptRef.y+lpt.y;
		break;
	case 1:
		lrcTo.left=lptRef.x-lpt.x;
		lrcTo.top=lptRef.y;
		lrcTo.right=lptRef.x;
		lrcTo.bottom=lptRef.y+lpt.y;
		break;
	case 2:
		lrcTo.left=lptRef.x-lpt.x;
		lrcTo.top=lptRef.y-lpt.y;
		lrcTo.right=lptRef.x;
		lrcTo.bottom=lptRef.y;
		break;
	case 3:
		lrcTo.left=lptRef.x;
		lrcTo.top=lptRef.y-lpt.y;
		lrcTo.right=lptRef.x+lpt.x;
		lrcTo.bottom=lptRef.y;
		break;
	}

	CMDShape*pNewShape;
	DoShapeScale(lrcExtents,lrcTo,&pNewShape);
}

void CMDGroupShape::WriteDXF(CFile* pFile, CString& strLayerName, LongRectPtr pDocExtPtr, double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	POSITION pos;
    pos=m_Members.GetTailPosition();
	while(pos) {
		CMDShape* pShape=m_Members.GetPrev(pos);
		pShape->WriteDXF( pFile, strLayerName, pDocExtPtr, vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
	}
}

CMDShape* CMDGroupShape::MemberFromPoint(const pointd &pt, CMapper *pMapper, bool bNoGroup)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		if(bNoGroup && pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
		{
			CMDGroupShape* pGroup=(CMDGroupShape*)pShape;
			pShape=pGroup->MemberFromPoint(pt,pMapper,bNoGroup);
			if(pShape) return pShape;
		}
		else
		{
			if(pShape->TestSelect(pt, pMapper)) return pShape;
		}
	}
	return NULL;
}

void CMDGroupShape::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos) m_Members.GetNext(pos)->MakeSelfContained(pDoc,CopyDIB);
}

void CMDGroupShape::AddMember(CMDShape *pShape)
{
	m_Members.AddHead(pShape);
}

int CMDGroupShape::SaveMacDraft(PhIOContext& ctx)
{
	short groupCount=0;
	CMDShape* pSavedGroup=ctx.pGroup;
	ctx.pGroup=this;
	CDWordArray members;
	POSITION pos=m_Members.GetTailPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetPrev(pos);
		short count=pShape->SaveMacDraft(ctx);
		if(count) members.Add((long)pShape);
		groupCount+=count;
	}
	ctx.pGroup=pSavedGroup;
	if(groupCount)
	{
		PhDSShape ps;
		ps.fID=PhIDGroup;
		ctx.pPhShape=(PhDSFillShape*)&ps;
		CMDShape::SaveMacDraft(ctx);

		uptr p((char*)&ps);
		p.swapDSShape();

		p.pl=(long*)members.GetData();
		p.swapl(members.GetSize());

		short count=(sizeof(PhDSShape)+sizeof(short)+members.GetSize()*sizeof(long));
		uptr((char*)&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(&ps,sizeof(PhDSShape));
		count=members.GetSize();
		uptr((char*)&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(members.GetData(),members.GetSize()*sizeof(long));
		SaveMacObjInfo(ctx);

		groupCount++;
	}
	return groupCount;
}

void CMDGroupShape::AddToLinkContext(CLinkVertexContext* pContext)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->AddToLinkContext(pContext);
}

bool CMDGroupShape::IsLinked(CLinkVertexList& list,int nLinkVertex, CMDShape** pLinkedShape) const
{
	if(nLinkVertex==-1)
	{
		POSITION pos=m_Members.GetHeadPosition();
		while(pos)
		{
			if(m_Members.GetNext(pos)->IsLinked(list,nLinkVertex,pLinkedShape))
				return true;
		}
	}
	return false;
}

void CMDGroupShape::RecalcLinkedVertex(CLinkVertexContext* pContext)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CLinkVertexContext theContext;
		theContext.m_pView=pContext->m_pView;
		theContext.m_pList=pContext->m_pList;
		theContext.Copy(*pContext);
		m_Members.GetNext(pos)->RecalcLinkedVertex(&theContext);
	}
}

void CMDGroupShape::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->GetVertexesFromPt(vertexes,shapes,pt,pMapper);
}

bool CMDGroupShape::DeleteLinks(CLinkVertexList* pList)
{
	bool bResult=false;
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		bResult|=m_Members.GetNext(pos)->DeleteLinks(pList);

	return bResult;
}

void CMDGroupShape::AdjustLinks(CLinkVertexList* pList,CMDShape* pShapeToReplace)
{
	ASSERT(pShapeToReplace->IsKindOf(RUNTIME_CLASS(CMDGroupShape)));
	CMDGroupShape* pGroup=(CMDGroupShape*)pShapeToReplace;

	POSITION pos=m_Members.GetHeadPosition();
	POSITION rpos=pGroup->m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->AdjustLinks(pList,pGroup->m_Members.GetNext(rpos));
}

int CMDGroupShape::GetObjectCount()
{
	int nCount=1;
	POSITION pos;
    pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos);
		nCount+=pShape->GetObjectCount();
	}
	return nCount;
}

CMDShape* CMDGroupShape::ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Members.GetNext(pos)->ShapeFromEdge(pEdge,nFromVertex,nToVertex);
		if(pShape) return pShape;
	}
	return NULL;
}

bool CMDGroupShape::IsMember(CMDShape *pShape, bool bInsideGroups)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
	{
		CMDShape* pCurrentShape=m_Members.GetNext(pos);
		if(bInsideGroups && pCurrentShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
		{
			CMDGroupShape* pGroup=(CMDGroupShape*)pCurrentShape;
			if(pGroup->IsMember(pShape,true)) return true;
		}
		else
		{
			if(pShape==pCurrentShape) return true;
		}
	}
	return false;
}

void CMDGroupShape::RemoveArea(CPCDraftView* pView)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->RemoveArea(pView);
}

// Library - PCDraft 6 - Infinisys Ltd.
void CMDGroupShape::ProcessShapesForLibraryArchiving(CVersionDoc *pDocNew,CVersionDoc *pDoc,int* used)
{
   POSITION pos=m_Members.GetHeadPosition();
   while(pos)
      m_Members.GetNext(pos)->ProcessShapesForLibraryArchiving(pDocNew, pDoc,used);
}

// Library - PCDraft 6 - Infinisys Ltd.
void CMDGroupShape::ProcessShapesFromLibraryArchiving(CPCDraftDoc *pDoc,CPCDraftDoc *pDocLib)
{
   POSITION pos=m_Members.GetHeadPosition();
   while(pos)
      m_Members.GetNext(pos)->ProcessShapesFromLibraryArchiving(pDoc,pDocLib);
}

void CMDGroupShape::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->ProcessShapesForCopy(pDoc, used);
}

void CMDGroupShape::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->ProcessShapesAfterCopy(pDoc, used);
}

void CMDGroupShape::ProcessShapesAfterSymLibraryCopy(CPackedItem* pitem,int *used)
{
	POSITION pos=m_Members.GetHeadPosition();
	while(pos)
		m_Members.GetNext(pos)->ProcessShapesAfterSymLibraryCopy(pitem, used);
}