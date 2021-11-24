#ifndef __PHMATH_H__
#define __PHMATH_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "lpoint.h"

#define	kFlipHorizontal					(-701)
#define	kFlipVertical					(-702)

// Constants

const double _M_PI = 3.1415926535897932384626433832795;

const Angle	 k5				= short(5);
const Angle	 k15			= short(15);
const Angle	 k20			= short(20);
const Angle	 k30			= short(30);
const Angle	 k45			= short(45);
const Angle	 k70			= short(70);
const Angle	 k90			= short(90);
const Angle	 k135			= short(135);
const Angle	 k180			= short(180);
const Angle	 k225			= short(225);
const Angle	 k270			= short(270);
const Angle	 k315			= short(315);
const Angle	 k360			= short(360);
const Angle	 k450			= short(450);


const double kRadToAng		= 180/_M_PI; 		/* 180/pi */
const double kAngToRad		= _M_PI/180;		/* pi/180 */

const double kShortToLong	= 65536.0;

const Coord	 kOnePixel		= short(1);		/* One pixel in long format				*/
const Coord	 kTwoPixels		= short(2);		/* Two pixels in long format			*/
const Coord	 k3Pixels		= short(3);		/* Three pixels in fixed pt. format.	*/
const Coord	 k4Pixels		= short(4);		/* Four pixels in fixed pt. format.		*/
const Coord	 k5Pixels		= short(5);		/* Five pixels in fixed pt. format.		*/
const Coord	 k6Pixels		= short(6);		/* Six pixels in fixed pt. format.		*/
const Coord	 k7Pixels		= short(7);		/* Seven pixels in fixed pt. format.	*/
const Coord	 k8Pixels		= short(8);		/* Eight pixels in fixed pt. format.	*/
const Coord	 k9Pixels		= short(9);		/* Nine pixels in fixed pt. format.		*/
const Coord	 k10Pixels		= short(10);	/* Ten pixels in fixed pt. format.		*/
const Coord	 k11Pixels		= short(11);	/* Eleven pixels in fixed pt. format.	*/
const Coord	 k12Pixels		= short(12);	/* Twelve pixels in fixed pt. format.	*/
const Coord	 k13Pixels		= short(13);	/* Thirteen pixels in fixed pt. format.	*/

const Angle	kUpperArcLimit = WrFixedFromFixed(23539870L);	/* 359.19 degrees in long format */
const Angle	kLowerArcLimit = WrFixedFromFixed(43910L);		/* 0.67 degrees in long format */

const double kRealToFrac	= double(2L<<30);			/* 2 to the 30th power	*/
const double kLongAngToRad	= _M_PI/180/65536;			/* pi / 180 / 65536		*/
const long	 kFastRadToAng	= long(65536.0*180/_M_PI);	/* 65536 * 180 / pi		*/
const long	 kFastAngToRad	= long(65536.0*_M_PI/180);  /* 65536 * P / 180		*/

/*---------------------------- inlines -------------------------------------*/

struct TSinCos
{
	TSinCos() {} ;
	TSinCos(Angle pAngle);

	double fSin ;
	double fCos ;
};

inline short mtRealToShort(double x)
{
	return (short)(x+(x<0.0?-0.5:0.5));
}

inline WrFixed mtRealToFix(double x )
{
	return WrFixedFromRawDoubleTruncated(x*kShortToLong+(x<0.0?-0.5:0.5));
}

inline short mtFixToShort(Coord f )
{
	long x = f.GetFixed();
	return short(( x + (x < 0L ? 32767L : 32768L)) >> 16) ;
}

// N.B. Coded this way to be 100% compatible with old versions of MacDraft
inline WrFixed mtRoundFixDbl(double dbl )
{
	return WrFixedFromRawDoubleTruncated(dbl +(dbl < 0.0 ? - 0.5 : 0.5)) ;
}

// For multiplying a fixed format number with a double80 format number.
// Use this macro for accurately multiplying a fixed number with a double
inline WrFixed mtFixMulDbl(WrFixed fxd, double dbl )
{
	return mtRoundFixDbl(fxd.GetRawDouble() * dbl);
}

// The overload below is to ensure we don't automatically scale a double up
// by 65536 and then get the raw double and multiply!
// SHOULD never be used, but is safe I think
inline WrFixed mtFixMulDbl(double d, double dbl)
{
	return mtRoundFixDbl(d*dbl);
}

/*------------------------ function prototypes -------------------------*/

void		mtAng2DMS(Angle, short*, short*, short*);
Angle		mtDMS2Ang(short, short, short);
Angle		mtFixAngle(Angle) ;

double		mtCos(Angle);
Fract		mtLCos(Angle);

double mtCos(double dAngle);
double mtSin(double dAngle);



double		mtSin(Angle);
Fract		mtLSin(Angle);

Angle		mtATan2(const LongPoint&);
Angle		mtATan2NoFix(const LongPoint&); // MD6.2.1 - Infinisys Ltd.
Angle		mtEATan2(const LongPoint&, Coord aAxis, Coord bAxis);
Angle		mtFastATan2(const LongPoint&);

LongPoint	mtRotPoint(const LongPoint&, const TSinCos&);
LongPoint	mtRotPoint(const LongPoint&, const TSinCos&, const LongPoint&);
LongPoint	mtRotPoint(const LongPoint&, Angle);
LongPoint	mtRotPoint(const LongPoint&, Angle, const LongPoint&);
CPoint		mtRotPointV(const CPoint&, const TSinCos&, CPoint);

Angle		mtFlipAngle(Angle, short whichWay);
void		mtFlipArcAngle(Angle*, Angle*, short whichWay);

inline Angle mtAngle(const LongPoint& p1, const LongPoint& p2 )
{
	return mtATan2(p2-p1);
}
// MD6.2.1 - Infinisys Ltd.
inline Angle mtAngleNoFix(const LongPoint& p1,const LongPoint& p2)
{
   return mtATan2NoFix(p2 - p1);
}

/******************************************************

	Copyright © Microspot Ltd, 1988-92


	Created			: 4/21/88

******************************************************/


#endif