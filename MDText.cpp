// MDText.cpp: implementation of the CMDText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDText.h"
#include "MDRectangle.h"

#include "gu.h"
#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "PCDraftView.h"
#include "PCDraftDoc.h"
#include "PhIOStruct.h"
#include "dxfutil.h"
#include "DraftContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDText,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDText::CMDText() : m_nType(kTextAutoSize)
{
}

CMDText::CMDText(TextType nType) : m_nType(nType)
{
}

void CMDText::Serialize(CArchive& ar)
{
	int i;

	CMDFillShape::Serialize(ar);
	
	if(ar.IsLoading())
	{
		ar >> i;
		m_nType=TextType(i);
		for(i=0;i<4;i++)
			ar >> m_ptCorner[i];
		ar >> m_TextParams;
	}
	else
	{
		ar << m_nType;
		for(i=0;i<4;i++)
			ar << m_ptCorner[i];
		ar << m_TextParams;
	}
	m_StyledText.Serialize(ar);
}

void CMDText::SetTextParams(const TextParams& tp)
{
	m_TextParams=tp;
	ModifyTextParams(tp,tpUseFont| tpUseVAlign |tpUseSpacing/*|tpUseCase*/,nullptr);
}

void CMDText::ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC)
{
	if(nWhat&tpUseFont)
	{
		CStyledTextStyle style;
		for(int i=0;i<m_StyledText.GetSize();i++)
		{
			m_StyledText.GetStyleAt(i,style);
			style.m_LogFont=tp.m_lf;
			m_StyledText.SetAt(i,style);
		}
	}
	if(nWhat&tpUseCase) m_StyledText.SetCase(tp.m_nCase);
	m_TextParams.Modify(tp,nWhat);
	if(pDC && m_nType==kTextAutoSize)
	{
		point bounds(0,0);
		int pos=m_StyledText.GetFirstTextPart();
		int lastBaseLine=-1;
		int lastBaseDescent=0;
		while(pos!=-1)
		{
			
         double lineAscent,baseAscent,baseDescent,width;

			pos=pDC->CalcLineExtent(m_StyledText,pos,lineAscent,baseAscent,baseDescent,width,true, true,bounds.x);

			if(lastBaseLine==-1)
			{

				lastBaseLine=lineAscent;

			}
			else
			{
				int n=lastBaseDescent+lineAscent;
				switch(m_TextParams.m_nSpacing)
				{
					case kSingleSpace: break;
					case k1_5Space: n=(n*3)/2; break;
					case kDoubleSpace: n=n*2; break;
					default:
						ASSERT(FALSE);
				}
				lastBaseLine+=n;
			}
			lastBaseDescent=baseDescent;
			if(width>bounds.x) bounds.x=width;
		}
		if(lastBaseLine==-1)
		{
			pDC->SelectFont(m_TextParams.m_lf);
			TEXTMETRIC tm;
			pDC->GetTextMetrics(&tm);
			pDC->UnselectFont();
			bounds.x=1;
			bounds.y=tm.tmAscent+tm.tmDescent+tm.tmExternalLeading;
		}
		else
		{
			bounds.y=lastBaseLine+lastBaseDescent;
		}

		CMapper* pMapper=pDC->GetMapper();
		LongRect lrc(0,0,pMapper->DPtoLP(bounds.x)+6,pMapper->DPtoLP(bounds.y)+6);
		lrc.Offset(GetShapePoint(0));
		for(int i=1;i<4;i++)
			SetShapePoint(i,mtRotPoint(lrc.GetCorner(i),m_RotAngle,GetShapePoint(0)));
	}
}

LongPoint CMDText::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<4);
	return m_ptCorner[nIndex];
}

void CMDText::SetShapePoint(int nIndex, const LongPoint &pt)
{
	ASSERT(nIndex>=0 && nIndex<4);
	m_ptCorner[nIndex]=pt;
}

