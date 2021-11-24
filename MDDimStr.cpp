// MDDimStr.cpp: implementation of the CMDDimStr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDDimStr.h"

#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "DimUnit.h"
#include "dxfutil.h"

#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDDimStr,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDDimStr::CMDDimStr(bool bAttached,CMDShape* pOwner) : m_nAlign(kDSCenter), m_bAttached(bAttached), m_pOwner(pOwner)
{
	memset(&m_lf,0,sizeof(LOGFONT));
	m_lf.lfHeight=-16;
	m_lf.lfWeight=FW_NORMAL;
	m_lf.lfOutPrecision=OUT_TT_ONLY_PRECIS;
	m_lf.lfQuality=PROOF_QUALITY;
	m_lf.lfPitchAndFamily=VARIABLE_PITCH|FF_SWISS;

#ifdef _JAPANESE
   
   m_lf.lfCharSet=SHIFTJIS_CHARSET;
	
   lstrcpy(m_lf.lfFaceName,"MS UI Gothic"); // System Font - Infinisys Ltd

#else
   
   m_lf.lfCharSet=ANSI_CHARSET;
	
   lstrcpy(m_lf.lfFaceName,"Segoe UI"); // System Font - Infinisys Ltd

#endif
   
}

void CMDDimStr::Serialize(CArchive& ar)
{
	CMDFillShape::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_sValue >> m_sTol1 >> m_sTol2;
		ar >> m_lptAnchor;
		ar >> m_lptExtent;
		ar >> m_nAlign;
		for(int i=0;i<4;i++) ar >> m_lpt[i];
		ar.Read(&m_lf,sizeof(LOGFONT));
        BYTE dataGet;
        ar >> dataGet;
        m_bAttached = dataGet == TRUE;
		ar >> m_pOwner;
	}
	else 
	{
		ar << m_sValue << m_sTol1 << m_sTol2;
		ar << m_lptAnchor;
		ar << m_lptExtent;
		ar << m_nAlign;
		for(int i=0;i<4;i++) ar << m_lpt[i];
		ar.Write(&m_lf,sizeof(LOGFONT));
		ar << (BYTE) ( m_bAttached ? 1:0);
		ar << m_pOwner;
	}
}

LongRect CMDDimStr::CalculateBasicExtents() const
{
	LongBoundRect lrc;
	for(int i=0;i<4;i++) lrc|=m_lpt[i];
	return lrc;
}

const TextParams& CMDDimStr::GetTextParams() const
{
static TextParams gTextParams;
	gTextParams.m_lf=m_lf;
	return gTextParams;
}

void CMDDimStr::ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC)
{
	if(nWhat&tpUseFont) m_lf=tp.m_lf;
	if(pDC)
	{
		RecalcExtent(pDC);
		CalcCorners();
	}
}

LongPoint CMDDimStr::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex==0);
	return m_lptAnchor;
}

void CMDDimStr::SetShapePoint(int nIndex, const LongPoint &pt)
{
	ASSERT(nIndex==0);
	m_lptAnchor=pt;
	CalcCorners();
}

void CMDDimStr::DrawShape(CDraftContext* pDC) const
{
   CMDFillShape::DrawFill(pDC);

   pDC->SetPen(m_PenModel);
   pDC->DrawDimText(m_sValue,m_sTol1,m_sTol2,m_lpt,m_lf);
#ifdef _DEBUG // System Font - Infinisys Ltd
   LOGFONT lf;
   pDC->GetCDC()->GetCurrentFont()->GetLogFont(&lf);
   TRACE("Font Name: %s", lf.lfFaceName);
#endif
}

void CMDDimStr::DrawHandles(CDraftContext* pDC) const
{
	for(int i=0;i<4;i++) pDC->DrawHandle(m_lpt[i],m_bLocked);
}

void CMDDimStr::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->MoveTo(m_lpt[3]);
	for(int i=0;i<4;i++) pDC->LineTo(m_lpt[i]);
}

