// DrawPrimArc.cpp: implementation of the CDrawPrim class for drawing arc.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawPrim.h"
#include "PCDraftDoc.h"
#include "phmath.h"
#include "DraftContext.h"
#include "float.h"

#include "MDLineShape.h"


// PCDraft Arc positive counter-clockwise 0 on x-axis
// GDI+ Arc positive clockwise 0 on x-axis


float angle_to_gdiplus(Angle a)
{
   return -a.GetDouble();
}

Angle angle_from_gdiplus(float f)
{
   return -f;
}


const long	kStrokeLen = 500L;

struct CArcStruct {
	double				fh, fv;
	double			fZoomedHRad, fZoomedVRad ;
	TSinCos			fSinCosRot ;
	LongPoint		lptStart,lptEnd;
	Angle			fStartAng, fEndAng;
	Angle			fLongRot;
	bool			fIsFine;
	bool			fIsClosed;
   bool        fIsEllipse; // Opacity/Transparency - Infinisys Ltd.
	short			fPenW;
	Alignment		fPenAlignment;
	int				nDirection;

	pointd arcPointWithCoords(double vx, double vy){
		double x, y;
/*		if ( fLongRot != FixedZero ){
			x= fh + (int)( ( fSinCosRot.fCos * vx - fSinCosRot.fSin * vy ) + 0.5f );
			y= fv + (int)( ( fSinCosRot.fSin * vx + fSinCosRot.fCos * vy ) + 0.5f );
		} else {
			x = fh + (int)vx;
			y = fv + (int)vy;
		}*/
		if ( fLongRot != FixedZero ){
			x= fh + mtRealToShort ( fSinCosRot.fCos * vx - fSinCosRot.fSin * vy );
			y= fv + mtRealToShort ( fSinCosRot.fSin * vx + fSinCosRot.fCos * vy );
		} else {
			x = fh + vx;
			y = fv + vy;
		}
		
		return pointd(x, y);
	}
};

void				arcPoint			(const CArcStruct&, Angle, pointd *);
void	arcSlow ( CDrawPrim* inPrim, CDraftContext* pDC, CArcStruct& inArc, bool bAdjustAlign);

void CDrawPrim::drawArc (
	CDraftContext* pDC,
	LongPointPtr	pCenter,
	Coord				pHRad,
	Coord				pVRad,
	const LongPoint&	lptStart,
	const LongPoint&	lptEnd,
	Angle				pRot,
	bool			pIsFine,
	int nDirection,
	bool bAdjustAlign, 
   bool bClose)
{
	if ( pHRad < FixedZero || pVRad < FixedZero )	return;

	CMapper* pMapper=pDC->GetMapper();
	const PenModel* penModel=&pDC->GetPen();
	ASSERT( penModel->m_nAlignment == kInsideLine || penModel->m_nAlignment == kCenterLine || penModel->m_nAlignment == kOutsideLine);

	Alignment		penAlignment = Alignment( penModel->m_nAlignment );

	CArcStruct	theArc;
   pointd pt;
   pMapper->LPtoDP(pt,*pCenter);
	theArc.fh=pt.x;
	theArc.fv=pt.y;
	theArc.fZoomedHRad = mtFixToReal(pMapper->NoZoomToZoom(pHRad));
	theArc.fZoomedVRad = mtFixToReal(pMapper->NoZoomToZoom(pVRad));
	theArc.lptStart=lptStart;
	theArc.lptEnd=lptEnd;
	theArc.fStartAng = guLPtToLAngle(pHRad,pVRad,mtRotPoint(lptStart,-pRot,*pCenter),*pCenter);
	theArc.fEndAng = guLPtToLAngle(pHRad,pVRad,mtRotPoint(lptEnd,-pRot,*pCenter),*pCenter);
	theArc.fLongRot = pRot;

	theArc.fPenW = pMapper->ScreenPenW(penModel->m_nWidth);
	theArc.fPenAlignment = penAlignment;

	theArc.fIsFine = pIsFine;
	theArc.fIsClosed = false;
   theArc.fIsEllipse = false;
	theArc.nDirection=nDirection;

//  in new 	theArc.fPenW = penModel->m_nWidth;

	//* draw a point if it is too small 
	if ( theArc.fZoomedHRad < 2.0 && theArc.fZoomedVRad < 2.0 )
	{
      pointd pt1; 
      pointd pt2; 
      pMapper->LPtoDP(pt1, lptStart);
      pMapper->LPtoDP(pt2, lptEnd);
		CDC* pCDC=pDC->GetCDC();
		if(pCDC->GetCurrentPosition()!= pointd(pt1.x, pt2.y))
			pCDC->MoveTo((LONG)pt1.x,(LONG)pt1.y);
		dashLine(pDC, pt1, pt2);
		return;
	}

	theArc.fSinCosRot = TSinCos( theArc.fLongRot );

/*	if ( theArc.fStartAng == theArc.fEndAng )
	{
		theArc.fStartAng = FixedZero ;
		theArc.fEndAng = k360;
	}
*/
	arcSlowNew(pDC, theArc, bAdjustAlign, bClose);

}// drawArc 


void CDrawPrim::drawEllipse (
	CDraftContext*	pDC,
	LongPointPtr	pCenter,
	Coord				pHRad,
	Coord				pVRad,
	Angle				pRot,
	bool			pIsFine)
{
		
	if ( pHRad < FixedZero || pVRad < FixedZero )	return;

	CMapper* pMapper=pDC->GetMapper();
	const PenModel* penModel=&pDC->GetPen();
	ASSERT( penModel->m_nAlignment == kInsideLine || penModel->m_nAlignment == kCenterLine || penModel->m_nAlignment == kOutsideLine);

	Alignment	penAlignment = Alignment( penModel->m_nAlignment );
	short penW = penModel->m_nWidth;

	CArcStruct	theArc;
   pointd pt;
   pMapper->LPtoDP(pt, *pCenter);
	theArc.fh=pt.x;
	theArc.fv=pt.y;
	theArc.fZoomedHRad = mtFixToReal(pMapper->NoZoomToZoom( pHRad ));//LPtoDP( pHRad );
	theArc.fZoomedVRad = mtFixToReal(pMapper->NoZoomToZoom( pVRad ));//LPtoDP( pVRad );

	theArc.lptStart=mtRotPoint(*pCenter+LongPoint(pHRad,0),pRot,*pCenter);
	theArc.lptEnd=theArc.lptStart;
	theArc.fStartAng = 0;
	theArc.fEndAng = k360;
	theArc.fLongRot = pRot;
	theArc.fPenW = pMapper->ScreenPenW(penW);
	theArc.fPenAlignment = penAlignment;

	theArc.fIsFine = pIsFine;
	theArc.fIsClosed = false;
   theArc.fIsEllipse = true;
	theArc.nDirection=AD_COUNTERCLOCKWISE;

	/* draw a point if it is too small */
	if ( theArc.fZoomedHRad < 2.0 && theArc.fZoomedVRad < 2.0 )
	{
		CDC* pCDC=pDC->GetCDC();
		if(pCDC->GetCurrentPosition()!= pointd(pt.x, pt.y))
			pCDC->MoveTo((LONG)pt.x,(LONG)pt.y);
		dashLine(pDC, pt, pointd(pt.x+1.f,pt.y));
		return;
	}

	if ( theArc.fLongRot != FixedZero )
		theArc.fSinCosRot = TSinCos ( theArc.fLongRot ) ;
	else
		theArc.fSinCosRot.fSin = theArc.fSinCosRot.fCos = 0.0 ;

	arcSlowNew ( pDC, theArc, true, true);

}/* drawEllipse */