LongPoint CMDText::GetShapeHandle(int nIndex) const
{
	if(nIndex<4) return CMDShape::GetShapeHandle(nIndex);
	ASSERT(nIndex>=4 && nIndex<8);
	return Average(m_ptCorner[nIndex%4],m_ptCorner[(nIndex+1)%4]);
}

void CMDText::DrawShape(CDraftContext* pDC) const
{
   
   CMDFillShape::DrawFill(pDC);

   UseRect useRect(pDC, (CMDShape *) this);

   if(m_PenModel.m_nPenType!=kPenInvis && (m_PenModel.m_nPenType==kPenPatEmbed || m_PenModel.m_nIndex>=0))
	{
      
      UsePath usepath(pDC);

		//CRgn rgn;
		//if(GetFillRgn(&rgn,pDC))
      if (GetFillRgn(NULL, pDC))
		{
			LongPoint ptCorner[4];
			ptCorner[0]=m_ptCorner[0];
			ptCorner[1]=m_ptCorner[1];
			ptCorner[2]=m_ptCorner[2];
			ptCorner[3]=m_ptCorner[3];
			if(m_nType!=kTextAutoSize && m_TextParams.m_nVAlign!=kAlignTop)
			{
				ptCorner[0]=mtRotPoint(ptCorner[0],-m_RotAngle,m_ptCorner[0]);
				ptCorner[1]=mtRotPoint(ptCorner[1],-m_RotAngle,m_ptCorner[0]);
				ptCorner[2]=mtRotPoint(ptCorner[2],-m_RotAngle,m_ptCorner[0]);
				ptCorner[3]=mtRotPoint(ptCorner[3],-m_RotAngle,m_ptCorner[0]);
				
				CRect rc;
				pDC->CalcBoundRect(m_StyledText,m_TextParams,rc,false, false,pDC->GetMapper()->LPtoDP(ptCorner[1].x-ptCorner[0].x-6));
				LongRect lrc=pDC->GetMapper()->DPtoLP(rc);
				lrc.Inset(-3,-3);
				if(m_TextParams.m_nVAlign==kAlignMiddle)
				{
					WrFixed dy=((ptCorner[2].y-ptCorner[0].y)-(lrc.bottom-lrc.top))>>1;
					ptCorner[0].y=ptCorner[1].y=ptCorner[0].y+dy;
					ptCorner[2].y=ptCorner[3].y=ptCorner[2].y+lrc.bottom-lrc.top;
				}
				else
				{
					ptCorner[0].y=ptCorner[1].y=ptCorner[3].y-(lrc.bottom-lrc.top);
				}

				ptCorner[0]=mtRotPoint(ptCorner[0],m_RotAngle,m_ptCorner[0]);
				ptCorner[1]=mtRotPoint(ptCorner[1],m_RotAngle,m_ptCorner[0]);
				ptCorner[2]=mtRotPoint(ptCorner[2],m_RotAngle,m_ptCorner[0]);
				ptCorner[3]=mtRotPoint(ptCorner[3],m_RotAngle,m_ptCorner[0]);
			}
			pDC->SetPen(m_PenModel);
			pDC->SetTextParams(m_TextParams);
			//pDC->DrawStyledText(m_PenModel.m_iPenOpacity, m_StyledText,ptCorner,m_nType==kTextAutoSize,&rgn);
         pDC->DrawStyledText(m_PenModel.m_iPenOpacity, m_StyledText, ptCorner, m_nType == kTextAutoSize, nullptr);
		}
	}
}

void CMDText::DrawHandles(CDraftContext* pDC) const
{
	CMDFillShape::DrawHandles(pDC);

	if(m_nType==kTextUserSize)
	{
		pDC->SetGreyPen();
		DrawShapeOutline(pDC);
	}
}

void CMDText::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->MoveTo(GetShapePoint(3));
	for(int i=0;i<4;i++)
	{
		pDC->LineTo(GetShapePoint(i));
	}
}

