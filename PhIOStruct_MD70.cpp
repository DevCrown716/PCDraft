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

#include "SavedView.h"
#include "AttachContext.h"

#include "Import.h"

#define DEV_DEBUG 0

#define ARRAY_COUNT(a) (sizeof(a)/sizeof(a[0]))
#define STATIC_ARRAY(a) a, ARRAY_COUNT(a)

CString acp_to_utf8(CString str);

CString utf8_to_acp(CString str);
CString utf8_to_maccp(CString str);
CString maccp_to_utf8(CString str);
CString oem_to_mac(CString str);

CString mac_to_oem(CString str);


using namespace formatMD70;


namespace formatMD70
{

   extern uchar mac2pc[256]=
   {//	  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
      0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,// 0x
      0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,// 1x
      0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,// 2x
      0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,// 3x
      0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,// 4x
      0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,// 5x
      0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,// 6x
      0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,// 7x
      0xC4,0xC5,0xC7,0xC9,0xD1,0xD6,0xDC,0xE1,0xE0,0xE2,0xE4,0xE3,0xE5,0xE7,0xE9,0xE8,// 8x
      0xEA,0xEB,0xED,0xEC,0xEE,0xEF,0xF1,0xF3,0xF2,0xF4,0xF5,0xF6,0xFA,0xF9,0xFB,0xFC,// 9x
      0x86,0xB0,0xA2,0xA3,0xA7,0x95,0xB6,0xDF,0xAE,0xA9,0x99,0xB4,0xA8,0xAD,0xC6,0xD8,// Ax
      0xB0,0xB1,0xB2,0xB3,0xA5,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xAA,0xBA,0xBD,0xE6,0xF8,// Bx
      0xBF,0xA1,0xAC,0xC3,0x83,0xC5,0xC6,0xAB,0xBB,0x85,0xA0,0xC0,0xC3,0xD5,0x8C,0x9C,// Cx
      0x96,0x97,0x93,0x94,0x91,0x92,0xF7,0xD7,0xFF,0x9F,0xDA,0xA4,0x8B,0x9B,0xDE,0xDF,// Dx
      0x87,0xB7,0x82,0x84,0x89,0xC2,0xCA,0xC1,0xCB,0xC8,0xCD,0xCE,0xCF,0xCC,0xD3,0xD4,// Ex
      0xF0,0xD2,0xDA,0xDB,0xD9,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xB8,0xFD,0xFE,0xFF	// Fx
   };
   
   extern uchar pc2mac[256]=
   {//	  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
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
      0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
      0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
      0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
      0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xAF,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
      0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
      0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
   };
   

   void serialize::operator()(CString & str)
   {

      if(m_bWrite)
      {

         CString strUtf8(str);

         strUtf8 = maccp_to_utf8(str);

         long l = strUtf8.GetLength();

         swap32(&l);

         m_pfile->Write(&l,sizeof(l));

         m_pfile->Write((const char *)strUtf8,strUtf8.GetLength());

      }
      else
      {

         CString strUtf8;

         m_pointer.str(strUtf8);

//         str = utf8_to_maccp(strUtf8);

         str = strUtf8;

      }

   }

   void serialize::s1(CString & str)
   {

      if(m_bWrite)
      {

         CString strUtf8(str);

         strUtf8 = utf8_to_maccp(str);

         long l = strUtf8.GetLength();

         swap32(&l);

         m_pfile->Write(&l,sizeof(l));

         m_pfile->Write((const char *)strUtf8,strUtf8.GetLength());

      }
      else
      {

         CString strUtf8;

         m_pointer.str(strUtf8);

         str = maccp_to_utf8(strUtf8);

      }

   }

   void serialize::operator()(CString & str,long l)
   {

      if(m_bWrite)
      {

         m_pfile->Write((const char *)str,l);

      }
      else
      {

         m_pointer.str(str,l);


      }

   }

   void serialize::operator()(DCoord & d)
   {

      //raw_type_context rawtypecontext(this);

      if(m_bWrite)
      {
         double dVal = d.m_d;
         dVal = dVal * (double)m_ratio.fD / (double)m_ratio.fN;
         operator()(dVal);
      }
      else
      {
         operator()(d.m_d);
         d.m_d = d.m_d * (double)m_ratio.fN / (double)m_ratio.fD;


      }


   }

   void serialize::operator()(DCoordPoint & point)
   {

      //raw_type_context rawtypecontext(this);

      operator()(point.y);
      operator()(point.x);
      operator()(point.reservedForZ);
   }


   void serialize::operator()(DCoordRect & rect)
   {

      //raw_type_context rawtypecontext(this);

      operator()(rect.t);
      operator()(rect.l);
      operator()(rect.b);
      operator()(rect.r);

   }

   void serialize::operator()(DDBPoint & point)
   {

      //raw_type_context rawtypecontext(this);

      operator()(point.fLongPt);
      operator()(point.fReserved);
      operator()(point.fFlags);
      operator()(point.fUse);
      operator()(point.fReserved1);

   }

   void serialize::operator()(Coord & f)
   {

      //raw_type_context rawtypecontext(this);

      if(m_bWrite)
      {
         float fVal = f.m_f;
         fVal = fVal * (float)m_ratio.fD / (float)m_ratio.fN;
         operator()(fVal);
      }
      else
      {
         operator()(f.m_f);
         f.m_f= f.m_f * (float)m_ratio.fN / (float)m_ratio.fD;
      }


   }


   void serialize::operator()(CoordPoint & point)
   {

      //raw_type_context rawtypecontext(this);

      operator()(point.y);
      operator()(point.x);

   }

   void serialize::operator()(CoordRect & rect)
   {

      //raw_type_context rawtypecontext(this);

      operator()(rect.t);
      operator()(rect.l);
      operator()(rect.b);
      operator()(rect.r);

   }

   void serialize::operator()(LongRatio & ratio)
   {

      //raw_type_context rawtypecontext(this);

      operator()(ratio.fN);
      operator()(ratio.fD);

   }


   void serialize::operator()(RGBColor & rgb)
   {

      //raw_type_context rawtypecontext(this);

      operator()(rgb.red);
      operator()(rgb.green);
      operator()(rgb.blue);

   }





   void serialize::operator()(RGBFloat & rgb)
   {

      //raw_type_context rawtypecontext(this);

      operator()(rgb.fRed);
      operator()(rgb.fGreen);
      operator()(rgb.fBlue);

   }


   void serialize::operator()(RGBAFloat & rgba)
   {

      //raw_type_context rawtypecontext(this);

      operator()((RGBFloat &)rgba);
      operator()(rgba.fOpacity);

   }


   void serialize::operator()(RGBAPos & rgbapos)
   {

      //raw_type_context rawtypecontext(this);

      operator()((RGBAFloat &)rgbapos);
      operator()(rgbapos.fPos);

   }

   void serialize::operator()(PhRect & rect)
   {

      //raw_type_context rawtypecontext(this);

      operator()(rect.t); // BigEndian-LittleEndian
      operator()(rect.l); // BigEndian-LittleEndian
      operator()(rect.b); // BigEndian-LittleEndian
      operator()(rect.r); // BigEndian-LittleEndian

   }


   void serialize::operator()(PhPoint & point)
   {

      //raw_type_context rawtypecontext(this);

      operator()(point.y);
      operator()(point.x);

   }

   //void serialize::operator()(PhMDTextScript & script)
   //{

   //   raw_type_context rawtypecontext(this);


   //   operator()(script.scrpStartChar);
   //   operator()(script.scrpHeight);
   //   operator()(script.scrpAscent);
   //   operator()(script.scrpFont);
   //   operator()(script.scrpFace);
   //   operator()(script.theFaceChar)	;
   //   operator()(script.scrpSize);
   //   operator()(script.scrpColor);

   //}

   void serialize::operator()(PhTextParams * ptextParams)
   {

      complex_type_context rawtypecontext(this);

      operator()(ptextParams->fTextFontName);
      operator()(ptextParams->fTextSize);
      operator()(ptextParams->fUnderline);
      operator()(ptextParams->fHorizTextJust);
      operator()(ptextParams->fVertTextAlign);
      operator()(ptextParams->fTextFlip);
      operator()((long &)ptextParams->fTextSpace);
      operator()((long &)ptextParams->fTextCase);
      operator()(ptextParams->fReserved2);
      operator()(ptextParams->fReserved3);
      operator()(ptextParams->fTextColor);
      operator()(ptextParams->fReserved4);
//      operator()(ptextParams->fMargins);

   }


   void serialize::operator()(PhParallelAttr * pparallelAttr)
   {

      raw_type_context rawtypecontext(this);

      operator()((PhFillModel *)pparallelAttr);
      operator()(pparallelAttr->fWidth);
      operator()(pparallelAttr->fAlignment);
      operator()(pparallelAttr->fEndCap);
      operator()(pparallelAttr->fLineJoin);
      operator()(pparallelAttr->fParaReserved1);
      operator()(pparallelAttr->fParaReserved2);

   }


