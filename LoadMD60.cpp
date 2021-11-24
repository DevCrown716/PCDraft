// MD6.2.1 - Infinisys Ltd
#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "FilletContext.h"
#include "ChamferContext.h"

#include "Layer.h"
#include "Import.h"
#include "PhIOStruct_MD60.h"
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

#define ARGB(a, r, g, b) (RGB((r), (g), (b)) | ((a) << 24))

const Coord kAlmostSameCriteria = 400.0 / 65536.0;
using namespace formatMD60;


bool CPCDraftDoc::LoadMacDraft60(const char *szPath, bool bUtf8Name)
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

   ::formatMD60::PhIOContext ctx(&file);

   ctx.pDoc = this;

   ctx.serialize.m_bWrite = false;

   ctx.pPalette=&m_Palette;

   ctx.pDoc=this;

   PhDocState phDocState;

   file.Read(&phDocState,sizeof(PhDocState));

   serialize serialize;

   serialize.m_piocontext = &ctx;

   serialize.m_bWrite = false;

   serialize.m_pointer.synch_type(&phDocState);

   serialize(&phDocState);

   if(phDocState.fType != kPhDocType) return false;

   if(phDocState.fHeaderID != kPhFileHdr || phDocState.fVersion != kPhRelease7) return false;

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
      for(int i=0; i<3; i++)
      {
         ctx.ConvertPhDimFmt(phDocState.fDocStds.fEnglishDimFormat[i],m_DimFmts[i]);
      }
   }
   else
   {
      ctx.ConvertPhDimStd(phDocState.fDocStds.fMetricDimStd,m_DimStd);
      for(int i=0; i<3; i++)
      {
         ctx.ConvertPhDimFmt(phDocState.fDocStds.fMetricDimFormat[i],m_DimFmts[i]);
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
   m_ptPrevZoomScroll.x=phDocState.fActiveEnvirons.fTrunk.fHCurScroll;
   m_ptPrevZoomScroll.y=phDocState.fActiveEnvirons.fTrunk.fVCurScroll;

   m_bSnapToObject=phDocState.fActiveEnvirons.fSnapToObject?true:false;
   m_bStandardRulers=phDocState.fActiveEnvirons.fStandardRulers?true:false;
   m_bHidePageBreaks=phDocState.fActiveEnvirons.fHidePageBreaks?true:false;

   m_bPrintLastPageFirst=phDocState.fActiveEnvirons.fPrintFlags&kPhPrintLastFirst?true:false;
   m_nPageNumbering=phDocState.fActiveEnvirons.fPrintFlags&kPhPrintByCols?0:1;
   m_bRegistrationMarks=phDocState.fActiveEnvirons.fPrintFlags&kPhRegMarks?true:false;

   m_nDisplayRes=phDocState.fDocUnits.fDefResolution.x;

#ifdef _FLOAT_ZOOM
   UnpackZoomFactor(phDocState.fActiveEnvirons.fPrevZoom,m_nPrevZoomFactor,m_fPrevZoomFactor);
   UnpackDoubleZoomFactor(phDocState.fActiveEnvirons.fTrunk.fZoomFactor,m_nZoomFactor,m_fZoomFactor);
#else
   m_nPrevZoomFactor=phDocState.fActiveEnvirons.fPrevZoom;
   m_nZoomFactor=phDocState.fZoom;
#endif

//   m_snapTable.setAngleSnaps((phDocState.fActiveEnvirons.fAngleSnapsToACirclePart1 << 8) | phDocState.fActiveEnvirons.fAngleSnapsToACirclePart2);
////	m_nDisplayRes=phDocState.fActiveEnvirons.;
////	m_bHideFills=phDocState.fActiveEnvirons.;
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
//      for(int i=0; i<nCount; i++)
//      {
//         lp->palPalEntry[i].peRed=p.swaps() >> 8;
//         lp->palPalEntry[i].peGreen=p.swaps() >> 8;
//         lp->palPalEntry[i].peBlue=p.swaps() >> 8;
//         lp->palPalEntry[i].peFlags=0;
//         p.swaps(5);
//      }
//
//      m_Palette.CreatePalette(lp);
//      delete[] lp;
//      delete pPal;
//   }
//

   memory mem;

   OffsetHeader header;

   CString strPaper;
   int iPageScale = 100;

   int iOrientation = -1;

   int iCustomPaperHeight = -1;
   int iCustomPaperWidth = -1;
   bool bCustomPaper = false;

   if(m_bUpdateViews)
   {

      if(header.read(serialize.m_pointer,mem,file,kMarkerFlatPageFormat))
      {

         long * pl = (long *)mem.m_pdata;

         ::formatMD60::swap32(&pl[0]);
         ::formatMD60::swap32(&pl[1]);

         CString str((char *)&pl[2],pl[0]);

         ::OutputDebugString(str);

         XDoc	doc;

         if(!doc.Load(str))
         {
            goto end_flat_page_format;
         }
         XNode * plist = doc.GetChild("plist");

         if (plist == NULL)
         {

            goto end_flat_page_format;

         }

         XNode * pmain = plist->GetChild("dict");

         if (pmain == NULL)
         {

            goto end_flat_page_format;

         }

         int iFind = pmain->IndexByValue("key", "com.apple.print.subTicket.paper_info_ticket");

         if (iFind >= 0)
         {

            XNode * pdict = pmain->GetChild(iFind + 1);

            if (pdict != NULL && pdict->name == "dict")
            {

               int iFind2 = pdict->IndexByValue("key", "com.apple.print.PageFormat.PMAdjustedPageRect");

               if (iFind2 >= 0)
               {

                  XNode * pdict2 = pdict->GetChild(iFind2 + 1);

                  if (pdict2 != NULL && pdict2->name == "dict")
                  {

                     int iFind3 = pdict2->IndexByValue("key", "com.apple.print.ticket.itemArray");

                     if (iFind3 >= 0)
                     {

                        XNode * parray = pdict2->GetChild(iFind3 + 1);

                        if (parray != NULL && parray->name == "array")
                        {

                           XNode * pdict3 = parray->GetChild(0);

                           if (pdict3 != NULL && pdict3->name == "dict")
                           {

                              int iFind4 = pdict3->IndexByValue("key", "com.apple.print.PageFormat.PMAdjustedPageRect");

                              if (iFind4 >= 0)
                              {

                                 XNode * parray = pdict3->GetChild(iFind4 + 1);

                                 if (parray)
                                 {

                                    CArray < double > da;

                                    if (parray->get_child_numbera(da) && da.GetCount() == 4)
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

               int iFind2B = pdict->IndexByValue("key", "com.apple.print.PaperInfo.PMCustomPaper");

               if (iFind2B >= 0)
               {

                  XNode * pdict2 = pdict->GetChild(iFind2B + 1);

                  if (iFind2 >= 0)
                  {

                     XNode * pdict2 = pdict->GetChild(iFind2B + 1);

                     if (pdict2 != NULL && pdict2->name == "dict")
                     {

                        int iFind3 = pdict2->IndexByValue("key", "com.apple.print.ticket.itemArray");

                        if (iFind3 >= 0)
                        {

                           XNode * parray = pdict2->GetChild(iFind3 + 1);

                           if (parray != NULL && parray->name == "array")
                           {

                              XNode * pdict3 = parray->GetChild(0);

                              if (pdict3 != NULL && pdict3->name == "dict")
                              {

                                 int iFind4 = pdict3->IndexByValue("key", "com.apple.print.PaperInfo.PMCustomPaper");

                                 if (iFind4 >= 0)
                                 {

                                    XNode * pbool = pdict3->GetChild(iFind4 + 1);

                                    if (pbool && pbool->name.CompareNoCase("true") == 0)
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

               if (!pdict->get_pinfo_string(strPaper, "PMPPDPaperCodeName"))
               {

                  if (!pdict->get_pinfo_string(strPaper, "com.apple.print.PaperInfo.PMPaperName"))
                  {


                  }

               }


            }

         }


         double dScale;

         if (pmain->get_pinfo_real(dScale, "com.apple.print.PageFormat.PMScaling"))
         {

            iPageScale = (int)(dScale * 100.0);

         }

         if (strPaper.IsEmpty())
         {

            if (!pmain->get_pinfo_string(strPaper, "com.apple.print.PaperInfo.PMCodeName"))
            {

               pmain->get_pinfo_string(strPaper, "com.apple.print.PaperInfo.PMPaperName");


            }

         }


         if (iOrientation < 0)
         {

            pmain->get_pinfo_integer(iOrientation, "com.apple.print.PageFormat.PMOrientation");

         }

      }


        /* XNode * plist = doc.GetChild("plist");

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

      }*/

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


      if (bCustomPaper && iCustomPaperWidth > 0 && iCustomPaperHeight > 0)
      {

         m_PrinterSettings.SetPageSize(DMPAPER_USER);

         if (iOrientation == 2)
         {

            m_PrinterSettings.SetCustomPageSize(MulDiv(iCustomPaperHeight, 254, 72), MulDiv(iCustomPaperWidth, 254, 72));

         }
         else
         {

            m_PrinterSettings.SetCustomPageSize(MulDiv(iCustomPaperWidth, 254, 72), MulDiv(iCustomPaperHeight, 254, 72));

         }

         m_PrinterSettings.SetPageScale(iPageScale);

         //m_PrinterSettings.SetPageOrientation(iOrientation == 2 ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT);


      }
      else
      {

         m_PrinterSettings.SetPageSize(iPaper);
         m_PrinterSettings.SetPageScale(iPageScale);
         m_PrinterSettings.SetPageOrientation(iOrientation == 2 ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT);

      }



   }

   if(header.read(serialize.m_pointer,mem,file,kMarkerFlatPrintSettings))
   {
      CString str(mem.m_pdata,mem.m_iSize);

      ::OutputDebugString(str);
   }

   CArray < int > fontNum;
   CStringArray fontName;

   if(header.read(serialize.m_pointer,mem,file,kMarkerFontInUseInfo))
   {
      
      long fontCount = serialize.m_pointer.i32();

      for(int i = 0; i < fontCount; i++)
      {

         int iFontNum = serialize.m_pointer.sh();

         fontNum.Add(iFontNum);

         char len = serialize.m_pointer.ch();

         CString strFontName(serialize.m_pointer.pch,len);

         OutputDebugString(strFontName);

         fontName.Add(strFontName);

         ctx.fontMap.SetAt(iFontNum,strFontName);

         serialize.m_pointer.pch += len;

      }

   }

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

            short theShort[7];

            serialize.serialize_static_array(theShort,7);

            m_Palette.DeleteObject();

            memory memPal;

            memPal.allocate(sizeof(LOGPALETTE) + (entries - 1)*sizeof(PALETTEENTRY));

            PhPaletteEntry entry;

            LPLOGPALETTE lp = (LPLOGPALETTE) memPal.m_pdata;
            lp->palVersion=0x300;
            lp->palNumEntries=entries;
            for(int i=0; i < entries; i++)
            {
               
               serialize(entry);

               lp->palPalEntry[i].peRed=entry.ciRGB.red >> 8;
               lp->palPalEntry[i].peGreen=entry.ciRGB.green >> 8;
               lp->palPalEntry[i].peBlue=entry.ciRGB.blue >> 8;
               lp->palPalEntry[i].peFlags=0;

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

         serialize.m_pointer.pch = mem.m_pdata;
         
         serialize(&header); // swap again to original value

         serialize.m_pointer.pch = mem.m_pdata + sizeof(UInt64);

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

      for(int i=0; i < cCount; i++)
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
               //if(i<72)
               //{
               //   m_Patterns[i - 1].FromDIBSection(hDib);
               //   m_PatternsHash[i - 1] = CalculateDIBHash(hDib);
               //   ::DeleteObject(hDib);
               //   //						if(m_DIBPatterns[i-1]) DeleteObject(m_DIBPatterns[i-1]);
               //   //						m_DIBPatterns[i-1]=hDib;
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

         ::formatMD60::serialize serialize;

         serialize.m_bWrite = false;

         if(serialize.m_pointer.read(mem,file,bitmap.fBitmapOffset,bitmap.fBitmapLength))
         {
            complex_type_context c(&serialize);
            if(!bitmap.fColor)
            {

               serialize(bitmap.m_ncbitmap.theLong);
               serialize(bitmap.m_ncbitmap.rowBytes);
               serialize(bitmap.m_ncbitmap.bounds);

               {
                  //no_swap_context noswapcontext(&serialize);
                  serialize(bitmap.m_ncbitmap.data,bitmap.m_ncbitmap.theLong);
               }

            }
            else
            {

               serialize(bitmap.m_bitmap.theLong);
               serialize(bitmap.m_bitmap.rowBytes);
               serialize(bitmap.m_bitmap.bounds);
               serialize(bitmap.m_bitmap.pmVersion);
               serialize(bitmap.m_bitmap.packType);
               serialize(bitmap.m_bitmap.packSize);
               serialize(bitmap.m_bitmap.hRes);
               serialize(bitmap.m_bitmap.vRes);
               serialize(bitmap.m_bitmap.pixelType);
               serialize(bitmap.m_bitmap.pixelSize);
               serialize(bitmap.m_bitmap.cmpCount);
               serialize(bitmap.m_bitmap.cmpSize);
               serialize(bitmap.m_bitmap.pixelFormat);
               serialize(bitmap.m_bitmap.pmTable);
               serialize(bitmap.m_bitmap.pmExt);
               
               {

                  //no_swap_context noswapcontext(&serialize);
                  serialize(bitmap.m_bitmap.data,bitmap.m_bitmap.theLong);
                  serialize(bitmap.m_bitmap.table,bitmap.m_bitmap.pmTable);

               }
            }

         }

         bitmapa.Add(bitmap);

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
//      for(int i=0; i<pList->fNumElems; i++)
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
//      for(int i=0; i<pList->fNumElems; i++)
//      {
//         p.p2c(kMaxViewName); p.skip(2); p.swapl(1); p.swaps(1);
//
//         CSavedView* pView=new CSavedView;
//         pView->m_sTitle=pSavedView[i].fViewName;
//         pView->m_ptOrg.x=pSavedView[i].fOldScroll.x;
//         pView->m_ptOrg.y=pSavedView[i].fOldScroll.y;
//         pView->m_nZoom=pSavedView[i].fZoomFactor;
//         pView->m_nShortcut=pSavedView[i].fCmdKey?(pSavedView[i].fCmdKey - '4'):0;
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
         
         for(int i=0; i < 6; i++) pReport->m_nColumnWidths[i]=report.fColumnWidths[i];
         
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
                     for(int i=0; i < 6; i++)
                     {
                        pReportRow->m_nJust[i]=ptrd->fJust[i];
                        pReportRow->m_sRowText[i]=ptrd->Str[i];
                     }
                  }
                  break;
                  case kPhCriteriaRow:
                  {
                     pReportRow->m_nRowType=kCriteriaRow;
                     PhCriteriaRow * pcrd=(PhCriteriaRow*)report.rowa[iRow];
                     for(int i=0; i < 6; i++) pReportRow->m_nFieldRefNum[i]=pcrd->fFieldRefNum[i];
                  }
                  break;
                  case kPhFunctionRow:
                  {
                     pReportRow->m_nRowType=kFunctionRow;
                     PhFunctionRow* pfrd=(PhFunctionRow*)report.rowa[iRow];
                     pReportRow->m_nFunction=pfrd->fSpare;
                     if(pReportRow->m_nFunction > kFunctTotalName) pReportRow->m_nFunction--;
                     for(int i=0; i < 6; i++)
                     {
                        pReportRow->m_nJust[i]=pfrd->fJust[i];
                        pReportRow->m_sRowText[i]=pfrd->Str[i];
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
   //         for(int i=0; i<count; i++)
   //         {
   //            if(!plp->fLinks[i].fFlags) // skip notify-only
   //            {
   //               CLinkVertex* pLinkVertex=new CLinkVertex;
   //               pLinkVertex->m_nKey=resNo;
   //               pLinkVertex->m_nShape=plp->fLinks[i].fObjectIndex;
   //               pLinkVertex->m_nVertex=plp->fLinks[i].fPointIndex;
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

         ::formatMD60::serialize serialize;

         serialize.m_piocontext = &ctx;

         serialize.m_ratio = layer.fCustomRatio;

         if(!serialize.m_pointer.read(mem,file,layer.fLayerOffset,layer.fLayerLength))
         {

            throw "error";

         }

         serialize.m_bWrite = false;


         UInt64 numShapes = serialize.m_pointer.ui64();
         CMDShapeArray a;

         a.SetSize(numShapes);

         int iPos = numShapes - 1;

//         numShapes= 1;

         if(numShapes > 0)
         {

            CMap<UInt64,UInt64,CMDShape*,CMDShape*> shapes;
            CMap<UInt64,UInt64,CMDShape*,CMDShape*> areas;
            CMap<UInt64,UInt64,CMDDimLine*,CMDDimLine*> dims;
            CMap<UInt64,UInt64,CMDMetaPolyShape*,CMDMetaPolyShape*> metas;
            CMap<UInt64,UInt64,CMDShapeList*,CMDShapeList*> metaMembers;
            CMap<UInt64,UInt64,CMDGroupShape*,CMDGroupShape*> groups;
            CMap<UInt64,UInt64,CMDShapeList*,CMDShapeList*> groupMembers;

            shapes.InitHashTable(2047,1);
            groups.InitHashTable(2047,1);
            groupMembers.InitHashTable(2047,1);

            long lObjectSize;
            long lInfoLocation;

#if DEV_DEBUG

            CString strIds;

#endif

            for(int i = 0; i < numShapes; i++)
            {
               if(i == 262)
               {
                  OutputDebugString("i==1");
               }

               CMDShape* pNewShape=NULL;
               eID eid = (eID)serialize.m_pointer.i32();
               lObjectSize = serialize.m_pointer.i32();
               lInfoLocation = serialize.m_pointer.i32();

               if(eid == ::formatMD60::IDText)
               {
                  OutputDebugString("IDText");
               }

               ::std::unique_ptr < PhMDShape > pshapeGuard;

               pshapeGuard.reset(PhMDShape::create(eid));

               PhMDShape * pshape = pshapeGuard.get();

               serialize(pshape);

               PenModel penModel;

               ctx.ConvertPhPenModel(pshape->cPenModel,penModel);


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

               if(pshape->fOwnerGroup != NULL)
               {

                  if(ctx.groupa64Debug.Find(pshape->fOwnerGroup) == NULL)
                  {

                     ctx.groupa64Debug.Add(pshape->fOwnerGroup);

                  }

                  strFormat.Format("group=%d ",ctx.groupa64Debug.Find(pshape->fOwnerGroup));

                  strIds += strFormat;

               }

               strFormat.Format("%s ", shape_text(eid));

               strIds += strFormat;

#endif

               switch(eid)
               {
               case ::formatMD60::IDLine:
               case ::formatMD60::IDLine2:
               {

                  PhMDLine * pline = (PhMDLine *)pshape;

                  pNewShape = new CMDLineShape(pline->fP1.fLongPt, pline->fP2.fLongPt,pline->cConAngle);

               }
               break;
               case ::formatMD60::IDFreehand:
               case ::formatMD60::IDPolygon:
               {

                  #if DEV_DEBUG
                  if(eid == ::formatMD60::IDFreehand)
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

                  pNewShape=new CMDPolygonShape(eid == ::formatMD60::IDFreehand?CMDPolygonShape::kFreehand:CMDPolygonShape::kPolygon,ppolygon->vertexCount,ppolygon->cClosed?true:false);

                  for(int i=0; i<ppolygon->vertexCount; i++)
                  {

                     pNewShape->SetShapePoint(i,ppolygon->data[i].fLongPt);

                  }

               }
               break;

               case ::formatMD60::IDRectCenter:
               case ::formatMD60::IDRectDiagonal:
               {

                  PhMDRectangle* prect=(PhMDRectangle*)pshape;

                  pNewShape=new CMDRectangle(eid == formatMD60::IDRectCenter?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);

                  pNewShape->SetShapePoint(0,prect->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,prect->fPoint[1].fLongPt);
                  pNewShape->SetShapePoint(2,prect->fPoint[2].fLongPt);
                  pNewShape->SetShapePoint(3,prect->fPoint[3].fLongPt);

               };
               break;
               case ::formatMD60::IDRndRectangle:
               {

                  PhMDRndRectangle * prect=(PhMDRndRectangle*)pshape;

                  CMDRndRectangle::CornerType type;

                  switch(prect->cCornerType)
                  {
                  case ::formatMD60::kProportionalCorner:
                     type=CMDRndRectangle::kCornerProportional;
                     break;
                  case ::formatMD60::kConstantCorner:
                     type=CMDRndRectangle::kCornerCircle;
                     break;
                  case ::formatMD60::kEllipticalCorner:
                     type=CMDRndRectangle::kCornerElliptical;
                     break;
                  }

                  pNewShape=new CMDRndRectangle(type);

                  DCoordPoint axisPt=Average(prect->fPoint[1].fLongPt,prect->fPoint[3].fLongPt);

                  //pNewShape->SetShapePoint(0,::formatMD60::mtRotPoint(prect->fPoint[0].fLongPt,prect->fRot,axisPt));
                  //pNewShape->SetShapePoint(1,::formatMD60::mtRotPoint(prect->fPoint[1].fLongPt,prect->fRot,axisPt));
                  //pNewShape->SetShapePoint(2,::formatMD60::mtRotPoint(prect->fPoint[2].fLongPt,prect->fRot,axisPt));
                  //pNewShape->SetShapePoint(3,::formatMD60::mtRotPoint(prect->fPoint[3].fLongPt,prect->fRot,axisPt));
                  pNewShape->SetShapePoint(0,prect->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,prect->fPoint[1].fLongPt);
                  pNewShape->SetShapePoint(2,prect->fPoint[2].fLongPt);
                  pNewShape->SetShapePoint(3,prect->fPoint[3].fLongPt);

                  ((CMDRndRectangle*)pNewShape)->m_lptCornerDelta=prect->cDiagonals.fLongPt.operator LongPoint() >> 1;

               }
               break;
               case ::formatMD60::IDFreeSpline:
               {

                  PhMDFreeSpline * pspline = (PhMDFreeSpline*)pshape;

                  pNewShape=new CMDPolygonShape(CMDPolygonShape::kFreeSpline,pspline->vertexCount,pspline->cClosed?true:false);

                  for(int i=0; i<pspline->vertexCount; i++)
                  {

                     pNewShape->SetShapePoint(i,pspline->data[i].fLongPt);

                  }

               }
               break;
               case ::formatMD60::IDPolySpline:
               {

                  PhMDPolySpline* pspline=(PhMDPolySpline*)pshape;

                  int n = 4 * pspline->vertexCount + 1;

                  if(!pspline->cClosed)
                  {

                     n += 2;

                  }

                  pNewShape = new CMDPolygonShape(CMDPolygonShape::kPolySpline, n, pspline->cClosed ? true : false);

                  for(int i=0; i<n; i++)
                  {

                     pNewShape->SetShapePoint(i,pspline->data[i].fLongPt);

                  }

               }
               break;
               case ::formatMD60::IDCircle:
               {

                  PhMDCircle* pcircle=(PhMDCircle*)pshape;

                  CMDCircle::Mode mode;

                  switch(pcircle->method)
                  {
                  case ::formatMD60::eCenterRadius:
                     mode=CMDCircle::kRadius;
                     break;
                  case ::formatMD60::eDiameter:
                     mode=CMDCircle::kDiameter;
                     break;
                  case ::formatMD60::e3Point:
                     mode=CMDCircle::kThreePoint;
                     break;
                  }
                  pNewShape=new CMDCircle(mode);
                  switch(pcircle->method)
                  {
                  case ::formatMD60::eCenterRadius:
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
                  case ::formatMD60::eDiameter:
                     pNewShape->SetShapePoint(0,pcircle->data[1].fLongPt);
                     pNewShape->SetShapePoint(1,pcircle->data[2].fLongPt);
                     break;
                  case ::formatMD60::e3Point:
                     pNewShape->SetShapePoint(0,pcircle->data[1].fLongPt);
                     pNewShape->SetShapePoint(1,pcircle->data[2].fLongPt);
                     pNewShape->SetShapePoint(2,pcircle->data[3].fLongPt);
                     break;
                  }
               }
               break;
               case ::formatMD60::IDArc:
               case ::formatMD60::ID3Arc:
               case ::formatMD60::IDEArc:
               {

                  PhMDArc * parc = (PhMDArc *)pshape;

                  int mode;

                  //ASSERT(parc->method != ::formatMD60::eEllipse);

                  switch(parc->method)
                  {
                  case ::formatMD60::eCenterRadius:
                     mode=CMDArcShape::kRadius;
                     break;
                  case ::formatMD60::e3Point:
                     mode=CMDArcShape::kThreePoint;
                     break;
                  case ::formatMD60::eEllipse:
                     mode=CMDArcShape::kElliptic;
                     break;
                  }
                  pNewShape=new CMDArcShape(mode);
                  switch(parc->method)
                  {
                  case ::formatMD60::eCenterRadius:
                  {

                     pNewShape->SetShapePoint(0,parc->fCenter.fLongPt);
                     
                     pNewShape->SetShapePoint(1,parc->StartPt.fLongPt);
                     
                     pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);

                     ::formatMD60::Angle a1 = parc->cStartAng;

                     ::formatMD60::Angle a2 = ::formatMD60::mtAngle(parc->fCenter.fLongPt,parc->EndPt.fLongPt);

                     ::formatMD60::Angle angA = a1 - a2;
                     
                     angA = ::formatMD60::mtFixAngle(angA);

                     ::Angle ang2 = ::mtFixAngle(parc->cEndAng - ::mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2)));
                     ::Angle ang180 = ::k180;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = ang <= ang180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection = angA >= 90.0?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_COUNTERCLOCKWISE;

                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD60::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>formatMD60::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD60::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>::formatMD60::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
#if DEV_DEBUG
                     penModel.m_nPenType=kPenColor;
                     penModel.m_crColor=RGB(0,255,0);
                     penModel.m_nStyle=0;
                     penModel.m_iPenOpacity = 255;
#endif
                     if(parc->cBackwards)
                     {
                           ((CMDArcShape*)pNewShape)->m_nArcDirection = AD_CLOCKWISE;
                     }
                  }
                     break;
                  case ::formatMD60::e3Point:
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
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD60::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>formatMD60::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(parc->cStartAng - mtFixAngle(formatMD60::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))>::formatMD60::kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

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
                  case ::formatMD60::eEllipse:
                  {
                     pNewShape->SetShapePoint(0,parc->fCenter.fLongPt);
                     pNewShape->SetShapePoint(1,parc->StartPt.fLongPt);
                     pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);
                     //pNewShape->SetShapePoint(3,parc->ThirdPt.fLongPt);
                     //pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(parc->Radii.x,parc->Radii.y),-pshape->fRot));
                     pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(parc->Radii.x,parc->Radii.y),-pshape->fRot));
                     //pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + DCoordPoint(parc->Radii.x,parc->Radii.y));
                     //pNewShape->SetShapePoint(3,parc->fCenter.fLongPt + Abs(parc->ThirdPt.fLongPt - parc->fCenter.fLongPt));

                     //parc->cStartAng = mtFixAngle(90 - parc->cStartAng);
                     //parc->cEndAng=mtFixAngle(90 - parc->cEndAng);
                     ////((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE; //(ps->fStartAng>ps->fEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                     //((CMDArcShape*)pNewShape)->m_nArcDirection=(parc->cStartAng>parc->cEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

                     DCoordPoint ptAxis1 = parc->fCenter.fLongPt;
                     DCoordPoint ptAxis2 = parc->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(parc->Radii.x,parc->Radii.y),-pshape->fRot);

                     DCoordPoint ptCenter = (ptAxis1 + ptAxis2) / 2.0;

                     //LongPoint ptCenter = parc->ThirdPt.fLongPt;

                     {
                        ::Angle angx=mtFixAngle(mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(1)) - mtAngle(pNewShape->GetShapePoint(0),pNewShape->GetShapePoint(2))) ;
                        double d = angx.GetRawDouble() / 65536.0;
                        ::formatMD60::Angle ang=::formatMD60::mtFixAngle(mtAngle(ptCenter,parc->StartPt.fLongPt) - mtAngle(ptCenter,parc->EndPt.fLongPt));
                        //((CMDArcShape*)pNewShape)->m_nArcDirection = ang < ::k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                        ::formatMD60::Angle  ang1= parc->cStartAng;
                        ::formatMD60::Angle  ang2 = parc->cEndAng;
                        ::formatMD60::Angle  dang = ang2 - ang1;

                        dang = ::formatMD60::mtFixAngle(dang);

                        ((CMDArcShape*)pNewShape)->m_nArcDirection = d >= 0 && d <= 90.0 ? AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

                     }


                     //{

                     //   ::formatMD60::Angle  ang1= parc->cStartAng;
                     //   ::formatMD60::Angle  ang2 = parc->cEndAng;
                     //   ang1 = ::formatMD60::mtFixAngle(ang1-90);
                     //   ang2 = ::formatMD60::mtFixAngle(ang2-90);
                     //   ::formatMD60::Angle  dang = ang2 - ang1;
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
                     //                     //if(Abs(parc->cStartAng - mtFixAngle(formatMD60::k90 - mtAngle(parc->fCenter.fLongPt,parc->StartPt.fLongPt)))<1.0)
                     //                     //if(dba < 33.0)
                                          if(((CMDArcShape*)pNewShape)->m_nArcDirection== AD_COUNTERCLOCKWISE)
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
                  //               case ::formatMD60::IDEArc:
                  //{

                  //   PhMDArc * parc =(PhMDArc*)pshape;

                  //   ASSERT(parc->method == ::formatMD60::eEllipse);
                  //   pNewShape=new CMDArcShape(CMDArcShape::kElliptic);
                  //   pNewShape->SetShapePoint(0,parc->fCenter.fLongPt);
                  //   pNewShape->SetShapePoint(1,parc->StartPt.fLongPt);
                  //   pNewShape->SetShapePoint(2,parc->EndPt.fLongPt);
                  //   pNewShape->SetShapePoint(3,parc->fCenter.fLongPt.operator LongPoint() + ::formatMD60::mtRotPoint(LongPoint(parc->Radii.x,parc->Radii.y),pshape->fRot));
                  //   //parc->cStartAng=mtFixAngle(90 - ps->cStartAng);
                  //   //parc->cEndAng=mtFixAngle(90 - ps->cEndAng);
                  //   ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE; //(ps->fStartAng>ps->fEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
                  }
                  break;
               }
               case ::formatMD60::IDEllipse:
               {

                  PhMDEllipse* ps=(PhMDEllipse*)pshape;

                  pNewShape=new CMDEllipse(ps->method == ::formatMD60::eCenterRadius?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);

                  pNewShape->SetShapePoint(0,ps->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(-ps->radius.x.m_f,-ps->radius.y.m_f),pshape->fRot));
                  pNewShape->SetShapePoint(1,ps->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(ps->radius.x.m_f,-ps->radius.y.m_f),pshape->fRot));
                  pNewShape->SetShapePoint(2,ps->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(ps->radius.x.m_f,ps->radius.y.m_f),pshape->fRot));
                  pNewShape->SetShapePoint(3,ps->fCenter.fLongPt + ::formatMD60::mtRotPoint(DCoordPoint(-ps->radius.x.m_f,ps->radius.y.m_f),pshape->fRot));
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
               case ::formatMD60::IDBezier:
               {

                  PhMDSpecificBezier * pbezier =(PhMDSpecificBezier*)pshape;

                  int n=pbezier->vertexCount * 3 + 1;

                  pNewShape=new CMDBezierShape(n);

                  for(int i=0; i < n; i++)
                  {

                     pNewShape->SetShapePoint(i,pbezier->data[i].fLongPt);

                  }

               }
               break;
               case ::formatMD60::IDCubic:
               {

                  PhMDCubic * pcubic =(PhMDCubic*)pshape;

                  int n = pcubic->vertexCount;

                  if(!pcubic->cClosed)
                  {

                     n++;

                  }

                  pNewShape=new CMDCubicShape(n, pcubic->cClosed ? true : false);

                  for(int i=0; i < n; i++)
                  {

                     pNewShape->SetShapePoint(i,pcubic->data[i * 3].fLongPt);

                  }

               }
               break;
               case ::formatMD60::IDText:
               {

                  PhMDText* ps = (PhMDText *)pshape;


                  //{

                  //   int iLen=ps->chars.GetLength();

                  //   WCHAR * pw = new WCHAR[iLen * 2];

                  //   int i1 = MultiByteToWideChar(CP_MACCP,0,ps->chars,ps->chars.GetLength(),pw,iLen *2);

                  //   int i2 = WideCharToMultiByte(CP_ACP,0,pw, i1,ps->chars.GetBufferSetLength(iLen*3),iLen*3, NULL, NULL);

                  //   ps->chars.ReleaseBuffer();

                  //   delete[]pw;

                  //}

                  TextParams tp;
                  switch(ps->cTextJust)
                  {
                  default:
                  case 0:
                     tp.m_nJust=kAlignLeft;
                     break;
                  case 1:
                     tp.m_nJust=kAlignCenter;
                     break;
                  case -1:
                     tp.m_nJust=kAlignRight;
                     break;
                  }
                  switch((ps->fShapeFlags&kPhVertTextAlignMask) >> kPhVertTextAlignCodeShift)
                  {
                  default:
                  case 0:
                     tp.m_nVAlign=kAlignTop;
                     break;
                  case 1:
                     tp.m_nVAlign=kAlignMiddle;
                     break;
                  case 2:
                     tp.m_nVAlign=kAlignBottom;
                     break;
                  }
                  switch(ps->cTextSpace)
                  {
                  default:
                  case ::formatMD60::kSingleSpace:
                     tp.m_nSpacing=::kSingleSpace;
                     break;
                  case ::formatMD60::kOne_halfSpace:
                     tp.m_nSpacing=::k1_5Space;
                     break;
                  case ::formatMD60::kDoubleSpace:
                     tp.m_nSpacing=::kDoubleSpace;
                     break;
                  }

                  tp.m_nCase=0;

                  pNewShape=new CMDText(ps->cParagraph?CMDText::kTextUserSize:CMDText::kTextAutoSize);
                  pNewShape->SetShapePoint(0,ps->data[0].fLongPt);
                  pNewShape->SetShapePoint(1,ps->data[1].fLongPt);
                  pNewShape->SetShapePoint(2,ps->data[2].fLongPt);
                  pNewShape->SetShapePoint(3,ps->data[3].fLongPt);
                  pNewShape->SetTextParams(tp);

                  penModel.m_nPenType = 1;


                  texta.Add(pNewShape);

                  //if(pshape->fObjectInfo)
                  //{
                  //   short count=p.swaps();
                  //   if(pNewShape)
                  //   {
                  //      PhObjectInfo* pInfo=(PhObjectInfo*)p.pc;
                  //      p.swaps(2);
                  //      count-=4;

                  //      CString sValues[5];
                  //      for(int i=0; i<5; i++)
                  //      {
                  //         int len=*p.pc++;
                  //         count--;
                  //         sValues[i]=p.pc;
                  //         p.skip(++len);
                  //         count-=len;
                  //      }
                  //      pNewShape->SetValues(sValues);
                  //   }
                  //   p.skip(count);
                  //   pshape->fObjectInfo=0;
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

                  //   for(int i=0; i<pl->fNumElems; i++)
                  //   {
                  //      p.swaps(1); p.swapl(1);
                  //      if(pl->fData[i].fType >= 600 && pl->fData[i].fType<700)
                  //      {
                  //         nCount++;
                  //         dwProperty[pl->fData[i].fType - 600]=pl->fData[i].fData;
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

                  CStyledText st;
                  CStyledTextStyle s;
                  int n=0;

                  HDC hDC=GetDC(NULL);
                  int nLogPixelsY=GetDeviceCaps(hDC,LOGPIXELSY);
                  ReleaseDC(NULL,hDC);
                  for(int i=0; i<ps->teLength; i++)
                  {
                     if(n<ps->scrCount && i == ps->scripta[n].scrpStartChar)
                     {
                        s.m_LogFont=tp.m_lf;
#ifdef _JAPANESE
                        s.m_LogFont.lfCharSet = SHIFTJIS_CHARSET;
#else
                        s.m_LogFont.lfCharSet = ANSI_CHARSET;
#endif
                        ctx.ConvertPhFont(ps->scripta[n].scrpFont,ps->scripta[n].scrpSize,ps->scripta[n].scrpFace,s.m_LogFont);
                        //s.m_LogFont.lfCharSet= ps->scripta[n].scrpFace;
                        int iR = ps->scripta[n].scrpColor.red >> 8;
                        int iG = ps->scripta[n].scrpColor.green >> 8;
                        int iB = ps->scripta[n].scrpColor.blue >> 8;
                        s.m_TextColor=RGB(iR,iG,iB);
                        //if(++n<ps2->scrpNStyles)
                        //{
                        //   p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
                        //}
                        n++;
                     }
                     if(ps->chars[i] == 0xD) ps->chars.SetAt(i, 0xA);
                     st.Add(ps->chars[i],s);
                  }
                  ((CMDText*)pNewShape)->SetText(st);
               }
               break;
               case ::formatMD60::IDBitmap:
               {
                  PhMDBitmap* ps=(PhMDBitmap*)pshape;

                  //p.swaps(3); // fm
                  //p.swapDBPoint(4);
                  //p.swapl(2); p.swaps(1);
                  //count-=56;
                  memory mem;

                  ::formatMD60::serialize serialize;

                  serialize.m_bWrite = false;

                  //if(serialize.m_pointer.read(mem, file, pBitmap


                  //char* pBuf=new char[pBitmapIO[ps->fMapHdlIndex].fBitmapLength - 4];
                  //LONG lSavedPos=fi.Seek(pBitmapIO[ps->fMapHdlIndex].fBitmapOffset + 4,CFile::begin);
                  //fi.Read(pBuf,pBitmapIO[ps->fMapHdlIndex].fBitmapLength - 4);
                  //fi.Seek(lSavedPos,CFile::begin);
                  HBITMAP hDib=NULL;
                  CRect rc(ps->cSrcRect.l,ps->cSrcRect.t,ps->cSrcRect.r,ps->cSrcRect.b);

                  int iHandleIndex = ps->cMapHdlIndex;

                  if(bitmapa[iHandleIndex].fColor)
                  {

                     ColorBitmap * pm = &bitmapa[ps->cMapHdlIndex].m_bitmap;
                     int iColorMap;

                     if(pm->pixelSize < 16)
                     {
                        iColorMap = (1 << pm->pixelSize);
                     }
                     else
                     {
                        iColorMap = 0;
                     }
                     BITMAPINFO * pinfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + sizeof(COLORREF) *iColorMap);
                     BITMAPINFOHEADER & bmih = pinfo->bmiHeader;
                     bmih.biSize=sizeof(BITMAPINFOHEADER);
                     bmih.biWidth=pm->bounds.r - pm->bounds.l;
                     bmih.biHeight=pm->bounds.b - pm->bounds.t;
                     bmih.biPlanes=1;
                     bmih.biBitCount=pm->pixelSize;
                     bmih.biCompression=BI_RGB;
                     bmih.biSizeImage=0;
                     bmih.biXPelsPerMeter=0;
                     bmih.biYPelsPerMeter=0;
                     bmih.biClrUsed=(bmih.biBitCount<16)?(1 << bmih.biBitCount):0;
                     bmih.biClrImportant=bmih.biClrUsed;
                     

                 
                     //LPVOID lpBits;
                     //hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0);

                     pm->rowBytes&=0x3FFF;
                     long nWidthBytes=(pm->rowBytes + 0x3)&(~0x3);

                     char* pData=(char *) pm->data.GetData();

                     if(bmih.biBitCount == 1)
                     {

                        unsigned char * lpBits;
                        hDib=CreateDIBSection(NULL,(LPBITMAPINFO)pinfo,DIB_RGB_COLORS,(void **)&lpBits,NULL,0);
                        unsigned char * lpSrc = (unsigned char *)pm->data.GetData();
                        int h = bmih.biHeight;
                        int w = (bmih.biWidth / 8 + 1) / 2 * 2;
                        for(int y = 0; y < h; y++)
                        {

                           memcpy(&lpBits[w * y],&lpSrc[w * (h - y - 1)],w);

                        }
                     }
                     else if(bmih.biBitCount == 8)
                     {
                        
                        int64_t * piSize = (int64_t *) pm->table.GetData();
                        int64_t iSize = *piSize;
                        COLORREF * pcr = (COLORREF *)&pinfo->bmiColors;
                        short * psh = (short *)&piSize[1];
                        int iCount= (pm->table.GetCount() - sizeof(int64_t)) / (sizeof(short) * 4);
                        for(int j=0; j< iCount; j++)
                        {
                           //pcr[j] = RGB(psh[0] / 256,psh[1] / 256, psh[2] / 256) | (255 << 24);
                           pcr[j] = ARGB(255,psh[1] >> 8,psh[2] >> 8,psh[2] >> 8);
                           psh+=4;
                        }
                        pinfo->bmiHeader.biClrImportant = 0;
                        pinfo->bmiHeader.biClrUsed = 0;
                        pinfo->bmiHeader.biBitCount = 32;
                        COLORREF * lpBits;
                        hDib=CreateDIBSection(NULL,(LPBITMAPINFO)pinfo,DIB_RGB_COLORS,(void **)&lpBits,NULL,0);
                        BYTE * lpSrc = (BYTE *)pm->data.GetData();
                        for(int y = 0; y < bmih.biHeight; y++)
                        {

                           //memcpy(&lpBits[bmih.biWidth * y],&lpSrc[bmih.biWidth * (bmih.biHeight - y - 1)],bmih.biWidth * sizeof(COLORREF));
                           unsigned char * pline = (unsigned char *)&lpSrc[bmih.biWidth * (bmih.biHeight - y - 1)];
                           COLORREF * plineDst = (COLORREF *)&lpBits[bmih.biWidth * y];
                           for(int x = 0; x < bmih.biWidth; x++)
                           {
                              *plineDst = pcr[*pline];
                                 //unsigned char ch = pline[0];
                                 //pline[0] = pline[2];
                                 //pline[2] = ch;
                                 //swap32(pline);
                                 plineDst++;
                              pline++;
                           }
                        }
                     }
                     else
                     {
                        COLORREF * lpBits;
                        hDib=CreateDIBSection(NULL,(LPBITMAPINFO)pinfo,DIB_RGB_COLORS,(void **)&lpBits,NULL,0);
                        COLORREF * lpSrc = (COLORREF *)pm->data.GetData();
                        for(int y = 0; y < bmih.biHeight; y++)
                        {

                           memcpy(&lpBits[bmih.biWidth * y],&lpSrc[bmih.biWidth * (bmih.biHeight - y - 1)],bmih.biWidth * sizeof(COLORREF));
                           unsigned char * pline = (unsigned char *)&lpBits[bmih.biWidth * y];
                           for(int x = 0; x < bmih.biWidth; x++)
                           {
                              //unsigned char ch = pline[0];
                              //pline[0] = pline[2];
                              //pline[2] = ch;
                              swap32(pline);
                              pline +=4;
                           }
                        }

                     }
                     free(pinfo);
                     //p.pc=pData + pm->theLong;
                     //ColorTable* ct=(ColorTable*)(p.pc);
                     //p.swapl(1); p.swaps(2);
                     //p.swaps((ct->ctSize + 1) << 2);
                     //p.pc=pData;
                     //for(int y=bmih.biHeight - 1; y >= 0; y--)
                     //{
                     //   switch(bmih.biBitCount)
                     //   {
                     //   case 16:
                     //      p.swaps(pm->rowBytes >> 1);
                     //      break;
                     //   case 24:
                     //      p.swapRGB(pm->rowBytes / 3);
                     //      break;
                     //   case 32:
                     //      p.swapl(pm->rowBytes >> 2);
                     //      break;
                     //   }
                     //   memcpy(((char*)lpBits) + y*nWidthBytes,pData,nWidthBytes);
                     //   pData+=pm->rowBytes;
                     //}

                     //if(bmih.biBitCount<16)
                     //{
                     //   LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,bmih.biClrUsed*sizeof(RGBQUAD)));
                     //   for(ushort c=0; c<bmih.biClrUsed; c++)
                     //   {
                     //      lpColors[c].rgbRed=ct->ctTable[c].rgb.red >> 8;
                     //      lpColors[c].rgbGreen=ct->ctTable[c].rgb.green >> 8;
                     //      lpColors[c].rgbBlue=ct->ctTable[c].rgb.blue >> 8;
                     //      lpColors[c].rgbReserved=0;
                     //   }
                     //   CWindowDC dcScreen(NULL);
                     //   CDC memDC;
                     //   memDC.CreateCompatibleDC(&dcScreen);
                     //   HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
                     //   SetDIBColorTable(memDC,0,bmih.biClrUsed,lpColors);
                     //   memDC.SelectObject(hOldBmp);
                     //   GlobalFree(GlobalHandle(lpColors));
                     //}
                     rc.OffsetRect(-pm->bounds.l,-pm->bounds.t);

                  }
                  else
                  {
                     NonColorBitmap * pm = &bitmapa[ps->cMapHdlIndex].m_ncbitmap;

                     struct
                     {
                        BITMAPINFOHEADER bmih;
                        COLORREF fColors[2];
                     } bmi;
                     bmi.bmih.biSize=sizeof(BITMAPINFOHEADER);
                     bmi.bmih.biWidth=pm->bounds.r - pm->bounds.l + 1;
                     bmi.bmih.biHeight=pm->bounds.b - pm->bounds.t + 1;
                     bmi.bmih.biPlanes=1;
                     bmi.bmih.biBitCount=1;
                     bmi.bmih.biCompression=BI_RGB;
                     bmi.bmih.biSizeImage=0;
                     bmi.bmih.biXPelsPerMeter=0;
                     bmi.bmih.biYPelsPerMeter=0;
                     bmi.bmih.biClrUsed=2;
                     bmi.bmih.biClrImportant=2;
                     bmi.fColors[0]=RGB(255,255,255);
                     bmi.fColors[1]=RGB(0,0,0);

                     LPVOID lpBits;
                     hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmi,DIB_RGB_COLORS,&lpBits,NULL,0);

                     //char* pData=pBuf + sizeof(BitMap);
                     //int rowBytes=(pm->rowBytes + 3)&(~3);
                     //for(int y=bmi.bmih.biHeight - 1; y >= 0; y--)
                     //{
                     //   memcpy(((char*)lpBits) + rowBytes*y,pData,pm->rowBytes);
                     //   pData+=pm->rowBytes;
                     //}
                     rc.OffsetRect(-pm->bounds.l,-pm->bounds.t);

                  }

                  LongRect lrc;
                  lrc.SetToBound(ps->data[0].fLongPt,ps->data[2].fLongPt);
                  pNewShape=new CMDBitmap(rc,lrc,hDib);

               }
               break;
               case ::formatMD60::IDGroup:
               {

                  PhMDGroup* pgroup=(PhMDGroup*)pshape;

#if DEV_DEBUG
                  if(pgroup->fRot != 180.0)
                  {
                     OutputDebugString("test");
                  }
#endif
                  CMDGroupShape* pGroup=new CMDGroupShape();
                  for(int i=0; i < pgroup->itemsCount; i++)
                  {
                     //data.s64();
                  }
                  //data.s64();
                  groups.SetAt(pshape->fPrev,pGroup);
                  pNewShape=pGroup;

#if DEV_DEBUG

                  if(ctx.groupa64Debug.Find(pshape->fPrev) == NULL)
                  {

                     ctx.groupa64Debug.Add(pshape->fPrev);

                  }

                  strFormat.Format("count=%I64d fPrev=%d ",pgroup->itemsCount,ctx.groupa64Debug.Find(pshape->fPrev));

                  strIds += strFormat;

#endif

               }
               break;
               case ::formatMD60::IDMetaPoly:
               {
                  PhMDMetaPoly* ps=(PhMDMetaPoly*)pshape;

                  CMDMetaPolyShape* pMeta=new CMDMetaPolyShape();
                  metas.SetAt(ps->fPrev,pMeta);
                  pNewShape=pMeta;

#if DEV_DEBUG

                  if(ctx.groupa64Debug.Find(ps->fPrev) == NULL)
                  {

                     ctx.groupa64Debug.Add(ps->fPrev);

                  }

                  strFormat.Format("count=%d fPrev=%d ",ps->shapes.GetCount(),ctx.groupa64Debug.Find(ps->fPrev));

                  strIds += strFormat;

#endif

               }
               break;

               case ::formatMD60::IDHLineDim:
               case ::formatMD60::IDVLineDim:
               case ::formatMD60::IDSLineDim:
               case ::formatMD60::IDPLineDim:
               {
                  PhMDLineDim* ps=(PhMDLineDim*)pshape;

//                  data.swap(ps);

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

                  fmt.sLabel = ps->dimLabel;

                  switch(eid)
                  {
                  case ::formatMD60::IDHLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kHoriz);
                     break;
                  case ::formatMD60::IDVLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kVert);
                     break;
                  case ::formatMD60::IDSLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kSlope,pshape->fRot);
                     break;
                  case ::formatMD60::IDPLineDim:
                     pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kPerp,pshape->fRot);
                     break;
                  }
                  pNewShape->SetShapePoint(0,ps->refPt1.fLongPt);
                  pNewShape->SetShapePoint(1,ps->refPt2.fLongPt);
                  pNewShape->SetShapePoint(2,ps->anchorPt.fLongPt);
                  pNewShape->SetShapePoint(3,ps->dimLineStart.fLongPt);
                  pNewShape->SetShapePoint(4,ps->dimLineEnd.fLongPt);
                  pNewShape->SetShapePoint(5,ps->extLineStart.fLongPt != ps->extLineEnd.fLongPt?ps->extLineEnd.fLongPt:pNewShape->GetShapePoint(4));
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
                  ctx.ConvertPhFont(ps->fShapeFont,ps->fShapeFontSize,0,tp.m_lf);
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
               case ::formatMD60::IDRRadialDim:
               case ::formatMD60::IDDRadialDim:
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
                  if(eid == ::formatMD60::IDDRadialDim) value>>=1;
                  pNewShape=new CMDRadialDim(std,fmt,6,6,eid == ::formatMD60::IDDRadialDim,ps->refPt1.fLongPt,value,ps->anchorPt.fLongPt);

                  CDocMapper mapper(this);

                  CMDDimStr* pDS=new CMDDimStr;
                  pDS->SetPenModel(penModel);
                  FillModel fm;
                  ctx.ConvertPhFillModel(ps->cFillModel,fm);
                  pDS->SetFillModel(fm);
                  TextParams tp;
                  ctx.ConvertPhFont(ps->fShapeFont,ps->fShapeFontSize,0,tp.m_lf);
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
               case ::formatMD60::IDCenterDim:
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
               case ::formatMD60::IDAngularDim:
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
                  ctx.ConvertPhFont(ps->fShapeFont,ps->fShapeFontSize,0,tp.m_lf);
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
               case ::formatMD60::IDDimPt2Pt:
               {
                  PhMDDim* ps=(PhMDDim*)pshape;

                  CMDDimLine* pDimLine=new CMDDimLine(ps->fP1.fLongPt,ps->fP2.fLongPt);
                  dims.SetAt(ps->fPrev,pDimLine);
                  pNewShape=pDimLine;
               }
               break;
               case ::formatMD60::IDDimString:
               {
                  PhMDDimStr* ps=(PhMDDimStr*)pshape;


                  areas.Lookup(ps->cObjHdl,pNewShape);
                  if(pNewShape) areas.RemoveKey(ps->cObjHdl);
                  else
                  {
                     CMDDimLine* pDimLine=NULL;
                     dims.Lookup(ps->cObjHdl,pDimLine);
                     if(pDimLine)
                     {
                        dims.RemoveKey(ps->cObjHdl);
                        pNewShape=new CMDDimStr(ps->fStatus&kPhDimStrAttached?true:false,pDimLine);
                        pDimLine->m_nTextDisplay=(ps->fStatus&kPhDimStrOnSlope)?((ps->fStatus&kPhDimStrAbove)?::formatMD60::kAbove : ::formatMD60::kAlongLine): ::formatMD60::kHorizontal;
                     }
                  }
                  if(pNewShape)
                  {
                     TextParams tp;
                     ctx.ConvertPhFont(ps->fShapeFont,ps->fShapeFontSize,0,tp.m_lf);
                     pNewShape->ModifyTextParams(tp,tpUseFont,NULL);
                     pNewShape->SetShapePoint(0,ps->cAnchorPt.fLongPt + ps->cStrOffset.fLongPt);
                     pNewShape->SetRotation(ps->fRot);
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
               case ::formatMD60::IDArea:
               {
                  PhMDArea * parea= (PhMDArea *)pshape;

                  CMDShape* pOwner=NULL;
                  shapes.Lookup(parea->cShapeHdl,pOwner);
                  if(pOwner)
                  {
                     //CMDArea* pArea=new CMDArea(pshape->fShapeFlags & kPhDimStrAttached?true:false,pOwner);
                     CMDArea* pArea=new CMDArea(parea->fStatus & kPhDimStrAttached?true:false,pOwner);
                     pNewShape = pArea;
                     //areas.SetAt(pshape->fPrev,pArea);
                     if(pArea)
                     {
                        TextParams tp;
                        ctx.ConvertPhFont(parea->fShapeFont,parea->fShapeFontSize,0,tp.m_lf);
                        pArea->ModifyTextParams(tp,tpUseFont,NULL);
                        //pNewShape->SetShapePoint(0,parea->c.fLongPt + parea->cPosOrOffset.fLongPt);
                        //pArea->SetShapePoint(0,pOwner->GetShapePoint(0) + parea->cPosOrOffset.fLongPt);
                        pArea->SetShapePoint(0,Middle(pOwner->GetExtents()));
                        pArea->SetRotation(parea->fRot);
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

               case ::formatMD60::IDParallelLine:
               {

                  PhMDParallelLine * pline = (PhMDParallelLine *)pshape;

                  ParallelAttrs attrs;

                  ctx.ConvertPhParaAttr(pline->fParallelAttr,attrs);

                  pNewShape=new CMDParallelLine(pline->data[0].fLongPt,pline->data[3].fLongPt,attrs);

               }
               break;

               case ::formatMD60::IDParallelPolygon:
               {

                  PhMDParallelPolygon * ppolygon = (PhMDParallelPolygon *)pshape;

                  ParallelAttrs attrs;

                  ctx.ConvertPhParaAttr(ppolygon->fParallelAttr,attrs);

                  pNewShape=new CMDParallelPolygon(ppolygon->vertexCount,ppolygon->cClosed?true:false,attrs);

                  for(int i = 0; i < ppolygon->vertexCount; i++)
                  {

                     pNewShape->SetShapePoint(i,ppolygon->data[i].fLongPt);

                  }

               }
               break;

               default:;
               }

//               if(count > 0) p.skip(count);
               //             else ASSERT(count == 0);
               int count;
               if(pshape->fObjectInfo)
               {
                  if(pNewShape)
                  {

                     CStringArray sValues;

                     serialize.serialize_array(sValues,5);

                     pNewShape->SetValues(sValues.GetData());

                  }
                  //count=data.swaps();
                  //if(pNewShape)
                  //{
                  //   PhObjectInfo* pInfo=(PhObjectInfo*)p.pc;
                  //   p.swaps(2);
                  //   count-=4;

                  //   CString sValues[5];
                  //   for(int i=0; i < 5; i++)
                  //   {
                  //      int len=*p.pc++;
                  //      count--;
                  //      sValues[i]=p.pc;
                  //      p.skip(++len);
                  //      count-=len;
                  //   }
                  //   pNewShape->SetValues(sValues);
                  //}
                  //p.skip(count);
               }

               CArray < PhPropListElem > propa;

               propa.RemoveAll();
               if(pshape->fObjectProps == 0 || pshape->fObjectProps == 1)
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
                           serialize(a.fReserved);
                           serialize(a.fDataLength);
                           //serialize(a.fReserved2);
                           //if(a.fDataLength != 0.0)
                           {
                              serialize(a.ptrLength);
                              serialize(a.data,a.ptrLength);
                           }
                        }
                        propa.Add(a);
                     }
                  }
               }

               if(pNewShape)
               {
                  if(pNewShape->IsKindOf(RUNTIME_CLASS(CMDFillShape)))
                  {
                     pNewShape->SetFillModel(fm);
                  }
                  int iPenEnd = pshape->fShapeFlags & 0xc00;
                  if(iPenEnd == 0x400) // round
                  {
                     penModel.makePenStyleRound();
                  }
                  pNewShape->SetPenModelDirect(penModel);
                  pNewShape->SetLocked(pshape->fShapeFlags&kPhObjLockedMask?true:false);
                  pNewShape->SetRotation(pshape->fRot);
                  ////pNewShape->SetShapeDatum(pshape->fDatumType);
                  pNewShape->SetShapeDatum(pshape->fDatum);
                  if(!pNewShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape))) pNewShape->CalculateExtents();
                  shapes.SetAt(pshape->fPrev,pNewShape);
                  if(pshape->fOwnerGroup)
                  {
                     if(pshape->fShapeFlags&kPhMetaPolyMemberMask)
                     {
                        if(!pNewShape->IsClosed())
                        {
                           CMDShapeList* pList;
                           if(!metaMembers.Lookup(pshape->fOwnerGroup,pList))
                           {
                              pList=new CMDShapeList;
                              metaMembers.SetAt(pshape->fOwnerGroup,pList);
                           }
                           pList->AddTail(pNewShape);
                        }
                     }
                     else
                     {
                        CMDShapeList* pList;
                        if(groupMembers.Lookup(pshape->fOwnerGroup,pList))
                        {
                           pList->AddTail(pNewShape);
                        }
                        else
                        {
                           CMDGroupShape * pgroup = NULL;
                           if(groups.Lookup(pshape->fOwnerGroup,pgroup) && pgroup != NULL)
                           {
                              pgroup->AddMember(pNewShape);
                              pNewShape = NULL;
                           }
                           else
                           {
                              pList=new CMDShapeList;
                              groupMembers.SetAt(pshape->fOwnerGroup,pList);
                              pList->AddTail(pNewShape);
                           }
                        }
                           
                     }
                  }

#if DEV_DEBUG

                  strFormat.Format("%d\n",eid);

                  strIds += strFormat;

#endif

                  //pLayer->m_ShapeList.AddHead(pNewShape);
                  if(pNewShape != NULL)
                  {
                     a[iPos] = pNewShape;
                     iPos--;
                     
                  }

#ifndef _PERSONAL_EDITION
                  linkVertexList.ResolveObjectIndex(nObjectIndex++,pNewShape);
#endif


               }

            }



            shapes.RemoveAll();

            if(iPos >= 0)
            {

               a.CMDShapeArrayBase::RemoveAt(0,iPos + 1);

            }

            {

               int iCount = texta.GetCount();

               for(int iText = 0; iText < iCount; iText++)
               {

                  if(a.Find(texta[iText]) == NULL)
                  {

                     OutputDebugString("text not found");

                  }

               }

               pLayer->m_ShapeList.Append(a);

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

            POSITION pos=metas.GetStartPosition();
            while(pos)
            {
               UInt64 key;
               CMDShapeList* pList;
               CMDMetaPolyShape* pMeta;
               metas.GetNextAssoc(pos,key,pMeta);
               if(metaMembers.Lookup(key,pList))
               {
                  bool bFound=true;
                  while(pList->GetCount() && bFound)
                  {
                     bFound=false;
                     POSITION spos=pList->GetHeadPosition();
                     while(spos)
                     {
                        POSITION rpos=spos;
                        CMDShape* pShape=pList->GetNext(spos);
                        if(pMeta->AddMember(pShape))
                        {
                           pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pShape));
                           pList->RemoveAt(rpos);
                           pLayer->m_LinkVertexList.DeleteAllByShape(pShape);
                           delete pShape;
                           bFound=true;
                        }
                     }
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
            metas.RemoveAll();
            metaMembers.RemoveAll();

            pos=groups.GetStartPosition();
            while(pos)
            {
               UInt64 key;
               CMDShapeList* pList;
               CMDGroupShape* pGroup;
               groups.GetNextAssoc(pos,key,pGroup);
               if(groupMembers.Lookup(key,pList))
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
                  groups.RemoveKey(key);
                  delete pGroup;
               }
            }
            pos=groups.GetStartPosition();
            while(pos)
            {
               UInt64 key;
               CMDGroupShape* pGroup;
               groups.GetNextAssoc(pos,key,pGroup);
               pGroup->CalculateExtents();
            }
            groups.RemoveAll();
            groupMembers.RemoveAll();

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
//   for(int i=0; i<ctx.embedDIBs.GetSize(); i++) ::DeleteObject((HBITMAP)ctx.embedDIBs[i]);
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
         phDocState.fActiveEnvirons.fTrunk.fHCurScroll 
          / 1.5,
         phDocState.fActiveEnvirons.fTrunk.fVCurScroll 
          / 1.5);

      UpdateAllViews(NULL,kZoomFactorChanged,(CObject*)&ptScroll);

   }

   return true;
}

namespace formatMD60
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
      case ::formatMD60::IDLine:
      case ::formatMD60::IDLine2:
         return new PhMDLine;
      case ::formatMD60::IDFreehand:
      case ::formatMD60::IDPolygon:

         return new   PhMDPolygon;
      case ::formatMD60::IDRectCenter:
      case ::formatMD60::IDRectDiagonal:
         return new PhMDRectangle;
      case ::formatMD60::IDRndRectangle:
         return new PhMDRndRectangle;
      case ::formatMD60::IDFreeSpline:
         return new PhMDFreeSpline;
      case ::formatMD60::IDPolySpline:
         return new PhMDPolySpline;
      case ::formatMD60::IDCircle:
         return new PhMDCircle;
         break;
      case ::formatMD60::IDArc:
      case ::formatMD60::ID3Arc:
         return new PhMDArc;
         break;
      case ::formatMD60::IDEArc:
         return new PhMDArc;

      case ::formatMD60::IDEllipse:
         return new PhMDEllipse;
      case ::formatMD60::IDBezier:
         return new PhMDSpecificBezier;
      case ::formatMD60::IDCubic:
         return new PhMDCubic;
      case ::formatMD60::IDText:
         return new PhMDText;

      case ::formatMD60::IDBitmap:
         return new PhMDBitmap;
      case ::formatMD60::IDGroup:
         return new PhMDGroup;
      case ::formatMD60::IDMetaPoly:
         return new PhMDMetaPoly;

      case ::formatMD60::IDHLineDim:
      case ::formatMD60::IDVLineDim:
      case ::formatMD60::IDSLineDim:
      case ::formatMD60::IDPLineDim:
         return new PhMDLineDim;
      case ::formatMD60::IDRRadialDim:
      case ::formatMD60::IDDRadialDim:
         return new PhMDLineDim;
      case ::formatMD60::IDCenterDim:
         return new PhMDCenterDim;
      case ::formatMD60::IDAngularDim:
         return new PhMDAngularDim;
      case ::formatMD60::IDDimPt2Pt:
         return  new PhMDDim;
      case ::formatMD60::IDDimString:
         return new PhMDDimStr;
      case ::formatMD60::IDArea:
         return new PhMDArea;

      case ::formatMD60::IDParallelLine:
         return new PhMDParallelLine;

      case ::formatMD60::IDParallelPolygon:
         return new PhMDParallelPolygon;

      case ::formatMD60::IDHatch:
         return new PhMDHatch;

      default:
         throw "error";

      };

      return NULL;

   }



} // namespace formatMD60




