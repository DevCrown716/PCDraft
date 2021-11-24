// DimUnit.cpp: implementation of the CDimUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "DimUnit.h"
#include "phmath.h"

#include <math.h>
#include <limits.h>
#include <stdarg.h>

#define kAsciiZero		'0'
#define kAsciiNine		'9'
#define kDecPoint		'.'

#define kHardSpace		((uchar)'\312')

#define kFeetScale		10						/* scale index for 1/2" = 1' */

CString CDimUnit::sFootSym,CDimUnit::sFootSym2,CDimUnit::sFootAbbr;
CString CDimUnit::sInchSym,CDimUnit::sInchSym2,CDimUnit::sInchAbbr;
CString CDimUnit::sMilliAbbr,CDimUnit::sCentiAbbr,CDimUnit::sDeciAbbr,CDimUnit::sMeterAbbr;
CString CDimUnit::sDegreeSym,CDimUnit::sDegreeAbbr;
CString CDimUnit::sMinuteSym,CDimUnit::sMinuteSym2,CDimUnit::sMinuteAbbr;
CString CDimUnit::sSecondSym,CDimUnit::sSecondSym2,CDimUnit::sSecondAbbr;
CString CDimUnit::sFracBar,CDimUnit::sDash,CDimUnit::sEnDash,CDimUnit::sEmDash;

CString CDimUnit::gUnitStr[kMaxDimUnits];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDimUnit::CDimUnit()
{
	m_bHideMetricUnits=false;

	if(sFootSym.IsEmpty())
	{
		sFootSym.LoadString(IDS_FOOT_SYMBOL); // \'
		sFootSym2.LoadString(IDS_FOOT_SYMBOL2);// -
		sFootAbbr.LoadString(IDS_FOOT_ABBR);// ft

		sInchSym.LoadString(IDS_INCH_SYMBOL); // \"
		sInchSym2.LoadString(IDS_INCH_SYMBOL2);// ・
		sInchAbbr.LoadString(IDS_INCH_ABBR);// in

		sMilliAbbr.LoadString(IDS_MILLIMETER_ABBR);// mm
		sCentiAbbr.LoadString(IDS_CENTIMETER_ABBR);// cm
		sDeciAbbr.LoadString(IDS_DECIMETER_ABBR);// dm
		sMeterAbbr.LoadString(IDS_METER_ABBR);// m

		sDegreeSym.LoadString(IDS_DEGREE_SYMBOL);// ｰ
		sDegreeAbbr.LoadString(IDS_DEGREE_ABBR);// d
		sMinuteSym.LoadString(IDS_MINUTE_SYMBOL);// \'
		sMinuteSym2.LoadString(IDS_MINUTE_SYMBOL2);// ・
		sMinuteAbbr.LoadString(IDS_MINUTE_ABBR);// m
		sSecondSym.LoadString(IDS_SECOND_SYMBOL);// \"
		sSecondSym2.LoadString(IDS_SECOND_SYMBOL2);// ・
		sSecondAbbr.LoadString(IDS_SECOND_ABBR);// s

		sFracBar.LoadString(IDS_FRACBAR);// /
		sDash.LoadString(IDS_DASH);// -
		sEnDash.LoadString(IDS_ENDASH);// ﾐ
		sEmDash.LoadString(IDS_EMDASH);// ﾑ

		for(int i=0;i<kMaxDimUnits;i++)
			gUnitStr[i].LoadString(IDS_UNITFIRST+i);
	}
}

CDimUnit::~CDimUnit()
{

}

