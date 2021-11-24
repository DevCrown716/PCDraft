// AttachContext.cpp: implementation of the CAttachContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "AttachContext.h"

#include "gu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
bool CAttachContext::CanAttach(CMDShapeList *pList)
{
	if(pList->GetCount()>1)
	{
		CMDShape* pShape;
		CMDShapeList workList;
		workList.AddTail(pList);

		pShape=workList.RemoveHead();
		if(IsValidShape(pShape))
		{
			DeleteEdges();
			AddEdges(pShape,true,false);

			LongPoint lptFirst,lptLast;
			lptFirst=pShape->GetFirstShapePoint();
			lptLast=pShape->GetLastShapePoint();
			int nCount=1;
			while(FindNextCoincide(&workList,lptFirst,lptLast)) nCount++;
			if(workList.GetCount()==0 && nCount>1) return true;
			DeleteEdges();
		}
	}
	return false;
}

bool CAttachContext::IsValidShape(CMDShape *pShape)
{
	return !pShape->IsLocked() && !pShape->IsClosed() && pShape->GetNumShapeEdges()>0 && !pShape->IsAreaShown();
}

bool CAttachContext::FindNextCoincide(CMDShapeList *pList, LongPoint &lptFirst, LongPoint &lptLast)
{
	bool bFound=false;
	POSITION pos=pList->GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=pList->GetNext(pos);

		if(IsValidShape(pShape))
		{

			LongPoint lptNewFirst,lptNewLast;
			lptNewFirst=pShape->GetFirstShapePoint();
			lptNewLast=pShape->GetLastShapePoint();

			if(guAlmostEqualPt(lptLast,lptNewFirst))
			{
				lptLast=lptNewLast;
				AddEdges(pShape,true,false);
				bFound=true;
			}
			else if(guAlmostEqualPt(lptLast,lptNewLast))
			{
				lptLast=lptNewFirst;
				AddEdges(pShape,true,true);
				bFound=true;
			}
			else if(guAlmostEqualPt(lptFirst,lptNewLast))
			{
				lptFirst=lptNewFirst;
				AddEdges(pShape,false,false);
				bFound=true;
			}
			else if(guAlmostEqualPt(lptFirst,lptNewFirst))
			{
				lptFirst=lptNewLast;
				AddEdges(pShape,false,true);
				bFound=true;
			}
				
			if(bFound)
			{
				pList->RemoveAt(pList->Find(pShape));
				break;
			}
		}
	}
	return bFound;
}