#define kHairWidth				0.24f

inline double Pythag(pointd pt) {
   return sqrt((double)(pt.x * pt.x + pt.y * pt.y));
}
short Pythag(CSize pt);
inline short Pythag(CSize pt) {
   return (short)sqrt((double)(pt.cx * pt.cx + pt.cy * pt.cy));
}


void CDrawPrim::arcSlowOutline(CDraftContext* pDC, 
							   CArcStruct &inArc)
{
	double HRadius = inArc.fZoomedHRad;
	double VRadius = inArc.fZoomedVRad;

	if(inArc.nDirection==AD_CLOCKWISE)
	{
		while( inArc.fStartAng < inArc.fEndAng){
			inArc.fStartAng+=k360;
		}
	}
	else
	{
		while ( inArc.fEndAng < inArc.fStartAng ){
 			inArc.fEndAng += k360;
		}
	}
/*	if(	inArc.fStartAng == inArc.fEndAng ){		
		inArc.fStartAng = FixedZero ;
		inArc.fEndAng = k360;
		inArc.nDirection = AD_COUNTERCLOCKWISE;
	}
*/
	CDC * pCDC = pDC->GetCDC();
   std::unique_ptr < Gdiplus::Graphics > g;
   CArray < pointd > pta;
   //if(pDC->m_PenPlus != NULL && !pDC->m_bOutline) // Interface Update - Infinisys Ltd.
   {
      pDC->graphics_on();
   }
	double vIncr= 0.0 + ChordTolerance ( Max ( (float)inArc.fZoomedHRad, (float)inArc.fZoomedVRad ) );
	register double vAngle = 0.0 + inArc.fStartAng;
	register double vEndAngle = 0.0 + inArc.fEndAng;

	register double cosAngle;
	register double sinAngle;

	CMapper* pMapper=pDC->GetMapper();
	bool bCenterAlign=pDC->GetPen().m_nAlignment == kCenterLine;

	pointd arcPt;
	if(bCenterAlign)
		pMapper->LPtoDP(arcPt, inArc.lptStart);
	else
		arcPoint(inArc,inArc.fStartAng,&arcPt);

   //if(pDC->m_bUseGDI || pDC->m_PenPlus == NULL || pDC->m_bOutline) // Interface Update - Infinisys Ltd.
   if (pDC->m_bUseGDI) // Interface Update - Infinisys Ltd.
   {
      
      if (pCDC->GetCurrentPosition() != arcPt)
      {

         pCDC->MoveTo((LONG)arcPt.x, (LONG)arcPt.y);

      }

   }
   else
   {
      pta.Add(arcPt);
   }

	if(inArc.nDirection==AD_CLOCKWISE){
		ASSERT(inArc.fStartAng >= inArc.fEndAng);
		vAngle -=vIncr;

		while ( vAngle > vEndAngle){
			cosAngle = mtCos ( vAngle);
			sinAngle = mtSin ( vAngle);
			
			register double vx = HRadius * cosAngle;
			register double vy = VRadius * sinAngle;

			arcPt=inArc.arcPointWithCoords(vx, vy);
         //if(pDC->m_bUseGDI ||pDC->m_PenPlus == NULL || pDC->m_bOutline) // Interface Update - Infinisys Ltd.
         if (pDC->m_bUseGDI ) // GDI+ - Infinisys Ltd.
         {
            pCDC ->LineTo(__point(arcPt));
         }
         else
         {
            pta.Add(arcPt);
         }
 			vAngle -= vIncr;
		}
	}else{
		vAngle +=vIncr;
		while ( vAngle < vEndAngle ){
			cosAngle = mtCos ( vAngle );
			sinAngle = mtSin ( vAngle );
			
			register double vx = HRadius * cosAngle;
			register double vy = VRadius * sinAngle;

			arcPt=inArc.arcPointWithCoords(vx, vy);
         //if(pDC->m_bUseGDI || pDC->m_PenPlus == NULL || pDC->m_bOutline) // Interface Update - Infinisys Ltd.
         if (pDC->m_bUseGDI ) // GDI+ - Infinisys Ltd.
         {
            pCDC ->LineTo(__point(arcPt));
         }
         else
         {
            pta.Add(arcPt);
         }
			vAngle += vIncr;
		}
	}	

	if(bCenterAlign )
		pMapper->LPtoDP(arcPt, inArc.lptEnd);
	else
		arcPoint(inArc,inArc.fEndAng,&arcPt);

   //if(pDC->m_bUseGDI || pDC->m_PenPlus == NULL || pDC->m_bOutline) // Interface Update - Infinisys Ltd.
   if (pDC->m_bUseGDI ) // Interface Update - Infinisys Ltd.
   {
      if(pCDC->GetCurrentPosition() != pointd(arcPt.x, arcPt.y))
         pCDC ->LineTo(__point(arcPt));
   }
   else
   {
      pta.Add(arcPt);
   }

	if( inArc.fIsClosed )
	{
      if(pDC->m_bUseGDI) // GDI+ - Infinisys Ltd.
         //if (pDC->m_PenPlus == NULL || pDC->m_bOutline) // Interface Update - Infinisys Ltd.
      {
         pCDC ->LineTo(inArc.fh,inArc.fv);
         arcPoint(inArc,inArc.fStartAng,&arcPt);
         pCDC ->LineTo(__point(arcPt));
      }
      else
      {
         pta.Add(pointd(inArc.fh, inArc.fv));
         arcPoint(inArc,inArc.fStartAng,&arcPt);
         pta.Add(arcPt);
      }
	} 


   if (!pDC->m_bUseGDI)// GDI+ - Infinisys Ltd.
   {
      if (pDC->m_PenPlus != NULL && !pDC->m_bOutline) // Interface Update - Infinisys Ltd.
      {

         CArray < pointf > pfa;

         for (int i = 0; i < pta.GetCount(); i++)
         {

            pfa.Add(pointf(pta[i].x, pta[i].y));

         }


         if (inArc.fIsEllipse)
         {
            pDC->m_pgraphics->DrawPolygon(pDC->m_PenPlus.get(), pfa.GetData(), (int)pfa.GetCount());
         }
         else
         {
            pDC->m_pgraphics->DrawLines(pDC->m_PenPlus.get(), pfa.GetData(), (int)pfa.GetCount());
         }

      }
      else if (pDC->m_ppath)
      {

         CArray < pointf > pfa;

         for (int i = 0; i < pta.GetCount(); i++)
         {

            auto p1 = pointf(pta[i].x, pta[i].y);

            pfa.Add(p1);

         }


         if (inArc.fIsEllipse)
         {
            pDC->m_ppath->AddPolygon(pfa.GetData(), (int)pfa.GetCount());
         }
         else
         {
            pDC->m_ppath->AddLines(pfa.GetData(), (int)pfa.GetCount());
         }

      }
      else
      {

         CArray < pointf > pfa;

         for (int i = 0; i < pta.GetCount(); i++)
         {

            pfa.Add(pointf(pta[i].x, pta[i].y));

         }


         if (inArc.fIsEllipse)
         {
            pDC->m_pgraphics->DrawPolygon(pDC->m_PenPlus.get(), pfa.GetData(), (int)pfa.GetCount());
         }
         else
         {
            pDC->m_pgraphics->DrawLines(pDC->m_PenPlus.get(), pfa.GetData(), (int)pfa.GetCount());
         }

      }

   }


}

inline bool tolerance_equal(double a, double b, double tolerance)
{

   return fabs(a - b) < tolerance;

}

