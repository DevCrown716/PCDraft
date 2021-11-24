#include "stdafx.h"

#include "DimStructs.h"

void DimStandard::FillDefaults(short type)
{
	nType=type;						/* ANSI, Metric */
	if(type==kDsEnglish)
	{
		WitnessExt=0.125;
		WitnessLineGap=0.0625;
		DimTextGap=0.0625;
		LeaderLen=0.125;
		OutsideLineLen=0.25;
		CircleCenterLen=0.25;
		CircleCenterGap=0.0625;
		CircleCenterExt=0.125;
	}
	else
	{
		WitnessExt=3;
		WitnessLineGap=2;
		DimTextGap=1;
		LeaderLen=3;
		OutsideLineLen=6;
		CircleCenterLen=6;
		CircleCenterGap=2;
		CircleCenterExt=3;
	}
	nToleranceSpaceScale=100;
	nToleranceTextScale=100;
}

bool DimStandard::Compare(const DimStandard& ds)
{
	if(nType!=ds.nType) return true;
	if(WitnessExt!=ds.WitnessExt) return true;
	if(WitnessLineGap!=ds.WitnessLineGap) return true;
	if(DimTextGap!=ds.DimTextGap) return true;
	if(LeaderLen!=ds.LeaderLen) return true;
	if(OutsideLineLen!=ds.OutsideLineLen) return true;
	if(nToleranceSpaceScale!=ds.nToleranceSpaceScale) return true;
	if(nToleranceTextScale!=ds.nToleranceTextScale) return true;
	if(CircleCenterLen!=ds.CircleCenterLen) return true;
	if(CircleCenterGap!=ds.CircleCenterGap) return true;
	return (CircleCenterExt!=ds.CircleCenterExt)?true:false;
}

DimStandard& DimStandard::operator=(DimStandard &inStd)
{
	nType=inStd.nType;
	WitnessExt=inStd.WitnessExt;
	WitnessLineGap=inStd.WitnessLineGap;
	DimTextGap=inStd.DimTextGap;
	LeaderLen=inStd.LeaderLen;
	OutsideLineLen=inStd.OutsideLineLen;
	nToleranceSpaceScale=inStd.nToleranceSpaceScale;
	nToleranceTextScale=inStd.nToleranceTextScale;
	CircleCenterLen=inStd.CircleCenterLen;
	CircleCenterGap=inStd.CircleCenterGap;
	CircleCenterExt=inStd.CircleCenterExt;
	return *this;
}

void DimStandard::ConvertForUnits(short nDsType)
{
	if(nType!=nDsType)
	{
		double factor=25.4;
		if(nType==kDsMetric) factor=1/factor;
		nType=nDsType;
		WitnessExt*=factor;
		WitnessLineGap*=factor;
		DimTextGap*=factor;
		LeaderLen*=factor;
		OutsideLineLen*=factor;
		CircleCenterLen*=factor;
		CircleCenterGap*=factor;
		CircleCenterExt*=factor;
	}
}

CArchive& operator<<(CArchive& ar, const DimStandard& ds)
{
	return ar << ds.nType << ds.WitnessExt << ds.WitnessLineGap
				<< ds.DimTextGap << ds.LeaderLen << ds.OutsideLineLen
				<< ds.nToleranceTextScale << ds.nToleranceSpaceScale
				<< ds.CircleCenterLen << ds.CircleCenterGap << ds.CircleCenterExt;
}

CArchive& operator>>(CArchive& ar, DimStandard& ds)
{
	return ar >> ds.nType >> ds.WitnessExt >> ds.WitnessLineGap
				>> ds.DimTextGap >> ds.LeaderLen >> ds.OutsideLineLen
				>> ds.nToleranceTextScale >> ds.nToleranceSpaceScale
				>> ds.CircleCenterLen >> ds.CircleCenterGap >> ds.CircleCenterExt;
}

DimFormat::DimFormat(): bTextCentered(false),
			 bTextFramed(false),
			 bArrowsInside(true),
			 bUseLabel(false),
			 bUseWitness(true),
			 nTolerance(kNoTolerance),
			 Tolerance1(0.5),
			 Tolerance2(0.5),
			 sLabel(""),
			 nRoundoff(0),
			 nTextDisplay(kHorizontal),
			 nLeader(kNoLeader),
			 nRDSymbol(kLeading){}

void DimFormat::FillDefaults(short type, short dsType)
{
	nType=type;
	bTextCentered=false;
	bTextFramed=false;
	bArrowsInside=true;
	bUseLabel=false;
	bUseWitness=true;
	nTextDisplay=(dsType==kDsEnglish || nType==kAngular)?kHorizontal:kAbove;
	nLeader=(nType==kRadial && dsType==kDsEnglish)?kLeft:kNoLeader;
	nRDSymbol=(nType==kRadial)?kLeading:kNoSymbol;
	nTolerance=kNoTolerance;
	Tolerance1=0.5;
	Tolerance2=0.5;
	sLabel="";
	nRoundoff=0;
}

