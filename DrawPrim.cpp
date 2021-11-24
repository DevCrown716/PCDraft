// DrawPrim.cpp: implementation of the CDrawPrim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawPrim.h"
#include "PCDraftDoc.h"

#include "phmath.h"
#include "DraftContext.h"

void copy(Gdiplus::Rect & r,LPCRECT lpcrect); // Interface Update - Infinisys Ltd.

void copy(Gdiplus::RectF & r,LPCRECT lpcrect); // Interface Update - Infinisys Ltd.

//////////////////////////////////////////////////////////////////////

// screenPenW -> gAppl.fLineZoomOn
// setDashTable in setForPen ??

//		ScreenFromIntl ( *lPt1 ) => pMapper->LPtoDP(*lPt1);
//		guSNoZoom2Zoom => pMapper->NoZoomToZoom(penW)
//	Coord _guLOrd ( short aShort ) { return Coord ( aShort ); } => WrFixed(short)

// Replace DrawDoc -> CMapper
// int pMapper->NoZoomToZoom(WrFixed penW) -> LPtoDP

// lineAdjust from RedrawShape.c
//#define kDimEndmarkOut			64 // from DimStruct.h

void		lineAdjust (
	LongPointPtr		p1,
	LongPointPtr		p2,
	Coord					adjust1,
	Coord					adjust2)
{
	LongPoint			delta(0,0) ;

	Angle thetaRadians = ATan2 ( *p1 - *p2 ) ;

	WrFixed cosTheta = Cos ( thetaRadians ) ;
	WrFixed sinTheta = Sin ( thetaRadians ) ;

	/* note Fixed format is same as longPoint format */

	if ( adjust1 != FixedZero )
	{
		if(p1->y!=p2->y) delta.y = adjust1 * sinTheta ;
		if(p1->x!=p2->x) delta.x = adjust1 * cosTheta ;
		*p1 -= delta ;
	}

	if ( adjust2 != FixedZero )
	{
		if(p1->y!=p2->y) delta.y = adjust2 * sinTheta ;
		if(p1->x!=p2->x) delta.x = adjust2 * cosTheta ;
		*p2 += delta ;
	}
}

CDrawPrim::CDrawPrim()
{
#ifdef DEBUG
   m_iDebug = 0;
   m_bPrev1 = false;
#endif
}

CDrawPrim::~CDrawPrim()
{
}

void	CDrawPrim::setDashTable(const CDrawPrim::DashTable* inDashStyles)
{
	m_DashTable = inDashStyles;
   m_DLSelect = 0;
   m_dDistanceWeNeedToCover = 0.;
   m_DashesIdx = -1;
	m_CurrentPt = pointd(0,0);
}
int g_iTest=0;
void	CDrawPrim::dashLine(CDraftContext* pDC,const pointd& startPt,const pointd& endPt, bool bForceDashing)
{
	short			dashCode/*, halfPenWidth*/;
	uchar			*dashCVector;

	const PenModel* penModelPtr=&pDC->GetPen();
	
//	if( penModelPtr->m_nIndex>=0)
	if(penModelPtr->m_nPenType!=kPenInvis && (penModelPtr->m_nPenType==kPenPatEmbed || penModelPtr->m_nIndex>=0))
	{
		dashCode = penModelPtr->Dashing();
		if(dashCode && (!(((CPCDraftApp*)AfxGetApp())->m_bHideLineStyles) || bForceDashing))
		{	
//			halfPenWidth = pMapper->ScreenPenW(penModelPtr->m_nWidth) >> 1;
			initDashCodes(startPt, dashCode/*, halfPenWidth*/);
			dashCVector = (uchar *)(((DashStyle *)m_DashTable) + m_DLSelect);	/* adding 1 add 16 bytes */

			//doDashLine (pDC, m_CurrentPt, endPt, dashCVector+1, penModelPtr->m_nWidth);
         // DOUBT
         doDashLine(pDC, m_CurrentPt, endPt, dashCVector + 1, penModelPtr->m_nWidth);
			m_CurrentPt = endPt;
			return;
		}

		CMapper* pMapper=pDC->GetMapper();
		short nPenW=pMapper->ScreenPenW(penModelPtr->m_nWidth);
		
		// by TSM
		//if(pDC->IsCreatingPath() || pDC->IsOutline() ||  (penModelPtr->m_nPenType<kPenPat))
      // Infinisys Ltd. GDI+
      if (pDC->IsCreatingPath() || pDC->IsOutline() || nPenW <= 1)
		{
         if (pDC->m_bUseGDI)
         {

            CDC* pCDC = pDC->GetCDC();

            CPoint pt = startPt;

            if (pCDC->GetCurrentPosition() != pt)
            {
            
               pDC->MoveTo(pt);

            }

            pCDC->LineTo(CPoint(endPt.x, endPt.y));

         }
         else
         {

            if (pDC->m_ppath)
            {

               pDC->m_ppath->AddLine(__pointf(startPt), __pointf(endPt));

            }
            else
            {

               pDC->graphics_on();

               pDC->m_pgraphics->DrawLine(pDC->m_PenPlus.get(), __pointf(startPt), __pointf(endPt));

            }

         }

		}
		else
		{
         LongPoint lpt1;
         LongPoint lpt2;
         lpt1=pMapper->DPtoLP(startPt);
         lpt2=pMapper->DPtoLP(endPt);
			LongPoint pp[4];
			guParallelLn(lpt1,lpt2,pMapper->PenW(penModelPtr->m_nWidth) / 10,kCenterLine,pp,pp+3,pp+1,pp+2);
         pointd pa[4];
			for(int i=0;i<4;i++) pMapper->LPtoDP(pa[i],pp[i]);
			if(pa[0].x==pa[1].x && pa[0].y==pa[1].y)
			{
				pa[1].x--;
				pa[1].y--;
			}
			if(pa[3].x==pa[2].x && pa[3].y==pa[2].y)
			{
				pa[2].x--;
				pa[2].y--;
			}

			CRgn rgn;

			if(!penModelPtr->isPenStyleRound())
         {
			
            pDC->CloseJoint(lpt1,pa);

				pDC->OpenJoint(lpt2,pa+2);

            pointf pfa[4];

            __copy(pfa, pa, 4);


            m_pfHead = pfa[2];

            //m_helperPoly.PushHeadPoint(pfa[2], pDC);
            m_helperPoly.PushPoints(pfa[0], pfa[1], pDC);

            m_pfTail = pfa[3];

            //m_helperPoly.PushTailPoint(pfa[3], pDC);

            //pointf pfa[4];

            //__copy(pfa, pa, 4);
            ////if (g_iTest == 20)
            ////{
            ////   OutputDebugString("20");
            ////}
            ////else if (g_iTest == 21)
            ////{
            ////   OutputDebugString("21");
            ////}
            ////else if (g_iTest == 22)
            ////{
            ////   OutputDebugString("22");
            ////}
            ////else if (g_iTest == 23)
            ////{
            ////   OutputDebugString("23");
            ////}
            ////else if (g_iTest == 24)
            ////{
            ////   OutputDebugString("24");
            ////}
            ////else if (g_iTest == 25)
            ////{
            ////   OutputDebugString("25");
            ////}
            ////else
            //{
            //   if (pDC->m_iPathAccumul > 0)
            //   {

            //      if (pDC->m_pointa1.GetCount() >= 2)
            //      {

            //         pDC->m_pointa1.RemoveAt(pDC->m_pointa1.GetUpperBound());
            //         pDC->m_pointa2.RemoveAt(0);

            //      }

            //      pDC->m_pointa1.Add(pfa[0]);
            //      pDC->m_pointa1.Add(pfa[2]);
            //      pDC->m_pointa2.InsertAt(0, pfa[3]);
            //      pDC->m_pointa2.InsertAt(1, pfa[1]);


            //   }
            //   else
            //   {
            //      pDC->path_new(false);
            //      // rgn.CreatePolygonRgn(pt, 4, ALTERNATE);
            //      pDC->m_ppath->AddPolygon(pfa, 4);
            //      pDC->PaintPenPath();
            //      pDC->path_destroy(false);
            //   }
            //}
            g_iTest++;
            //pDC->graphics_on();

            //pDC->m_pgraphics->DrawLine(pDC->m_PenPlus.get(), __pointf(startPt), __pointf(endPt));



//				rgn.CreatePolygonRgn(pa,4,ALTERNATE);
			}else{
				pDC->ResetJoint();
				CDC* pCDC=pDC->GetCDC();
				CGDIArcDirection dir(pCDC, AD_CLOCKWISE);


				if(pCDC->GetCurrentPosition()!=CPoint(pa[0].x, pa[1].y)) pCDC->MoveTo(CPoint(pa[0].x, pa[1].y));

				pCDC->BeginPath();

				WrFixed halfWidth = pMapper->PenW(penModelPtr->m_nWidth)/2;
				
				CRect boundRect;
				LongPoint pt1(lpt1.x-halfWidth, lpt1.y-halfWidth);
				LongPoint pt2(lpt1.x+halfWidth, lpt1.y+ halfWidth);
			
				boundRect.SetRect(pMapper->LPtoDP(pt1), pMapper->LPtoDP(pt2));
				bool oldOutline;
				oldOutline= pDC->m_bOutline;
				pDC->m_bOutline=true;
            pDC->path_new(false);
				pDC->DrawArc(lpt1, LongPoint(halfWidth, halfWidth), pp[0], pp[1],AD_CLOCKWISE,0,
						0, 0, false);
				pDC->m_bOutline = oldOutline;

	//			m_pDC->ArcTo( &boundRect, pa[0], pa[1]);
            pCDC->LineTo(CPoint(pa[2].x, pa[2].y));

				pt1=LongPoint(lpt2.x+halfWidth, lpt2.y + halfWidth);
				pt2=LongPoint(lpt2.x-halfWidth, lpt2.y - halfWidth);
				boundRect.SetRect(pMapper->LPtoDP(pt1), pMapper->LPtoDP(pt2));
				oldOutline= pDC->m_bOutline;
				pDC->m_bOutline=true;
				
				pDC->DrawArc(lpt2, LongPoint(halfWidth, halfWidth), pp[2], pp[3],AD_CLOCKWISE, 0,
						0, 0, false);
				pDC->m_bOutline = oldOutline;

	//			m_pDC->ArcTo( &boundRect, pa[2], pa[3]);
				pCDC->LineTo(CPoint(pa[0].x, pa[0].y));
				pCDC->EndPath();
				//if(rgn.CreateFromPath(pCDC)){
    //           CPoint ptOrg;
				//	GetViewportOrgEx(pCDC->GetSafeHdc(),&ptOrg);
				//	rgn.OffsetRgn(-ptOrg);	// temporary fix, should not be here
				//}
            pDC->PaintPenPath();
         }
			pDC->SetCurrentPos(lpt2);
		}
	}
}


void CDrawPrim::dashLineTo(CDraftContext* pDC,const pointd& endPoint)
{
	const PenModel* penModelPtr=&pDC->GetPen();

	CDC* pCDC=pDC->GetCDC();
	if ( !penModelPtr ){
		pCDC->LineTo (CPoint(endPoint.x, endPoint.y));
	} else {
		if (penModelPtr->m_nPenType == kPenInvis)
			pCDC->LineTo (CPoint(endPoint.x, endPoint.y));
		else{
			short dashCode = penModelPtr->Dashing();
			if (!dashCode)
				pCDC->LineTo(CPoint(endPoint.x, endPoint.y));
			else{
				CMapper* pMapper=pDC->GetMapper();

				short screenPen =pMapper->ScreenPenW(penModelPtr->m_nWidth);
				short halfPenWidth = (screenPen<=1) ? 0 : ((screenPen + 1) >> 1);

				pointd endPt(endPoint.x + halfPenWidth, endPoint.y + halfPenWidth);
				//initDashCodes (m_CurrentPt, dashCode, halfPenWidth);
            initDashCodes(m_CurrentPt, dashCode);
				uchar* dashCVector = (uchar *)((DashStyle *)*m_DashTable + m_DLSelect);	/* adding 1 add 16 bytes */
				doDashLine (pDC, m_CurrentPt, endPt, dashCVector+1, penModelPtr->m_nWidth);
				m_CurrentPt = endPt;
			}
			pCDC->MoveTo(CPoint(endPoint.x, endPoint.y) );
		}
	}
}