bool CMDText::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	LongPoint lpt=pMapper->DPtoLP(pt);

	LongPoint pt1,pt2;
	pt1=GetShapePoint(0);
	pt2=GetShapePoint(1);
	bool result=guPtInPoly(kBeginPtInPoly,&lpt,&pt1,&pt2);
	pt1=pt2; pt2=GetShapePoint(2);
	result=result || guPtInPoly(kMorePtInPoly,&lpt,&pt1,&pt2);
	pt1=pt2; pt2=GetShapePoint(3);
	result=result || guPtInPoly(kMorePtInPoly,&lpt,&pt1,&pt2);
	pt1=pt2; pt2=GetShapePoint(0);
	result=result || guPtInPoly(kEndPtInPoly,&lpt,&pt1,&pt2);
	return result;
}

void CMDText::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongRatio xRatio,yRatio;
	xRatio.fN=(lrcTo.right-lrcTo.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrcTo.bottom-lrcTo.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();

	if(m_nType==kTextAutoSize && (FixedOne*xRatio)==(FixedOne*yRatio))
	{
		LongPoint lptMiddle=Middle(lrcFrom);
		DoShapeOffset(-lptMiddle);
		DoShapeScale(yRatio);
		DoShapeOffset(Middle(lrcTo));
	}
	else
	{
		LongPoint lptMiddle=Average(m_ptCorner[0],m_ptCorner[2]);
		LongPoint lptOffset=lptMiddle-lrcFrom.GetCorner(0);
		lptOffset.x*=xRatio;
		lptOffset.y*=yRatio;
		lptOffset+=lrcTo.GetCorner(0);
		DoShapeOffset(lptOffset-lptMiddle);

		if(yRatio.fN&0x80000000)
		{
			DoShapeFlip(true);
			CalculateExtents();
		}
	}
}

void CMDText::DoShapeScale(const LongRatio& ratio,double factor)
{
	if(ratio.fN==ratio.fD) return;	// do nothing if no scaling, also prevents divide by zero

	m_StyledText.DoScale(ratio,0 /*factor*/);	// it seems that we should ignore factor for text shapes

	double nHeight=m_TextParams.m_lf.lfHeight;
//	if(factor) nHeight*=factor;
	nHeight=(nHeight*ratio.fN)/ratio.fD;
	m_TextParams.m_lf.lfHeight=(long)nHeight;
	if(m_TextParams.m_lf.lfHeight==0) m_TextParams.m_lf.lfHeight=1;

	CMDFillShape::DoShapeScale(ratio,0 /*factor*/);
}

void CMDText::DoShapeMirror(const LongPoint &lptRef1, const LongPoint &lptRef2)
{
	LongBoundRect lrc;
	for(int i=0;i<GetNumShapePoints();i++) lrc|=m_ptCorner[i];
	LongPoint lptCenter=Middle(lrc);
	LongPoint delta;
	guNearPtLnPt(lptCenter,lptRef1,lptRef2,&delta);
	delta-=lptCenter;
	delta<<=1;
	DoShapeOffset(delta);
}

void CMDText::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	CMDFillShape::DoShapeFlip(bVertical,pLpt);
	NormalizePoints(m_ptCorner);
}

void CMDText::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	CMDFillShape::MakeSelfContained(pDoc,CopyDIB);
	m_StyledText.MakeSelfContained(pDoc->m_Palette);
}

CTrackContext* CMDText::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CTextTrackContext* p=new CTextTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
    int i = 0;
	for(;i<4;i++)
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

		for(i=0;i<4;i++)
			p->pt[i]=mtRotPoint(p->pt[i], pCtx->m_Angle, lptRot);
	}
	return p;
}

