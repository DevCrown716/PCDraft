#if !defined(AFX_EDITPATTERNDLG_H__C4A92262_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_EDITPATTERNDLG_H__C4A92262_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditPatternDlg.h : header file
//

#include "EditPatternWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CEditPatternDlg dialog

class CEditPatternDlg : public CDialog
{
   enum
   {
      CDPF_SIZE=1,
      CDPF_COLORS=2,
      CDPF_FLIPV=4,
      CDPF_FLIPH=8,
      CDPF_ROP=16
   };
   struct CopyDIBParams
   {
      int nFlags;
      int nNewSize;
      bool bNewBW;
      DWORD dwROP;
   public:
   };
// Construction
public:

   void SetColor(COLORREF color);
   COLORREF m_Color;
   bool m_bIsBW;
   HBITMAP *m_phPattern;
   HBITMAP m_hEditPattern;
   HBITMAP m_hUndo;
   int m_nTool;
   int m_nSize;
   CEditPatternDlg(HBITMAP *phDib, CWnd* pParent = NULL);   // standard constructor
   void CopyDIB(HBITMAP& hDstDib,HBITMAP& hSrcDib,CopyDIBParams *pp=NULL);
   void SaveUndo(bool bAsCopy=true);
   void Colorize(COLORREF color);
   void UpdateEditInfo();

// Dialog Data
   //{{AFX_DATA(CEditPatternDlg)
   enum { IDD = IDD_EDIT_PATTERN };
   CStatic	m_color;
   CButton	m_toolPen;
   CButton	m_preview;
   CButton	m_scale;
   CButton	m_invert;
   CButton	m_flipVert;
   CButton	m_flipHorz;
   CButton	m_bw;
   CButton	m_revert;
   CButton	m_undo;
   //}}AFX_DATA
   CEditPatternWnd	m_edit;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CEditPatternDlg)
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CEditPatternDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnBlackWhite();
   afx_msg void OnClear();
   afx_msg void OnFill();
   afx_msg void OnFlipHorizontal();
   afx_msg void OnFlipVertical();
   afx_msg void OnInvert();
   afx_msg void OnOther();
   afx_msg void OnPattern();
   afx_msg void OnRevert();
   afx_msg void OnUndo();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   virtual void OnOK();
   afx_msg void OnColor();
   afx_msg void OnPaint();
   //}}AFX_MSG
   afx_msg	void OnToolsRange(UINT nID);
   afx_msg	void OnSizeRange(UINT nID);
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnStnClickedEditPlaceholder(); // Pattern/Gradient - Infinisys Ltd
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPATTERNDLG_H__C4A92262_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
