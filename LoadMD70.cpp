// MD7.0 - Infinisys Ltd
#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "FilletContext.h"
#include "ChamferContext.h"

#include "Layer.h"
#include "Import.h"
#include "PhIOStruct_MD70.h"
#include "ColIOStruct.h"

#include "DocMapper.h"
#include "DraftContext.h"

#include "MDLineShape.h"
#include "MDRectangle.h"
#include "MDRndRectangle.h"
#include "MDEllipse.h"
#include "MDCircle.h"
#include "MDArcShape.h"
#include "MDBezierShape.h"
#include "MDCubicShape.h"
#include "MDParallelLine.h"
#include "MDParallelPolygon.h"
#include "MDMetaPolyShape.h"
#include "MDDimStr.h"
#include "MDCenterDim.h"
#include "MDLineDim.h"
#include "MDRadialDim.h"
#include "MDAngularDim.h"
#include "MDText.h"
#include "MDBitmap.h"
#include "MDReport.h"
#include "MDArea.h"
#include "MDDimLine.h"
#include "SavedView.h"
#include "AttachContext.h"
#include <share.h>
#include <strstream>
#include "LoadRTF.h"

template < typename INTA >
   bool inta_contains(const INTA & inta,int i)
{
   for(int i = 0; i < inta.GetCount(); i++)
   {

      if(inta[i] == i)
      {
         return true;

      }

   }
   return false;
}
#define ARGB(a, r, g, b) (RGB((r), (g), (b)) | ((a) << 24))

const Coord kAlmostSameCriteria = 400.0 / 65536.0;

namespace formatMD70
{


   class process_layer
   {
   public:

      CMDShapeArray a;
      int iPos;
      CMap<UInt64,UInt64,CMDShape*,CMDShape*> shapes;
      CMap<UInt64,UInt64,CMDShape*,CMDShape*> areas;
      CMap<UInt64,UInt64,CMDDimLine*,CMDDimLine*> dims;
      CMap<UInt64,UInt64,CMDMetaPolyShape*,CMDMetaPolyShape*> metas;
      CMap<UInt64,UInt64,CArray<SInt64> *,CArray<SInt64> *> metasOrder;
      CMap<UInt64,UInt64,CMDShapeArray*,CMDShapeArray*> metaMembers;
      CMap<UInt64,UInt64,CMDGroupShape*,CMDGroupShape*> groups;
      CMap<UInt64,UInt64,CMDShapeList*,CMDShapeList*> groupMembers;
      CStringArray sValues;
      CArray < PhPropListElem > propa;
      bool m_bNewShape;

      process_layer(int numShapes)
      {
         shapes.InitHashTable(2047,1);
         groups.InitHashTable(2047,1);
         groupMembers.InitHashTable(2047,1);

         a.SetSize(numShapes * 2);

         iPos = numShapes * 2 - 1;


      }

      void on_read_new_shape()
      {
         sValues.RemoveAll();
         propa.RemoveAll();
         m_bNewShape = true;


      }

      void process_shape(PhMDShape * pshape,CMDShape* pNewShape,serialize & serialize,PenModel & penModel,FillModel & fm)
      {

         if(m_bNewShape)
         {

            if(pshape->the64ObjectInfo)
            {
             
               if(pNewShape)
               {

                  serialize.serialize_array(sValues,5);

               }
               //count=data.swaps();
               //if(pNewShape)
               //{
               //   PhObjectInfo* pInfo=(PhObjectInfo*)p.pc;
               //   p.swaps(2);
               //   count-=4;

               //   CString sValues[5];
               //   for(int j=0; j < 5; j++)
               //   {
               //      int len=*p.pc++;
               //      count--;
               //      sValues[j]=p.pc;
               //      p.skip(++len);
               //      count-=len;
               //   }
               //   pNewShape->SetValues(sValues);
               //}
               //p.skip(count);
            }

            if(pshape->theLong == 0 || pshape->theLong == 1)
               //if(pshape->theLong == 1)
            {
               UInt64 propCount = serialize.m_pointer.ui64();
               //TRACE("propCoutn=%ulld", propCount);
               if(propCount)
               {
                  for(UInt64 ui = 0; ui < propCount; ui++)
                  {
                     PhPropListElem a;
                     {

                        complex_type_context complextypecontext(&serialize);

                        serialize(a.fType);
                        serialize(a.fFlags);
                        serialize(a.fDataLength);
                        serialize(a.fArea);
                        serialize(a.fPerimeter);
                        serialize(a.fReserved2);
                        if(a.fDataLength != 0.0)
                        {
                           serialize(a.ptrLength);
                           serialize(a.data,a.ptrLength);
                        }
                     }
                     propa.Add(a);
                  }
               }
            }

         }

         if(pNewShape)
         {
            
            if(sValues.GetCount())
            {

               pNewShape->SetValues(sValues.GetData());

            }

            if(pNewShape->IsKindOf(RUNTIME_CLASS(CMDFillShape)))
            {
               pNewShape->SetFillModel(fm);
            }
            int iPenEnd = pshape->cShapeFlags & 0xc00;
            if(iPenEnd == 0x400) // round
            {
               penModel.makePenStyleRound();
            }
            pNewShape->SetPenModelDirect(penModel);
            pNewShape->SetLocked(pshape->cShapeFlags&kPhObjLockedMask?true:false);
            pNewShape->SetRotation(pshape->theAngle);
            ////pNewShape->SetShapeDatum(pshape->fDatumType);
            pNewShape->SetShapeDatum(pshape->cDatum);
            if(!pNewShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape))) pNewShape->CalculateExtents();
            shapes.SetAt(pshape->the64,pNewShape);
            if(pshape->cOwnerGroup)
            {

               if(pshape->cShapeFlags&kPhMetaPolyMemberMask)
               {

                  if (!pNewShape->IsClosed())
                  {

                     CMDShapeArray* pList;

                     if (!metaMembers.Lookup(pshape->cOwnerGroup, pList))
                     {
                        pList = new CMDShapeArray;
                        metaMembers.SetAt(pshape->cOwnerGroup, pList);
                     }
                     int iFind = -1;

                     for (int i = 0; i < metasOrder[pshape->cOwnerGroup]->GetCount(); i++)
                     {

                        auto i64 = metasOrder[pshape->cOwnerGroup]->ElementAt(i);

                        if (i64 == pshape->the64
                           && (i >= pList->GetCount() || pList->ElementAt(i) == NULL))
                        {

                           iFind = i;

                        }

                     }

                     if (iFind < 0)
                     {

                        for (int i = 0; i < metasOrder[pshape->cOwnerGroup]->GetCount(); i++)
                        {

                           if (metasOrder[pshape->cOwnerGroup]->ElementAt(i) == NULL)
                           {

                              iFind = i;

                           }

                        }


                     }

                     if (iFind < 0)
                     {

                        iFind = pList->GetCount();

                     }

                     pList->SetAtGrow(iFind,pNewShape);

                  }

               }
               else
               {
                  CMDShapeList* pList;
                  if(groupMembers.Lookup(pshape->cOwnerGroup,pList))
                  {
                     pList->AddTail(pNewShape);
                  }
                  else
                  {
                     CMDGroupShape * pgroup = NULL;
                     if(groups.Lookup(pshape->cOwnerGroup,pgroup) && pgroup != NULL)
                     {
                        pgroup->AddMember(pNewShape);
                        pNewShape = NULL;
                     }
                     else
                     {
                        pList=new CMDShapeList;
                        groupMembers.SetAt(pshape->cOwnerGroup,pList);
                        pList->AddTail(pNewShape);
                     }
                  }

               }
            }


            if(pNewShape)
            {

               if(m_bNewShape)
               {

                  a[iPos] = pNewShape;
                  iPos--;

               }
//               else
  //             {
               //
                 // a.InsertAt(iPos,pNewShape);

               //}

            }


         }

#if DEV_DEBUG

         strFormat.Format("%d\n",eid);

         strIds += strFormat;

#endif

         m_bNewShape = false;

      }

   };

} // namespace formatMD70

using namespace formatMD70;


Gdiplus::Bitmap * LoadPNG(const CArray<unsigned char> & data);
HBITMAP CreateDIB(Gdiplus::Bitmap * pbitmap, bool bSwap);

