#include "stdafx.h"
#include "resource.h"
#include "MDShape.h"

#include "PCDraftView.h"
#include "PCDraftDoc.h"

#include "phmath.h"
#include "gu.h"
#include "BezierEdge.h"
#include "DraftContext.h"
#include "MDReport.h"
#include "DrawPrim.h"
#include "DimUnit.h"
#include "MDArea.h"
#include "DSShape.h"
#include "MDAssocDim.h"
#include "Layer.h"

#include "PhIOStruct.h"
#include "MDMetaPolyShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EQUALRGB(color1Ptr,color2Ptr)		\
		(												\
			(color1Ptr)->red == (color2Ptr)->red &&		\
			(color1Ptr)->green == (color2Ptr)->green &&	\
			(color1Ptr)->blue == (color2Ptr)->blue		\
		)

CTrackContext::CTrackContext()
{
	nWhichHandle=0;
	lParam=0;
	bSwappedVertexes=false;
	bShift=false;
	bOption=false;
}

CTrackContext::~CTrackContext()
{
}

CMDShapeList CMDShape::gDeletedShapes;
CMap<CMDShape*,CMDShape*,CMDShape*,CMDShape*> CMDShape::gReplacedShapes;

IMPLEMENT_SERIAL(CMDShape,CObject,1)
//////////////////////////////////////////////////////////////////////

CMDShape::CMDShape()
{
	m_RotAngle=0; 
	m_nDatum=kCenterDatum;
	m_lptDatum=LongPoint::kZeroPt; 
	m_lrcExtents=LongRect::kZeroRect;
	m_bLocked=false;
	m_pAttached=NULL;
}

/*CMDShape::~CMDShape()
{
	if(m_pAttached) {
		delete m_pAttached;
		m_pAttached = NULL;
	}
}*/

void CMDShape::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_PenModel;
		ar >> m_RotAngle;
		ar >> m_nDatum;
		ar >> m_lptDatum;
		ar >> m_lrcExtents;
        BYTE getData;
        ar >> getData;
        m_bLocked = getData == TRUE;
		for(int i=0;i<kMaxAttrib;i++)
			ar >> m_sValues[i];
		ar >> m_pAttached;

/*		int nCount;
		ar >> nCount;
		m_Points.SetSize(nCount);
		for(i=0;i<nCount;i++)
			ar >> m_Points[i];
*/	}
	else
	{
		ar << m_PenModel;
		ar << m_RotAngle;
		ar << m_nDatum;
		ar << m_lptDatum;
		ar << m_lrcExtents;
        ar << (BYTE)(m_bLocked?1:0);
		for(int i=0;i<kMaxAttrib;i++)
			ar << m_sValues[i];
		ar << m_pAttached;

/*		int nCount=m_Points.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++)
			ar << m_Points[i];
*/	}
}

LongPoint CMDShape::GetShapeDatum() const
{
	return m_lptDatum;
}

void CMDShape::SetShapeDatum(short nDatum)
{
	if(nDatum==kFreeDatum || nDatum>=GetNumShapePoints()) nDatum=kDefaultDatum;
	if(nDatum==kDefaultDatum)
	{
		nDatum=DefaultDatumCode();
		if(nDatum==kFreeDatum) CalcDefaultDatum(m_lptDatum);
	}
	switch(nDatum)
	{
	case kFreeDatum:
		break;
	case kCenterDatum:
		CalcCenterDatum(m_lptDatum);
		break;
	default:	// anchored
		m_lptDatum=GetShapePoint(nDatum);
		break;
	}
	m_nDatum=nDatum;
}

void CMDShape::SetShapeDatum(const LongPoint& lptDatum)
{
	m_nDatum=kFreeDatum;
	m_lptDatum=lptDatum;
}

LongRect CMDShape::CalculateBasicExtents() const
{
	LongBoundRect lbr;
	for(int i=0;i<GetNumShapePoints();i++)
		lbr|=GetShapePoint(i);
	return lbr;
}

extern short getEndcapIndex(short endcap, short penWidth);

void CMDShape::ExtendForPen(LongRect& lrcExtent) const
{
	Coord adjust=0;
	if(m_PenModel.m_nAlignment!=kInsideLine && m_PenModel.m_nWidth!=0x81)
	{
		Coord penW;
		switch(m_PenModel.m_nWidth)
		{
		case 0xC1:
			adjust=FixedHalf;
			break;
		case 0xA1:
			adjust=WrFixedFromFixed(0x0000C000);
			break;
		default:
			adjust=(m_PenModel.m_nWidth & 0x1F);
		}
		if(m_PenModel.m_nAlignment==kCenterLine) adjust>>=1;
	}
	lrcExtent.Inset(-adjust,-adjust);

	if(!IsClosed())
	{
		if(m_PenModel.EndCap1())
		{
			LongPoint lpt=GetFirstShapePoint();
			adjust=Coord(gEndcaps[getEndcapIndex(m_PenModel.EndCap1(),m_PenModel.m_nWidth&0x1F)].width);
			LongRect lrc;
			lrc.Set(lpt.x - adjust, lpt.y - adjust, lpt.x + adjust +1 , lpt.y + adjust +1);
			lrcExtent|=lrc;
		}
		if(m_PenModel.EndCap2())
		{
			LongPoint lpt=GetLastShapePoint();
			adjust=Coord(gEndcaps[getEndcapIndex(m_PenModel.EndCap2(),m_PenModel.m_nWidth&0x1F)].width);
			LongRect lrc;
			lrc.Set(lpt.x - adjust, lpt.y - adjust, lpt.x + adjust +1 , lpt.y + adjust +1);
			lrcExtent|=lrc;
		}
	}
}

void CMDShape::CalculateExtents()
{
	m_lrcExtents=CalculateBasicExtents();
}

bool CMDShape::ModifyPenModel(const PenModel &penModel, int nWhat)
{
	bool bResult=true;
	if(nWhat&pmUseEndCaps && (IsClosed() || IsRadial() || IsParallel() || IsKindOf(RUNTIME_CLASS(CMDAssocDim))))
	{
		nWhat&=~pmUseEndCaps;
		bResult=false;
	}
	if(nWhat)
	{
		m_PenModel.Modify(penModel,nWhat);
		CalculateExtents();
	}
	return bResult;
}

const FillModel& CMDShape::GetFillModel() const
{
static FillModel gFillModel;
	return gFillModel;
}

const TextParams& CMDShape::GetTextParams() const
{
static TextParams gTextParams;
	return gTextParams;
}

const ParallelAttrs& CMDShape::GetParallelAttrs() const
{
static ParallelAttrs gParallelAttrs(kNoFill);
	return gParallelAttrs;
}

CEdge* CMDShape::GetShapeEdgeFromPt(const pointd &pt,const CMapper* pMapper) const
{
	CEdge* pEdge=NULL;
	for(int i=0;i<GetNumShapeEdges();i++)
	{
		pEdge=GetShapeEdge(i);
		if(pEdge->PtAtEdge(pt,pMapper)) break;
		else delete pEdge;
		pEdge=NULL;
	}
	return pEdge;
}

void CMDShape::DrawHandles(CDraftContext* pDC) const
{
	for(int i=0;i<GetNumShapeHandles();i++)
	{
		pDC->DrawHandle(GetShapeHandle(i),m_bLocked);
	}
}

bool CMDShape::TestSelect(const pointd& pt, CMapper* pMapper) const
{
	return PointInFill(pt,pMapper) || PointInShape(pt,pMapper);
}

BOOL CMDShape::TestHandle(const pointd& hpt,const pointd& pt)
{
	CRect rc(__point(hpt),__point(hpt));
	rc.InflateRect(kTrackHandleSize,kTrackHandleSize);
	return rc.PtInRect(__point(pt));
}

int	CMDShape::TestHandles(const pointd& pt, CMapper* pMapper) const
{
	for(int i=GetNumShapeHandles()-1;i>=0;i--)
	{
		if(TestHandle(pMapper->LPtoDPF(GetShapeHandle(i)),pt))
			return i;
	}
	return -1; // miss it
}

int	CMDShape::TestResizeHandles(const pointd& pt, CMapper* pMapper) const
{
	return TestHandles(pt,pMapper);
}

void CMDShape::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	int nVertex=TestHandles(pt,pMapper);
	if(nVertex>=0)
	{
		vertexes.Add(nVertex);
		shapes.Add(this);
	}
}

void CMDShape::DoShapeOffset(const LongPoint& delta)
{
	// just offset all shape points by delta
	int nPoints=GetNumShapePoints();
	for(int i=0;i<nPoints;i++)
		SetShapePoint(i,GetShapePoint(i)+delta);
	// reasonable, that extents and datum also moved
	m_lptDatum+=delta;
	m_lrcExtents+=delta;
}

void CMDShape::DoShapeMove(const LongPoint& pos)
{
	LongPoint delta=pos-m_lptDatum;
	DoShapeOffset(delta);
	CMDShape* pShape=GetAttachedShape();
	if(pShape) pShape->DoShapeOffset(delta);
}

void CMDShape::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongRatio xRatio,yRatio;
	xRatio.fN=(lrcTo.right-lrcTo.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrcTo.bottom-lrcTo.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();

	// adjust rotation
	if(xRatio.fN<0)	m_RotAngle=mtFixAngle(-m_RotAngle);
	if(yRatio.fN<0)	m_RotAngle=mtFixAngle(180-m_RotAngle);

	int nPoints=GetNumShapePoints();
	for(int i=0;i<nPoints;i++)
	{
		LongPoint lptOffset=GetShapePoint(i)-lrcFrom.GetCorner(0);
		lptOffset.x*=xRatio;
		lptOffset.y*=yRatio;
		SetShapePoint(i,lrcTo.GetCorner(0)+lptOffset);
	}
	if(kFreeDatum==m_nDatum)
	{
		LongPoint lptOffset=GetShapeDatum()-lrcFrom.GetCorner(0);
		lptOffset.x*=xRatio;
		lptOffset.y*=yRatio;
		SetShapeDatum(lrcTo.GetCorner(0)+lptOffset);
	}
//	if(m_pAttached) m_pAttached->DoShapeScale(lrcFrom,lrcTo,ppNewShape,pList);
	CalculateExtents();
	ValidateDatum();
}

void CMDShape::DoShapeScale(const LongRatio& ratio,double factor)
{
	int nPoints=GetNumShapePoints();
	for(int i=0;i<nPoints;i++)
	{
		LongPoint lpt=GetShapePoint(i);
		if(factor) lpt*=factor;
		lpt*=ratio;
		SetShapePoint(i,lpt);
	}
	if(kFreeDatum==m_nDatum)
	{
		LongPoint lpt=GetShapeDatum();
		if(factor) lpt*=factor;
		lpt*=ratio;
		SetShapeDatum(lpt);
	}
//	if(m_pAttached) m_pAttached->DoShapeScale(ratio,factor);
	CalculateExtents();
	ValidateDatum();
}

void CMDShape::DoShapeRotate(Angle angle, short nAbout,const LongPoint& lptRef)
{
	LongPoint lptRot;
	switch(nAbout)
	{
	case kAboutCenter:
		lptRot=IsRadial()?GetCenter():Middle(GetExtents());
		break;
	case kAboutDatum:
		lptRot=GetShapeDatum();
		break;
	case kAboutRefPt:
		lptRot=lptRef;
		break;
	default:
		ASSERT(0);
	}

	angle=mtFixAngle(angle);
	m_RotAngle=mtFixAngle(angle+m_RotAngle);
	Angle rotAngle=angle;

	int nPoints=GetNumShapePoints();
	for(int i=0;i<nPoints;i++)
		SetShapePoint(i,mtRotPoint(GetShapePoint(i),rotAngle,lptRot));
	if(nAbout!=kAboutDatum) m_lptDatum=mtRotPoint(m_lptDatum,rotAngle,lptRot);
	CalculateExtents();
}

void CMDShape::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	// mirror rotation
	Angle refAngle=mtAngle(lptRef1,lptRef2);
	m_RotAngle=mtFixAngle(2*refAngle-m_RotAngle);

	// mirror points
	for(int i=0;i<GetNumShapePoints();i++)
	{
		LongPoint perpPt,lpt=GetShapePoint(i);
		guNearPtLnPt(lpt,lptRef1,lptRef2,&perpPt);
		perpPt-=lpt;
		perpPt<<=1;
		SetShapePoint(i,lpt+perpPt);
	}
	if(m_nDatum>=0)	SetShapeDatum(GetShapePoint(m_nDatum));
	else
	{
		LongPoint perpPt,lpt=GetShapeDatum();
		guNearPtLnPt(lpt,lptRef1,lptRef2,&perpPt);
		perpPt-=lpt;
		perpPt<<=1;
		SetShapeDatum(lpt+perpPt);
	}
	CalculateExtents();
}

