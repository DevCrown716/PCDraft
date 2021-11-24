// Infinisys Ltd

#include "stdafx.h"
#include "MDAngularDim.h"
#include "MDArcShape.h"
#include "MDArea.h"
#include "MDAssocDim.h"
#include "MDBezierShape.h"
#include "MDBitmap.h"
#include "MDCenterDim.h"
#include "MDCircle.h"
#include "MDCubicShape.h"
#include "MDDimLine.h"
#include "MDDimStr.h"
#include "MDEllipse.h"
#include "MDFillShape.h"
#include "MDGroupShape.h"
#include "MDLineDim.h"
#include "MDLineShape.h"
#include "MDMetaPolyShape.h"
#include "MDParallelLine.h"
#include "MDParallelPolygon.h"
#include "MDPolygonShape.h"
#include "MDPrintDlg.h"
#include "MDRAdialDim.h"
#include "MDRectangle.h"
#include "MDReport.h"
#include "MDRndRectangle.h"
#include "MDShape.h"
#include "MDText.h""

#include "phmath.h"
#include "gu.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "LinkVertex.h"
#include "dxfutil.h"
#include "Edge.h"
#include "PhIOStruct_55.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
using namespace format55;

int CMDAngularDim::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSAngularDim ps;
   ps.fID=PhIDAngularDim;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDAssocDim::SavePCDraft55(ctx);

   ps.refPt1.fUse=0;
   ps.refPt1.fFlags=kPhRealVertex;
   ps.refPt1.fLongPt=m_adp.lptFrom;
   ps.refPt2.fUse=0;
   ps.refPt2.fFlags=kPhRealVertex;
   ps.refPt2.fLongPt=m_adp.lptTo;
   ps.dimLineStart.fUse=0;
   ps.dimLineStart.fFlags=0;
   ps.dimLineStart.fLongPt=m_adp.lptDimStart;
   ps.dimLineEnd.fUse=0;
   ps.dimLineEnd.fFlags=0;
   ps.dimLineEnd.fLongPt=m_adp.lptDimEnd;
   ps.anchorPt.fUse=0;
   ps.anchorPt.fFlags=kPhRealVertex;
   ps.anchorPt.fLongPt=m_adp.lptAnchor;
   ps.extPt.fUse=0;
   ps.extPt.fFlags=0;
   ps.extPt.fLongPt=m_adp.lptExtent;
   ps.centerPt.fUse=0;
   ps.centerPt.fFlags=kPhRealVertex | kPhNoHighlight;
   ps.centerPt.fLongPt=m_adp.lptCenter;
   ps.witnessStart1.fUse=0;
   ps.witnessStart1.fFlags=0;
   ps.witnessStart1.fLongPt=m_adp.lptWit1Start;
   ps.witnessEnd1.fUse=0;
   ps.witnessEnd1.fFlags=0;
   ps.witnessEnd1.fLongPt=m_adp.lptWit1End;
   ps.witnessStart2.fUse=0;
   ps.witnessStart2.fFlags=0;
   ps.witnessStart2.fLongPt=m_adp.lptWit2Start;
   ps.witnessEnd2.fUse=0;
   ps.witnessEnd2.fFlags=0;
   ps.witnessEnd2.fLongPt=m_adp.lptWit2End;
   ps.fRadius=Pythag(m_adp.lptAnchor - m_adp.lptCenter);
   ps.fStartAngle1=mtATan2(m_adp.lptFrom - m_adp.lptCenter);
   ps.fArcAngle1=mtATan2(m_adp.lptTo - m_adp.lptCenter) - ps.fStartAngle1;
   ps.fCaps1=0;
   ps.fStartAngle2=mtATan2(m_adp.lptTo - m_adp.lptCenter);
   ps.fArcAngle2=mtATan2(m_adp.lptExtent - m_adp.lptCenter) - ps.fStartAngle2;
   ps.fCaps2=0;

   uptr p((char*)&ps);
   p.swapDSShape();

   p.swaps(3); // fm
   p.swaps(3);	p.skip(12);
   p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   p.swaps(1); p.swapl(10); // dim std
   p.swaplpt(8);
   p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);
   p.swapDBPoint(11);
   p.swapl(3); p.swaps(1); p.swapl(2); p.swaps(1);

   short count=sizeof(PhDSAngularDim);
   uptr(&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSAngularDim));
   SaveMacDimLabel(ctx);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDArcShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   if(m_nMode == kElliptic)
   {
      PhDSEArc ps;
      ps.fID=PhIDEArc;
      ctx.pPhShape=&ps;
      CMDFillShape::SavePCDraft55(ctx);

      ps.fMethod=kPhEllipse;
      ps.fDummy=0;
      ps.fCenter.fUse=0;
      ps.fCenter.fFlags=kPhRealVertex;
      ps.fCenter.fLongPt=GetCenter();
      LongPoint radii=GetRadii();
      ps.fHRadius=radii.x;
      ps.fVRadius=radii.y;

      ps.fPoint[0].fUse=0;
      ps.fPoint[0].fFlags=kPhRealVertex;
      ps.fPoint[1].fUse=0;
      ps.fPoint[1].fFlags=kPhRealVertex;

      if(m_nArcDirection == AD_CLOCKWISE)
      {
         ps.fPoint[0].fLongPt=m_pt[2];
         ps.fPoint[1].fLongPt=m_pt[1];
      }
      else
      {
         ps.fPoint[0].fLongPt=m_pt[1];
         ps.fPoint[1].fLongPt=m_pt[2];
      }
      ps.fStartAng=mtFixAngle(k90 - guLPtToLAngle(ps.fHRadius,ps.fVRadius,ps.fPoint[0].fLongPt,ps.fCenter.fLongPt));
      ps.fEndAng=mtFixAngle(k90 - guLPtToLAngle(ps.fHRadius,ps.fVRadius,ps.fPoint[1].fLongPt,ps.fCenter.fLongPt));

      Angle midAngle=ps.fEndAng - ps.fStartAng;
      if(midAngle<0) midAngle=360 - midAngle;
      midAngle=mtFixAngle(ps.fStartAng + (midAngle >> 1));
      ps.fPoint[2].fUse=0;
      ps.fPoint[2].fFlags=kPhRealVertex;
      ps.fPoint[2].fLongPt.x=ps.fCenter.fLongPt.x + mtFixMulDbl(ps.fHRadius,mtCos(midAngle));
      ps.fPoint[2].fLongPt.y=ps.fCenter.fLongPt.y + mtFixMulDbl(ps.fVRadius,mtSin(midAngle));
      ps.fPoint[2].fLongPt=mtRotPoint(ps.fPoint[2].fLongPt,m_RotAngle,ps.fCenter.fLongPt);

      uptr p((char*)&ps);
      p.swapDSShape();
      p.swaps(3); // fm

      p.skip(2);
      p.swapDBPoint(1);
      p.swapl(4);
      p.swapDBPoint(3);

      ushort count=sizeof(PhDSEArc) << 8;	// swapped
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(&ps,sizeof(PhDSEArc));
   }
   else
   {
      PhDSArc ps;
      ps.fID=(m_nMode == kRadius)?PhIDArc:PhID3Arc;
      ctx.pPhShape=&ps;
      CMDFillShape::SavePCDraft55(ctx);

      ps.fMethod=(m_nMode == kRadius)?kPhCenterRadius:kPh3Point;
      ps.fDummy=0;
      ps.fCenter.fUse=0;
      ps.fCenter.fFlags=kPhRealVertex;
      ps.fCenter.fLongPt=GetCenter();
      ps.fRadius=GetRadii().x;

      switch(m_nMode)
      {
      case kRadius:
      {
         ps.fPoint[0].fUse=0;
         ps.fPoint[0].fFlags=kPhRealVertex;
         ps.fPoint[0].fLongPt=m_pt[1];
         ps.fPoint[1].fUse=0;
         ps.fPoint[1].fFlags=kPhRealVertex;
         ps.fPoint[1].fLongPt=m_pt[2];
         ps.fPoint[2].fUse=0;
         ps.fPoint[2].fFlags=0;
         ps.fPoint[2].fLongPt=LongPoint::kZeroPt;
      }
      break;
      case kThreePoint:
      {
         ps.fPoint[0].fUse=0;
         ps.fPoint[0].fFlags=kPhRealVertex;
         ps.fPoint[0].fLongPt=m_pt[0];
         ps.fPoint[1].fUse=0;
         ps.fPoint[1].fFlags=kPhRealVertex;
         ps.fPoint[1].fLongPt=m_pt[2];
         ps.fPoint[2].fUse=0;
         ps.fPoint[2].fFlags=kPhRealVertex;
         ps.fPoint[2].fLongPt=m_pt[1];
      }
      break;
      }
      if(m_nArcDirection == AD_CLOCKWISE)
      {
         ps.fStartAng=mtFixAngle(k90 - mtAngle(ps.fCenter.fLongPt,ps.fPoint[1].fLongPt));
         ps.fEndAng=mtFixAngle(k90 - mtAngle(ps.fCenter.fLongPt,ps.fPoint[0].fLongPt));
      }
      else
      {
         ps.fStartAng=mtFixAngle(k90 - mtAngle(ps.fCenter.fLongPt,ps.fPoint[0].fLongPt));
         ps.fEndAng=mtFixAngle(k90 - mtAngle(ps.fCenter.fLongPt,ps.fPoint[1].fLongPt));
      }

      uptr p((char*)&ps);
      p.swapDSShape();
      p.swaps(3); // fm

      p.skip(2);
      p.swapDBPoint(1);
      p.swapl(3);
      p.swapDBPoint(3);

      ushort count=sizeof(PhDSArc) << 8;	// swapped
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(&ps,sizeof(PhDSArc));
   }
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDArea::SavePCDraft55(::format55::PhIOContext& ctx)
{
   {
      PhDSArea ps;
      ps.fID=PhIDArea;
      ctx.pPhShape=(PhDSFillShape*)&ps;
      CMDFillShape::SavePCDraft55(ctx);

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
      ps.fDimStrHdl=long(this) + 1;

      uptr p((char*)&ps);
      p.swapDSShape();

      p.swaps(1);
      p.skip(12);
      p.swapl(2);

      ushort count=sizeof(PhDSArea) << 8;	// swapped
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(&ps,sizeof(PhDSArea));
      SaveMacObjInfo(ctx);
   }

   {
      PhDSDimStr ps;
      ps.fID=PhIDDimString;
      ctx.pPhShape=(PhDSFillShape*)&ps;
      CMDFillShape::SavePCDraft55(ctx);

      ps.fPrev=long(this) + 1;
      ps.fStatus=kPhDimVisible | kPhAreaStr;
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
      p.swaps(3); // fm

      p.swaps(3);
      p.swapDBPoint(2);
      p.swapl(1);
      p.skip(kDimStrSize);

      ushort count=sizeof(PhDSDimStr) << 8;	// swapped
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(&ps,sizeof(PhDSDimStr));
      SaveMacObjInfo(ctx);
   }
   return 2;
}

