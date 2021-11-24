// MDBitmap.cpp: implementation of the CMDBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCDraft.h"
#include "MDBitmap.h"

#include "Mapper.h"
#include "DraftContext.h"
#include "PCDraftView.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDBitmap,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDBitmap::CMDBitmap()
{
	m_hDib=NULL;
}

CMDBitmap::CMDBitmap(const CRect& rc,const LongRect& lrc,HBITMAP hDib)
{
	m_rc=rc;
	m_lrc=lrc;
	m_hDib=hDib;
	// bitmaps have white fill by default
	m_FillModel.m_nFillType=kFillColor;
	m_FillModel.m_crColor=PALETTEINDEX(0);
}

CMDBitmap::~CMDBitmap()
{
	if(m_hDib) DeleteObject(m_hDib);
}

void CMDBitmap::Serialize(CArchive& ar)
{
	CMDFillShape::Serialize(ar);

	if(ar.IsLoading())
	{
		ar >> m_rc >> m_lrc;
		ReadDIB(ar,m_hDib);
	}
	else
	{
		ar << m_rc << m_lrc;
		WriteDIB(ar,m_hDib);
	}
}

void CMDBitmap::CalcCenterDatum(LongPoint& lpt)
{
	lpt=Middle(m_lrc);
}

LongRect CMDBitmap::CalculateBasicExtents() const
{
	return m_lrc;
}

LongPoint CMDBitmap::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex==0);
	return m_lrc.GetCorner(0);
}

LongPoint CMDBitmap::GetShapeHandle(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<8);
	if(nIndex<4) return m_lrc.GetCorner(nIndex);
	return Average(m_lrc.GetCorner(nIndex%4),m_lrc.GetCorner((nIndex+1)%4));
}

void CMDBitmap::DrawShape(CDraftContext* pDC) const
{
	pDC->SetPen(m_PenModel);
	pDC->SetFill(m_FillModel);
	pDC->StretchBlt(m_rc,m_lrc,m_hDib,true);

}

void CMDBitmap::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->MoveTo(m_lrc.GetCorner(3));
	for(int i=0;i<4;i++)
		pDC->LineTo(m_lrc.GetCorner(i));
}

bool CMDBitmap::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
   pointd pt1;
   pMapper->LPtoDP(pt1, m_lrc.GetCorner(0));
   pointd pt2;
   pMapper->LPtoDP(pt2,m_lrc.GetCorner(1));
	bool result=guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	pt1=pt2; 
   pMapper->LPtoDP(pt2, m_lrc.GetCorner(2));
	result=result || guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	pt1=pt2; 
    pMapper->LPtoDP(pt2, m_lrc.GetCorner(3));
	result=result || guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	pt1=pt2; 
   pMapper->LPtoDP(pt2, m_lrc.GetCorner(0));
	result=result || guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	return result;
}

void CMDBitmap::DoShapeOffset(const LongPoint& delta)
{
	m_lrc.Offset(delta);
	m_lptDatum+=delta;
	m_lrcExtents+=delta;
}

void CMDBitmap::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongRatio xRatio,yRatio;
	xRatio.fN=(lrcTo.right-lrcTo.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrcTo.bottom-lrcTo.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();

	LongPoint lptOffset=m_lrc.GetCorner(0)-lrcFrom.GetCorner(0);
	lptOffset.x*=xRatio;
	lptOffset.y*=yRatio;
	lptOffset+=lrcTo.GetCorner(0);
	m_lrc.left=lptOffset.x;
	m_lrc.top=lptOffset.y;

	lptOffset=m_lrc.GetCorner(2)-lrcFrom.GetCorner(0);
	lptOffset.x*=xRatio;
	lptOffset.y*=yRatio;
	lptOffset+=lrcTo.GetCorner(0);
	m_lrc.right=lptOffset.x;
	m_lrc.bottom=lptOffset.y;

	m_lrc.Normalize();

	CMDShape::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
}