void CMDShape::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	// flip rotation
	m_RotAngle=mtFixAngle((bVertical?180:0)-m_RotAngle);

	LongPoint lptCenterMul2;
	if(pLpt) lptCenterMul2=*pLpt;
	else
	{
		lptCenterMul2.x=m_lrcExtents.left+m_lrcExtents.right;
		lptCenterMul2.y=m_lrcExtents.top+m_lrcExtents.bottom;
	}
	for(int i=0;i<GetNumShapePoints();i++)
	{
		LongPoint lpt=GetShapePoint(i);
		if(bVertical) lpt.y=lptCenterMul2.y-lpt.y;
		else lpt.x=lptCenterMul2.x-lpt.x;
		SetShapePoint(i,lpt);
	}
	if(m_nDatum>=0)	SetShapeDatum(GetShapePoint(m_nDatum));
	else
	{
		LongPoint lpt=GetShapeDatum();
		if(bVertical) lpt.y=lptCenterMul2.y-lpt.y;
		else lpt.x=lptCenterMul2.x-lpt.x;
		SetShapeDatum(lpt);
	}
}

CMDShape* CMDShape::CreateCopy() const
{
	CMDShape* pNewShape;

	CMemFile mf;
	{
		CArchive ar(&mf,CArchive::store);
		ar << this;
		ar.Close();
	}

	mf.Seek(0,CFile::begin);
	{
		CArchive ar(&mf,CArchive::load);
		ar >> pNewShape;
		ar.Close();
	}

	return pNewShape;
}

int CMDShape::Snap(LongPoint *lptSnap, const LongRect &lrcProx, short nType) const
{
	for(int i=0;i<GetNumShapePoints();i++)
	{
		if(lrcProx.Contains(GetShapePoint(i)))
		{
			*lptSnap=GetShapePoint(i);
			return i;
		}
	}
	return -1;
}

void CMDShape::GetValues(CString &sValue1, CString &sValue2, CString &sValue3, CString &sValue4, CString &sValue5) const
{
	sValue1=m_sValues[0];
	sValue2=m_sValues[1];
	sValue3=m_sValues[2];
	sValue4=m_sValues[3];
	sValue5=m_sValues[4];
}

void CMDShape::SetValues(CString &sValue1, CString &sValue2, CString &sValue3, CString &sValue4, CString &sValue5)
{
	m_sValues[0]=sValue1;
	m_sValues[1]=sValue2;
	m_sValues[2]=sValue3;
	m_sValues[3]=sValue4;
	m_sValues[4]=sValue5;
}

void CMDShape::SetValues(CString *sValues)
{
	for(int i=0;i<kMaxAttrib;i++) m_sValues[i]=sValues[i];
}

void CMDShape::GetValues(CString *sValues) const
{
	for(int i=0;i<kMaxAttrib;i++) sValues[i]=m_sValues[i];
}

bool CMDShape::DoTrackShape(int nWhichHandle, CPoint pt, CPCDraftView* pView, LPCRECT lpcRect /*=NULL*/)
{
	CFlag cFlag;
	CTrackContext* pTrackContext=TrackCreateContext(nWhichHandle, 0, &pView->m_Mapper);
	
	if(pTrackContext)
	{
		pTrackContext->pView=pView;

		pView->ShowCursorPrompt(false);
		pView->ShowCrossCursor(false);
		pView->SetTrackingShape(true);
		bool bEmptyPrompt=true;
		pView->SetCursorPrompt(IDS_EMPTYPROMPT);
		pt-=pView->GetDeviceScrollPosition();
		pView->AdjustCursor(pt,false);

		bool bMouseHasMoved=false;
		pView->SetCapture();

		CClientDC cdc(pView);
		pView->OnPrepareDC(&cdc);
		if(lpcRect!=NULL)
		{
			CRgn rgn;
			rgn.CreateRectRgnIndirect(lpcRect);
			cdc.SelectClipRgn(&rgn);
		}
		//cdc.SetROP2(R2_NOT);
		CGDIBackMode bm(&cdc,TRANSPARENT);
		CDraftContext dc(&cdc,&pView->m_Mapper);
		dc.SetOutlinePen(false);

      //Infinisys Ltd. GDI+
      int iDrawSelectionMargin = 0;
      CRect rectSelection;
      std::unique_ptr<Gdiplus::Bitmap> pbitmap;
      pbitmap.reset(pView->BitmapFromSelection(rectSelection, 0.5, iDrawSelectionMargin));
      std::unique_ptr<Gdiplus::Bitmap> pbitmapView;
      pbitmapView.reset(pView->BitmapFromView());
      std::unique_ptr<Gdiplus::Bitmap> pbitmapDraw;
      pbitmapDraw.reset(pView->BitmapFromView());
      Gdiplus::Graphics gDraw(pbitmapDraw.get());

		LongPoint lptMiddle=Middle(GetExtents());
		LongPoint ptLast=GetShapeHandle(nWhichHandle);
//		LongPoint delta=ptLast-pView->m_Mapper.DPtoLP(pt);

		pointd ptOrg=pView->GetDeviceScrollPosition();
		//cdc.SetViewportOrg(-ptOrg);
		//{
		//	CPen pen(PS_DOT,1,RGB(0,0,0));
		//	CGDIObject<CPen> oldPen(&cdc,&pen);
		//	TrackWireFrame(&dc, pTrackContext);
		//}
		TrackShowSize(pTrackContext);
		pView->ShowCrossCursor();
		pView->ShowCursorPrompt();

		while(1)
		{
			MSG msg;
			bool bResult=false;

			LONG lIdle = 0;
			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				lIdle = 0;
			
				switch(msg.message)
				{
				case WM_RBUTTONDOWN:
				case WM_CAPTURECHANGED:
				case WM_CANCELMODE:
				case WM_LBUTTONUP:
cancelTrack:
					pView->ShowCursorPrompt(false);
					pView->ShowCrossCursor(false);
					//{
					//	CPen pen(PS_DOT,1,RGB(0,0,0));
					//	CGDIObject<CPen> oldPen(&cdc,&pen);
					//	TrackWireFrame(&dc, pTrackContext);
					//}

               {
                  Gdiplus::Graphics g(cdc.GetSafeHdc());

                  g.DrawImage(pbitmapView.get(), 0, 0);

               }

					if(msg.message==WM_LBUTTONUP && bMouseHasMoved)
					{
						pView->InvalidateShape(this,true);
						TrackComplete(pTrackContext);
						if(m_pAttached && m_pAttached->IsAttached())
						{
							pView->InvalidateShape(m_pAttached,true);
							m_pAttached->DoShapeOffset(Middle(GetExtents())-lptMiddle);
							pView->InvalidateShape(m_pAttached,true);
						}
						ReCalc(&pView->m_Mapper);
						UpdateDependentShapes(pView);
						ValidateDatum();
						pView->InvalidateShape(this,true);
						bResult=true;
					}
					ReleaseCapture();
					delete pTrackContext;
					pTrackContext = NULL;
					pView->ClearShowSize();
//					SetCursor(AfxGetApp()->LoadCursor(IDC_POINTER));
					pView->SetTrackingShape(false);
					if(bEmptyPrompt) pView->SetCursorPrompt(0);
					pView->CancelMouseTracking(false);
					pView->ShowCrossCursor();
					pView->ShowCursorPrompt();
					return bResult;
				case WM_KEYDOWN:
					if(msg.wParam==VK_ESCAPE) goto cancelTrack;
				case WM_KEYUP:
				case WM_MOUSEMOVE:
					{
						pView->ScreenToClient(&msg.pt);
						LongPoint ptNew=pView->m_Mapper.DPtoLP(pointd(msg.pt)+pView->GetDeviceScrollPosition()); //+delta;
						pView->SnapPtToDocument(ptNew);
						if(GetAsyncKeyState(VK_COMMAND)>=0) pView->SnapLPt(ptNew,true);
						if(/*ptNew!=ptOld && */pView->PtInDocument(ptNew))
						{
							int nType=0;
							if(GetAsyncKeyState(0x43)<0) nType=IDC_CENTER_SNAP;		// VK_C
							else if(GetAsyncKeyState(0x45)<0) nType=IDC_END_SNAP;	// VK_E
							else if(GetAsyncKeyState(0x4D)<0) nType=IDC_MID_SNAP;	// VK_M
							if(nType)
							{
								if(bEmptyPrompt)
								{
									pView->SetCursorPrompt(0);
									bEmptyPrompt=false;
								}
								SetCursor(AfxGetApp()->LoadCursor(nType));
							}
							else
							{
								if(!bEmptyPrompt)
								{
									pView->SetCursorPrompt(IDS_EMPTYPROMPT);
									bEmptyPrompt=true;
								}
								SetCursor(AfxGetApp()->LoadCursor(IDC_SMALL_CROSS));
							}
						}
						CPoint ptCursor=msg.pt;
						pView->AdjustCursor(ptCursor,true);
						if(ptNew!=ptLast && pView->PtInDocument(ptNew))
						{
							pView->ShowCursorPromptNoLock(false);
							pView->ShowCrossCursor(false);


							//{
							//	//CPen pen(PS_DOT,1,RGB(0,0,0));
							//	//CGDIObject<CPen> oldPen(&cdc,&pen);
       //                 dc.m_PenPlus.reset(new Gdiplus::Pen(Gdiplus::Color::Black, 1.0f));
							//	TrackWireFrame(&dc, pTrackContext);
							//}
                     {
                        Gdiplus::Graphics g(cdc.GetSafeHdc());

                        g.DrawImage(pbitmapView.get(), 0, 0);

                     }
						
							pView->SketcherScroll(msg.pt);
							ptOrg=pView->GetDeviceScrollPosition();
							//cdc.SetViewportOrg(-ptOrg);

							pTrackContext->bShift=(GetAsyncKeyState(VK_SHIFT)<0);
							pTrackContext->bOption=(GetAsyncKeyState(VK_OPTION)<0);
							TrackFeedback(ptNew, pTrackContext);
							{
								//CPen pen(PS_DOT,1,RGB(0,0,0));
								//CGDIObject<CPen> oldPen(&cdc,&pen);
#undef new
                        Gdiplus::Pen * ppen = new Gdiplus::Pen(Gdiplus::Color(200,180, 180, 180), 1.0f);
                        //ppen->SetDashStyle(Gdiplus::DashStyleDash);
                        dc.m_PenPlus.reset(ppen);
                        UsePath usepath(&dc);
                        TrackWireFrame(&dc, pTrackContext);
//                        TrackWireFrame(&dc, pTrackContext);
                        auto ppath = dc.path();
                        dc.graphics_on();
                        dc.graphics()->TranslateTransform(-ptOrg.x, -ptOrg.y);
                        dc.graphics()->DrawPath(dc.m_PenPlus.get(), ppath);
                        dc.graphics()->TranslateTransform(ptOrg.x, ptOrg.y);
                        dc.graphics_off();
							}
							TrackShowSize(pTrackContext);
							ptLast=ptNew;
							bMouseHasMoved=true;
							pView->ShowCrossCursor();
							pView->ShowCursorPromptNoLock();
						}
					}
					break;
				default:
					DispatchMessage(&msg);
					break;
				}
			}
			else
			{
				// let MFC do its idle processing
				while(AfxGetApp()->OnIdle(lIdle++));  

				point ptMsg;
				GetCursorPos(&ptMsg);
				pView->ScreenToClient(&ptMsg);

            CPoint ptClientCorner;
				pView->GetClientCorner(ptClientCorner);
				CSize szClientSize;
				pView->GetClientSize(szClientSize);
				CRect rcClient(ptClientCorner,szClientSize);
				if(!rcClient.PtInRect(ptMsg))
				{
					//{
					//	CPen pen(PS_DOT,1,RGB(0,0,0));
					//	CGDIObject<CPen> oldPen(&cdc,&pen);
					//	TrackWireFrame(&dc, pTrackContext);
					//}
               if (pView->SketcherScroll(ptMsg))
               {
                  pView->ShowCursorPromptNoLock(false);
                  pView->ShowCrossCursor(false);

                  pbitmapView.reset(pView->BitmapFromView());
                  pView->ShowCursorPromptNoLock();
                  pView->ShowCrossCursor();

               }
					ptOrg=pView->GetDeviceScrollPosition();
					//cdc.SetViewportOrg(-ptOrg);

               {
                  //pbitmapView.reset(pView->BitmapFromView());
                  //Gdiplus::Graphics g(cdc.GetSafeHdc());
                  //g.DrawImage(pbitmapView.get(), (FLOAT)ptOrg.x, (FLOAT)ptOrg.y);
                  //pointd pt = m_Mapper.LPtoDP(ptOld);
                  //g.DrawImage(pbitmap.get(),pt.x - ptShift.x,pt.y - ptShift.y);
               }

					//{
					//	CPen pen(PS_DOT,1,RGB(0,0,0));
					//	CGDIObject<CPen> oldPen(&cdc,&pen);
					//	TrackWireFrame(&dc, pTrackContext);
					//}
					WPARAM fwKeys=0;
					if(GetAsyncKeyState(VK_CONTROL)<0) fwKeys|=MK_CONTROL;
					if(GetAsyncKeyState(VK_LBUTTON)<0) fwKeys|=MK_LBUTTON;
					if(GetAsyncKeyState(VK_MBUTTON)<0) fwKeys|=MK_MBUTTON;
					if(GetAsyncKeyState(VK_RBUTTON)<0) fwKeys|=MK_RBUTTON;
					if(GetAsyncKeyState(VK_SHIFT)<0) fwKeys|=MK_SHIFT;
					PostMessage(pView->GetSafeHwnd(),WM_MOUSEMOVE,fwKeys,MAKELONG(ptMsg.x,ptMsg.y));
				}
			}
		}
	}
	if(pTrackContext) delete pTrackContext;
	return false;
}

