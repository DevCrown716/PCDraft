// LinkVertex.cpp: implementation of the CLinkVertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "LinkVertex.h"
#include "MDShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CLinkVertex,CObject,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinkVertex::CLinkVertex()
{
	m_nKey=0;
	m_pShape=NULL;
	m_nVertex=0;
	m_bResolved=false;
}

CLinkVertex::CLinkVertex(const CLinkVertex& lv)
{
	m_nKey=lv.m_nKey;
	m_pShape=lv.m_pShape;
	m_nVertex=lv.m_nVertex;
	m_bResolved=lv.m_bResolved;
}

CLinkVertex::~CLinkVertex()
{

}

void CLinkVertex::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_nKey;
		ar >> m_pShape;
		ar >> m_nVertex;
		m_bResolved=true; // always resolved,if restoring from archive
	}
	else
	{
		ar << m_nKey;
		ar << m_pShape;
		ar << m_nVertex;
	}
}

bool CLinkVertexContext::Include(CMDShape* pShape)
{
	for(int i=GetSize();--i>=0;)
		if(GetAt(i)==pShape)
			return true;
	return false;
}

void CLinkVertexList::DeleteAll()
{
	while(GetCount())
	{
		delete RemoveHead();
	}
}

void CLinkVertexList::DeleteAllByShapeAndVertex(CMDShape* pShape,int nVertex,bool bIncludeLine)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pShape && pLinkVertex->m_nVertex==nVertex)
		{
			if(bIncludeLine || pLinkVertex->m_nKey>0) // remove only non-line links
			{
				RemoveAt(cur);
				delete pLinkVertex;
			}
		}
	}
	RemoveSinglePoints();
}

void CLinkVertexList::ReplaceShape(CMDShape* pOldShape,CMDShape* pNewShape)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pOldShape)
			pLinkVertex->m_pShape=pNewShape;
	}
}

void CLinkVertexList::ReplaceShape(CMDShape* pOldShape,int nOldVertex,CMDShape* pNewShape,int nNewVertex)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pOldShape && pLinkVertex->m_nVertex==nOldVertex)
		{
			pLinkVertex->m_pShape=pNewShape;
			pLinkVertex->m_nVertex=nNewVertex;
		}
	}
}

void CLinkVertexList::SplitShape(CMDShape* pOldShape,int nSplitVertex,CMDShape* pNewShape1,CMDShape* pNewShape2)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pOldShape)
		{
			if(pLinkVertex->m_nVertex>=nSplitVertex) pLinkVertex->m_pShape=pNewShape2;
			else pLinkVertex->m_pShape=pNewShape1;
		}
	}
}

void CLinkVertexList::SwapShapeVertex(CMDShape* pShape,int nVertex1,int nVertex2)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pShape)
		{
			if(pLinkVertex->m_nVertex==nVertex1)
				pLinkVertex->m_nVertex=nVertex2;
			else if(pLinkVertex->m_nVertex==nVertex2)
				pLinkVertex->m_nVertex=nVertex1;
		}
	}
}

void CLinkVertexList::GetVertexes(CUIntArray& vertexes,CMDShape* pShape,int nVertex)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_nKey>=0 && pLinkVertex->m_pShape==pShape && pLinkVertex->m_nVertex==nVertex)
		{
			long nKey=pLinkVertex->m_nKey;
			pos=GetHeadPosition();
			while(pos)
			{
				CLinkVertex* pLinkVertex=GetNext(pos);
				if(pLinkVertex->m_pShape==pShape && pLinkVertex->m_nKey==nKey)
					vertexes.Add(pLinkVertex->m_nVertex);
			}
			break;
		}
	}
}

bool CLinkVertexList::DeleteAllByShape(CMDShape* pShape)
{
	bool bResult=false;
	int nKey=0;
	bool bHaveLocked,bHaveDeleted;
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_nKey!=nKey)
		{
			nKey=pLinkVertex->m_nKey;
			bHaveLocked=false;
			bHaveDeleted=false;
		}
		if(pLinkVertex->m_pShape==pShape)
		{
			RemoveAt(cur);
			delete pLinkVertex;
			bHaveDeleted=true;
		}
		else if(pLinkVertex->m_pShape->IsLocked())
		{
			bHaveLocked=true;
		}
		bResult|=(bHaveLocked && bHaveDeleted);
	}
	RemoveSinglePoints();
	return bResult;
}

