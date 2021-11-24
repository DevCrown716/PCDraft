#pragma once
class memory_dc : 
   public CDC
{
public:

   memory_dc(CDC& dc, CWnd* pWnd);
   memory_dc(CDC& dc, const CRect& rect);

   virtual ~memory_dc();

protected:
   CDC&     m_dc;
   CBitmap  m_bmp;
   CBitmap* m_pOldBmp;
   CRect    m_rect;
};