void DimFormat::Modify(const DimFormat& df,short nWhat)
{
    // SMK.SSA may be means & instead of && ???
	if(nWhat&&dfUseTextCentered) bTextCentered=df.bTextCentered;
	if(nWhat&&dfUseTextFramed) bTextFramed=df.bTextFramed;
	if(nWhat&&dfUseArrows) bArrowsInside=df.bArrowsInside;
	if(nWhat&&dfUseUseLabel) bUseLabel=df.bUseLabel;
	if(nWhat&&dfUseUseWitness) bUseWitness=df.bUseWitness;
	if(nWhat&&dfUseTextDisplay) nTextDisplay=df.nTextDisplay;
	if(nWhat&&dfUseLeader) nLeader=df.nLeader;
	if(nWhat&&dfUseRDSymbol) nRDSymbol=df.nRDSymbol;
	if(nWhat&&dfUseTolerance) nTolerance=df.nTolerance;
	if(nWhat&&dfUseTolerance1) Tolerance1=df.Tolerance1;
	if(nWhat&&dfUseTolerance2) Tolerance2=df.Tolerance2;
	if(nWhat&&dfUseLabelText) sLabel=df.sLabel;
	if(nWhat&&dfUseRounoff) nRoundoff=df.nRoundoff;
}

short DimFormat::Compare(const DimFormat& df)
{
	short nWhat=0;
	if(bTextCentered!=df.bTextCentered) nWhat|=dfUseTextCentered;
	if(bTextFramed!=df.bTextFramed) nWhat|=dfUseTextFramed;
	if(bArrowsInside!=df.bArrowsInside) nWhat|=dfUseArrows;
	if(bUseLabel!=df.bUseLabel) nWhat|=dfUseUseLabel;
	if(bUseWitness!=df.bUseWitness) nWhat|=dfUseUseWitness;
	if(nTextDisplay!=df.nTextDisplay) nWhat|=dfUseTextDisplay;
	if(nLeader!=df.nLeader) nWhat|=dfUseLeader;
	if(nRDSymbol!=df.nRDSymbol) nWhat|=dfUseRDSymbol;
	if(nTolerance!=df.nTolerance) nWhat|=dfUseTolerance;
	if(Tolerance1!=df.Tolerance1) nWhat|=dfUseTolerance1;
	if(Tolerance2!=df.Tolerance2) nWhat|=dfUseTolerance2;
	if(sLabel!=df.sLabel) nWhat|=dfUseLabelText;
	if(nRoundoff!=df.nRoundoff) nWhat|=dfUseRounoff;
	return nWhat;
}

DimFormat& DimFormat::operator=(DimFormat &inFmt)
{
	nType=inFmt.nType;
	bTextCentered=inFmt.bTextCentered;
	bTextFramed=inFmt.bTextFramed;
	bArrowsInside=inFmt.bArrowsInside;
	bUseLabel=inFmt.bUseLabel;
	bUseWitness=inFmt.bUseWitness;
	nTextDisplay=inFmt.nTextDisplay;
	nLeader=inFmt.nLeader;
	nRDSymbol=inFmt.nRDSymbol;
	nTolerance=inFmt.nTolerance;
	Tolerance1=inFmt.Tolerance1;
	Tolerance2=inFmt.Tolerance2;
	sLabel=inFmt.sLabel;
	nRoundoff=inFmt.nRoundoff;
	return *this;
}

CArchive& operator<<(CArchive& ar, const DimFormat& df)
{
    return ar << df.nType << (BYTE)( df.bTextCentered ? 1 : 0) << (BYTE)( df.bTextFramed ? 1:0)
        << (BYTE)(df.bArrowsInside ? 1:0)<< (BYTE)(df.bUseLabel?1:0) << (BYTE)(df.bUseWitness ? 1:0)
        << df.nTextDisplay << df.nLeader << df.nRDSymbol
        << df.nTolerance << df.Tolerance1 << df.Tolerance2
        << df.sLabel << df.nRoundoff;
}

CArchive& operator>>(CArchive& ar, DimFormat& df)
{
    BYTE BTextCentered, BTextFramed, BArrowsInside, BUseLabel, BUseWitness;
    ar >> df.nType >> BTextCentered >> BTextFramed
        >> BArrowsInside >> BUseLabel >> BUseWitness
        >> df.nTextDisplay >> df.nLeader >> df.nRDSymbol
        >> df.nTolerance >> df.Tolerance1 >> df.Tolerance2
        >> df.sLabel >> df.nRoundoff;
    df.bTextCentered = BTextCentered == TRUE;
    df.bTextFramed = BTextFramed == TRUE;
    df.bArrowsInside = BArrowsInside == TRUE;
    df.bUseLabel = BUseLabel == TRUE;
    df.bUseWitness = BUseWitness == TRUE;
    return ar;
}