void CDimUnit::setDimUnits(short units,bool bHideMetricUnits,short places,int res, short angUnits, short angPlaces)
{
	short fraction=1;
	double conversion,denominator;

	m_bHideMetricUnits=bHideMetricUnits;

	m_nPlaces=places;
	m_nAngUnits=angUnits;
	m_nAngPlaces=angPlaces;
	denominator=kFractDimDenom;
	conversion=res;	/* has to be square resolution */

	switch(units)
	{
		case kUMillimeters:
			m_Display=kDimMM;
			m_Conversion=kMetRes/10.0;
			m_nFraction=1L;
			break;

		case kUCentimeters:
			m_Display=kDimCM;
			m_Conversion=kMetRes;
			m_nFraction=1L;
			break;

		case kUDecimeters:
			m_Display=kDimDM;
			m_Conversion=kMetRes*10.0;
			m_nFraction=1L;
			break;

		case kUMeters:
			m_Display=kDimM;
			m_Conversion=kMetRes*100.0;
			m_nFraction=1L;
			break;

		case kUDecInches:
			m_Display=kDimIn_Dec;
			m_Conversion=conversion;
			m_nFraction=1L;
			break;

		case kUDecFeetInches:
			m_Display=kDimFt_In_Dec;
			m_Conversion=conversion;
			m_nFraction=1L;
			break;

		case kUDecFeet:
			if(res==kEngRes)
			{
				m_Display=kDimFt_Dec;
				m_Conversion=conversion*12;
			}
			else
			{
				m_Display=kDimFt_Dec;
				m_Conversion=conversion*10.0;
			}
			m_nFraction=1L;
			break;

		case kUFracInches:
			m_Display=kDimIn;
			/* in 1/8ths, 1/16th, ...,1/64th  of inch */
			m_Conversion=conversion/denominator;
			m_nFraction=(short)denominator;
			break;

		case kUFracFeetInches:
			m_Display=kDimFt_In;
			m_Conversion=conversion/denominator;
			m_nFraction=(short)denominator;
			break;
	}
} /* setDimUnits */

/*------------------------------------------------------------------------
	calcDimStr - 	use mode passed in dimUnits to create dimension string
------------------------------------------------------------------------*/

CString CDimUnit::calcDimStr(double dimDist,bool bTolerance)
{
	bool negative;
	short length;
	short i;
	short units;
	CString sText;

	if((negative=dimDist<0)!=0)
		dimDist=-dimDist;

	switch(units=m_Display)
	{
		case kDimIn:		/* Fractional Inches  */
			sText=inches(dimDist);
			break;

		case kDimFt_In:		/* feet and fractional inches */
			sText=feetInches(dimDist);
			break;

		case kDimFt_In_Dec:	/* feet and decimal inches */
			sText=decFeetInches(dimDist);
			break;

		case kDimFt_Dec:	/* Decimal Feet */
		case kDimIn_Dec:	/* Decimal Inches  */
		case kDimM:			/* meters */
		case kDimDM:		/* decimeters */
		case kDimCM:		/* centimeters */
		case kDimMM:		/* millimeters */
		case kDimNoUnits:	/* decimal no units */
			if(m_bHideMetricUnits && ((units==kDimM) || (units==kDimDM) ||
									 (units==kDimCM) || (units==kDimMM))|| bTolerance)
				units=kDimNoUnits;
			sText=decimalDim(dimDist, units);
			break;
	}

	if(negative)
	{
		if(sText.GetLength()>0 && isdigit(sText[0]))
		{
			sText="-"+sText;
		}
	}

	if(bTolerance)
	{
		// strip units
		i=sText.GetLength();
		length=0;
		while(--i>=0)
		{
			char c=sText.GetAt(i);
			// test for digit, / (fraction, ., -, or blank
			if(!(c>='0' && c<='9') && (c!='/' && c!='.' && c!='-' && c!=' '))
			{
				length++;
			}
			else
				break;
		}
		if(length)
			sText=sText.Left(sText.GetLength()-length);
	}
	return sText;
}

/*----------------------------------------------------------------------------
  Create an inches string as a mixed fraction   i-n/d"
  ----------------------------------------------------------------------------*/
