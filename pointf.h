#pragma once


struct POINTD
{

   double x;
   double y;

};

class point : 
   public CPoint
{
public:
   using CPoint::CPoint;


};


class pointd;

class pointf :
   public Gdiplus::PointF
{
public:

   using Gdiplus::PointF::PointF;
   pointf() : Gdiplus::PointF(0.f, 0.f) {  }
   pointf(const Gdiplus::SizeF & s) : PointF(s.Width, s.Height) {}
   pointf(const Gdiplus::PointF & s) : PointF(s.X, s.Y) {}
   pointf(const POINTD & p) { X = (Gdiplus::REAL) p.x; Y = (Gdiplus::REAL)p.y; }
   pointf(const POINT & p) { X = (Gdiplus::REAL)p.x; Y = (Gdiplus::REAL)p.y;}
   pointf(const SIZE & s) { X = (Gdiplus::REAL)s.cx; Y = (Gdiplus::REAL)s.cy; }

   using Gdiplus::PointF::operator=;
   pointf & operator =(const POINT & p) { X = (Gdiplus::REAL)p.x; Y = (Gdiplus::REAL)p.y; return *this; }


   operator CPoint() const { return CPoint(LONG(X+0.5f), LONG(Y+0.5f)); }

   bool operator == (const POINT & p)const
   {

      return operator CPoint() == p;

   }

   bool operator != (const POINT & p)const
   {

      return !operator == (p);

   }

   pointf operator -() const
   {

      return pointf( -X, -Y );

   }

   pointf operator -(const pointf & p) const
   {
      return pointf(X - p.X, Y - p.Y);

   }
   pointf operator +(const pointf & p) const
   {
      return pointf(X + p.X, Y + p.Y);

   }

   void Offset(FLOAT dx, FLOAT dy)
   {
      X += dx;
      Y += dy;

   }

};

// temporary / scaffolding Infinisys GDI+
inline CPoint __point(pointf p)
{
   return CPoint((long)p.X, (long)p.Y);

}
inline pointf __pointf(CPoint p)
{
   return pointf((FLOAT)p.x, (FLOAT)p.y);

}
inline pointf __pointf(POINT p)
{
   return pointf((FLOAT)p.x, (FLOAT)p.y);

}

inline pointf & operator += (pointf &p1, const pointf &p2)
{
   p1.X += p2.X;
   p1.Y += p2.Y;
   return p1;

}
inline pointf & operator += (pointf &p1, CPoint &p2)
{
   p1.X += (FLOAT)p2.x;
   p1.Y += (FLOAT)p2.y;
   return p1;

}


inline pointf & operator -= (pointf &p1, const pointf &p2)
{
   p1.X -= p2.X;
   p1.Y -= p2.Y;
   return p1;

}
//inline pointf & operator -= (pointf &p1, CPoint &p2)
//{
//   p1.x -= (FLOAT)p2.x;
//   p1.y -= (FLOAT)p2.y;
//   return p1;
//
//}

// (Precision/Floating number comparison issue) temporary / scaffolding Infinisys GDI+ 
//inline bool operator ==(const pointf & p1, const pointf & p2)
//{
//
//   return p1.x == p2.x && p1.y == p2.y;
//
//}
//
//inline bool operator ==(const CPoint & p1, const CPoint & p2)
//{
//
//   return p1 == p2);
//
//}
//
//inline bool operator !=(const CPoint & p1, const CPoint & p2)
//{
//
//   return !(p1 == p2);
//
//}
//
//inline bool operator ==(const CPoint & p1, const pointf & p2)
//{
//
//   return p1.x == (LONG)p2.x && p1.x == (LONG)p2.y;
//
//}
//
//inline bool operator !=(const CPoint & p1, const pointf & p2)
//{
//
//   return !(p1 == p2);
//
//}

//inline bool operator ==(const pointf & p1, const CPoint & p2)
//{
//
//   return (LONG)p1.x == p2.x && (LONG)p1.y == p2.y;
//
//}
//
//inline bool operator !=(const pointf & p1, const CPoint & p2)
//{
//
//   return !(p1 == p2);
//
//}



inline void __copy(POINT * p, const Gdiplus::PointF * p1, int c)
{

   while (c > 0)
   {

      p->x = (LONG) p1->X;
      p->y = (LONG) p1->Y;
      c--;
      p++;
      p1++;

   }


}


inline void __copy(Gdiplus::PointF * p, const POINT * p1, int c)
{

   while (c > 0)
   {

      p->X = (Gdiplus::REAL)p1->x;
      p->Y = (Gdiplus::REAL)p1->y;
      c--;
      p++;
      p1++;

   }


}



