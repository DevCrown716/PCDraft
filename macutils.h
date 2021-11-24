#ifndef __MACUTILS_H__
#define __MACUTILS_H__

typedef long Fixed;
typedef long Fract;

inline long Fix2Long(Fixed l)
{
	return (l+(l>=0?0x7FFF:-0x7FFF))>>16;
}

inline Fixed FixRatio(int numer, int denom)
{
	return MulDiv(numer,65536,denom);
}

inline Fixed Frac2Fix(Fract l)
{
	return (l+(l>=0?0x7FFF:-0x7FFF))>>14;
}

inline Fixed FixMul(Fixed mul1, Fixed mul2)
{
	return MulDiv(mul1, mul2, 65536);
}

inline Fixed FixDiv(Fixed div, Fixed denom)
{
	return MulDiv(div,65536,denom);
}

inline long roundtol(const double d)
{
	return long(d+(d>=0?0.5:-0.5));
}

#endif