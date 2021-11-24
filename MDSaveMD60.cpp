// MD6.2.1 - Infinisys Ltd

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
#include "PhIOStruct_MD60.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
using namespace formatMD60;

int CMDAngularDim::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDAngularDim ps;
   ps.fID=::formatMD60::IDAngularDim;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   CMDAssocDim::SaveMacDraft620(ctx);

   ps.ref1DBPoint.fUse=0;
   ps.ref1DBPoint.fFlags=kPhRealVertex;
   ps.ref1DBPoint.fLongPt=m_adp.lptFrom;
   ps.ref2DBPoint.fUse=0;
   ps.ref2DBPoint.fFlags=kPhRealVertex;
   ps.ref2DBPoint.fLongPt=m_adp.lptTo;
   ps.lineStartDBPoint.fUse=0;
   ps.lineStartDBPoint.fFlags=0;
   ps.lineStartDBPoint.fLongPt=m_adp.lptDimStart;
   ps.lineEndDBPoint.fUse=0;
   ps.lineEndDBPoint.fFlags=0;
   ps.lineEndDBPoint.fLongPt=m_adp.lptDimEnd;
   ps.anchorDBPoint.fUse=0;
   ps.anchorDBPoint.fFlags=kPhRealVertex;
   ps.anchorDBPoint.fLongPt=m_adp.lptAnchor;
   ps.extDBPoint.fUse=0;
   ps.extDBPoint.fFlags=0;
   ps.extDBPoint.fLongPt=m_adp.lptExtent;
   ps.centerDBPoint.fUse=0;
   ps.centerDBPoint.fFlags=kPhRealVertex | kPhNoHighlight;
   ps.centerDBPoint.fLongPt=m_adp.lptCenter;
   ps.witnessStart1DBPoint.fUse=0;
   ps.witnessStart1DBPoint.fFlags=0;
   ps.witnessStart1DBPoint.fLongPt=m_adp.lptWit1Start;
   ps.witnessEnd1DBPoint.fUse=0;
   ps.witnessEnd1DBPoint.fFlags=0;
   ps.witnessEnd1DBPoint.fLongPt=m_adp.lptWit1End;
   ps.witnessStart2DBPoint.fUse=0;
   ps.witnessStart2DBPoint.fFlags=0;
   ps.witnessStart2DBPoint.fLongPt=m_adp.lptWit2Start;
   ps.witnessEnd2DBPoint.fUse=0;
   ps.witnessEnd2DBPoint.fFlags=0;
   ps.witnessEnd2DBPoint.fLongPt=m_adp.lptWit2End;
   ps.cRadius=Pythag(m_adp.lptAnchor - m_adp.lptCenter);
   ps.cStartAngle1= mtATan2NoFix(ps.ref1DBPoint.fLongPt - ps.centerDBPoint.fLongPt); 
   double dAngle = mtATan2NoFix(ps.ref2DBPoint.fLongPt - ps.centerDBPoint.fLongPt);
   ps.cArcAngle1=dAngle - ps.cStartAngle1;
   ps.cValue = fabs(ps.cArcAngle1);
   ps.cCaps1=4;
   //ps.cStartAngle2=mtATan2(ps.ref2DBPoint.fLongPt - ps.centerDBPoint.fLongPt);
   //ps.cArcAngle2=mtATan2(ps.extDBPoint.fLongPt - ps.centerDBPoint.fLongPt) - ps.cStartAngle2;
   ps.cStartAngle2= 0.0;
   ps.cArcAngle2=0.0;
   ps.cCaps2=0;

   ctx.serialize(ps);

   //uptr p((char*)&ps);
   //p.swapDSShape();

   //p.swaps(3); // fm
   //p.swaps(3);	p.skip(12);
   //p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   //p.swaps(1); p.swapl(10); // dim std
   //p.swaplpt(8);
   //p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);
   //p.swapDBPoint(11);
   //p.swapl(3); p.swaps(1); p.swapl(2); p.swaps(1);

   //short count=sizeof(PhMDAngularDim);
   //uptr(&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDAngularDim));
   //SaveMacDimLabel(ctx);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDArcShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   if(m_nMode == kElliptic)
   {
      PhMDArc ps;
      ps.cBackwards = 0;
      ps.fID=::formatMD60::IDEArc;
      ctx.pPhShape=&ps;
      CMDFillShape::SaveMacDraft620(ctx);

      ps.method=formatMD60::eEllipse;
      //ps.fDummy=0;
      ps.fCenter.fUse=0;
      ps.fCenter.fFlags=kPhRealVertex;
      ps.fCenter.fLongPt=GetCenter();
      LongPoint radii=GetRadii();
      ps.Radii = radii;

      ps.fCenter.fUse=0;
      ps.fCenter.fFlags=kPhRealVertex;
      ps.StartPt.fUse=0;
      ps.StartPt.fFlags=kPhRealVertex;

      if(m_nArcDirection == AD_CLOCKWISE)
      {
         ps.StartPt.fLongPt=m_pt[2];
         ps.EndPt.fLongPt=m_pt[1];
      }
      else
      {
         ps.StartPt.fLongPt=m_pt[1];
         ps.EndPt.fLongPt=m_pt[2];
      }
      ps.cStartAng=mtFixAngle(90.0 - guLPtToLAngle(ps.Radii.x,ps.Radii.y,ps.StartPt.fLongPt,ps.fCenter.fLongPt));
      ps.cEndAng=mtFixAngle(90.0 - guLPtToLAngle(ps.Radii.x,ps.Radii.y,ps.EndPt.fLongPt,ps.fCenter.fLongPt));

      DAngle midAngle=ps.cEndAng - ps.cStartAng;
      if(midAngle<0) midAngle=360 - midAngle;
      midAngle=mtFixAngle(ps.cStartAng + midAngle/2.0);
      ps.ThirdPt.fUse=0;
      ps.ThirdPt.fFlags=kPhRealVertex;
      ps.ThirdPt.fLongPt.x=ps.fCenter.fLongPt.x.m_d + mtFixMulDbl(ps.Radii.x,mtCos(midAngle));
      ps.ThirdPt.fLongPt.y=ps.fCenter.fLongPt.y.m_d + mtFixMulDbl(ps.Radii.y,mtSin(midAngle));
      ps.ThirdPt.fLongPt=mtRotPoint(ps.ThirdPt.fLongPt,m_RotAngle,ps.fCenter.fLongPt);

      ctx.serialize(ps);
      //uptr p((char*)&ps);
      //p.swapDSShape();
      //p.swaps(3); // fm

      //p.skip(2);
      //p.swapDBPoint(1);
      //p.swapl(4);
      //p.swapDBPoint(3);

      //ushort count=sizeof(PhMDEArc) << 8;	// swapped
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(&ps,sizeof(PhMDEArc));
   }
   else
   {
      PhMDArc ps;
      ps.cBackwards = 0;
      ps.fID=(m_nMode == kRadius)?::formatMD60::IDArc : ::formatMD60::ID3Arc;
      ctx.pPhShape=&ps;
      CMDFillShape::SaveMacDraft620(ctx);

      ps.method=(m_nMode == kRadius)?::formatMD60::eCenterRadius : ::formatMD60::e3Point;
      //ps.fDummy=0;
      ps.fCenter.fUse=0;
      ps.fCenter.fFlags=kPhRealVertex;
      ps.fCenter.fLongPt=GetCenter();
      ps.Radii=GetRadii();

      switch(m_nMode)
      {
      case kRadius:
      {
         ps.StartPt.fUse=0;
         ps.StartPt.fFlags=kPhRealVertex;
         ps.StartPt.fLongPt=m_pt[1];
         ps.EndPt.fUse=0;
         ps.EndPt.fFlags=kPhRealVertex;
         ps.EndPt.fLongPt=m_pt[2];
         ps.ThirdPt.fUse=0;
         ps.ThirdPt.fFlags=0;
         ps.ThirdPt.fLongPt=LongPoint::kZeroPt;
      }
      break;
      case kThreePoint:
      {
         ps.StartPt.fUse=0;
         ps.StartPt.fFlags=kPhRealVertex;
         ps.StartPt.fLongPt=m_pt[0];
         ps.EndPt.fUse=0;
         ps.EndPt.fFlags=kPhRealVertex;
         ps.EndPt.fLongPt=m_pt[2];
         ps.ThirdPt.fUse=0;
         ps.ThirdPt.fFlags=kPhRealVertex;
         ps.ThirdPt.fLongPt=m_pt[1];
      }
      break;
      }
      if(m_nArcDirection == AD_CLOCKWISE)
      {
         ps.cStartAng=mtFixAngle(90.0 - mtAngle(ps.fCenter.fLongPt,ps.EndPt.fLongPt));
         ps.cEndAng=mtFixAngle(90.0 - mtAngle(ps.fCenter.fLongPt,ps.StartPt.fLongPt));
         ps.cBackwards = TRUE;
      }
      else
      {
         ps.cStartAng=mtFixAngle(90.0 - mtAngle(ps.fCenter.fLongPt,ps.StartPt.fLongPt));
         ps.cEndAng=mtFixAngle(90.0 - mtAngle(ps.fCenter.fLongPt,ps.EndPt.fLongPt));
         ps.cBackwards = FALSE;
      }

      ctx.serialize(ps);

      //uptr p((char*)&ps);
      //p.swapDSShape();
      //p.swaps(3); // fm

      //p.skip(2);
      //p.swapDBPoint(1);
      //p.swapl(3);
      //p.swapDBPoint(3);

      //ushort count=sizeof(PhMDArc) << 8;	// swapped
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(&ps,sizeof(PhMDArc));
   }
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDArea::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   {
      PhMDArea ps;
      ps.fID=::formatMD60::IDArea;
      ctx.pPhShape=(PhMDFillShape*)&ps;
      CMDFillShape::SaveMacDraft620(ctx);

      ps.fStatus=(m_bAttached)?kPhDimStrAttached:0;
      ps.fStatus |= kPhAreaStr;
      ps.fStatus |= kPhDimVisible;
      //ps.fShapeFlags = 32;
      //ps.fStatus = 7;
      /*	switch(m_nTextDisplay)
      {
      case kAbove:
      ps.fStatus|=kPhDimStrAbove;
      case kAlongLine:
      ps.fStatus|=kPhDimStrOnSlope;
      break;
      }*/
      ps.cShapeHdl=long(m_pOwner);
      //ps.cDimStrHdl=long(this) + 1;


      CPCDraftDoc* pDoc=ctx.pDoc;
      LongRatio ratio;
      pDoc->GetActiveScaleRatio(ratio);

      ps.cAreaValue = m_pOwner->GetRawArea(ratio);

      ps.DimString = m_sValue;
      Style nStyle;
      ctx.ConvertLogFont(m_lf,ps.fShapeFont,ps.fShapeFontSize,nStyle);

      ps.cPosOrOffset.fFlags = 0;
      ps.cPosOrOffset.fLongPt.x = 0;
      ps.cPosOrOffset.fLongPt.y = 0;

      ctx.serialize(ps);

      //uptr p((char*)&ps);
      //p.swapDSShape();

      //p.swaps(1);
      //p.skip(12);
      //p.swapl(2);

      //ushort count=sizeof(PhMDArea) << 8;	// swapped
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(&ps,sizeof(PhMDArea));
      SaveMacObjInfo(ctx);
   }

   {
      //PhMDDimStr ps;
      //ps.fID=::formatMD60::IDDimString;
      //ctx.pPhShape=(PhMDFillShape*)&ps;
      //CMDFillShape::SaveMacDraft620(ctx);

      //ps.fPrev=long(this) + 1;
      //ps.fStatus=kPhDimVisible | kPhAreaStr;
      //if(m_bAttached) ps.fStatus|=kPhDimStrAttached;
      /*	switch(m_nTextDisplay)
      {
      case kAbove:
      ps.fStatus|=kPhDimStrAbove;
      case kAlongLine:
      ps.fStatus|=kPhDimStrOnSlope;
      break;
      }*/
      //Style nStyle;
      //ctx.ConvertLogFont(m_lf,ps.fFont,ps.fFontSize,nStyle);
      //ps.fAnchorPt.fUse=0;
      //ps.fAnchorPt.fFlags=kPhRealVertex;
      //ps.fAnchorPt.fLongPt=m_lptAnchor;
      //ps.fStrOffset.fUse=0;
      //ps.fStrOffset.fFlags=kPhRealVertex;
      //ps.fStrOffset.fLongPt=LongPoint::kZeroPt;
      //ps.fObjHdl=(long)this;
      //ctx.ConvertC2P(m_sValue,ps.fString,kDimStrSize);

      //uptr p((char*)&ps);
      //p.swapDSShape();
      //p.swaps(3); // fm

      //p.swaps(3);
      //p.swapDBPoint(2);
      //p.swapl(1);
      //p.skip(kDimStrSize);

      //ushort count=sizeof(PhMDDimStr) << 8;	// swapped
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(&ps,sizeof(PhMDDimStr));
      //SaveMacObjInfo(ctx);
   }
   return 1;
}

