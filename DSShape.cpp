#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "DSShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

PenModel::PenModel() : m_nPenType(kPenColor),m_nStyle(0),m_nAlignment(kCenterLine),m_nAccurateWidth(0),m_nSquareEnds(0),m_nEndCaps(0),m_nWidth(1), m_iPenOpacity(255)
{
	m_crColor=PALETTEINDEX(1);
}

PenModel::PenModel(const PenModel& pm)
{
	operator=(pm);
}

PenModel::~PenModel()
{
	if(m_nPenType==kPenPatEmbed) DeleteObject(m_hDib);
}

void PenModel::Modify(const PenModel &pm, int nWhat)
{
	if(nWhat&pmUsePenType)
	{
		if(m_nPenType==kPenPatEmbed) DeleteObject(m_hDib);
		m_nPenType=pm.m_nPenType;
		if(m_nPenType==kPenPatEmbed) m_hDib=CopyDIB(pm.m_hDib);
		else m_nIndex=pm.m_nIndex;
	}
	if(nWhat&pmUseStyle) m_nStyle=pm.m_nStyle;
	if(nWhat&pmUseEndCap1) m_nEndCap1=pm.m_nEndCap1;
	if(nWhat&pmUseEndCap2) m_nEndCap2=pm.m_nEndCap2;
	if(nWhat&pmUseAlign) m_nAlignment=pm.m_nAlignment;
	if(nWhat&pmUseWidth) m_nWidth=pm.m_nWidth;
	if(nWhat&pmUsePenStyle)
	{
		m_nAccurateWidth=pm.m_nAccurateWidth;
		m_nSquareEnds=pm.m_nSquareEnds;
	}
   if(nWhat&pmUsePenStyle) // Opacity/Transparency - Infinisys Ltd
   {
      m_nAccurateWidth=pm.m_nAccurateWidth;
      m_nSquareEnds=pm.m_nSquareEnds;
   }
   if(nWhat&pmUseOpacity)
   {
      m_iPenOpacity=pm.m_iPenOpacity;
   }
}

PenModel& PenModel::operator=(const PenModel& pm)
{
	if(m_nPenType==kPenPatEmbed) DeleteObject(m_hDib);
	m_nPenType=pm.m_nPenType;
	if(m_nPenType==kPenPatEmbed) m_hDib=CopyDIB(pm.m_hDib);
	else m_nIndex=pm.m_nIndex;
	m_nStyleAlign=pm.m_nStyleAlign;
	m_nEndCaps=pm.m_nEndCaps;
	m_nWidth=pm.m_nWidth;

   m_iPenOpacity = pm.m_iPenOpacity; // Opacity/Transparency - Infinisys Ltd
	return *this;
}

CArchive& operator<<(CArchive& ar,const PenModel& pm)
{
	ar << pm.m_nPenType << pm.m_nStyleAlign << pm.m_nEndCaps << pm.m_nWidth;
	if(pm.m_nPenType==kPenPatEmbed) WriteDIB(ar,pm.m_hDib);
	else ar << pm.m_nIndex;
   if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
   {

      ar << pm.m_iPenOpacity; // Opacity/Transparency - Infinisys Ltd

   }
	return ar;
}

CArchive& operator>>(CArchive& ar, PenModel& pm)
{
	ar >> pm.m_nPenType >> pm.m_nStyleAlign >> pm.m_nEndCaps >> pm.m_nWidth;
	if(pm.m_nPenType==kPenPatEmbed) ReadDIB(ar,pm.m_hDib);
	else ar >> pm.m_nIndex;
   if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
   {

      ar >> pm.m_iPenOpacity; // Opacity/Transparency - Infinisys Ltd

   }
	return ar;
}

FillModel::FillModel() : m_nFillType(kNoFill),m_nIndex(0),m_iFillOpacity(255)
{
}

FillModel::FillModel(const FillModel& fm)
{
	operator=(fm);
}

FillModel::~FillModel()
{
	if(m_nFillType==kFillPatEmbed) DeleteObject(m_hDib);
}

void FillModel::ModifyFillModel(const FillModel& fm,int nWhat) // Opacity/Transparency - Infinisys Ltd
{

   if(nWhat & fmAllButOpacity)
   {
      if(m_nFillType == kFillPatEmbed) DeleteObject(m_hDib);
      m_nFillType=fm.m_nFillType;
      if(m_nFillType == kFillPatEmbed) m_hDib=CopyDIB(fm.m_hDib);
      else m_nIndex=fm.m_nIndex;
   }
   if(nWhat & fmUseOpacity)
   {
      m_iFillOpacity = fm.m_iFillOpacity; // Opacity/Transparency - Infinisys Ltd
   }

}


FillModel& FillModel::operator=(const FillModel& fm)
{
	if(m_nFillType==kFillPatEmbed) DeleteObject(m_hDib);
	m_nFillType=fm.m_nFillType;
	if(m_nFillType==kFillPatEmbed) m_hDib=CopyDIB(fm.m_hDib);
	else m_nIndex=fm.m_nIndex;
   m_iFillOpacity = fm.m_iFillOpacity; // Opacity/Transparency - Infinisys Ltd
	return *this;
}