void CMDDimStr::GetValues(CString& sValue,CString& sTol1,CString& sTol2)
{
	sValue=m_sValue;
	sTol1=m_sTol1;
	sTol2=m_sTol2;
}

void CMDDimStr::SetValues(CDraftContext* pDC,const CString& sValue,const CString& sTol1,const CString& sTol2)
{
	m_sValue=sValue;
	m_sTol1=sTol1;
	m_sTol2=sTol2;
	RecalcExtent(pDC);
	CalcCorners();
}

void CMDDimStr::CalcCorners()
{
	LongRect lrc;
	LongPoint lptHalfExt=m_lptExtent>>1;
	switch(m_nAlign)
	{
	case kDSCenter:
		lrc.SetToBound(m_lptAnchor-lptHalfExt,m_lptAnchor+lptHalfExt);
		break;
	case kDSLeft:
		lrc.Set(m_lptAnchor.x,m_lptAnchor.y-lptHalfExt.y,m_lptAnchor.x+m_lptExtent.x,m_lptAnchor.y+lptHalfExt.y);
		break;
	case kDSRight:
		lrc.Set(m_lptAnchor.x-m_lptExtent.x,m_lptAnchor.y-lptHalfExt.y,m_lptAnchor.x,m_lptAnchor.y+lptHalfExt.y);
		break;
	case kDSTop:
		lrc.Set(m_lptAnchor.x-lptHalfExt.x,m_lptAnchor.y,m_lptAnchor.x+lptHalfExt.x,m_lptAnchor.y+m_lptExtent.y);
		break;
	case kDSBottom:
		lrc.Set(m_lptAnchor.x-lptHalfExt.x,m_lptAnchor.y-m_lptExtent.y,m_lptAnchor.x+lptHalfExt.x,m_lptAnchor.y);
		break;
	default:
		ASSERT(0);
	}
	for(int i=0;i<4;i++) m_lpt[i]=mtRotPoint(lrc.GetCorner(i),m_RotAngle,m_lptAnchor);
}

void CMDDimStr::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	CMDShape::DoShapeMirror(lptRef1,lptRef2);
	if(m_RotAngle>90 && m_RotAngle<=270)
	{
		m_RotAngle=mtFixAngle(m_RotAngle-180);
		CalcCorners();
	}
}

CTrackContext* CMDDimStr::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	if(nWhichHandle==kRotateHandle) return NULL;
	CDimStrTrackContext* p=new CDimStrTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	for(int i=0;i<4;i++) p->lpt[i]=m_lpt[i];
	return p;
}

void CMDDimStr::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CDimStrTrackContext* p=(CDimStrTrackContext*)pTrackContext;
	if(p->nWhichHandle==kMoveHandle)
	{
		for(int i=0;i<4;i++) p->lpt[i]+=pt;
	}
}

void CMDDimStr::TrackComplete(CTrackContext* pTrackContext)
{
	CDimStrTrackContext* p=(CDimStrTrackContext*)pTrackContext;
	for(int i=0;i<4;i++) m_lpt[i]=p->lpt[i];
	CalculateExtents();
}

void CMDDimStr::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CDimStrTrackContext* p=(CDimStrTrackContext*)pTrackContext;
	pDC->MoveTo(p->lpt[0]);
	for(int i=1;i<4;i++)
		pDC->LineTo(p->lpt[i]);
	pDC->LineTo(p->lpt[0]);

}

