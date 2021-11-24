/******************************************************

	ColUtils.h

	Copyright © Microspot Ltd 1992

	Created					: 02/18/92

******************************************************/

#ifndef _ColUtilsH_
#define	_ColUtilsH_

//#include "ColShapes.h"
//#include "PhDocStructs.h"
#include "PhMath.h"


//class CMDDimStr;

/*------------------------------- MACROS ( & inlines ) -----------------------------------------*/

#define kArcToCuRatio 		0.5522847498

/*
  Forms the smallest rectangle which encloses both points
*/
/*inline void ptsToRect ( ColFdRect* rcPtr , ColFdPt* pPtr , ColFdPt* qPtr )
	{
	if ( qPtr->x > pPtr->x )
    	{
    	rcPtr->left = pPtr->x ;
    	rcPtr->right = qPtr->x ;
    	}
    else
    	{
    	rcPtr->left = qPtr->x ;
    	rcPtr->right = pPtr->x ;
    	}

	if ( qPtr->y > pPtr->y )
    	{
    	rcPtr->top = pPtr->y ;
    	rcPtr->bottom = qPtr->y ;
    	}
    else
    	{
    	rcPtr->top = qPtr->y ;
    	rcPtr->bottom = pPtr->y ;
    	}
    }

*/
/*
  This will set the four points at pPtPtr to the corners of the
  rectangle at pRect, starting at top left.
*/
/*inline void rectToCorners ( ColFdRect* pRect , ColFdPtPtr pPtPtr )
	{
    ((pPtPtr)->x)   = ((pPtPtr+3)->x) = ((pRect)->left) ;
    ((pPtPtr+1)->x) = ((pPtPtr+2)->x) = ((pRect)->right) ;
    ((pPtPtr)->y)   = ((pPtPtr+1)->y) = ((pRect)->top) ;
    ((pPtPtr+2)->y) = ((pPtPtr+3)->y) = ((pRect)->bottom) ;
    }
*/

inline short mFdAngToTxtAng ( Angle x )
	{
	return TruncToShort ( ( ( ( x ) << 2 ) + ( x ) ) >> 15 ) ;
	}

/*inline void mColMidPt ( ColFdPt* src1 , ColFdPt*  src2 , ColFdPt* dst )
    {
    dst->x = ( src1->x + src2->x ) >> 1 ;
    dst->y = ( src1->y + src2->y ) >> 1 ;
    } 

inline void operator+= ( ColFdPt& dst , const ColFdPt& src )
	{
    dst.x += src.x ;
    dst.y += src.y ;
    }

inline void operator-= ( ColFdPt& dst , const ColFdPt& src )
	{
    dst.x -= src.x ;
    dst.y -= src.y ;
    }*/

/* For rounding a number in fixed double format to a fixed format number. */
inline WrFixed mRndFdDbl ( double dbl )
	{
	return WrFixedFromRawDoubleTruncated ( dbl + ( dbl < 0.0 ? - 0.5 : 0.5 ) ) ;
} 

/* mOppPt reflects src2 around src1, returned in dst. */
/*inline void mOppPt ( ColFdPt* src1 , ColFdPt* src2 , ColFdPt* dst )
    {
    dst->x = ( src1->x << 1 ) - src2->x ;
    dst->y = ( src1->y << 1 ) - src2->y ;
    } */


/* For multiplying a fixed format number with a double format number. */
/* Use this macro for accurately multiplying a fixed number with a double */
inline WrFixed mFdMulDbl ( WrFixed fd , double dbl )
	{
	return mRndFdDbl ( fd.GetRawDouble () * dbl ) ;
} 


/*---------------------------------------------------------------------------------*/

Angle m360Ang(Angle fdAngle);
//Angle mAngle ( WrFixed x1 , WrFixed y1 , WrFixed x2 , WrFixed y2 ) ;

/*---------------------------------------------------------------------------------*/

#endif