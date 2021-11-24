// MDCubicShape.cpp: implementation of the CMDCubicShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDCubicShape.h"

#include "gu.h"
#include "DraftContext.h"
#include "DrawPrim.h"
#include "dxfutil.h"
#include "BezierEdge.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDCubicShape,CMDPolygonShape,1)

//////////////////////////////////////////////////////////////////////

CMDCubicShape::CMDCubicShape(int nNumPoints,bool bClosed) : CMDPolygonShape(kCubic,nNumPoints,bClosed)
{
}

LongRect CMDCubicShape::CalculateBasicExtents() const
{
	LongBoundRect lbr;

	if(m_nCount<3) return CMDPolygonShape::CalculateBasicExtents();
	else
	{
		LongPoint lpt1,lpt2,lpt3;
		LongPoint lpt12,lpt32;
		lpt1=GetShapePoint(0);
		lpt2=GetShapePoint(1);

		lbr|=lpt1;
		if(m_bClosed)
		{
			guBezSplinePts(m_pt[m_nCount-1],lpt1,lpt2,&lpt12,&lpt32);
			lbr|=lpt12;
			lbr|=lpt32;
		}
		for(int i=2;i<(m_nCount+(m_bClosed?1:0));i++)
		{
			lpt3=GetShapePoint(i%m_nCount);
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			lbr|=lpt12;
			lbr|=lpt2;
			lbr|=lpt32;
			lpt1=lpt2;
			lpt2=lpt3;
		}
		if(!m_bClosed) lbr|=lpt3;
	}

	return lbr;
}