void CMDAssocDim::SaveMacDimLabel(::formatMD60::PhIOContext &ctx)
{
   //if(m_DimFmt.bUseLabel && !m_DimFmt.sLabel.IsEmpty())
   //{
   //   short count=m_DimFmt.sLabel.GetLength() + 1;
   //   uptr(&count).swaps(1);
   //   ctx.f->Write(&count,sizeof(short));
   //   count=m_DimFmt.sLabel.GetLength();
   //   ctx.f->Write(&count,1);
   //   ctx.f->Write(m_DimFmt.sLabel,count);
   //}
}


int CMDAssocDim::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
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

   CMDShape::SaveMacDraft620(ctx);
   PhMDAssocDim* ps=(PhMDAssocDim*)ctx.pPhShape;
   ctx.ConvertFillModel(fm,ps->cFillModel);

   //ps->cPenModel.cPenStyle&=0xFF00;
   //ps->cPenModel.cPenStyle|=(m_nDimCap1 & 0xF);
   //ps->cPenModel.cPenStyle|=(m_nDimCap2 & 0xF) << 4;

   ps->cPenModel.fEndCap[0] = m_nDimCap1;
   ps->cPenModel.fEndCap[1] = m_nDimCap2;

   ps->fStatus=kPhDimVisible;
   Style nStyle;
   ctx.ConvertLogFont(tp.m_lf,ps->fShapeFont,ps->fShapeFontSize,nStyle);
   //for(int i=0;i<12;i++) ps->fValue[i]=0;
   ctx.ConvertDimFmt(m_DimFmt,ps->format);
   // roundoff
   ps->fShapeFlags&=0xF0FF;
   ps->fShapeFlags|=((m_DimFmt.nRoundoff & 0xF) << 8);

   ctx.ConvertDimStd(m_DimStd,ps->standard);
   ps->cTextExtent= CoordRect(0,0,0,0);
   ps->cDimStrDraw=CoordRect(0,0,0,0);
   ps->tol1Draw=CoordRect(0,0,0,0);
   ps->tol2Draw=CoordRect(0,0,0,0);

   ps->dimStr = sValue;
   ps->tol1Str = sTol1;
   ps->tol2Str = sTol2;
   return 0;
}

int CMDBezierShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   //int size=sizeof(PhMDPolygon) + sizeof(PhDBPoint)*(m_nCount - 1);
   //PhMDPolygon* ps=(PhMDPolygon*)malloc(size);
   PhMDSpecificBezier shape;
   shape.fID=::formatMD60::IDBezier;
   ctx.pPhShape=&shape;
   CMDFillShape::SaveMacDraft620(ctx);

   shape.cClosed=m_bClosed;
   //shape.fDummy=0;
   //shape.fCount=(m_nCount - 1) / 3;
   shape.vertexCount = (m_nCount - 1) / 3;

   //uptr p((char*)ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   //p.skip(2); p.swaps(1);

   for(int i=0;i<m_nCount;i++)
   {
      //ps->fPoint[i].fFlags=kPhRealVertex;
      //ps->fPoint[i].fLongPt=m_pt[i];
      //ps->fPoint[i].fUse=0;
      //p.swapDBPoint(1);
      
      shape.data.Add(DDBPoint(m_pt[i], kPhRealVertex,0));

   }

   //ushort count=size;
   //uptr((char*)&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(ps,size);

   ctx.serialize(shape);

   SaveMacObjInfo(ctx);
   //free(ps);

   return 1;
}


int CMDBitmap::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDBitmap ps;
   ps.fID=::formatMD60::IDBitmap;
   ctx.pPhShape=&ps;
   CMDFillShape::SaveMacDraft620(ctx);

   for(int i=0;i<4;i++)
   {
      ps.data[i].fUse=0;
      ps.data[i].fFlags=0;
      ps.data[i].fLongPt=m_lrc.GetCorner(i);
   }
   ps.cSrcRect.l=(short)m_rc.left;
   ps.cSrcRect.t=(short)m_rc.top;
   ps.cSrcRect.r=(short)m_rc.right;
   ps.cSrcRect.b=(short)m_rc.bottom;
   ps.cMapHdlIndex=ctx.bitmaps.Add((DWORD)m_hDib);

   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   //p.swapDBPoint(4);
   //p.swapl(2); p.swaps(1);

   //short count=sizeof(PhMDBitmap) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDBitmap));

   ctx.serialize(ps);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDCenterDim::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDCenterDim ps;
   ps.fID=::formatMD60::IDCenterDim;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   CMDAssocDim::SaveMacDraft620(ctx);

   ps.cCenterPt.fUse=0;
   ps.cCenterPt.fFlags=kPhRealVertex;
   ps.cCenterPt.fLongPt=m_lptCenter;
   ps.cRadii=m_Radii;

   ctx.serialize(ps);

   //uptr p((char*)&ps);
   //p.swapDSShape();

   //p.swaps(3); // fm
   //p.swaps(3);	p.skip(12);
   //p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   //p.swaps(1); p.swapl(10); // dim std
   //p.swaplpt(8);
   //p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);

   //p.swapDBPoint(1);
   //p.swapl(2);

   //short count=sizeof(PhMDCenterDim);
   //uptr(&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDCenterDim));

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDCircle::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDCircle ps;
   ps.fID=::formatMD60::IDCircle;
   ctx.pPhShape=&ps;
   CMDFillShape::SaveMacDraft620(ctx);

   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm

   ps.radius = GetRadii().x;
   switch(m_nType)
   {
   case kRadius:
      ps.data.SetSize(2);
      ps.method=::formatMD60::eCenterRadius;
      ps.data[0].fFlags=kPhRealVertex;
      ps.data[0].fLongPt=GetCenter();
      ps.data[0].fUse=0;
      ps.data[1].fFlags=kPhRealVertex;
      ps.data[1].fLongPt=m_pt[1];
      ps.data[1].fUse=0;
      break;
   case kDiameter:
      ps.data.SetSize(3);
      ps.method=::formatMD60::eDiameter;
      ps.data[0].fFlags=kPhRealVertex;
      ps.data[0].fLongPt=GetCenter();
      ps.data[0].fUse=0;
      ps.data[1].fFlags=kPhRealVertex;
      ps.data[1].fLongPt=m_pt[0];
      ps.data[1].fUse=0;
      ps.data[2].fFlags=kPhRealVertex;
      ps.data[2].fLongPt=m_pt[1];
      ps.data[2].fUse=0;
      break;
   case kThreePoint:
      ps.data.SetSize(4);
      ps.method=::formatMD60::e3Point;
      ps.data[0].fFlags=kPhRealVertex;
      ps.data[0].fLongPt=GetCenter();
      ps.data[0].fUse=0;
      ps.data[1].fFlags=kPhRealVertex;
      ps.data[1].fLongPt=m_pt[0];
      ps.data[1].fUse=0;
      ps.data[2].fFlags=kPhRealVertex;
      ps.data[2].fLongPt=m_pt[1];
      ps.data[2].fUse=0;
      ps.data[3].fFlags=kPhRealVertex;
      ps.data[3].fLongPt=m_pt[2];
      ps.data[3].fUse=0;
      break;
   }

   //p.skip(2);
   //p.skip(2); p.swaplpt(1);	// DBPoint
   //p.swapl(1);
   //p.skip(2); p.swaplpt(1);	// DBPoint
   //p.skip(2); p.swaplpt(1);	// DBPoint
   //p.skip(2); p.swaplpt(1);	// DBPoint

   //ushort count=sizeof(PhMDCircle) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDCircle));

   ctx.serialize(ps);

   SaveMacObjInfo(ctx);

   return 1;

}


int CMDCubicShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   
   long count=m_nCount - (m_bClosed?0:1);

   // int size=sizeof(PhMDPolygon) + sizeof(PhDBPoint)*(3 * count);
   // PhMDPolygon* ps=(PhMDPolygon*)malloc(size);
   
   PhMDCubic ps;

   ps.data.SetSize(3 * count + 1);
   
   ps.fID=::formatMD60::IDCubic;
   
   ctx.pPhShape = &ps;
   
   CMDFillShape::SaveMacDraft620(ctx);

   ps.cClosed=m_bClosed;
   //ps.fDummy=0;
   ps.vertexCount=count;

   //uptr p((char*)ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   //p.skip(2); p.swaps(1);

   LongPoint lpt1,lpt2,lpt3;
   LongPoint lpt12,lpt32;
   lpt1=GetShapePoint(0);
   lpt2=GetShapePoint(1);
   lpt3=GetShapePoint(2);

   ps.data[0].fUse=0;
   ps.data[0].fFlags=kPhRealVertex;
   ps.data[0].fLongPt=lpt1;
   if(m_bClosed)
   {
      guBezSplinePts(m_pt[m_nCount - 1],lpt1,lpt2,&lpt12,&lpt32);
      ps.data[1].fUse=0;
      ps.data[1].fFlags=kPhRealVertex;
      ps.data[1].fLongPt=lpt32;

      ps.data[3 * count].fUse=0;
      ps.data[3 * count].fFlags=kPhRealVertex;
      ps.data[3 * count].fLongPt=lpt1;

      ps.data[3 * count - 1].fUse=0;
      ps.data[3 * count - 1].fFlags=kPhRealVertex;
      ps.data[3 * count - 1].fLongPt=lpt12;
   }
   else
   {
      ps.data[1].fUse=0;
      ps.data[1].fFlags=kPhRealVertex;
      ps.data[1].fLongPt=lpt1;
   }
   int i = 2;
   for(;i<(m_nCount + (m_bClosed?1:0));i++)
   {
      lpt3=GetShapePoint(i%m_nCount);
      guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

      ps.data[i * 3 - 4].fUse=0;
      ps.data[i * 3 - 4].fFlags=kPhRealVertex;
      ps.data[i * 3 - 4].fLongPt=lpt12;

      ps.data[i * 3 - 3].fUse=0;
      ps.data[i * 3 - 3].fFlags=kPhRealVertex;
      ps.data[i * 3 - 3].fLongPt=lpt2;

      ps.data[i * 3 - 2].fUse=0;
      ps.data[i * 3 - 2].fFlags=kPhRealVertex;
      ps.data[i * 3 - 2].fLongPt=lpt32;

      lpt1=lpt2;
      lpt2=lpt3;
   }
   if(!m_bClosed)
   {
      ps.data[i * 3 - 4].fUse=0;
      ps.data[i * 3 - 4].fFlags=kPhRealVertex;
      ps.data[i * 3 - 4].fLongPt=lpt3;

      ps.data[i * 3 - 3].fUse=0;
      ps.data[i * 3 - 3].fFlags=kPhRealVertex;
      ps.data[i * 3 - 3].fLongPt=lpt3;
   }

