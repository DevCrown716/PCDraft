#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#include "FilletContext.h"
#include "ChamferContext.h"

#include "Layer.h"
#include "PhIOStruct.h"
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

HBITMAP dib32_from_hbitmap(HBITMAP);

const Coord kAlmostSameCriteria = 400.0 / 65536.0;

bool CPCDraftDoc::ImportWinDraft(const char *szPath)
{
   short count;
   CFileException fileexception;

   CFile* pfile = GetFile(szPath, CFile::modeRead, &fileexception);

   ColIODoc colIODoc;
   pfile->Read(&colIODoc,sizeof(ColIODoc));

   if(colIODoc.fDocType!=kColDocType || colIODoc.fCreator != kColMDCreator) return false;

// copy doc data

   LongPoint size=colIODoc.fDefPageSize.GetSize();
   m_PageSize.cx=size.x.GetLong();
   m_PageSize.cy=size.y.GetLong();

   size=colIODoc.fDocSize.GetSize()/size;
   m_NumPage.x=size.x.GetLong();
   m_NumPage.y=size.x.GetLong();
   UpdateAllViews(NULL,kDocumentSizeChanged);

//	m_nUnit=colIODoc.fDocUnits.fDefUnits;
//	m_bEnglishUnits=(m_nUnit&kUEnglishMask?true:false);
//	m_nScale=colIODoc.fDocunits.fDefScaleIndex;
//	m_nAngleUnit=colIODoc.fDocunits.fRotDisp;
//	m_DimStd
//	m_DimFmts
//	SetupUnits(

   m_Fill.m_nFillType=colIODoc.fActiveEnvirons.fFillModel.fFillType;
   m_Fill.m_nIndex=colIODoc.fActiveEnvirons.fFillModel.fFillIndex-1;

//	m_Pen
   m_Pen.m_nPenType=colIODoc.fActiveEnvirons.fPenModel.fPenType/3;
   m_Pen.m_nIndex=colIODoc.fActiveEnvirons.fPenModel.fPenIndex-1;
   m_Pen.m_nStyle=colIODoc.fActiveEnvirons.fPenModel.fDashIndex;
   m_Pen.m_nEndCap1=colIODoc.fActiveEnvirons.fPenModel.fEndCap1;
   m_Pen.m_nEndCap2=colIODoc.fActiveEnvirons.fPenModel.fEndCap2;
   m_Pen.m_nAlignment=colIODoc.fActiveEnvirons.fPenModel.fAlignment;
   if(colIODoc.fActiveEnvirons.fPenModel.fHairline) m_Pen.m_nWidth=0x81;
   else m_Pen.m_nWidth=(uchar)colIODoc.fActiveEnvirons.fPenModel.fPenW.GetLong();


   m_bShowRulers=colIODoc.fActiveEnvirons.fShowRulers?true:false;
   m_bStandardRulers=colIODoc.fActiveEnvirons.fStandardRulers?true:false;
   m_bHidePageBreaks=colIODoc.fActiveEnvirons.fHidePageBreaks?true:false;
   m_bHideGridLines=colIODoc.fActiveEnvirons.fHideGrid?true:false;
   m_bSnapToObject=colIODoc.fActiveEnvirons.fSnapToObject?true:false;
   m_nPrevZoomFactor=colIODoc.fActiveEnvirons.fPrevZoom;
   m_nZoomFactor=colIODoc.fActiveEnvirons.fZoom;


//	m_sFieldNames=colIODoc.fActiveEnvirons.;
//	m_TextParams=colIODoc.fActiveEnvirons.;
//	m_ParallelAttrs=colIODoc.fActiveEnvirons.;
//	m_nDisplayRes=colIODoc.fActiveEnvirons.;
//	m_bHideFills=colIODoc.fActiveEnvirons.;

// Read Printer Information
   pfile->Seek(colIODoc.fPrintInfoOff,CFile::begin);
//	SetFPos (pDataRefNum, fsFromStart, vIODoc.fPrintInfoOff);
//	if (!readPrintInfo (pDataRefNum))
//		return 0L;

// Read Bitmap List
   ColBitmapIO* bmpIO=NULL;
   short bmpCount=0;
   pfile->Seek(colIODoc.fBitmapListOff,CFile::begin);
   pfile->Read(&bmpCount,sizeof(short));
   if(bmpCount>0)
   {
      bmpIO=new ColBitmapIO[bmpCount];
      pfile->Read(bmpIO,bmpCount*sizeof(ColBitmapIO));
   }

// Read Font Information
//	SetFPos (pDataRefNum, fsFromStart, vIODoc.fFontInfoOff);
//	if (!readFontInfo (pDataRefNum))
//		return 0L;
   pfile->Seek(colIODoc.fFontInfoOff,CFile::begin);
   pfile->Read(&count,sizeof(short));
   pfile->Seek(-(int)sizeof(short),CFile::current);
   count=sizeof(IOFontInfo)+sizeof(IOLogFont)*(count-1);
   IOFontInfo* pfi=(IOFontInfo*)new char[count];
   pfile->Read(pfi,count);

// Read Layer Information
   short nActiveLayer=0;
   pfile->Seek(colIODoc.fLayerInfo.fLayerOff,CFile::begin);
   for(int l=0; l<colIODoc.fLayerInfo.fNumLayers; l++)
   {
      ColLayerIO colLayerIO;
      pfile->Read(&colLayerIO,sizeof(ColLayerIO));
      CLayer* pLayer;
      if(l) pLayer=AddLayer();
      else pLayer=GetLayer(0);

      pLayer->m_strName=colLayerIO.fLayerName;
      pLayer->m_nScale=colLayerIO.fScaleIndex+1;
      pLayer->m_nSnap=colLayerIO.fSnapIndex;
      if(colLayerIO.fLayerFlags & kColActive) nActiveLayer=l;
      pLayer->m_bGrayed=(colLayerIO.fLayerFlags&kColNotGrayed)?false:true;
      pLayer->m_bHidden=(colLayerIO.fLayerFlags&kColVisible)?false:true;

      if(colLayerIO.fNumShapes)
      {
         CMap<long,long,CMDShape*,CMDShape*> shapes;
         CMap<long,long,CMDGroupShape*,CMDGroupShape*> groups;
         CMap<long,long,CMDShapeList*,CMDShapeList*> groupMembers;

         pfile->Seek(colLayerIO.fShapesOff,CFile::begin);

         // read shapes
         ColIOShapeHeader colIOShHdr;
         pfile->Read(&colIOShHdr,sizeof(ColIOShapeHeader));
         char* pShapes=new char[colIOShHdr.fBlkSize];
         pfile->Read(pShapes,colIOShHdr.fBlkSize);
         int offset=colLayerIO.fFirstShape;
         while(offset!=-1)
         {
            ColFillShape* pShape=(ColFillShape*)(pShapes+offset);
            CMDShape* pNewShape=NULL;
            switch(pShape->fID)
            {
            case ColIDLine:
            {
               ColLine* p=(ColLine*)pShape;
               pNewShape=new CMDLineShape(p->fPts[0],p->fPts[1]);
            }
            break;
            case ColIDDimPt2Pt:
            {
               ColLine* p=(ColLine*)pShape;
               pNewShape=new CMDDimLine(p->fPts[0],p->fPts[1]);
            }
            break;
            case ColIDRectangle:
            {
               ColDSRect* p=(ColDSRect*)pShape;
               pNewShape=new CMDRectangle();
               pNewShape->SetShapePoint(0,p->fPts[0]);
               pNewShape->SetShapePoint(1,p->fPts[1]);
               pNewShape->SetShapePoint(2,p->fPts[2]);
               pNewShape->SetShapePoint(3,p->fPts[3]);
            }
            break;
            case ColIDFreehand:
            case ColIDPolygon:
            {
               ColDSInvarShape* p=(ColDSInvarShape*)pShape;
               pNewShape=new CMDPolygonShape(pShape->fID==ColIDFreehand?CMDPolygonShape::kFreehand:CMDPolygonShape::kPolygon,p->fCount,p->fClosed);
               for(int i=0; i<p->fCount; i++)
               {
                  pNewShape->SetShapePoint(i,p->fPts[i]);
               }
            }
            break;
            case ColIDPolySpline:
            case ColIDFreeSpline:
            {
               ColDSInvarShape* p=(ColDSInvarShape*)pShape;
               int n=p->fCount;
               p->fCount-=1;
               p->fCount/=3;
               if(!p->fClosed)
               {
                  p->fCount++;
                  if(p->fID==ColIDPolySpline) p->fCount++;
               }
               pNewShape=new CMDPolygonShape(pShape->fID==ColIDFreeSpline?CMDPolygonShape::kFreeSpline:CMDPolygonShape::kPolySpline,p->fCount,p->fClosed?true:false);
               for(int i=0; i<p->fCount; i++)
               {
                  pNewShape->SetShapePoint(i,p->fPts[n+i]);
               }
            }
            break;
            case ColIDBezier:
            {
               ColDSInvarShape* p=(ColDSInvarShape*)pShape;
               pNewShape=new CMDBezierShape(p->fCount);
               if(p->fClosed) p->fCount-=1;
               for(int i=0; i<p->fCount; i++)
               {
                  pNewShape->SetShapePoint(i,p->fPts[i]);
               }
            }
            break;
            case ColIDCubic:
            {
               ColDSInvarShape* p=(ColDSInvarShape*)pShape;
               pNewShape=new CMDCubicShape(p->fCount,p->fClosed);
               if(p->fClosed) p->fCount-=1;
               for(int i=0; i<p->fCount; i++)
               {
                  pNewShape->SetShapePoint(i,p->fPts[i]);
               }
            }
            break;
            case ColIDRndRectangle:
            {
               ColDSRndRect* p=(ColDSRndRect*)pShape;
               CMDRndRectangle::CornerType type;
               switch(p->fCornerType)
               {
               case kColProportionalCorner:
                  type=CMDRndRectangle::kCornerProportional;
                  break;
               case kColConstantCorner:
                  type=CMDRndRectangle::kCornerCircle;
                  break;
               case kColEllipticalCorner:
                  type=CMDRndRectangle::kCornerElliptical;
                  break;
               }
               pNewShape=new CMDRndRectangle(type);
               pNewShape->SetShapePoint(0,p->fPts[0]);
               pNewShape->SetShapePoint(1,p->fPts[1]);
               pNewShape->SetShapePoint(2,p->fPts[2]);
               pNewShape->SetShapePoint(3,p->fPts[3]);
               ((CMDRndRectangle*)pNewShape)->m_lptCornerDelta=p->fDiagonals>>1;
            }
            break;
            case ColIDArc:
            case ColID3Arc:
            case ColIDEArc:
            {
               ColDSArc* p=(ColDSArc*)pShape;
               int mode;
               switch(p->fMethod)
               {
               case kColCenterRadius:
                  mode=CMDArcShape::kRadius;
                  break;
               case kColEllipse:
                  mode=CMDArcShape::kElliptic;
                  break;
               case kCol3Point:
                  mode=CMDArcShape::kThreePoint;
                  break;
               }
               pNewShape=new CMDArcShape(mode);
               switch(p->fMethod)
               {
               case kColCenterRadius:
                  pNewShape->SetShapePoint(0,p->fPts[0]);
                  pNewShape->SetShapePoint(1,p->fPts[1]);
                  pNewShape->SetShapePoint(2,p->fPts[2]);
                  break;
               case kColEllipse:
                  pNewShape->SetShapePoint(0,p->fPts[0]);
                  pNewShape->SetShapePoint(1,p->fPts[1]);
                  pNewShape->SetShapePoint(2,p->fPts[2]);
                  pNewShape->SetShapePoint(3,p->fPts[0]+LongPoint(p->fRadius,p->fVRadius));
                  break;
               case kCol3Point:
                  pNewShape->SetShapePoint(0,p->fPts[1]);
                  pNewShape->SetShapePoint(1,p->fPts[2]);
                  pNewShape->SetShapePoint(2,p->fPts[3]);
                  break;
               }
            }
            break;
            case ColIDCircle:
            {
               ColDSCircle* p=(ColDSCircle*)pShape;
               CMDCircle::Mode mode;
               switch(p->fMethod)
               {
               case kColCenterRadius:
                  mode=CMDCircle::kRadius;
                  break;
               case kColDiameter:
                  mode=CMDCircle::kDiameter;
                  break;
               case kCol3Point:
                  mode=CMDCircle::kThreePoint;
                  break;
               }
               pNewShape=new CMDCircle(mode);
               switch(p->fMethod)
               {
               case kColCenterRadius:
                  pNewShape->SetShapePoint(0,p->fPts[0]);
                  pNewShape->SetShapePoint(1,p->fPts[1]);
                  break;
               case kColDiameter:
                  pNewShape->SetShapePoint(0,p->fPts[1]);
                  pNewShape->SetShapePoint(1,p->fPts[2]);
                  break;
               case kCol3Point:
                  pNewShape->SetShapePoint(0,p->fPts[1]);
                  pNewShape->SetShapePoint(1,p->fPts[2]);
                  pNewShape->SetShapePoint(2,p->fPts[3]);
                  break;
               }
            }
            break;
            case ColIDEllipse:
            {
               ColDSEllipse* p=(ColDSEllipse*)pShape;
               pNewShape=new CMDEllipse();
               pNewShape->SetShapePoint(0,p->fPts[0]+LongPoint(-p->fHRadius,-p->fVRadius));
               pNewShape->SetShapePoint(1,p->fPts[0]+LongPoint( p->fHRadius,-p->fVRadius));
               pNewShape->SetShapePoint(2,p->fPts[0]+LongPoint( p->fHRadius, p->fVRadius));
               pNewShape->SetShapePoint(3,p->fPts[0]+LongPoint(-p->fHRadius, p->fVRadius));
            }
            break;
            case ColIDGroup:
            {
               ColDSGroup* p=(ColDSGroup*)pShape;
               CMDGroupShape* pGroup=new CMDGroupShape();
               groups.SetAt(offset,pGroup);
               pNewShape=pGroup;
            }
            break;
            case ColIDDimString:
            {
               ColDSDimStr* p=(ColDSDimStr*)pShape;

               CMDShape* pOwner=NULL;
               shapes.Lookup(p->fDepShapeOff,pOwner);
               if(pOwner)
               {
                  if(p->fPtToPt)
                  {
                     pNewShape=new CMDDimStr(p->fStatus?true:false,pOwner);
                     ((CMDDimLine*)pOwner)->m_nTextDisplay=(p->fPenModel.fDimTextAlgn==kColHorToLine)?kHorizontal:((p->fPenModel.fDimTextAlgn==kColNextToLine)?kAbove:kAlongLine);
                  }
                  else
                  {
                     pNewShape=new CMDArea(p->fStatus&kPhDimStrAttached?true:false,pOwner);
                  }

                  TextParams tp;
                  IOLogFont& iolf=pfi->fIddFont[p->fFont];
                  if(p->fFontSize)
                  {
                     HDC hDC=GetDC(NULL);
                     tp.m_lf.lfHeight=-MulDiv(p->fFontSize,GetDeviceCaps(hDC,LOGPIXELSY),72);
                     ReleaseDC(NULL,hDC);
                  }
//							tp.m_lf.lfWidth=iolf.lfWidth;
//							tp.m_lf.lfEscapement=iolf.lfEscapement;
//							tp.m_lf.lfOrientation=iolf.lfOrientation;
//							tp.m_lf.lfWeight=(fSRElem[n].fStyle&0x2)?FW_BOLD:FW_NORMAL;
//							tp.m_lf.lfItalic=(fSRElem[n].fStyle&0x4)?TRUE:FALSE;
//							tp.m_lf.lfUnderline=(fSRElem[n].fStyle&0x8)?TRUE:FALSE;
//							tp.m_lf.lfStrikeOut=(fSRElem[n].fStyle&0x10)?TRUE:FALSE;
//							tp.m_lf.lfCharSet=iolf.lfCharSet;
//							tp.m_lf.lfOutPrecision=iolf.lfOutPrecision;
//							tp.m_lf.lfClipPrecision=iolf.lfClipPrecision;
//							tp.m_lf.lfQuality=iolf.lfQuality;
//							tp.m_lf.lfPitchAndFamily=iolf.lfPitchAndFamily;
                  lstrcpyn(tp.m_lf.lfFaceName,iolf.lfFaceName,LF_FACESIZE);

                  pNewShape->ModifyTextParams(tp,tpUseFont,NULL);
                  pNewShape->SetShapePoint(0,p->fAnchorPt/*+p->fStrOffset*/);
                  pNewShape->SetRotation(p->fRot);
                  {
                     CDC cdc;
                     cdc.CreateCompatibleDC(NULL);
                     CDocMapper mapper(this);
                     CDraftContext dc(&cdc,&mapper);
                     ((CMDDimStr*)pNewShape)->SetValues(&dc,p->fString);
                  }
                  ((CMDDimStr*)pNewShape)->GetOwner()->Attach((CMDDimStr*)pNewShape);
               }
            }
            break;
            case ColIDArea:
            {
               ColDSDimStr* p=(ColDSDimStr*)pShape;
            }
            break;
            case ColIDBitmap:
            {
               ColDSBitmap* p=(ColDSBitmap*)pShape;
               if(p->fMapHdlIndex<bmpCount)
               {
                  DWORD pos=pfile->GetPosition();
                  HANDLE hBuf=GlobalAlloc(GPTR,bmpIO[p->fMapHdlIndex].fSize);
                  char* pBuf=(char*)GlobalLock(hBuf);
                  pfile->Seek(bmpIO[p->fMapHdlIndex].fBmpInfoOff,CFile::begin);
                  pfile->Read(pBuf,bmpIO[p->fMapHdlIndex].fSize);
                  GlobalUnlock(hBuf);
                  HBITMAP hDib=NULL;
                  if(bmpIO[p->fMapHdlIndex].fNumColors)
                  {
                     CPackedDIB dib(hBuf);
                     hDib=dib.CreateDIBSection();
                  }
                  else
                  {
                     GlobalFree(hBuf);
                  }
                  if(hDib)
                  {
                     CRect rc(p->fSrcRect.left,p->fSrcRect.top,p->fSrcRect.right,p->fSrcRect.bottom);
                     pNewShape=new CMDBitmap(rc,p->fExtent,hDib);
                  }
                  pfile->Seek(pos,CFile::begin);
               }
            }
            break;
            case ColIDText:
            {
               ColDSText* p=(ColDSText*)pShape;
               ColSRElem* fSRElem=(ColSRElem*)(p+1);
               ColLElem* fLElem=(ColLElem*)(fSRElem+p->fStyleRunsSize);
               char* fText=(char*)(fLElem+p->fNumLinesSize);

               TextParams tp;
               switch(p->fTextJust)
               {
               default:
               case kColLeftJus:
                  tp.m_nJust=kAlignLeft;
                  break;
               case kColCenterJus:
                  tp.m_nJust=kAlignCenter;
                  break;
               case kColRightJus:
                  tp.m_nJust=kAlignRight;
                  break;
               }
               switch(p->fTextSpace)
               {
               default:
               case kColSingleSpace:
                  tp.m_nSpacing=kSingleSpace;
                  break;
               case kCol1_1_2Space:
                  tp.m_nSpacing=k1_5Space;
                  break;
               case kColDblSpace:
                  tp.m_nSpacing=kDoubleSpace;
                  break;
               }
//						tp.m_nCase=0;

               pNewShape=new CMDText(p->fParagraph?CMDText::kTextUserSize:CMDText::kTextAutoSize);
               pNewShape->SetShapePoint(0,p->fPts[0]);
               pNewShape->SetShapePoint(1,p->fPts[1]);
               pNewShape->SetShapePoint(2,p->fPts[2]);
               pNewShape->SetShapePoint(3,p->fPts[3]);
               pNewShape->SetTextParams(tp);

               CStyledText st;
               CStyledTextStyle s;
               int n=0;
               for(int i=0; i<p->fTextLen; i++)
               {
                  if(n<p->fNumStyles && i==fSRElem[n].fStartChar)
                  {
                     s.m_LogFont=tp.m_lf;
                     IOLogFont& iolf=pfi->fIddFont[fSRElem[n].fStyleIdx];
                     if(fSRElem[n].fSize)
                     {
                        HDC hDC=GetDC(NULL);
                        s.m_LogFont.lfHeight=-MulDiv(fSRElem[n].fSize,GetDeviceCaps(hDC,LOGPIXELSY),72);
                        ReleaseDC(NULL,hDC);
                     }
//								s.m_LogFont.lfWidth=iolf.lfWidth;
//								s.m_LogFont.lfEscapement=iolf.lfEscapement;
//								s.m_LogFont.lfOrientation=iolf.lfOrientation;
                     s.m_LogFont.lfWeight=(fSRElem[n].fStyle&0x2)?FW_BOLD:FW_NORMAL;
                     s.m_LogFont.lfItalic=(fSRElem[n].fStyle&0x4)?TRUE:FALSE;
                     s.m_LogFont.lfUnderline=(fSRElem[n].fStyle&0x8)?TRUE:FALSE;
                     s.m_LogFont.lfStrikeOut=(fSRElem[n].fStyle&0x10)?TRUE:FALSE;
//								s.m_LogFont.lfCharSet=iolf.lfCharSet;
//								s.m_LogFont.lfOutPrecision=iolf.lfOutPrecision;
//								s.m_LogFont.lfClipPrecision=iolf.lfClipPrecision;
//								s.m_LogFont.lfQuality=iolf.lfQuality;
//								s.m_LogFont.lfPitchAndFamily=iolf.lfPitchAndFamily;
                     lstrcpyn(s.m_LogFont.lfFaceName,iolf.lfFaceName,LF_FACESIZE);
                     s.m_TextColor=PALETTEINDEX(fSRElem[n].fPenIndex-1);
                     n++;
                  }
                  if(*fText==0xD) *fText=0xA;
                  st.Add(*fText++,s);
               }
               ((CMDText*)pNewShape)->SetText(st);
            }
            break;
            }
            if(pNewShape)
            {
               if(pNewShape->IsKindOf(RUNTIME_CLASS(CMDFillShape)))
               {
                  FillModel fm;
                  fm.m_nFillType=pShape->fFillModel.fFillType;
                  switch(fm.m_nFillType)
                  {
                  case 3:
                     fm.m_nFillType=kFillColor;
                  case kFillColor:
                     fm.m_nIndex=PALETTEINDEX(pShape->fFillModel.fFillIndex-1);
                     break;
                  case kFillPat:
                     fm.m_nIndex=pShape->fFillModel.fFillIndex-1;
                     break;
                  }
                  pNewShape->SetFillModel(fm);
               }
               {
                  PenModel pm;
                  pm.m_nPenType=pShape->fPenModel.fPenType;
                  if(pm.m_nPenType>kPenColor) pm.m_nPenType--;
                  switch(pm.m_nPenType)
                  {
                  case kPenColor:
                     pm.m_crColor=PALETTEINDEX(pShape->fPenModel.fPenIndex-1);
                     break;
                  case kPenPat:
                     pm.m_nIndex=pShape->fPenModel.fPenIndex-1;
                     break;
                  }
                  pm.m_nStyle=pShape->fPenModel.fDashIndex;
                  pm.m_nEndCaps=(pShape->fPenModel.fEndCap2<<4) | pShape->fPenModel.fEndCap1;
                  pm.m_nAlignment=pShape->fPenModel.fAlignment;
                  if(pShape->fPenModel.fHairline) pm.m_nWidth=0x81;
                  else pm.m_nWidth=(uchar)pShape->fPenModel.fPenW.GetLong();
                  pNewShape->SetPenModel(pm);
               }
               pNewShape->SetLocked(pShape->fLocked);
               pNewShape->SetRotation(pShape->fRot);
               //pNewShape->SetShapeDatum(pShape->fDatumType);
               pNewShape->SetShapeDatum(pShape->fDatum);
               pNewShape->SetExtents(pShape->fExtent);

//					if(!pNewShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape))) pNewShape->CalculateExtents();
               if(pShape->fOwnerGroup!=-1)
               {
                  CMDShapeList* pList;
                  if(!groupMembers.Lookup(pShape->fOwnerGroup,pList))
                  {
                     pList=new CMDShapeList;
                     groupMembers.SetAt(pShape->fOwnerGroup,pList);
                  }
                  pList->AddTail(pNewShape);
               }
               pLayer->m_ShapeList.AddHead(pNewShape);
               shapes.SetAt(offset,pNewShape);
            }
            offset=pShape->fNext;
         }

         shapes.RemoveAll();

         POSITION pos=groups.GetStartPosition();
         while(pos)
         {
            long key;
            CMDShapeList* pList;
            CMDGroupShape* pGroup;
            groups.GetNextAssoc(pos,key,pGroup);
            if(groupMembers.Lookup(key,pList))
            {
               while(pList->GetCount())
               {
                  CMDShape* pShape=pList->RemoveHead();
                  pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pShape));
                  pGroup->AddMember(pShape);
               }
               delete pList;
            }
            else
            {
               pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pGroup));
               delete pGroup;
            }
         }
         groupMembers.RemoveAll();

         /*			pos=groups.GetStartPosition();
         			while(pos)
         			{
         				long key;
         				CMDGroupShape* pGroup;
         				groups.GetNextAssoc(pos,key,pGroup);
         				pGroup->CalculateExtents();
         			}
         */			groups.RemoveAll();

         delete[] pShapes;

         // skip Hole List
         pfile->Read(&count,sizeof(short));
         pfile->Seek(6+count*8,CFile::current);

         //skip Selected List
         pfile->Read(&count,sizeof(short));
         if(count) pfile->Seek(count*4,CFile::current);
      }
   }

   delete pfi;
   if(bmpIO) delete[] bmpIO;