void CMDCubicShape::DrawShape(CDraftContext* pDC) const
{
   pDC->m_bOutlineForFill = true;
	CMDFillShape::DrawFill(pDC);
   pDC->m_bOutlineForFill = false;
	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDCubicShape::DrawShapeOutline(CDraftContext* pDC) const
{
	if(m_nCount<3) pDC->Line(GetShapePoint(0),GetShapePoint(1));
	else
	{
      pDC->m_DrawPrim.m_bUsePolyHelperIfDashed = true;
		LongPoint lpt1,lpt2,lpt3;
		lpt1=GetShapePoint(0);
		lpt2=GetShapePoint(1);
		lpt3=GetShapePoint(2);
		LongPoint lpt12,lpt32;
		guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
      pDC->path_new(true);
		if(m_bClosed) pDC->MoveTo(lpt2);
		else pDC->BezierDash(lpt1,lpt1,lpt12,lpt2,-1);
		for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
		{
			LongPoint lpt0=lpt2;
			lpt1=lpt32;
			lpt2=lpt3;
			lpt3=GetShapePoint(i%m_nCount);
			guBezSplinePts(lpt0,lpt2,lpt3,&lpt12,&lpt32);
			pDC->BezierDash(lpt0,lpt1,lpt12,lpt2);
		}

		if(!m_bClosed)
		{
			
         pDC->BezierDash(lpt2,lpt32,lpt3,lpt3,0,-1);

		}

      pDC->m_DrawPrim.m_bUsePolyHelperIfDashed = false;

      if (!pDC->m_bOutlineForFill)
      {

         int lineThickness = 0;

         const PenModel* penModelPtr = &pDC->GetPen();

         int penWidth = penModelPtr->m_nWidth;

         if (pDC->m_bPrinting)
         {

            lineThickness = penWidth;

         }
         else
         {

            lineThickness = pDC->GetMapper()->ScreenPenW(penWidth);

         }


         if (lineThickness <= 0)
         {

            lineThickness = 1;

         }

         if (lineThickness <= 1)
         {

            pDC->m_DrawPrim.m_helperPoly.DrawLineNow(pDC);
            pDC->m_DrawPrim.m_helperPoly.Reset();

         }
         else
         {

            pDC->m_DrawPrim.m_helperPoly.DrawNow(pDC);
            pDC->m_DrawPrim.m_helperPoly.Reset();

         }

         if (lineThickness <= 1 || !pDC->shouldUseDash())
         {

            if (pDC->m_pointa1.GetCount() > 0)
            {

               if (pDC->path_destroy(true))
               {

                  CArray < pointf > a;

                  //pDC->m_pointa1.RemoveAt(pDC->m_pointa1.GetCount() / 2, pDC->m_pointa1.GetCount() / 2);
                  // pDC->m_pointa1.RemoveAt(0, pDC->m_pointa1.GetCount() / 2);

                  a.Append(pDC->m_pointa1);
                  //a.RemoveAt(a.GetUpperBound());
                  //a.RemoveAt(a.GetUpperBound());
                  //pDC->m_pointa2.RemoveAt(0, pDC->m_pointa2.GetCount() / 2);
                  a.Append(pDC->m_pointa2);
                  //a.RemoveAt(a.GetUpperBound());
                  //a.RemoveAt(a.GetUpperBound());

                  pDC->path_new(false, Gdiplus::FillModeWinding);
                  pDC->path()->AddPolygon(a.GetData(), a.GetCount());
                  //pDC->path()->AddLines(a.GetData(), a.GetCount());
                  pDC->graphics_on();
                  pDC->graphics()->DrawPath(pDC->m_PenPlus.get(), pDC->path());
                  pDC->PaintPenPath();
                  pDC->path_destroy(false);

                  pDC->m_pointa1.RemoveAll();
                  pDC->m_pointa2.RemoveAll();

               }

            }
            else
            {

               auto ppath = pDC->path();

               pDC->graphics_on();

               pDC->graphics()->DrawPath(pDC->m_PenPlus.get(), pDC->path());

               pDC->path_destroy(true);

            }

         }

      }
      else
      {

         pDC->m_DrawPrim.m_helperPoly.DrawNow(pDC);
         pDC->m_DrawPrim.m_helperPoly.Reset();

      }

	}

}


bool CMDCubicShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);

   pointd p1; pointd p2;
   pMapper->LPtoDP(p1, GetShapePoint(0));
   pMapper->LPtoDP(p2, GetShapePoint(1));
	if(m_nCount<3) return guPtAtLine(pt,p1,p2,nWidth,NULL);
	else
	{
		CDrawPrim dp;
		pointd pt1,pt2,pt3;

		LongPoint lpt1,lpt2,lpt3;
		lpt1=GetShapePoint(0);
		lpt2=GetShapePoint(1);
		lpt3=GetShapePoint(2);
		LongPoint lpt12,lpt32;
		guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
		if(m_bClosed);
		else
		{
			pMapper->LPtoDP(pt1,lpt1);
			pMapper->LPtoDP(pt2, lpt12);
			pMapper->LPtoDP(pt3, lpt2);
			if(dp.checkBezier(pt1,pt1,pt2,pt3,pt,nWidth)) return true;
		}
		for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
		{
			LongPoint lpt0=lpt2;
			lpt1=lpt32;
			lpt2=lpt3;
			lpt3=GetShapePoint(i%m_nCount);
			guBezSplinePts(lpt0,lpt2,lpt3,&lpt12,&lpt32);

			pointd pt0=pt3;
			pMapper->LPtoDP(pt1, lpt1);
			pMapper->LPtoDP(pt2, lpt12);
			pMapper->LPtoDP(pt3,lpt2);
			if(dp.checkBezier(pt0,pt1,pt2,pt3,pt,nWidth)) return true;
		}
		if(!m_bClosed)
		{
			pMapper->LPtoDP(pt1, lpt3);
			pMapper->LPtoDP(pt2, lpt32);
			if(dp.checkBezier(pt3,pt2,pt1,pt1,pt,nWidth)) return true;
		}
	}
	return false;
}

void CMDCubicShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;
	if(m_nCount<3) pDC->Line(p->pt[0],p->pt[1]);
	else
	{
		LongPoint lpt1,lpt2,lpt3;
		lpt1=p->pt[0];
		lpt2=p->pt[1];
		lpt3=p->pt[2];
		LongPoint lpt12,lpt32;
		guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
		if(m_bClosed) pDC->MoveTo(lpt2);
		else pDC->Bezier(lpt1,lpt1,lpt12,lpt2);
		for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
		{
			LongPoint lpt0=lpt2;
			lpt1=lpt32;
			lpt2=lpt3;
			lpt3=p->pt[i%m_nCount];
			guBezSplinePts(lpt0,lpt2,lpt3,&lpt12,&lpt32);
			pDC->BezierTo(lpt1,lpt12,lpt2);
		}
		if(!m_bClosed)
		{
			pDC->BezierTo(lpt32,lpt3,lpt3);
		}
	}
}

double CMDCubicShape::GetRawArea(const LongRatio& ratio) const
{
	double area=0;
	double adjustment=0;
	for(int i=0;i<GetNumShapeEdges();i++)
	{
		CBezierEdge *pEdge=(CBezierEdge*)GetShapeEdge(i);
		area+=ScaleDist(ratio,pEdge->m_lptTo.y+pEdge->m_lptFrom.y)*ScaleDist(ratio,pEdge->m_lptTo.x-pEdge->m_lptFrom.x)/2;
		adjustment+=BezEdge(ratio,pEdge->m_lptFrom,pEdge->m_lptFromCtl,pEdge->m_lptToCtl,pEdge->m_lptTo);
	}
	if(!m_bClosed)
	{
		LongPoint lptPrev,lptCur;
		lptCur=GetShapePoint(0);
		lptPrev=GetShapePoint(m_nCount-1);
		area+=ScaleDist(ratio,lptCur.y+lptPrev.y)*ScaleDist(ratio,lptCur.x-lptPrev.x)/2;
	}
//	area/=2;
	area+=adjustment;
	return fabs(area);
}

int	CMDCubicShape::GetNumShapeEdges() const
{
	return (m_bClosed)?m_nCount:(m_nCount-1);
}

CEdge* CMDCubicShape::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());
	LongPoint lpt11, lpt12, lptTmp;
	if (m_nCount==2)
	{
		return new CBezierEdge(GetShapePoint(0),GetShapePoint(0),GetShapePoint(1),GetShapePoint(1));
	}
	if (m_bClosed)
	{
		if (!nIndex)
		{
			guBezSplinePts(GetShapePoint(m_nCount-1), GetShapePoint(0), GetShapePoint(1) , &lptTmp, &lpt12);
			guBezSplinePts(GetShapePoint(0), GetShapePoint(1), GetShapePoint(2) , &lpt11, &lptTmp);
			return new CBezierEdge(GetShapePoint(nIndex),lpt12,lpt11,GetShapePoint(nIndex+1));
		}
		guBezSplinePts(GetShapePoint(nIndex-1), GetShapePoint(nIndex), GetShapePoint((nIndex+1)%m_nCount) , &lptTmp, &lpt12);
		guBezSplinePts(GetShapePoint(nIndex), GetShapePoint((nIndex+1)%m_nCount), GetShapePoint((nIndex+2)%m_nCount) , &lpt11, &lptTmp);
		return new CBezierEdge(GetShapePoint(nIndex),lpt12,lpt11,GetShapePoint((nIndex+1)%m_nCount));
	}
	else
	{
		if (!nIndex)
		{
			guBezSplinePts(GetShapePoint(0), GetShapePoint(1), GetShapePoint(2) , &lpt11, &lpt12);
			return new CBezierEdge(GetShapePoint(0),GetShapePoint(0),lpt11,GetShapePoint(1));
		}
		if (nIndex==m_nCount-2)
		{
			guBezSplinePts(GetShapePoint(m_nCount-3), GetShapePoint(m_nCount-2), GetShapePoint(m_nCount-1) , &lpt11, &lpt12);
			return new CBezierEdge(GetShapePoint(m_nCount-2),lpt12,GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-1));
		}
		guBezSplinePts(GetShapePoint(nIndex-1), GetShapePoint(nIndex), GetShapePoint(nIndex+1) , &lptTmp, &lpt12);
		guBezSplinePts(GetShapePoint(nIndex), GetShapePoint(nIndex+1), GetShapePoint(nIndex+2) , &lpt11, &lptTmp);
		return new CBezierEdge(GetShapePoint(nIndex),lpt12,lpt11,GetShapePoint(nIndex+1));
	}
}