void CDrawPrim::arcSlowNew ( CDraftContext* pDC, 
							CArcStruct& inArc, 
							bool bAdjustAlign, bool bClose)
{
   
   double halfPenWidth = (double) inArc.fPenW / 2.0;

   if (bAdjustAlign && halfPenWidth > 0)
   {
      if (inArc.fPenAlignment == kInsideLine)
      {
         inArc.fZoomedHRad -= halfPenWidth;
         inArc.fZoomedVRad -= halfPenWidth;
      }
      else if (inArc.fPenAlignment == kOutsideLine)
      {
         inArc.fZoomedHRad += halfPenWidth;
         inArc.fZoomedVRad += halfPenWidth;
      }
   }

// If this is true then we only want to draw a simple arc
if (pDC->IsOutline() ||
   pDC->IsCreatingPath()) {
   arcSlowOutline(pDC, inArc);
   return;
}

//if (inArc.nDirection == AD_CLOCKWISE) {
//   Angle temp = inArc.fEndAng;
//   inArc.fEndAng = inArc.fStartAng;
//   inArc.fStartAng = temp;
//
//   LongPoint lptTemp = inArc.lptStart;
//   inArc.lptStart = inArc.lptEnd;
//   inArc.lptEnd = lptTemp;
//}

if (inArc.nDirection != AD_CLOCKWISE)
{

   while (inArc.fEndAng < inArc.fStartAng)
   {

      inArc.fEndAng += k360;

   }

}

//double vIncr = 0.0 + ChordTolerance(Max((float)inArc.fZoomedHRad, (float)inArc.fZoomedVRad));
//vIncr /= 4.0;
double vIncr = 72.0 / (2.0 * 3.14159265359 * Max((float)inArc.fZoomedHRad, (float)inArc.fZoomedVRad));
register double vAngle = 0.0 + inArc.fStartAng;
const register double vEndAngle = 0.0 + inArc.fEndAng;
// Set up the dashing table

unsigned short dashCodes[kDashCodeEntrySize] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                                       0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                                       0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                                       0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
const PenModel* penModel = &pDC->GetPen();
short dashCode = (penModel ? penModel->Dashing() : 0);
if (dashCode) {
   uchar dashData[255];
   getDashStr(dashCode, dashData);

   if (dashData[2] == 0xFF) {
      dashCode = 0;
   }

   if (dashCode) {
      long blackCount = 0;
      long whiteCount = 0;
      CMapper* pMapper = pDC->GetMapper();
      for (short i = 0; i < kMaxDashCodes; ++i) {
         uchar tempChar = dashData[i + 1];
         if (tempChar != 0xFF) {

            long tempShort = pMapper->NoZoomToZoom((long)TruncToShort(Coord((short)tempChar)));
            if (i & 1) // white
               whiteCount += tempShort;
            else
               blackCount += tempShort;
            dashCodes[i] = (tempShort ? tempShort : 1);
         }
      }
      if (whiteCount == 0)
         dashCode = 0;
      else
         if (blackCount == 0)
            return;
   }
   if (vIncr > 1) {
      vIncr = 60.0f / Max(inArc.fZoomedHRad, inArc.fZoomedVRad);
   }
}

// Set up the radii
float zoomedPenWidth = 1.0;

float penf;
//beware of posible bug because of unknown using of fPenStyle in Mac Version
short hairLineCount = (penModel ? penModel->m_nWidth & HairlineMask : 0);
if (hairLineCount)
{
   switch (penModel->HairlineCnt())
   {
   case 1:		// 0.5 pt
   {
      penf = 0.5;
      break;
   }

   case 2:		// 0.75
   {
      penf = 0.75;
      break;
   }

   case 0:		// true hairline
   default:
   {
      penf = kHairWidth;
      break;
   }
   }
}
else
{
   penf = inArc.fPenW;
}

CMapper* pMapper = pDC->GetMapper();
//	zoomedPenWidth = gAppl.fLineZoomOn ? ( penf * gfZoomFactor ): penf;
//	zoomedPenWidth = pMapper->fScreenPenW(penf);
zoomedPenWidth = penf;
//double halfPW = (zoomedPenWidth/* + 1.0f*/) * 0.5f;
////if (halfPW <= 0.5 && !pDC->IsPenPatternFillEx()) halfPW = 0;	// tweak
//if (halfPW <= 0.5) halfPW = 0;	// tweak
//
//
//	if(zoomedPenWidth > 1 )
//   {
//      if(bAdjustAlign)
//      {
//         if ( inArc.fPenAlignment == kInsideLine ){
//            inArc.fZoomedHRad -= halfPW;
//            inArc.fZoomedVRad -= halfPW;
//         } else {
//            if ( inArc.fPenAlignment == kOutsideLine ){
//               inArc.fZoomedHRad += halfPW;
//               inArc.fZoomedVRad += halfPW;
//            }
//         }
//      }
//   }else{
//      halfPW = 0.0 ;
//   }


const double HRadOutside = inArc.fZoomedHRad + halfPenWidth;
const double VRadOutside = inArc.fZoomedVRad + halfPenWidth;

const double HRadInside = inArc.fZoomedHRad - halfPenWidth;
const double VRadInside = inArc.fZoomedVRad - halfPenWidth;

//	const register float cosObjectRotation = inArc.fSinCosRot.fCos;
//	const register float sinObjectRotation = inArc.fSinCosRot.fSin;

pointd positionOfLastChange;		// Position of the last time we changed from drawing to not drawing
pointd positionOfLastChangeOutside;		// Position of the last time we changed from drawing to not drawing
//bool weAreDrawing = true;	// Always start drawing
//double distanceWeNeedToCover = pMapper->NoZoomToZoom(TruncToShort(Coord((short)m_dDistanceWeNeedToCover)));
double distanceWeNeedToCover = m_dDistanceWeNeedToCover;
//short dashIndex = m_DashesIdx;

CDC* pCDC = pDC->GetCDC();
{
   //CMDFiloPoly helperPoly(zoomedPenWidth < 2 && !pDC->IsPenPatternFillEx());
   //CMDFiloPoly helperPoly(zoomedPenWidth < 2);
   m_helperPoly.setFrameIt(zoomedPenWidth < 2);
   auto & helperPoly = m_helperPoly;
   register double vx;
   register double vy;


   //if (dashCode)
   //{
   //   distanceWeNeedToCover = dashCodes[dashIndex];
   //}

   //if (dashCode && m_PixelCount == 0)
   //{


   //}
   pointd inside;
   pointd outside;
   double cosAngle;
   double sinAngle;
   bool doTest = true;



   double dArcBeg;

   double dArcEnd;

   double dXRadiusInside;
   double dYRadiusInside;
   double dXRadiusOutside;
   double dYRadiusOutside;

   if (inArc.nDirection == AD_CLOCKWISE)
   {
      
      dArcBeg = vAngle;

      dArcEnd = vEndAngle;

      while (dArcBeg < dArcEnd)
      {

         dArcBeg += 360.0;

      }

      vAngle = dArcBeg;

      dXRadiusInside = HRadOutside;

      dYRadiusInside = VRadOutside;

      dXRadiusOutside = HRadInside;

      dYRadiusOutside = VRadInside;

   }
   else
   {

      dArcBeg = vAngle;

      dArcEnd = vEndAngle;

      while (dArcEnd < dArcBeg)
      {

         dArcEnd += 360.0;

      }

      vAngle = dArcBeg;

      dXRadiusInside = HRadInside;

      dYRadiusInside = VRadInside;

      dXRadiusOutside = HRadOutside;

      dYRadiusOutside = VRadOutside;

   }

   double dAngle1 = vAngle;
   double dAngle2;

   pointd oldInside(0, 0);

   vx = dXRadiusOutside * mtCos(vAngle);
   vy = dYRadiusOutside * mtSin(vAngle);
   positionOfLastChangeOutside = inArc.arcPointWithCoords(vx, vy);
   vx = dXRadiusInside * mtCos(vAngle);
   vy = dYRadiusInside * mtSin(vAngle);

   positionOfLastChange = inArc.arcPointWithCoords(vx, vy);


   pointd firstPointInside = positionOfLastChange;
   pointd firstPointOutside = positionOfLastChangeOutside;

   dAngle1 = vAngle;

   dAngle2 = dArcEnd;

   if (dashCode)
   {

      bool bFinishDrawing = false;

      do
      {

         if (inArc.nDirection == AD_CLOCKWISE)
         {

            if (vAngle < dArcEnd)
            {

               bFinishDrawing = true;

            }

         }
         else
         {

            if (vAngle >= dArcEnd)
            {

               bFinishDrawing = true;

            }

         }

         if (vAngle >= 180.0 && vAngle < 200.0)
         {

            OutputDebugString(".");

         }

         bool bWasCurrentDashFilled = false;

         //if (dashCode)
         {

            bWasCurrentDashFilled = isCurrentDashFilled();

            while (distanceWeNeedToCover <= 0)
            {

               m_DashesIdx++;

               auto dashCode = dashCodes[m_DashesIdx];

               if (dashCode == 0xffff)
               {

                  distanceWeNeedToCover = 0;

                  m_DashesIdx = -1;

               }
               else
               {

                  distanceWeNeedToCover = dashCode;

               }

            }

            if (!bWasCurrentDashFilled && isCurrentDashFilled())
            {

               dAngle1 = vAngle;

            }

            if (isCurrentDashFilled())
            {

               dAngle2 = vAngle;

            }

         }

         cosAngle = mtCos(vAngle);
         sinAngle = mtSin(vAngle);

         vx = dXRadiusInside * cosAngle;
         vy = dYRadiusInside * sinAngle;
         inside = inArc.arcPointWithCoords(vx, vy);
         //if (zoomedPenWidth > 1 || pDC->IsPenPatternFillEx())
         if (zoomedPenWidth > 1)
         {
            vx = dXRadiusOutside * cosAngle;
            vy = dYRadiusOutside * sinAngle;
            outside = inArc.arcPointWithCoords(vx, vy);
         }
         else
         {
            outside = inside;
         }

         //if (dashCode && isCurrentDashFilled() && zoomedPenWidth >= 2)
         if (isCurrentDashFilled() && (zoomedPenWidth >= 2 || m_bUsePolyHelperIfDashed))
         {

            helperPoly.PushPoints(inside, outside, pDC);

         }

         if (
            (bWasCurrentDashFilled && !isCurrentDashFilled())
            ||
            (isCurrentDashFilled() && bFinishDrawing && bClose))
         {

            if (bClose && bFinishDrawing)
            {

               ::OutputDebugString("bClsoe && bFinishDrawing");

            }

            Gdiplus::Pen * ppen = pDC->m_PenPlus.get();

            if (ppen != nullptr && zoomedPenWidth < 2 && !m_bUsePolyHelperIfDashed)
            {

               Gdiplus::RectF r;
               r.X = -inArc.fZoomedHRad;
               r.Y = -inArc.fZoomedVRad;
               r.Width = inArc.fZoomedHRad * 2;
               r.Height = inArc.fZoomedVRad * 2;
               double dRot = inArc.fLongRot.GetDouble();
               pDC->graphics_on();
               pDC->graphics_high();
               pDC->graphics()->TranslateTransform(inArc.fh, inArc.fv);
               if (dRot != 0.)
               {
                  pDC->graphics()->RotateTransform(-dRot);
               }
               //double dA = dAngle1 * _M_PI / 180.0;
               //double dB = dAngle2 * _M_PI / 180.0;
               //double dSinA = sin(dA);
               //double dSinB = sin(dB);
               //double dCosA = cos(dA);
               //double dCosB = cos(dB);
               //double d1 = atan2(dSinA *inArc.fZoomedVRad, dCosA * inArc.fZoomedHRad);
               //double d2 = atan2(dSinB *inArc.fZoomedVRad, dCosB * inArc.fZoomedHRad);
               //double dM = d1 * 180.0 / _M_PI;
               //double dN = d2 * 180.0 / _M_PI;
               //if (dM < 0.)
               //   dM += 360.0;
               //if (dN < 0.)
               //   dN += 360.0;
               //double dSpan =  dM - dN;

               double dA = dAngle1 * _M_PI / 180.0;
               double dB = dAngle2 * _M_PI / 180.0;
               double dSinA = sin(dA);
               double dSinB = sin(dB);
               double dCosA = cos(dA);
               double dCosB = cos(dB);
               double d1 = atan2(dSinA * inArc.fZoomedVRad, dCosA * inArc.fZoomedHRad);
               double d2 = atan2(dSinB * inArc.fZoomedVRad, dCosB * inArc.fZoomedHRad);

               double angle1 = d1 * 180.0 / _M_PI;
               double angle2 = d2 * 180.0 / _M_PI;

               double dAngle1Gdiplus = angle_to_gdiplus(angle1);
               double dAngle2Gdiplus = angle_to_gdiplus(angle2);

               if (inArc.nDirection == AD_CLOCKWISE)
               {

                  while (dAngle2Gdiplus < dAngle1Gdiplus)
                  {

                     dAngle2Gdiplus += 360.0;

                  }

               }
               else
               {

                  while (dAngle1Gdiplus < dAngle2Gdiplus)
                  {

                     dAngle1Gdiplus += 360.0;

                  }

               }

               vx = dXRadiusInside * mtCos(dAngle1);
               vy = dYRadiusInside * mtSin(dAngle2);


               pDC->graphics()->DrawArc(ppen, r, dAngle1Gdiplus, dAngle2Gdiplus - dAngle1Gdiplus);
               //               pDC->graphics()->DrawArc(ppen, r, dM, dSpan);
               if (dRot != 0.)
               {
                  pDC->graphics()->RotateTransform(dRot);
               }
               pDC->graphics()->TranslateTransform(-inArc.fh, -inArc.fv);
            }
            else
            {

   
               if (zoomedPenWidth < 2)
               {

                  helperPoly.DrawLineNow(pDC);
                  helperPoly.Reset();

               }
               else
               {

                  helperPoly.DrawNow(pDC);
                  helperPoly.Reset();

               }

            }

            

         }

         if (dashCode)
         {

            auto deltaDistance = Pythag(inside - positionOfLastChange);

            distanceWeNeedToCover -= deltaDistance;

         }


         //if (bWasCurrentDashFilled && !isCurrentDashFilled())
         //{
         //
         //   //if (zoomedPenWidth <= 1 && !pDC->IsPenPatternFillEx())
         //   if (zoomedPenWidth <= 1)
         //   {

         //      outside = inside;

         //   }

         //   helperPoly.PushHeadPoint(inside, pDC);

         //   helperPoly.PushTailPoint(outside, pDC);

         //}

         positionOfLastChange = inside;

         positionOfLastChangeOutside = outside;

         if (inArc.nDirection == AD_CLOCKWISE)
         {

            vAngle -= vIncr;

         }
         else
         {

            vAngle += vIncr;

         }


      }  while (!bFinishDrawing);


   }

   //helperPoly.DrawNow(pDC);
   //helperPoly.Reset();

   if((!pDC->m_DrawPrim.m_bUsePolyHelperIfDashed || pDC->m_bFinalizeDash || zoomedPenWidth <= 1) && !dashCode)
   {

      Gdiplus::Pen * ppen = pDC->m_PenPlus.get();

      if (ppen != nullptr && !dashCode && !m_bUsePolyHelperIfDashed)
      {

         Gdiplus::RectF r;
         r.X = -inArc.fZoomedHRad;
         r.Y = -inArc.fZoomedVRad;
         r.Width = inArc.fZoomedHRad * 2;
         r.Height = inArc.fZoomedVRad * 2;
         double dRot = inArc.fLongRot.GetDouble();
         pDC->graphics_on();
         pDC->graphics_high();
         pDC->graphics()->TranslateTransform(inArc.fh, inArc.fv);
         if (dRot != 0.)
         {
            pDC->graphics()->RotateTransform(-dRot);
         }
         //double dA = dAngle1 * _M_PI / 180.0;
         //double dB = dAngle2 * _M_PI / 180.0;
         //double dSinA = sin(dA);
         //double dSinB = sin(dB);
         //double dCosA = cos(dA);
         //double dCosB = cos(dB);
         //double d1 = atan2(dSinA *inArc.fZoomedVRad, dCosA * inArc.fZoomedHRad);
         //double d2 = atan2(dSinB *inArc.fZoomedVRad, dCosB * inArc.fZoomedHRad);
         //double dM = d1 * 180.0 / _M_PI;
         //double dN = d2 * 180.0 / _M_PI;
         //if (dM < 0.)
         //   dM += 360.0;
         //if (dN < 0.)
         //   dN += 360.0;
         //double dSpan =  dM - dN;

         double dA = dAngle1 * _M_PI / 180.0;
         double dB = dAngle2 * _M_PI / 180.0;
         double dSinA = sin(dA);
         double dSinB = sin(dB);
         double dCosA = cos(dA);
         double dCosB = cos(dB);
         double d1 = atan2(dSinA * inArc.fZoomedVRad, dCosA * inArc.fZoomedHRad);
         double d2 = atan2(dSinB * inArc.fZoomedVRad, dCosB * inArc.fZoomedHRad);

         double angle1 = d1 * 180.0 / _M_PI;
         double angle2 = d2 * 180.0 / _M_PI;

         double dAngle1Gdiplus = angle_to_gdiplus(angle1);
         double dAngle2Gdiplus = angle_to_gdiplus(angle2);

         if (inArc.nDirection == AD_CLOCKWISE)
         {

            while (dAngle2Gdiplus < dAngle1Gdiplus)
            {

               dAngle2Gdiplus += 360.0;

            }

         }
         else
         {

            while (dAngle1Gdiplus < dAngle2Gdiplus)
            {

               dAngle1Gdiplus += 360.0;

            }

         }

         //vx = dXRadiusInside * cos(d1);
         //vy = dYRadiusInside * sin(d1);
#ifdef _DEBUG
         //if (m_bPrev1)
         //{

         //   //return;

         //}
         //else
#endif
         if (tolerance_equal(dAngle1Gdiplus, 0.0, 0.0001)
            && tolerance_equal(dAngle2Gdiplus, 0.0, 0.0001))
         {
            pDC->graphics()->DrawEllipse(ppen, r);

         }
         else
         {
            //pDC->graphics()->DrawLine(ppen,(float)vx,(float) vy,(float) vx, (float)vy);
            pDC->graphics()->DrawArc(ppen, r, dAngle1Gdiplus, dAngle2Gdiplus - dAngle1Gdiplus);
         }
         //pDC->graphics()->DrawArc(ppen, r, dM, dSpan);
         if (dRot != 0.)
         {
            pDC->graphics()->RotateTransform(dRot);
         }
         pDC->graphics()->TranslateTransform(-inArc.fh, -inArc.fv);

      }
      else
      {

         helperPoly.DrawNow(pDC);
         helperPoly.Reset();

      }

   }


#ifdef DEBUG
   if (m_bPrev1)
   {

      OutputDebugString("test");

   }
#endif

   //if (!m_bPrev1)
   {

      if (penModel->isPenStyleRound() && !penModel->Dashing()) {
         drawFilledHalfCircle(pDC, inArc.lptStart, firstPointInside, firstPointOutside);
         drawFilledHalfCircle(pDC, inArc.lptEnd, positionOfLastChangeOutside, positionOfLastChange);
      }

      if (!penModel->Dashing()) {
         if (inArc.nDirection == AD_CLOCKWISE) {
            pointd pts[2] = { positionOfLastChangeOutside, positionOfLastChange };
            pDC->CloseJoint(inArc.lptEnd, pts);
            pts[0] = firstPointInside;
            pts[1] = firstPointOutside;
            pDC->OpenJoint(inArc.lptStart, pts);
         }
         else {
            pointd pts[2] = { firstPointOutside, firstPointInside };
            pDC->CloseJoint(inArc.lptStart, pts);
            pts[0] = positionOfLastChange;
            pts[1] = positionOfLastChangeOutside;
            pDC->OpenJoint(inArc.lptEnd, pts);
         }
      }


   }
   //m_CurrentPt.x = (positionOfLastChange.x + positionOfLastChangeOutside.x) / 2;
   //m_CurrentPt.y = (positionOfLastChange.y + positionOfLastChangeOutside.y) / 2;

}

if (inArc.fIsClosed)
{
   pointd arcPt;
   arcPoint(inArc, inArc.fEndAng, &arcPt);
   pCDC->MoveTo(int(arcPt.x - zoomedPenWidth), int(arcPt.y - zoomedPenWidth));
   dashLineTo(pDC, pointd(int(inArc.fh - zoomedPenWidth),
      int(inArc.fv - zoomedPenWidth)));
   arcPoint(inArc, inArc.fStartAng, &arcPt);
   dashLineTo(pDC, pointd(int(arcPt.x - zoomedPenWidth), int(arcPt.y - zoomedPenWidth)));
}
pMapper->LPtoDP(m_CurrentPt, inArc.lptEnd);
//m_PixelCount = pMapper->ZoomToNoZoom((int)distanceWeNeedToCover);
m_dDistanceWeNeedToCover = distanceWeNeedToCover;

}
		