double CMDShape::ScaleDist(const LongRatio& ratio,WrFixed dist)
{
	double result=mtFixToReal(dist);
	result*=ratio.fD;
	result/=ratio.fN;
	return result;
}

void CMDShape::ScaleLPoint(const LongRatio& ratio,const LongPoint& lpt,DoublePoint& pt)
{
	pt.x = mtFixToReal(lpt.x);
	pt.y = mtFixToReal(lpt.y);
	pt.x*=ratio.fD;
	pt.x/=ratio.fN;
	pt.y*=ratio.fD;
	pt.y/=ratio.fN;
}

CMDShape* CMDShape::GetAttachedShape() const
{
	return (m_pAttached && m_pAttached->IsAttached())?m_pAttached:NULL;
}

bool CMDShape::IsAreaShown() const
{
	return (HaveArea() && m_pAttached)?true:false;
}

void CMDShape::ShowArea(bool bShow,CPCDraftView* pView)
{
	if(bShow)
	{
		if(!m_pAttached)
		{
			m_pAttached=new CMDArea(true,this);

			CPCDraftDoc* pDoc=pView->GetDocument();
			LongRatio ratio;
			pDoc->GetActiveScaleRatio(ratio);

			CClientDC cdc(pView);
			CDraftContext dc(&cdc,&pView->m_Mapper);
			((CMDArea*)m_pAttached)->SetValues(&dc,pDoc->GetDimUnit()->calcAreaStr(GetRawArea(ratio)));
			m_pAttached->SetPenModel(GetPenModel());
			m_pAttached->SetFillModel(GetFillModel());
			//m_pAttached->ModifyTextParams(pDoc->m_TextParams,tpUseFont,&dc);
         // Infinisys Ltd
         m_pAttached->ModifyTextParams(pDoc->m_TextParams,0,&dc);
			m_pAttached->SetShapePoint(0,Middle(GetExtents()));
			m_pAttached->CalculateExtents();
			m_pAttached->SetShapeDatum(kCenterDatum);
			pDoc->CreateShape(m_pAttached);
			pView->SelectShape(m_pAttached,true);
		}
	}
	else
	{
		if(m_pAttached)
		{
			pView->UnselectShape(m_pAttached,false);
			pView->InvalidateShape(m_pAttached,true);
			pView->GetDocument()->DeleteShape(m_pAttached);
			m_pAttached=NULL;
		}
	}
}

void CMDShape::RemoveArea(CPCDraftView* pView)
{
	if(IsAreaShown())
		ShowArea(false,pView);
}

void CMDShape::DeleteAttached()
{
	if(m_pAttached)
	{
		delete m_pAttached;
		m_pAttached=NULL;
	}
}

double CMDShape::ReportCalcWidth(const LongRatio& ratio) const
{
	if(m_RotAngle!=0)
	{
		LongPoint lpt;
		CMDShape* pCopy=CreateCopy();
		pCopy->DoShapeRotate(-m_RotAngle,kAboutCenter,lpt);
		double value=pCopy->ReportCalcXDim(ratio);
		delete pCopy;
		return value;
	}
	else return ReportCalcXDim(ratio);
}

double CMDShape::ReportCalcHeight(const LongRatio& ratio) const
{
	if(m_RotAngle!=0)
	{
		LongPoint lpt;
		CMDShape* pCopy=CreateCopy();
		pCopy->DoShapeRotate(-m_RotAngle,kAboutCenter,lpt);
		double value=pCopy->ReportCalcYDim(ratio);
		delete pCopy;
		return value;
	}
	else return ReportCalcYDim(ratio);
}

double CMDShape::ReportCalcLength(const LongRatio& ratio) const
{
	double width=ReportCalcWidth(ratio);
	double height=ReportCalcHeight(ratio);
	return width>height?width:height;
}

double CMDShape::ReportCalcPerimeter(const LongRatio& ratio) const
{
	return ReportCalcLength(ratio);
}

double CMDShape::ReportCalcXDim(const LongRatio& ratio) const
{
	return ScaleDist(ratio,Width(CalculateBasicExtents()));
}

double CMDShape::ReportCalcYDim(const LongRatio& ratio) const
{
	return ScaleDist(ratio,Height(CalculateBasicExtents()));
}

double CMDShape::ReportCalcArea(const LongRatio& ratio) const
{
	return GetRawArea(ratio);
}

bool CMDShape::ReportFitsCriteria(CMDReportRow* pCriteria,CMDReportRow* pFunction)
{
	ASSERT(pCriteria!=NULL && pCriteria->m_nRowType==kCriteriaRow);
	ASSERT(pFunction!=NULL && pFunction->m_nRowType==kFunctionRow);
	for(int i=0;i<6;i++)
	{
		short nFieldRefNum=pCriteria->m_nFieldRefNum[i];
		if(nFieldRefNum!=kUnDefined && nFieldRefNum!=kResultField && nFieldRefNum!=kFunctionNameField)
		{
			CString sRowText=pFunction->m_sRowText[i];
			if(!sRowText.IsEmpty() && sRowText.CompareNoCase(m_sValues[nFieldRefNum-1]))
				return false;
		}
	}
	return true;
}

long CMDShape::ReportCalcFieldTotal(short nFieldType)
{
	long lResult=0;
	CString str; 
	int n;
	switch(nFieldType)
	{
		case kFunctTotalName: n=0; goto docheck;
		case kFunctTotalF2: n=1; goto docheck;
		case kFunctTotalF3: n=2; goto docheck;
		case kFunctTotalF4:	n=3; goto docheck;
		case kFunctTotalF5:	n=4;
docheck:
			str=m_sValues[n];
			if(!str.IsEmpty())
			{
				LPCTSTR p=str;
				while(*p && (*p>='0' && *p<='9' || *p==' ')) ++p;
				if(*p=='\0')
					lResult=atoi(str);
			}
	}
	return lResult;
}

void CMDShape::WriteDXF(CFile* pFile, CString& strLayerName, LongRectPtr pDocExtPtr, double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette* ptrPalette) const
{
}

void CMDShape::DxfWritePtCord(CFile *pFile, CString& pLayerName, LongPointPtr pPtPtr, double *pUnitFacPtr, short pDashIdx) const
{
	CString	strBuffer;
	short	nIdx1, nIdx2;
	double	vDblValX, vDblValY;

  	/* Custom Line Style */
  	if (pDashIdx) {
		CString strDashName;

		findStrFromTab (kLTYPE, strDashName, kTablesTab);
		strBuffer.Format("%3d\r\n%s%02d\r\n", kLineTypeName, strDashName, pDashIdx);
		pFile->Write(strBuffer, strBuffer.GetLength());
	}

  	nIdx1 = kPrimaryXCoord;
	nIdx2 = kPrimaryYCoord;

  	vDblValX = mtFixToReal (pPtPtr->x) / (*pUnitFacPtr);
  	vDblValY = mtFixToReal (pPtPtr->y) / (*pUnitFacPtr);

  	strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", nIdx1, vDblValX, nIdx2, vDblValY, kPrimaryZCoord, 0.0);
	pFile->Write(strBuffer, strBuffer.GetLength());
}

short CMDShape::DxfGetColorIndex(const PenModel*   pPenModelPtr, CPalette* ptrPalette) const
{
	short	vEquivColIdx = 0;  /* Black in ACAD */
	short	vColorIndex = pPenModelPtr->m_nIndex;

  	if (pPenModelPtr->m_nPenType == kPenInvis)
  		vColorIndex = 1;	// White 
  	else 
	  if (/*pPenModelPtr->m_nPenType == kPenGray ||*/ pPenModelPtr->m_nPenType == kPenPat)
  		vColorIndex = 2;	// Black 

	if (DxfEquivColIndex (ptrPalette, vColorIndex, &vEquivColIdx))
		vEquivColIdx--;

  	return (vEquivColIdx);
} /* getColorIndex */

bool CMDShape::DxfEquivColIndex (CPalette* ptrPalette1, short colIndex1,short *colIndex2) const
{
	
	
	short			i, entries;
	double			r, g, b, matchTol, newTol;
	PALETTEENTRY    acadColor, rgb1, rgb2;

	*colIndex2 = 0;
	if ( !colIndex1 )	// no fill 
		return false;

	//getPalColor(palette1,colIndex1-1,&rgb1);
	ptrPalette1->GetPaletteEntries(colIndex1-1,1,&rgb1);

	entries = palMacSystem.palNumEntries;

	for (i=0; i< entries; i++) {
		if ( rgb1.peBlue == palMacSystem.palEntries[i].peBlue &&
			rgb1.peGreen == palMacSystem.palEntries[i].peGreen &&
			rgb1.peRed == palMacSystem.palEntries[i].peRed ) {
			*colIndex2 = i+1;
			return true;
		}
	} // for i 

	for (i=0; i< entries; i++) {
		acadColor = palMacSystem.palEntries[i];
		r = (double)acadColor.peRed - (double)rgb1.peRed;
		g = (double)acadColor.peGreen - (double)rgb1.peGreen;
		b = (double)acadColor.peBlue - (double)rgb1.peBlue;

		newTol = sqrt(r*r + g*g + b*b);

		if (!i || newTol < matchTol) {
			matchTol = newTol;
			rgb2 = acadColor;
			*colIndex2 = i+1;
		}
	} // for i 

	if (*colIndex2 != 0)
		return true;		// found match for color 

	return false;		
} // equivDXFColIndex */

void CMDShape::DxfWritePolyHead (
	CFile*			pFile,
	CString&		pLayerStr,
	const PenModel*	pPenModelPtr,
	LongRect*		/*pDocExtPtr*/,
	double			pUnitFactor,
	short			pClosed ,
	CPalette*		ptrPalette) const
{
	
	short		vIdx1, vIdx2;
	double		vLnWidth;
	short		vPenColIdx;
	short		vDashIdx = pPenModelPtr->Dashing();
	CString		strIDName, strBuffer;

  	findStrFromTab (kDxfPolyline, strIDName, kEntitiesTab);

  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;

  	/* Write the Shape name and the Layer name */
  	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
  	pFile->Write(strBuffer, strBuffer.GetLength());

  	/* Custom Line Style */
  	if (vDashIdx) {
  		CString strDashName;

		findStrFromTab (kLTYPE, strDashName, kTablesTab);
		strBuffer.Format("%3d\r\n%s%02d\r\n", kLineTypeName, strDashName, vDashIdx);
		pFile->Write(strBuffer, strBuffer.GetLength());
  	}

  	/* Write the Vertices follow flag and Polyline flag */
  	vIdx1 = kEntitiesFollow;
  	vIdx2 = kOtherIntValMin;
  	strBuffer.Format("%3d\r\n%3d\r\n%3d\r\n%3d\r\n", vIdx1, 1, vIdx2, pClosed);
	pFile->Write(strBuffer, strBuffer.GetLength());
  	
  	/* Write the Pen Color */
  	vIdx1 = kColorNumber;
  	vPenColIdx = DxfGetColorIndex (pPenModelPtr, ptrPalette);
	strBuffer.Format("%3d\r\n%3d\r\n", vIdx1, vPenColIdx);
	pFile->Write(strBuffer, strBuffer.GetLength());


  	// Dont bother if it is 1 or less as thin lines often get drawn thicker than one pixel in AutoCAD. RPH
  	double thickness = (double)(DxfWidthConversion(pPenModelPtr->m_nWidth));
  	if ( thickness > 1.0 )
	{
  		/* Write the Pen thickness - starting width and the ending width */
  		vIdx1 = kOtherFloatValMin;
  		vIdx2 = kFlatVal2;
  		vLnWidth = thickness / pUnitFactor;
		strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", vIdx1, vLnWidth, vIdx2, vLnWidth);
  		pFile->Write(strBuffer, strBuffer.GetLength());
	}

} /* writePolyHead */