void CMDCubicShape::Unglue(CMDShapeList* pList) const
{
	for (int i=0; i<GetNumShapeEdges(); i++)
	{
		CEdge* pEdge=GetShapeEdge(i);
		CMDShape* pShape=pEdge->CreateShape();
		delete pEdge;
		pShape->SetPenModel(m_PenModel);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}
}

int CMDCubicShape::AddPt(const pointd& pt, CMapper* pMapper)
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	if(m_nCount<3)
	{
		if(guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(0)),pMapper->LPtoDPF(GetShapePoint(1)),nWidth,NULL))
		{
			LongPoint lpt;
			if(GetAsyncKeyState(VK_SHIFT)<0) lpt=Average(GetShapePoint(0),GetShapePoint(1));
			else lpt=pMapper->DPtoLP(pt);
			m_pt.InsertAt(1,lpt);
			m_nCount++;
			return 1;
		}
	}
	else
	{
		CDrawPrim dp;
		pointd pt1,pt2,pt3;

		LongPoint lpt1,lpt2,lpt3;
		lpt1=GetShapePoint(0);
		lpt2=GetShapePoint(1);
		lpt3=GetShapePoint(2);
		LongPoint lpt12,lpt32;
		guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
		if(m_bClosed);
		else
		{
			pt1=pMapper->LPtoDPF(lpt1);
			pt2=pMapper->LPtoDPF(lpt12);
			pt3=pMapper->LPtoDPF(lpt2);
			if(dp.checkBezier(pt1,pt1,pt2,pt3,pt,nWidth))
			{
				LongPoint lpt;
//				if(GetAsyncKeyState(VK_SHIFT)<0) lpt=Average(GetShapePoint(i-1),GetShapePoint(0));
//				else
				lpt=pMapper->DPtoLP(pt);
				m_pt.InsertAt(1,lpt);
				m_nCount++;
				return 1;
			}
		}
		for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
		{
			LongPoint lpt0=lpt2;
			lpt1=lpt32;
			lpt2=lpt3;
			lpt3=GetShapePoint(i%m_nCount);
			guBezSplinePts(lpt0,lpt2,lpt3,&lpt12,&lpt32);

			pointd pt0=pt3;
			pt1=pMapper->LPtoDPF(lpt1);
			pt2=pMapper->LPtoDPF(lpt12);
			pt3=pMapper->LPtoDPF(lpt2);
			if(dp.checkBezier(pt0,pt1,pt2,pt3,pt,nWidth))
			{
				LongPoint lpt;
//				if(GetAsyncKeyState(VK_SHIFT)<0) lpt=Average(GetShapePoint(i-1),GetShapePoint(i));
//				else
				lpt=pMapper->DPtoLP(pt);
				m_pt.InsertAt((i-1)%m_nCount,lpt);
				m_nCount++;
				return (i-1)%m_nCount;
			}
		}
		if(!m_bClosed)
		{
			pt1=pMapper->LPtoDPF(lpt3);
			pt2=pMapper->LPtoDPF(lpt32);
			if(dp.checkBezier(pt3,pt2,pt1,pt1,pt,nWidth))
			{
				LongPoint lpt;
//				if(GetAsyncKeyState(VK_SHIFT)<0) lpt=Average(GetShapePoint(i-1),GetShapePoint(0));
//				else
				lpt=pMapper->DPtoLP(pt);
				m_pt.InsertAt(m_nCount-1,lpt);
				m_nCount++;
				return m_nCount-2;
			}
		}
	}
	return -1;
}

