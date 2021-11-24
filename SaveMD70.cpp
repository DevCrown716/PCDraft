// MD7.0 - Infinisys Ltd
#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "FilletContext.h"
#include "ChamferContext.h"

#include "Layer.h"
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

#include "Import.h"
#include <share.h>
bool SavePNG(CArray<unsigned char> & data,Gdiplus::Bitmap * pbitmap);
using namespace formatMD70;

class CStdioFile2: public CStdioFile
{
public:

   CStdioFile2(FILE * f) :
      CStdioFile(f)
   {
      m_bCloseOnDelete = true;
   }

};

//CFile* CPCDraftDoc::GetFile(LPCTSTR lpszFileName,UINT nOpenFlags,
//      CFileException* pError, bool bUtf8Name)
//   {
//      if(bUtf8Name)
//      {
//         
//
//            FILE * f = _wfsopen(get_uni(lpszFileName),L"wb",_SH_DENYRW);
//
//            if(f == NULL)
//            {
//
//               return NULL;
//
//            }
//
//            CStdioFile2 * pfile = new CStdioFile2(f);
//
//            return pfile;
//      
//      }
//      else
//      {
//
//         return CDocument::GetFile(lpszFileName,nOpenFlags,pError);
//
//      }
//
//    
//   }


bool CPCDraftDoc::SaveMacDraft70(const char *szPath, bool bUtf8Name)
{
   
   CFileException fe;

   m_bUtf8Name = true;

   CFile* fo=GetFile(szPath,CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive,&fe);

   if(fo == NULL)
   {

      ReportSaveLoadException(szPath,&fe,TRUE,AFX_IDP_INVALID_FILENAME);

      return false;

   }

   TRY
   {
      CWaitCursor wait;

      ::formatMD70::PhIOContext ctx(fo);
      ctx.m_numDIBPatterns = m_numDIBPatterns;
      ctx.pPalette=&m_Palette;
      ctx.pDoc=this;

      //::formatMD70::MDFooter mdf;
      //mdf.numBytes=0;
      //mdf.tag1='RBAL';
      //mdf.tag2='RPH ';
      //mdf.version=0x10000;
      //mdf.type='RSRC';
      //mdf.size=sizeof(::formatMD70::MDFooter);

      PhDocState phDocState;
      //memset(&phDocState,0,sizeof(PhDocState));
      phDocState.fType=kPhDocType;
      phDocState.fHeaderID=kPhFileHdr;
      phDocState.fVersion=kPhRelease8;

      phDocState.fUpgradeVersionDS = 0x2bc;

      phDocState.fWindowRect.r=(short)m_PageSize.cx;
      phDocState.fWindowRect.b=(short)m_PageSize.cy;

      phDocState.fMViewPages.x=(short)m_NumPage.x;
      phDocState.fMViewPages.y=(short)m_NumPage.y;

      phDocState.fDocUnits.fDefUnits=m_nUnit;
      phDocState.fDocUnits.fDefPlaces=m_nPlaces;
      phDocState.fDocUnits.fDefAngleUnits=GetAngleUnit();
      phDocState.fDocUnits.fDefAngleUnits=phDocState.fDocUnits.fDefAngleUnits < 0?1024 - phDocState.fDocUnits.fDefAngleUnits:(phDocState.fDocUnits.fDefAngleUnits << 8);
      phDocState.fDocUnits.fDefAnglePlaces=m_nAnglePlaces;
      phDocState.fDocUnits.fDefResolution.x=m_nDisplayRes;
      phDocState.fDocUnits.fDefResolution.y=m_nDisplayRes;
      phDocState.fDocUnits.fDefScaleIndex=m_nDefScale;
      //      phDocState.fDocUnits.fDefScaleRatio=m_ratioCustom; todo custom ratio
      if(!m_bEnglishUnits && phDocState.fDocUnits.fDefScaleIndex > 20) phDocState.fDocUnits.fDefScaleIndex++;
      phDocState.fDocUnits.fDefSnapIndex=4;

      if(m_bEnglishUnits)
      {
         DimStandard std;
         std.FillDefaults(kDsMetric);
         ctx.ConvertDimStd(m_DimStd,phDocState.fDocStds.fEnglishDimStd);
         ctx.ConvertDimStd(std,phDocState.fDocStds.fMetricDimStd);
         for(int i=0; i < 3; i++)
         {
            DimFormat fmt;
            fmt.FillDefaults(i,kDsMetric);
            ctx.ConvertDimFmt(m_DimFmts[i],phDocState.fDocStds.fEnglishDimFormat[i]);
            ctx.ConvertDimFmt(fmt,phDocState.fDocStds.fMetricDimFormat[i]);
         }
      }
      else
      {
         DimStandard std;
         std.FillDefaults(kDsEnglish);
         ctx.ConvertDimStd(m_DimStd,phDocState.fDocStds.fMetricDimStd);
         ctx.ConvertDimStd(std,phDocState.fDocStds.fEnglishDimStd);
         for(int i=0; i < 3; i++)
         {
            DimFormat fmt;
            fmt.FillDefaults(i,kDsEnglish);
            ctx.ConvertDimFmt(m_DimFmts[i],phDocState.fDocStds.fMetricDimFormat[i]);
            ctx.ConvertDimFmt(fmt,phDocState.fDocStds.fEnglishDimFormat[i]);
         }
      }

      phDocState.fRulerOrigin=m_RulerOrigin;

      //		phDocState.fActiveEnvirons.fToolID=0;

      ctx.ConvertFillModel(m_Fill,phDocState.fActiveEnvirons.fFillModel);
      ctx.ConvertPenModel(m_Pen,phDocState.fActiveEnvirons.fPenModel);
      //		m_TextParams.m_lf=phDocState.fActiveEnvirons.fTextParams.
      ctx.ConvertTextParams(m_TextParams,phDocState.fActiveEnvirons.fTextParams);
      ctx.ConvertParaAttr(m_ParallelAttrs,phDocState.fActiveEnvirons.fParallelAttr);

      phDocState.fActiveEnvirons.fPrintFlags	= 0;
      phDocState.fActiveEnvirons.fAngleSnapsToACircle	= m_snapTable.getAngleSnaps();
      phDocState.fActiveEnvirons.fDimTextAlgn = 0;

      //		phDocState.fActiveEnvirons.fDimVisible=0;
      phDocState.fActiveEnvirons.fHasGrid=!m_bHideGridLines;
      phDocState.fActiveEnvirons.fHideText=m_bHideText;
      phDocState.fActiveEnvirons.fShowRulers=m_bShowRulers;
      phDocState.fActiveEnvirons.fShowGuides = m_bShowGuideLines;  // Infinisys Ltd. - PC Draft 7 - Guides
      phDocState.fActiveEnvirons.fPrevZoomScroll.x=m_ptPrevZoomScroll.x;
      phDocState.fActiveEnvirons.fPrevZoomScroll.y=m_ptPrevZoomScroll.y;
      //		phDocState.fActiveEnvirons.fRulerOrigin
      phDocState.fActiveEnvirons.fSnapToObject=m_bSnapToObject;
      phDocState.fActiveEnvirons.fStandardRulers=m_bStandardRulers;
      phDocState.fActiveEnvirons.fHidePageBreaks=m_bHidePageBreaks;
      //		phDocState.fActiveEnvirons.fPlotterMode=0;
      //		phDocState.fActiveEnvirons.fPrintFlags=0;
      //if(m_bPrintLastPageFirst) phDocState.fActiveEnvirons.fPrintFlags|=kPhPrintLastFirst;
      //if(m_nPageNumbering == 0) phDocState.fActiveEnvirons.fPrintFlags|=kPhPrintByCols;
      //if(m_bRegistrationMarks) phDocState.fActiveEnvirons.fPrintFlags|=kPhRegMarks;

      //		phDocState.fActiveEnvirons.fDimLineFlag
      //		phDocState.fActiveEnvirons.fDimTextAlgn;
#ifdef _FLOAT_ZOOM
      phDocState.fActiveEnvirons.fPrevZoom=(char)PackZoomFactor(m_nPrevZoomFactor,m_fPrevZoomFactor);
#else
      phDocState.fActiveEnvirons.fPrevZoom=m_nPrevZoomFactor;
#endif

      CPCDraftView* pView=NULL;
      POSITION pos=GetFirstViewPosition();
      if(pos) pView=(CPCDraftView*)GetNextView(pos); // 1 view assumed
      if(pView)
      {

         phDocState.fActiveEnvirons.screenViewRectOrigin.y = (short)pView->GetDeviceScrollPosition().x;
         phDocState.fActiveEnvirons.screenViewRectOrigin.x = (short)pView->GetDeviceScrollPosition().y;

         phDocState.fActiveEnvirons.fFilletAttr.fIsSmallerArc=pView->m_pFilletCtx->m_bSmallerArc;
         phDocState.fActiveEnvirons.fFilletAttr.fRadius=pView->m_pFilletCtx->m_fRadius;
         phDocState.fActiveEnvirons.fFilletAttr.fTrimMethod=(TrimMethod)(kNoTrim - pView->m_pFilletCtx->m_nTrimMethod);

         phDocState.fActiveEnvirons.fChamferAttr.fLine1SetBack=pView->m_pChamferCtx->m_fLine1SetBack;
         phDocState.fActiveEnvirons.fChamferAttr.fLine2SetBack=pView->m_pChamferCtx->m_fLine2SetBack;
         phDocState.fActiveEnvirons.fChamferAttr.fTrimMethod=(TrimMethod)(kNoTrim - pView->m_pChamferCtx->m_nTrimMethod);
      }
      
      phDocState.fActiveEnvirons.fSnapToGuides = m_bSnapToGuides;

      phDocState.fActiveEnvirons.fZoomFactor=PackDoubleZoomFactor(m_nZoomFactor,m_fZoomFactor);

      serialize serialize(fo);


      serialize(&phDocState);

      //fo->Write(&phDocState,sizeof(PhDocState));

      //CMemFile mf;
      //   PhFListHeader list;
      //   memset(&list,0,sizeof(PhFListHeader));
      //   list.fNumElems=m_Layers.GetSize();
      //   list.fElemSize=sizeof(PhLayerStruct);
      //   list.fGrowSize=sizeof(PhLayerStruct);
      //   list.fKeylen=kPhMaxLayerNameLen;

      //   uptr p((char*)&list);
      //   p.swaps(1); p.swapl(1); p.swaps(1);
      //   p.swapl(3); p.swaps(3); p.swapl(1);
      //   mf.Write(&list,sizeof(PhFListHeader));

      // linked vertexes
      ctx.nObjectIndex=0;

      memory mem;

      memory mem8;

      mem8.allocate(8);

      ::ZeroMemory(mem8.get_data(),mem8.get_size());

      CMemFile file1;

      int64_t i = 0;

      file1.Write(&i,sizeof(i));

      OffsetHeader header;

      fo->Seek(kMarkerDataStream,CFile::begin);

      //header.write(serialize.m_pointer, mem, fo,kMarkerGenericPattern,file1,0);
      //header.write(serialize.m_pointer, mem, fo,kMarkerLayerStruct,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerCDocObjectInfo,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerShapes,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerViews,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerAllLinks,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerReports,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerFlatPageFormat,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerFlatPrintSettings,file1,0);
      fo->Seek(kMarkerReserved_8kMarkStartStructs,CFile::begin);
      fo->Write(mem8.get_data(),mem8.get_size());
      //      header.write(serialize.m_pointer,mem,fo,kMarkerReserved_8kMarkStartStructs,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerQuickLookPreview,file1,0);
      fo->Seek(kMarkerQuickLookPreview,CFile::begin);
      fo->Write(mem8.get_data(),mem8.get_size());
      fo->Seek(kMarkerReserved_7kMarkStartStructs,CFile::begin);
      fo->Write(mem8.get_data(),mem8.get_size());
      //      header.write(serialize.m_pointer,mem,fo,kMarkerReserved_7kMarkStartStructs,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerLayersCustomScaleInfo,file1,0);
      fo->Seek(kMarkerLayersCustomScaleInfo,CFile::begin);
      fo->Write(mem8.get_data(),mem8.get_size());
      header.write(serialize.m_pointer,mem,fo,kMarkerGuidesInfo,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerColorSchemeInfo,file1,0);
      header.write(serialize.m_pointer,mem,fo,kMarkerDocVersionSave,file1,0);
      fo->Seek(kMarkerReserved_6,CFile::begin);
      fo->Write(mem8.get_data(),mem8.get_size());
      fo->Write(mem8.get_data(),mem8.get_size());
      fo->Write(mem8.get_data(),mem8.get_size());
      fo->Write(mem8.get_data(),mem8.get_size());
      fo->Write(mem8.get_data(),mem8.get_size());
      fo->Write(mem8.get_data(),mem8.get_size());
      //header.write(serialize.m_pointer,mem,fo,kMarkerReserved_6,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerReserved_5,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerReserved_4,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerReserved_3,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerReserved_2,file1,0);
      //header.write(serialize.m_pointer,mem,fo,kMarkerReserved_1,file1,0);

      {

         CMemFile fileAppVersion;

         int iAppVersion = 0x2bc;

         swap32(&iAppVersion);

         fileAppVersion.Write(&iAppVersion,sizeof(iAppVersion));

         header.write(serialize.m_pointer,mem,fo,kMarkerDocVersionSave,fileAppVersion);

      }

      {

         ColorSchemeInfo info;

         info.schemea[0].red = 1.f;
         info.schemea[0].green = 1.f;
         info.schemea[0].blue = 1.f;
         info.schemea[1].red = 0.f;
         info.schemea[1].green = 0.f;
         info.schemea[1].blue = 0.f;
         info.schemea[2].red = 1.f;
         info.schemea[2].green = 0.f;
         info.schemea[2].blue = 0.f;
         info.schemea[3].red = 1.f;
         info.schemea[3].green = 0.f;
         info.schemea[3].blue = 0.f;

         for(int i = 0; i < 4; i++)
         {
            swap32(&info.schemea[i].red);
            swap32(&info.schemea[i].green);
            swap32(&info.schemea[i].blue);
         }

         CMemFile fileColorSchemeInfo;

         fileColorSchemeInfo.Write(&info,sizeof(info));

         header.write(serialize.m_pointer,mem,fo,kMarkerColorSchemeInfo,fileColorSchemeInfo);

      }


      CLinkVertexList linkVertexList;
      ctx.pLinkVertexList=&linkVertexList;

      CPtrArray linkedVertexResPtr;
      CDWordArray linkedVertexResLen;

      // Page Format

      {

         int iPageA4W = 595;
      
         int iPageA4H = 842;

         //CSize thePageSize = CalcPrinterPageSize();

         CSize thePageSize = get_synthesized_printer_page_size();

         CSize thePageSizeUnscaled;

         thePageSizeUnscaled.cx = thePageSize.cx * m_PrinterSettings.GetPageScale() / 100;

         thePageSizeUnscaled.cy = thePageSize.cy * m_PrinterSettings.GetPageScale() / 100;

         CString str;

         {

            memory mem;

            mem.read_resource(GetModuleHandle(NULL),IDR_XML_PAPER,"XML");

            str = CString(mem.get_data(),mem.get_size());

            str.Replace("\r\n","\r");

         }

         int iPageSize = get_synthesized_printer_page_size_index();

         bool bCustomPaper = iPageSize == 0 || iPageSize == 256;

         {

            CString strCustomPaper;

            if(bCustomPaper)
            {

               memory mem;

               mem.read_resource(GetModuleHandle(NULL),IDR_XML_PAPER_CUSTOM,"XML");

               strCustomPaper = CString(mem.get_data(),mem.get_size());

               strCustomPaper.Replace("\r\n","\r");

            }

            str.Replace("%CUSTOMPAPER%",strCustomPaper);

         }

         XDoc	doc;

         if(doc.Load(str))
         {

            XNode * plist = doc.GetChild("plist");

            ASSERT(plist != NULL);

            XNode * pmain = plist->GetChild("dict");

            ASSERT(pmain != NULL);

            XNode * pnode = pmain->get_pinfo_node("com.apple.print.PageFormat.PMOrientation");

            bool bSwap;

            int iAdjW;

            int iAdjH;

            if(m_PrinterSettings.GetPageOrientation() == DMORIENT_LANDSCAPE)
            {

               pnode->value = "2";

               bSwap = true;

               iAdjW = iPageA4H;

               iAdjH = iPageA4W;

               auto cxSwap = thePageSizeUnscaled.cx;

               thePageSizeUnscaled.cx = thePageSizeUnscaled.cy;

               thePageSizeUnscaled.cy = cxSwap;

            }
            else
            {

               pnode->value = "1";

               bSwap = false;

               iAdjW = iPageA4W;

               iAdjH = iPageA4H;

            }

            CString strPaper;

            CString strTiogaPaper;

            if(!bCustomPaper)
            {

               switch(iPageSize)
               {
               case 0:
                  strPaper = "";
                  break;
               case DMPAPER_TABLOID_EXTRA:
                  strPaper = "archb";
                  break;
               case DMPAPER_LEGAL:
                  strPaper = "legal";
                  break;
               case DMPAPER_LETTER:
                  strPaper = "letter";
                  break;
               case DMPAPER_B5_EXTRA:
                  strPaper = "roc16k";
                  break;
               case DMPAPER_JENV_CHOU3:
                  strPaper = "envchou3";
                  break;
               case DMPAPER_B_PLUS:
                  strPaper = "13x19";
                  break;
               case DMPAPER_ENV_DL:
                  strPaper = "envdl";
                  break;
               case DMPAPER_B5:
                  strPaper = "b5";
                  break;
               case DMPAPER_ENV_B5:
                  strPaper = "isob5";
                  break;
               case DMPAPER_A5:
                  strPaper = "a5";
                  strTiogaPaper = "iso-a5";
                  break;
               case DMPAPER_A4:
                  strPaper = "a4";
                  strTiogaPaper = "iso-a4";
                  break;
               case DMPAPER_A3:
                  strPaper = "a3";
                  strTiogaPaper = "iso-a3";
                  break;
               case DMPAPER_ENV_10:
                  strPaper = "env10";
                  break;
               case DMPAPER_TABLOID:
                  strPaper = "Tabloid";
                  break;
               default:
                  strPaper = "Letter";
                  break;
               }

            }

            if(strTiogaPaper.IsEmpty())
            {

               strTiogaPaper = strPaper;

               strTiogaPaper.MakeLower();

            }

            int iFind = pmain->IndexByValue("key","com.apple.print.subTicket.paper_info_ticket");

            XNode * pticket = NULL;

            if(iFind >= 0)
            {

               pticket = pmain->GetChild(iFind + 1);

            }

            if(pticket != NULL)
            {

               XNode * pnode = pticket->get_pinfo_node("PMPPDPaperCodeName");

               if(pnode != NULL)
               {

                  pnode->value = strPaper;

               }

               pnode = pticket->get_pinfo_node("PMPPDTranslationStringPaperName");

               if(pnode != NULL)
               {

                  pnode->value = strPaper;

               }

               pnode = pticket->get_pinfo_node("PMTiogaPaperName");

               if(pnode != NULL)
               {

                  pnode->value = strTiogaPaper;

               }

               pnode = pticket->get_pinfo_node("com.apple.print.PaperInfo.PMPaperName");

               if(pnode != NULL)
               {

                  pnode->value = strTiogaPaper;

               }

               pnode = pticket->get_pinfo_node("com.apple.print.PaperInfo.ppd.PMPaperName");

               if(pnode != NULL)
               {

                  pnode->value = strPaper;

               }

               pnode = pticket->get_pinfo_node("com.apple.print.PageFormat.PMAdjustedPageRect");

               //CSize sizePaper = m_PrinterSettings.GetCustomPageSize();

               int iW = thePageSize.cx;

               int iH = thePageSize.cy;

               int iValue;

               CString str;

               if(pnode != NULL)
               {

                  if(bCustomPaper)
                  {

                     str.Format("%d",0);
                     pnode->GetChild(bSwap ? 1 : 0)->value = str;
                     str.Format("%d",0);
                     pnode->GetChild(bSwap ? 0 : 1)->value = str;
                     str.Format("%d",iH);
                     pnode->GetChild(bSwap ? 3 : 2)->value = str;
                     str.Format("%d",iW);
                     pnode->GetChild(bSwap ? 2 : 3)->value = str;

                  }
                  else
                  {

                     iValue = atoi(pnode->GetChild(bSwap ? 1 : 0)->value);
                     str.Format("%d", iValue * thePageSize.cy / iAdjH);
                     pnode->GetChild(bSwap ? 1 : 0)->value = str;

                     iValue = atoi(pnode->GetChild(bSwap ? 0 : 1)->value);
                     str.Format("%d",iValue * thePageSize.cx / iAdjW);
                     pnode->GetChild(bSwap ? 0 : 1)->value = str;

                     iValue = atoi(pnode->GetChild(bSwap ? 3 : 2)->value);
                     str.Format("%d",iValue* thePageSize.cy / iAdjH);
                     pnode->GetChild(bSwap ? 3 : 2)->value = str;

                     iValue = atoi(pnode->GetChild(bSwap ? 2 : 3)->value);
                     str.Format("%d",iValue * thePageSize.cx / iAdjW);
                     pnode->GetChild(bSwap ? 2 : 3)->value = str;

                  }

               }

               pnode = pticket->get_pinfo_node("com.apple.print.PageFormat.PMAdjustedPaperRect");

               if(pnode != NULL)
               {

                  if(bCustomPaper)
                  {

                     str.Format("%d",0);
                     pnode->GetChild(bSwap ? 1 : 0)->value = str;
                     str.Format("%d",0);
                     pnode->GetChild(bSwap ? 0 : 1)->value = str;
                     str.Format("%d",iH);
                     pnode->GetChild(bSwap ? 3 : 2)->value = str;
                     str.Format("%d",iW);
                     pnode->GetChild(bSwap ? 2 : 3)->value = str;

                  }
                  else
                  {

                     iValue = atoi(pnode->GetChild(bSwap ? 1 : 0)->value);
                     str.Format("%d",iValue * thePageSize.cy / iAdjH);
                     pnode->GetChild(bSwap ? 1 : 0)->value = str;

                     iValue = atoi(pnode->GetChild(bSwap ? 0 : 1)->value);
                     str.Format("%d",iValue * thePageSize.cx / iAdjW);
                     pnode->GetChild(bSwap ? 0 : 1)->value = str;

                     iValue = atoi(pnode->GetChild(bSwap ? 3 : 2)->value);
                     str.Format("%d",iValue* thePageSize.cy / iAdjH);
                     pnode->GetChild(bSwap ? 3 : 2)->value = str;

                     iValue = atoi(pnode->GetChild(bSwap ? 2 : 3)->value);
                     str.Format("%d",iValue * thePageSize.cx / iAdjW);
                     pnode->GetChild(bSwap ? 2 : 3)->value = str;
                  }

               }

               pnode = pticket->get_pinfo_node("com.apple.print.PaperInfo.PMUnadjustedPageRect");

               if(pnode != NULL)
               {

                  if(bCustomPaper)
                  {

                     str.Format("%d",0);
                     pnode->GetChild(0)->value = str;
                     str.Format("%d",0);
                     pnode->GetChild(1)->value = str;
                     str.Format("%d",iH);
                     pnode->GetChild(2)->value = str;
                     str.Format("%d",iW);
                     pnode->GetChild(3)->value = str;

                  }
                  else
                  {


                     iValue = atoi(pnode->GetChild(0)->value);
                     str.Format("%d",iValue * thePageSizeUnscaled.cy / iPageA4H);
                     pnode->GetChild(0)->value = str;

                     iValue = atoi(pnode->GetChild(1)->value);
                     str.Format("%d",iValue * thePageSizeUnscaled.cx / iPageA4W);
                     pnode->GetChild(1)->value = str;

                     iValue = atoi(pnode->GetChild(2)->value);
                     str.Format("%d",iValue* thePageSizeUnscaled.cy / iPageA4H);
                     pnode->GetChild(2)->value = str;

                     iValue = atoi(pnode->GetChild( 3)->value);
                     str.Format("%d",iValue * thePageSizeUnscaled.cx / iPageA4W);
                     pnode->GetChild(3)->value = str;
                  }

               }

               pnode = pticket->get_pinfo_node("com.apple.print.PaperInfo.PMUnadjustedPaperRect");

               if(pnode != NULL)
               {

                  if(bCustomPaper)
                  {

                     str.Format("%d",0);
                     pnode->GetChild(0)->value = str;
                     str.Format("%d",0);
                     pnode->GetChild(1)->value = str;
                     str.Format("%d",iH);
                     pnode->GetChild(2)->value = str;
                     str.Format("%d",iW);
                     pnode->GetChild(3)->value = str;
                  }
                  else
                  {

                     str.Format("%d",atoi(pnode->GetChild(0)->value) * thePageSizeUnscaled.cy / iPageA4H);
                     pnode->GetChild(0)->value = str;
                     str.Format("%d",atoi(pnode->GetChild(1)->value) * thePageSizeUnscaled.cx / iPageA4W);
                     pnode->GetChild(1)->value = str;
                     str.Format("%d",atoi(pnode->GetChild(2)->value) * thePageSizeUnscaled.cy / iPageA4H);
                     pnode->GetChild(2)->value = str;
                     str.Format("%d",atoi(pnode->GetChild(3)->value) * thePageSizeUnscaled.cx / iPageA4W);
                     pnode->GetChild(3)->value = str;

                  }

               }

            }

            int iPageScale = m_PrinterSettings.GetPageScale();

            if(iPageScale > 0)
            {

               pnode = pmain->get_pinfo_node("com.apple.print.PageFormat.PMScaling");

               if(pnode != NULL)
               {

                  CString str;

                  str.Format("%f",(double)iPageScale / 100.0);

                  pnode->value = str;

               }

            }

            str = doc.GetXML();

         }
         else
         {

            memory mem;

            mem.read_resource(GetModuleHandle(NULL),IDR_XML_PRINT1,"XML");

            str = CString(mem.get_data(),mem.get_size());

         }

         str.Replace("\r\n","\r");

         str.TrimLeft();

         long lLength = str.GetLength();

         CMemFile fileFlatPrintSettings;

         ::serialize serializeFlatPageFormat(&fileFlatPrintSettings);

         serializeFlatPageFormat(lLength);

         serializeFlatPageFormat(str);

         header.write(serialize.m_pointer,mem,fo,kMarkerFlatPageFormat,fileFlatPrintSettings);

      }

      {

         memory mem;

         mem.read_resource(GetModuleHandle(NULL),IDR_XML_PRINT_SETTINGS_EMPTY, "XML");

         CString str(mem.get_data(),mem.get_size());

         str.Replace("\r\n","\r");

         long lLength = str.GetLength();

         CMemFile fileFlatPrintSettings;

         ::serialize serializeFlatPageFormat(&fileFlatPrintSettings);

         serializeFlatPageFormat(lLength);
         serializeFlatPageFormat(str);

         header.write(serialize.m_pointer,mem,fo,kMarkerFlatPrintSettings,fileFlatPrintSettings);

      }

      CMemFile file;

      {

         file.SetLength(0);

         ::serialize serialize(&file);

         serialize(m_sFieldNames[1]);
         serialize(m_sFieldNames[2]);
         serialize(m_sFieldNames[3]);
         serialize(m_sFieldNames[4]);

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerCDocObjectInfo,file);

      {

         file.SetLength(0);

         ::serialize serialize(&file);

         UInt64 thePalSize = m_Palette.GetEntryCount();

         serialize(thePalSize);

         if(thePalSize > 0)
         {

            short entries = m_Palette.GetEntryCount();

            serialize(entries);

            //short theShort[7];

            //memset(theShort,0,7 * sizeof(short));

            //serialize.serialize_static_array(theShort,7);

            PhPaletteEntry entry;

            entry.ciUsage = 2;
            entry.ciTolerance = 0;
            entry.ciDataFields[0] = 0;
            entry.ciDataFields[1] = 0;
            entry.ciDataFields[2] = 0;

            PALETTEENTRY e;

            for(int i = 0; i < m_Palette.GetEntryCount(); i++)
            {

               m_Palette.GetPaletteEntries(i,1,&e);

               entry.ciRGB.fRed = (float) ((double) e.peRed / 255.);
               entry.ciRGB.fGreen = (float) ((double)e.peGreen / 255.);
               entry.ciRGB.fBlue = (float) ((double)e.peBlue / 255.);

               serialize(entry);

            }

            file.SeekToBegin();

            thePalSize = file.GetLength() - sizeof(thePalSize);

            serialize(thePalSize);

         }

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerPalette,file);


      // bitmaps
      {

         file.SetLength(0);

         ::serialize serialize(&file);

         UINT uiRealBitmapCount = 0;

         serialize(uiRealBitmapCount);

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerBitmaps,file);
   

      {

         file.SetLength(0);

         ::serialize serialize(&file);

         file.Write(g_dashtableDefault,sizeof(g_dashtableDefault));

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerDashTable,file);


      
      {

         file.SetLength(0);

         ::serialize serialize(&file);

         PhGenericPatternHeader header;

         header.numItems = m_packeditema.GetCount() + 1;
         header.sizeVersionNumber = 0x18;
         header.idOfVersionNumber = 0x1000030;
         header.versionNumber = 0x6000000000000LL;

         serialize(&header);

         if(m_packeditema.GetCount() > 0)
         {

            TRACE("numofpats %d",m_packeditema.GetCount());

            for(int i=0; i < m_packeditema.GetCount(); i++)
            {

               serialize(m_packeditema[i]);

            }

         }

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerGenericPattern,file);


      //PhComplexPtPicData cppd;
      //cppd.fSignature='MD40';
      //cppd.fKind=0x3B0;
      //cppd.fSpare1S=0;
      //cppd.fSpare2L=0;
      //cppd.fSpare3L=0;
      //cppd.fVersion=0;
      //cppd.fComplexType=kPhComplexPtLinkPoint;

      //uptr pp((char*)&cppd);
      //pp.swapl(1); pp.swaps(2); pp.swapl(4);

      ULONGLONG ulBegShapes;
      ULONGLONG ulEndShapes;

      {

         file.SetLength(0);

         ::serialize serializeLayers(&file);

         UInt32 numLayers = m_Layers.GetSize();

         serializeLayers(numLayers);

         ulBegShapes = fo->GetPosition();

         for(int i=0; i < m_Layers.GetSize(); i++)
         {
            
            CMemFile fileShapes;

            ctx.serialize.m_pfile = &fileShapes;

            CLayer * pLayer=m_Layers[i];

            LayerStruct layer;
            layer.fReserved1 = 0;
            layer.fReserved2 = 0;
            layer.fReserved3 = 0;
            layer.fReserved4 = 0;
            layer.fReserved5 = 0;
            layer.fReserved6 = 0;
            layer.fReserved7 = 0;
            layer.fReserved8 = 6;
            layer.fAdditionalFlags = 0;
            layer.fLayerActiveFlag = i == m_nActiveLayer;
            layer.fCFRLayerName = pLayer->m_strName;
            layer.fLayerFlags=kPhSelectable | kPhScaled | kPhFillsOn;
            if(!pLayer->m_bGrayed) layer.fLayerFlags|=kPhNotGrayed;
            if(!pLayer->m_bHidden) layer.fLayerFlags|=kPhVisible;
            GetScaleRatio(pLayer->m_nScale,layer.fCustomRatio);
            //layer.cScaleIndex=;
            //if(!m_bEnglishUnits & layer.fScaleIndex>20) layer.fScaleIndex++;
            layer.fSnapIndex=pLayer->m_nSnap - 1;
            layer.fNumShapes=pLayer->m_ShapeList.GetCount();
            //layer.fLayerOffset=fo->GetPosition();
            ctx.serialize.m_ratio = layer.fCustomRatio;
            UInt64 numShapes = 0;
            
            ULONGLONG ullBegShapes = ctx.serialize.m_pfile->GetPosition();

            ctx.serialize(numShapes);

            POSITION pos=pLayer->m_ShapeList.GetTailPosition();
            
            while(pos)
            {
               
               CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);

               //if(numShapes >= 250)
               //{
                  //OutputDebugString("264");
               //}

               numShapes += pShape->SaveMacDraft70(ctx);
               //if(numShapes >= 250)
               //{
                 // OutputDebugString("264");
               //}

            }

            layer.fNumShapes = numShapes;
            
            ULONGLONG ullEndShapes= ctx.serialize.m_pfile->GetPosition();
            ctx.serialize.m_pfile->Seek(ullBegShapes,CFile::begin);
            ctx.serialize(numShapes);
            ctx.serialize.m_pfile->Seek(ullEndShapes,CFile::begin);
            ctx.serialize.m_pfile = fo;

            serialize.write(layer.fLayerOffset,layer.fLayerLength,&fileShapes);

            serializeLayers(layer);


         }

         ulEndShapes = fo->GetPosition();

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerLayerStruct,file);

      header.uiOffset = ulBegShapes;
      header.uiLength = ulEndShapes - ulBegShapes;

      header.write(serialize.m_pointer,fo,kMarkerShapes);


      file.SetLength(0);

      // bitmaps
      if(ctx.bitmaps.GetSize() <= 0)
      {

         ::serialize serialize(&file);

         UINT uiRealBitmapCount = ctx.bitmaps.GetCount();

         serialize(uiRealBitmapCount);


      }
      else
      {


         
         ::serialize serialize(&file);

         UINT uiRealBitmapCount = ctx.bitmaps.GetCount();

         serialize(uiRealBitmapCount);

         for(int ui = 0; ui < uiRealBitmapCount; ui++)
         {
            
            HBITMAP hDib=(HBITMAP)ctx.bitmaps[ui];
            
            auto pb = new Gdiplus::Bitmap(hDib,NULL);


            PhBitmapIOStruct bitmap;


            if(SavePNG(bitmap.imageData,pb))
            {

               bitmap.imageDataSize = bitmap.imageData.GetSize();

               CMemFile fileBitmap;

               {
                  
                  ::formatMD70::serialize serialize(&fileBitmap);

                  bitmap.fColor = pb->GetPixelFormat() != PixelFormat1bppIndexed;

                  serialize(bitmap.imageDataSize);

                  serialize(bitmap.imageData,bitmap.imageDataSize);

                  ctx.serialize.write(bitmap.fBitmapOffset,bitmap.fBitmapLength,&fileBitmap);

               }

               {
                  bitmap.fColor = 1;
                  bitmap.fShareCount = 1;
                  bitmap.fUnused = 0;
                  serialize(bitmap.fBitmapOffset);
                  serialize(bitmap.fBitmapLength);
                  serialize(bitmap.fShareCount);
                  serialize(bitmap.fColor);
                  serialize(bitmap.fUnused);

               }
            }

            delete pb;



         }

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerBitmaps,file);


      // Views
      file.SetLength(0);

      int iViews = 0; // Dummy

      // Views
      if(iViews <= 0)
      {

         ::serialize serialize(&file);

         UINT uiViewCount = iViews;

         serialize(uiViewCount);


      }

      header.write(serialize.m_pointer,mem,fo,kMarkerViews,file);


      file.SetLength(0);

      // kMarkerGuidesInfo
      {

         PhGuidesInfo guidesinfo;

         for (int i = 0; i < m_guidea.GetCount(); i++)
         {

            auto & guide = m_guidea[i];

            PhGuideEntry guideentry;

            guideentry.fType = guide.m_bVertical ? 1:0;

            LongRatio ratio;

            GetActiveScaleRatio(ratio);

            double uservalue = GetDimUnit()->numDBToMm(ratio, guide.m_coord);

            double d = uservalue;

            guideentry.fLineValue = d * 10.0f;
               
            guidesinfo.guides.Add(guideentry);

         }

         guidesinfo.numGuides = guidesinfo.guides.GetCount();

         ::serialize serialize(&file);

         guidesinfo(serialize);

      }

      header.write(serialize.m_pointer,mem,fo,kMarkerGuidesInfo,file);


      // All Links
      file.SetLength(0);

      int iAllLinks = 0; // Dummy

      // All Links
      if(iAllLinks <= 0)
      {

         ::serialize serialize(&file);

         UINT uiAllLinksCount = iAllLinks;

         serialize(uiAllLinksCount);


      }

      header.write(serialize.m_pointer,mem,fo,kMarkerAllLinks,file);



      // Reports
      file.SetLength(0);

      int iReports = 0; // Dummy

      // Reports
      if(iReports <= 0)
      {

         ::serialize serialize(&file);

         UINT uiReportsCount = iReports;

         serialize(uiReportsCount);


      }

      header.write(serialize.m_pointer,mem,fo,kMarkerReports,file);




      //bitmaps
      //memset(&list,0,sizeof(PhFListHeader));
      //list.fNumElems=ctx.bitmaps.GetSize();
      //list.fElemSize=sizeof(PhBitmapIOStruct);
      //list.fInitSize=list.fNumElems*list.fElemSize;
      //list.fGrowSize=sizeof(PhBitmapIOStruct);
      //list.fKeylen=4;

      //uptr p((char*)&list);
      //p.swaps(1); p.swapl(1); p.swaps(1);
      //p.swapl(3); p.swaps(3); p.swapl(1);
      //mf.Write(&list,sizeof(PhFListHeader));

      //for(int i=0; i<ctx.bitmaps.GetSize(); i++)
      //{
      //   HBITMAP hDib=(HBITMAP)ctx.bitmaps[i];
      //   DIBSECTION ds;
      //   ::GetObject(hDib,sizeof(DIBSECTION),&ds);

      //   // something is wrong with 24-bit pict format so we'll make it 32-bit
      //   HBITMAP hCopy=NULL;
      //   if(ds.dsBmih.biBitCount == 24)
      //   {
      //      ds.dsBmih.biBitCount=32;
      //      LPVOID lpBits;
      //      HDC hdc=::GetDC(NULL);
      //      hCopy=::CreateDIBSection(hdc,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
      //      {
      //         HDC hSrcDC,hDstDC;
      //         hSrcDC=::CreateCompatibleDC(hdc);
      //         hDstDC=::CreateCompatibleDC(hdc);
      //         HGDIOBJ hSrcOldBmp=::SelectObject(hSrcDC,hDib);
      //         HGDIOBJ hDstOldBmp=::SelectObject(hDstDC,hCopy);
      //         ::BitBlt(hDstDC,0,0,ds.dsBmih.biWidth,ds.dsBmih.biHeight,hSrcDC,0,0,SRCCOPY);
      //         ::SelectObject(hSrcDC,hSrcOldBmp);
      //         ::SelectObject(hDstDC,hDstOldBmp);
      //         VERIFY(::DeleteDC(hDstDC));
      //         VERIFY(::DeleteDC(hSrcDC));
      //      }
      //      ::ReleaseDC(NULL,hdc);
      //      ::GetObject(hCopy,sizeof(DIBSECTION),&ds);
      //      hDib=hCopy;
      //   }

      //   ds.dsBm.bmWidthBytes+=0x3;
      //   ds.dsBm.bmWidthBytes&=~0x3;

      //   PixMap pm;
      //   pm.baseAddr=ds.dsBmih.biSizeImage;
      //   pm.rowBytes=0x8000 | ((short)ds.dsBm.bmWidthBytes);
      //   pm.bounds.l=0;
      //   pm.bounds.t=0;
      //   pm.bounds.r=(short)ds.dsBmih.biWidth;
      //   pm.bounds.b=(short)ds.dsBmih.biHeight;
      //   pm.pmVersion=0;
      //   pm.packType=0;
      //   pm.packSize=0;
      //   pm.hRes=0x00480000;
      //   pm.vRes=0x00480000;
      //   pm.pixelType=ds.dsBmih.biBitCount<16?0:16;
      //   pm.pixelSize=ds.dsBmih.biBitCount;
      //   pm.cmpCount=pm.pixelType?3:1;
      //   pm.cmpSize=(ds.dsBmih.biBitCount<16)?pm.pixelSize:(ds.dsBmih.biBitCount == 16)?5:8;
      //   pm.planeBytes=0;
      //   pm.pmHandle=(ds.dsBmih.biBitCount<16)?sizeof(ColorTable) + (ds.dsBmih.biClrUsed - 1)*sizeof(ColorSpec):sizeof(ColorTable);
      //   pm.pmReserved=0;

      //   long size=sizeof(PixMap) + pm.baseAddr + pm.pmHandle;
      //   PhBitmapIOStruct bitmapIO;
      //   bitmapIO.fBitmapOffset=ctx.f->GetPosition();
      //   bitmapIO.fBitmapLength=size + sizeof(long);
      //   bitmapIO.fColor=1;
      //   bitmapIO.fShareCount=1;
      //   bitmapIO.fUnused=0;

      //   uptr(&size).swapl(1);
      //   ctx.f->Write(&size,sizeof(long));

      //   uptr p((char*)&pm);
      //   p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
      //   p.swapl(3);	p.swaps(4); p.swapl(3);
      //   ctx.f->Write(&pm,sizeof(PixMap));

      //   char* lpBits=(char*)ds.dsBm.bmBits;
      //   char* pBuf=new char[ds.dsBm.bmWidthBytes];
      //   for(int y=ds.dsBmih.biHeight - 1; y >= 0; y--)
      //   {
      //      memcpy(pBuf,lpBits + y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
      //      p.pc=pBuf;
      //      switch(ds.dsBmih.biBitCount)
      //      {
      //      case 16:
      //         p.swaps(ds.dsBm.bmWidthBytes >> 1);
      //         break;
      //      case 24:
      //         p.swapRGB(ds.dsBm.bmWidthBytes / 3);
      //         break;
      //      case 32:
      //         p.swapl(ds.dsBm.bmWidthBytes >> 2);
      //         break;
      //      }
      //      ctx.f->Write(pBuf,ds.dsBm.bmWidthBytes);
      //   }
      //   delete[] pBuf;
      //   if(ds.dsBmih.biBitCount<16)
      //   {
      //      short count=(short)ds.dsBmih.biClrUsed;
      //      CWindowDC dcScreen(NULL);
      //      CDC memDC;
      //      memDC.CreateCompatibleDC(&dcScreen);
      //      HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
      //      LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
      //      GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
      //      memDC.SelectObject(hOldBmp);

      //      ColorTable ct;
      //      ct.ctSeed=0;
      //      ct.ctFlags=0;
      //      ct.ctSize=count - 1;
      //      uptr p((char*)&ct);
      //      p.swapl(1); p.swaps(2);
      //      ctx.f->Write(&ct,sizeof(ColorTable) - sizeof(ColorSpec));
      //      for(short c=0; c<count; c++)
      //      {
      //         ColorSpec cs;
      //         cs.value=c;
      //         cs.rgb.red=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
      //         cs.rgb.green=(lpColors[c].rgbGreen << 8) + lpColors[c].rgbGreen;
      //         cs.rgb.blue=(lpColors[c].rgbBlue << 8) + lpColors[c].rgbBlue;
      //         uptr((char*)&cs).swaps(4);
      //         ctx.f->Write(&cs,sizeof(ColorSpec));
      //      }
      //      GlobalFree(GlobalHandle(lpColors));
      //   }
      //   else
      //   {
      //      ColorTable ct;
      //      memset(&ct,0,sizeof(ColorTable));
      //      ctx.f->Write(&ct,sizeof(ColorTable));
      //   }

      //   bitmapIO.fBitmapLength=ctx.f->GetPosition();
      //   bitmapIO.fBitmapLength-=bitmapIO.fBitmapOffset;

      //   p.pc=(char*)&bitmapIO;
      //   p.swapl(2); p.swaps(1); p.skip(2);
      //   mf.Write(&bitmapIO,sizeof(PhBitmapIOStruct));
      //   if(hCopy)
      //   {
      //      ::DeleteObject(hCopy);
      //      hCopy=NULL;
      //   }
      //}

      //WriteMFResource(fo,mdf.numBytes,mf,'BITL',0);


      //   linkVertexList.Copy(pLayer->m_LinkVertexList);
      //   POSITION pos=pLayer->m_ShapeList.GetTailPosition();
      //   while(pos)
      //   {
      //      CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
      //      layer.fNumShapes+=pShape->SaveMacDraft620(ctx);
      //   }
      //   layer.fLayerLength=fo->GetPosition();
         //if(layer.fNumShapes)
         //{
         //   long count=layer.fNumShapes;
         //   uptr((char*)&count).swapl(1);
         //   fo->Seek(layer.fLayerOffset,CFile::begin);
         //   fo->Write(&count,sizeof(long));
         //   fo->Seek(layer.fLayerLength,CFile::begin);
         //}
         //layer.fLayerLength-=layer.fLayerOffset;

      //   uptr p((char*)&layer);
      //   p.skip(kPhMaxLayerNameLen);
      //   p.swaps(3); p.swapl(3);
      //   mf.Write(&layer,sizeof(PhLayerStruct));

      //   if(linkVertexList.GetCount())
      //   {
      //      long nPrevKey=0;
      //      CMemFile lvf;
      //      do
      //      {
      //         CLinkVertex *pLinkVertex=linkVertexList.RemoveHead();
      //         CLinkVertex* pLinkVertex2;
      //         if(!pLinkVertex->m_bResolved)
      //         {
      //            PhLinkResInfo lri;
      //            if(pLinkVertex->m_nKey != nPrevKey)
      //            {
      //               int len=lvf.GetLength();
      //               if(len)
      //               {
      //                  linkedVertexResLen.Add(len);
      //                  linkedVertexResPtr.Add(lvf.Detach());
      //               }
      //               nPrevKey=pLinkVertex->m_nKey;

      //               if(pLinkVertex->m_nKey >= 0)
      //               {
      //                  if(pLinkVertex->m_nVertex == kCenterHandle)
      //                  {
      //                     pLinkVertex2=linkVertexList.RemoveHead();//GetHead();
      //                     cppd.fComplexType=kPhComplexPtLinkPoint;
      //                     uptr((char*)&cppd.fComplexType).swapl();
      //                     lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                     lri.fObjectIndex=pLinkVertex->m_nShape;
      //                     lri.fPointIndex=2;
      //                     lri.fFlags=0;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                     lri.fObjectIndex=pLinkVertex2->m_nShape;
      //                     lri.fPointIndex=0;
      //                     lri.fFlags=1;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                     linkedVertexResLen.Add(lvf.GetLength());
      //                     linkedVertexResPtr.Add(lvf.Detach());

      //                     lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                     lri.fObjectIndex=pLinkVertex->m_nShape;
      //                     lri.fPointIndex=1;
      //                     lri.fFlags=0;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                     lri.fObjectIndex=pLinkVertex2->m_nShape;
      //                     lri.fPointIndex=0;
      //                     lri.fFlags=1;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                     linkedVertexResLen.Add(lvf.GetLength());
      //                     linkedVertexResPtr.Add(lvf.Detach());

      //                     lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                     lri.fObjectIndex=pLinkVertex->m_nShape;
      //                     lri.fPointIndex=0;
      //                     lri.fFlags=0;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                     lri.fObjectIndex=pLinkVertex2->m_nShape;
      //                     lri.fPointIndex=0;
      //                     lri.fFlags=1;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                     linkedVertexResLen.Add(lvf.GetLength());
      //                     linkedVertexResPtr.Add(lvf.Detach());

      //                     cppd.fComplexType=kPhComplexPtArcDimPoint;
      //                  }
      //                  else cppd.fComplexType=kPhComplexPtLinkPoint;
      //                  uptr((char*)&cppd.fComplexType).swapl();
      //                  lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                  if(pLinkVertex->m_nVertex == kCenterHandle)
      //                  {
      //                     lri.fObjectIndex=pLinkVertex2->m_nShape;
      //                     lri.fPointIndex=0;
      //                     lri.fFlags=0;
      //                     uptr((char*)&lri).swapl(3);
      //                     lvf.Write(&lri,sizeof(PhLinkResInfo));

      //                  }
      //               }
      //               else
      //               {
      //                  // two more shapes
      //                  ASSERT(linkVertexList.GetCount()>1);
      //                  pLinkVertex2=linkVertexList.RemoveHead();

      //                  cppd.fComplexType=kPhComplexPtLinkPoint;
      //                  uptr((char*)&cppd.fComplexType).swapl();
      //                  lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                  lri.fObjectIndex=pLinkVertex2->m_nShape;
      //                  lri.fPointIndex=pLinkVertex2->m_nVertex;
      //                  lri.fFlags=0;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  lri.fObjectIndex=pLinkVertex->m_nShape;
      //                  lri.fPointIndex=pLinkVertex->m_nVertex;
      //                  lri.fFlags=1;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  linkedVertexResLen.Add(lvf.GetLength());
      //                  linkedVertexResPtr.Add(lvf.Detach());

      //                  CLinkVertex* pLinkVertex3=linkVertexList.RemoveHead();

      //                  lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                  lri.fObjectIndex=pLinkVertex3->m_nShape;
      //                  lri.fPointIndex=pLinkVertex3->m_nVertex;
      //                  lri.fFlags=0;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  lri.fObjectIndex=pLinkVertex->m_nShape;
      //                  lri.fPointIndex=pLinkVertex->m_nVertex;
      //                  lri.fFlags=1;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  linkedVertexResLen.Add(lvf.GetLength());
      //                  linkedVertexResPtr.Add(lvf.Detach());

      //                  cppd.fComplexType=kPhComplexPtAngularDimPoint;
      //                  uptr((char*)&cppd.fComplexType).swapl();
      //                  lvf.Write(&cppd,sizeof(PhComplexPtPicData));
      //                  lri.fObjectIndex=pLinkVertex->m_nShape;
      //                  lri.fPointIndex=pLinkVertex->m_nVertex;
      //                  lri.fFlags=0;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  lri.fObjectIndex=pLinkVertex3->m_nShape;
      //                  lri.fPointIndex=pLinkVertex3->m_nVertex;
      //                  lri.fFlags=1;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  lri.fObjectIndex=pLinkVertex2->m_nShape;
      //                  lri.fPointIndex=pLinkVertex2->m_nVertex;
      //                  lri.fFlags=1;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  lri.fObjectIndex=pLinkVertex->m_nShape;
      //                  lri.fPointIndex=pLinkVertex->m_nVertex;
      //                  lri.fFlags=1;
      //                  uptr((char*)&lri).swapl(3);
      //                  lvf.Write(&lri,sizeof(PhLinkResInfo));
      //                  linkedVertexResLen.Add(lvf.GetLength());
      //                  linkedVertexResPtr.Add(lvf.Detach());

      //                  delete pLinkVertex;
      //                  delete pLinkVertex2;
      //                  delete pLinkVertex3;
      //                  continue;
      //               }
      //            }
      //            lri.fObjectIndex=pLinkVertex->m_nShape;
      //            if(pLinkVertex->m_nVertex<0)
      //            {
      //               lri.fPointIndex=0;
      //               lri.fFlags=1;
      //            }
      //            else
      //            {
      //               lri.fPointIndex=pLinkVertex->m_nVertex;
      //               lri.fFlags=0;
      //            }
      //            uptr((char*)&lri).swapl(3);
      //            lvf.Write(&lri,sizeof(PhLinkResInfo));

      //         }
      //         delete pLinkVertex;
      //      }
      //      while(linkVertexList.GetCount());
      //      int len=lvf.GetLength();
      //      if(len)
      //      {
      //         linkedVertexResLen.Add(len);
      //         linkedVertexResPtr.Add(lvf.Detach());
      //      }
      //   }
      //}
      //for(int i=0; i<ctx.metaMembers.GetSize(); i++)
      //{
      //   CMDShape* pShape=(CMDShape*)ctx.metaMembers[i];
      //   delete pShape;
      //}

      //PhObjectInfoFieldSet fields;
      //ctx.ConvertC2P(m_sFieldNames[1],fields.fF2Name,31);
      //ctx.ConvertC2P(m_sFieldNames[2],fields.fF3Name,31);
      //ctx.ConvertC2P(m_sFieldNames[3],fields.fF4Name,31);
      //ctx.ConvertC2P(m_sFieldNames[4],fields.fF5Name,31);
      //fo->Write(&fields,sizeof(PhObjectInfoFieldSet));

      // bitmaps
      //if(ctx.bitmaps.GetSize())
      //{
      //   CMemFile mf;
      //   memset(&list,0,sizeof(PhFListHeader));
      //   list.fNumElems=ctx.bitmaps.GetSize();
      //   list.fElemSize=sizeof(PhBitmapIOStruct);
      //   list.fInitSize=list.fNumElems*list.fElemSize;
      //   list.fGrowSize=sizeof(PhBitmapIOStruct);
      //   list.fKeylen=4;

      //   uptr p((char*)&list);
      //   p.swaps(1); p.swapl(1); p.swaps(1);
      //   p.swapl(3); p.swaps(3); p.swapl(1);
      //   mf.Write(&list,sizeof(PhFListHeader));

      //   for(int i=0; i<ctx.bitmaps.GetSize(); i++)
      //   {
      //      HBITMAP hDib=(HBITMAP)ctx.bitmaps[i];
      //      DIBSECTION ds;
      //      ::GetObject(hDib,sizeof(DIBSECTION),&ds);

      //      // something is wrong with 24-bit pict format so we'll make it 32-bit
      //      HBITMAP hCopy=NULL;
      //      if(ds.dsBmih.biBitCount == 24)
      //      {
      //         ds.dsBmih.biBitCount=32;
      //         LPVOID lpBits;
      //         HDC hdc=::GetDC(NULL);
      //         hCopy=::CreateDIBSection(hdc,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
      //         {
      //            HDC hSrcDC,hDstDC;
      //            hSrcDC=::CreateCompatibleDC(hdc);
      //            hDstDC=::CreateCompatibleDC(hdc);
      //            HGDIOBJ hSrcOldBmp=::SelectObject(hSrcDC,hDib);
      //            HGDIOBJ hDstOldBmp=::SelectObject(hDstDC,hCopy);
      //            ::BitBlt(hDstDC,0,0,ds.dsBmih.biWidth,ds.dsBmih.biHeight,hSrcDC,0,0,SRCCOPY);
      //            ::SelectObject(hSrcDC,hSrcOldBmp);
      //            ::SelectObject(hDstDC,hDstOldBmp);
      //            VERIFY(::DeleteDC(hDstDC));
      //            VERIFY(::DeleteDC(hSrcDC));
      //         }
      //         ::ReleaseDC(NULL,hdc);
      //         ::GetObject(hCopy,sizeof(DIBSECTION),&ds);
      //         hDib=hCopy;
      //      }

      //      ds.dsBm.bmWidthBytes+=0x3;
      //      ds.dsBm.bmWidthBytes&=~0x3;

      //      PixMap pm;
      //      pm.baseAddr=ds.dsBmih.biSizeImage;
      //      pm.rowBytes=0x8000 | ((short)ds.dsBm.bmWidthBytes);
      //      pm.bounds.l=0;
      //      pm.bounds.t=0;
      //      pm.bounds.r=(short)ds.dsBmih.biWidth;
      //      pm.bounds.b=(short)ds.dsBmih.biHeight;
      //      pm.pmVersion=0;
      //      pm.packType=0;
      //      pm.packSize=0;
      //      pm.hRes=0x00480000;
      //      pm.vRes=0x00480000;
      //      pm.pixelType=ds.dsBmih.biBitCount<16?0:16;
      //      pm.pixelSize=ds.dsBmih.biBitCount;
      //      pm.cmpCount=pm.pixelType?3:1;
      //      pm.cmpSize=(ds.dsBmih.biBitCount<16)?pm.pixelSize:(ds.dsBmih.biBitCount == 16)?5:8;
      //      pm.planeBytes=0;
      //      pm.pmHandle=(ds.dsBmih.biBitCount<16)?sizeof(ColorTable) + (ds.dsBmih.biClrUsed - 1)*sizeof(ColorSpec):sizeof(ColorTable);
      //      pm.pmReserved=0;

      //      long size=sizeof(PixMap) + pm.baseAddr + pm.pmHandle;
      //      PhBitmapIOStruct bitmapIO;
      //      bitmapIO.fBitmapOffset=ctx.f->GetPosition();
      //      bitmapIO.fBitmapLength=size + sizeof(long);
      //      bitmapIO.fColor=1;
      //      bitmapIO.fShareCount=1;
      //      bitmapIO.fUnused=0;

      //      uptr(&size).swapl(1);
      //      ctx.f->Write(&size,sizeof(long));

      //      uptr p((char*)&pm);
      //      p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
      //      p.swapl(3);	p.swaps(4); p.swapl(3);
      //      ctx.f->Write(&pm,sizeof(PixMap));

      //      char* lpBits=(char*)ds.dsBm.bmBits;
      //      char* pBuf=new char[ds.dsBm.bmWidthBytes];
      //      for(int y=ds.dsBmih.biHeight - 1; y >= 0; y--)
      //      {
      //         memcpy(pBuf,lpBits + y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
      //         p.pc=pBuf;
      //         switch(ds.dsBmih.biBitCount)
      //         {
      //         case 16:
      //            p.swaps(ds.dsBm.bmWidthBytes >> 1);
      //            break;
      //         case 24:
      //            p.swapRGB(ds.dsBm.bmWidthBytes / 3);
      //            break;
      //         case 32:
      //            p.swapl(ds.dsBm.bmWidthBytes >> 2);
      //            break;
      //         }
      //         ctx.f->Write(pBuf,ds.dsBm.bmWidthBytes);
      //      }
      //      delete[] pBuf;
      //      if(ds.dsBmih.biBitCount<16)
      //      {
      //         short count=(short)ds.dsBmih.biClrUsed;
      //         CWindowDC dcScreen(NULL);
      //         CDC memDC;
      //         memDC.CreateCompatibleDC(&dcScreen);
      //         HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
      //         LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
      //         GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
      //         memDC.SelectObject(hOldBmp);

      //         ColorTable ct;
      //         ct.ctSeed=0;
      //         ct.ctFlags=0;
      //         ct.ctSize=count - 1;
      //         uptr p((char*)&ct);
      //         p.swapl(1); p.swaps(2);
      //         ctx.f->Write(&ct,sizeof(ColorTable) - sizeof(ColorSpec));
      //         for(short c=0; c<count; c++)
      //         {
      //            ColorSpec cs;
      //            cs.value=c;
      //            cs.rgb.red=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
      //            cs.rgb.green=(lpColors[c].rgbGreen << 8) + lpColors[c].rgbGreen;
      //            cs.rgb.blue=(lpColors[c].rgbBlue << 8) + lpColors[c].rgbBlue;
      //            uptr((char*)&cs).swaps(4);
      //            ctx.f->Write(&cs,sizeof(ColorSpec));
      //         }
      //         GlobalFree(GlobalHandle(lpColors));
      //      }
      //      else
      //      {
      //         ColorTable ct;
      //         memset(&ct,0,sizeof(ColorTable));
      //         ctx.f->Write(&ct,sizeof(ColorTable));
      //      }

      //      bitmapIO.fBitmapLength=ctx.f->GetPosition();
      //      bitmapIO.fBitmapLength-=bitmapIO.fBitmapOffset;

      //      p.pc=(char*)&bitmapIO;
      //      p.swapl(2); p.swaps(1); p.skip(2);
      //      mf.Write(&bitmapIO,sizeof(PhBitmapIOStruct));
      //      if(hCopy)
      //      {
      //         ::DeleteObject(hCopy);
      //         hCopy=NULL;
      //      }
      //   }

      //   WriteMFResource(fo,mdf.numBytes,mf,'BITL',0);
      //}

      //// layers
      //WriteMFResource(fo,mdf.numBytes,mf,'LAYI',0);

      //// linked vertexes
      //int lvResId=256;
      //for(int i=0; i<linkedVertexResLen.GetSize(); i++)
      //{
      //   if(linkedVertexResPtr[i])
      //   {
      //      WriteResource(fo,mdf.numBytes,linkedVertexResPtr[i],linkedVertexResLen[i],'Link',lvResId++);
      //      free(linkedVertexResPtr[i]);
      //   }
      //}

      //// dash styles
      //WriteResource(fo,mdf.numBytes,m_DashStyles,256,'DASH',128);


      if(ctx.fontInUse.GetCount() > 0)
      {

         ::serialize serialize(&file);

         long fontCount = ctx.fontInUse.GetCount() ;

         serialize(fontCount);

         for(short i=0; i<ctx.fontInUse.GetSize(); i++)
         {

            serialize(i);

            CString strFontName(ctx.fontInUse[i]);

            char len = strFontName.GetLength();

            serialize(len);

            //OutputDebugString(strFontName);

            serialize.m_pfile->Write(strFontName,len);

         }

      }

//      header.write(serialize.m_pointer,mem,fo,kMarkerFontInUseInfo,file);

      //   WriteMFResource(fo,mdf.numBytes,mf,'FNUS',1);
      //}

      //// palette
      //{
      //   int nCount=m_Palette.GetEntryCount();
      //   long length=16 + 16 * nCount;
      //   char* pPal=new char[length];
      //   memset(pPal,0,length);
      //   p.pc=pPal;
      //   *p.ps=nCount;
      //   p.swaps();
      //   p.swapl(1); p.swaps(1); p.swapl(2);
      //   for(int i=0; i<nCount; i++)
      //   {
      //      PALETTEENTRY pe;
      //      m_Palette.GetPaletteEntries(i,1,&pe);
      //      *p.ps=pe.peRed << 8; p.swaps(1);
      //      *p.ps=pe.peGreen << 8; p.swaps(1);
      //      *p.ps=pe.peBlue << 8; p.swaps(1);
      //      *p.ps=0x2; // pmTolerant
      //      p.swaps(5);
      //   }
      //   WriteResource(fo,mdf.numBytes,pPal,length,'pltt',128);
      //   delete[] pPal;
      //   //m_Palette.ResizePalette(83);	// strip extra entries
      //}

      //// patterns
      //{
      //   memset(&list,0,sizeof(PhFListHeader));
      //   list.fListID=128;
      //   list.fListType='PATL';
      //   // by TSM
      //   //list.fNumElems=72+ctx.embedDIBs.GetSize();
      //   list.fNumElems=1 + m_numDIBPatterns + ctx.embedDIBs.GetSize();
      //   list.fElemSize=sizeof(PhFillDescript);
      //   list.fInitSize=sizeof(PhFillDescript);
      //   list.fGrowSize=sizeof(PhFillDescript);
      //   list.fIDData=128;
      //   list.fDataType='PLDT';
      //   list.fKeylen=6;

      //   uptr p((char*)&list);
      //   p.swaps(1); p.swapl(1); p.swaps(1);
      //   p.swapl(3); p.swaps(3); p.swapl(1);
      //   WriteResource(fo,mdf.numBytes,&list,sizeof(PhFListHeader),'PATL',128);

      //   CMemFile mf;
      //   PhFillDescript phf;
      //   memset(&phf,0,sizeof(PhFillDescript));
      //   phf.fColor=1;
      //   phf.fResID=-1;
      //   uptr((char*)&phf).swaps(2);
      //   mf.Write(&phf,sizeof(PhFillDescript));
      //   short ppatID=128;

      //   // by TSM
      //   //for(int i=0;i<(71+ctx.embedDIBs.GetSize());i++)
      //   for(int i=0; i<m_numDIBPatterns + ctx.embedDIBs.GetSize(); i++)
      //   {
      //      // by TSM
      //      /*HBITMAP hDib;
      //      if(i<71) hDib=m_Patterns[i].CreateDIBSection();
      //      else hDib=(HBITMAP)ctx.embedDIBs[i-71];
      //      */
      //      HBITMAP hDib;
      //      if(i<m_numDIBPatterns) hDib=m_Patterns[i].CreateDIBSection();
      //      else hDib=(HBITMAP)ctx.embedDIBs[i - m_numDIBPatterns];
      //      //end by TSM
      //      DIBSECTION ds;
      //      ::GetObject(hDib,sizeof(DIBSECTION),&ds);
      //      if(ds.dsBmih.biBitCount == 1)
      //      {
      //         phf.fColor=0;
      //         phf.fResID=0;
      //         for(int j=0; j<8; j++)
      //         {
      //            phf.fData.fPattern[j]=(uchar)~((LPDWORD)ds.dsBm.bmBits)[j];
      //         }
      //      }
      //      else
      //      {
      //         //=============================================================================
      //         CMemFile mf;
      //         PixPatMap ppm;
      //         ppm.pp.patType=0x1;
      //         ppm.pp.patMap=sizeof(PixPat);
      //         ppm.pp.patData=sizeof(PixPatMap);
      //         ppm.pp.patXData=0;
      //         ppm.pp.patXValid=-1;
      //         ppm.pp.patXMap=0;
      //         ppm.pp.pat1Data[0]=0xAA;
      //         ppm.pp.pat1Data[1]=0x55;
      //         ppm.pp.pat1Data[2]=0xAA;
      //         ppm.pp.pat1Data[3]=0x55;
      //         ppm.pp.pat1Data[4]=0xAA;
      //         ppm.pp.pat1Data[5]=0x55;
      //         ppm.pp.pat1Data[6]=0xAA;
      //         ppm.pp.pat1Data[7]=0x55;

      //         ppm.pm.baseAddr=0;
      //         ppm.pm.rowBytes=0x8000 | ((short)ds.dsBm.bmWidthBytes);
      //         ppm.pm.bounds.l=0;
      //         ppm.pm.bounds.t=0;
      //         ppm.pm.bounds.r=(short)ds.dsBmih.biWidth;
      //         ppm.pm.bounds.b=(short)ds.dsBmih.biHeight;
      //         ppm.pm.pmVersion=0;
      //         ppm.pm.packType=0;
      //         ppm.pm.packSize=0;
      //         ppm.pm.hRes=0x00480000;
      //         ppm.pm.vRes=0x00480000;
      //         ppm.pm.pixelType=ds.dsBmih.biBitCount<16?0:16;
      //         ppm.pm.pixelSize=ds.dsBmih.biBitCount;
      //         ppm.pm.cmpCount=ppm.pm.pixelType?3:1;
      //         ppm.pm.cmpSize=(ds.dsBmih.biBitCount<16)?ppm.pm.pixelSize:(ds.dsBmih.biBitCount == 16)?5:8;
      //         ppm.pm.planeBytes=0;
      //         ppm.pm.pmHandle=sizeof(PixPatMap) + ds.dsBmih.biSizeImage;
      //         ppm.pm.pmReserved=0;

      //         uptr p((char*)&ppm);
      //         p.swaps(1); p.swapl(3);
      //         p.swaps(1); p.swapl(1); p.skip(8);
      //         p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
      //         p.swapl(3);	p.swaps(4); p.swapl(3);
      //         mf.Write(&ppm,sizeof(PixPatMap));

      //         char* lpBits=(char*)ds.dsBm.bmBits;
      //         char* pBuf=new char[ds.dsBm.bmWidthBytes];
      //         for(int y=ds.dsBmih.biHeight - 1; y >= 0; y--)
      //         {
      //            memcpy(pBuf,lpBits + y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
      //            p.pc=pBuf;
      //            switch(ds.dsBmih.biBitCount)
      //            {
      //            case 16:
      //               p.swaps(ds.dsBm.bmWidthBytes >> 1);
      //               break;
      //            case 24:
      //               p.swapRGB(ds.dsBm.bmWidthBytes / 3);
      //               break;
      //            case 32:
      //               p.swapl(ds.dsBm.bmWidthBytes >> 2);
      //               break;
      //            }
      //            mf.Write(pBuf,ds.dsBm.bmWidthBytes);
      //         }
      //         delete[] pBuf;
      //         if(ds.dsBmih.biBitCount<16)
      //         {
      //            short count=(short)ds.dsBmih.biClrUsed;
      //            CWindowDC dcScreen(NULL);
      //            CDC memDC;
      //            memDC.CreateCompatibleDC(&dcScreen);
      //            HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
      //            LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
      //            GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
      //            memDC.SelectObject(hOldBmp);

      //            ColorTable ct;
      //            ct.ctSeed=0;
      //            ct.ctFlags=0;
      //            ct.ctSize=count - 1;
      //            uptr p((char*)&ct);
      //            p.swapl(1); p.swaps(2);
      //            mf.Write(&ct,sizeof(ColorTable) - sizeof(ColorSpec));
      //            for(short c=0; c<count; c++)
      //            {
      //               ColorSpec cs;
      //               cs.value=c;
      //               cs.rgb.red=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
      //               cs.rgb.green=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
      //               cs.rgb.blue=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
      //               uptr((char*)&cs).swaps(4);
      //               mf.Write(&cs,sizeof(ColorSpec));
      //            }
      //            GlobalFree(GlobalHandle(lpColors));
      //         }
      //         else
      //         {
      //            ColorTable ct;
      //            memset(&ct,0,sizeof(ColorTable));
      //            mf.Write(&ct,sizeof(ColorTable));
      //         }

      //         WriteMFResource(fo,mdf.numBytes,mf,'ppat',ppatID);

      //         memset(&phf,0,sizeof(PhFillDescript));
      //         phf.fColor=1;
      //         phf.fResID=ppatID++;
      //         //=============================================================================
      //      }
      //      uptr((char*)&phf).swaps(2);
      //      mf.Write(&phf,sizeof(PhFillDescript));

      //      // by TSM
      //      if(i<m_numDIBPatterns) ::DeleteObject(hDib);
      //      // end by TSM
      //   }

      //   WriteMFResource(fo,mdf.numBytes,mf,'PLDT',128);
      //}

      //// gradients
      //{

      //   // Infinisys Ltd
      //   CMemFile mf;

      //   mf.Write(&m_numGradients,sizeof(m_numGradients));

      //   for(int i=0; i<m_numGradients; i++)
      //   {
      //      m_Gradients[i].Save55(mf);
      //   }

      //   WriteMFResource(fo,mdf.numBytes,mf,'GRAD',128);
      //}

      //// packed items
      //{

      //   // Infinisys Ltd
      //   CMemFile mf;

      //   int iSize = m_packeditema.GetCount();

      //   mf.Write(&iSize,sizeof(iSize));

      //   for(int i=0; i<iSize; i++)
      //   {
      //      mf.Write(&m_packeditema[i].m_etype,sizeof(m_packeditema[i].m_etype));
      //      int iIndex = m_packeditema[i].get_index(this);
      //      mf.Write(&iIndex,sizeof(iIndex));
      //   }

      //   WriteMFResource(fo,mdf.numBytes,mf,'PCKI',128);
      //}

      //// saved views
      //if(m_SavedViews.GetSize())
      //      
      //   CMemFile mf;

      //   memset(&list,0,sizeof(PhFListHeader));
      //   list.fNumElems=m_SavedViews.GetSize();
      //   list.fElemSize=sizeof(PhSavedView);
      //   list.fGrowSize=sizeof(PhSavedView);
      //   list.fKeylen=kMaxViewName;

      //   uptr p((char*)&list);
      //   p.swaps(1); p.swapl(1); p.swaps(1);
      //   p.swapl(3); p.swaps(3); p.swapl(1);
      //   mf.Write(&list,sizeof(PhFListHeader));

      //   PhSavedView phsv;
      //   for(short i=0; i<m_SavedViews.GetSize(); i++)
      //   {
      //      CSavedView* pView=(CSavedView*)m_SavedViews[i];
      //      ctx.ConvertC2P(pView->m_sTitle,phsv.fViewName,kMaxViewName);
      //      phsv.fOldScroll.x=(short)pView->m_ptOrg.x;
      //      phsv.fOldScroll.y=(short)pView->m_ptOrg.y;
      //      phsv.fZoomFactor=pView->m_nZoom;
      //      phsv.fCmdKey=pView->m_nShortcut?('4' + pView->m_nShortcut):0;

      //      p.pc=(char*)&phsv;
      //      p.skip(kMaxViewName); p.skip(2); p.swapl(1); p.swaps(1);
      //      mf.Write(&phsv,sizeof(PhSavedView));
      //   }

      //   WriteMFResource(fo,mdf.numBytes,mf,'VIEW',1);
      //}

      //// reports
      //if(m_Reports.GetSize())
      //{
      //   for(int r=0; r<m_Reports.GetSize();)
      //   {
      //      CMDReport* pReport=m_Reports[r++];

      //      PhReport phr;
      //      memset(&phr,0,sizeof(PhReport));
      //      phr.cVersion=3;
      //      ctx.ConvertC2P(pReport->m_sTitle,phr.cfTitle,64);
      //      TextParams tp;
      //      tp.m_lf=pReport->m_LogFont;
      //      ctx.ConvertTextParams(tp,phr.cDefaultTextParams);
      //      for(int i=0; i<6; i++) phr.cfColumnWidths[i]=pReport->m_nColumnWidths[i];
      //      for(int i=0; i<4; i++)
      //         ctx.ConvertC2P(m_sFieldNames[i + 1],phr.cFieldNames[i],32);
      //      phr.cfNumRows=pReport->m_Rows.GetSize();

      //      uptr p((char*)&phr);
      //      p.swaps(2); p.swapl(1); p.skip(64);
      //      p.swapl(1); p.swaps(1);
      //      p.swaps(2); p.skip(2); p.swaps(1); p.skip(2); p.swaps(3); // tp
      //      p.swaps(13); p.skip(32 * 4);
      //      p.swaps(1); p.swapl(1); p.swaps(4);
      //      p.skip(2); p.swapl(1); p.swaps(1); p.swapl(1);
      //      p.swapl(1); p.swaps(1); p.swapl(1); p.skip(64);
      //      p.swaps(2); p.skip(32);
      //      p.swapl(1); p.swaps(2); p.swapl(2); p.swaps(2); p.swapl(1);
      //      p.swapl(2);

      //      WriteResource(fo,mdf.numBytes,&phr,sizeof(PhReport),'mdrp',r);

      //      PhRowInfo* pri=new PhRowInfo[pReport->m_Rows.GetSize()];
      //      short rowCount[3]= {0,0,0};
      //      CMemFile mf[3];
      //      for(int i=0; i<3; i++)
      //      {
      //         short cnt=20 << 8;	// swapped
      //         mf[i].Write(&cnt,sizeof(short));
      //      }

      //      p.pc=(char*)pri;
      //      for(int i=0; i<pReport->m_Rows.GetSize(); i++)
      //      {
      //         CMDReportRow* pReportRow=(CMDReportRow*)pReport->m_Rows[i];
      //         short typeIdx=pReportRow->m_nRowType - 1;
      //         pri[i].fRowFlags=0;
      //         pri[i].fRowID=0;
      //         pri[i].fRowData=1;

      //         switch(pReportRow->m_nRowType)
      //         {
      //         case kTextRow:
      //         {
      //            short count=sizeof(PhTextRowData);
      //            pri[i].fRowType=kPhTextRow;
      //            PhTextRowData ptrd;
      //            ptrd.fChanged=0;
      //            ptrd.fRowType=kPhTextRow;
      //            ptrd.fSpare=0;
      //            for(int j=0; j<6; j++)
      //            {
      //               ptrd.fJust[j]=pReportRow->m_nJust[j];
      //               count+=pReportRow->m_sRowText[j].GetLength() + 2;
      //            }
      //            ptrd.fUseStyle=0;

      //            uptr p((char*)&ptrd);
      //            p.swaps(1); p.skip(2);
      //            p.skip(6); p.swaps(1);// p.skip(12);

      //            uptr(&count).swaps(1);
      //            mf[typeIdx].Write(&count,sizeof(short));
      //            mf[typeIdx].Write(&ptrd,sizeof(PhTextRowData));
      //            for(int j=0; j<6; j++)
      //            {
      //               short len=pReportRow->m_sRowText[j].GetLength();
      //               mf[typeIdx].Write(&len,1);
      //               mf[typeIdx].Write(pReportRow->m_sRowText[j],len + 1);
      //            }
      //         }
      //         break;
      //         case kCriteriaRow:
      //         {
      //            pri[i].fRowType=kPhCriteriaRow;
      //            PhCriteriaRowData pcrd;
      //            pcrd.fChanged=0;
      //            pcrd.fRowType=kPhCriteriaRow;
      //            pcrd.fSpare=0;
      //            for(int j=0; i<6; i++) pcrd.fFieldRefNum[j]=pReportRow->m_nFieldRefNum[j];

      //            uptr p((char*)&pcrd);
      //            p.swaps(1); p.skip(2); p.swaps(6);
      //            short count=sizeof(PhCriteriaRowData);
      //            uptr(&count).swaps(1);
      //            mf[typeIdx].Write(&count,sizeof(short));
      //            mf[typeIdx].Write(&pcrd,sizeof(PhCriteriaRowData));
      //         }
      //         break;
      //         case kFunctionRow:
      //         {
      //            short count=sizeof(PhFunctRowData);
      //            pri[i].fRowType=kPhFunctionRow;
      //            PhFunctRowData pfrd;
      //            pfrd.fChanged=0;
      //            pfrd.fRowType=kPhFunctionRow;
      //            pfrd.fSpare=(uchar)pReportRow->m_nFunction;
      //            if(pfrd.fSpare >= kFunctTotalName) pfrd.fSpare++;
      //            for(int j=0; j<6; j++)
      //            {
      //               pfrd.fJust[j]=pReportRow->m_nJust[j];
      //               count+=pReportRow->m_sRowText[j].GetLength() + 2;
      //            }
      //            pfrd.fResult=0;
      //            pfrd.dummy[0]=0;
      //            pfrd.dummy[1]=0;
      //            pfrd.fUseStyle=0;

      //            uptr p((char*)&pfrd);
      //            p.swaps(1); p.skip(2);
      //            p.skip(6); p.skip(8); p.swaps(3);
      //            uptr(&count).swaps(1);
      //            mf[typeIdx].Write(&count,sizeof(short));
      //            mf[typeIdx].Write(&pfrd,sizeof(PhFunctRowData));
      //            for(int j=0; j<6; j++)
      //            {
      //               short len=pReportRow->m_sRowText[j].GetLength();
      //               mf[typeIdx].Write(&len,1);
      //               mf[typeIdx].Write(pReportRow->m_sRowText[j],len + 1);
      //            }
      //         }
      //         break;
      //         }

      //         rowCount[typeIdx]++;
      //         if(rowCount[typeIdx] % 20 == 0)
      //         {
      //            WriteMFResource(fo,mdf.numBytes,mf[typeIdx],::reportRowType[typeIdx],100 * ((rowCount[typeIdx] / 20) - 1) + r);

      //            short cnt=20 << 8;	// swapped
      //            mf[typeIdx].Write(&cnt,sizeof(short));
      //         }
      //         p.skip(2); p.swaps(1); p.swapl(1);
      //      }

      //      for(int i=0; i<3; i++)
      //      {
      //         if(rowCount[i] % 20>0)
      //         {
      //            long length=mf[i].GetLength();
      //            void* pl=mf[i].Detach();
      //            *((short*)pl)=(rowCount[i] % 20) << 8;	// swapped
      //            WriteResource(fo,mdf.numBytes,pl,length,::formatMD70::reportRowType[i],100 * (rowCount[i] / 20) + r);
      //            free(pl);
      //         }
      //      }

      //      WriteResource(fo,mdf.numBytes,pri,pReport->m_Rows.GetSize()*sizeof(PhRowInfo),'mdrl',r);
      //      delete[] pri;
      //   }
      //}

      //mdf.offset=mdf.numBytes + sizeof(::formatMD70::MDFooter);
      //uptr((char*)&mdf).swapl(7);
      //fo->Write(&mdf,sizeof(::formatMD70::MDFooter));

      ReleaseFile(fo,FALSE);

   }
   CATCH_ALL(e)
   {
      ReleaseFile(fo,TRUE);
      TRY
      {
         ReportSaveLoadException(szPath,e,TRUE,AFX_IDP_FAILED_TO_SAVE_DOC);
      }
      END_TRY
      //		e->Delete();
      return false;
   }
   END_CATCH_ALL

   return true;
}


namespace formatMD70
{


   bool OffsetHeader::write(struct pointer & pointer,memory & mem,CFile * pfileDst,int iMarker,CFile & fileSrc,int iLen)
   {

      mem.allocate(fileSrc.GetLength());

      fileSrc.SeekToBegin();

      fileSrc.Read(mem.get_data(),mem.get_size());

      uiOffset = pfileDst->GetPosition();

      pfileDst->Write(mem.get_data(),mem.get_size());

      if(iLen >= 0)
      {

         uiLength = iLen;

      }
      else
      {

         uiLength = mem.get_size();

      }

      write(pointer,pfileDst,iMarker);

      return true;

   }

   bool OffsetHeader::write(struct pointer & pointer,CFile * pfileDst,int iMarker)
   {

      ULONGLONG ullLastOffset = pfileDst->GetPosition();

      pfileDst->Seek(iMarker,CFile::begin);

      pointer.synch_type(this);

      pointer.swap(uiOffset);

      pointer.swap(uiLength);

      pfileDst->Write(this,sizeof(*this));

      pfileDst->Seek(ullLastOffset,CFile::begin);

      return true;

   }

} // namespace formatMD70


