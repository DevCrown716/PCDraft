// MDBezierShape.cpp: implementation of the CMDBezierShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDBezierShape.h"

#include "gu.h"
#include "DraftContext.h"
#include "DrawPrim.h"
#include "BezierEdge.h"
#include "dxfutil.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDBezierShape,CMDPolygonShape,1)

//////////////////////////////////////////////////////////////////////

CMDBezierShape::CMDBezierShape(int nNumPoints) : CMDPolygonShape(kBezier, nNumPoints, false)
{
}

int	CMDBezierShape::GetNumShapeEdges() const
{
	return (m_nCount-1)/3;
}

CEdge* CMDBezierShape::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());
	nIndex*=3;
	auto pedge = new CBezierEdge(GetShapePoint(nIndex),GetShapePoint(nIndex+1),GetShapePoint(nIndex+2),GetShapePoint(nIndex+3));
   pedge->m_iBezierIndex = nIndex;
   return pedge;
}

void CMDBezierShape::DrawShape(CDraftContext* pDC) const
{

   bool isClosed = IsClosed();

   {

      UsePath path(pDC);

      CMDFillShape::DrawFill(pDC);

      if (m_PenModel.m_nPenType == kPenInvis)
      {

         return;

      }

      pDC->SetPen(m_PenModel);

      if (m_PenModel.Dashing() == 0)
      {

         if (!IsFilled())
         {

            GetFillRgn(nullptr, pDC);

         }

         auto p = pDC->path();

         if (p == nullptr)
         {

            OutputDebugString("null path");

         }

         pDC->graphics_on();

         pDC->graphics()->DrawPath(pDC->m_PenPlus.get(), pDC->path());

         if (!isClosed)
         {

            pDC->BezierDashCaps(GetShapePoint(0), GetShapePoint(1), GetShapePoint(2), GetShapePoint(3), -1, 0);
            pDC->BezierDashCaps(GetShapePoint(m_nCount - 4), GetShapePoint(m_nCount - 3), GetShapePoint(m_nCount - 2), GetShapePoint(m_nCount - 1), 0, -1);


         }

         return;

      }

   }
   pDC->m_DrawPrim.m_bUsePolyHelperIfDashed = true;
   //pDC->MoveTo(GetShapePoint(0));
   for (int i = 1; i < (m_nCount - 2); i += 3)
   {
      //		pDC->BezierTo(GetShapePoint(i),GetShapePoint(i+1),GetShapePoint(i+2));
      short cap1, cap2;
      cap1 = 0, cap2 = 0;
      if (!isClosed)
      {
         if (i == 1) // is first
         {
            cap1 = -1;
         }
         if (i + 3 >= (m_nCount - 2)) // is last
         {
            cap2 = -1;
         }
      }
      pDC->BezierDash(GetShapePoint(i - 1), GetShapePoint(i), GetShapePoint(i + 1), GetShapePoint(i + 2), cap1, cap2);
   }

   pDC->m_DrawPrim.m_bUsePolyHelperIfDashed = false;

   if (pDC->m_DrawPrim.m_helperPoly.has_points())
   {

      int lineThickness = 0;

      const PenModel* penModelPtr = &pDC->GetPen();

      int penWidth = penModelPtr->m_nWidth;

      lineThickness = pDC->GetMapper()->ScreenPenW(penWidth);

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

   }

}


void CMDBezierShape::DrawHandles(CDraftContext* pDC) const
{
	LongPoint lpt;
	for(int i=0;i<m_nCount;i++)
	{
		bool bDraw=(lpt!=m_pt[i]);
		lpt=m_pt[i];
		if(i==0 || i%3==2) pDC->MoveTo(lpt);
		else pDC->LineTo(lpt);
		if(bDraw) pDC->DrawHandle(lpt,m_bLocked);
	}
}

