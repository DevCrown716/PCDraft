#include "stdafx.h"
#include "phmath.h"

#include <math.h>

/******************************************************

	Copyright © 1990-98 by Microspot Ltd. All Rights Reserved.
	
	 
	  Created					: 4/21/88
	  
******************************************************/

void mtAng2DMS(Angle longAngle,	short *degree, short *minute, short	*second)
{
	*degree = TruncToShort(longAngle);
	longAngle = longAngle.FracPart () * short(60);
	*minute = TruncToShort(longAngle);
	longAngle = longAngle.FracPart () * short(60);
	*second = mtFixToShort(longAngle);
	
	if(*second == 60 )
	{
		*second = 0;
		if(++(*minute) == 60 )
		{
			*minute = 0;
			if(++(*degree) == 360 )
				*degree = 0;
		}
	}
}

Angle mtDMS2Ang(short degree, short	minute, short second)
{
	degree%=360; minute%=60; second%=60;
	return WrFixed(degree+minute/60.0+second/3600.0);
}

Angle mtFixAngle(Angle pAngle)
{
	while(pAngle<FixedZero)
		pAngle+=short(360);
	while(pAngle>=short(360))
		pAngle-=short(360);
	return pAngle;
}

Angle mtFastATan2(const LongPoint& p)
{
	if(p.x==FixedZero && p.y==FixedZero)
		return FixedZero;
#if TARGET_CPU_68K
	Angle vAngle = WrFixedFromFixed(FixMul(FixATan2 (p.x.GetFixed () , p.y.GetFixed () ), kFastRadToAng));
#elif TARGET_CPU_PPC
	Angle vAngle = mtRealToFix(atan2(p.y.GetRawDouble(), p.x.GetRawDouble())*kRadToAng);
#else
	Angle vAngle = mtRealToFix(atan2(p.y.GetRawDouble(), p.x.GetRawDouble())*kRadToAng);
#endif
	vAngle = k360 - vAngle;/* flip angle */
	if(vAngle>=k360)
		vAngle-=k360;
	return vAngle;
}
const double kRadianToDegree		= 45.0 / atan(1.0); // MD6.2.1 - Infinisys Ltd.
const double kDegreeToRadian		= atan(1.0) / 45.0; // MD6.2.1 - Infinisys Ltd.

Angle mtATan2(const LongPoint& p)
{
	if(p.x == FixedZero && p.y == FixedZero)
		return FixedZero;
   Angle vAngle = mtRealToFix(atan2(p.y.GetRawDouble(),p.x.GetRawDouble())*kRadianToDegree);
	vAngle = k360-vAngle;
	if(vAngle >= k360 )
		vAngle -= k360;
	
	return vAngle;
}/* mtATan2 */

// MD6.2.1 - Infinisys Ltd.
Angle mtATan2NoFix(const LongPoint& p) 
{
   if(p.x == FixedZero && p.y == FixedZero)
      return FixedZero;
   Angle vAngle = -atan2(p.y.GetRawDouble(),p.x.GetRawDouble())*kRadToAng;
   return vAngle;
}/* mtATan2NoFix */

/*----------------------------------------------------------------------*/

Angle	mtEATan2 (
				  const LongPoint& p,
				  Coord	aAxis,
				  Coord	bAxis)
{
	if(aAxis == FixedZero && bAxis == FixedZero)return FixedZero;
	
	if(aAxis <= bAxis )
		return(mtATan2(LongPoint(p.x , WrFixedFromRawDoubleTruncated(p.y.GetRawDouble () *(aAxis.GetRawDouble () / bAxis.GetRawDouble ())))));
	else
		return(mtATan2(LongPoint(WrFixedFromRawDoubleTruncated(p.x.GetRawDouble () *(bAxis.GetRawDouble () / aAxis.GetRawDouble ())) , p.y)));
}/* mtEATan2 */

/*----------------------------------------------------------------------*/

const double kOneOver2toPower30=1.0/1073741824.0;


double mtCos(double dAngle) { return ::cos((360.0 - dAngle) * _M_PI / 180.0); }
double mtSin(double dAngle) { return ::sin((360.0 - dAngle) * _M_PI / 180.0); }