/*--------------------------------------------------------------------------*/

/*
  Write Poly Seqend in a DXF file.
*/

void CMDShape::DxfWriteSeqend (
	CFile*		pFile,
	CString&	pLayerStr ) const
{
	CString strIDName, strBuffer;
	short	vIdx1, vIdx2;

  	/* Write the Seqend entity */
  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;
  	findStrFromTab (kDxfSeqEnd, strIDName, kEntitiesTab);
  	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
  	pFile->Write(strBuffer, strBuffer.GetLength());
} /* writeSeqend */

void CMDShape::DxfWriteVertex (
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pPtPtr,
	double * 		pUnitFacPtr,
	double * 		/*pBulgePtr*/,
	short			/*pFlags*/,
	short			pDashIdx ) const
{
	CString strIDName, strBuffer;
	short	vIdx1, vIdx2;
	double	vDblValX, vDblValY;
	short	vErrCode = kDxfNoErr;

  	findStrFromTab (kDxfVertex, strIDName, kEntitiesTab);

  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;
  	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
	pFile->Write(strBuffer, strBuffer.GetLength());
  	
  	/* Custom Line Style */
  	if (pDashIdx) {
		CString strDashName;

		findStrFromTab (kLTYPE, strDashName, kTablesTab);
		strBuffer.Format("%3d\r\n%s%02d\r\n", kLineTypeName, strDashName, pDashIdx);
		pFile->Write(strBuffer, strBuffer.GetLength());
  	}

  	vIdx1 = kPrimaryXCoord;   vIdx2 = kPrimaryYCoord;

  	vDblValX = mtFixToReal (pPtPtr->x) / (*pUnitFacPtr);
  	vDblValY = mtFixToReal (pPtPtr->y) / (*pUnitFacPtr);
	strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", vIdx1, vDblValX, vIdx2, vDblValY, kPrimaryZCoord, 0.0);

	pFile->Write(strBuffer, strBuffer.GetLength());

} /* writeVertex */

void CMDShape::DxfDrawEndcap (
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pPtr1,
	LongPoint*		pPtr2,
	short			pEndCap,
	unsigned short	pMap,
	PenModel*		pPenModelPtr,
	bool			pIsCurve,
	Coord			outsideLineLen,
	LongRect*		pDocExtPtr,
	double			pUnitFactor,
	int				nZoomFactor,
	CPalette*		ptrPalette) const
{
	
	LongPoint		vPt1, vPt2;
	short			zPenW;
	CDrawPrim::EndcapRec*  	vEndCapPtr;
	PenModel		copyOfPenModel;

  	vPt1 = *pPtr1;
  	vPt2 = *pPtr2;

	zPenW = DxfWidthConversion(pPenModelPtr->m_nWidth);
	copyOfPenModel = *pPenModelPtr;

  	vEndCapPtr = &gEndcaps[getEndcapIndex(pEndCap, pPenModelPtr->m_nWidth)];

  	switch (vEndCapPtr->type) {
		case kArrow:
		case kVArrow:
			DxfArrowEM (pFile, pLayerStr, &vPt1, &vPt2,
						vEndCapPtr, pMap, zPenW, &copyOfPenModel,
						pIsCurve, outsideLineLen, pDocExtPtr, pUnitFactor, nZoomFactor, ptrPalette);
			break;

		case kOval:
			copyOfPenModel.m_nWidth = 1;
			DxfDotEM (pFile, pLayerStr, &vPt1, &vPt2,
					 vEndCapPtr->width, pMap, &copyOfPenModel, outsideLineLen,
	  				 pDocExtPtr, pUnitFactor, ptrPalette);
			break;

		case kX:
			DxfXEM (pFile, pLayerStr, &vPt1, &vPt2,
				   vEndCapPtr->width, pMap, zPenW, pPenModelPtr, outsideLineLen,
				   pDocExtPtr, pUnitFactor, ptrPalette);
			break;

		case kSlash:
			DxfSlashEM (pFile, pLayerStr, &vPt1, &vPt2,
					   vEndCapPtr->width, pMap, zPenW, pPenModelPtr, outsideLineLen,
					   pDocExtPtr, pUnitFactor, ptrPalette);
			break;
  	}
} /* dxfDrawEndcap */


void CMDShape::DxfDotEM (
	CFile*			pFile,
	CString&			pLayerStr,
	LongPoint*		pPtr1,
	LongPoint*		pPtr2,
	short			pWidth,
	unsigned short	pMap,
	PenModel*		pPenModelPtr,
	Coord			outsideLineLen,
	LongRect*		pDocExtPtr,
	double			pUnitFactor,
	CPalette*		ptrPalette) const
{
	
	Coord			vRadius;
	PenModel		copyOfPenModel;

	copyOfPenModel = *pPenModelPtr;
	copyOfPenModel.m_nWidth = 1;
	if (copyOfPenModel.m_nPenType != kPenColor) {
		copyOfPenModel.m_nPenType = kPenColor;
		copyOfPenModel.m_nIndex = kBlackIndex;
	}
	/* Remove the dashing bit */
	if (pPenModelPtr->Dashing())
		copyOfPenModel.m_nStyle &= ~DashingMask;

  	vRadius = _guLOrd (pWidth);

	if (pMap & kDimEndmarkOut)
		DxfOutsideDimLine (	pFile, pLayerStr, pPtr1, pPtr2,
							pMap, outsideLineLen,
							pPenModelPtr, pDocExtPtr, pUnitFactor, ptrPalette);
  	if (pMap & kEnd1)
		DxfWriteCircle (	pFile, pLayerStr, pPtr1,
					    		vRadius, &copyOfPenModel, pDocExtPtr, pUnitFactor, ptrPalette);
  	if (pMap & kEnd2)
		DxfWriteCircle (	pFile, pLayerStr, pPtr2,
						    vRadius, &copyOfPenModel, pDocExtPtr, pUnitFactor, ptrPalette);
} /* dxfDotEM */

/*--------------------------------------------------------------------*/

/*
 dxfSlashEM - draw the slash endmark at p1 and p2
*/

void CMDShape::DxfSlashEM (
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pPtr1,
	LongPoint*		pPtr2,
	short			pWidth,
	unsigned short	pMap,
	short			penWidth,
	PenModel*		pPenModelPtr,
	Coord			outsideLineLen,
	LongRect*		pDocExtPtr,
	double			pUnitFactor,
	CPalette		*ptrPalette) const
{
	
	WrFixed		angRad;
	Coord		fixHalfPen, fixMarkLen;
	LongPoint	pt1, pt2, p1, p2;
	PenModel	copyOfPenModel;

	copyOfPenModel = *pPenModelPtr;
	copyOfPenModel.m_nWidth = 1;
	if (copyOfPenModel.m_nPenType != kPenColor) {
		copyOfPenModel.m_nPenType = kPenColor;
		copyOfPenModel.m_nIndex = kBlackIndex;
	}
	/* Remove the dashing bit */
	if (pPenModelPtr->Dashing())
		copyOfPenModel.m_nStyle &= ~DashingMask;

	if (pMap & kDimEndmarkOut)
		DxfOutsideDimLine (pFile, pLayerStr, pPtr1, pPtr2,
						   pMap, outsideLineLen,
						   pPenModelPtr, pDocExtPtr, pUnitFactor, ptrPalette);

	fixMarkLen = _guLOrd(pWidth);
	fixHalfPen = _guLOrd(penWidth) >> 1;

	pt1 = *pPtr1;
	pt2 = *pPtr2;

	angRad = ATan2 ( pt1 - pt2 ) ;
	angRad += kFixedPie4;
	LongPoint UnitVec = SinCos ( angRad ) ;

	LongPoint Vec = UnitVec * fixMarkLen ;
	Vec.x = - Vec.x ;

	if (pMap & kEnd1) {
		p1 = pt1 + Vec ;
		p2 = pt1 - Vec ;

		DxfWriteALine (	pFile, pLayerStr, &copyOfPenModel, &p1, &p2,
						pDocExtPtr, pUnitFactor, ptrPalette);
	}

	if (pMap & kEnd2) {
		p1 = pt2 + Vec ;
		p2 = pt2 - Vec ;

		DxfWriteALine (	pFile, pLayerStr, &copyOfPenModel, &p1, &p2,
			    		pDocExtPtr, pUnitFactor, ptrPalette);
	}

} /* dxfSlashEM */

/*--------------------------------------------------------------------*/

/*
	dxfXEM -	draw the X endmark at p1 and p2
*/

void CMDShape::DxfXEM (
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pPtr1,
	LongPoint*		pPtr2,
	short			pWidth,
	unsigned short	pMap,
	short			penWidth,
	PenModel*   	pPenModelPtr,
	Coord			outsideLineLen,
	LongRect*		pDocExtPtr,
	double			pUnitFactor,
	CPalette*		ptrPalette) const
{
	
	WrFixed		angRad;
	Coord		fixHalfPen, fixMarkLen;
	LongPoint	pt1, pt2, p1, p2;
	PenModel	copyOfPenModel;

	copyOfPenModel = *pPenModelPtr;
	copyOfPenModel.m_nWidth = 1;
	if (copyOfPenModel.m_nPenType != kPenColor) {
		copyOfPenModel.m_nPenType = kPenColor;
		copyOfPenModel.m_nIndex = kBlackIndex;
	}
	/* Remove the dashing bit */
	if (pPenModelPtr->Dashing())
		copyOfPenModel.m_nStyle &= ~DashingMask;

	if (pMap & kDimEndmarkOut)
		DxfOutsideDimLine (	pFile, pLayerStr, pPtr1, pPtr2,
							pMap, outsideLineLen,
							pPenModelPtr, pDocExtPtr, pUnitFactor, ptrPalette);

	fixMarkLen = _guLOrd(pWidth);
	fixHalfPen = _guLOrd(penWidth) >> 1;

	pt1 = *pPtr1;
	pt2 = *pPtr2;

	angRad = ATan2 ( pt1 - pt2 ) ;
	angRad += kFixedPie4;
	LongPoint UnitVec = SinCos ( angRad ) ;

	LongPoint vec1 = UnitVec * fixMarkLen ;
	vec1.x = - vec1.x ;

	angRad += kFixedPie2;
	UnitVec = SinCos ( angRad ) ; // Could we perhaps use CosSin of original angle
		// and not have to invert the h coordinate ? 

	LongPoint vec2 = UnitVec * fixMarkLen ;
	vec2.x = - vec2.x ;

  	if (pMap & kEnd1) {
		p1 = pt1 + vec1 ;
		p2 = pt1 - vec1 ;

		DxfWriteALine (	pFile, pLayerStr, &copyOfPenModel, &p1, &p2,
						pDocExtPtr, pUnitFactor, ptrPalette);

		p1 = pt1 + vec2 ;
		p2 = pt1 - vec2 ;

		DxfWriteALine (	pFile, pLayerStr, &copyOfPenModel, &p1, &p2,
						pDocExtPtr, pUnitFactor, ptrPalette);
  	}

  	if (pMap & kEnd2) {
		p1 = pt2 + vec1 ;
		p2 = pt2 - vec1 ;

		DxfWriteALine (	pFile, pLayerStr, &copyOfPenModel, &p1, &p2,
						pDocExtPtr, pUnitFactor, ptrPalette);

		p1 = pt2 + vec2 ;
		p2 = pt2 - vec2 ;

		DxfWriteALine (	pFile, pLayerStr, &copyOfPenModel, &p1, &p2,
						pDocExtPtr, pUnitFactor, ptrPalette);
  	}
} /* dxfXEM */