void arcSlow ( CDrawPrim* inPrim, CDraftContext* pDC, CArcStruct& inArc, bool bAdjustAlign){

	if ( bAdjustAlign && inArc.fPenW > 0 )
	{
		inArc.fPenW >>= 1;

		if(inArc.fPenAlignment==kInsideLine)
		{
			inArc.fZoomedHRad-=inArc.fPenW;
			inArc.fZoomedVRad-=inArc.fPenW;
		}
		else if(inArc.fPenAlignment==kOutsideLine)
		{
			inArc.fZoomedHRad+=inArc.fPenW;
			inArc.fZoomedVRad+=inArc.fPenW;
		}
	}

	register Angle vIncr;
	/* Find an increment for the angle. */
	vIncr = WrFixedFromRawDoubleTruncated ( kStrokeLen / (inArc.fZoomedHRad + inArc.fZoomedVRad + 1.0) ) ;
	if ( vIncr < short ( 4 ) )	vIncr = short ( 4 ) ;
	if ( !inArc.fIsFine  &&  vIncr < short ( 20 ) )	vIncr = short ( 20 ) ;
	if ( vIncr > Abs(inArc.fEndAng-inArc.fStartAng) )
		vIncr = Abs(inArc.fEndAng - inArc.fStartAng) / short ( 5 ) ;

	CMapper* pMapper=pDC->GetMapper();
	const PenModel* inPenModelPtr=&pDC->GetPen(); 
	
	
	/* Get the starting point of the arc */
   pointd arcPt;
	if( inPenModelPtr->m_nAlignment == kCenterLine)
	{
		pMapper->LPtoDP(arcPt, inArc.lptStart);
	}
	else
	{
		arcPoint ( inArc, inArc.fStartAng, &arcPt);
	}

	CDC* pCDC=pDC->GetCDC();
	if(pCDC->GetCurrentPosition()!= pointd(arcPt.x, arcPt.y)) //(inArc.lptStart==inArc.lptEnd)
	{
//		if(pDC->IsCreatingPath()) pCDC->LineTo(arcPt.x,arcPt.y);
//		else
			pCDC->MoveTo((LONG) arcPt.x,(LONG)arcPt.y);
	} 

	/* Get the points along the arc. */
   pointd theStartPt;
	register Angle vAngle;
	if(inArc.nDirection==AD_CLOCKWISE)
	{
		vAngle = inArc.fStartAng - vIncr;
		while ( vAngle > inArc.fEndAng)	{
			theStartPt = arcPt;
			arcPoint ( inArc, vAngle, &arcPt);
			inPrim->dashLine ( pDC, theStartPt, arcPt );
			vAngle -= vIncr;
		}
	}
	else
	{
		vAngle = inArc.fStartAng + vIncr;
		while ( vAngle < inArc.fEndAng)	{
			theStartPt = arcPt;
			arcPoint ( inArc, vAngle, &arcPt);
			inPrim->dashLine ( pDC, theStartPt, arcPt );
			vAngle += vIncr;
		}
	} 

	/* Last point. */
	if( inPenModelPtr->m_nAlignment == kCenterLine)
	{
		 pMapper->LPtoDP(theStartPt, inArc.lptEnd);
	}
	else
	{
		arcPoint ( inArc, inArc.fEndAng, &theStartPt);
	}

	inPrim->dashLine ( pDC, arcPt, theStartPt );

	if ( inArc.fIsClosed )	{
		arcPt = pointd( inArc.fh, inArc.fv);
		inPrim->dashLine ( pDC, theStartPt, arcPt );
		theStartPt = arcPt;

		arcPoint ( inArc, inArc.fStartAng, &arcPt);
		inPrim->dashLine ( pDC, theStartPt, arcPt );
	}

}