void CMDText::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CTextTrackContext* p=(CTextTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle==kMoveHandle)
	{
		for(int i=0;i<4;i++) p->pt[i]+=pt;
	}
	else
	{
		if(m_RotAngle!=0)
		{
			for(int i=0;i<4;i++)
				p->pt[i]=mtRotPoint(p->pt[i],-m_RotAngle);
		}

		if(m_nType==kTextAutoSize)
		{
			LongPoint pt0=GetShapePoint(0),pt2=GetShapePoint(2);
			if(m_RotAngle!=0)
			{
				pt=mtRotPoint(pt,-m_RotAngle);
				pt0=mtRotPoint(pt0,-m_RotAngle);
				pt2=mtRotPoint(pt2,-m_RotAngle);
			}
			LongRect lrcExtents(pt0,pt2);
			LongPoint size=lrcExtents.GetSize();
			LongPoint delta=pt-lrcExtents.GetCorner(p->nWhichHandle^2);
			delta=Abs(delta);
			double xRatio=delta.x.GetRawDouble()/size.x.GetRawDouble();
			double yRatio=delta.y.GetRawDouble()/size.y.GetRawDouble();
			if(xRatio<yRatio) delta.y=delta.x*(size.y.GetRawDouble()/size.x.GetRawDouble());
			else delta.x=delta.y*(size.x.GetRawDouble()/size.y.GetRawDouble());

			switch(p->nWhichHandle)
			{
			case 0:
				delta.y=-delta.y;
				delta.x=-delta.x;
				break;
			case 1:
				delta.y=-delta.y;
				break;
			case 2:
				break;
			case 3:
				delta.x=-delta.x;
				break;
			}

			switch(p->nWhichHandle)
			{
			case 0:
				p->pt[0].x=p->pt[3].x=lrcExtents.right+delta.x;
				p->pt[0].y=p->pt[1].y=lrcExtents.bottom+delta.y;
				break;
			case 1:
				p->pt[1].x=p->pt[2].x=lrcExtents.left+delta.x;
				p->pt[0].y=p->pt[1].y=lrcExtents.bottom+delta.y;
				break;
			case 2:
				p->pt[1].x=p->pt[2].x=lrcExtents.left+delta.x;
				p->pt[2].y=p->pt[3].y=lrcExtents.top+delta.y;
				break;
			case 3:
				p->pt[0].x=p->pt[3].x=lrcExtents.right+delta.x;
				p->pt[2].y=p->pt[3].y=lrcExtents.top+delta.y;
				break;
			}
		}
		else
		{
			if(m_RotAngle!=0)
				pt=mtRotPoint(pt,-m_RotAngle);

			LongPoint delta=pt-p->pt[nHandle % 4];

			LongPoint delta0=LongPoint::kZeroPt;
			LongPoint delta2=LongPoint::kZeroPt;

			switch(nHandle)
			{
			case 0:
				delta0=delta;
				break;
			case 1:
				delta0.y=delta.y; delta2.x=delta.x;
				break;
			case 2:
				delta2=delta;
				break;
			case 3:
				delta0.x=delta.x; delta2.y=delta.y;
				break;
			case 4:
				delta0.y=delta.y;
				break;
			case 5:
				delta2.x=delta.x;
				break;
			case 6:
				delta2.y=delta.y;
				break;
			case 7:
				delta0.x=delta.x;
				break;
			}
			p->pt[0]+=delta0; p->pt[2]+=delta2;
			p->pt[3].x=p->pt[0].x;
			p->pt[1].y=p->pt[0].y;
			p->pt[1].x=p->pt[2].x;
			p->pt[3].y=p->pt[2].y;
		}

		if(m_RotAngle!=0)
		{
			for(int i=0;i<4;i++)
				p->pt[i]=mtRotPoint(p->pt[i],m_RotAngle);
		}
	}
}

void CMDText::TrackShowSize(CTrackContext* pTrackContext)
{
	CTextTrackContext* p=(CTextTrackContext*)pTrackContext;
	p->pView->SetShowSize(0,1,Pythag(p->pt[1]-p->pt[0]));
	p->pView->SetShowSize(1,2,Pythag(p->pt[2]-p->pt[1]));
}