//   p.swapDBPoint(count * 3 + 1);

   //count=size;
   //uptr((char*)&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(ps,size);

   ctx.serialize(ps);

   SaveMacObjInfo(ctx);
   //free(ps);
   return 1;
}


int CMDDimLine::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDDim ps;
   ps.fID=::formatMD60::IDDimPt2Pt;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   CMDShape::SaveMacDraft620(ctx);

   ps.cConAngle=m_nConAngle;
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
   case ::kAbove:
      ps.fStatus|=kPhDimStrAbove;
   case ::kAlongLine:
      ps.fStatus|=kPhDimStrOnSlope;
      break;
   }
   ps.cDimStrHdl=(long)m_pAttached;

   ctx.serialize(ps);

   //uptr p((char*)&ps);
   //p.swapDSShape();

   //p.swaps(1);
   //p.swapDBPoint(2);
   //p.swaps(1);
   //p.skip(12);
   //p.swapl(1);

   //ushort count=sizeof(PhMDDimShape) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDDimShape));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDDimStr::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDDimStr ps;
   ps.fID=::formatMD60::IDDimString;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   CMDFillShape::SaveMacDraft620(ctx);

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
   ctx.ConvertLogFont(m_lf,ps.fShapeFont,ps.fShapeFontSize,nStyle);
   ps.cAnchorPt.fUse=0;
   ps.cAnchorPt.fFlags=kPhRealVertex;
   ps.cAnchorPt.fLongPt=m_lptAnchor;
   ps.cStrOffset.fUse=0;
   ps.cStrOffset.fFlags=kPhRealVertex;
   ps.cStrOffset.fLongPt=LongPoint::kZeroPt;
   ps.cObjHdl=(long)m_pOwner;
   ps.str = m_sValue;

   ctx.serialize(ps);

   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm

   //p.swaps(3);
   //p.swapDBPoint(2);
   //p.swapl(1);
   //p.skip(kDimStrSize);

   //ushort count=sizeof(PhMDDimStr) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDDimStr));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDEllipse::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDEllipse ps;
   ps.fID=::formatMD60::IDEllipse;
   ctx.pPhShape=&ps;
   CMDFillShape::SaveMacDraft620(ctx);

   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm

   ps.method=(m_nType == kRectCenter)?::formatMD60::eCenterRadius:  ::formatMD60::eDiameter;
   //ps.fDummy=0;
   ps.fCenter.fUse=0;
   ps.fCenter.fFlags=kPhRealVertex;
   ps.fCenter.fLongPt=GetCenter();
   LongPoint lptRadii=GetRadii();
   ps.radius.y = lptRadii.y;
   ps.radius.x = lptRadii.x;

   //p.skip(2);
   //p.skip(2); p.swaplpt(1);	// DBPoint
   //p.swapl(2);

   //ushort count=sizeof(PhMDEllipse) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDEllipse));
   ctx.serialize(ps);
   SaveMacObjInfo(ctx);
   return 1;
}

int CMDFillShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   CMDShape::SaveMacDraft620(ctx);
   PhMDFillShape * pfillshape = (PhMDFillShape *)ctx.pPhShape;
   ctx.ConvertFillModel(m_FillModel,pfillshape->cFillModel);
   pfillshape->cFillShapeReserved = 0;
   pfillshape->cFillShapeReserved1 = 0;
   pfillshape->cFillShapeReserved2 = 0;
   return 0;
}



int CMDGroupShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   
   PhMDGroup ps;

   ps.fID=::formatMD60::IDGroup;

   ctx.pPhShape= (PhMDShape *) &ps;

   CMDShape::SaveMacDraft620(ctx);

   ps.itemsCount = 0;

   CMDShape* pSavedGroup = ctx.pGroup;

   ctx.pGroup = this;

   POSITION pos=m_Members.GetTailPosition();

   while(pos)
   {

      CMDShape* pShape = m_Members.GetPrev(pos);

      ps.pointers.Add((long)pShape);

      ps.itemsCount++;

   }

   int iCount = 0;

   if(ps.itemsCount > 0)
   {

      iCount = 1;

      ctx.serialize(ps);

      SaveMacObjInfo(ctx);

      POSITION pos=m_Members.GetTailPosition();
      
      while(pos)
      {
         
         CMDShape* pShape=m_Members.GetPrev(pos);
         
         iCount += pShape->SaveMacDraft620(ctx);

      }


      //uptr p((char*)&ps);
      //p.swapDSShape();

      //p.pl=(long*)members.GetData();
      //p.swapl(members.GetSize());

      //short count=(sizeof(PhMDShape) + sizeof(short) + members.GetSize()*sizeof(long));
      //uptr((char*)&count).swaps(1);
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(&ps,sizeof(PhMDShape));
      //count=members.GetSize();
      //uptr((char*)&count).swaps(1);
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(members.GetData(),members.GetSize()*sizeof(long));

//      groupCount++;
   }
   ctx.pGroup=pSavedGroup;


   return iCount;

}


