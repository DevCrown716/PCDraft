#include "stdafx.h"
#include "resource.h"
#include "MDLineShape.h"
#include "MDPolygonShape.h"

#include "gu.h"
#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "Edge.h"
#include "dxfutil.h"
#include "DrawPrim.h"
#include "PCDraftView.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDLineShape,CMDShape,1)

//////////////////////////////////////////////////////////////////////

CMDLineShape::CMDLineShape(const LongPoint& lptFrom,const LongPoint& lptTo,int nConAngle)
{
	SetShapePoint(0,lptFrom);
	SetShapePoint(1,lptTo);
	m_nConAngle=nConAngle;
}

void CMDLineShape::Serialize(CArchive& ar)
{
	CMDShape::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_pt[0] << m_pt[1] << m_nConAngle;
	}
	else 
	{
		ar >> m_pt[0] >> m_pt[1] >> m_nConAngle;
	}
}

LongPoint CMDLineShape::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<2);
	return m_pt[nIndex];
}

void CMDLineShape::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<2);
	m_pt[nIndex]=lpt;
}

CEdge* CMDLineShape::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex==0);
	return new CEdge(GetShapePoint(0),GetShapePoint(1));
}

void CMDLineShape::DrawShape(CDraftContext* pDC) const
{
	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDLineShape::DrawShapeOutline(CDraftContext* pDC) const
{
	
   pDC->Line(GetShapePoint(0),GetShapePoint(1),-1,-1,kBothEnd);
   
   pDC->path_destroy(true);

}

bool CMDLineShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	pointd pt1=pMapper->LPtoDPF(GetShapePoint(0));
	pointd pt2=pMapper->LPtoDPF(GetShapePoint(1));
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	return guPtAtLine(pt,pt1,pt2,nWidth,kCenterLine);
}

CTrackContext* CMDLineShape::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CLineTrackContext* p=new CLineTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	for(int i=0;i<2;i++)
		p->pt[i]=GetShapePoint(i);
	if(nWhichHandle==kRotateHandle)
	{
		CRotateContext* pCtx=(CRotateContext*)lParam;
		LongPoint lptRot;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			lptRot=Middle(GetExtents());
			break;
		case kAboutDatum:
			lptRot=GetShapeDatum();
			break;
		case kAboutRefPt:
			lptRot=pCtx->m_ptOrigin;
			break;
		default:
			ASSERT(0);
		}
		for(int i=0;i<2;i++)
			p->pt[i]=mtRotPoint(p->pt[i], pCtx->m_Angle, lptRot);
	}
	return p;
}

void CMDLineShape::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CLineTrackContext* p=(CLineTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle==kMoveHandle)
	{
		p->pt[0]+=pt;
		p->pt[1]+=pt;
	}
	else
	{
		LongPoint pt0,pt1,vDelta;
		pt0=p->pt[!nHandle];
		pt1=p->pt[nHandle];
		vDelta=pt-pt1;
		if(m_nConAngle)
		{
			Angle vAngle=mtFixAngle(-mtAngle(pt0,pt1));
			LongPoint tempDelta=mtRotPoint(vDelta,vAngle);
			tempDelta.y=FixedZero;
			vDelta=mtRotPoint(tempDelta,mtFixAngle(-vAngle));
		}
		pt=pt1+vDelta;
		p->pt[nHandle]=pt;
	}
}

void CMDLineShape::TrackShowSize(CTrackContext* pTrackContext)
{
	CLineTrackContext* p=(CLineTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	p->pView->SetShowSize(0,1,Pythag(p->pt[1]-p->pt[0]));
	p->pView->SetShowSize(1,-1,mtAngle(p->pt[!nHandle],p->pt[nHandle]));
}

void CMDLineShape::TrackComplete(CTrackContext* pTrackContext)
{
	CLineTrackContext* p=(CLineTrackContext*)pTrackContext;
	for(int i=0;i<2;i++)
		SetShapePoint(i,p->pt[i]);
	CalculateExtents();
}

void CMDLineShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CLineTrackContext* p=(CLineTrackContext*)pTrackContext;
	pDC->MoveTo(p->pt[0]);
	pDC->LineTo(p->pt[1]);
}

double CMDLineShape::ReportCalcLength(const LongRatio& ratio) const
{
	return ScaleDist(ratio,Pythag(GetShapePoint(1)-GetShapePoint(0)));
}

