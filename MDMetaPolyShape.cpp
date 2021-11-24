// MDMetaPolyShape.cpp: implementation of the CMDMetaPolyShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCDraft.h"
#include "MDMetaPolyShape.h"

#include "gu.h"
#include "PCDraftView.h"
#include "DraftContext.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDMetaPolyShape,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDMetaPolyShape::CMDMetaPolyShape()
{

}

CMDMetaPolyShape::CMDMetaPolyShape(CEdgeList* pList)
{

	m_Edges.AddTail(pList);
}

CMDMetaPolyShape::~CMDMetaPolyShape()
{
	while (!m_Edges.IsEmpty()){
		delete m_Edges.GetHead();
		m_Edges.RemoveHead();
	}
}

void CMDMetaPolyShape::Serialize(CArchive& ar)
{
	CMDFillShape::Serialize(ar);
	m_Edges.Serialize(ar);
}

bool CMDMetaPolyShape::IsClosed() const
{
	return (m_Edges.GetCount()>1 && guAlmostEqualPt(m_Edges.GetHead()->m_lptFrom,m_Edges.GetTail()->m_lptTo))?true:false;;
}

void CMDMetaPolyShape::CalcDefaultDatum(LongPoint &lpt)
{
	lpt=m_Edges.GetHead()->m_lptFrom;
}

LongRect CMDMetaPolyShape::CalculateBasicExtents() const
{
	LongBoundRect lrcExtents;
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		lrcExtents|=pEdge->GetExtents();
	}
	return lrcExtents;
}

LongPoint CMDMetaPolyShape::GetFirstShapePoint() const
{
	return m_Edges.GetCount()?m_Edges.GetHead()->m_lptFrom:LongPoint::kZeroPt;
}

LongPoint CMDMetaPolyShape::GetLastShapePoint() const
{
	return m_Edges.GetCount()?m_Edges.GetTail()->m_lptTo:LongPoint::kZeroPt;
}

LongPoint CMDMetaPolyShape::GetShapeHandle(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<4);
	return m_lrcExtents.GetCorner(nIndex);
}

int	CMDMetaPolyShape::GetNumShapeEdges() const
{
	return (int) m_Edges.GetCount();
}

CEdge* CMDMetaPolyShape::GetShapeEdge(int nIndex) const
{
	return m_Edges.GetAt(m_Edges.FindIndex(nIndex))->CreateCopy();
}

void CMDMetaPolyShape::DrawShape(CDraftContext* pDC) const
{
	DrawFill(pDC);
	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDMetaPolyShape::DrawShapeOutline(CDraftContext* pDC) const
{

   if (pDC->m_bOutlineForFill)
   {

      bool bClosed = IsClosed();

      POSITION pos = m_Edges.GetHeadPosition();
      while (pos)
      {
         CEdge* pEdge = m_Edges.GetNext(pos);
         pEdge->DrawOutlinePath(pDC);
      }
   }
   else
   {

      pDC->MetaPoly(&m_Edges,IsClosed());

   }

	//if(IsClosed()) pDC->CloseJoint(m_Edges.GetHead()->m_lptFrom);
	//*/
}

bool CMDMetaPolyShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		if(pEdge->PtAtEdge(pt,pMapper)) return true;
	}
	return false;
}

void CMDMetaPolyShape::DoShapeOffset(const LongPoint& delta)
{
	CMDShape::DoShapeOffset(delta);
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		pEdge->Offset(delta);
	}
}

void CMDMetaPolyShape::DoShapeRotate(Angle angle, short nAbout, const LongPoint &lptRef)
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

	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		pEdge->Rotate(angle,lptRot);
	}
	CMDShape::DoShapeRotate(angle,kAboutRefPt,lptRot);
	ValidateDatum();
}

void CMDMetaPolyShape::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
/*	LongRatio ratio;
	if(Width(lrcFrom)>Height(lrcFrom))
	{
		ratio.fN=Width(lrcTo).GetFixed();
		ratio.fD=Width(lrcFrom).GetFixed();
	}
	else
	{
		ratio.fN=Height(lrcTo).GetFixed();
		ratio.fD=Height(lrcFrom).GetFixed();
	}

	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		pEdge->Offset(-lrcFrom.GetCorner(0));
		pEdge->Scale(ratio);
		pEdge->Offset(lrcTo.GetCorner(0));
	}
*/
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos) m_Edges.GetNext(pos)->Scale(lrcFrom,lrcTo);
	CMDShape::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
//	CalculateExtents();
}

void CMDMetaPolyShape::DoShapeScale(const LongRatio& ratio,double factor)
{
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos) m_Edges.GetNext(pos)->Scale(ratio,factor);
	CMDShape::DoShapeScale(ratio,factor);
//	CalculateExtents();
}

void CMDMetaPolyShape::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos) m_Edges.GetNext(pos)->Mirror(lptRef1,lptRef2);
	ReverseEdges();
	CMDShape::DoShapeMirror(lptRef1,lptRef2);
}