int CMDCubicShape::SaveMacDraft(PhIOContext& ctx)
{
	short count=m_nCount-(m_bClosed?0:1);
	int size=sizeof(PhDSPolygon)+sizeof(PhDBPoint)*(3*count);
	PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
	ps->fID=PhIDCubic;
	ctx.pPhShape=ps;
	CMDFillShape::SaveMacDraft(ctx);

	ps->fClosed=m_bClosed;
	ps->fDummy=0;
	ps->fCount=count;

	uptr p((char*)ps);
	p.swapDSShape();
	p.swaps(2); // fm
	p.skip(2); p.swaps(1);

	LongPoint lpt1,lpt2,lpt3;
	LongPoint lpt12,lpt32;
	lpt1=GetShapePoint(0);
	lpt2=GetShapePoint(1);
    // SMK.SSA Should FIX it!
    if (m_nCount > 2)
        lpt3 = GetShapePoint(2);
    else
        lpt3 = lpt2;

	ps->fPoint[0].fUse=0;
	ps->fPoint[0].fFlags=kPhRealVertex;
	ps->fPoint[0].fLongPt=lpt1;
	if(m_bClosed)
	{
		guBezSplinePts(m_pt[m_nCount-1],lpt1,lpt2,&lpt12,&lpt32);
		ps->fPoint[1].fUse=0;
		ps->fPoint[1].fFlags=kPhRealVertex;
		ps->fPoint[1].fLongPt=lpt32;

		ps->fPoint[3*count].fUse=0;
		ps->fPoint[3*count].fFlags=kPhRealVertex;
		ps->fPoint[3*count].fLongPt=lpt1;

		ps->fPoint[3*count-1].fUse=0;
		ps->fPoint[3*count-1].fFlags=kPhRealVertex;
		ps->fPoint[3*count-1].fLongPt=lpt12;
	}
	else
	{
		ps->fPoint[1].fUse=0;
		ps->fPoint[1].fFlags=kPhRealVertex;
		ps->fPoint[1].fLongPt=lpt1;
	}
    int i = 2;
	for(;i<(m_nCount+(m_bClosed?1:0));i++)
	{
		lpt3=GetShapePoint(i%m_nCount);
		guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

		ps->fPoint[i*3-4].fUse=0;
		ps->fPoint[i*3-4].fFlags=kPhRealVertex;
		ps->fPoint[i*3-4].fLongPt=lpt12;

		ps->fPoint[i*3-3].fUse=0;
		ps->fPoint[i*3-3].fFlags=kPhRealVertex;
		ps->fPoint[i*3-3].fLongPt=lpt2;

		ps->fPoint[i*3-2].fUse=0;
		ps->fPoint[i*3-2].fFlags=kPhRealVertex;
		ps->fPoint[i*3-2].fLongPt=lpt32;

		lpt1=lpt2;
		lpt2=lpt3;
	}
	if(!m_bClosed)
	{
		ps->fPoint[i*3-4].fUse=0;
		ps->fPoint[i*3-4].fFlags=kPhRealVertex;
		ps->fPoint[i*3-4].fLongPt=lpt3;

		ps->fPoint[i*3-3].fUse=0;
		ps->fPoint[i*3-3].fFlags=kPhRealVertex;
		ps->fPoint[i*3-3].fLongPt=lpt3;
	}

	p.swapDBPoint(count*3+1);

	count=size;
	uptr((char*)&count).swaps(1);
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(ps,size);
	SaveMacObjInfo(ctx);
	free(ps);
	return 1;
}