void CMDBitmap::DoShapeScale(const LongRatio& ratio,double factor)
{
	m_lrc.left*=ratio;
	m_lrc.right*=ratio;
	m_lrc.top*=ratio;
	m_lrc.bottom*=ratio;
	if(factor) m_lrc*=factor;
	CMDShape::DoShapeScale(ratio,factor);
}

void CMDBitmap::DoShapeMirror(const LongPoint &lptRef1, const LongPoint &lptRef2)
{
	LongPoint lptCenter=Middle(m_lrc);
	LongPoint delta;
	guNearPtLnPt(lptCenter,lptRef1,lptRef2,&delta);
	delta-=lptCenter;
	delta<<=1;
	DoShapeOffset(delta);
}

void CMDBitmap::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	DIBSECTION ds;
	GetObject(m_hDib,sizeof(DIBSECTION),&ds);
	LPVOID lpBits;
	HBITMAP hDibCopy=CreateDIBSection(NULL,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
	ASSERT(hDibCopy);
	if(ds.dsBmih.biBitCount<=8)
	{
		CWindowDC dcScreen(NULL);
		CDC memDC;
		memDC.CreateCompatibleDC(&dcScreen);

		HGDIOBJ hOldBmp=memDC.SelectObject(m_hDib);
		LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
		GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
		memDC.SelectObject(hDibCopy);
		SetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
		GlobalFree(GlobalHandle(lpColors));
		memDC.SelectObject(hOldBmp);
	}
	{
		CWindowDC dcScreen(NULL);
		CGDIBitmapDC theDibCopy(&dcScreen,CBitmap::FromHandle(hDibCopy));
		CGDIBitmapDC theDib(&dcScreen,CBitmap::FromHandle(m_hDib));
		if(bVertical)
		{
			theDibCopy.StretchBlt(0,ds.dsBm.bmHeight-1,ds.dsBm.bmWidth,-ds.dsBm.bmHeight,&theDib,0,0,ds.dsBm.bmWidth,ds.dsBm.bmHeight,SRCCOPY);
		}
		else
		{
			theDibCopy.StretchBlt(ds.dsBm.bmWidth-1,0,-ds.dsBm.bmWidth,ds.dsBm.bmHeight,&theDib,0,0,ds.dsBm.bmWidth,ds.dsBm.bmHeight,SRCCOPY);
		}
	}
	DeleteObject(m_hDib);
	m_hDib=hDibCopy;
}

CTrackContext* CMDBitmap::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	if(nWhichHandle==kRotateHandle)
	{
	}

	CBitmapTrackContext* p=new CBitmapTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->bCrop=false;
	p->lrc=m_lrc;
	return p;
}