bool CPCDraftDoc::LoadMacDraft70(const char *szPath, bool bUtf8Name)
{

   CStdioFile file;

   CFILE f(NULL);

   if(bUtf8Name)
   {

      f.open(get_uni(szPath),L"rb",_SH_DENYNO);

      if(f.m_pf == NULL)
      {

         return false;

      }

      file.m_pStream = f.m_pf;

   }
   else
   {

      if(!file.Open(szPath,CFile::modeRead | CFile::typeBinary))
      {

         return false;

      }

   }

   ::formatMD70::PhIOContext ctx(&file);

   ctx.pDoc = this;

   ctx.serialize.m_bWrite = false;

   ctx.pPalette=&m_Palette;

   ctx.pDoc=this;

   PhDocState phDocState;

   memory mem;

   mem.allocate(960);

   file.Read(mem.get_data(),mem.get_size());

   serialize serialize;

   serialize.m_piocontext = &ctx;

   serialize.m_bWrite = false;

   //serialize.m_pointer.synch_type(mem.m_pdata);
   serialize.m_pointer.pch = mem.get_data();
   serialize.m_pointer.pend = mem.get_data() + mem.get_size();

   serialize(&phDocState);

   if(phDocState.fType != kPhDocType) return false;

   if(phDocState.fHeaderID != kPhFileHdr || phDocState.fVersion != kPhRelease8) return false;

   //MDFooter mdf;
   //fi.Seek(-(int)sizeof(MDFooter),CFile::end);
   //fi.Read(&mdf,sizeof(MDFooter));
   //uptr((char*)&mdf).swapl(7);
   //if(mdf.tag1 != 'RBAL' || mdf.tag2 != 'RPH ' || mdf.version != 0x10000 || mdf.type != 'RSRC') return false;

   //fi.Seek(-mdf.offset,CFile::end);
   //char *resFork=new char[mdf.numBytes];
   //fi.Read(resFork,mdf.numBytes);
   //swapMDR(resFork,mdf.numBytes);

   //// used fonts
   //long len;
   //char* pFont=GetResource('FNUS',1,resFork,mdf.numBytes,&len);
   //if(pFont)
   //{
   //   uptr p(pFont);
   //   while(len > 0)
   //   {
   //      WORD font=p.swaps();
   //      unsigned char l = *(unsigned char *)(p.pc);
   //      CString str = CString((const char *)(p.pc + 1),l);
   //      int iFind = str.Find("|");
   //      if(iFind >= 0)
   //      {
   //         ctx.fontCharset[font] = atoi(str.Mid(iFind + 1));
   //         ctx.fontMap.SetAt(font,str.Left(iFind));
   //      }
   //      else
   //      {
   //         ctx.fontMap.SetAt(font,str);
   //      }
   //      p.pc += l + 1;
   //      len-=(3 + l);
   //      //p.p2c_nt();
   //   }
   //}

   // copy doc data
   m_PageSize.cx=phDocState.fWindowRect.r - phDocState.fWindowRect.l;
   m_PageSize.cy=phDocState.fWindowRect.b - phDocState.fWindowRect.t;
   //xxx m_PrinterSettings.SetCustomPageSize(MulDiv(phDocState.fPrintRec.rPaper.r - phDocState.fPrintRec.rPaper.l,254,72),MulDiv(phDocState.fPrintRec.rPaper.b - phDocState.fPrintRec.rPaper.t,254,72));
   //xxx m_PrinterSettings.SetPageOrientation((phDocState.fPrintRec.prStl.wDev && !(phDocState.fPrintRec.prStl.wDev & 0x2))?DMORIENT_LANDSCAPE:DMORIENT_PORTRAIT);

   m_NumPage.x=phDocState.fMViewPages.x;
   m_NumPage.y=phDocState.fMViewPages.y;
   if((phDocState.fDocUnits.fDefUnits & kUEnglishMask) == 0/*!m_bEnglishUnits*/ && phDocState.fDocUnits.fDefScaleIndex>21) phDocState.fDocUnits.fDefScaleIndex--;
   //SetupUnits(phDocState.fDocUnits.fDefUnits,false,phDocState.fDocUnits.fDefAngleUnits>1024?(1024 - phDocState.fDocUnits.fDefAngleUnits):(phDocState.fDocUnits.fDefAngleUnits >> 8),phDocState.fDocUnits.fDefPlaces,phDocState.fDocUnits.fDefAnglePlaces,phDocState.fDocUnits.fDefScaleIndex,NULL,0,&phDocState.fDocUnits.fDefScaleRatio);
   SetupUnits(phDocState.fDocUnits.fDefUnits,false,phDocState.fDocUnits.fDefAngleUnits>1024?(1024 - phDocState.fDocUnits.fDefAngleUnits):(phDocState.fDocUnits.fDefAngleUnits >> 8),phDocState.fDocUnits.fDefPlaces,phDocState.fDocUnits.fDefAnglePlaces,phDocState.fDocUnits.fDefScaleIndex,NULL,0);
   if(m_bEnglishUnits)
   {
      ctx.ConvertPhDimStd(phDocState.fDocStds.fEnglishDimStd,m_DimStd);
      for(int j=0; j<3; j++)
      {
         ctx.ConvertPhDimFmt(phDocState.fDocStds.fEnglishDimFormat[j],m_DimFmts[j]);
      }
   }
   else
   {
      ctx.ConvertPhDimStd(phDocState.fDocStds.fMetricDimStd,m_DimStd);
      for(int j=0; j<3; j++)
      {
         ctx.ConvertPhDimFmt(phDocState.fDocStds.fMetricDimFormat[j],m_DimFmts[j]);
      }
   }

   m_RulerOrigin=phDocState.fRulerOrigin;

   m_NumPage.x = phDocState.fMViewPages.x;
   m_NumPage.y = phDocState.fMViewPages.y;

   int iPrevZoom = phDocState.fActiveEnvirons.fPrevZoom;

//
   ctx.ConvertPhFillModel(phDocState.fActiveEnvirons.fFillModel,m_Fill);
   ctx.ConvertPhPenModel(phDocState.fActiveEnvirons.fPenModel,m_Pen);
//	m_TextParams.m_lf=phDocState.fActiveEnvirons.fTextParams.
   ctx.ConvertPhTextParams(phDocState.fActiveEnvirons.fTextParams,m_TextParams);
   ctx.ConvertPhParaAttr(phDocState.fActiveEnvirons.fParallelAttr,m_ParallelAttrs);
//
////	fDimVisible
   m_bHideGridLines=phDocState.fActiveEnvirons.fHasGrid?false:true;
   m_bHideText=phDocState.fActiveEnvirons.fHideText?true:false;
   m_bShowRulers=phDocState.fActiveEnvirons.fShowRulers?true:false;
   m_bShowGuideLines = phDocState.fActiveEnvirons.fShowGuides ? true : false; // Infinisys Ltd. 2020 Guides
   m_ptPrevZoomScroll.x=phDocState.fActiveEnvirons.screenViewRectOrigin.x;
   m_ptPrevZoomScroll.y=phDocState.fActiveEnvirons.screenViewRectOrigin.y;

   m_bSnapToObject=phDocState.fActiveEnvirons.fSnapToObject?true:false;
   m_bStandardRulers=phDocState.fActiveEnvirons.fStandardRulers?true:false;
   m_bHidePageBreaks=phDocState.fActiveEnvirons.fHidePageBreaks?true:false;

   m_bPrintLastPageFirst=phDocState.fActiveEnvirons.fPrintFlags&kPhPrintLastFirst?true:false;
   m_nPageNumbering=phDocState.fActiveEnvirons.fPrintFlags&kPhPrintByCols?0:1;
   m_bRegistrationMarks=phDocState.fActiveEnvirons.fPrintFlags&kPhRegMarks?true:false;

   m_nDisplayRes=phDocState.fDocUnits.fDefResolution.x;

   m_bSnapToGuides = phDocState.fActiveEnvirons.fSnapToGuides ? true : false;

   m_fZoomFactor = phDocState.fActiveEnvirons.fZoomFactor;
   //swap64(&phDocState.fActiveEnvirons.fTrunk.fZoomFactor);
#ifdef _FLOAT_ZOOM
   //UnpackZoomFactor(phDocState.fActiveEnvirons.fPrevZoom,m_nPrevZoomFactor,m_fPrevZoomFactor);
   //UnpackDoubleZoomFactor(phDocState.fActiveEnvirons.fZoomFactor,m_nZoomFactor,m_fZoomFactor);
#else
   m_nPrevZoomFactor=phDocState.fActiveEnvirons.fPrevZoom;
   m_nZoomFactor=phDocState.fZoom;
#endif

   m_snapTable.setAngleSnaps(phDocState.fActiveEnvirons.fAngleSnapsToACircle);
	//m_nDisplayRes=phDocState.fActiveEnvirons.;
	//m_bHideFills=phDocState.fActiveEnvirons.bh;
//
   CPCDraftView* pView=NULL;
   POSITION pos=GetFirstViewPosition();
   if(pos) pView=(CPCDraftView*)GetNextView(pos); // 1 view assumed
//   if(pView)
//   {
//      pView->m_pFilletCtx->m_bSmallerArc=phDocState.fActiveEnvirons.fFilletAttr.fIsSmallerArc?true:false;
//      pView->m_pFilletCtx->m_fRadius=phDocState.fActiveEnvirons.fFilletAttr.fRadius;
//      pView->m_pFilletCtx->m_nTrimMethod=(kPhAutoTrim - phDocState.fActiveEnvirons.fFilletAttr.fTrimMethod);
//
//      pView->m_pChamferCtx->m_fLine1SetBack=phDocState.fActiveEnvirons.fChamferAttr.fLine1SetBack;
//      pView->m_pChamferCtx->m_fLine2SetBack=phDocState.fActiveEnvirons.fChamferAttr.fLine2SetBack;
//      pView->m_pChamferCtx->m_nTrimMethod=(kPhAutoTrim - phDocState.fActiveEnvirons.fChamferAttr.fTrimMethod);
//   }

// layers
//   PhFListHeader* pList=(PhFListHeader*)GetResource('LAYI',0,resFork,mdf.numBytes);
//   if(!pList) return false;
//   uptr p((char*)pList);
//   p.swaps(1); p.swapl(1); p.swaps(1);
//   p.swapl(3); p.swaps(3); p.swapl(1);
//
//// dash styles
//   char* pDash=GetResource('DASH',128,resFork,mdf.numBytes);
//   if(pDash)
//   {
//      memcpy(m_DashStyles,pDash,256);
//      delete pDash;
//   }
//


//// palette
//   char* pPal=GetResource('pltt',128,resFork,mdf.numBytes);
//   if(pPal)
//   {
//      m_Palette.DeleteObject();
//
//      uptr p(pPal);
//      int nCount=p.swaps();
//      p.swapl(1); p.swaps(1); p.swapl(2);
//
//      LPLOGPALETTE lp=(LPLOGPALETTE)new BYTE[sizeof(LOGPALETTE) + (nCount - 1)*sizeof(PALETTEENTRY)];
//      lp->palVersion=0x300;
//      lp->palNumEntries=nCount;
//      for(int j=0; j<nCount; j++)
//      {
//         lp->palPalEntry[j].peRed=p.swaps() >> 8;
//         lp->palPalEntry[j].peGreen=p.swaps() >> 8;
//         lp->palPalEntry[j].peBlue=p.swaps() >> 8;
//         lp->palPalEntry[j].peFlags=0;
//         p.swaps(5);
//      }
//
//      m_Palette.CreatePalette(lp);
//      delete[] lp;
//      delete pPal;
//   }
//


   OffsetHeader header;

   CString strPaper;
   int iPageScale = 100;

   int iOrientation = -1;

   int iCustomPaperWidth = -1;

   int iCustomPaperHeight = -1;

   bool bCustomPaper = false;

   if(m_bUpdateViews)
   {

      if(header.read(serialize.m_pointer,mem,file,kMarkerFlatPageFormat))
      {

         long * pl = (long *)mem.get_data();

         if (mem.get_size() >= 8)
         {

            ::formatMD70::swap32(&pl[0]);
            ::formatMD70::swap32(&pl[1]);

         }

         int iDocSize = pl[0];

         int iMemSize = mem.get_size() - 8;

         int iSize = min(iDocSize, iMemSize);

         CString str((char *)&pl[2], iSize);

         ::OutputDebugString(str);

         XDoc	doc;

         if(!doc.Load(str))
         {
            goto end_flat_page_format;
         }

         XNode * plist = doc.GetChild("plist");

         if(plist == NULL)
         {

            goto end_flat_page_format;

         }

         XNode * pmain = plist->GetChild("dict");

         if(pmain == NULL)
         {

            goto end_flat_page_format;

         }

         int iFind = pmain->IndexByValue("key","com.apple.print.subTicket.paper_info_ticket");

         if(iFind >= 0)
         {

            XNode * pdict = pmain->GetChild(iFind + 1);

            if(pdict != NULL && pdict->name == "dict")
            {

               int iFind2 = pdict->IndexByValue("key","com.apple.print.PageFormat.PMAdjustedPageRect");

               if(iFind2 >= 0)
               {

                  XNode * pdict2 = pdict->GetChild(iFind2 + 1);

                  if(pdict2 != NULL && pdict2->name == "dict")
                  {

                     int iFind3 = pdict2->IndexByValue("key","com.apple.print.ticket.itemArray");

                     if(iFind3 >= 0)
                     {

                        XNode * parray = pdict2->GetChild(iFind3 + 1);

                        if(parray != NULL && parray->name == "array")
                        {

                           XNode * pdict3 = parray->GetChild(0);

                           if(pdict3 != NULL && pdict3->name == "dict")
                           {

                              int iFind4 = pdict3->IndexByValue("key","com.apple.print.PageFormat.PMAdjustedPageRect");

                              if(iFind4 >= 0)
                              {

                                 XNode * parray = pdict3->GetChild(iFind4 + 1);

                                 if(parray)
                                 {

                                    CArray < double > da;

                                    if(parray->get_child_numbera(da) && da.GetCount() == 4)
                                    {

                                       iCustomPaperHeight = da[2] - da[0];

                                       iCustomPaperWidth = da[3] - da[1];

                                    }

                                 }

                              }

                           }

                        }

                     }

                  }

               }

                int iFind2B = pdict->IndexByValue("key","com.apple.print.PaperInfo.PMCustomPaper");

                  if(iFind2B >= 0)
                  {

                     XNode * pdict2 = pdict->GetChild(iFind2B + 1);

                     if(iFind2 >= 0)
                     {

                        XNode * pdict2 = pdict->GetChild(iFind2B + 1);

                        if(pdict2 != NULL && pdict2->name == "dict")
                        {

                           int iFind3 = pdict2->IndexByValue("key","com.apple.print.ticket.itemArray");

                           if(iFind3 >= 0)
                           {

                              XNode * parray = pdict2->GetChild(iFind3 + 1);

                              if(parray != NULL && parray->name == "array")
                              {

                                 XNode * pdict3 = parray->GetChild(0);

                                 if(pdict3 != NULL && pdict3->name == "dict")
                                 {

                                    int iFind4 = pdict3->IndexByValue("key","com.apple.print.PaperInfo.PMCustomPaper");

                                    if(iFind4 >= 0)
                                    {

                                       XNode * pbool= pdict3->GetChild(iFind4+1);

                                       if(pbool && pbool->name.CompareNoCase("true") == 0)
                                       {

                                          bCustomPaper = true;

                                       }

                                    }

                                 }


                              }


                           }

                        }


                  }

               }
               
               if(!pdict->get_pinfo_string(strPaper,"PMPPDPaperCodeName"))
               {

                  if(!pdict->get_pinfo_string(strPaper,"com.apple.print.PaperInfo.PMPaperName"))
                  {


                  }

               }


            }

         }


         double dScale;

         if(pmain->get_pinfo_real(dScale,"com.apple.print.PageFormat.PMScaling"))
         {

            iPageScale = (int)(dScale * 100.0);

         }

         if(strPaper.IsEmpty())
         {

            if(!pmain->get_pinfo_string(strPaper,"com.apple.print.PaperInfo.PMCodeName"))
            {

               pmain->get_pinfo_string(strPaper,"com.apple.print.PaperInfo.PMPaperName");


            }

         }


         if(iOrientation < 0)
         {

            pmain->get_pinfo_integer(iOrientation,"com.apple.print.PageFormat.PMOrientation");

         }

      }

   }
end_flat_page_format:;

   strPaper.MakeLower();

   int iPaper = DMPAPER_LETTER;

/*   DMPAPER_LETTER	Letter,8 1 / 2 - by 11 - inches
      DMPAPER_LEGAL	Legal,8 1 / 2 - by 14 - inches
      DMPAPER_9X11	9 - by 11 - inch sheet
      DMPAPER_10X11	10 - by 11 - inch sheet
      DMPAPER_10X14	10 - by 14 - inch sheet
      DMPAPER_15X11	15 - by 11 - inch sheet
      DMPAPER_11X17	11 - by 17 - inch sheet
      DMPAPER_12X11	12 - by 11 - inch sheet
      DMPAPER_A2	A2 sheet,420 x 594 - millimeters
      DMPAPER_A3	A3 sheet,297 - by 420 - millimeters
      DMPAPER_A3_EXTRA	A3 Extra 322 x 445 - millimeters
      DMPAPER_A3_EXTRA_TRAVERSE	A3 Extra Transverse 322 x 445 - millimeters
      DMPAPER_A3_ROTATED	A3 rotated sheet,420 - by 297 - millimeters
      DMPAPER_A3_TRAVERSE	A3 Transverse 297 x 420 - millimeters
      DMPAPER_A4	A4 sheet,210 - by 297 - millimeters
      DMPAPER_A4_EXTRA	A4 sheet,9.27 x 12.69 inches
      DMPAPER_A4_PLUS	A4 Plus 210 x 330 - millimeters
      DMPAPER_A4_ROTATED	A4 rotated sheet,297 - by 210 - millimeters
      DMPAPER_A4SMALL	A4 small sheet,210 - by 297 - millimeters
      DMPAPER_A4_TRANSVERSE	A4 Transverse 210 x 297 millimeters
      DMPAPER_A5	A5 sheet,148 - by 210 - millimeters
      DMPAPER_A5_EXTRA	A5 Extra 174 x 235 - millimeters
      DMPAPER_A5_ROTATED	A5 rotated sheet,210 - by 148 - millimeters
      DMPAPER_A5_TRANSVERSE	A5 Transverse 148 x 210 - millimeters
      DMPAPER_A6	A6 sheet,105 - by 148 - millimeters
      DMPAPER_A6_ROTATED	A6 rotated sheet,148 - by 105 - millimeters
      DMPAPER_A_PLUS	SuperA / A4 227 x 356 - millimeters
      DMPAPER_B4	B4 sheet,250 - by 354 - millimeters
      DMPAPER_B4_JIS_ROTATED	B4(JIS) rotated sheet,364 - by 257 - millimeters
      DMPAPER_B5	B5 sheet,182 - by 257 - millimeter paper
      DMPAPER_B5_EXTRA	B5(ISO) Extra 201 x 276 - millimeters
      DMPAPER_B5_JIS_ROTATED	B5(JIS) rotated sheet,257 - by 182 - millimeters
      DMPAPER_B6_JIS	B6(JIS) sheet,128 - by 182 - millimeters
      DMPAPER_B6_JIS_ROTATED	B6(JIS) rotated sheet,182 - by 128 - millimeters
      DMPAPER_B_PLUS	SuperB / A3 305 x 487 - millimeters
      DMPAPER_CSHEET	C Sheet,17 - by 22 - inches
      DMPAPER_DBL_JAPANESE_POSTCARD	Double Japanese Postcard,200 - by 148 - millimeters
      DMPAPER_DBL_JAPANESE_POSTCARD_ROTATED	Double Japanese Postcard Rotated,148 - by 200 - millimeters
      DMPAPER_DSHEET	D Sheet,22 - by 34 - inches
      DMPAPER_ENV_9	#9 Envelope,3 7 / 8 - by 8 7 / 8 - inches
      DMPAPER_ENV_10	#10 Envelope,4 1 / 8 - by 9 1 / 2 - inches
      DMPAPER_ENV_11	#11 Envelope,4 1 / 2 - by 10 3 / 8 - inches
      DMPAPER_ENV_12	#12 Envelope,4 3 / 4 - by 11 - inches
      DMPAPER_ENV_14	#14 Envelope,5 - by 11 1 / 2 - inches
      DMPAPER_ENV_C5	C5 Envelope,162 - by 229 - millimeters
      DMPAPER_ENV_C3	C3 Envelope,324 - by 458 - millimeters
      DMPAPER_ENV_C4	C4 Envelope,229 - by 324 - millimeters
      DMPAPER_ENV_C6	C6 Envelope,114 - by 162 - millimeters
      DMPAPER_ENV_C65	C65 Envelope,114 - by 229 - millimeters
      DMPAPER_ENV_B4	B4 Envelope,250 - by 353 - millimeters
      DMPAPER_ENV_B5	B5 Envelope,176 - by 250 - millimeters
      DMPAPER_ENV_B6	B6 Envelope,176 - by 125 - millimeters
      DMPAPER_ENV_DL	DL Envelope,110 - by 220 - millimeters
      DMPAPER_ENV_INVITE	Envelope Invite 220 x 220 mm
      DMPAPER_ENV_ITALY	Italy Envelope,110 - by 230 - millimeters
      DMPAPER_ENV_MONARCH	Monarch Envelope,3 7 / 8 - by 7 1 / 2 - inches
      DMPAPER_ENV_PERSONAL	6 3 / 4 Envelope,3 5 / 8 - by 6 1 / 2 - inches
      DMPAPER_ESHEET	E Sheet,34 - by 44 - inches
      DMPAPER_EXECUTIVE	Executive,7 1 / 4 - by 10 1 / 2 - inches
      DMPAPER_FANFOLD_US	US Std Fanfold,14 7 / 8 - by 11 - inches
      DMPAPER_FANFOLD_STD_GERMAN	German Std Fanfold,8 1 / 2 - by 12 - inches
      DMPAPER_FANFOLD_LGL_GERMAN	German Legal Fanfold,8 - by 13 - inches
      DMPAPER_FOLIO	Folio,8 1 / 2 - by 13 - inch paper
      DMPAPER_ISO_B4	B4(ISO) 250 - by 353 - millimeters paper
      DMPAPER_JAPANESE_POSTCARD	Japanese Postcard,100 - by 148 - millimeters
      DMPAPER_JAPANESE_POSTCARD_ROTATED	Japanese Postcard Rotated,148 - by 100 - millimeters
      DMPAPER_JENV_CHOU3	Japanese Envelope Chou #3
      DMPAPER_JENV_CHOU3_ROTATED	Japanese Envelope Chou #3 Rotated
      DMPAPER_JENV_CHOU4	Japanese Envelope Chou #4
      DMPAPER_JENV_CHOU4_ROTATED	Japanese Envelope Chou #4 Rotated
      DMPAPER_JENV_KAKU2	Japanese Envelope Kaku #2
      DMPAPER_JENV_KAKU2_ROTATED	Japanese Envelope Kaku #2 Rotated
      DMPAPER_JENV_KAKU3	Japanese Envelope Kaku #3
      DMPAPER_JENV_KAKU3_ROTATED	Japanese Envelope Kaku #3 Rotated
      DMPAPER_JENV_YOU4	Japanese Envelope You #4
      DMPAPER_JENV_YOU4_ROTATED	Japanese Envelope You #4 Rotated
      DMPAPER_LAST	DMPAPER_PENV_10_ROTATED
      DMPAPER_LEDGER	Ledger,17 - by 11 - inches
      DMPAPER_LEGAL_EXTRA	Legal Extra 9 1 / 2 x 15 inches.
      DMPAPER_LETTER_EXTRA	Letter Extra 9 1 / 2 x 12 inches.
      DMPAPER_LETTER_EXTRA_TRANSVERSE	Letter Extra Transverse 9 1 / 2 x 12 inches.
      DMPAPER_LETTER_ROTATED	Letter Rotated 11 by 8 1 / 2 inches
      DMPAPER_LETTERSMALL	Letter Small,8 1 / 2 - by 11 - inches
      DMPAPER_LETTER_TRANSVERSE	Letter Transverse 8 1 / 2 x 11 - inches
      DMPAPER_NOTE	Note,8 1 / 2 - by 11 - inches
      DMPAPER_P16K	PRC 16K,146 - by 215 - millimeters
      DMPAPER_P16K_ROTATED	PRC 16K Rotated,215 - by 146 - millimeters
      DMPAPER_P32K	PRC 32K,97 - by 151 - millimeters
      DMPAPER_P32K_ROTATED	PRC 32K Rotated,151 - by 97 - millimeters
      DMPAPER_P32KBIG	PRC 32K(Big)97 - by 151 - millimeters
      DMPAPER_P32KBIG_ROTATED	PRC 32K(Big)Rotated,151 - by 97 - millimeters
      DMPAPER_PENV_1	PRC Envelope #1,102 - by 165 - millimeters
      DMPAPER_PENV_1_ROTATED	PRC Envelope #1 Rotated,165 - by 102 - millimeters
      DMPAPER_PENV_2	PRC Envelope #2,102 - by 176 - millimeters
      DMPAPER_PENV_2_ROTATED	PRC Envelope #2 Rotated,176 - by 102 - millimeters
      DMPAPER_PENV_3	PRC Envelope #3,125 - by 176 - millimeters
      DMPAPER_PENV_3_ROTATED	PRC Envelope #3 Rotated,176 - by 125 - millimeters
      DMPAPER_PENV_4	PRC Envelope #4,110 - by 208 - millimeters
      DMPAPER_PENV_4_ROTATED	PRC Envelope #4 Rotated,208 - by 110 - millimeters
      DMPAPER_PENV_5	PRC Envelope #5,110 - by 220 - millimeters
      DMPAPER_PENV_5_ROTATED	PRC Envelope #5 Rotated,220 - by 110 - millimeters
      DMPAPER_PENV_6	PRC Envelope #6,120 - by 230 - millimeters
      DMPAPER_PENV_6_ROTATED	PRC Envelope #6 Rotated,230 - by 120 - millimeters
      DMPAPER_PENV_7	PRC Envelope #7,160 - by 230 - millimeters
      DMPAPER_PENV_7_ROTATED	PRC Envelope #7 Rotated,230 - by 160 - millimeters
      DMPAPER_PENV_8	PRC Envelope #8,120 - by 309 - millimeters
      DMPAPER_PENV_8_ROTATED	PRC Envelope #8 Rotated,309 - by 120 - millimeters
      DMPAPER_PENV_9	PRC Envelope #9,229 - by 324 - millimeters
      DMPAPER_PENV_9_ROTATED	PRC Envelope #9 Rotated,324 - by 229 - millimeters
      DMPAPER_PENV_10	PRC Envelope #10,324 - by 458 - millimeters
      DMPAPER_PENV_10_ROTATED	PRC Envelope #10 Rotated,458 - by 324 - millimeters
      DMPAPER_QUARTO	Quarto,215 - by 275 - millimeter paper
      DMPAPER_STATEMENT	Statement,5 1 / 2 - by 8 1 / 2 - inches
      DMPAPER_TABLOID	Tabloid,11 - by 17 - inches
      DMPAPER_TABLOID_EXTRA	Tabloid,11.69 x 18 - inches*/  

   if(strPaper == "archb")
   {

      iPaper = DMPAPER_TABLOID_EXTRA;

   }
   else if(strPaper == "legal")
   {

      iPaper = DMPAPER_LEGAL;

   }
   else if(strPaper == "letter")
   {

      iPaper = DMPAPER_LETTER;

   }
   else if(strPaper == "roc16k")
   {

      iPaper = DMPAPER_B5_EXTRA;

   }
   else if(strPaper == "tabloid" || strPaper.Find("tabloid") >= 0)
   {
      
      iPaper = DMPAPER_TABLOID;

   }
   else if(strPaper == "envchou3")
   {

      iPaper = DMPAPER_JENV_CHOU3;

   }
   else if(strPaper == "13x19")
   {

      iPaper = DMPAPER_B_PLUS;

   }
   else if(strPaper == "envdl")
   {

      iPaper = DMPAPER_ENV_DL;

   }
   else if(strPaper == "letter")
   {

      iPaper = DMPAPER_LETTER;

   }
   else if(strPaper == "b5")
   {

      iPaper = DMPAPER_B5;

   }
   else if(strPaper == "isob5")
   {

      iPaper = DMPAPER_ENV_B5;

   }
   else if(strPaper == "iso-a5" || strPaper == "a5")
   {

      iPaper = DMPAPER_A5;

   }
   else if(strPaper == "iso-a4" || strPaper == "a4")
   {

      iPaper = DMPAPER_A4;

   }
   else if(strPaper == "iso-a3" || strPaper == "a3" || strPaper == "a3jis")
   {

      iPaper = DMPAPER_A3;

   }
   else if(strPaper == "env10")
   {

      iPaper = DMPAPER_ENV_10;

   }
   else
   {

      iPaper = DMPAPER_LETTER;

   }

   if(m_bUpdateViews)
   {

      if(bCustomPaper && iCustomPaperWidth > 0 && iCustomPaperHeight > 0)
      {

         if(iOrientation == 2)
         {

            m_PrinterSettings.SetCustomPageSize(MulDiv(iCustomPaperHeight,254,72),MulDiv(iCustomPaperWidth,254,72));

         }
         else
         {

            m_PrinterSettings.SetCustomPageSize(MulDiv(iCustomPaperWidth,254,72),MulDiv(iCustomPaperHeight,254,72));

         }

      }
      else
      {

         m_PrinterSettings.SetPageSize(iPaper);

      }

      m_PrinterSettings.SetPageScale(iPageScale);

      m_PrinterSettings.SetPageOrientation(iOrientation == 2 ? DMORIENT_LANDSCAPE :  DMORIENT_PORTRAIT);

   }

   if(header.read(serialize.m_pointer,mem,file,kMarkerFlatPrintSettings))
   {
      long * pl = (long *)mem.get_data();

      if (mem.get_size() >= 8)
      {

         ::formatMD70::swap32(&pl[0]);
         ::formatMD70::swap32(&pl[1]);

      }

      int iDocSize = pl[0];

      int iMemSize = mem.get_size() - 8;

      int iSize = min(iDocSize, iMemSize);

      CString str((char *)&pl[2], iSize);

      ::OutputDebugString(str);
   }

   //CArray < int > fontNum;
   //CStringArray fontName;

   //if(header.read(serialize.m_pointer,mem,file,kMarkerFontInUseInfo))
   //{
   //   
   //   long fontCount = serialize.m_pointer.i32();

   //   for(int j = 0; j < fontCount; j++)
   //   {

   //      int iFontNum = serialize.m_pointer.sh();

   //      fontNum.Add(iFontNum);

   //      char len = serialize.m_pointer.ch();

   //      CString strFontName(serialize.m_pointer.pch,len);

   //      OutputDebugString(strFontName);

   //      fontName.Add(strFontName);

   //      ctx.fontMap.SetAt(iFontNum,strFontName);

   //      serialize.m_pointer.pch += len;

   //   }

   //}

   if(m_bUpdateViews)
   {

      CSize thePageSize=CalcPrinterPageSize();
      if(thePageSize != m_PageSize)
      {
         m_PageSize=thePageSize;
         CheckDocSize(true);
         UpdateAllViews(NULL,kDocumentSizeChanged);
         //SetModifiedFlag();
      }
      //UpdateAllViews(NULL,kLockWindowUpdate);

   }

   if(header.read(serialize.m_pointer,mem,file,kMarkerPalette))
   {

      UInt64 thePalSize = serialize.m_pointer.i64();

      if(thePalSize > 0)
      {

         short entries = serialize.m_pointer.sh();

         if(entries > 0)
         {

//            short theShort[7];

//            serialize.serialize_static_array(theShort,7);

            m_Palette.DeleteObject();

            memory memPal;

            memPal.allocate(sizeof(LOGPALETTE) + (entries - 1)*sizeof(PALETTEENTRY));

            PhPaletteEntry entry;

            LPLOGPALETTE lp = (LPLOGPALETTE) memPal.get_data();
            lp->palVersion=0x300;
            lp->palNumEntries=entries;
            for(int j=0; j < entries; j++)
            {
               
               serialize(entry);

               lp->palPalEntry[j].peRed=entry.ciRGB.fRed * 255.f;
               lp->palPalEntry[j].peGreen=entry.ciRGB.fGreen* 255.f;
               lp->palPalEntry[j].peBlue=entry.ciRGB.fBlue * 255.f;
               lp->palPalEntry[j].peFlags=0;

            }

            m_Palette.CreatePalette(lp);

         }

      }

   }

   if(header.read(serialize.m_pointer, mem,file,kMarkerGenericPattern))
   {

      PhGenericPatternHeader header;

      serialize(&header);

      TRACE("numofpats %d",header.numItems);

      UInt64 cCount = header.numItems;

      char * pnext;

      if(header.sizeVersionNumber == 0x18 && header.idOfVersionNumber == 0x1000030 && header.versionNumber == 0x6000000000000LL)
      {

         pnext = serialize.m_pointer.pch;

         cCount--;

      }
      else if(header.idOfVersionNumber == 0x1000011)
      {

         serialize.m_pointer.pch = mem.get_data();

         serialize(&header); // swap again to original value

         serialize.m_pointer.pch = mem.get_data() + sizeof(UInt64);

         pnext = serialize.m_pointer.pch;

      }
      else
      {
         
         goto end_pat;

      }

      CPackedItem item;

      m_packeditema.RemoveAll();

      m_numDIBPatterns = 0;
      m_numGradients = 0;

      for(int j=0; j < cCount; j++)
      {

         PatternBase base;

         serialize.m_pointer.pch = pnext;

         serialize(&base);

         pnext += base.Size;

         if(base.Id == IDPixelPattern)
         {

            PixelPattern pattern;

            pattern.Size = base.Size;
            pattern.Id = base.Id;

            serialize(&pattern);

            HBITMAP hDib = NULL;
            if(pattern.fBitsPerChannel == 8)
            {
               BITMAPINFOHEADER bmih;
               bmih.biSize=sizeof(BITMAPINFOHEADER);
               bmih.biWidth=pattern.fWidth;
               bmih.biHeight=pattern.fHeight;
               bmih.biPlanes=1;
               bmih.biBitCount=32;
               bmih.biCompression=BI_RGB;
               bmih.biSizeImage=0;
               bmih.biXPelsPerMeter=0;
               bmih.biYPelsPerMeter=0;
               bmih.biClrUsed=0;
               bmih.biClrImportant=0;
               bmih.biSizeImage = sizeof(long) * bmih.biWidth *bmih.biHeight;

               COLORREF * lpBits;
               hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,(void **)&lpBits,NULL,0);
               COLORREF * lpSrc = (COLORREF *)pattern.data.GetData();
               for(int y = 0; y < bmih.biHeight; y++)
               {

                  memcpy(&lpBits[bmih.biWidth * y],&lpSrc[bmih.biWidth * (bmih.biHeight - y - 1)],bmih.biWidth * sizeof(COLORREF));
                  unsigned char * pline = (unsigned char *) &lpBits[bmih.biWidth * y];
                  for(int x = 0; x < bmih.biWidth; x++)
                  {
                     unsigned char ch = pline[0];
                     pline[0] = pline[2];
                     pline[2] = ch;
                     pline +=4;
                  }


               }

            }
            if(hDib)
            {
               // First we need to check if there is such pattern already and only if
               // don't  - insert it into palette. First 71 entries - it's a system
               // palette, don't changed.
               //if(j<72)
               //{
               //   m_Patterns[j - 1].FromDIBSection(hDib);
               //   m_PatternsHash[j - 1] = CalculateDIBHash(hDib);
               //   ::DeleteObject(hDib);
               //   //						if(m_DIBPatterns[j-1]) DeleteObject(m_DIBPatterns[j-1]);
               //   //						m_DIBPatterns[j-1]=hDib;
               //}
               //else
               //{
               //   // by TSM
               int found = 0;
               int hash = CalculateDIBHash(hDib);
               //for(int k=1; k <= m_numDIBPatterns; k++)
               //{
               //   if(hash == m_PatternsHash[k - 1])
               //   {
               //      HBITMAP hDib2 = m_Patterns[k - 1].CreateDIBSection();
               //      if(!CompareDIBs(hDib,hDib2))
               //      {
               //         ::DeleteObject(hDib2);
               //         found = k; break;
               //      }
               //      ::DeleteObject(hDib2);
               //   }
               //}
               //if(found)
               //{
               //   ctx.embedDIBsIndex.Add((WORD)(found - 1));
               //   ::DeleteObject(hDib);
               //}
               //else
               {
                  ctx.embedDIBsIndex.Add((WORD)m_numDIBPatterns);
                  m_PatternsHash[m_numDIBPatterns] = CalculateDIBHash(hDib);
                  item.m_pdib = &m_Patterns[m_numDIBPatterns];
                  item.m_bOwn = false;
                  item.m_etype = packed_type_dib;
                  m_Patterns[m_numDIBPatterns++].FromDIBSection(hDib);
                  ::DeleteObject(hDib);
                  m_packeditema.Add(item);
               }
               // end by TSM
               //ctx.embedDIBs.Add((DWORD)hDib);
            }


         }
         else if(base.Id == IDBlendPattern)
         {

            int iGradient = m_numGradients;

            BlendPattern pattern;

            pattern.Size = base.Size;
            pattern.Id = base.Id;

            serialize(&pattern);

            m_Gradients[iGradient].reset();
            m_Gradients[iGradient].m_bRadial = !pattern.fLinearNotRadial;
            m_Gradients[iGradient].m_dX = pattern.fCenterX - 0.5f;
            m_Gradients[iGradient].m_dY = pattern.fCenterY - 0.5f;
            m_Gradients[iGradient].m_dRotation = pattern.fRotation;

            for(int iGrad = 0; iGrad < pattern.data.GetCount(); iGrad++)
            {
               m_Gradients[iGradient].m_cra.Add(
               RGB(byte_clip(pattern.data[iGrad].fRed * 255.f),
                   byte_clip(pattern.data[iGrad].fGreen* 255.f),
                   byte_clip(pattern.data[iGrad].fBlue* 255.f))
               | (0xff << 24));
               m_Gradients[iGradient].m_da.Add(pattern.data[iGrad].fPos);

            }
            item.m_pgradient = &m_Gradients[iGradient];
            item.m_bOwn = false;
            item.m_etype = packed_type_gradient;
            m_packeditema.Add(item);

            m_numGradients++;

         }
         else
         {
            TRACE("unknown generic pattern");
         }

      }

   }

