#pragma once

#include "AttribSettingsWnd.h"
#include "LineWidthWnd.h"
#include "ShowBmpWnd.h"
#include "EndmarksWnd.h"
#include "TransparencySettingsWnd.h"
#include "afxwin.h"

class CPCDraftView;
class CRibbonBar;
/////////////////////////////////////////////////////////////////////////////
// CRibbonProperties window


class CRibbonProperties: public CDialog
{
   // Construction
public:
   CRibbonProperties(CRibbonBar * pbar);
   HBRUSH m_brush;
   CRibbonBar * m_pbar;
   // Attributes
public:
   CPCDraftView * m_pview;
   CAttribSettingsWnd m_wndFillIcon;
   CAttribSettingsWnd m_wndPFillIcon;
   CAttribSettingsWnd m_wndPenIcon;
   CAttribSettingsWnd m_wndFill;
   CAttribSettingsWnd m_wndPFill;
   CAttribSettingsWnd m_wndPen;
   CAttribSettingsWnd m_wndFillTransparency; // Opacity/Transparency - Infinisys Ltd
   CAttribSettingsWnd m_wndPFillTransparency; // Opacity/Transparency - Infinisys Ltd
   CAttribSettingsWnd m_wndPenTransparency; // Opacity/Transparency - Infinisys Ltd
   CLineWidthWnd m_wndLineWidth;
   CShowBmpWnd m_wndLineStyle;
   CEndmarksWnd m_wndEndmarks;
   CSepWnd m_wndSep1; // Opacity/Transparency - Infinisys Ltd
   CSepWnd m_wndSep2; // Opacity/Transparency - Infinisys Ltd
   CSepWnd m_wndSep3; // Opacity/Transparency - Infinisys Ltd
   bool m_bShowParallel;
   CFont m_font;
   Gdiplus::Font m_font2;
   enum { IDD = IDD_RIBBON_PROPERTIES };
   // Operations
public:

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CRibbonProperties)
   //}}AFX_VIRTUAL

   // Implementation
public:
   BOOL Create(CWnd * pParentWnd);
   virtual ~CRibbonProperties();

   // Generated message map functions
protected:
   //virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual LRESULT WindowProc(UINT nMsg,WPARAM wParam,LPARAM lParam);
   afx_msg void OnNcLButtonDown(UINT n,CPoint pt);
   afx_msg void OnPaint();
   afx_msg void OnNcPaint();
   void OffsetWindow(UINT nID,int nDelta);
   void ShowHideParallel();
   afx_msg LRESULT OnMaximizeClick(WPARAM wParam,LPARAM lParam);
   //{{AFX_MSG(CRibbonProperties)
   //}}AFX_MSG
   // Opacity/Transparency - Infinisys Ltd
   afx_msg void OnOK();
   void DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP()
   void OnUpdateCmdUI(CFrameWnd* pTarget,BOOL bDisableIfNoHndler);


public:
   afx_msg HBRUSH OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor);
protected:
   afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam,LPARAM lParam);
public:
   virtual BOOL OnInitDialog();
};