void CMDAssocDim::SaveMacDimLabel(::format55::PhIOContext &ctx)
{
   if(m_DimFmt.bUseLabel && !m_DimFmt.sLabel.IsEmpty())
   {
      short count = m_DimFmt.sLabel.GetLength() + 1;
      uptr(&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));
      count = m_DimFmt.sLabel.GetLength() + 1;
      char * p = (char*)malloc(count);
      ctx.ConvertC2P(m_DimFmt.sLabel,p,count);
      ctx.f->Write(p,count);
      free(p);
   }
}


int CMDAssocDim::SavePCDraft55(::format55::PhIOContext& ctx)
{
   FillModel fm;
   CString sValue,sTol1,sTol2;
   TextParams tp;
   if(m_pDimStr)
   {
      fm=m_pDimStr->GetFillModel();
      tp=m_pDimStr->GetTextParams();
      m_pDimStr->GetValues(sValue,sTol1,sTol2);
   }

   CMDShape::SavePCDraft55(ctx);
   PhDSAssocDim* ps=(PhDSAssocDim*)ctx.pPhShape;
   ctx.ConvertFillModel(fm,ps->fFillModel);

   ps->fPenModel.fPenStyle&=0xFF00;
   ps->fPenModel.fPenStyle|=(m_nDimCap1 & 0xF);
   ps->fPenModel.fPenStyle|=(m_nDimCap2 & 0xF) << 4;

   ps->fStatus=kPhDimVisible;
   Style nStyle;
   ctx.ConvertLogFont(tp.m_lf,ps->fFont,ps->fFontSize,nStyle);
   for(int i=0;i<12;i++) ps->fValue[i]=0;
   ctx.ConvertDimFmt(m_DimFmt,ps->format);
   // roundoff
   ps->fShapeFlags&=0xF0FF;
   ps->fShapeFlags|=((m_DimFmt.nRoundoff & 0xF) << 8);

   ctx.ConvertDimStd(m_DimStd,ps->standard);
   ps->textExtent=LongRect(0,0,0,0);
   ps->dimStrDraw=LongRect(0,0,0,0);
   ps->tol1Draw=LongRect(0,0,0,0);
   ps->tol2Draw=LongRect(0,0,0,0);

   ctx.ConvertC2P(sValue,ps->dimStr,kDimStrSize);
   ctx.ConvertC2P(sTol1,ps->tol1Str,kDimTolStrSize);
   ctx.ConvertC2P(sTol2,ps->tol2Str,kDimTolStrSize);
   return 0;
}

int CMDBezierShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   int size=sizeof(PhDSPolygon) + sizeof(PhDBPoint)*(m_nCount - 1);
   PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
   ps->fID=PhIDBezier;
   ctx.pPhShape=ps;
   CMDFillShape::SavePCDraft55(ctx);

   ps->fClosed=m_bClosed;
   ps->fDummy=0;
   ps->fCount=(m_nCount - 1) / 3;

   uptr p((char*)ps);
   p.swapDSShape();
   p.swaps(3); // fm
   p.skip(2); p.swaps(1);

   for(int i=0;i<m_nCount;i++)
   {
      ps->fPoint[i].fFlags=kPhRealVertex;
      ps->fPoint[i].fLongPt=m_pt[i];
      ps->fPoint[i].fUse=0;
      p.swapDBPoint(1);
   }

   ushort count=size;
   uptr((char*)&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(ps,size);
   SaveMacObjInfo(ctx);
   free(ps);
   return 1;
}


int CMDBitmap::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSBitmap ps;
   ps.fID=PhIDBitmap;
   ctx.pPhShape=&ps;
   CMDFillShape::SavePCDraft55(ctx);

   for(int i=0;i<4;i++)
   {
      ps.fCorner[i].fUse=0;
      ps.fCorner[i].fFlags=0;
      ps.fCorner[i].fLongPt=m_lrc.GetCorner(i);
   }
   ps.fSrcRect.l=(short)m_rc.left;
   ps.fSrcRect.t=(short)m_rc.top;
   ps.fSrcRect.r=(short)m_rc.right;
   ps.fSrcRect.b=(short)m_rc.bottom;
   ps.fMapHdlIndex=ctx.bitmaps.Add((DWORD)m_hDib);

   uptr p((char*)&ps);
   p.swapDSShape();
   p.swaps(3); // fm
   p.swapDBPoint(4);
   p.swapl(2); p.swaps(1);

   short count=sizeof(PhDSBitmap) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSBitmap));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDCenterDim::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSCenterDim ps;
   ps.fID=PhIDCenterDim;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDAssocDim::SavePCDraft55(ctx);

   ps.centerPt.fUse=0;
   ps.centerPt.fFlags=kPhRealVertex;
   ps.centerPt.fLongPt=m_lptCenter;
   ps.hRad=m_Radii.x;
   ps.vRad=m_Radii.y;

   uptr p((char*)&ps);
   p.swapDSShape();

   p.swaps(3); // fm
   p.swaps(3);	p.skip(12);
   p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   p.swaps(1); p.swapl(10); // dim std
   p.swaplpt(8);
   p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);

   p.swapDBPoint(1);
   p.swapl(2);

   short count=sizeof(PhDSCenterDim);
   uptr(&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSCenterDim));

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDCircle::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSCircle ps;
   ps.fID=PhIDCircle;
   ctx.pPhShape=&ps;
   CMDFillShape::SavePCDraft55(ctx);

   uptr p((char*)&ps);
   p.swapDSShape();
   p.swaps(3); // fm

   ps.fCenter.fFlags=kPhRealVertex;
   ps.fCenter.fLongPt=GetCenter();
   ps.fCenter.fUse=0;
   ps.fRadius=GetRadii().x;
   switch(m_nType)
   {
   case kRadius:
      ps.fMethod=kPhCenterRadius;
      ps.fPoint[0].fFlags=kPhRealVertex;
      ps.fPoint[0].fLongPt=m_pt[1];
      ps.fPoint[0].fUse=0;
      break;
   case kDiameter:
      ps.fMethod=kPhDiameter;
      ps.fPoint[0].fFlags=kPhRealVertex;
      ps.fPoint[0].fLongPt=m_pt[0];
      ps.fPoint[0].fUse=0;
      ps.fPoint[1].fFlags=kPhRealVertex;
      ps.fPoint[1].fLongPt=m_pt[1];
      ps.fPoint[1].fUse=0;
      break;
   case kThreePoint:
      ps.fMethod=kPh3Point;
      ps.fPoint[0].fFlags=kPhRealVertex;
      ps.fPoint[0].fLongPt=m_pt[0];
      ps.fPoint[0].fUse=0;
      ps.fPoint[1].fFlags=kPhRealVertex;
      ps.fPoint[1].fLongPt=m_pt[1];
      ps.fPoint[1].fUse=0;
      ps.fPoint[2].fFlags=kPhRealVertex;
      ps.fPoint[2].fLongPt=m_pt[2];
      ps.fPoint[2].fUse=0;
      break;
   }

   p.skip(2);
   p.skip(2); p.swaplpt(1);	// DBPoint
   p.swapl(1);
   p.skip(2); p.swaplpt(1);	// DBPoint
   p.skip(2); p.swaplpt(1);	// DBPoint
   p.skip(2); p.swaplpt(1);	// DBPoint

   ushort count=sizeof(PhDSCircle) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSCircle));
   SaveMacObjInfo(ctx);
   return 1;
}



int CMDCubicShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   short count=m_nCount - (m_bClosed?0:1);
   int size=sizeof(PhDSPolygon) + sizeof(PhDBPoint)*(3 * count);
   PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
   ps->fID=PhIDCubic;
   ctx.pPhShape=ps;
   CMDFillShape::SavePCDraft55(ctx);

   ps->fClosed=m_bClosed;
   ps->fDummy=0;
   ps->fCount=count;

   uptr p((char*)ps);
   p.swapDSShape();
   p.swaps(3); // fm
   p.skip(2); p.swaps(1);

   LongPoint lpt1,lpt2,lpt3;
   LongPoint lpt12,lpt32;
   lpt1=GetShapePoint(0);
   lpt2=GetShapePoint(1);
   lpt3=GetShapePoint(2);

   ps->fPoint[0].fUse=0;
   ps->fPoint[0].fFlags=kPhRealVertex;
   ps->fPoint[0].fLongPt=lpt1;
   if(m_bClosed)
   {
      guBezSplinePts(m_pt[m_nCount - 1],lpt1,lpt2,&lpt12,&lpt32);
      ps->fPoint[1].fUse=0;
      ps->fPoint[1].fFlags=kPhRealVertex;
      ps->fPoint[1].fLongPt=lpt32;

      ps->fPoint[3 * count].fUse=0;
      ps->fPoint[3 * count].fFlags=kPhRealVertex;
      ps->fPoint[3 * count].fLongPt=lpt1;

      ps->fPoint[3 * count - 1].fUse=0;
      ps->fPoint[3 * count - 1].fFlags=kPhRealVertex;
      ps->fPoint[3 * count - 1].fLongPt=lpt12;
   }
   else
   {
      ps->fPoint[1].fUse=0;
      ps->fPoint[1].fFlags=kPhRealVertex;
      ps->fPoint[1].fLongPt=lpt1;
   }
   int i = 2;
   for(;i<(m_nCount + (m_bClosed?1:0));i++)
   {
      lpt3=GetShapePoint(i%m_nCount);
      guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

      ps->fPoint[i * 3 - 4].fUse=0;
      ps->fPoint[i * 3 - 4].fFlags=kPhRealVertex;
      ps->fPoint[i * 3 - 4].fLongPt=lpt12;

      ps->fPoint[i * 3 - 3].fUse=0;
      ps->fPoint[i * 3 - 3].fFlags=kPhRealVertex;
      ps->fPoint[i * 3 - 3].fLongPt=lpt2;

      ps->fPoint[i * 3 - 2].fUse=0;
      ps->fPoint[i * 3 - 2].fFlags=kPhRealVertex;
      ps->fPoint[i * 3 - 2].fLongPt=lpt32;

      lpt1=lpt2;
      lpt2=lpt3;
   }
   if(!m_bClosed)
   {
      ps->fPoint[i * 3 - 4].fUse=0;
      ps->fPoint[i * 3 - 4].fFlags=kPhRealVertex;
      ps->fPoint[i * 3 - 4].fLongPt=lpt3;

      ps->fPoint[i * 3 - 3].fUse=0;
      ps->fPoint[i * 3 - 3].fFlags=kPhRealVertex;
      ps->fPoint[i * 3 - 3].fLongPt=lpt3;
   }

   p.swapDBPoint(count * 3 + 1);

   count=size;
   uptr((char*)&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(ps,size);
   SaveMacObjInfo(ctx);
   free(ps);
   return 1;
}


int CMDDimLine::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSDimShape ps;
   ps.fID=PhIDDimPt2Pt;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDShape::SavePCDraft55(ctx);

   ps.fConAngle=m_nConAngle;
   ps.fP1.fFlags=kPhRealVertex;
   ps.fP1.fLongPt=m_pt[0];
   ps.fP1.fUse=0;
   ps.fP2.fFlags=kPhRealVertex;
   ps.fP2.fLongPt=m_pt[1];
   ps.fP2.fUse=0;

   ps.fStatus=kPhDimVisible;
   if(GetAttachedShape() != NULL) ps.fStatus|=kPhDimStrAttached;
   switch(m_nTextDisplay)
   {
   case kAbove:
      ps.fStatus|=kPhDimStrAbove;
   case kAlongLine:
      ps.fStatus|=kPhDimStrOnSlope;
      break;
   }
   ps.fDimStrHdl=(long)m_pAttached;

   uptr p((char*)&ps);
   p.swapDSShape();

   p.swaps(1);
   p.swapDBPoint(2);
   p.swaps(1);
   p.skip(12);
   p.swapl(1);

   ushort count=sizeof(PhDSDimShape) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSDimShape));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDDimStr::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSDimStr ps;
   ps.fID=PhIDDimString;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDFillShape::SavePCDraft55(ctx);

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
   p.swaps(3); // fm

   p.swaps(3);
   p.swapDBPoint(2);
   p.swapl(1);
   p.skip(kDimStrSize);

   ushort count=sizeof(PhDSDimStr) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSDimStr));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDEllipse::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSEllipse ps;
   ps.fID=PhIDEllipse;
   ctx.pPhShape=&ps;
   CMDFillShape::SavePCDraft55(ctx);

   uptr p((char*)&ps);
   p.swapDSShape();
   p.swaps(3); // fm

   ps.fMethod=(m_nType == kRectCenter)?kPhCenterRadius:kPhDiameter;
   ps.fDummy=0;
   ps.fCenter.fUse=0;
   ps.fCenter.fFlags=kPhRealVertex;
   ps.fCenter.fLongPt=GetCenter();
   LongPoint lptRadii=GetRadii();
   ps.fHRadius=lptRadii.x;
   ps.fVRadius=lptRadii.y;

   p.skip(2);
   p.skip(2); p.swaplpt(1);	// DBPoint
   p.swapl(2);

   ushort count=sizeof(PhDSEllipse) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSEllipse));
   SaveMacObjInfo(ctx);
   return 1;
}

int CMDFillShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   CMDShape::SavePCDraft55(ctx);
   ctx.ConvertFillModel(m_FillModel,ctx.pPhShape->fFillModel);
   return 0;
}


int CMDGroupShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   short groupCount=0;
   CMDShape* pSavedGroup=ctx.pGroup;
   ctx.pGroup=this;
   CDWordArray members;
   POSITION pos=m_Members.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=m_Members.GetPrev(pos);
      short count=pShape->SavePCDraft55(ctx);
      if(count) members.Add((long)pShape);
      groupCount+=count;
   }
   ctx.pGroup=pSavedGroup;
   if(groupCount)
   {
      PhDSShape ps;
      ps.fID=PhIDGroup;
      ctx.pPhShape=(PhDSFillShape*)&ps;
      CMDShape::SavePCDraft55(ctx);

      uptr p((char*)&ps);
      p.swapDSShape();

      p.pl=(long*)members.GetData();
      p.swapl(members.GetSize());

      short count=(sizeof(PhDSShape) + sizeof(short) + members.GetSize()*sizeof(long));
      uptr((char*)&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(&ps,sizeof(PhDSShape));
      count=members.GetSize();
      uptr((char*)&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(members.GetData(),members.GetSize()*sizeof(long));
      SaveMacObjInfo(ctx);

      groupCount++;
   }
   return groupCount;
}


int CMDLineDim::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSLineDim ps;
   switch(m_nType)
   {
   case kHoriz:
      ps.fID=PhIDHLineDim;
      break;
   case kVert:
      ps.fID=PhIDVLineDim;
      break;
   case kSlope:
      ps.fID=PhIDSLineDim;
      break;
   case kPerp:
      ps.fID=PhIDPLineDim;
      break;
   }
   ctx.pPhShape=(PhDSFillShape*)&ps;
   Angle savedAngle=m_RotAngle;
   m_RotAngle=m_Angle;
   CMDAssocDim::SavePCDraft55(ctx);
   m_RotAngle=savedAngle;

   ps.refPt1.fUse=0;
   ps.refPt1.fFlags=kPhRealVertex;
   ps.refPt1.fLongPt=m_ldp.lptFrom;
   ps.refPt2.fUse=0;
   ps.refPt2.fFlags=kPhRealVertex;
   ps.refPt2.fLongPt=m_ldp.lptTo;
   ps.anchorPt.fUse=0;
   ps.anchorPt.fFlags=kPhRealVertex;
   ps.anchorPt.fLongPt=m_ldp.lptAnchor;
   ps.witnessStart1.fUse=0;
   ps.witnessStart1.fFlags=kPhRealVertex;
   ps.witnessStart1.fLongPt=m_ldp.lptWit1Start;
   ps.witnessEnd1.fUse=0;
   ps.witnessEnd1.fFlags=kPhRealVertex;
   ps.witnessEnd1.fLongPt=m_ldp.lptWit1End;
   ps.witnessStart2.fUse=0;
   ps.witnessStart2.fFlags=kPhRealVertex;
   ps.witnessStart2.fLongPt=m_ldp.lptWit2Start;
   ps.witnessEnd2.fUse=0;
   ps.witnessEnd2.fFlags=kPhRealVertex;
   ps.witnessEnd2.fLongPt=m_ldp.lptWit2End;
   ps.dimLineStart.fUse=0;
   ps.dimLineStart.fFlags=kPhRealVertex;
   ps.dimLineStart.fLongPt=m_ldp.lptDimStart;
   ps.dimLineEnd.fUse=0;
   ps.dimLineEnd.fFlags=kPhRealVertex;
   ps.dimLineEnd.fLongPt=m_ldp.lptDimEnd;
   ps.extLineStart.fUse=0;
   ps.extLineStart.fFlags=kPhRealVertex;
   ps.extLineStart.fLongPt=m_ldp.lptDimEnd;
   ps.extLineEnd.fUse=0;
   ps.extLineEnd.fFlags=kPhRealVertex;
   ps.extLineEnd.fLongPt=m_ldp.lptExtent;
   ps.leaderLineStart.fUse=0;
   ps.leaderLineStart.fFlags=kPhRealVertex;
   ps.leaderLineStart.fLongPt=m_ldp.lptExtent;
   ps.leaderLineEnd.fUse=0;
   ps.leaderLineEnd.fFlags=kPhRealVertex;
   ps.leaderLineEnd.fLongPt=m_ldp.lptLeader;

   uptr p((char*)&ps);
   p.swapDSShape();

   p.swaps(3); // fm
   p.swaps(3);	p.skip(12);
   p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   p.swaps(1); p.swapl(10); // dim std
   p.swaplpt(8);
   p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);
   p.swapDBPoint(13);

   short count=sizeof(PhDSLineDim);
   uptr(&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSLineDim));
   SaveMacDimLabel(ctx);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDLineShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSLine ps;
   ps.fID=PhIDLine;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDShape::SavePCDraft55(ctx);

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

   ushort count=sizeof(PhDSLine) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSLine));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDMetaPolyShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   short metaCount=0;
   CMemFile mf;
   CFile* pSavedFile=ctx.f;
   ctx.f=&mf;
   CMDShape* pSavedGroup=ctx.pGroup;
   ctx.pGroup=this;
   CDWordArray members;
   POSITION pos=m_Edges.GetHeadPosition();
   while(pos)
   {
      CEdge* pEdge=m_Edges.GetNext(pos);
      CMDShape* pShape=pEdge->CreateShape();
      pShape->SetPenModel(m_PenModel);
      pShape->CalculateExtents();
      pShape->SetShapeDatum(kDefaultDatum);

      short count=pShape->SavePCDraft55(ctx);
      if(count)
      {
         members.Add((long)pShape);
         ctx.metaMembers.Add((long)pShape);
      }
      metaCount+=count;
   }
   ctx.f=pSavedFile;
   ctx.pGroup=pSavedGroup;
   if(metaCount)
   {
      PhDSMetaPoly ps;
      ps.fID=PhIDMetaPoly;
      ctx.pPhShape=&ps;
      CMDFillShape::SavePCDraft55(ctx);

      ps.fStartPoint=m_Edges.GetHead()->m_lptFrom;
      ps.fClosed=IsClosed();
      ps.fDummy=0;
      ps.fNumValidMembers=members.GetSize();
      ps.fNumClosedMembers=0;
      ps.fNumMembers=members.GetSize();

      uptr p((char*)&ps);
      p.swapDSShape();
      p.swaps(3); // fm
      p.swaplpt(1);
      p.skip(2);
      p.swaps(3);

      p.pl=(long*)members.GetData();
      p.swapl(members.GetSize());

      short count=(sizeof(PhDSMetaPoly) + (members.GetSize() - 1)*sizeof(long));
      uptr(&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(&ps,sizeof(PhDSMetaPoly) - sizeof(long));
      ctx.f->Write(members.GetData(),members.GetSize()*sizeof(long));
      SaveMacObjInfo(ctx);
      metaCount++;

      long length=mf.GetLength();
      void* pl=mf.Detach();
      ctx.f->Write(pl,length);
      free(pl);
   }
   return metaCount;
}