   void serialize::operator()(PhChamferAttr * pattr)
   {

      raw_type_context rawtypecontext(this);

      operator()(pattr->fLine1SetBack);
      operator()(pattr->fLine2SetBack);
      operator()((long&)pattr->fTrimMethod);

   }


   void serialize::operator()(PhFilletAttr * pattr)
   {

      raw_type_context rawtypecontext(this);

      operator()(pattr->fRadius);
      operator()((long&)pattr->fTrimMethod);
      operator()(pattr->fIsSmallerArc);

   }


   void serialize::operator()(PhWindowSplit * psplit)
   {

      raw_type_context rawtypecontext(this);

      operator()(psplit->f);
      operator()(psplit->fSplitRatio);
      operator()(psplit->fHCurScroll);
      operator()(psplit->fVCurScroll);
      operator()(psplit->fZoomFactor);

   }


   void serialize::operator()(PhDocUnits * pdocUnits)
   {

      raw_type_context rawtypecontext(this);

      operator()(pdocUnits->fDefUnits);
      operator()(pdocUnits->fDefPlaces);
      operator()(pdocUnits->fDefAngleUnits);
      operator()(pdocUnits->fDefAnglePlaces);
      operator()(pdocUnits->fDefResolution);
      operator()(pdocUnits->fDefScaleIndex);
      operator()(pdocUnits->fDefSnapIndex);

   }

   void serialize::operator()(PhDimStd * pdimStd)
   {

      raw_type_context rawtypecontext(this);

      operator()(pdimStd->dsType);						/* ANSI, Metric */
      operator()(pdimStd->dsReserved1);
      operator()(pdimStd->dsReserved2);
      operator()(pdimStd->dsWitnessExt);
      operator()(pdimStd->dsWitnessLineGap);
      operator()(pdimStd->dsDimTextGap);
      operator()(pdimStd->dsLeaderLen);
      operator()(pdimStd->dsOutsideLineLen);
      operator()(pdimStd->dsToleranceTextScale);
      operator()(pdimStd->dsToleranceSpaceScale);
      operator()(pdimStd->dsCircleCenterLen);
      operator()(pdimStd->dsCircleCenterGap);
      operator()(pdimStd->dsCenterExt);

   }


   void serialize::operator()(PhDimFormat * pdimFormat)
   {

      raw_type_context rawtypecontext(this);

      operator()(pdimFormat->dfType);			/* Linear, Radial, Angular */
      operator()(pdimFormat->dfTextCentered);
      operator()(pdimFormat->dfTextFramed);
      operator()(pdimFormat->dfArrowsInside);
      operator()(pdimFormat->dfUseLabel);
      operator()(pdimFormat->dfUseWitness);
      operator()(pdimFormat->dfSpare1);
      operator()((short&)pdimFormat->dfTextDisplay);	/* Horizontal, Along Line, Above, Below */
      operator()((short&)pdimFormat->dfLeader);			/* None, Left, Right */
      operator()((short&)pdimFormat->dfRDSymbol);		/* None, Leading, Trailing */
      operator()((short&)pdimFormat->dfTolerance);		/* None, One, Two, Limits */
      operator()(pdimFormat->dfTolerance1);
      operator()(pdimFormat->dfTolerance2);
      operator()(pdimFormat->dfDimLabelID);
      operator()(pdimFormat->dfReserved);


   }

   void serialize::operator()(PhDimFormat2 * pdimFormat) // MD70 - Infinisys Ltd.
   {

      complex_type_context rawtypecontext(this);

      operator()(pdimFormat->dfType);			/* Linear, Radial, Angular */
      operator()(pdimFormat->dfTextCentered);
      operator()(pdimFormat->dfTextFramed);
      operator()(pdimFormat->dfArrowsInside);
      operator()(pdimFormat->dfUseLabel);
      operator()(pdimFormat->dfUseWitness);
      operator()(pdimFormat->dfSpare1);
      operator()((short&)pdimFormat->dfTextDisplay);	/* Horizontal, Along Line, Above, Below */
      operator()((short&)pdimFormat->dfLeader);			/* None, Left, Right */
      operator()((short&)pdimFormat->dfRDSymbol);		/* None, Leading, Trailing */
      operator()((short&)pdimFormat->dfTolerance);		/* None, One, Two, Limits */
      operator()(pdimFormat->dfTolerance1);
      operator()(pdimFormat->dfTolerance2);
      operator()(pdimFormat->dfDimLabel);


   }

   void serialize::operator()(PhDocStds * pdocStds)
   {

      raw_type_context rawtypecontext(this);

      operator()(&pdocStds->fEnglishDimStd);
      operator()(&pdocStds->fMetricDimStd);
      operator()(&pdocStds->fEnglishDimFormat[0]);
      operator()(&pdocStds->fEnglishDimFormat[1]);
      operator()(&pdocStds->fEnglishDimFormat[2]);
      operator()(&pdocStds->fMetricDimFormat[0]);
      operator()(&pdocStds->fMetricDimFormat[1]);
      operator()(&pdocStds->fMetricDimFormat[2]);

   }


   void serialize::operator()(PhActiveEnvirons * pactiveEnvirons)
   {

      complex_type_context complextypecontext(this);

      operator()(&pactiveEnvirons->fFillModel);
      operator()(&pactiveEnvirons->fPenModel);
      operator()(&pactiveEnvirons->fTextParams);
      operator()(&pactiveEnvirons->fParallelAttr);
      operator()(pactiveEnvirons->fPrevZoomScroll);
      operator()(pactiveEnvirons->fRulerOriginReserved);
      operator()(pactiveEnvirons->fPrintFlags);
      operator()(pactiveEnvirons->fAngleSnapsToACircle);
      operator()(pactiveEnvirons->fDimTextAlgn);
      operator()(pactiveEnvirons->fDimVisible);
      operator()(pactiveEnvirons->fHasGrid);
      operator()(pactiveEnvirons->fHideText);
      operator()(pactiveEnvirons->fShowRulers);
      operator()(pactiveEnvirons->fSnapToObject);
      operator()(pactiveEnvirons->fStandardRulers);
      operator()(pactiveEnvirons->fHidePageBreaks);
      operator()(pactiveEnvirons->fDimLineFlag);
      operator()(pactiveEnvirons->fPrevZoom);
      operator()(pactiveEnvirons->fPenShape);
      operator()(pactiveEnvirons->fDashMenuItem);
      operator()(pactiveEnvirons->fDimVisible);
      operator()(&pactiveEnvirons->fFilletAttr);
      operator()(&pactiveEnvirons->fChamferAttr);
      operator()(pactiveEnvirons->fColorUsageSettings);
      operator()(pactiveEnvirons->fReserved00);
      operator()(pactiveEnvirons->fReserved01);
      operator()(pactiveEnvirons->fHideMetrics);
      operator()(pactiveEnvirons->fPrintWithBGColor);
      operator()(pactiveEnvirons->fShowGuides);
      operator()(pactiveEnvirons->fSmartSnap);
      operator()(pactiveEnvirons->fSnapToGuides);
      operator()(pactiveEnvirons->fDimEndCap_1);
      operator()(pactiveEnvirons->fDimEndCap_2);
      operator()(pactiveEnvirons->fDimType_2);
      operator()(pactiveEnvirons->fGridType);
      operator()(pactiveEnvirons->fTrunk);
      operator()(pactiveEnvirons->screenViewRectOrigin.x);
      operator()(pactiveEnvirons->screenViewRectOrigin.y);
      operator()(pactiveEnvirons->fZoomFactor);

   }


   void serialize::operator()(PhStdMacHeader * pheader)
   {

      raw_type_context rawtypecontext(this);

      /* Std Mac Header - always 128 bytes */
      operator()((long &)pheader->fType);			/* OS Type						*/
      operator()(pheader->fHeaderID);		/* 'MD'	for Dreams				*/
      operator()(pheader->fVersion);		/* Version Number				*/
      operator()(pheader->fUpgradeVersionDS); /* For future use */
      operator()(pheader->fWindowRect);  /* Window Rect in Global Coordinates */
      operator()(pheader->fMViewPages); /* Number of Pages horizontally * pvertically */

   }

   void serialize::operator()(PhDocState * pstate)
   {

      complex_type_context complextypecontext(this);

      operator()((PhStdMacHeader *)pstate);
      operator()(&pstate->fDocUnits);
      operator()(&pstate->fDocStds);
      operator()(pstate->fRulerOrigin);
      operator()(&pstate->fActiveEnvirons);


   }