const ResizeInfo* CMDLineShape::GetResizeInfo() const
{
static ResizeInfo lineResize =
{
	IDS_LINE, 2, {IDS_LENGTH,kUnitlessValue, IDS_ANGLE,kAngleValue}
};
	return &lineResize;
}

void CMDLineShape::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDShape::GetResizeValue(pValues,nHandle);
	pValues[0].m_Unitless=Pythag(GetShapePoint(0)-GetShapePoint(1));
	pValues[1].m_Angle=mtAngle(GetShapePoint(0),GetShapePoint(1));
}

void CMDLineShape::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	if(nHandle<0 || nHandle>1) nHandle=1;
	LongPoint lpt=GetShapePoint(1-nHandle);
	if(nHandle)	lpt.x+=pValues[0].m_Unitless;
	else lpt.x-=pValues[0].m_Unitless;
	if(mtAngle(GetShapePoint(0),GetShapePoint(1))!=pValues[1].m_Angle) m_nConAngle=0; // unconstrain if angle changed
	SetShapePoint(nHandle,mtRotPoint(lpt,pValues[1].m_Angle,GetShapePoint(1-nHandle)));
	CMDShape::SetResizeValue(pValues);
}

void CMDLineShape::WriteDXF(CFile*			pFile,
							CString&		pLayerStr,
							LongRectPtr 	pDocExtPtr,
							double			vUnitFactor,
							int				nZoomFactor,
							Coord			vDisplayResValue,
							CPalette*		ptrPalette) const
{
	short		vDashIdx;
	LongPoint	p1, p2;


	PenModel capsPenModel = GetPenModel();
	PenModel aPenModel = capsPenModel ;
	short penW = DxfWidthConversion(capsPenModel.m_nWidth);
	Coord lHalfPen = _guLOrd(penW) >> 1;
	short cap1 = aPenModel.EndCap1();
	short cap2 = aPenModel.EndCap2();

	// dbPeekLPoint
	LongPoint lPt1 = GetShapePoint (0) ;
	LongPoint lPt2 = GetShapePoint (1) ;

	LongPoint trueLPt1 = lPt1;
	LongPoint trueLPt2 = lPt2;

	if (cap1 || cap2)
		DxfLineCapped (&lPt1, &lPt2, &capsPenModel, kBothEnd, &lPt1, &lPt2, nZoomFactor);

	if (aPenModel.m_nAlignment != kCenterLine)
		DxfLineBorderPos (&aPenModel, false, &lPt1, &lPt2, &lPt1, &lPt2, nZoomFactor);

	vDashIdx = GetPenModel().Dashing();

	p1 = lPt1;
	p2 = lPt2;

	if (aPenModel.Dashing())
		DxfMakeButtCaps (penW, lPt1, lPt2, &p1, &p2);

	if (! cap1)		lPt1 = p1;
	if (! cap2)		lPt2 = p2;

    DxfWriteALine (pFile, pLayerStr, &aPenModel, &lPt1, &lPt2,
			    pDocExtPtr, vUnitFactor, ptrPalette);

	if (cap1)
		DxfDrawEndcap (pFile, pLayerStr, &trueLPt1, &trueLPt2, cap1, kEnd1,
					   &capsPenModel, false, kLOrdEndmarkOutset,
					   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);

	if (cap2)
		DxfDrawEndcap (pFile, pLayerStr, &trueLPt1, &trueLPt2, cap2, kEnd2,
					  				   &capsPenModel, false, kLOrdEndmarkOutset,
					  				   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
}


bool CMDLineShape::CanAddPt(CMDShape **ppNewShape) const
{
	CMDShape* pShape=new CMDPolygonShape(CMDPolygonShape::kPolygon,2,false);
	for(int i=0;i<2;i++) pShape->SetShapePoint(i,GetShapePoint(i));
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	*ppNewShape=pShape;
	return true;
}

int CMDLineShape::SaveMacDraft(PhIOContext& ctx)
{
	PhDSLine ps;
	ps.fID=PhIDLine;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDShape::SaveMacDraft(ctx);

	ps.fConAngle=m_nConAngle;
	ps.fP1.fFlags=kPhRealVertex;
	ps.fP1.fLongPt=m_pt[0];
	ps.fP1.fUse=0;
	ps.fP2.fFlags=kPhRealVertex;
	ps.fP2.fLongPt=m_pt[1];
	ps.fP2.fUse=0;

	uptr p((char*)&ps);
	p.swapDSShape();

	p.swaps(1);
	p.skip(2); p.swaplpt(1);	// DBPoint
	p.skip(2); p.swaplpt(1);	// DBPoint

	ushort count=sizeof(PhDSLine)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSLine));
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDLineShape::OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex)
{
	int nConAngle=m_nConAngle;
	m_nConAngle=0; // unconstrain temporarily
	CMDShape::OnUpdateLinkedVertex(pView,nVertex,pCauseShape,nCauseVertex);
	m_nConAngle=nConAngle;
}

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/

