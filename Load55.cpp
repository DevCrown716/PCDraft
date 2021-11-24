// Infinisys Ltd
#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "FilletContext.h"
#include "ChamferContext.h"

#include "Layer.h"
#include "Import.h"
#include "PhIOStruct_55.h"
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


const Coord kAlmostSameCriteria = 400.0 / 65536.0;
using namespace format55;

namespace format55
{

   extern unsigned long reportRowType[3];

} // namespace format55

bool CPCDraftDoc::LoadPCDraft55(const char *szPath)
{
   CFile fi;
   if(!fi.Open(szPath,CFile::modeRead)) return false;;

   ::format55::PhIOContext ctx(&fi);
   ctx.pPalette=&m_Palette;
   ctx.pDoc=this;

   PhDocState phDocState;
   fi.Read(&phDocState,sizeof(PhDocState));
   uptr((char*)&phDocState).swapPhDocState();
   if(phDocState.fType != kPhDocType && phDocState.fType != kPhPadType) return false;
   if(phDocState.fHeaderID != kPhFileHdr || phDocState.fVersion != kPhRelease55) return false;

   MDFooter mdf;
   fi.Seek(-(int)sizeof(MDFooter),CFile::end);
   fi.Read(&mdf,sizeof(MDFooter));
   uptr((char*)&mdf).swapl(7);
   if(mdf.tag1 != 'RBAL' || mdf.tag2 != 'RPH ' || mdf.version != 0x10000 || mdf.type != 'RSRC') return false;

   fi.Seek(-mdf.offset,CFile::end);
   char *resFork=new char[mdf.numBytes];
   fi.Read(resFork,mdf.numBytes);
   swapMDR(resFork,mdf.numBytes);

   // used fonts
   long len;
   char* pFont=GetResource('FNUS',1,resFork,mdf.numBytes,&len);
   if(pFont)
   {
      uptr p(pFont);
      while(len > 0)
      {
         WORD font=p.swaps();
         unsigned char l = *(unsigned char *)(p.pc);
         CString str = CString((const char *)(p.pc + 1),l);
         int iFind = str.Find("|");
         if(iFind >= 0)
         {
            ctx.fontCharset[font] = atoi(str.Mid(iFind + 1));
            ctx.fontMap.SetAt(font,str.Left(iFind));
         }
         else
         {
            ctx.fontMap.SetAt(font,str);
         }
         p.pc += l + 1;
         len-=(3 + l);
         //p.p2c_nt();
      }
   }

   // copy doc data
   m_PageSize.cx=phDocState.fPrintRec.prInfo.rPage.r;
   m_PageSize.cy=phDocState.fPrintRec.prInfo.rPage.b;
   m_PrinterSettings.SetPageOrientation((phDocState.fPrintRec.prStl.wDev && !(phDocState.fPrintRec.prStl.wDev & 0x2))?DMORIENT_LANDSCAPE:DMORIENT_PORTRAIT);
   if (phDocState.fPrintRec.prStl.feed == 2
      && phDocState.fPrintRec.prStl.iPageV == 1320
      && phDocState.fPrintRec.prStl.iPageH == 1020)
   {

      m_PrinterSettings.SetCustomPageSize(MulDiv(phDocState.fPrintRec.rPaper.r - phDocState.fPrintRec.rPaper.l, 254, 72), MulDiv(phDocState.fPrintRec.rPaper.b - phDocState.fPrintRec.rPaper.t, 254, 72));
      
      if(!m_PrinterSettings.defer_set_page_size(MulDiv(phDocState.fPrintRec.rPaper.r - phDocState.fPrintRec.rPaper.l, 254, 72), MulDiv(phDocState.fPrintRec.rPaper.b - phDocState.fPrintRec.rPaper.t, 254, 72)))
      {
   
         m_PrinterSettings.defer_set_page_size(phDocState.fPrintRec.prInfo.rPage.r, phDocState.fPrintRec.prInfo.rPage.b);

      }

   }
   else if (phDocState.fPrintRec.prStl.feed == 0)
   {

      m_PrinterSettings.SetCustomPageSize(phDocState.fPrintRec.prStl.iPageH, phDocState.fPrintRec.prStl.iPageV);

   }
   else
   {

      m_PrinterSettings.SetPageSize(phDocState.fPrintRec.prStl.feed);

   }

   m_NumPage.x=phDocState.fMViewPages.x;
   m_NumPage.y=phDocState.fMViewPages.y;
   if((phDocState.fDocUnits.fDefUnits & kUEnglishMask) == 0/*!m_bEnglishUnits*/ && phDocState.fDocUnits.fDefScaleIndex>21) phDocState.fDocUnits.fDefScaleIndex--;
   SetupUnits(phDocState.fDocUnits.fDefUnits,false,phDocState.fDocUnits.fDefAngleUnits>1024?(1024 - phDocState.fDocUnits.fDefAngleUnits):(phDocState.fDocUnits.fDefAngleUnits >> 8),phDocState.fDocUnits.fDefPlaces,phDocState.fDocUnits.fDefAnglePlaces,phDocState.fDocUnits.fDefScaleIndex,NULL,0,&phDocState.fDocUnits.fDefScaleRatio);
   UpdateAllViews(NULL,kDocumentSizeChanged);
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

   ctx.ConvertPhFillModel(phDocState.fActiveEnvirons.fFillModel,m_Fill);
   ctx.ConvertPhPenModel(phDocState.fActiveEnvirons.fPenModel,m_Pen);
   //	m_TextParams.m_lf=phDocState.fActiveEnvirons.fTextParams.
   ctx.ConvertPhTextParams(phDocState.fActiveEnvirons.fTextParams,m_TextParams);
   ctx.ConvertPhParaAttr(phDocState.fActiveEnvirons.fParallelAttr,m_ParallelAttrs);

   //	fDimVisible
   m_bHideGridLines=phDocState.fActiveEnvirons.fHasGrid?false:true;
   m_bHideText=phDocState.fActiveEnvirons.fHideText?true:false;
   m_bShowRulers=phDocState.fActiveEnvirons.fShowRulers?true:false;
   m_ptPrevZoomScroll.x=phDocState.fActiveEnvirons.fPrevZoomScroll.x;
   m_ptPrevZoomScroll.y=phDocState.fActiveEnvirons.fPrevZoomScroll.y;

   m_bSnapToObject=phDocState.fActiveEnvirons.fSnapToObject?true:false;
   m_bStandardRulers=phDocState.fActiveEnvirons.fStandardRulers?true:false;
   m_bHidePageBreaks=phDocState.fActiveEnvirons.fHidePageBreaks?true:false;

   m_bPrintLastPageFirst=phDocState.fActiveEnvirons.fPrintFlags&kPhPrintLastFirst?true:false;
   m_nPageNumbering=phDocState.fActiveEnvirons.fPrintFlags&kPhPrintByCols?0:1;
   m_bRegistrationMarks=phDocState.fActiveEnvirons.fPrintFlags&kPhRegMarks?true:false;

#ifdef _FLOAT_ZOOM
   UnpackZoomFactor(phDocState.fActiveEnvirons.fPrevZoom,m_nPrevZoomFactor,m_fPrevZoomFactor);
   UnpackZoomFactor(phDocState.fZoom,m_nZoomFactor,m_fZoomFactor);
#else
   m_nPrevZoomFactor=phDocState.fActiveEnvirons.fPrevZoom;
   m_nZoomFactor=phDocState.fZoom;
#endif

   m_snapTable.setAngleSnaps((phDocState.fActiveEnvirons.fAngleSnapsToACirclePart1 << 8) | phDocState.fActiveEnvirons.fAngleSnapsToACirclePart2);
   //	m_nDisplayRes=phDocState.fActiveEnvirons.;
   //	m_bHideFills=phDocState.fActiveEnvirons.;

   CPCDraftView* pView=NULL;
   POSITION pos=GetFirstViewPosition();
   if(pos) pView=(CPCDraftView*)GetNextView(pos); // 1 view assumed
   if(pView)
   {
      pView->m_pFilletCtx->m_bSmallerArc=phDocState.fActiveEnvirons.fFilletAttr.fIsSmallerArc?true:false;
      pView->m_pFilletCtx->m_fRadius=phDocState.fActiveEnvirons.fFilletAttr.fRadius;
      pView->m_pFilletCtx->m_nTrimMethod=(kPhAutoTrim - phDocState.fActiveEnvirons.fFilletAttr.fTrimMethod);

      pView->m_pChamferCtx->m_fLine1SetBack=phDocState.fActiveEnvirons.fChamferAttr.fLine1SetBack;
      pView->m_pChamferCtx->m_fLine2SetBack=phDocState.fActiveEnvirons.fChamferAttr.fLine2SetBack;
      pView->m_pChamferCtx->m_nTrimMethod=(kPhAutoTrim - phDocState.fActiveEnvirons.fChamferAttr.fTrimMethod);
   }

   // layers
   PhFListHeader* pList=(PhFListHeader*)GetResource('LAYI',0,resFork,mdf.numBytes);
   if(!pList) return false;
   uptr p((char*)pList);
   p.swaps(1); p.swapl(1); p.swaps(1);
   p.swapl(3); p.swaps(3); p.swapl(1);

   // dash styles
   char* pDash=GetResource('DASH',128,resFork,mdf.numBytes);
   if(pDash)
   {
      memcpy(m_DashStyles,pDash,256);
      delete pDash;
   }

   // palette
   char* pPal=GetResource('pltt',128,resFork,mdf.numBytes);
   if(pPal)
   {
      m_Palette.DeleteObject();

      uptr p(pPal);
      int nCount=p.swaps();
      p.swapl(1); p.swaps(1); p.swapl(2);

      LPLOGPALETTE lp=(LPLOGPALETTE)new BYTE[sizeof(LOGPALETTE) + (nCount - 1)*sizeof(PALETTEENTRY)];
      lp->palVersion=0x300;
      lp->palNumEntries=nCount;
      for(int i=0; i<nCount; i++)
      {
         lp->palPalEntry[i].peRed=p.swaps() >> 8;
         lp->palPalEntry[i].peGreen=p.swaps() >> 8;
         lp->palPalEntry[i].peBlue=p.swaps() >> 8;
         lp->palPalEntry[i].peFlags=0;
         p.swaps(5);
      }

      m_Palette.CreatePalette(lp);
      delete[] lp;
      delete pPal;
   }

   // patterns
   char* pPatL=GetResource('PATL',128,resFork,mdf.numBytes);
   if(pPatL)
   {
      PhFListHeader* pList=(PhFListHeader*)pPatL;
      uptr p(pPatL);
      p.swaps(1); p.swapl(1); p.swaps(1); p.swapl(3);
      p.swaps(3); p.swapl(1);

      char* pPat=GetResource(pList->fDataType,pList->fIDData,resFork,mdf.numBytes);
      if(pPat)
      {
         PhFillDescript* pFill=(PhFillDescript*)pPat;
         uptr p(pPat);
         for(int i=0; i<pList->fNumElems; i++)
         {
            p.swaps(2);
            HBITMAP hDib=NULL;
            if(pFill[i].fColor)
            {
               p.skip(8);
               if(pFill[i].fResID == -1) continue;
               ASSERT(pFill[i].fResID);
               char* pPix=GetResource('ppat',pFill[i].fResID,resFork,mdf.numBytes);
               if(pPix)
               {
                  PixPatMap* ppm=(PixPatMap*)pPix;
                  uptr p(pPix);
                  p.swaps(1); p.swapl(3);
                  p.swaps(1); p.swapl(1); p.skip(8);

                  p.swapl(1); p.swaps(7); p.swapl(3);
                  p.swaps(4); p.swapl(3);

                  BITMAPINFOHEADER bmih;
                  bmih.biSize=sizeof(BITMAPINFOHEADER);
                  bmih.biWidth=ppm->pm.bounds.r;
                  bmih.biHeight=ppm->pm.bounds.b;
                  bmih.biPlanes=1;
                  bmih.biBitCount=32;
                  bmih.biCompression=BI_RGB;
                  bmih.biSizeImage=0;
                  bmih.biXPelsPerMeter=0;
                  bmih.biYPelsPerMeter=0;
                  bmih.biClrUsed=0;
                  bmih.biClrImportant=0;

                  LPVOID lpBits;
                  hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0);

                  char* pData=pPix + ppm->pp.patData;
                  switch(ppm->pm.pixelType)
                  {
                  case 0:
                  {
                     p.pc=pPix + ppm->pm.pmHandle;
                     ColorTable* ct=(ColorTable*)(p.pc);
                     p.swapl(1); p.swaps(2);
                     p.swaps((ct->ctSize + 1) << 2);
                     short n,s,m,c;
                     for(int y=bmih.biHeight - 1; y >= 0; y--)
                     {
                        for(int x=0; x<bmih.biWidth; x++)
                        {
                           switch(ppm->pm.pixelSize)
                           {
                           case 1:
                              n=x / 8;
                              s=7 - x % 8;
                              m=0x1;
                              break;
                           case 2:
                              n=x / 4;
                              s=(3 - x % 4) * 2;
                              m=0x3;
                              break;
                           case 4:
                              n=x / 2;
                              s=(1 - x % 2) * 4;
                              m=0xF;
                              break;
                           case 8:
                              n=x;
                              s=0;
                              m=0xFF;
                              break;
                           }
                           c=(pData[n] >> s)&m;
                           ((LPDWORD)lpBits)[y*bmih.biWidth + x]=RGB(ct->ctTable[c].rgb.blue,ct->ctTable[c].rgb.green,ct->ctTable[c].rgb.red);
                        }
                        pData+=ppm->pm.rowBytes & 0x3FFF;
                     }
                  }
                  break;
                  case 16:
                  {
                     /*								p.pc=pPix+ppm->pm.pmHandle;
                     ColorTable* ct=(ColorTable*)(p.pc);
                     p.swapl(1); p.swaps(2);
                     p.swaps((ct->ctSize+1)<<2);
                     */								for(int y=bmih.biHeight - 1; y >= 0; y--)
                     {
                        switch(ppm->pm.pixelSize)
                        {
                        case 32:
                        {
                           memcpy(((LPDWORD)lpBits) + y*bmih.biWidth,pData,ppm->pm.rowBytes & 0x3FFF);
                        }
                        break;
                        case 16:
                        {
                           for(int x=0; x<bmih.biWidth; x++)
                           {
                           }
                        }
                        break;
                        }
                        pData+=ppm->pm.rowBytes & 0x3FFF;
                     }
                     uptr((char*)lpBits).swapl(bmih.biWidth*bmih.biHeight);
                  }
                  break;
                  }

                  delete pPix;
               }
            }
            else
            {
               p.skip(8);
               struct
               {
                  BITMAPINFOHEADER bmih;
                  COLORREF colors[2];
               } bmi;
               bmi.bmih.biSize=sizeof(BITMAPINFOHEADER);
               bmi.bmih.biWidth=8;
               bmi.bmih.biHeight=8;
               bmi.bmih.biPlanes=1;
               bmi.bmih.biBitCount=1;
               bmi.bmih.biCompression=BI_RGB;
               bmi.bmih.biSizeImage=0;
               bmi.bmih.biXPelsPerMeter=0;
               bmi.bmih.biYPelsPerMeter=0;
               bmi.bmih.biClrUsed=0;
               bmi.bmih.biClrImportant=0;
               bmi.colors[0]=RGB(0,0,0);
               bmi.colors[1]=RGB(255,255,255);

               LPVOID lpBits;
               hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmi,DIB_RGB_COLORS,&lpBits,NULL,0);
               for(int j=0; j<8; j++)
               {
                  ((LPDWORD)lpBits)[j]=~pFill[i].fData.fPattern[j];
               }
            }
            if(hDib)
            {
               // First we need to check if there is such pattern already and only if
               // don't  - insert it into palette. First 71 entries - it's a system
               // palette, don't changed.
               if(i<72)
               {
                  m_Patterns[i - 1].FromDIBSection(hDib);
                  m_PatternsHash[i - 1] = CalculateDIBHash(hDib);
                  ::DeleteObject(hDib);
                  //						if(m_DIBPatterns[i-1]) DeleteObject(m_DIBPatterns[i-1]);
                  //						m_DIBPatterns[i-1]=hDib;
               }
               else
               {
                  // by TSM
                  int found = 0;
                  int hash = CalculateDIBHash(hDib);
                  for(int k=1; k <= m_numDIBPatterns; k++)
                  {
                     if(hash == m_PatternsHash[k - 1])
                     {
                        HBITMAP hDib2 = m_Patterns[k - 1].CreateDIBSection();
                        if(!CompareDIBs(hDib,hDib2))
                        {
                           ::DeleteObject(hDib2);
                           found = k; break;
                        }
                        ::DeleteObject(hDib2);
                     }
                  }
                  if(found)
                  {
                     ctx.embedDIBsIndex.Add((WORD)(found - 1));
                     ::DeleteObject(hDib);
                  }
                  else
                  {
                     ctx.embedDIBsIndex.Add((WORD)m_numDIBPatterns);
                     m_PatternsHash[m_numDIBPatterns] = CalculateDIBHash(hDib);
                     m_Patterns[m_numDIBPatterns++].FromDIBSection(hDib);
                     ::DeleteObject(hDib);
                  }
                  // end by TSM
                  //ctx.embedDIBs.Add((DWORD)hDib);
               }
            }
         }
         delete pPat;
      }
      delete pPatL;
   }


   // gradients
   {

      // Infinisys Ltd
      CMemFile mf;

      if(GetResource(mf,'GRAD',128,resFork,mdf.numBytes))
      {

         mf.Read(&m_numGradients,sizeof(m_numGradients));

         for(int i=0; i < m_numGradients; i++)
         {
            m_Gradients[i].Load55(mf);
         }

      }

   }

   // packed items
   {

      // Infinisys Ltd
      CMemFile mf;

      if(GetResource(mf,'PCKI',128,resFork,mdf.numBytes))
      {

         auto iSize = m_packeditema.GetCount();

         mf.Read(&iSize,sizeof(iSize));

         m_packeditema.SetSize(iSize);

         for(int i=0; i < iSize; i++)
         {
            mf.Read(&m_packeditema[i].m_etype,sizeof(m_packeditema[i].m_etype));
            int iIndex = 0;
            mf.Read(&iIndex,sizeof(iIndex));
            m_packeditema[i].set_index(this, iIndex);
         }

      }

   }


   // bitmaps
   PhBitmapIOStruct* pBitmapIO=NULL;
   char* pBitl=GetResource('BITL',0,resFork,mdf.numBytes);
   if(pBitl)
   {
      PhFListHeader* pList=(PhFListHeader*)pBitl;
      uptr p(pBitl);
      p.swaps(1); p.swapl(1); p.swaps(1); p.swapl(3);
      p.swaps(3); p.swapl(1);

      pBitmapIO=(PhBitmapIOStruct*)p.pc;
      for(int i=0; i<pList->fNumElems; i++)
      {
         p.swapl(2); p.swaps(1); p.skip(2);
      }
   }

   // saved views
   char* pViews=GetResource('VIEW',1,resFork,mdf.numBytes);
   if(pViews)
   {
      PhFListHeader* pList=(PhFListHeader*)pViews;
      uptr p(pViews);
      p.swaps(1); p.swapl(1); p.swaps(1); p.swapl(3);
      p.swaps(3); p.swapl(1);

      PhSavedView* pSavedView=(PhSavedView*)p.pc;
      for(int i=0; i<pList->fNumElems; i++)
      {
         p.p2c(kMaxViewName); p.skip(2); p.swapl(1); p.swaps(1);

         CSavedView* pView=new CSavedView;
         pView->m_sTitle=pSavedView[i].fViewName;
         pView->m_ptOrg.x=pSavedView[i].fOldScroll.x;
         pView->m_ptOrg.y=pSavedView[i].fOldScroll.y;
         pView->m_nZoom=pSavedView[i].fZoomFactor;
         pView->m_nShortcut=pSavedView[i].fCmdKey?(pSavedView[i].fCmdKey - '4'):0;
         m_SavedViews.Add(pView);
      }
      delete pViews;
   }

   // reports
   int r=1;
   char* pRep;
   while((pRep=GetResource('mdrp',r,resFork,mdf.numBytes,&len)) != NULL)
   {
      PhReport* pr=(PhReport*)pRep;
      uptr p(pRep);
      p.swaps(2); p.swapl(1); p.p2c(64);
      p.swapl(1); p.swaps(1);
      p.swaps(2); p.skip(2); p.swaps(1); p.skip(2); p.swaps(3); // tp
      p.swaps(13); p.p2c(32); p.p2c(32); p.p2c(32); p.p2c(32);
      p.swaps(1); p.swapl(1); p.swaps(4);
      p.skip(2); p.swapl(1); p.swaps(1); p.swapl(1);
      p.swapl(1); p.swaps(1); p.swapl(1); p.skip(64);
      p.swaps(2); p.skip(32);
      //		p.swapl(1); p.swaps(2); p.swapl(2); p.swaps(2); p.swapl(1);
      //		p.swapl(2);

      CMDReport* pReport=new CMDReport(false);
      pReport->m_sTitle=pr->cfTitle;
      for(int i=0; i<6; i++) pReport->m_nColumnWidths[i]=pr->cfColumnWidths[i];
      TextParams tp;
      ctx.ConvertPhTextParams(pr->cDefaultTextParams,tp);
      pReport->m_LogFont=tp.m_lf;

      char* pRowList=GetResource('mdrl',r,resFork,mdf.numBytes,&len);
      if(pRowList)
      {
         short blockCount[3]= {0,0,0};
         short *prd[3]= {NULL,NULL,NULL};
         uptr pu[3];

         PhRowInfo* pri=(PhRowInfo*)pRowList;
         p.pc=pRowList;
         for(int i=0; i<pr->cfNumRows; i++)
         {
            p.skip(2); p.swaps(1); p.swapl(1);

            CMDReportRow* pReportRow=new CMDReportRow;
            if(pri[i].fRowData && pri[i].fRowType)
            {
               short typeIdx=pri[i].fRowType - 1;
               if(prd[typeIdx] == NULL || (*prd[typeIdx]) == 0)
               {
                  if(prd[typeIdx]) delete prd[typeIdx];
                  prd[typeIdx]=(short*)GetResource(::format55::reportRowType[typeIdx],blockCount[typeIdx] + r,resFork,mdf.numBytes,&len);
                  pu[typeIdx].ps=prd[typeIdx];

                  pu[typeIdx].swaps(1);

                  blockCount[typeIdx]+=100; // kMaxReports
               }
               (*prd[typeIdx])--;

               short count=pu[typeIdx].swaps();
               switch(pri[i].fRowType)
               {
               case kPhTextRow:
               {
                  pReportRow->m_nRowType=kTextRow;
                  PhTextRowData* ptrd=(PhTextRowData*)pu[typeIdx].pc;
                  uptr p((char*)ptrd);
                  p.swaps(1); p.skip(2);
                  p.skip(6); p.swaps(1);// p.skip(12);
                  for(int i=0; i<6; i++)
                  {
                     pReportRow->m_nJust[i]=ptrd->fJust[i];
                     pReportRow->m_sRowText[i]=p.p2c();
                     p.skip(1);
                  }
               }
               break;
               case kPhCriteriaRow:
               {
                  pReportRow->m_nRowType=kCriteriaRow;
                  PhCriteriaRowData* pcrd=(PhCriteriaRowData*)pu[typeIdx].pc;
                  uptr p((char*)pcrd);
                  p.swaps(1); p.skip(2);
                  for(int i=0; i<6; i++) pReportRow->m_nFieldRefNum[i]=p.swaps();
               }
               break;
               case kPhFunctionRow:
               {
                  pReportRow->m_nRowType=kFunctionRow;
                  PhFunctRowData* pfrd=(PhFunctRowData*)pu[typeIdx].pc;
                  uptr p((char*)pfrd);
                  p.swaps(1); p.skip(2);
                  p.skip(6); p.skip(8); p.swaps(3);
                  pReportRow->m_nFunction=pfrd->fSpare;
                  if(pReportRow->m_nFunction>kFunctTotalName) pReportRow->m_nFunction--;
                  for(int i=0; i<6; i++)
                  {
                     pReportRow->m_nJust[i]=pfrd->fJust[i];
                     pReportRow->m_sRowText[i]=p.p2c();
                     p.skip(1);
                  }
               }
               break;
               }
               pu[typeIdx].skip(count);
            }
            pReport->m_Rows.Add(pReportRow);
         }

         if(prd[0]) delete prd[0];
         if(prd[1]) delete prd[1];
         if(prd[2]) delete prd[2];
         delete pRowList;
      }

      m_Reports.Add(pReport);
      r++;
      delete pRep;
   }

   // linked vertexes
