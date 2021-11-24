#include "stdafx.h"
#include "gu.h"
#include "phmath.h"
#include "DraftContext.h"

#include <math.h>

/*--------------------------- local constants --------------------------*/

#define	kPickRectMaxCoord		1000
#define	kMaxShort				32767.0
#define	kMinShort				-32768.0
#define	kMaxLong				2147483647.0		/* 0x7FFFFFFFL 		*/
#define	kMinLong				-2147483647.0		/* 0x80000001L			*/
#define	kMaxBoundary			536870912L			/* 0x20000000L			*/

#define	kDistRatio				0.39				/* approx dist ratio of bez ctrl pts */

#define	kSigDigits				6

#define	kMax3PtArc				393216000.0			/* 6000 as fixed.			*/
#define	kToSmall				0.0000000001		/* Limit on divisors.	*/

#define	kSplineDist				25560L				/* Distance ratio of spline ctrl pts.	*/

const Coord kAlmostSameCriteria = 400.0 / 65536.0;

static WrFixed 	sScaleH = FixedMinDelta;
static WrFixed 	sScaleV = FixedMinDelta;
static bool	sForDXF = false;

/////////////////////////////////////////////////////////////////////////////
/* greatest common denominator */
/*
	This binary gcd algorithm is faster than Euclid's traditional gcd algorithm.
	This algorithm was found in "Seminumerical Algorithms", Volume 2 of "The Art
	of Computer Programming", by Donald Knuth, p. 321.
*/

long gcd (
	register long u,
	register long v)
{
	register long t;
	register short k;


	if (u < 0) {
		u = -u;
	}
	if (v < 0) {
		v = -v;
	}

	k = 0;
	while (!(u & 1) && !(v & 1)) {
		++k;
		u >>= 1;
		v >>= 1;
	}

	t = (u & 1) ? -v : u;

	while (t) {
		while (!(t & 1)) {
			t >>= 1;
		}
		if (t > 0) {
			u = t;
		} else {
			v = -t;
		}
		t = u - v;
	}

	return((u) ? u << k : 1);
} /* gcd */

/////////////////////////////////////////////////////////////////////////////

Coord Pythag(const LongPoint& pt)
{
	return WrFixedFromRawDoubleTruncated(RawDoublePythag(pt)+0.5);
}

double RawDoublePythag(const LongPoint& pt)
{
	return sqrt(RawDoublePythagSqr(pt));
}

double RawDoublePythagSqr(const LongPoint& pt)
{
	double dh=pt.x.GetRawDouble();
	double dv=pt.y.GetRawDouble();
	return dh*dh+dv*dv;
}

bool guParallelLn(
	const LongPoint&		p1,					/* known line end point					*/
	const LongPoint&		p2,					/* known line end point					*/
	Coord					pWidth,				/* gap between both lines				*/
	short					pJustification,		/* kLeftJust, kCenterJust, kRightJust	*/
	LongPointPtr			pP1,				/* parallel line end point				*/
	LongPointPtr			pP2,				/* parallel line end point				*/
	LongPointPtr			pP3,				/* = p1 if left/right just, parallel 	*/
													/* line end point if center just	*/
	LongPointPtr			pP4)				/* = p2 if left/right just, parallel	*/
													/* line end point if center just	*/

/*
DESCRIPTION:		LongPoint version.
						Given two points defining a line, find the parallel
						line that lies pWidth from it.
						The given line must be to the pJustification of
						the parallel line.

AUTHOR:				Johny Sutardji		12-03-87

MODIFICATION:

RETURNS:				true if no error,
						false otherwise:
							-	out of bounds, pIPoint is forces to the limit,
*/

{
		LongPoint			delta;
		bool				result = true;
		 double	dx1, dy1, dx2, dy2;

	*pP3 = p1;										/* if not center just */
	*pP4 = p2;

	/* calculate delta from original point to parallel point */
	delta.x = pWidth;
	delta.y = pWidth;

	/* start from right justification if centered */
	if(p1.x==p2.x)						/* vertical line */
	{
		delta.y = FixedZero;
		if(p1.y < p2.y)						/* reverse it, pos y downward */
			delta.x = -delta.x;
	}
	else if(p1.y==p2.y)					/* horizontal line */
	{
		delta.x = FixedZero;
		if(p1.x > p2.x)
			delta.y = -delta.y;
	}
	else												/* angled line */
		{
		delta.x = FixedZero;
		delta = mtRotPoint(delta, mtATan2(p2 - p1));
		}

	if(pJustification==kCenterJust)
	{
		/* get left justified line */
		delta.x >>= 1;
		delta.y >>= 1;
		dx1 = p1.x.GetRawDouble()- delta.x.GetRawDouble();
		dy1 = p1.y.GetRawDouble()- delta.y.GetRawDouble();
		dx2 = p2.x.GetRawDouble()- delta.x.GetRawDouble();
		dy2 = p2.y.GetRawDouble()- delta.y.GetRawDouble();
		if(dx1 < kMinLong || dx1 > kMaxLong ||
			  dy1 < kMinLong || dy1 > kMaxLong ||
			  dx2 < kMinLong || dx2 > kMaxLong ||
			  dy2 < kMinLong || dy2 > kMaxLong)
			result = false;

		pP3->x.SetFromRawDoubleTruncated((dx1<kMinLong)? kMinLong :((dx1>kMaxLong)? kMaxLong : dx1));
		pP3->y.SetFromRawDoubleTruncated((dy1<kMinLong)? kMinLong :((dy1>kMaxLong)? kMaxLong : dy1));
		pP4->x.SetFromRawDoubleTruncated((dx2<kMinLong)? kMinLong :((dx2>kMaxLong)? kMaxLong : dx2));
		pP4->y.SetFromRawDoubleTruncated((dy2<kMinLong)? kMinLong :((dy2>kMaxLong)? kMaxLong : dy2));
	}
	else if(pJustification==kRightJust)
		delta = -delta;

	dx1 = p1.x.GetRawDouble()+ delta.x.GetRawDouble();
	dy1 = p1.y.GetRawDouble()+ delta.y.GetRawDouble();
	dx2 = p2.x.GetRawDouble()+ delta.x.GetRawDouble();
	dy2 = p2.y.GetRawDouble()+ delta.y.GetRawDouble();
	if(dx1 < kMinLong || dx1 > kMaxLong ||
		  dy1 < kMinLong || dy1 > kMaxLong ||
		  dx2 < kMinLong || dx2 > kMaxLong ||
		  dy2 < kMinLong || dy2 > kMaxLong)
		result = false;

	pP1->x.SetFromRawDoubleTruncated((dx1<kMinLong)? kMinLong :((dx1>kMaxLong)? kMaxLong : dx1));
	pP1->y.SetFromRawDoubleTruncated((dy1<kMinLong)? kMinLong :((dy1>kMaxLong)? kMaxLong : dy1));
	pP2->x.SetFromRawDoubleTruncated((dx2<kMinLong)? kMinLong :((dx2>kMaxLong)? kMaxLong : dx2));
	pP2->y.SetFromRawDoubleTruncated((dy2<kMinLong)? kMinLong :((dy2>kMaxLong)? kMaxLong : dy2));

	return(result);
}/* guParallelLn */