end_pat:;

   array < PhBitmapIOStruct > bitmapa;

   // bitmaps
   if(header.read(serialize.m_pointer,mem,file,kMarkerBitmaps))
   {

      UINT uiRealBitmapCount = serialize.m_pointer.ui32();

      for(int ui = 0; ui < uiRealBitmapCount; ui++)
      {

         PhBitmapIOStruct bitmap;

         complex_type_context c(&serialize);
         serialize(bitmap.fBitmapOffset);
         serialize(bitmap.fBitmapLength);
         serialize(bitmap.fShareCount);
         serialize(bitmap.fColor);
         serialize(bitmap.fUnused);

         memory mem;

         ::formatMD70::serialize serialize;

         serialize.m_bWrite = false;

         if(serialize.m_pointer.read(mem,file,bitmap.fBitmapOffset,bitmap.fBitmapLength))
         {
            complex_type_context c(&serialize);
            serialize(bitmap.imageDataSize);
            serialize(bitmap.imageData,bitmap.imageDataSize);
         }

         bitmapa.Add(bitmap);

      }

   }

   // Guides - Infinisys Ltd. 2020
   if (header.read(serialize.m_pointer, mem, file, kMarkerGuidesInfo))
   {

      PhGuidesInfo guidesinfo;

      guidesinfo(serialize);

      m_guidea.SetSize(guidesinfo.numGuides);

      for (int i = 0; i < guidesinfo.guides.GetCount(); i++)
      {

         auto & guideentry = guidesinfo.guides[i];

         CGuide guide;

         guide.m_bVertical = guideentry.fType != 0;

         CString strFormat;

         strFormat.Format("%fmm", guideentry.fLineValue / 10.0f);

         double userValue = guideentry.fLineValue;

         if (GetDimUnit()->numStr2User(strFormat, userValue))
         {

            LongRatio ratio;

            GetActiveScaleRatio(ratio);

            guide.m_coord = GetDimUnit()->numUser2DB(ratio, userValue);
            
         }

         m_guidea[i] = guide;

      }

   }