void CMDDimStr::RecalcExtent(CDraftContext *pDC)
{
	LongPoint lptValueExt,lptTol1Ext,lptTol2Ext;
	lptValueExt=pDC->GetTextExtent(m_sValue,&m_lf);
	lptTol1Ext=pDC->GetTextExtent(m_sTol1,&m_lf,true);
	lptTol2Ext=pDC->GetTextExtent(m_sTol2,&m_lf,true);
	Coord textGap=2;
	if(pDC->GetDimStandard())
	{
		LongRatio ratio;
		ratio.fN=pDC->GetDimStandard()->nToleranceSpaceScale;
		ratio.fD=100;
		lptTol1Ext.y+=(lptTol1Ext.y*ratio-lptTol1Ext.y)>>1;
		lptTol2Ext.y+=(lptTol2Ext.y*ratio-lptTol2Ext.y)>>1;
		textGap=pDC->GetMapper()->Scalar2Long(pDC->GetDimStandard()->DimTextGap);
	}
	m_lptExtent.x=lptValueExt.x+max(lptTol1Ext.x,lptTol2Ext.x)+(textGap<<1);
	m_lptExtent.y=max(lptValueExt.y,lptTol1Ext.y+lptTol2Ext.y)+4;
}

void CMDDimStr::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	CString strBuffer, strIDName, strValue;
	short	vIdx3, vIdx4, vPenColIdx;
	double	vDocOffsetX, vDocOffsetY,vDblValX,vDblValY;
	WrFixed x,y;
	LongPoint lPoint;

	LOGFONT lfDXF=m_lf;
	HDC hDC=GetDC(NULL);
	lfDXF.lfHeight=MulDiv(lfDXF.lfHeight,72,GetDeviceCaps(hDC,LOGPIXELSY));
	ReleaseDC(NULL,hDC);

	Angle rotAngle=mtAngle(m_lpt[0],m_lpt[1]);
	LongPoint lptCenter=Average(m_lpt[0],m_lpt[2]);
	int nWidth=(int)mtFixToReal(Pythag(m_lpt[1]-m_lpt[0]));
	int nHeight=(int)mtFixToReal(Pythag(m_lpt[2]-m_lpt[1]));

	LongPoint lptRef,lptExtent;
	CSize size;
	Coord textGap;
