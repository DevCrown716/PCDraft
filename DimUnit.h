// DimUnit.h: interface for the CDimUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIMUNIT_H__312DA903_E523_11D2_9931_000000000000__INCLUDED_)
#define AFX_DIMUNIT_H__312DA903_E523_11D2_9931_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dimstructs.h"
#include "wrfixed.h"

#define	kMaxDimUnits			9		/* for units array */
#define kMaxDimUnitsStrlen		8		/* ditto */

#define	kFractDimDenom			64.0	/* 64ths are the smallest unit in dimensions */

typedef enum {				/* dimension display types */
	 kAreaIn = 0,
	 kAreaFt,
	 kDimM,
	 kDimDM,
	 kDimCM,
	 kDimMM,
	 kDimFt_Dec,
	 kDimIn_Dec,
	 kDimFt_In,
	 kDimFt_In_Dec,
	 kDimIn,
	 kDimNoUnits
} DimDisp_t;

class CPCDraftDoc;

class CDimUnit  
{
public:
	CDimUnit();
	virtual ~CDimUnit();
	void setDimUnits(short units,bool bHideMetricUnits,short places,int res, short angUnits, short angPlaces);
	void switchUnits(short units, short res);
	CString numUser2Str(double value,bool bTolerance);
   //CString numUser2Mm(double value, bool bTolerance);
	double numDB2User(LongRatio& scaleRatio,WrFixed dbValue);
   double numDBToMm(LongRatio& scaleRatio, WrFixed dbValue);
	WrFixed numUser2DB(LongRatio& scaleRatio,double userValue);
	CString numAngle2Str(Angle angle);
	bool numStr2AngNoRng(LPCTSTR str,Angle& degrees);
	bool numStr2Angle(LPCTSTR str,Angle& degrees);
	CString calcAreaStr(double rawArea);
	bool numStr2User(LPCTSTR str,double& value);
	bool numStr2PosInt(LPCTSTR str,long& value);
	CString makeADistStr(Angle angle,bool bDegreeOnly);
	CString calcDimStr(double dimDist,bool bTolerance);

public:
	DimDisp_t m_Display;
	int m_ScaleIndex;
	bool m_bHideMetricUnits;

protected:
	double m_Conversion;
	short m_nFraction;
	short m_nPlaces;
	short m_nAngUnits;
	short m_nAngPlaces;
static CString gUnitStr[kMaxDimUnits];	

static CString sFootSym,sFootSym2,sFootAbbr;
static CString sInchSym,sInchSym2,sInchAbbr;
static CString sMilliAbbr,sCentiAbbr,sDeciAbbr,sMeterAbbr;
static CString sDegreeSym,sDegreeAbbr;
static CString sMinuteSym,sMinuteSym2,sMinuteAbbr;
static CString sSecondSym,sSecondSym2,sSecondAbbr;
static CString sFracBar,sDash,sEnDash,sEmDash;

protected:
	bool IsFootSymbol(LPCTSTR& str);

	CString inches(double dimDist);
	CString feetInches(double dimDist);
	CString decFeetInches(double dimDist);
	CString decimalDim(double dimDist,short units);
	CString formatDecDim(double dim,short units);

	CString metricAreaStr(double rawArea, short which);
	CString feetAreaStr(double rawArea);
	CString inchAreaStr(double rawArea);

	bool fracFtIn(LPCTSTR& str,double& inches);
	bool fracIn(LPCTSTR& str,double& inches);
	bool decFtIn(LPCTSTR& str,double& inches);
	bool decIn(LPCTSTR& str,double& value);
	bool decMetric(LPCTSTR& str,double& value);
	bool isFraction(LPCTSTR& str,double& value);
	bool decimalStr(LPCTSTR& str,double& value);
	bool integer(LPCTSTR& str,long& value);
	bool digits(LPCTSTR& str,double& value);
	short checkStr(LPCTSTR& str,short numStrs,...);
	void space(LPCTSTR& str);
};

#endif // !defined(AFX_DIMUNIT_H__312DA903_E523_11D2_9931_000000000000__INCLUDED_)