int CMDLineDim::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDLineDim ps;
   switch(m_nType)
   {
   case kHoriz:
      ps.fID=::formatMD60::IDHLineDim;
      break;
   case kVert:
      ps.fID=::formatMD60::IDVLineDim;
      break;
   case kSlope:
      ps.fID=::formatMD60::IDSLineDim;
      break;
   case kPerp:
      ps.fID=::formatMD60::IDPLineDim;
      break;
   }
   ctx.pPhShape=(PhMDFillShape*)&ps;
   ::formatMD60::Angle savedAngle=m_RotAngle.GetRawDouble() / 65536.0;
   m_RotAngle=m_Angle;
   
   if(m_pDimStr && m_DimFmt.nTextDisplay != ::kHorizontal)
   {

      m_RotAngle = mtAngleNoFix(m_pDimStr->m_lpt[0],m_pDimStr->m_lpt[1]);

   }

   CMDAssocDim::SaveMacDraft620(ctx);
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

   ctx.serialize(ps);
   //uptr p((char*)&ps);
   //p.swapDSShape();

   //p.swaps(3); // fm
   //p.swaps(3);	p.skip(12);
   //p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   //p.swaps(1); p.swapl(10); // dim std
   //p.swaplpt(8);
   //p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);
   //p.swapDBPoint(13);

   //short count=sizeof(PhMDLineDim);
   //uptr(&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDLineDim));
   SaveMacDimLabel(ctx);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDLineShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDLine ps;
   ps.fID=::formatMD60::IDLine;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   CMDShape::SaveMacDraft620(ctx);

   ps.cConAngle=m_nConAngle;
   ps.fP1.fFlags=kPhRealVertex;
   ps.fP1.fLongPt=m_pt[0];
   ps.fP1.fUse=0;
   ps.fP2.fFlags=kPhRealVertex;
   ps.fP2.fLongPt=m_pt[1];
   ps.fP2.fUse=0;
   ps.mHeadArrowSize = 0.0;

   ctx.serialize(ps);

   //uptr p((char*)&ps);
   //p.swapDSShape();

   //p.swaps(1);
   //p.skip(2); p.swaplpt(1);	// DBPoint
   //p.skip(2); p.swaplpt(1);	// DBPoint

   //::formatMD60::ushort count=sizeof(PhMDLine) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDLine));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDMetaPolyShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{

   short metaCount=0;

   //CMemFile mf;
   //CFile* pSavedFile=ctx.f;
   //ctx.f=&mf;

   CMDShapeArray shapea;

   POSITION pos=m_Edges.GetHeadPosition();

   while(pos)
   {

      CEdge* pEdge=m_Edges.GetNext(pos);
      CMDShape* pShape=pEdge->CreateShape();
      pShape->SetPenModel(m_PenModel);
      pShape->CalculateExtents();
      pShape->SetShapeDatum(kDefaultDatum);

      shapea.Add(pShape);
      ctx.metaMembers.Add((long)pShape);

   }

   if(shapea.GetCount() > 0)
   {

      metaCount = 1;

      PhMDMetaPoly ps;
      ps.fID=::formatMD60::IDMetaPoly;
      ctx.pPhShape=&ps;
      CMDFillShape::SaveMacDraft620(ctx);

      ps.cStartPoint=m_Edges.GetHead()->m_lptFrom;
      ps.cClosed=IsClosed();
      //ps.fDummy=0;
      ps.cNumValidMembers=shapea.GetCount();
      ps.cNumClosedMembers=0;
      ps.itemsCount=shapea.GetCount();


      for(int i = 0; i < shapea.GetCount(); i++)
      {

         ps.shapes.Add((INT_PTR) shapea.ElementAt(i));

      }

      ps.fReserved = 0;
      ps.fReserved1 = 0;
      ps.fReserved2 = 0;

      //uptr p((char*)&ps);
      //p.swapDSShape();
      //p.swaps(3); // fm
      //p.swaplpt(1);
      //p.skip(2);
      //p.swaps(3);

      //p.pl=(long*)members.GetData();
      //p.swapl(members.GetSize());

      //short count=(sizeof(PhMDMetaPoly) + (members.GetSize() - 1)*sizeof(long));
      //uptr(&count).swaps(1);
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(&ps,sizeof(PhMDMetaPoly) - sizeof(long));
      //ctx.f->Write(members.GetData(),members.GetSize()*sizeof(long));
      //SaveMacObjInfo(ctx);
      //metaCount++;

      //long length=mf.GetLength();
      //void* pl=mf.Detach();
      //ctx.f->Write(pl,length);
      //free(pl);

      ctx.serialize(ps);

      SaveMacObjInfo(ctx);

      CMDShape* pSavedGroup=ctx.pGroup;
      ctx.pGroup=this;
      for(int i = 0; i < shapea.GetCount(); i++)
      {

         CMDShape * pshape = shapea[i];

         metaCount += pshape->SaveMacDraft620(ctx);


      }
      ctx.pGroup=pSavedGroup;


   }
   

   return metaCount;

}


int CMDParallelLine::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDParallelLine ps;
   ps.fID=::formatMD60::IDParallelLine;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   CMDShape::SaveMacDraft620(ctx);

   //uptr p((char*)&ps);
   //p.swapDSShape();

   ctx.ConvertParaAttr(m_Attrs,ps.fParallelAttr);
   ps.cConAngle=0;

   //p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
   //p.swaps(1);

   LongPoint pp1,pp2,pp3,pp4;
   guParallelLn(m_pt[0],m_pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);

   ps.data[0].fFlags=kPhRealVertex;
   ps.data[0].fLongPt=m_pt[0];
   ps.data[0].fUse=0;
   ps.data[1].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.data[1].fLongPt=pp1;
   ps.data[1].fUse=0;
   ps.data[2].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.data[2].fLongPt=pp3;
   ps.data[2].fUse=0;
   ps.data[3].fFlags=kPhRealVertex;
   ps.data[3].fLongPt=m_pt[1];
   ps.data[3].fUse=0;
   ps.data[4].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.data[4].fLongPt=pp2;
   ps.data[4].fUse=0;
   ps.data[5].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.data[5].fLongPt=pp4;
   ps.data[5].fUse=0;
   //p.swapDBPoint(6);

   ctx.serialize(ps);

   //ushort count=sizeof(PhMDParallelLine) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDParallelLine));
   SaveMacObjInfo(ctx);
   return 1;
}


int CMDParallelPolygon::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
//   int size=sizeof(PhMDParallelPolygon) + sizeof(PhDBPoint)*(m_nCount * 3 - 1);
  // PhMDParallelPolygon* ps=(PhMDParallelPolygon*)malloc(size);

   PhMDParallelPolygon ps;


   ps.vertexCount = m_nCount * 3;

   ps.data.SetSize(ps.vertexCount);

   ps.fID=::formatMD60::IDParallelPolygon;
   ctx.pPhShape=&ps;
   CMDFillShape::SaveMacDraft620(ctx);

   ps.cClosed=m_bClosed;
   //ps->fDummy=0;
   ps.vertexCount=m_nCount;
   ctx.ConvertParaAttr(m_Attrs,ps.fParallelAttr);

   //uptr p((char*)ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   //p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
   //p.skip(2); p.swaps(1);

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

   ps.data[0].fFlags=kPhRealVertex;
   ps.data[0].fLongPt=pp14;
   ps.data[0].fUse=0;
   ps.data[m_nCount].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.data[m_nCount].fLongPt=pp12;
   ps.data[m_nCount].fUse=0;
   ps.data[m_nCount * 2].fFlags=kPhCanSelect | kPhNoHighlight;
   ps.data[m_nCount * 2].fLongPt=m_pt[0];
   ps.data[m_nCount * 2].fUse=0;

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

      ps.data[i].fFlags=kPhRealVertex;
      ps.data[i].fLongPt=pp14;
      ps.data[i].fUse=0;
      ps.data[i + m_nCount].fFlags=kPhCanSelect | kPhNoHighlight;
      ps.data[i + m_nCount].fLongPt=pp12;
      ps.data[i + m_nCount].fUse=0;
      ps.data[i + m_nCount * 2].fFlags=kPhCanSelect | kPhNoHighlight;
      ps.data[i + m_nCount * 2].fLongPt=m_pt[i];
      ps.data[i + m_nCount * 2].fUse=0;
   }
   if(m_bClosed)
   {
   }
   else
   {
      ps.data[i].fFlags=kPhRealVertex;
      ps.data[i].fLongPt=pp24;
      ps.data[i].fUse=0;
      ps.data[i + m_nCount].fFlags=kPhCanSelect | kPhNoHighlight;
      ps.data[i + m_nCount].fLongPt=pp22;
      ps.data[i + m_nCount].fUse=0;
      ps.data[i + m_nCount * 2].fFlags=kPhCanSelect | kPhNoHighlight;
      ps.data[i + m_nCount * 2].fLongPt=m_pt[i];
      ps.data[i + m_nCount * 2].fUse=0;
   }

   //p.swapDBPoint(m_nCount * 3);

   //ushort count=size;
   //uptr((char*)&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(ps,size);
   ctx.serialize(ps);
   SaveMacObjInfo(ctx);
   //free(ps);
   return 1;
}



int CMDPolygonShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   short phID;
   if(m_nType<kFreeSpline)
   {
      phID=m_nType == kFreehand?::formatMD60::IDFreehand : ::formatMD60::IDPolygon;
      PhMDPolygon ps;
      //ps.vertexCount = m_nCount - 1;
//      int size=sizeof(PhMDPolygon) + sizeof(PhDBPoint)*(m_nCount - 1);
      //PhMDPolygon* ps=(PhMDPolygon*)malloc(size);
      ps.fID=(::formatMD60::eID) phID;
      ctx.pPhShape=&ps;
      CMDFillShape::SaveMacDraft620(ctx);
      std::swap(ps.cPenModel.fEndCap[0],ps.cPenModel.fEndCap[1]);
      ps.cClosed=m_bClosed;
      //ps.fDummy=0;
      ps.vertexCount=m_nCount;

      //uptr p((char*)ps);
      //p.swapDSShape();
      //p.swaps(3); // fm
      //p.skip(2); p.swaps(1);

      ps.data.SetSize(m_nCount);

      for(int i=0;i<m_nCount;i++)
      {
         ps.data[i].fFlags=kPhRealVertex;
         ps.data[i].fLongPt=m_pt[i];
         ps.data[i].fUse=0;
         //p.swapDBPoint(1);
      }

      ctx.serialize(ps);

      //ushort count=size;
      //uptr((char*)&count).swaps(1);
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(ps,size);
      SaveMacObjInfo(ctx);
//      free(ps);
      return 1;
   }
   else
   {
      switch(m_nType)
      {
      case kFreeSpline:
         phID=::formatMD60::IDFreeSpline;
         break;
      case kPolySpline:
         phID=::formatMD60::IDPolySpline;
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

      //int size=sizeof(PhMDPolygon) + sizeof(PhDBPoint)*(3 * nCount + m_nCount);
      //PhMDPolygon* ps=(PhMDPolygon*)malloc(size);
      PhMDPolygon ps;
      ps.vertexCount = 3 * nCount + m_nCount;
      ps.fID=(::formatMD60::eID) phID;
      ctx.pPhShape=&ps;
      CMDFillShape::SaveMacDraft620(ctx);

      ps.cClosed=m_bClosed;
      //ps.fDummy=0;
      ps.vertexCount=nCount;

      //uptr p((char*)ps);
      //p.swapDSShape();
      //p.swaps(3); // fm
      //p.skip(2); p.swaps(1);

      ps.data.SetSize(ps.vertexCount);

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
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhRealVertex;
            ps.data[i++].fLongPt=lpt1;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhRealVertex;
            ps.data[i++].fLongPt=lpt1;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=lpt12;
//            p.swapDBPoint(3);
         }
         for(int j=3;j<(m_nCount + (m_bClosed?3:0));j++)
         {
            LongPoint lpt0=lpt2;
            lpt1=lpt32;
            lpt2=lpt3;
            if(j == m_nCount - 1 && !m_bClosed) lpt3=GetShapePoint(j);
            else lpt3=CuVertex(GetShapePoint((j - 1) % m_nCount),GetShapePoint(j%m_nCount),GetShapePoint((j + 1) % m_nCount));
            guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=lpt0;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=lpt1;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=lpt12;
            //p.swapDBPoint(3);
         }
         ps.data[i].fUse=0;
         ps.data[i].fFlags=0x01;
         ps.data[i++].fLongPt=lpt2;
         //p.swapDBPoint(1);
         if(!m_bClosed)
         {
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=lpt32;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhRealVertex;
            ps.data[i++].fLongPt=lpt3;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhRealVertex;
            ps.data[i++].fLongPt=lpt3;
            //p.swapDBPoint(3);
         }
      }
      else
      {
         LongPoint lptCtrl,lpt1;
         if(m_bClosed)
         {
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=BezVertex(GetShapePoint(m_nCount - 1),GetShapePoint(0));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount - 1));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=BezVertex(GetShapePoint(0),GetShapePoint(1));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
            //p.swapDBPoint(5);
         }
         else
         {
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=GetShapePoint(0);;
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
            //p.swapDBPoint(2);
         }
         for(int j=1;j<(m_nCount - (m_bClosed?1:2));j++)
         {
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(j),GetShapePoint(j + 1));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=BezVertex(GetShapePoint(j),GetShapePoint(j + 1));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(j + 1),GetShapePoint(j));
            //p.swapDBPoint(3);
         }
         if(m_bClosed)
         {
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(m_nCount - 1),GetShapePoint(0));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=BezVertex(GetShapePoint(m_nCount - 1),GetShapePoint(0));
            //p.swapDBPoint(2);
         }
         else
         {
            ps.data[i].fUse=0;
            ps.data[i].fFlags=kPhControlVertex;
            ps.data[i++].fLongPt=BezCtrlPt(GetShapePoint(m_nCount - 1),GetShapePoint(m_nCount - 2));
            ps.data[i].fUse=0;
            ps.data[i].fFlags=0x01;
            ps.data[i++].fLongPt=GetShapePoint(m_nCount - 1);
            //p.swapDBPoint(2);
         }
      }
      int n=nCount * 3 + 1;
      for(i=0;i<m_nCount;i++)
      {
         ps.data[n + i].fUse=0;
         ps.data[n + i].fFlags=kPhRealVertex;
         ps.data[n + i].fLongPt=m_pt[i];
         //p.swapDBPoint(1);
      }

      //ushort count=size;
      //uptr((char*)&count).swaps(1);
      //ctx.f->Write(&count,sizeof(short));
      //ctx.f->Write(ps,size);
      
      ctx.serialize(ps);

      SaveMacObjInfo(ctx);
      //free(ps);
      return 1;
   }
}


int CMDRadialDim::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDLineDim ps;
   ps.fID=m_bDiameter?::formatMD60::IDDRadialDim : ::formatMD60::IDRRadialDim;
   ctx.pPhShape=(PhMDFillShape*)&ps;
   
   if(m_pDimStr)
   {

      m_RotAngle = mtAngleNoFix(m_pDimStr->m_lpt[0],m_pDimStr->m_lpt[1]);

      double da[4];

      DCoordPoint pt0;
      DCoordPoint pt1;
      DCoordPoint pt2;
      DCoordPoint pt3;

      pt0 = m_pDimStr->m_lpt[0];
      pt1 = m_pDimStr->m_lpt[1];
      pt2 = m_pDimStr->m_lpt[2];
      pt3 = m_pDimStr->m_lpt[3];

      DCoordPoint ptCenter;
      
      ptCenter = m_rdp.lptFrom;

      da[0] = pt0.x.m_d - ptCenter.x.m_d;
      da[1] = pt1.x.m_d - ptCenter.x.m_d;
      da[2] = pt2.x.m_d - ptCenter.x.m_d;
      da[3] = pt3.x.m_d - ptCenter.x.m_d;

      int iFind = 0;

      double dMax = Abs(da[0]);

      for(int i = 1; i < 4; i++)
      {

         if(fabs(da[i]) > dMax)
         {

            dMax = fabs(da[i]);

            iFind = i;

         }

      }

      double dAngle = m_RotAngle.GetRawDouble() / 65535.0;

      double dFind = da[iFind];

      double dAngle2 = dAngle;

      if(dFind < 0)
      {

         m_RotAngle += ::k180;

         dAngle2 = m_RotAngle.GetRawDouble() / 65535.0;

      }
      
      CString strFormat;

      strFormat.Format("%f, %f",dAngle,dAngle2);

   }

   CMDAssocDim::SaveMacDraft620(ctx);

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

   //uptr p((char*)&ps);
   //p.swapDSShape();

   //p.swaps(3); // fm
   //p.swaps(3);	p.skip(12);
   //p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
   //p.swaps(1); p.swapl(10); // dim std
   //p.swaplpt(8);
   //p.skip(kDimStrSize + kDimTolStrSize + kDimTolStrSize);
   //p.swapDBPoint(13);

   //short count=sizeof(PhMDLineDim);
   //uptr(&count).swaps(1);
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDLineDim));

   ctx.serialize(ps);

   SaveMacDimLabel(ctx);

   SaveMacObjInfo(ctx);
   return 1;
}


