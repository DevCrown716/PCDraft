// GradientEditWnd.h : header file
// Infinisys Ltd
//
#pragma once


class CGradientDlg;
class CGradientPack;
/////////////////////////////////////////////////////////////////////////////
// CGradientEditWnd window

class CGradientEditWnd: public CWnd
{
public:

   DWORD             m_dwLButtonDownStart;
   CDC               m_dc;
   CBitmap           m_bitmap;
   HGDIOBJ           m_hOld;
   int               m_iSquareSize;
   int               m_iDrag;
   int               m_iMin;
   int               m_iMax;
   int               m_iDragMin;
   int               m_iDragMax;
   int               m_bMove;
   CGradientDlg *    m_pParentDlg;


   CGradientEditWnd();
   virtual ~CGradientEditWnd();

   double calc_rate(CPoint pt);
   CPoint calc_point(double dRate);
   void get_between_rect(int i,LPRECT lprect);
   void get_rect(int i,LPRECT lprect);
   int hit_test(CPoint pt);
   int between_hit_test(CPoint pt);
   void move_rate(CPoint pt);

   //{{AFX_VIRTUAL(CGradientEditWnd)
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(CGradientEditWnd)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


