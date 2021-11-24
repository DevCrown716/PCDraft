// FilletContext.cpp: implementation of the CFilletContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "FilletContext.h"

#include "gu.h"
#include "phmath.h"
#include "FilletSetupDlg.h"
#include "MDLineShape.h"
#include "MDArcShape.h"
#include "PCDraftView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilletContext::CFilletContext()
{
	m_fRadius=WrFixedFromFixed(72<<14);
	m_bSmallerArc=true;
	m_Fillet[0]=NULL;
	m_Fillet[1]=NULL;
	m_Fillet[2]=NULL;
	m_Fillet[3]=NULL;
	m_bCreating=false;
}

CFilletContext::~CFilletContext()
{
}

bool CFilletContext::CanFillet(CMDShapeList *pList)
{
	LongPoint lpt1,lpt2,lpt3,lpt4;
	if(!m_bCreating && Have2IntersectLine(pList,lpt1,lpt2,lpt3,lpt4))
	{
		return true;
	}
	return false;
}

bool CFilletContext::makeFillet(const LongPoint &lpt1, const LongPoint &lpt2, const LongPoint &lpt3, const LongPoint &lpt4, LongPoint &lptCenter, LongPoint &lip12, LongPoint& lip34)
{
	double ipx,ipy;

	if(!guLongLnLnDblPt(lpt1,lpt2,lpt3,lpt4,&ipx,&ipy)) return false;

	double rad=m_fRadius.GetRawDouble();

	Angle ang1=mtAngle(lpt2,lpt1);
	Angle ang2=mtAngle(lpt4,lpt3);
	Angle p1ipp3AngDiv2=Abs(ang1-ang2)>>1;
	Angle iang=mtFixAngle((ang1+ang2)>>1);

	if(p1ipp3AngDiv2>=k90)
	{
		p1ipp3AngDiv2=Abs(k180-p1ipp3AngDiv2);
		iang=mtFixAngle(iang+k180);
	}

	double vSin=mtSin(iang);
	double vCos=mtCos(iang);

	rad=fabs(rad/mtSin(p1ipp3AngDiv2));

	lptCenter.x.SetFromRawDoubleTruncated(rad * vCos + ipx);
	lptCenter.y.SetFromRawDoubleTruncated(rad * vSin + ipy);

	if(!guNearPtLnPt(lptCenter,lpt1,lpt2,&lip12) || 
		!guNearPtLnPt(lptCenter,lpt3,lpt4,&lip34)) return false;

	return true;
}

int CFilletContext::CreateFillet()
{
	LongPoint lpt1,lpt2,lpt3,lpt4,lipt;
	lpt1=m_pLines[0]->GetShapePoint(0);
	lpt2=m_pLines[0]->GetShapePoint(1);
	lpt3=m_pLines[1]->GetShapePoint(0);
	lpt4=m_pLines[1]->GetShapePoint(1);

	LongRect rip1,rip2,rip3,rip4;

	rip1=guPt2Rect(m_lipt,lpt1);
	rip2=guPt2Rect(m_lipt,lpt2);
	rip3=guPt2Rect(m_lipt,lpt3);
	rip4=guPt2Rect(m_lipt,lpt4);

	LongPoint lptCenter,lip1,lip2;

	int nCount=0;
	m_Fillet[0]=NULL;

	if(!rip2.Contains(lpt1) && !rip4.Contains(lpt3))
	{
		if(makeFillet(lpt1,lpt2,lpt3,lpt4,lptCenter,lip1,lip2))
		{
			m_Fillet[0]=createArc(lptCenter,lip1,lip2);
			nCount++;
		}
	}

	m_Fillet[1]=NULL;
	if(!rip2.Contains(lpt1) && !rip3.Contains(lpt4))
	{
		if(makeFillet(lpt1,lpt2,lpt4,lpt3,lptCenter,lip1,lip2))
		{
			m_Fillet[1]=createArc(lptCenter,lip1,lip2);
			nCount++;
		}
	}

	m_Fillet[2]=NULL;
	if(!rip1.Contains(lpt2) && !rip4.Contains(lpt3))
	{
		if(makeFillet(lpt2,lpt1,lpt3,lpt4,lptCenter,lip1,lip2))
		{
			m_Fillet[2]=createArc(lptCenter,lip1,lip2);
			nCount++;
		}
	}

	m_Fillet[3]=NULL;
	if(!rip1.Contains(lpt2) && !rip3.Contains(lpt4))
	{
		if(makeFillet(lpt2,lpt1,lpt4,lpt3,lptCenter,lip1,lip2))
		{
			m_Fillet[3]=createArc(lptCenter,lip1,lip2);
			nCount++;
		}
	}

	if(nCount)
	{
		if(nCount>1)
		{
			m_bCreating=true;
			return -1;
		}
		for(int i=0;i<4;i++) if(m_Fillet[i]) return i+1;
	}
	return 0;
}