/*----------------------------------------------------------------------*/

bool guExtendLineToRect(pointd *p1, pointd *p2,	const CRect& clippingRect)
{
	double vDeltax, m, b, px1, py1, px2, py2;
	short count = 0;
	pointd p[2];

	CRect r = clippingRect;
	r.DeflateRect(-1, -1);

	if(p1->x==p2->x)
	{
		if(p1->x>r.left && p1->x<r.right)
		{
			p1->y=clippingRect.top;
			p2->y=clippingRect.bottom;
			return true;
		}
		else
			return false;
	}
	else if(p1->y==p2->y)
	{
		if(p1->y>r.top && p1->y<r.bottom)
		{
			p1->x=clippingRect.left;
			p2->x=clippingRect.right;
			return true;
		}
		else
			return false;
	}
	else
	{
		px1=p1->x;
		py1=p1->y;
		px2=p2->x;
		py2=p2->y;
		vDeltax=px2-px1;
		m=(py2 - py1)/vDeltax;
		b=py1-m*px1;
		
		px1=p[count].x=clippingRect.left;
		py1=m*px1+b;
		if(py1>(double)r.top && py1<(double)r.bottom)
			p[count++].y=(long)py1;
		px1=p[count].x=clippingRect.right;
		py1=m*px1+b;
		if(py1>(double)r.top && py1<(double)r.bottom)
			p[count++].y=(long)py1;
		if(count<2)
		{
			py1=p[count].y=clippingRect.top;
			px1=(py1-b)/m;
			if(px1>(double)r.left && px1<(double)r.right)
				p[count++].x=(long)px1;
            // SMK.SSA - May be one is Abs(p[0].y-p[1].y)<3 ???
			if(count==2 && Abs(p[0].x-p[1].x)<3 && Abs(p[0].x-p[1].x)<3)
				count--;
			if(count < 2)
			{
				py1 =p[count].y= clippingRect.bottom;
				px1 =(py1 - b)/ m;
				if(px1 >(double)r.left && px1 <(double)r.right)
					p[count++].x=(long)px1;
			}
		}
		
		if(count==2)
		{
			*p1=p[0];
			*p2=p[1];
			return true;
		}
		else
			return false;
	}
}

/*----------------------------------------------------------------------*/

void guBezSplinePts(
	const LongPoint&		p1,
	const LongPoint&		p2,
	const LongPoint&		p3,
	 LongPointPtr	p21,
	 LongPointPtr	p23)
	{
	Angle a1 = mtFastATan2(p1 - p2);
	if(a1==FixedZero)
		a1 = short(360);

	Angle a2 = mtFastATan2(p3 - p2);
	if(a2==FixedZero)
		a2 = short(360);

	if(p2==p1)
		{
		a1 = mtFixAngle(a2 + short(180));
		if(a1==FixedZero)
			a1 = short(360);
		}
	if(p2==p3)
		{
		a2 = mtFixAngle(a1 + short(180));
		if(a2==FixedZero)
			a2 = short(360);
		}

	Angle midAng = mtFixAngle((a1 + a2)>> 1);

	Angle a11 = mtFixAngle(midAng - short(90));
	if(a11==FixedZero)
		a11 = short(360);
	Angle a22 = mtFixAngle(midAng + short(90));
	if(a22==FixedZero)
		a22 = short(360);

	Angle delAng1 = Abs(a11 - a1);
	if(delAng1 > short(180))
		delAng1 = short(360)- delAng1;
	Angle delAng2 = Abs(a22-a1);
	if(delAng2 > short(180))
		delAng2 = short(360)- delAng2;
	if(delAng1 > delAng2)
		{
		midAng = a11;
		a11 = a22;
		a22 = midAng;
		}

	a11 = mtFixAngle(a11);
	a22 = mtFixAngle(a22);

	double d1 =(p1.x - p2.x).GetRawDouble();
	double tmp =(p1.y - p2.y).GetRawDouble();
	d1 = sqrt(d1*d1 + tmp*tmp)* kDistRatio + 0.5; // Could use Pythag ??
	double d2 =(p3.x - p2.x).GetRawDouble();
	tmp =(p3.y - p2.y).GetRawDouble();
	d2 = sqrt(d2*d2 + tmp*tmp)* kDistRatio + 0.5; // Could use Pythag ??

	LongPoint delta;
	
	delta.x.SetFromRawDoubleTruncated(d1);
	delta.y = FixedZero;
	delta = mtRotPoint(delta , a11);
	*p21 = p2 + delta;

	delta.x.SetFromRawDoubleTruncated(d2);
	delta.y = FixedZero;
	delta = mtRotPoint(delta , a22);
	*p23 = p2 + delta;

	if(mtFastATan2(p1 - p2)== mtFastATan2(p3 - p2))
		{
		delta = *p21;
		*p21 = *p23;
		*p23 = delta;
		}
}/* guBezSplinePts */

/*----------------------------------------------------------------------*/

/* DON'T REMOVE THIS CODE. */
#if 0
guBezSplinePts(*p1, *p2, *p3, p21, p23)
 LongPointPtr	p1, p2, p3, p21, p23;

{
	WrFixed dist1 = Pythag(*p2 - *p1)* kSplineDist;
	WrFixed dist2 = Pythag(*p2 - *p3)* kSplineDist;
	LongPoint unitVect =(*p2 - *p1)* dist2 +(*p3 - *p2)* dist1;

	if(unitVect.IsZero())	{
		if(dist1 != 0L)	{
			unitVect.x = p2->y - p1->y;
			unitVect.y = p1->x - p2->x;
		}
		else if(dist2 != 0L)	{
			unitVect.x = p2->y - p3->y;
			unitVect.y = p3->x - p2->x;
		}
		else
			unitVect.x = 65536L;
	}
	WrFixed dist = Pythag(unitVect);
	unitVect /= dist;

	*p21 = *p2 - unitVect * dist1;
	*p23 = *p2 + unitVect * dist2;
}/* guBezSplinePts */

/*----------------------------------------------------------------------*/
#endif

/*----------------------------------------------------------------------*/

/*
	Given an offset within srcRect, guMapOffset maps it to a
	similarly located offset within dstRect(that is, to
	where it would fall if it were part of a drawing being
	expanded or shrunk to fit dstRect).
	The result is returned in offset.
	srcRect and dstRect may overlap, and offset need not
	actually be within srcRect.

	true if no error,
	false otherwise: out of bounds.
*/

bool guMapOffset(LongPointPtr ptPtr, const LongRect& srcRect, const LongRect& dstRect)
{
	Coord scaleH, scaleV;

	if(!sForDXF)
		{
		LongPoint srcSize = srcRect.GetSize();
		LongPoint dstSize = dstRect.GetSize();
		if(srcSize.x==FixedZero || srcSize.y==FixedZero 
		|| dstSize.x==FixedZero || dstSize.y==FixedZero)
			return(false);

		scaleH = dstSize.x / srcSize.x;
		scaleV = dstSize.y / srcSize.y;
		}
	else
	{
		scaleH = sScaleH;
		scaleV = sScaleV;
	}

	ptPtr->x *= scaleH;
	ptPtr->y *= scaleV;
	return true;
	}/* guMapOffset */

/*----------------------------------------------------------------------*/

