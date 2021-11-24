#include "stdafx.h"
#include "matrix3x2.h"
#include <math.h>
#include "phmath.h"


double LVector2D ::length () const { return hypot (x,y); }

LPoint2D operator * ( const LPoint2D& V , const LMatrix3x2& M )
{
	LPoint2D Result ;
	Result.x = V.x * M [ 0 ] [ 0 ] + V.y * M [ 1 ] [ 0 ] + M [ 2 ] [ 0 ] ;
	Result.y = V.x * M [ 0 ] [ 1 ] + V.y * M [ 1 ] [ 1 ] + M [ 2 ] [ 1 ] ;
	return Result ;
}

void operator *= ( LPoint2D& R , const LMatrix3x2& M )
{
	LPoint2D Result ;
	Result.x = R.x * M [ 0 ] [ 0 ] + R.y * M [ 1 ] [ 0 ] + M [ 2 ] [ 0 ] ;
	Result.y = R.x * M [ 0 ] [ 1 ] + R.y * M [ 1 ] [ 1 ] + M [ 2 ] [ 1 ] ;

	R.x = Result.x ;
	R.y = Result.y ;
}

LVector2D operator * ( const LVector2D& V , const LMatrix3x2& M )
{
	LVector2D Result ;
	Result.x = V.x * M [ 0 ] [ 0 ] + V.y * M [ 1 ] [ 0 ] ;
	Result.y = V.x * M [ 0 ] [ 1 ] + V.y * M [ 1 ] [ 1 ] ;
	return Result ;
}

void operator *= ( LVector2D& R , const LMatrix3x2& M )
{
	LPoint2D Result ;
	Result.x = R.x * M [ 0 ] [ 0 ] + R.y * M [ 1 ] [ 0 ] ;
	Result.y = R.x * M [ 0 ] [ 1 ] + R.y * M [ 1 ] [ 1 ] ;

	R.x = Result.x ;
	R.y = Result.y ;
}

LMatrix3x2 operator * ( const LMatrix3x2& A , const LMatrix3x2& B )
{
	LMatrix3x2 Result ;
	Result [ 0 ] [ 0 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 0 ] [ 1 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 0 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 1 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 2 ] [ 0 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 0 ] + B [ 2 ] [ 0 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	Result [ 2 ] [ 1 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 1 ] + B [ 2 ] [ 1 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	return Result ;
}

void operator *= ( LMatrix3x2& A , const LMatrix3x2& B )
{
	LMatrix3x2 Result ;
	Result [ 0 ] [ 0 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 0 ] [ 1 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 0 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 1 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 2 ] [ 0 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 0 ] + B [ 2 ] [ 0 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	Result [ 2 ] [ 1 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 1 ] + B [ 2 ] [ 1 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	A = Result ;
}

bool IsMirrored ( const LMatrix3x2& M )
{
	return M [ 0 ] [ 0 ] * M [ 1 ] [ 1 ] - M [ 0 ] [ 1 ] * M [ 1 ] [ 0 ] < 0 ; // cos^2 + sin^2 == 1, leaving xScale * yScale. RPH
}

bool LMatrix3x2::IsUniformRotateMatrix() const
{
	if( (Abs( Abs(X0) - Abs(Y1)) < 1e-9) && (Abs(Abs(X1) - Abs(Y0)) < 1e-9)) return true;
	return false;
}

bool LMatrix3x2::IsMirrored () const
{
	return X0 * Y1 - X1 * Y0 < 0 ; // cos^2 + sin^2 == 1, leaving xScale * yScale. RPH
}		

LVector2D LMatrix3x2::GetScale () const
{
//	return LVector2D (hypot (X0,Y0), hypot (X1,Y1));
	return LVector2D (hypot (X0,X1), hypot (Y0,Y1));

}

double LMatrix3x2::GetAngle () const
{
	return 	atan2 (X1 , Y1);
}
LMatrix3x2 ScaleMat3x2 ( const double X , const double Y )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = X ;
	M [ 0 ] [ 1 ] = 0.0 ;
	
	M [ 1 ] [ 0 ] = 0.0 ;
	M [ 1 ] [ 1 ] = Y ;
	
	M [ 2 ] [ 0 ] = 0.0 ;
	M [ 2 ] [ 1 ] = 0.0 ;
	return M ;
}

LMatrix3x2 TranslateMat3x2 ( const LVector2D v )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = 1.0 ;
	M [ 0 ] [ 1 ] = 0.0 ;
	
	M [ 1 ] [ 0 ] = 0.0 ;
	M [ 1 ] [ 1 ] = 1.0 ;
	
	M [ 2 ] [ 0 ] = v.x ;
	M [ 2 ] [ 1 ] = v.y ;
	return M ;
}

LMatrix3x2 TranslateMat3x2 ( double x , double y )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = 1.0 ;
	M [ 0 ] [ 1 ] = 0.0 ;
	
	M [ 1 ] [ 0 ] = 0.0 ;
	M [ 1 ] [ 1 ] = 1.0 ;
	
	M [ 2 ] [ 0 ] = x ;
	M [ 2 ] [ 1 ] = y ;
	return M ;
}

LMatrix3x2 RotateMat3x2 ( const double Radians )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = cos ( Radians ) ;
	M [ 0 ] [ 1 ] = -sin ( Radians ) ;
	
	M [ 1 ] [ 0 ] = sin ( Radians ) ;
	M [ 1 ] [ 1 ] = cos ( Radians ) ;
	
	M [ 2 ] [ 0 ] = 0.0 ;
	M [ 2 ] [ 1 ] = 0.0 ;
	return M ;
}

LMatrix3x2 RotateMat3x2 ( const LPoint2D Pivot , const double Radians )
{
	// Could be optimised easily
	return TranslateMat3x2 ( -Pivot.x , -Pivot.y ) * RotateMat3x2 ( Radians ) * TranslateMat3x2 ( Pivot.x , Pivot.y ) ;
}
