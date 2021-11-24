// SubContext.cpp: implementation of the CSubContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "SubContext.h"

#include "Edge.h"
#include "DraftContext.h"
#include "MDLineShape.h"
#include "MDPolygonShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CSubContext::CSubContext()
{
}

CSubContext::~CSubContext()
{
}

bool CSubContext::IsValidShape(CMDShape *pShape)
{
	LongRatio ratio={1,1};
	return !pShape->IsLocked() && ((pShape->IsClosed() && pShape->GetRawArea(ratio) && !pShape->IsAreaShown()) || pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape));
}

bool CSubContext::CanSub(CMDShape* pSuber,CMDShapeList *pSubList)
{
	if(IsValidShape(pSuber))
	{
		LongRect lrcExt=pSuber->GetExtents(),lrc;
		POSITION pos=pSubList->GetHeadPosition();
		while(pos)
		{
			CMDShape* pShape=pSubList->GetNext(pos);
			if(IsValidShape(pShape) && guSectRect(lrcExt,pShape->GetExtents(),&lrc)) return true;
		}
	}
	return false;
}

void CSubContext::Sub(CMDShape* pSuber,CMDShapeList *pSubList,CMDShapeList *pNewList)
{
	if(IsValidShape(pSuber))
	{
		CMDShapeList workList;
		workList.AddTail(pSubList);
		while(workList.GetCount())
		{
			CMDShape* pShape=workList.RemoveHead();
			if(IsValidShape(pShape) && SubShape(pShape,pSuber,pNewList)) continue;
			pSubList->RemoveAt(pSubList->Find(pShape));
		}
	}
}

bool CSubContext::SubShape(CMDShape *pBase, CMDShape *pSuber, CMDShapeList *pList)
{
	bool bFound=false;

	if(pSuber->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape))
	{
		CMDShape* pCutter=GetLineCutterShape(pBase,pSuber->GetShapePoint(0),pSuber->GetShapePoint(1),true);
		if(pCutter)
if(GetAsyncKeyState(VK_CONTROL)<0)
{
	bFound=true;
	pList->AddTail(pCutter);
	pList->AddTail(GetLineCutterShape(pBase,pSuber->GetShapePoint(0),pSuber->GetShapePoint(1),false));
}
else
		{
			if(SubShape(pBase,pCutter))
			{
				delete pCutter;
				pCutter=GetLineCutterShape(pBase,pSuber->GetShapePoint(0),pSuber->GetShapePoint(1),false);
				bFound=SubShape(pBase,pCutter);
			}
			delete pCutter;
		}
	}
	else
	{
		if(pSuber->IsClosed())
		{
			bFound=SubShape(pBase,pSuber);
		}
	}
	if(bFound)
	{
		pList->AddTail(&m_newShape);
		m_newShape.RemoveAll();
	}

	return bFound;
}

int CSubContext::posType(const LongPoint& lpt,CMDShape* pShape)
{
	LongRect lrc=pShape->GetExtents();
	int count=0;
	if(lrc.Contains(lpt))
	{
		CEdge edge(lpt,LongPoint(lrc.right+5,lpt.y));
		CEdge::ClearLastEdgeInfo();
		for(int i=0;i<pShape->GetNumShapeEdges();i++)
		{
			CEdge* pShapeEdge=pShape->GetShapeEdge(i);
			if(pShapeEdge->PtAtEdge(lpt))
			{
				delete pShapeEdge;
				return kIntersect;
			}
			count+=pShapeEdge->Intersect(&edge);
			delete pShapeEdge;
		}
	}
	return count&2?kInside:kOutside;
}

int CSubContext::edgeType(CEdge* pEdge,CMDShape* pShape)
{
	int pos=posType(pEdge->m_lptFrom,pShape);
	if(pos!=kIntersect) return pos;
	pos=posType(pEdge->m_lptTo,pShape);
	if(pos!=kIntersect) return pos;
	return posType(pEdge->MidPoint(),pShape);
}