CMDFiloPoly::CMDFiloPoly(){
	polyOffset.x = 0;
	polyOffset.y = 0;
   m_bP1Set = false;
	fFrameIt = true;
	Reset();
	}

CMDFiloPoly::CMDFiloPoly( bool frameIt)
	{
   m_bP1Set = false;
	polyOffset.x = 0;
	polyOffset.y = 0;
	fFrameIt = frameIt;
	Reset();
	}

CMDFiloPoly::~CMDFiloPoly ()
	{
	}

/*------------------------------------------------------------------------------*/
		
void
CMDFiloPoly::DrawNow( CDraftContext* pDC ) const
	{
	if (!(fHeadPoints.GetSize() && fTailPoints.GetSize()) ){
		return;
	}
	if(!fFrameIt){
		int size=fHeadPoints.GetSize()+fTailPoints.GetSize();
		//LPPOINT points=new POINT[size];
		//register long countTail = fTailPoints.GetSize(); 
		//for(int i=fHeadPoints.GetSize(); --i>=0; ){
		//	points[countTail+i]=__point(fHeadPoints[i]); 	
		//}
		//for(int i=countTail;--i>=0;){
		//	points[i]=__point(fTailPoints[countTail-i-1]); 	
		//}
		//CRgn polyRgn;
		//if(polyRgn.CreatePolygonRgn(points, size,ALTERNATE)){
		//	polyRgn.OffsetRgn(__point(polyOffset));
		//	pDC->PaintPenRgn( &polyRgn);

		//}
		//delete[] points;
      
      // Infinisys Ltd. GDI+

      CArray < pointf > points;

      points.SetSize(size);

		long countTail = fTailPoints.GetSize(); 

		for(int i=fHeadPoints.GetSize(); --i>=0; )
      {

			points[countTail+i]=__pointf(fHeadPoints[i]);  	

		}

		for(int i=countTail;--i>=0;)
      {

			points[i]=__pointf(fTailPoints[countTail-i-1]); 	

		}

      for (int i = 0; i < size;i++) 
      {

         points[i].Offset(polyOffset.x, polyOffset.y);

      }

      pDC->path_new(false, Gdiplus::FillModeWinding);

      //pDC->path()->SetFillMode(Gdiplus::FillModeAlternate);

      pDC->path()->AddPolygon(points.GetData(), size);

      //pDC->path()->CloseFigure();

      pDC->PaintPenPath();

      pDC->path_destroy(false);

   }
   else if(pDC->m_PenPlus.get())
   { 
      pDC->graphics_on();
//			pointd theFirstPoint( 0, 0 );
//			//CDC* pCDC = pDC->GetCDC();
//
//			
///*			CPen m_Pen(PS_SOLID , 1, RGB(255, 0, 0));
//			CGDIObject<CPen> gdiPen(pCDC,&m_Pen);*/
//         pDC->graphics_on();
//
//			if ( fHeadPoints.GetSize() )
//				theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fHeadPoints.GetAt(fHeadPoints.GetUpperBound());
//			else
//				{
//				if ( fTailPoints.GetSize() )
//					theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fTailPoints.GetAt(0);
//				}
//				
//			//pCDC->MoveTo( __point(theFirstPoint+polyOffset ));
//         pointd ptLast = theFirstPoint + polyOffset;
//			register long count = fHeadPoints.GetSize();
//			if ( count ){
//				do	{
//					pointd currPt=fHeadPoints[ --count ] +polyOffset;
//					//pCDC->LineTo( __point(currPt));
//               pDC->graphics()->DrawLine(pDC->m_PenPlus.get(), __pointf(ptLast), __pointf(currPt));
//               ptLast = currPt;
//				}while ( count );
//			}
//			//pDC->LineTo(theFirstPoint);
      int size = fHeadPoints.GetSize() + fTailPoints.GetSize();
      //LPPOINT points=new POINT[size];
      //register long countTail = fTailPoints.GetSize(); 
      //for(int i=fHeadPoints.GetSize(); --i>=0; ){
      //	points[countTail+i]=__point(fHeadPoints[i]); 	
      //}
      //for(int i=countTail;--i>=0;){
      //	points[i]=__point(fTailPoints[countTail-i-1]); 	
      //}
      //CRgn polyRgn;
      //if(polyRgn.CreatePolygonRgn(points, size,ALTERNATE)){
      //	polyRgn.OffsetRgn(__point(polyOffset));
      //	pDC->PaintPenRgn( &polyRgn);

      //}
      //delete[] points;

      // Infinisys Ltd. GDI+

      CArray < pointf > points;

      points.SetSize(size);

      long countTail = fTailPoints.GetSize();

      for (int i = fHeadPoints.GetSize(); --i >= 0; )
      {

         auto p1 = __pointf(fHeadPoints[i]);

         points[countTail + i] = p1;

      }

      for (int i = countTail; --i >= 0;)
      {

         auto p2 = __pointf(fTailPoints[countTail - i - 1]);

         points[i] = p2;

      }

      for (int i = 0; i < size; i++)
      {

         auto & p = points[i];

         p.Offset(polyOffset.x, polyOffset.y);

      }

      Gdiplus::Pen * ppen = pDC->m_PenPlus.get();

      Gdiplus::PointF * ppoints = points.GetData();

      pDC->graphics()->DrawLines(ppen, ppoints, size);

      //pDC->PaintPenRgn((Gdiplus::Region*)nullptr);

	}

	
/*	if (fFrameIt)
			pDC->FrameRgn( &polyRgn, C ); */
		
			// throw it away
		

		
		//PolyHandle aPoly = ::OpenPoly();
		
		//if ( aPoly )
//			{
			// Find the start of the poly
/*			pointd theFirstPoint( 0, 0 );
			
			if ( fHeadPoints.GetSize() )
				theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fHeadPoints.GetAt(fHeadPoints.GetUpperBound());
			else
				{
				if ( fTailPoints.GetSize() )
					theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fTailPoints.GetAt(0);
				}
				
			pDC->MoveTo( theFirstPoint+polyOffset );
//DBG			TRACE("\n DrawNow: FP {%d, %d} ", 	(theFirstPoint+polyOffset).x,(theFirstPoint+polyOffset).y);
			// Now walk the point arrays to build up the poly we want to draw
						
			// walk down the head points....
			register long count = fHeadPoints.GetSize();
			if ( count ){
				do	{
					pointd currPt=fHeadPoints[ --count ] +polyOffset;
					pDC->LineTo( currPt);
//DBG					TRACE("{%d, %d}", currPt.x, currPt.y);
				}while ( count );
			}
				
			// walk up the tail points
			count = fTailPoints.GetSize();
			if ( count ){
				register long index = 0;
				do	{
					pointd currPt=fTailPoints[ index++ ] +polyOffset;
					pDC->LineTo( currPt);
//DBG					TRACE("{%d, %d}", currPt.x, currPt.y);
				}while ( index < count );
			}
				
			// Close the object
			pDC->LineTo( theFirstPoint +polyOffset); */
//DBG		TRACE("{%d, %d} end DrawNow \n", 	(theFirstPoint+polyOffset).x,(theFirstPoint+polyOffset).y);
//			::ClosePoly();
			
			// draw the poly
//			::OffsetPoly( aPoly, polyOffset.x, polyOffset.y );
//			:: PaintPoly( aPoly );
//			if (fFrameIt)
//				::FramePoly( aPoly );
			
			// throw it away
//			::KillPoly( aPoly );
//			}
//		} 
	}

   void
      CMDFiloPoly::DrawLineNow(CDraftContext* pDC) const
   {
      if (!(fHeadPoints.GetSize() && fTailPoints.GetSize())) {
         return;
      }
         pDC->graphics_on();
         //			pointd theFirstPoint( 0, 0 );
         //			//CDC* pCDC = pDC->GetCDC();
         //
         //			
         ///*			CPen m_Pen(PS_SOLID , 1, RGB(255, 0, 0));
         //			CGDIObject<CPen> gdiPen(pCDC,&m_Pen);*/
         //         pDC->graphics_on();
         //
         //			if ( fHeadPoints.GetSize() )
         //				theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fHeadPoints.GetAt(fHeadPoints.GetUpperBound());
         //			else
         //				{
         //				if ( fTailPoints.GetSize() )
         //					theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fTailPoints.GetAt(0);
         //				}
         //				
         //			//pCDC->MoveTo( __point(theFirstPoint+polyOffset ));
         //         pointd ptLast = theFirstPoint + polyOffset;
         //			register long count = fHeadPoints.GetSize();
         //			if ( count ){
         //				do	{
         //					pointd currPt=fHeadPoints[ --count ] +polyOffset;
         //					//pCDC->LineTo( __point(currPt));
         //               pDC->graphics()->DrawLine(pDC->m_PenPlus.get(), __pointf(ptLast), __pointf(currPt));
         //               ptLast = currPt;
         //				}while ( count );
         //			}
         //			//pDC->LineTo(theFirstPoint);
         int size = fHeadPoints.GetSize() + fTailPoints.GetSize();
         //LPPOINT points=new POINT[size];
         //register long countTail = fTailPoints.GetSize(); 
         //for(int i=fHeadPoints.GetSize(); --i>=0; ){
         //	points[countTail+i]=__point(fHeadPoints[i]); 	
         //}
         //for(int i=countTail;--i>=0;){
         //	points[i]=__point(fTailPoints[countTail-i-1]); 	
         //}
         //CRgn polyRgn;
         //if(polyRgn.CreatePolygonRgn(points, size,ALTERNATE)){
         //	polyRgn.OffsetRgn(__point(polyOffset));
         //	pDC->PaintPenRgn( &polyRgn);

         //}
         //delete[] points;

         // Infinisys Ltd. GDI+

         CArray < pointf > points;

         points.SetSize(size/ 2);

         long countTail = fTailPoints.GetSize();

         long countHead = fHeadPoints.GetSize();

         for (int i = fHeadPoints.GetUpperBound(); i >= 0; i--)
         {

            auto p1 = __pointf(fHeadPoints[i]);

            auto p2 = __pointf(fTailPoints[i]);

            points[i].X = (p2.X + p1.X) / 2.0;
            points[i].Y = (p2.Y + p1.Y) / 2.0;

         }

       
         for (int i = 0; i < points.GetCount(); i++)
         {

            auto & p = points[i];

            p.Offset(polyOffset.x, polyOffset.y);

         }

         Gdiplus::Pen * ppen = pDC->m_PenPlus.get();

         Gdiplus::PointF * ppoints = points.GetData();

         pDC->graphics()->DrawLines(ppen, ppoints, points.GetCount());

         //pDC->PaintPenRgn((Gdiplus::Region*)nullptr);



      /*	if (fFrameIt)
               pDC->FrameRgn( &polyRgn, C ); */

               // throw it away



            //PolyHandle aPoly = ::OpenPoly();

            //if ( aPoly )
      //			{
               // Find the start of the poly
      /*			pointd theFirstPoint( 0, 0 );

               if ( fHeadPoints.GetSize() )
                  theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fHeadPoints.GetAt(fHeadPoints.GetUpperBound());
               else
                  {
                  if ( fTailPoints.GetSize() )
                     theFirstPoint = (const_cast<CMDFiloPoly*>(this))->fTailPoints.GetAt(0);
                  }

               pDC->MoveTo( theFirstPoint+polyOffset );
      //DBG			TRACE("\n DrawNow: FP {%d, %d} ", 	(theFirstPoint+polyOffset).x,(theFirstPoint+polyOffset).y);
               // Now walk the point arrays to build up the poly we want to draw

               // walk down the head points....
               register long count = fHeadPoints.GetSize();
               if ( count ){
                  do	{
                     pointd currPt=fHeadPoints[ --count ] +polyOffset;
                     pDC->LineTo( currPt);
      //DBG					TRACE("{%d, %d}", currPt.x, currPt.y);
                  }while ( count );
               }

               // walk up the tail points
               count = fTailPoints.GetSize();
               if ( count ){
                  register long index = 0;
                  do	{
                     pointd currPt=fTailPoints[ index++ ] +polyOffset;
                     pDC->LineTo( currPt);
      //DBG					TRACE("{%d, %d}", currPt.x, currPt.y);
                  }while ( index < count );
               }

               // Close the object
               pDC->LineTo( theFirstPoint +polyOffset); */
               //DBG		TRACE("{%d, %d} end DrawNow \n", 	(theFirstPoint+polyOffset).x,(theFirstPoint+polyOffset).y);
               //			::ClosePoly();

                        // draw the poly
               //			::OffsetPoly( aPoly, polyOffset.x, polyOffset.y );
               //			:: PaintPoly( aPoly );
               //			if (fFrameIt)
               //				::FramePoly( aPoly );

                        // throw it away
               //			::KillPoly( aPoly );
               //			}
               //		} 
   }