   void serialize::operator()(PhFillModel * pmodel)
   {

      //raw_type_context rawtypecontext(this);

      operator()(pmodel->color);
      operator()(pmodel->fFillType);
      operator()(pmodel->fFillIndex);
      operator()(pmodel->fReserved);

   }




   void serialize::operator()(PhPenModel * pmodel)
   {

      //raw_type_context rawtypecontext(this);

      operator()(&pmodel->fillModel);
      operator()(pmodel->width);
      operator()(pmodel->fEndCap[0]);
      operator()(pmodel->fEndCap[1]);
      operator()((long&)pmodel->fAlignment);
      serialize_static_array(pmodel->fDash.seg,sizeof(pmodel->fDash));
      operator()((long &)pmodel->fPenReserved);

   }


   void serialize::operator()(PhGenericPatternHeader * pheader)
   {

      complex_type_context rawtypecontext(this);

      operator()(pheader->numItems);
      operator()(pheader->sizeVersionNumber);
      operator()(pheader->idOfVersionNumber);
      operator()(pheader->versionNumber);

   }


   void serialize::operator()(PatternBase * pbase)
   {

      complex_type_context rawtypecontext(this);

      operator()(pbase->Size);
      operator()(pbase->Id);

   }


   void serialize::operator()(PixelPattern * ppattern)
   {

      complex_type_context rawtypecontext(this);

      operator()(ppattern->fBitsPerChannel);
      operator()(ppattern->fWidth);
      operator()(ppattern->fHeight);
      operator()(ppattern->fRotation);

      {
         no_swap_context noswapcontext(this);
         operator()(ppattern->data,ppattern->fWidth * ppattern->fHeight);
      }


   }


   void serialize::operator()(BlendPattern * ppattern)
   {

      complex_type_context rawtypecontext(this);

      operator()(ppattern->fRotation);
      operator()(ppattern->fLinearNotRadial);
      operator()(ppattern->fCenterX);
      operator()(ppattern->fCenterY);
      if(m_bWrite)
      {
         operator()(ppattern->data,ppattern->data.GetCount());
      }
      else
      {
         operator()(ppattern->data,(ppattern->Size - 32) / sizeof(RGBAPos));
      }

   }


#if DEV_DEBUG

   class static_debug
   {
   public:

      static_debug()
      {

         {
            int xxx = 0x40400000;
            float * pf = (float*)&xxx;

            float fff = *pf;
            OutputDebugString("fff");
         }


         {
            uint64_t xxx64= 0x403E000000000000ULL;
            double * pd = (double*)&xxx64;

            double ddd = *pd;
            OutputDebugString("ddd");
         }


         int i = sizeof(PhChamferAttr);
         int iCompare = sizeof(float) + sizeof(float) + sizeof(TrimMethod);
         ASSERT(i == iCompare);
         ASSERT(i == 12);

         i = sizeof(PhFilletAttr);
         iCompare = sizeof(Coord) + sizeof(TrimMethod) + sizeof(Boolean);
         ASSERT(i == iCompare);
         ASSERT(i == 9);

         int iPos = 0;
         int iSize = 788;
         i = sizeof(PhStdMacHeader);
         ASSERT(i == 24);
         iSize -= 24;
         iPos += 24;

         i = sizeof(PhDocUnits);
         ASSERT(i == 16);
         iSize -= 16;
         iPos += 16;

         i = sizeof(PhDimStd);
         ASSERT(i == 80);

         i = sizeof(PhDimFormat);
         ASSERT(i == 48);

         int iPos2 = iPos + sizeof(PhDimStd) * 2;

         i = sizeof(PhDocStds);
         ASSERT(i == sizeof(PhDimStd) * 2 + sizeof(PhDimFormat) * 6);
         iSize -= i;
         iPos = iPos2 + sizeof(PhDimFormat) * 6;

         i = sizeof(DCoordPoint);
         ASSERT(sizeof(DCoordPoint) == 24);
         iSize -= 24;
         iPos += 24;

         i = sizeof(PhActiveEnvirons); //todo verificar com Dennis/Saturn
         int Delta = i - iSize;
         //ASSERT(i == iSize);

         i = sizeof(PhDocStateBase);
         //ASSERT(i == 788);

         i = sizeof(PhDocState);
         //ASSERT(i == 960);

      }


   };


   static_debug g_staticdebug;

#endif



} // namespace formatMD70


namespace formatMD70
{

