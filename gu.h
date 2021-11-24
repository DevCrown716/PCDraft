/******************************************************

	Copyright © Microspot Ltd, 1988-98

	Graphic Utilities
	
	Created				: 4/21/88

******************************************************/

#ifndef __GU_H__
#define __GU_H__

#if _MSC_VER>1000
#pragma once
#endif

#include "lpoint.h"
#include "pointf.h" // Infinisys Ltd. GDI+
#include "pointd.h" // Infinisys Ltd. GDI+
#include "math.h"

enum LineAttribute
{
	kLeftJust,
	kCenterJust,
	kRightJust
};

#include "DSShape.h"


#define	kSelectOffset		2				/* max proximity						*/

#define	kBeginPtInPoly		1				/* for PtInPoly						*/
#define	kMorePtInPoly		2
#define	kEndPtInPoly		4

long gcd (register long u,register long v);

inline LongPoint Abs(const LongPoint& p)
{
	return LongPoint(Abs(p.x), Abs(p.y));
}
inline WrFixed ATan2(Coord x, Coord y)
{
	return double(atan2(y.GetRawDouble(), x.GetRawDouble()));
}

inline WrFixed ATan2(long x, long y)
{
    return (double)atan2((double)y, (double)x);
}
inline WrFixed ATan2(float x, float y)
{
   return (double)atan2(y, x);
}
inline WrFixed ATan2(double x, double y)
{
   return (double)atan2(y, x);
}

inline WrFixed ATan2(const LongPoint& p)// Radians, not degrees hence not 'Angle'
{
	return double(atan2(p.y.GetRawDouble (), p.x.GetRawDouble ()));
}

inline WrFixed Sin(WrFixed a)// Radians, not degrees hence not 'Angle'
{
	return double(sin(a.GetRawDouble () / 65536.0));
}

inline WrFixed Cos(WrFixed a)// Radians, not degrees hence not 'Angle'
{
	return double(cos(a.GetRawDouble () / 65536.0));
}

inline LongPoint CosSin(WrFixed a)// Radians, not degrees hence not 'Angle'
{
	return LongPoint(cos(a.GetRawDouble()/ 65536.0), sin(a.GetRawDouble()/ 65536.0));
}
	
inline LongPoint SinCos(WrFixed a)// Radians, not degrees hence not 'Angle'
{
	return LongPoint(sin(a.GetRawDouble()/ 65536.0), cos(a.GetRawDouble()/ 65536.0));
}

inline LongPoint Average(const LongPoint& p1, const LongPoint& p2)
{
	return p1 +((p2 - p1)>> 1);
}

inline LongPoint Middle(const LongRect& r)
{
	return Average(r[topLeft], r[bottomRight]);
}

inline LongPoint Dimensions(const LongRect& r)
{
	return r[bottomRight]-r[ topLeft ];
}

inline Coord Width(const LongRect& r)
{
	return r.right - r.left;
}

inline Coord Height(const LongRect& r)
{
	return r.bottom - r.top;
}

inline pointd Average(const pointd& p1, const pointd& p2)
{
	return pointd((p1.x+ p2.x) /2.0, (p1.y + p2.y)/ 2.0);
}

inline pointd Middle(const CRect& r)
{
	return Average(*((pointd*)&r),((pointd*)&r)[ 1 ]);
}

inline pointd Dimensions(const CRect& r)
{
	return pointd(r.right - r.left, r.bottom - r.top);
}

inline short Width(const CRect& r)
{
	return short(r.right-r.left);
}

inline short Height(const CRect& r)
{
	return short(r.bottom-r.top);
}

inline Coord _guLOrd(short aShort)
{
	return Coord(aShort);
}


Coord Pythag(const LongPoint& P);
double RawDoublePythag(const LongPoint& P);
double RawDoublePythagSqr(const LongPoint& P);

inline void SetPt(pointd* p, short h, short v)
{
	p->x=h;
	p->y=v;
}

Angle ChordTolerance ( float radiusInPixels ); 

/*--------------------------- graphical utilities ----------------------*/



