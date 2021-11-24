#pragma once




class pointd :
   public POINTD
{
public:

   pointd() { x = 0.; y = 0.; }
   pointd(double x0, double y0) { x = x0; y = y0; }
   pointd(const Gdiplus::SizeF & s) : pointd(s.Width, s.Height) {}
   pointd(const Gdiplus::PointF & s) : pointd(s.X, s.Y) {}
   pointd(const POINT & p) { x = p.x; y = p.y; }
   pointd(const SIZE & s) { x = s.cx; y = s.cy; }

   pointd & operator =(const POINTD & p) { x = p.x; y = p.y; return *this; }

   pointd & operator =(const POINT & p) { x = p.x; y = p.y; return *this; }


   operator CPoint() const { return CPoint(LONG(x + 0.5f), LONG(y + 0.5f)); }

   bool operator == (const POINT & p)const
   {

      return operator CPoint() == p;

   }

   bool operator != (const POINT & p)const
   {

      return !operator == (p);

   }

   pointd operator -() const
   {

      return pointd(-x, -y);

   }

   pointd operator -(const pointd & p) const
   {
      return pointd(x - p.x, y - p.y);

   }
   pointd operator +(const pointd & p) const
   {
      return pointd(x + p.x, y + p.y);

   }
   pointd operator /(double d) const
   {
      return pointd(x /d, y /d);

   }

   void Offset(FLOAT dx, FLOAT dy)
   {
      x += dx;
      y += dy;

   }


};

// temporary / scaffolding Infinisys GDI+
inline CPoint __point(pointd p)
{
   return CPoint((long)p.x, (long)p.y);

}
inline pointd __pointd(CPoint p)
{
   return pointd((double)p.x, (double)p.y);

}
inline pointf __pointf(pointd p)
{
   return pointf((FLOAT)p.x, (FLOAT)p.y);
}




inline pointd & operator += (pointd &p1, const pointd &p2)
{
   p1.x += p2.x;
   p1.y += p2.y;
   return p1;

}
inline pointd & operator += (pointd &p1, CPoint &p2)
{
   p1.x += (FLOAT)p2.x;
   p1.y += (FLOAT)p2.y;
   return p1;

}


inline pointd & operator -= (pointd &p1, const pointd &p2)
{
   p1.x -= p2.x;
   p1.y -= p2.y;
   return p1;

}
inline void __copy(POINTD * p, const Gdiplus::PointF * p1, int c)
{

   while (c > 0)
   {

      p->x = p1->X;
      p->y = p1->Y;
      c--;
      p++;
      p1++;

   }


}


inline void __copy(Gdiplus::PointF * p, const POINTD * p1, int c)
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






inline void __copy(POINTD * p, const POINT * p1, int c)
{

   while (c > 0)
   {

      p->x = p1->x;
      p->y = p1->y;
      c--;
      p++;
      p1++;

   }


}


inline void __copy(POINT * p, const POINTD * p1, int c)
{

   while (c > 0)
   {

      p->x = (LONG) p1->x;
      p->y = (LONG) p1->y;
      c--;
      p++;
      p1++;

   }


}