CArchive& operator<<(CArchive& ar,const FillModel& fm)
{
	ar << fm.m_nFillType;
	if(fm.m_nFillType==kFillPatEmbed) WriteDIB(ar,fm.m_hDib);
	else
   ar << fm.m_nIndex; // Gradient - Infinisys Ltd
   
   if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
   {

      ar << fm.m_iFillOpacity; // Opacity/Transparency - Infinisys Ltd

   }
	return ar;
}

CArchive& operator>>(CArchive& ar,FillModel& fm)
{
	ar >> fm.m_nFillType;
	if(fm.m_nFillType==kFillPatEmbed) ReadDIB(ar,fm.m_hDib);
   else
   {
      ar >> fm.m_nIndex; // Gradient - Infinisys Ltd
      if(ar.m_pDocument != NULL && ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion <= 1) // Infinisys Ltd
      {

         if(fm.m_nFillType == kFillPat && fm.m_nIndex > 71)
         {

            fm.m_nFillType = kFillPatEmbed;

         }

      }

   }
   
   if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
   {

      ar >> fm.m_iFillOpacity; // Opacity/Transparency - Infinisys Ltd

   }
	return ar;
}

ParallelAttrs::ParallelAttrs(BYTE nFillType) : m_nAlign(kCenterLine),m_nEndCap(3),m_nJoint(0)
{
	m_fWidth=(72>>3);
	m_Fill.m_nFillType=nFillType;
	m_Fill.m_nIndex=PALETTEINDEX(0); 
}

CArchive& operator>>(CArchive& ar, ParallelAttrs& attrs)
{
	return ar >> attrs.m_nAlign >> attrs.m_nEndCap >> attrs.m_nJoint
			>> attrs.m_Fill >> attrs.m_fWidth;
}

CArchive& operator<<(CArchive& ar, const ParallelAttrs& attrs)
{
	return ar << attrs.m_nAlign << attrs.m_nEndCap << attrs.m_nJoint
			<< attrs.m_Fill << attrs.m_fWidth;
}

CArchive& operator>>(CArchive& ar, TextParams& params)
{
	ar.Read(&params.m_lf,sizeof(LOGFONT));
	return ar >> params.m_nJust >> params.m_nVAlign >> params.m_nSpacing >> params.m_nCase;
}

CArchive& operator<<(CArchive& ar, const TextParams& params)
{
	ar.Write(&params.m_lf,sizeof(LOGFONT));
	return ar << params.m_nJust << params.m_nVAlign << params.m_nSpacing << params.m_nCase;
}

ShapeAttributes::ShapeAttributes() : m_nFill(1),m_nPen(1),m_bInfo(false),m_nPara(1)
{
}

ShapeAttributes& ShapeAttributes::operator=(ShapeAttributes& sa)
{
	m_nFill=sa.m_nFill;
	m_nPen=sa.m_nPen;
	m_bInfo=sa.m_bInfo;
	m_Fill=sa.m_Fill;
	m_Pen=sa.m_Pen;
	for(int i=0;i<5;i++) m_sInfo[i]=sa.m_sInfo[i];
	m_nPara=sa.m_nPara;
	m_Para=sa.m_Para;
	return *this;
}

TextParams::TextParams():m_nJust(kAlignLeft),m_nVAlign(kAlignTop),m_nSpacing(kSingleSpace),m_nCase(-1)
{
	memset(&m_lf,0,sizeof(LOGFONT));
	m_lf.lfHeight=-16;
	m_lf.lfWeight=FW_NORMAL;
	m_lf.lfOutPrecision=OUT_TT_ONLY_PRECIS;
	m_lf.lfQuality=PROOF_QUALITY;
	m_lf.lfPitchAndFamily=VARIABLE_PITCH|FF_SWISS;
#ifdef _JAPANESE
	m_lf.lfCharSet=SHIFTJIS_CHARSET;
	lstrcpy(m_lf.lfFaceName,"MS UI Gothic");
#else
	m_lf.lfCharSet=DEFAULT_CHARSET;
	lstrcpy(m_lf.lfFaceName,"Arial");
#endif
}

void TextParams::Modify(const TextParams &tp, int nWhat)
{
	if(nWhat&tpUseFont)
	{
		memcpy(&m_lf,&tp.m_lf,sizeof(LOGFONT));
	}
	if(nWhat&tpUseJust)
	{
		m_nJust=tp.m_nJust;
	}
	if(nWhat&tpUseVAlign)
	{
		m_nVAlign=tp.m_nVAlign;
	}
	if(nWhat&tpUseSpacing)
	{
		m_nSpacing=tp.m_nSpacing;
	}
	if(nWhat&tpUseCase)
	{
		m_nCase=tp.m_nCase;
	}
}


bool PenModel::isPenStyleRound() const
{
	return (m_nAccurateWidth==1 && m_nSquareEnds == 0);
}

void PenModel::makePenStyleRound()
{
	m_nAccurateWidth=1;
	m_nSquareEnds = 0;
}

void PenModel::makePenStyleBlade()
{
	m_nAccurateWidth=1;
	m_nSquareEnds = 1;
}