Angle		guLPtToLAngle(Coord, Coord, const LongPoint&, const LongPoint&);
Angle		guLAngToLEAng(Coord, Coord, Angle);
bool		guExtendLineToRect(pointd*, pointd*, const CRect&);
void		guBezSplinePts(const LongPoint&, const LongPoint&, const LongPoint&, LongPointPtr, LongPointPtr);
bool		guMapOffset(LongPointPtr pt, const LongRect& srcRect, const LongRect& dstRect);
LongPoint	guMapPt(const LongPoint& pt, const LongRect& srcRect, const LongRect& dstRect);
bool		guPropMapOffset(LongPointPtr offset, const LongRect& srcRect, const LongRect& dstRect);
LongPoint	guPropMapPt(const LongPoint& pt, const LongRect& srcRect, const LongRect& dstRect);
bool		guParallelLn(const LongPoint&, const LongPoint&, Coord, short LineAttribute, LongPointPtr, LongPointPtr, LongPointPtr, LongPointPtr);
bool		guLongLnLnPt(const LongPoint&, const LongPoint&, const LongPoint&, const LongPoint&, LongPointPtr);
bool		guPtAtLine(pointd,pointd,pointd, short, short LineAttribute);
bool		guPtAtCirc(pointd,const pointd& pCenterPt, short pRadius, short pPenWidth, short Alignment, bool pHasFill);
bool		guPtAtEllip	(pointd,const pointd& pCenterPt, short pHRad, short pVRad, Angle pRot, short pPenWidth,Alignment Alignment, bool pHasFill);

bool		guPtInPoly(short, LongPointPtr, LongPointPtr, LongPointPtr);
void		flipArc(Angle*, Angle*, Angle*);
bool		guCircBy3Pt(LongPointPtr, Coord*, const LongPoint&, const LongPoint&, const LongPoint&);
bool		guArcBy3Pt(LongPointPtr, Coord*, Angle*, Angle*, const LongPoint&, const LongPoint&, const LongPoint&);
void		guGetHV45LPt(LongPointPtr constrainedPt, LongPointPtr startPt);
void		guGetHVLPt(LongPointPtr constrainedPt, LongPointPtr startPt);
void 		guSnapLPtToLength(LongPointPtr, LongPointPtr);
void		guBisectCurve(LongPointPtr, LongPointPtr, LongPointPtr);
/*--------------------------- rect utilities ---------------------------*/


LongRect	guPt2Rect(const LongPoint&, const LongPoint&);
LongRect	guUnionRect(const LongRect&, const LongRect&);
bool		guSectRect(const LongRect&, const LongRect&, LongRect *);
bool		guRectInRect(const LongRect&, const LongRect&);
bool		guEmptyRect(const LongRect&);
bool		guHasALenRect(const LongRect&);

void		guRotRect(const LongRect&, Angle, LongPoint*);
LongPoint	guPinRect(const LongRect&, const LongPoint&);

/* just change format, same zoom */

LongRect	guSRect2LRect(const CRect&);		/* format change only */
CRect		guLRect2SRect(const LongRect&);	/* format change only */

/* just change zoom, same format */

LongRect	guLRect2ZLRect(const LongRect&);
// LongRect	guZLRect2LRect(const LongRect&);

CRect		guSRect2ZSRect(const CRect&);
CRect		guZSRect2SRect(const CRect&);

/* change format and zoom - good for screen operations */



/*--------------------------- zoomed <-> long --------------------------*/


/* database insertion utilities */

LongRect	IntlFromScreen(const CRect&);
LongPoint	IntlFromScreen(const pointd);

/* database extraction utilities */
CRect		ScreenFromIntl(const LongRect&);
pointd		ScreenFromIntl(const LongPoint&);

/*--------------------------- unzoomed short <-> long ------------------*/

LongPoint	guQDPt2LongPt(const pointd);
pointd		guLongPt2QDPt(const LongPoint& aLongPt);

/*----------------------------------------------------------------------*/

double 	guLOrd2Double(Coord);
bool	guNearPtLnPt(const LongPoint&, const LongPoint&, const LongPoint&, LongPointPtr);

bool	guLongLnLnDblPt(const LongPoint&, const LongPoint&, const LongPoint& ,
											const LongPoint&, double*, double*);

bool	guAlmostEqualPt(const LongPoint&, const LongPoint&);

inline LongRect BoundRectOfCircle(LongPoint& centre, Coord radius)
{
	return LongRect(
		centre.x-radius,
		centre.y-radius,
		centre.x+radius,
		centre.y+radius);	
}
 
inline CRect BoundRectOfCircle(pointd& centre, short radius)
{
	CRect result;
	result.left=(LONG) (centre.x-radius);
	result.top=(LONG) (centre.y-radius);
	result.right=(LONG) (centre.x+radius);
	result.bottom=(LONG) (centre.y+radius);
	return result;
}
 
inline LongRect BoundRectOfEllipse(LongPoint& centre, LongPoint radii)
{
	return LongRect(
		centre.x-radii.x,
		centre.y-radii.y,
		centre.x+radii.x,
		centre.y + radii.y);
}
 
inline CRect BoundRectOfEllipse(pointd& centre, pointd radii)
{
	CRect result;
	result.left = (LONG) (centre.x - radii.x);
	result.top = (LONG) (centre.y - radii.y);
	result.right = (LONG) (centre.x + radii.x);
	result.bottom = (LONG) (centre.y + radii.y);
	return result;	
}

#endif