CEdge* CSubContext::getNextEdge(POSITION& pos,int nBase,bool bReverse)
{
	CEdge* pEdge;
	if(bReverse)
	{
		pEdge=m_Edges[nBase].GetPrev(pos);
		if(!pos) pos=m_Edges[nBase].GetTailPosition();
	}
	else
	{
		pEdge=m_Edges[nBase].GetNext(pos);
		if(!pos) pos=m_Edges[nBase].GetHeadPosition();
	}
	return pEdge;
}

CEdge* CSubContext::getEdgeByPoint(const LongPoint& lpt,POSITION& pos,int& nBase,bool& bReverse)
{
	int nNewBase=1-nBase;
	bReverse=false;

	POSITION posI=0;
	bool bReverseI;

	pos=m_Edges[nNewBase].GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges[nNewBase].GetAt(pos);
		int type=edgeType(pEdge,m_Shape[nBase]);
		if(type==kIntersect && (lpt==pEdge->m_lptTo || lpt==pEdge->m_lptFrom))
		{
			posI=pos;
			bReverseI=(lpt==pEdge->m_lptTo);
		}
		else
		{
			if((nNewBase && type==kOutside) || (nBase && type==kInside))
			{
				if(lpt==pEdge->m_lptTo)
				{
					bReverse=true;
					break;
				}
				else if(lpt==pEdge->m_lptFrom) break;
			}
		}
		m_Edges[nNewBase].GetNext(pos);
	}

	if(pos)
	{
		nBase=nNewBase;
		POSITION opos=pos;
		CEdge* pEdge=getNextEdge(pos,nBase,bReverse);
		m_Edges[nBase].RemoveAt(opos);
		return pEdge;
	}
	else if(posI)
	{
		nBase=nNewBase;
		pos=posI;
		bReverse=bReverseI;
		CEdge* pEdge=getNextEdge(pos,nBase,bReverse);
		m_Edges[nBase].RemoveAt(posI);
		return pEdge;
	}
	return NULL;
}