CString CDimUnit::inches(double dimDist)
{
	short denm;
	long intInches,num;

	denm=m_nFraction;
	dimDist/=(m_Conversion*denm);

	/*  Convert to integral inches and n/d fractional inches. 				*/
	intInches=(long)dimDist;
	num=(long)((dimDist-intInches)*denm+0.5);

	/*  Rounding: gCurrDimUnits.fPlaces = number of accurate decimal places.
		Thus if gCurrDimUnits.fPlaces = 0 we cannot support fractions.		*/
	if(!m_nPlaces)
	{
		if(num>31L)	intInches++;
		num=0L;
	}
	else if(num/denm)
	{
		intInches++;
		num=0L;
	}

	/* Build the string. 													*/
	CString sResult;
	if(num)
	{
		/*  Reduce the fraction assuming that the divisor is a power of 2.	*/
		while(!(num & 0x0001L))
		{
			num>>=1;
			denm>>=1;
		}
		if(intInches)					/* if dim > 1" */
			sResult.Format("%ld-%ld/%d%s", intInches, num, denm, (LPCTSTR)sInchSym);
		else
			sResult.Format("%ld/%d%s",num, denm, (LPCTSTR)sInchSym);
	}
	else
		sResult.Format("%ld%s",intInches, (LPCTSTR)sInchSym);
	return sResult;
} /* inches */

/*-------------------------------------------------------------------------
	Create a feet-inches dimension string as a mixed fraction;  f'-i-n/d"
/*-----------------------------------------------------------------------*/
CString CDimUnit::feetInches(double dimDist)
{
	short denm;
	long feet, inches, num;

	denm=m_nFraction;
	dimDist/=(m_Conversion*denm);

	/*  Convert to integral inches and n/d fractional inches. */
	inches=(long)dimDist;
	num=(long)((dimDist-inches)*denm+0.5);

	/*  Rounding: gCurrDimUnits.fPlaces = number of accurate decimal places.
		Thus if gCurrDimUnits.fPlaces = 0 we cannot support fractions.		*/
	if(!m_nPlaces)
	{
		if(num>31L)
			inches++;
		num=0L;
	}
	else if(num/denm)
	{
		inches++;
		num=0L;
	}

	feet=inches/12L;
	inches%=12L;

	/* Reduce the fraction, assuming that the divisor is a power of 2. 		*/
	if(num)
		while(!(num & 0x0001L))
		{
			num>>=1;
			denm>>=1;
		}

	/* Build the string */
	CString sResult;
	if(feet && inches && num)	sResult.Format("%ld%s %ld-%ld/%d%s", feet, (LPCTSTR)sFootSym, inches, num, denm, (LPCTSTR)sInchSym);
	else if(feet && inches)		sResult.Format("%ld%s %ld%s", feet, (LPCTSTR)sFootSym, inches, (LPCTSTR)sInchSym);
	else if(feet && num)		sResult.Format("%ld%s %ld/%d%s", feet, (LPCTSTR)sFootSym, num, denm, (LPCTSTR)sInchSym);
	else if(feet)				sResult.Format("%ld%s",feet, (LPCTSTR)sFootSym);
	else if(inches && num)		sResult.Format("%ld-%ld/%d%s", inches, num, denm, (LPCTSTR)sInchSym);
	else if(inches)				sResult.Format("%ld%s", inches, (LPCTSTR)sInchSym);
	else if(num)				sResult.Format("%ld/%d%s", num, denm, (LPCTSTR)sInchSym);
	else						sResult="0"+sInchSym;
	
	return sResult;
} /* feetInches */

/*----------------------------------------------------------------------------
  decFeetInches -	create the feet and decimal inches dimension string
  ----------------------------------------------------------------------------*/
CString CDimUnit::decFeetInches(double dimDist)
{
	long feet;
	double dim,in;
	double sigPlace;
	short i;

	dim=dimDist/m_Conversion;

	feet=(long)(dim/12.0);
	in=fmod(dim,12.0);

	/* Check if 11.xxxx inches will be rounded up to 12.0 inches */

	sigPlace=1.0;
	for(i=0;i<m_nPlaces;i++)
		sigPlace*=10.0;

	sigPlace=(12.0-in)*sigPlace;

	if(sigPlace<=0.5)
	{
		in=0.0;
		feet++;
	}

	CString sResult;
	if(feet)
	{
		sResult.Format("%ld%s ",feet, gUnitStr[kDimFt_Dec]);
		sResult+=formatDecDim(in, kDimIn_Dec);
	}
	else
		sResult=formatDecDim(dim,kDimIn_Dec);

	return sResult;
} /* decFtInches */