int CMDRectangle::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDRectangle ps;
   ps.fID=(m_nType == kRectCenter)?::formatMD60::IDRectCenter: ::formatMD60::IDRectDiagonal;
   ctx.pPhShape=&ps;
   CMDFillShape::SaveMacDraft620(ctx);

   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   for(int i=0;i<4;i++)
   {
      ps.fPoint[i].fFlags=kPhRealVertex;
      ps.fPoint[i].fLongPt=m_ptCorner[i];
      ps.fPoint[i].fUse=0;
   //   p.skip(2); p.swaplpt(1);	// DBPoint
   }

   //ushort count=sizeof(PhMDRectangle) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDRectangle));
   ctx.serialize(ps);
   SaveMacObjInfo(ctx);
   return 1;
}


void CMDShape::SaveMacObjInfo(::formatMD60::PhIOContext& ctx)
{

   CString strFormat;

#if DEB_DEBUG

   if(ctx.pPhShape->fOwnerGroup != NULL)
   {

      if(ctx.groupaDebug.Find((CMDShape *)(INT_PTR)ctx.pPhShape->fOwnerGroup) == NULL)
      {

         ctx.groupaDebug.AddTail((CMDShape *) (INT_PTR) ctx.pPhShape->fOwnerGroup);

      }

      strFormat.Format("group=%d ",ctx.groupaDebug.Find((CMDShape *)(INT_PTR)ctx.pPhShape->fOwnerGroup));

      ctx.strDebug += strFormat;

   }

   strFormat.Format("%s ",shape_text(ctx.pPhShape->fID));

   ctx.strDebug += strFormat;

   if(ctx.pPhShape->fID == ::formatMD60::IDMetaPoly)
   {

      PhMDMetaPoly* ps=(PhMDMetaPoly*)ctx.pPhShape;

      if(ctx.groupaDebug.Find((CMDShape *)(INT_PTR)ctx.pPhShape->fPrev) == NULL)
      {

         ctx.groupaDebug.AddTail((CMDShape *)(INT_PTR)ctx.pPhShape->fPrev);

      }

      strFormat.Format("count=%d fPrev=%d ",ps->shapes.GetCount(),ctx.groupaDebug.Find((CMDShape *)(INT_PTR)ctx.pPhShape->fPrev));

      ctx.strDebug += strFormat;

   }
   else if(ctx.pPhShape->fID == ::formatMD60::IDGroup)
   {

      PhMDGroup* pgroup=(PhMDGroup*)ctx.pPhShape;

      if(ctx.groupaDebug.Find((CMDShape *)(INT_PTR)ctx.pPhShape->fPrev) == NULL)
      {

         ctx.groupaDebug.AddTail((CMDShape *)(INT_PTR)ctx.pPhShape->fPrev);

      }

      strFormat.Format("count=%I64d fPrev=%d ",pgroup->itemsCount,ctx.groupaDebug.Find((CMDShape *)(INT_PTR)ctx.pPhShape->fPrev));

      ctx.strDebug += strFormat;

   }
   
   strFormat.Format("%d\n",ctx.pPhShape->fID);

   ctx.strDebug += strFormat;

#endif

   short count;
   if(ctx.pPhShape->fObjectInfo)
   {
      PhObjectInfo pInfo;
      pInfo.fVersion=1;
      pInfo.fSetNumber=1;
      //uptr((char*)&pInfo).swaps(2);

      //count=sizeof(PhObjectInfo);
      //int i = 0;
      //for(;i<5;i++) count+=min(m_sValues[i].GetLength() + 2,32);

//      ctx.serialize(&pInfo);

      long l = 5;

      ctx.serialize.serialize_static_array(m_sValues,l);

      //CMemFile mf;
      //uptr(&count).swaps(1);
      //mf.Write(&count,sizeof(short));
      //mf.Write(&pInfo,sizeof(PhObjectInfo));
      //for(i=0;i<5;i++)
      //{
      //   char buf[32];
      //   ctx.ConvertC2P(m_sValues[i],buf,31);
      //   buf[m_sValues[i].GetLength() + 1]=0;
      //   mf.Write(buf,*buf + 2);
      //}

      //long length=mf.GetLength();
      //void* pl=mf.Detach();
      //ctx.f->Write(pl,length);

      //ctx.serialize(

      //free(pl);
   }
   if(ctx.pPhShape->cRef || ctx.nCount || ctx.pPhShape->fObjectProps == 0 || ctx.pPhShape->fObjectProps == 1 || m_pAttached)
   {
      
      UInt64 propCount = ctx.nCount + (m_pAttached ? 1 : 0);
      
      ctx.serialize(propCount);

      if(propCount)
      {
         for(UInt64 ui = 0; ui < propCount; ui++)
         {
            PhPropListElem a;

            if(m_pAttached && ui == 0)
            {
               a.fType = 102;
               a.fReserved = 0;
               a.fDataLength = (unsigned long)(m_pAttached);
               a.ptrLength = 0;
               a.data.SetSize(0);
            }
            else
            {
               a.fType = 600 + ui;
               a.fReserved = 0;
               a.fDataLength = 0;
               a.ptrLength = 0;
               a.data.SetSize(0);

            }
            {

               complex_type_context complextypecontext(&ctx.serialize);

               ctx.serialize(a.fType);
               ctx.serialize(a.fReserved);
               ctx.serialize(a.fDataLength);
               //serialize(a.fReserved2);
               //if(a.fDataLength != 0.0)
               {
                  ctx.serialize(a.ptrLength);
                  ctx.serialize(a.data,a.ptrLength);
               }
            }
         }
      }

      //ctx.nCount = (ctx.nCount + 3) / 4;

      //PhPropList pl;
      //memset(&pl,0,sizeof(PhPropList));
      //pl.fNumElems=ctx.nCount + (ctx.pPhShape->fRef ? 1:0);
      //pl.fElemSize=sizeof(PhPropListElem);
      //pl.fInitSize=pl.fNumElems * sizeof(PhPropListElem);
      //pl.fGrowSize=sizeof(PhPropListElem);
      //pl.fKeylen=2;

      //count=sizeof(PhPropList) + sizeof(PhPropListElem) * (pl.fNumElems - 1);
      //uptr(&count).swaps(1);
      //ctx.f->Write(&count,sizeof(short));

      //uptr p((char*)&pl);
      //p.swaps(1); p.swapl(1); p.swaps(1);
      //p.swapl(3); p.swaps(3); p.swapl(1);
      //ctx.f->Write(&pl,sizeof(PhPropList) - sizeof(PhPropListElem));

      //if(ctx.pPhShape->fRef)
      //{
      //   pl.fData[0].fType=kPhShapeToAreaLink;
      //   pl.fData[0].fData=long(m_pAttached);
      //   p.swaps(1); p.swapl(1);
      //   ctx.f->Write(&pl.fData,sizeof(PhPropListElem));
      //}

      //for(count = 0; ctx.nCount > 0;count++)
      //{
      //   ctx.nCount--;
      //   pl.fData[0].fType=600 + count;
      //   pl.fData[0].fData=ctx.dwProperty[count];
      //   uptr p((char*)&pl.fData);
      //   p.swaps(1); p.swapl(1);
      //   ctx.f->Write(&pl.fData,sizeof(PhPropListElem));
      //}
   }

   ULONGLONG ullBeg = ctx.ullBeg;
   ULONGLONG ullEnd= ctx.serialize.m_pfile->GetPosition();
   ctx.serialize.m_pfile->Seek(ullBeg + sizeof(long),CFile::begin);
   long lObjectSize = ullEnd - ullBeg - sizeof(long) - sizeof(long) - sizeof(long);
   swap32(&lObjectSize);
   ctx.serialize.m_pfile->Write(&lObjectSize,sizeof(lObjectSize));
   long lInfoLocation = 0;
   ctx.serialize.m_pfile->Write(&lInfoLocation,sizeof(lInfoLocation));
   ctx.serialize.m_pfile->Seek(ullEnd,CFile::begin);

}