void CMDText::TrackComplete(CTrackContext* pTrackContext)
{
	CTextTrackContext* p=(CTextTrackContext*)pTrackContext;
	if(m_nType==kTextAutoSize)
	{
		LongRect lrcFrom,lrcTo;
		if(m_RotAngle!=0)
		{
			lrcFrom.Set(mtRotPoint(GetShapePoint(0),-m_RotAngle),mtRotPoint(GetShapePoint(2),-m_RotAngle));
			lrcTo.Set(mtRotPoint(p->pt[0],-m_RotAngle),mtRotPoint(p->pt[2],-m_RotAngle));
		}
		else
		{
			lrcFrom.Set(GetShapePoint(0),GetShapePoint(2));
			lrcTo.Set(p->pt[0],p->pt[2]);
		}
		LongRatio ratio;
		ratio.fN=(lrcTo.right-lrcTo.left).GetFixed();
		ratio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
		DoShapeScale(ratio,0);

		{
			CPCDraftDoc* pDoc=pTrackContext->pView->GetDocument();
			CMapper* pMapper=&pTrackContext->pView->m_Mapper;
			CClientDC theDC(pTrackContext->pView);
			CDraftContext dc(&theDC,pMapper,pDoc);
			CRect rc;
			int nZoomFactor=pDoc->m_nZoomFactor;
			pDoc->m_nZoomFactor=0;
			dc.CalcBoundRect(m_StyledText,m_TextParams,rc, true);
			LongRect lrc=pMapper->DPtoLP(rc);
			pDoc->m_nZoomFactor=nZoomFactor;
			LongPoint lptTopLeft=GetShapePoint(0);
			lrc.Set(lptTopLeft,Width(lrc)+6,Height(lrc)+6);
			for(int i=0;i<4;i++)
				SetShapePoint(i,mtRotPoint(lrc.GetCorner(i),m_RotAngle,lptTopLeft));
		}

		LongPoint ptCorner=lrcTo.GetCorner(0);
		if(m_RotAngle!=0)
		{
			ptCorner=mtRotPoint(ptCorner,m_RotAngle);
		}
		DoShapeOffset(ptCorner-GetShapePoint(0));
	}
	else
	{
		int i;
		for(i=0;i<4;i++)
			p->pt[i]=mtRotPoint(p->pt[i],-m_RotAngle/*,p->pt[0]*/);
		LongRect rc;
		rc.Set(p->pt[0],p->pt[2]);
		rc.Normalize();
		for(i=0;i<4;i++)
			p->pt[i]=mtRotPoint(rc.GetCorner(i),m_RotAngle/*,rc.GetCorner(0)*/);
		for(i=0;i<4;i++)
			SetShapePoint(i,p->pt[i]);
		CalculateExtents();
	}
}

void CMDText::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CTextTrackContext* p=(CTextTrackContext*)pTrackContext;
	pDC->MoveTo(p->pt[3]);
	for(int i=0;i<4;i++) pDC->LineTo(p->pt[i]);
}

bool CMDText::ModifyPenModel(const PenModel &penModel, int nWhat)
{
	bool bResult=CMDShape::ModifyPenModel(penModel,nWhat & ~pmUseWidth);
	if(nWhat&pmUsePenType)
	{
		if(penModel.m_nPenType==kPenColor)
		{
			CStyledTextStyle style;
			for(int i=0;i<m_StyledText.GetSize();i++)
			{
				m_StyledText.GetStyleAt(i,style);
				style.m_TextColor=penModel.m_crColor;
				m_StyledText.SetAt(i,style);
			}
		}
	}
	return bResult;
}