int CMDParallelLine::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSParallelLine ps;
   ps.fID=PhIDParallelLine;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDShape::SavePCDraft55(ctx);

   uptr p((char*)&ps);
   p.swapDSShape();

   ctx.ConvertParaAttr(m_Attrs,ps.fParallelAttr);
   ps.fConAngle=0;

   p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
   p.swaps(1);

   LongPoint pp1,pp2,pp3,pp4;
   guParallelLn(m_pt[0],m_pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);

   ps.fPoint[0].fFlags=kPhRealVertex;
   ps.fPoint[0].fLongPt=m_pt[0];
   ps.fPoint[0].fUse=0;
   ps.fPoint[1].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.fPoint[1].fLongPt=pp1;
   ps.fPoint[1].fUse=0;
   ps.fPoint[2].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.fPoint[2].fLongPt=pp3;
   ps.fPoint[2].fUse=0;
   ps.fPoint[3].fFlags=kPhRealVertex;
   ps.fPoint[3].fLongPt=m_pt[1];
   ps.fPoint[3].fUse=0;
   ps.fPoint[4].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.fPoint[4].fLongPt=pp2;
   ps.fPoint[4].fUse=0;
   ps.fPoint[5].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.fPoint[5].fLongPt=pp4;
   ps.fPoint[5].fUse=0;
   p.swapDBPoint(6);

   ushort count=sizeof(PhDSParallelLine) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSParallelLine));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDParallelPolygon::SavePCDraft55(::format55::PhIOContext& ctx)
{
   int size=sizeof(PhDSParallelPolygon) + sizeof(PhDBPoint)*(m_nCount * 3 - 1);
   PhDSParallelPolygon* ps=(PhDSParallelPolygon*)malloc(size);

   ps->fID=PhIDParallelPolygon;
   ctx.pPhShape=ps;
   CMDFillShape::SavePCDraft55(ctx);

   ps->fClosed=m_bClosed;
   ps->fDummy=0;
   ps->fCount=m_nCount;
   ctx.ConvertParaAttr(m_Attrs,ps->fParallelAttr);

   uptr p((char*)ps);
   p.swapDSShape();
   p.swaps(3); // fm
   p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
   p.skip(2); p.swaps(1);

   LongPoint pp11,pp12,pp13,pp14;
   LongPoint pp21,pp22,pp23,pp24;
   if(m_bClosed)
   {
      guParallelLn(GetShapePoint(m_nCount - 1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
      guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
      guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
      guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
   }
   else
   {
      guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
   }

   ps->fPoint[0].fFlags=kPhRealVertex;
   ps->fPoint[0].fLongPt=m_pt[0];
   ps->fPoint[0].fUse=0;
   ps->fPoint[m_nCount].fFlags=kPhCanSelect | kPhNoHighlight;
   ps->fPoint[m_nCount].fLongPt=pp12;
   ps->fPoint[m_nCount].fUse=0;
   ps->fPoint[m_nCount * 2].fFlags=kPhCanSelect | kPhNoHighlight;
   ps->fPoint[m_nCount * 2].fLongPt=pp14;
   ps->fPoint[m_nCount * 2].fUse=0;

   int i = 1;
   for(;i<(m_nCount - (m_bClosed?0:1));i++)
   {
      pp11=pp12;
      pp12=pp22;
      pp13=pp14;
      pp14=pp24;

      guParallelLn(GetShapePoint(i),GetShapePoint((i + 1) % m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

      guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
      guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

      ps->fPoint[i].fFlags=kPhRealVertex;
      ps->fPoint[i].fLongPt=m_pt[i];
      ps->fPoint[i].fUse=0;
      ps->fPoint[i + m_nCount].fFlags=kPhCanSelect | kPhNoHighlight;
      ps->fPoint[i + m_nCount].fLongPt=pp12;
      ps->fPoint[i + m_nCount].fUse=0;
      ps->fPoint[i + m_nCount * 2].fFlags=kPhCanSelect | kPhNoHighlight;
      ps->fPoint[i + m_nCount * 2].fLongPt=pp14;
      ps->fPoint[i + m_nCount * 2].fUse=0;
   }
   if(m_bClosed)
   {
   }
   else
   {
      ps->fPoint[i].fFlags=kPhRealVertex;
      ps->fPoint[i].fLongPt=m_pt[i];
      ps->fPoint[i].fUse=0;
      ps->fPoint[i + m_nCount].fFlags=kPhCanSelect | kPhNoHighlight;
      ps->fPoint[i + m_nCount].fLongPt=pp22;
      ps->fPoint[i + m_nCount].fUse=0;
      ps->fPoint[i + m_nCount * 2].fFlags=kPhCanSelect | kPhNoHighlight;
      ps->fPoint[i + m_nCount * 2].fLongPt=pp24;
      ps->fPoint[i + m_nCount * 2].fUse=0;
   }

   p.swapDBPoint(m_nCount * 3);

   ushort count=size;
   uptr((char*)&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(ps,size);
   SaveMacObjInfo(ctx);
   free(ps);
   return 1;
}



int CMDPolygonShape::SavePCDraft55(::format55::PhIOContext& ctx)
{
   short phID;
   if(m_nType<kFreeSpline)
   {
      phID=m_nType == kFreehand?PhIDFreehand:PhIDPolygon;
      int size=sizeof(PhDSPolygon) + sizeof(PhDBPoint)*(m_nCount - 1);
      PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
      ps->fID=phID;
      ctx.pPhShape=ps;
      CMDFillShape::SavePCDraft55(ctx);

      ps->fClosed=m_bClosed;
      ps->fDummy=0;
      ps->fCount=m_nCount;

      uptr p((char*)ps);
      p.swapDSShape();
      p.swaps(3); // fm
      p.skip(2); p.swaps(1);

      for(int i=0;i<m_nCount;i++)
      {
         ps->fPoint[i].fFlags=kPhRealVertex;
         ps->fPoint[i].fLongPt=m_pt[i];
         ps->fPoint[i].fUse=0;
         p.swapDBPoint(1);
      }

      ushort count=size;
      uptr((char*)&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(ps,size);
      SaveMacObjInfo(ctx);
      free(ps);
      return 1;
   }
   else
   {
      switch(m_nType)
      {
      case kFreeSpline:
         phID=PhIDFreeSpline;
         break;
      case kPolySpline:
         phID=PhIDPolySpline;
         break;
      default:
         return 0;
      }
      int nCount=m_nCount;
      if(!m_bClosed)
      {
         nCount--;
         if(m_nType == kPolySpline) nCount--;
      }

      int size=sizeof(PhDSPolygon) + sizeof(PhDBPoint)*(3 * nCount + m_nCount);
      PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
      ps->fID=phID;
      ctx.pPhShape=ps;
      CMDFillShape::SavePCDraft55(ctx);

      ps->fClosed=m_bClosed;
      ps->fDummy=0;
      ps->fCount=nCount;

      uptr p((char*)ps);
      p.swapDSShape();
      p.swaps(3); // fm
      p.skip(2); p.swaps(1);

      int i=0;
      if(m_nType == kFreeSpline)
      {
         LongPoint lpt1,lpt2,lpt3;
         if(m_bClosed)
         {
            lpt1=CuVertex(GetShapePoint(m_nCount - 1),GetShapePoint(0),GetShapePoint(1));
         }
         else lpt1=GetShapePoint(0);
         lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
         if(m_nCount<4) lpt3=GetShapePoint(2);
         else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

         LongPoint lpt12,lpt32;
         guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
         if(!m_bClosed)
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhRealVertex;
            ps->fPoint[i++].fLongPt=lpt1;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhRealVertex;
            ps->fPoint[i++].fLongPt=lpt1;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=lpt12;
            p.swapDBPoint(3);
         }
         for(int j=3;j<(m_nCount + (m_bClosed?3:0));j++)
         {
            LongPoint lpt0=lpt2;
            lpt1=lpt32;
            lpt2=lpt3;
            if(j == m_nCount - 1 && !m_bClosed) lpt3=GetShapePoint(j);
            else lpt3=CuVertex(GetShapePoint((j - 1) % m_nCount),GetShapePoint(j%m_nCount),GetShapePoint((j + 1) % m_nCount));
            guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=lpt0;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=lpt1;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=lpt12;
            p.swapDBPoint(3);
         }
         ps->fPoint[i].fUse=0;
         ps->fPoint[i].fFlags=0x01;
         ps->fPoint[i++].fLongPt=lpt2;
         p.swapDBPoint(1);
         if(!m_bClosed)
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=lpt32;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhRealVertex;
            ps->fPoint[i++].fLongPt=lpt3;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhRealVertex;
            ps->fPoint[i++].fLongPt=lpt3;
            p.swapDBPoint(3);
         }
      }
      else
      {
         LongPoint lptCtrl,lpt1;
         if(m_bClosed)
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(m_nCount - 1),GetShapePoint(0));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount - 1));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(0),GetShapePoint(1));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
            p.swapDBPoint(5);
         }
         else
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=GetShapePoint(0);;
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
            p.swapDBPoint(2);
         }
         for(int j=1;j<(m_nCount - (m_bClosed?1:2));j++)
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(j),GetShapePoint(j + 1));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(j),GetShapePoint(j + 1));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(j + 1),GetShapePoint(j));
            p.swapDBPoint(3);
         }
         if(m_bClosed)
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(m_nCount - 1),GetShapePoint(0));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(m_nCount - 1),GetShapePoint(0));
            p.swapDBPoint(2);
         }
         else
         {
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=kPhControlVertex;
            ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(m_nCount - 1),GetShapePoint(m_nCount - 2));
            ps->fPoint[i].fUse=0;
            ps->fPoint[i].fFlags=0x01;
            ps->fPoint[i++].fLongPt=GetShapePoint(m_nCount - 1);
            p.swapDBPoint(2);
         }
      }
      int n=nCount * 3 + 1;
      for(i=0;i<m_nCount;i++)
      {
         ps->fPoint[n + i].fUse=0;
         ps->fPoint[n + i].fFlags=kPhRealVertex;
         ps->fPoint[n + i].fLongPt=m_pt[i];
         p.swapDBPoint(1);
      }

      ushort count=size;
      uptr((char*)&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));
      ctx.f->Write(ps,size);
      SaveMacObjInfo(ctx);
      free(ps);
      return 1;
   }
}