// Read Scale Table
//	SetFPos (pDataRefNum, fsFromStart, vIODoc.fScaleTableOff);
//	if (!(vScaleTabHdl = readScaleTable (pDataRefNum)))
//		return 0L;
   pfile->Seek(colIODoc.fScaleTableOff,CFile::begin);

// Read Dash Table
   pfile->Seek(colIODoc.fDashTableOff,CFile::begin);
   pfile->Read(m_DashStyles,16*4);

// Read View List
   pfile->Seek(colIODoc.fViewsOff,CFile::begin);
   pfile->Read(&count,sizeof(short));
   while(count--)
   {
      ColSavedView colSavedView;
      pfile->Read(&colSavedView,sizeof(ColSavedView));
   }

   /* Read Missing Font Information -- Missing Fonts from PC are not used */
//	SetFPos (pDataRefNum, fsFromStart, vIODoc.fMissFontOff);
//	if (!readMissFont (pDataRefNum))
//		return 0L;

// Read Color Palette
   pfile->Seek(colIODoc.fColorPalOff,CFile::begin);
   ColRGBTriple colRGB;
   PALETTEENTRY palEntry;
   palEntry.peFlags=0;
   for(count=0; count<colIODoc.fNumColors; count++)
   {
      pfile->Read(&colRGB,sizeof(ColRGBTriple));
      palEntry.peRed=colRGB.rgbtRed;
      palEntry.peGreen=colRGB.rgbtGreen;
      palEntry.peBlue=colRGB.rgbtBlue;
      m_Palette.SetPaletteEntries(count,1,&palEntry);
   }
   palEntry.peRed=255;
   palEntry.peGreen=255;
   palEntry.peBlue=255;
   for(; count<83; count++)
   {
      m_Palette.SetPaletteEntries(count,1,&palEntry);
   }


