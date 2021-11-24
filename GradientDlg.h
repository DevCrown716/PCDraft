// GradientDlg.h : header file
// Infinisys Ltd
//
#pragma once

#include "DimStructs.h"
#include "lpoint.h"
#include "GradientEditWnd.h"
#include "GradientPreviewWnd.h"
#include "GradientPack.h"

class CPCDraftDoc;

class CGradientDlg: public CDialog
{
public:

   enum { IDD = IDD_EDIT_GRADIENT };

   CStatic	            m_stPlaceHolder1;
   CStatic	            m_stRotation;
   CStatic	            m_stMoveCenter;
   CButton	            m_buttonApplyToSelected;
   CComboBox	         m_comboGradientType;
   CButton	            m_checkPreview;
   CEdit	               m_editRotation;
   CGradientEditWnd     m_editwnd;
   CGradientPreviewWnd  m_previewwnd;

   CGradientPack        m_packCopy;
   CGradientPack        m_pack;
   CGradientPack *      m_ppackRef;
   CGradientPack *      m_ppack;
   bool                 m_bPreview;
   bool                 m_bEdit;
   CPCDraftDoc *        m_pDoc;


   CGradientDlg(CPCDraftDoc* pDoc,CGradientPack * ppack,bool bEdit,CWnd* pParent = NULL); 


   void defer_redraw(bool bForcePreview = false);
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support



   void on_update_gradient_type();
   void UpdateUserDefinedEdit();

   virtual BOOL OnInitDialog();
   virtual void OnOK();

   afx_msg void OnBnClickedCheckPreview();
   afx_msg void OnCbnSelchangeGradientType();
   afx_msg void OnBnClickedCancel();
   afx_msg void OnEnChangeEditRotation();
   afx_msg void OnBnClickedApplyToSelected();

   DECLARE_MESSAGE_MAP()

};