/*-----------------------------------------------------------------------
	decimalDim -	build a dimension string for decimal type dimension
------------------------------------------------------------------------*/
CString CDimUnit::decimalDim(double dimDist,short units)
{
	double dim=dimDist/m_Conversion; // /m_Conversion;
	return formatDecDim(dim,units);
} /* decimalDim */

/*---------------------------------------------------------------------
	formatDecDim -	create a string from decimal using current dim units
						for # of decimal places
------------------------------------------------------------------------*/
CString CDimUnit::formatDecDim(double dim,short units)
{
	CString sResult;
	if(units==kDimNoUnits)
		sResult.Format("%.*f",m_nPlaces,dim);
	else
		sResult.Format("%.*f%s", m_nPlaces, dim, gUnitStr[units]);	/* our old friend */
	return sResult;
} /* formatDecDim */


/*
	numUser2Str converts the given value in user coordinates to its printed
	equivalent.
*/
CString CDimUnit::numUser2Str(double value,bool bTolerance)
{
	value*=m_Conversion*m_nFraction;
	return calcDimStr(value,bTolerance);
}

//CString CDimUnit::numUser2Mm(double dimDist, bool bTolerance)
//{
//   dimDist *= m_Conversion * m_nFraction;
//   bool negative;
//   short length;
//   short i;
//   short units;
//   CString sText;
//
//   if ((negative = dimDist < 0) != 0)
//      dimDist = -dimDist;
//
//
//   units = kDimMM;
//   sText = decimalDim(dimDist, units);
//
//   if (negative)
//   {
//      if (sText.GetLength() > 0 && isdigit(sText[0]))
//      {
//         sText = "-" + sText;
//      }
//   }
//
//   if (bTolerance)
//   {
//      // strip units
//      i = sText.GetLength();
//      length = 0;
//      while (--i >= 0)
//      {
//         char c = sText.GetAt(i);
//         // test for digit, / (fraction, ., -, or blank
//         if (!(c >= '0' && c <= '9') && (c != '/' && c != '.' && c != '-' && c != ' '))
//         {
//            length++;
//         }
//         else
//            break;
//      }
//      if (length)
//         sText = sText.Left(sText.GetLength() - length);
//   }
//   return sText;
//}
//
double CDimUnit::numDB2User(LongRatio& scaleRatio,WrFixed dbValue)
{
	double d;
	d=(mtFixToReal(dbValue)*(double)scaleRatio.fD)/(double)scaleRatio.fN;
	d/=m_Conversion*m_nFraction;
	return d;
}

double CDimUnit::numDBToMm(LongRatio& scaleRatio, WrFixed dbValue)
{
   double d;
   d = (mtFixToReal(dbValue)*(double)scaleRatio.fD) / (double)scaleRatio.fN;
   d = d * 25.4 / 72.0;
   return d;
}

WrFixed CDimUnit::numUser2DB(LongRatio& scaleRatio,double userValue)
{
	userValue*=m_Conversion*m_nFraction;
	return WrFixed((userValue*(double)scaleRatio.fN)/(double)scaleRatio.fD);
} /* numUser2DB */

CString CDimUnit::numAngle2Str(Angle angle)
{
	short deg, min, sec;
	CString sText="",sTemp;
	
	if(angle<FixedZero)
	{
		angle=-angle;
		sText="-";
	}

	mtAng2DMS(angle, &deg, &min, &sec);
	CString sSymbol;
	if(deg || !(deg || min || sec))
	{
		sSymbol.LoadString(IDS_DEGREE_SYMBOL);
		sTemp.Format("%d", deg);
		sText+=sTemp+sSymbol;
	}
	if(min || (deg && sec))
	{
		sSymbol.LoadString(IDS_MINUTE_SYMBOL);
		sTemp.Format(" %d", min);
		sText+=sTemp+sSymbol;
	}
	if(sec)
	{
		sSymbol.LoadString(IDS_SECOND_SYMBOL);
		sTemp.Format(" %d", sec);
		sText+=sTemp+sSymbol;
	}
	return sText;
}