//   PhBitmapIOStruct* pBitmapIO=NULL;
//   char* pBitl=GetResource('BITL',0,resFork,mdf.numBytes);
//   if(pBitl)
//   {
//      PhFListHeader* pList=(PhFListHeader*)pBitl;
//      uptr p(pBitl);
//      p.swaps(1); p.swapl(1); p.swaps(1); p.swapl(3);
//      p.swaps(3); p.swapl(1);
//
//      pBitmapIO=(PhBitmapIOStruct*)p.pc;
//      for(int j=0; j<pList->fNumElems; j++)
//      {
//         p.swapl(2); p.swaps(1); p.skip(2);
//      }
//   }
//
//// saved views
//   char* pViews=GetResource('VIEW',1,resFork,mdf.numBytes);
//   if(pViews)
//   {
//      PhFListHeader* pList=(PhFListHeader*)pViews;
//      uptr p(pViews);
//      p.swaps(1); p.swapl(1); p.swaps(1); p.swapl(3);
//      p.swaps(3); p.swapl(1);
//
//      PhSavedView* pSavedView=(PhSavedView*)p.pc;
//      for(int j=0; j<pList->fNumElems; j++)
//      {
//         p.p2c(kMaxViewName); p.skip(2); p.swapl(1); p.swaps(1);
//
//         CSavedView* pView=new CSavedView;
//         pView->m_sTitle=pSavedView[j].fViewName;
//         pView->m_ptOrg.x=pSavedView[j].fOldScroll.x;
//         pView->m_ptOrg.y=pSavedView[j].fOldScroll.y;
//         pView->m_nZoom=pSavedView[j].fZoomFactor;
//         pView->m_nShortcut=pSavedView[j].fCmdKey?(pSavedView[j].fCmdKey - '4'):0;
//         m_SavedViews.Add(pView);
//      }
//      delete pViews;
//   }
//
   // reports
   int r=1;
   char* pRep;
   if(header.read(serialize.m_pointer,mem,file,kMarkerReports))
   {
      
      UINT theLong = serialize.m_pointer.ui32();

      short numReports = 0;

      if(theLong != 0)
      {

         numReports = serialize.m_pointer.sh();

      }

      for(short sh = 0; sh < numReports; sh++)
      {

         PhReport report;

         serialize(report);

         CMDReport* pReport=new CMDReport(false);
         
         pReport->m_sTitle=report.fCFRTitle;
         
         for(int j=0; j < 6; j++) pReport->m_nColumnWidths[j]=report.fColumnWidths[j];
         
         TextParams tp;
         
         ctx.ConvertPhTextParams(report.fDefaultTextParams,tp);
         
         pReport->m_LogFont=tp.m_lf;

         for(int iRow = 0; iRow < report.fNumRows; iRow++)
         {
            CMDReportRow* pReportRow=new CMDReportRow;
            switch(report.rowinfoa[iRow].fRowType)
                  {
                  case kPhTextRow:
                  {
                     pReportRow->m_nRowType=kTextRow;
                     PhTextRow* ptrd=(PhTextRow*)report.rowa[iRow];
                     for(int j=0; j < 6; j++)
                     {
                        pReportRow->m_nJust[j]=ptrd->fJust[j];
                        pReportRow->m_sRowText[j]=ptrd->Str[j];
                     }
                  }
                  break;
                  case kPhCriteriaRow:
                  {
                     pReportRow->m_nRowType=kCriteriaRow;
                     PhCriteriaRow * pcrd=(PhCriteriaRow*)report.rowa[iRow];
                     for(int j=0; j < 6; j++) pReportRow->m_nFieldRefNum[j]=pcrd->fFieldRefNum[j];
                  }
                  break;
                  case kPhFunctionRow:
                  {
                     pReportRow->m_nRowType=kFunctionRow;
                     PhFunctionRow* pfrd=(PhFunctionRow*)report.rowa[iRow];
                     pReportRow->m_nFunction=pfrd->fSpare;
                     if(pReportRow->m_nFunction > kFunctTotalName) pReportRow->m_nFunction--;
                     for(int j=0; j < 6; j++)
                     {
                        pReportRow->m_nJust[j]=pfrd->fJust[j];
                        pReportRow->m_sRowText[j]=pfrd->Str[j];
                     }
                  }
                  break;
                  }
               pReport->m_Rows.Add(pReportRow);
            }

            m_Reports.Add(pReport);
            r++;

         }

   }

   // linked vertexes
#ifndef _PERSONAL_EDITION
   long nObjectIndex=0;
   CLinkVertexList linkVertexList;
   //if(!((CPCDraftApp*)AfxGetApp())->m_bRemoveLinks)
   //{
   //   for(short resNo=256;; resNo++)
   //   {
   //      long len;
   //      PhLinkedPtPicData* plp=(PhLinkedPtPicData*)GetResource('Link',resNo,resFork,mdf.numBytes,&len);
   //      if(NULL == plp) break;

   //      uptr p((char*)plp);
   //      p.swapl(1); p.swaps(2); p.swapl(4);
   //      long count=(len - sizeof(PhComplexPtPicData)) / sizeof(PhLinkResInfo);
   //      p.swapl(3 * count);

   //      switch(plp->fComplexType)
   //      {
   //      case kPhComplexPtArcDimPoint:
   //      {
   //         ASSERT(count>1);
   //         CLinkVertex* pLinkVertex=new CLinkVertex;
   //         pLinkVertex->m_nKey=resNo;
   //         pLinkVertex->m_nShape=plp->fLinks[count - 2].fObjectIndex;
   //         pLinkVertex->m_nVertex=kCenterHandle;
   //         linkVertexList.AddTail(pLinkVertex);

   //         pLinkVertex=new CLinkVertex;
   //         pLinkVertex->m_nKey=resNo;
   //         pLinkVertex->m_nShape=plp->fLinks[count - 1].fObjectIndex;
   //         pLinkVertex->m_nVertex=kCenterHandle;
   //         linkVertexList.AddTail(pLinkVertex);
   //      }
   //      break;				// temporarily
   //      case kPhComplexPtOnLinePoint:
   //         count-=3;
   //      case kPhComplexPtLinkPoint:
   //      {
   //         CLinkVertexList list;
   //         for(int j=0; j<count; j++)
   //         {
   //            if(!plp->fLinks[j].fFlags) // skip notify-only
   //            {
   //               CLinkVertex* pLinkVertex=new CLinkVertex;
   //               pLinkVertex->m_nKey=resNo;
   //               pLinkVertex->m_nShape=plp->fLinks[j].fObjectIndex;
   //               pLinkVertex->m_nVertex=plp->fLinks[j].fPointIndex;
   //               list.AddTail(pLinkVertex);
   //            }
   //         }
   //         if(list.GetCount()>1) linkVertexList.AddTail(&list);
   //         else list.DeleteAll();
   //      }
   //      break;
   //      case kPhComplexPtPerpDimPoint:
   //      case kPhComplexPtAngularDimPoint:
   //      {
   //         ASSERT(count>3);
   //         CLinkVertex* pLinkVertex=new CLinkVertex;
   //         pLinkVertex->m_nKey=-resNo;
   //         pLinkVertex->m_nShape=plp->fLinks[count - 1].fObjectIndex;
   //         pLinkVertex->m_nVertex=plp->fLinks[count - 1].fPointIndex;
   //         linkVertexList.AddTail(pLinkVertex);

   //         pLinkVertex=new CLinkVertex;
   //         pLinkVertex->m_nKey=-resNo;
   //         pLinkVertex->m_nShape=plp->fLinks[count - 2].fObjectIndex;
   //         pLinkVertex->m_nVertex=plp->fLinks[count - 2].fPointIndex;
   //         linkVertexList.AddTail(pLinkVertex);

   //         pLinkVertex=new CLinkVertex;
   //         pLinkVertex->m_nKey=-resNo;
   //         pLinkVertex->m_nShape=plp->fLinks[count - 3].fObjectIndex;
   //         pLinkVertex->m_nVertex=plp->fLinks[count - 3].fPointIndex;
   //         linkVertexList.AddTail(pLinkVertex);
   //      }
   //      break;
   //      default:
   //         ASSERT(0);
   //         break;
   //      }
   //   }
   //}
#endif

   CMDShapeArray texta;

