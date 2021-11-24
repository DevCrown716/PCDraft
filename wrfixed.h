#ifndef __WRFIXED_H__
#define __WRFIXED_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "macutils.h"

const double doubleOneOver65536=1.0/65536.0;

class WrFixed
{
public:
				WrFixed() {}
				WrFixed(const int s) { l = long(s)<<16; }
				WrFixed(const unsigned int s) { l =(unsigned long)(s)<<16; }
				WrFixed(const WrFixed& f) { l = f.l; }
				WrFixed(const double d) { l = long(d*65536.0+0.5); }
	void		SetFromFixed(const Fixed f) { l = f; }
	void		SetFromRawDoubleRounded(const double f) { l = roundtol(f); }
	void		SetFromRawDoubleTruncated(const double f) { l = long(f); }
	void		SetFromDoubleTruncated(const double f) { l = long(f*65536.0); }
	Fixed		GetFixed() const { return Fixed(l); }
	long		GetLong() const { return Fix2Long(l); }
   double		GetDouble() const { return double(l)/65536.0; }
	double		GetRawDouble() const { return double(l); }
	WrFixed		FracPart() const { return WrFixed(long(l & 0x0000FFFF)); }

	WrFixed		operator-() const { return WrFixed(-l); }

	void		operator++() { l += 65536; }
	void		operator--() { l -= 65536; }

	WrFixed		operator=(const WrFixed& b) { l = b.l; return *this; }
	void		operator=(const int b) { l =(long(b)<<16); }
	void		operator=(const double b) { l = roundtol(b*65536.0); }
	WrFixed		operator+(const WrFixed& b) const { return WrFixed(l+b.l); }
	WrFixed		operator+(const int b) const { return WrFixed(l +(long(b)<<16)); }
	double	operator+(const double b) const { return l*doubleOneOver65536+b; }
	void		operator+=(const WrFixed& b) { l += b.l; }
	void		operator+=(const int b) { l +=(long(b)<<16); }
	void		operator+=(const double b) { l += roundtol(b*65536.0); }
	WrFixed		operator-(const WrFixed& b) const { return WrFixed(l-b.l); }
	WrFixed		operator-(const int b) const { return WrFixed(l -(long(b)<<16)); }
	double	operator-(const double b) const { return l*doubleOneOver65536-b; }
	void		operator-=(const WrFixed& b) { l -= b.l; }
	void		operator-=(const int b) { l -=(long(b)<<16); }
	void		operator-=(const double b) { l -= roundtol(b*65536.0); }
	WrFixed		operator*(const WrFixed& b) const { return WrFixed(FixMul(l, b.l)); }
	WrFixed		operator*(const int b) const { return WrFixed(long(l*b)); }
	double		operator*(const double b) const { return l*doubleOneOver65536*b; }
	void		operator*=(const WrFixed& b) { l = FixMul(l, b.l); }
	void		operator*=(const int b) { l *= b; }
	void		operator*=(const double b) { l = roundtol(l*b); }
	WrFixed		operator/(const WrFixed& b) const { return WrFixed(FixDiv(l, b.l)); }
	WrFixed		operator/(const int b) const { return WrFixed(long(l/b)); }
	double	operator/(const double b) const { return l*doubleOneOver65536/b; }
	WrFixed		operator%(const WrFixed& b) const { return WrFixed(l % b.l); } // Angle processing does % k360 a lot
	void		operator/=(const WrFixed& b) { l = FixDiv(l, b.l); }
	void		operator/=(const int b) { l /= b; }
	void		operator/=(const double b) { l = roundtol(l/b); }
	WrFixed		operator<<(const int b) const { return WrFixed(l<<b); }
	void		operator<<=(const int b) { l <<= b; }
	WrFixed		operator>>(const int b) const { return WrFixed(l>>b); }
	void		operator>>=(const int b) { l >>= b; }
 