#define _HIWORD(l)  (*(((short*) &(l)) + 1))

short	calOffset (	Fixed delta,short offset)
{
	Fixed			tempFixed;
	short			tempOffset;

	tempFixed = 0;
	_HIWORD(tempFixed) = offset;
	tempFixed = FixMul (delta,tempFixed);
	if ( tempFixed < 0 )
		tempOffset = - short(Fix2Long (-tempFixed));
	else
		tempOffset = short(Fix2Long (tempFixed));
	return (tempOffset);
}

void CDrawPrim::doDashLine(CDraftContext* pDC,const pointd& startPt,const pointd& endPt, const uchar *pCode, short	penWidth)
{
//	uchar				tempChar;
	short				i;
	short				dashCodes[kDashCodeEntrySize]; //, *dashSVector;
	double			dashThickness;
   double         dX, dY;
	double   		begLineX, begLineY;
	short				begMapX, begMapY;
	short				*endMapXPtr, *endMapYPtr;
	short				initEndMapX, initEndMapY;
	double			dXAOffset, dYAOffset, dXBOffset, dYBOffset;
   double         dxa,dya,dxb,dyb;
   double         countdown;

	double         dXOverdY, dYOverdX;
	double			mapXStep, mapYStep;
   double		   mapX, mapY;
   double         dStepX,dStepY;
   double         dMapX,dMapY;



	pointd				pointA, pointB, pointC, pointD;
   pointd   pA,pB,pC,pD;
	CRect				srcRect, dstRect;
	CBitmap				srcBitMap;

//	uchar				thisDash[kDashCodeEntrySize];

	register short		segcountdown;
	register BYTE		*bitMapPtr, *mapYPtr;

	CMapper* pMapper=pDC->GetMapper();
/*---------------------------------------------------
		Set up zoomed dash code integer table
---------------------------------------------------*/

//	memcpy( thisDash, pCode, kDashCodeEntrySize-1);

	memset(dashCodes,0xFF,kDashCodeEntrySize*sizeof(short));
//	for ( i = 0; i < kDashCodeEntrySize; dashCodes[i++] = short(0xFFFF));

//	dashSVector = dashCodes;


	if (penWidth)
	{
		dashThickness = pMapper->ScreenPenW(penWidth);

		if (!dashThickness )
			dashThickness = 1;
		for ( i = 0; i < kMaxDashCodes; i++/*, dashSVector++*/)
		{
			if ( (/*tempChar = */pCode[i]) != 0xFF)
			{
            short shDash = pCode[i];
            //int iMap = pMapper->LPtoDP(shDash);
            int iMap = shDash;
            if (iMap <= 0)
            {
               dashCodes[i] = 1;
            }
            else
            {
               dashCodes[i] = iMap;
            }
			}
			else
			{
				dashCodes[i]=-1;
				break;
			}
		}
	}
	else
	{
		
      dashThickness = 1;
		
      m_dDistanceWeNeedToCover = 0.;

      m_DashesIdx = -1;

		for ( i = 0; (i < kMaxDashCodes) && (*pCode != 0xFF); i++ )
			/**dashSVector++*/dashCodes[i] = *pCode++ ;

	}
   //if(pDC->m_PenModel.m_nPenType != kPenInvis && GetObjectType(*pDC->m_pDC) != OBJ_ENHMETADC
   //   &&  pDC->m_PenModel.m_nPenType > kPenColor && pDC->m_iPackedItemPenFill > 0)
   //{
   //   dashThickness++;
   //}

/*--------------------------------------------------------------
		Setup octant line generation parameters.
--------------------------------------------------------------*/

	begLineX = startPt.x;
	begLineY = startPt.y;

   double dZoom = pDC->m_pMapper->LPtoDPF(1);


 //  dX = endPt.x - startPt.x;
	//dY = endPt.y - startPt.y;

	//dXOverdY = dY != 0L ? dX / dY : 0L;

	//dYOverdX = dX != 0L ? dY / dX : 0L;

	//mapX = mapY = 0;
	//if (dY >= 0)
	//{
	//	if (dX >= 0)
	//	{
	//		if (dX >= dY)
	//		{
	//			countdown = dX + 1.0;	/*  Octant 1 Line Generation  */
	//			mapXStep = 1.0;
	//			mapYStep = dYOverdX;
	//			mapY = -1.0;
	//		}
	//		else
	//		{
	//			countdown = dY + 1.0;	/*  Octant 2 Line Generation  */
	//			mapXStep = dXOverdY;
	//			mapYStep = 1.0;
	//			mapX = -1.0;
	//		}
	//	}
	//	else
	//	{
	//		if (dY >= -dX)
	//		{
	//			countdown = dY + 1.0;	/*  Octant 3 Line Generation  */
	//			mapXStep = dXOverdY;
	//			mapYStep = 1.0;
	//			mapX = -1.0;
	//		}
	//		else
	//		{
	//			countdown = -dX + 1.0;	/*  Octant 4 Line Generation  */
	//			mapXStep = -1.0;
	//			mapYStep = -dYOverdX;
	//			mapY = -1.0;
	//		}
	//	}
	//}
	//else
	//{					/* dY < 0 */
	//	if (dX < 0)
	//	{
	//		if (-dX >= -dY)
	//		{
	//			countdown = -dX + 1.0;	/*  Octant 5 Line Generation  */
	//			mapXStep = -1.0;
	//			mapYStep = -dYOverdX;
	//			mapY = -1.0;
	//		}
	//		else
	//		{
	//			countdown = -dY+ 1.0;		/*  Octant 6 Line Generation  */
	//			mapXStep = -dXOverdY;
	//			mapYStep = -1.0;
	//			mapX = -1.0;
	//		}
	//	}
	//	else
	//	{					/* dX >= 0 */
	//		if (-dY > dX)
	//		{
	//			countdown = -dY+ 1.0;		/*  Octant 7 Line Generation  */
	//			mapXStep = -dXOverdY;
	//			mapYStep = -1.0;
	//			mapX = -1.0;
	//		}
	//		else
	//		{
	//			countdown = dX + 1.0;	/*  Octant 8 Line Generation  */
	//			mapXStep = 1.0;
	//			mapYStep = dYOverdX;
	//			mapY = -1.0;
	//		}
	//	}
	//}

   //dStepX = mapXStep * dZoom;
   //dStepY = mapYStep * dZoom;
   dStepX = mapXStep;
   dStepY = mapYStep;

/*--------------------------------------------------------------
		Test for line width greater than 1 and execute
		line with polygons.
--------------------------------------------------------------*/

   m_helperPoly.setFrameIt(dashThickness < 2);
	//if (dashThickness > 1) // Interface Update - Infinisys Ltd.
	{

      //LongRect r;

      //pDC->m_prect = &r;

      pointd delta(endPt - startPt);
      double distance = sqrt((double)(delta.x * delta.x + delta.y * delta.y));
      countdown = distance;
      double hyp = distance;
      double dA = atan2(delta.x, delta.y);
      double vInc = dZoom / 5.0;
      double dX = vInc * sin(dA);
      double dY = vInc * cos(dA);

      double deltaX = cos(-dA) * dashThickness;
      double deltaY = sin(-dA) * dashThickness;

      if (pDC->m_bSwapInsideOutside)
      {

         deltaX = -deltaX;
         deltaY = -deltaY;

      }

      //bool bFirstPointSet = false;


	  // begin dash line with gradient fix - Infinisys Ltd.
      //if(hyp == 0)
      //{
      //   r.left=pDC->m_pMapper->DPtoLP(min(startPt.x,endPt.x) - dashThickness/2 - 1);
      //   r.right=pDC->m_pMapper->DPtoLP(max(endPt.x,startPt.x) + dashThickness/2);
      //   r.top=pDC->m_pMapper->DPtoLP(min(startPt.y,endPt.y) - dashThickness/2 - 1);
      //   r.bottom=pDC->m_pMapper->DPtoLP(max(endPt.y,startPt.y) + dashThickness/2);
      //}
      //else
      //{
      //   pointd ptThickness(delta.y *dashThickness / distance,delta.x *dashThickness / distance);
      //   r.left =  pDC->m_pMapper->DPtoLP(min(startPt.x,endPt.x) - ptThickness.x - 1);
      //   r.right = pDC->m_pMapper->DPtoLP(max(endPt.x,startPt.x) + ptThickness.x);
      //   r.top = pDC->m_pMapper->DPtoLP(min(startPt.y,endPt.y) - ptThickness.y - 1);
      //   r.bottom = pDC->m_pMapper->DPtoLP(max(endPt.y,startPt.y) + ptThickness.y);

      //}
  	  // end dash line with gradient fix - Infinisys Ltd.

		mapX = begLineX;
		mapY = begLineY;

      dMapX = mapX;
      dMapY = mapY;



		//if (hyp==0)
		//{
		//	dYAOffset = delta.x * dashThickness ;
		//	dYBOffset = -dYAOffset * 0.5;
		//	dYAOffset += dYBOffset;
  //       dya = delta.x * dashThickness;
  //       dyb = - dya * 0.5;
  //       dya += dyb;

		//	dXAOffset = - delta.y * dashThickness ;
		//	dXBOffset = -dXAOffset * 0.5;
		//	dXAOffset += dXBOffset;
  //       dxa = - delta.y * dashThickness;
  //       dxb = - dxa * 0.5;
  //       dxa += dxb;


		//}
		//else
		//{
		//	dYAOffset = ( delta.x * dashThickness ) / hyp;
		//	dYBOffset = -dYAOffset * 0.5;
		//	dYAOffset += dYBOffset;
  //       dya = delta.x * dashThickness / distance;
  //       dyb = - dya * 0.5;
  //       dya += dyb;

		//	dXAOffset = -( delta.y * dashThickness ) / hyp;
		//	dXBOffset = -dXAOffset * 0.5;
		//	dXAOffset += dXBOffset;
  //       dxa = - delta.y * dashThickness / distance;
  //       dxb = - dxa * 0.5;
  //       dxa += dxb;
		//}
/*		offsetThickness = 0;
		_HIWORD(offsetThickness) = dashThickness;

		if ( !LOWORD(mapXStep) )
		{
			tempFixed = FixMul (mapXStep,offsetThickness);
			dYAOffset = _HIWORD(tempFixed);
			dYBOffset = -(dYAOffset >> 1);
			dYAOffset += dYBOffset;
			if ( dX == 0L )
			{
				dXAOffset = _HIWORD(offsetThickness) >> 1;
				dXBOffset = -dXAOffset;
			}
			else
			{
				dXAOffset = calOffset (-dYOverdX,dYAOffset);
				dXBOffset = calOffset (-dYOverdX,dYBOffset);
			}
		}
		else
		{
			tempFixed = FixMul (-mapYStep,offsetThickness);
			dXAOffset = _HIWORD(tempFixed);
			dXBOffset = -(dXAOffset >> 1);
			dXAOffset += dXBOffset;
			dYAOffset = calOffset (-dXOverdY,dXAOffset);
			dYBOffset = calOffset (-dXOverdY,dXBOffset);
		}
*/
		//if ( *( dashCodes + 1 ) == (short)0xFFFF ) // this handles dashed line with only one dash
		//{
		////	pointA.x = mapX + dXAOffset;
		////	pointA.y = mapY + dYAOffset;
		////	pointB.x = mapX + dXBOffset;
		////	pointB.y = mapY + dYBOffset;

		////	pointD.x = mapX + dX + dXAOffset;
		////	pointD.y = mapY + dY + dYAOffset;
		////	pointC.x = mapX + dX + dXBOffset;
		////	pointC.y = mapY + dY + dYBOffset;

		////	//if ( (dashThickness % 2) && (pointA.y == pointB.y) && (pointC.y == pointD.y) &&
		////	//	  (pointA.x != pointB.x) && (pointC.x != pointD.x) )
		////	//{
		////	//	pointA.x++;
		////	//	pointD.x++;
		////	//}



		////	pointd pt[4];
		////	pt[0] = pointA;
		////	pt[1] = pointB;
		////	pt[2] = pointC;
		////	pt[3] = pointD;
		////	//CRgn	poly;
		////	//poly.CreatePolygonRgn(pt,4,ALTERNATE);
  ////       pDC->m_ppath = new Gdiplus::GraphicsPath();
  ////       pointf pfa[4];
  ////       __copy(pfa, pt, 4);
  ////       pDC->m_ppath->AddPolygon(pfa, 4);
		////	//pDC->PaintPenRgn(&poly);
  ////       pDC->PaintPenPath();

		//}
  //    else
      {



         pDC->graphics_on();
         pDC->graphics_high();


         //pDC->graphics()->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);


         double x = startPt.x;
         double y = startPt.y;
         double dBeg = -1.0;
         double dEnd = -1.0;

         bool bP1Set = false;

         {

            while (true)
            {

               bool bWasCurrentDashFilled = isCurrentDashFilled();

               while (m_dDistanceWeNeedToCover <= 0.)
               {

                  m_DashesIdx++;

                  auto dashCode = dashCodes[m_DashesIdx];

                  if (dashCode == -1)
                  {

                     m_dDistanceWeNeedToCover = 0.;

                     m_DashesIdx = -1;

                  }
                  else
                  {

                     m_dDistanceWeNeedToCover = pMapper->NoZoomToZoom((double)dashCode);

                  }


               }

               bool bDashOnRamp = (!bWasCurrentDashFilled && isCurrentDashFilled());

               if ((isCurrentDashFilled() &&
                  (!m_helperPoly.m_bP1Set ||
                     !bP1Set)) || bDashOnRamp)
               {

                  pA.x = x - deltaX / 2.0;
                  pA.y = y - deltaY / 2.0;
                  pB.x = x + deltaX / 2.0;
                  pB.y = y + deltaY / 2.0;
                  dBeg = countdown;


                  //if (dashThickness > 1 && pDC->m_bUsePolyHelperIfDashed)
                  if (m_bUsePolyHelperIfDashed)
                  {

                     m_helperPoly.setFrameIt(dashThickness < 2);
                     m_helperPoly.PushPoints(pA, pB, pDC);

                  }

                  m_helperPoly.m_bP1Set = true;
                  bP1Set = true;

               }

               if (bWasCurrentDashFilled && !isCurrentDashFilled() || (countdown <= 0. && isCurrentDashFilled()))
               {
                  
                  if (countdown <= 0)
                  {

                     OutputDebugString("");

                  }

                  dEnd = countdown;

                  if (dEnd < 0)
                  {

                     pD.x = endPt.x - deltaX / 2.0;
                     pD.y = endPt.y - deltaY / 2.0;
                     pC.x = endPt.x + deltaX / 2.0;
                     pC.y = endPt.y + deltaY / 2.0;

                  }
                  else
                  {

                     pD.x = x - deltaX / 2.0;
                     pD.y = y - deltaY / 2.0;
                     pC.x = x + deltaX / 2.0;
                     pC.y = y + deltaY / 2.0;

                  }

                  if (pA.x == 0.0 || pA.y == 0. || pointA.x == 0. || pointA.y == 0.)
                  {

                     OutputDebugString("test");

                  }

//                  if (bFirstPointSet)
                  if (pDC->m_DrawPrim.m_bUsePolyHelperIfDashed)
                  {


                     m_helperPoly.PushPoints(pD, pC, pDC);

                     /*              m_iDebug++;

                                   if (m_iDebug == 2)
                                   {

                                      pDC->m_PenBrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color::Red));

                                   }*/

                     if (countdown > 0)
                     {

                        if (dashThickness <= 1)
                        {

                           m_helperPoly.DrawLineNow(pDC);
                           m_helperPoly.Reset();

                        }
                        else
                        {

                           m_helperPoly.DrawNow(pDC);
                           m_helperPoly.Reset();

                        }

                     }

                  }
                  else
                  {

                     //if (pDC->m_PenPlus)
                     //if (dBeg > 0)
                     if (dashThickness <= 1)
                     {
                        if (bP1Set)
                        {
                           ///*--------------------------------------------------------------
                           //		Execute line width = 1 with graphic context solid lines
                           //--------------------------------------------------------------*/
                           //

                           //Gdiplus::GraphicsPath path;
                           pointf p1 = (pA + pB) / 2.0;
                           pointf p2 = (pC + pD) / 2.0;

                           pDC->graphics()->DrawLine(pDC->m_PenPlus.get(), p1, p2);

                           //pDC->PaintPen();

                        }

                     }
                     else
                     {

                       
                           //Gdiplus::GraphicsPath path;
                           pointf pfa[4];
                           __copy(&pfa[0], &pA, 1);
                           __copy(&pfa[1], &pB, 1);
                           __copy(&pfa[2], &pC, 1);
                           __copy(&pfa[3], &pD, 1);

                           pDC->path_new(false, Gdiplus::FillModeWinding);

                           pDC->path()->AddPolygon(pfa, 4);

                           pDC->PaintPenPath();
                        }

//                     }

                  }

               }

               if (countdown <= 0.)
               {

                  //m_PixelCount += ceil(abs(countdown));

                  break;

               }

//               paintNow = 0;

               x += dX;

               y += dY;

               m_dDistanceWeNeedToCover -= vInc;

               countdown -= vInc;

            }


         }

      }

   }

   //m_CurrentPt = endPt;

   //m_PixelCount++;

///*--------------------------------------------------------------
//		Execute line width = 1 with bit map constructs.
//--------------------------------------------------------------*/
//
//	else  {
//
///*--------------------------------------------------------------
//		Get dashes bit image from heap and set up the
//		source and destination bit maps.
//--------------------------------------------------------------*/
//
//		BYTE	bits[kBitMapSize];
//		memset(&bits,0,kBitMapSize);
//		srcBitMap.CreateBitmap(kCopyMapByteWidth * 8,kCopyMapByteWidth * 8, 1 ,1, &bits);
//		
//		CDC	tempDC;
//		tempDC.CreateCompatibleDC(pDC->GetCDC());
//
//		//		bitMapPtr = *shBitMap;
//		bitMapPtr = bits;
//		
//
//		CRect theOrigRect;
//
//		srcRect.top = srcRect.left = 0;
//		srcRect.right = srcRect.bottom = kCopyMapByteWidth * 8;
//
//		theOrigRect = srcRect;
//
///*--------------------------------------------------------------
//		Setup up srcRect for line seqment plot.
//--------------------------------------------------------------*/
//
//		begMapX = srcRect.left + 5;
//		endMapXPtr = (short*)&srcRect.right;
//		initEndMapX = 6;
//		if ( dX < 0 )  {
//			begMapX = srcRect.right - 6;
//			endMapXPtr = (short*)&srcRect.left;
//			initEndMapX = -5;
//		}
//
//		begMapY = srcRect.top + 5;
//		endMapYPtr = (short*)&srcRect.bottom;
//		initEndMapY = 6;
//		if ( dY < 0 )  {
//			begMapY = srcRect.bottom - 6;
//			endMapYPtr = (short*)&srcRect.top;
//			initEndMapY = -5;
//		}
//
//
///*--------------------------------------------------------------
//		Setup full line and line seqment parameters.
//--------------------------------------------------------------*/
//
//		while ( countdown > 0 )  {
//			dstRect = theOrigRect;
//			dstRect.OffsetRect ( begLineX-5,begLineY-5);
//
//			_HIWORD(mapX) = begMapX;
//			_HIWORD(mapY) = begMapY;
//
//			*endMapXPtr = initEndMapX;
//			*endMapYPtr = initEndMapY;
//
//			segcountdown = (kCopyMapByteWidth * 8) - 10;
//			if ( segcountdown < countdown )
//				countdown += 1 - segcountdown;
//			else  {
//				segcountdown = countdown;
//				countdown = 0;
//			}
//
//
///*--------------------------------------------------------------
//		Plot line segment with line width = 1 in bit map
//--------------------------------------------------------------*/
//
//			while ( segcountdown-- > 0 )  {
//
//
//				while (m_PixelCount == 0)  {
//					if ( (m_PixelCount = *(dashCodes + m_DashesIdx++)) == (short)0xFFFF )
//						m_DashesIdx = m_PixelCount = 0;
//				}
//				if ( isCurrentDashFilled())  {
//					mapYPtr = bitMapPtr + (_HIWORD(mapY) << kCopyMapByteShift)
//								+ (_HIWORD(mapX) >> 3);
//					*mapYPtr |= (0x80 >> (_HIWORD(mapX) & 7));
//				}
//
//				if ( segcountdown > 0 ) {
//					mapX += mapXStep;
//					mapY += mapYStep;
//					--m_PixelCount;
//				}
//			}
//
///*--------------------------------------------------------------
//		Setup srcRect and dstRect for CopyBits
//--------------------------------------------------------------*/
//
//			*endMapXPtr += _HIWORD(mapX);
//			*endMapYPtr += _HIWORD(mapY);
//
//			if ( dX < 0 )  {
//				dstRect.left -= (srcRect.right-srcRect.left-11);
//			}
//			dstRect.right = dstRect.left + (srcRect.right-srcRect.left);
//
//			if ( dY < 0 )  {
//				dstRect.top -= (srcRect.bottom-srcRect.top-11);
//			}
//			dstRect.bottom = dstRect.top + (srcRect.bottom-srcRect.top);
//
//			begLineX += (_HIWORD(mapX) - begMapX);
//			begLineY += (_HIWORD(mapY) - begMapY);
//
//			srcBitMap.SetBitmapBits(kBitMapSize, &bits);
//			CBitmap* pOldBmp = tempDC.SelectObject(&srcBitMap);
//			pDC->GetCDC()->BitBlt( dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), &tempDC, srcRect.left, srcRect.top, 0x00E20746);
//			tempDC.SelectObject(pOldBmp);
//
//
///*--------------------------------------------------------------
//		Clear dashes bit map before next line segument.
//--------------------------------------------------------------*/
//
//			memset(&bits,0,kBitMapSize);
//		}
//
//	}							/*	End of bit map constructs	*/
}