/*
	Given a point within srcRect, guMapPt maps it to a
	similarly located point within dstRect(that is, to
	where it would fall if it were part of a drawing being
	expanded or shrunk to fit dstRect).
	The result is returned in pt.
	A corner point of srcRect would be mapped to the
	corresponding corner point of dstRect, and the
	center of srcRect to the center of dstRect.
	srcRect and dstRect may overlap, and pt need not
	actually be within srcRect.

	RETURNS true if no error, false otherwise: out of bounds.
*/

LongPoint	guMapPt(
	const LongPoint&	pt ,
	const LongRect&		srcRect ,
	const LongRect&		dstRect)
	{
	if(!sForDXF)
		{
		LongPoint offset = pt - srcRect [ topLeft ];
		if(!guMapOffset(&offset, srcRect, dstRect))
			return pt;

		return offset + dstRect [ topLeft ];
		}

	LongPoint result = pt;
	result.x *= sScaleH;
	result.y *= sScaleV;
	return result;
	}/* guMapPt */

/*----------------------------------------------------------------------*/

void MapPt(pointd* pt,const CRect& srcRect,const CRect& dstRect)
{
	pointd offset=*pt-__pointf(srcRect.TopLeft());
	offset.x=(offset.x*dstRect.Width())/srcRect.Width();
	offset.y=(offset.y*dstRect.Height())/srcRect.Height();
	*pt=__pointd(dstRect.TopLeft())+offset;
}
/*----------------------------------------------------------------------*/

/*
DESCRIPTION:		Similar to guMapOffset.
						This one make sure it maps the pt to the average
						propotional rect.

RETURNS:				true if no error,
						false otherwise: out of bounds.
*/


bool	guPropMapOffset(
	 LongPointPtr	ptPtr,
	const LongRect&	srcRect,
	const LongRect&	dstRect)
	{
	 Coord		scaleH, scaleV;

	if(!sForDXF)
		{
		LongPoint srcSize = srcRect.GetSize();
		LongPoint dstSize = dstRect.GetSize();
		if(srcSize.x==FixedZero || srcSize.y==FixedZero 
		|| dstSize.x==FixedZero || dstSize.y==FixedZero)
			return(false);

		scaleH = dstSize.x / srcSize.x;
		scaleV = dstSize.y / srcSize.y;

		if(scaleH==FixedOne)
			scaleH = Abs(scaleV); // FISHY is this ever executed except when scaleH is already equal to scaleV ?
		if(scaleV==FixedOne)
			scaleV = Abs(scaleH); // FISHY is this ever executed except when scaleV is already equal to scaleH ?
		}
	else
		{
		scaleH = sScaleH;
		scaleV = sScaleV;
		}

	ptPtr->x *= scaleH;
	ptPtr->y *= scaleV;

	return true;
	}/* guPropMapOffset */

/*----------------------------------------------------------------------*/

/*
DESCRIPTION:		Similar to guMapPt.
						This one make sure it maps the pt to the average
						propotional rect.

RETURNS:				true if no error,
						false otherwise: out of bounds.
*/

LongPoint	guPropMapPt(const LongPoint& pt, const LongRect& srcRect, const LongRect& dstRect)
{
	if(!sForDXF)
	{
		LongPoint offset=pt-srcRect[topLeft];
		VERIFY(guPropMapOffset(&offset, srcRect, dstRect));
		return offset+dstRect[topLeft];
	}
	LongPoint result=pt;
	result.x*=sScaleH;
	result.y*=sScaleV;
	return result;
}


/*----------------------------------------------------------------------*/
/*
DESCRIPTION:		Given an arbitrary point and a line, find the normal
						base point, which is the nearest point on the given
						line to the given arbitrary point.

						CAUTION:
							This function does not check wheather the
							intersection point is somewhere along the given
							line.

						Note:
							To maximize the speed, this function does not call
							any available functions at all.

RETURNS:				true if no error,
						false otherwise(out of bounds).
*/

bool	guNearPtLnPt(
 	const LongPoint&	pAPoint,				/* arbitrary point		 					*/
 	const LongPoint&	pPoint1,				/* line end point								*/
	const LongPoint&	pPoint2,				/* line end point								*/
	LongPointPtr		pIPoint)				/* normal base point							*/
{
		double				pax, pay,
								plx1, ply1,
								plx2, ply2,
								pix, piy;
		 double	vm1, vm2, vb1, vb2;


	if(pPoint1.x==pPoint2.x)							/* check vertical line */
	{
		pIPoint->x = pPoint1.x;
		pIPoint->y = pAPoint.y;
		return(true);
	}

	if(pPoint1.y==pPoint2.y)						/* check horizontal line */
	{
		pIPoint->x = pAPoint.x;
		pIPoint->y = pPoint1.y;
		return(true);
	}

	pax = pAPoint.x.GetRawDouble();
	pay = pAPoint.y.GetRawDouble(),
	plx1 = pPoint1.x.GetRawDouble();
	ply1 = pPoint1.y.GetRawDouble(),
	plx2 = pPoint2.x.GetRawDouble();
	ply2 = pPoint2.y.GetRawDouble();

	vm1 =(ply2 - ply1)/(plx2 - plx1);			/* line equation */
	vb1 = ply1 - vm1 * plx1;

	vm2 = -1.0 / vm1;											/* perpendicular line */
	vb2 = pay - vm2 * pax;

	pix =(vb1 - vb2)/(vm2 - vm1);						/* intersection point */
	piy = vm2 * pix + vb2;
	if(pix >= kMinLong && pix <= kMaxLong &&
		  piy >= kMinLong && piy <= kMaxLong)
	{
		pIPoint->x.SetFromRawDoubleTruncated(pix);
		pIPoint->y.SetFromRawDoubleTruncated(piy);
		return(true);
	}
	else
		return(false);
}/* guNearPtLnPt */


/*----------------------------------------------------------------------*/
bool guLongLnLnDblPt(
	const LongPoint&	pPoint1,			/* line end point						*/
	const LongPoint&	pPoint2,			/* line end point						*/
	const LongPoint&	pPoint3,			/* other line end point					*/
	const LongPoint&	pPoint4,			/* other line end point					*/
	double		*pix,				/* intersection point					*/
	double		*piy)				/* intersection point					*/

/*
DESCRIPTION:		Double version.
						Given 2 set of line points, find the intersection
						point of both lines.

						CAUTION:
							This function does not check wheather the
							intersection point is somewhere along the given
							line.
							And it does NOT check if the intersection point
							is valid(within the boundary)or not.

						Note:
							To maximize the speed, this function does not call
							any available functions at all.

AUTHOR:				Johny Sutardji		05-12-88

MODIFICATION:

RETURNS:				true if no error,
						false otherwise: no intersection.
*/