/*--------------------------------------------------------------------*/

/*
dxfArrowEM -  Draw arrow at pt1, pt2 with specified width and height
	rotating it with the line .
	Fill if boolean fill is true

	outside	width
	|-------|	|----|

	o 21	11 o	-
	+	+	|
	+	200	100	+	|  height
2  o-------o 20 ---o---------o--- 10 o------o 1 -
	+	+
	+	+
	o 22	12 o

 draw a more sophisticated arrow with s pointed nose and tucked in tail
*/

void  CMDShape::DxfArrowEM (
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pPtr1,
	LongPoint*		pPtr2,
	CDrawPrim::EndcapRec*	pEndCapPtr,
	unsigned short	pMap,
	short			penWidth,	/* zoomed */
	PenModel*		pPenModelPtr,
	bool			pIsCurve,
	Coord			outsideLineLen,
	LongRect*		pDocExtPtr,
	double			pUnitFactor,
	int				nZoomFactor,
	CPalette*		ptrPalette) const
{
	
	//CPCDraftDoc*	pDoc=GetDocument();
	
	LongPoint		p10, p11, p12, p20, p21, p22, p1n, p2n, p1t, p2t, p100, p200;
	short			arrowWidth, arrowHeight, tail, nose, vZHalfPenW;
	WrFixed 		thetaRadians, fixArrowWidth, fixArrowHeight,
					fixTail, fixNose, fixOutsideW;
	short			vIdx1;
	LongPoint		vVertexPt, vPoly[5];
	uchar			style;
	short			veePen, halfPenW;			/* zoomed */
	Coord			fixHalfPen;
	Coord			fixVPen, adjust1, adjust2;
	short			type, closed, numPts;
	LongPoint		pt1, pt2, pLoc;
	PenModel		copyOfPenModel;

	copyOfPenModel = *pPenModelPtr;
	copyOfPenModel.m_nWidth = 1;
	if (copyOfPenModel.m_nPenType != kPenColor) {
		copyOfPenModel.m_nPenType = kPenColor;
		copyOfPenModel.m_nIndex = kBlackIndex;
	}
	/* Remove the dashing bit */
	if (pPenModelPtr->Dashing())
		copyOfPenModel.m_nStyle &= ~DashingMask;

  	arrowWidth =   pEndCapPtr->width;
  	arrowHeight =  pEndCapPtr->height;

  	tail =  pEndCapPtr->tail;
  	nose =  pEndCapPtr->nose;

	style = (uchar)pEndCapPtr->opcode;

  	vZHalfPenW = penWidth >> 1;
	fixHalfPen = _guLOrd(penWidth) >> 1;

	pt1 = *pPtr1;
	pt2 = *pPtr2;

	type = pEndCapPtr->type;
	if (type == kVArrow)  {
		if (!(veePen = (penWidth + 1) >> 1))  veePen = 1;
		fixVPen = _guLOrd (veePen);
		adjust1 = adjust2 = FixedZero ;
		if (pMap & kEnd1)  adjust1 = fixVPen >> nZoomFactor + 1;
		if (pMap & kEnd2)  adjust2 = fixVPen >> nZoomFactor + 1;
		DxfLineAdjust (&pt1, &pt2, adjust1, adjust2);
		halfPenW = veePen >> 1;
	}

	thetaRadians = ATan2 ( pt1 - pt2 ) ;
	if (pMap & kDimEndmarkOut)		/* reverse arrow orientation */
		thetaRadians += kFixedPie;

	LongPoint UnitVec = CosSin ( thetaRadians ) ;

	fixArrowWidth = _guLOrd(arrowWidth);
	fixArrowHeight = _guLOrd(arrowHeight);
	fixTail = _guLOrd(tail);
	fixNose = _guLOrd(nose);

  	/* do outside Endcaps */
  	if (pMap & kDimEndmarkOut)	/* reverse arrow orientation */
  	{
		fixOutsideW = outsideLineLen;

		LongPoint outVec = UnitVec * fixOutsideW ;

		if (pMap & kEnd1) {
			pLoc = pt1;

			p100 =  pLoc - outVec ;

			DxfWriteALine (	pFile, pLayerStr, pPenModelPtr, &pLoc, &p100,
							pDocExtPtr, pUnitFactor, ptrPalette);
		}

		if (pMap & kEnd2) {
			pLoc = pt2;

			p200 =  pLoc + outVec ;

			DxfWriteALine (	pFile, pLayerStr, pPenModelPtr, &pLoc, &p200,
							pDocExtPtr, pUnitFactor, ptrPalette);
		}
  	} /* end if  (pMap & kDimEndmarkOut)  */

	LongPoint w = UnitVec * fixArrowWidth ;
	LongPoint wVec = UnitVec * fixArrowHeight ;
	LongPoint noseVec = UnitVec * fixNose ;
	LongPoint tailVec = UnitVec * fixTail ;
	LongPoint penWVec = UnitVec * fixHalfPen ;

	if (pMap & kEnd1) {
		p1n = pt1 - noseVec ;

		if (pIsCurve) 
			p1n += penWVec + w - tailVec ;

		p10 = p1n - w ;

		p11.x = p10.x + wVec.y ;
		p11.y = p10.y - wVec.x ;

		p12.x = p10.x - wVec.y ;
		p12.y = p10.y + wVec.x ;

		p1t = p10 + tailVec ;

		if (style & (kFill | kFrame))		/* do arrowhead */
		{
			vPoly[0] = p1n;
			vPoly[1] = p11;
			vPoly[2] = p1t;
			vPoly[3] = p12;
			vPoly[4] = p1n;

			DxfWritePolyHead ( pFile, pLayerStr, &copyOfPenModel,
							   pDocExtPtr, pUnitFactor, 1, ptrPalette);

			for (vIdx1 = 0; vIdx1 < 5; vIdx1++) {
				/* Write the vertices */
				vVertexPt.x = vPoly[vIdx1].x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vPoly[vIdx1].y;
				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
								&pUnitFactor, 0L, 0, 0);
			} /* for vIdx1 */

			DxfWriteSeqend (pFile, pLayerStr);
		}
		else if (type == kVArrow)  {
			closed = style & kClose ? 1 : 0;
			numPts = 3;
			vPoly[0] = p11;
			vPoly[1] = p1n;
			vPoly[2] = p12;
			if (closed)
			{	vPoly[3] = p11;
				numPts = 4;
			}
			closed = style & kClose ? 1 : 0;
			DxfWritePolyHead (	pFile, pLayerStr, &copyOfPenModel,
								pDocExtPtr, pUnitFactor, closed, ptrPalette);

			for (vIdx1 = 0; vIdx1 < numPts; vIdx1++) {
				/* Write the vertices */
				vVertexPt.x = vPoly[vIdx1].x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vPoly[vIdx1].y;
				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
								&pUnitFactor, 0L, 0, 0);
			} /* for vIdx1 */

			DxfWriteSeqend (pFile, pLayerStr);
		}
	} /* pMap & kEnd1 */

	if (pMap & kEnd2) {
		p2n = pt2 + noseVec ;

		if ( pIsCurve )
			p2n -= penWVec + w - tailVec ;

		p20 = p2n + w ;

		p21.x = p20.x + wVec.y ;
		p21.y = p20.y - wVec.x ;

		p22.x = p20.x - wVec.y ;
		p22.y = p20.y + wVec.x ;

		p2t = p20 - tailVec ;

		if (style & (kFill | kFrame))		/* do arrowhead */
		{
			vPoly[0] = p2n;
			vPoly[1] = p21;
			vPoly[2] = p2t;
			vPoly[3] = p22;
			vPoly[4] = p2n;

			DxfWritePolyHead (	pFile, pLayerStr, &copyOfPenModel,
								pDocExtPtr, pUnitFactor, 1, ptrPalette);

			for (vIdx1 = 0; vIdx1 < 5; vIdx1++) {
				/* Write the vertices */
				vVertexPt.x = vPoly[vIdx1].x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vPoly[vIdx1].y;
				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
								&pUnitFactor, 0L, 0, 0);
			} /* for vIdx1 */

			DxfWriteSeqend (pFile, pLayerStr);
		}
		else if (type == kVArrow)  {
			closed = style & kClose ? 1 : 0;
			numPts = 3;
			vPoly[0] = p21;
			vPoly[1] = p2n;
			vPoly[2] = p22;
			if (closed)
			{	vPoly[3] = p21;
				numPts = 4;
			}
			closed = style & kClose ? 1 : 0;
			DxfWritePolyHead (	pFile, pLayerStr, &copyOfPenModel,
								pDocExtPtr, pUnitFactor, closed, ptrPalette);
			for (vIdx1 = 0; vIdx1 < numPts; vIdx1++) {
				/* Write the vertices */
				vVertexPt.x = vPoly[vIdx1].x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vPoly[vIdx1].y;
				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
								&pUnitFactor, 0L, 0, 0);
			} /* for vIdx1 */

			DxfWriteSeqend (pFile, pLayerStr);
		}
	} /* pMap & kEnd2 */

} /* dxfArrowEM */

/*--------------------------------------------------------------------------*/

void CMDShape::DxfOutsideDimLine(
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pPtr1,
	LongPoint*		pPtr2,
	unsigned short	pMap,
	Coord			outsideLineLen,
	PenModel*		pPenModelPtr,
	LongRect*		pDocExtPtr,
	double			pUnitFactor ,
	CPalette*		ptrPalette) const
{
	
	LongPoint		p100, p200;

	LongPoint pt1 = *pPtr1;
	LongPoint pt2 = *pPtr2;
	short penSize = DxfWidthConversion(pPenModelPtr->m_nWidth);
	WrFixed thetaRadians = ATan2 ( pt1 - pt2 ) ;
	thetaRadians += kFixedPie;
	WrFixed cosTheta = Cos ( thetaRadians ) ;
	WrFixed sinTheta = Sin ( thetaRadians ) ;

	Coord fixOutsideW = outsideLineLen;
	
	Coord outCosTheta = fixOutsideW * cosTheta ;
	Coord outSinTheta = fixOutsideW * sinTheta ;

	Coord lHalfPen = (_guLOrd (penSize)) >> 1;
	if (pMap & kEnd1)
	{
		p100.x =  pt1.x - outCosTheta;
		p100.y =  pt1.y - outSinTheta;

		DxfWriteALine (	pFile, pLayerStr, pPenModelPtr, &pt1, &p100,
						pDocExtPtr, pUnitFactor, ptrPalette);
	}

	if (pMap & kEnd2)
	{
		p200.x =  pt2.x + outCosTheta;
		p200.y =  pt2.y + outSinTheta;

		DxfWriteALine (	pFile, pLayerStr, pPenModelPtr, &pt2, &p200,
						pDocExtPtr, pUnitFactor, ptrPalette);
	}

} /* dxfOutsideDimLine */

void CMDShape::DxfWriteCircle (
	CFile*			pFile,
	CString&		pLayerStr,
	LongPoint*		pCenFdPtr,
	Coord			pRadius,
	PenModel*		pPenModelPtr,
	LongRect*		pDocExtPtr,
	double			pUnitFactor,
	CPalette*		ptrPalette) const
{
	short		vIdx1, vIdx2;
	LongPoint	vVertexPt;
	double		vRadius;
	short		vPenColIdx;
	CString		strBuffer, strIDName;
	short		vDashIdx = pPenModelPtr->Dashing();

  	findStrFromTab (kDxfCircle, strIDName, kEntitiesTab);

  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;

  	/* Write the Shape name and the Layer name */
  	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
	pFile->Write(strBuffer, strBuffer.GetLength());

	/* Write the Pen Color */
  	vIdx1 = kColorNumber;
  	vPenColIdx = DxfGetColorIndex (pPenModelPtr, ptrPalette);
  	strBuffer.Format("%3d\r\n%3d\r\n", vIdx1, vPenColIdx);
	pFile->Write(strBuffer, strBuffer.GetLength());
  	
 	/* Write the center of the circle */
  	vVertexPt.x = pCenFdPtr->x - pDocExtPtr->left;
  	vVertexPt.y = pDocExtPtr->bottom - pCenFdPtr->y;
  	DxfWritePtCord (pFile, pLayerStr, &vVertexPt,
					&pUnitFactor, vDashIdx);

  	/* Write the radius of circle */
  	vIdx1 = kOtherFloatValMin;
  	vRadius = mtFixToReal (pRadius) / pUnitFactor;
  	strBuffer.Format("%3d\r\n%.6f\r\n", vIdx1, vRadius);
	pFile->Write(strBuffer, strBuffer.GetLength());
} /* writeCircle */

