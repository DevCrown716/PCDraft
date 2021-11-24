#ifndef __LPOINT_H__
#define __LPOINT_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "wrfixed.h"

#pragma pack(1)

struct LongRatio
{
   long fN;
   long fD;

   bool operator ==(const LongRatio & lr) const { return lr.fD == fD && lr.fN == fN; } // Scale/Unit - Infinisys Ltd

   bool operator !=(const LongRatio & lr) const { return !operator==(lr); } // Scale/Unit - Infinisys Ltd

};

#pragma pack()

inline WrFixed operator*(const WrFixed f, const LongRatio& r)
{
   return WrFixedFromFixed(MulDiv(f.GetFixed(),r.fN,r.fD));
//	return WrFixedFromFixed(f.GetFixed()*r.fN/r.fD);
}

inline void operator*=(WrFixed& f, const LongRatio& r)
{
   f=WrFixedFromFixed(MulDiv(f.GetFixed(),r.fN,r.fD));
//	f=WrFixedFromFixed(f.GetFixed()*r.fN/r.fD);
}

inline CArchive& operator<<(CArchive& ar, const LongRatio& r)
{
   return ar << r.fN << r.fD;
}

inline CArchive& operator>>(CArchive& ar, LongRatio& r)
{
   return ar >> r.fN >> r.fD;
}

typedef WrFixed Angle;
typedef WrFixed Coord;

// moved from gu.h
// adapted for win32/msvc

enum XYSelect { xSel, ySel };

class LongPoint;
class LongRect;

//----------------------------------------------------------------------------------------
// From CTemplatePoint_AC.h
// Copyright © 1996 by Apple Computer, Inc. All rights reserved.
// With extensive modifications, which are copyright Microspot Ltd
//----------------------------------------------------------------------------------------

#pragma pack(1)

class LongPoint
{
public:
   LongPoint() : x(0), y(0) {}

   LongPoint(Coord x_, Coord y_) : x(x_), y(y_) {}

   LongPoint(const CPoint& pt) : x(short(pt.x)), y(short(pt.y)) {}

   LongPoint(const LongPoint& pt) : x(pt.x), y(pt.y) {}

   const LongPoint& operator=(const LongPoint& pt)
   {
      x=pt.x; y=pt.y;
      return *this;
   }

   Coord& operator[](XYSelect sel)
   {
      return (sel == ySel) ? y : x;
   }

   const Coord& operator[](XYSelect sel) const
   {
      return (sel == ySel) ? y : x;
   }

   LongPoint& operator+=(const LongPoint& pt)
   {
      x+=pt.x;
      y+=pt.y;
      return *this;
   }

   LongPoint& operator-=(const LongPoint& pt)
   {
      x-=pt.x;
      y-=pt.y;
      return *this;
   }

   LongPoint& operator*=(const LongPoint& pt)
   {
      x*=pt.x;
      y*=pt.y;
      return *this;
   }

   LongPoint& operator*=(const double scalar )
   {
      x*=scalar;
      y*=scalar;
      return *this;
   }

   LongPoint& operator*=(const WrFixed& scalar )
   {
      x*=scalar;
      y*=scalar;
      return *this;
   }

   LongPoint& operator*=(const LongRatio& r)
   {
      x*=r;
      y*=r;
      return *this;
   }

   LongPoint& operator/=(const LongPoint& pt)
   {
      x/=pt.x;
      y/=pt.y;
      return *this;
   }

   LongPoint& operator/=(const double scalar )
   {
      double reciprocal=1.0/scalar;
      x*=reciprocal;
      y*=reciprocal;
      return *this;
   }

   LongPoint& operator/=(const short scalar )
   {
      x/=scalar;
      y/=scalar;
      return *this;
   }

   LongPoint operator+(const LongPoint& pt) const
   {
      return LongPoint(*this)+=pt;
   }

   LongPoint operator-(const LongPoint& pt) const
   {
      return LongPoint(*this)-=pt;
   }