   DashTable g_dashtableDefault =
   {
      {0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
      {2,6,2,255,255,255,255,255,255,255,255,255,255,255,255,255},
      {2,12,2,255,255,255,255,255,255,255,255,255,255,255,255,255},
      {2,24,3,255,255,255,255,255,255,255,255,255,255,255,255,255},
      {2,48,4,255,255,255,255,255,255,255,255,255,255,255,255,255},
      {4,6,2,1,2,255,255,255,255,255,255,255,255,255,255,255},
      {4,12,2,1,2,255,255,255,255,255,255,255,255,255,255,255},
      {4,24,3,2,3,255,255,255,255,255,255,255,255,255,255,255},
      {4,48,4,2,4,255,255,255,255,255,255,255,255,255,255,255},
      {6,6,2,1,2,1,2,255,255,255,255,255,255,255,255,255},
      {6,12,2,1,2,1,2,255,255,255,255,255,255,255,255,255},
      {6,24,3,2,2,2,3,255,255,255,255,255,255,255,255,255},
      {6,48,4,2,2,2,4,255,255,255,255,255,255,255,255,255},
      {8,12,2,1,2,1,2,1,2,255,255,255,255,255,255,255},
      {8,24,3,2,2,2,2,2,3,255,255,255,255,255,255,255},
      {8,48,4,2,3,2,3,2,4,255,255,255,255,255,255,255},

   };

   short PhIOContext::UseFont(const CString &sFont,int iCS)
   {
      int iSizeFontsInUse = fontInUse.GetCount();
      int iSizeFontCS = fontCS.GetCount();
      for(short i=0; i < iSizeFontsInUse; i++)
      {
         CString strFontInUse = fontInUse[i];
         int iFontCS = fontCS[i];
         if(strFontInUse == sFont && iFontCS == iCS)
             return i;
      }
      fontCS.Add(iCS);
      return fontInUse.Add(sFont);
   }

   ushort grayFill[10]={0x0000,0xE665,0xCCCC,0xB332,0x9999,0x7FFF,0x6666,0x4CCC,0x3333,0x1999};

   void PhIOContext::ConvertPhPenModel(const PhPenModel& phpm,PenModel& pm)
   {

      pm.m_nPenType=(uchar)phpm.fillModel.fFillType;
      switch(pm.m_nPenType)
      {
         //case kPhFillGray:
         //{
         //   fm.m_nFillType=kFillColor;
         //   ushort grey=grayFill[phfm.fFillIndex / 10] >> 8;
         //   fm.m_crColor=RGB(grey,grey,grey);
         //}
         //break;
      case kPhFillColor:
         pm.m_crColor = RGB(byte_clip(phpm.fillModel.color.fRed*255.f),byte_clip(phpm.fillModel.color.fGreen*255.f),byte_clip(phpm.fillModel.color.fBlue*255.f));
         break;
      case kPhFillPat:
         pm.m_nIndex=phpm.fillModel.fFillIndex - 1;
         break;
      default:
         //pm.m_crColor = RGB(byte_clip(phpm.fillModel.color.fRed*255.f),byte_clip(phpm.fillModel.color.fGreen*255.f),byte_clip(phpm.fillModel.color.fBlue*255.f));
         //pm.m_nPenType = kPhFillColor;
         break;
      }

      pm.m_nEndCap1=phpm.fEndCap[0];
      pm.m_nEndCap2=phpm.fEndCap[1];
      //pm.m_nStyle=(phpm.fPenStyle >> 8) & 0xF;
      pm.m_nStyle=0;
      pm.m_nAlignment=phpm.fAlignment - 1;
      if(phpm.width > 0.20 && phpm.width < 0.25)
      {
         pm.m_nWidth=(BYTE)129;
      }
      else if(phpm.width > 0.41 && phpm.width < 0.59)
      {
         pm.m_nWidth=(BYTE)0xa1;
      }
      else if(phpm.width > 0.66 && phpm.width < 0.84)
      {
         pm.m_nWidth=(BYTE)0xc1;
      }
      else
      {
         pm.m_nWidth=(BYTE)phpm.width;
      }
      //if(phpm.fPenStyle & 0x8000)
      //{
      //   pm.m_nWidth=0x81;
      //   pm.m_nWidth|=(phpm.fPenStyle >> 7) & 0x60;
      //}
      pm.m_iPenOpacity = byte_clip(phpm.fillModel.color.fOpacity * 255.f);

      for(int iStyle = 0; iStyle < kDashCodeEntryCount; iStyle++)
      {
         if(!memcmp(g_dashtableDefault[iStyle].seg,phpm.fDash.seg,sizeof(DashStyle)))
         {
            if(iStyle == 0)
            {
               pm.m_nStyle = 16;
            }
            else
            {
               pm.m_nStyle = iStyle;
            }
            break;
         }
      }
      // todo
      //pm.m_nPenType=phpm.fPenType;
      //switch(pm.m_nPenType)
      //{
      //case kPhPenColor:
      //   if(phpm.fColorIndex > 199)
      //   {
      //      PALETTEENTRY pe;
      //      pPalette->GetPaletteEntries(phpm.fColorIndex - 1,1,&pe);
      //      pm.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
      //   }
      //   else pm.m_crColor=PALETTEINDEX(phpm.fColorIndex - 1);
      //   break;
      //case kPhPenGray:
      //{
      //   pm.m_nPenType--;
      //   ushort grey=grayFill[phpm.fColorIndex / 10] >> 8;
      //   pm.m_crColor=RGB(grey,grey,grey);
      //}
      //break;
      //case kPhPenPat:
      //   // by TSM
      //   pm.m_nIndex=phpm.fColorIndex - 1;
      //   if(pm.m_nIndex > 70)
      //   {
      //      // by TSM
      //      //pm.m_hDib=CopyDIB((HBITMAP)embedDIBs[pm.m_nIndex-71]);
      //      //pm.m_nIndex = embedDIBsIndex[pm.m_nIndex-71];
      //      pm.m_nPenType--;
      //      // end by TSM
      //   }
      //   else pm.m_nPenType--;
      //   break;
      //}
      //pm.m_nEndCap1=(phpm.fPenStyle) & 0xF;
      //pm.m_nEndCap2=(phpm.fPenStyle >> 4) & 0xF;
      //pm.m_nStyle=(phpm.fPenStyle >> 8) & 0xF;
      //pm.m_nAlignment=phpm.fAlignment - 1;
      //pm.m_nWidth=phpm.fPenW & 0x1F;
      //if(phpm.fPenStyle & 0x8000)
      //{
      //   pm.m_nWidth=0x81;
      //   pm.m_nWidth|=(phpm.fPenStyle >> 7) & 0x60;
      //}
      //pm.m_iPenOpacity = phpm.fOpacity;
   }

   bool color_component_is_equal(char ch,float f)
   {

      if(fabs((double) ch / 255.f - (double) f) < 0.002)
      {

         return true;

      }

      return false;

   }

   void PhIOContext::ConvertPenModel(const PenModel& pm,PhPenModel& phpm)
   {
      // todo
      phpm.fillModel.fFillType=pm.m_nPenType;
      UINT idx = 0;
      if((pm.m_crColor & 0xff000000) != 0)
      {
         idx = pm.m_crColor & 0x00ffffff;
         PALETTEENTRY pe;
         pPalette->GetPaletteEntries(idx,1,&pe);
         phpm.fillModel.color.fRed      = pe.peRed / 255.0f;
         phpm.fillModel.color.fGreen    = pe.peGreen / 255.0f;
         phpm.fillModel.color.fBlue     = pe.peBlue / 255.0f;
         phpm.fillModel.color.fOpacity  = pm.m_iPenOpacity / 255.0f;
      }
      else
      {
         phpm.fillModel.color.fRed      = GetRValue(pm.m_crColor) / 255.0f;
         phpm.fillModel.color.fGreen    = GetGValue(pm.m_crColor) / 255.0f;
         phpm.fillModel.color.fBlue     = GetBValue(pm.m_crColor) / 255.0f;
         phpm.fillModel.color.fOpacity  = pm.m_iPenOpacity / 255.0f;
      }
      switch(pm.m_nPenType)
      {
      case kFillPatEmbed:
         phpm.fillModel.fFillType--;
         //   // by TSM
         //   //phfm.fFillIndex=72+embedDIBs.Add((DWORD)/*CopyDIB*/(fm.m_hDib));
         phpm.fillModel.fFillIndex=1 + m_numDIBPatterns + embedDIBs.Add((DWORD)/*CopyDIB*/(pm.m_hDib));
         //idx = m_numDIBPatterns + embedDIBs.Add((DWORD)/*CopyDIB*/(pm.m_hDib));
         break;
         case kFillPat:
         phpm.fillModel.fFillIndex=pm.m_nIndex + 1;
         //   idx = pm.m_nIndex;
            break;
         default:
         {
            phpm.fillModel.fFillIndex=idx + 1;
           // phpm.fillModel.fFillIndex=idx+1;
            for(int i = 0; i < pPalette->GetEntryCount(); i++)
            {
               PALETTEENTRY pe;
               pPalette->GetPaletteEntries(i,1,&pe);
               if(color_component_is_equal(pe.peRed,phpm.fillModel.color.fRed)
                  && color_component_is_equal(pe.peGreen,phpm.fillModel.color.fGreen)
                  && color_component_is_equal(pe.peBlue,phpm.fillModel.color.fBlue))
               {
                  phpm.fillModel.fFillIndex=i+1;
                  break;
               }
            }

         }
            break;
      }
      // Opacity/Transparency - Infinisys Ltd
      int iStyle = pm.m_nStyle;

      if(iStyle < 0 || iStyle >= 16)
      {

         iStyle = 0;

      }

      memcpy(phpm.fDash.seg,g_dashtableDefault[iStyle].seg,sizeof(DashStyle));

      //      pe.peRed=GetRValue(pm.m_crColor);
      //      pe.peGreen=GetGValue(pm.m_crColor);
      //      pe.peBlue=GetBValue(pm.m_crColor);
      //      pe.peFlags=0;
      //      pPalette->SetPaletteEntries(phpm.fColorIndex - 1,1,&pe);
      //      VERIFY(pPalette->SetPaletteEntries(phpm.fColorIndex - 1,1,&pe));
      //      break;
      //   }
      //   else phpm.fColorIndex=(pm.m_nIndex & 0xFF) + 1;
      //   break;
      //}

      phpm.fEndCap[0] = pm.m_nEndCap1;
      phpm.fEndCap[1] = pm.m_nEndCap2;
      //phpm.fPenStyle|=(pm.m_nStyle & 0xF) << 8;
      phpm.fAlignment=pm.m_nAlignment + 1;
      if(pm.m_nWidth == 129)
      {
         
         phpm.width = 0.24;

      }
      else if(pm.m_nWidth == 0xa1)
      {
         
         phpm.width = 0.5;

      }
      else if(pm.m_nWidth == 0xc1)
      {

         phpm.width = 0.75;

      }
      else
      {

         phpm.width = pm.m_nWidth;

      }

      //if(pm.m_nWidth & 0x80)
      //{
      //   phpm.fPenStyle|=0x8000;
      //   phpm.fPenStyle|=(pm.m_nWidth & 0x60) << 7;
      //}
      //phpm.fOpacity = pm.m_iPenOpacity;
   }

   void PhIOContext::ConvertPhFillModel(const PhFillModel& phfm,FillModel& fm)
   {


      fm.m_nFillType=(uchar)phfm.fFillType;
      switch(fm.m_nFillType)
      {
         //case kPhFillGray:
         //{
         //   fm.m_nFillType=kFillColor;
         //   ushort grey=grayFill[phfm.fFillIndex / 10] >> 8;
         //   fm.m_crColor=RGB(grey,grey,grey);
         //}
         //break;
      case kPhFillColor:
         fm.m_crColor = RGB(byte_clip(phfm.color.fRed*255.f),byte_clip(phfm.color.fGreen*255.f),byte_clip(phfm.color.fBlue*255.f));
         //fm.m_nIndex=phfm.fFillIndex;
         break;
      case kPhFillPat:
         fm.m_nIndex=phfm.fFillIndex - 1;
         break;
      }

      if(fm.m_nFillType == 3)
      {
         fm.m_nFillType = kPhNoFill;
         fm.m_crColor = 0;
         fm.m_nIndex = 0;
      }

      //   if(phfm.fFillIndex > 199)
      ////   {
      ////      PALETTEENTRY pe;
      ////      pPalette->GetPaletteEntries(phfm.fFillIndex - 1,1,&pe);
      ////      fm.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
      ////   }
      ////   else fm.m_crColor=PALETTEINDEX(phfm.fFillIndex - 1);
      //   break;
      //case kPhFillPat:
      //   fm.m_nIndex=phfm.fFillIndex - 1;
      //   //if(fm.m_nIndex>70)
      //   //{
      //   // by TSM
      //   //fm.m_nFillType=kFillPatEmbed;
      //   //fm.m_hDib=CopyDIB((HBITMAP)embedDIBs[fm.m_nIndex-71]);
      //   //fm.m_nIndex = embedDIBsIndex[fm.m_nIndex-71];
      //   // end by TSM
      //   //}
      //   break;
      //}
      //// Opacity/Transparency - Infinisys Ltd
      fm.m_iFillOpacity = byte_clip(phfm.color.fOpacity*255.f);
   }

   void PhIOContext::ConvertFillModel(const FillModel& fm,PhFillModel& phfm)
   {
      // todo
      phfm.fFillType=fm.m_nFillType;
      UINT idx = 0;
      if((fm.m_crColor & 0xff000000) != 0)
      {
         idx =fm.m_crColor & 0x00ffffff;
         PALETTEENTRY pe;
         pPalette->GetPaletteEntries(idx,1,&pe);
         phfm.color.fRed      = pe.peRed / 255.0f;
         phfm.color.fGreen    = pe.peGreen / 255.0f;
         phfm.color.fBlue     = pe.peBlue / 255.0f;
         phfm.color.fOpacity  = fm.m_iFillOpacity / 255.0f;
      }
      else
      {
         phfm.color.fRed      = GetRValue(fm.m_crColor) / 255.0f;
         phfm.color.fGreen    = GetGValue(fm.m_crColor) / 255.0f;
         phfm.color.fBlue     = GetBValue(fm.m_crColor) / 255.0f;
         phfm.color.fOpacity  = fm.m_iFillOpacity / 255.0f;
      }
      switch(fm.m_nFillType)
      {
      case kFillPatEmbed:
         phfm.fFillType--;
      //   // by TSM
      //   //phfm.fFillIndex=72+embedDIBs.Add((DWORD)/*CopyDIB*/(fm.m_hDib));
         phfm.fFillIndex=1 + m_numDIBPatterns + embedDIBs.Add((DWORD)/*CopyDIB*/(fm.m_hDib));
         break;
      case kFillPat:
         phfm.fFillIndex= fm.m_nIndex + 1;
         break;
      default:
         phfm.fFillIndex= idx;
         break;

      }
      // Opacity/Transparency - Infinisys Ltd
      
   }

   void PhIOContext::ConvertPhParaAttr(const PhParallelAttr& phpa,ParallelAttrs& pa)
   {
      //pa.m_nAlign= (phpa.fAlignment + 2) % 3;
      pa.m_nAlign= 3 - phpa.fAlignment;
      pa.m_nEndCap=phpa.fEndCap;
      pa.m_nJoint=phpa.fLineJoin;
      ConvertPhFillModel(phpa,pa.m_Fill);
      pa.m_fWidth=phpa.fWidth;
   }

   void PhIOContext::ConvertParaAttr(const ParallelAttrs& pa,PhParallelAttr& phpa)
   {
      ConvertFillModel(pa.m_Fill,phpa);
      phpa.fWidth=pa.m_fWidth;
      //phpa.fAlignment= (pa.m_nAlign + 1) % 3;
      phpa.fAlignment= 3 - pa.m_nAlign;
      phpa.fEndCap =  (pa.m_nEndCap) & 0x3 ;
      phpa.fLineJoin = pa.m_nJoint & 0x1 ;

   }

   void PhIOContext::ConvertPhTextParams(const PhTextParams& phtp,TextParams& tp)
   {
      // todo
      //ConvertPhFont(phtp.fTextFont,phtp.fTextSize,phtp.fTextStyle,tp.m_lf);

      strncpy(tp.m_lf.lfFaceName,phtp.fTextFontName,min(phtp.fTextFontName.GetLength() + 1,sizeof(tp.m_lf.lfFaceName)));
      tp.m_lf.lfHeight = phtp.fTextSize;
      switch(phtp.fHorizTextJust)
      {
      default:
      case teHorizJustLeft:
         tp.m_nJust=kAlignLeft;
         break;
      case teHorizJustCenter:
         tp.m_nJust=kAlignCenter;
         break;
      case teHorizJustRight:
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
      // todo

      //phtp.fMargins.l = 3.0f;
      //phtp.fMargins.t = 3.0f;
      //phtp.fMargins.r = 3.0f;
      //phtp.fMargins.b = 3.0f;

      long lFont = 0;
      long lSize = 0;
      //ConvertLogFont(tp.m_lf,lFont, lSize,phtp.fTextStyle);
      phtp.fTextFontName = tp.m_lf.lfFaceName;
      phtp.fTextSize = tp.m_lf.lfHeight;

      switch(tp.m_nSpacing)
      {
      case ::kSingleSpace:
         phtp.fTextSpace=::formatMD70::kSingleSpace;
         break;
      case ::k1_5Space:
         phtp.fTextSpace=::formatMD70::kOne_halfSpace;
         break;
      case ::kDoubleSpace:
         phtp.fTextSpace=::formatMD70::kDoubleSpace;
         break;
      }
      switch(tp.m_nJust)
      {
      case ::kAlignLeft:
         phtp.fHorizTextJust=teHorizJustLeft;
         break;
      case ::kAlignCenter:
         phtp.fHorizTextJust=teHorizJustCenter;
         break;
      case ::kAlignRight:
         phtp.fHorizTextJust=teHorizJustRight;
         break;
      default:
         phtp.fHorizTextJust=teHorizJustUndefined;
      }
      switch(tp.m_nCase)
      {
      case -1:
         phtp.fTextCase=kPhAsIs;
         break;
      case ::kCaseUpper:
         phtp.fTextCase=kPhUppercase;
         break;
      case ::kCaseLower:
         phtp.fTextCase=kPhLowercase;
         break;
      case ::kCaseTitle:
         phtp.fTextCase=kPhTitleText;
         break;
      }
      switch(tp.m_nVAlign)
      {
      case ::kAlignTop:
         phtp.fVertTextAlign=::kAlignTop;
         break;
      case ::kAlignMiddle:
         phtp.fVertTextAlign=::kAlignMiddle;
         break;
      case ::kAlignBottom:
         phtp.fVertTextAlign=::kAlignBottom;
         break;
      }
   }

   void PhIOContext::ConvertPhDimStd(const PhDimStd& phstd,DimStandard& std)
   {
      memset(&std,0,sizeof(std));
      std.nType=phstd.dsType;
      //std.WitnessExt=mtFixToReal(phstd.dsWitnessExt);
      //std.WitnessLineGap=mtFixToReal(phstd.dsWitnessLineGap);
      //std.DimTextGap=mtFixToReal(phstd.dsDimTextGap);
      //std.LeaderLen=mtFixToReal(phstd.dsLeaderLen);
      //std.OutsideLineLen=mtFixToReal(phstd.dsOutsideLineLen);
      //std.nToleranceTextScale=phstd.dsToleranceTextScale;
      //std.nToleranceSpaceScale=phstd.dsToleranceSpaceScale;
      //std.CircleCenterLen=mtFixToReal(phstd.dsCircleCenterLen);
      //std.CircleCenterGap=mtFixToReal(phstd.dsCircleCenterGap);
      //std.CircleCenterExt=mtFixToReal(phstd.dsCenterExt);
      std.WitnessExt=phstd.dsWitnessExt.m_d; // Infinisys Ltd. 2021
      std.WitnessLineGap = phstd.dsWitnessLineGap.m_d; // Infinisys Ltd. 2021
      std.DimTextGap = phstd.dsDimTextGap.m_d; // Infinisys Ltd. 2021
      std.LeaderLen = phstd.dsLeaderLen.m_d; // Infinisys Ltd. 2021
      std.OutsideLineLen = phstd.dsOutsideLineLen.m_d; // Infinisys Ltd. 2021
      std.nToleranceTextScale=phstd.dsToleranceTextScale; // Infinisys Ltd. 2021
      std.nToleranceSpaceScale=phstd.dsToleranceSpaceScale; // Infinisys Ltd. 2021
      std.CircleCenterLen = phstd.dsCircleCenterLen.m_d; // Infinisys Ltd. 2021
      std.CircleCenterGap = phstd.dsCircleCenterGap.m_d; // Infinisys Ltd. 2021
      std.CircleCenterExt = phstd.dsCenterExt.m_d; // Infinisys Ltd. 2021
   }

   void PhIOContext::ConvertDimStd(const DimStandard& std,PhDimStd& phstd)
   {
      memset(&phstd,0,sizeof(phstd));
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

   void PhIOContext::ConvertPhDimFmt(const PhDimFormat2& phfmt,DimFormat& fmt) // MD70 - Infinisys Ltd.
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
      fmt.sLabel=phfmt.dfDimLabel;
      fmt.nRoundoff = 0;
   }

   void PhIOContext::ConvertDimFmt(const DimFormat& fmt,PhDimFormat2& phfmt)// MD70 - Infinisys Ltd.
   {
      //memset(&phfmt,0,sizeof(phfmt));
      phfmt.dfType=fmt.nType;
      phfmt.dfTextCentered=fmt.bTextCentered;
      phfmt.dfTextFramed=fmt.bTextFramed;
      phfmt.dfArrowsInside=fmt.bArrowsInside;
      phfmt.dfUseLabel=fmt.bUseLabel;
      phfmt.dfUseWitness=fmt.bUseWitness;
      phfmt.dfTextDisplay=(::formatMD70::TextDisplayType) fmt.nTextDisplay;
      phfmt.dfLeader= (::formatMD70::LeaderType) fmt.nLeader;
      phfmt.dfRDSymbol=(::formatMD70::RDSymbolType) fmt.nRDSymbol;
      phfmt.dfTolerance=(::formatMD70::ToleranceType) fmt.nTolerance;
      phfmt.dfTolerance1=fmt.Tolerance1;
      phfmt.dfTolerance2=fmt.Tolerance2;

      if(fmt.bUseLabel && !fmt.sLabel.IsEmpty())
      {

         phfmt.dfDimLabel = fmt.sLabel;

      }
      else
      {

         phfmt.dfDimLabel.Empty();

      }
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
      //fmt.sLabel=phfmt.dfDimLabel;
      fmt.sLabel="";
      fmt.nRoundoff = 0;
   }

   void PhIOContext::ConvertDimFmt(const DimFormat& fmt,PhDimFormat& phfmt)
   {
      memset(&phfmt,0,sizeof(phfmt));
      phfmt.dfType=fmt.nType;
      phfmt.dfTextCentered=fmt.bTextCentered;
      phfmt.dfTextFramed=fmt.bTextFramed;
      phfmt.dfArrowsInside=fmt.bArrowsInside;
      phfmt.dfUseLabel=fmt.bUseLabel;
      phfmt.dfUseWitness=fmt.bUseWitness;
      phfmt.dfTextDisplay=(::formatMD70::TextDisplayType) fmt.nTextDisplay;
      phfmt.dfLeader= (::formatMD70::LeaderType) fmt.nLeader;
      phfmt.dfRDSymbol=(::formatMD70::RDSymbolType) fmt.nRDSymbol;
      phfmt.dfTolerance=(::formatMD70::ToleranceType) fmt.nTolerance;
      phfmt.dfTolerance1=fmt.Tolerance1;
      phfmt.dfTolerance2=fmt.Tolerance2;
      phfmt.dfDimLabelID=fmt.bUseLabel && !fmt.sLabel.IsEmpty()?0:1;
   }

   void PhIOContext::ConvertLogFont(const LOGFONT &lf,long& nFont,long& nSize,Style& nStyle)
   {
      nFont=UseFont(lf.lfFaceName,lf.lfCharSet);
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

   void PhIOContext::ConvertLogFont(const LOGFONT &lf,CString & strFontName,float & fSize,Style& nStyle)// MD70 - Infinisys Ltd.
   {
      strFontName = lf.lfFaceName;
      if(lf.lfHeight < 0)
      {
         HDC hDC=GetDC(NULL);
         fSize=MulDiv(-lf.lfHeight,72,GetDeviceCaps(hDC,LOGPIXELSY));
         ReleaseDC(NULL,hDC);
      }
      else fSize=(short)lf.lfHeight;
      nStyle = 0;
      if(lf.lfWeight == FW_BOLD) nStyle|=0x1; // NB: some bold fonts have other values
      if(lf.lfItalic) nStyle|=0x2;
      if(lf.lfUnderline) nStyle|=0x4;
   }


   void PhIOContext::ConvertFont(CString strFontName,float fSize,LOGFONT& lf) // MD70 - Infinisys Ltd.
   {
      HDC hDC=GetDC(NULL);
      lf.lfHeight=-MulDiv(fSize,GetDeviceCaps(hDC,LOGPIXELSY),72);
      ReleaseDC(NULL,hDC);
      lf.lfWeight=FW_NORMAL;
      lf.lfItalic=0;
      lf.lfUnderline=0;
      lstrcpyn(lf.lfFaceName,strFontName,LF_FACESIZE);
      int iCharset = ANSI_CHARSET;

   }

   void PhIOContext::ConvertPhFont(short nFont,short nSize,Style nStyle,LOGFONT &lf)
   {
      HDC hDC=GetDC(NULL);
      lf.lfHeight=-MulDiv(nSize,GetDeviceCaps(hDC,LOGPIXELSY),96);
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

   void PhIOContext::ConvertC2P(const CString& str,char * pBuf,int nMaxSize)
   {
      uchar len=min(str.GetLength(),nMaxSize - 1);
      *pBuf++=len;
      for(int i=0; i < len; i++)
         pBuf[i]=pc2mac[(uchar)str[i]];
   }
   bool OffsetHeader::read(pointer & pointer,memory & mem,CFile & file,int iMarker)
   {

      if(file.Seek(iMarker,CFile::SeekPosition::begin) != iMarker)
      {

         return false;

      }

      if(file.Read(this,sizeof(*this)) != sizeof(*this))
      {

         return false;

      }

      pointer.synch_type(this);

      pointer.swap(uiOffset);

      pointer.swap(uiLength);

      if(!mem.read_file(file,uiOffset,uiLength))
      {

         return false;

      }

      pointer.pch = mem.get_data();

      pointer.pend = pointer.pch + uiLength;

      return true;

   }


   void LayerStruct::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(fNumShapes);
      serialize(fLayerOffset);
      serialize(fLayerLength);
      serialize(fReserved1);
      serialize(fReserved2);
      serialize(fReserved3);
      serialize(fReserved4);
      serialize(fReserved5);
      serialize(fReserved6);
      serialize(fReserved7);
      serialize(fReserved8);
      serialize(fAdditionalFlags);
      serialize(fLayerActiveFlag);
      serialize(fScaleIndex);
      serialize(fCustomRatio);
      serialize(fSnapIndex);
      serialize(fLayerFlags);
      serialize.s1(fCFRLayerName);

   }


   void FlatPageFormat::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(theSize);

      if(theSize != 0)
      {
         serialize(handleLength);
         serialize.serialize_array(data,handleLength);
      }

   }


   void FlatPrintSettings::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(theSize);

      if(theSize != 0)
      {
         serialize(handleLength);
         serialize.serialize_array(data,handleLength);
      }

   }


   void PhReportRowInfo::operator()(serialize & serialize)
   {
         
      complex_type_context complextypecontext(&serialize);

      serialize(fRowFlags);
      serialize(fRowType);
      serialize(fRowID);
      serialize(theLong);

   }


   void PhTextRow::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(size);
      serialize(sizeText);
      serialize(fChanged);
      serialize(fRowType);
      serialize(fSpare);
      serialize.serialize_static_array(STATIC_ARRAY(fJust));
      serialize(fUseStyle);
      serialize.serialize_static_array(STATIC_ARRAY(unused));
      serialize.serialize_static_array(STATIC_ARRAY(Str));

   }

   
   void PhCriteriaRow::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(size);
      serialize(fChanged);
      serialize(fRowType);
      serialize(fSpare);
      serialize.serialize_static_array(STATIC_ARRAY(fFieldRefNum));

   }


   void PhFunctionRow::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(size);
      serialize(sizeText);
      serialize(fChanged);
      serialize(fRowType);
      serialize(fSpare);
      serialize.serialize_static_array(STATIC_ARRAY(fJust));
      serialize(fResult);
      serialize(dummy);
      serialize(fUseStyle);
      serialize.serialize_static_array(unused, sizeof(unused));
      serialize.serialize_static_array(Str, 6);

   }

   PhReport::~PhReport()
   {
      for(int i = 0; i < rowa.GetCount(); i++)
      {
         delete rowa[i];

      }


   }

   void PhReport::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(fVersion);
      serialize(fCFRTitle);
      serialize(&fDefaultTextParams);
      serialize(fDefaultColumnWidth);
      serialize.serialize_static_array(fColumnWidths,6);
      serialize(fRowHeight);
      serialize(fNumRows);
      serialize(fLinkInfo_fLinkType);
      serialize(fLinkInfo_fLinkFileSpec_vRefNum);
      serialize(fLinkInfo_fLinkFileSpec_parID);
      serialize.serialize_string(fLinkInfo_fLinkFileSpec_name,linkNameLength);
      serialize(fLinkInfo_fLastLinkError);
      serialize(fLinkInfo_fUnitType);
      serialize.serialize_static_array(reservedfiller,32);
      serialize(FlatPageFormat);
      serialize(FlatPrintSettings);
      serialize(rowInfoCount);
      serialize.serialize_array(rowinfoa,rowInfoCount);

      ASSERT(rowInfoCount == fNumRows);

      for(int i = 0;i < min(fNumRows, rowInfoCount); i++)
      {

         int iRowType = rowinfoa[i].fRowType;

         if(iRowType == kPhTextRow)
         {
            rowa.Add(new PhTextRow);
         }
         else if(iRowType == kPhCriteriaRow)
         {
            rowa.Add(new PhCriteriaRow);
         }
         else if(iRowType == kPhFunctionRow)
         {
            rowa.Add(new PhFunctionRow);
         }

         if(iRowType)
         {
            serialize(rowa.ElementAt(rowa.GetUpperBound()));

         }

      }


   }


   void  PhPaletteEntry::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(ciRGB);
      serialize(ciUsage);
      serialize(ciTolerance);
      serialize.serialize_static_array(ciDataFields,3);

   }


   void  PhGuideEntry::operator()(serialize & serialize)
   {

      serialize(fType);
      serialize(fLineValue);

   }


   void  PhGuidesInfo::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      serialize(numGuides); //Repeat the following for each guide (from first to numGuides) if numGuides > 0
      serialize(guides, numGuides);

   }


   void  PhMDShape::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      if(serialize.m_bWrite)
      {
         serialize.m_piocontext->ullBeg = serialize.m_pfile->GetPosition();
         serialize((long &) fID);
         serialize(lObjectSize);
         serialize(lInfoLocation);
      }
      serialize(cDatum);
      serialize(the64); 
      //unsigned char * p = (unsigned char *) &the64;
      //if (p[4] == 255 && p[5] == 255 && p[6] == 255 && p[7] == 255)
      //{
      //   p[4] = 0;
      //   p[5] = 0;
      //   p[6] = 0;
      //   p[7] = 0;

      //}
      serialize(cOwnerGroup);
      //p = (unsigned char *)&cOwnerGroup;
      //if (p[4] == 255 && p[5] == 255 && p[6] == 255 && p[7] == 255)
      //{
      //   p[4] = 0;
      //   p[5] = 0;
      //   p[6] = 0;
      //   p[7] = 0;

      //}
      serialize(cRef);
      serialize(theLong);
      serialize(the64ObjectInfo);
      serialize(theAngle);
      serialize(&cPenModel);
      serialize(cShapeFlags);
      serialize(fClassification);
      serialize(cShapeReserved1);
      serialize(cShapeReserved2);
      serialize(cShapeReserved3);
