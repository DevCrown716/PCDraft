// ExtrudeSketcher.cpp: implementation of the CExtrudeSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "ExtrudeSketcher.h"
#include "LinkVertex.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "Edge.h"
#include "MDShape.h"
#include "MetaContext.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CExtrudeSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	guNearPtLnPt(pt,m_lpt1,m_lpt2,m_pt);
	m_pt[++m_nCurPoint]=pt;

	double halfWidth=m_Attrs.m_fWidth.GetRawDouble()/2;
	double dist1=RawDoublePythag(m_pt[0]-m_lpt1);
	double dist2=RawDoublePythag(m_pt[0]-m_lpt2);
	if(dist1<dist2)	m_limitAngle=mtRealToFix(asin(halfWidth/dist1)*kRadToAng);
	else m_limitAngle=mtRealToFix(asin(halfWidth/dist2)*kRadToAng);
}

CSketcher::ContinueMode CExtrudeSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_LBUTTONDOWN:
		return ContinueUpdatePt;
	case WM_LBUTTONUP:
		{
			bool bNearPoints=IsNearPoints(m_pt[0],m_pt[1]);
			if(m_nCurPoint==1 && bNearPoints && ((CPCDraftApp*)AfxGetApp())->m_bClickToStartPoly) return Continue;
			return (m_nCurPoint>1 || !bNearPoints)?(m_bPoly?ContinueAddPt:Complete):Cancel;
		}
	default:
		return CPolySketcher::QueryContinue();
	}
}

void CExtrudeSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	LongPoint lpt=ptUpdate;
	if(m_nCurPoint==1)	// limit angles
	{
		if(GetAsyncKeyState(VK_OPTION)<0)
		{
			LongPoint delta;
			guNearPtLnPt(lpt,m_lpt1,m_lpt2,&delta);
			lpt+=m_pt[0]-delta;
		}
		else
		{
			Angle lineAngle=mtFixAngle(k90-mtAngle(m_lpt1,m_lpt2));
			Angle angle=mtFixAngle(mtAngle(m_pt[0],ptUpdate)+lineAngle);
			if(angle<k90 && angle>k90-m_limitAngle) angle=k90-m_limitAngle;
			else if(angle>=k90 && angle<k90+m_limitAngle) angle=k90+m_limitAngle;
			else if(angle<k270 && angle>k270-m_limitAngle) angle=k270-m_limitAngle;
			else if(angle>=k270 && angle<k270+m_limitAngle) angle=k270+m_limitAngle;
			angle=mtFixAngle(angle-lineAngle);
			LongPoint lpt=m_pt[0];
			lpt.x+=Pythag(ptUpdate-m_pt[0]);
			lpt=mtRotPoint(lpt,angle,m_pt[0]);
		}
	}
	CPolySketcher::OnUpdatePt(lpt);
}

void CExtrudeSketcher::OnDrawSketcher(CDC * pDC)
{
	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;

	guParallelLn(m_pt[0],m_pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	guLongLnLnPt(pp12,pp22,m_lpt1,m_lpt2,&pp12);
	guLongLnLnPt(pp14,pp24,m_lpt1,m_lpt2,&pp14);

	for(int i=1;i<m_nCurPoint;i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(m_pt[i],m_pt[i+1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

      pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp11));
      pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp12));
      pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp13));
      pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp14));

	}

   pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp12));
   pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp22));
   pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp14));
   pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp24));

	if(m_Attrs.m_nEndCap&2)
	{
      pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp22));
      pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp24));
	}

	if(m_bPoly)
	{
		
      pointd pt=m_pView->m_Mapper.LPtoDP(m_pt[m_nCurPoint-1]);
      
      pDC->PatBlt(pt.x-2,pt.y-2,5,5,DSTINVERT);

	}

}