   LongPoint operator*(const LongPoint& pt) const
   {
      return LongPoint(*this)*=pt;
   }

   LongPoint operator*(const double scalar) const
   {
      return LongPoint(x*scalar, y*scalar);
   }

   LongPoint operator*(const WrFixed& scalar) const
   {
      return LongPoint(x*scalar, y*scalar);
   }

   bool Rescale(const LongRatio& r)
   {
      bool bResult=false;
      Coord temp=x*r;
      if(temp.GetFixed()==-1) bResult=true;
      else x=temp;
      temp=y*r;
      if(temp.GetFixed()==-1) bResult=true;
      else y=temp;
      return bResult;
   }

   LongPoint operator*(const LongRatio& r) const
   {
      return LongPoint(x*r, y*r);
   }

   LongPoint operator/(const LongPoint& pt) const
   {
      return LongPoint(*this)/=pt;
   }

   LongPoint operator/(const double scalar) const
   {
      double reciprocal = 1.0/scalar;
      return LongPoint(x*reciprocal,y*reciprocal);
   }

   LongPoint operator/(const WrFixed& scalar) const
   {
      return LongPoint(x/scalar, y/scalar);
   }

   LongPoint operator/(const short scalar) const
   {
      return LongPoint(x/scalar,y/scalar);
   }

   LongPoint operator>>(const short shift) const
   {
      return LongPoint(x>>shift, y>>shift);
   }

   LongPoint& operator>>=(const short shift)
   {
      x >>= shift;
      y >>= shift;
      return *this;
   }

   LongPoint operator<<(const short shift) const
   {
      return LongPoint(x<<shift, y<<shift);
   }

   LongPoint& operator<<=(const short shift)
   {
      x <<= shift;
      y <<= shift;
      return *this;
   }

   LongPoint operator-() const
   {
      return LongPoint(-x,-y);
   }

   LongPoint& operator++()
   {
      ++x; ++y;
      return *this;
   }

   LongPoint& operator--()
   {
      --x; --y;
      return *this;
   }

   const LongPoint operator++(int)
   {
      const LongPoint returnVal(*this);
      ++*this;
      return returnVal;
   }

   const LongPoint operator--(int)
   {
      const LongPoint returnVal(*this);
      --*this;
      return returnVal;
   }

   bool LongPoint::operator==(const LongPoint& pt) const
   {
      return x==pt.x && y==pt.y;
   }

   bool LongPoint::operator>=(const LongPoint& pt) const
   {
      return x>=pt.x && y>=pt.y;
   }

   bool LongPoint::operator<=(const LongPoint& pt) const
   {
      return x<=pt.x && y<=pt.y;
   }

   bool LongPoint::operator>(const LongPoint& pt) const
   {
      return x>pt.x && y>pt.y;
   }

   bool LongPoint::operator<(const LongPoint& pt) const
   {
      return x<pt.x && y<pt.y;
   }

   bool operator!=(const LongPoint& pt) const
   {
      return !(*this==pt);
   }

   bool IsZero() const
   {
      return (x==FixedZero) && (y==FixedZero);
   }

   void Clear()
   {
      x=FixedZero;
      y=FixedZero;
   }

   void Set(Coord x_, Coord y_)
   {
      x = x_; y = y_;
   }

   void Offset(Coord x_, Coord y_)
   {
      x+=x_; y+=y_;
   }

   void ConstrainTo(const LongRect& rt);

   void SwapXY() {	Coord t=y; y=x; x=t; }

public:
   Coord x;
   Coord y;

public:
   static const LongPoint kZeroPt;
   static const LongPoint kMinDeltaPt;
};

inline CArchive& operator<<(CArchive& ar, const LongPoint& pt)
{
   return ar << pt.x << pt.y;
}

inline CArchive& operator>>(CArchive& ar, LongPoint& pt)
{
   return ar >> pt.x >> pt.y;
}

typedef LongPoint* LongPointPtr;
typedef CArray<LongPoint,const LongPoint&> LongPointArray;