{
		double				plx1, ply1,
								plx2, ply2,
								plx3, ply3,
								plx4, ply4;
		 double	vm1 = 12345.0, vm2 = 12345.0, vb1, vb2;
		bool				vVertical1 = false,
								vVertical2 = false;

	if(pPoint1.x==pPoint2.x)						/* check vertical line1 */
		vVertical1 = true;
	else
	{
		plx1 = pPoint1.x.GetRawDouble();
		ply1 = pPoint1.y.GetRawDouble();
		plx2 = pPoint2.x.GetRawDouble();
		ply2 = pPoint2.y.GetRawDouble();
		vm1 =(ply2 - ply1)/(plx2 - plx1);		/* line1 equation */
		vb1 = ply1 - vm1 * plx1;
	}

	if(pPoint3.x==pPoint4.x)						/* check vertical line2 */
		vVertical2 = true;
	else
	{
		plx3 = pPoint3.x.GetRawDouble();
		ply3 = pPoint3.y.GetRawDouble(),
		plx4 = pPoint4.x.GetRawDouble();
		ply4 = pPoint4.y.GetRawDouble();
		vm2 =(ply4 - ply3)/(plx4 - plx3);		/* line2 equation */
		vb2 = ply3 - vm2 * plx3;
	}

	if((vVertical1 && vVertical2)||(vm1==vm2))
		return(false);

	if(vVertical1)
	{
		*pix = pPoint1.x.GetRawDouble();
		*piy = vm2 * *pix + vb2;
	}
	else if(vVertical2)
	{
		*pix = pPoint3.x.GetRawDouble();
		*piy = vm1 * *pix + vb1;
	}
	else
	{
		*pix =(vb1 - vb2)/(vm2 - vm1);					/* intersection point */
		*piy = vm2 * *pix + vb2;
	}

	return(true);
}/* guLongLnLnDblPt */

/*----------------------------------------------------------------------*/


bool guLongLnLnPt(
	const LongPoint&	pPoint1,			/* line end point					*/
	const LongPoint&	pPoint2,			/* line end point					*/
	const LongPoint&	pPoint3,			/* other line end point				*/
	const LongPoint&	pPoint4,			/* other line end point				*/
	LongPointPtr	pIPoint)			/* intersection point				*/

/*
DESCRIPTION:		LongPoint version.
						Given 2 set of line points, find the intersection
						point of both lines.

						CAUTION:
							This function does not check wheather the
							intersection point is somewhere along the given
							line.

						Note:
							To maximize the speed, this function does not call
							any available functions at all.


RETURNS:				true if no error,
						false otherwise:
							out of bounds or no intersection, pIPoint is pPoint2.
*/

{
		double				pix, piy;
		bool				isValid;

	if((isValid = guLongLnLnDblPt(pPoint1, pPoint2, pPoint3, pPoint4, &pix, &piy))!= 0)
		if((isValid =(pix >= kMinLong && pix <= kMaxLong && piy >= kMinLong && piy <= kMaxLong))!= 0)
		{
			pIPoint->x.SetFromRawDoubleTruncated(pix);
			pIPoint->y.SetFromRawDoubleTruncated(piy);
		}

	if(!isValid)
		*pIPoint = pPoint2;

	return(isValid);
}/* guLongLnLnPt */

/* --------------------------------------------------------------------------- */

bool	guPtAtLine(
	pointd	pPoint,			/* in view coordinate							*/
	pointd	pLinePoint1,
	pointd	pLinePoint2,
	short	pPenWidth,		/* in view coordinate							*/
	short	pJustification)/* kInsideLine, kCenterLine, kOutsideLine	*/

/*
DESCRIPTION:		Check if the given point is at the specified line.

						Note:
							kInsideLine means also RIGHT justification.
							kOutsideLine means also LEFT justification.

AUTHOR:				Johny Sutardji		09-06-87

MODIFICATION:

RETURNS:				true if local point is at the line,
						false otherwise.
*/

{
		pointd				vOffPoint,
							vSelPoint;
		short				vMaxDist,
							vPickDist;
		CRect				vLineRect;
		CRect				vPickRect;
		short				deltah, deltav;

	if((pPenWidth >>= 1)> 1 &&
		  pJustification != kCenterLine)		/* left/right justification */
	{
		deltah = deltav = pPenWidth;

		if(pLinePoint2.x==pLinePoint1.x)
			deltav = 0;
		else if(pJustification==kInsideLine)		/* right just */
		{
			if(pLinePoint2.x < pLinePoint1.x)
				deltav = -deltav;
		}
		else														/* left just */
			if(pLinePoint2.x > pLinePoint1.x)
				deltav = -deltav;

		if(pLinePoint2.y==pLinePoint1.y)
			deltah = 0;
		else if(pJustification==kInsideLine)		/* right just */
		{
			if(pLinePoint2.y > pLinePoint1.y)
				deltah = -deltah;
		}
		else														/* left just */
			if(pLinePoint2.y < pLinePoint1.y)
				deltah = -deltah;

		pLinePoint1.x += deltah;
		pLinePoint1.y += deltav;
		pLinePoint2.x += deltah;
		pLinePoint2.y += deltav;

	}

	pPenWidth =(pPenWidth > kSelectOffset)? pPenWidth : kSelectOffset;

	vLineRect.SetRect(__point(pLinePoint1),__point(pLinePoint2));
	vLineRect.NormalizeRect();
	vLineRect.DeflateRect(-pPenWidth-1, -pPenWidth-1);

	if(!vLineRect.PtInRect(__point(pPoint)))	return false;
	if(pLinePoint1.x==pLinePoint2.x || pLinePoint1.y==pLinePoint2.y) return true;
	else
	{
		vPickRect.top = vPickRect.left = 0;
		vPickRect.bottom = vPickRect.right =  kPickRectMaxCoord;

		vSelPoint = pPoint;

		/* calculate offset */
		vOffPoint.x = vLineRect.left +(pPenWidth << 1);
		vOffPoint.y = vLineRect.top +(pPenWidth << 1);
		MapPt(&vOffPoint, vLineRect, vPickRect);
		vMaxDist =short((vOffPoint.x >= vOffPoint.y)? vOffPoint.x : vOffPoint.y);

		/* check selection point */
		MapPt(&vSelPoint, vLineRect, vPickRect);
		if(pLinePoint1.x > pLinePoint2.x)
		{
			if(pLinePoint1.y > pLinePoint2.y)
				vPickDist = short(vSelPoint.x - vSelPoint.y);
			else
				vPickDist = short(vSelPoint.x - kPickRectMaxCoord + vSelPoint.y);
		}
		else
			if(pLinePoint1.y > pLinePoint2.y)
				vPickDist = short(vSelPoint.x - kPickRectMaxCoord + vSelPoint.y);
			else
				vPickDist = short(vSelPoint.x - vSelPoint.y);
	}

	if(vPickDist < 0)
		vPickDist = -vPickDist;

	return(vPickDist <= vMaxDist);
}/* guPtAtLine */

/*----------------------------------------------------------------------*/

bool	guPtAtCirc(
	pointd			pPoint,				/* point to be checked, in view coordinate*/
	const pointd&	pCenterPt,
	short			pRadius,			/* in view coordinate							*/
	short			pPenWidth,			/* in view coordinate							*/
	Alignment		pJustification,		/* kInsideLine, kCenterLine, kOutsideLine	*/
	bool			pHasFill)			/* true or false */

/*
DESCRIPTION:		Check if the given point is at the specified circle.

						Note:
							For checking the arc, upon true returned, check if
							the angle formed by the center and the local point
							is within the arc angle.

AUTHOR:				Johny Sutardji		10-01-87

MODIFICATION:

RETURNS:				true if local point is at the circle,
						false otherwise.
*/