// Read Pattern List
   pfile->Seek(colIODoc.fPatListOff,CFile::begin);
   for(count=0; count<colIODoc.fNumPats; count++)
   {
      short color;
      pfile->Read(&color,sizeof(short));
      HBITMAP hDib=NULL;
      if(color)
      {
         char bits[64];
         pfile->Read(&color,sizeof(short));
         pfile->Read(bits,64);
         ColRGBTriple* colors=new ColRGBTriple[color];
         pfile->Read(colors,color*sizeof(ColRGBTriple));

         BITMAPINFOHEADER bmih;
         bmih.biSize=sizeof(BITMAPINFOHEADER);
         bmih.biWidth=8;
         bmih.biHeight=8;
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

         for(int i=0; i<64; i++)
         {
            ((LPDWORD)lpBits)[i]=RGB(colors[bits[i]].rgbtRed,colors[bits[i]].rgbtGreen,colors[bits[i]].rgbtBlue);
         }
         delete[] colors;
      }
      else
      {
         short bits[8];
         pfile->Read(bits,8*sizeof(short));
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
         for(int i=0; i<8; i++)
         {
            ((LPDWORD)lpBits)[i]=bits[i];
         }
      }
      if(hDib && count<72)
      {
         m_Patterns[count].FromDIBSection(hDib);
         ::DeleteObject(hDib);
      }
   }

   m_nActiveLayer=nActiveLayer;
   UpdateFrameCounts();
   UpdateAllViews(NULL,kRefresh);
   SetModifiedFlag();
   return true;
}


uchar mac2pc[256]=
{
   //x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
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
   0xF0,0xD2,0xDA,0xDB,0xD9,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xB8,0xFD,0xFE,0xFF // Fx
};

uchar pc2mac[256]=
{
   //	  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
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
   swaps(1); skip(2); swaps(2); // pm
   swaplpt(1); swaps(1); swapl(1);
}
char* uptr::p2c(int count)
{
   char *s=pc;
   short len=*(uchar*)pc;
   if(count) count-=len+1;
   uchar* p=(uchar*)pc+1;
   while(len--) *pc++=mac2pc[*p++];
   *pc++=0;
   pc+=count;
   return s;
}

char* uptr::p2c_nt(int count)
{
   char *s=pc;
   short len=*(uchar*)pc;
   if(count) count-=len+1;
   uchar* p=(uchar*)pc+1;
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

char* GetResource(unsigned long resType, short resId, char* resFork,long len, long* pLen=NULL)
{
   union
   {
      char* pc;
      MDResource* mdr;
   } p;
   p.pc=resFork;
   while(len)
   {
      if(p.mdr->resType==resType && p.mdr->resId==resId)
      {
         char* res=new char[p.mdr->length];
         memcpy(res,p.pc+sizeof(MDResource)+p.mdr->resourceNameLength,p.mdr->length);
         if(pLen) *pLen=p.mdr->length;
         return res;
      }
      len-=p.mdr->offset;
      p.pc+=p.mdr->offset;
   }
   return NULL;
}

// format .p55 - Infinisys Ltd
bool GetResource(CMemFile & memFile, unsigned long resType,short resId,char* resFork,long len)
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
         memFile.Write(res,p.mdr->length);
         memFile.SeekToBegin();
         return true;
      }
      len-=p.mdr->offset;
      p.pc+=p.mdr->offset;
   }
   return false;
}

unsigned long reportRowType[3]= { 'mrt#','mrc#','mrf#' };

bool CPCDraftDoc::LoadMacDraft(const char *szPath)
{

   CFileException fileexception;

   CFile* pfile = GetFile(szPath, CFile::modeRead, &fileexception);

   PhIOContext ctx(pfile);
   ctx.pPalette=&m_Palette;
   ctx.pDoc=this;

   PhDocState phDocState;
   pfile->Read(&phDocState,sizeof(PhDocState));
   char * pszType = (char *) &phDocState.fType; // MD6.2.1 - Infinisys Ltd.
   if(!memcmp(pszType,"MD60",4))
   {
      // format .md60 - Infinisys Ltd
      pfile->Close();
      return LoadMacDraft60(szPath);
   }
   else if(!memcmp(pszType,"MD70",4))
   {
      // format .md70 - Infinisys Ltd
      pfile->Close();
      return LoadMacDraft70(szPath);
   }
   uptr((char*)&phDocState).swapPhDocState();
   if(phDocState.fType != kPhDocType && phDocState.fType != kPhPadType) return false;
   if(phDocState.fHeaderID != kPhFileHdr || phDocState.fVersion != kPhRelease4_0_0)
   {
      // format .p55 - Infinisys Ltd
      pfile->Close();
      return LoadPCDraft55(szPath);
   }

   MDFooter mdf;
   pfile->Seek(-(int)sizeof(MDFooter),CFile::end);
   pfile->Read(&mdf,sizeof(MDFooter));
   uptr((char*)&mdf).swapl(7);
   if(mdf.tag1!='RBAL' || mdf.tag2!='RPH ' || mdf.version!=0x10000 || mdf.type!='RSRC') return false;

   pfile->Seek(-mdf.offset,CFile::end);
   char *resFork=new char[mdf.numBytes];
   pfile->Read(resFork,mdf.numBytes);
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
            ctx.fontMap.SetAt(font, str.Left(iFind));
         }
         else
         {
            ctx.fontMap.SetAt(font,str);
         }
         p.pc += l+1;
         len -=(l + 3);
      }
   }