//// layers & shapes
   short nActiveLayer=0;

   if(header.read(serialize.m_pointer,mem,file,kMarkerLayerStruct))
   {

      long numLayers = serialize.m_pointer.i32();

      for(int l = 0; l < numLayers; l++)
      {

         LayerStruct layer;

         serialize(layer);

         CLayer * pLayer;
         if(l < m_Layers.GetSize())
         {
            pLayer = GetLayer(l);
            pLayer->DeleteAllShapes();
         }
         else
         {
            pLayer = new CLayer;
            m_Layers.Add(pLayer);
         }

         pLayer->m_strName=layer.fCFRLayerName;
         //if(!m_bEnglishUnits && pstruct->fScaleIndex>21) pstruct->fScaleIndex--;
         //pLayer->m_nScale=pstruct->fScaleIndex;
         pLayer->m_nScale = GetNearestScale(layer.fCustomRatio);
         pLayer->m_nSnap=layer.fSnapIndex + 1;
         pLayer->m_bGrayed=(layer.fLayerFlags&kPhNotGrayed)?false:true;
         pLayer->m_bHidden=(layer.fLayerFlags&kPhVisible)?false:true;
         ctx.player = pLayer;
         if(layer.fLayerActiveFlag)
         {
            nActiveLayer=l;
            if(pLayer->m_bHidden)
            {
               m_bActiveLayerHidden=true;
               pLayer->m_bHidden=false;
            }
         }

         memory mem;

         ::formatMD70::serialize serialize;

         serialize.m_piocontext = &ctx;

         serialize.m_ratio = layer.fCustomRatio;

         if(!serialize.m_pointer.read(mem,file,layer.fLayerOffset,layer.fLayerLength))
         {

            throw "error";

         }

         serialize.m_bWrite = false;


         UInt64 numShapes = serialize.m_pointer.ui64();

//         numShapes= 1;

         if(numShapes > 0)
         {

            process_layer processlayer(numShapes);



            long lObjectSize;
            long lInfoLocation;

#if DEV_DEBUG

            CString strIds;

#endif

            for(int i = 0; i < numShapes; i++)
            {
               if(i == 9)
               {
                  OutputDebugString("j==9");
               }

               CMDShape* pNewShape=NULL;
               eID eid = (eID)serialize.m_pointer.i32();
               lObjectSize = serialize.m_pointer.i32();
               lInfoLocation = serialize.m_pointer.i32();

               if(eid == ::formatMD70::IDText)
               {
                  OutputDebugString("IDText");
               }

               ::std::unique_ptr < PhMDShape > pshapeGuard;

               pshapeGuard.reset(PhMDShape::create(eid));

               PhMDShape * pshape = pshapeGuard.get();

               serialize(pshape);

               PenModel penModel;

               ctx.ConvertPhPenModel(pshape->cPenModel,penModel);

               processlayer.on_read_new_shape();

               PhMDFillShape* pfillshape = dynamic_cast <PhMDFillShape *> (pshape);
               FillModel fm;

               if(pfillshape == NULL)
               {
                  //OutputDebugString("NotFillShape");
               }
               else
               {
                  //OutputDebugString("FillShape");
                  ctx.ConvertPhFillModel(pfillshape->cFillModel,fm);
               }

#if DEV_DEBUG

               CString strFormat;

               if(pshape->cOwnerGroup != NULL)
               {

                  if(ctx.groupa64Debug.Find(pshape->cOwnerGroup) == NULL)
                  {

                     ctx.groupa64Debug.Add(pshape->cOwnerGroup);

                  }

                  strFormat.Format("group=%d ",ctx.groupa64Debug.Find(pshape->cOwnerGroup));

                  strIds += strFormat;

               }

               strFormat.Format("%s ", shape_text(eid));

               strIds += strFormat;

#endif

               switch(eid)
               {
               case ::formatMD70::IDLine:
               case ::formatMD70::IDLine2:
               {

                  PhMDLine * pline = (PhMDLine *)pshape;

                  pNewShape = new CMDLineShape(pline->fP1.fLongPt, pline->fP2.fLongPt,pline->cConAngle);

               }
               break;
               case ::formatMD70::IDFreehand:
               case ::formatMD70::IDPolygon:
               {

                  #if DEV_DEBUG
                  if(eid == ::formatMD70::IDFreehand)
                  {

                     penModel.m_nPenType=kPenColor;
                     penModel.m_crColor=RGB(255,100,255);
                     penModel.m_nStyle=0;
                     penModel.m_iPenOpacity = 0;
                  }
                  else
                  {

                     penModel.m_nPenType=kPenColor;
                     penModel.m_crColor=RGB(128,128,128);
                     penModel.m_nStyle=0;
                     penModel.m_iPenOpacity = 128;
                     fm.m_crColor=RGB(128,128,128);
                     fm.m_iFillOpacity = 0;
                     fm.m_nFillType = 1;

                  }

#endif
                  int i1 = penModel.m_nEndCap1;
                  int i2 = penModel.m_nEndCap2;
                  penModel.m_nEndCap1 = i2;
                  penModel.m_nEndCap2 = i1;
                  PhMDPolygon* ppolygon=(PhMDPolygon*)pshape;

                  pNewShape=new CMDPolygonShape(eid == ::formatMD70::IDFreehand?CMDPolygonShape::kFreehand:CMDPolygonShape::kPolygon,ppolygon->vertexCount,ppolygon->cClosed?true:false);

                  for(int j=0; j<ppolygon->vertexCount; j++)
                  {

                     pNewShape->SetShapePoint(j,ppolygon->data[j].fLongPt);

                  }

               }
               break;

               case ::formatMD70::IDRectCenter:
               case ::formatMD70::IDRectDiagonal:
               {

                  PhMDRectangle* prect=(PhMDRectangle*)pshape;

                  pNewShape=new CMDRectangle(eid == formatMD70::IDRectCenter?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);

                  pNewShape->SetShapePoint(0,prect->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,prect->fPoint[1].fLongPt);
                  pNewShape->SetShapePoint(2,prect->fPoint[2].fLongPt);
                  pNewShape->SetShapePoint(3,prect->fPoint[3].fLongPt);

               };
               break;
               case ::formatMD70::IDRndRectangle:
               {

                  PhMDRndRectangle * prect=(PhMDRndRectangle*)pshape;

                  CMDRndRectangle::CornerType type;

                  switch(prect->cCornerType)
                  {
                  case ::formatMD70::kProportionalCorner:
                     type=CMDRndRectangle::kCornerProportional;
                     break;
                  case ::formatMD70::kConstantCorner:
                     type=CMDRndRectangle::kCornerCircle;
                     break;
                  case ::formatMD70::kEllipticalCorner:
                     type=CMDRndRectangle::kCornerElliptical;
                     break;
                  }

                  pNewShape=new CMDRndRectangle(type);

                  DCoordPoint axisPt=Average(prect->fPoint[1].fLongPt,prect->fPoint[3].fLongPt);

                  //pNewShape->SetShapePoint(0,::formatMD70::mtRotPoint(prect->fPoint[0].fLongPt,prect->theAngle,axisPt));
                  //pNewShape->SetShapePoint(1,::formatMD70::mtRotPoint(prect->fPoint[1].fLongPt,prect->theAngle,axisPt));
                  //pNewShape->SetShapePoint(2,::formatMD70::mtRotPoint(prect->fPoint[2].fLongPt,prect->theAngle,axisPt));
                  //pNewShape->SetShapePoint(3,::formatMD70::mtRotPoint(prect->fPoint[3].fLongPt,prect->theAngle,axisPt));
                  pNewShape->SetShapePoint(0,prect->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,prect->fPoint[1].fLongPt);
                  pNewShape->SetShapePoint(2,prect->fPoint[2].fLongPt);
                  pNewShape->SetShapePoint(3,prect->fPoint[3].fLongPt);

                  ((CMDRndRectangle*)pNewShape)->m_lptCornerDelta=prect->cDiagonals.fLongPt.operator LongPoint() >> 1;

               }
               break;
               case ::formatMD70::IDFreeSpline:
               {

                  PhMDFreeSpline * pspline = (PhMDFreeSpline*)pshape;
                  
                  int iCount =pspline->vertexCount;
                  
                  pNewShape=new CMDPolygonShape(CMDPolygonShape::kFreeSpline,iCount,pspline->cClosed?true:false);
                  
                  if(pspline->cClosed)
                  {
                     
                     iCount--;

                  }
                  
                  int j = pspline->cClosed? 1:0;

                  for(; j<iCount; j++)
                  {
                     
                     auto & p = pspline->data[j].fLongPt;
                     
                     pNewShape->SetShapePoint(j - (pspline->cClosed? 1:- 0),p);

                     if(j == iCount - 1)
                     {
                        
                        pNewShape->SetShapePoint(j,p);

                     }

                  }

                  if(pspline->cClosed)
                  {

                     auto & p = pspline->data[1].fLongPt;
                     
                     pNewShape->SetShapePoint(j,p);

                  }

               }
               break;
               case ::formatMD70::IDPolySpline:
               {

                  PhMDPolySpline* pspline=(PhMDPolySpline*)pshape;

                  int n = 4 * pspline->vertexCount + 1;

                  if(!pspline->cClosed)
                  {

                     n += 2;

                  }

                  pNewShape = new CMDPolygonShape(CMDPolygonShape::kPolySpline, n, pspline->cClosed ? true : false);

                  for(int j=0; j<n; j++)
                  {

                     pNewShape->SetShapePoint(j,pspline->data[j].fLongPt);

                  }

               }
               break;
               case ::formatMD70::IDCircle:
               {

                  PhMDCircle* pcircle=(PhMDCircle*)pshape;

                  CMDCircle::Mode mode;

                  switch(pcircle->method)
                  {
                  case ::formatMD70::eCenterRadius:
                     mode=CMDCircle::kRadius;
                     break;
                  case ::formatMD70::eDiameter:
                     mode=CMDCircle::kDiameter;
                     break;
                  case ::formatMD70::e3Point:
                     mode=CMDCircle::kThreePoint;
                     break;
                  }
                  pNewShape=new CMDCircle(mode);
                  switch(pcircle->method)
                  {
                  case ::formatMD70::eCenterRadius:
                     pNewShape->SetShapePoint(0,pcircle->data[0].fLongPt);
                     pNewShape->SetShapePoint(1,pcircle->data[1].fLongPt);
                     #if DEV_DEBUG
                     penModel.m_nPenType=kPenColor;
                     penModel.m_crColor=RGB(255,0,0);
                     penModel.m_nStyle=0;
                     penModel.m_iPenOpacity = 0;
                     fm.m_crColor=RGB(128,128,128);
                     fm.m_iFillOpacity = 20;
                     fm.m_nFillType = 1;
#endif
                     break;
                  case ::formatMD70::eDiameter:
                     pNewShape->SetShapePoint(0,pcircle->data[1].fLongPt);
                     pNewShape->SetShapePoint(1,pcircle->data[2].fLongPt);
                     break;
                  case ::formatMD70::e3Point:
                     pNewShape->SetShapePoint(0,pcircle->data[1].fLongPt);
                     pNewShape->SetShapePoint(1,pcircle->data[2].fLongPt);
                     pNewShape->SetShapePoint(2,pcircle->data[3].fLongPt);
                     break;
                  }
               }
               break;
               case ::formatMD70::IDArc:
               case ::formatMD70::ID3Arc:
               case ::formatMD70::IDEArc:
               {

                  PhMDArc * parc = (PhMDArc *)pshape;

                  int mode;

                  //ASSERT(parc->method != ::formatMD70::eEllipse);

                  switch(parc->method)
                  {
                  case ::formatMD70::eCenterRadius:
                     mode=CMDArcShape::kRadius;
                     break;
                  case ::formatMD70::e3Point:
                     mode=CMDArcShape::kThreePoint;
                     break;
                  case ::formatMD70::eEllipse:
                     mode=CMDArcShape::kElliptic;
                     break;
                  }
                  pNewShape=new CMDArcShape(mode);
                  switch(parc->method)
                  {
                  case ::formatMD70::eCenterRadius:
                  {

                     pNewShape->SetShapePoint(0,parc->fCenter.fLongPt);
                     
                     pNewShape->SetShapePoint(1,parc->StartPt.fLongPt);
                     
                     pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);

                     ::formatMD70::Angle a1 = parc->cStartAng;

                     ::formatMD70::Angle a2 = ::formatMD70::mtAngle(parc->fCenter.fLongPt,parc->EndPt.fLongPt);

                     ::formatMD70::Angle angA = a1 - a2;
                     
                     angA = ::formatMD70::mtFixAngle(angA);

                     ::Angle ang2 = ::mtFixAngle(parc->cEndAng - ::mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2)));
                     ::Angle ang180 = ::k180;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = ang <= ang180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = angA >= 90.0?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = AD_CLOCKWISE;

                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD70::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>formatMD70::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD70::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>::formatMD70::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
#if DEV_DEBUG
                     penModel.m_nPenType=kPenColor;
                     penModel.m_crColor=RGB(0,255,0);
                     penModel.m_nStyle=0;
                     penModel.m_iPenOpacity = 255;
#endif
                     //if(parc->cBackwards)
                     //{
                       // ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;
                     //}
                  }
                     break;
                  case ::formatMD70::e3Point:
                  {
                     pNewShape->SetShapePoint(0,parc->StartPt.fLongPt);
                     pNewShape->SetShapePoint(1,parc->ThirdPt.fLongPt);
                     pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);
                     {
                        ::Angle ang=mtFixAngle(mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(1)) - mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2)));
                        ((CMDArcShape*)pNewShape)->m_nArcDirection = ang <= ::k180?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;

                  }
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = ang <= ::k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = parc->cBackwards ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD70::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>formatMD70::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD70::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>::formatMD70::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

                     //switch(ps->fMethod)
                     //{
                     //case kPhCenterRadius:
                     //   pNewShape->SetShapePoint(0,ps->fCenter.fLongPt);
                     //   pNewShape->SetShapePoint(1,ps->fPoint[0].fLongPt);
                     //   pNewShape->SetShapePoint(2,ps->fPoint[1].fLongPt);
                     //   break;
                     //case kPh3Point:
                     //   pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
                     //   pNewShape->SetShapePoint(1,ps->fPoint[2].fLongPt);
                     //   pNewShape->SetShapePoint(2,ps->fPoint[1].fLongPt);
                     //   break;
                     //}
                     if(parc->cBackwards)
                     {
                        if(((CMDArcShape*)pNewShape)->m_nArcDirection == AD_CLOCKWISE)
                        {
                           ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;
                        }
                        else
                        {
                           ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_CLOCKWISE;
                        }
                     }

                  }
                     break;
                  case ::formatMD70::eEllipse:
                  {
                     pNewShape->SetShapePoint(0,parc->fCenter.fLongPt);
                     pNewShape->SetShapePoint(1,parc->StartPt.fLongPt);
                     pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);
                     //pNewShape->SetShapePoint(3,parc->ThirdPt.fLongPt);
                     //pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(parc->Radii.x,parc->Radii.y),-pshape->fRot));
                     pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(parc->Radii.x,parc->Radii.y),-pshape->theAngle));
                     //pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + DCoordPoint(parc->Radii.x,parc->Radii.y));
                     //pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + Abs(parc->ThirdPt.fLongPt - parc->fCenter.fLongPt));

                     //parc->cStartAng = mtFixAngle(90 - parc->cStartAng);
                     //parc->cEndAng=mtFixAngle(90 - parc->cEndAng);
                     ////((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE; //(ps->fStartAng>ps->fEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(parc->cStartAng>parc->cEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

                     DCoordPoint ptAxis1 = parc->fCenter.fLongPt;
                     DCoordPoint ptAxis2 = parc->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(parc->Radii.x,parc->Radii.y),-pshape->theAngle);

                     DCoordPoint ptCenter = (ptAxis1 + ptAxis2) / 2.0;

                     //LongPoint ptCenter = parc->ThirdPt.fLongPt;

                     {
                        ::Angle angx=mtFixAngle(mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(1)) - mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2))) ;
                        double d = angx.GetRawDouble() / 65536.0;
                        ::formatMD70::Angle ang=::formatMD70::mtFixAngle(mtAngle(ptCenter,parc->StartPt.fLongPt) - mtAngle(ptCenter,parc->EndPt.fLongPt));
                        //((CMDArcShape*)pNewShape)->m_nArcDirection = ang < ::k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                        ::formatMD70::Angle  ang1= parc->cStartAng;
                        ::formatMD70::Angle  ang2 = parc->cEndAng;
                        ::formatMD70::Angle  dang = ang2 - ang1;

                        dang = ::formatMD70::mtFixAngle(dang);

                        ((CMDArcShape*)pNewShape)->m_nArcDirection = d >= 0 && d <= 90.0 ? AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

                     }


                     //{

                     //   ::formatMD70::Angle  ang1= parc->cStartAng;
                     //   ::formatMD70::Angle  ang2 = parc->cEndAng;
                     //   ang1 = ::formatMD70::mtFixAngle(ang1-90);
                     //   ang2 = ::formatMD70::mtFixAngle(ang2-90);
                     //   ::formatMD70::Angle  dang = ang2 - ang1;
                     //   //((CMDArcShape*)pNewShape)->m_nArcDirection = (ang1 > ang2) ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE;
                     //   ((CMDArcShape*)pNewShape)->m_nArcDirection = dang < 180.0 ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE;

                     //}

                     /*                    LongPoint ptAxis1 = pNewShape->GetShapePoint(0);
                     LongPoint ptAxis2 = pNewShape->GetShapePoint(3);
                     LongPoint ptCenter = (ptAxis1 + ptAxis2) / 2.0;

                     {
                     ::Angle ang=mtFixAngle(mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(1)) - mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2)));
                     ((CMDArcShape*)pNewShape)->m_nArcDirection = ang < ::k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

                     }

                     */                    //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(mtFixAngle(parc->cStartAng + pshape->fRot *2.0) - mtFixAngle(mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))<30.0)? AD_COUNTERCLOCKWISE : AD_CLOCKWISE;


                     //double d = 90.0;
                     //
                     //parc->cStartAng=mtFixAngle(parc->cStartAng);
                     //
                     //parc->cEndAng=mtFixAngle(parc->cEndAng);

                     //                     double dStart = parc->cStartAng;
                     //                     double dEnd = parc->cEndAng;
                     //                     double dRot = parc->fRot;
                     //                     double dBeg1 = mtFixAngle(mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt));
                     //                     double dBeg = mtFixAngle(90.0 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt));
                     //                     double dFin = mtFixAngle(mtAngle(parc->fCenter.fLongPt,parc->EndPt.fLongPt));
                     //
                     //                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(mtFixAngle(parc->cStartAng + pshape->fRot *2.0) - mtFixAngle(mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))<30.0)? AD_COUNTERCLOCKWISE : AD_CLOCKWISE;
                     //
                     //                     /*if(parc->cStartAng > parc->cEndAng)*/
                     //                     double d1 = abs(dStart - dEnd);
                     //                     double d2 = abs(dBeg - dFin);
                     //                     double d3 = abs(dStart - dBeg);
                     //
                     //                     double dB = mtFixAngle(90.0-dEnd);
                     //                     double dA = mtFixAngle(90.0-dStart);
                     //                     double dD = mtFixAngle(dFin);
                     //                     double dC = mtFixAngle(dBeg);
                     //                     double dba = abs(dB - dA);
                     //                     double ddc = abs(dD - dC);
                     //                     double dda = abs(dD - dA);
                     //
                     //                     if(ddc > 180.0)
                     //                     {
                     //                        ddc = ddc - 180.0;
                     //                     }
                     //
                     //                     //if(dB > dA)
                     //                     //if(Abs(parc->cStartAng - mtFixAngle(formatMD70::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))<1.0)
                     //                     //if(dba < 33.0)
                     if(((CMDArcShape*)pNewShape)->m_nArcDirection == AD_COUNTERCLOCKWISE)
                     {
#if DEV_DEBUG >= 2
                        penModel.m_nPenType=kPenColor;
                        penModel.m_crColor=RGB(0,200,200);
                        penModel.m_nStyle=0;
                        penModel.m_iPenOpacity = 255;
#endif
                     }
                     else
                     {

#if DEV_DEBUG >= 3
                        penModel.m_nPenType=kPenColor;
                        penModel.m_crColor=RGB(0,0,255);
                        penModel.m_nStyle=0;
                        penModel.m_iPenOpacity = 255;
#endif
                     }

                     if(parc->cBackwards)
                     {
                        if(((CMDArcShape*)pNewShape)->m_nArcDirection == AD_CLOCKWISE)
                        {
                           ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;
                        }
                        else
                        {
                           ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_CLOCKWISE;
                        }
                     }

                     //
                     //                     ((CMDArcShape*)pNewShape)->m_nArcDirection= AD_COUNTERCLOCKWISE;
                     //
                     //                     {
                     //                        DAngle d = mtFixAngle(parc->fRot);
                     //                        if(d > 90.0 && d <= 180.0)
                     //                        {
                     //                           ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_CLOCKWISE;
                     //                        }
                     //                        else
                     //                        {
                     //                           ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE;
                     //                        }
                     //                     }
                     //                     //((CMDArcShape*)pNewShape)->m_nArcDirection= AD_CLOCKWISE;
                     //
                     //                     //((CMDArcShape*)pNewShape)->m_nArcDirection= AD_COUNTERCLOCKWISE;
                     //#if DEV_DEBUG
                     //                     if(pshape->fRot != 180.0)
                     //                     {
                     //                        OutputDebugString("NonPi");
                     //                     }
                     //                     if(parc->cBackwards)
                     //                     {
                     //                        OutputDebugString("Bk");
                     //                     }
                     //                     else
                     //                     {
                     //                        OutputDebugString("Fw");
                     //                     }
                     //#endif
                     //                     //((CMDArcShape*)pNewShape)->m_nArcDirection = parc->cBackwards ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE;
                     //                     //((CMDArcShape*)pNewShape)->SetFillOpacity(128);
                     ////                     PenModel p = ((CMDArcShape*)pNewShape)->m_PenModel;
                     //                     //penModel.m_nWidth = 1;
                     //  //                   ((CMDArcShape*)pNewShape)->SetPenModel(p);
                     //                     //{
                     //                     //   DAngle d = mtFixAngle(parc->fRot);
                     //                     //   if(d > 90.0 && d <= 180.0)
                     //                     //   {
                     //                     //      ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_CLOCKWISE;
                     //                     //   }
                     //                     //   else
                     //                     //   {
                     //                     //      ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE;
                     //                     //   }
                     //                     //}

                     //{
                     //   ::Angle ang1=mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(1));
                     //   ::Angle angDeltaBefore=mtFixAngle(mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2)) - ang1);
                     //   ::Angle angDeltaAfter=mtFixAngle(mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2)) - ang1);

                     //   if(angDeltaBefore>::k270 && angDeltaAfter<::k90)
                     //      ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE;
                     //   else if(angDeltaBefore<::k90 && angDeltaAfter>::k270)
                     //      ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_CLOCKWISE;
                     //       }

                     //       break;
                     //    }
                     //    
                     // }
                     //if(parc->cBackwards)
                     //{
                     //   ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_CLOCKWISE;
                     //}
                     //else
                     //{
                     //   ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;
                     //}
                  }
                  break;
                  //               case ::formatMD70::IDEArc:
                  //{

                  //   PhMDArc * parc =(PhMDArc*)pshape;

                  //   ASSERT(parc->method == ::formatMD70::eEllipse);
                  //   pNewShape=new CMDArcShape(CMDArcShape::kElliptic);
                  //   pNewShape->SetShapePoint(0,parc->fCenter.fLongPt);
                  //   pNewShape->SetShapePoint(1,parc->StartPt.fLongPt);
                  //   pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);
                  //   pNewShape->SetShapePoint(3,parc->fCenter.fLongPt.operator LongPoint() + ::formatMD70::mtRotPoint(LongPoint(parc->Radii.x,parc->Radii.y),pshape->theAngle));
                  //   //parc->cStartAng=mtFixAngle(90 - ps->cStartAng);
                  //   //parc->cEndAng=mtFixAngle(90 - ps->cEndAng);
                  //   ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE; //(ps->fStartAng>ps->fEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                  }
                  break;
               }
               case ::formatMD70::IDEllipse:
               {

                  PhMDEllipse* ps=(PhMDEllipse*)pshape;

                  pNewShape=new CMDEllipse(ps->method == ::formatMD70::eCenterRadius?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);

                  //pNewShape->SetShapePoint(0,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(-ps->radius.x.m_f,-ps->radius.y.m_f),pshape->theAngle));
                  //pNewShape->SetShapePoint(1,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(ps->radius.x.m_f,-ps->radius.y.m_f),pshape->theAngle));
                  //pNewShape->SetShapePoint(2,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(ps->radius.x.m_f,ps->radius.y.m_f),pshape->theAngle));
                  //pNewShape->SetShapePoint(3,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(-ps->radius.x.m_f,ps->radius.y.m_f),pshape->theAngle));
                  pNewShape->SetShapePoint(0,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(-ps->radius.x.m_f,-ps->radius.y.m_f),-pshape->theAngle));
                  pNewShape->SetShapePoint(1,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(ps->radius.x.m_f,-ps->radius.y.m_f),-pshape->theAngle));
                  pNewShape->SetShapePoint(2,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(ps->radius.x.m_f,ps->radius.y.m_f),-pshape->theAngle));
                  pNewShape->SetShapePoint(3,ps->fCenter.fLongPt + ::formatMD70::mtRotPoint(DCoordPoint(-ps->radius.x.m_f,ps->radius.y.m_f),-pshape->theAngle));