 	bool		operator==(const WrFixed& b) const { return l == b.l; }
	bool		operator==(const int b) const { return l ==(long(b)<<16); }
	bool		operator==(const double b) const { return l*doubleOneOver65536 == b; }
 	bool		operator!=(const WrFixed& b) const { return l != b.l; }
	bool		operator!=(const int b) const { return l !=(long(b)<<16); }
	bool		operator!=(const double b) const { return l*doubleOneOver65536 != b; }
 	bool		operator<(const WrFixed& b) const { return l < b.l; }
	bool		operator<(const int b) const { return l <(long(b)<<16); }
	bool		operator<(const double b) const { return l*doubleOneOver65536 < b; }
 	bool		operator>(const WrFixed& b) const { return l > b.l; }
	bool		operator>(const int b) const { return l >(long(b)<<16); }
	bool		operator>(const double b) const { return l*doubleOneOver65536 > b; }
 	bool		operator<=(const WrFixed& b) const { return l<=b.l; }
	bool		operator<=(const int b) const { return l <=(long(b)<<16); }
	bool		operator<=(const double b) const { return l*doubleOneOver65536<=b; }
 	bool		operator>=(const WrFixed& b) const { return l>=b.l; }
	bool		operator>=(const int b) const { return l >=(long(b)<<16); }
	bool		operator>=(const double b) const { return l*doubleOneOver65536>=b; }

   void from_raw_long(long lParam) { l=lParam; } // Free Scale -  Infinisys Ltd
 	
 	
	friend void operator+=(double& a, const WrFixed& b);
 	friend WrFixed operator-(const int a, const WrFixed& b);
 	friend double operator-(const double a, const WrFixed& b);
 	friend void operator-=(double& a, const WrFixed& b);
	friend void operator*=(double& a, const WrFixed& b);
	friend void operator*=(int& a, const WrFixed& b);
	friend double operator/(const double a, const WrFixed& b);
	friend void operator/=(double& a, const WrFixed& b);
	friend void operator/=(int& a, const WrFixed& b);
	friend WrFixed Abs(const WrFixed& a);
	friend WrFixed Sign(const WrFixed& a);
	friend WrFixed Average(const WrFixed& P1, const WrFixed& P2);
	friend WrFixed Average(const WrFixed& P1, const WrFixed& P2, const WrFixed& P3);
	friend WrFixed Average(const WrFixed& P1, const WrFixed& P2, const WrFixed& P3, const WrFixed& P4);
	friend bool NearlyEqual(const WrFixed& a, const WrFixed& b);
	friend WrFixed WrFixedFromFixed(const Fixed f);
	friend WrFixed WrFixedFromFract(const Fract f);
	friend WrFixed WrFixedFromRatio(int numer, int denom);
	friend WrFixed WrFixedFromRawDoubleRounded(const double d);
	friend WrFixed WrFixedFromRawDoubleTruncated(const double d);
	friend short TruncToShort(const WrFixed& a);
	friend short RoundToShort(const WrFixed& a);
	friend double mtFixToReal(const WrFixed& a); // Temporarily use old macro name, rename later
	friend CArchive& operator<<(CArchive&, const WrFixed& wr);
	friend CArchive& operator>>(CArchive&, WrFixed& wr);
private:
	WrFixed(const long l_) { l = l_; }
	long	l;
};
	

inline WrFixed operator+(const int a, const WrFixed& b)
{
	return b+a;
} 

inline double operator+(const double a, const WrFixed& b)
{
	return b+a;
} 

inline void operator+=(double& a, const WrFixed& b)
{
	a += b.l*doubleOneOver65536;
}

inline WrFixed operator-(const int a, const WrFixed& b)
{
	return WrFixed(( long(a)<<16)-b.l);
} 

inline double operator-(const double a, const WrFixed& b)
{
	return a-b.l*doubleOneOver65536; 
} 

inline void operator-=(double& a, const WrFixed& b)
{
	a -= b.l*doubleOneOver65536;
}

inline WrFixed operator*(const int a, const WrFixed& b)
{
	return b*a;
} 

inline double operator*(const double a, const WrFixed& b)
{
	return b*a;
} 

inline void operator*=(double& a, const WrFixed& b)
{
	a *= b.l*doubleOneOver65536;
}

inline void operator*=(int& a, const WrFixed& b)
{
	a =(FixMul(long(a)<<16, b.l)+0x00008000)>>16; // Not optimal, but will be rarely used
}

inline WrFixed operator/(const int a, const WrFixed& b)
{
	return WrFixed(a)/b; // calls operator/ for two "WrFixed"s
} 

inline double operator/(const double a, const WrFixed& b)
{
	return a /(b.l*doubleOneOver65536); 
} 

inline void operator/=(double& a, const WrFixed& b)
{
	a /= b.l*doubleOneOver65536;
}

inline void operator/=(int& a, const WrFixed& b)
{
	a =(FixDiv(long(a)<<16, b.l)+0x00008000)>>16; // Not optimal, but will be rarely used
}