/*==============================================================

		initDashCodes ( dashCode, halfPenWidth )

		Used by both DashLine and DashLineTo to test for a dash
		line continuation.  If the new dash line start point is
		different than the current pen position or if it is a
		different dash line style, the dash code parameters are
		initialized.

==============================================================*/

void CDrawPrim::initDashCodes(const pointd& inStartPt, short dashCode/*, short halfPenWidth*/)
{
	bool			restart;

	restart = false;
	if ( (int)(inStartPt.x *10.0) != (int) (m_CurrentPt.x * 10.0) ||
      (int) (inStartPt.y * 10.) != (int) (m_CurrentPt.y * 10.0) )  {
		m_CurrentPt = inStartPt;
		restart = true;
	}

   if (dashCode != m_DLSelect) {
      m_DLSelect = dashCode;
      restart = true;
   }

	if ( restart )
	{
		
      m_dDistanceWeNeedToCover = 0.;

      m_DashesIdx = -1;

	}
}

//void CDrawPrim::initDashCodes(const short& inStartPt, short dashCode/*, short halfPenWidth*/)
//{
//
//}



//#define kHairLine	0
#define kLine1		1
#define kLine2		2
#define kLine3		3
#define kLine4		4
#define kLine5		5
#define kLine6		6
#define kLine7		7
#define kLine8		8
#define kLine9		9
#define kLine11		11
#define kLine12		12
#define kLine13		13
#define kLine16		16