/*------------------------------------------------------------------------------*/
		
void
CMDFiloPoly::Reset( void )
	{
	fLastHeadPoint.x = fLastHeadPoint.y = 0;
	fLastTailPoint.x = fLastTailPoint.y = 0;
		
	// Wipe out the point lists
	fHeadPoints.RemoveAll();
	fTailPoints.RemoveAll();
   m_bP1Set = false;
	}
	
/*------------------------------------------------------------------------------*/
		
//void
//CMDFiloPoly::PushHeadPoint( const pointd& newHeadPt, CDraftContext* pDC)
//	{
//	if ( ( fLastHeadPoint.x != newHeadPt.x ) || ( fLastHeadPoint.y != newHeadPt.y ) )
//		{
//		//if ( fHeadPoints.GetSize() > 1 )
//		//	{
//		//	pointd fLastButOneHeadPoint = fHeadPoints[ fHeadPoints.GetSize() - 2 ];
//
//		//	double dv1 = fLastButOneHeadPoint.y - fLastHeadPoint.y;
//  //       double dv2 = fLastHeadPoint.y - newHeadPt.y;
//		//	
//  //       double dh1 = fLastButOneHeadPoint.x - fLastHeadPoint.x;
//  //       double dh2 = fLastHeadPoint.x - newHeadPt.x;
//		//	
//		//	if ( ( dv1 * dh2 ) == ( dv2 * dh1 ) )  // Cross multiplying
//		//		{
//		//		// colinear
//		//		
//		//		if (	( ( dv1 >= 0 ) && ( dv2 >= 0 ) ) ||
//		//				( ( dv1 < 0 ) && ( dv2 < 0 ) ) )
//		//			{
//		//			if (	( ( dh1 >= 0 ) && ( dh2 >= 0 ) ) ||
//		//					( ( dh1 < 0 ) && ( dh2 < 0 ) ) )
//		//				{
//		//				// fLastHeadPoint is in the middle of the other two
//		//				
//		//				// overwrite the previous fLastHeadPoint
//		//				fLastHeadPoint = newHeadPt;
//		//				
//		//				// overwrite the same point in our list
//		//				fHeadPoints.SetAt(fHeadPoints.GetUpperBound(), newHeadPt);
//		//				return;
//		//				}
//		//			}
//		//		}
//		//	}
//		//	
//		fLastHeadPoint = newHeadPt;
//		fHeadPoints.Add( newHeadPt);
//		}
//   m_bP1Set = true;
//
//	CheckPointsAndPurge(pDC);
//	}
//	
///*------------------------------------------------------------------------------*/
//		
//void CMDFiloPoly::PushTailPoint( const pointd& newTailPt, CDraftContext* pDC)
//{
//	if ( ( fLastTailPoint.x != newTailPt.x ) || ( fLastTailPoint.y != newTailPt.y ) )
//	{
//		//if ( fTailPoints.GetSize() > 1 )
//		//{
//		//	pointd fLastButOneTailPoint = fTailPoints[ fTailPoints.GetSize() - 2 ];
//		//	
//  //       double dv1 = fLastButOneTailPoint.y - fLastTailPoint.y;
//  //       double dv2 = fLastTailPoint.y - newTailPt.y;
//		//	
//  //       double dh1 = fLastButOneTailPoint.x - fLastTailPoint.x;
//  //       double dh2 = fLastTailPoint.x - newTailPt.x;
//		//	
//		//	if ( ( dv1 * dh2 ) == ( dv2 * dh1 ) )  // Cross multiplying
//		//	{
//		//		// colinear
//		//		
//		//		if (	( ( dv1 >= 0 ) && ( dv2 >= 0 ) ) ||
//		//				( ( dv1 < 0 ) && ( dv2 < 0 ) ) )
//		//		{
//		//			if (	( ( dh1 >= 0 ) && ( dh2 >= 0 ) ) ||
//		//					( ( dh1 < 0 ) && ( dh2 < 0 ) ) )
//		//			{
//		//				// fLastTailPoint is in the middle of the other two
//		//				
//		//				// overwrite the previous fLastTailPoint
//		//				fLastTailPoint = newTailPt;
//		//				
//		//				// overwrite the same point in our list
//		//				fTailPoints.SetAt(fTailPoints.GetUpperBound(), newTailPt);
//		//				return;
//		//			}
//		//		}
//		//	}
//		//}
//		//	
//		fLastTailPoint = newTailPt;
//		fTailPoints.Add( newTailPt);
//	}
//   m_bP1Set = true;
//
//		
//	CheckPointsAndPurge(pDC);
//}

		// Infinisys Ltd. 2021