{
		 long	deltaH, deltaV, distance;
		short				inOffset, outOffset;


	deltaH =(long)pCenterPt.x -(long)pPoint.x;
	deltaV =(long)pCenterPt.y -(long)pPoint.y;
	distance = deltaH * deltaH + deltaV * deltaV;

	inOffset = 0;
	outOffset = pPenWidth;
	if(pJustification==kCenterLine)
	{
		outOffset++;
		outOffset >>= 1;
		inOffset = outOffset;
	}
	else  if(pJustification==kInsideLine)
	{
		inOffset = outOffset;
		outOffset = 0;
	}

	if(inOffset < kSelectOffset)
		inOffset = kSelectOffset;
	if(outOffset < kSelectOffset)
		outOffset = kSelectOffset;

	if(pHasFill)
		return(distance <(((long)pRadius+outOffset)*((long)pRadius+outOffset)));
	else
	{
		if(pRadius < inOffset)
			pRadius = inOffset;
		deltaH =(long)pRadius - inOffset;
		deltaV =(long)pRadius + outOffset;
		return((deltaH*deltaH)<= distance &&
					distance <=(deltaV*deltaV));
	}
}/* guPtAtCirc */

/*----------------------------------------------------------------------*/

bool	guPtAtEllip(
	pointd			pPoint,				/* point to be checked, in view coordinate*/
	const pointd&	pCenterPt,
	short			pHRad,				/* in view coordinate							*/
	short			pVRad,				/* in view coordinate							*/
	Angle			pRot,					/* ellipse rotation								*/
	short			pPenWidth,			/* in view coordinate							*/
	Alignment	pJustification,	/* kInsideLine, kCenterLine, kOutsideLine	*/
	bool		pHasFill)			/* true or false */

/*
DESCRIPTION:		Check if the given point is at the specified ellipse.

						Note:
							For checking the elliptical arc, upon true returned,
							check if the angle formed by the center and the local
							point is within the elliptical arc angle.

AUTHOR:				Johny Sutardji		04-20-88

MODIFICATION:

RETURNS:				true if local point is at the ellipse,
						false otherwise.
*/

{
		CRect					ellipRect, circRect;
		 short		circRad;
		pointd					offset;

	if(pRot != FixedZero)
		{
		TSinCos sinCos(mtFixAngle(-pRot));
		pPoint = __pointf(mtRotPointV(__point(pPoint), sinCos , __point(pCenterPt)));
		}

	ellipRect.SetRect(pCenterPt.x-pHRad, pCenterPt.y-pVRad,
				 pCenterPt.x+pHRad, pCenterPt.y+pVRad);

	circRad =(pHRad < pVRad)? pHRad : pVRad;
	if(circRad)
	{
		circRect.SetRect(pCenterPt.x-circRad, pCenterPt.y-circRad,
					 pCenterPt.x+circRad, pCenterPt.y+circRad);

		MapPt(&pPoint, ellipRect, circRect);

		offset.x = pCenterPt.x + pPenWidth;
		offset.y = pCenterPt.y + pPenWidth;
		MapPt(&offset, ellipRect, circRect);
		offset.x -= pCenterPt.x;
		offset.y -= pCenterPt.y;
      pPenWidth = short((offset.x + offset.y + (fabs(offset.x - offset.y)/2.0f)) /2.0f);

		return(guPtAtCirc(pPoint, pCenterPt, circRad, pPenWidth,
									pJustification, pHasFill));
	}
	else
	{
		ellipRect.InflateRect(1,1);
		return ellipRect.PtInRect(__point(pPoint))?true:false;
	}
}/* guPtAtEllip */

/*----------------------------------------------------------------------*/

bool guPtInPoly(
	short					what,
	LongPointPtr			pPoint,				/* point to be checked						*/
	 LongPointPtr	p1,		/* polygon line segment						*/
	 LongPointPtr	p2)

/*
DESCRIPTION:		Check if the given point is inside the given polygon,
						which is constructed by the given line segments.

						what =
							kBeginPtInPoly when passing the first segment;
							kEndPtInPoly when passing the last segment.
							kMorePtInPoly when passing the rest of the segments;

						kBeginPtInPoly must be passed for each new polygon,
						or it will return random result otherwise.
						kEndPtInPoly must be passed to obtain the 'real'
						result.

						This function will automatically close the polygon
						if necessary.

AUTHOR:				Johny Sutardji		4-12-87

MODIFICATION:

RETURNS:				true if local point is in the polygon,
						false otherwise.
						NOTE:
							Only obtain the result when passing kEndPtInPoly.
*/

{
		static LongPoint				checkPt,
											firstPt,
											secondPt,
											prevPt;
		static short					count;
		static bool					atLeastOneAtTheRight;

		bool							isSpecialCase;
		 bool				gotIt;
		 Coord					iH;
		LongRect							longRect;
		 double				vm,
											d1h, d1v, d2h, d2v;

	if(what==kBeginPtInPoly)
	{
		checkPt = *pPoint;
		firstPt = prevPt = *p1;
		secondPt = *p2;
		count = 0;
		atLeastOneAtTheRight = false;
		return(false);
	}

	if(p1->x != p2->x || p1->y != p2->y)
	{
		longRect = guPt2Rect(*p1 , *p2);

		/* check for visible intersection */
		if((gotIt =(checkPt.y >= longRect.top && checkPt.y <= longRect.bottom))!= 0)
		{
			if(p1->x==p2->x)						/* check vertical */
				iH = p1->x;
			else if(p1->y==p2->y)					/* check horizontal */
				iH = p1->x;
			else												/* line equation */
			{
				d1h = p1->x.GetRawDouble();
				d1v = p1->y.GetRawDouble(),
				d2h = p2->x.GetRawDouble();
				d2v = p2->y.GetRawDouble();
				vm =(d2v - d1v)/(d2h - d1h);

				iH.SetFromRawDoubleTruncated((checkPt.y.GetRawDouble()-(d1v - vm * d1h))/ vm);
			}

			longRect = guPt2Rect(prevPt , *p2);	/* reuse the longRect */
			if((isSpecialCase =
							(iH==p1->x && checkPt.y==p1->y &&
							 p1->y >= longRect.top && p1->y <= longRect.bottom))!= 0)	/* not maxmin */
				gotIt = !((p1->y != p2->y && p1->y != prevPt.y)||
							 (p1->y==p2->y && p1->y < prevPt.y));

			if(gotIt)
			{
				if(iH <= checkPt.x)
				{
					count++;

					if(isSpecialCase &&			/* prev. horizontal */
						  p1->y==prevPt.y &&
						  p1->y >= p2->y)			/* upward */
						count++;
				}
				else if(!atLeastOneAtTheRight)
				{
					count++;
					atLeastOneAtTheRight = true;
				}

			}/* if gotIt */

		}/* if intersects */

		prevPt = *p1;								/* for minima-maxima */
	}

	if(what==kEndPtInPoly)
	{
		/* close it if necessary */
		/* it may be an extra effort but worth it, */
		/* caller doesn't need to bother with closed or opened poly */
		guPtInPoly(kMorePtInPoly, pPoint, p2, &firstPt);

		/* check on the first segment which is skipped at the very beginning */
		guPtInPoly(kMorePtInPoly, pPoint, &firstPt, &secondPt);

		return(!(count % 2)&& atLeastOneAtTheRight);
	}

	return(false);
}/* guPtInPoly */