#ifndef _PERSONAL_EDITION
   long nObjectIndex=0;
   CLinkVertexList linkVertexList;
   if(!((CPCDraftApp*)AfxGetApp())->m_bRemoveLinks)
   {
      for(short resNo=256;; resNo++)
      {
         long len;
         PhLinkedPtPicData* plp=(PhLinkedPtPicData*)GetResource('Link',resNo,resFork,mdf.numBytes,&len);
         if(NULL == plp) break;

         uptr p((char*)plp);
         p.swapl(1); p.swaps(2); p.swapl(4);
         long count=(len - sizeof(PhComplexPtPicData)) / sizeof(PhLinkResInfo);
         p.swapl(3 * count);

         switch(plp->fComplexType)
         {
         case kPhComplexPtArcDimPoint:
         {
            ASSERT(count>1);
            CLinkVertex* pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=resNo;
            pLinkVertex->m_nShape=plp->fLinks[count - 2].fObjectIndex;
            pLinkVertex->m_nVertex=kCenterHandle;
            linkVertexList.AddTail(pLinkVertex);

            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=resNo;
            pLinkVertex->m_nShape=plp->fLinks[count - 1].fObjectIndex;
            pLinkVertex->m_nVertex=kCenterHandle;
            linkVertexList.AddTail(pLinkVertex);
         }
         break;				// temporarily
         case kPhComplexPtOnLinePoint:
            count-=3;
         case kPhComplexPtLinkPoint:
         {
            CLinkVertexList list;
            for(int i=0; i<count; i++)
            {
               if(!plp->fLinks[i].fFlags) // skip notify-only
               {
                  CLinkVertex* pLinkVertex=new CLinkVertex;
                  pLinkVertex->m_nKey=resNo;
                  pLinkVertex->m_nShape=plp->fLinks[i].fObjectIndex;
                  pLinkVertex->m_nVertex=plp->fLinks[i].fPointIndex;
                  list.AddTail(pLinkVertex);
               }
            }
            if(list.GetCount()>1) linkVertexList.AddTail(&list);
            else list.DeleteAll();
         }
         break;
         case kPhComplexPtPerpDimPoint:
         case kPhComplexPtAngularDimPoint:
         {
            ASSERT(count>3);
            CLinkVertex* pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=-resNo;
            pLinkVertex->m_nShape=plp->fLinks[count - 1].fObjectIndex;
            pLinkVertex->m_nVertex=plp->fLinks[count - 1].fPointIndex;
            linkVertexList.AddTail(pLinkVertex);

            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=-resNo;
            pLinkVertex->m_nShape=plp->fLinks[count - 2].fObjectIndex;
            pLinkVertex->m_nVertex=plp->fLinks[count - 2].fPointIndex;
            linkVertexList.AddTail(pLinkVertex);

            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=-resNo;
            pLinkVertex->m_nShape=plp->fLinks[count - 3].fObjectIndex;
            pLinkVertex->m_nVertex=plp->fLinks[count - 3].fPointIndex;
            linkVertexList.AddTail(pLinkVertex);
         }
         break;
         default:
            ASSERT(0);
            break;
         }
      }
   }