void
CMDFiloPoly::PushPoints( const pointd& newHeadPt, const pointd& newTailPt,  CDraftContext* pDC)
	{
	if ( ( fLastHeadPoint.x != newHeadPt.x ) || ( fLastHeadPoint.y != newHeadPt.y ) 
      ||
      (fLastTailPoint.x != newTailPt.x) || (fLastTailPoint.y != newTailPt.y))
   {
		//if ( fHeadPoints.GetSize() > 1 )
		//	{
		//	pointd fLastButOneHeadPoint = fHeadPoints[ fHeadPoints.GetSize() - 2 ];

		//	double dv1 = fLastButOneHeadPoint.y - fLastHeadPoint.y;
  //       double dv2 = fLastHeadPoint.y - newHeadPt.y;
		//	
  //       double dh1 = fLastButOneHeadPoint.x - fLastHeadPoint.x;
  //       double dh2 = fLastHeadPoint.x - newHeadPt.x;
		//	
		//	if ( ( dv1 * dh2 ) == ( dv2 * dh1 ) )  // Cross multiplying
		//		{
		//		// colinear
		//		
		//		if (	( ( dv1 >= 0 ) && ( dv2 >= 0 ) ) ||
		//				( ( dv1 < 0 ) && ( dv2 < 0 ) ) )
		//			{
		//			if (	( ( dh1 >= 0 ) && ( dh2 >= 0 ) ) ||
		//					( ( dh1 < 0 ) && ( dh2 < 0 ) ) )
		//				{
		//				// fLastHeadPoint is in the middle of the other two
		//				
		//				// overwrite the previous fLastHeadPoint
		//				fLastHeadPoint = newHeadPt;
		//				
		//				// overwrite the same point in our list
		//				fHeadPoints.SetAt(fHeadPoints.GetUpperBound(), newHeadPt);
		//				return;
		//				}
		//			}
		//		}
		//	}
		//	
		fLastHeadPoint = newHeadPt;
		fHeadPoints.Add( newHeadPt);
      fLastTailPoint = newTailPt;
      fTailPoints.Add(newTailPt);

		}
   m_bP1Set = true;

	CheckPointsAndPurge(pDC);
	}


/*------------------------------------------------------------------------------*/
		
void CMDFiloPoly::SetOffset( const point& newOffset )
{
	polyOffset = newOffset;
}
	
/*------------------------------------------------------------------------------*/
		
void CMDFiloPoly::CheckPointsAndPurge(CDraftContext* pDC)
{
	if ( ( fHeadPoints.GetSize() > 4096 ) || ( fTailPoints.GetSize() > 4096 ) )
	{
		pointd HeadPt = fHeadPoints.GetAt(fHeadPoints.GetUpperBound());
		pointd TailPt = fTailPoints.GetAt(fTailPoints.GetUpperBound());
		
		DrawNow(pDC);
		Reset();
		
		PushPoints( HeadPt, TailPt, pDC );
		//PushTailPoint( TailPt, pDC );
	}
}