int CMDRadialDim::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSLineDim ps;
   ps.fID=m_bDiameter?PhIDDRadialDim:PhIDRRadialDim;
   ctx.pPhShape=(PhDSFillShape*)&ps;
   CMDAssocDim::SavePCDraft55(ctx);

   ps.refPt1.fUse=0;
   ps.refPt1.fFlags=kPhRealVertex;
   ps.refPt1.fLongPt=m_rdp.lptFrom;
   ps.refPt2.fUse=0;
   ps.refPt2.fFlags=kPhRealVertex;
   ps.refPt2.fLongPt=m_rdp.lptFrom;;
   ps.anchorPt.fUse=0;
   ps.anchorPt.fFlags=kPhRealVertex;
   ps.anchorPt.fLongPt=m_rdp.lptAnchor;
   ps.witnessStart1.fUse=0;
   ps.witnessStart1.fFlags=kPhRealVertex;
   ps.witnessStart1.fLongPt=LongPoint::kZeroPt;
   ps.witnessEnd1.fUse=0;
   ps.witnessEnd1.fFlags=kPhRealVertex;
   ps.witnessEnd1.fLongPt=LongPoint::kZeroPt;
   ps.witnessStart2.fUse=0;
   ps.witnessStart2.fFlags=kPhRealVertex;
   ps.witnessStart2.fLongPt=LongPoint::kZeroPt;
   ps.witnessEnd2.fUse=0;
   ps.witnessEnd2.fFlags=kPhRealVertex;
   ps.witnessEnd2.fLongPt=LongPoint::kZeroPt;
   ps.dimLineStart.fUse=0;
   ps.dimLineStart.fFlags=kPhRealVertex;
   ps.dimLineStart.fLongPt=m_rdp.lptDimStart;
   ps.dimLineEnd.fUse=0;
   ps.dimLineEnd.fFlags=kPhRealVertex;
   ps.dimLineEnd.fLongPt=m_rdp.lptDimEnd;
   ps.extLineStart.fUse=0;
   ps.extLineStart.fFlags=kPhRealVertex;
   ps.extLineStart.fLongPt=m_rdp.lptDimEnd;
   ps.extLineEnd.fUse=0;
   ps.extLineEnd.fFlags=kPhRealVertex;
   ps.extLineEnd.fLongPt=m_rdp.lptExtent;
   ps.leaderLineStart.fUse=0;
   ps.leaderLineStart.fFlags=kPhRealVertex;
   ps.leaderLineStart.fLongPt=m_rdp.lptExtent;
   ps.leaderLineEnd.fUse=0;
   ps.leaderLineEnd.fFlags=kPhRealVertex;
   ps.leaderLineEnd.fLongPt=m_rdp.lptLeader;

   uptr p((char*)&ps);
   p.swapDSShape();

   p.swaps(3); // fm
   p.swaps(3);	p.skip(12);
   p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   p.swaps(1); p.swapl(10); // dim std
   p.swaplpt(8);
   p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);
   p.swapDBPoint(13);

   short count=sizeof(PhDSLineDim);
   uptr(&count).swaps(1);
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSLineDim));
   SaveMacDimLabel(ctx);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDRectangle::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSRectangle ps;
   ps.fID=(m_nType == kRectCenter)?PhIDRectCenter:PhIDRectDiagonal;
   ctx.pPhShape=&ps;
   CMDFillShape::SavePCDraft55(ctx);

   uptr p((char*)&ps);
   p.swapDSShape();
   p.swaps(3); // fm
   for(int i=0;i<4;i++)
   {
      ps.fPoint[i].fFlags=kPhRealVertex;
      ps.fPoint[i].fLongPt=m_ptCorner[i];
      ps.fPoint[i].fUse=0;
      p.skip(2); p.swaplpt(1);	// DBPoint
   }

   ushort count=sizeof(PhDSRectangle) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSRectangle));
   SaveMacObjInfo(ctx);
   return 1;
}