// copy doc data
   m_PageSize.cx=phDocState.fPrintRec.prInfo.rPage.r;
   m_PageSize.cy=phDocState.fPrintRec.prInfo.rPage.b;

   if (!m_PrinterSettings.defer_set_page_size(m_PageSize.cx, m_PageSize.cy))
   {

      m_PrinterSettings.SetPageOrientation((phDocState.fPrintRec.prStl.wDev && !(phDocState.fPrintRec.prStl.wDev & 0x2)) ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT);
      m_PrinterSettings.SetCustomPageSize(m_PageSize.cx, m_PageSize.cy);

   }


   m_NumPage.x=phDocState.fMViewPages.x;
   m_NumPage.y=phDocState.fMViewPages.y;
   if((phDocState.fDocUnits.fDefUnits & kUEnglishMask)==0/*!m_bEnglishUnits*/ && phDocState.fDocUnits.fDefScaleIndex>21) phDocState.fDocUnits.fDefScaleIndex--;
   SetupUnits(phDocState.fDocUnits.fDefUnits,false,phDocState.fDocUnits.fDefAngleUnits>1024?(1024-phDocState.fDocUnits.fDefAngleUnits):(phDocState.fDocUnits.fDefAngleUnits>>8),phDocState.fDocUnits.fDefPlaces,phDocState.fDocUnits.fDefAnglePlaces,phDocState.fDocUnits.fDefScaleIndex, NULL, 0, NULL);
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

   m_snapTable.setAngleSnaps((phDocState.fActiveEnvirons.fAngleSnapsToACirclePart1<<8)|phDocState.fActiveEnvirons.fAngleSnapsToACirclePart2);