int CMDText::SaveMacDraft(PhIOContext& ctx)
{
	int textLen=m_StyledText.GetSize();
	if(textLen==0) return 0; // crash protection for old files - generally illegal condition

	PhDSText ps;
	ps.fID=PhIDText;
	ctx.pPhShape=&ps;
	CMDFillShape::SaveMacDraft(ctx);

	ps.fParagraph=m_nType==kTextUserSize;
	ps.fObjOpen=0;
	ps.fZoomTEH=0;
	ps.fNoZoomCopy=0;
	ps.fZoomFactor=0;
	switch(m_TextParams.m_nSpacing)
	{
	case kSingleSpace:
		ps.fTextSpace=-3;
		break;
	case k1_5Space:
		ps.fTextSpace=-2;
		break;
	case kDoubleSpace:
		ps.fTextSpace=-1;
		break;
	}
	ps.fDummy=0;
	switch(m_TextParams.m_nJust)
	{
	case kAlignLeft:
		ps.fTextJust=0;
		break;
	case kAlignCenter:
		ps.fTextJust=1;
		break;
	case kAlignRight:
		ps.fTextJust=-1;
		break;
	}
	switch(m_TextParams.m_nVAlign)
	{
	default:
	case kAlignTop:
		break;
	case kAlignMiddle:
		ps.fShapeFlags|=(1<<kPhVertTextAlignCodeShift);
		break;
	case kAlignBottom:
		ps.fShapeFlags|=(2<<kPhVertTextAlignCodeShift);
		break;
	}
	
	long oldRef = ctx.pPhShape->fRef;
	ctx.pPhShape->fRef = 1;
   uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm
	p.skip(2); p.swapl(2); p.swaps(1); p.skip(2); p.swaps(1);
    int i = 0;
	for(;i<4;i++)
	{
		ps.fPoints[i].fFlags=kPhControlVertex;
		ps.fPoints[i].fLongPt=m_ptCorner[i];
		ps.fPoints[i].fUse=0;
		p.skip(2); p.swaplpt(1);	// DBPoint
	}

	ushort count=sizeof(PhDSText)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSText));
	ctx.pPhShape->fRef = oldRef;
	
	PhTextData pt;
	memset(&pt,0,sizeof(PhTextData));
	if(m_RotAngle==0)
	{
		pt.fViewRect.l=pt.fDestRect.l=(short)ctx.pDoc->LPtoDP(m_ptCorner[0].x);
		pt.fViewRect.t=pt.fDestRect.t=(short)ctx.pDoc->LPtoDP(m_ptCorner[0].y);
		pt.fViewRect.r=pt.fDestRect.r=(short)ctx.pDoc->LPtoDP(m_ptCorner[2].x);
		pt.fViewRect.b=pt.fDestRect.b=(short)ctx.pDoc->LPtoDP(m_ptCorner[2].y);
	}
   pt.fTELength=textLen;

	char* pText=new char[textLen];
	memcpy(pText,m_StyledText.GetTextPtr(),textLen);
	for(i=0;i<textLen;i++)
	{
		if(pText[i]==0xA) pText[i]=0xD;
	}

	short nStyleCount=0;
	bool bLineFeed;
	CString text;
   CString theTextPlus1;
	CStyledTextStyle s;
	ScrpSTElement ste;
	CMemFile mf;
	int pos=m_StyledText.GetFirstTextPart();
	ctx.nCount = 0;
	memset(ctx.cCharset, 0, sizeof(ctx.cCharset));
   while(pos<m_StyledText.GetSize())
	{
		nStyleCount++;
		ste.scrpStartChar=pos;
		m_StyledText.GetNextTextPart(pos,text,theTextPlus1, s,bLineFeed);

		ctx.ConvertLogFont(s.m_LogFont,ste.scrpFont,ste.scrpSize,ste.scrpFace);
		if (ctx.nCount<=400)
			ctx.cCharset[ctx.nCount++]=s.m_LogFont.lfCharSet;

		ste.scrpHeight=abs(s.m_LogFont.lfHeight);
		ste.scrpAscent=ste.scrpSize;
/*		ste.scrpHeight=abs(s.m_LogFont.lfHeight);
		ste.scrpAscent=abs(s.m_LogFont.lfHeight);
		ste.scrpFont=ctx.UseFont(s.m_LogFont.lfFaceName)+1;
		ste.scrpFace=0;
		if(s.m_LogFont.lfWeight==FW_BOLD) ste.scrpFace|=0x1;
		if(s.m_LogFont.lfItalic) ste.scrpFace|=0x2;
		if(s.m_LogFont.lfUnderline) ste.scrpFace|=0x4;
		ste.scrpDummy=0;
		ste.scrpSize=abs(s.m_LogFont.lfHeight);
*/		COLORREF c=s.m_TextColor;
		if(c&0xFF000000)
		{
			PALETTEENTRY pe;
			ctx.pPalette->GetPaletteEntries(c&0xFF,1,&pe);
			c=RGB(pe.peRed,pe.peGreen,pe.peBlue);
		}
		ste.scrpColor.red=GetRValue(c)<<8;
		ste.scrpColor.green=GetGValue(c)<<8;
		ste.scrpColor.blue=GetBValue(c)<<8;
		uptr p((char*)&ste);
		p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
		mf.Write(&ste,sizeof(ScrpSTElement));
	}

	SaveMacObjInfo(ctx);

	pt.fNZScrpSize=sizeof(short)+nStyleCount*sizeof(ScrpSTElement);
	p.pc=(char*)&pt;
	p.swapl(4); p.swaps(1); p.swapl(1);
	ctx.f->Write(&pt,sizeof(PhTextData));

	ctx.f->Write(pText,textLen);
	delete[] pText;

	p.pc=(char*)&nStyleCount;
	p.swaps(1);
	ctx.f->Write(&nStyleCount,sizeof(short));

	long length=mf.GetLength();
	void* pl=mf.Detach();
	ctx.f->Write(pl,length);
	free(pl);

	return 1;
}