#define kArrow				1
#define kOval				3
#define kSlash				4
#define kX					5
#define kVArrow				6

#define kSlashEnd			3

#define kClose				8
#define kOpen				4

#define kFill				2
#define kFrame				1

#define	kNEndcaps			16
#define kNPenWidths			13

CDrawPrim::EndcapRec	gEndcaps [kNEndcaps * kNPenWidths] = {
	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine1,	  kArrow,   kFrame,		5,			3,			0,			0,			0,
	kLine1,   kArrow, 	kFill,  	5,			3,			0,			0,			0,
	kLine1,   kArrow, 	kFrame, 	9,			3,			0,			2,			0,
	kLine1,   kArrow,	kFill,		9,			3,			0,			2,			0,
	kLine1,   kArrow,	kFrame,		9,			3,			0,			0,			0,
	kLine1,   kArrow,	kFill,		9,			3,			0,			0,			0,
/* New Small Dots. */
	kLine1,	  kOval,	kFrame,		2,			2,			0,			2,			0,
	kLine1,	  kOval,	kFill,		2,			2,			0,			2,			0,
	kLine1,   kOval,	kFrame,		4,			4,			0,			4,			0,
	kLine1,   kOval,	kFill,		4,			4,			0,			4,			0,
/* Small Slash */
	kLine1,   kSlash,	0,			3,			3,			0,			3,			0,
	kLine1,   kSlash,	0,			9,			9,			0,			9,			0,
	kLine1,   kX,		0,			9,			9,			0,			9,			0,
	kLine1,   kVArrow,	kClose,		9,			3,			0,			0,			0,
	kLine1,   kVArrow,	kOpen,		9,			3,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine2,	  kArrow,	kFrame,		6,			4,			0,			0,			0,
	kLine2,	  kArrow,	kFill,		6,			4,			0,			0,			0,
	kLine2,	  kArrow,   kFrame,		10,			3,			0,			2,			0,
	kLine2,	  kArrow,   kFill,		10,			3,			0,			2,			0,
	kLine2,	  kArrow,   kFrame,		10,			3,			0,			0,			0,
	kLine2,	  kArrow,   kFill,		10,			3,			0,			0,			0,
/* New Small Dots. */
	kLine2,	  kOval,	kFrame,		2,			2,			0,			2,			0,
	kLine2,	  kOval,	kFill,		2,			2,			0,			2,			0,
	kLine2,	  kOval,    kFrame,		4,			4,			0,			4,			0,
	kLine2,	  kOval,	kFill,		4,			4,			0,			4,			0,
/* Small Slash */
	kLine2,   kSlash,	0,			3,			3,			0,			3,			0,
	kLine2,   kSlash,	0,			9,			9,			0,			9,			0,
	kLine2,   kX,		0,			9,			9,			0,			9,			0,
	kLine2,	  kVArrow,  kClose,		10,			3,			0,			0,			0,
	kLine2,	  kVArrow,  kOpen,		10,			3,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine3,	  kArrow,	kFrame,		7,			4,			0,			0,			0,
	kLine3,	  kArrow,	kFill,		7,			4,			0,			0,			0,
	kLine3,	  kArrow,   kFrame,		13,			4,			0,			3,			0,
	kLine3,	  kArrow,   kFill,		13,			4,			0,			3,			0,
	kLine3,	  kArrow,   kFrame,		13,			4,			0,			0,			0,
	kLine3,	  kArrow,   kFill,		13,			4,			0,			0,			0,
/* New Small Dots. */
	kLine3,	  kOval,	kFrame,		2,			2,			0,			2,			0,
	kLine3,	  kOval,	kFill,		2,			2,			0,			2,			0,
	kLine3,	  kOval,    kFrame,		5,			5,			0,			5,			0,
	kLine3,	  kOval,	kFill,		5,			5,			0,			5,			0,
/* Small Slash */
	kLine3,   kSlash,	0,			3,			3,			0,			3,			0,
	kLine3,   kSlash,	0,			9,			9,			0,			9,			0,
	kLine3,   kX,		0,			9,			9,			0,			9,			0,
	kLine3,	  kVArrow,  kClose,		13,			4,			0,			0,			0,
	kLine3,	  kVArrow,  kOpen,		13,			4,			0,			0,			0,

	0, 		  0,		0,			0,			0,			0,			0,			0,
	kLine4,	  kArrow,	kFrame,		9,			6,			0,			0,			0,
	kLine4,	  kArrow,	kFill,		9,			6,			0,			0,			0,
	kLine4,	  kArrow,	kFrame,		18,			5,			0,			3,			0,
	kLine4,	  kArrow,	kFill,		18,			5,			0,			3,			0,
	kLine4,	  kArrow,	kFrame,		18,			5,			0,			0,			0,
	kLine4,	  kArrow,	kFill,		18,			5,			0,			0,			0,
/* Small Dots. */
	kLine4,	  kOval,	kFrame,		3,			3,			0,			3,			0,
	kLine4,	  kOval,	kFill,		3,			3,			0,			3,			0,
	kLine4,	  kOval,	kFrame,		6,			6,			0,			6,			0,
	kLine4,	  kOval,	kFill,		6,			6,			0,			6,			0,
/* Small Slash */
	kLine4,   kSlash,	0,			3,			3,			0,			3,			0,
	kLine4,   kSlash,	0,			10,			10,			0,			10,			0,
	kLine4,   kX,		0,			10,			10,			0,			10,			0,
	kLine4,	  kVArrow,	kClose,		18,			5,			0,			0,			0,
	kLine4,	  kVArrow,	kOpen,		18,			5,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine5,	  kArrow,	kFrame,		11,			7,			0,			0,			0,
	kLine5,	  kArrow,	kFill,		11,			7,			0,			0,			0,
	kLine5,	  kArrow,	kFrame,		22,			6,			0,			4,			0,
	kLine5,	  kArrow,	kFill,		22,			6,			0,			4,			0,
	kLine5,	  kArrow,	kFrame,		22,			6,			0,			0,			0,
	kLine5,	  kArrow,	kFill,		22,			6,			0,			0,			0,
/* Small Dots. */
	kLine5,	  kOval,	kFrame,		4,			4,			0,			4,			0,
	kLine5,	  kOval,	kFill,		4,			4,			0,			4,			0,
	kLine5,	  kOval,	kFrame,		8,			8,			0,			8,			0,
	kLine5,	  kOval,	kFill,		8,			8,			0,			8,			0,
/* Small Slash */
	kLine5,   kSlash,	0,			4,			4,			0,			4,			0,
	kLine5,   kSlash,	0,			11,			11,			0,			11,			0,
	kLine5,   kX,		0,			11,			11,			0,			11,			0,
	kLine5,	  kVArrow,	kClose,		22,			6,			0,			0,			0,
	kLine5,	  kVArrow,	kOpen,		22,			6,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine6,	  kArrow,	kFrame,		13,			8,			0,			0,			0,
	kLine6,	  kArrow,	kFill,		13,			8,			0,			0,			0,
	kLine6,	  kArrow,	kFrame,		27,			7,			0,			4,			0,
	kLine6,	  kArrow,	kFill,		27,			7,			0,			4,			0,
	kLine6,	  kArrow,	kFrame,		27,			7,			0,			0,			0,
	kLine6,	  kArrow,	kFill,		27,			7,			0,			0,			0,
/* Small Dots. */
	kLine6,	  kOval,	kFrame,		4,			4,			0,			4,			0,
	kLine6,	  kOval,	kFill,		4,			4,			0,			4,			0,
	kLine6,	  kOval,	kFrame,		9,			9,			0,			9,			0,
	kLine6,	  kOval,	kFill,		9,			9,			0,			9,			0,
/* Small Slash */
	kLine6,   kSlash,	0,			4,			4,			0,			4,			0,
	kLine6,   kSlash,	0,			12,			12,			0,			12,			0,
	kLine6,   kX,		0,			12,			12,			0,			12,			0,
	kLine6,	  kVArrow,	kClose,		27,			7,			0,			0,			0,
	kLine6,	  kVArrow,	kOpen,		27,			7,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine7,	  kArrow,	kFrame,		15,			10,			0,			0,			0,
	kLine7,	  kArrow,	kFill,		15,			10,			0,			0,			0,
	kLine7,	  kArrow,	kFrame,		31,			8,			0,			5,			0,
	kLine7,	  kArrow,	kFill,		31,			8,			0,			5,			0,
	kLine7,	  kArrow,	kFrame,		31,			8,			0,			0,			0,
	kLine7,	  kArrow,	kFill,		31,			8,			0,			0,			0,
/* Small Dots. */
	kLine7,	  kOval,	kFrame,		5,			5,			0,			5,			0,
	kLine7,	  kOval,	kFill,		5,			5,			0,			5,			0,
	kLine7,	  kOval,	kFrame,		10,			10,			0,			10,			0,
	kLine7,	  kOval,	kFill,		10,			10,			0,			10,			0,
/* Small Slash */
	kLine7,   kSlash,	0,			7,			7,			0,			7,			0,
	kLine7,   kSlash,	0,			13,			13,			0,			13,			0,
	kLine7,   kX,		0,			13,			13,			0,			13,			0,
	kLine7,	  kVArrow,	kClose,		31,			8,			0,			0,			0,
	kLine7,	  kVArrow,	kOpen,		31,			8,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine8,	  kArrow,	kFrame,		17,			11,			0,			0,			0,
	kLine8,	  kArrow,	kFill,		17,			11,			0,			0,			0,
	kLine8,	  kArrow,	kFrame,		36,			9,			0,			6,			0,
	kLine8,	  kArrow,	kFill,		36,			9,			0,			6,			0,
	kLine8,	  kArrow,	kFrame,		36,			9,			0,			0,			0,
	kLine8,	  kArrow,	kFill,		36,			9,			0,			0,			0,
/* Small Dots. */
	kLine8,	  kOval,	kFrame,		5,			5,			0,			5,			0,
	kLine8,	  kOval,	kFill,		5,			5,			0,			5,			0,
	kLine8,	  kOval,	kFrame,		11,			11,			0,			11,			0,
	kLine8,	  kOval,	kFill,		11,			11,			0,			11,			0,
/* Small Slash */
	kLine8,   kSlash,	0,			5,			5,			0,			5,			0,
	kLine8,	  kSlash,	0,			14,			14,			0,			14,			0,
	kLine8,	  kX,		0,			14,			14,			0,			14,			0,
	kLine8,	  kVArrow,	kClose,		36,			9,			0,			0,			0,
	kLine8,	  kVArrow,	kOpen,		36,			9,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine9,	  kArrow,	kFrame,		19,			12,			0,			0,			0,
	kLine9,	  kArrow,	kFill,		19,			12,			0,			0,			0,
	kLine9,	  kArrow,	kFrame,		40,			11,			0,			7,			0,
	kLine9,	  kArrow,	kFill,		40,			11,			0,			7,			0,
	kLine9,	  kArrow,	kFrame,		40,			11,			0,			0,			0,
	kLine9,	  kArrow,	kFill,		40,			11,			0,			0,			0,
/* Small Dots. */
	kLine9,	  kOval,	kFrame,		6,			6,			0,			6,			0,
	kLine9,	  kOval,	kFill,		6,			6,			0,			6,			0,
	kLine9,	  kOval,	kFrame,		12,			12,			0,			12,			0,
	kLine9,	  kOval,	kFill,		12,			12,			0,			12,			0,
/* Small Slash */
	kLine9,   kSlash,	0,			9,			9,			0,			9,			0,
	kLine9,	  kSlash,	0,			16,			16,			0,			16,			0,
	kLine9,	  kX,		0,			16,			16,			0,			16,			0,
	kLine9,	  kVArrow,	kClose,		40,			11,			0,			0,			0,
	kLine9,	  kVArrow,	kOpen,		40,			11,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine11,  kArrow,	kFrame,		24,			15,			0,			0,			0,
	kLine11,  kArrow,	kFill,		24,			15,			0,			0,			0,
	kLine11,  kArrow,	kFrame,		49,			13,			0,			9,			0,
	kLine11,  kArrow,	kFill,		49,			13,			0,			9,			0,
	kLine11,  kArrow,	kFrame,		49,			13,			0,			0,			0,
	kLine11,  kArrow,	kFill,		49,			13,			0,			0,			0,
/* Small Dots. */
	kLine11,	kOval,	kFrame,		7,			7,			0,			7,			0,
	kLine11,	kOval,	kFill,		7,			7,			0,			7,			0,
	kLine11,  kOval,	kFrame,		15,			15,			0,			15,			0,
	kLine11,  kOval,	kFill,		15,			15,			0,			15,			0,
/* Small Slash */
	kLine11,   kSlash,	0,			11,			11,			0,			11,			0,
	kLine11,  kSlash,	0,			18,			18,			0,			18,			0,
	kLine11,  kX,		0,			18,			18,			0,			18,			0,
	kLine11,  kVArrow,	kClose,		49,			13,			0,			0,			0,
	kLine11,  kVArrow,	kOpen,		49,			13,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine12,  kArrow,	kFrame,		26,			17,			0,			0,			0,
	kLine12,  kArrow,	kFill,		26,			17,			0,			0,			0,
	kLine12,  kArrow,	kFrame,		53,			14,			0,			9,			0,
	kLine12,  kArrow,	kFill,		53,			14,			0,			9,			0,
	kLine12,  kArrow,	kFrame,		53,			14,			0,			0,			0,
	kLine12,  kArrow,	kFill,		53,			14,			0,			0,			0,
/* Small Dots. */
	kLine12,	kOval,	kFrame,		8,			8,			0,			8,			0,
	kLine12,	kOval,	kFill,		8,			8,			0,			8,			0,
	kLine12,  kOval,	kFrame,		16,			16,			0,			16,			0,
	kLine12,  kOval,	kFill,		16,			16,			0,			16,			0,
/* Small Slash */
	kLine12,   kSlash,	0,			6,			6,			0,			6,			0,
	kLine12,  kSlash,	0,			19,			19,			0,			19,			0,
	kLine12,  kX,		0,			19,			19,			0,			19,			0,
	kLine12,  kVArrow,	kClose,		53,			14,			0,			0,			0,
	kLine12,  kVArrow,	kOpen,		53,			14,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine13,  kArrow,	kFrame,		27,			18,			0,			0,			0,
	kLine13,  kArrow,	kFill,		27,			18,			0,			0,			0,
	kLine13,  kArrow,	kFrame,		58,			15,			0,			10,			0,
	kLine13,  kArrow,	kFill,		58,			15,			0,			10,			0,
	kLine13,  kArrow,	kFrame,		58,			15,			0,			0,			0,
	kLine13,  kArrow,	kFill,		58,			15,			0,			0,			0,
/* Small Dots. */
	kLine13,	kOval,	kFrame,		8,			8,			0,			8,			0,
	kLine13,	kOval,	kFill,		8,			8,			0,			8,			0,
	kLine13,  kOval,	kFrame,		17,			17,			0,			17,			0,
	kLine13,  kOval,	kFill,		17,			17,			0,			17,			0,
/* Small Slash */
	kLine13,   kSlash,	0,			13,			13,			0,			13,			0,
	kLine13,  kSlash,	0,			20,			20,			0,			20,			0,
	kLine13,  kX,		0,			20,			20,			0,			20,			0,
	kLine13,  kVArrow,	kClose,		58,			15,			0,			0,			0,
	kLine13,  kVArrow,	kOpen,		58,			15,			0,			0,			0,

	0,		  0,		0,			0,			0,			0,			0,			0,
	kLine16,  kArrow,	kFrame,		34,			23,			0,			0,			0,
	kLine16,  kArrow,	kFill,		34,			23,			0,			0,			0,
	kLine16,  kArrow,	kFrame,		71,			18,			0,			12,			0,
	kLine16,  kArrow,	kFill,		71,			18,			0,			12,			0,
	kLine16,  kArrow,	kFrame,		71,			18,			0,			0,			0,
	kLine16,  kArrow,	kFill,		71,			18,			0,			0,			0,
/* Small Dots. */
	kLine16,	kOval,	kFrame,		10,			10,			0,			10,			0,
	kLine16,	kOval,	kFill,		10,			10,			0,			10,			0,
	kLine16,  kOval,	kFrame,		21,			21,			0,			21,			0,
	kLine16,  kOval,	kFill,		21,			21,			0,			21,			0,
/* Small Slash */
	kLine16,   kSlash,	0,			8,			8,			0,			8,			0,
	kLine16,  kSlash,	0,			25,			25,			0,			25,			0,
	kLine16,  kX,		0,			25,			25,			0,			25,			0,
	kLine16,  kVArrow,	kClose,		71,			18,			0,			0,			0,
	kLine16,  kVArrow,	kOpen,		71,			18,			0,			0,			0,
};