/*----------------------------------------------------------------------*/

/* Given Point1, Point2 and Point3 find the radius and the center of
	the circle passing through these three points.  Check if the center
	lies within the allowed boundaries(else return false).           	*/

bool guCircBy3Pt(
	LongPoint		*pCenter,
	Coord			*pRadius,
	const LongPoint& Point1,
	const LongPoint& Point2,
	const LongPoint& Point3)
{
		static double		px1, py1, px2, py2,
								vcx1, vcy1,
								vm1, vb1;
		static bool		vVertical1;
		double				px3, py3,
								vcx2, vcy2;
		 double	vm2, vb2;
		bool				vVertical2;
		double				vCx, vCy, vRadius;

	/* Check if two points coincide. */
	if((Point1.x==Point2.x && Point1.y==Point2.y)||
		 (Point1.x==Point3.x && Point1.y==Point3.y)||
		 (Point2.x==Point3.x && Point2.y==Point3.y))
		return(false);

	/* Check if the static points(1&2)have moved, to gain sketching speed.	*/
	if(px1 != Point1.x.GetRawDouble()|| py1 != Point1.y.GetRawDouble()||
		  px2 != Point2.x.GetRawDouble()|| py2 != Point2.y.GetRawDouble())
	{
		px1 = Point1.x.GetRawDouble();
		py1 = Point1.y.GetRawDouble();
		px2 = Point2.x.GetRawDouble();
		py2 = Point2.y.GetRawDouble();

		/* Calculate the line perpendicular to the segment pt1-pt2 passing
		through the mid point of that segment. This line passes through the
		center of the circle. vm1 is the slope, vb1 is the vertical intercept.	*/
		vcx1 =(px1 + px2)/ 2.0;
		vcy1 =(py1 + py2)/ 2.0;
		if(Point1.y==Point2.y)
			vVertical1 = true;
		else	{
			vVertical1 = false;
			vm1 =(px1 - px2)/(py2 - py1);
			vb1 = vcy1 - vm1 * vcx1;
		}
	}

	/* Calculate the line perpendicular to the segment pt2-pt3 passing
	through the mid point of that segment. This line passes through the
	center of the circle. vm2 is the slope, vb2 is the vertical intercept.		*/
	px3 = Point3.x.GetRawDouble();
	py3 = Point3.y.GetRawDouble();
	vcx2 =(px2 + px3)/ 2.0;
	vcy2 =(py2 + py3)/ 2.0;

	if(Point2.y==Point3.y)
		vVertical2 = true;
	else	{
		vVertical2 = false;
		vm2 =(px2 - px3)/(py3 - py2);
		vb2 = vcy2 - vm2 * vcx2;
	}

	/* The intersection of the 2 lines is now the center of the circle. */
	if((vVertical1 &&  vVertical2)||
		 (!vVertical1 && !vVertical2 && Abs(vm1 - vm2)< kToSmall))
		return(false);
	if(vVertical1)	{
		vCx = vcx1;
		vCy = vm2 * vCx + vb2;
	}
	else if(vVertical2)	{
		vCx = vcx2;
		vCy = vm1 * vCx + vb1;
	}
	else	{
		vCx =(vb2 - vb1)/(vm1 - vm2);
		vCy = vm1 * vCx + vb1;
	}

	if(Abs(vCx)> kMax3PtArc || Abs(vCy)> kMax3PtArc)	return(false);

	pCenter->x.SetFromRawDoubleTruncated(vCx +(vCx < 0.0 ? - 0.5 : 0.5));
	pCenter->y.SetFromRawDoubleTruncated(vCy +(vCy < 0.0 ? - 0.5 : 0.5));

	vRadius = sqrt((double)((vCx-px1)*(vCx-px1)+(vCy-py1)*(vCy-py1)));
	if(vRadius > kMax3PtArc)	return(false);
	pRadius->SetFromRawDoubleTruncated(vRadius + 0.5);

	return(true);
}/* guCircBy3Pt */

/*----------------------------------------------------------------------*/

/* Given pPoint1, pPoint2 and pPoint3 find the center, the radius and the
starting and ending angles of the arc passing through these points. */

bool guArcBy3Pt(
	LongPoint		*pCenter,
	Coord			*pRadius,
	Angle			*pStartAng,
	Angle			*pEndAng,
	const LongPoint& Point1,
	const LongPoint& Point2,
	const LongPoint& Point3)
{
		Angle		midAng;

	/* Find the radius and the center and check if out of range. */
	if(! guCircBy3Pt(pCenter, pRadius, Point1, Point2, Point3))
		return(false);

	/* Get the arc angles. */
	*pStartAng = mtAngle(*pCenter , Point1);
	*pEndAng = mtAngle(*pCenter , Point3);
	midAng = mtAngle(*pCenter , Point2);

	/* Swap the angles according to the orientation. */
	flipArc(pStartAng, &midAng, pEndAng);

	return(true);
}/* guArcBy3Pt */

/*----------------------------------------------------------------------*/

/* Swap the arc angles according to the orientation of the arc. This function
	can be used for any arcs where the start, end and mid angles are known.	 */

void flipArc(
	Angle		*pStart,
	Angle		*pMid,
	Angle		*pEnd)
{
	if(*pStart < *pEnd  &&(*pMid < *pStart || *pMid > *pEnd)
										|| (*pMid < *pStart && *pMid > *pEnd))	{
		*pMid = *pStart;
		*pStart = *pEnd;
		*pEnd = *pMid;
	}
}/* flipArc */

/*----------------------------------------------------------------------*/

void guGetHV45LPt(
	 LongPointPtr		constrainedPt,
	 LongPointPtr		startPt)

/*
Description:		LongPoint version.
						Called by LineSketcher.c, PolygonSketcher.c,
						ParallelPolySketcher.c, and other Sketchers.
						guGetHV45Pt will constrain the line being sketched
						to 45¡ increments.
Author:				Chuck Soper			9-10-87
Modified:			Johny Sutardji		6/22/88
*/

{
	 Coord		deltaX, deltaY, max;

	deltaX = constrainedPt->x - startPt->x;
	deltaY = constrainedPt->y - startPt->y;
	max =(Abs(deltaX)> Abs(deltaY))? deltaX : deltaY;
	max = max >> 1;


	/* 1st Quad - 45¡  */
	if((constrainedPt->x >(startPt->x)+ Abs(max))&&
		(constrainedPt->y <(startPt->y)- Abs(max))){
			constrainedPt->x = startPt->x + Abs(max << 1);
			constrainedPt->y = startPt->y - Abs(max << 1);
	}
	/* 2nd Quad - 135¡ */
	else if((constrainedPt->x <(startPt->x)+ max)&&
		(constrainedPt->y <(startPt->y)+ max)){
			constrainedPt->x = startPt->x +(max << 1);
			constrainedPt->y = startPt->y +(max << 1);
	}
	/* 3rd Quad - 225¡ */
	else if((constrainedPt->x <(startPt->x)- Abs(max))&&
		(constrainedPt->y >(startPt->y)+ Abs(max))){
			constrainedPt->x = startPt->x - Abs(max << 1);
			constrainedPt->y = startPt->y + Abs(max << 1);
	}
	/* 4th Quad - 315¡ */
	else if((constrainedPt->x >(startPt->x)+ max)&&
		(constrainedPt->y >(startPt->y)+ max)){
			constrainedPt->x = startPt->x +(max << 1);
			constrainedPt->y = startPt->y +(max << 1);
	}

 	/* Vertical - 90¡ or 270¡ */
	else if(Abs(startPt->x - constrainedPt->x)<
		 Abs(startPt->y - constrainedPt->y))
			constrainedPt->x = startPt->x;

	/* Horizontal - 0¡ or 180¡ */
	else
			constrainedPt->y = startPt->y;
}/* guGetHV45LPt */