void CMDShape::DxfWriteALine(CFile* pFile, CString& pLayerStr, PenModel* pPenPtr, LongPointPtr pLPt1Ptr, LongPointPtr pLPt2Ptr, LongRectPtr pDocExtPtr, double pUnitFactor, CPalette *ptrPalette) const
{
	CString		strName, strBuffer;
	short		nIdx1, nIdx2;
	LongPoint	vVertexPt;
	double		vDblValX, vDblValY;
	short		nPenColIdx;
	short		vDashIdx;

	vDashIdx = pPenPtr->Dashing();
  	findStrFromTab (kDxfLine, strName, kEntitiesTab);

  	nIdx1 = kEntryStart;
  	nIdx2 = kLayerName;

  	/* Write the Shape name and the Layer name */
  	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nIdx1, strName, nIdx2, pLayerStr);
	pFile->Write(strBuffer, strBuffer.GetLength());
  	
  	/* Write the Pen Color */
  	nIdx1 = kColorNumber;
 	nPenColIdx = DxfGetColorIndex (pPenPtr, ptrPalette);
	strBuffer.Format("%3d\r\n%3d\r\n", nIdx1, nPenColIdx);
  	pFile->Write(strBuffer, strBuffer.GetLength());

 	/* Write the first point */
  	vVertexPt.x = pLPt1Ptr->x - pDocExtPtr->left;
  	vVertexPt.y = pDocExtPtr->bottom - pLPt1Ptr->y;
	
  	DxfWritePtCord (pFile, pLayerStr, &vVertexPt, &pUnitFactor, vDashIdx);

 	/* Write the second point */
  	vVertexPt.x = pLPt2Ptr->x - pDocExtPtr->left;
  	vVertexPt.y = pDocExtPtr->bottom - pLPt2Ptr->y;

  	nIdx1 = kOtherXCoordMin;   nIdx2 = kOtherYCoordMin;

  	vDblValX = mtFixToReal (vVertexPt.x) / pUnitFactor;
  	vDblValY = mtFixToReal (vVertexPt.y) / pUnitFactor;

	strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", nIdx1, vDblValX, nIdx2, vDblValY, kOtherZCoordMin, 0.0);
	pFile->Write(strBuffer, strBuffer.GetLength());

}

void CMDShape::DxfLineAdjust(LongPointPtr p1, LongPointPtr p2, Coord adjust1, Coord adjust2) const
{
	LongPoint UnitVec = CosSin ( ATan2 ( *p1 - *p2 ) ) ;

	if ( adjust1 != FixedZero )
		*p1 -= UnitVec * adjust1 ;

	if ( adjust2 != FixedZero )
		*p2 += UnitVec * adjust2 ; // Cant we subtract adjust1 from adjust2 and do this in one go ?
}

void CMDShape::DxfWriteOneBezierSeg(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, CBezierEdge* vEdgePtr) const
{
	short				vIdx1;
	LongPoint			vVertexPt;
	short				vDashIdx;
	short				vPtCount;
	CLongPointArray		vPtHdl;

	LongPoint			bPt[4];
	short				i, j, cc[124], *ccPtr, vSeg, offsetH, offsetV;
	long				x, y;
	long				vPtPtr,	vPtAry;
	long				vDbPtPtr;
	short				vTotPtCount, vNum;

	vSeg = 1;
	vPtCount = 4;

	vDashIdx = GetPenModel().Dashing();

	vDbPtPtr = 0;

	vPtHdl.SetSize(32*vSeg+2);
	vPtPtr = vPtAry = 0;
  	
  	ccPtr = cc;
  	for ( i = 1; i < 32; i++)  {
		j = 32 - i;
		*ccPtr++ = j * j * j;
		*ccPtr++ = 3 * i * j * j;
		*ccPtr++ = 3 * i * i * j;
		*ccPtr++ = i * i * i;
  	}

  	vPtHdl[vPtPtr] = vEdgePtr->m_lptFrom;
	bPt [ 0 ] = vEdgePtr->m_lptFrom;
	bPt [ 1 ] = vEdgePtr->m_lptFromCtl;
	(bPt + 1)->x -= (bPt)->x ;
	(bPt + 1)->y -= (bPt)->y ;
	bPt [ 2 ] = vEdgePtr->m_lptToCtl;
	(bPt + 2)->x -= (bPt)->x ;
	(bPt + 2)->y -= (bPt)->y ;
	bPt [ 3 ] = vEdgePtr->m_lptTo;
	(bPt + 3)->x -= (bPt)->x ;
	(bPt + 3)->y -= (bPt)->y ;
//		} /* for j */
	offsetH = (short)mtFixToReal(bPt->x);
	offsetV = (short)mtFixToReal(bPt->y);
	for ( ccPtr = cc; ccPtr < cc + 121; ccPtr += 4)  {
		x = (long)(*(ccPtr + 1) * mtFixToReal((bPt+1)->x) +
		*(ccPtr + 2) * mtFixToReal((bPt+2)->x) + *(ccPtr + 3) * mtFixToReal((bPt+3)->x));
		y = (long)(*(ccPtr + 1) * mtFixToReal((bPt+1)->y) +
		*(ccPtr + 2) * mtFixToReal((bPt+2)->y) + *(ccPtr + 3) * mtFixToReal((bPt+3)->y));
		vPtPtr++;
		vPtHdl[vPtPtr].x = offsetH + (short)(x >> 15);
		vPtHdl[vPtPtr].y = offsetV + (short)(y >> 15);
	} /* for ccPtr */

	(bPt+3)->x += bPt->x ;
	(bPt+3)->y += bPt->y ;
	vPtPtr++;
	
	vPtHdl[vPtPtr] = *(bPt+3);

  	/* Fill the curve. */
  	vSeg <<= 5;
 	++vSeg;
  	
	vPtPtr=0;
  	vTotPtCount = vSeg;
  	while ( vTotPtCount > 0 ) {
		if ( vTotPtCount > kDxfOutMaxPolyPts )
			vNum = kDxfOutMaxPolyPts;
		else
			vNum = vTotPtCount;

		if ( vNum > 2 ) {
			for (vIdx1 = 0; vIdx1 < vNum; vIdx1++, vPtPtr++) {
				/* Write the vertices */
				vVertexPt = vPtHdl[vPtPtr];
				vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;
				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
									 &vUnitFactor, 0L, 0, vDashIdx);
			} /* for vIdx1 */
		} /* if vNum */
		else {
			vVertexPt = vPtHdl[vPtPtr];
			vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
			vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;

			DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
							&vUnitFactor, 0L, 0, vDashIdx);

			/* Send the last point again because columbo accpects a polyline
			   to have atleast three points.
			*/
			DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
						 &vUnitFactor, 0L, 0, vDashIdx);

			vPtPtr++;
			vVertexPt = vPtHdl[vPtPtr];
			vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
			vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;

			DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
							 &vUnitFactor, 0L, 0, vDashIdx);
		}

		vPtPtr--;
		vTotPtCount -= kDxfOutMaxPolyPts;

		if ( vTotPtCount ) vTotPtCount++;
  	}
}

void CMDShape::DxfGetArcPts (	CArray <LongPoint, LongPoint> &pPtHdl,
					LongPoint	&pCenter,
					Coord		pHRad,
					Coord		pVRad,
					Angle		pStartAng,
					Angle		pEndAng,
					Angle		pRot,
					bool		pIsFine,
					PenModel*	penModel) const
{
	TSinCos			fSinCosRot;
		
	if ( pHRad < FixedZero || pVRad < FixedZero )	return;

	/* draw a point if it is too small */
	if ( pHRad.GetRawDouble () < 2.0 && pVRad.GetRawDouble ()  < 2.0 )	{
		pPtHdl.Add(pCenter);
		return;
	}

	if ( pRot != FixedZero )
		fSinCosRot = TSinCos ( pRot ) ;
	else
		fSinCosRot.fSin = fSinCosRot.fCos = 0.0 ;

 	if ( pEndAng < pStartAng )	
		pEndAng += k360;
	

	Coord fPenWCoord = _guLOrd (DxfWidthConversion(penModel->m_nWidth));

	LongPoint			arcPt;
	Angle		vIncr, vAngle;

	if ( fPenWCoord > FixedZero )
	{
		fPenWCoord >>= 1;

		if ( penModel->m_nAlignment == kInsideLine )	{
			pHRad -= fPenWCoord.GetRawDouble () ;
			pVRad -= fPenWCoord.GetRawDouble () ;
		}
		else if ( penModel->m_nAlignment == kOutsideLine )	{
			pHRad += fPenWCoord.GetRawDouble () ;
			pVRad += fPenWCoord.GetRawDouble () ;
		}
	}

	/* Find an increment for the angle. */
	//vIncr = _guLOrd( 500 ) ;
	//vIncr /= (pHRad + pVRad + 65536L);
	//if ( vIncr < short ( 4 ) )	vIncr = short ( 4 ) ;
	vIncr = ChordTolerance((float)mtFixToReal(Max(pHRad,pVRad)));
	if ( !pIsFine  &&  vIncr < short ( 20 ) )	vIncr = short ( 20 );
	if ( vIncr > (pEndAng - pStartAng) )
		 vIncr = (pEndAng - pStartAng) / short ( 5 ) ;

	/* Get the starting point of the arc */
	DxfArcLongPoint (pStartAng, &arcPt, pCenter, pHRad, pVRad, fSinCosRot, pRot);
	pPtHdl.Add(LongPoint(arcPt.x - fPenWCoord, arcPt.y - fPenWCoord));

	/* Get the points along the arc. */
	vAngle = pStartAng + vIncr;
	while ( vAngle < pEndAng )	{
		DxfArcLongPoint (vAngle, &arcPt, pCenter, pHRad, pVRad, fSinCosRot, pRot);
		pPtHdl.Add(LongPoint(arcPt.x - fPenWCoord, arcPt.y - fPenWCoord));
		vAngle += vIncr;
	}

	/* Last point. */
	DxfArcLongPoint ( pEndAng, &arcPt, pCenter, pHRad, pVRad, fSinCosRot, pRot);
	pPtHdl.Add(LongPoint(arcPt.x - fPenWCoord, arcPt.y - fPenWCoord));
} /* getArcPts */

void CMDShape::DxfArcLongPoint (Angle	angle, LongPoint *arcPt, LongPoint	&pCenter, Coord pHRad, Coord pVRad, TSinCos& fSinCosRot, Angle fLongRot) const
{
	Coord vlx ;
	Coord vly ;
	//vlx.SetFromRawDoubleTruncated ( pHRad * mtCos ( angle ) ) ;
	//vly.SetFromRawDoubleTruncated ( pVRad * mtSin ( angle ) ) ;
	vlx = pHRad * mtCos ( angle );
	vly = pVRad * mtSin ( angle );
	if ( fLongRot != 0) {
		arcPt->x = WrFixedFromRawDoubleTruncated ( pCenter.x.GetRawDouble () + fSinCosRot.fCos * vlx.GetRawDouble () - fSinCosRot.fSin * vly.GetRawDouble () ) ;
		arcPt->y = WrFixedFromRawDoubleTruncated ( pCenter.y.GetRawDouble () + fSinCosRot.fSin * vlx.GetRawDouble () + fSinCosRot.fCos * vly.GetRawDouble () ) ;
	}
	else {
		arcPt->x = pCenter.x + vlx ;
		arcPt->y = pCenter.y + vly ;
	}
} /* arcLongPoint */

void CMDShape::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	pValues[6].m_Unitless=GetShapeDatum().x;
	pValues[7].m_Unitless=GetShapeDatum().y;
}

void CMDShape::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	LongPoint delta;
	delta.x=pValues[6].m_Unitless-GetShapeDatum().x;
	delta.y=pValues[7].m_Unitless-GetShapeDatum().y;
	DoShapeOffset(delta);
	CalculateExtents();
	ValidateDatum();
}