/*----------------------------- 	Private prototypes	-----------------*/

//void	drawOutsideDimLine		(LongPointPtr, LongPointPtr, PenModelPtr, short, short, Coord);

void	arrowEM		( CDraftContext* pDC, LongPointPtr, LongPointPtr, CDrawPrim::EndcapRec*, ushort, short, bool, PenModel*, Coord, int thinPenSize);
void	dotEM		( CDraftContext* pDC, LongPointPtr, LongPointPtr, CDrawPrim::EndcapRec*, ushort, PenModel*, Coord);
void	slashEM(CDraftContext* pDC,LongPointPtr,LongPointPtr,CDrawPrim::EndcapRec*,ushort,PenModel*,short,Coord);
void	xEM(CDraftContext* pDC,LongPointPtr,LongPointPtr,CDrawPrim::EndcapRec*,ushort,PenModel*,short,Coord);
void	vArrowEM(CDraftContext* pDC,LongPointPtr,LongPointPtr,CDrawPrim::EndcapRec*,ushort,PenModel*,short,Coord);

/*-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
	getEndcapIndex -	given endcap number from PenModel, return index into
						gEndcaps.  (pmo)
-------------------------------------------------------------------------*/

short	getEndcapIndex	(
	short	endcap,
	short	penWidth)
{
	short	index;

	/* more flexable method */
	for (index = 0; index < kNPenWidths; index++)
		if ((penWidth & 0x00FF) == gEndcaps[(index * kNEndcaps) + endcap].penWidth)
			return((index * kNEndcaps) + endcap);

	return(endcap);
}	/* getEndcapIndex */

/*------------------------------------------------------------------------
	drawEndcap -	print an endcap at a point on shape at orientation specified
					by two points.
-------------------------------------------------------------------------*/

void CDrawPrim::drawEndcap(CDraftContext* pDC, const LongPoint& inlPt1, const LongPoint& inlPt2, short endcap, unsigned short map, const PenModel& penModel, bool isCurve, bool isGrayed, Coord outsideLineLen)
{

	if(pDC->m_PenModel.m_nPenType==kPenInvis) return;
	
	LongPoint		lPt1, lPt2;
	short			zPenW;
	CDrawPrim::EndcapRec*		endcapPtr;

	CDC* pCDC=pDC->GetCDC();
	CMapper* pMapper=pDC->GetMapper();

	lPt1 = inlPt1;
	lPt2 = inlPt2;

	int thinPenSize=pMapper->ScreenPenW(1);
	if(!thinPenSize) thinPenSize=1;

	zPenW   = pMapper->ScreenPenW(penModel.m_nWidth);

	endcapPtr = &gEndcaps[getEndcapIndex(endcap, penModel.m_nWidth)];

	switch (endcapPtr->type)
		{
		case kArrow:
		case kVArrow:
			arrowEM (pDC, &lPt1, &lPt2, endcapPtr, map, zPenW, isCurve, (PenModel*)&penModel, outsideLineLen, thinPenSize);
			break;

		case kOval:
			dotEM (pDC, &lPt1, &lPt2, endcapPtr, map, (PenModel*)&penModel, outsideLineLen);
			break;

		case kX:
			xEM (pDC, &lPt1, &lPt2, endcapPtr, map, (PenModel*)&penModel, zPenW, outsideLineLen);
			break;

		case kSlash:
			slashEM (pDC, &lPt1, &lPt2, endcapPtr, map, (PenModel*)&penModel, zPenW, outsideLineLen);
			break;
		}

}	/* drawEndcap */

/*----------------------------------------------------------------------
	dotEM -		draw the dot endmark at point1, and point2
-----------------------------------------------------------------------*/

void	dotEM (
	CDraftContext* pDC,
	LongPointPtr			lPt1,
	LongPointPtr			lPt2,
	register CDrawPrim::EndcapRec*	endcapPtr,
	ushort					map,
	PenModel*				penModel,
	Coord					outsideLineLen)
{
	register short 			markW, markH;
	short 					fill;
	CRect 					r1,r2;
	pointd					pt1, pt2;

	CDC* pCDC=pDC->GetCDC();
	CMapper* pMapper=pDC->GetMapper();

	pMapper->LPtoDP(pt1, *lPt1);
	pMapper->LPtoDP(pt2, *lPt2);

	markH = (short)pMapper->LPtoDP(endcapPtr->width);
	markW = markH;

	r1.SetRect (pt1.x - markW, pt1.y - markH, pt1.x + markW +1 , pt1.y + markH +1);
	r2.SetRect (pt2.x - markW, pt2.y - markH, pt2.x + markW+1, pt2.y + markH+1);

	fill = endcapPtr->opcode;

	int nWidth = pMapper->ScreenPenW(1);
	if (!nWidth) nWidth = 1;

   //if(pDC->m_bTransparentBackground)
   {
      // Infinisys Ltd.
      ::std::unique_ptr<Gdiplus::Pen> penStore;
      ::std::unique_ptr<Gdiplus::Brush> brushStore;
      Gdiplus::Pen * thePen = nullptr;
      Gdiplus::Brush * brush = nullptr;
      pDC->graphics_on();
      if(penModel->m_nPenType == kPenColor)
      {
         COLORREF cr = penModel->m_crColor;
         if(cr & PALETTEINDEX(0))
         {
            PALETTEENTRY pe;
            pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
            cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
         }
         thePen = pDC->m_PenPlus.get();
         if(fill & kFill) 
            brush = pDC->m_PenBrushPlus.get();
         else
         {
            brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255)));
            brush = brushStore.get();
         }
      }
      else
      {
         penStore.reset(new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), Gdiplus::REAL(nWidth)));
         thePen = penStore.get();
         if(fill & kFill)
            brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,0,0,0)));
         else
         {
            brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255)));
            brush = brushStore.get();
         }

      }

      Gdiplus::Rect r;

      if(map & kEnd1)
      {
         copy(r,r1);
         if(brush)
            pDC->graphics()->FillEllipse(brush, r);
         pDC->graphics()->DrawEllipse(thePen, r);
      }

      if(map & kEnd2)
      {
         copy(r,r2);
         if(brush)
            pDC->graphics()->FillEllipse(brush,r);
         pDC->graphics()->DrawEllipse(thePen,r);
      }

   }
   //else
   //{
   //   CPen thePen;
   //   CBrush	brush;
   //   if(penModel->m_nPenType == kPenColor)
   //   {
   //      thePen.CreatePen(PS_SOLID,nWidth,penModel->m_crColor);
   //      if(!(fill & kFill)) brush.CreateStockObject(WHITE_BRUSH);
   //   }
   //   else
   //   {
   //      thePen.CreatePen(PS_SOLID,nWidth,RGB(0,0,0));
   //      brush.CreateStockObject((fill & kFill)?BLACK_BRUSH:WHITE_BRUSH);
   //   }
   //   CGDIObject<CPen> gdiPen(pCDC,&thePen);
   //   CBrush* pOldBrush;
   //   if(brush.GetSafeHandle())
   //   {
   //      pOldBrush = pCDC->SelectObject(&brush);
   //   }

   //   if(map & kEnd1)
   //   {
   //      pCDC->Ellipse(r1.left,r1.top,r1.right,r1.bottom);
   //   }

   //   if(map & kEnd2)
   //   {
   //      pCDC->Ellipse(r2.left,r2.top,r2.right,r2.bottom);
   //   }

   //   if(brush.GetSafeHandle())
   //   {
   //      ASSERT(pOldBrush);
   //      pCDC->SelectObject(pOldBrush);
   //   }
   //}
}	/* dotEM */