#endif

   // layers & shapes
   PhLayerStruct* pLayers=(PhLayerStruct*)(pList + 1);
   short nActiveLayer=0;
   int l = 0;
   for(; l<pList->fNumElems; l++)
   {
      p.skip(kPhMaxLayerNameLen);
      p.swaps(3); p.swapl(3);

      CLayer* pLayer;
      if(l<m_Layers.GetSize())
      {
         pLayer=GetLayer(l);
         pLayer->DeleteAllShapes();
      }
      else
      {
         pLayer=new CLayer;
         m_Layers.Add(pLayer);
      }

      pLayer->m_strName=pLayers[l].fLayerName;
      pLayer->m_strName = get_cp(get_uni(pLayer->m_strName,CP_ACP),CP_UTF8);  // Infinisys Ltd.
      if(!m_bEnglishUnits && pLayers[l].fScaleIndex>21) pLayers[l].fScaleIndex--;
      pLayer->m_nScale=pLayers[l].fScaleIndex;
      pLayer->m_nSnap=pLayers[l].fSnapIndex + 1;
      pLayer->m_bGrayed=(pLayers[l].fLayerFlags&kPhNotGrayed)?false:true;
      pLayer->m_bHidden=(pLayers[l].fLayerFlags&kPhVisible)?false:true;
      if(pLayers[l].fLayerFlags & kPhActive)
      {
         nActiveLayer=l;
         if(pLayer->m_bHidden)
         {
            m_bActiveLayerHidden=true;
            pLayer->m_bHidden=false;
         }
      }

      if(pLayers[l].fNumShapes)
      {
         CMap<long,long,CMDShape*,CMDShape*> shapes;
         CMap<long,long,CMDShape*,CMDShape*> areas;
         CMap<long,long,CMDDimLine*,CMDDimLine*> dims;
         CMap<long,long,CMDMetaPolyShape*,CMDMetaPolyShape*> metas;
         CMap<long,long,CMDShapeList*,CMDShapeList*> metaMembers;
         CMap<long,long,CMDGroupShape*,CMDGroupShape*> groups;
         CMap<long,long,CMDShapeList*,CMDShapeList*> groupMembers;

         // read shapes
         char* pShapes=new char[pLayers[l].fLayerLength];
         fi.Seek(pLayers[l].fLayerOffset,CFile::begin);
         fi.Read(pShapes,pLayers[l].fLayerLength);

         uptr p(pShapes);
         p.swapl(1);
         for(int n=0; n<pLayers[l].fNumShapes; n++)
         {
            CMDShape* pNewShape=NULL;
            short count=p.swaps();

            PhDSFillShape* pShape=(PhDSFillShape*)p.pc;
            p.swapDSShape();
            count-=42;

            PenModel penModel;
            ctx.ConvertPhPenModel(pShape->fPenModel,penModel);
            penModel.m_nAccurateWidth=pShape->fShapeFlags&kPhAccurateWidthFlag?1:0;
            penModel.m_nSquareEnds=pShape->fShapeFlags&kPhSquareEndsFlag?1:0;

            switch(pShape->fID)
            {
            case PhIDLine:
            {
               p.swaps(1);
               p.swapDBPoint(2);
               count-=22;

               PhDSLine* ps=(PhDSLine*)pShape;
               pNewShape=new CMDLineShape(ps->fP1.fLongPt,ps->fP2.fLongPt,ps->fConAngle);
            }
            break;
            case PhIDFreehand:
            case PhIDPolygon:
            {
               p.swaps(3); // fm
               p.skip(2); p.swaps(1);
               count-=10;

               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               pNewShape=new CMDPolygonShape(pShape->fID == PhIDFreehand?CMDPolygonShape::kFreehand:CMDPolygonShape::kPolygon,ps->fCount,ps->fClosed?true:false);
               for(int i=0; i<ps->fCount; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
                  pNewShape->SetShapePoint(i,ps->fPoint[i].fLongPt);
               }
            }
            break;
            case PhIDRectCenter:
            case PhIDRectDiagonal:
            {
               p.swaps(3); // fm
               p.swapDBPoint(4);
               count-=46;

               PhDSRectangle* ps=(PhDSRectangle*)pShape;
               pNewShape=new CMDRectangle(pShape->fID == PhIDRectCenter?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);
               pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
               pNewShape->SetShapePoint(1,ps->fPoint[1].fLongPt);
               pNewShape->SetShapePoint(2,ps->fPoint[2].fLongPt);
               pNewShape->SetShapePoint(3,ps->fPoint[3].fLongPt);
            }
            break;
            case PhIDRndRectangle:
            {
               p.swaps(3); // fm
               p.swapDBPoint(4);
               p.skip(2);
               p.swapDBPoint(1);
               count-=58;

               PhDSRndRectangle* ps=(PhDSRndRectangle*)pShape;
               CMDRndRectangle::CornerType type;
               switch(ps->fCornerType)
               {
               case kPhProportionalCorner:
                  type=CMDRndRectangle::kCornerProportional;
                  break;
               case kPhConstantCorner:
                  type=CMDRndRectangle::kCornerCircle;
                  break;
               case kPhEllipticalCorner:
                  type=CMDRndRectangle::kCornerElliptical;
                  break;
               }
               pNewShape=new CMDRndRectangle(type);
               LongPoint axisPt=Average(ps->fPoint[1].fLongPt,ps->fPoint[3].fLongPt);
               pNewShape->SetShapePoint(0,mtRotPoint(ps->fPoint[0].fLongPt,pShape->fRot,axisPt));
               pNewShape->SetShapePoint(1,mtRotPoint(ps->fPoint[1].fLongPt,pShape->fRot,axisPt));
               pNewShape->SetShapePoint(2,mtRotPoint(ps->fPoint[2].fLongPt,pShape->fRot,axisPt));
               pNewShape->SetShapePoint(3,mtRotPoint(ps->fPoint[3].fLongPt,pShape->fRot,axisPt));
               ((CMDRndRectangle*)pNewShape)->m_lptCornerDelta=ps->fDiagonals.fLongPt >> 1;
            }
            break;
            case PhIDFreeSpline:
            case PhIDPolySpline:
            {
               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               p.swaps(3); // fm
               p.skip(2); p.swaps(1);
               count-=10;

               int n=3 * ps->fCount + 1;
               if(!ps->fClosed)
               {
                  ps->fCount++;
                  if(ps->fID == PhIDPolySpline) ps->fCount++;
               }
               pNewShape=new CMDPolygonShape(pShape->fID == PhIDFreeSpline?CMDPolygonShape::kFreeSpline:CMDPolygonShape::kPolySpline,ps->fCount,ps->fClosed?true:false);
               for(int i=0; i<n; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
               }
               for(int i=0; i<ps->fCount; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
                  pNewShape->SetShapePoint(i,ps->fPoint[n + i].fLongPt);
               }

            }
            break;
            case PhIDCircle:
            {
               p.swaps(3); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(1);
               p.swapDBPoint(3);
               count-=52;

               PhDSCircle* ps=(PhDSCircle*)pShape;
               CMDCircle::Mode mode;
               switch(ps->fMethod)
               {
               case kPhCenterRadius:
                  mode=CMDCircle::kRadius;
                  break;
               case kPhDiameter:
                  mode=CMDCircle::kDiameter;
                  break;
               case kPh3Point:
                  mode=CMDCircle::kThreePoint;
                  break;
               }
               pNewShape=new CMDCircle(mode);
               switch(ps->fMethod)
               {
               case kPhCenterRadius:
                  pNewShape->SetShapePoint(0,ps->fCenter.fLongPt);
                  pNewShape->SetShapePoint(1,ps->fPoint[0].fLongPt);
                  break;
               case kPhDiameter:
                  pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,ps->fPoint[1].fLongPt);
                  break;
               case kPh3Point:
                  pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,ps->fPoint[1].fLongPt);
                  pNewShape->SetShapePoint(2,ps->fPoint[2].fLongPt);
                  break;
               }
            }
            break;
            case PhIDArc:
            case PhID3Arc:
            {
               p.swaps(3); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(3);
               p.swapDBPoint(3);
               count-=60;

               PhDSArc* ps=(PhDSArc*)pShape;
               int mode;
               ASSERT(ps->fMethod != kPhEllipse);
               switch(ps->fMethod)
               {
               case kPhCenterRadius:
                  mode=CMDArcShape::kRadius;
                  break;
               case kPh3Point:
                  mode=CMDArcShape::kThreePoint;
                  break;
               }
               pNewShape=new CMDArcShape(mode);
               switch(ps->fMethod)
               {
               case kPhCenterRadius:
                  pNewShape->SetShapePoint(0,ps->fCenter.fLongPt);
                  pNewShape->SetShapePoint(1,ps->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(2,ps->fPoint[1].fLongPt);
                  break;
               case kPh3Point:
                  pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
                  pNewShape->SetShapePoint(1,ps->fPoint[2].fLongPt);
                  pNewShape->SetShapePoint(2,ps->fPoint[1].fLongPt);
                  break;
               }
               ((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(ps->fStartAng - mtFixAngle(k90 - mtAngle(ps->fCenter.fLongPt,ps->fPoint[0].fLongPt)))>kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
            }
            break;
            case PhIDEArc:
            {
               p.swaps(3); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(4);
               p.swapDBPoint(3);
               count-=64;

               PhDSEArc* ps=(PhDSEArc*)pShape;

               ASSERT(ps->fMethod == kPhEllipse);
               pNewShape=new CMDArcShape(CMDArcShape::kElliptic);
               pNewShape->SetShapePoint(0,ps->fCenter.fLongPt);
               pNewShape->SetShapePoint(1,ps->fPoint[0].fLongPt);
               pNewShape->SetShapePoint(2,ps->fPoint[1].fLongPt);
               pNewShape->SetShapePoint(3,ps->fCenter.fLongPt + mtRotPoint(LongPoint(ps->fHRadius,ps->fVRadius),pShape->fRot));
               ps->fStartAng=mtFixAngle(90 - ps->fStartAng);
               ps->fEndAng=mtFixAngle(90 - ps->fEndAng);
               ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE; //(ps->fStartAng>ps->fEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
            }
            break;
            case PhIDEllipse:
            {
               p.swaps(3); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(2);
               count-=26;

               PhDSEllipse* ps=(PhDSEllipse*)pShape;
               pNewShape=new CMDEllipse(ps->fMethod == kPhCenterRadius?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);
               pNewShape->SetShapePoint(0,ps->fCenter.fLongPt + mtRotPoint(LongPoint(-ps->fHRadius,-ps->fVRadius),pShape->fRot));
               pNewShape->SetShapePoint(1,ps->fCenter.fLongPt + mtRotPoint(LongPoint(ps->fHRadius,-ps->fVRadius),pShape->fRot));
               pNewShape->SetShapePoint(2,ps->fCenter.fLongPt + mtRotPoint(LongPoint(ps->fHRadius,ps->fVRadius),pShape->fRot));
               pNewShape->SetShapePoint(3,ps->fCenter.fLongPt + mtRotPoint(LongPoint(-ps->fHRadius,ps->fVRadius),pShape->fRot));
            }
            break;
            case PhIDBezier:
            {
               p.swaps(3); // fm
               p.skip(2); p.swaps(1);
               count-=10;

               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               ps->fCount=ps->fCount * 3 + 1;
               pNewShape=new CMDBezierShape(ps->fCount);
               for(int i=0; i<ps->fCount; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
                  pNewShape->SetShapePoint(i,ps->fPoint[i].fLongPt);
               }
            }
            break;
            case PhIDCubic:
            {
               p.swaps(3); // fm
               p.skip(2); p.swaps(1);
               count-=10;

               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               if(ps->fClosed)
               {
                  p.swapDBPoint(3);
                  count-=30;
               }
               else ps->fCount++;
               pNewShape=new CMDCubicShape(ps->fCount,ps->fClosed?true:false);
               p.swapDBPoint(1);
               count-=10;
               pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
               for(int i=1; i<ps->fCount; i++)
               {
                  p.swapDBPoint(3);
                  count-=30;
                  pNewShape->SetShapePoint(i,ps->fPoint[i * 3].fLongPt);
               }
            }
            break;
            case PhIDText:
            {
               PhDSText* ps=(PhDSText*)pShape;
               p.swaps(3); // fm
               p.skip(2); p.swapl(2); p.swaps(1); p.skip(2); p.swaps(1);
               p.swapDBPoint(4);
               count-=62;

               TextParams tp;
               switch(ps->fTextJust)
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
               switch(ps->fTextSpace)
               {
               default:
               case -3:
                  tp.m_nSpacing=kSingleSpace;
                  break;
               case -2:
                  tp.m_nSpacing=k1_5Space;
                  break;
               case -1:
                  tp.m_nSpacing=kDoubleSpace;
                  break;
               }
               //						tp.m_nCase=0;

               pNewShape=new CMDText(ps->fParagraph?CMDText::kTextUserSize:CMDText::kTextAutoSize);
               pNewShape->SetShapePoint(0,ps->fPoints[0].fLongPt);
               pNewShape->SetShapePoint(1,ps->fPoints[1].fLongPt);
               pNewShape->SetShapePoint(2,ps->fPoints[2].fLongPt);
               pNewShape->SetShapePoint(3,ps->fPoints[3].fLongPt);
               pNewShape->SetTextParams(tp);

               if(pShape->fObjectInfo)
               {
                  short count=p.swaps();
                  if(pNewShape)
                  {
                     PhObjectInfo* pInfo=(PhObjectInfo*)p.pc;
                     p.swaps(2);
                     count-=4;

                     CString sValues[5];
                     for(int i=0; i<5; i++)
                     {
                        int len=*p.pc++;
                        count--;
                        sValues[i]=p.pc;
                        p.skip(++len);
                        count-=len;
                     }
                     pNewShape->SetValues(sValues);
                  }
                  p.skip(count);
                  pShape->fObjectInfo=0;
               }
               union
               {
                  BYTE cCharset[400];
                  DWORD dwProperty[100];
               };
               int nCount = 0;
               memset(cCharset,0,sizeof(cCharset));
               if(pShape->fRef)
               {
                  short count=p.swaps();
                  PhPropList* pl=(PhPropList*)p.pc;
                  p.swaps(1); p.swapl(1); p.swaps(1);
                  p.swapl(3); p.swaps(3); p.swapl(1);
                  count -= 30;

                  for(int i=0; i<pl->fNumElems; i++)
                  {
                     p.swaps(1); p.swapl(1);
                     if(pl->fData[i].fType >= 600 && pl->fData[i].fType<700)
                     {
                        nCount++;
                        dwProperty[pl->fData[i].fType - 600]=pl->fData[i].fData;
                     }
                     count -= 6;
                  }
                  ASSERT(count == 0);
                  nCount*=4;
                  pShape->fRef=0;
               }

               PhTextData* pt=(PhTextData*)p.pc;
               p.swapl(4); p.swaps(1); p.swapl(1);

               char* pText=p.pc;
               p.skip(pt->fTELength);

               StScrpRec* pStyle=(StScrpRec*)p.pc;
               p.swaps(1);
               p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);

               CStyledText st;
               CStyledTextStyle s;
               int n=0;

               HDC hDC=GetDC(NULL);
               int nLogPixelsY=GetDeviceCaps(hDC,LOGPIXELSY);
               ReleaseDC(NULL,hDC);
               for(int i=0; i<pt->fTELength; i++)
               {
                  if(n<pStyle->scrpNStyles && i == pStyle->scrpStyleTab[n].scrpStartChar)
                  {
                     s.m_LogFont=tp.m_lf;
                     ctx.ConvertPhFont(pStyle->scrpStyleTab[n].scrpFont,pStyle->scrpStyleTab[n].scrpSize,pStyle->scrpStyleTab[n].scrpFace,s.m_LogFont);
                     if(nCount)
                        s.m_LogFont.lfCharSet=cCharset[n<nCount? n : nCount - 1];
                     s.m_TextColor=RGB(pStyle->scrpStyleTab[n].scrpColor.red >> 8,pStyle->scrpStyleTab[n].scrpColor.green >> 8,pStyle->scrpStyleTab[n].scrpColor.blue >> 8);
                     if(++n<pStyle->scrpNStyles)
                     {
                        p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
                     }
                     
                     CString strFaceName(s.m_LogFont.lfFaceName);
                     
                     strFaceName.Trim();

                     if (strFaceName.IsEmpty())
                     {

                        if (s.m_LogFont.lfCharSet == SHIFTJIS_CHARSET)
                        {

                           strcpy(s.m_LogFont.lfFaceName, "MS Mincho");

                        }
                        else
                        {

                           strcpy(s.m_LogFont.lfFaceName, "Arial");

                        }

                     }
                  }
                  if(*pText == 0xD) *pText=0xA;
                  st.Add(*pText++,s);
               }
               ((CMDText*)pNewShape)->SetText(st);
            }
            break;
            case PhIDBitmap:
            {
               PhDSBitmap* ps=(PhDSBitmap*)pShape;

               p.swaps(3); // fm
               p.swapDBPoint(4);
               p.swapl(2); p.swaps(1);
               count-=56;

               char* pBuf=new char[pBitmapIO[ps->fMapHdlIndex].fBitmapLength - 4];
               auto lSavedPos=fi.Seek(pBitmapIO[ps->fMapHdlIndex].fBitmapOffset + 4,CFile::begin);
               fi.Read(pBuf,pBitmapIO[ps->fMapHdlIndex].fBitmapLength - 4);
               fi.Seek(lSavedPos,CFile::begin);
               HBITMAP hDib=NULL;

               if(pBitmapIO[ps->fMapHdlIndex].fColor)
               {
                  PixMap* pm=(PixMap*)pBuf;
                  uptr p(pBuf);
                  p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
                  p.swapl(3);	p.swaps(4); p.swapl(3);

                  BITMAPINFOHEADER bmih;
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

                  LPVOID lpBits;
                  hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0);

                  pm->rowBytes&=0x3FFF;
                  long nWidthBytes=(pm->rowBytes + 0x3)&(~0x3);

                  char* pData=pBuf + sizeof(PixMap);
                  p.pc=pData + pm->baseAddr;
                  ColorTable* ct=(ColorTable*)(p.pc);
                  p.swapl(1); p.swaps(2);
                  p.swaps((ct->ctSize + 1) << 2);
                  p.pc=pData;
                  for(int y=bmih.biHeight - 1; y >= 0; y--)
                  {
                     switch(bmih.biBitCount)
                     {
                     case 16:
                        p.swaps(pm->rowBytes >> 1);
                        break;
                     case 24:
                        p.swapRGB(pm->rowBytes / 3);
                        break;
                     case 32:
                        p.swapl(pm->rowBytes >> 2);
                        break;
                     }
                     memcpy(((char*)lpBits) + y*nWidthBytes,pData,nWidthBytes);
                     pData+=pm->rowBytes;
                  }

                  if(bmih.biBitCount<16)
                  {
                     LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,bmih.biClrUsed*sizeof(RGBQUAD)));
                     for(ushort c=0; c<bmih.biClrUsed; c++)
                     {
                        lpColors[c].rgbRed=ct->ctTable[c].rgb.red >> 8;
                        lpColors[c].rgbGreen=ct->ctTable[c].rgb.green >> 8;
                        lpColors[c].rgbBlue=ct->ctTable[c].rgb.blue >> 8;
                        lpColors[c].rgbReserved=0;
                     }
                     CWindowDC dcScreen(NULL);
                     CDC memDC;
                     memDC.CreateCompatibleDC(&dcScreen);
                     HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
                     SetDIBColorTable(memDC,0,bmih.biClrUsed,lpColors);
                     memDC.SelectObject(hOldBmp);
                     GlobalFree(GlobalHandle(lpColors));
                  }

                  CRect rc(ps->fSrcRect.l,ps->fSrcRect.t,ps->fSrcRect.r,ps->fSrcRect.b);
                  rc.OffsetRect(-pm->bounds.l,-pm->bounds.t);
                  LongRect lrc;
                  lrc.SetToBound(ps->fCorner[0].fLongPt,ps->fCorner[2].fLongPt);
                  pNewShape=new CMDBitmap(rc,lrc,hDib);
               }
               else
               {
                  BitMap* pm=(BitMap*)pBuf;
                  uptr p(pBuf);
                  p.swapl(1); p.swaps(1); p.swapl(2);

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

                  char* pData=pBuf + sizeof(BitMap);
                  int rowBytes=(pm->rowBytes + 3)&(~3);
                  for(int y=bmi.bmih.biHeight - 1; y >= 0; y--)
                  {
                     memcpy(((char*)lpBits) + rowBytes*y,pData,pm->rowBytes);
                     pData+=pm->rowBytes;
                  }

                  CRect rc(ps->fSrcRect.l,ps->fSrcRect.t,ps->fSrcRect.r,ps->fSrcRect.b);
                  rc.OffsetRect(-pm->bounds.l,-pm->bounds.t);
                  LongRect lrc;
                  lrc.SetToBound(ps->fCorner[0].fLongPt,ps->fCorner[2].fLongPt);
                  pNewShape=new CMDBitmap(rc,lrc,hDib);
               }

               delete[] pBuf;

            }
            break;
            case PhIDGroup:
            {
               p.swaps(1);
               count-=2;

               PhDSGroup* ps=(PhDSGroup*)pShape;

               CMDGroupShape* pGroup=new CMDGroupShape();
               for(int i=0; i<ps->fNumMembers; i++)
               {
                  p.swapl(1);
                  count-=4;
               }
               groups.SetAt(pShape->fPrev,pGroup);
               pNewShape=pGroup;
            }
            break;
            case PhIDMetaPoly:
            {
               PhDSMetaPoly* ps=(PhDSMetaPoly*)pShape;
               p.swaps(3); // fm
               p.swaplpt(1);
               p.skip(2);
               p.swaps(3);
               count-=22;

               CMDMetaPolyShape* pMeta=new CMDMetaPolyShape();
               for(int i=0; i<ps->fNumMembers; i++)
               {
                  p.swapl(1);
                  count-=4;
               }
               metas.SetAt(ps->fPrev,pMeta);
               pNewShape=pMeta;
            }
            break;

            case PhIDHLineDim:
            case PhIDVLineDim:
            case PhIDSLineDim:
            case PhIDPLineDim:
            {
               PhDSLineDim* ps=(PhDSLineDim*)pShape;
               p.swaps(3); // fm

               p.swaps(3);
               p.skip(12);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(13);
               count-=336;

               DimStandard std;
               ctx.ConvertPhDimStd(ps->standard,std);
               DimFormat fmt;
               ctx.ConvertPhDimFmt(ps->format,fmt);
               fmt.nRoundoff=(ps->fStatus >> 8) & 0xF;
               if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
               {
                  short cnt=p.swaps();
                  char* label=p.pc;
                  p.p2c(cnt);
                  fmt.sLabel=label;
               }

               switch(pShape->fID)
               {
               case PhIDHLineDim:
                  pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kHoriz);
                  break;
               case PhIDVLineDim:
                  pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kVert);
                  break;
               case PhIDSLineDim:
                  pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kSlope,pShape->fRot);
                  break;
               case PhIDPLineDim:
                  pNewShape=new CMDLineDim(std,fmt,penModel.m_nEndCap1,penModel.m_nEndCap2,CMDLineDim::kPerp,pShape->fRot);
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
               ctx.ConvertPhFillModel(pShape->fFillModel,fm);
               pDS->SetFillModel(fm);
               TextParams tp;
               ctx.ConvertPhFont(ps->fFont,ps->fFontSize,0,tp.m_lf);
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
            case PhIDRRadialDim:
            case PhIDDRadialDim:
            {
               PhDSLineDim* ps=(PhDSLineDim*)pShape;
               p.swaps(3); // fm

               p.swaps(3);
               p.skip(12);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(13);
               count-=336;

               DimStandard std;
               ctx.ConvertPhDimStd(ps->standard,std);
               DimFormat fmt;
               ctx.ConvertPhDimFmt(ps->format,fmt);
               fmt.nRoundoff=(ps->fStatus >> 8) & 0xF;
               if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
               {
                  short cnt=p.swaps();
                  char* label=p.pc;
                  p.p2c(cnt);
                  fmt.sLabel=label;
               }

               Coord value=Pythag(ps->dimLineEnd.fLongPt - ps->dimLineStart.fLongPt);
               if(pShape->fID == PhIDDRadialDim) value>>=1;
               pNewShape=new CMDRadialDim(std,fmt,6,6,pShape->fID == PhIDDRadialDim,ps->refPt1.fLongPt,value,ps->anchorPt.fLongPt);

               CDocMapper mapper(this);

               CMDDimStr* pDS=new CMDDimStr;
               pDS->SetPenModel(penModel);
               FillModel fm;
               ctx.ConvertPhFillModel(pShape->fFillModel,fm);
               pDS->SetFillModel(fm);
               TextParams tp;
               ctx.ConvertPhFont(ps->fFont,ps->fFontSize,0,tp.m_lf);
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
            case PhIDCenterDim:
            {
               PhDSCenterDim* ps=(PhDSCenterDim*)pShape;
               p.swaps(3); // fm

               p.swaps(3);
               p.skip(12);
               //						p.swapl(1);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(1);
               p.swapl(2);
               count-=224;

               DimStandard std;
               ctx.ConvertPhDimStd(ps->standard,std);
               DimFormat fmt;
               ctx.ConvertPhDimFmt(ps->format,fmt);
               if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
               {
                  short cnt=p.swaps();
                  char* label=p.pc;
                  p.p2c(cnt);
                  fmt.sLabel=label;
               }

               pNewShape=new CMDCenterDim(std,fmt,ps->centerPt.fLongPt,LongPoint(ps->hRad,ps->vRad));
            }
            break;
            case PhIDAngularDim:
            {
               PhDSAngularDim* ps=(PhDSAngularDim*)pShape;
               p.swaps(3); // fm

               p.swaps(3);
               p.skip(12);
               //						p.swapl(1);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(11);
               p.swapl(3); p.swaps(1); p.swapl(2); p.swaps(1);

               count-=340;

               DimStandard std;
               ctx.ConvertPhDimStd(ps->standard,std);
               DimFormat fmt;
               ctx.ConvertPhDimFmt(ps->format,fmt);
               if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
               {
                  short cnt=p.swaps();
                  char* label=p.pc;
                  p.p2c(cnt);
                  fmt.sLabel=label;
               }

               pNewShape=new CMDAngularDim(std,fmt,6,6,true);
               pNewShape->SetShapePoint(0,ps->refPt1.fLongPt);
               pNewShape->SetShapePoint(1,ps->refPt2.fLongPt);
               pNewShape->SetShapePoint(2,ps->anchorPt.fLongPt);
               pNewShape->SetShapePoint(12,ps->centerPt.fLongPt);

               CDocMapper mapper(this);

               CMDDimStr* pDS=new CMDDimStr;
               pDS->SetPenModel(penModel);
               FillModel fm;
               ctx.ConvertPhFillModel(pShape->fFillModel,fm);
               pDS->SetFillModel(fm);
               TextParams tp;
               ctx.ConvertPhFont(ps->fFont,ps->fFontSize,0,tp.m_lf);
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
            case PhIDDimPt2Pt:
            {
               PhDSDimShape* ps=(PhDSDimShape*)pShape;
               p.swaps(1);
               p.swapDBPoint(2);
               p.swaps(1);
               p.skip(12);
               p.swapl(1);
               count-=40;

               CMDDimLine* pDimLine=new CMDDimLine(ps->fP1.fLongPt,ps->fP2.fLongPt);
               dims.SetAt(ps->fPrev,pDimLine);
               pNewShape=pDimLine;
            }
            break;
            case PhIDDimString:
            {
               PhDSDimStr* ps=(PhDSDimStr*)pShape;
               p.swaps(3); // fm

               p.swaps(3);
               p.swapDBPoint(2);
               p.swapl(1);
               p.p2c(kDimStrSize);
               count-=60;

               areas.Lookup(ps->fObjHdl,pNewShape);
               if(pNewShape) areas.RemoveKey(ps->fObjHdl);
               else
               {
                  CMDDimLine* pDimLine=NULL;
                  dims.Lookup(ps->fObjHdl,pDimLine);
                  if(pDimLine)
                  {
                     dims.RemoveKey(ps->fObjHdl);
                     pNewShape=new CMDDimStr(ps->fStatus&kPhDimStrAttached?true:false,pDimLine);
                     pDimLine->m_nTextDisplay=(ps->fStatus&kPhDimStrOnSlope)?((ps->fStatus&kPhDimStrAbove)?kAbove:kAlongLine):kHorizontal;
                  }
               }
               if(pNewShape)
               {
                  TextParams tp;
                  ctx.ConvertPhFont(ps->fFont,ps->fFontSize,0,tp.m_lf);
                  pNewShape->ModifyTextParams(tp,tpUseFont,NULL);
                  pNewShape->SetShapePoint(0,ps->fAnchorPt.fLongPt + ps->fStrOffset.fLongPt);
                  pNewShape->SetRotation(ps->fRot);
                  {
                     CDC cdc;
                     cdc.CreateCompatibleDC(NULL);
                     CDocMapper mapper(this);
                     CDraftContext dc(&cdc,&mapper);
                     ((CMDDimStr*)pNewShape)->SetValues(&dc,ps->fString);
                  }
                  ((CMDDimStr*)pNewShape)->GetOwner()->Attach((CMDDimStr*)pNewShape);
               }
            }
            break;
            case PhIDArea:
            {
               PhDSArea* ps=(PhDSArea*)pShape;
               p.swaps(1);
               p.skip(12);
               p.swapl(2);
               count-=22;

               CMDShape* pOwner=NULL;
               shapes.Lookup(ps->fShapeHdl,pOwner);
               if(pOwner)
               {
                  CMDArea* pArea=new CMDArea(ps->fStatus&kPhDimStrAttached?true:false,pOwner);
                  areas.SetAt(ps->fPrev,pArea);
               }
            }
            break;

            case PhIDParallelLine:
            {
               p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
               p.swaps(1);
               p.swapDBPoint(6);
               count-=72;

               PhDSParallelLine* ps=(PhDSParallelLine*)pShape;
               ParallelAttrs attrs;
               ctx.ConvertPhParaAttr(ps->fParallelAttr,attrs);
               pNewShape=new CMDParallelLine(ps->fPoint[0].fLongPt,ps->fPoint[3].fLongPt,attrs);
            }
            break;
            case PhIDParallelPolygon:
            {
               p.swaps(3); // fm
               p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
               p.skip(2); p.swaps(1);
               count-=20;

               PhDSParallelPolygon* ps=(PhDSParallelPolygon*)pShape;
               ParallelAttrs attrs;
               ctx.ConvertPhParaAttr(ps->fParallelAttr,attrs);
               pNewShape=new CMDParallelPolygon(ps->fCount,ps->fClosed?true:false,attrs);
               int i = 0;
               for(; i<ps->fCount; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
                  pNewShape->SetShapePoint(i,ps->fPoint[i].fLongPt);
               }
               for(; i<ps->fCount * 3; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
               }
            }
            break;
            }
            if(count>0) p.skip(count);
            else ASSERT(count == 0);

            if(pShape->fObjectInfo)
            {
               count=p.swaps();
               if(pNewShape)
               {
                  PhObjectInfo* pInfo=(PhObjectInfo*)p.pc;
                  p.swaps(2);
                  count-=4;

                  CString sValues[5];
                  for(int i=0; i<5; i++)
                  {
                     int len=*p.pc++;
                     count--;
                     sValues[i]=p.pc;
                     p.skip(++len);
                     count-=len;
                  }
                  pNewShape->SetValues(sValues);
               }
               p.skip(count);
            }

            if(pShape->fRef)
            {
               count=p.swaps();
               PhPropList* pl=(PhPropList*)p.pc;
               p.skip(count);
            }

            if(pNewShape)
            {
               if(pNewShape->IsKindOf(RUNTIME_CLASS(CMDFillShape)))
               {
                  FillModel fm;
                  ctx.ConvertPhFillModel(pShape->fFillModel,fm);
                  pNewShape->SetFillModel(fm);
               }
               pNewShape->SetPenModelDirect(penModel);
               pNewShape->SetLocked(pShape->fShapeFlags&kPhObjLockedMask?true:false);
               pNewShape->SetRotation(pShape->fRot);
               //pNewShape->SetShapeDatum(pShape->fDatumType);
               pNewShape->SetShapeDatum(pShape->fDatum);
               if(!pNewShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape))) pNewShape->CalculateExtents();

               if(pShape->fOwnerGroup)
               {
                  if(pShape->fShapeFlags&kPhMetaPolyMemberMask)
                  {
                     if(!pNewShape->IsClosed())
                     {
                        CMDShapeList* pList;
                        if(!metaMembers.Lookup(pShape->fOwnerGroup,pList))
                        {
                           pList=new CMDShapeList;
                           metaMembers.SetAt(pShape->fOwnerGroup,pList);
                        }
                        pList->AddTail(pNewShape);
                     }
                  }
                  else
                  {
                     CMDShapeList* pList;
                     if(!groupMembers.Lookup(pShape->fOwnerGroup,pList))
                     {
                        pList=new CMDShapeList;
                        groupMembers.SetAt(pShape->fOwnerGroup,pList);
                     }
                     pList->AddTail(pNewShape);
                  }
               }
               pLayer->m_ShapeList.AddHead(pNewShape);
               shapes.SetAt(pShape->fPrev,pNewShape);