void CMDShape::SaveMacObjInfo(::format55::PhIOContext& ctx)
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
      for(;i<5;i++) count+=min(m_sValues[i].GetLength() + 2,32);

      CMemFile mf;
      uptr(&count).swaps(1);
      mf.Write(&count,sizeof(short));
      mf.Write(&pInfo,sizeof(PhObjectInfo));
      for(i=0;i<5;i++)
      {
         char buf[32];
         ctx.ConvertC2P(m_sValues[i],buf,31);
         buf[m_sValues[i].GetLength() + 1]=0;
         mf.Write(buf,*buf + 2);
      }

      long length=mf.GetLength();
      void* pl=mf.Detach();
      ctx.f->Write(pl,length);
      free(pl);
   }
   if(ctx.pPhShape->fRef || ctx.nCount)
   {
      ctx.nCount = (ctx.nCount + 3) / 4;

      PhPropList pl;
      memset(&pl,0,sizeof(PhPropList));
      pl.fNumElems=ctx.nCount + (ctx.pPhShape->fRef ? 1:0);
      pl.fElemSize=sizeof(PhPropListElem);
      pl.fInitSize=pl.fNumElems * sizeof(PhPropListElem);
      pl.fGrowSize=sizeof(PhPropListElem);
      pl.fKeylen=2;

      count=sizeof(PhPropList) + sizeof(PhPropListElem) * (pl.fNumElems - 1);
      uptr(&count).swaps(1);
      ctx.f->Write(&count,sizeof(short));

      uptr p((char*)&pl);
      p.swaps(1); p.swapl(1); p.swaps(1);
      p.swapl(3); p.swaps(3); p.swapl(1);
      ctx.f->Write(&pl,sizeof(PhPropList) - sizeof(PhPropListElem));

      if(ctx.pPhShape->fRef)
      {
         pl.fData[0].fType=kPhShapeToAreaLink;
         pl.fData[0].fData=long(m_pAttached);
         p.swaps(1); p.swapl(1);
         ctx.f->Write(&pl.fData,sizeof(PhPropListElem));
      }

      for(count = 0; ctx.nCount > 0;count++)
      {
         ctx.nCount--;
         pl.fData[0].fType=600 + count;
         pl.fData[0].fData=ctx.dwProperty[count];
         uptr p((char*)&pl.fData);
         p.swaps(1); p.swapl(1);
         ctx.f->Write(&pl.fData,sizeof(PhPropListElem));
      }
   }
}


int CMDRndRectangle::SavePCDraft55(::format55::PhIOContext& ctx)
{
   PhDSRndRectangle ps;
   ps.fID=PhIDRndRectangle;
   ctx.pPhShape=&ps;
   CMDFillShape::SavePCDraft55(ctx);

   uptr p((char*)&ps);
   p.swapDSShape();
   p.swaps(3); // fm
   LongPoint axisPt=Average(m_ptCorner[1],m_ptCorner[3]);
   for(int i=0;i<4;i++)
   {
      ps.fPoint[i].fFlags=kPhRealVertex;
      ps.fPoint[i].fLongPt=mtRotPoint(m_ptCorner[i],-m_RotAngle,axisPt);
      ps.fPoint[i].fUse=0;
      p.skip(2); p.swaplpt(1);	// DBPoint
   }
   switch(m_nCornerType)
   {
   case kCornerProportional:
      ps.fCornerType=kPhProportionalCorner;
      break;
   case kCornerCircle:
      ps.fCornerType=kPhConstantCorner;
      break;
   case kCornerElliptical:
      ps.fCornerType=kPhEllipticalCorner;
      break;
   }
   ps.fDiagonals.fFlags=0;
   ps.fDiagonals.fLongPt=m_lptCornerDelta << 1;
   ps.fDiagonals.fUse=0;
   p.skip(2);
   p.skip(2); p.swaplpt(1);	// DBPoint

   ushort count=sizeof(PhDSRndRectangle) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSRndRectangle));
   SaveMacObjInfo(ctx);
   return 1;
}



int CMDShape::SavePCDraft55(::format55::PhIOContext& ctx)
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


int CMDText::SavePCDraft55(::format55::PhIOContext& ctx)
{
   int textLen=m_StyledText.GetSize();
   if(textLen == 0) return 0; // crash protection for old files - generally illegal condition

   PhDSText ps;
   ps.fID=PhIDText;
   ctx.pPhShape=&ps;
   CMDFillShape::SavePCDraft55(ctx);

   ps.fParagraph=m_nType == kTextUserSize;
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
      ps.fShapeFlags|=(1 << kPhVertTextAlignCodeShift);
      break;
   case kAlignBottom:
      ps.fShapeFlags|=(2 << kPhVertTextAlignCodeShift);
      break;
   }

   long oldRef = ctx.pPhShape->fRef;
   ctx.pPhShape->fRef = 1;
   uptr p((char*)&ps);
   p.swapDSShape();
   p.swaps(3); // fm
   p.skip(2); p.swapl(2); p.swaps(1); p.skip(2); p.swaps(1);
   int i = 0;
   for(;i<4;i++)
   {
      ps.fPoints[i].fFlags=kPhControlVertex;
      ps.fPoints[i].fLongPt=m_ptCorner[i];
      ps.fPoints[i].fUse=0;
      p.skip(2); p.swaplpt(1);	// DBPoint
   }

   ushort count=sizeof(PhDSText) << 8;	// swapped
   ctx.f->Write(&count,sizeof(short));
   ctx.f->Write(&ps,sizeof(PhDSText));
   ctx.pPhShape->fRef = oldRef;

   PhTextData pt;
   memset(&pt,0,sizeof(PhTextData));
   if(m_RotAngle == 0)
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
      if(pText[i] == 0xA) pText[i]=0xD;
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
   memset(ctx.cCharset,0,sizeof(ctx.cCharset));
   while(pos< m_StyledText.GetSize())
   {
      nStyleCount++;
      ste.scrpStartChar=pos;
      m_StyledText.GetNextTextPart(pos,text,theTextPlus1,s,bLineFeed);

      ctx.ConvertLogFont(s.m_LogFont,ste.scrpFont,ste.scrpSize,ste.scrpFace);
      if(ctx.nCount <= 400)
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
      if(c & 0xFF000000)
      {
         PALETTEENTRY pe;
         ctx.pPalette->GetPaletteEntries(c & 0xFF,1,&pe);
         c=RGB(pe.peRed,pe.peGreen,pe.peBlue);
      }
      ste.scrpColor.red=GetRValue(c) << 8;
      ste.scrpColor.green=GetGValue(c) << 8;
      ste.scrpColor.blue=GetBValue(c) << 8;
      uptr p((char*)&ste);
      p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
      mf.Write(&ste,sizeof(ScrpSTElement));
   }

   SaveMacObjInfo(ctx);

   pt.fNZScrpSize=sizeof(short) + nStyleCount*sizeof(ScrpSTElement);
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