//      serialize(cShapeReserved4);

   }

   void  PhMDFillShape::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      PhMDShape::operator()(serialize);
      serialize(&cFillModel);
      serialize(cEvenOddFill);
      serialize(cFillShapeReserved);
      serialize(cFillShapeReserved1);
      serialize(cFillShapeReserved2);


   }

   void  PhMDDimension::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(fStatus);
      serialize(fShapeFont);
      serialize(fShapeFontSize);
   }

   void  PhMDAssocDim::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDDimension::operator()(serialize);
      serialize(cValue);
      serialize(&format);
      serialize(&standard);
      serialize(cTextExtent);
      serialize(cDimStrDraw);
      serialize(tol1Draw);
      serialize(tol2Draw);
      serialize(dimStr);
      serialize(tol1Str);
      serialize(tol2Str);
      serialize(cReserved);
      //serialize(handleSize); //String	DimLabel(if handleSize > 0)	;
      //if(handleSize > 0)
      //{
      //   serialize(dimLabel);
      //}
   }

   void  PhMDText::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);
      PhMDFillShape::operator()(serialize);
      serialize(cParagraph);
      serialize(cTextJust);
      serialize(data[0]);
      serialize(data[1]);
      serialize(data[2]);
      serialize(data[3]);
      serialize(textDataSize);
      if(serialize.m_bWrite)
      {

         serialize.m_pfile->Write((const char *) m_strRtf,m_strRtf.GetLength());

      }
      else if(textDataSize >= 0)
      {
         
         LPSTR lpsz = m_strRtf.GetBufferSetLength(textDataSize + 1);

         UINT uiRead;
         //if(serialize.m_pfile != NULL)
         //{

         //   uiRead = serialize.m_pfile->Read(lpsz,textDataSize);
         //}
         //else
         {
            
            uiRead = min(textDataSize,serialize.m_pointer.pend - serialize.m_pointer.pch);
            
            memcpy(lpsz,serialize.m_pointer.pch, uiRead);



         }

         ASSERT(uiRead == textDataSize);

         serialize.m_pointer.pch += textDataSize;

         m_strRtf.ReleaseBuffer(textDataSize);

      }


   }


   void  PhMDProtoArc::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(Radii);
      serialize(cStartAng);
      serialize(cEndAng);
      serialize(cBackwards);
      serialize(StartPt);
      serialize(EndPt);
      serialize(ThirdPt);
      serialize((long&)method);
      serialize(fCenter);
      serialize(cProtoReserved);
      serialize(cProtoReserved1);
      serialize(cProtoReserved2);

   }


   void  PhMDLine::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDShape::operator()(serialize);
      serialize(cConAngle);
      serialize(mHeadArrowSize);
      serialize(fP1);
      serialize(fP2);
      serialize(cReserved1);
      serialize(cReserved2);

   }


   void PhMDPolygon::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(cClosed);
      serialize(vertexCount); //DDBPoint structs written vertexCount times
      serialize(data,vertexCount);
      serialize(cReserved);
      serialize(cReserved1);
      serialize(cReserved2);
      serialize(cReserved3);
   }


   void  PhMDRectangle::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize.serialize_static_array(fPoint,4);
      serialize(cReserved);


   }


   void  PhMDRndRectangle::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDRectangle::operator()(serialize);
      serialize((long &)cCornerType);
      serialize(cDiagonals);
      serialize(cReserved);
      serialize(cReserved1);
      serialize(cReserved2);

   }

   void  PhMDFreeSpline::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(cClosed);
      serialize(vertexCount);
      serialize(data,vertexCount);

   }
   void  PhMDPolySpline::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(cClosed);
      serialize(vertexCount);
      serialize(data,vertexCount);

   }
   void  PhMDCircle::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize((long &)method);
      serialize(radius);
      switch(method)
      {
      case eCenterRadius:
         serialize(data,2);
         break;
      case ::formatMD70::eDiameter:
         serialize(data,3);
         break;
      case ::formatMD70::e3Point:
         serialize(data,4);
         //serialize(data,3);
         break;
      }
      serialize(fReserved);
      serialize(fReserved1);
      serialize(fReserved2);
      serialize(fReserved3);


   }
   void  PhMDArc::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);
      PhMDProtoArc::operator()(serialize);
      serialize(cReserved);
   }
   void  PhMDEllipse::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize((long&)method);
      serialize(fCenter);
      serialize(radius);
      serialize(cReserved);
      serialize(cReserved1);
      serialize(cReserved2);
      serialize(cReserved3);
   }
   void  PhMDSpecificBezier::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(cClosed);
      serialize(numPoints);
      serialize(data,numPoints);

   }
   void  PhMDCubic::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);
      PhMDFillShape::operator()(serialize);
      serialize(cClosed);
      serialize(vertexCount);
      serialize(data,3 * vertexCount + 1);

   }
   //void  PhMDText::operator()(serialize & serialize)
   //{
   //   complex_type_context complextypecontext(&serialize);

   //   PhMDFillShape::operator()(serialize);
   //   serialize(fInPixels);
   //   serialize(fMargins);
   //   serialize(cParagraph);
   //   serialize(cObjOpen);
   //   serialize(cfZoomFactor);
   //   serialize((long &)cTextSpace);
   //   serialize(cTextJust);
   //   serialize(destRect);
   //   serialize(viewRect);
   //   serialize(teLength);
   //   serialize.serialize_static_array(data,4);
   //   serialize(chars,teLength);
   //   serialize(scrCount);
   //   //If scrCount != 0 then the following fields(colored) are written scrCount times	;
   //   serialize(scripta,scrCount);
   //   serialize(fReserved);
   //   serialize(fReserved2);

   //}

   void  PhMDBitmap::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize.serialize_static_array(data,4);
      serialize(cSrcRect);
      serialize(cMapHdlIndex);
      serialize(fBitMapAngle);
      serialize(fReserved);

   }

   void  PhMDGroup::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      PhMDShape::operator()(serialize);
      serialize(itemsCount);
      serialize(pointers,itemsCount);
      serialize(fReserved);

   }



   void  PhMDMetaPoly::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(cStartPoint);
      serialize(cClosed);
      serialize(cNumValidMembers);
      serialize(cNumClosedMembers);
      serialize(itemsCount);
      serialize(shapes,itemsCount);
      serialize(fReserved);
      serialize(fReserved1);
      serialize(fReserved2);

   }

   void  PhMDLineDim::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDAssocDim::operator()(serialize);
      serialize(refPt1);
      serialize(refPt2);
      serialize(anchorPt);
      serialize(witnessStart1);
      serialize(witnessEnd1);
      serialize(witnessStart2);
      serialize(witnessEnd2);
      serialize(dimLineStart);
      serialize(dimLineEnd);
      serialize(extLineStart);
      serialize(extLineEnd);
      serialize(leaderLineStart);
      serialize(leaderLineEnd);
      serialize(theLong);
      serialize(mHeadArrowSize);
      serialize(mLineDimention);
      serialize(fReserved);

   }

   void  PhMDCenterDim::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);
      PhMDAssocDim::operator()(serialize);
      serialize(cCenterPt);
      serialize(cRadii);
      serialize(fReserved);
   }
   void  PhMDAngularDim::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);
      PhMDAssocDim::operator()(serialize);
      serialize(ref1DBPoint);
      serialize(ref2DBPoint);
      serialize(lineStartDBPoint);
      serialize(lineEndDBPoint);
      serialize(anchorDBPoint);
      serialize(extDBPoint);
      serialize(centerDBPoint);
      serialize(witnessStart1DBPoint);
      serialize(witnessEnd1DBPoint);
      serialize(witnessStart2DBPoint);
      serialize(witnessEnd2DBPoint);
      serialize(cRadius);
      serialize(cStartAngle1);
      serialize(cArcAngle1);
      serialize(cCaps1);
      serialize(cStartAngle2);
      serialize(cArcAngle2);
      serialize(cCaps2);
      serialize(mHeadArrowSize);
      serialize(fReserved);

   }


   void PhMDDim::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);


      PhMDLine::operator()(serialize);
      serialize(fStatus);
      serialize(cValue);
      serialize(cDimStrHdl);
      serialize(cReserved);

   }


   void  PhMDDimStr::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);


      PhMDDimension::operator()(serialize);
      serialize(cAnchorPt);
      serialize(cStrOffset);
      serialize(cObjHdl);
      serialize(str);
      serialize(cReserved);

   }

   void  PhMDArea::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      PhMDDimension::operator()(serialize);

      serialize(cAreaValue);
      serialize(cShapeHdl);
      serialize(cPosOrOffset);
      serialize(DimString);
      serialize(cReserved1);
      serialize(cReserved2);
      serialize(cReserved3);
      serialize(cReserved4);

   }

   void  PhMDParallelLine::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDShape::operator()(serialize);
      serialize(&fParallelAttr);
      serialize(fFillParallelShapeReserved);
      serialize(cConAngle);
      serialize.serialize_static_array(data,6);
      serialize(cReserved);
      serialize(cReserved1);
   }

   void  PhMDParallelPolygon::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);
      serialize(cClosed);
      serialize(vertexCount);
      serialize(data,3 * vertexCount);
      serialize(&fParallelAttr);
      serialize(fFillParallelShapeReserved);

   }
   void  PhMDWall::operator()(serialize & serialize)
   {

      complex_type_context complextypecontext(&serialize);

      PhMDFillShape::operator()(serialize);

      external_type_context externaltypecontext(&serialize);

      serialize(segmentsCount);
      UInt64 u;
      if(serialize.m_bWrite)
      {

         for(int i = 0; i < segmentsCount; i++)
         {

            u = segments[i].x;
            serialize(u);
            u = segments[i].y;
            serialize(u);

         }

      }
      else
      {

         segments.SetSize(segmentsCount);

         for(int i = 0; i < segmentsCount; i++)
         {

            serialize(u);
            segments[i].x = u;
            serialize(u);
            segments[i].y = u;

         }

      }
      serialize(pointsCount);
      serialize(data,pointsCount);
      serialize(&fParallelAttr);
      serialize(fFillParallelShapeReserved);

   }

   void PhMDHatch::operator()(serialize & serialize)
   {
      complex_type_context complextypecontext(&serialize);

      PhMDPolygon::operator()(serialize);

   }


   bool memory::read_resource(HINSTANCE hinstance,UINT nID,LPCTSTR lpcszType)
   {

      HRSRC hrsrc = FindResource(hinstance,MAKEINTRESOURCE(nID),lpcszType);
      HGLOBAL hglobalResource;
      uint32_t dwResourceSize;
      BOOL bOk;
      UINT FAR* pResource;

      if(hrsrc == NULL)
         return FALSE;

      hglobalResource = LoadResource(hinstance,hrsrc);

      if(hglobalResource == NULL)
         return FALSE;

      dwResourceSize = SizeofResource(hinstance,hrsrc);

      if(hglobalResource != NULL)
      {

         bOk = FALSE;

         pResource = (UINT FAR*) LockResource(hglobalResource);

         allocate(dwResourceSize);

         memcpy(m_pdata,pResource, m_iSize);

         FreeResource(hglobalResource);

         return bOk;
      }

      return FALSE;

   }


   void serialize::operator()(CPackedDIB * pdib)
   {

      if(m_bWrite)
      {

         PixelPattern pattern;

         HBITMAP hbitmap = pdib->CreateDIBSection();

         BITMAP b;

         memset(&b,0,sizeof(b));

         ::GetObject(hbitmap,sizeof(BITMAP),&b);

         pattern.fBitsPerChannel = 8;
         pattern.fWidth = b.bmWidth;
         pattern.fHeight = b.bmHeight;
         pattern.fRotation = 0.f;

         pattern.data.SetSize(b.bmWidth * b.bmHeight);

         COLORREF * lpDst = (COLORREF *)pattern.data.GetData();

         char * lpBits = (char *)b.bmBits;

         if(b.bmBitsPixel == 1)
         {

            for(int y = 0; y < b.bmHeight; y++)
            {

               unsigned char * pline = (unsigned char *)&lpDst[b.bmWidth * y];

               for(int x = 0; x < b.bmWidth; )
               {

                  unsigned char byte = lpBits[b.bmWidthBytes * (b.bmHeight - y - 1) + x / 8];

                  for(int bit = 0; bit < 8; bit++,x++)
                  {

                     COLORREF cr = !((1 << bit) & byte) ? 0xff000000: 0xffffffff;

                     *(COLORREF*)pline = cr;

                     //unsigned char ch = pline[0];

                     //pline[0] = pline[2];

                     //pline[2] = ch;

                     pline +=4;

                  }

               }

            }

         }
         else if(b.bmBitsPixel == 32)
         {

            for(int y = 0; y < b.bmHeight; y++)
            {

               unsigned char * pline = (unsigned char *)&lpDst[b.bmWidth * y];

               memcpy(pline,&lpBits[b.bmWidthBytes * (b.bmHeight - y - 1)],b.bmWidth * sizeof(COLORREF));

               for(int x = 0; x < b.bmWidth; x++)
               {

                  unsigned char ch = pline[0];

                  pline[0] = pline[2];

                  pline[2] = ch;

                  pline +=4;

               }


            }

         }
         else
         {

            throw "unsupported bit depth";

         }

         operator()(&pattern);

         ::DeleteObject(hbitmap);

      }

   }


   void serialize::operator()(CGradientPack * pgradient)
   {

      if(m_bWrite)
      {

         BlendPattern pattern;

         pattern.fLinearNotRadial = !pgradient->m_bRadial;
         pattern.fCenterX  = pgradient->m_dX + 0.5f;
         pattern.fCenterY  = pgradient->m_dY + 0.5f;
         pattern.fRotation = pgradient->m_dRotation;

         for(int iGrad = 0; iGrad < min(pgradient->m_cra.GetCount(),pgradient->m_da.GetCount()); iGrad++)
         {

            COLORREF cr = pgradient->m_cra[iGrad];

            RGBAPos pos;

            pos.fBlue = GetBValue(cr) / 255.f;
            pos.fGreen = GetGValue(cr) / 255.f;
            pos.fRed = GetRValue(cr) / 255.f;
            pos.fOpacity = 1.0;
            pos.fPos = pgradient->m_da[iGrad];
            
            pattern.data.Add(pos);

         }

         operator()(&pattern);

      }

   }


   void serialize::operator()(CPackedItem & item)
   {

      if(m_bWrite)
      {

         PatternBase base;

         ULONGLONG posBeg = m_pfile->GetPosition();

         operator()(&base);

         if(item.m_etype == packed_type_dib)
         {

            operator()(item.m_pdib);

         }
         else if(item.m_etype == packed_type_gradient)
         {

            operator()(item.m_pgradient);

         }
         else
         {

            throw "unpexpected pattern type";

         }

         ULONGLONG posEnd = m_pfile->GetPosition();

         m_pfile->Seek(posBeg,CFile::begin);

         base.Id = item.m_etype == packed_type_dib ? IDPixelPattern: IDBlendPattern;
         
         base.Size = posEnd - posBeg;

         operator()(&base);

         m_pfile->Seek(posEnd,CFile::begin);

      }


   }


} // namespace formatMD70