CString CDimUnit::makeADistStr(Angle angle, bool bDegreeOnly)
{
	short deg, min, sec;
	short places=bDegreeOnly?0:m_nAngPlaces;
	CString sText="";

	if(m_nAngUnits<kDecimalDegrees)	// user defined
	{
		double dblAngle=-(mtFixToReal(angle)*m_nAngUnits)/360.0 ; // Convert from degrees to user units
		if(places==0) sText.Format("%d",short(dblAngle+0.5));
		else sText.Format("%3.*f",places,dblAngle);
	}
	else if(m_nAngUnits == kDecimalDegrees)
	{
		sText.Format("%3.*f%s",places,mtFixToReal(angle),(LPCTSTR)sDegreeSym);
	}
	else
	{
		mtAng2DMS(angle, &deg, &min, &sec);
		if(m_nAngUnits == kDegreesMinutes)
		{
			if(sec>=30) min++;
			if(min==60) {  deg++;  min=0; }
			if(deg==360) deg=0;

			sText.Format("%3d%s %2d%s", deg, (LPCTSTR)sDegreeSym, min, (LPCTSTR)sMinuteSym);
		}
		else if(m_nAngUnits == kDegMinSeconds)
		{
			sText.Format("%3d%s %2d%s %2d%s", deg, (LPCTSTR)sDegreeSym, min, (LPCTSTR)sMinuteSym, sec, (LPCTSTR)sSecondSym);
		}
	}
	return sText;
}

CString CDimUnit::inchAreaStr(double rawArea)
{
	double area;

	area = rawArea / (m_Conversion * m_Conversion);
	if(m_Display == kDimIn) area /= (m_nFraction * m_nFraction);

	return formatDecDim (area, kAreaIn);
}

CString CDimUnit::feetAreaStr(double rawArea)
{
	double area;
	bool fractional;

	area = rawArea / (m_Conversion * m_Conversion);

	if (((fractional = (m_Display == kDimFt_In)) != 0) || m_Display == kDimFt_In_Dec)
	{
		if (fractional)
		{
			area /= (m_nFraction * m_nFraction);
		}

		if (area < 144.0)   // print in inches
		{
			return formatDecDim (area, kAreaIn);
		}
		area /= 144.0;		// sq inches to sq feet
	}

	return formatDecDim(area, kAreaFt);
}

CString CDimUnit::metricAreaStr(double rawArea, short which)
{
	double area;
	area = rawArea / (m_Conversion * m_Conversion);
	return formatDecDim (area, which);
}

CString CDimUnit::calcAreaStr(double rawArea)
{
	CString areaStr;
	switch (m_Display) {

		case kDimIn:		/* Fractional Inches  */
		case kDimIn_Dec:	/* Decimal Inches  */
			areaStr = inchAreaStr (rawArea);
			break;

		case kDimFt_In:		/* feet and fractional inches */
		case kDimFt_In_Dec:	/* feet and decimal inches */
		case kDimFt_Dec:	/* Decimal Feet */
			areaStr = feetAreaStr (rawArea);
			break;

		case kDimM:			/* meters */
			areaStr = metricAreaStr (rawArea, kDimM);
			break;

		case kDimDM:		/* decimeters */
			areaStr = metricAreaStr (rawArea, kDimDM);
			break;

		case kDimCM:		/* centimeters */
			areaStr = metricAreaStr (rawArea, kDimCM);
			break;

		case kDimMM:		/* millimeters */
			areaStr = metricAreaStr (rawArea, kDimMM);
			break;
	}
   //areaStr += '\xB2';
   areaStr += "\xc2\xb2"; // Infinisys Ltd. square sign (two superscript) - 2021
	return areaStr;
}