void CMDShape::DxfLineCapped(LongPoint* p1, LongPoint* p2, PenModel *penModel, short whichCap, LongPoint* newP1, LongPoint* newP2, int nZoomFactor) const
{
	CDrawPrim::EndcapRec* capPtr1, *capPtr2;
	CDrawPrim prim;


	/* unzoomed quantities */
	WrFixed					fixWidth1, fixNose1, fixTail1, fixVArrowRatio,
							fixWidth2, fixNose2, fixTail2,
							d1, d2;

	short					cap1, cap2, pmPenW, zPenW;
	LongPoint				sp1, sp2;

	fixWidth1 = fixWidth2 = d1 = d2 = FixedZero ;
	pmPenW = DxfWidthConversion(penModel->m_nWidth);
	zPenW = pmPenW;
	fixVArrowRatio = WrFixedFromRatio (3, 2) ;

	sp1 = *p1;
	sp2 = *p2;

	if ( (whichCap & kEnd1) && ((cap1 = penModel->EndCap1()) != 0) ) {
		capPtr1 = &(gEndcaps[ getEndcapIndex(cap1,pmPenW) ]);
		// if printing Zoom procedures do not modify values 

		fixWidth1 = _guLOrd(capPtr1->width);	// guLOrd converts to fixed 
		fixNose1 = _guLOrd(capPtr1->nose);
		fixTail1 = _guLOrd(capPtr1->tail);
	}
	if ( (whichCap & kEnd2) && ((cap2 = penModel->EndCap2()) != 0) ) {
		capPtr2 = &gEndcaps[ getEndcapIndex(cap2,pmPenW) ];
		fixWidth2 = _guLOrd(capPtr2->width);
		fixNose2 = _guLOrd(capPtr2->nose);
		fixTail2 = _guLOrd(capPtr2->tail);
	}

	// note Fixed format is same as longPoint format 

	if ( fixWidth1 != FixedZero )
		{
		if (capPtr1->type == kVArrow)
			d1 = ( _guLOrd (zPenW) >> nZoomFactor ) * fixVArrowRatio ;
		else
			d1 = fixWidth1 + fixNose1 - fixTail1;
		}
	if ( fixWidth2 != FixedZero )
		{
		if (capPtr2->type == kVArrow)
			d2 = ( _guLOrd (zPenW) >> nZoomFactor ) * fixVArrowRatio ;
		else
			d2 = fixWidth2 + fixNose2 - fixTail2;
		}

	DxfLineAdjust (&sp1, &sp2, d1, d2);

	*newP1 = sp1;
	*newP2 = sp2;
}

void CMDShape::DxfLineBorderPos(PenModel *penModel, bool isPrinting, LongPointPtr p1, LongPointPtr p2, LongPointPtr newP1, LongPointPtr newP2, int nZoomFactor) const
{
	Coord			lPenWidth;
	LongPoint		tmpP1, tmpP2;
	Angle			theAngle ;

	tmpP1 = *newP1 = *p1;
	tmpP2 = *newP2 = *p2;

	if (penModel->m_nAlignment == kCenterLine && ! isPrinting)
		return;

	if (penModel->m_nAlignment == kInsideLine && isPrinting)
		return;

	if (! penModel->EndCap1() || ! penModel->EndCap2()) {
		lPenWidth = _guLOrd (DxfWidthConversion(penModel->m_nWidth)) - FixedMinDelta ;
		if (isPrinting) {
			if (penModel->m_nStyle & HairlineMask)
				lPenWidth >>= 2;
			if (penModel->m_nAlignment == kCenterLine)
				lPenWidth >>= 1;
		}
		else {
			lPenWidth >>= 1;
			if (penModel->m_nAlignment == kInsideLine)
				lPenWidth = -lPenWidth;
			if (nZoomFactor) // ????
				{
				if ( nZoomFactor < 0 )
					lPenWidth >>= -nZoomFactor;
				else if (nZoomFactor > 0)
					lPenWidth <<= nZoomFactor;
				}
		}

		LongPoint UnitVec = CosSin ( ATan2(tmpP1.y - tmpP2.y , tmpP1.x - tmpP2.x) ) ;

		if (! penModel->EndCap1())
			tmpP1 += UnitVec * lPenWidth ;

		if (! penModel->EndCap2())
			tmpP2 -= UnitVec * lPenWidth ;

		*newP1 = tmpP1;
		*newP2 = tmpP2;
	}
}

void CMDShape::DxfMakeButtCaps(short penWidth, LongPoint p1, LongPoint p2, LongPoint *newP1, LongPoint *newP2) const
{
	*newP1 = p1;
	*newP2 = p2;
	Coord lPenWidth = _guLOrd(penWidth);

	if ((p1.x != p2.x) && (p1.y != p2.y))
		{
		if ( Abs ( p2.x - p1.x ) <= lPenWidth ) return ;
		if ( Abs ( p2.y - p1.y ) <= lPenWidth ) return ;
		}

	Coord lHalfPen = _guLOrd((penWidth >> 1));
	LongPoint lD = *newP1 - *newP2 ;

	Angle theAngle = mtFastATan2 ( lD ) ;
	WrFixed cosAngle = WrFixedFromFract ( mtLCos ( theAngle ) ) ;
	WrFixed sinAngle = WrFixedFromFract ( mtLSin ( theAngle ) ) ;

	newP1->y += lHalfPen * cosAngle ;
	newP1->x += lHalfPen * sinAngle ;

	newP2->y -= lHalfPen * cosAngle ;
	newP2->x -= lHalfPen * sinAngle ;
}

void CMDShape::CalcDefaultDatum(LongPoint &lpt)
{
	lpt=GetShapePoint(0);
}

void CMDShape::CalcCenterDatum(LongPoint &lpt)
{
	short num=(short)GetNumShapePoints();
	double datumX,datumY;
	datumX=datumY=0;
	for(int i=0;i<num;i++)
	{
		LongPoint lptTmp=GetShapePoint(i);
        datumX += lptTmp.x.GetRawDouble();
        datumY += lptTmp.y.GetRawDouble();
	}
	lpt.x.SetFromRawDoubleTruncated(datumX/num);
	lpt.y.SetFromRawDoubleTruncated(datumY/num);
}

void CMDShape::NormalizePoints(LongPoint *lpt)
{
	LongPoint lptRot=lpt[0];

	LongRect lrc;
	lrc.SetToBound(lptRot,mtRotPoint(lpt[2],-m_RotAngle,lptRot));
	lrc.Normalize();

	for(int i=0;i<4;i++)
	{
		lpt[i]=mtRotPoint(lrc.GetCorner(i),m_RotAngle,lptRot);
	}
}

void CMDShape::UpdateDependentShapes(CPCDraftView *pView)
{
	if(HaveArea() && m_pAttached)
	{
		CPCDraftDoc* pDoc=pView->GetDocument();
		LongRatio ratio;
		pDoc->GetActiveScaleRatio(ratio);

		pView->InvalidateShape(m_pAttached,true);
		CClientDC cdc(pView);
		CDraftContext dc(&cdc,&pView->m_Mapper);
		m_pAttached->SetValues(&dc,pDoc->GetDimUnit()->calcAreaStr(GetRawArea(ratio)));
		m_pAttached->CalculateExtents();
		pView->InvalidateShape(m_pAttached,true);
	}
}

void CMDShape::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	switch(m_PenModel.m_nPenType)
	{
	case kPenColor:
		if(m_PenModel.m_crColor & PALETTEINDEX(0))
		{
			PALETTEENTRY pe;
			pDoc->m_Palette.GetPaletteEntries(m_PenModel.m_nIndex&0xFF,1,&pe);
			m_PenModel.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
		}
		break;
	case kPenPat:
		{
			// by TSM
			if(CopyDIB && (m_PenModel.m_nIndex > 70))
			{
				//m_PenModel.m_hDib=pDoc->m_Patterns[m_PenModel.m_nIndex].CreateDIBSection();
            if (pDoc->m_packeditema[m_PenModel.m_nIndex].m_etype
               == packed_type_dib)
            {
               m_PenModel.m_hDib = pDoc->m_packeditema[m_PenModel.m_nIndex].m_pdib->CreateDIBSection();
               m_PenModel.m_nPenType = kPenPatEmbed;

            }
			}
			// end by TSM
		}
		break;
	}
}

void CMDShape::Attach(CMDDimStr* pShape)
{
	if(m_pAttached) delete m_pAttached;
	m_pAttached=pShape;
	m_pAttached->SetOwner(this);
}

CMDDimStr* CMDShape::Detach()
{
	CMDDimStr* pShape=m_pAttached;
	m_pAttached=NULL;
	return pShape;
}

void CMDShape::ValidateDatum()
{
	if(kFreeDatum==m_nDatum)
	{
		if(!GetExtents().Contains(GetShapeDatum())) SetShapeDatum(kDefaultDatum);
	}
	else SetShapeDatum(m_nDatum);
}

int CMDShape::SaveMacDraft(PhIOContext& ctx)
{
	ctx.pPhShape->fPrev=(long)this;
	ctx.pPhShape->fOwnerGroup=(long)ctx.pGroup;
	ctx.pPhShape->fShapeFlags=0;
	ctx.pPhShape->fRot=m_RotAngle;
	ctx.pPhShape->fRef=(m_pAttached && m_pAttached->IsKindOf(RUNTIME_CLASS(CMDArea)))?1:0;
	ctx.ConvertPenModel(m_PenModel,ctx.pPhShape->fPenModel);
	if(m_PenModel.m_nAccurateWidth) ctx.pPhShape->fShapeFlags|=kPhAccurateWidthFlag;
	if(m_PenModel.m_nSquareEnds) ctx.pPhShape->fShapeFlags|=kPhSquareEndsFlag;

	ctx.pPhShape->fDatum=m_lptDatum;
	switch(m_nDatum)
	{
	case kFreeDatum:
		ctx.pPhShape->fShapeFlags|=kPhFreeDatumCodeMask;
		break;
	case kCenterDatum:
		ctx.pPhShape->fShapeFlags|=kPhCtrDatumCodeMask;
		break;
	}
	if(m_bLocked) ctx.pPhShape->fShapeFlags|=kPhObjLockedMask;
	if(ctx.pGroup && ctx.pGroup->IsKindOf(RUNTIME_CLASS(CMDMetaPolyShape))) ctx.pPhShape->fShapeFlags|=kPhMetaPolyMemberMask;
	ctx.pPhShape->fObjectInfo=(m_sValues[0].IsEmpty() && m_sValues[1].IsEmpty() && m_sValues[2].IsEmpty() && m_sValues[3].IsEmpty() && m_sValues[4].IsEmpty())?0:1;
	ctx.pLinkVertexList->FlattenObjectIndex(this,ctx.nObjectIndex++);
	return 0;
}




void CMDShape::SaveMacObjInfo(PhIOContext& ctx)
{
	short count;
	if(ctx.pPhShape->fObjectInfo)
	{
		PhObjectInfo pInfo;
		pInfo.fVersion=1;
		pInfo.fSetNumber=1;
		uptr((char*)&pInfo).swaps(2);

		count=sizeof(PhObjectInfo);
        int i = 0;
		for(;i<5;i++) count+=min(m_sValues[i].GetLength()+2,32);

		CMemFile mf;
		uptr(&count).swaps(1);
		mf.Write(&count,sizeof(short));
		mf.Write(&pInfo,sizeof(PhObjectInfo));
		for(i=0;i<5;i++)
		{
			char buf[32];
			ctx.ConvertC2P(m_sValues[i],buf,31);
			buf[m_sValues[i].GetLength()+1]=0;
			mf.Write(buf,*buf+2);
		}

		long length=mf.GetLength();
		void* pl=mf.Detach();
		ctx.f->Write(pl,length);
		free(pl);
	}
	if(ctx.pPhShape->fRef || ctx.nCount)
	{
		ctx.nCount = (ctx.nCount + 3)/4;
		
		PhPropList pl;
		memset(&pl,0,sizeof(PhPropList));
		pl.fNumElems=ctx.nCount + (ctx.pPhShape->fRef ? 1:0);
		pl.fElemSize=sizeof(PhPropListElem);
		pl.fInitSize=pl.fNumElems * sizeof(PhPropListElem);
		pl.fGrowSize=sizeof(PhPropListElem);
		pl.fKeylen=2;

		count=sizeof(PhPropList) + sizeof(PhPropListElem) * (pl.fNumElems-1);
		uptr(&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));

		uptr p((char*)&pl);
		p.swaps(1); p.swapl(1); p.swaps(1);
		p.swapl(3); p.swaps(3); p.swapl(1);
		ctx.f->Write(&pl,sizeof(PhPropList) - sizeof(PhPropListElem));

		if (ctx.pPhShape->fRef)
		{
			pl.fData[0].fType=kPhShapeToAreaLink;
			pl.fData[0].fData=long(m_pAttached);
			p.swaps(1); p.swapl(1);
			ctx.f->Write(&pl.fData, sizeof(PhPropListElem));
		}
		
		for (count = 0; ctx.nCount > 0;count++)
		{
			ctx.nCount--;
			pl.fData[0].fType=600 + count;
			pl.fData[0].fData=ctx.dwProperty[count];
			uptr p((char*)&pl.fData);
			p.swaps(1); p.swapl(1);
			ctx.f->Write(&pl.fData, sizeof(PhPropListElem));
		}
	}
}