void CMDBezierShape::DrawShapeOutline(CDraftContext* pDC) const
{
	//pDC->MoveTo(GetShapePoint(0));
	//for(int i=1;i<(m_nCount-2);i+=3)
	//{
		//pDC->BezierTo(GetShapePoint(i),GetShapePoint(i+1),GetShapePoint(i+2));
	//}

   CArray < pointf > points;
   points.SetSize(m_nCount);
   for (int i = 0; i < m_nCount; i++)
   {
      pointd p;
      pDC->GetMapper()->LPtoDP(p, GetShapePoint(i));
      points[i] = p;

   }
   //// pDC->MoveTo(GetShapePoint(0));
   //for (int i = 0; i < (m_nCount - 2); i += 3)
   //{
   //pDC->BezierTo(GetShapePoint(i), GetShapePoint(i + 1), GetShapePoint(i + 2));
   ////   pDC->path()->AddBezier(points[i], points[i+1], points[i+2], points[i+3]);
   //}

   pDC->path()->AddBeziers(points.GetData(), points.GetCount());
}

int	CMDBezierShape::TestHandles(const pointd& pt, CMapper* pMapper) const
{
	for(int i=1;i<GetNumShapeHandles();i+=3)
	{
      pointd p1;
      pMapper->LPtoDP(p1, GetShapeHandle(i));
		if(TestHandle(p1,pt))
			return i;
      pointd p2;
      pMapper->LPtoDP(p2, GetShapeHandle(i+1));
      if(TestHandle(p2,pt))
			return i+1;
	}
	for(int i=0;i<GetNumShapeHandles();i+=3)
	{
      pointd p1;
      pMapper->LPtoDP(p1, GetShapeHandle(i));
		if(TestHandle(p1,pt))
			return i;
	}
	return -1; // miss it
}

bool CMDBezierShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	CDrawPrim dp;
	pointd pt0,pt1,pt2,pt3;
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	pMapper->LPtoDP(pt3,GetShapePoint(0));
	for(int i=1;i<(m_nCount-2);i+=3)
	{
		pt0=pt3;
		pMapper->LPtoDP(pt1,GetShapePoint(i));
		pMapper->LPtoDP(pt2,GetShapePoint(i+1));
		pMapper->LPtoDP(pt3,GetShapePoint(i+2));
		if(dp.checkBezier(pt0,pt1,pt2,pt3,pt,nWidth)) return true;
	}
	return false;
}

void CMDBezierShape::Unglue(CMDShapeList* pList) const
{
	CMDBezierShape* pShape;
	for(int i=1;i<(m_nCount-2);i+=3)
	{
		pShape=new CMDBezierShape(4);
		pShape->SetShapePoint(0,GetShapePoint(i-1));
		pShape->SetShapePoint(1,GetShapePoint(i));
		pShape->SetShapePoint(2,GetShapePoint(i+1));
		pShape->SetShapePoint(3,GetShapePoint(i+2));
		pShape->SetPenModel(m_PenModel);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}
}

int  CMDBezierShape::AddPt(const pointd& pt, CMapper* pMapper)
{
	CDrawPrim dp;
	pointd pt0,pt1,pt2,pt3;
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	pMapper->LPtoDP(pt3,GetShapePoint(0));
	for(int i=1;i<(m_nCount-2);i+=3)
	{
		pt0=pt3;
		pMapper->LPtoDP(pt1,GetShapePoint(i));
		pMapper->LPtoDP(pt2,GetShapePoint(i+1));
		pMapper->LPtoDP(pt3,GetShapePoint(i+2));
		if(dp.checkBezier(pt0,pt1,pt2,pt3,pt,nWidth))
		{
			LongPoint lpt[3];
			if(GetAsyncKeyState(VK_SHIFT)<0)
			{
				LongPoint temp;
				temp=(GetShapePoint(i)+GetShapePoint(i+1))>>1;
				SetShapePoint(i,(GetShapePoint(i-1)+GetShapePoint(i))>>1);
				SetShapePoint(i+1,(GetShapePoint(i+1)+GetShapePoint(i+2))>>1);
				lpt[0]=temp+GetShapePoint(i)>>1;
				lpt[2]=temp+GetShapePoint(i+1)>>1;
				lpt[1]=(lpt[0]+lpt[2])>>1;
			}
			else
			{
            lpt[1]=pMapper->DPtoLP(pt);
				guBezSplinePts(GetShapePoint(i-1),lpt[1],GetShapePoint(i+2),lpt,lpt+2);
			}

			m_pt.InsertAt(i+1,lpt[2]);
			m_pt.InsertAt(i+1,lpt[1]);
			m_pt.InsertAt(i+1,lpt[0]);
			m_nCount+=3;
			return (i+2);
		}
	}
	return -1;
}

