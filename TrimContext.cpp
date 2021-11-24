// TrimContext.cpp: implementation of the CTrimContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "TrimContext.h"

#include "gu.h"
#include "MDLineShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrimContext::CTrimContext()
{
	m_nTrimMethod=0;
	m_pLines[0]=NULL;
	m_pLines[1]=NULL;
	m_pNewLines[0]=NULL;
	m_pNewLines[1]=NULL;
}

CTrimContext::~CTrimContext()
{
}

bool CTrimContext::Have2IntersectLine(CMDShapeList *pList,LongPoint& lpt1,LongPoint& lpt2,LongPoint& lpt3,LongPoint& lpt4)
{
	if(pList->GetCount()==2)
	{
		CMDShape* pLines[2];
		pLines[0]=pList->GetHead();
		pLines[1]=pList->GetTail();
		if(!pLines[0]->IsLocked() && pLines[0]->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape) && !pLines[1]->IsLocked() && pLines[1]->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape))
		{
			lpt1=pLines[0]->GetShapePoint(0);
			lpt2=pLines[0]->GetShapePoint(1);
			lpt3=pLines[1]->GetShapePoint(0);
			lpt4=pLines[1]->GetShapePoint(1);

			LongPoint lipt;
			if(guLongLnLnPt(lpt1,lpt2,lpt3,lpt4,&lipt))
			{
				m_pLines[0]=(CMDLineShape*)pLines[0];
				m_pLines[1]=(CMDLineShape*)pLines[1];
				m_lipt=lipt;
				return true;
			}
		}
	}
	return false;
}

bool CTrimContext::CanTrim(CMDShapeList *pList, bool bBoth)
{
	LongPoint lpt1,lpt2,lpt3,lpt4,lipt;
	if(Have2IntersectLine(pList,lpt1,lpt2,lpt3,lpt4))
	{
		LongRect r1,r2,r;
		r1.SetToBound(lpt1,lpt2);
		r2.SetToBound(lpt3,lpt4);

		if(guEmptyRect(r1))			/* in case of horizontal/vertical line */
		{
			if(r1.left==r1.right) r1.Inset(-FixedMinDelta, FixedZero);
			else r1.Inset(FixedZero, -FixedMinDelta);
		}

		if(guEmptyRect(r2))
		{
			if (r2.left==r2.right) r2.Inset(-FixedMinDelta, FixedZero);
			else r2.Inset(FixedZero, -FixedMinDelta);
		}

		if(bBoth)
		{
			if(guSectRect(r1,r2,&r) && r.Contains(m_lipt)) return true;
		}
		else
		{
			if(r1.Contains(m_lipt) || r2.Contains(m_lipt)) return true;
		}
	}
	return false;
}

void CTrimContext::Trim()
{
	m_pNewLines[0]=TrimLine(m_lipt,m_lipt,m_pLines[0],false);
	m_pNewLines[1]=TrimLine(m_lipt,m_lipt,m_pLines[1],false);
}

CMDLineShape* CTrimContext::TrimLine(const LongPoint &lipt,const LongPoint &lpt,CMDLineShape *pLine,bool bExtend,int nTrimBy)
{
	CMDLineShape* pNewLine=NULL;

	LongPoint lpt1,lpt2,ltpt;
	lpt1=pLine->GetShapePoint(0);
	lpt2=pLine->GetShapePoint(1);
	LongRect r=guPt2Rect(lpt1,lpt2);
	r.Inset(-FixedMinDelta,-FixedMinDelta);

	guNearPtLnPt(lpt,lpt1,lpt2,&ltpt);

	if(bExtend && !r.Contains(ltpt))
	{
		double dist1 = RawDoublePythagSqr(lpt1-ltpt);
		double dist2 = RawDoublePythagSqr(lpt2-ltpt);
		if(dist1>dist2) pLine->SetShapePoint(1,ltpt);
		else pLine->SetShapePoint(0,ltpt);
		pLine->CalculateExtents();
	}
	else
	{
		if(m_nTrimMethod!=kNoTrim)
		{
			if(m_nTrimMethod==kManualTrim)
			{
				if(lpt1!=ltpt && lpt2!=ltpt)
				{
					pNewLine=(CMDLineShape*)pLine->CreateCopy();
					pNewLine->SetShapePoint(0,ltpt);
					pLine->SetShapePoint(1,ltpt);
				}
			}
			else	// kAutoTrim
			{
				double dist1,dist2;
				switch(nTrimBy)
				{
				case  kTrimByDist:
					dist1=RawDoublePythagSqr(lpt1-ltpt);
					dist2=RawDoublePythagSqr(lpt2-ltpt);
					break;
				case kTrimByNearest:
					dist1=RawDoublePythagSqr(lpt1-lipt);
					dist2=RawDoublePythagSqr(lpt2-lipt);
					break;
				case kTrimByQuadrant:
					dist1=RawDoublePythagSqr(lpt1-lipt);
					dist2=RawDoublePythagSqr(lpt1-ltpt);
					break;
				}
				if(dist1>=dist2)
				{
					if(dist1==dist2 && kTrimByDist==nTrimBy)
					{
						pNewLine=(CMDLineShape*)pLine->CreateCopy();
						pNewLine->SetShapePoint(0,ltpt);
						pLine->SetShapePoint(1,ltpt);
					}
					pLine->SetShapePoint(1,ltpt);
				}
				else pLine->SetShapePoint(0,ltpt);
				pLine->CalculateExtents();
			}
			if(pNewLine)
			{
				pNewLine->CalculateExtents();
				pNewLine->SetShapeDatum(kDefaultDatum);
			}
		}
	}
	return pNewLine;
}