/* Find a point on sArc at the angle pAngle. */
void arcPoint (
	const CArcStruct& inArc,
	register Angle		angle,
	register pointd		*arcPt)
{
	if ( inArc.fLongRot != FixedZero )	{
		double vx, vy;
		vx = inArc.fZoomedHRad * mtCos ( angle );
		vy = inArc.fZoomedVRad * mtSin ( angle );
		arcPt->x = inArc.fh + mtRealToShort (inArc.fSinCosRot.fCos * vx - inArc.fSinCosRot.fSin * vy);
		arcPt->y = inArc.fv + mtRealToShort (inArc.fSinCosRot.fSin * vx + inArc.fSinCosRot.fCos * vy);
	}
	else	{
		arcPt->x = inArc.fh + mtRealToShort (inArc.fZoomedHRad * mtCos ( angle ) );
		arcPt->y = inArc.fv + mtRealToShort (inArc.fZoomedVRad * mtSin ( angle ) );
	}
}/* arcPoint */

#define	kMaxCtrl	4			/* maximum control points						*/
#define	kSegLen		10			/* segment length (15:very smooth-25:ok)	*/
#define	kMinSeg		16			/* minimum segments per curve					*/
#define	kMaxSeg		128		/* maximum segments per fast curve			*/
#define	kScrLimit	LONG_MAX	/* screen limit									*/

