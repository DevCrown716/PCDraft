#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "FilletContext.h"
#include "ChamferContext.h"

#include "Layer.h"
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

#include "Import.h"

using namespace format55;

const Coord kAlmostSameCriteria = 400.0 / 65536.0;


namespace format55
{


   uchar mac2pc[256] ={
      //x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF

      0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
      0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
      0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
      0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
      0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
      0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
      0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
      0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
      0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
      0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
      0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xD8,
      0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
      0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
      0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0x00,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
      0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
      0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
   };


   uchar pc2mac[256]={
      //x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
      0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
      0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
      0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
      0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
      0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
      0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
      0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
      0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
      0xDB,0x81,0xE2,0xC4,0xE3,0xC9,0xA0,0xE0,0xF6,0xE4,0x8A,0xDC,0xCE,0x8D,0x8E,0x8F,
      0x90,0xD4,0xD5,0xD2,0xD3,0xA5,0xD0,0xD1,0xF7,0xAA,0x9A,0xDD,0xCF,0x9D,0x9E,0xD9,
      0xCA,0xC1,0xA2,0xA3,0xA4,0xB4,0xA6,0xA4,0xAC,0xA9,0xBB,0xC7,0xC2,0xAD,0xA8,0xF8,
      0xA1,0xB1,0xB2,0xB3,0xAB,0xB5,0xA6,0xE1,0xFC,0xB9,0xBC,0xC8,0xBC,0xBD,0xBE,0xC0,
      0xCB,0xE7,0xE5,0xCC,0x80,0x81,0xAE,0x82,0xE9,0x83,0xE6,0xE8,0xED,0xEA,0xEB,0xEC,
      0xD0,0x84,0xF1,0xEE,0xEF,0xCD,0x85,0xD7,0xAF,0xF4,0xF2,0xF3,0x86,0xDD,0xDE,0xA7,
      0x88,0x87,0x89,0x8B,0x8A,0x8C,0xBE,0x8D,0x8F,0x8E,0x90,0x91,0x93,0x92,0x94,0x95,
      0xF0,0x96,0x98,0x97,0x99,0x9B,0x9A,0xD6,0xBF,0x9D,0x9C,0x9E,0x9F,0xFD,0xFE,0xD8
   };
   short uptr::swaps()
   {
      char t;
      t=pc[1]; pc[1]=pc[0]; pc[0]=t;
      return *ps++;
   }
   void uptr::swaps(int count)
   {
      char t;
      while(count--)
      {
         t=pc[1]; pc[1]=pc[0]; pc[0]=t;
         ps++;
      }
   }
   long uptr::swapl()
   {
      char t;
      t=pc[3]; pc[3]=pc[0]; pc[0]=t;
      t=pc[2]; pc[2]=pc[1]; pc[1]=t;
      return *pl++;
   }
   void uptr::swapl(int count)
   {
      char t;
      while(count--)
      {
         t=pc[3]; pc[3]=pc[0]; pc[0]=t;
         t=pc[2]; pc[2]=pc[1]; pc[1]=t;
         pl++;
      }
   }
   void uptr::swaplpt(int count)
   {
      long t;
      while(count--)
      {
         t=pl[0]; pl[0]=pl[1]; pl[1]=t;
         swapl(2);
      }
   }
   void uptr::swapRGB(int count)
   {
      char t;
      while(count--)
      {
         t=pc[2]; pc[2]=pc[0]; pc[0]=t;
         pc+=3;
      }
   }
   void uptr::swapPhDocState()
   {
      swapl(1); swaps(2); // PhDocHeader

      swaps(1);			// TPrint
      swaps(3); swapl(2);	// TPrInfo
      swapl(2);
      swaps(3); skip(2);	// TPrStl
      swaps(3); swapl(2);	// TPrInfo
      swaps(5); skip(6);	// TPrXInfo
      swaps(3); skip(2); swapl(2); swaps(1); skip(2);	// TPrInfo
      swaps(19);

      swapl(2); swaps(1); swapl(2);
      swaps(4); /*skip(2); swaps(1);*/ swapl(1); swaps(2); // PhDocUnits

      swaps(1); swapl(10);
      swaps(1); swapl(10);

      swaps(1); skip(6); swaps(4); swapl(3);
      swaps(1); skip(6); swaps(4); swapl(3);
      swaps(1); skip(6); swaps(4); swapl(3);

      swaps(1); skip(6); swaps(4); swapl(3);
      swaps(1); skip(6); swaps(4); swapl(3);
      swaps(1); skip(6); swaps(4); swapl(3);
      swaplpt(1);

      swaps(1);

      swaps(2);	// fm
      swaps(1); skip(2); swaps(2); // pm

      swaps(2); skip(2); swaps(1); skip(2); swaps(3); // tp

      swaps(2); swapl(1); swaps(1);

      skip(4); swaps(2); swaplpt(1);
      skip(4); swaps(1); skip(2); swaps(1); skip(2);

      swapl(1); skip(2); // fillet attr
      swapl(2); skip(2); // chamfer attr
   }
   void uptr::swapDBPoint(int count)
   {
      while(count--)
      {
         skip(2); swaplpt(1);
      }
   }
   void uptr::swapDSShape()
   {
      swapl(1); swaps(1); swapl(3);
      swaps(1); skip(2); swaps(3); // pm
      swaplpt(1); swaps(1); swapl(1);
   }
   char* uptr::p2c(int count)
   {
      char *s=pc;
      short len=*(uchar*)pc;
      if(count) count-=len + 1;
      uchar* p=(uchar*)pc + 1;
      while(len--) *pc++=mac2pc[*p++];
      *pc++=0;
      pc+=count;
      return s;
   }

   char* uptr::p2c_nt(int count)
   {
      char *s=pc;
      short len=*(uchar*)pc;
      if(count) count-=len + 1;
      uchar* p=(uchar*)pc + 1;
      while(len--) *pc++=*p++;
      *pc++=0;
      pc+=count;
      return s;
   }

#pragma pack(1)
   struct MDFooter
   {
      // if this is not 'RSRC' then read another footer, sizeof(MDFooter) back from this record
      long offset;		// offset from the end of file to the first resource
      long numBytes;		// the number of bytes that this "fork" contains
      long type;			// current 'RSRC' but may be adding another type in future versions
      long version;		// currently 0x00010000
      long size;			// always sizeof (MDFooter); added longs will be added at the BEGINNING of this struct
      long tag1;			// always 'RBAL'
      long tag2;			// always 'RPH '. if it is not these then it is not a MacDraft footer
   };

   struct MDResource
   {
      long offset;			// offset to the next resource
      long length;			// size of the resource data after the resource name (see below)
      unsigned long resType;
      short resId;
      unsigned char flags;	// the standard Mac resource flags (locked, purgeable, resource, etc)
      unsigned char resourceNameLength;		// usually zero
      // if not zero, the next MDResource.resourceNameLength number of bytes is the resource name
      // now comes the resource data of size MDResource.length
   };
#pragma pack()

   void swapMDR(char* resFork,long len)
   {
      union
      {
         char* pc;
         MDResource* mdr;
      } p;
      p.pc=resFork;
      while(len)
      {
         uptr pu(p.pc);
         pu.swapl(3); pu.swaps(1);
         len-=p.mdr->offset;
         p.pc+=p.mdr->offset;
      }
   }

   char* GetResource(unsigned long resType,short resId,char* resFork,long len,long* pLen=NULL)
   {
      union
      {
         char* pc;
         MDResource* mdr;
      } p;
      p.pc=resFork;
      while(len)
      {
         if(p.mdr->resType == resType && p.mdr->resId == resId)
         {
            char* res=new char[p.mdr->length];
            memcpy(res,p.pc + sizeof(MDResource) + p.mdr->resourceNameLength,p.mdr->length);
            if(pLen) *pLen=p.mdr->length;
            return res;
         }
         len-=p.mdr->offset;
         p.pc+=p.mdr->offset;
      }
      return NULL;
   }

   unsigned long reportRowType[3]= {'mrt#','mrc#','mrf#'};



//void WriteResource(CFile* fo,long& forkLen,void* pRes,long len,unsigned long resType,short resId,const CString& resName="")
//{
//   MDResource mdr;
//   mdr.resourceNameLength=resName.GetLength();
//   mdr.length=len;
//   mdr.offset=16 + mdr.resourceNameLength + mdr.length;
//   mdr.resType=resType;
//   mdr.resId=resId;
//   mdr.flags=0;
//
//   forkLen+=mdr.offset;
//   uptr p((char*)&mdr);
//   p.swapl(3); p.swaps(1);
//   fo->Write(&mdr,sizeof(MDResource));
//   if(mdr.resourceNameLength) fo->Write(LPCSTR(resName),mdr.resourceNameLength);
//   fo->Write(pRes,len);
//}
//
//void WriteMFResource(CFile* fo,long& forkLen,CMemFile& mf,unsigned long resType,short resId,const CString& resName)
//{
//   long length=mf.GetLength();
//   if(length)
//   {
//      void* pl=mf.Detach();
//      WriteResource(fo,forkLen,pl,length,resType,resId,resName);
//      free(pl);
//   }
//}

} // namespace format55