void CMDMetaPolyShape::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	LongPoint lptCenterMul2;
	if(pLpt) lptCenterMul2=*pLpt;
	else
	{
		lptCenterMul2.x=m_lrcExtents.left+m_lrcExtents.right;
		lptCenterMul2.y=m_lrcExtents.top+m_lrcExtents.bottom;
	}
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos) m_Edges.GetNext(pos)->Flip(bVertical,lptCenterMul2);
	ReverseEdges();
	CMDShape::DoShapeFlip(bVertical,&lptCenterMul2);
}

int CMDMetaPolyShape::Snap(LongPoint *lptSnap, const LongRect &lrcProx, short nType) const
{
	if(nType!=0)
	{
		POSITION pos=m_Edges.GetHeadPosition();
		while(pos)
		{
			CEdge* pEdge=m_Edges.GetNext(pos);
			if(pEdge->Snap(lptSnap,lrcProx,nType)) return 0;
		}
	}
	return -1;
}

void CMDMetaPolyShape::Unglue(CMDShapeList* pList) const
{
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_Edges.GetNext(pos)->CreateShape();
		pShape->SetPenModel(m_PenModel);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}
}

CTrackContext* CMDMetaPolyShape::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CMetaTrackContext* p=new CMetaTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->lrc=m_lrcExtents;
	return p;
}

void CMDMetaPolyShape::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CMetaTrackContext* p=(CMetaTrackContext*)pTrackContext;
	if(p->nWhichHandle==kMoveHandle) p->lrc.Offset(pt);
	else
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
}

void CMDMetaPolyShape::TrackShowSize(CTrackContext* pTrackContext)
{
	CMetaTrackContext* p=(CMetaTrackContext*)pTrackContext;
	p->pView->SetShowSize(0,1,Width(p->lrc));
	p->pView->SetShowSize(1,2,Height(p->lrc));
}

void CMDMetaPolyShape::TrackComplete(CTrackContext* pTrackContext)
{
	CMetaTrackContext* p=(CMetaTrackContext*)pTrackContext;
	if(p->nWhichHandle>=0)
	{
		CMDShape* pNewShape;
		DoShapeScale(m_lrcExtents,p->lrc,&pNewShape);
	}
}

void CMDMetaPolyShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CMetaTrackContext* p=(CMetaTrackContext*)pTrackContext;
	if(p->nWhichHandle==kRotateHandle)
	{
		CRotateContext* pCtx=(CRotateContext*)p->lParam;
		LongPoint lptAbout;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			lptAbout=Middle(GetExtents());
			break;
		case kAboutDatum:
			lptAbout=GetShapeDatum();
			break;
		case kAboutRefPt:
			lptAbout=pCtx->m_ptOrigin;
			break;
		default:
			ASSERT(0);
		}

		POSITION pos=m_Edges.GetHeadPosition();
		while(pos)
		{
			CEdge* pEdge=m_Edges.GetNext(pos)->CreateCopy();
			pEdge->Rotate(pCtx->m_Angle,lptAbout);
			pEdge->Draw(pDC);
			delete pEdge;
		}
	}
	else
	{
		pDC->MoveTo(p->lrc.GetCorner(3));
		for(int i=0;i<4;i++)
			pDC->LineTo(p->lrc.GetCorner(i));
	}
}

double CMDMetaPolyShape::GetRawArea(const LongRatio &ratio) const
{
	double area=0.00001;	// force nonzero area - tricky
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		/*LongPoint lpt1=pEdge->m_lptFrom;
		LongPoint lpt2=pEdge->m_lptTo;
		area+=ScaleDist(ratio,lpt2.y+lpt1.y)*ScaleDist(ratio,lpt2.x-lpt1.x)/2;*/
		area+=pEdge->SubAreaCalc(ratio);
	}
//	area/=2;
	return fabs(area);
}

const ResizeInfo* CMDMetaPolyShape::GetResizeInfo() const
{
static ResizeInfo metaResize =
{
	IDS_METAPOLY, 3, {IDS_PERCENT,kLongValue,IDS_WIDTH,kUnitlessValue, IDS_HEIGHT,kUnitlessValue}
};
	return &metaResize;
}

void CMDMetaPolyShape::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDShape::GetResizeValue(pValues,nHandle);
	pValues[0].m_Long=100;
	LongPoint lpt=m_lrcExtents.GetSize();
	pValues[1].m_Unitless=lpt.x;
	pValues[2].m_Unitless=lpt.y;
}

void CMDMetaPolyShape::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	CMDShape::SetResizeValue(pValues);

	LongPoint lpt=m_lrcExtents.GetSize();
	if(pValues[0].m_Long==100)
	{
		lpt.x=pValues[1].m_Unitless;
		lpt.y=pValues[2].m_Unitless;
	}
	else lpt*=(pValues[0].m_Long/100.0);
	if(nHandle<0 || nHandle>3) nHandle=2;
	if(nHandle<2) nHandle+=2;
	else nHandle-=2;

	LongPoint lptRef=m_lrcExtents.GetCorner(nHandle);
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
	DoShapeScale(m_lrcExtents,lrcTo,&pNewShape);
}

