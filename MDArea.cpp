// MDArea.cpp: implementation of the CMDArea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDArea.h"

#include "DraftContext.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDArea,CMDDimStr,1)

//////////////////////////////////////////////////////////////////////

CMDArea::CMDArea(bool bAttached,CMDShape* pOwner):
CMDDimStr(bAttached,pOwner)
{
   // System Font - Infinisys Ltd
   //#ifdef _JAPANESE
   //m_lf.lfCharSet=SHIFTJIS_CHARSET;
   //lstrcpy(m_lf.lfFaceName,"MS UI Gothic");
   //#else
   //m_lf.lfCharSet=DEFAULT_CHARSET;
   //lstrcpy(m_lf.lfFaceName,"Arial");
   //#endif
   // System Font - Infinisys Ltd
   m_lf.lfCharSet=ANSI_CHARSET;
   lstrcpy(m_lf.lfFaceName,"");

}


void CMDArea::Serialize(CArchive& ar)
{
	CMDDimStr::Serialize(ar);
}

void CMDArea::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
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

void CMDArea::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
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

int CMDArea::SaveMacDraft(PhIOContext& ctx)
{
	{
		PhDSArea ps;
		ps.fID=PhIDArea;
		ctx.pPhShape=(PhDSFillShape*)&ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps.fStatus=(m_bAttached)?kPhDimStrAttached:0;
	/*	switch(m_nTextDisplay)
		{
		case kAbove:
			ps.fStatus|=kPhDimStrAbove;
		case kAlongLine:
			ps.fStatus|=kPhDimStrOnSlope;
			break;
		}*/
		ps.fShapeHdl=long(m_pOwner);
		ps.fDimStrHdl=long(this)+1;

		uptr p((char*)&ps);
		p.swapDSShape();

		p.swaps(1);
		p.skip(12);
		p.swapl(2);

		ushort count=sizeof(PhDSArea)<<8;	// swapped
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(&ps,sizeof(PhDSArea));
		SaveMacObjInfo(ctx);
	}

	{
		PhDSDimStr ps;
		ps.fID=PhIDDimString;
		ctx.pPhShape=(PhDSFillShape*)&ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps.fPrev=long(this)+1;
		ps.fStatus=kPhDimVisible|kPhAreaStr;
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
		ps.fObjHdl=(long)this;
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
	}
	return 2;
}