/*---------------------------------------------------------------------------------*/
// Formally in DrawCurvePr.h. Had to combine these files when removed the xxxPr.h files. Matt

unsigned short cBHdl[] =
{
/*   0 */    65535,      0,      0,      0,
/*   1 */    63999,   1523,     12,      0,
/*   2 */    62487,   2999,     47,      0,
/*   3 */    60999,   4427,    107,      0,
/*   4 */    59535,   5808,    188,      2,
/*   5 */    58095,   7142,    292,      3,
/*   6 */    56678,   8431,    418,      6,
/*   7 */    55284,   9674,    564,     10,
/*   8 */    53914,  10873,    730,     16,
/*   9 */    52566,  12027,    917,     23,
/*  10 */    51241,  13138,   1122,     31,
/*  11 */    49938,  14206,   1347,     42,
/*  12 */    48658,  15232,   1589,     55,
/*  13 */    47399,  16215,   1849,     70,
/*  14 */    46163,  17158,   2125,     87,
/*  15 */    44948,  18059,   2418,    107,
/*  16 */    43755,  18921,   2727,    131,
/*  17 */    42583,  19743,   3051,    157,
/*  18 */    41432,  20526,   3389,    186,
/*  19 */    40302,  21270,   3742,    219,
/*  20 */    39193,  21977,   4107,    255,
/*  21 */    38104,  22647,   4486,    296,
/*  22 */    37036,  23280,   4877,    340,
/*  23 */    35988,  23876,   5280,    389,
/*  24 */    34960,  24438,   5694,    442,
/*  25 */    33951,  24964,   6118,    499,
/*  26 */    32962,  25456,   6553,    562,
/*  27 */    31993,  25914,   6996,    629,
/*  28 */    31043,  26339,   7449,    702,
/*  29 */    30112,  26732,   7910,    780,
/*  30 */    29199,  27092,   8379,    863,
/*  31 */    28305,  27421,   8854,    953,
/*  32 */    27430,  27719,   9336,   1048,
/*  33 */    26573,  27986,   9825,   1149,
/*  34 */    25734,  28224,  10318,   1257,
/*  35 */    24912,  28433,  10817,   1371,
/*  36 */    24109,  28613,  11319,   1492,
/*  37 */    23323,  28765,  11825,   1620,
/*  38 */    22554,  28889,  12335,   1755,
/*  39 */    21802,  28987,  12846,   1897,
/*  40 */    21067,  29059,  13360,   2047,
/*  41 */    20349,  29104,  13875,   2205,
/*  42 */    19648,  29125,  14391,   2370,
/*  43 */    18962,  29121,  14907,   2543,
/*  44 */    18293,  29093,  15422,   2725,
/*  45 */    17640,  29041,  15937,   2915,
/*  46 */    17002,  28967,  16450,   3114,
/*  47 */    16380,  28870,  16961,   3321,
/*  48 */    15774,  28752,  17470,   3538,
/*  49 */    15182,  28613,  17975,   3764,
/*  50 */    14606,  28453,  18476,   3999,
/*  51 */    14044,  28273,  18973,   4243,
/*  52 */    13497,  28074,  19464,   4498,
/*  53 */    12964,  27856,  19951,   4763,
/*  54 */    12446,  27619,  20431,   5037,
/*  55 */    11941,  27365,  20904,   5322,
/*  56 */    11450,  27094,  21370,   5618,
/*  57 */    10973,  26807,  21828,   5924,
/*  58 */    10510,  26503,  22278,   6242,
/*  59 */    10059,  26185,  22719,   6570,
/*  60 */     9622,  25851,  23150,   6910,
/*  61 */     9198,  25503,  23571,   7261,
/*  62 */     8786,  25142,  23981,   7624,
/*  63 */     8386,  24767,  24380,   7999,
/*  64 */     7999,  24380,  24767,   8386,
/*  65 */     7624,  23981,  25142,   8786,
/*  66 */     7261,  23571,  25503,   9198,
/*  67 */     6910,  23150,  25851,   9622,
/*  68 */     6570,  22719,  26185,  10059,
/*  69 */     6242,  22278,  26503,  10510,
/*  70 */     5924,  21828,  26807,  10973,
/*  71 */     5618,  21370,  27094,  11450,
/*  72 */     5322,  20904,  27365,  11941,
/*  73 */     5037,  20431,  27619,  12446,
/*  74 */     4763,  19951,  27856,  12964,
/*  75 */     4498,  19464,  28074,  13497,
/*  76 */     4243,  18973,  28273,  14044,
/*  77 */     3999,  18476,  28453,  14606,
/*  78 */     3764,  17975,  28613,  15182,
/*  79 */     3538,  17470,  28752,  15774,
/*  80 */     3321,  16961,  28870,  16380,
/*  81 */     3114,  16450,  28967,  17002,
/*  82 */     2915,  15937,  29041,  17640,
/*  83 */     2725,  15422,  29093,  18293,
/*  84 */     2543,  14907,  29121,  18962,
/*  85 */     2370,  14391,  29125,  19648,
/*  86 */     2205,  13875,  29104,  20349,
/*  87 */     2047,  13360,  29059,  21067,
/*  88 */     1897,  12846,  28987,  21802,
/*  89 */     1755,  12335,  28889,  22554,
/*  90 */     1620,  11825,  28765,  23323,
/*  91 */     1492,  11319,  28613,  24109,
/*  92 */     1371,  10817,  28433,  24912,
/*  93 */     1257,  10318,  28224,  25734,
/*  94 */     1149,   9825,  27986,  26573,
/*  95 */     1048,   9336,  27719,  27430,
/*  96 */      953,   8854,  27421,  28305,
/*  97 */      863,   8379,  27092,  29199,
/*  98 */      780,   7910,  26732,  30112,
/*  99 */      702,   7449,  26339,  31043,
/* 100 */      629,   6996,  25914,  31993,
/* 101 */      562,   6553,  25456,  32962,
/* 102 */      499,   6118,  24964,  33951,
/* 103 */      442,   5694,  24438,  34960,
/* 104 */      389,   5280,  23876,  35988,
/* 105 */      340,   4877,  23280,  37036,
/* 106 */      296,   4486,  22647,  38104,
/* 107 */      255,   4107,  21977,  39193,
/* 108 */      219,   3742,  21270,  40302,
/* 109 */      186,   3389,  20526,  41432,
/* 110 */      157,   3051,  19743,  42583,
/* 111 */      131,   2727,  18921,  43755,
/* 112 */      107,   2418,  18059,  44948,
/* 113 */       87,   2125,  17158,  46163,
/* 114 */       70,   1849,  16215,  47399,
/* 115 */       55,   1589,  15232,  48658,
/* 116 */       42,   1347,  14206,  49938,
/* 117 */       31,   1122,  13138,  51241,
/* 118 */       23,    917,  12027,  52566,
/* 119 */       16,    730,  10873,  53914,
/* 120 */       10,    564,   9674,  55284,
/* 121 */        6,    418,   8431,  56678,
/* 122 */        3,    292,   7142,  58095,
/* 123 */        2,    188,   5808,  59535,
/* 124 */        0,    107,   4427,  60999,
/* 125 */        0,     47,   2999,  62487,
/* 126 */        0,     12,   1523,  63999,
/* 127 */        0,      0,      0,  65534
};


