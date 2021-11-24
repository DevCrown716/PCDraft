// GradientPreviewWnd.h : header file
// Infinisys Ltd
//
#pragma once

class CGradientDlg;
class CGradientPack;
/////////////////////////////////////////////////////////////////////////////
// CGradientPreviewWnd window

class CGradientPreviewWnd: public CWnd
{
   enum Constraints
   {
      kNone=0,
      kTest,
      kVertical,
      kHorizontal
   };

   friend class CGradientDlg;

   // Construction
public:
   CGradientPreviewWnd();

   // Attributes
public:
   int m_nConstraint;
   COLORREF m_BlendColor;
   int m_nEditY;
   int m_nEditX;
   int m_nY;
   int m_nX;
   void UpdateEditInfo();
   int m_nEditSize;
   int m_nSide;
   bool m_bTrack;
   bool m_bToggle;
   HCURSOR m_hThumb;
   CGradientDlg *    m_pParentDlg;
   CGradientPack *   m_ppack;
   void defer_update_center(CPoint point);
   // Operations
public:

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGradientPreviewWnd)
   //}}AFX_VIRTUAL

   // Implementation
public:
   void AverageColor(COLORREF src,COLORREF& dst);
   void UpdateColor();
   void UpdateThumbColor();
   virtual ~CGradientPreviewWnd();
   void SetParentDlg(CGradientDlg* parentDlg);
   bool ApplyTool();

   // Generated message map functions
protected:
   //{{AFX_MSG(CGradientPreviewWnd)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg BOOL OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