bool CSubContext::SubShape(CMDShape *pBase, CMDShape *pSuber)
{
	bool bFound=false;
	CEdgeList bisectBaseEdges,bisectSuberEdges;

	m_Shape[0]=pSuber;
	m_Edges[0].RemoveAll();
	for(int si=0;si<pSuber->GetNumShapeEdges();si++) m_Edges[0].AddTail(pSuber->GetShapeEdge(si));
	m_Shape[1]=pBase;
	m_Edges[1].RemoveAll();
	for(int bi=0;bi<pBase->GetNumShapeEdges();bi++) m_Edges[1].AddTail(pBase->GetShapeEdge(bi));

	POSITION bpos=m_Edges[1].GetHeadPosition();
	while(bpos)
	{
		CEdge* pBaseEdge=m_Edges[1].GetAt(bpos);

		POSITION spos=m_Edges[0].GetHeadPosition();
		while(spos)
		{
			CEdge* pSuberEdge=m_Edges[0].GetAt(spos);
			if(pBaseEdge->Intersect(pSuberEdge,&bisectBaseEdges,&bisectSuberEdges))
			{

				if(bisectBaseEdges.GetCount())
				{
					m_Edges[1].InsertAfter(bpos,bisectBaseEdges.RemoveHead());
//					bisectBaseEdges.RemoveAll();
				}

				if(bisectSuberEdges.GetCount())
				{
					m_Edges[0].InsertAfter(spos,bisectSuberEdges.RemoveHead());
//					bisectSuberEdges.RemoveAll();
				}
				bFound=true;
			}
			m_Edges[0].GetNext(spos);
		}
		m_Edges[1].GetNext(bpos);
	}

// ----------------------------------------------------------------	
/*	
if(GetAsyncKeyState(VK_SHIFT)<0)
{
	bpos=m_Edges[0].GetHeadPosition();
	while(bpos) {
		CEdge* pBaseEdge=m_Edges[0].GetAt(bpos);
		CMDShape* pShape=pBaseEdge->CreateShape();
		m_newShape.AddTail(pShape);
		m_Edges[0].GetNext(bpos);
	}
	bpos=m_Edges[1].GetHeadPosition();
	while(bpos) {
		CEdge* pBaseEdge=m_Edges[1].GetAt(bpos);
		CMDShape* pShape=pBaseEdge->CreateShape();
		m_newShape.AddTail(pShape);
		m_Edges[1].GetNext(bpos);
	}
    
	return true;
}*/
// ----------------------------------------------------------------

	if(bFound)		// intersection exist
	{
		int  nBase=1;
		bool bReverse=false;
		bFound=false;
		DeleteEdges();
		POSITION pos=m_Edges[1].GetHeadPosition();
		while(m_Edges[nBase].GetCount())
		{
			POSITION opos=pos;
			CEdge* pEdge=getNextEdge(pos,nBase,bReverse);
			if(bReverse) pEdge->Reverse();
			m_Edges[nBase].RemoveAt(opos);

			int type=edgeType(pEdge,m_Shape[1-nBase]);
			if((type==kOutside && nBase) || (type==kInside && nBase==0))
			{
				m_newEdges.AddTail(pEdge);
				if(m_Edges[nBase].GetCount()==0)
				{
					pEdge=getEdgeByPoint(pEdge->m_lptTo,pos,nBase,bReverse);
					if(pEdge)
					{
						if(bReverse) pEdge->Reverse();
						m_newEdges.AddTail(pEdge);
					}
				}
			}
			else	// (type==kInside && nBase==0) || (type==kOutside && nBase)
			{
				if(m_newEdges.GetCount()==0) delete pEdge;
				else
				{
					LongPoint lpt=pEdge->m_lptFrom;
					delete pEdge;
					pEdge=getEdgeByPoint(lpt,pos,nBase,bReverse);
					if(!pEdge) return false;
					if(bReverse) pEdge->Reverse();
					m_newEdges.AddTail(pEdge);
				}
			}
			if(m_newEdges.GetCount() && m_newEdges.GetHead()->m_lptFrom==m_newEdges.GetTail()->m_lptTo)
			{
				m_newShape.AddTail(CreateShape(m_Shape[1]));
				bFound=true;
				nBase=1;
				bReverse=false;
				if(m_Edges[1].GetCount()) pos=m_Edges[1].GetHeadPosition();
			}
		}
	}

	while(m_Edges[0].GetCount()) delete m_Edges[0].RemoveHead();
	while(m_Edges[1].GetCount()) delete m_Edges[1].RemoveHead();
	DeleteEdges();

	return bFound;
}

CMDShape* CSubContext::GetLineCutterShape(const CMDShape *pBase, const LongPoint& lpt1, const LongPoint& lpt2, bool bSide) const
{
	LongRect lrcExt=pBase->GetExtents();
	lrcExt.Inset(-20,-20);

	LongPoint lipt[2];
	LongPoint lpt[5];
	int j=0,n=0;
	LongPoint lpt3,lpt4;
	lpt4=lrcExt.GetCorner(3);
    int i = 0;
	for(;i<4;i++)
	{
		lpt3=lpt4;
		lpt4=lrcExt.GetCorner(i);
		if(guLongLnLnPt(lpt1,lpt2,lpt3,lpt4,&lipt[j]))
		{
			if(i%2)
			{
				if(lipt[j].x>=lrcExt.left && lipt[j].x<=lrcExt.right)
				{
					lpt[n++]=lipt[j++];
				}
			}
			else
			{
				if(lipt[j].y>=lrcExt.top && lipt[j].y<=lrcExt.bottom)
				{
					lpt[n++]=lipt[j++];
				}
			}
		}
		if(j>1) break;
		if((j && bSide) || !(j || bSide)) lpt[n++]=lpt4;
	}
	if(!bSide)
	{
		while(i<4) lpt[n++]=lrcExt.GetCorner(i++);
	}
	CMDShape* pShape=NULL;
	if(j>1)
	{
		pShape=new CMDPolygonShape(CMDPolygonShape::kPolygon,n,true);
		while(0<n--) pShape->SetShapePoint(n,lpt[n]);
		pShape->CalculateExtents();
	}
	return pShape;
}