int CMDRndRectangle::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   PhMDRndRectangle ps;
   ps.fID=::formatMD60::IDRndRectangle;
   ctx.pPhShape=&ps;
   CMDFillShape::SaveMacDraft620(ctx);

   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   LongPoint axisPt=Average(m_ptCorner[1],m_ptCorner[3]);
   for(int i=0;i<4;i++)
   {
      ps.fPoint[i].fFlags=kPhRealVertex;
      //ps.fPoint[i].fLongPt=mtRotPoint(m_ptCorner[i],-m_RotAngle,axisPt);
      ps.fPoint[i].fLongPt=m_ptCorner[i];
      ps.fPoint[i].fUse=0;
      //p.skip(2); p.swaplpt(1);	// DBPoint
   }
   switch(m_nCornerType)
   {
   case kCornerProportional:
      ps.cCornerType=::formatMD60::kProportionalCorner;
      break;
   case kCornerCircle:
      ps.cCornerType=::formatMD60::kConstantCorner;
      break;
   case kCornerElliptical:
      ps.cCornerType=::formatMD60::kEllipticalCorner;
      break;
   }
   ps.cDiagonals.fFlags=0;
   //ps.cDiagonals.fLongPt=m_lptCornerDelta << 1;
   ps.cDiagonals.fLongPt.x=m_lptCornerDelta.x/2;
   ps.cDiagonals.fLongPt.y=m_lptCornerDelta.x / 2;
   ps.cDiagonals.fUse=0;
   //p.skip(2);
   //p.skip(2); p.swaplpt(1);	// DBPoint

   //ushort count=sizeof(PhMDRndRectangle) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDRndRectangle));

   ctx.serialize(ps);

   SaveMacObjInfo(ctx);
   return 1;
}



int CMDShape::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{
   ctx.pPhShape->fPrev=(long)this;
   ctx.pPhShape->fOwnerGroup=(long)ctx.pGroup;
   ctx.pPhShape->fShapeFlags=0;
   ctx.pPhShape->fRot=m_RotAngle.GetRawDouble() / (65536.0);
   ctx.pPhShape->cRef=(m_pAttached && m_pAttached->IsKindOf(RUNTIME_CLASS(CMDArea)))?1:0;
   ctx.pPhShape->fObjectProps = 0;
   ctx.ConvertPenModel(m_PenModel,ctx.pPhShape->cPenModel);
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
   ctx.pPhShape->fClassification = 0;
   ctx.pPhShape->cShapeReserved1 = 0;
   ctx.pPhShape->cShapeReserved2 = 0;
   ctx.pPhShape->cShapeReserved3 = 0;
   return 0;
}


int CMDText::SaveMacDraft620(::formatMD60::PhIOContext& ctx)
{

   int textLen = m_StyledText.GetSize();

   if(textLen == 0) return 0; // crash protection for old files - generally illegal condition

   PhMDText ps;

   ps.fID=::formatMD60::IDText;

   ctx.pPhShape=&ps;

   CMDFillShape::SaveMacDraft620(ctx);

   ps.fInPixels = false;
   ps.fMargins.l = 0.0;
   ps.fMargins.t = 0.0;
   ps.fMargins.r = 0.0;
   ps.fMargins.b = 0.0;

   ps.cParagraph=m_nType == kTextUserSize;

   ps.cObjOpen=0;
   //ps.cZoomTEH=0;
   //ps.cNoZoomCopy=0;
   ps.cfZoomFactor=0;
   switch(m_TextParams.m_nSpacing)
   {
   case ::kSingleSpace:
      ps.cTextSpace=(formatMD60::kSingleSpace) ;
      break;
   case ::k1_5Space:
      ps.cTextSpace=(formatMD60::kOne_halfSpace) ;
      break;
   case ::kDoubleSpace:
      ps.cTextSpace=(formatMD60::kDoubleSpace);
      break;
   }
   //ps.fDummy=0;
   switch(m_TextParams.m_nJust)
   {
   case kAlignLeft:
      ps.cTextJust=0;
      break;
   case kAlignCenter:
      ps.cTextJust=1;
      break;
   case kAlignRight:
      ps.cTextJust=-1;
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
   
   long oldRef = ctx.pPhShape->cRef;
   ctx.pPhShape->cRef = 1;
   //uptr p((char*)&ps);
   //p.swapDSShape();
   //p.swaps(3); // fm
   //p.skip(2); p.swapl(2); p.swaps(1); p.skip(2); p.swaps(1);
   int i = 0;
   for(;i<4;i++)
   {
      ps.data[i].fFlags=kPhControlVertex;
      ps.data[i].fLongPt=m_ptCorner[i];
      ps.data[i].fUse=0;
   //   p.skip(2); p.swaplpt(1);	// DBPoint
   }

   //ushort count=sizeof(PhMDText) << 8;	// swapped
   //ctx.f->Write(&count,sizeof(short));
   //ctx.f->Write(&ps,sizeof(PhMDText));
   //ctx.pPhShape->fRef = oldRef;

   //PhTextData pt;
   memset(&ps.destRect,0,sizeof(ps.destRect));
   memset(&ps.viewRect,0,sizeof(ps.viewRect));
   if(m_RotAngle == 0)
   {
      ps.viewRect.l=ps.destRect.l=(short)ctx.pDoc->LPtoDP(m_ptCorner[0].x);
      ps.viewRect.t=ps.destRect.t=(short)ctx.pDoc->LPtoDP(m_ptCorner[0].y);
      ps.viewRect.r=ps.destRect.r=(short)ctx.pDoc->LPtoDP(m_ptCorner[2].x);
      ps.viewRect.b=ps.destRect.b=(short)ctx.pDoc->LPtoDP(m_ptCorner[2].y);
   }
   ps.teLength=textLen;

   char* pText=new char[textLen];
   memcpy(pText,m_StyledText.GetTextPtr(),textLen);
   for(i=0;i<textLen;i++)
   {
      if(pText[i] == 0xA) pText[i]=0xD;
   }

   ps.chars = CString(pText,textLen);

   short nStyleCount=0;
   bool bLineFeed;
   CString text;
   CString theTextPlus1;
   CStyledTextStyle s;
   PhMDTextScript ste;
   CMemFile mf;
   int pos=m_StyledText.GetFirstTextPart();
   ctx.nCount = 0;
   memset(ctx.cCharset,0,sizeof(ctx.cCharset));
   while(pos<textLen)
   {
      nStyleCount++;
      ste.scrpStartChar=pos;
      m_StyledText.GetNextTextPart(pos,text,theTextPlus1,s,bLineFeed);

      long lFont;
      long lSize;
      Style Face;
      ctx.ConvertLogFont(s.m_LogFont,lFont,lSize,Face);
      ste.scrpFont = lFont;
      ste.scrpSize = lSize;
      ste.scrpFace = Face;
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
      ps.scripta.Add(ste);
      //uptr p((char*)&ste);
      //p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
      //mf.Write(&ste,sizeof(ScrpSTElement));
   }


   ps.scrCount = ps.scripta.GetCount();

   ctx.serialize(ps);

   SaveMacObjInfo(ctx);

   //pt.fNZScrpSize=sizeof(short) + nStyleCount*sizeof(ScrpSTElement);
   //p.pc=(char*)&pt;
   //p.swapl(4); p.swaps(1); p.swapl(1);
   //ctx.f->Write(&pt,sizeof(PhTextData));

   //ctx.f->Write(pText,textLen);
   delete[] pText;

   //p.pc=(char*)&nStyleCount;
   //p.swaps(1);
   //ctx.f->Write(&nStyleCount,sizeof(short));

   //long length=mf.GetLength();
   //void* pl=mf.Detach();
   //ctx.f->Write(pl,length);
   //free(pl);

   return 1;
}