/*
	numStr2User returns true if the expression in the input string is a valid
	ordinate specified in the current units.  value is set to the value of the
	expression in the current units.
*/

bool CDimUnit::numStr2User(LPCTSTR str,double& value)
{
	LPCTSTR savedPos;
	bool negative;
	bool result=false;
	double conversion=1.0;
	DimDisp_t units=m_Display;
	bool metric=false;

	if(*str)
	{
		negative=checkStr(str,3,sDash, sEnDash, sEmDash)?true:false;
		savedPos=str;
		switch(units)
		{
			case kDimM:
			case kDimDM:
			case kDimCM:
			case kDimMM:
				metric=true;
				if(!(result=decMetric(str,value)))
				{
					switch(units)
					{
						case kDimM:
							conversion=0.0254;
							break;
						case kDimDM:
							conversion=0.254;
							break;
						case kDimCM:
							conversion=2.54;
							break;
						case kDimMM:
							conversion=25.4;
							break;
					}
					str=savedPos;
					if(!(result=decFtIn(str,value)))
					{
						str=savedPos;
						result=fracFtIn(str,value);
					}
				}
				break;
			case kDimFt_Dec:
				if((result=decimalStr(str,value))!= 0)
				{
					IsFootSymbol(str);
					result=!*str;
				}
				if(!result)
				{
					str=savedPos;
					conversion/=12.0;
					if(!(result=decFtIn(str,value)))
					{
						str=savedPos;
						result=fracFtIn(str,value);
					}
				}
				break;
			case kDimIn_Dec:
				if(!(result=decIn(str,value)))
				{
					str=savedPos;
					if(!(result=decFtIn(str,value)))
					{
						str=savedPos;
						result=fracFtIn(str,value);
					}
				}
				break;
			case kDimFt_In:
				if(!(result=fracFtIn(str,value)))
				{
					str=savedPos;
					result=decFtIn(str,value);
				}
				break;
			case kDimFt_In_Dec:
				if(!(result=decFtIn(str,value)))
				{
					str=savedPos;
					result=fracFtIn(str,value);
				}
				break;
			case kDimIn:
				if(!(result=fracIn(str,value)))
				{
					str=savedPos;
					if(!(result=fracFtIn(str,value)))
					{
						str=savedPos;
						result=decFtIn(str,value);
					}
				}
				break;
		}
		if(!result && !metric)
		{
			str=savedPos;
			result=decMetric(str,value);
		}
	}
	value*=conversion;
	if(negative)
	{
		value=-value;
	}
	return result;
}

/*
	fracFtIn returns true if the expression in the input string is specified in
	fractional feet and inches.  inches is set to the value of the expression.
*/

bool CDimUnit::fracFtIn(LPCTSTR& str,double& inches)
{
	long ft;
	double in;
	LPCTSTR savedPos;
	ft=0;
	in=0;
	savedPos=str;
	if(m_ScaleIndex>=kFeetScale || !fracIn(str,in))
	{
		in=0;
		str=savedPos;
		if(integer(str,ft))
		{
			IsFootSymbol(str);
			if(!fracIn(str,in))
			{
				in=0;
			}
			if(*str)
			{
				str=savedPos;
				ft=0;
				fracIn(str,in);
			}
		}
	}
	inches=ft*12+in;
	return !*str;
}

/*----------------------------------------------------------------------*/

/*
	fracIn returns true if the expression in the input string is specified in
	fractional inches.  inches is set to the value of the expression.
*/

bool CDimUnit::fracIn(LPCTSTR& str,double& inches)
{
	long number;
	LPCTSTR savedPos;
	double fracPart;
	savedPos=str;
	if(!isFraction(str,inches))
	{
		str=savedPos;
		if(!integer(str,number))
		{
			return false;
		}
		inches=number;
		checkStr(str,3,sDash,sEnDash,sEmDash);
		savedPos=str;
		if(isFraction(str,fracPart))
		{
			inches+=fracPart;
		}
		else
		{
			str=savedPos;
		}
	}
	checkStr(str,3,sInchSym,sInchSym2,sInchAbbr);
	return !*str;
}

