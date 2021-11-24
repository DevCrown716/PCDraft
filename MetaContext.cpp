// MetaContext.cpp: implementation of the CMetaContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MetaContext.h"

#include "MDPolygonShape.h"
#include "MDMetaPolyShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CMetaContext::CMetaContext()
{
}

CMetaContext::~CMetaContext()
{
	DeleteEdges();
}

void CMetaContext::AddEdges(CMDShape *pShape, bool bAtEnd, bool bReverse)
{
	if((bAtEnd && !bReverse) || (!bAtEnd && bReverse))
	{
		for(int i=0;i<pShape->GetNumShapeEdges();i++)
		{
			CEdge* pEdge=pShape->GetShapeEdge(i);
			if(bReverse) pEdge->Reverse();
			if(bAtEnd) m_newEdges.AddTail(pEdge);
			else m_newEdges.AddHead(pEdge);
		}
	}
	else
	{
		for(int i=pShape->GetNumShapeEdges()-1;i>=0;i--)
		{
			CEdge* pEdge=pShape->GetShapeEdge(i);
			if(bReverse) pEdge->Reverse();
			if(bAtEnd) m_newEdges.AddTail(pEdge);
			else m_newEdges.AddHead(pEdge);
		}
	}
}

void CMetaContext::DeleteEdges()
{
	while(m_newEdges.GetCount())
	{
		delete m_newEdges.RemoveHead();
	}
}

bool CMetaContext::IsPoly()
{
	if(m_newEdges.GetCount())
	{
		POSITION pos=m_newEdges.GetHeadPosition();
		while(pos) if(m_newEdges.GetNext(pos)->m_nType!=CEdge::kLineEdge) return false;
		return true;
	}
	return false;
}

CMDShape* CMetaContext::CreateShape(CMDShape* pSrcShape, bool bConcatenate)
{
	if(bConcatenate)
	{
		POSITION pos=m_newEdges.GetHeadPosition();
		if(pos)
		{
			CEdge* pPrevEdge=m_newEdges.GetNext(pos);
			while(pos)
			{
				POSITION opos=pos;
				CEdge* pEdge=m_newEdges.GetNext(pos);
				if(pPrevEdge->Concatenate(pEdge))
				{
					m_newEdges.RemoveAt(opos);
					delete pEdge;
				}
				else pPrevEdge=pEdge;
			}
			CEdge* pEdge=m_newEdges.GetHead();
			if(pEdge!=pPrevEdge && pPrevEdge->Concatenate(pEdge))
			{
				m_newEdges.RemoveHead();
				delete pEdge;
			}
		}
	}

	bool bClosed=guAlmostEqualPt(m_newEdges.GetHead()->m_lptFrom,m_newEdges.GetTail()->m_lptTo)?true:false;
	CMDShape* pShape;
	if(IsPoly())
	{
		pShape=new CMDPolygonShape(CMDPolygonShape::kPolygon,m_newEdges.GetCount()+(bClosed?0:1),bClosed);
		int i=0;
		POSITION pos=m_newEdges.GetHeadPosition();
		while(pos) pShape->SetShapePoint(i++,m_newEdges.GetNext(pos)->m_lptFrom);
		if(!bClosed) pShape->SetShapePoint(i,m_newEdges.GetTail()->m_lptTo);
		DeleteEdges();
	}
	else
	{
		pShape=new CMDMetaPolyShape(&m_newEdges);
		m_newEdges.RemoveAll();
	}
	if(pSrcShape)
	{
		pShape->SetPenModel(pSrcShape->GetPenModel());
		if(bClosed) pShape->SetFillModel(pSrcShape->GetFillModel());
	}
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	return pShape;
}