bool CExtrudeSketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);

	m_pView->SaveUndo(IDS_EXTRUDE);

	CMDShapeList newShapes;
	CMetaContext metaCtx;
    int n = 0;
	for(;n<m_pShape->GetNumShapeEdges();n++)
	{
		CEdge* pEdge=m_pShape->GetShapeEdge(n);
		if(pEdge->m_lptFrom==m_lpt1)
		{
			delete pEdge;
			break;
		}
		metaCtx.m_newEdges.AddTail(pEdge);
	}
	int nSplitVertex=n+1;

	CEdgeList edgeList;

	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;

	guParallelLn(m_pt[0],m_pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	guLongLnLnPt(pp12,pp22,m_lpt1,m_lpt2,&pp12);
	guLongLnLnPt(pp14,pp24,m_lpt1,m_lpt2,&pp14);

	int bSide=(Pythag(pp12-m_lpt1)>Pythag(pp14-m_lpt1));
	if(bSide)
	{
		if(m_lpt1!=pp14) metaCtx.m_newEdges.AddTail(new CEdge(m_lpt1,pp14));
	}
	else
	{
		if(m_lpt1!=pp12) metaCtx.m_newEdges.AddTail(new CEdge(m_lpt1,pp12));
	}

	int nShift=2;
	for(int i=1;i<m_nCurPoint-1;i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(m_pt[i],m_pt[i+1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		if(bSide)
		{
			metaCtx.m_newEdges.AddTail(new CEdge(pp13,pp14));
			edgeList.AddHead(new CEdge(pp12,pp11));
		}
		else
		{
			metaCtx.m_newEdges.AddTail(new CEdge(pp11,pp12));
			edgeList.AddHead(new CEdge(pp14,pp13));
		}
		nShift++;
	}

	if(bSide)
	{
		metaCtx.m_newEdges.AddTail(new CEdge(pp14,pp24));
		edgeList.AddHead(new CEdge(pp22,pp12));
	}
	else
	{
		metaCtx.m_newEdges.AddTail(new CEdge(pp12,pp22));
		edgeList.AddHead(new CEdge(pp24,pp14));
	}

	pp13=edgeList.GetTail()->m_lptTo;
	if(m_lpt2!=pp13) edgeList.AddTail(new CEdge(pp13,m_lpt2));

	for(++n;n<m_pShape->GetNumShapeEdges();n++)
	{
		CEdge* pEdge=m_pShape->GetShapeEdge(n);
		edgeList.AddTail(pEdge);
	}

	if(m_Attrs.m_nEndCap&2)
	{
		if(bSide)  metaCtx.m_newEdges.AddTail(new CEdge(pp24,pp22));
		else metaCtx.m_newEdges.AddTail(new CEdge(pp22,pp24));
		metaCtx.m_newEdges.AddTail(&edgeList);
	}
	else
	{
		if(!m_pShape->IsClosed()) newShapes.AddTail(metaCtx.CreateShape(m_pShape));
		metaCtx.m_newEdges.AddHead(&edgeList);
	}

	newShapes.AddTail(metaCtx.CreateShape(m_pShape));

	CLinkVertexList& linkList=m_pView->GetActiveLinkVertexList();
	if(newShapes.GetCount()==1)
	{
		CMDShape* pNewShape=newShapes.GetHead();
		linkList.ReplaceShape(m_pShape,pNewShape);
		linkList.AdjustForAddVertex(pNewShape,nSplitVertex,nShift<<1);
	}
	else
	{
		CMDShape* pNewShape=newShapes.GetTail();
		linkList.SplitShape(m_pShape,nSplitVertex,newShapes.GetHead(),pNewShape);
		linkList.AdjustForAddVertex(pNewShape,nSplitVertex,nShift-nSplitVertex);
	}

	m_pView->UnselectShape(m_pShape);
	m_pView->GetDocument()->CreateShapes(&newShapes,m_pShape);
	while(newShapes.GetCount())
	{
		m_pView->SelectShape(newShapes.RemoveHead(),true);
	}

	return true;
}