//	m_nDisplayRes=phDocState.fActiveEnvirons.;
//	m_bHideFills=phDocState.fActiveEnvirons.;

   CPCDraftView* pView=NULL;
   POSITION pos=GetFirstViewPosition();
   if(pos) pView=(CPCDraftView*)GetNextView(pos); // 1 view assumed
   if(pView)
   {
      pView->m_pFilletCtx->m_bSmallerArc=phDocState.fActiveEnvirons.fFilletAttr.fIsSmallerArc?true:false;
      pView->m_pFilletCtx->m_fRadius=phDocState.fActiveEnvirons.fFilletAttr.fRadius;
      pView->m_pFilletCtx->m_nTrimMethod=(kPhAutoTrim-phDocState.fActiveEnvirons.fFilletAttr.fTrimMethod);

      pView->m_pChamferCtx->m_fLine1SetBack=phDocState.fActiveEnvirons.fChamferAttr.fLine1SetBack;
      pView->m_pChamferCtx->m_fLine2SetBack=phDocState.fActiveEnvirons.fChamferAttr.fLine2SetBack;
      pView->m_pChamferCtx->m_nTrimMethod=(kPhAutoTrim-phDocState.fActiveEnvirons.fChamferAttr.fTrimMethod);
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

      LPLOGPALETTE lp=(LPLOGPALETTE)new BYTE[sizeof(LOGPALETTE)+(nCount-1)*sizeof(PALETTEENTRY)];
      lp->palVersion=0x300;
      lp->palNumEntries=nCount;
      for(int i=0; i<nCount; i++)
      {
         lp->palPalEntry[i].peRed=p.swaps()>>8;
         lp->palPalEntry[i].peGreen=p.swaps()>>8;
         lp->palPalEntry[i].peBlue=p.swaps()>>8;
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
               if(pFill[i].fResID==-1) continue;
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

                  char* pData=pPix+ppm->pp.patData;
                  switch(ppm->pm.pixelType)
                  {
                  case 0:
                  {
                     p.pc=pPix+ppm->pm.pmHandle;
                     ColorTable* ct=(ColorTable*)(p.pc);
                     p.swapl(1); p.swaps(2);
                     p.swaps((ct->ctSize+1)<<2);
                     short n,s,m,c;
                     for(int y=bmih.biHeight-1; y>=0; y--)
                     {
                        for(int x=0; x<bmih.biWidth; x++)
                        {
                           switch(ppm->pm.pixelSize)
                           {
                           case 1:
                              n=x/8;
                              s=7-x%8;
                              m=0x1;
                              break;
                           case 2:
                              n=x/4;
                              s=(3-x%4)*2;
                              m=0x3;
                              break;
                           case 4:
                              n=x/2;
                              s=(1-x%2)*4;
                              m=0xF;
                              break;
                           case 8:
                              n=x;
                              s=0;
                              m=0xFF;
                              break;
                           }
                           c=(pData[n]>>s)&m;
                           ((LPDWORD)lpBits)[y*bmih.biWidth+x]=RGB(ct->ctTable[c].rgb.blue,ct->ctTable[c].rgb.green,ct->ctTable[c].rgb.red);
                        }
                        pData+=ppm->pm.rowBytes&0x3FFF;
                     }
                  }
                  break;
                  case 16:
                  {
                     /*								p.pc=pPix+ppm->pm.pmHandle;
                     								ColorTable* ct=(ColorTable*)(p.pc);
                     								p.swapl(1); p.swaps(2);
                     								p.swaps((ct->ctSize+1)<<2);
                     */								for(int y=bmih.biHeight-1; y>=0; y--)
                     {
                        switch(ppm->pm.pixelSize)
                        {
                        case 32:
                        {
                           memcpy(((LPDWORD)lpBits)+y*bmih.biWidth,pData,ppm->pm.rowBytes&0x3FFF);
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
                        pData+=ppm->pm.rowBytes&0x3FFF;
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
                  m_Patterns[i-1].FromDIBSection(hDib);
                  m_PatternsHash[i-1] = CalculateDIBHash(hDib);
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
                     if(hash == m_PatternsHash[k-1])
                     {
                        HBITMAP hDib2 = m_Patterns[k-1].CreateDIBSection();
                        if(!CompareDIBs(hDib, hDib2))
                        {
                           ::DeleteObject(hDib2);
                           found = k; break;
                        }
                        ::DeleteObject(hDib2);
                     }
                  }
                  if(found)
                  {
                     ctx.embedDIBsIndex.Add((WORD)(found-1));
                     ::DeleteObject(hDib);
                  }
                  else
                  {
                     //ctx.embedDIBsIndex.Add((WORD)m_numDIBPatterns);
                     m_PatternsHash[m_numDIBPatterns] = CalculateDIBHash(hDib);
                     m_Patterns[m_numDIBPatterns].FromDIBSection(hDib);
                     CPackedItem item;
                     item.m_etype = packed_type_dib;
                     item.m_pdib = &m_Patterns[m_numDIBPatterns];
                     m_packeditema.Add(item);
                     ctx.embedDIBsIndex.Add(m_packeditema.GetUpperBound());
                     m_numDIBPatterns++;
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
         pView->m_nShortcut=pSavedView[i].fCmdKey?(pSavedView[i].fCmdKey-'4'):0;
         m_SavedViews.Add(pView);
      }
      delete pViews;
   }

// reports
   int r=1;
   char* pRep;
   while((pRep=GetResource('mdrp',r,resFork,mdf.numBytes,&len))!=NULL)
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
         short blockCount[3]= { 0,0,0 };
         short *prd[3]= { NULL,NULL,NULL };
         uptr pu[3];

         PhRowInfo* pri=(PhRowInfo*)pRowList;
         p.pc=pRowList;
         for(int i=0; i<pr->cfNumRows; i++)
         {
            p.skip(2); p.swaps(1); p.swapl(1);

            CMDReportRow* pReportRow=new CMDReportRow;
            if(pri[i].fRowData && pri[i].fRowType)
            {
               short typeIdx=pri[i].fRowType-1;
               if(prd[typeIdx]==NULL || (*prd[typeIdx])==0)
               {
                  if(prd[typeIdx]) delete prd[typeIdx];
                  prd[typeIdx]=(short*)GetResource(reportRowType[typeIdx],blockCount[typeIdx]+r,resFork,mdf.numBytes,&len);
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
         if(NULL==plp) break;

         uptr p((char*)plp);
         p.swapl(1); p.swaps(2); p.swapl(4);
         long count=(len-sizeof(PhComplexPtPicData))/sizeof(PhLinkResInfo);
         p.swapl(3*count);

         switch(plp->fComplexType)
         {
         case kPhComplexPtArcDimPoint:
         {
            ASSERT(count>1);
            CLinkVertex* pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=resNo;
            pLinkVertex->m_nShape=plp->fLinks[count-2].fObjectIndex;
            pLinkVertex->m_nVertex=kCenterHandle;
            linkVertexList.AddTail(pLinkVertex);

            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=resNo;
            pLinkVertex->m_nShape=plp->fLinks[count-1].fObjectIndex;
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
            pLinkVertex->m_nShape=plp->fLinks[count-1].fObjectIndex;
            pLinkVertex->m_nVertex=plp->fLinks[count-1].fPointIndex;
            linkVertexList.AddTail(pLinkVertex);

            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=-resNo;
            pLinkVertex->m_nShape=plp->fLinks[count-2].fObjectIndex;
            pLinkVertex->m_nVertex=plp->fLinks[count-2].fPointIndex;
            linkVertexList.AddTail(pLinkVertex);

            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=-resNo;
            pLinkVertex->m_nShape=plp->fLinks[count-3].fObjectIndex;
            pLinkVertex->m_nVertex=plp->fLinks[count-3].fPointIndex;
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
   PhLayerStruct* pLayers=(PhLayerStruct*)(pList+1);
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
      pLayer->m_strName = get_cp(get_uni(pLayer->m_strName,CP_ACP));
      if(!m_bEnglishUnits && pLayers[l].fScaleIndex>21) pLayers[l].fScaleIndex--;
      pLayer->m_nScale=pLayers[l].fScaleIndex;
      pLayer->m_nSnap=pLayers[l].fSnapIndex+1;
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
         pfile->Seek(pLayers[l].fLayerOffset,CFile::begin);
         pfile->Read(pShapes,pLayers[l].fLayerLength);

         uptr p(pShapes);
         p.swapl(1);
         for(int n=0; n<pLayers[l].fNumShapes; n++)
         {
            CMDShape* pNewShape=NULL;
            short count=p.swaps();

            PhDSFillShape* pShape=(PhDSFillShape*)p.pc;
            p.swapDSShape();
            count-=40;

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
               p.swaps(2); // fm
               p.skip(2); p.swaps(1);
               count-=8;

               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               pNewShape=new CMDPolygonShape(pShape->fID==PhIDFreehand?CMDPolygonShape::kFreehand:CMDPolygonShape::kPolygon,ps->fCount,ps->fClosed?true:false);
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
               p.swaps(2); // fm
               p.swapDBPoint(4);
               count-=44;

               PhDSRectangle* ps=(PhDSRectangle*)pShape;
               pNewShape=new CMDRectangle(pShape->fID==PhIDRectCenter?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);
               pNewShape->SetShapePoint(0,ps->fPoint[0].fLongPt);
               pNewShape->SetShapePoint(1,ps->fPoint[1].fLongPt);
               pNewShape->SetShapePoint(2,ps->fPoint[2].fLongPt);
               pNewShape->SetShapePoint(3,ps->fPoint[3].fLongPt);
            }
            break;
            case PhIDRndRectangle:
            {
               p.swaps(2); // fm
               p.swapDBPoint(4);
               p.skip(2);
               p.swapDBPoint(1);
               count-=56;

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
               ((CMDRndRectangle*)pNewShape)->m_lptCornerDelta=ps->fDiagonals.fLongPt>>1;
            }
            break;
            case PhIDFreeSpline:
            case PhIDPolySpline:
            {
               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               p.swaps(2); // fm
               p.skip(2); p.swaps(1);
               count-=8;

               int n=3*ps->fCount+1;
               if(!ps->fClosed)
               {
                  ps->fCount++;
                  if(ps->fID==PhIDPolySpline) ps->fCount++;
               }
               pNewShape=new CMDPolygonShape(pShape->fID==PhIDFreeSpline?CMDPolygonShape::kFreeSpline:CMDPolygonShape::kPolySpline,ps->fCount,ps->fClosed?true:false);
               for(int i=0; i<n; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
               }
               for(int i=0; i<ps->fCount; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
                  pNewShape->SetShapePoint(i,ps->fPoint[n+i].fLongPt);
               }

            }
            break;
            case PhIDCircle:
            {
               p.swaps(2); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(1);
               p.swapDBPoint(3);
               count-=50;

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
               p.swaps(2); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(3);
               p.swapDBPoint(3);
               count-=58;

               PhDSArc* ps=(PhDSArc*)pShape;
               int mode;
               ASSERT(ps->fMethod!=kPhEllipse);
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
               ((CMDArcShape*)pNewShape)->m_nArcDirection=(Abs(ps->fStartAng - mtFixAngle(k90-mtAngle(ps->fCenter.fLongPt,ps->fPoint[0].fLongPt)))>kAlmostSameCriteria)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
            }
            break;
            case PhIDEArc:
            {
               p.swaps(2); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(4);
               p.swapDBPoint(3);
               count-=62;

               PhDSEArc* ps=(PhDSEArc*)pShape;

               ASSERT(ps->fMethod==kPhEllipse);
               pNewShape=new CMDArcShape(CMDArcShape::kElliptic);
               pNewShape->SetShapePoint(0,ps->fCenter.fLongPt);
               pNewShape->SetShapePoint(1,ps->fPoint[0].fLongPt);
               pNewShape->SetShapePoint(2,ps->fPoint[1].fLongPt);
               pNewShape->SetShapePoint(3,ps->fCenter.fLongPt+mtRotPoint(LongPoint(ps->fHRadius,ps->fVRadius),pShape->fRot));
               ps->fStartAng=mtFixAngle(90-ps->fStartAng);
               ps->fEndAng=mtFixAngle(90-ps->fEndAng);
               ((CMDArcShape*)pNewShape)->m_nArcDirection=AD_COUNTERCLOCKWISE; //(ps->fStartAng>ps->fEndAng)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
            }
            break;
            case PhIDEllipse:
            {
               p.swaps(2); // fm
               p.skip(2);
               p.swapDBPoint(1);
               p.swapl(2);
               count-=24;

               PhDSEllipse* ps=(PhDSEllipse*)pShape;
               pNewShape=new CMDEllipse(ps->fMethod==kPhCenterRadius?CMDRectangle::kRectCenter:CMDRectangle::kRectDiagonal);
               pNewShape->SetShapePoint(0,ps->fCenter.fLongPt+mtRotPoint(LongPoint(-ps->fHRadius,-ps->fVRadius),pShape->fRot));
               pNewShape->SetShapePoint(1,ps->fCenter.fLongPt+mtRotPoint(LongPoint( ps->fHRadius,-ps->fVRadius),pShape->fRot));
               pNewShape->SetShapePoint(2,ps->fCenter.fLongPt+mtRotPoint(LongPoint( ps->fHRadius, ps->fVRadius),pShape->fRot));
               pNewShape->SetShapePoint(3,ps->fCenter.fLongPt+mtRotPoint(LongPoint(-ps->fHRadius, ps->fVRadius),pShape->fRot));
            }
            break;
            case PhIDBezier:
            {
               p.swaps(2); // fm
               p.skip(2); p.swaps(1);
               count-=8;

               PhDSPolygon* ps=(PhDSPolygon*)pShape;
               ps->fCount=ps->fCount*3+1;
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
               p.swaps(2); // fm
               p.skip(2); p.swaps(1);
               count-=8;

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
                  pNewShape->SetShapePoint(i,ps->fPoint[i*3].fLongPt);
               }
            }
            break;
            case PhIDText:
            {
               PhDSText* ps=(PhDSText*)pShape;
               p.swaps(2); // fm
               p.skip(2); p.swapl(2); p.swaps(1); p.skip(2); p.swaps(1);
               p.swapDBPoint(4);
               count-=60;

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
               switch((ps->fShapeFlags&kPhVertTextAlignMask)>>kPhVertTextAlignCodeShift)
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
               memset(cCharset, 0, sizeof(cCharset));
               if(pShape->fRef)
               {
                  short count=p.swaps();
                  PhPropList* pl=(PhPropList*)p.pc;
                  p.swaps(1); p.swapl(1); p.swaps(1);
                  p.swapl(3); p.swaps(3); p.swapl(1);
                  count -= 30;

                  for (int i=0; i<pl->fNumElems; i++)
                  {
                     p.swaps(1); p.swapl(1);
                     if(pl->fData[i].fType>=600 && pl->fData[i].fType<700)
                     {
                        nCount++;
                        dwProperty[pl->fData[i].fType-600]=pl->fData[i].fData;
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
                  if(n<pStyle->scrpNStyles && i==pStyle->scrpStyleTab[n].scrpStartChar)
                  {
                     s.m_LogFont=tp.m_lf;
                     ctx.ConvertPhFont(pStyle->scrpStyleTab[n].scrpFont,pStyle->scrpStyleTab[n].scrpSize,pStyle->scrpStyleTab[n].scrpFace,s.m_LogFont);
                     if (nCount)
                        s.m_LogFont.lfCharSet=cCharset[n<nCount? n : nCount-1];
                     s.m_TextColor=RGB(pStyle->scrpStyleTab[n].scrpColor.red>>8,pStyle->scrpStyleTab[n].scrpColor.green>>8,pStyle->scrpStyleTab[n].scrpColor.blue>>8);
                     if(++n<pStyle->scrpNStyles)
                     {
                        p.swapl(1); p.swaps(3); p.skip(2); p.swaps(4);
                     }
                  }
                  if(*pText==0xD) *pText=0xA;
                  st.Add(*pText++,s);
               }
               ((CMDText*)pNewShape)->SetText(st);
            }
            break;
            case PhIDBitmap:
            {
               PhDSBitmap* ps=(PhDSBitmap*)pShape;

               p.swaps(2); // fm
               p.swapDBPoint(4);
               p.swapl(2); p.swaps(1);
               count-=54;

               char* pBuf=new char[pBitmapIO[ps->fMapHdlIndex].fBitmapLength-4];
               LONG lSavedPos= pfile->Seek(pBitmapIO[ps->fMapHdlIndex].fBitmapOffset+4,CFile::begin);
               pfile->Read(pBuf,pBitmapIO[ps->fMapHdlIndex].fBitmapLength-4);
               pfile->Seek(lSavedPos,CFile::begin);
               HBITMAP hDib=NULL;

               if(pBitmapIO[ps->fMapHdlIndex].fColor)
               {
                  PixMap* pm=(PixMap*)pBuf;
                  uptr p(pBuf);
                  p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
                  p.swapl(3);	p.swaps(4); p.swapl(3);

                  BITMAPINFOHEADER bmih;
                  bmih.biSize=sizeof(BITMAPINFOHEADER);
                  bmih.biWidth=pm->bounds.r-pm->bounds.l;
                  bmih.biHeight=pm->bounds.b-pm->bounds.t;
                  bmih.biPlanes=1;
                  bmih.biBitCount=pm->pixelSize;
                  bmih.biCompression=BI_RGB;
                  bmih.biSizeImage=0;
                  bmih.biXPelsPerMeter=0;
                  bmih.biYPelsPerMeter=0;
                  bmih.biClrUsed=(bmih.biBitCount<16)?(1<<bmih.biBitCount):0;
                  bmih.biClrImportant=bmih.biClrUsed;

                  LPVOID lpBits;
                  hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0);

                  pm->rowBytes&=0x3FFF;
                  long nWidthBytes=(pm->rowBytes+0x3)&(~0x3);

                  char* pData=pBuf+sizeof(PixMap);
                  p.pc=pData+pm->baseAddr;
                  ColorTable* ct=(ColorTable*)(p.pc);
                  p.swapl(1); p.swaps(2);
                  p.swaps((ct->ctSize+1)<<2);
                  p.pc=pData;
                  for(int y=bmih.biHeight-1; y>=0; y--)
                  {
                     switch(bmih.biBitCount)
                     {
                     case 16:
                        p.swaps(pm->rowBytes>>1);
                        break;
                     case 24:
                        p.swapRGB(pm->rowBytes/3);
                        break;
                     case 32:
                        p.swapl(pm->rowBytes>>2);
                        break;
                     }
                     memcpy(((char*)lpBits)+y*nWidthBytes,pData,nWidthBytes);
                     pData+=pm->rowBytes;
                  }

                  if(bmih.biBitCount<16)
                  {
                     LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,bmih.biClrUsed*sizeof(RGBQUAD)));
                     for(ushort c=0; c<bmih.biClrUsed; c++)
                     {
                        lpColors[c].rgbRed=ct->ctTable[c].rgb.red>>8;
                        lpColors[c].rgbGreen=ct->ctTable[c].rgb.green>>8;
                        lpColors[c].rgbBlue=ct->ctTable[c].rgb.blue>>8;
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
                  // Infinisys Ltd. Transform in 32 bit
                  HBITMAP hDib32 = NULL;
                  {
                     hDib32 = dib32_from_hbitmap(hDib);

                     if(hDib32 == NULL)
                     {
                        hDib32 = hDib;
                     }
                     else
                     {
                        ::DeleteObject(hDib);
                     }
                  }
                  pNewShape=new CMDBitmap(rc,lrc,hDib32);
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
                  bmi.bmih.biWidth=pm->bounds.r-pm->bounds.l+1;
                  bmi.bmih.biHeight=pm->bounds.b-pm->bounds.t+1;
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

                  char* pData=pBuf+sizeof(BitMap);
                  int rowBytes=(pm->rowBytes+3)&(~3);
                  for(int y=bmi.bmih.biHeight-1; y>=0; y--)
                  {
                     memcpy(((char*)lpBits)+rowBytes*y,pData,pm->rowBytes);
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
               p.swaps(2); // fm
               p.swaplpt(1);
               p.skip(2);
               p.swaps(3);
               count-=20;

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
               p.swaps(2); // fm

               p.swaps(3);
               p.skip(12);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(13);
               count-=334;

               DimStandard std;
               ctx.ConvertPhDimStd(ps->standard,std);
               DimFormat fmt;
               ctx.ConvertPhDimFmt(ps->format,fmt);
               fmt.nRoundoff=(ps->fStatus>>8)&0xF;
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
               pNewShape->SetShapePoint(5,ps->extLineStart.fLongPt!=ps->extLineEnd.fLongPt?ps->extLineEnd.fLongPt:pNewShape->GetShapePoint(4));
               pNewShape->SetShapePoint(6,ps->leaderLineStart.fLongPt!=ps->leaderLineEnd.fLongPt?ps->leaderLineEnd.fLongPt:pNewShape->GetShapePoint(5));
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
               p.swaps(2); // fm

               p.swaps(3);
               p.skip(12);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(13);
               count-=334;

               DimStandard std;
               ctx.ConvertPhDimStd(ps->standard,std);
               DimFormat fmt;
               ctx.ConvertPhDimFmt(ps->format,fmt);
               fmt.nRoundoff=(ps->fStatus>>8)&0xF;
               if(fmt.bUseLabel && ps->format.dfDimLabelHdl)
               {
                  short cnt=p.swaps();
                  char* label=p.pc;
                  p.p2c(cnt);
                  fmt.sLabel=label;
               }

               Coord value=Pythag(ps->dimLineEnd.fLongPt-ps->dimLineStart.fLongPt);
               if(pShape->fID==PhIDDRadialDim) value>>=1;
               pNewShape=new CMDRadialDim(std,fmt,6,6,pShape->fID==PhIDDRadialDim,ps->refPt1.fLongPt,value,ps->anchorPt.fLongPt);

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
               p.swaps(2); // fm

               p.swaps(3);
               p.skip(12);
//						p.swapl(1);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(1);
               p.swapl(2);
               count-=222;

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
               p.swaps(2); // fm

               p.swaps(3);
               p.skip(12);
//						p.swapl(1);
               p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
               p.swaps(1); p.swapl(10); // dim std
               p.swaplpt(8);
               p.p2c(kDimStrSize);	p.p2c(kDimTolStrSize); p.p2c(kDimTolStrSize);

               p.swapDBPoint(11);
               p.swapl(3); p.swaps(1); p.swapl(2); p.swaps(1);

               count-=338;

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
               p.swaps(2); // fm

               p.swaps(3);
               p.swapDBPoint(2);
               p.swapl(1);
               p.p2c(kDimStrSize);
               count-=58;

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
                  pNewShape->SetShapePoint(0,ps->fAnchorPt.fLongPt+ps->fStrOffset.fLongPt);
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
               p.swaps(2); // fm
               p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
               p.skip(2); p.swaps(1);
               count-=18;

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
               for(; i<ps->fCount*3; i++)
               {
                  p.swapDBPoint(1);
                  count-=10;
               }
            }
            break;
            }
            if(count>0) p.skip(count);
            else ASSERT(count==0);

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
      pfile->Seek(pLayers[l-1].fLayerOffset+pLayers[l-1].fLayerLength,CFile::begin);
      PhObjectInfoFieldSet fields;
      pfile->Read(&fields,sizeof(PhObjectInfoFieldSet));
      uptr p((char*)&fields);
      p.p2c(32); p.p2c(32); p.p2c(32); p.p2c(32);
      m_sFieldNames[1]=fields.fF2Name;
      m_sFieldNames[2]=fields.fF3Name;
      m_sFieldNames[3]=fields.fF4Name;
      m_sFieldNames[4]=fields.fF5Name;
   }

   if(pFont) delete[] pFont;
   if(pBitl) delete pBitl;
   for(int i=0; i<ctx.embedDIBs.GetSize(); i++) ::DeleteObject((HBITMAP)ctx.embedDIBs[i]);
   delete[] resFork;

   //m_Palette.ResizePalette(83);	// strip extra entries
//	SetActiveLayer(nActiveLayer);
   m_nActiveLayer=nActiveLayer;
   UpdateFrameCounts();
   UpdateMaxZoom();
   UpdateAllViews(NULL,kRefresh);
   CPoint ptScroll(phDocState.fOldScroll.x,phDocState.fOldScroll.y);
   UpdateAllViews(NULL,kZoomFactorChanged,(CObject*)&ptScroll);
   return true;
}

void WriteResource(CFile* fo,long& forkLen,void* pRes,long len,unsigned long resType, short resId,const CString& resName="")
{
   MDResource mdr;
   mdr.resourceNameLength=resName.GetLength();
   mdr.length=len;
   mdr.offset=16+mdr.resourceNameLength+mdr.length;
   mdr.resType=resType;
   mdr.resId=resId;
   mdr.flags=0;

   forkLen+=mdr.offset;
   uptr p((char*)&mdr);
   p.swapl(3); p.swaps(1);
   fo->Write(&mdr,sizeof(MDResource));
   if(mdr.resourceNameLength) fo->Write(LPCSTR(resName),mdr.resourceNameLength);
   fo->Write(pRes,len);
}

void WriteMFResource(CFile* fo,long& forkLen,CMemFile& mf,unsigned long resType, short resId,const CString& resName="")
{
   long length=mf.GetLength();
   if(length)
   {
      void* pl=mf.Detach();
      WriteResource(fo,forkLen,pl,length,resType,resId,resName);
      free(pl);
   }
}

// 1 English Scales
extern Ratio EnglishScales[];

// 2 Metric Scales
extern Ratio MetricScales[];

bool CPCDraftDoc::SaveMacDraft(const char *szPath)
{
   CFileException fe;
   CFile* fo=GetFile(szPath,CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);
   if(fo == NULL)
   {
      ReportSaveLoadException(szPath, &fe, TRUE, AFX_IDP_INVALID_FILENAME);
      return false;
   }
   TRY
   {
      CWaitCursor wait;

      PhIOContext ctx(fo);
      ctx.m_numDIBPatterns = m_numDIBPatterns;
      ctx.pPalette=&m_Palette;
      ctx.pDoc=this;

      MDFooter mdf;
      mdf.numBytes=0;
      mdf.tag1='RBAL';
      mdf.tag2='RPH ';
      mdf.version=0x10000;
      mdf.type='RSRC';
      mdf.size=sizeof(MDFooter);

      PhDocState phDocState;
      memset(&phDocState,0,sizeof(PhDocState));
      phDocState.fType=kPhDocType;
      phDocState.fHeaderID=kPhFileHdr;
      phDocState.fVersion=kPhRelease4_0_0;

      phDocState.fPrintRec.prInfo.iHRes=72;
      phDocState.fPrintRec.prInfo.iVRes=72;
      phDocState.fPrintRec.prInfo.rPage.r=(short)m_PageSize.cx;
      phDocState.fPrintRec.prInfo.rPage.b=(short)m_PageSize.cy;
      phDocState.fPrintRec.prStl.wDev=(m_PrinterSettings.GetPageOrientation()==DMORIENT_LANDSCAPE)?869:871;
      phDocState.fPrintRec.prStl.iPageV=1320;
      phDocState.fPrintRec.prStl.iPageH=1020;
      phDocState.fPrintRec.prStl.feed=2;

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
      phDocState.fDocUnits.fDefAngleUnits=phDocState.fDocUnits.fDefAngleUnits<0?1024-phDocState.fDocUnits.fDefAngleUnits:(phDocState.fDocUnits.fDefAngleUnits<<8);
      phDocState.fDocUnits.fDefAnglePlaces=m_nAnglePlaces;
      phDocState.fDocUnits.fDefResolution.x=m_nDisplayRes;
      phDocState.fDocUnits.fDefResolution.y=m_nDisplayRes;

      int nDefScaleIndex=(m_nUnit & kUEnglishMask)?4:6;// default // Scale/Units - format .p55 - Infinisys Ltd

      int nDefScaleOld = m_nDefScale;

      if(nDefScaleOld < 0)
      {

         Ratio* ratio;
         if(m_nUnit & kUEnglishMask) ratio=EnglishScales;
         else ratio=MetricScales;
         int i = 1;
         for(i = 1; i < ratio->fD; i++)
         {
            if(((double) m_ratioCustom.fN / (double) m_ratioCustom.fD) >= ((double) ratio[i].fN / (double)ratio[i].fD))
            {
               nDefScaleIndex = i;
               break;

            }


         }

         SetupUnits(m_nUnit,m_bHideMetricUnits?true:false,GetAngleUnit(),m_nPlaces,m_nAnglePlaces,nDefScaleIndex,&m_DimStd,IDS_UNITS_NOT_CHANGED,NULL);

      }

      phDocState.fDocUnits.fDefScaleIndex=m_nDefScale < 0 ? nDefScaleIndex: m_nDefScale;

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
      if(m_nPageNumbering==0) phDocState.fActiveEnvirons.fPrintFlags|=kPhPrintByCols;
      if(m_bRegistrationMarks) phDocState.fActiveEnvirons.fPrintFlags|=kPhRegMarks;

//		phDocState.fActiveEnvirons.fDimLineFlag
//		phDocState.fActiveEnvirons.fDimTextAlgn;
#ifdef _FLOAT_ZOOM
      phDocState.fActiveEnvirons.fPrevZoom=(char)PackZoomFactor(m_nPrevZoomFactor,m_fPrevZoomFactor);
#else
      phDocState.fActiveEnvirons.fPrevZoom=m_nPrevZoomFactor;
#endif

      phDocState.fActiveEnvirons.fAngleSnapsToACirclePart1=m_snapTable.getAngleSnaps()>>8;
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
         phDocState.fActiveEnvirons.fFilletAttr.fTrimMethod=(kNoTrim-pView->m_pFilletCtx->m_nTrimMethod);

         phDocState.fActiveEnvirons.fChamferAttr.fLine1SetBack=pView->m_pChamferCtx->m_fLine1SetBack;
         phDocState.fActiveEnvirons.fChamferAttr.fLine2SetBack=pView->m_pChamferCtx->m_fLine2SetBack;
         phDocState.fActiveEnvirons.fChamferAttr.fTrimMethod=(kNoTrim-pView->m_pChamferCtx->m_nTrimMethod);
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
         strName = get_cp(get_uni(strName),CP_ACP); // Infinisys Ltd.
         lstrcpyn(layer.fLayerName,strName,kPhMaxLayerNameLen);
         layer.fLayerFlags=kPhSelectable|kPhScaled|kPhFillsOn;
         if(i==m_nActiveLayer) layer.fLayerFlags|=kPhActive;
         if(!pLayer->m_bGrayed) layer.fLayerFlags|=kPhNotGrayed;
         if(!pLayer->m_bHidden) layer.fLayerFlags|=kPhVisible;
         if(pLayer->m_nScale < 0)
         {
            layer.fScaleIndex= nDefScaleIndex;
         }
         else
         {
            layer.fScaleIndex=pLayer->m_nScale;
         }
         if(!m_bEnglishUnits && layer.fScaleIndex>20) layer.fScaleIndex++;
         layer.fSnapIndex=pLayer->m_nSnap-1;
         layer.fNumShapes=0;
         layer.fLayerOffset=fo->GetPosition();

         fo->Write(&layer.fNumShapes,sizeof(long));

         linkVertexList.Copy(pLayer->m_LinkVertexList);
         POSITION pos=pLayer->m_ShapeList.GetTailPosition();
         while(pos)
         {
            CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
            layer.fNumShapes+=pShape->SaveMacDraft(ctx);
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
                  if(pLinkVertex->m_nKey!=nPrevKey)
                  {
                     int len=lvf.GetLength();
                     if(len)
                     {
                        linkedVertexResLen.Add(len);
                        linkedVertexResPtr.Add(lvf.Detach());
                     }
                     nPrevKey=pLinkVertex->m_nKey;

                     if(pLinkVertex->m_nKey>=0)
                     {
                        if(pLinkVertex->m_nVertex==kCenterHandle)
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
                        if(pLinkVertex->m_nVertex==kCenterHandle)
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
      ctx.ConvertC2P(m_sFieldNames[1],fields.fF2Name,31);
      ctx.ConvertC2P(m_sFieldNames[2],fields.fF3Name,31);
      ctx.ConvertC2P(m_sFieldNames[3],fields.fF4Name,31);
      ctx.ConvertC2P(m_sFieldNames[4],fields.fF5Name,31);
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
            if(ds.dsBmih.biBitCount==24)
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
            pm.rowBytes=0x8000|((short)ds.dsBm.bmWidthBytes);
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
            pm.cmpSize=(ds.dsBmih.biBitCount<16)?pm.pixelSize:(ds.dsBmih.biBitCount==16)?5:8;
            pm.planeBytes=0;
            pm.pmHandle=(ds.dsBmih.biBitCount<16)?sizeof(ColorTable)+(ds.dsBmih.biClrUsed-1)*sizeof(ColorSpec):sizeof(ColorTable);
            pm.pmReserved=0;

            long size=sizeof(PixMap)+pm.baseAddr+pm.pmHandle;
            PhBitmapIOStruct bitmapIO;
            bitmapIO.fBitmapOffset=ctx.f->GetPosition();
            bitmapIO.fBitmapLength=size+sizeof(long);
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
            for(int y=ds.dsBmih.biHeight-1; y>=0; y--)
            {
               memcpy(pBuf,lpBits+y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
               p.pc=pBuf;
               switch(ds.dsBmih.biBitCount)
               {
               case 16:
                  p.swaps(ds.dsBm.bmWidthBytes>>1);
                  break;
               case 24:
                  p.swapRGB(ds.dsBm.bmWidthBytes/3);
                  break;
               case 32:
                  p.swapl(ds.dsBm.bmWidthBytes>>2);
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
               ct.ctSize=count-1;
               uptr p((char*)&ct);
               p.swapl(1); p.swaps(2);
               ctx.f->Write(&ct,sizeof(ColorTable)-sizeof(ColorSpec));
               for(short c=0; c<count; c++)
               {
                  ColorSpec cs;
                  cs.value=c;
                  cs.rgb.red=(lpColors[c].rgbRed<<8)+lpColors[c].rgbRed;
                  cs.rgb.green=(lpColors[c].rgbGreen<<8)+lpColors[c].rgbGreen;
                  cs.rgb.blue=(lpColors[c].rgbBlue<<8)+lpColors[c].rgbBlue;
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
            unsigned short font= (i+1) << 8;
            mf.Write(&font,sizeof(unsigned short));
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
         long length=16+16*nCount;
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
            *p.ps=pe.peRed<<8; p.swaps(1);
            *p.ps=pe.peGreen<<8; p.swaps(1);
            *p.ps=pe.peBlue<<8; p.swaps(1);
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
         for(int i=0; i<m_numDIBPatterns+ctx.embedDIBs.GetSize(); i++)
         {
            // by TSM
            /*HBITMAP hDib;
            if(i<71) hDib=m_Patterns[i].CreateDIBSection();
            else hDib=(HBITMAP)ctx.embedDIBs[i-71];
            */
            HBITMAP hDib;
            if(i<m_numDIBPatterns) hDib=m_Patterns[i].CreateDIBSection();
            else hDib=(HBITMAP)ctx.embedDIBs[i-m_numDIBPatterns];
            //end by TSM
            DIBSECTION ds;
            ::GetObject(hDib,sizeof(DIBSECTION),&ds);
            if(ds.dsBmih.biBitCount==1)
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
               ppm.pm.rowBytes=0x8000|((short)ds.dsBm.bmWidthBytes);
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
               ppm.pm.cmpSize=(ds.dsBmih.biBitCount<16)?ppm.pm.pixelSize:(ds.dsBmih.biBitCount==16)?5:8;
               ppm.pm.planeBytes=0;
               ppm.pm.pmHandle=sizeof(PixPatMap)+ds.dsBmih.biSizeImage;
               ppm.pm.pmReserved=0;

               uptr p((char*)&ppm);
               p.swaps(1); p.swapl(3);
               p.swaps(1); p.swapl(1); p.skip(8);
               p.swapl(1); p.swaps(1); p.swapl(2); p.swaps(2);
               p.swapl(3);	p.swaps(4); p.swapl(3);
               mf.Write(&ppm,sizeof(PixPatMap));

               char* lpBits=(char*)ds.dsBm.bmBits;
               char* pBuf=new char[ds.dsBm.bmWidthBytes];
               for(int y=ds.dsBmih.biHeight-1; y>=0; y--)
               {
                  memcpy(pBuf,lpBits+y*ds.dsBm.bmWidthBytes,ds.dsBm.bmWidthBytes);
                  p.pc=pBuf;
                  switch(ds.dsBmih.biBitCount)
                  {
                  case 16:
                     p.swaps(ds.dsBm.bmWidthBytes>>1);
                     break;
                  case 24:
                     p.swapRGB(ds.dsBm.bmWidthBytes/3);
                     break;
                  case 32:
                     p.swapl(ds.dsBm.bmWidthBytes>>2);
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
                  ct.ctSize=count-1;
                  uptr p((char*)&ct);
                  p.swapl(1); p.swaps(2);
                  mf.Write(&ct,sizeof(ColorTable)-sizeof(ColorSpec));
                  for(short c=0; c<count; c++)
                  {
                     ColorSpec cs;
                     cs.value=c;
                     cs.rgb.red=(lpColors[c].rgbRed<<8)+lpColors[c].rgbRed;
                     cs.rgb.green=(lpColors[c].rgbRed<<8)+lpColors[c].rgbRed;
                     cs.rgb.blue=(lpColors[c].rgbRed<<8)+lpColors[c].rgbRed;
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
            phsv.fCmdKey=pView->m_nShortcut?('4'+pView->m_nShortcut):0;

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
               ctx.ConvertC2P(m_sFieldNames[i+1],phr.cFieldNames[i],32);
            phr.cfNumRows=pReport->m_Rows.GetSize();

            uptr p((char*)&phr);
            p.swaps(2); p.swapl(1); p.skip(64);
            p.swapl(1); p.swaps(1);
            p.swaps(2); p.skip(2); p.swaps(1); p.skip(2); p.swaps(3); // tp
            p.swaps(13); p.skip(32*4);
            p.swaps(1); p.swapl(1); p.swaps(4);
            p.skip(2); p.swapl(1); p.swaps(1); p.swapl(1);
            p.swapl(1); p.swaps(1); p.swapl(1); p.skip(64);
            p.swaps(2); p.skip(32);
            p.swapl(1); p.swaps(2); p.swapl(2); p.swaps(2); p.swapl(1);
            p.swapl(2);

            WriteResource(fo,mdf.numBytes,&phr,sizeof(PhReport),'mdrp',r);

            PhRowInfo* pri=new PhRowInfo[pReport->m_Rows.GetSize()];
            short rowCount[3]= { 0,0,0 };
            CMemFile mf[3];
            for(int i=0; i<3; i++)
            {
               short cnt=20<<8;	// swapped
               mf[i].Write(&cnt,sizeof(short));
            }

            p.pc=(char*)pri;
            for(int i=0; i<pReport->m_Rows.GetSize(); i++)
            {
               CMDReportRow* pReportRow=(CMDReportRow*)pReport->m_Rows[i];
               short typeIdx=pReportRow->m_nRowType-1;
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
                     count+=pReportRow->m_sRowText[j].GetLength()+2;
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
                     mf[typeIdx].Write(pReportRow->m_sRowText[j],len+1);
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
                  if(pfrd.fSpare>=kFunctTotalName) pfrd.fSpare++;
                  for(int j=0; j<6; j++)
                  {
                     pfrd.fJust[j]=pReportRow->m_nJust[j];
                     count+=pReportRow->m_sRowText[j].GetLength()+2;
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
                     mf[typeIdx].Write(pReportRow->m_sRowText[j],len+1);
                  }
               }
               break;
               }

               rowCount[typeIdx]++;
               if(rowCount[typeIdx]%20==0)
               {
                  WriteMFResource(fo,mdf.numBytes,mf[typeIdx],reportRowType[typeIdx],100*((rowCount[typeIdx]/20)-1)+r);

                  short cnt=20<<8;	// swapped
                  mf[typeIdx].Write(&cnt,sizeof(short));
               }
               p.skip(2); p.swaps(1); p.swapl(1);
            }

            for(int i=0; i<3; i++)
            {
               if(rowCount[i]%20>0)
               {
                  long length=mf[i].GetLength();
                  void* pl=mf[i].Detach();
                  *((short*)pl)=(rowCount[i]%20)<<8;	// swapped
                  WriteResource(fo,mdf.numBytes,pl,length,reportRowType[i],100*(rowCount[i]/20)+r);
                  free(pl);
               }
            }

            WriteResource(fo,mdf.numBytes,pri,pReport->m_Rows.GetSize()*sizeof(PhRowInfo),'mdrl',r);
            delete[] pri;
         }
      }

      mdf.offset=mdf.numBytes+sizeof(MDFooter);
      uptr((char*)&mdf).swapl(7);
      fo->Write(&mdf,sizeof(MDFooter));

      ReleaseFile(fo, FALSE);

      if(nDefScaleOld < 0)
      {

         SetupUnits(m_nUnit,m_bHideMetricUnits?true:false,GetAngleUnit(),m_nPlaces,m_nAnglePlaces,-1,&m_DimStd,IDS_UNITS_NOT_CHANGED,&m_ratioCustom);

      }

   }
   CATCH_ALL(e)
   {
      ReleaseFile(fo, TRUE);
      TRY
      {
         ReportSaveLoadException(szPath, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
      }
      END_TRY
//		e->Delete();
      return false;
   }
   END_CATCH_ALL

   return true;
}

short PhIOContext::UseFont(const CString &sFont, int iCS)
{
   for(short i=0; i<fontInUse.GetSize(); i++)
   {
      if(fontInUse[i]==sFont && fontCS[i] == iCS) return i;
   }
   fontCS.Add(iCS);
   return fontInUse.Add(sFont);
}

ushort grayFill[10]= { 0x0000, 0xE665, 0xCCCC, 0xB332, 0x9999, 0x7FFF, 0x6666, 0x4CCC, 0x3333, 0x1999 };

void PhIOContext::ConvertPhPenModel(const PhPenModel& phpm, PenModel& pm)
{
   pm.m_nPenType=phpm.fPenType;
   switch(pm.m_nPenType)
   {
   case kPhPenColor:
      if(phpm.fColorIndex>199)
      {
         PALETTEENTRY pe;
         pPalette->GetPaletteEntries(phpm.fColorIndex-1,1,&pe);
         pm.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
      }
      else pm.m_crColor=PALETTEINDEX(phpm.fColorIndex-1);
      break;
   case kPhPenGray:
   {
      pm.m_nPenType--;
      ushort grey=grayFill[phpm.fColorIndex/10]>>8;
      pm.m_crColor=RGB(grey,grey,grey);
   }
   break;
   case kPhPenPat:
      // by TSM
      pm.m_nIndex=phpm.fColorIndex-1;
      if(pm.m_nIndex>70)
      {
         // by TSM
         //pm.m_hDib=CopyDIB((HBITMAP)embedDIBs[pm.m_nIndex-71]);
         pm.m_nIndex = embedDIBsIndex[pm.m_nIndex-71];
         pm.m_nPenType--;
         // end by TSM

      }
      else pm.m_nPenType--;
      break;
   }
   pm.m_nEndCap1=(phpm.fPenStyle)&0xF;
   pm.m_nEndCap2=(phpm.fPenStyle>>4)&0xF;
   pm.m_nStyle=(phpm.fPenStyle>>8)&0xF;
   pm.m_nAlignment=phpm.fAlignment-1;
   pm.m_nWidth=phpm.fPenW&0x1F;
   if(phpm.fPenStyle&0x8000)
   {
      pm.m_nWidth=0x81;
      pm.m_nWidth|=(phpm.fPenStyle>>7)&0x60;
   }
}

void PhIOContext::ConvertPenModel(const PenModel& pm, PhPenModel& phpm)
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
      phpm.fColorIndex=(pm.m_nIndex&0xFF)+1;
      // Gradients/Extra Patterns - Infinisys Ltd
      if(phpm.fColorIndex > 71) // format .p55 - Infinisys Ltd
      {
         phpm.fPenType = kPenColor;
         phpm.fColorIndex = 2; // black
      }
      break;
   case kPenColor:
      if(pm.m_crColor<PALETTEINDEX(0))
      {
         //int i = GetEntryCount();
         PALETTEENTRY pe;
         // Looking for existing entry with the same color
         UINT idx  = pPalette->GetNearestPaletteIndex(pm.m_crColor);
         if (idx != CLR_INVALID && pPalette->GetPaletteEntries(idx,1,&pe) && RGB (pe.peRed,pe.peGreen,pe.peBlue) == pm.m_crColor)
         {
            phpm.fColorIndex=idx+1;
            break;
         }

         phpm.fColorIndex=pPalette->GetEntryCount()+1;
         pPalette->ResizePalette(phpm.fColorIndex);

         pe.peRed=GetRValue(pm.m_crColor);
         pe.peGreen=GetGValue(pm.m_crColor);
         pe.peBlue=GetBValue(pm.m_crColor);
         pe.peFlags=0;
         pPalette->SetPaletteEntries(phpm.fColorIndex-1,1,&pe);
         VERIFY (pPalette->SetPaletteEntries(phpm.fColorIndex-1,1,&pe));
         break;
      }
      else phpm.fColorIndex=(pm.m_nIndex&0xFF)+1;
      break;
   }

   phpm.fPenStyle=(pm.m_nEndCap1&0xF);
   phpm.fPenStyle|=(pm.m_nEndCap2&0xF)<<4;
   phpm.fPenStyle|=(pm.m_nStyle&0xF)<<8;
   phpm.fAlignment=pm.m_nAlignment+1;
   phpm.fPenW=pm.m_nWidth&0x1F;
   if(pm.m_nWidth&0x80)
   {
      phpm.fPenStyle|=0x8000;
      phpm.fPenStyle|=(pm.m_nWidth&0x60)<<7;
   }
}

void PhIOContext::ConvertPhFillModel(const PhFillModel& phfm, FillModel& fm)
{
   fm.m_nFillType=(uchar)phfm.fFillType;
   switch(fm.m_nFillType)
   {
   case kPhFillGray:
   {
      fm.m_nFillType=kFillColor;
      ushort grey=grayFill[phfm.fFillIndex/10]>>8;
      fm.m_crColor=RGB(grey,grey,grey);
   }
   break;
   case kPhFillColor:
      if(phfm.fFillIndex>199)
      {
         PALETTEENTRY pe;
         pPalette->GetPaletteEntries(phfm.fFillIndex-1,1,&pe);
         fm.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
      }
      else fm.m_crColor=PALETTEINDEX(phfm.fFillIndex-1);
      break;
   case kPhFillPat:
      fm.m_nIndex=phfm.fFillIndex-1;
      if(fm.m_nIndex>70)
      {
         // by TSM
         //fm.m_nFillType=kFillPatEmbed;
         //fm.m_hDib=CopyDIB((HBITMAP)embedDIBs[fm.m_nIndex-71]);
         fm.m_nIndex = embedDIBsIndex[fm.m_nIndex-71];
         // end by TSM
      }
      break;
   }
}

void PhIOContext::ConvertFillModel(const FillModel& fm, PhFillModel& phfm)
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
      if(fm.m_crColor<PALETTEINDEX(0))
      {
         PALETTEENTRY pe;
         UINT idx  = pPalette->GetNearestPaletteIndex(fm.m_crColor);
         if (idx != CLR_INVALID && pPalette->GetPaletteEntries(idx,1,&pe) && RGB (pe.peRed,pe.peGreen,pe.peBlue) == fm.m_crColor)
         {
            phfm.fFillIndex=idx+1;
            break;
         }
         phfm.fFillIndex=pPalette->GetEntryCount()+1;
         pPalette->ResizePalette(phfm.fFillIndex);
         pe.peRed=GetRValue(fm.m_crColor);
         pe.peGreen=GetGValue(fm.m_crColor);
         pe.peBlue=GetBValue(fm.m_crColor);
         pe.peFlags=0;
         pPalette->SetPaletteEntries(phfm.fFillIndex-1,1,&pe);
      }
      else
      {
         phfm.fFillIndex=(fm.m_crColor & 0xFF) + 1;
      }
      break;
   case kFillPat:
      phfm.fFillIndex=(fm.m_nIndex & 0xFF) + 1;
      // Gradients/Extra Patterns - Infinisys Ltd
      if(phfm.fFillIndex > 71)
      {
         phfm.fFillType = kFillColor;
         phfm.fFillIndex = PALETTEINDEX(1);
      }
      break;
   }
}

void PhIOContext::ConvertPhParaAttr(const PhParallelAttr& phpa, ParallelAttrs& pa)
{
   pa.m_nAlign=3-(phpa.fLineAttr&0x3);
   pa.m_nEndCap=((phpa.fLineAttr>>2)&0x3);
   pa.m_nJoint=(phpa.fLineAttr>>6)&0x1;
   ConvertPhFillModel(phpa.fFillModel,pa.m_Fill);
   pa.m_fWidth=phpa.fWidth;
}

void PhIOContext::ConvertParaAttr(const ParallelAttrs& pa, PhParallelAttr& phpa)
{
   phpa.fLineAttr=((3-pa.m_nAlign)&0x3)|(((pa.m_nEndCap)&0x3)<<2)|((pa.m_nJoint&0x1)<<6);
   ConvertFillModel(pa.m_Fill,phpa.fFillModel);
   phpa.fWidth=pa.m_fWidth;
}

void PhIOContext::ConvertPhTextParams(const PhTextParams& phtp, TextParams& tp)
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

void PhIOContext::ConvertTextParams(const TextParams& tp, PhTextParams& phtp)
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

void PhIOContext::ConvertPhDimStd(const PhDimStd& phstd, DimStandard& std)
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

void PhIOContext::ConvertDimStd(const DimStandard& std, PhDimStd& phstd)
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

void PhIOContext::ConvertPhDimFmt(const PhDimFormat& phfmt, DimFormat& fmt)
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

void PhIOContext::ConvertDimFmt(const DimFormat& fmt, PhDimFormat& phfmt)
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

void PhIOContext::ConvertLogFont(const LOGFONT &lf, short& nFont, short& nSize, Style& nStyle)
{
   nFont=UseFont(lf.lfFaceName, lf.lfCharSet)+1;
   if(lf.lfHeight<0)
   {
      HDC hDC=GetDC(NULL);
      nSize=MulDiv(-lf.lfHeight,72,GetDeviceCaps(hDC,LOGPIXELSY));
      ReleaseDC(NULL,hDC);
   }
   else nSize=(short)lf.lfHeight;
   nStyle=0;
   if(lf.lfWeight==FW_BOLD) nStyle|=0x1; // NB: some bold fonts have other values
   if(lf.lfItalic) nStyle|=0x2;
   if(lf.lfUnderline) nStyle|=0x4;
}

void PhIOContext::ConvertPhFont(short nFont, short nSize, Style nStyle, LOGFONT &lf)
{
   HDC hDC=GetDC(NULL);
   lf.lfHeight=-MulDiv(nSize,GetDeviceCaps(hDC,LOGPIXELSY),72);
   ReleaseDC(NULL,hDC);
   lf.lfWeight=(nStyle&0x1)?FW_BOLD:FW_NORMAL;
   lf.lfItalic=(nStyle&0x2)?TRUE:FALSE;
   lf.lfUnderline=(nStyle&0x4)?TRUE:FALSE;
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
   else
   {
      lf.lfCharSet = ANSI_CHARSET;
   }
}

void PhIOContext::ConvertC2P(const CString& str,char* pBuf,int nMaxSize)
{
   uchar len=min(str.GetLength(),nMaxSize-1);
   *pBuf++=len;
   for(int i=0; i<len; i++)
      pBuf[i]=pc2mac[(uchar)str[i]];
}
