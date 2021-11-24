#include "stdafx.h"
#include "Layer.h"

#include "DraftContext.h"
#include "PCDraftDoc.h"
#include "MDText.h"
#include "MDDimStr.h"
#include "LinkVertex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LAYER_VERSION 2

IMPLEMENT_SERIAL(CLayer,CObject,VERSIONABLE_SCHEMA|LAYER_VERSION)

//////////////////////////////////////////////////////////////////////

CLayer::CLayer() : m_bHidden(false), m_bGrayed(false), m_nVertexKey(0) // changed m_bGrayed to false 4/16/2
{

}


CLayer::CLayer(const CLayer & layer):
m_bHidden(layer.m_bHidden),
m_bGrayed(layer.m_bGrayed),
m_nVertexKey(layer.m_nVertexKey),
m_nScale(layer.m_nScale),
m_nSnap(layer.m_nSnap),
m_strName(layer.m_strName)
{

}


CLayer::~CLayer()
{
	DeleteAllShapes();
}

bool CLayer::bIgnoreShapeList=false;

void CLayer::Serialize(CArchive& ar)
{
	if(ar.IsLoading())
	{
        BYTE dataGet1, dataGet2;
        ar >> dataGet1 >> dataGet2;
        m_bGrayed = dataGet1 == TRUE;
        m_bHidden = dataGet2 == TRUE;
		ar >> m_strName;
		ar >> m_nScale >> m_nSnap;
	}
	else
	{
        ar << (BYTE)(m_bGrayed ? 1 : 0) << (BYTE)(m_bHidden ? 1:0);
		ar << m_strName;
		ar << m_nScale << m_nSnap;
	}
	if(!bIgnoreShapeList)
	{
		m_ShapeList.Serialize(ar);
		if(ar.IsLoading())
		{
         if(ar.m_nObjectSchema > 1)
			{
				m_LinkVertexList.Serialize(ar);
				ar >> m_nVertexKey;
			}
		}
		else
		{
         m_LinkVertexList.Serialize(ar);
         ar << m_nVertexKey;
		}
	}
}

bool CLayer::Draw(CDraftContext *pDC, bool bForceFill,bool bHideText,CMDShape* pActiveShape)
{
	if(!m_bHidden)
	{
		pDC->SetGreyMode(m_bGrayed && !bForceFill);
		POSITION pos=m_ShapeList.GetTailPosition();
      int iShape = 0;
		while(pos)
		{
			if(GetAsyncKeyState(VK_CONTROL)<0 && GetAsyncKeyState(VK_CANCEL)<0)
				return true;
			CMDShape* pShape=m_ShapeList.GetPrev(pos);
			if(pShape==pActiveShape) continue;
			if(bHideText && pShape->IsKindOf(RUNTIME_CLASS(CMDText))) continue;

			LongRect lrc=pShape->GetExtents();
			pShape->ExtendForPen(lrc);
         if (pDC->m_bPrinting)
         {
            if (pDC->RectVisible(lrc)) {
               UseRect userect(pDC, pShape);

               iShape++;
               pShape->DrawShape(pDC);

            }
         }
         else
         {
            if (!pDC->m_bCheckRectVisible || pDC->RectActiveClient(lrc)) {
               UseRect userect(pDC, pShape);

               iShape++;
               pShape->DrawShape(pDC);

            }

         }
		}
	}
	return false;
}

CMDShape* CLayer::ShapeFromPoint(const pointd& pt, CMapper* pMapper,bool bHideText,bool bNoGroup,CMDShape* pStart)
{
	POSITION pos,epos=NULL;
	if(m_ShapeList.GetCount()>1 && pStart)
	{
		pos=m_ShapeList.Find(pStart);
		if(pos)
		{
			epos=pos;
			m_ShapeList.GetNext(pos);
		}
	}
	else pos=m_ShapeList.GetHeadPosition();
	while(pos!=epos)
	{
		if(pStart && pos==NULL) pos=m_ShapeList.GetHeadPosition();
		CMDShape* pShape=m_ShapeList.GetNext(pos);
		if(bNoGroup && pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
		{
			CMDGroupShape* pGroup=(CMDGroupShape*)pShape;
			pShape=pGroup->MemberFromPoint(pt,pMapper,bNoGroup);
			if(pShape) return pShape;
		}
		else
		{
			if(pShape->TestSelect(pt, pMapper))
			{
				if(bHideText && pShape->IsKindOf(RUNTIME_CLASS(CMDText))) continue;
				return pShape;
			}
		}
	}
	return NULL;
}

CMDShape* CLayer::ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex)
{
	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_ShapeList.GetNext(pos)->ShapeFromEdge(pEdge,nFromVertex,nToVertex);
		if(pShape) return pShape;
	}
	return NULL;
}

extern long gcd (register long u,register long v);

bool CLayer::SetScale(int nScale, CPCDraftDoc* pDoc)
{
	LongRatio srcRatio,dstRatio;
	pDoc->GetScaleRatio(m_nScale,srcRatio);
	pDoc->GetScaleRatio(nScale,dstRatio);
	return Rescale(nScale,pDoc,srcRatio,dstRatio);
}


