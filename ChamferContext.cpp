// ChamferContext.cpp: implementation of the CChamferContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "ChamferContext.h"

#include "gu.h"
#include "phmath.h"
#include "MDLineShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CChamferContext::CChamferContext()
{
	m_fLine1SetBack=m_fLine2SetBack=WrFixedFromFixed(72<<14);
}

CChamferContext::~CChamferContext()
{
}

bool CChamferContext::CanChamfer(CMDShapeList *pList)
{
	LongPoint lpt1,lpt2,lpt3,lpt4;
	if(Have2IntersectLine(pList,lpt1,lpt2,lpt3,lpt4))
	{
		Coord dist11=Pythag(m_lipt-lpt1);
		Coord dist12=Pythag(m_lipt-lpt2);
		if(dist11>=dist12) m_lptFar1=lpt1;
		else m_lptFar1=lpt2;

		Coord dist21=Pythag(m_lipt-lpt3);
		Coord dist22=Pythag(m_lipt-lpt4);
		if(dist21>=dist22) m_lptFar2=lpt3;
		else m_lptFar2=lpt4;

		return true;
	}
	return false;
}

bool CChamferContext::FindSetBack(Coord dist,const LongPoint &lipt, const LongPoint &lptFar, LongPoint &lptSetBack)
{
	if(lipt.y==lptFar.y)
	{
		if(lipt.x<lptFar.x) lptSetBack.x=lipt.x+dist;
		else lptSetBack.x=lipt.x-dist;
		lptSetBack.y=lipt.y;
	}
	else if(lipt.x==lptFar.x)
	{
		if(lipt.y<lptFar.y) lptSetBack.y=lipt.y+dist;
		else lptSetBack.y=lipt.y-dist;
		lptSetBack.x=lipt.x;
	}
	else
	{
		lptSetBack=lipt;
		lptSetBack.x+=dist;
		lptSetBack=mtRotPoint(lptSetBack,mtAngle(lipt,lptFar),lipt);
	}

	return false;
}

CMDLineShape* CChamferContext::CreateChamfer()
{
	CMDLineShape* pLine=NULL;
	if(Pythag(m_lptFar1-m_lipt) < m_fLine1SetBack || Pythag(m_lptFar2-m_lipt) < m_fLine2SetBack)
	{
		AfxMessageBox(IDS_CHAMFER_LENGTH_ERR);
	}
	else
	{
		LongPoint lptSetBack1,lptSetBack2;
		FindSetBack(m_fLine1SetBack,m_lipt,m_lptFar1,lptSetBack1);
		FindSetBack(m_fLine2SetBack,m_lipt,m_lptFar2,lptSetBack2);

		m_pNewLines[0]=TrimLine(m_lipt,lptSetBack1,m_pLines[0],true,kTrimByNearest);
		m_pNewLines[1]=TrimLine(m_lipt,lptSetBack2,m_pLines[1],true,kTrimByNearest);

		pLine=new CMDLineShape(lptSetBack1,lptSetBack2);
		pLine->SetPenModel(m_pLines[0]->GetPenModel());
		pLine->CalculateExtents();
		pLine->SetShapeDatum(kDefaultDatum);
	}
	return pLine;
}