/*----------------------------------------------------------------------*/


void guGetHVLPt(
	 LongPointPtr		constrainedPt,
	 LongPointPtr		startPt)
	{
	 Coord deltaX = Abs(constrainedPt->x - startPt->x);
	 Coord deltaY = Abs(constrainedPt->y - startPt->y);

 	/* Vertical - 90¡ or 270¡ */
	if(deltaY > deltaX)
		constrainedPt->x = startPt->x;
	/* Horizontal - 0¡ or 180¡ */
	else
		constrainedPt->y = startPt->y;
	}/* guGetHVLPt */

/*----------------------------------------------------------------------*/
//
//	guSnapLPtToLength - snap a long point to a length that is an even multiple of the grid snap
//
/*----------------------------------------------------------------------*/
void guSnapLPtToLength(
	 LongPointPtr startPtPtr,
	 LongPointPtr endPtPtr)
{
/*	Snap snap =(*(StdViewHdl)gOpViewHdl)->fSnap;
 	if(!snap.fSnapRatio.fN)
		return;*/
	Angle angle=mtAngle(*startPtPtr,*endPtPtr);
	Coord dist=Pythag(*startPtPtr-*endPtPtr);
//	snapLength(&dist,snap);
	endPtPtr->x.SetFromRawDoubleTruncated(startPtPtr->x.GetRawDouble()+(double)(mtCos(angle)* dist.GetRawDouble())+0.5);
	endPtPtr->y.SetFromRawDoubleTruncated(startPtPtr->y.GetRawDouble()+(double)(mtSin(angle)* dist.GetRawDouble())+0.5);
}


/*----------------------------------------------------------------------*/

/*
	guBisectCurve takes the bezier control points in curve and returns the two
	halves in half1 and half2.  The curve is bisected exactly and is not
	distorted in any way.
*/

void guBisectCurve(LongPoint curve[4], LongPoint half1[4], LongPoint half2[4])
{
	LongPoint	temp;

	half1[0]=curve[0];
	half2[3]=curve[3];
	half1[1].x=(curve[0].x+curve[1].x)>>1;
	half1[1].y=(curve[0].y+curve[1].y)>>1;
	half2[2].x=(curve[2].x+curve[3].x)>>1;
	half2[2].y=(curve[2].y+curve[3].y)>>1;
	temp.x=(curve[1].x+curve[2].x)>>2;
	temp.y=(curve[1].y+curve[2].y)>>2;
	half1[2].x=(half1[1].x >>1)+ temp.x;
	half1[2].y=(half1[1].y >>1)+ temp.y;
	half2[1].x=(half2[2].x >>1)+ temp.x;
	half2[1].y=(half2[2].y >>1)+ temp.y;
	half1[3].x=(half1[2].x+half2[1].x)>>1;
	half1[3].y=(half1[2].y+half2[1].y)>>1;
	half2[0]=half1[3];
}

/*----------------------------------------------------------------------*/
/* Given 2 long points, set long type rectangle */
LongRect guPt2Rect(const LongPoint&	p1, const LongPoint& p2)
{
	LongRect r;
	r.SetToBound(p1,p2);
	return r;
}


/*----------------------------------------------------------*/
/* accepts rects which are lines, but rejects points and
   negative area rects */
LongRect guUnionRect(const LongRect& rect1,	const LongRect&	rect2)
{
	if(!guHasALenRect(rect1))
		return rect2;
	if(!guHasALenRect(rect2))
		return rect1;
	LongRect unionRect;
	unionRect.top=Min(rect1.top,rect2.top);
	unionRect.left=Min(rect1.left,rect2.left);
	unionRect.bottom=Max(rect1.bottom,rect2.bottom);
	unionRect.right=Max(rect1.right,rect2.right);
	return unionRect;
}

/*----------------------------------------------------------------------*/
bool guSectRect(const LongRect&	rect1, const LongRect& rect2, LongRect *sect)
{
	if(!guHasALenRect(rect1))
	{
		*sect=rect2;
		return false;
	}
	if(!guHasALenRect(rect2))
	{
		*sect=rect1;
		return false;
	}

	sect->top = Max(rect1.top,rect2.top);
	sect->left = Max(rect1.left,rect2.left);
	sect->bottom = Min(rect1.bottom,rect2.bottom);
	sect->right = Min(rect1.right,rect2.right);
	
	return(sect->top <= sect->bottom && sect->left <= sect->right);
} /* guSectRect()*/

/*------------------------------------------------------------*/
/*	returns true if testRect completely inside the encloseRect */
bool guRectInRect(
	const LongRect&			testRect,
	const LongRect&			encloseRect)
	{
	return(
		testRect.top >= encloseRect.top &&
		testRect.left >= encloseRect.left &&
		testRect.bottom <= encloseRect.bottom &&
		testRect.right <= encloseRect.right
		);
	} /* guRectInRect()*/

/*----------------------------------------------------------------------*/

bool guEmptyRect(const LongRect& lRect)
{
	return(lRect.top >= lRect.bottom || lRect.left >= lRect.right);
}

/*----------------------------------------------------------------------*/
/*
	returns true if rect either side has a length and other side has
	length >= 0
*/

bool guHasALenRect(
	const LongRect&		lRect)
	{
	return(
		(lRect.bottom > lRect.top && lRect.right >= lRect.left)||
		(lRect.right > lRect.left && lRect.bottom >= lRect.top)
		);
	}

/*----------------------------------------------------------------------*/

void guRotRect(const LongRect& lRect, Angle	angle, LongPoint rotatedLPoints[4])
{
	TSinCos sinCos(angle);
	LongPoint centerLPt=Middle(lRect);
/*	0    1
	+----+
	+----+
	3	 2 */
	for(short i=3;i>=0;--i)
		rotatedLPoints[i]=mtRotPoint(lRect.GetCorner(i),sinCos,centerLPt);
}

/*----------------------------------------------------------------------*/

LongPoint guPinRect(
	const LongRect&			longRect,
	const LongPoint&		longPt)
	{
	LongPoint pinPt = longPt;

	if(pinPt.y < longRect.top)
		pinPt.y = longRect.top;
	else if(pinPt.y > longRect.bottom)
		pinPt.y = longRect.bottom;

	if(pinPt.x < longRect.left)
		pinPt.x = longRect.left;
	else if(pinPt.x > longRect.right)
		pinPt.x = longRect.right;
	return pinPt;
	}/* guPinRect */