void CMDBitmap::TrackFeedback(LongPoint lpt, CTrackContext* pTrackContext)
{
	CBitmapTrackContext* p=(CBitmapTrackContext*)pTrackContext;
	if(p->nWhichHandle==kMoveHandle)
	{
		p->lrc.Offset(lpt);
	}
	else
	{
		p->bCrop=(p->bShift && p->bOption);
		if(p->bCrop)
		{
			BITMAP bmp;
			GetObject(m_hDib,sizeof(BITMAP),&bmp);
			LongRect lrc;
			lrc.left=p->pView->m_Mapper.DPtoLP(-m_rc.left);
			lrc.top=p->pView->m_Mapper.DPtoLP(-m_rc.top);
			lrc.right=lrc.left+p->pView->m_Mapper.DPtoLP(bmp.bmWidth);
			lrc.bottom=lrc.top+p->pView->m_Mapper.DPtoLP(bmp.bmHeight);
			CRect rc=p->pView->m_Mapper.LPtoDP(m_lrc);
			LongRatio ratio;
			ratio.fN=rc.Width();
			ratio.fD=m_rc.Width();
			lrc.left*=ratio;
			lrc.right*=ratio;
			ratio.fN=rc.Height();
			ratio.fD=m_rc.Height();
			lrc.top*=ratio;
			lrc.bottom*=ratio;
			lrc.Offset(m_lrc.GetCorner(0));

			if(lpt.x<lrc.left) lpt.x=lrc.left;
			else if(lpt.x>lrc.right) lpt.x=lrc.right;
			if(lpt.y<lrc.top) lpt.y=lrc.top;
			else if(lpt.y>lrc.bottom) lpt.y=lrc.bottom;
		}
      if(p->bShift)
      {
         BITMAP bmp;
         GetObject(m_hDib,sizeof(BITMAP),&bmp);
         LongRect lrc;
         lrc.left=p->pView->m_Mapper.DPtoLP(-m_rc.left);
         lrc.top=p->pView->m_Mapper.DPtoLP(-m_rc.top);
         lrc.right=lrc.left + p->pView->m_Mapper.DPtoLP(bmp.bmWidth);
         lrc.bottom=lrc.top + p->pView->m_Mapper.DPtoLP(bmp.bmHeight);
         CRect rc=p->pView->m_Mapper.LPtoDP(m_lrc);
         Coord width = p->lrc.right - p->lrc.left;
         Coord height = p->lrc.bottom - p->lrc.top;
         LongRect lrc1 = p->lrc;
         switch(p->nWhichHandle)
         {
         case 0:
            lrc1.left=lpt.x;
            lrc1.top=lpt.y;
            break;
         case 1:
            lrc1.right=lpt.x;
            lrc1.top=lpt.y;
            break;
         case 2:
            lrc1.right=lpt.x;
            lrc1.bottom=lpt.y;
            break;
         case 3:
            lrc1.left=lpt.x;
            lrc1.bottom=lpt.y;
            break;
         case 4:
            lrc1.top=lpt.y;
            break;
         case 5:
            lrc1.right=lpt.x;
            break;
         case 6:
            lrc1.bottom=lpt.y;
            break;
         case 7:
            lrc1.left=lpt.x;
            break;
         }
         Coord width1 = lrc1.right - lrc1.left;
         Coord height1 = lrc1.bottom - lrc1.top;
         WrFixed ratiox = width1 / width;
         WrFixed ratioy = height1 / height;
         WrFixed ratio;
         Coord half;
         if(p->nWhichHandle >= 0 && p->nWhichHandle <= 3)
         {
            ratio = min(ratiox,ratioy);
         }
         else if(p->nWhichHandle % 2 == 0)
         {
            ratio = ratioy;
            half = ((ratioy - 1.0) / 2.0) * width;
         }
         else
         {
            ratio = ratiox;
            half = ((ratiox - 1.0) / 2.0) * height;
         }
         switch(p->nWhichHandle)
         {
         case 0:
            p->lrc.top=p->lrc.bottom - height * ratio;
            p->lrc.left=p->lrc.right - width * ratio;
            break;
         case 1:
            p->lrc.right=p->lrc.left + width * ratio;
            p->lrc.top=p->lrc.bottom - height * ratio;
            break;
         case 2:
            p->lrc.right=p->lrc.left + width * ratio;
            p->lrc.bottom=p->lrc.top + height * ratio;
            break;
         case 3:
            p->lrc.left=p->lrc.right - width * ratio;
            p->lrc.bottom=p->lrc.top + height * ratio;
            break;
         case 4:
            p->lrc.top=lpt.y;
            p->lrc.left=p->lrc.left - half;
            p->lrc.right=p->lrc.right + half;
            break;
         case 5:
            p->lrc.right=lpt.x;
            p->lrc.top=p->lrc.top - half;
            p->lrc.bottom=p->lrc.bottom + half;
            break;
         case 6:
            p->lrc.bottom=lpt.y;
            p->lrc.left=p->lrc.left - half;
            p->lrc.right=p->lrc.right + half;
            break;
         case 7:
            p->lrc.left=lpt.x;
            p->lrc.top=p->lrc.top - half;
            p->lrc.bottom=p->lrc.bottom + half;
            break;
         }

      return;
      }
      switch(p->nWhichHandle)
		{
		case 0:
			p->lrc.left=lpt.x;
			p->lrc.top=lpt.y;
			break;
		case 1:
			p->lrc.right=lpt.x;
			p->lrc.top=lpt.y;
			break;
		case 2:
			p->lrc.right=lpt.x;
			p->lrc.bottom=lpt.y;
			break;
		case 3:
			p->lrc.left=lpt.x;
			p->lrc.bottom=lpt.y;
			break;
		case 4:
			p->lrc.top=lpt.y;
			break;
		case 5:
			p->lrc.right=lpt.x;
			break;
		case 6:
			p->lrc.bottom=lpt.y;
			break;
		case 7:
			p->lrc.left=lpt.x;
			break;
		}
	}
}