double mtCos(
			   Angle pAngle)
{
	pAngle = short(360)- pAngle;			/* flip angle */
	
	if((pAngle == short(90)) || (pAngle == short(270)) )
		return(( double)0);
	else
		if(pAngle == FixedZero )
			return(( double)1);
		else
			if(pAngle == short(180))
				return(( double)-1);
			else
			{
#if TARGET_CPU_68K
				return FracCos(( pAngle * WrFixedFromFixed(kFastAngToRad)).GetFixed ())* kOneOver2toPower30;
#elif TARGET_CPU_PPC
				return cos(pAngle.GetRawDouble () * kLongAngToRad);
#else // presumably TARGET_CPU_X86
				return cos(pAngle.GetRawDouble () * kLongAngToRad);
				// Assuming x86 has a fast hardware floating point cosine instruction
#endif
			}
}/* mtCos */


/*----------------------------------------------------------------------*/


Fract mtLCos (
			  Angle pAngle)
{
	pAngle = short(360)- pAngle;			/* flip angle */
	
	if(( pAngle == short(90)) ||(pAngle == short(270)) )
		return(0L);
	else
		if(pAngle == FixedZero )
			return(0x40000000);			/* 1 << 30 */
		else
			if(pAngle == short(180))
				return(0xC0000000);		/* -1 << 30 */
			else
			{
#if TARGET_CPU_68K
				return FracCos(( pAngle * WrFixedFromFixed(kFastAngToRad)).GetFixed ());
#elif TARGET_CPU_PPC
				return roundtol(cos(pAngle.GetRawDouble () * kLongAngToRad)* kRealToFrac);
#else // presumably TARGET_CPU_X86
				return roundtol(cos(pAngle.GetRawDouble () * kLongAngToRad)* kRealToFrac);
				// Assuming x86 has a fast hardware floating point cosine instruction
#endif
			}
}/* mtLCos */

/*----------------------------------------------------------------------*/

double mtSin (
				Angle pAngle)
{
	pAngle = short(360)- pAngle;			/* flip angle */
	
	if(( pAngle == FixedZero)||(pAngle == short(180)) )
		return(( double)0);
	else
		if(pAngle == short(90))
			return(( double)1);
		else
			if(pAngle == short(270))
				return(( double)-1);
			else
			{
#if TARGET_CPU_68K
				return FracSin(( pAngle * WrFixedFromFixed(kFastAngToRad)).GetFixed ())* kOneOver2toPower30;
#elif TARGET_CPU_PPC
				return sin(pAngle.GetRawDouble () * kLongAngToRad);
#else // presumably TARGET_CPU_X86
				return sin(pAngle.GetRawDouble () * kLongAngToRad);
				// Assuming x86 has a fast hardware floating point cosine instruction
#endif
			}
}/* mtSin */

Fract mtLSin(Angle pAngle)
{
	pAngle = short(360)- pAngle;		/* flip angle */
	
	if(( pAngle == FixedZero)||(pAngle == short(180)) )
		return( 0L );
	else
		if(pAngle == short(90))
			return( 0x40000000 );	/* 1 << 30 */
		else
			if(pAngle == short(270))
				return( 0xC0000000 );	/* -1 << 30 */
			else
			{
#if TARGET_CPU_68K
				return FracSin(( pAngle * WrFixedFromFixed(kFastAngToRad)).GetFixed ());
#elif TARGET_CPU_PPC
				return roundtol(sin(pAngle.GetRawDouble () * kLongAngToRad)* kRealToFrac);
#else // presumably TARGET_CPU_X86
				return roundtol(sin(pAngle.GetRawDouble () * kLongAngToRad)* kRealToFrac);
				// Assuming x86 has a fast hardware floating point cosine instruction
#endif
			}
}

/*  Rotate a point around a given reference point, by a angle given by
its sine and cosine fract value.									*/

LongPoint mtRotPoint(const LongPoint& pPoint, const TSinCos& sinCos)
{
	LongPoint result;
	
	double h = pPoint.x.GetRawDouble ();
	double v = pPoint.y.GetRawDouble ();
	
	result.x.SetFromRawDoubleTruncated(h * sinCos.fCos - v * sinCos.fSin);
	result.y.SetFromRawDoubleTruncated(h * sinCos.fSin + v * sinCos.fCos);
	
	return result;
}