struct DoublePoint
{
   double		x, y;
};

typedef DoublePoint* DoublePointPtr;

enum PointSelect { topLeft, bottomRight };

class LongRect
{
public:

   LongRect() {}

   LongRect(Coord left_, Coord top_, Coord right_, Coord bottom_)
      : left(left_), top(top_), right(right_), bottom(bottom_) {}

   LongRect(const LongPoint& topLeft_, const LongPoint& bottomRight_)
   {
      Set(topLeft_, bottomRight_);
   }

   LongRect(const LongRect& aRect)
      : left(aRect.left), top(aRect.top), right(aRect.right), bottom(aRect.bottom) {}

   LongRect(const CRect& aRect)
      : left((short)aRect.left), top((short)aRect.top), right((short)aRect.right), bottom((short)aRect.bottom) {}

   LongPoint& operator[](PointSelect sel)
   {
      return (sel==topLeft)?((LongPoint&)left):((LongPoint&)right);
   }

   const LongPoint& operator[](PointSelect sel) const
   {
      return (sel==topLeft)?((const LongPoint&)left):((const LongPoint&)right);
   }

   LongRect& operator+=(const LongRect& rc)
   {
      left+=rc.left;
      top+=rc.top;
      right+=rc.right;
      bottom+=rc.bottom;
      return *this;
   }

   LongRect& operator-=(const LongRect& rc)
   {
      left-=rc.left;
      top-=rc.top;
      right-=rc.right;
      bottom-=rc.bottom;
      return *this;
   }

   LongRect& operator*=(const LongRect& rc)
   {
      left*=rc.left;
      top*=rc.top;
      right*=rc.right;
      bottom*=rc.bottom;
      return *this;
   }

   LongRect& operator*=(const LongRatio& r)
   {
      left*=r;
      top*=r;
      right*=r;
      bottom*=r;
      return *this;
   }

   LongRect& operator*=(const double scalar)
   {
      left*=scalar;
      top*=scalar;
      right*=scalar;
      bottom*=scalar;
      return *this;
   }

   LongRect& operator/=(const LongRect& rc)
   {
      left/=rc.left;
      top/=rc.top;
      right/=rc.right;
      bottom/=rc.bottom;
      return *this;
   }

   LongRect operator+(const LongRect& rc) const
   {
      return LongRect(*this)+=rc;
   }

   LongRect operator-(const LongRect& rc) const
   {
      return LongRect(*this)-=rc;
   }

   LongRect operator*(const LongRect& rc) const
   {
      return LongRect(*this)*=rc;
   }

   LongRect operator/(const LongRect& rc) const
   {
      return LongRect(*this)/=rc;
   }

   LongRect& operator+=(const LongPoint& pt)
   {
      left+=pt.x;
      top+=pt.y;
      right+=pt.x;
      bottom+=pt.y;
      return *this;
   }

   LongRect& operator-=(const LongPoint& pt)
   {
      left-=pt.x;
      top-=pt.y;
      right-=pt.x;
      bottom-=pt.y;
      return *this;
   }

   LongRect& operator*=(const LongPoint& pt)
   {
      left*=pt.x;
      top*=pt.y;
      right*=pt.x;
      bottom*=pt.y;
      return *this;
   }

   LongRect& operator/=(const LongPoint& pt)
   {
      left/=pt.x;
      top/=pt.y;
      right/=pt.x;
      bottom/=pt.y;
      return *this;
   }

   LongRect operator+(const LongPoint& pt) const
   {
      return LongRect(*this)+=pt;
   }

   LongRect operator-(const LongPoint& pt) const
   {
      return LongRect(*this)-=pt;
   }

   LongRect operator*(const LongPoint& pt) const
   {
      return LongRect(*this)*=pt;
   }

   LongRect operator/(const LongPoint& pt) const
   {
      return LongRect(*this)/=pt;
   }

   LongRect operator-() const
   {
      return LongRect(-left,-top,-right,-bottom);
   }