void CMDMetaPolyShape::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	CMDShape* pShape;
	if (CanUnglue()) {
		CMDShapeList shapeList;
		Unglue(&shapeList);
		ASSERT(shapeList.GetCount());
		while (shapeList.GetCount()) {
			pShape = shapeList.RemoveHead();
			pShape ->WriteDXF(pFile, pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
			delete pShape;
		}
	}
}

bool CMDMetaPolyShape::AddMember(CMDShape* pShape)
{
	if(m_Edges.GetCount())
	{
		LongPoint lptFirst=pShape->GetFirstShapePoint();
		LongPoint lptLast=pShape->GetLastShapePoint();

      if(guAlmostEqualPt(m_Edges.GetTail()->m_lptTo,lptFirst))
      {
         for(int i=0;i < pShape->GetNumShapeEdges();i++)
         {
            CEdge* pEdge=pShape->GetShapeEdge(i);
            m_Edges.AddTail(pEdge);
         }
      }
      else if(guAlmostEqualPt(m_Edges.GetTail()->m_lptTo,lptLast))
      {
         int j = 0;
         for(int i=pShape->GetNumShapeEdges() - 1;i >= 0;i--)
         {
            CEdge* pEdge=pShape->GetShapeEdge(i);
            pEdge->Reverse();
            if(pEdge->m_iBezierIndex >= 0)
            {
               pEdge->m_iBezierIndex = j * 3;
            }
            m_Edges.AddTail(pEdge);
            j++;
         }
      }
      else if(guAlmostEqualPt(m_Edges.GetHead()->m_lptFrom,lptFirst))
      {
         for(int i=0;i < pShape->GetNumShapeEdges();i++)
         {
            CEdge* pEdge=pShape->GetShapeEdge(i);
            pEdge->Reverse();
            m_Edges.AddHead(pEdge);
         }
      }
      else if(guAlmostEqualPt(m_Edges.GetHead()->m_lptFrom,lptLast))
      {
         int j = pShape->GetNumShapeEdges() - 1;
         for(int i=pShape->GetNumShapeEdges() - 1;i >= 0;i--)
         {
            CEdge* pEdge=pShape->GetShapeEdge(i);
            if(pEdge->m_iBezierIndex >= 0)
            {
               pEdge->m_iBezierIndex = j * 3;
            }
            m_Edges.AddHead(pEdge);
            j--;
         }
      }
      else
      {
         return false;
      }

	}
	else
	{
		for(int i=0;i<pShape->GetNumShapeEdges();i++)
		{
			CEdge* pEdge=pShape->GetShapeEdge(i);
			m_Edges.AddTail(pEdge);
		}
	}

	return true;
}

int CMDMetaPolyShape::SaveMacDraft(PhIOContext& ctx)
{
	short metaCount=0;
	CMemFile mf;
	CFile* pSavedFile=ctx.f;
	ctx.f=&mf;
	CMDShape* pSavedGroup=ctx.pGroup;
	ctx.pGroup=this;
	CDWordArray members;
	POSITION pos=m_Edges.GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges.GetNext(pos);
		CMDShape* pShape=pEdge->CreateShape();
		pShape->SetPenModel(m_PenModel);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);

		short count=pShape->SaveMacDraft(ctx);
		if(count)
		{
			members.Add((LONG_PTR)pShape);
			ctx.metaMembers.Add((LONG_PTR)pShape);
		}
		metaCount+=count;
	}
	ctx.f=pSavedFile;
	ctx.pGroup=pSavedGroup;
	if(metaCount)
	{
		PhDSMetaPoly ps;
		ps.fID=PhIDMetaPoly;
		ctx.pPhShape=&ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps.fStartPoint=m_Edges.GetHead()->m_lptFrom;
		ps.fClosed=IsClosed();
		ps.fDummy=0;
		ps.fNumValidMembers=(short) members.GetSize();
		ps.fNumClosedMembers=0;
		ps.fNumMembers=(short) members.GetSize();

		uptr p((char*)&ps);
		p.swapDSShape();
		p.swaps(2); // fm
		p.swaplpt(1);
		p.skip(2);
		p.swaps(3);

		p.pl=(long*)members.GetData();
		p.swapl(members.GetSize());

		short count=(sizeof(PhDSMetaPoly)+(members.GetSize()-1)*sizeof(long));
		uptr(&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(&ps,sizeof(PhDSMetaPoly)-sizeof(long));
		ctx.f->Write(members.GetData(),members.GetSize()*sizeof(long));
		SaveMacObjInfo(ctx);
		metaCount++;

		long length=mf.GetLength();
		void* pl=mf.Detach();
		ctx.f->Write(pl,length);
		free(pl);
	}
	return metaCount;
}

void CMDMetaPolyShape::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
}

void CMDMetaPolyShape::ReverseEdges()
{
	int n=m_Edges.GetCount();
	POSITION pos=m_Edges.GetTailPosition();
	while(pos)
	{
		POSITION rpos=pos;
		CEdge* pEdge=m_Edges.GetPrev(pos);
		m_Edges.RemoveAt(rpos);
		pEdge->Reverse();
		m_Edges.AddTail(pEdge);
	}
}