bool	IsStraight (pointd pBezier[4], register pointd *p1, register pointd *p2);
void	bezierDrawFast ( CDrawPrim*  pDrawPrim, CDraftContext* pDC, pointd pBezier[4], short cPtCount);
bool	bezierCheckFast (pointd pBezier[4], short cPtCount, pointd testPt, short penSize);
bool	bezierSlow (pointd	/*pBezier*/[4],	unsigned short /*what*/);

void CDrawPrim::drawBezier (CDraftContext* pDC, pointd* vBezier)
{
	register int	vMinx, vMiny, vMaxx, vMaxy;
//	pointd	vBezier[4];
	register int	i;

//	vBezier[0]=pPoint1;
//	vBezier[1]=pPoint2;
//	vBezier[2]=pPoint3;
//	vBezier[3]=pPoint4;
	vMinx = vMaxx = vBezier[0].x;
	vMiny = vMaxy = vBezier[0].y;


	for ( i = 1; i < kMaxCtrl; i++ ){
		if ( vBezier[i].x < vMinx )
			vMinx = vBezier[i].x;
		else if ( vBezier[i].x > vMaxx )
				vMaxx = vBezier[i].x;
		if ( vBezier[i].y < vMiny )
			vMiny = vBezier[i].y;
		else if ( vBezier[i].y > vMaxy )
				vMaxy = vBezier[i].y;
	}


	/* fast draw (dot only) if it is too small */
	//if ( ((vMaxx-vMinx) < 2) && ((vMaxy-vMiny) < 2) ){
	//	dashLine( pDC, pointd(vMinx, vMiny), pointd(vMinx, vMiny) );
	//	return;
	//}
   if(((vMaxx - vMinx) < 1) && ((vMaxy - vMiny) < 1)){
      dashLine(pDC, pointd(vMinx,vMiny), pointd(vMinx,vMiny));
      return;
   }

	/* forming a straight line */
	{
      pointd	tempP1,tempP2;
		if (IsStraight (vBezier,&tempP1,&tempP2))
		{
			dashLine( pDC, tempP1, tempP2);
			return;
		}
	}

	short cPtCount = ((vMaxx-vMinx) + (vMaxy-vMiny)) / kSegLen;
	if ( cPtCount < kMinSeg ) cPtCount = kMinSeg;
	if ( cPtCount > kMaxSeg ) cPtCount = kMaxSeg;

	if 	((vMinx > -kScrLimit) && (vMaxx < kScrLimit) &&
		 (vMiny > -kScrLimit) && (vMaxy < kScrLimit) )
	{
		bezierDrawFast ( this, pDC, vBezier, cPtCount);
	}
//	else return(bezierSlow (vBezier,whatToDo));
}/* genBezier */

bool CDrawPrim::checkBezier (const pointd& pPoint1,const  pointd& pPoint2, const  pointd &pPoint3,const  pointd& pPoint4,const  pointd& testPt, short penSize)
{
	register int	vMinx, vMiny, vMaxx, vMaxy;
   pointd	vBezier[4];
	register int	i;

	vBezier[0]=pPoint1;
	vBezier[1]=pPoint2;
	vBezier[2]=pPoint3;
	vBezier[3]=pPoint4;
	vMinx = vMaxx = vBezier[0].x;
	vMiny = vMaxy = vBezier[0].y;


	for ( i = 1; i < kMaxCtrl; i++ ){
		if ( vBezier[i].x < vMinx )
			vMinx = vBezier[i].x;
		else if ( vBezier[i].x > vMaxx )
				vMaxx = vBezier[i].x;
		if ( vBezier[i].y < vMiny )
			vMiny = vBezier[i].y;
		else if ( vBezier[i].y > vMaxy )
				vMaxy = vBezier[i].y;
	}


	/* fast draw (dot only) if it is too small */
	if ( ((vMaxx-vMinx) < 2) && ((vMaxy-vMiny) < 2) ){
		return false;
	}

	/* forming a straight line */
	{
		pointd	tempP1,tempP2;

		if (IsStraight (vBezier,&tempP1,&tempP2))
			return(guPtAtLine(testPt, tempP1, tempP2,penSize,kCenterLine));
	}

	short cPtCount = ((vMaxx-vMinx) + (vMaxy-vMiny)) / kSegLen;
	if ( cPtCount < kMinSeg ) cPtCount = kMinSeg;
	if ( cPtCount > kMaxSeg ) cPtCount = kMaxSeg;

	if 	((vMinx > -kScrLimit) && (vMaxx < kScrLimit) &&
		 (vMiny > -kScrLimit) && (vMaxy < kScrLimit) )
	{
			return(bezierCheckFast( vBezier,cPtCount,testPt,penSize));
	}
//	else return(bezierSlow (vBezier,whatToDo));
	return false;
}/* checkBezier */