void CFilletContext::DrawFillet(CDraftContext* pDC)
{
	for(int i=0;i<4;i++)
	{
		if(m_Fillet[i])
		{
			m_Fillet[i]->DrawShape(pDC);
			m_Fillet[i]->DrawHandles(pDC);
		}
	}
}

int CFilletContext::TestFillet(const LongPoint& lpt)
{
	LongPoint lpt1,lpt2,lpt3,lpt4,lipt;
	lpt1=m_pLines[0]->GetShapePoint(0);
	lpt2=m_pLines[0]->GetShapePoint(1);
	lpt3=m_pLines[1]->GetShapePoint(0);
	lpt4=m_pLines[1]->GetShapePoint(1);

	for(int i=0;i<4;i++)
	{
		if(m_Fillet[i])
		{
			LongPoint lptCenter=m_Fillet[i]->GetShapePoint(0);
			LongRect lrc;
			lrc=guPt2Rect(lptCenter,lpt);

			guLongLnLnPt(lpt1,lpt2,lptCenter,lpt,&lipt);
			if(!lrc.Contains(lipt))
			{
				guLongLnLnPt(lpt3,lpt4,lptCenter,lpt,&lipt);
				if(!lrc.Contains(lipt)) return i;
			}
		}
	}
	return -1;
}

void CFilletContext::ClearFillet()
{
	for(int i=0;i<4;i++)
	{
		if(m_Fillet[i])
		{
			delete m_Fillet[i];
			m_Fillet[i]=NULL;
		}
	}
	m_bCreating=false;
}

CMDShape* CFilletContext::FinishCreateFillet(int nFillet)
{
	CMDShape* pFillet=m_Fillet[nFillet];
	m_Fillet[nFillet]=NULL;
	ClearFillet();
	LongPoint lptCenter=pFillet->GetShapePoint(0);
	m_pNewLines[0]=TrimLine(m_lipt,lptCenter,m_pLines[0],true,kTrimByQuadrant);
	m_pNewLines[1]=TrimLine(m_lipt,lptCenter,m_pLines[1],true,kTrimByQuadrant);
	return pFillet;
}

CMDArcShape* CFilletContext::createArc(const LongPoint &lptCenter, const LongPoint& lptFrom, const LongPoint& lptTo)
{
	CMDArcShape* pShape=new CMDArcShape;
	pShape->SetShapePoint(0,lptCenter);
	pShape->SetShapePoint(1,lptFrom);
	pShape->SetShapePoint(2,lptTo);

	Angle fromAngle=mtAngle(lptCenter,lptFrom);
	Angle toAngle=mtAngle(lptCenter,lptTo);
	int arcDir;
	if(fromAngle<toAngle)
	{
		arcDir=AD_CLOCKWISE;
		if((toAngle-fromAngle)<180)
		{
			if(m_bSmallerArc) arcDir=AD_COUNTERCLOCKWISE;
		}
		else
		{
			if(!m_bSmallerArc) arcDir=AD_COUNTERCLOCKWISE;
		}
	}
	else
	{
		arcDir=AD_COUNTERCLOCKWISE;
		if((fromAngle-toAngle)<180)
		{
			if(m_bSmallerArc) arcDir=AD_CLOCKWISE;
		}
		else
		{
			if(!m_bSmallerArc) arcDir=AD_CLOCKWISE;
		}
	}
	pShape->m_nArcDirection=arcDir;
	pShape->SetPenModel(m_pLines[0]->GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	return pShape;
}