inline WrFixed Abs(const WrFixed& a)
{
	return a.l < 0 ? -a : a;
}

inline WrFixed Sign(const WrFixed& a)
{
	if(a.l>0)
		return WrFixed(1);
	if(a.l<0)
		return WrFixed(-1);
	return WrFixed(0);
}

inline WrFixed ratio(// Lower case because Ratio is defined as a struct !!!(in GUStructs.h)
					 const WrFixed&		V1 ,
					 const WrFixed&		V2 ,
					 const WrFixed&		R1 ,
					 const WrFixed&		R2)
{
	return(V1*R2+V2*R1) /(R1+R2);
}


inline WrFixed ratio(// Lower case because Ratio is defined as a struct !!!(in GUStructs.h)
					 const WrFixed&		V1 ,
					 const WrFixed&		V2 ,
					 const WrFixed&		V3 ,
					 const WrFixed&		R1 ,
					 const WrFixed&		R2 ,
					 const WrFixed&		R3)
{
	return(V1*R2*R3+V2*R3*R1+V3*R1*R2) /(R1*R2+R2*R3+R3*R1);
}

inline WrFixed Average(const WrFixed& P1, const WrFixed& P2)
{
	return P1 +(( P2-P1)>>1);
}

inline WrFixed Average(const WrFixed& P1, const WrFixed& P2, const WrFixed& P3)
{
	return WrFixed(long((P1.l+P2.l+P3.l+1)/3));
}

inline WrFixed Average(const WrFixed& P1, const WrFixed& P2, const WrFixed& P3, const WrFixed& P4)
{
	return WrFixed(long((P1.l+P2.l+P3.l+P4.l+1)>>2));
}

inline bool NearlyEqual(const WrFixed& a, const WrFixed& b)
{
	register long d = a.l-b.l;
	return d>=-1 && d<=1;
}

inline WrFixed WrFixedFromFixed(const Fixed f) 
{
	return WrFixed(long(f));
}

inline WrFixed WrFixedFromFract(const Fract f) 
{
	return WrFixed(long(Frac2Fix(f)));
}

inline WrFixed WrFixedFromRatio(int numer, int denom)
{
	return WrFixed(long(FixRatio(numer, denom)));
}

inline WrFixed WrFixedFromRawDoubleTruncated(const double d) 
{
	return WrFixedFromFixed(long(d));
}

inline WrFixed WrFixedFromRawDoubleRounded(const double d) 
{
	return WrFixedFromFixed(roundtol(d));
}

inline double mtFixToReal(const WrFixed& a) // Temporarily use old macro name, rename later
{
	return a.l*doubleOneOver65536;
}

inline CArchive& operator<<(CArchive& ar, const WrFixed& wr)
{
	return ar << wr.l;
}

inline CArchive& operator>>(CArchive& ar, WrFixed& wr)
{
	return ar >> wr.l;
}

inline short TruncToShort(const WrFixed& a) { return short(a.l>>16); }
inline short RoundToShort(const WrFixed& a) { return short(( a.l+0x00008000)>>16); }

const WrFixed FixedZero = WrFixedFromFixed(0);
const WrFixed FixedHalf = WrFixedFromFixed(0x00008000);
const WrFixed FixedOne  = 1;
const WrFixed FixedOnePointTwoFive = WrFixedFromFixed(0x00014000);
const WrFixed FixedOnePointFive = WrFixedFromFixed(0x00018000);
const WrFixed FixedTwo  = 2;
const WrFixed FixedMinDelta = WrFixedFromFixed(1L);
const WrFixed FixedMinDeltaTimes2 = WrFixedFromFixed(2L);
const WrFixed FixedMin = WrFixedFromFixed(LONG_MIN);
const WrFixed FixedMax = WrFixedFromFixed(LONG_MAX);

template<typename T>
inline T Abs(const T& a) { return a>=0?a:-a; }

template<typename T>
inline T Sign(const T a) { return a>0?1:a<0?-1:0; }

template<typename T>
inline T Min(const T a, const T b) { return a<b?a:b; }

template<typename T>
inline T Max(const T a, const T b) { return a>b?a:b; }

template<typename T>
inline T MinMax(const T MinVal, const T expression, const T MaxVal)
{
	return Min(Max(expression, MinVal), MaxVal); 
}

// By Roger Holmes
// Copyright Microspot Ltd 1998

#endif