/*----------------------------------------------------------------------*/

bool IsStraight (
	pointd			pBezier[4],	/* Bezier control points							*/
	register pointd	*p1,
	register pointd	*p2)
{
	/* forming a straight line */
	if  (pBezier[0] == pBezier[1] &&
		 pBezier[2] == pBezier[3] )
	{
		*p1 = pBezier[0];
		*p2 = pBezier[2];
		return ( true );
	}
	if ( pBezier[0] == pBezier[2] &&
		 pBezier[1] == pBezier[3] )
	{
		*p1 = pBezier[0];
		*p2 = pBezier[1];
		return ( true );
	}

	return ( false );
}/* IsStraight */
/*----------------------------------------------------------------------*/


void bezierDrawFast (
	CDrawPrim*  pDrawPrim, CDraftContext* pDC, 
   pointd		pBezier[4],		/* Bezier control points	*/
	short		cPtCount)
{
	register unsigned short	*vBPtr;
	register short	i,vCount,vIncr;
	pointd	p1,p2;
	double	myLong;

	vBPtr = cBHdl;/* beginning of blending functions */
	vIncr =  kMaxSeg / cPtCount;
	vCount = vIncr * kMaxCtrl;

	/* generate points along the bezier curve */
	p1 = pBezier[0];	/* first point */
//	pDC->MoveTo ( p1.x, p1.y); // temp fix, moved to dashLine

	for ( i = 1 + vIncr, vBPtr = vBPtr + kMaxCtrl + vCount;
	      i < kMaxSeg;
	      i = i + 1 + vIncr, vBPtr += vCount )
	{
		myLong = pBezier[0].x * (*vBPtr++);
		myLong = myLong + pBezier[1].x * (*vBPtr++);
		myLong = myLong + pBezier[2].x * (*vBPtr++);
		myLong = myLong + pBezier[3].x * (*vBPtr++);
		p2.x = myLong / 65536.0;
		vBPtr -= kMaxCtrl;
		myLong = pBezier[0].y * (*vBPtr++);
		myLong = myLong + pBezier[1].y * (*vBPtr++);
		myLong = myLong + pBezier[2].y * (*vBPtr++);
		myLong = myLong + pBezier[3].y * (*vBPtr++);
		p2.y = myLong / 65536.0;

      double dx = p1.x - p2.x;
      double dy = p1.y - p2.y;
      double distanceSquare = dx * dx + dy * dy;
      if(distanceSquare >= 1.0)
      {
         pDrawPrim->dashLine(pDC,p1,p2);
	   	p1 = p2;
      }
   }

	p2 = pBezier[3];	/* first point */

	//pDrawPrim->dashLine( pDC, p1, p2 ); // 2021-02-25

}/* BezierDrawFast */

/*----------------------------------------------------------------------*/

bool	bezierCheckFast (
	pointd		pBezier[4],		/* Bezier control points	*/
	short		cPtCount,
	pointd		testPt,
	short		penSize)
{
   register unsigned short	*vBPtr;
   register short	i, vCount, vIncr;
   pointd	p1, p2;
   double	myLong;

   vBPtr = cBHdl;/* beginning of blending functions */
   vIncr = kMaxSeg / cPtCount;
   vCount = vIncr * kMaxCtrl;

   /* generate points along the bezier curve */
   p1 = pBezier[0];	/* first point */
//	pDC->MoveTo ( p1.x, p1.y); // temp fix, moved to dashLine
   bool success = 0;

   for (i = 1 + vIncr, vBPtr = vBPtr + kMaxCtrl + vCount;
      i < kMaxSeg;
      i = i + 1 + vIncr, vBPtr += vCount)
   {

      myLong = pBezier[0].x * (*vBPtr++);
      myLong = myLong + pBezier[1].x * (*vBPtr++);
      myLong = myLong + pBezier[2].x * (*vBPtr++);
      myLong = myLong + pBezier[3].x * (*vBPtr++);
      p2.x = myLong / 65536.0;
      vBPtr -= kMaxCtrl;
      myLong = pBezier[0].y * (*vBPtr++);
      myLong = myLong + pBezier[1].y * (*vBPtr++);
      myLong = myLong + pBezier[2].y * (*vBPtr++);
      myLong = myLong + pBezier[3].y * (*vBPtr++);
      p2.y = myLong / 65536.0;

      double dx = p1.x - p2.x;
      double dy = p1.y - p2.y;
      double distanceSquare = dx * dx + dy * dy;
      if (distanceSquare >= 1.0)
      {
         success = guPtAtLine(testPt, p1, p2, penSize, kCenterLine);
      //   pDrawPrim->dashLine(pDC, p1, p2);
         if (success)
         {
            break;
         }
         p1 = p2;
      }
   }
	if(!success){
		p2 = pBezier[3];	/* first point */
		success = guPtAtLine(testPt,p1,p2,penSize,kCenterLine);
	}
	return success;
}/* BezierCheckFast */

/*----------------------------------------------------------------------*/

bool	bezierSlow (
	pointd		/*pBezier*/[4],			/* curve control points	*/
	unsigned short		/*what*/)
{
	return true;
}/* BezierSlow */

void CDrawPrim::drawFilledHalfCircle(CDraftContext* pDC, const LongPoint& lpt, const pointd& ptStart, const pointd& ptEnd)
{
	CMapper* pMapper = pDC->GetMapper();
	const PenModel* penModel = &pDC->GetPen();
	WrFixed halfWidth = pMapper->PenW(penModel->m_nWidth)/2;
			
	CRect boundRect;
			
	LongPoint pt1(lpt.x - halfWidth, lpt.y-halfWidth);
	LongPoint pt2(lpt.x + halfWidth, lpt.y+ halfWidth);
		
	boundRect.SetRect(pMapper->LPtoDP(pt1), pMapper->LPtoDP(pt2));
	CDC* pCDC = pDC->GetCDC();

	if(pCDC->GetCurrentPosition()!=ptEnd){
		pCDC->MoveTo(__point(ptEnd));
	}
	/*pCDC->BeginPath(); 

	pCDC->LineTo(__point(ptStart)); 
	pCDC->ArcTo( &boundRect, __point(ptStart), __point(ptEnd));
	pCDC->EndPath();*/

   pDC->path_new(false);
   pointf pA = __pointf(ptEnd);
   pointf pB = __pointf(ptStart);
   pointf pCenter;
   pCenter.X =(pA.X+pB.X) /2.0f;
   pCenter.Y = (pA.Y + pB.Y) / 2.0f;
   pDC->m_ppath->AddLine(pA, pB);
   FLOAT dX = pA.X - pCenter.X;
   FLOAT dY = pA.Y - pCenter.Y;
   float dStartAngle = atan2(dX, dY);
   Gdiplus::RectF r(boundRect.left, boundRect.top, boundRect.Width(), boundRect.Height());
   pDC->m_ppath->AddArc(r, dStartAngle, 180.0);

   pDC->PaintPenPath();

	//CRgn rgn;
	//if(rgn.CreateFromPath(pCDC)){
	//	CPoint ptOrg;
	//	GetViewportOrgEx(pCDC->GetSafeHdc(),&ptOrg);
	//	rgn.OffsetRgn(-ptOrg);	// temporary fix, should not be here

 //     pDC->PaintPenPath();
	//	//pDC->PaintPenRgn(&rgn);
	//}

}
