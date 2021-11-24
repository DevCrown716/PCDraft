// AddContext.cpp: implementation of the CAddContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "AddContext.h"

#include "gu.h"
#include "MDPolygonShape.h"
#include "MDMetaPolyShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CAddContext::CAddContext()
{
}

CAddContext::~CAddContext()
{
}

bool CAddContext::IsValidShape(CMDShape *pShape)
{
	LongRatio ratio={1,1};
	return !pShape->IsLocked() && pShape->IsClosed() && pShape->GetNumShapeEdges()>0 && pShape->GetRawArea(ratio)/* && !pShape->IsAreaShown()*/;
}

bool CAddContext::CanAdd(CMDShape* pAdder,CMDShapeList *pAddList)
{
	if(IsValidShape(pAdder))
	{
		LongRect lrcExt=pAdder->GetExtents(),lrc;
		POSITION pos=pAddList->GetHeadPosition();
		while(pos)
		{
			CMDShape* pShape=pAddList->GetNext(pos);
			if(IsValidShape(pShape) && guSectRect(lrcExt,pShape->GetExtents(),&lrc)) return true;
		}
	}
	return false;
}

void CAddContext::Add(CMDShape* pAdder,CMDShapeList *pAddList,CMDShapeList *pNewList)
{
	if(IsValidShape(pAdder))
	{
		bool bFound=true;
		CMDShapeList workList,workList2;
		workList2.AddTail(pAddList);
		// Need to check if a memory leakage in CMDShapeLists

		while(bFound && workList2.GetCount())
		{
			bFound=false;
			workList.AddTail(&workList2);
			workList2.RemoveAll();
			while(workList.GetCount())
			{
				CMDShape* pShape=workList.RemoveHead();
				if(IsValidShape(pShape))
				{
					if(AddShape(pShape,pAdder,pNewList))
					{
						pAdder=pNewList->RemoveTail();
						bFound=true;
					}
					else workList2.AddTail(pShape);
				}
				else
				{
					pAddList->RemoveAt(pAddList->Find(pShape));
					// By TSM
//					if(pShape) delete pShape;
				}
			}
		}
		while(workList2.GetCount())
		{
			CMDShape* pShape=workList2.RemoveHead();
			// Need to check
			pAddList->RemoveAt(pAddList->Find(pShape));
		}
		pNewList->AddTail(pAdder);
	}
}

bool CAddContext::AddShape(CMDShape *pBase, CMDShape *pAdder, CMDShapeList *pList)
{
	bool bFound=false;

	if(pAdder->IsClosed())
	{
		bFound=AddShape(pBase,pAdder);
	}
	if(bFound)
	{
		pList->AddTail(&m_newShape);
		m_newShape.RemoveAll();
	}

	return bFound;
}

int CAddContext::posType(const LongPoint& lpt,CMDShape* pShape)
{
	int count=0;
	LongRect lrc=pShape->GetExtents();
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

int CAddContext::edgeType(CEdge* pEdge,CMDShape* pShape)
{
	int pos=posType(pEdge->m_lptFrom,pShape);
	if(pos!=kIntersect) return pos;
	pos=posType(pEdge->m_lptTo,pShape);
	if(pos!=kIntersect) return pos;
	return posType(pEdge->MidPoint(),pShape);
}

CEdge* CAddContext::getNextEdge(POSITION& pos,int nBase,bool bReverse)
{
	CEdge* pEdge;
	POSITION rpos=pos;
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
	m_Edges[nBase].RemoveAt(rpos);
	return pEdge;
}

CEdge* CAddContext::getEdgeByPoint(const LongPoint& lpt,POSITION& pos,int& nBase,bool& bReverse)
{
	int nNewBase=1-nBase;
	bReverse=false;
	pos=m_Edges[nNewBase].GetHeadPosition();
	while(pos)
	{
		CEdge* pEdge=m_Edges[nNewBase].GetAt(pos);
		if(edgeType(pEdge,m_Shape[nBase])==kOutside) //(nBase?kOutside:kInside))
		{
			if(lpt==pEdge->m_lptTo)
			{
				bReverse=true;
				break;
			}
			else if(lpt==pEdge->m_lptFrom) break;
		}

		m_Edges[nNewBase].GetNext(pos);
	}
	if(pos)
	{
		nBase=nNewBase;
		return getNextEdge(pos,nBase,bReverse);
	}
	else return NULL;
}

bool CAddContext::AddShape(CMDShape *pBase, CMDShape *pAdder)
{
	bool bFound=false;
	CEdgeList bisectBaseEdges,bisectAdderEdges;

	m_Shape[0]=pAdder;
	m_Edges[0].RemoveAll();
	for(int si=0;si<pAdder->GetNumShapeEdges();si++) m_Edges[0].AddTail(pAdder->GetShapeEdge(si));
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
			CEdge* pAdderEdge=m_Edges[0].GetAt(spos);
			if(pBaseEdge->Intersect(pAdderEdge,&bisectBaseEdges,&bisectAdderEdges))
			{
				if(bisectBaseEdges.GetCount())
				{
					m_Edges[1].InsertAfter(bpos,bisectBaseEdges.RemoveHead());
//					bisectBaseEdges.RemoveAll();
				}

				if(bisectAdderEdges.GetCount())
				{
					m_Edges[0].InsertAfter(spos,bisectAdderEdges.RemoveHead());
//					bisectAdderEdges.RemoveAll();
				}
				bFound=true;
			}
			m_Edges[0].GetNext(spos);
		}
		m_Edges[1].GetNext(bpos);
	}

	if(bFound)		// intersection exist
	{
		int  nBase=1;
		bool bReverse=false;
		bFound=false;
		DeleteEdges();
		POSITION pos=m_Edges[1].GetHeadPosition();
		bool bSwitched=false;
		while(m_Edges[nBase].GetCount())
		{
			CEdge* pEdge=getNextEdge(pos,nBase,bReverse);
			if(bReverse) pEdge->Reverse();

			int type=edgeType(pEdge,m_Shape[1-nBase]);
			if(type==kInside)
			{
				if(m_newEdges.GetCount()==0) delete pEdge;
				else
				{
					LongPoint lpt=pEdge->m_lptFrom;
					delete pEdge;
					pEdge=getEdgeByPoint(lpt,pos,nBase,bReverse);
					if(!pEdge) break;
					if(bReverse) pEdge->Reverse();
					m_newEdges.AddTail(pEdge);
					bSwitched=true;
				}
			}
			else
			{
				m_newEdges.AddTail(pEdge);	// kOutside, kIntersect
				if(m_Edges[nBase].GetCount()==0)
				{
					pEdge=getEdgeByPoint(pEdge->m_lptTo,pos,nBase,bReverse);
					if(pEdge)
					{
						if(bReverse) pEdge->Reverse();
						m_newEdges.AddTail(pEdge);
						bSwitched=true;
					}
				}
			}
			if(m_newEdges.GetCount() && bSwitched && m_newEdges.GetHead()->m_lptFrom==m_newEdges.GetTail()->m_lptTo)
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