#if DEV_DEBUG
                  penModel.m_nPenType=kPenColor;
                  penModel.m_crColor=RGB(128,128,128);
                  penModel.m_nStyle=0;
                  penModel.m_iPenOpacity = 0;
                  fm.m_crColor=RGB(128,128,128);
                  fm.m_iFillOpacity = 128;
                  fm.m_nFillType = 1;
#endif
               }
               break;
               case ::formatMD70::IDBezier:
               {

                  PhMDSpecificBezier * pbezier =(PhMDSpecificBezier*)pshape;

                  int n=pbezier->numPoints;

                  auto pshape = new CMDBezierShape(n);

                  pNewShape = pshape;

                  pshape->SetClosed(pbezier->cClosed);

                  bool bLastRealVertex = false;

                  bool bRealVertex;

                  for(int j=0; j < n; j++)
                  {

                     auto & point = pbezier->data[j];

                     int flags = point.fFlags;

                     bRealVertex = (flags & kPhRealVertex) == kPhRealVertex;

                     if(bRealVertex && bLastRealVertex)
                     {

                        OutputDebugString("Bezier MoveTo");

                     }

                     pshape->SetShapePoint(j,pbezier->data[j].fLongPt);

                     bLastRealVertex = bRealVertex;

                  }

               }
               break;
               case ::formatMD70::IDCubic:
               {

                  PhMDCubic * pcubic =(PhMDCubic*)pshape;

                  int n = pcubic->vertexCount;

                  if(!pcubic->cClosed)
                  {

                     n++;

                  }

                  pNewShape=new CMDCubicShape(n, pcubic->cClosed ? true : false);

                  for(int j=0; j < n; j++)
                  {

                     pNewShape->SetShapePoint(j,pcubic->data[j * 3].fLongPt);

                  }

               }
               break;
               case ::formatMD70::IDText:
               {

                  PhMDText* ps = (PhMDText *)pshape;

                  OutputDebugString(ps->m_strRtf);


                  //std::string data(ps->m_strRtf,ps->m_strRtf.GetLength());
                  std::istrstream mb(ps->m_strRtf);

                  //{

                  //   int iLen=ps->chars.GetLength();

                  //   WCHAR * pw = new WCHAR[iLen * 2];

                  //   int i1 = MultiByteToWideChar(CP_MACCP,0,ps->chars,ps->chars.GetLength(),pw,iLen *2);

                  //   int i2 = WideCharToMultiByte(CP_ACP,0,pw, i1,ps->chars.GetBufferSetLength(iLen*3),iLen*3, NULL, NULL);

                  //   ps->chars.ReleaseBuffer();

                  //   delete[]pw;

                  //}

//                  TextParams tp;
//                  switch(ps->cTextJust)
//                  {
//                  default:
//                  case 0:
//                     tp.m_nJust=kAlignLeft;
//                     break;
//                  case 1:
//                     tp.m_nJust=kAlignCenter;
//                     break;
//                  case -1:
//                     tp.m_nJust=kAlignRight;
//                     break;
//                  }
//                  switch((ps->cShapeFlags&kPhVertTextAlignMask) >> kPhVertTextAlignCodeShift)
//                  {
//                  default:
//                  case 0:
//                     tp.m_nVAlign=kAlignTop;
//                     break;
//                  case 1:
//                     tp.m_nVAlign=kAlignMiddle;
//                     break;
//                  case 2:
//                     tp.m_nVAlign=kAlignBottom;
//                     break;
//                  }
//                  switch(ps->cTextSpace)
//                  {
//                  default:
//                  case ::formatMD70::kSingleSpace:
//                     tp.m_nSpacing=::kSingleSpace;
//                     break;
//                  case ::formatMD70::kOne_halfSpace:
//                     tp.m_nSpacing=::k1_5Space;
//                     break;
//                  case ::formatMD70::kDoubleSpace:
//                     tp.m_nSpacing=::kDoubleSpace;
//                     break;
//                  }
//
//                  tp.m_nCase=0;
//

                  auto paragraph = ps->cParagraph;

                  auto ptext = new CMDText(paragraph?CMDText::kTextUserSize:CMDText::kTextAutoSize);
                  //auto ptext = new CMDText(CMDText::kTextUserSize);
                  pNewShape= ptext;
                  pNewShape->SetShapePoint(0,ps->data[0].fLongPt);
                  pNewShape->SetShapePoint(1,ps->data[1].fLongPt);
                  pNewShape->SetShapePoint(2,ps->data[2].fLongPt);
                  pNewShape->SetShapePoint(3,ps->data[3].fLongPt);

                  

                  ::text_format_rtf::std_source source(mb);

                  std::ostrstream strLog;

                  ::text_format_rtf::std_logger logger(strLog);

                  ::md_text_rtf_processor proc;

                  TextParams textparams;

                  textparams = ptext->GetTextParams();

                  //text_format_rtf::output o;
                  switch(ps->cTextJust)
                  {
                  default:
                  case 0:
                     textparams.m_nJust=kAlignLeft;
                     break;
                  case 1:
                     textparams.m_nJust=kAlignCenter;
                     break;
                  case -1:
                     textparams.m_nJust=kAlignRight;
                     break;
                  }

                  ptext->SetTextParams(textparams);

                  proc.m_ptext = ptext;

                  ::text_format_rtf::impl::processor processor(nullptr);

                  processor.rtf_process(&source,&proc,&logger);

//                  //pNewShape->SetTextParams(tp);
////
//                  penModel.m_nPenType = 1;
//
//
//                  texta.Add(pNewShape);
//
                  //if(pshape->the64ObjectInfo)
                  //{
                  //   short count=p.swaps();
                  //   if(pNewShape)
                  //   {
                  //      PhObjectInfo* pInfo=(PhObjectInfo*)p.pc;
                  //      p.swaps(2);
                  //      count-=4;

                  //      CString sValues[5];
                  //      for(int j=0; j<5; j++)
                  //      {
                  //         int len=*p.pc++;
                  //         count--;
                  //         sValues[j]=p.pc;
                  //         p.skip(++len);
                  //         count-=len;
                  //      }
                  //      pNewShape->SetValues(sValues);
                  //   }
                  //   p.skip(count);
                  //   pshape->the64ObjectInfo=0;
                  //}
                  //union
                  //{
                  //   BYTE cCharset[400];
                  //   DWORD dwProperty[100];
                  //};
                  //int nCount = 0;
                  //memset(cCharset,0,sizeof(cCharset));
                  //if(pshape->fRef)
                  //{
                  //   short count=p.swaps();
                  //   PhPropList* pl=(PhPropList*)p.pc;
                  //   p.swaps(1); p.swapl(1); p.swaps(1);
                  //   p.swapl(3); p.swaps(3); p.swapl(1);
                  //   count -= 30;

                  //   for(int j=0; j<pl->fNumElems; j++)
                  //   {
                  //      p.swaps(1); p.swapl(1);
                  //      if(pl->fData[j].fType >= 600 && pl->fData[j].fType<700)
                  //      {
                  //         nCount++;
                  //         dwProperty[pl->fData[j].fType - 600]=pl->fData[j].fData;
                  //      }
                  //      count -= 6;
                  //   }
                  //   ASSERT(count == 0);
                  //   nCount*=4;
                  //   pshape->fRef=0;
                  //}

                  //PhTextData* pt=(PhTextData*)p.pc;
                  //p.swapl(4); p.swaps(1); p.swapl(1);

                  //char* pText=p.pc;
                  //p.skip(pt->fTELength);

                  //StScrpRec* pStyle=(StScrpRec*)p.pc;
                  //p.swaps(1);
                  //p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);

                  //CStyledText st;
                  //CStyledTextStyle s;
                  //int n=0;

                  //HDC hDC=GetDC(NULL);
                  //int nLogPixelsY=GetDeviceCaps(hDC,LOGPIXELSY);
                  //ReleaseDC(NULL,hDC);
//                  for(int j=0; j<ps->teLength; j++)
//                  {
//                     if(n<ps->scrCount && j == ps->scripta[n].scrpStartChar)
//                     {
//                        s.m_LogFont=tp.m_lf;
//#ifdef _JAPANESE
//                        s.m_LogFont.lfCharSet = SHIFTJIS_CHARSET;
//#else
//                        s.m_LogFont.lfCharSet = ANSI_CHARSET;
//#endif
//                        ctx.ConvertPhFont(ps->scripta[n].scrpFont,ps->scripta[n].scrpSize,ps->scripta[n].scrpFace,s.m_LogFont);
//                        //s.m_LogFont.lfCharSet= ps->scripta[n].scrpFace;
//                        int iR = ps->scripta[n].scrpColor.red >> 8;
//                        int iG = ps->scripta[n].scrpColor.green >> 8;
//                        int iB = ps->scripta[n].scrpColor.blue >> 8;
//                        s.m_TextColor=RGB(iR,iG,iB);
//                        //if(++n<ps2->scrpNStyles)
//                        //{
//                        //   p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
//                        //}
//                        n++;
//                     }
//                     if(ps->chars[j] == 0xD) ps->chars.SetAt(j, 0xA);
//                     st.Add(ps->chars[j],s);
//                  }

                  // temporary text

//                  CString strText = "(Temp Text)";
//                  s.m_LogFont ={};
//                  strcpy(s.m_LogFont.lfFaceName, "Arial");
//#ifdef _JAPANESE
//                  s.m_LogFont.lfCharSet = SHIFTJIS_CHARSET;
//#else
//                  s.m_LogFont.lfCharSet = ANSI_CHARSET;
//#endif
//                  s.m_LogFont.lfHeight =18;
//                  //s.m_LogFont.lfCharSet= ps->scripta[n].scrpFace;
//                  int iR = 0;
//                  int iG = 0;
//                  int iB = 0;
//                  s.m_TextColor=RGB(iR,iG,iB);
//                  for(int j=0; j < strText.GetLength(); j++)
//                                       {
//                                          st.Add(strText[j],s);
//                                       }
//                     ((CMDText*)pNewShape)->SetText(st);
               }
               break;
               case ::formatMD70::IDBitmap:
               {
                  PhMDBitmap* ps=(PhMDBitmap*)pshape;

                  //p.swaps(3); // fm
                  //p.swapDBPoint(4);
                  //p.swapl(2); p.swaps(1);
                  //count-=56;
                  memory mem;

                  ::formatMD70::serialize serialize;

                  serialize.m_bWrite = false;

                  //if(serialize.m_pointer.read(mem, file, pBitmap


                  //char* pBuf=new char[pBitmapIO[ps->fMapHdlIndex].fBitmapLength - 4];
                  //LONG lSavedPos=fi.Seek(pBitmapIO[ps->fMapHdlIndex].fBitmapOffset + 4,CFile::begin);
                  //fi.Read(pBuf,pBitmapIO[ps->fMapHdlIndex].fBitmapLength - 4);
                  //fi.Seek(lSavedPos,CFile::begin);
                  HBITMAP hDib=NULL;
                  CRect rc(ps->cSrcRect.l,ps->cSrcRect.t,ps->cSrcRect.r,ps->cSrcRect.b);

                  int iHandleIndex = ps->cMapHdlIndex;

                  try
                  {
                     Gdiplus::Bitmap * pb = LoadPNG(bitmapa[ps->cMapHdlIndex].imageData);
                     hDib = CreateDIB(pb, true);
                     delete pb;
                  }
                  catch(...)
                  {
                  }

                  LongRect lrc;
                  lrc.SetToBound(ps->data[0].fLongPt,ps->data[2].fLongPt);
                  pNewShape=new CMDBitmap(rc,lrc,hDib);

               }
               break;
               case ::formatMD70::IDGroup:
               {

                  PhMDGroup* pgroup=(PhMDGroup*)pshape;

#if DEV_DEBUG
                  if(pgroup->theAngle != 180.0)
                  {
                     OutputDebugString("test");
                  }
#endif
                  CMDGroupShape* pGroup=new CMDGroupShape();
                  for(int j=0; j < pgroup->itemsCount; j++)
                  {
                     //data.s64();
                  }
                  //data.s64();
                  processlayer.groups.SetAt(pshape->the64,pGroup);
                  pNewShape=pGroup;

#if DEV_DEBUG

                  if(ctx.groupa64Debug.Find(pshape->the64) == NULL)
                  {

                     ctx.groupa64Debug.Add(pshape->the64);

                  }

                  strFormat.Format("count=%I64d the64=%d ",pgroup->itemsCount,ctx.groupa64Debug.Find(pshape->the64));

                  strIds += strFormat;

#endif

               }
               break;
               case ::formatMD70::IDMetaPoly:
               {
                  PhMDMetaPoly* ps=(PhMDMetaPoly*)pshape;

                  CMDMetaPolyShape* pMeta=new CMDMetaPolyShape();
                  processlayer.metas.SetAt(ps->the64,pMeta);
                  pNewShape=pMeta;

                  auto piaNew = new CArray < SInt64 >();

                  piaNew->Append(ps->shapes);

                  processlayer.metasOrder.SetAt(ps->the64,piaNew);

#if DEV_DEBUG

                  if(ctx.groupa64Debug.Find(ps->the64) == NULL)
                  {

                     ctx.groupa64Debug.Add(ps->the64);

                  }

                  strFormat.Format("count=%d the64=%d ",ps->shapes.GetCount(),ctx.groupa64Debug.Find(ps->the64));

                  strIds += strFormat;

#endif

               }
               break;

               case ::formatMD70::IDHLineDim:
               case ::formatMD70::IDVLineDim:
               case ::formatMD70::IDSLineDim:
               case ::formatMD70::IDPLineDim:
               {
                  PhMDLineDim* ps=(PhMDLineDim*)pshape;

//                  data.swap(ps);

                  DimStandard std;
                  ctx.ConvertPhDimStd(ps->standard,std);
                  DimFormat fmt;
                  ctx.ConvertPhDimFmt(ps->format,fmt);
                  fmt.nRoundoff=(ps->fStatus >> 8) & 0xF;
                  if(fmt.bUseLabel)
                  {
                     fmt.sLabel=ps->dimStr;
                  }

                  //fmt.sLabel = ps->dimStr;

                  switch(eid)
                  {
                  case ::formatMD70::IDHLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kHoriz);
                     break;
                  case ::formatMD70::IDVLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kVert);
                     break;
                  case ::formatMD70::IDSLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kSlope,pshape->theAngle);
                     break;
                  case ::formatMD70::IDPLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kPerp,pshape->theAngle);
                     break;
                  }
                  pNewShape->SetShapePoint(0,ps->refPt1.fLongPt);
                  pNewShape->SetShapePoint(1,ps->refPt2.fLongPt);
                  pNewShape->SetShapePoint(2,ps->anchorPt.fLongPt);
                  pNewShape->SetShapePoint(3,ps->dimLineStart.fLongPt);
                  pNewShape->SetShapePoint(4,ps->dimLineEnd.fLongPt);
                  pNewShape->SetShapePoint(5,
                     ps->extLineStart.fLongPt.x == 0
                     && ps->extLineStart.fLongPt.y == 0
                     && ps->extLineEnd.fLongPt.x == 0
                     && ps->extLineEnd.fLongPt.y == 0 ?
                     pNewShape->GetShapePoint(4) : ps->extLineEnd.fLongPt);
                  pNewShape->SetShapePoint(6,ps->leaderLineStart.fLongPt != ps->leaderLineEnd.fLongPt?ps->leaderLineEnd.fLongPt:pNewShape->GetShapePoint(5));
                  pNewShape->SetShapePoint(7,pNewShape->GetShapePoint(6));
                  pNewShape->SetShapePoint(8,ps->witnessStart1.fLongPt);
                  pNewShape->SetShapePoint(9,ps->witnessEnd1.fLongPt);
                  pNewShape->SetShapePoint(10,ps->witnessStart2.fLongPt);
                  pNewShape->SetShapePoint(11,ps->witnessEnd2.fLongPt);

                  CDocMapper mapper(this);

                  CMDDimStr* pDS=new CMDDimStr;
                  pDS->SetPenModel(penModel);
                  FillModel fm;
                  ctx.ConvertPhFillModel(ps->cFillModel,fm);
                  pDS->SetFillModel(fm);
                  TextParams tp;
                  ctx.ConvertFont(ps->fShapeFont,ps->fShapeFontSize,tp.m_lf);
                  pDS->ModifyTextParams(tp,tpUseFont,NULL);
                  pDS->SetShapePoint(0,ps->anchorPt.fLongPt);
                  {
                     CDC cdc;
                     cdc.CreateCompatibleDC(NULL);
                     CDraftContext dc(&cdc,&mapper);
                     dc.SetDimStandard(&std);
                     if(fmt.bUseLabel && !fmt.sLabel.IsEmpty())
                     {
                        pDS->SetValues(&dc,fmt.sLabel);
                     }
                     else if(!ps->dimStr.IsEmpty())
                     {
                        pDS->SetValues(&dc,ps->dimStr,ps->tol1Str,ps->tol2Str);
                     }
                  }
                  ((CMDAssocDim*)pNewShape)->SetDimStr(pDS);
                  pNewShape->ReCalc(&mapper);
               }
               break;
               case ::formatMD70::IDRRadialDim:
               case ::formatMD70::IDDRadialDim:
               {
                  PhMDLineDim* ps=(PhMDLineDim*)pshape;

                  DimStandard std;
                  ctx.ConvertPhDimStd(ps->standard,std);
                  DimFormat fmt;
                  ctx.ConvertPhDimFmt(ps->format,fmt);
                  fmt.nRoundoff=(ps->fStatus >> 8) & 0xF;
                  //if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
                  //{
                  //   short cnt=p.swaps();
                  //   char* label=p.pc;
                  //   p.p2c(cnt);
                  //   fmt.sLabel=label;
                  //}

                  ::Coord value=Pythag(ps->dimLineEnd.fLongPt - ps->dimLineStart.fLongPt);
                  if(eid == ::formatMD70::IDDRadialDim) value>>=1;
                  pNewShape=new CMDRadialDim(std,fmt,6,6,eid == ::formatMD70::IDDRadialDim,ps->refPt1.fLongPt,value,ps->anchorPt.fLongPt);

                  CDocMapper mapper(this);

                  CMDDimStr* pDS=new CMDDimStr;
                  pDS->SetPenModel(penModel);
                  FillModel fm;
                  ctx.ConvertPhFillModel(ps->cFillModel,fm);
                  pDS->SetFillModel(fm);
                  TextParams tp;
                  ctx.ConvertFont(ps->fShapeFont,ps->fShapeFontSize,tp.m_lf);
                  pDS->ModifyTextParams(tp,tpUseFont,NULL);
                  pDS->SetShapePoint(0,ps->anchorPt.fLongPt);

                  {
                     CDC cdc;
                     cdc.CreateCompatibleDC(NULL);
                     CDraftContext dc(&cdc,&mapper);
                     dc.SetDimStandard(&std);
                     if(fmt.bUseLabel) pDS->SetValues(&dc,fmt.sLabel);
                     else pDS->SetValues(&dc,ps->dimStr,ps->tol1Str,ps->tol2Str);
                  }
                  ((CMDAssocDim*)pNewShape)->SetDimStr(pDS);
                  pNewShape->ReCalc(&mapper);
               }
               break;
               case ::formatMD70::IDCenterDim:
               {
                  PhMDCenterDim* ps=(PhMDCenterDim*)pshape;

                  DimStandard std;
                  ctx.ConvertPhDimStd(ps->standard,std);
                  DimFormat fmt;
                  ctx.ConvertPhDimFmt(ps->format,fmt);
                  //if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
                  //{
                  //   short cnt=p.swaps();
                  //   char* label=p.pc;
                  //   p.p2c(cnt);
                  //   fmt.sLabel=label;
                  //}

                  pNewShape=new CMDCenterDim(std,fmt,ps->cCenterPt.fLongPt,LongPoint(ps->cRadii.x,ps->cRadii.y));
               }
               break;
               case ::formatMD70::IDAngularDim:
               {
                  PhMDAngularDim* ps=(PhMDAngularDim*)pshape;


                  DimStandard std;
                  ctx.ConvertPhDimStd(ps->standard,std);
                  DimFormat fmt;
                  ctx.ConvertPhDimFmt(ps->format,fmt);
                  //if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
                  //{
                  //   short cnt=p.swaps();
                  //   char* label=p.pc;
                  //   p.p2c(cnt);
                  //   fmt.sLabel=label;
                  //}

                  pNewShape=new CMDAngularDim(std,fmt,6,6,true);
                  pNewShape->SetShapePoint(0,ps->ref1DBPoint.fLongPt);
                  pNewShape->SetShapePoint(1,ps->ref2DBPoint.fLongPt);
                  pNewShape->SetShapePoint(2,ps->anchorDBPoint.fLongPt);
                  pNewShape->SetShapePoint(3,ps->lineStartDBPoint.fLongPt);
                  pNewShape->SetShapePoint(4,ps->lineEndDBPoint.fLongPt);
                  pNewShape->SetShapePoint(5,ps->extDBPoint.fLongPt);
                  pNewShape->SetShapePoint(8,ps->witnessStart1DBPoint.fLongPt);
                  pNewShape->SetShapePoint(9,ps->witnessEnd1DBPoint.fLongPt);
                  pNewShape->SetShapePoint(10,ps->witnessStart2DBPoint.fLongPt);
                  pNewShape->SetShapePoint(11,ps->witnessEnd2DBPoint.fLongPt);
                  pNewShape->SetShapePoint(12,ps->centerDBPoint.fLongPt);

                  CDocMapper mapper(this);

                  CMDDimStr* pDS=new CMDDimStr;
                  pDS->SetPenModel(penModel);
                  FillModel fm;
                  ctx.ConvertPhFillModel(ps->cFillModel,fm);
                  pDS->SetFillModel(fm);
                  TextParams tp;
                  //ctx.ConvertPhFont(ps->fShapeFont,ps->fShapeFontSize,0,tp.m_lf);
                  ctx.ConvertFont(ps->fShapeFont,ps->fShapeFontSize,tp.m_lf);
                  pDS->ModifyTextParams(tp,tpUseFont,NULL);
                  pDS->SetShapePoint(0,ps->anchorDBPoint.fLongPt);

                  {
                     CDC cdc;
                     cdc.CreateCompatibleDC(NULL);
                     CDraftContext dc(&cdc,&mapper);
                     dc.SetDimStandard(&std);
                     if(fmt.bUseLabel) pDS->SetValues(&dc,fmt.sLabel);
                     else pDS->SetValues(&dc,ps->dimStr,ps->tol1Str,ps->tol2Str);
                  }
                  ((CMDAssocDim*)pNewShape)->SetDimStr(pDS);
                  pNewShape->ReCalc(&mapper);
               }
               break;
               case ::formatMD70::IDDimPt2Pt:
               {
                  PhMDDim* ps=(PhMDDim*)pshape;

                  CMDDimLine* pDimLine=new CMDDimLine(ps->fP1.fLongPt,ps->fP2.fLongPt);
                  processlayer.dims.SetAt(ps->the64,pDimLine);
                  pNewShape=pDimLine;
               }
               break;
               case ::formatMD70::IDDimString:
               {
                  PhMDDimStr* ps=(PhMDDimStr*)pshape;


                  processlayer.areas.Lookup(ps->cObjHdl,pNewShape);
                  if(pNewShape) processlayer.areas.RemoveKey(ps->cObjHdl);
                  else
                  {
                     CMDDimLine* pDimLine=NULL;
                     processlayer.dims.Lookup(ps->cObjHdl,pDimLine);
                     if(pDimLine)
                     {
                        processlayer.dims.RemoveKey(ps->cObjHdl);
                        pNewShape=new CMDDimStr(ps->fStatus&kPhDimStrAttached?true:false,pDimLine);
                        pDimLine->m_nTextDisplay=(ps->fStatus&kPhDimStrOnSlope)?((ps->fStatus&kPhDimStrAbove)?::formatMD70::kAbove : ::formatMD70::kAlongLine): ::formatMD70::kHorizontal;
                     }
                  }
                  if(pNewShape)
                  {
                     TextParams tp;
                     //ctx.ConvertPhFont(ps->fShapeFont,ps->fShapeFontSize,0,tp.m_lf);
                     ctx.ConvertFont(ps->fShapeFont,ps->fShapeFontSize,tp.m_lf);
                     pNewShape->ModifyTextParams(tp,tpUseFont,NULL);
                     pNewShape->SetShapePoint(0,ps->cAnchorPt.fLongPt + ps->cStrOffset.fLongPt);
                     pNewShape->SetRotation(ps->theAngle);
                     {
                        CDC cdc;
                        cdc.CreateCompatibleDC(NULL);
                        CDocMapper mapper(this);
                        CDraftContext dc(&cdc,&mapper);
                        ((CMDDimStr*)pNewShape)->SetValues(&dc,ps->str);
                     }
                     ((CMDDimStr*)pNewShape)->GetOwner()->Attach((CMDDimStr*)pNewShape);
                  }
               }
               break;
               case ::formatMD70::IDArea:
               {
                  PhMDArea * parea= (PhMDArea *)pshape;

                  CMDShape* pOwner=NULL;
                  processlayer.shapes.Lookup(parea->cShapeHdl,pOwner);
                  if(pOwner)
                  {
                     //CMDArea* pArea=new CMDArea(pshape->cShapeFlags & kPhDimStrAttached?true:false,pOwner);
                     CMDArea* pArea=new CMDArea(parea->fStatus & kPhDimStrAttached?true:false,pOwner);
                     pNewShape = pArea;
                     //areas.SetAt(pshape->the64,pArea);
                     if(pArea)
                     {
                        TextParams tp;
                        //ctx.ConvertPhFont(parea->fShapeFont,parea->fShapeFontSize,0,tp.m_lf);
                        ctx.ConvertFont(parea->fShapeFont,parea->fShapeFontSize,tp.m_lf);
                        pArea->ModifyTextParams(tp,tpUseFont,NULL);
                        //pNewShape->SetShapePoint(0,parea->c.fLongPt + parea->cPosOrOffset.fLongPt);
                        //pArea->SetShapePoint(0,pOwner->GetShapePoint(0) + parea->cPosOrOffset.fLongPt);
                        pArea->SetShapePoint(0,Middle(pOwner->GetExtents()));
                        pArea->SetRotation(parea->theAngle);
                        {
                           CDC cdc;
                           cdc.CreateCompatibleDC(NULL);
                           CDocMapper mapper(this);
                           CDraftContext dc(&cdc,&mapper);
                           ((CMDDimStr*)pArea)->SetValues(&dc,parea->DimString);
                        }
                        ((CMDDimStr*)pArea)->GetOwner()->Attach((CMDDimStr*)pArea);
                        pOwner->UpdateDependentShapes(pView);
                     }

                  }
               }
               break;

               //case ::formatMD70::IDParallelLine:
               //{

               //   PhMDParallelLine * pline = (PhMDParallelLine *)pshape;

               //   ParallelAttrs attrs;

               //   ctx.ConvertPhParaAttr(pline->fParallelAttr,attrs);

               //   pNewShape=new CMDParallelLine(pline->data[0].fLongPt,pline->data[3].fLongPt,attrs);

               //}
               //break;

               //case ::formatMD70::IDParallelPolygon:
               //{

               //   PhMDParallelPolygon * ppolygon = (PhMDParallelPolygon *)pshape;

               //   ParallelAttrs attrs;

               //   ctx.ConvertPhParaAttr(ppolygon->fParallelAttr,attrs);

               //   pNewShape=new CMDParallelPolygon(ppolygon->vertexCount,ppolygon->cClosed?true:false,attrs);

               //   for(int j = 0; j < ppolygon->vertexCount; j++)
               //   {

               //      pNewShape->SetShapePoint(j,ppolygon->data[j].fLongPt);

               //   }

               //}
               //break;
               case ::formatMD70::IDWall:
               {

                  PhMDWall * ppolygon = (PhMDWall *)pshape;

                  if(ppolygon->segmentsCount <= 0)
                  {

                     ParallelAttrs attrs;

                     ctx.ConvertPhParaAttr(ppolygon->fParallelAttr,attrs);

                     pNewShape=new CMDParallelPolygon(ppolygon->pointsCount,false,attrs);

                     for(int j = 0; j < ppolygon->pointsCount; j++)
                     {

                        pNewShape->SetShapePoint(j,ppolygon->data[j].fLongPt);

                     }

                  }
                  else
                  {

                     int iLast = -1;

                     int iLast1 = -1;
                     int i1;

                     int i2;

                     CMDGroupShape* pGroup=new CMDGroupShape();
                     pNewShape=pGroup;

                     CMDShape* pshapeNew=NULL;

                     CArray < DDBPoint > pointa;
                     processlayer.process_shape(pshape,pNewShape,serialize,penModel,fm);

                     CArray < int > intaRestart;

                  restart_1:

                     for(int jParam = 0; jParam <= ppolygon->segmentsCount; jParam++)
                     {


                        int j = jParam;

                        if(j < ppolygon->segmentsCount)
                        {
                           i1 = ppolygon->segments[j].x;

                           i2 = ppolygon->segments[j].y;

                        }

                        if(i1 == i2 && j < ppolygon->segmentsCount)
                        {

                        }
                        else
                        {

                           if(iLast >= 0 && j < ppolygon->segmentsCount - 1)
                           {

                              bool bSwap = false;

                              if(ppolygon->data[iLast1].fLongPt.y.m_d - ppolygon->data[iLast].fLongPt.y.m_d == 0.)
                              {
                                 if(ppolygon->data[i1].fLongPt.y.m_d - ppolygon->data[i2].fLongPt.y.m_d == 0.)
                                 {

                                    bSwap = !inta_contains(intaRestart,jParam);

                                 }
                              }
                              else if(ppolygon->data[iLast1].fLongPt.x.m_d - ppolygon->data[iLast].fLongPt.x.m_d == 0.)
                              {
                                 if(ppolygon->data[i1].fLongPt.x.m_d - ppolygon->data[i2].fLongPt.x.m_d == 0.)
                                 {
                                    bSwap = !inta_contains(intaRestart,jParam);
                                 }
                              }
                              else
                              {

                                 double dSlope1 =(ppolygon->data[iLast1].fLongPt.x.m_d - ppolygon->data[iLast].fLongPt.x.m_d) /
                                    (ppolygon->data[iLast1].fLongPt.y.m_d - ppolygon->data[iLast].fLongPt.y.m_d);
                                 double dSlope2 =(ppolygon->data[i1].fLongPt.x.m_d - ppolygon->data[i2].fLongPt.x.m_d) /
                                    (ppolygon->data[i1].fLongPt.y.m_d - ppolygon->data[i2].fLongPt.y.m_d);
                                 if(abs(dSlope1 - dSlope2) < 0.00001)
                                 {
                                    bSwap = !inta_contains(intaRestart,jParam);
                                 }

                              }

                              if(bSwap)
                              {

                                 auto segment1 = ppolygon->segments[j];

                                 auto segment2 = ppolygon->segments[j + 1];

                                 ppolygon->segments[j] = segment2;

                                 ppolygon->segments[j + 1] = segment1;

                                 intaRestart.Add(jParam);


                                 iLast = -1;

                                 iLast1 = -1;

                                 pointa.RemoveAll();

                                 goto restart_1;

                              }

                           }

                           if(j < ppolygon->segmentsCount && (iLast < 0 || i1 == iLast))
                           {

                              pointa.Add(ppolygon->data[i1]);

                           }
                           else
                           {

                              {

                                 if(iLast >= 0)
                                 {

                                    pointa.Add(ppolygon->data[iLast]);

                                 }

                                 ParallelAttrs attrs;

                                 ctx.ConvertPhParaAttr(ppolygon->fParallelAttr,attrs);

                                 if(pointa.GetCount() == 2 && ppolygon->segmentsCount == 1)
                                 {

                                    pshapeNew=new CMDParallelLine(pointa[0].fLongPt,pointa[1].fLongPt,attrs);

                                 }
                                 else
                                 {

                                    bool bClosed = pointa[pointa.GetUpperBound()].fLongPt.x == pointa[0].fLongPt.x
                                       && pointa[pointa.GetUpperBound()].fLongPt.y == pointa[0].fLongPt.y;

                                    int iCount = pointa.GetCount();

                                    if(bClosed)
                                    {

                                       iCount--;

                                    }

                                    pshapeNew=new CMDParallelPolygon(iCount,bClosed,attrs);

                                    for(int k = 0; k < iCount; k++)
                                    {

                                       auto & point = pointa[k];

                                       pshapeNew->SetShapePoint(k,pointa[k].fLongPt);

                                    }

                                 }

                                 pointa.RemoveAll();

                                 processlayer.process_shape(pshape,pshapeNew,serialize,penModel,fm);

                                 pGroup->AddMember(pshapeNew);

                              }


                              pointa.Add(ppolygon->data[i1]);

                           }

                        }

                        iLast1 = i1;

                        iLast = i2;

                     }

                     pNewShape->CalculateExtents();

#ifndef _PERSONAL_EDITION

                     linkVertexList.ResolveObjectIndex(nObjectIndex++,pNewShape);

#endif

                     continue;

                  }

               }
               break;

               default:;
               }

               processlayer.process_shape(pshape,pNewShape,serialize,penModel,fm);