void CMDBezierShape::RemovePt(int nHandle, CMDShape** ppNewShape)
{
	if(nHandle>=0 && nHandle<m_nCount && nHandle%3==0)
	{
		int nCount=(nHandle<m_nCount-1)?3:2;
		if(nHandle) nHandle--;
		else nCount--;
		m_pt.RemoveAt(nHandle,nCount);
		m_nCount-=nCount;
		
		if(m_nCount==4)
		{
			m_bClosed=false;
			m_FillModel.m_nFillType=kNoFill;
		}
	}
}

void CMDBezierShape::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle==kMoveHandle)
	{
		for(int i=0;i<p->pt.GetSize();i++) p->pt[i]+=pt;
	}
	else
	{
		LongPoint delta=pt-p->pt[nHandle];
		p->pt[nHandle]=pt;
		if((nHandle%3)==0)
		{
			if(nHandle>0) p->pt[nHandle-1]+=delta;
			if(nHandle<m_nCount-1) p->pt[nHandle+1]+=delta;
		}
		else
		{
			if(p->bShift)
			{
				if(nHandle>3 && ((nHandle-1)%3==0))
					p->pt[nHandle-2]=(p->pt[nHandle-1]<<1)-p->pt[nHandle];
				if(nHandle<(m_nCount-4) && ((nHandle+1)%3==0))
					p->pt[nHandle+2]=(p->pt[nHandle+1]<<1)-p->pt[nHandle];
			}
			else
			{
				if(nHandle>3 && ((nHandle-1)%3==0))
					p->pt[nHandle-2]=m_pt[nHandle-2];
				if(nHandle<(m_nCount-4) && ((nHandle+1)%3==0))
					p->pt[nHandle+2]=m_pt[nHandle+2];
			}
		}
	}
}

void CMDBezierShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;
   if (pDC->m_bUseGDI)
   {

      pDC->MoveTo(p->pt[0]);

   }
	for(int i=1;i<(m_nCount-2);i+=3)
	{
		pDC->MoveTo(p->pt[i]);
		pDC->LineTo(p->pt[i-1]);
		pDC->BezierTo(p->pt[i],p->pt[i+1],p->pt[i+2]);
		pDC->MoveTo(p->pt[i+1]);
		pDC->LineTo(p->pt[i+2]);
	}
}

double CMDBezierShape::GetRawArea(const LongRatio& ratio) const
{
	double area=0;
	double adjustment=0;
	LongPoint lptPrev,lptCur=GetShapePoint(0);
	for(int i=3;i<m_nCount;i+=3)
	{
		lptPrev=lptCur;
		lptCur=GetShapePoint(i);
		area+=ScaleDist(ratio,lptCur.y+lptPrev.y)*ScaleDist(ratio,lptCur.x-lptPrev.x)/2;
		adjustment+=BezEdge(ratio,lptPrev,GetShapePoint(i-2),GetShapePoint(i-1),lptCur);
	}
	if(!m_bClosed)
	{
		lptPrev=GetShapePoint(0);
		area+=ScaleDist(ratio,lptPrev.y+lptCur.y)*ScaleDist(ratio,lptPrev.x-lptCur.x)/2;
	}
//	area/=2;
	area+=adjustment;
	return fabs(area);
}

int CMDBezierShape::SaveMacDraft(PhIOContext& ctx)
{
	int size=sizeof(PhDSPolygon)+sizeof(PhDBPoint)*(m_nCount-1);
	PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
	ps->fID=PhIDBezier;
	ctx.pPhShape=ps;
	CMDFillShape::SaveMacDraft(ctx);

	ps->fClosed=m_bClosed;
	ps->fDummy=0;
	ps->fCount=(m_nCount-1)/3;

	uptr p((char*)ps);
	p.swapDSShape();
	p.swaps(2); // fm
	p.skip(2); p.swaps(1);

	for(int i=0;i<m_nCount;i++)
	{
		ps->fPoint[i].fFlags=kPhRealVertex;
		ps->fPoint[i].fLongPt=m_pt[i];
		ps->fPoint[i].fUse=0;
		p.swapDBPoint(1);
	}

	ushort count=size;
	uptr((char*)&count).swaps(1);
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(ps,size);
	SaveMacObjInfo(ctx);
	free(ps);
	return 1;
}