LongPoint mtRotPoint(const LongPoint& pPoint, const TSinCos& sinCos, const LongPoint& pRefPoint)
{
	LongPoint result;
	
	double h =(pPoint.x - pRefPoint.x ).GetRawDouble ();
	double v =(pPoint.y - pRefPoint.y ).GetRawDouble ();
	
	result.x.SetFromRawDoubleTruncated(h * sinCos.fCos - v * sinCos.fSin + pRefPoint.x.GetRawDouble ());
	result.y.SetFromRawDoubleTruncated(h * sinCos.fSin + v * sinCos.fCos + pRefPoint.y.GetRawDouble ());
	
	return result;
}

LongPoint mtRotPoint(const LongPoint& pPoint, Angle angle)
{
	if(angle==FixedZero)
		return pPoint;
	TSinCos sinCos(angle);
	return mtRotPoint(pPoint, sinCos);
}

LongPoint mtRotPoint(const LongPoint& pPoint, Angle angle, const LongPoint&	pRefPoint)
{
	if(angle==FixedZero)
		return pPoint;
	TSinCos sinCos(angle);
	return mtRotPoint(pPoint, sinCos, pRefPoint);
}

/*  Rotate a point around a given zoomed reference point, by a angle
given by its sine and cosine fract value. The point is returned in
zoomed coordinates.													*/

CPoint mtRotPointV(const CPoint&	pPoint,	const TSinCos& sinCos, CPoint pRefPtPtr)
{
	CPoint result;
	
	int h = pPoint.x - pRefPtPtr.x;
	int v = pPoint.y - pRefPtPtr.y;
	
	result.x = long(((sinCos.fCos * h - sinCos.fSin * v)+ pRefPtPtr.x));
	result.y = long(((sinCos.fSin * h + sinCos.fCos * v)+ pRefPtPtr.y));
	
	return result;
}

/* Flip the given angle. The direction of the flipping indicated by whichWay.
	For ARC angles, use mtFlipArcAngle().*/

Angle mtFlipAngle(Angle	pAngle, short whichWay)
{
	if(whichWay==kFlipHorizontal )
		return mtFixAngle(-pAngle);
	else
		return mtFixAngle(short(180)-pAngle);
}

/* Flip the given arc angles.
The direction of the flipping indicated by whichWay.

	Angle must be in real angle, 0 is at 3 o'clock
	and angle goes counter clockwise.
	Before or after calling this function,
	call mtFlipAngle() for the arc rotation. */

void mtFlipArcAngle(Angle *pStartAngle, Angle *pEndAngle, short whichWay)
{
	Angle sta=*pStartAngle, eda=*pEndAngle;
	if(whichWay==kFlipHorizontal )
	{
		*pStartAngle=mtFixAngle(-eda);
		*pEndAngle=mtFixAngle(-sta);
	}
	else
	{
		*pStartAngle=mtFixAngle(short(180)-eda);
		*pEndAngle=mtFixAngle(short(180)-sta);
	}
}

/////////////////////////////////////////////////////////////
// TSinCos
TSinCos::TSinCos(Angle pAngle) //Replaces mtLSin and mtLCos
{
	pAngle=short(360)-pAngle;		/*flip angle*/
	if(pAngle==FixedZero)
	{
		fSin=0.0;
		fCos=1.0;
	}
	else if(pAngle==short(90))
	{
		fSin=1.0;
		fCos=0.0;
	}
	else if(pAngle==short(180))
	{
		fSin=0.0;
		fCos=-1.0;
	}
	else if(pAngle==short(270))
	{
		fSin=-1.0;
		fCos=0.0;
	}
	else
	{
#if TARGET_CPU_68K
		Fixed radians=(pAngle*WrFixedFromFixed(kFastAngToRad)).GetFixed();
		fSin=FracSin(radians)*kOneOver2toPower30;
		fCos=FracCos(radians)*kOneOver2toPower30;
#elif TARGET_CPU_PPC
		double radian=pAngle.GetRawDouble()*kLongAngToRad;
		fSin=sin(radian);
		fCos=cos(radian);
#else
		double radian=pAngle.GetRawDouble()*kLongAngToRad;
		fSin=sin(radian);
		fCos=cos(radian);
#endif
	}
}