#ifndef _PERSONAL_EDITION
               linkVertexList.ResolveObjectIndex(nObjectIndex++,pNewShape);
#endif

            }



            processlayer.shapes.RemoveAll();

            if(processlayer.iPos >= 0)
            {

               processlayer.a.CMDShapeArrayBase::RemoveAt(0,processlayer.iPos + 1);

            }

            {

               int iCount = texta.GetCount();

               for(int iText = 0; iText < iCount; iText++)
               {

                  if(processlayer.a.Find(texta[iText]) == NULL)
                  {

                     OutputDebugString("text not found");

                  }

               }

               pLayer->m_ShapeList.Append(processlayer.a);

               for(int iText = 0; iText < iCount; iText++)
               {

                  if(pLayer->m_ShapeList.Find(texta[iText]) == NULL)
                  {

                     OutputDebugString("text not found");

                  }

               }

               iCount = 0;

               POSITION pos = pLayer->m_ShapeList.GetHeadPosition();

               while(pos != NULL)
               {

                  CMDShape * pshape = pLayer->m_ShapeList.GetNext(pos);

                  if(texta.Find(pshape) != NULL)
                  {

                     iCount++;

                  }

               }

               CString strFormat;

               strFormat.Format("Count=%d",iCount);

               OutputDebugString(strFormat);

            }

            POSITION pos=processlayer.metas.GetStartPosition();
            while(pos)
            {
               UInt64 key;
               CMDShapeArray* pList;
               CMDMetaPolyShape* pMeta;
               processlayer.metas.GetNextAssoc(pos,key,pMeta);
               if(processlayer.metaMembers.Lookup(key,pList))
               {

                  while(pList->GetCount())
                  {

                     CMDShape* pShape=pList->ElementAt(0);

                     if(!pMeta->AddMember(pShape))
                     {

                        break;

                     }

                     pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pShape));
                     
                     pList->CMDShapeArrayBase::RemoveAt(0);

                     pLayer->m_LinkVertexList.DeleteAllByShape(pShape);

                     delete pShape;

                  }

                  pMeta->CalculateExtents();

                  delete pList;

               }
               else
               {
                  pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pMeta));
                  delete pMeta;
               }
            }
            processlayer.metas.RemoveAll();
            processlayer.metaMembers.RemoveAll();

            pos=processlayer.groups.GetStartPosition();
            while(pos)
            {
               UInt64 key;
               CMDShapeList* pList;
               CMDGroupShape* pGroup;
               processlayer.groups.GetNextAssoc(pos,key,pGroup);
               if(processlayer.groupMembers.Lookup(key,pList))
               {
                  POSITION spos=pList->GetHeadPosition();
                  while(spos)
                  {
                     CMDShape* pShape=pList->GetNext(spos);
                     pGroup->AddMember(pShape);
                     pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pShape));
                  }
                  //					pGroup->CalculateExtents();
                  delete pList;
               }
               else if(pGroup->GetMembersCount() <= 0)
               {
                  pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pGroup));
                  processlayer.groups.RemoveKey(key);
                  delete pGroup;
               }
            }
            pos=processlayer.groups.GetStartPosition();
            while(pos)
            {
               UInt64 key;
               CMDGroupShape* pGroup;
               processlayer.groups.GetNextAssoc(pos,key,pGroup);
               pGroup->CalculateExtents();
            }

         }

      }

   }

   OutputDebugString("End");

   if(header.read(serialize.m_pointer,mem,file,kMarkerLayersCustomScaleInfo))
   {

      UInt32 layersInfoCount = serialize.m_pointer.i32();

      CString strFormat;

      strFormat.Format("\nLayersInfoCount = %d\n");

      OutputDebugString(strFormat);

   }


   if(header.read(serialize.m_pointer,mem,file,kMarkerCDocObjectInfo))
   {

      serialize(m_sFieldNames[1]);
      serialize(m_sFieldNames[2]);
      serialize(m_sFieldNames[3]);
      serialize(m_sFieldNames[4]);

   }