/*----------------------------------------------------------------------
	slashEM -		draw the slash endmark at p1 and p2
-----------------------------------------------------------------------*/

void slashEM(
   CDraftContext* pDC, // Infinisys Ltd.
			 LongPointPtr	lPt1,
			LongPointPtr	lPt2,
			CDrawPrim::EndcapRec*		endcapPtr,
			ushort			map,
			PenModel*		penModel,
			short			penWidth,
			Coord			outsideLineLen)
{
	pointd			pt1, pt2;
	short			halfPenW;
	Angle			angRad;
	WrFixed			angCos, angSin;
	Coord			fixHalfPen, fixMarkLen;
	register long	lenCos, lenSin;

   CDC* pCDC=pDC->GetCDC();
   CMapper* pMapper=pDC->GetMapper();

		fixMarkLen = Coord(short(pMapper->LPtoDP(Coord(endcapPtr->width))));
		fixHalfPen = Coord(penWidth) >> 1;
		halfPenW = penWidth >> 1;
//		PenSize(penWidth, penWidth);

		/* do outside Endcaps - reverse dot orientation */
//		if (map & kDimEndmarkOut)
//			drawOutsideDimLine(lPt1, lPt2, penModel, map, penWidth, outsideLineLen);

		pMapper->LPtoDP (pt1, *lPt1 ) ;
		pMapper->LPtoDP (pt2, *lPt2 ) ;

#ifdef SUB_HALF_PEN
		pt1.x -= halfPenW;
		pt1.y -= halfPenW;
		pt2.x -= halfPenW;
		pt2.y -= halfPenW;
#endif

		angRad = ATan2(pt2.x - pt1.x, pt2.y - pt1.y) ;
//		angRad = WrFixedFromFixed ( FixATan2((long)(pt1.h - pt2.h), (long)(pt1.v - pt2.v)) ) ;
		angRad += kFixedPie4;
		angCos = Cos ( angRad ) ;
		angSin = Sin ( angRad ) ;

		lenCos = ( fixMarkLen * angCos ).GetLong () ;
		lenSin = ( fixMarkLen * angSin ).GetLong () ;

      //if(pDC->m_bTransparentBackground)
      {
         // Infinisys Ltd.
         ::std::unique_ptr<Gdiplus::Pen> penStore;
         ::std::unique_ptr<Gdiplus::Brush> brushStore;
         Gdiplus::Pen * thePen = nullptr;
         Gdiplus::Brush * brush = nullptr;
         pDC->graphics_on();
         if(penModel->m_nPenType == kPenColor)
         {
            COLORREF cr = penModel->m_crColor;
            if(cr & PALETTEINDEX(0))
            {
               PALETTEENTRY pe;
               pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
               cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
            }
            //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,GetRValue(cr),GetGValue(cr),GetBValue(cr)),Gdiplus::REAL(penWidth)));
            thePen = pDC->m_PenPlus.get();
         }
         else
         {
            //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,0,0,0),Gdiplus::REAL(penWidth)));
            thePen = pDC->m_PenPlus.get();
         }

         if(map & kEnd1)
         {
            pDC->graphics()->DrawLine(thePen, (FLOAT)(pt1.x - lenSin),(FLOAT) (pt1.y + lenCos),(FLOAT) (pt1.x + lenSin),(FLOAT) (pt1.y - lenCos));
         }

         if(map & kEnd2)
         {
            pDC->graphics()->DrawLine(thePen,(FLOAT) (pt2.x - lenSin),(FLOAT) (pt2.y + lenCos), (FLOAT) (pt2.x + lenSin), (FLOAT) (pt2.y - lenCos));
         }

      }
      /*else
      {
         CPen thePen;
         thePen.CreatePen(PS_SOLID,penWidth,(penModel->m_nPenType == kPenColor)?penModel->m_crColor:RGB(0,0,0));
         CGDIObject<CPen> gdiPen(pCDC,&thePen);

         if(map & kEnd1)
         {
            pCDC->MoveTo(pt1.x - lenSin,pt1.y + lenCos);
            pCDC->LineTo(pt1.x + lenSin,pt1.y - lenCos);
         }

         if(map & kEnd2)
         {
            pCDC->MoveTo(pt2.x - lenSin,pt2.y + lenCos);
            pCDC->LineTo(pt2.x + lenSin,pt2.y - lenCos);
         }

      }
*/
//		if (gSavePSNow)
//			{
//			MD_PicComment(kPostScriptEnd,0,0L);
//			hybridBegin();
//			psEndmark(kPS_SSlash, lPt1, lPt2, endcapPtr, map, penModel);
//			hybridEnd();
//			}
//		}
}	/* slashEM */

/*----------------------------------------------------------------------
	xEM -		draw the X endmark at p1 and p2
-----------------------------------------------------------------------*/

void	xEM (
   CDraftContext* pDC, // Infinisys Ltd.
		LongPointPtr	lPt1,
		LongPointPtr	lPt2,
		CDrawPrim::EndcapRec*	endcapPtr,
		ushort			map,
		PenModel*		penModel,
		short			penWidth,
		Coord			outsideLineLen)
{
	pointd			pt1, pt2;
	short			halfPenW;
	Angle			angRad ;
	WrFixed			angCos, angSin;
	Coord			fixHalfPen, fixMarkLen;
	register long	lenCos1, lenSin1, lenCos2, lenSin2;
   CDC* pCDC=pDC->GetCDC();
   CMapper* pMapper=pDC->GetMapper();

//	if (gOutputDevice == bDevLaser)
//		psEndmark(kPS_DSlash, lPt1, lPt2, endcapPtr, map, penModel);
//	else
//		{
//		if (gSavePSNow)
//			MD_PicComment(kPostScriptBegin, 0, 0L);

		fixMarkLen = Coord(short(pMapper->LPtoDP(endcapPtr->width)));
		fixHalfPen = Coord(penWidth) >> 1;
		halfPenW = penWidth >> 1;
//		PenSize(penWidth, penWidth);

		/* do outside Endcaps - reverse dot orientation */
//		if (map & kDimEndmarkOut)
//			drawOutsideDimLine(lPt1, lPt2, penModel, map, penWidth, outsideLineLen);

		pMapper->LPtoDP (pt1, *lPt1 ) ;
		pMapper->LPtoDP (pt2, *lPt2 ) ;

#ifdef SUB_HALF_PEN
		pt1.x -= halfPenW;
		pt1.y -= halfPenW;
		pt2.x -= halfPenW;
		pt2.y -= halfPenW;
#endif
		angRad =  ATan2((pt2.x - pt1.x), (pt2.y - pt1.y));

		angRad += kFixedPie4;
		angCos = Cos ( angRad ) ;
		angSin = Sin ( angRad ) ;

		lenCos1 = ( fixMarkLen * angCos ).GetLong () ;
		lenSin1 = ( fixMarkLen * angSin ).GetLong () ;

		angRad += kFixedPie2;
		angCos = Cos ( angRad ) ;
		angSin = Sin ( angRad ) ;

		lenCos2 = ( fixMarkLen * angCos ).GetLong () ;
		lenSin2 = ( fixMarkLen * angSin ).GetLong () ;

      //if(pDC->m_bTransparentBackground)
      {
         // Infinisys Ltd.
         ::std::unique_ptr<Gdiplus::Pen> penStore;
         ::std::unique_ptr<Gdiplus::Brush> brushStore;
         Gdiplus::Pen * thePen = nullptr;
         Gdiplus::Brush * brush = nullptr;
         pDC->graphics_on();
         //Gdiplus::Graphics g(pCDC->GetSafeHdc());
         if(penModel->m_nPenType == kPenColor)
         {
            COLORREF cr = penModel->m_crColor;
            if(cr & PALETTEINDEX(0))
            {
               PALETTEENTRY pe;
               pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
               cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
            }
            //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,GetRValue(cr),GetGValue(cr),GetBValue(cr)),Gdiplus::REAL(penWidth)));
            thePen = pDC->m_PenPlus.get();
         }
         else
         {
            //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,0,0,0),Gdiplus::REAL(penWidth)));
            thePen = pDC->m_PenPlus.get();
         }

         if(map & kEnd1)
         {
            //g.DrawLine(thePen.get(),(FLOAT)(pt1.x - lenSin1), (FLOAT)(pt1.y + lenCos1), (FLOAT)(pt1.x + lenSin1), (FLOAT)(pt1.y - lenCos1));
            //g.DrawLine(thePen.get(), (FLOAT)(pt1.x - lenSin2), (FLOAT)(pt1.y + lenCos2), (FLOAT)(pt1.x + lenSin2), (FLOAT)(pt1.y - lenCos2));
            pDC->graphics()->DrawLine(thePen,(FLOAT)(pt1.x - lenSin1), (FLOAT)(pt1.y + lenCos1), (FLOAT)(pt1.x + lenSin1), (FLOAT)(pt1.y - lenCos1));
            pDC->graphics()->DrawLine(thePen, (FLOAT)(pt1.x - lenSin2), (FLOAT)(pt1.y + lenCos2), (FLOAT)(pt1.x + lenSin2), (FLOAT)(pt1.y - lenCos2));
         }

         if(map & kEnd2)
         {
            pDC->graphics()->DrawLine(thePen, (FLOAT)(pt2.x - lenSin1), (FLOAT)(pt2.y + lenCos1), (FLOAT)(pt2.x + lenSin1), (FLOAT)(pt2.y - lenCos1));
            pDC->graphics()->DrawLine(thePen, (FLOAT)(pt2.x - lenSin2), (FLOAT)(pt2.y + lenCos2), (FLOAT)(pt2.x + lenSin2), (FLOAT)(pt2.y - lenCos2));
         }

      }
      //else
      //{
      //   CPen thePen;
      //   thePen.CreatePen(PS_SOLID,penWidth,(penModel->m_nPenType == kPenColor)?penModel->m_crColor:RGB(0,0,0));
      //   CGDIObject<CPen> gdiPen(pCDC,&thePen);

      //   if(map & kEnd1)
      //   {
      //      pCDC->MoveTo(pt1.x - lenSin1,pt1.y + lenCos1);
      //      pCDC->LineTo(pt1.x + lenSin1,pt1.y - lenCos1);
      //      pCDC->MoveTo(pt1.x - lenSin2,pt1.y + lenCos2);
      //      pCDC->LineTo(pt1.x + lenSin2,pt1.y - lenCos2);
      //   }

      //   if(map & kEnd2)
      //   {
      //      pCDC->MoveTo(pt2.x - lenSin1,pt2.y + lenCos1);
      //      pCDC->LineTo(pt2.x + lenSin1,pt2.y - lenCos1);
      //      pCDC->MoveTo(pt2.x - lenSin2,pt2.y + lenCos2);
      //      pCDC->LineTo(pt2.x + lenSin2,pt2.y - lenCos2);
      //   }

      //}

//		if ( gSavePSNow ) {
//			MD_PicComment(kPostScriptEnd, 0, 0L);
//			hybridBegin();
//			psEndmark(kPS_DSlash, lPt1, lPt2, endcapPtr, map, penModel);
//			hybridEnd();
//		}
//
}	/* xEM */