void CLinkVertexList::ResolveObjectIndex(long nObjectIndex,CMDShape* pShape)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(!pLinkVertex->m_bResolved && pLinkVertex->m_nShape==nObjectIndex)
		{
			pLinkVertex->m_pShape=pShape;
			pLinkVertex->m_bResolved=true;
		}
	}
}

void CLinkVertexList::FlattenObjectIndex(CMDShape* pShape,long nObjectIndex)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_bResolved && pLinkVertex->m_pShape==pShape)
		{
			pLinkVertex->m_nShape=nObjectIndex;
			pLinkVertex->m_bResolved=false;
		}
	}
}


void CLinkVertexList::TransferResolved(CLinkVertexList& linkVertexList)
{
	POSITION pos=linkVertexList.GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=linkVertexList.GetNext(pos);
		if(pLinkVertex->m_bResolved)
		{
			linkVertexList.RemoveAt(cur);
			AddTail(pLinkVertex);
		}
//		else delete pLinkVertex;
	}
	RemoveSinglePoints();
}

void CLinkVertexList::Copy(const CLinkVertexList &linkVertexList)
{
	POSITION pos=linkVertexList.GetHeadPosition();
	while(pos)
	{
		CLinkVertex* pLinkVertex=linkVertexList.GetNext(pos);
		if(pLinkVertex->m_bResolved)
			AddTail(new CLinkVertex(*pLinkVertex));
	}
}

bool CLinkVertexList::IsLinkedShape(const CMDShape *pShape, int nLinkVertex, CMDShape** pLinkedShape)
{
	CMDShape* pOtherShape=NULL;
	int nKey=0;
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_nKey!=nKey)
		{
			nKey=pLinkVertex->m_nKey;
			pOtherShape=pLinkVertex->m_pShape;
		}
		if(pLinkVertex->m_pShape==pShape && (nLinkVertex==-1 || pLinkVertex->m_nVertex==nLinkVertex))
		{
			if(pLinkedShape)
			{
				if(pShape==pOtherShape) pOtherShape=GetAt(pos)->m_pShape;
				*pLinkedShape=pOtherShape;
			}
			return true;
		}
	}
	return false;
}

void CLinkVertexList::RemoveIfSingle(POSITION rpos, int nKey, int count)
{
	if(rpos==NULL) return;
	POSITION pos=rpos;
	CLinkVertex* pLinkVertex=GetNext(pos);
	if(count==1)				// single
	{
		RemoveAt(rpos);
		delete pLinkVertex;
	}
	else if(nKey<0 && count==2)	// or partial line
	{
		RemoveAt(rpos);
		delete pLinkVertex;
		pLinkVertex=GetAt(pos);
		RemoveAt(pos);
		delete pLinkVertex;
	}
}

void CLinkVertexList::RemoveSinglePoints()
{
	int nKey=0;
	int count=0;
	POSITION rpos=NULL;
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_nKey!=nKey)
		{
			RemoveIfSingle(rpos,nKey,count);
			nKey=pLinkVertex->m_nKey;
			rpos=cur;
			count=0;
		}
		count++;
	}
	RemoveIfSingle(rpos,nKey,count);
}

void CLinkVertexList::AdjustForAddVertex(CMDShape *pShape, int nVertex, int nShift)
{
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pShape && pLinkVertex->m_nVertex>=nVertex)
			pLinkVertex->m_nVertex+=nShift;
	}
}

void CLinkVertexList::AdjustForSubVertex(CMDShape *pShape, int nVertex, int nShift)
{
	DeleteAllByShapeAndVertex(pShape,nVertex);
	POSITION pos=GetHeadPosition();
	while(pos)
	{
		POSITION cur=pos;
		CLinkVertex* pLinkVertex=GetNext(pos);
		if(pLinkVertex->m_pShape==pShape && pLinkVertex->m_nVertex>=nVertex)
			pLinkVertex->m_nVertex-=nShift;
	}
}