//	if(m_pDimStd) 
//		textGap=m_pMapper->Scalar2Long(m_pDimStd->DimTextGap);
//	else 
		textGap=3;

	for (int i = 0; i < 3; i++) {

		switch (i) {
			case 0: {
					CWindowDC hDC(NULL);
					CFont font;
					font.CreateFontIndirect(&lfDXF);
					CFont* pOldFont = hDC.SelectObject(&font);
					size = hDC.GetTextExtent(m_sValue, m_sValue.GetLength());
					lptExtent = LongPoint( (int) size.cx, (int) size.cy);
					hDC.SelectObject(&pOldFont);
					lptRef=Average(m_lpt[0],m_lpt[3])+mtRotPoint(LongPoint(textGap,-(lptExtent.y>>1)),rotAngle);
					x			= lptRef.x;
					y			= lptRef.y;
					strValue	= m_sValue;
				}
				break;
			case 1: 
				{
					CWindowDC hDC(NULL);
					CFont font;
					font.CreateFontIndirect(&lfDXF);
					CFont* pOldFont = hDC.SelectObject(&font);
					size = hDC.GetTextExtent(m_sTol1, m_sTol1.GetLength());
					lptExtent = LongPoint((int)size.cx, (int)size.cy);
					hDC.SelectObject(&pOldFont);
					if(m_sTol2.GetLength()==0) 
						lptRef=Average(m_lpt[1],m_lpt[2])+mtRotPoint(LongPoint(-textGap,-(lptExtent.y>>1)),rotAngle);
					else {
						if (m_sValue.GetLength() == 0) 
							lptRef=Average(m_lpt[0],m_lpt[1])+mtRotPoint(LongPoint(lptExtent.x>>1,3),rotAngle);
						else {
/*							if (m_pDimStd) {
								LongRatio ratio;
								ratio.fN=m_pDimStd->nToleranceSpaceScale;
								ratio.fD=100;
								lptExtent.y+=(lptExtent.y*ratio-lptExtent.y)>>1;
							}*/
							lptRef=Average(m_lpt[1],m_lpt[2])+mtRotPoint(LongPoint(-textGap,-lptExtent.y),rotAngle);
						}
					}
					x = lptRef.x - lptExtent.x;
					y = lptRef.y;
					strValue	= m_sTol1;
				}
				break;
			case 2: 
				{
					CWindowDC hDC(NULL);
					CFont font;
					font.CreateFontIndirect(&lfDXF);
					CFont* pOldFont = hDC.SelectObject(&font);
					size = hDC.GetTextExtent(m_sTol2, m_sTol2.GetLength());
					lptExtent = LongPoint((int)size.cx, (int)size.cy);
					hDC.SelectObject(&pOldFont);
					if (m_sValue.GetLength()==0) 
						lptRef=Average(m_lpt[2],m_lpt[3])+mtRotPoint(LongPoint(lptExtent.x>>1,-3),rotAngle);
					else {
/*						if(m_pDimStd) {
							LongRatio ratio;
							ratio.fN=m_pDimStd->nToleranceSpaceScale;
							ratio.fD=100;
							lptExtent.y+=(lptExtent.y*ratio-lptExtent.y)>>1;
						}*/
						lptRef=Average(m_lpt[1],m_lpt[2])+mtRotPoint(LongPoint(-textGap,lptExtent.y),rotAngle);
					}
				
					x			= lptRef.x - lptExtent.x;
					y			= lptRef.y - lptExtent.y;
					strValue	= m_sTol2;
				}
				break;
			default :
				ASSERT(0);
		}

		if ( strValue.GetLength() == 0)
			continue;
		
  		vDocOffsetX = mtFixToReal(pDocExtPtr->left);
  		vDocOffsetY = mtFixToReal(pDocExtPtr->bottom);

		vIdx3 = kEntryStart;
		vIdx4 = kLayerName;
		findStrFromTab (kDxfText, strIDName, kEntitiesTab);
		
		strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx3, strIDName, vIdx4, pLayerStr);

		pFile->Write(strBuffer, strBuffer.GetLength());

  		/* Write the Pen Color */
  		vIdx3 = kColorNumber;
  		vPenColIdx = DxfGetColorIndex (&GetPenModel(), ptrPalette);
		strBuffer.Format("%3d\r\n%3d\r\n", vIdx3, vPenColIdx);
		pFile->Write(strBuffer, strBuffer.GetLength());

		vIdx3 = kPrimaryXCoord;   vIdx4 = kPrimaryYCoord;

		vDblValX = ((double)mtFixToReal(x) - vDocOffsetX) / vUnitFactor;
		vDblValY = (vDocOffsetY - (double)mtFixToReal(y)) / vUnitFactor;
		strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n",
				 vIdx3, vDblValX, vIdx4, vDblValY, kPrimaryZCoord, 0.0);
			
		pFile->Write(strBuffer, strBuffer.GetLength());

		vIdx3 = kOtherFloatValMin;  /* Height */
		vDblValX = (double)abs(lfDXF.lfHeight) / vUnitFactor;

		strBuffer.Format("%3d\r\n%.6f\r\n", vIdx3, vDblValX);
		pFile->Write(strBuffer, strBuffer.GetLength());

		vIdx3 = kTextValue;  /* text value */
		strBuffer.Format("%3d\r\n", vIdx3);

		pFile->Write(strBuffer, strBuffer.GetLength());

		pFile->Write( strValue, strValue.GetLength());

		strBuffer.Format("\r\n");
		pFile->Write(strBuffer, strBuffer.GetLength());

		if (mtFixToReal(GetRotation())) {
			vIdx3 = kAngleMin;          // Rotation angle 
			strBuffer.Format("%3d\r\n%.6f\r\n", vIdx3, mtFixToReal(GetRotation()));
			pFile->Write(strBuffer, strBuffer.GetLength());
		}
						
		vIdx3 = kHorizAlign;	// Horizontal justification :72 
		strBuffer.Format("%3d\r\n%3d\r\n", vIdx3, kFitAlign);
		pFile->Write(strBuffer, strBuffer.GetLength());

		vIdx3 = kOtherXCoordMin;   vIdx4 = kOtherYCoordMin;
		vDblValX = ((double)mtFixToReal(x+size.cx) - vDocOffsetX) / vUnitFactor;
		vDblValY = (vDocOffsetY - (double)mtFixToReal(y)) / vUnitFactor;
		strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n",
						vIdx3, vDblValX, vIdx4, vDblValY, kOtherZCoordMin, 0.0);
		pFile->Write(strBuffer, strBuffer.GetLength());
	}
}