/*----------------------------------------------------------------------
	arrowEM -		Draw arrow at pt1, pt2 with specified width and height
					rotating it with the line .
						Fill if boolean fill is true

						outside		              width
					   |-------|				 |----|

					   o 21					  11 o			-
					 +							   +		|
				  +         200         100          +		|  height
			2  o-------o 20 ---o---------o--- 10 o----o 1	-
				  +									 +
					 +							   +
					   o 22					  12 o

5/1 - 		draw a more sophisticated arrow with s pointed nose and tucked in tail
------------------------------------------------------------------------------------------*/

void	arrowEM (
	CDraftContext* pDC,
	LongPointPtr			lPt1,
	LongPointPtr			lPt2,
	register CDrawPrim::EndcapRec*	endcapPtr,
	ushort					map,
	short					penWidth,		/* zoomed */
	bool					isCurve,
	PenModel*				penModel,
	Coord					outsideLineLen,
	int						thinPenSize
	)
{
	register long		hCosTheta, hSinTheta, wCosTheta, wSinTheta;
	long				noseCosTheta, noseSinTheta, tailSinTheta, tailCosTheta;
	pointd				p10, p11, p12, p20, p21, p22, p1n, p2n, p1t, p2t, p100, p200;
	WrFixed 			sinTheta , cosTheta ;
	Coord				fixArrowWidth , fixArrowHeight ;
	WrFixed 			thetaRadians , fixTail , fixNose , fixOutsideW ;
	WrFixed				fixZHalfPenW;
	long				penWCosTheta,penWSinTheta;
//	PolyHandle			leftArrow, rightArrow;
	uchar				style;
	short				arrowWidth, arrowHeight, nose, tail;
	short				debug_ = 0;
	pointd				p1, p2;
	short				halfPenW;

	short				veePen;			/* zoomed */
	Coord				fixVPen, adjust1, adjust2;
	short				type;
//	PenState			penState;
//	short				penSize;


#ifdef DEBUGGING
//	debug_ = (short) debugMenu;
#endif

	CDC* pCDC=pDC->GetCDC();
	CMapper* pMapper=pDC->GetMapper();

	pMapper->LPtoDP (p1, *lPt1 ) ;
	pMapper->LPtoDP (p2 , *lPt2 ) ;

	arrowWidth = endcapPtr->width;
	arrowHeight =  endcapPtr->height;
	nose = endcapPtr->nose;
	tail = endcapPtr->tail;
	style = (uchar)endcapPtr->opcode;

	/* Dont want to change the thin pen set in drawEndcap */
	halfPenW = pMapper->ScreenPenW(1) >> 1;

	type = endcapPtr->type;
	if (type == kVArrow)  {
		if (!(veePen = (penWidth + 1) >> 1))  veePen = 1;
		fixVPen = Coord (veePen);
		adjust1 = adjust2 = FixedZero ;
		if (map & kEnd1)  
		{
			// pMapper->m_nZoomFactor > 0 ?????????????
			adjust1 = pMapper->NoZoomToZoom(fixVPen);
			adjust1 >>= 1;
		}
		if (map & kEnd2) 
		{
			adjust2 = pMapper->NoZoomToZoom(fixVPen);
			adjust2 >>= 1;
		}
		lineAdjust (lPt1, lPt2, adjust1, adjust2);
		halfPenW = veePen >> 1;
	}

#ifdef SUB_HALF_PEN
	p1.x -= halfPenW;
	p1.y -= halfPenW;
	p2.x -= halfPenW;
	p2.y -= halfPenW;
#endif
	thetaRadians = ATan2 ((long)(p1.x-p2.x), (long)(p1.y - p2.y) ) ;
	if (map & kDimEndmarkOut)		/* reverse arrow orientation */
		thetaRadians += kFixedPie;

	cosTheta = Cos ( thetaRadians ) ;
	sinTheta = Sin ( thetaRadians ) ;

	fixArrowWidth = Coord(short( pMapper->LPtoDP(arrowWidth)));
	fixArrowHeight = Coord(short(pMapper->LPtoDP(arrowHeight)));
	fixTail = Coord(short(pMapper->LPtoDP(tail)));
	fixNose = Coord(short(pMapper->LPtoDP(nose)));

	fixZHalfPenW = Coord(penWidth) >> 1;

	wCosTheta = ( fixArrowWidth * cosTheta ).GetLong () ;
	wSinTheta = ( fixArrowWidth * sinTheta ).GetLong () ;

	hCosTheta = ( fixArrowHeight * cosTheta ).GetLong () ;
	hSinTheta = ( fixArrowHeight * sinTheta ).GetLong () ;

	noseSinTheta = ( fixNose * sinTheta ).GetLong () ;
	noseCosTheta = ( fixNose * cosTheta ).GetLong () ;

	tailSinTheta = ( fixTail * sinTheta ).GetLong () ;
	tailCosTheta = ( fixTail * cosTheta ).GetLong () ;

	penWSinTheta = ( fixZHalfPenW * sinTheta ).GetLong () ;
	penWCosTheta = ( fixZHalfPenW * cosTheta ).GetLong () ;

	if (map & kEnd1) {
		p1n.x = p1.x - noseCosTheta;
		p1n.y = p1.y - noseSinTheta;

		if (isCurve) {
			p1n.x += penWCosTheta + wCosTheta-tailCosTheta;
			p1n.y += penWSinTheta + wSinTheta-tailSinTheta;
		}

		p10.x = p1n.x - wCosTheta;
		p10.y = p1n.y - wSinTheta;

		p11.x = p10.x + hSinTheta;
		p11.y = p10.y - hCosTheta;

		p12.x = p10.x - hSinTheta;
		p12.y = p10.y + hCosTheta;

		p1t.x = p10.x + tailCosTheta;
		p1t.y = p10.y + tailSinTheta;


      //if(pDC->m_bTransparentBackground)
      //if (false)
      {
         // Infinisys Ltd.
         ::std::unique_ptr<Gdiplus::Pen> penStore;
         ::std::unique_ptr<Gdiplus::Brush> brushStore;

         Gdiplus::Pen * thePen = nullptr;
         Gdiplus::Brush * brush = nullptr;
         //Gdiplus::Graphics g(pCDC->GetSafeHdc());

         pDC->graphics_on();

         Gdiplus::Point pt[4];
         pt[0] = Gdiplus::Point(p1n.x,p1n.y);
         pt[1] = Gdiplus::Point(p11.x,p11.y);
         pt[2] = Gdiplus::Point(p1t.x,p1t.y);
         pt[3] = Gdiplus::Point(p12.x,p12.y);
         if(style & (kFill | kFrame))		/* do arrowhead */
         {
            COLORREF cr = penModel->m_crColor;
            if(cr & PALETTEINDEX(0))
            {
               PALETTEENTRY pe;
               pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
               cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
            }
            //if(penModel->m_nPenType == kPenColor)
            //{
            //   thePen = pDC->m_PenPlus;
            //}
            //else
            //{
            //   thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,0, 0, 0),Gdiplus::REAL(thinPenSize)));
            //}
            thePen = pDC->m_PenPlus.get();
            brush = pDC->m_PenBrushPlus.get();
            //if(style & kFill)
            //{
            //   if(penModel->m_nPenType == kPenColor)
            //   {
            //       brush.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,GetRValue(cr),GetGValue(cr),GetBValue(cr))));
            //   }
            //   else
            //   {
            //      brush.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,0,0,0)));
            //   }
            //}
            if(style & kFrame)
            {
               ASSERT(!(style & kFill));
               brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,255,255,255)));
               brush = brushStore.get();
            }
            if(!brush)
            {
               brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,255,255,255)));
               brush = brushStore.get();
            }

            pDC->graphics()->FillPolygon(brush, pt, 4);

            pDC->graphics()->DrawPolygon(thePen, pt, 4);

         }

         else if(type == kVArrow)  {
            // Infinisys Ltd.
            ::std::unique_ptr<Gdiplus::Pen> penStore;

            Gdiplus::Pen * thePen = nullptr;
            COLORREF cr = penModel->m_crColor;
            if(cr & PALETTEINDEX(0))
            {
               PALETTEENTRY pe;
               pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
               cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
            }
            if(penModel->m_nPenType == kPenColor)
            {
               //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,GetRValue(cr),GetGValue(cr),GetBValue(cr)),Gdiplus::REAL(veePen)));
               thePen = pDC->m_PenPlus.get();
            }
            else
            {
               //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,0,0,0),Gdiplus::REAL(veePen)));
               thePen = pDC->m_PenPlus.get();
            }
            //Gdiplus::Graphics g(pCDC->GetSafeHdc());
            pDC->graphics()->DrawLine(thePen,pt[1],pt[0]) ;
            pDC->graphics()->DrawLine(thePen,pt[0],pt[3]) ;
            if(style & kClose)
            {
               pDC->graphics()->DrawLine(thePen,pt[3],pt[1]) ;
            }
         }

      }
      //else
      //{
      //   CBrush	brush,*pOldBrush=NULL;

      //   if(style & (kFill | kFrame))		/* do arrowhead */
      //   {
      //      pointd pt[4];
      //      pt[0] = p1n;
      //      pt[1] = p11;
      //      pt[2] = p1t;
      //      pt[3] = p12;
      //      CRgn	poly;
      //      poly.CreatePolygonRgn(pt,4,ALTERNATE);

      //      CBrush whiteBrush,fillBrush;
      //      whiteBrush.CreateStockObject(WHITE_BRUSH);
      //      if(penModel->m_nPenType == kPenColor) fillBrush.CreateSolidBrush(penModel->m_crColor);
      //      else fillBrush.CreateStockObject(BLACK_BRUSH);

      //      CBrush *pOldBrush=NULL,*pFrameBrush=NULL;

      //      if(style & kFill)
      //      {
      //         pOldBrush=pCDC->SelectObject(&fillBrush);
      //      }
      //      if(style & kFrame)
      //      {
      //         ASSERT(!(style & kFill));
      //         pOldBrush=pCDC->SelectObject(&whiteBrush);
      //      }
      //      pCDC->PaintRgn(&poly);
      //      pCDC->SelectObject(pOldBrush);

      //      pCDC->FrameRgn(&poly,&fillBrush,thinPenSize,thinPenSize);
      //   }

      //   else if(type == kVArrow)  {
      //      CPen thePen;
      //      thePen.CreatePen(PS_SOLID,veePen,(penModel->m_nPenType == kPenColor)?penModel->m_crColor:RGB(0,0,0));
      //      CGDIObject<CPen> gdiPen(pCDC,&thePen);

      //      pCDC->MoveTo(p11) ;
      //      pCDC->LineTo(p1n) ;
      //      pCDC->LineTo(p12) ;
      //      if(style & kClose)
      //         pCDC->LineTo(p11) ;
      //   }

      //}
	}

	if (map & kEnd2) {
		p2n.x = p2.x + noseCosTheta;
		p2n.y = p2.y + noseSinTheta;

		if (isCurve) {
			p2n.x -= penWCosTheta + wCosTheta-tailCosTheta;
			p2n.y -= penWSinTheta + wSinTheta-tailSinTheta;
		}

		p20.x = p2n.x + wCosTheta;
		p20.y = p2n.y + wSinTheta;

		p21.x = p20.x + hSinTheta;
		p21.y = p20.y - hCosTheta;

		p22.x = p20.x - hSinTheta;
		p22.y = p20.y + hCosTheta;

		p2t.x = p20.x - tailCosTheta;
		p2t.y = p20.y - tailSinTheta;
      //if(pDC->m_bTransparentBackground)
      {     // Infinisys Ltd.
         ::std::unique_ptr<Gdiplus::Pen> penStore;
         ::std::unique_ptr<Gdiplus::Brush> brushStore;

         Gdiplus::Pen * thePen = nullptr;
         Gdiplus::Brush * brush = nullptr;
         //Gdiplus::Graphics g(pCDC->GetSafeHdc());
         Gdiplus::Point pt[4];
         pt[0] = Gdiplus::Point(p2n.x,p2n.y);
         pt[1] = Gdiplus::Point(p21.x,p21.y);
         pt[2] = Gdiplus::Point(p2t.x,p2t.y);
         pt[3] = Gdiplus::Point(p22.x,p22.y);
         if(style & (kFill | kFrame))		/* do arrowhead */
         {


            COLORREF cr = penModel->m_crColor;
            if(cr & PALETTEINDEX(0))
            {
               PALETTEENTRY pe;
               pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
               cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
            }
            //if(penModel->m_nPenType == kPenColor)
            //{
            //   thePen = pDC->m_PenPlus;
            //}
            //else
            //{
            //   thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,0, 0, 0),Gdiplus::REAL(thinPenSize)));
            //}
            thePen = pDC->m_PenPlus.get();
            brush = pDC->m_PenBrushPlus.get();
            //if(style & kFill)
            //{
            //   if(penModel->m_nPenType == kPenColor)
            //   {
            //       brush.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,GetRValue(cr),GetGValue(cr),GetBValue(cr))));
            //   }
            //   else
            //   {
            //      brush.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255,0,0,0)));
            //   }
            //}
            if (style & kFrame)
            {
               ASSERT(!(style & kFill));
               brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255)));
               brush = brushStore.get();
            }
            if (!brush)
            {
               brushStore.reset(new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255)));
               brush = brushStore.get();
            }


            pDC->graphics()->FillPolygon(brush, pt, 4);

            pDC->graphics()->DrawPolygon(thePen, pt, 4);

         }
         else if(type == kVArrow)  {
            // Infinisys Ltd.
            ::std::unique_ptr<Gdiplus::Pen> penStore;

            Gdiplus::Pen * thePen = nullptr;
            COLORREF cr = penModel->m_crColor;
            if(cr & PALETTEINDEX(0))
            {
               PALETTEENTRY pe;
               pDC->m_pDoc->m_Palette.GetPaletteEntries(cr & 0xFFFF,1,&pe);
               cr=RGB(pe.peRed,pe.peGreen,pe.peBlue);
            }
            if (penModel->m_nPenType == kPenColor)
            {
               //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,GetRValue(cr),GetGValue(cr),GetBValue(cr)),Gdiplus::REAL(veePen)));
               thePen = pDC->m_PenPlus.get();
            }
            else
            {
               //thePen.reset(new Gdiplus::Pen(Gdiplus::Color(255,0,0,0),Gdiplus::REAL(veePen)));
               thePen = pDC->m_PenPlus.get();
            }
            //Gdiplus::Graphics g(pCDC->GetSafeHdc());
            pDC->graphics()->DrawLine(thePen, pt[1], pt[0]);
            pDC->graphics()->DrawLine(thePen, pt[0], pt[3]);
            if (style & kClose)
            {
               pDC->graphics()->DrawLine(thePen, pt[3], pt[1]);
            }
         }

      }
      //else
      //{
      //   if(style & (kFill | kFrame))		/* do arrowhead */
      //   {
      //      pointd pt[4];
      //      pt[0] = p2n;
      //      pt[1] = p21;
      //      pt[2] = p2t;
      //      pt[3] = p22;
      //      CRgn	poly;
      //      poly.CreatePolygonRgn(pt,4,ALTERNATE);

      //      CBrush whiteBrush,fillBrush;
      //      whiteBrush.CreateStockObject(WHITE_BRUSH);
      //      if(penModel->m_nPenType == kPenColor) fillBrush.CreateSolidBrush(penModel->m_crColor);
      //      else fillBrush.CreateStockObject(BLACK_BRUSH);

      //      CBrush *pOldBrush=NULL,*pFrameBrush=NULL;

      //      if(style & kFill)
      //      {
      //         pOldBrush=pCDC->SelectObject(&fillBrush);
      //      }
      //      if(style & kFrame)
      //      {
      //         ASSERT(!(style & kFill));
      //         pOldBrush=pCDC->SelectObject(&whiteBrush);
      //      }
      //      pCDC->PaintRgn(&poly);
      //      pCDC->SelectObject(pOldBrush);

      //      pCDC->FrameRgn(&poly,&fillBrush,thinPenSize,thinPenSize);
      //   }
      //   else if(type == kVArrow)  {
      //      CPen thePen;
      //      thePen.CreatePen(PS_SOLID,veePen,(penModel->m_nPenType == kPenColor)?penModel->m_crColor:RGB(0,0,0));
      //      CGDIObject<CPen> gdiPen(pCDC,&thePen);

      //      pCDC->MoveTo(p21) ;
      //      pCDC->LineTo(p2n) ;
      //      pCDC->LineTo(p22) ;
      //      if(style & kClose)
      //         pCDC->LineTo(p21) ;
      //   }
      //}
	}
}	/* arrowEM */