   bool operator==(const LongRect& rc) const
   {
      return left==rc.left && top==rc.top &&
             right==rc.right && bottom==rc.bottom;
   }

   bool operator!=(const LongRect& rc) const
   {
      return !(*this==rc);
   }

   LongRect& operator&=(const LongRect& rt);
   LongRect& operator|=(const LongRect& rt);
   LongRect& operator|=(const LongPoint& p);

   LongRect operator&(const LongRect& rt) const
   {
      return LongRect(*this)&=rt;
   }

   LongRect operator|(const LongRect& rt) const
   {
      return LongRect(*this)|=rt;
   }

   void Clear()
   {
      left=FixedZero;
      top=FixedZero;
      right=FixedZero;
      bottom=FixedZero;
   }

   void Set(Coord left_, Coord top_, Coord right_, Coord bottom_)
   {
      left = left_; top = top_; right = right_; bottom = bottom_;
   }

   void Set(const LongPoint& topLeft_, Coord width, Coord height)
   {
      left=topLeft_.x;
      top=topLeft_.y;
      right=left+width;
      bottom=top+height;
   }

   void Set(const LongPoint& topLeft_, const LongPoint& bottomRight_)
   {
      ((LongPoint&)left)=topLeft_; ((LongPoint&)right)=bottomRight_;
   }

   void SetToBound (const LongPoint& a, const LongPoint& b);

   void Offset(Coord dx, Coord dy)
   {
      left+=dx; top+=dy;
      right+=dx; bottom+=dy;
   }

   void Offset(const LongPoint& delta)
   {
      *this+=delta;
   }

   void Inset(WrFixed dx, WrFixed dy)
   {
      left+=dx; top+=dy;
      right-=dx; bottom-=dy;
   }

   void Inset(const LongPoint& delta)
   {
      left+=delta.x; top+=delta.y;
      right-=delta.x; bottom-=delta.y;
   }

   void Inset(const LongRect& delta)
   {
      left += delta.left;
      top += delta.top;
      right -= delta.right;
      bottom -= delta.bottom;
   }

   bool IsEmpty() const
   {
      return (right<left && bottom<top);
   }

   bool Empty() const { return IsEmpty(); }

   bool IsValid() const
   {
      return left<=right && top<=bottom;
   }

   bool Valid() const { return IsValid(); }

   void Validate();
   void Normalize() { Validate(); }

   Coord GetLength(XYSelect sel) const
   {
      return sel==ySel?bottom-top:right-left;
   }

   LongPoint GetSize() const
   {
      return LongPoint(right-left, bottom-top);
   }

   bool Contains(const LongPoint& pt) const
   {
      return pt.y >= top && pt.y <= bottom && pt.x >= left && pt.x <= right;
   }

   bool Contains(const LongRect& rc) const
   {
      return rc.top >= top && rc.bottom <= bottom && rc.left >= left && rc.right <= right;
   }

   LongPoint GetCorner ( const long Index ) const;
   // 0 for topLeft, 1 topRight , 2 botRight , 3 botLeft
   // N.B. the [] operator uses 1 as botRight, so incompatible
public:
   Coord left;
   Coord top;
   Coord right;
   Coord bottom;
public:
   static const LongRect kZeroRect;
};

inline CArchive& operator<<(CArchive& ar, const LongRect& rc)
{
   return ar << rc.left << rc.top << rc.right << rc.bottom;
}

inline CArchive& operator>>(CArchive& ar, LongRect& rc)
{
   return ar >> rc.left >> rc.top >> rc.right >> rc.bottom;
}

class LongBoundRect : public LongRect
{
public:
   LongBoundRect() : LongRect(
      WrFixedFromFixed(0x7FFFFFFF), WrFixedFromFixed(0x7FFFFFFF),
      WrFixedFromFixed(0x80000001), WrFixedFromFixed(0x80000001)) {}
};

#pragma pack()

typedef LongRect* LongRectPtr;

#endif



