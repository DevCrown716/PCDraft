// StarSketcher.cpp: implementation of the CStarSketcher class.
// Infinisys Ltd
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StarSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDPolygonShape.h"
#include "MDGroupShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CStarSketcher::CStarSketcher(Mode nMode,int nSides,int iThickness): m_nMode(nMode),m_nSides(nSides),m_iThickness(iThickness)
{
   m_pPoints=new LongPoint[nSides * 2];
}

CStarSketcher::~CStarSketcher()
{
   delete[] m_pPoints;
}

void CStarSketcher::OnBeginSketch(CDC* pDC,const LongPoint& pt)
{
   CSketcher::OnBeginSketch(pDC,pt);
   m_ptFrom=m_ptTo=pt;
}

void CStarSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
   m_ptTo=ptUpdate;
   m_pView->GetDocument()->SnapToAngle(m_ptTo,m_ptFrom,GetKeyState(VK_SHIFT)<0);
   int nType=3;	// D
   if(m_nMode == kCenterVertex) nType=4; // R
   m_pView->SetShowSize(0,nType,Pythag(m_ptTo - m_ptFrom));
}

void CStarSketcher::OnDrawSketcher(CDC * pDC)
{
   int i;
   Angle trackAngle=mtFastATan2(m_ptFrom - m_ptTo);
   if(trackAngle == FixedZero)
      trackAngle=k360;
   Angle sideAngle=k360 / (short)m_nSides;
   Angle halfSideAngle=sideAngle >> 1;

   TSinCos sinCos(halfSideAngle);

   //WrFixed oddRatio = sinCos.fCos / (1.0 + sinCos.fCos);

   //WrFixed hundredRatio = sinCos.fCos / (1.0 + sinCos.fCos);
   //WrFixed hundredRatio = 1.0;

   double dHundred = sinCos.fCos ;

   WrFixed oddRatio = 0.5;

   LongPoint centerPt=m_ptFrom;
   LongPoint polyPt=m_ptTo;
   LongPoint polyPtInt=m_ptTo;
   bool onSide = false;

   switch(m_nMode)
   {
   case kDiameter:
      if(m_nSides % 2)
      {
         centerPt=m_ptTo - (m_ptTo - m_ptFrom)*oddRatio;
         polyPt=m_ptFrom;
      }
      else
      {
         //onSide=true;
         centerPt=Average(m_ptFrom,m_ptTo);
      }
      break;
   case kCenterVertex:
      if(m_nSides % 2)
         polyPt=m_ptTo + (centerPt - m_ptTo) / oddRatio;
      else
         onSide=true;
      break;

   }

   if(onSide)
   {
      polyPt=centerPt + (polyPt - centerPt) / sinCos.fCos;
      polyPt=mtRotPoint(polyPt,sinCos,centerPt);
   }

   polyPtInt = (polyPt - centerPt) * ((double)m_iThickness * dHundred / 100.0) + centerPt;

   m_pPoints[0]=polyPt;

   Angle rotAngle=FixedZero;
   sideAngle /= 2.0;
   for(i=1;i<m_nSides*2;i+=2)
   {
      rotAngle-=sideAngle;
      m_pPoints[i]=mtRotPoint(polyPtInt,rotAngle,centerPt);

      if(i + 1 >= m_nSides * 2)
      {
         break;
      }
      rotAngle-=sideAngle;
      m_pPoints[i + 1]=mtRotPoint(polyPt,rotAngle,centerPt);
   }

   pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pPoints[0]));
   for(i=1;i<m_nSides*2;i++)
      pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pPoints[i]));
   pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pPoints[0]));
   // draw carcass
   pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_ptFrom));
   pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ptTo));
}

bool CStarSketcher::OnCompleteSketch(CDC *pDC,const LongPoint& pt)
{
   CSketcher::OnCompleteSketch(pDC,pt);
   if(m_ptFrom != m_ptTo)
   {
      m_pView->SaveUndo(IDS_CREATION);
      CMDPolygonShape* pShape=new CMDPolygonShape(CMDPolygonShape::kPolygon,m_nSides * 2,true);

      for(int i=0;i<m_nSides *2;i++)
         pShape->SetShapePoint(i,m_pPoints[i]);

      pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
      pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
      pShape->CalculateExtents();
      pShape->SetShapeDatum(kDefaultDatum);

      CMDGroupShape* pGroup=new CMDGroupShape(pShape);
      pGroup->CalculateExtents();
      pGroup->SetShapeDatum(kDefaultDatum);

      m_pView->GetDocument()->CreateShape(pGroup);
      m_pView->SelectShape(pGroup,false);
      return true;
   }
   return false;
}