/*----------------------------------------------------------------------*/

/*
	decFtIn returns true if the expression in the input string is specified in
	decimal feet and inches.  inches is set to the value of the expression.
*/

bool CDimUnit::decFtIn(LPCTSTR& str,double& inches)
{
	double ft,in;
	LPCTSTR savedPos;
	ft=in=0;
	savedPos=str;
	if(m_ScaleIndex>=kFeetScale || !decIn(str,in))
	{
		in=0;
		str=savedPos;
		if(decimalStr(str,ft))
		{
			savedPos=str;
			if(IsFootSymbol(str) || str!=savedPos)
			{
				if(!decIn(str,in))
				{
					in=0;
				}
				if(*str)
				{
					str=savedPos;
					ft=0;
					decIn(str,in);
				}
			}
		}
	}
	inches=ft*12+in;
	return !*str;
}

/*----------------------------------------------------------------------*/

/*
	decIn returns true if the expression in the input string is specified in
	decimal inches.  value is set to the value of the expression.
*/

bool CDimUnit::decIn(LPCTSTR& str,double& value)
{
	if(decimalStr(str,value))
	{
		checkStr(str,3,sInchSym,sInchSym2,sInchAbbr);
		return !*str;
	}
	return false;
}

/*----------------------------------------------------------------------*/

/*
	decMetric returns true if the expression in the input string is specified in
	any decimal metric measurement.  value is set to the value of the expression.
*/

bool CDimUnit::decMetric(LPCTSTR& str,double& value)
{
	LPCTSTR savedPos;
	bool result;
	double conversion=1.0;
	if((result=decimalStr(str,value))!=0)
	{
		if(*str)
		{
			savedPos=str;
			switch(m_Display)
			{
				case kDimM:
					break;
				case kDimDM:
					conversion=10.0;
					break;
				case kDimCM:
					conversion=100.0;
					break;
				case kDimMM:
					conversion=1000.0;
					break;
				default:
					conversion=39.37;
					break;
			}
			if(!(result=(checkStr(str,1,sMeterAbbr) && !*str)))
			{
				str=savedPos;
				conversion/=10.0;
				if(!(result=checkStr(str,1,sDeciAbbr)?true:false))
				{
					str=savedPos;
					conversion/=10.0;
					if(!(result=checkStr(str,1,sCentiAbbr)?true:false))
					{
						str=savedPos;
						conversion/=10.0;
						result=checkStr(str,1,sMilliAbbr)?true:false;
					}
				}
			}
		}
	}
	if(result)
	{
		value*=conversion;
	}
	return result;
}

/*----------------------------------------------------------------------*/

/*
	isFraction returns true if the next expression in the input string is a
	fraction.  value is set to the value of the fraction.
*/

bool CDimUnit::isFraction(LPCTSTR& str,double& value)
{
	long numer,denom;
	if(integer(str,numer) && checkStr(str,1,sFracBar) && integer(str,denom) && denom)
	{
		value=(double)numer/denom;
		return true;
	}
	return false;
} /* isFraction */

/*----------------------------------------------------------------------*/

/*
	decimal returns true if the next expression in the input string is a real
	number.  value is set to the value of the real number.
*/

bool CDimUnit::decimalStr(LPCTSTR& str,double& value)
{
	double fracPart;
	bool isWhole;
	LPCTSTR savedPos;
	short i;
	isWhole=digits(str,value);
	if(*str==kDecPoint)
	{
		++str;
		savedPos=str;
		if(!digits(str,fracPart) && !isWhole)
		{
			return false;
		}
		isWhole=true;
		i=str-savedPos;
		while(--i>=0)
		{
			fracPart/=10;
		}
		value+=fracPart;
	}
	return isWhole;
}

/*----------------------------------------------------------------------*/

