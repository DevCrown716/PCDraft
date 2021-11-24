#ifndef __MATRIX3x2__
#define __MATRIX3x2__

struct LPoint2D
{
	double x ;
	double y ;

	LPoint2D () : x (0), y (0) {}
	LPoint2D (double _x, double _y) : x (_x),  y (_y) { }
	double& operator [] ( unsigned long L ) { return ( ( double* ) ( &x ) ) [ L ] ; }
	const double& operator [] ( unsigned long L ) const { return ( ( double* ) ( &x ) ) [ L ] ; }
} ;

struct LVector2D
{
	double x ;
	double y ;

	LVector2D () : x (0), y (0) {}
	LVector2D (double _x, double _y) : x (_x),  y (_y) { }
	double& operator [] ( unsigned long L ) { return ( ( double* ) ( &x ) ) [ L ] ; }
	const double& operator [] ( unsigned long L ) const { return ( ( double* ) ( &x ) ) [ L ] ; }
	double length () const;
} ;

struct LMatrix3x2
{
	double X0 , Y0 ; // Scale / Rotation
	double X1 , Y1 ; // Scale / Rotation
	double X2 , Y2 ; // Offset

	LPoint2D& operator [] ( unsigned long L ) { return ( ( LPoint2D* ) ( &X0 ) ) [ L ] ; }
	const LPoint2D& operator [] ( unsigned long L ) const { return ( ( LPoint2D* ) ( &X0 ) ) [ L ] ; }
	double GetAngle () const;
	bool IsMirrored () const;
	LVector2D GetScale () const;
	bool IsUniformRotateMatrix() const;
} ;

LPoint2D operator * ( const LPoint2D& V , const LMatrix3x2& M );
void operator *= ( LPoint2D& R , const LMatrix3x2& M );
LVector2D operator * ( const LVector2D& V , const LMatrix3x2& M );
void operator *= ( LVector2D& R , const LMatrix3x2& M );
LMatrix3x2 operator * ( const LMatrix3x2& A , const LMatrix3x2& B );
void operator *= ( LMatrix3x2& A , const LMatrix3x2& B );
bool IsMirrored ( const LMatrix3x2& M );

LMatrix3x2 ScaleMat3x2 ( const double X , const double Y );
LMatrix3x2 TranslateMat3x2 ( const LVector2D v );
LMatrix3x2 TranslateMat3x2 ( double x , double y );
LMatrix3x2 RotateMat3x2 ( const double Radians );
LMatrix3x2 RotateMat3x2 ( const LPoint2D Pivot , const double Radians );

#endif //__MATRIX3x2__