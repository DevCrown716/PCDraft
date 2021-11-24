// StarDialog.h : header file
// Infinisys Ltd

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CStarDialog dialog

class CStarDialog: public CDialog
{
   // Construction
public:
   CStarDialog(CWnd* pParent = NULL);   // standard constructor

   // Dialog Data
   //{{AFX_DATA(CStarDialog)
   enum { IDD = IDD_STAR };
   int		m_nSides;
   int      m_iThickness;
   //}}AFX_DATA


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CStarDialog)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CStarDialog)
   // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