void CMDText::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	CString strBuffer, strIDName;
	short	vIdx3, vIdx4, vPenColIdx;
	double	vDocOffsetX, vDocOffsetY,vDblValX,vDblValY;
	LongPoint	vBasePt;

	CString theText, theStyledText;
   CString theTextPlus1;
	CStyledTextStyle style;


	bool bLineFeed,bLastLineFeed=false;
	int nWidth=(int)mtFixToReal(Pythag(m_ptCorner[1]-m_ptCorner[0])+6), nOffset1=0;
	
	int oldPos = 0;
	int pos=m_StyledText.GetFirstTextPart();

	int lastBaseLine=-1;
	int lastBaseDescent=0,n = 0;
	while(pos!=-1)
	{
		oldPos = pos;
		m_StyledText.GetNextTextPart(pos, theStyledText, theTextPlus1, style, bLineFeed);
		double lineAscent,baseAscent,baseDescent,width;
		
		CWindowDC hDC(NULL);
		CMapper mapper;
		CDraftContext draftContext(&hDC,&mapper,NULL);
		draftContext.SelectFont(style.m_LogFont);
		draftContext.CalcLineExtent(m_StyledText,oldPos,lineAscent,baseAscent,baseDescent,width,true,true,nWidth);
		int i=0;
		while (i<theStyledText.GetLength()){
			width=0;
			theText="";
			if (bLastLineFeed)
				nOffset1=0;
			for(;i<theStyledText.GetLength();i++) {
				CSize size=hDC.GetTextExtent(((LPCTSTR)theStyledText)+i,1);
				if (nOffset1+width+size.cx > nWidth && theText.GetLength()) {
					bLineFeed = true;
					break;
				}
				theText += theStyledText.GetAt(i);
				width+=size.cx;
			}

			if(lastBaseLine==-1)
				lastBaseLine=lineAscent;
			else {
				n=lastBaseDescent+lineAscent;
				switch(m_TextParams.m_nSpacing)	{
					case kSingleSpace: break;
					case k1_5Space: n=(n*3)/2; break;
					case kDoubleSpace: n=n*2; break;
					default:
						ASSERT(FALSE);
				}
			}
			if (bLastLineFeed)
				lastBaseLine+=n;
			bLastLineFeed = bLineFeed;
			lastBaseDescent=baseDescent;
			int offset;
			switch(m_TextParams.m_nJust) {
				case kAlignLeft: 
					offset=0; 
					break;
				case kAlignCenter: 
					offset=(nWidth-width)/2;  
					break;
				case kAlignRight: 
					offset=nWidth-width; 
					break;
				default:
					ASSERT(FALSE);
			}
			//vBasePt = m_pMapper->LPtoDP(lptCorners[0]+mtRotPoint(LongPoint(m_pMapper->DPtoLP(offset),m_pMapper->DPtoLP(lastBaseLine)),rotAngle))
			if (!theText.GetLength())
				continue;

			HDC hDC=GetDC(NULL);
			DWORD nSize=MulDiv(abs(style.m_LogFont.lfHeight),72,GetDeviceCaps(hDC,LOGPIXELSY));
			ReleaseDC(NULL,hDC);

			vBasePt = m_ptCorner[0]+mtRotPoint(LongPoint(offset+nOffset1,lastBaseLine+(double)nSize),GetRotation());

  			vDocOffsetX = mtFixToReal(pDocExtPtr->left);
  			vDocOffsetY = mtFixToReal(pDocExtPtr->bottom);

			vIdx3 = kEntryStart;
			vIdx4 = kLayerName;
			findStrFromTab (kDxfText, strIDName, kEntitiesTab);
		
			strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx3, strIDName, vIdx4, pLayerStr);
			pFile->Write(strBuffer, strBuffer.GetLength());

  			/* Write the Pen Color */
  			vIdx3 = kColorNumber;
			vPenColIdx = ptrPalette->GetNearestPaletteIndex(style.m_TextColor);
			DxfEquivColIndex (ptrPalette, vPenColIdx, &vPenColIdx);
			if (vPenColIdx)
				vPenColIdx--;
			strBuffer.Format("%3d\r\n%3d\r\n", vIdx3, vPenColIdx);
			pFile->Write(strBuffer, strBuffer.GetLength());

			vIdx3 = kPrimaryXCoord;   vIdx4 = kPrimaryYCoord;
			vDblValX = ((double)mtFixToReal(vBasePt.x) - vDocOffsetX) / vUnitFactor;
			vDblValY = (vDocOffsetY - (double)mtFixToReal(vBasePt.y)) / vUnitFactor;
			strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n",
							 vIdx3, vDblValX, vIdx4, vDblValY, kPrimaryZCoord, 0.0);
			pFile->Write(strBuffer, strBuffer.GetLength());

			vIdx3 = kOtherFloatValMin;  /* Height */
			vDblValX = (double)nSize / vUnitFactor;
			strBuffer.Format("%3d\r\n%.6f\r\n", vIdx3, vDblValX);
			pFile->Write(strBuffer, strBuffer.GetLength());

			vIdx3 = kTextValue;  /* text value */
			strBuffer.Format("%3d\r\n", vIdx3);
			pFile->Write(strBuffer, strBuffer.GetLength());
			pFile->Write( theText, theText.GetLength());
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
			vDblValX = ((double)mtFixToReal(vBasePt.x+width) - vDocOffsetX) / vUnitFactor;
			vDblValY = (vDocOffsetY - (double)mtFixToReal(vBasePt.y)) / vUnitFactor;
			strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n",
							vIdx3, vDblValX, vIdx4, vDblValY, kOtherZCoordMin, 0.0);
			pFile->Write(strBuffer, strBuffer.GetLength());

			nOffset1 += width;

		}
		draftContext.UnselectFont();
	}
}

void CMDText::OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex)
{
	if(pCauseShape->IsKindOf(RUNTIME_CLASS(CMDRectangle)))
	{ // get rotation if linked to text
		SetRotation(pCauseShape->GetRotation());
	}
	CMDFillShape::OnUpdateLinkedVertex(pView,nVertex,pCauseShape,nCauseVertex);
}

void CMDText::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	if(kTextUserSize==m_nType)
	{
		vertexes.Add(TestHandles(pt,pMapper));
		shapes.Add(this);
	}
}

