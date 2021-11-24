#pragma once


#include "PCDraftDoc.h"
#include "NumberEdit.h"

// AddGuideByPositionDlg dialog

class AddGuideByPositionDlg : public CDialog
{
	DECLARE_DYNAMIC(AddGuideByPositionDlg)

public:

   CGuide    &    m_guide;
   CPCDraftDoc *  m_pdoc;
   CNumberEdit    m_edit;
   CStatic m_staticWarning;

	AddGuideByPositionDlg(CPCDraftDoc* pdoc, CGuide & guide, CWnd* pParent = nullptr);   // standard constructor
	virtual ~AddGuideByPositionDlg();

   void SetVertical(bool bVertical = true);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_GUIDE_BY_POSITION };
#endif

   void validate(bool & bOk, bool & bValid);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHorizontal();
   afx_msg void OnBnClickedVertical();
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   afx_msg LRESULT OnMessage123(WPARAM wparam, LPARAM lparam);
};