short CMDShape::DxfWidthConversion(short nPenW) const
{
	switch(nPenW) {
		case 0x81:
			return 1;
		case 0xA1:
			return 1;
		case 0xC1:
			return 1;
		default:
			return nPenW & 0x1F;
	}
}

void CMDShape::AddToLinkContext(CLinkVertexContext* pContext)
{
	pContext->Add(this);
}

bool CMDShape::IsLinked(CLinkVertexList& list,int nLinkVertex, CMDShape** pLinkedShape) const
{
	return list.IsLinkedShape(this,nLinkVertex,pLinkedShape);
}

void CMDShape::RecalcLinkedVertex(CLinkVertexContext* pContext)
{
	CLinkVertexContext theContext;
	POSITION pos=pContext->m_pList->GetHeadPosition();
	while(pos)
	{
		CLinkVertex* pAnyLinkVertex=pContext->m_pList->GetNext(pos);
		if(pAnyLinkVertex->m_pShape==this)
		{
			long nKey=pAnyLinkVertex->m_nKey;
			if(nKey<0)
			{
				POSITION pos2=pContext->m_pList->GetHeadPosition();
				while(pos2)
				{
					CLinkVertex* pThisLinkVertex=pContext->m_pList->GetNext(pos2);
					if(pThisLinkVertex->m_nKey==nKey)
					{
						CMDShape* pShape=pThisLinkVertex->m_pShape;
						if(pShape==this || pContext->Include(pShape))
							break;
						if(pShape->IsLocked())
							AfxThrowUserException();
						if(!theContext.Include(pShape))
						{
							pContext->m_pView->InvalidateShape(pShape,true);
							theContext.Add(pShape);
						}
						CLinkVertex* pFirstLinkVertex=pContext->m_pList->GetNext(pos2);
						CLinkVertex* pSecondLinkVertex=pContext->m_pList->GetNext(pos2);
						pShape->OnUpdateLineLinkedVertex(pContext->m_pView,pThisLinkVertex->m_nVertex,this,pFirstLinkVertex->m_nVertex,pSecondLinkVertex->m_nVertex);
						pContext->m_pView->InvalidateShape(pShape,true);
						break;
					}
				}
			}
			else
			{
				POSITION pos2=pContext->m_pList->GetHeadPosition();
				while(pos2)
				{
					CLinkVertex* pThisLinkVertex=pContext->m_pList->GetNext(pos2);
					if(pThisLinkVertex->m_nKey==nKey && !pContext->Include(pThisLinkVertex->m_pShape))
					{
						CMDShape* pShape=pThisLinkVertex->m_pShape;
						if(pShape->IsLocked())
							AfxThrowUserException();
						if(!theContext.Include(pShape))
						{
							pContext->m_pView->InvalidateShape(pShape,true);
							theContext.Add(pShape);
						}
						pShape->OnUpdateLinkedVertex(pContext->m_pView,pThisLinkVertex->m_nVertex,this,pAnyLinkVertex->m_nVertex);
					}
				}
			}
		}
	}
	pContext->Append(theContext);
	for(int i=theContext.GetSize();--i>=0;)
	{
		CMDShape* pShape=theContext.GetAt(i);
		pContext->m_pView->InvalidateShape(pShape,true);
		pShape->RecalcLinkedVertex(pContext);
	}
}

void CMDShape::OnUpdateLineLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex1,int nCauseVertex2)
{
}

void CMDShape::OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex)
{
	if(nCauseVertex<0 || nVertex<0) return;
	CTrackContext* pTrackContext=TrackCreateContext(nVertex, 0, &pView->m_Mapper);
	if(pTrackContext)
	{
		LongPoint lptMiddle=Middle(GetExtents());
		pTrackContext->pView=pView;
		TrackFeedback(pCauseShape->GetShapeHandle(nCauseVertex),pTrackContext);
		TrackComplete(pTrackContext);
		delete pTrackContext;
		pTrackContext = NULL;
		if(m_pAttached && m_pAttached->IsAttached())
		{
			pView->InvalidateShape(m_pAttached,true);
			m_pAttached->DoShapeOffset(Middle(GetExtents())-lptMiddle);
			pView->InvalidateShape(m_pAttached,true);
		}
		UpdateDependentShapes(pView);
		ValidateDatum();
		pView->InvalidateShape(this,true);
		pView->GetDocument()->GetActiveLayer()->RecalcGroupExtents(this);
	}
}

int CMDShape::GetShapePointIndex(const LongPoint& lpt)
{
	for(int i=0;i<GetNumShapePoints();i++)
		if(guAlmostEqualPt(GetShapePoint(i),lpt)) return i;
	return -1;
}

bool CMDShape::DeleteLinks(CLinkVertexList* pList)
{
	return pList->DeleteAllByShape(this);
}

void CMDShape::AdjustLinks(CLinkVertexList* pList,CMDShape* pShapeToReplace)
{
	pList->ReplaceShape(pShapeToReplace,this);
}

void CMDShape::ProcessDeletedShapes(CLinkVertexList *pList)
{
	POSITION pos=gReplacedShapes.GetStartPosition();
	while(pos)
	{
		CMDShape *pShape,*pNewShape;
		gReplacedShapes.GetNextAssoc(pos,pShape,pNewShape);
		pList->ReplaceShape(pShape,pNewShape);
	}
	gReplacedShapes.RemoveAll();

	while(gDeletedShapes.GetCount())
	{
		CMDShape* pShape=gDeletedShapes.RemoveHead();
		pShape->DeleteLinks(pList);
		delete pShape;
	}
}

CMDShape* CMDShape::ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex)
{
	for(int i=0;i<GetNumShapeEdges();i++)
	{
		CEdge* pShapeEdge=GetShapeEdge(i);
		if(pEdge->IsEqual(pShapeEdge))
		{
			nFromVertex=GetShapePointIndex(pEdge->m_lptFrom);
			nToVertex=GetShapePointIndex(pEdge->m_lptTo);
			if(nFromVertex<0 || nToVertex<0) continue;
			delete pShapeEdge;
			return this;
		}
		delete pShapeEdge;
	}
	return NULL;

}

// Library - PCDraft 6 - Infinisys Ltd.
void CMDShape::ProcessShapesForLibraryArchiving(CVersionDoc *pDocNew,CVersionDoc *pDoc,int* used)
{
   if(m_PenModel.m_nPenType == kPenPat)
   {

      if(used[m_PenModel.m_nIndex] < 0)
      {
         used[m_PenModel.m_nIndex] = pDocNew->m_packeditema.GetSize();
         pDocNew->m_packeditema.Add(pDoc->m_packeditema[m_PenModel.m_nIndex]);
         m_PenModel.m_nIndex = used[m_PenModel.m_nIndex];
      }
      else
      {
         m_PenModel.m_nIndex = used[m_PenModel.m_nIndex];
      }

   }
}

// Library - PCDraft 6 - Infinisys Ltd.
void CMDShape::ProcessShapesFromLibraryArchiving(CPCDraftDoc *pDoc,CPCDraftDoc *pDocLib)
{
   if(m_PenModel.m_nPenType == kPenPat)
   {
      if(pDocLib->m_packeditema[m_PenModel.m_nIndex].m_etype == packed_type_dib)
      {
         HBITMAP hDib = pDocLib->m_packeditema[m_PenModel.m_nIndex].m_pdib->CreateDIBSection();
         int found = -1;
         int hash = CalculateDIBHash(hDib);
         for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
         {
            if(pDoc->m_packeditema[k].m_etype == packed_type_dib)
            {
               if(hash == pDoc->m_PatternsHash[pDoc->m_packeditema[k].get_index(pDoc)])
               {
                  HBITMAP hDib2 = pDoc->m_Patterns[pDoc->m_packeditema[k].get_index(pDoc)].CreateDIBSection();
                  if(!CompareDIBs(hDib,hDib2))
                  {
                     ::DeleteObject(hDib2);
                     found = k;
                     break;
                  }
                  ::DeleteObject(hDib2);
               }
            }
         }
         if(found >= 0)
         {
            m_PenModel.m_nIndex = found;
         }
         else
         {
            CPackedItem item;
            item.m_etype = packed_type_dib;
            item.m_pdib = &pDoc->m_Patterns[pDoc->m_numDIBPatterns];
            m_PenModel.m_nIndex = pDoc->m_packeditema.GetCount();
            pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
            pDoc->m_Patterns[pDoc->m_numDIBPatterns++].FromDIBSection(hDib);
            pDoc->m_packeditema.Add(item);
         }
         ::DeleteObject(hDib);
      }
      else if(pDocLib->m_packeditema[m_PenModel.m_nIndex].m_etype == packed_type_gradient)
      {
         CGradientPack  * pgrad = pDocLib->m_packeditema[m_PenModel.m_nIndex].m_pgradient;
         int found = -1;
         for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
         {
            if(pDoc->m_packeditema[k].m_etype == packed_type_gradient)
            {
               if(*pgrad == pDoc->m_Gradients[pDoc->m_packeditema[k].get_index(pDoc)])
               {
                  found = k;
                  break;
               }
            }
         }
         if(found >= 0)
         {
            m_PenModel.m_nIndex = found;
         }
         else
         {
            CPackedItem item;
            item.m_etype = packed_type_gradient;
            item.m_pgradient = &pDoc->m_Gradients[pDoc->m_numGradients];
            m_PenModel.m_nIndex =pDoc->m_packeditema.GetCount();
            pDoc->m_Gradients[pDoc->m_numGradients++] = *pgrad;
            pDoc->m_packeditema.Add(item);
         }
      }
   }
}



void CMDShape::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	if(m_PenModel.m_nPenType == kPenPat) used[m_PenModel.m_nIndex] = 1;
}

void CMDShape::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	if(m_PenModel.m_nPenType == kPenPat) {
		m_PenModel.m_nIndex = used[m_PenModel.m_nIndex ];
	}
	if(m_PenModel.m_nPenType == kPenPatEmbed) {
		m_PenModel.m_nPenType = kPenPat;
		HBITMAP hDib = m_PenModel.m_hDib;
		int found = 0;
		int hash = CalculateDIBHash(hDib);
		for(int k=1; k <= pDoc->m_numDIBPatterns; k++) {
			if(hash == pDoc->m_PatternsHash[k-1]) {
				HBITMAP hDib2 = pDoc->m_Patterns[k-1].CreateDIBSection();
				if(!CompareDIBs(hDib, hDib2)) {
					::DeleteObject(hDib2);
					found = k; break;
				}
				::DeleteObject(hDib2);
			}
		}
		if(found) {
			m_PenModel.m_nIndex = found-1;
			::DeleteObject(hDib);
		} else {
			m_PenModel.m_nIndex = pDoc->m_numDIBPatterns;
			pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
			pDoc->m_Patterns[pDoc->m_numDIBPatterns++].FromDIBSection(hDib);
			::DeleteObject(hDib);
		}
	}
}

void CMDShape::ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used)
{
   if(m_PenModel.m_nPenType == kPenPat)
	{
      if(pitem[m_PenModel.m_nIndex].m_etype == packed_type_dib)
      {
         m_PenModel.m_nPenType = kPenPatEmbed;
         m_PenModel.m_hDib = pitem[m_PenModel.m_nIndex].m_pdib->CreateDIBSection();
      }
      else if(pitem[m_PenModel.m_nIndex].m_etype == packed_type_gradient)
      {
         m_PenModel.m_nIndex = used[m_PenModel.m_nIndex];
      }
   }
}


void CMDShape::ModifyFillModel(const FillModel& fillModel,int nWhat)
{
   
   FillModel fm = GetFillModel();

   fm.ModifyFillModel(fillModel,nWhat);
 
   SetFillModel(fm);

}