bool CLayer::Rescale(int nScale, CPCDraftDoc* pDoc,const LongRatio& oldRatio,const LongRatio& newRatio,double factor)
{
	LongRatio changeRatio;
	changeRatio.fN=newRatio.fN*oldRatio.fD;
	changeRatio.fD=newRatio.fD*oldRatio.fN;
	long theGCD=gcd(changeRatio.fN,changeRatio.fD);
	changeRatio.fN/=theGCD;
	changeRatio.fD/=theGCD;

	if(changeRatio.fN>changeRatio.fD)
	{
		LongRect lrc=GetCumulativeExtents();
		LongPoint lpt=lrc.GetCorner(2);
		if(lpt.Rescale(changeRatio) || !pDoc->IsLegalDocSize(lpt))
			return false;
	}

	bool shownTextRescaleMessage = false;
	bool rescaleTextShapes = false;

	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_ShapeList.GetNext(pos);
		if(!pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
		{
			pShape->DoShapeScale(changeRatio,factor);	
		}
		else
		{
			if(changeRatio.fD!=1.0 || changeRatio.fN!=1.0 || (factor && factor!=1.0))
			{
				if(!shownTextRescaleMessage)
				{
					rescaleTextShapes= (IDYES==AfxMessageBox(IDS_INSERT_TEXT_WITH_DIFFERENT_SCALE, MB_YESNO|MB_DEFBUTTON2));
					shownTextRescaleMessage=true;
				} 
				if(rescaleTextShapes)
				{
					pShape->DoShapeScale(changeRatio,factor);	
				}
			}
			else
			{
				pShape->DoShapeScale(changeRatio,factor);	
			}	
		}
	}
	
	if(factor && m_nSnap) m_nSnap=3;	// revert to default snap
	m_nScale=nScale;
	return true;
}

bool CLayer::ReplaceShape(CMDShape *pShape, CMDShape *pNewShape)
{
	POSITION rpos=m_ShapeList.Find(pShape);
	if(rpos)
	{
		m_ShapeList.InsertAfter(rpos,pNewShape);
		pShape->DeleteLinks(&m_LinkVertexList);
		m_ShapeList.RemoveAt(rpos);
/*		if(pShape->IsAreaShown())
		{
			if(pNewShape->HaveArea()
			pNewShape->Attach(pShape->Detach());
		}
*/		delete pShape;
		return true;
	}
	return false;
}

bool CLayer::ReplaceShape(CMDShape *pShape, CMDShapeList *pNewShapes)
{
	POSITION rpos=m_ShapeList.Find(pShape);
	if(rpos)
	{
		POSITION pos=pNewShapes->GetTailPosition();
		while(pos)
		{
			m_ShapeList.InsertAfter(rpos,pNewShapes->GetPrev(pos));
		}
		pShape->DeleteLinks(&m_LinkVertexList);
		m_ShapeList.RemoveAt(rpos);
		delete pShape;
		return true;
	}
	return false;
}

POSITION CLayer::TopmostShape(CMDShapeList *pList)
{
	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		POSITION npos=pList->Find(m_ShapeList.GetNext(pos));
		if(npos) return npos;
	}
	return NULL;
}

void CLayer::SortShapes(CMDShapeList &list,bool bRemoveLocked)
{
	CMDShapeList workList;
	workList.AddTail(&list);
	list.RemoveAll();
	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		POSITION npos=workList.Find(m_ShapeList.GetNext(pos));
		if(npos)
		{
			CMDShape* pShape=workList.GetAt(npos);
			workList.RemoveAt(npos);
			if(!bRemoveLocked || !pShape->IsLocked())
				list.AddTail(pShape);
		}
	}
}

void CLayer::UpdateDependentShapes(CPCDraftView *pView)
{
	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_ShapeList.GetNext(pos);
		pShape->UpdateDependentShapes(pView);
	}
}

void CLayer::DeleteAllShapes()
{
	while(!m_LinkVertexList.IsEmpty())
		delete m_LinkVertexList.RemoveHead();
	while(!m_ShapeList.IsEmpty())
		delete m_ShapeList.RemoveHead();
}

LongRect CLayer::GetCumulativeExtents(bool bBasic)
{
	LongRect lrcExtent(0,0,-1,-1);
	POSITION pos;
	pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_ShapeList.GetNext(pos);
		if(lrcExtent.IsEmpty())
		{
			lrcExtent=pShape->CalculateBasicExtents();
			if(!bBasic)	pShape->ExtendForPen(lrcExtent);
		}
		else
		{
			LongRect lrc=pShape->CalculateBasicExtents();
			if(!bBasic)	pShape->ExtendForPen(lrc);
			lrcExtent|=lrc;
		}
	}
	return lrcExtent;
}

int CLayer::GetObjectCount(bool bOverall)
{
	if(bOverall)
	{
		int nCount=0;
		POSITION pos;
		pos=m_ShapeList.GetHeadPosition();
		while(pos)
		{
			CMDShape* pShape=m_ShapeList.GetNext(pos);
			nCount+=pShape->GetObjectCount();
		}
		return nCount;
	}
	return m_ShapeList.GetCount();
}

void CLayer::RecalcGroupExtents(CMDShape *pShape)
{
	bool bFound=false;
	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos && !bFound)
	{
		CMDShape* pMemberShape=m_ShapeList.GetNext(pos);
		if(pMemberShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
			bFound=((CMDGroupShape*)pMemberShape)->RecalcExtents(pShape);
		else bFound=(pMemberShape==pShape);
	}
}

void CLayer::MakeSelfContainedShapes(CPCDraftDoc* pDoc)
{
	POSITION pos=m_ShapeList.GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_ShapeList.GetNext(pos);
		if(pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr))) {} else 
			pShape->MakeSelfContained(pDoc, true);
	}
}