/*================== format conversion, same zoom ====================*/
/* converts a short point rect to a longPoint rect */
LongRect guSRect2LRect(const CRect& sRect)
{
	LongRect result;
	result.top = _guLOrd((short)sRect.top);
	result.left = _guLOrd((short)sRect.left);
	result.bottom = _guLOrd((short)sRect.bottom);
	result.right = _guLOrd((short)sRect.right);
	return result;
}


/*----------------------------------------------------------------------*/

/* Converts a longPoint rect to a shortPoint rect. The values
	are rounded to the closest integer.
*/

CRect guLRect2SRect(
	const LongRect& lRect)
	{
	CRect sRect;
	sRect.top = mtFixToShort(lRect.top);
	sRect.left = mtFixToShort(lRect.left);
	sRect.bottom = mtFixToShort(lRect.bottom);
	sRect.right = mtFixToShort(lRect.right);
	return sRect;
	}/* guLRect2SRect */

/*======================= zoom conversion, same format ====================*/

/*----------------------------------------------------------------------*/
/*
	This function finds a quickdraw angle in a long(fixed pt.)format.
	(See Inside Macintosh vol. I ch. 6 Quickdraw.).
	The angle is computed in double and rounded to a fixed point format.
*/
Angle guLPtToLAngle(
	Coord				longHRad,
	Coord				longVRad,
	const LongPoint&	longPt,
	const LongPoint&	longCenterPt)
	{
	double refh =(longPt.x - longCenterPt.x).GetRawDouble()*(double)longVRad.GetRawDouble();
	double refv =(longCenterPt.y - longPt.y).GetRawDouble()*(double)longHRad.GetRawDouble();
	if(refh==0.0 && refv==0.0)
		return FixedZero;

	refh = (atan2(refv, refh)* kRadToAng)* kShortToLong;
	refh +=(refh < 0.0 ? - 0.5 : 0.5);
	Angle lAng;
	lAng.SetFromRawDoubleTruncated(refh);
	if(lAng < FixedZero)	lAng += short(360);		/* Change the range from [-180,180] to [0,360] */
	return lAng;
	}/* guLPtToLAngle */

/*----------------------------------------------------------------------*/

Angle	guLAngToLEAng(
	Coord		hRad,
	Coord		vRad,
	Angle		realAng)
{
	double		refh, refv, dblAng, dblRealRad;

	if(realAng.GetFixed()% short(90)== 0)
		return realAng;		/* No need to correct right angles */

	dblRealRad = mtFixToReal(realAng)* kAngToRad;

	refh = cos(dblRealRad)* hRad.GetRawDouble();
	refv = sin(dblRealRad)* vRad.GetRawDouble();

	dblAng = atan2(refh, refv)* kRadToAng;

	if(dblAng < 0.0)dblAng += 360.0;
	return WrFixedFromRawDoubleTruncated((dblAng * 65536)+ 0.5);
} /* guLAngToLEAng */

/*----------------------------------------------------------------------*/

LongPoint guQDPt2LongPt(const pointd pt)
{
	LongPoint result;
	result.x=(short)pt.x;
	result.y=(short)pt.y;
	return result;
}

/*----------------------------------------------------------------------*/

/* converts a LongPoint to quickdraw point - rounding is done */

pointd guLongPt2QDPt(const LongPoint& aLongPt)
{
	pointd result;
	result.x=mtFixToShort(aLongPt.x);
	result.y=mtFixToShort(aLongPt.y);
	return result;
};

/* ---------------------------------------------------------------------------------
	convert a long point to double
------------------------------------------------------------------------------------ */
double guLOrd2Double(Coord longOrd)
{
// This whole routine seems FISHY . R.P.x.
// Extensive research leaves us the impression it a bag of worms
// It can change a number of over 10000 in the first decimal position so that
// 32767.75 becomes 32767.8, but the output numbers are then used in arithmetic
// and mashed about horribly, so what is the point ?
	double				d;
	 short		digits;
	 short		intPart;
	double				fractPart;
	 long		fractDenom;
	 short		ten = 10;


	intPart = short(longOrd.GetFixed()>>16);
	d = intPart;
	digits = 0;
	while(intPart){
		++digits;
		intPart /= ten;
	}
	if((digits = kSigDigits - digits)> 0){
		fractDenom = ten;
		--digits;
		while(--digits >= 0){
			fractDenom *= ten;
		}
		fractPart =(unsigned short)longOrd.GetFixed();
		fractPart /= 65536.0;
		fractPart *= fractDenom;
		fractPart += 0.5;
		fractPart =(long)fractPart;
		fractPart /= fractDenom;
		d += fractPart;
	}
	return(d);
}

bool guAlmostEqualPt(const LongPoint& pt1, const LongPoint& pt2)
{
	bool theSame=((Abs(pt1.x - pt2.x)<=kAlmostSameCriteria) &&
					(Abs(pt1.y - pt2.y)<=kAlmostSameCriteria));
	return theSame;
}

/******************************************************

	Copyright © 1990-98 by Microspot Ltd. All Rights Reserved.


	Created				: 4/21/88

******************************************************/
/*--------------------------- references -------------------------------

Handbook Of Mathematical Formulas, Bartsch,
	Academic Press New York and London, 1974.

Algorithms For Graphics And Image Processing, Theo Pavlidis,
	Computer Science Press, 1982.
*/

Angle ChordTolerance ( float radiusInPixels ) // RPH may 2000 derived from MacPlot and MacInteriors
	{	
/*
	This is more or less the original comment from Interiors
	THE CODE BELOW SHOULD BE EQUIVALENT TO THIS :-
	if ( radiusInPixels <= 1 )
		CIRCLEINC = DEG90
	else
		CIRCLEINC = Round ( ( ARCTAN ( SQRT ( radiusInPixels + radiusInPixels - 1 ) /
								( radiusInPixels - 1 ) ) / PiBy32768 ) ) ;
	if ( CIRCLEINC <= 128 )
		CIRCLEINC = 128
	else
		{
		while ( ( DEG90 MOD CIRCLEINC ) != 0 )
		&& ( ( CIRCLEINC & 127 ) != 0 ) Do
			CIRCLEINC -= 128 ;
		}
*/
	Angle result ;
	// Limits are formed by evaluating MaxRad = 1 / ( 1 - Cos ( Inc ) )

	if ( radiusInPixels <= 52.0f )
		if ( radiusInPixels <= 3.0f )
			if ( radiusInPixels <= 1.0f )
				result = k90 ; // 1/4 of a circle
			else
				result = k45 ; // 1/8 of a circle
		else
			if ( radiusInPixels <= 13.0f )
				result.SetFromFixed ( 0x00168000L ) ; // 1/16 of a circle
			else
				result.SetFromFixed ( 0x000B4000L ) ; // 1/32 of a circle
	else
		if ( radiusInPixels <= 830.0f )
			if ( radiusInPixels <= 207.0f )
				result.SetFromFixed ( 0x0005A000L ) ; // 1/64 of a circle
			else
				result.SetFromFixed ( 0x0002D000L ) ; // 1/128 of a circle
		else
			if ( radiusInPixels <= 3320.0f )
				result.SetFromFixed ( 0x00016800L ) ; // 1/256 of a circle
			else
				result.SetFromFixed ( 0x0000B400L ) ; // 1/512 of a circle
	return result ;
}