void CMDDimStr::DXFWriteFrame(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	LongPoint lp1, lp2;
	PenModel penModel = GetPenModel();
	for (int i = 0; i < 4; i++) {
		lp1 = m_lpt[i];
		lp2 = (i == 3) ? m_lpt[0]: m_lpt[i+1];
		DxfWriteALine(pFile, pLayerStr, &penModel, &lp1, &lp2, pDocExtPtr, vUnitFactor, ptrPalette);
	}
}

int CMDDimStr::SaveMacDraft(PhIOContext& ctx)
{
	PhDSDimStr ps;
	ps.fID=PhIDDimString;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDFillShape::SaveMacDraft(ctx);

	ps.fStatus=kPhDimVisible;
	if(m_bAttached) ps.fStatus|=kPhDimStrAttached;
/*	switch(m_nTextDisplay)
	{
	case kAbove:
		ps.fStatus|=kPhDimStrAbove;
	case kAlongLine:
		ps.fStatus|=kPhDimStrOnSlope;
		break;
	}*/
	Style nStyle;
	ctx.ConvertLogFont(m_lf,ps.fFont,ps.fFontSize,nStyle);
	ps.fAnchorPt.fUse=0;
	ps.fAnchorPt.fFlags=kPhRealVertex;
	ps.fAnchorPt.fLongPt=m_lptAnchor;
	ps.fStrOffset.fUse=0;
	ps.fStrOffset.fFlags=kPhRealVertex;
	ps.fStrOffset.fLongPt=LongPoint::kZeroPt;
	ps.fObjHdl=(long)m_pOwner;
	ctx.ConvertC2P(m_sValue,ps.fString,kDimStrSize);

	uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm

	p.swaps(3);
	p.swapDBPoint(2);
	p.swapl(1);
	p.skip(kDimStrSize);

	ushort count=sizeof(PhDSDimStr)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSDimStr));
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDDimStr::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
}

Gdiplus::GraphicsPath* CMDDimStr::GetClip(CDraftContext *pDC)
{
	//CRgn* pClip=NULL;
   Gdiplus::GraphicsPath * pClip = NULL;
	if(m_FillModel.m_nFillType==kNoFill)
	{
      pDC->path_new(false);
		GetFillRgn(nullptr,pDC);
      pClip = pDC->path_detach();
	}
	return pClip;
}

bool CMDDimStr::PointInFill(const pointd& pt, CMapper *pMapper) const
{
	CDC cdc;
	cdc.CreateCompatibleDC(NULL);
	CDraftContext dc(&cdc,pMapper);
	//CRgn rgn;
   {
      UsePath usepath(&dc);
         if (GetFillRgn(nullptr, &dc) && dc.path()->IsVisible((FLOAT)pt.x, (FLOAT)pt.y)) return true;
   }
	// also check handles
	pointd spt;
	for(int i=0;i<4;i++)
	{
		spt=pMapper->LPtoDPF(m_lpt[i]);
		CRect rc(__point(spt),__point(spt));
		rc.InflateRect(kTrackHandleSize,kTrackHandleSize);
		if(rc.PtInRect(__point(pt))) return true;
	}
	return false;
}
