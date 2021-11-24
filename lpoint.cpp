#include "stdafx.h"
#include "lpoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////
// LongPoint

const LongPoint LongPoint::kZeroPt(FixedZero, FixedZero);
const LongPoint LongPoint::kMinDeltaPt(FixedMinDelta, FixedMinDelta);

void LongPoint::ConstrainTo(const LongRect& rc)
{
	if(x<rc.left) x=rc.left;
	if(y<rc.top) y=rc.top;
	if(x>rc.right) x=rc.right;
	if(y>rc.bottom) y=rc.bottom;
}

/////////////////////////////////////////////////////////////
// LongRect

const LongRect LongRect::kZeroRect(FixedZero,FixedZero,FixedZero,FixedZero);

LongRect& LongRect::operator&=(const LongRect& rc)
{
	left=Max(left, rc.left);
	top=Max(top, rc.top);
	right=Min(right, rc.right);
	bottom=Min(bottom, rc.bottom);
	if(!Valid())
		left=top=right=bottom=FixedZero;
	return *this;
}

LongRect& LongRect::operator|=(const LongRect& rc)
{
	left=Min(left, rc.left);
	top=Min(top, rc.top);
	right=Max(right, rc.right);
	bottom=Max(bottom, rc.bottom);
	return *this;
}

LongRect& LongRect::operator|=(const LongPoint& pt)
{
	left=Min(left,pt.x);
	top=Min(top,pt.y);
	right=Max(right,pt.x);
	bottom=Max(bottom,pt.y);
	return *this;
}

void LongRect::SetToBound(const LongPoint& a, const LongPoint& b) 
{
	if(a.x<=b.x) { left = a.x; right = b.x; }
	else { left = b.x; right = a.x; }
	if(a.y<=b.y) { top = a.y; bottom = b.y; }
	else { top = b.y; bottom = a.y; }
}	

void LongRect::Validate()
{
	if (top > bottom)
	{
		WrFixed tmp = top;
		top = bottom;
		bottom = tmp;
	}
	if (left > right)
	{
		WrFixed tmp = left;
		left = right;
		right = tmp;
	}
}

LongPoint LongRect::GetCorner(const long nIndex) const
{
	switch(nIndex)
	{
	case 0:
		return LongPoint(left,top);
	case 1:
		return LongPoint(right,top);
	case 2:
		return LongPoint(right,bottom);
	case 3:
		return LongPoint(left,bottom);
	}
	return LongPoint::kZeroPt;
}