void CMDBitmap::TrackShowSize(CTrackContext* pTrackContext)
{
	CBitmapTrackContext* p=(CBitmapTrackContext*)pTrackContext;
	p->pView->SetShowSize(0,1,Width(p->lrc));
	p->pView->SetShowSize(1,2,Height(p->lrc));
}

void CMDBitmap::TrackComplete(CTrackContext* pTrackContext)
{
	CBitmapTrackContext* p=(CBitmapTrackContext*)pTrackContext;
	p->lrc.Normalize();
	if(p->bCrop)
	{
		LongRect lrc=p->lrc;
		lrc.Offset(-m_lrc.GetCorner(0));
		CRect rc=p->pView->m_Mapper.LPtoDP(m_lrc);
		LongRatio ratio;
		ratio.fN=m_rc.Width();
		ratio.fD=rc.Width();
		lrc.left*=ratio;
		lrc.right*=ratio;
		ratio.fN=m_rc.Height();
		ratio.fD=rc.Height();
		lrc.top*=ratio;
		lrc.bottom*=ratio;
//		rc=p->pView->m_Mapper.LPtoDP(lrc);
		rc.left=p->pView->m_Mapper.LPtoDP(lrc.left);
		rc.top=p->pView->m_Mapper.LPtoDP(lrc.top);
		rc.right=p->pView->m_Mapper.LPtoDP(lrc.right);
		rc.bottom=p->pView->m_Mapper.LPtoDP(lrc.bottom);
		rc.OffsetRect(m_rc.TopLeft());
		m_rc=rc;
	}
	m_lrc=p->lrc;
	CalculateExtents();
}

void CMDBitmap::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CBitmapTrackContext* p=(CBitmapTrackContext*)pTrackContext;
	pDC->MoveTo(p->lrc.GetCorner(3));
	for(int i=0;i<4;i++)
		pDC->LineTo(p->lrc.GetCorner(i));
}

int CMDBitmap::SaveMacDraft(PhIOContext& ctx)
{
	PhDSBitmap ps;
	ps.fID=PhIDBitmap;
	ctx.pPhShape=&ps;
	CMDFillShape::SaveMacDraft(ctx);

	for(int i=0;i<4;i++)
	{
		ps.fCorner[i].fUse=0;
		ps.fCorner[i].fFlags=0;
		ps.fCorner[i].fLongPt=m_lrc.GetCorner(i);
	}
	ps.fSrcRect.l=(short)m_rc.left;
	ps.fSrcRect.t=(short)m_rc.top;
	ps.fSrcRect.r=(short)m_rc.right;
	ps.fSrcRect.b=(short)m_rc.bottom;
	ps.fMapHdlIndex=ctx.bitmaps.Add((DWORD)m_hDib);

	uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm
	p.swapDBPoint(4);
	p.swapl(2); p.swaps(1);

	short count=sizeof(PhDSBitmap)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSBitmap));
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDBitmap::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
}