#ifndef _PERSONAL_EDITION
               linkVertexList.ResolveObjectIndex(nObjectIndex++,pNewShape);
#endif
            }
         }


#ifndef _PERSONAL_EDITION
         pLayer->m_LinkVertexList.TransferResolved(linkVertexList);
#endif
         shapes.RemoveAll();

         POSITION pos=metas.GetStartPosition();
         while(pos)
         {
            long key;
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
            long key;
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
            else
            {
               pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pGroup));
               groups.RemoveKey(key);
               delete pGroup;
            }
         }
         pos=groups.GetStartPosition();
         while(pos)
         {
            long key;
            CMDGroupShape* pGroup;
            groups.GetNextAssoc(pos,key,pGroup);
            pGroup->CalculateExtents();
         }
         groups.RemoveAll();
         groupMembers.RemoveAll();

         //			CheckDocSize(true);
         delete[] pShapes;
      }
   }
   while(l<m_Layers.GetSize())
   {
      delete m_Layers[l];
      m_Layers.RemoveAt(l);
   }

   {
      fi.Seek(pLayers[l - 1].fLayerOffset + pLayers[l - 1].fLayerLength,CFile::begin);
      PhObjectInfoFieldSet fields;
      fi.Read(&fields,sizeof(PhObjectInfoFieldSet));
      uptr p((char*)&fields);
      //p.p2c(32); p.p2c(32); p.p2c(32); p.p2c(32);
      m_sFieldNames[1]=fields.fF2Name;
      m_sFieldNames[2]=fields.fF3Name;
      m_sFieldNames[3]=fields.fF4Name;
      m_sFieldNames[4]=fields.fF5Name;
   }

   if(pFont) delete [] pFont;
   if(pBitl) delete pBitl;
   for(int i=0; i<ctx.embedDIBs.GetSize(); i++) ::DeleteObject((HBITMAP)ctx.embedDIBs[i]);
   delete[] resFork;

   //m_Palette.ResizePalette(83);	// strip extra entries
   //	SetActiveLayer(nActiveLayer);
   m_nActiveLayer=nActiveLayer;
   UpdateFrameCounts();
   UpdateMaxZoom();
   if(pView)
   {
      for(int i=0; i < GetLayersCount(); i++)
      {
         GetLayer(i)->UpdateDependentShapes(pView);
      }
   }
   UpdateAllViews(NULL,kRefresh);
   CPoint ptScroll(phDocState.fOldScroll.x,phDocState.fOldScroll.y);
   UpdateAllViews(NULL,kZoomFactorChanged,(CObject*)&ptScroll);
   return true;
}