//
//   {
//      fi.Seek(pLayers[l - 1].fLayerOffset + pLayers[l - 1].fLayerLength,CFile::begin);
//      PhObjectInfoFieldSet fields;
//      fi.Read(&fields,sizeof(PhObjectInfoFieldSet));
//      uptr p((char*)&fields);
//      p.p2c(32); p.p2c(32); p.p2c(32); p.p2c(32);
//      m_sFieldNames[1]=fields.fF2Name;
//      m_sFieldNames[2]=fields.fF3Name;
//      m_sFieldNames[3]=fields.fF4Name;
//      m_sFieldNames[4]=fields.fF5Name;
//   }
//
//   if(pFont) delete[] pFont;
//   if(pBitl) delete pBitl;
//   for(int j=0; j<ctx.embedDIBs.GetSize(); j++) ::DeleteObject((HBITMAP)ctx.embedDIBs[j]);
//   delete[] resFork;
//
////m_Palette.ResizePalette(83);	// strip extra entries
	SetActiveLayer(nActiveLayer);
   m_nActiveLayer=nActiveLayer;
   UpdateFrameCounts();

   UpdateMaxZoom();

   if(m_bUpdateViews)
   {

      //   UpdateAllViews(NULL,kUnlockWindowUpdate);

      UpdateAllViews(NULL,kRefresh);

      CPoint ptScroll(
         phDocState.fActiveEnvirons.screenViewRectOrigin.y 
          / 1.5,
         phDocState.fActiveEnvirons.screenViewRectOrigin.x 
          / 1.5);

      UpdateAllViews(NULL,kZoomFactorChanged,(CObject*)&ptScroll);

   }

   return true;
}

namespace formatMD70
{

   char * get_memory(UInt64 uiOffset,UInt64 uiSize,CFile & file)
   {

      ULONGLONG ull = file.Seek(uiOffset,CFile::SeekPosition::begin);

      if(ull != uiOffset)
      {

         return NULL;

      }

      char * pdata = new char[uiSize];

      if(pdata == NULL)
      {

         return NULL;

      }

      ull = file.Read(pdata,uiSize);

      if(ull != uiSize)
      {

         delete pdata;

         return NULL;

      }

      return pdata;

   }
















   PhMDShape * PhMDShape::create(eID eid)
   {

      switch(eid)
      {
      case ::formatMD70::IDLine:
      case ::formatMD70::IDLine2:
         return new PhMDLine;
      case ::formatMD70::IDFreehand:
      case ::formatMD70::IDPolygon:

         return new   PhMDPolygon;
      case ::formatMD70::IDRectCenter:
      case ::formatMD70::IDRectDiagonal:
         return new PhMDRectangle;
      case ::formatMD70::IDRndRectangle:
         return new PhMDRndRectangle;
      case ::formatMD70::IDFreeSpline:
         return new PhMDFreeSpline;
      case ::formatMD70::IDPolySpline:
         return new PhMDPolySpline;
      case ::formatMD70::IDCircle:
         return new PhMDCircle;
         break;
      case ::formatMD70::IDArc:
      case ::formatMD70::ID3Arc:
         return new PhMDArc;
         break;
      case ::formatMD70::IDEArc:
         return new PhMDArc;

      case ::formatMD70::IDEllipse:
         return new PhMDEllipse;
      case ::formatMD70::IDBezier:
         return new PhMDSpecificBezier;
      case ::formatMD70::IDCubic:
         return new PhMDCubic;
      case ::formatMD70::IDText:
         return new PhMDText;

      case ::formatMD70::IDBitmap:
         return new PhMDBitmap;
      case ::formatMD70::IDGroup:
         return new PhMDGroup;
      case ::formatMD70::IDMetaPoly:
         return new PhMDMetaPoly;

      case ::formatMD70::IDHLineDim:
      case ::formatMD70::IDVLineDim:
      case ::formatMD70::IDSLineDim:
      case ::formatMD70::IDPLineDim:
         return new PhMDLineDim;
      case ::formatMD70::IDRRadialDim:
      case ::formatMD70::IDDRadialDim:
         return new PhMDLineDim;
      case ::formatMD70::IDCenterDim:
         return new PhMDCenterDim;
      case ::formatMD70::IDAngularDim:
         return new PhMDAngularDim;
      case ::formatMD70::IDDimPt2Pt:
         return  new PhMDDim;
      case ::formatMD70::IDDimString:
         return new PhMDDimStr;
      case ::formatMD70::IDArea:
         return new PhMDArea;

      //case ::formatMD70::IDParallelLine:
      //   return new PhMDParallelLine;

      //case ::formatMD70::IDParallelPolygon:
      //   return new PhMDParallelPolygon;
      case ::formatMD70::IDWall:
         return new PhMDWall;
      case ::formatMD70::IDHatch:
         return new PhMDHatch;

      default:
         throw "error";

      };

      return NULL;

   }



} // namespace formatMD70