bool CPCDraftDoc::SavePCDraft55(const char *szPath)
{
   CFileException fe;
   CFile* fo=GetFile(szPath,CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive,&fe);
   if(fo == NULL)
   {
      ReportSaveLoadException(szPath,&fe,TRUE,AFX_IDP_INVALID_FILENAME);
      return false;
   }
   TRY
   {
      CWaitCursor wait;

      ::format55::PhIOContext ctx(fo);
      ctx.m_numDIBPatterns = m_numDIBPatterns;
      ctx.pPalette = &m_Palette;
      ctx.pDoc = this;

      ::format55::MDFooter mdf;
      mdf.numBytes = 0;
      mdf.tag1 = 'RBAL';
      mdf.tag2 = 'RPH ';
      mdf.version = 0x10000;
      mdf.type = 'RSRC';
      mdf.size = sizeof(::format55::MDFooter);

      PhDocState phDocState;
      memset(&phDocState,0,sizeof(PhDocState));
      phDocState.fType = kPhDocType;
      phDocState.fHeaderID = kPhFileHdr;
      phDocState.fVersion = kPhRelease55;

      phDocState.fPrintRec.prInfo.iHRes = 72;
      phDocState.fPrintRec.prInfo.iVRes = 72;
      phDocState.fPrintRec.prInfo.rPage.r = (short)m_PageSize.cx;
      phDocState.fPrintRec.prInfo.rPage.b = (short)m_PageSize.cy;
      phDocState.fPrintRec.prStl.wDev = (m_PrinterSettings.GetPageOrientation() == DMORIENT_LANDSCAPE) ? 869 : 871;
      //phDocState.fPrintRec.prStl.iPageV = 1320; // 2019 and before
      //phDocState.fPrintRec.prStl.iPageH = 1020; // 2019 and before
      //phDocState.fPrintRec.prStl.feed = 2; // 2019 and before
      CSize sizeCustomPage = m_PrinterSettings.GetCustomPageSize();
      phDocState.fPrintRec.prStl.iPageV = sizeCustomPage.cy;
      phDocState.fPrintRec.prStl.iPageH = sizeCustomPage.cx;
      if (m_PrinterSettings.GetPageSize() == 0 || m_PrinterSettings.GetPageSize() == DMPAPER_USER)
      {
         phDocState.fPrintRec.prStl.feed = 0;
      }
      else
      {
         phDocState.fPrintRec.prStl.feed = m_PrinterSettings.GetPageSize();
      }

      //		phDocState.fWindowRect
#ifdef _FLOAT_ZOOM
      phDocState.fZoom=PackZoomFactor(m_nZoomFactor,m_fZoomFactor);
#else
      phDocState.fZoom=m_nZoomFactor;
#endif
      phDocState.fMViewPages.x=(short)m_NumPage.x;
      phDocState.fMViewPages.y=(short)m_NumPage.y;

      phDocState.fDocUnits.fDefUnits=m_nUnit;
      phDocState.fDocUnits.fDefPlaces=m_nPlaces;
      phDocState.fDocUnits.fDefAngleUnits=GetAngleUnit();
      phDocState.fDocUnits.fDefAngleUnits=phDocState.fDocUnits.fDefAngleUnits<0?1024 - phDocState.fDocUnits.fDefAngleUnits:(phDocState.fDocUnits.fDefAngleUnits << 8);
      phDocState.fDocUnits.fDefAnglePlaces=m_nAnglePlaces;
      phDocState.fDocUnits.fDefResolution.x=m_nDisplayRes;
      phDocState.fDocUnits.fDefResolution.y=m_nDisplayRes;
      phDocState.fDocUnits.fDefScaleIndex=m_nDefScale;
      phDocState.fDocUnits.fDefScaleRatio=m_ratioCustom;
      if(!m_bEnglishUnits && phDocState.fDocUnits.fDefScaleIndex>20) phDocState.fDocUnits.fDefScaleIndex++;
      phDocState.fDocUnits.fDefSnapIndex=4;

      if(m_bEnglishUnits)
      {
         DimStandard std;
         std.FillDefaults(kDsMetric);
         ctx.ConvertDimStd(m_DimStd,phDocState.fDocStds.fEnglishDimStd);
         ctx.ConvertDimStd(std,phDocState.fDocStds.fMetricDimStd);
         for(int i=0; i<3; i++)
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
         for(int i=0; i<3; i++)
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

      //		phDocState.fActiveEnvirons.fDimVisible=0;
      phDocState.fActiveEnvirons.fHasGrid=!m_bHideGridLines;
      phDocState.fActiveEnvirons.fHideText=m_bHideText;
      phDocState.fActiveEnvirons.fShowRulers=m_bShowRulers;
      phDocState.fActiveEnvirons.fPrevZoomScroll.x=(short)m_ptPrevZoomScroll.x;
      phDocState.fActiveEnvirons.fPrevZoomScroll.y=(short)m_ptPrevZoomScroll.y;
      //		phDocState.fActiveEnvirons.fRulerOrigin
      phDocState.fActiveEnvirons.fSnapToObject=m_bSnapToObject;
      phDocState.fActiveEnvirons.fStandardRulers=m_bStandardRulers;
      phDocState.fActiveEnvirons.fHidePageBreaks=m_bHidePageBreaks;
      //		phDocState.fActiveEnvirons.fPlotterMode=0;
      //		phDocState.fActiveEnvirons.fPrintFlags=0;
      if(m_bPrintLastPageFirst) phDocState.fActiveEnvirons.fPrintFlags|=kPhPrintLastFirst;
      if(m_nPageNumbering == 0) phDocState.fActiveEnvirons.fPrintFlags|=kPhPrintByCols;
      if(m_bRegistrationMarks) phDocState.fActiveEnvirons.fPrintFlags|=kPhRegMarks;

      //		phDocState.fActiveEnvirons.fDimLineFlag
      //		phDocState.fActiveEnvirons.fDimTextAlgn;
#ifdef _FLOAT_ZOOM
      phDocState.fActiveEnvirons.fPrevZoom=(char)PackZoomFactor(m_nPrevZoomFactor,m_fPrevZoomFactor);
#else
      phDocState.fActiveEnvirons.fPrevZoom=m_nPrevZoomFactor;
#endif

      phDocState.fActiveEnvirons.fAngleSnapsToACirclePart1=m_snapTable.getAngleSnaps() >> 8;
      phDocState.fActiveEnvirons.fAngleSnapsToACirclePart2=(char)m_snapTable.getAngleSnaps();

      CPCDraftView* pView=NULL;
      POSITION pos=GetFirstViewPosition();
      if(pos) pView=(CPCDraftView*)GetNextView(pos); // 1 view assumed
      if(pView)
      {
         phDocState.fOldScroll.x=(short)pView->GetDeviceScrollPosition().x;
         phDocState.fOldScroll.y=(short)pView->GetDeviceScrollPosition().y;

         phDocState.fActiveEnvirons.fFilletAttr.fIsSmallerArc=pView->m_pFilletCtx->m_bSmallerArc;
         phDocState.fActiveEnvirons.fFilletAttr.fRadius=pView->m_pFilletCtx->m_fRadius;
         phDocState.fActiveEnvirons.fFilletAttr.fTrimMethod=(kNoTrim - pView->m_pFilletCtx->m_nTrimMethod);

         phDocState.fActiveEnvirons.fChamferAttr.fLine1SetBack=pView->m_pChamferCtx->m_fLine1SetBack;
         phDocState.fActiveEnvirons.fChamferAttr.fLine2SetBack=pView->m_pChamferCtx->m_fLine2SetBack;
         phDocState.fActiveEnvirons.fChamferAttr.fTrimMethod=(kNoTrim - pView->m_pChamferCtx->m_nTrimMethod);
      }

      uptr((char*)&phDocState).swapPhDocState();
      fo->Write(&phDocState,sizeof(PhDocState));

      CMemFile mf;
      PhFListHeader list;
      memset(&list,0,sizeof(PhFListHeader));
      list.fNumElems=m_Layers.GetSize();
      list.fElemSize=sizeof(PhLayerStruct);
      list.fGrowSize=sizeof(PhLayerStruct);
      list.fKeylen=kPhMaxLayerNameLen;

      uptr p((char*)&list);
      p.swaps(1); p.swapl(1); p.swaps(1);
      p.swapl(3); p.swaps(3); p.swapl(1);
      mf.Write(&list,sizeof(PhFListHeader));

      // linked vertexes
      ctx.nObjectIndex=0;
      CLinkVertexList linkVertexList;
      ctx.pLinkVertexList=&linkVertexList;

      CPtrArray linkedVertexResPtr;
      CDWordArray linkedVertexResLen;

      PhComplexPtPicData cppd;
      cppd.fSignature='MD40';
      cppd.fKind=0x3B0;
      cppd.fSpare1S=0;
      cppd.fSpare2L=0;
      cppd.fSpare3L=0;
      cppd.fVersion=0;
      cppd.fComplexType=kPhComplexPtLinkPoint;

      uptr pp((char*)&cppd);
      pp.swapl(1); pp.swaps(2); pp.swapl(4);

      for(int i=0; i<m_Layers.GetSize(); i++)
      {
         CLayer* pLayer=m_Layers[i];

         PhLayerStruct layer;
         CString strName = pLayer->m_strName;
         strName = get_cp(get_uni(strName), CP_ACP); // Infinisys Ltd.
         lstrcpyn(layer.fLayerName,strName,kPhMaxLayerNameLen);
         layer.fLayerFlags=kPhSelectable | kPhScaled | kPhFillsOn;
         if(i == m_nActiveLayer) layer.fLayerFlags|=kPhActive;
         if(!pLayer->m_bGrayed) layer.fLayerFlags|=kPhNotGrayed;
         if(!pLayer->m_bHidden) layer.fLayerFlags|=kPhVisible;
         layer.fScaleIndex=pLayer->m_nScale;
         if(!m_bEnglishUnits && layer.fScaleIndex>20) layer.fScaleIndex++;
         layer.fSnapIndex=pLayer->m_nSnap - 1;
         layer.fNumShapes=0;
         layer.fLayerOffset=fo->GetPosition();

         fo->Write(&layer.fNumShapes,sizeof(long));

         linkVertexList.Copy(pLayer->m_LinkVertexList);
         POSITION pos=pLayer->m_ShapeList.GetTailPosition();
         while(pos)
         {
            CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
            layer.fNumShapes+=pShape->SavePCDraft55(ctx);
         }
         layer.fLayerLength=fo->GetPosition();
         if(layer.fNumShapes)
         {
            long count=layer.fNumShapes;
            uptr((char*)&count).swapl(1);
            fo->Seek(layer.fLayerOffset,CFile::begin);
            fo->Write(&count,sizeof(long));
            fo->Seek(layer.fLayerLength,CFile::begin);
         }
         layer.fLayerLength-=layer.fLayerOffset;

         uptr p((char*)&layer);
         p.skip(kPhMaxLayerNameLen);
         p.swaps(3); p.swapl(3);
         mf.Write(&layer,sizeof(PhLayerStruct));

         if(linkVertexList.GetCount())
         {
            long nPrevKey=0;
            CMemFile lvf;
            do
            {
               CLinkVertex *pLinkVertex=linkVertexList.RemoveHead();
               CLinkVertex* pLinkVertex2;
               if(!pLinkVertex->m_bResolved)
               {
                  PhLinkResInfo lri;
                  if(pLinkVertex->m_nKey != nPrevKey)
                  {
                     int len=lvf.GetLength();
                     if(len)
                     {
                        linkedVertexResLen.Add(len);
                        linkedVertexResPtr.Add(lvf.Detach());
                     }
                     nPrevKey=pLinkVertex->m_nKey;

                     if(pLinkVertex->m_nKey >= 0)
                     {
                        if(pLinkVertex->m_nVertex == kCenterHandle)
                        {
                           pLinkVertex2=linkVertexList.RemoveHead();//GetHead();
                           cppd.fComplexType=kPhComplexPtLinkPoint;
                           uptr((char*)&cppd.fComplexType).swapl();
                           lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                           lri.fObjectIndex=pLinkVertex->m_nShape;
                           lri.fPointIndex=2;
                           lri.fFlags=0;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));
                           lri.fObjectIndex=pLinkVertex2->m_nShape;
                           lri.fPointIndex=0;
                           lri.fFlags=1;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));
                           linkedVertexResLen.Add(lvf.GetLength());
                           linkedVertexResPtr.Add(lvf.Detach());

                           lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                           lri.fObjectIndex=pLinkVertex->m_nShape;
                           lri.fPointIndex=1;
                           lri.fFlags=0;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));
                           lri.fObjectIndex=pLinkVertex2->m_nShape;
                           lri.fPointIndex=0;
                           lri.fFlags=1;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));
                           linkedVertexResLen.Add(lvf.GetLength());
                           linkedVertexResPtr.Add(lvf.Detach());

                           lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                           lri.fObjectIndex=pLinkVertex->m_nShape;
                           lri.fPointIndex=0;
                           lri.fFlags=0;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));
                           lri.fObjectIndex=pLinkVertex2->m_nShape;
                           lri.fPointIndex=0;
                           lri.fFlags=1;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));
                           linkedVertexResLen.Add(lvf.GetLength());
                           linkedVertexResPtr.Add(lvf.Detach());

                           cppd.fComplexType=kPhComplexPtArcDimPoint;
                        }
                        else cppd.fComplexType=kPhComplexPtLinkPoint;
                        uptr((char*)&cppd.fComplexType).swapl();
                        lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                        if(pLinkVertex->m_nVertex == kCenterHandle)
                        {
                           lri.fObjectIndex=pLinkVertex2->m_nShape;
                           lri.fPointIndex=0;
                           lri.fFlags=0;
                           uptr((char*)&lri).swapl(3);
                           lvf.Write(&lri,sizeof(PhLinkResInfo));

                        }
                     }
                     else
                     {
                        // two more shapes
                        ASSERT(linkVertexList.GetCount()>1);
                        pLinkVertex2=linkVertexList.RemoveHead();

                        cppd.fComplexType=kPhComplexPtLinkPoint;
                        uptr((char*)&cppd.fComplexType).swapl();
                        lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                        lri.fObjectIndex=pLinkVertex2->m_nShape;
                        lri.fPointIndex=pLinkVertex2->m_nVertex;
                        lri.fFlags=0;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        lri.fObjectIndex=pLinkVertex->m_nShape;
                        lri.fPointIndex=pLinkVertex->m_nVertex;
                        lri.fFlags=1;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        linkedVertexResLen.Add(lvf.GetLength());
                        linkedVertexResPtr.Add(lvf.Detach());

                        CLinkVertex* pLinkVertex3=linkVertexList.RemoveHead();

                        lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                        lri.fObjectIndex=pLinkVertex3->m_nShape;
                        lri.fPointIndex=pLinkVertex3->m_nVertex;
                        lri.fFlags=0;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        lri.fObjectIndex=pLinkVertex->m_nShape;
                        lri.fPointIndex=pLinkVertex->m_nVertex;
                        lri.fFlags=1;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        linkedVertexResLen.Add(lvf.GetLength());
                        linkedVertexResPtr.Add(lvf.Detach());

                        cppd.fComplexType=kPhComplexPtAngularDimPoint;
                        uptr((char*)&cppd.fComplexType).swapl();
                        lvf.Write(&cppd,sizeof(PhComplexPtPicData));
                        lri.fObjectIndex=pLinkVertex->m_nShape;
                        lri.fPointIndex=pLinkVertex->m_nVertex;
                        lri.fFlags=0;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        lri.fObjectIndex=pLinkVertex3->m_nShape;
                        lri.fPointIndex=pLinkVertex3->m_nVertex;
                        lri.fFlags=1;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        lri.fObjectIndex=pLinkVertex2->m_nShape;
                        lri.fPointIndex=pLinkVertex2->m_nVertex;
                        lri.fFlags=1;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        lri.fObjectIndex=pLinkVertex->m_nShape;
                        lri.fPointIndex=pLinkVertex->m_nVertex;
                        lri.fFlags=1;
                        uptr((char*)&lri).swapl(3);
                        lvf.Write(&lri,sizeof(PhLinkResInfo));
                        linkedVertexResLen.Add(lvf.GetLength());
                        linkedVertexResPtr.Add(lvf.Detach());

                        delete pLinkVertex;
                        delete pLinkVertex2;
                        delete pLinkVertex3;
                        continue;
                     }
                  }
                  lri.fObjectIndex=pLinkVertex->m_nShape;
                  if(pLinkVertex->m_nVertex<0)
                  {
                     lri.fPointIndex=0;
                     lri.fFlags=1;
                  }
                  else
                  {
                     lri.fPointIndex=pLinkVertex->m_nVertex;
                     lri.fFlags=0;
                  }
                  uptr((char*)&lri).swapl(3);
                  lvf.Write(&lri,sizeof(PhLinkResInfo));

               }
               delete pLinkVertex;
            }
            while(linkVertexList.GetCount());
            int len=lvf.GetLength();
            if(len)
            {
               linkedVertexResLen.Add(len);
               linkedVertexResPtr.Add(lvf.Detach());
            }
         }
      }
      for(int i=0; i<ctx.metaMembers.GetSize(); i++)
      {
         CMDShape* pShape=(CMDShape*)ctx.metaMembers[i];
         delete pShape;
      }

      PhObjectInfoFieldSet fields;
      //ctx.ConvertC2P(m_sFieldNames[1],fields.fF2Name,31);
      //ctx.ConvertC2P(m_sFieldNames[2],fields.fF3Name,31);
      //ctx.ConvertC2P(m_sFieldNames[3],fields.fF4Name,31);
      //ctx.ConvertC2P(m_sFieldNames[4],fields.fF5Name,31);
      strncpy(fields.fF2Name, m_sFieldNames[1], 31);
      strncpy(fields.fF3Name, m_sFieldNames[2], 31);
      strncpy(fields.fF4Name, m_sFieldNames[3], 31);
      strncpy(fields.fF5Name, m_sFieldNames[4], 31);
      fo->Write(&fields,sizeof(PhObjectInfoFieldSet));

      // bitmaps
      if(ctx.bitmaps.GetSize())
      {
         CMemFile mf;
         memset(&list,0,sizeof(PhFListHeader));
         list.fNumElems=ctx.bitmaps.GetSize();
         list.fElemSize=sizeof(PhBitmapIOStruct);
         list.fInitSize=list.fNumElems*list.fElemSize;
         list.fGrowSize=sizeof(PhBitmapIOStruct);
         list.fKeylen=4;

         uptr p((char*)&list);
         p.swaps(1); p.swapl(1); p.swaps(1);
         p.swapl(3); p.swaps(3); p.swapl(1);
         mf.Write(&list,sizeof(PhFListHeader));

         for(int i=0; i<ctx.bitmaps.GetSize(); i++)
         {
            HBITMAP hDib=(HBITMAP)ctx.bitmaps[i];
            DIBSECTION ds;
            ::GetObject(hDib,sizeof(DIBSECTION),&ds);

            // something is wrong with 24-bit pict format so we'll make it 32-bit
            HBITMAP hCopy=NULL;
            if(ds.dsBmih.biBitCount == 24)
            {
               ds.dsBmih.biBitCount=32;
               LPVOID lpBits;
               HDC hdc=::GetDC(NULL);
               hCopy=::CreateDIBSection(hdc,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
               {
                  HDC hSrcDC,hDstDC;
                  hSrcDC=::CreateCompatibleDC(hdc);
                  hDstDC=::CreateCompatibleDC(hdc);
                  HGDIOBJ hSrcOldBmp=::SelectObject(hSrcDC,hDib);
                  HGDIOBJ hDstOldBmp=::SelectObject(hDstDC,hCopy);
                  ::BitBlt(hDstDC,0,0,ds.dsBmih.biWidth,ds.dsBmih.biHeight,hSrcDC,0,0,SRCCOPY);
                  ::SelectObject(hSrcDC,hSrcOldBmp);
                  ::SelectObject(hDstDC,hDstOldBmp);
                  VERIFY(::DeleteDC(hDstDC));
                  VERIFY(::DeleteDC(hSrcDC));
               }
               ::ReleaseDC(NULL,hdc);
               ::GetObject(hCopy,sizeof(DIBSECTION),&ds);
               hDib=hCopy;
            }

            ds.dsBm.bmWidthBytes+=0x3;
            ds.dsBm.bmWidthBytes&=~0x3;

            PixMap pm;
            pm.baseAddr=ds.dsBmih.biSizeImage;
            pm.rowBytes=0x8000 | ((short)ds.dsBm.bmWidthBytes);
            pm.bounds.l=0;
            pm.bounds.t=0;
            pm.bounds.r=(short)ds.dsBmih.biWidth;
            pm.bounds.b=(short)ds.dsBmih.biHeight;
            pm.pmVersion=0;
            pm.packType=0;
            pm.packSize=0;
            pm.hRes=0x00480000;
            pm.vRes=0x00480000;
            pm.pixelType=ds.dsBmih.biBitCount<16?0:16;
            pm.pixelSize=ds.dsBmih.biBitCount;
            pm.cmpCount=pm.pixelType?3:1;
            pm.cmpSize=(ds.dsBmih.biBitCount<16)?pm.pixelSize:(ds.dsBmih.biBitCount == 16)?5:8;
            pm.planeBytes=0;
            pm.pmHandle=(ds.dsBmih.biBitCount<16)?sizeof(ColorTable) + (ds.dsBmih.biClrUsed - 1)*sizeof(ColorSpec):sizeof(ColorTable);
            pm.pmReserved=0;

            long size=sizeof(PixMap) + pm.baseAddr + pm.pmHandle;
            PhBitmapIOStruct bitmapIO;
            bitmapIO.fBitmapOffset=ctx.f->GetPosition();
            bitmapIO.fBitmapLength=size + sizeof(long);
            bitmapIO.fColor=1;
            bitmapIO.fShareCount=1;
            bitmapIO.fUnused=0;

            uptr(&size).swapl(1);
            ctx.f->Write(&size,sizeof(long));

            uptr p((char*)&pm);
            p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
            p.swapl(3);	p.swaps(4); p.swapl(3);
            ctx.f->Write(&pm,sizeof(PixMap));

            char* lpBits=(char*)ds.dsBm.bmBits;
            char* pBuf=new char[ds.dsBm.bmWidthBytes];
            for(int y=ds.dsBmih.biHeight - 1; y >= 0; y--)
            {
               memcpy(pBuf,lpBits + y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
               p.pc=pBuf;
               switch(ds.dsBmih.biBitCount)
               {
               case 16:
                  p.swaps(ds.dsBm.bmWidthBytes >> 1);
                  break;
               case 24:
                  p.swapRGB(ds.dsBm.bmWidthBytes / 3);
                  break;
               case 32:
                  p.swapl(ds.dsBm.bmWidthBytes >> 2);
                  break;
               }
               ctx.f->Write(pBuf,ds.dsBm.bmWidthBytes);
            }
            delete[] pBuf;
            if(ds.dsBmih.biBitCount<16)
            {
               short count=(short)ds.dsBmih.biClrUsed;
               CWindowDC dcScreen(NULL);
               CDC memDC;
               memDC.CreateCompatibleDC(&dcScreen);
               HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
               LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
               GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
               memDC.SelectObject(hOldBmp);

               ColorTable ct;
               ct.ctSeed=0;
               ct.ctFlags=0;
               ct.ctSize=count - 1;
               uptr p((char*)&ct);
               p.swapl(1); p.swaps(2);
               ctx.f->Write(&ct,sizeof(ColorTable) - sizeof(ColorSpec));
               for(short c=0; c<count; c++)
               {
                  ColorSpec cs;
                  cs.value=c;
                  cs.rgb.red=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
                  cs.rgb.green=(lpColors[c].rgbGreen << 8) + lpColors[c].rgbGreen;
                  cs.rgb.blue=(lpColors[c].rgbBlue << 8) + lpColors[c].rgbBlue;
                  uptr((char*)&cs).swaps(4);
                  ctx.f->Write(&cs,sizeof(ColorSpec));
               }
               GlobalFree(GlobalHandle(lpColors));
            }
            else
            {
               ColorTable ct;
               memset(&ct,0,sizeof(ColorTable));
               ctx.f->Write(&ct,sizeof(ColorTable));
            }

            bitmapIO.fBitmapLength=ctx.f->GetPosition();
            bitmapIO.fBitmapLength-=bitmapIO.fBitmapOffset;

            p.pc=(char*)&bitmapIO;
            p.swapl(2); p.swaps(1); p.skip(2);
            mf.Write(&bitmapIO,sizeof(PhBitmapIOStruct));
            if(hCopy)
            {
               ::DeleteObject(hCopy);
               hCopy=NULL;
            }
         }

         WriteMFResource(fo,mdf.numBytes,mf,'BITL',0);
      }

      // layers
      WriteMFResource(fo,mdf.numBytes,mf,'LAYI',0);

      // linked vertexes
      int lvResId=256;
      for(int i=0; i<linkedVertexResLen.GetSize(); i++)
      {
         if(linkedVertexResPtr[i])
         {
            WriteResource(fo,mdf.numBytes,linkedVertexResPtr[i],linkedVertexResLen[i],'Link',lvResId++);
            free(linkedVertexResPtr[i]);
         }
      }

      // dash styles
      WriteResource(fo,mdf.numBytes,m_DashStyles,256,'DASH',128);

      // used fonts
      {
         CMemFile mf;
         for(short i=0; i<ctx.fontInUse.GetSize(); i++)
         {
            unsigned short font=(i + 1) << 8;
            mf.Write(&font,sizeof(short));
            CString strFont = ctx.fontInUse[i];
            strFont += "|";
            char sz[32];
            strFont += itoa(ctx.fontCS[i],sz,10);
            unsigned char len=strFont.GetLength();
            mf.Write(&len,1);
            mf.Write(strFont,len);
         }

         WriteMFResource(fo,mdf.numBytes,mf,'FNUS',1);
      }

      // palette
      {
         int nCount=m_Palette.GetEntryCount();
         long length=16 + 16 * nCount;
         char* pPal=new char[length];
         memset(pPal,0,length);
         p.pc=pPal;
         *p.ps=nCount;
         p.swaps();
         p.swapl(1); p.swaps(1); p.swapl(2);
         for(int i=0; i<nCount; i++)
         {
            PALETTEENTRY pe;
            m_Palette.GetPaletteEntries(i,1,&pe);
            *p.ps=pe.peRed << 8; p.swaps(1);
            *p.ps=pe.peGreen << 8; p.swaps(1);
            *p.ps=pe.peBlue << 8; p.swaps(1);
            *p.ps=0x2; // pmTolerant
            p.swaps(5);
         }
         WriteResource(fo,mdf.numBytes,pPal,length,'pltt',128);
         delete[] pPal;
         //m_Palette.ResizePalette(83);	// strip extra entries
      }

      // patterns
      {
         memset(&list,0,sizeof(PhFListHeader));
         list.fListID=128;
         list.fListType='PATL';
         // by TSM
         //list.fNumElems=72+ctx.embedDIBs.GetSize();
         list.fNumElems=1 + m_numDIBPatterns + ctx.embedDIBs.GetSize();
         list.fElemSize=sizeof(PhFillDescript);
         list.fInitSize=sizeof(PhFillDescript);
         list.fGrowSize=sizeof(PhFillDescript);
         list.fIDData=128;
         list.fDataType='PLDT';
         list.fKeylen=6;

         uptr p((char*)&list);
         p.swaps(1); p.swapl(1); p.swaps(1);
         p.swapl(3); p.swaps(3); p.swapl(1);
         WriteResource(fo,mdf.numBytes,&list,sizeof(PhFListHeader),'PATL',128);

         CMemFile mf;
         PhFillDescript phf;
         memset(&phf,0,sizeof(PhFillDescript));
         phf.fColor=1;
         phf.fResID=-1;
         uptr((char*)&phf).swaps(2);
         mf.Write(&phf,sizeof(PhFillDescript));
         short ppatID=128;

         // by TSM
         //for(int i=0;i<(71+ctx.embedDIBs.GetSize());i++)
         for(int i=0; i<m_numDIBPatterns + ctx.embedDIBs.GetSize(); i++)
         {
            // by TSM
            /*HBITMAP hDib;
            if(i<71) hDib=m_Patterns[i].CreateDIBSection();
            else hDib=(HBITMAP)ctx.embedDIBs[i-71];
            */
            HBITMAP hDib;
            if(i<m_numDIBPatterns) hDib=m_Patterns[i].CreateDIBSection();
            else hDib=(HBITMAP)ctx.embedDIBs[i - m_numDIBPatterns];
            //end by TSM
            DIBSECTION ds;
            ::GetObject(hDib,sizeof(DIBSECTION),&ds);
            if(ds.dsBmih.biBitCount == 1)
            {
               phf.fColor=0;
               phf.fResID=0;
               for(int j=0; j<8; j++)
               {
                  phf.fData.fPattern[j]=(uchar)~((LPDWORD)ds.dsBm.bmBits)[j];
               }
            }
            else
            {
               //=============================================================================
               CMemFile mf;
               PixPatMap ppm;
               ppm.pp.patType=0x1;
               ppm.pp.patMap=sizeof(PixPat);
               ppm.pp.patData=sizeof(PixPatMap);
               ppm.pp.patXData=0;
               ppm.pp.patXValid=-1;
               ppm.pp.patXMap=0;
               ppm.pp.pat1Data[0]=0xAA;
               ppm.pp.pat1Data[1]=0x55;
               ppm.pp.pat1Data[2]=0xAA;
               ppm.pp.pat1Data[3]=0x55;
               ppm.pp.pat1Data[4]=0xAA;
               ppm.pp.pat1Data[5]=0x55;
               ppm.pp.pat1Data[6]=0xAA;
               ppm.pp.pat1Data[7]=0x55;

               ppm.pm.baseAddr=0;
               ppm.pm.rowBytes=0x8000 | ((short)ds.dsBm.bmWidthBytes);
               ppm.pm.bounds.l=0;
               ppm.pm.bounds.t=0;
               ppm.pm.bounds.r=(short)ds.dsBmih.biWidth;
               ppm.pm.bounds.b=(short)ds.dsBmih.biHeight;
               ppm.pm.pmVersion=0;
               ppm.pm.packType=0;
               ppm.pm.packSize=0;
               ppm.pm.hRes=0x00480000;
               ppm.pm.vRes=0x00480000;
               ppm.pm.pixelType=ds.dsBmih.biBitCount<16?0:16;
               ppm.pm.pixelSize=ds.dsBmih.biBitCount;
               ppm.pm.cmpCount=ppm.pm.pixelType?3:1;
               ppm.pm.cmpSize=(ds.dsBmih.biBitCount<16)?ppm.pm.pixelSize:(ds.dsBmih.biBitCount == 16)?5:8;
               ppm.pm.planeBytes=0;
               ppm.pm.pmHandle=sizeof(PixPatMap) + ds.dsBmih.biSizeImage;
               ppm.pm.pmReserved=0;

               uptr p((char*)&ppm);
               p.swaps(1); p.swapl(3);
               p.swaps(1); p.swapl(1); p.skip(8);
               p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
               p.swapl(3);	p.swaps(4); p.swapl(3);
               mf.Write(&ppm,sizeof(PixPatMap));

               char* lpBits=(char*)ds.dsBm.bmBits;
               char* pBuf=new char[ds.dsBm.bmWidthBytes];
               for(int y=ds.dsBmih.biHeight - 1; y >= 0; y--)
               {
                  memcpy(pBuf,lpBits + y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
                  p.pc=pBuf;
                  switch(ds.dsBmih.biBitCount)
                  {
                  case 16:
                     p.swaps(ds.dsBm.bmWidthBytes >> 1);
                     break;
                  case 24:
                     p.swapRGB(ds.dsBm.bmWidthBytes / 3);
                     break;
                  case 32:
                     p.swapl(ds.dsBm.bmWidthBytes >> 2);
                     break;
                  }
                  mf.Write(pBuf,ds.dsBm.bmWidthBytes);
               }
               delete[] pBuf;
               if(ds.dsBmih.biBitCount<16)
               {
                  short count=(short)ds.dsBmih.biClrUsed;
                  CWindowDC dcScreen(NULL);
                  CDC memDC;
                  memDC.CreateCompatibleDC(&dcScreen);
                  HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
                  LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
                  GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
                  memDC.SelectObject(hOldBmp);

                  ColorTable ct;
                  ct.ctSeed=0;
                  ct.ctFlags=0;
                  ct.ctSize=count - 1;
                  uptr p((char*)&ct);
                  p.swapl(1); p.swaps(2);
                  mf.Write(&ct,sizeof(ColorTable) - sizeof(ColorSpec));
                  for(short c=0; c<count; c++)
                  {
                     ColorSpec cs;
                     cs.value=c;
                     cs.rgb.red=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
                     cs.rgb.green=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
                     cs.rgb.blue=(lpColors[c].rgbRed << 8) + lpColors[c].rgbRed;
                     uptr((char*)&cs).swaps(4);
                     mf.Write(&cs,sizeof(ColorSpec));
                  }
                  GlobalFree(GlobalHandle(lpColors));
               }
               else
               {
                  ColorTable ct;
                  memset(&ct,0,sizeof(ColorTable));
                  mf.Write(&ct,sizeof(ColorTable));
               }

               WriteMFResource(fo,mdf.numBytes,mf,'ppat',ppatID);

               memset(&phf,0,sizeof(PhFillDescript));
               phf.fColor=1;
               phf.fResID=ppatID++;
               //=============================================================================
            }
            uptr((char*)&phf).swaps(2);
            mf.Write(&phf,sizeof(PhFillDescript));

            // by TSM
            if(i<m_numDIBPatterns) ::DeleteObject(hDib);
            // end by TSM
         }

         WriteMFResource(fo,mdf.numBytes,mf,'PLDT',128);
      }

      // gradients
      {

         // Infinisys Ltd
         CMemFile mf;

         mf.Write(&m_numGradients,sizeof(m_numGradients));

         for(int i=0; i<m_numGradients; i++)
         {
            m_Gradients[i].Save55(mf);
         }

         WriteMFResource(fo,mdf.numBytes,mf, 'GRAD',128);
      }

      // packed items
      {

         // Infinisys Ltd
         CMemFile mf;

         int iSize = m_packeditema.GetCount();

         mf.Write(&iSize,sizeof(iSize));

         for(int i=0; i<iSize; i++)
         {
            mf.Write(&m_packeditema[i].m_etype,sizeof(m_packeditema[i].m_etype));
            int iIndex = m_packeditema[i].get_index(this);
            mf.Write(&iIndex,sizeof(iIndex));
         }

         WriteMFResource(fo,mdf.numBytes,mf,'PCKI',128);
      }

      // saved views
      if(m_SavedViews.GetSize())
      {
         CMemFile mf;

         memset(&list,0,sizeof(PhFListHeader));
         list.fNumElems=m_SavedViews.GetSize();
         list.fElemSize=sizeof(PhSavedView);
         list.fGrowSize=sizeof(PhSavedView);
         list.fKeylen=kMaxViewName;

         uptr p((char*)&list);
         p.swaps(1); p.swapl(1); p.swaps(1);
         p.swapl(3); p.swaps(3); p.swapl(1);
         mf.Write(&list,sizeof(PhFListHeader));

         PhSavedView phsv;
         for(short i=0; i<m_SavedViews.GetSize(); i++)
         {
            CSavedView* pView=(CSavedView*)m_SavedViews[i];
            ctx.ConvertC2P(pView->m_sTitle,phsv.fViewName,kMaxViewName);
            phsv.fOldScroll.x=(short)pView->m_ptOrg.x;
            phsv.fOldScroll.y=(short)pView->m_ptOrg.y;
            phsv.fZoomFactor=pView->m_nZoom;
            phsv.fCmdKey=pView->m_nShortcut?('4' + pView->m_nShortcut):0;

            p.pc=(char*)&phsv;
            p.skip(kMaxViewName); p.skip(2); p.swapl(1); p.swaps(1);
            mf.Write(&phsv,sizeof(PhSavedView));
         }

         WriteMFResource(fo,mdf.numBytes,mf,'VIEW',1);
      }

      // reports
      if(m_Reports.GetSize())
      {
         for(int r=0; r<m_Reports.GetSize();)
         {
            CMDReport* pReport=m_Reports[r++];

            PhReport phr;
            memset(&phr,0,sizeof(PhReport));
            phr.cVersion=3;
            ctx.ConvertC2P(pReport->m_sTitle,phr.cfTitle,64);
            TextParams tp;
            tp.m_lf=pReport->m_LogFont;
            ctx.ConvertTextParams(tp,phr.cDefaultTextParams);
            for(int i=0; i<6; i++) phr.cfColumnWidths[i]=pReport->m_nColumnWidths[i];
            for(int i=0; i<4; i++)
               ctx.ConvertC2P(m_sFieldNames[i + 1],phr.cFieldNames[i],32);
            phr.cfNumRows=pReport->m_Rows.GetSize();

            uptr p((char*)&phr);
            p.swaps(2); p.swapl(1); p.skip(64);
            p.swapl(1); p.swaps(1);
            p.swaps(2); p.skip(2); p.swaps(1); p.skip(2); p.swaps(3); // tp
            p.swaps(13); p.skip(32 * 4);
            p.swaps(1); p.swapl(1); p.swaps(4);
            p.skip(2); p.swapl(1); p.swaps(1); p.swapl(1);
            p.swapl(1); p.swaps(1); p.swapl(1); p.skip(64);
            p.swaps(2); p.skip(32);
            p.swapl(1); p.swaps(2); p.swapl(2); p.swaps(2); p.swapl(1);
            p.swapl(2);

            WriteResource(fo,mdf.numBytes,&phr,sizeof(PhReport),'mdrp',r);

            PhRowInfo* pri=new PhRowInfo[pReport->m_Rows.GetSize()];
            short rowCount[3]= {0,0,0};
            CMemFile mf[3];
            for(int i=0; i<3; i++)
            {
               short cnt=20 << 8;	// swapped
               mf[i].Write(&cnt,sizeof(short));
            }

            p.pc=(char*)pri;
            for(int i=0; i<pReport->m_Rows.GetSize(); i++)
            {
               CMDReportRow* pReportRow=(CMDReportRow*)pReport->m_Rows[i];
               short typeIdx=pReportRow->m_nRowType - 1;
               pri[i].fRowFlags=0;
               pri[i].fRowID=0;
               pri[i].fRowData=1;

               switch(pReportRow->m_nRowType)
               {
               case kTextRow:
               {
                  short count=sizeof(PhTextRowData);
                  pri[i].fRowType=kPhTextRow;
                  PhTextRowData ptrd;
                  ptrd.fChanged=0;
                  ptrd.fRowType=kPhTextRow;
                  ptrd.fSpare=0;
                  for(int j=0; j<6; j++)
                  {
                     ptrd.fJust[j]=pReportRow->m_nJust[j];
                     count+=pReportRow->m_sRowText[j].GetLength() + 2;
                  }
                  ptrd.fUseStyle=0;

                  uptr p((char*)&ptrd);
                  p.swaps(1); p.skip(2);
                  p.skip(6); p.swaps(1);// p.skip(12);

                  uptr(&count).swaps(1);
                  mf[typeIdx].Write(&count,sizeof(short));
                  mf[typeIdx].Write(&ptrd,sizeof(PhTextRowData));
                  for(int j=0; j<6; j++)
                  {
                     short len=pReportRow->m_sRowText[j].GetLength();
                     mf[typeIdx].Write(&len,1);
                     mf[typeIdx].Write(pReportRow->m_sRowText[j],len + 1);
                  }
               }
               break;
               case kCriteriaRow:
               {
                  pri[i].fRowType=kPhCriteriaRow;
                  PhCriteriaRowData pcrd;
                  pcrd.fChanged=0;
                  pcrd.fRowType=kPhCriteriaRow;
                  pcrd.fSpare=0;
                  for(int j=0; i<6; i++) pcrd.fFieldRefNum[j]=pReportRow->m_nFieldRefNum[j];

                  uptr p((char*)&pcrd);
                  p.swaps(1); p.skip(2); p.swaps(6);
                  short count=sizeof(PhCriteriaRowData);
                  uptr(&count).swaps(1);
                  mf[typeIdx].Write(&count,sizeof(short));
                  mf[typeIdx].Write(&pcrd,sizeof(PhCriteriaRowData));
               }
               break;
               case kFunctionRow:
               {
                  short count=sizeof(PhFunctRowData);
                  pri[i].fRowType=kPhFunctionRow;
                  PhFunctRowData pfrd;
                  pfrd.fChanged=0;
                  pfrd.fRowType=kPhFunctionRow;
                  pfrd.fSpare=(uchar)pReportRow->m_nFunction;
                  if(pfrd.fSpare >= kFunctTotalName) pfrd.fSpare++;
                  for(int j=0; j<6; j++)
                  {
                     pfrd.fJust[j]=pReportRow->m_nJust[j];
                     count+=pReportRow->m_sRowText[j].GetLength() + 2;
                  }
                  pfrd.fResult=0;
                  pfrd.dummy[0]=0;
                  pfrd.dummy[1]=0;
                  pfrd.fUseStyle=0;

                  uptr p((char*)&pfrd);
                  p.swaps(1); p.skip(2);
                  p.skip(6); p.skip(8); p.swaps(3);
                  uptr(&count).swaps(1);
                  mf[typeIdx].Write(&count,sizeof(short));
                  mf[typeIdx].Write(&pfrd,sizeof(PhFunctRowData));
                  for(int j=0; j<6; j++)
                  {
                     short len=pReportRow->m_sRowText[j].GetLength();
                     mf[typeIdx].Write(&len,1);
                     mf[typeIdx].Write(pReportRow->m_sRowText[j],len + 1);
                  }
               }
               break;
               }

               rowCount[typeIdx]++;
               if(rowCount[typeIdx] % 20 == 0)
               {
                  WriteMFResource(fo,mdf.numBytes,mf[typeIdx],::reportRowType[typeIdx],100 * ((rowCount[typeIdx] / 20) - 1) + r);

                  short cnt=20 << 8;	// swapped
                  mf[typeIdx].Write(&cnt,sizeof(short));
               }
               p.skip(2); p.swaps(1); p.swapl(1);
            }

            for(int i=0; i<3; i++)
            {
               if(rowCount[i] % 20>0)
               {
                  long length=mf[i].GetLength();
                  void* pl=mf[i].Detach();
                  *((short*)pl)=(rowCount[i] % 20) << 8;	// swapped
                  WriteResource(fo,mdf.numBytes,pl,length,::format55::reportRowType[i],100 * (rowCount[i] / 20) + r);
                  free(pl);
               }
            }

            WriteResource(fo,mdf.numBytes,pri,pReport->m_Rows.GetSize()*sizeof(PhRowInfo),'mdrl',r);
            delete[] pri;
         }
      }

      mdf.offset=mdf.numBytes + sizeof(::format55::MDFooter);
      uptr((char*)&mdf).swapl(7);
      fo->Write(&mdf,sizeof(::format55::MDFooter));

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

namespace format55
{

   short PhIOContext::UseFont(const CString &sFont, int iCS)
   {
      for(short i=0; i < fontInUse.GetSize(); i++)
      {
         if(fontInUse[i] == sFont && fontCS[i] == iCS) return i;
      }
      fontCS.Add(iCS);
      return fontInUse.Add(sFont);
   }

   ushort grayFill[10]= {0x0000,0xE665,0xCCCC,0xB332,0x9999,0x7FFF,0x6666,0x4CCC,0x3333,0x1999};

   void PhIOContext::ConvertPhPenModel(const PhPenModel& phpm,PenModel& pm)
   {
      pm.m_nPenType=phpm.fPenType;
      switch(pm.m_nPenType)
      {
      case kPhPenColor:
         if(phpm.fColorIndex > 499)
         {
            PALETTEENTRY pe;
            pPalette->GetPaletteEntries(phpm.fColorIndex - 1,1,&pe);
            pm.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
         }
         else pm.m_crColor=PALETTEINDEX(phpm.fColorIndex - 1);
         break;
      case kPhPenGray:
      {
         pm.m_nPenType--;
         ushort grey=grayFill[phpm.fColorIndex / 10] >> 8;
         pm.m_crColor=RGB(grey,grey,grey);
      }
      break;
      case kPhPenPat:
         // by TSM
         pm.m_nIndex=phpm.fColorIndex - 1;
         if(pm.m_nIndex > 70)
         {
            // by TSM
            //pm.m_hDib=CopyDIB((HBITMAP)embedDIBs[pm.m_nIndex-71]);
            //pm.m_nIndex = embedDIBsIndex[pm.m_nIndex-71];
            pm.m_nPenType--;
            // end by TSM
         }
         else pm.m_nPenType--;
         break;
      }
      pm.m_nEndCap1=(phpm.fPenStyle) & 0xF;
      pm.m_nEndCap2=(phpm.fPenStyle >> 4) & 0xF;
      pm.m_nStyle=(phpm.fPenStyle >> 8) & 0xF;
      pm.m_nAlignment=phpm.fAlignment - 1;
      pm.m_nWidth=phpm.fPenW & 0x1F;
      if(phpm.fPenStyle & 0x8000)
      {
         pm.m_nWidth=0x81;
         pm.m_nWidth|=(phpm.fPenStyle >> 7) & 0x60;
      }
      pm.m_iPenOpacity = phpm.fOpacity;
   }

   void PhIOContext::ConvertPenModel(const PenModel& pm,PhPenModel& phpm)
   {
      phpm.fPenType=pm.m_nPenType;
      switch(pm.m_nPenType)
      {
      case kPenPatEmbed:
         // by TSM
         //phpm.fColorIndex=72+embedDIBs.Add((DWORD)pm.m_hDib);
         phpm.fColorIndex=1 + m_numDIBPatterns + embedDIBs.Add((DWORD)pm.m_hDib);
         break;
      case kPenPat:
         phpm.fPenType++;
         phpm.fColorIndex=(pm.m_nIndex & 0xFF) + 1;
         break;
      case kPenColor:
         if(pm.m_crColor < PALETTEINDEX(0))
         {
            //int i = GetEntryCount();
            PALETTEENTRY pe;
            // Looking for existing entry with the same color
            UINT idx  = pPalette->GetNearestPaletteIndex(pm.m_crColor);
            if(idx != CLR_INVALID && pPalette->GetPaletteEntries(idx,1,&pe) && RGB(pe.peRed,pe.peGreen,pe.peBlue) == pm.m_crColor)
            {
               phpm.fColorIndex=idx + 1;
               break;
            }

            phpm.fColorIndex=pPalette->GetEntryCount() + 1;
            pPalette->ResizePalette(phpm.fColorIndex);

            pe.peRed=GetRValue(pm.m_crColor);
            pe.peGreen=GetGValue(pm.m_crColor);
            pe.peBlue=GetBValue(pm.m_crColor);
            pe.peFlags=0;
            pPalette->SetPaletteEntries(phpm.fColorIndex - 1,1,&pe);
            VERIFY(pPalette->SetPaletteEntries(phpm.fColorIndex - 1,1,&pe));
            break;
         }
         else phpm.fColorIndex=(pm.m_nIndex & 0xFF) + 1;
         break;
      }

      phpm.fPenStyle=(pm.m_nEndCap1 & 0xF);
      phpm.fPenStyle|=(pm.m_nEndCap2 & 0xF) << 4;
      phpm.fPenStyle|=(pm.m_nStyle & 0xF) << 8;
      phpm.fAlignment=pm.m_nAlignment + 1;
      phpm.fPenW=pm.m_nWidth & 0x1F;
      if(pm.m_nWidth & 0x80)
      {
         phpm.fPenStyle|=0x8000;
         phpm.fPenStyle|=(pm.m_nWidth & 0x60) << 7;
      }
      phpm.fOpacity = pm.m_iPenOpacity;
   }

   void PhIOContext::ConvertPhFillModel(const PhFillModel& phfm,FillModel& fm)
   {
      fm.m_nFillType=(uchar)phfm.fFillType;
      switch(fm.m_nFillType)
      {
      case kPhFillGray:
      {
         fm.m_nFillType=kFillColor;
         ushort grey=grayFill[phfm.fFillIndex / 10] >> 8;
         fm.m_crColor=RGB(grey,grey,grey);
      }
      break;
      case kPhFillColor:
         if(phfm.fFillIndex > 499)
         {
            PALETTEENTRY pe;
            pPalette->GetPaletteEntries(phfm.fFillIndex - 1,1,&pe);
            fm.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
         }
         else fm.m_crColor=PALETTEINDEX(phfm.fFillIndex - 1);
         break;
      case kPhFillPat:
         fm.m_nIndex=phfm.fFillIndex - 1;
         //if(fm.m_nIndex>70)
         //{
         // by TSM
         //fm.m_nFillType=kFillPatEmbed;
         //fm.m_hDib=CopyDIB((HBITMAP)embedDIBs[fm.m_nIndex-71]);
         //fm.m_nIndex = embedDIBsIndex[fm.m_nIndex-71];
         // end by TSM
         //}
         break;
      }
      // Opacity/Transparency - Infinisys Ltd
      fm.m_iFillOpacity = phfm.fOpacity;
   }

   void PhIOContext::ConvertFillModel(const FillModel& fm,PhFillModel& phfm)
   {
      phfm.fFillType=fm.m_nFillType;
      switch(fm.m_nFillType)
      {
      case kFillPatEmbed:
         phfm.fFillType--;
         // by TSM
         //phfm.fFillIndex=72+embedDIBs.Add((DWORD)/*CopyDIB*/(fm.m_hDib));
         phfm.fFillIndex=1 + m_numDIBPatterns + embedDIBs.Add((DWORD)/*CopyDIB*/(fm.m_hDib));
         break;
      case kFillColor:
         if(fm.m_crColor < PALETTEINDEX(0))
         {
            PALETTEENTRY pe;
            UINT idx  = pPalette->GetNearestPaletteIndex(fm.m_crColor);
            if(idx != CLR_INVALID && pPalette->GetPaletteEntries(idx,1,&pe) && RGB(pe.peRed,pe.peGreen,pe.peBlue) == fm.m_crColor)
            {
               phfm.fFillIndex=idx + 1;
               break;
            }
            phfm.fFillIndex=pPalette->GetEntryCount() + 1;
            pPalette->ResizePalette(phfm.fFillIndex);
            pe.peRed=GetRValue(fm.m_crColor);
            pe.peGreen=GetGValue(fm.m_crColor);
            pe.peBlue=GetBValue(fm.m_crColor);
            pe.peFlags=0;
            pPalette->SetPaletteEntries(phfm.fFillIndex - 1,1,&pe);
            break;
         }
      case kFillPat:
         phfm.fFillIndex=(fm.m_nIndex & 0xFF) + 1;
         break;
      }
      // Opacity/Transparency - Infinisys Ltd
      phfm.fOpacity = fm.m_iFillOpacity;
   }

   void PhIOContext::ConvertPhParaAttr(const PhParallelAttr& phpa,ParallelAttrs& pa)
   {
      pa.m_nAlign=3 - (phpa.fLineAttr & 0x3);
      pa.m_nEndCap=((phpa.fLineAttr >> 2) & 0x3);
      pa.m_nJoint=(phpa.fLineAttr >> 6) & 0x1;
      ConvertPhFillModel(phpa.fFillModel,pa.m_Fill);
      pa.m_fWidth=phpa.fWidth;
   }

   void PhIOContext::ConvertParaAttr(const ParallelAttrs& pa,PhParallelAttr& phpa)
   {
      phpa.fLineAttr=((3 - pa.m_nAlign) & 0x3) | (((pa.m_nEndCap) & 0x3) << 2) | ((pa.m_nJoint & 0x1) << 6);
      ConvertFillModel(pa.m_Fill,phpa.fFillModel);
      phpa.fWidth=pa.m_fWidth;
   }

   void PhIOContext::ConvertPhTextParams(const PhTextParams& phtp,TextParams& tp)
   {
      ConvertPhFont(phtp.fTextFont,phtp.fTextSize,phtp.fTextStyle,tp.m_lf);

      switch(phtp.fTextJust)
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
      switch(phtp.fTextSpace)
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
      switch(phtp.fTextCase)
      {
      case kPhAsIs:
         tp.m_nCase=-1;
         break;
      case kPhUppercase:
         tp.m_nCase=kCaseUpper;
         break;
      case kPhLowercase:
         tp.m_nCase=kCaseLower;
         break;
      case kPhTitleText:
         tp.m_nCase=kCaseTitle;
         break;
      }
   }

   void PhIOContext::ConvertTextParams(const TextParams& tp,PhTextParams& phtp)
   {
      ConvertLogFont(tp.m_lf,phtp.fTextFont,phtp.fTextSize,phtp.fTextStyle);
      phtp.fDummy=0;

      switch(tp.m_nSpacing)
      {
      case kSingleSpace:
         phtp.fTextSpace=kPhSingleSpace;
         break;
      case k1_5Space:
         phtp.fTextSpace=kPhOne_halfSpace;
         break;
      case kDoubleSpace:
         phtp.fTextSpace=kPhDoubleSpace;
         break;
      }
      switch(tp.m_nJust)
      {
      case kAlignLeft:
         phtp.fTextJust=0;
         break;
      case kAlignCenter:
         phtp.fTextJust=1;
         break;
      case kAlignRight:
         phtp.fTextJust=-1;
         break;
      }
      switch(tp.m_nCase)
      {
      case -1:
         phtp.fTextCase=kPhAsIs;
         break;
      case kCaseUpper:
         phtp.fTextCase=kPhUppercase;
         break;
      case kCaseLower:
         phtp.fTextCase=kPhLowercase;
         break;
      case kCaseTitle:
         phtp.fTextCase=kPhTitleText;
         break;
      }
   }

   void PhIOContext::ConvertPhDimStd(const PhDimStd& phstd,DimStandard& std)
   {
      std.nType=phstd.dsType;
      std.WitnessExt=mtFixToReal(phstd.dsWitnessExt);
      std.WitnessLineGap=mtFixToReal(phstd.dsWitnessLineGap);
      std.DimTextGap=mtFixToReal(phstd.dsDimTextGap);
      std.LeaderLen=mtFixToReal(phstd.dsLeaderLen);
      std.OutsideLineLen=mtFixToReal(phstd.dsOutsideLineLen);
      std.nToleranceTextScale=phstd.dsToleranceTextScale;
      std.nToleranceSpaceScale=phstd.dsToleranceSpaceScale;
      std.CircleCenterLen=mtFixToReal(phstd.dsCircleCenterLen);
      std.CircleCenterGap=mtFixToReal(phstd.dsCircleCenterGap);
      std.CircleCenterExt=mtFixToReal(phstd.dsCenterExt);
   }

   void PhIOContext::ConvertDimStd(const DimStandard& std,PhDimStd& phstd)
   {
      phstd.dsType=std.nType;
      phstd.dsWitnessExt=std.WitnessExt;
      phstd.dsWitnessLineGap=std.WitnessLineGap;
      phstd.dsDimTextGap=std.DimTextGap;
      phstd.dsLeaderLen=std.LeaderLen;
      phstd.dsOutsideLineLen=std.OutsideLineLen;
      phstd.dsToleranceTextScale=std.nToleranceTextScale;
      phstd.dsToleranceSpaceScale=std.nToleranceSpaceScale;
      phstd.dsCircleCenterLen=std.CircleCenterLen;
      phstd.dsCircleCenterGap=std.CircleCenterGap;
      phstd.dsCenterExt=std.CircleCenterExt;
   }

   void PhIOContext::ConvertPhDimFmt(const PhDimFormat& phfmt,DimFormat& fmt)
   {
      fmt.nType=phfmt.dfType;
      fmt.bTextCentered=phfmt.dfTextCentered?true:false;
      fmt.bTextFramed=phfmt.dfTextFramed?true:false;
      fmt.bArrowsInside=phfmt.dfArrowsInside?true:false;
      fmt.bUseLabel=phfmt.dfUseLabel?true:false;
      fmt.bUseWitness=phfmt.dfUseWitness?true:false;
      fmt.nTextDisplay=phfmt.dfTextDisplay;
      fmt.nLeader=phfmt.dfLeader;
      fmt.nRDSymbol=phfmt.dfRDSymbol;
      fmt.nTolerance=phfmt.dfTolerance;
      fmt.Tolerance1=mtFixToReal(phfmt.dfTolerance1);
      fmt.Tolerance2=mtFixToReal(phfmt.dfTolerance2);
      fmt.sLabel=""; //phfmt.dfDimLabelHdl;
      fmt.nRoundoff = 0;
   }

   void PhIOContext::ConvertDimFmt(const DimFormat& fmt,PhDimFormat& phfmt)
   {
      phfmt.dfType=fmt.nType;
      phfmt.dfTextCentered=fmt.bTextCentered;
      phfmt.dfTextFramed=fmt.bTextFramed;
      phfmt.dfArrowsInside=fmt.bArrowsInside;
      phfmt.dfUseLabel=fmt.bUseLabel;
      phfmt.dfUseWitness=fmt.bUseWitness;
      phfmt.dfTextDisplay=fmt.nTextDisplay;
      phfmt.dfLeader=fmt.nLeader;
      phfmt.dfRDSymbol=fmt.nRDSymbol;
      phfmt.dfTolerance=fmt.nTolerance;
      phfmt.dfTolerance1=fmt.Tolerance1;
      phfmt.dfTolerance2=fmt.Tolerance2;
      phfmt.dfDimLabelHdl=(fmt.bUseLabel && !fmt.sLabel.IsEmpty())?1:0;
   }

   void PhIOContext::ConvertLogFont(const LOGFONT &lf,short& nFont,short& nSize,Style& nStyle)
   {
      nFont=UseFont(lf.lfFaceName, lf.lfCharSet) + 1;
      if(lf.lfHeight < 0)
      {
         HDC hDC=GetDC(NULL);
         nSize=MulDiv(-lf.lfHeight,72,GetDeviceCaps(hDC,LOGPIXELSY));
         ReleaseDC(NULL,hDC);
      }
      else nSize=(short)lf.lfHeight;
      nStyle=0;
      if(lf.lfWeight == FW_BOLD) nStyle|=0x1; // NB: some bold fonts have other values
      if(lf.lfItalic) nStyle|=0x2;
      if(lf.lfUnderline) nStyle|=0x4;
   }

   void PhIOContext::ConvertPhFont(short nFont,short nSize,Style nStyle,LOGFONT &lf)
   {
      HDC hDC=GetDC(NULL);
      lf.lfHeight=-MulDiv(nSize,GetDeviceCaps(hDC,LOGPIXELSY),72);
      ReleaseDC(NULL,hDC);
      lf.lfWeight=(nStyle & 0x1)?FW_BOLD:FW_NORMAL;
      lf.lfItalic=(nStyle & 0x2)?TRUE:FALSE;
      lf.lfUnderline=(nStyle & 0x4)?TRUE:FALSE;
      CString pFaceName;
      if(fontMap.Lookup(nFont,pFaceName))
      {
         lstrcpyn(lf.lfFaceName,pFaceName,LF_FACESIZE);
      }
      int iCharset = ANSI_CHARSET;
      if(fontCharset.Lookup(nFont,iCharset))
      {
         lf.lfCharSet = iCharset;
      }
   }

   void PhIOContext::ConvertC2P(const CString& str,char* pBuf,int nMaxSize)
   {
      uchar len=min(str.GetLength(),nMaxSize - 1);
      *pBuf++=len;
      for(int i=0; i < len; i++)
         pBuf[i]=pc2mac[(uchar)str[i]];
   }


} // namespace format55