bool CDimUnit::numStr2PosInt(LPCTSTR str,long& value)
{
	bool retVal;
	if((retVal=integer(str,value))!=0)
		value=max(1,value);
	else
		value=1;
	return retVal;
} /* numStr2PosInt */

/*----------------------------------------------------------------------*/
/*
	integer returns true if the next expression in the input string is an
	integer.  value is set to the value of the integer.
*/

bool CDimUnit::integer(LPCTSTR& str,long& value)
{
	double number;
	if(digits(str,number))
	{
		if(number<=LONG_MAX)
		{
			value=(long)number;
			return true;
		}
	}
	return false;
}

/*----------------------------------------------------------------------*/

/*
	digits returns true if the next expression in the input string is a string
	of digits.  value is set to the value of the digits.
*/

bool CDimUnit::digits(LPCTSTR& str,double& value)
{
	unsigned char ch;
	value=0;
	if(((ch=*str)!=0) && isdigit(ch))
	{
		do
		{
			value=value*10+(ch-kAsciiZero);
			++str;
		}
		while(((ch=*str)!=0) && isdigit(ch));
		return true;
	}
	return false;
}

/*----------------------------------------------------------------------*/

/*
	checkStr checks if the next substring in the input string matches any of the
	given strings.  It returns the number of the string that matches or zero if
	none matches.
*/

short CDimUnit::checkStr(LPCTSTR& str,short numStrs,...)
{
	va_list marker;
	va_start(marker,numStrs);
	short i=0;
	space(str);
	LPCTSTR savedPos=str;
	while(++i<=numStrs)
	{
		LPCTSTR strPtr=va_arg(marker,LPCTSTR);
		while(*strPtr && *strPtr==tolower(*str))
		{
			++str;
			++strPtr;
		}
		if(!*strPtr)
		{
			space(str);
			return i;
		}
		str=savedPos;
	}
	va_end(marker);
	return 0;
}

/*----------------------------------------------------------------------*/

/*
	space skips any whitespace in the input string.
*/

void CDimUnit::space(LPCTSTR& str) 
{
	char ch;
    // SMK SSA. char ch is -127 - 128. kHardSpace = \312 ????
	while(((ch=*str)!=0) && (isspace(ch) || ch==kHardSpace))
	{
		++str;
	}
}

/*----------------------------------------------------------------------*/

/*
	numStr2Angle returns true if the expression in the input string is an angle.
	degrees is set to the value of the expression.
*/

bool CDimUnit::numStr2Angle(LPCTSTR str,Angle& degrees)
{
	bool negative;
	double number,parts[3],deg;
	long wholeDeg;
	short i=0;
	if(*str)
	{
		negative=checkStr(str,3,sDash,sEnDash,sEmDash)?true:false;
		parts[0]=parts[1]=parts[2]=0;
		while(i<3 && decimalStr(str,number))
		{
			switch(checkStr(str,8,sDegreeSym,sDegreeAbbr,sMinuteSym,sMinuteSym2,sMinuteAbbr,sSecondSym,sSecondSym2,sSecondAbbr))
			{
				case 1:
				case 2:
					if(i)
					{
						return false;
					}
					break;
				case 3:
				case 4:
				case 5:
					if(i>1)
					{
						return false;
					}
					i=1;
					break;
				case 6:
				case 7:
				case 8:
					i=2;
					break;
				}
			parts[i++]=number;
		}
		deg=parts[0]+(parts[1]+parts[2]/60)/60;
		wholeDeg=(long)deg;
		degrees.SetFromDoubleTruncated((deg-wholeDeg+(wholeDeg%360)));
		if(negative)
		{
			degrees=-degrees;
		}
		return !*str;
	}
	return false;
}

void CDimUnit::switchUnits(short units, short res)
{
//_Display=DimDisp_t(units);
	setDimUnits(units,m_bHideMetricUnits,m_nPlaces,res,m_nAngUnits,m_nAngPlaces);
}

bool CDimUnit::IsFootSymbol(LPCTSTR &str)
{
	return checkStr(str,3,sFootSym,sFootSym2,sFootAbbr)?true:false;
}