void CDrawPrim::lineCapped (
	CMapper* pMapper,
	LongPointPtr		p1,
	LongPointPtr		p2,
	short				cap1,
	short				cap2,
	short				whichCap,
	short				pmPenW,
	LongPointPtr		newP1,
	LongPointPtr		newP2)
{
	register CDrawPrim::EndcapRec	*capPtr1, *capPtr2;

	/* unzoomed quantities */
	Coord							fixWidth1, fixNose1, fixTail1, fixVArrowRatio,
									fixWidth2, fixNose2, fixTail2,
									d1, d2;
	short							zPenW;
	LongPoint						sp1,sp2;

	fixWidth1 = fixWidth2 = d1 = d2 = FixedZero ;
	zPenW = pMapper->ScreenPenW(pmPenW);
	fixVArrowRatio = WrFixedFromRatio ( 3 , 2 ) ;

	sp1 = *p1;
	sp2 = *p2;

	if ( (whichCap & kEnd1) && cap1 != 0 ) {
		capPtr1 = &gEndcaps[ getEndcapIndex(cap1,pmPenW) ];
		/* if printing Zoom procedures do not modify values */

		fixWidth1 = capPtr1->width;	/* guLOrd converts to fixed */
		fixNose1 = capPtr1->nose;
		fixTail1 = capPtr1->tail;
	}
	if ( (whichCap & kEnd2) && cap2 != 0 ) {
		capPtr2 = &gEndcaps[ getEndcapIndex(cap2,pmPenW) ];
		fixWidth2 = capPtr2->width;
		fixNose2 = capPtr2->nose;
		fixTail2 = capPtr2->tail;
	}

	/* note Fixed format is same as longPoint format */

	if ( fixWidth1 != FixedZero ) {
		if (capPtr1->type == kVArrow)  {
			d1 = pMapper->DPtoLP(zPenW); // Fishy, what if gZoomFactor was < 0 ?
			d1 *= fixVArrowRatio ;
		}
		else
			d1 = fixWidth1 + fixNose1 - fixTail1;
	}
	if ( fixWidth2 != FixedZero ) {
		if (capPtr2->type == kVArrow)  {
			d2 = pMapper->DPtoLP(zPenW);; // Fishy, what if gZoomFactor was < 0 ?
			d2 *= fixVArrowRatio ;
		}
		else
			d2 = fixWidth2 + fixNose2 - fixTail2;
	}

	lineAdjust (&sp1, &sp2, d1, d2);

	*newP1 = sp1;
	*newP2 = sp2;
} /* lineCapped */

/*------------------------------------------------*/

void CDrawPrim::makeButtCaps (
	short					penWidth,
	register const pointd&	p1,
	register const pointd&	p2,
	register pointd		*newP1,
	register pointd		*newP2)
{
	Coord			lPenWidth;
	Coord			lDh, lDv;
	Angle			theAngle ;
	WrFixed			cosAngle, sinAngle;

	*newP1 = p1;
	*newP2 = p2;

	if ((p1.y != p2.y) && (p1.x != p2.x)) {
		if (abs(p2.y - p1.y) <= penWidth) return;
		if (abs(p2.x - p1.x) <= penWidth) return;
	}

	lPenWidth = short(penWidth >> 1);
	lDh = short(newP1->x - newP2->y);
	lDv = short(newP1->x - newP2->y);

	theAngle = mtFastATan2 ( LongPoint ( lDh, lDv ) ) ;
	cosAngle = WrFixedFromFract (mtLCos(theAngle));
	sinAngle = WrFixedFromFract (mtLSin(theAngle));

	newP1->x += mtFixToShort ( lPenWidth * cosAngle ) ;
	newP1->y += mtFixToShort ( lPenWidth * sinAngle ) ;

	newP2->x -= mtFixToShort ( lPenWidth * cosAngle ) ;
	newP2->y -= mtFixToShort ( lPenWidth * sinAngle ) ;
} /* makeButtCaps() */

void	CDrawPrim::LineBorderPos (
	CMapper* pMapper,
	PenModel* penModel,
	short cap1,
	short cap2,
	unsigned short adjustEnd,
	LongPoint *p1,
	LongPoint *p2,
	LongPoint *newP1,
	LongPoint *newP2)
{
	Coord		lPenWidth;
	WrFixed		theAngle ; // In Radians
	WrFixed		cosAngle, sinAngle;

	LongPoint tmpP1 = *newP1 = *p1;
	LongPoint tmpP2 = *newP2 = *p2;

	if (penModel->m_nAlignment == kCenterLine)
		return;

	if (!cap1 || !cap2) 
	{
		lPenWidth = pMapper->PenW(penModel->m_nWidth) - FixedMinDelta;
		lPenWidth >>= 1;
		switch(penModel->m_nAlignment)
		{
		case kInsideLine:
			lPenWidth = -lPenWidth;
			break;
		case kExtraLine:
			lPenWidth*=2;
			break;
		}
/*		if (!((CPCDraftApp*)AfxGetApp())->m_bZoomLineWeights)
		{
			lPenWidth = pMapper->NoZoomToZoom(lPenWidth );
		}
*/
		theAngle = ATan2 ( tmpP1 - tmpP2 ) ;

		cosAngle = WrFixedFromFixed(Cos(theAngle).GetFixed() & ~1L);
		sinAngle = WrFixedFromFixed(Sin(theAngle).GetFixed() & ~1L);

		if (!cap1 && (adjustEnd & kEnd1)) {
			tmpP1.x += lPenWidth * cosAngle ;
			tmpP1.y += lPenWidth * sinAngle ;
		}

		if (!cap2 && (adjustEnd & kEnd2)) {
			tmpP2.x -= lPenWidth * cosAngle ;
			tmpP2.y -= lPenWidth * sinAngle ;
		}

		*newP1 = tmpP1;
		*newP2 = tmpP2;
	}
} /* LineBorderPos */

void CDrawPrim::getDashStr(short index, uchar *dashCodeStr)
{
	::memcpy(dashCodeStr, (DashStyle *)*m_DashTable + index, kDashCodeEntrySize);
}

