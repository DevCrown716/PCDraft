#pragma once


// OptionsAutoSaveDialog dialog

class OptionsAutoSaveDialog : public CDialogEx
{
	DECLARE_DYNAMIC(OptionsAutoSaveDialog)

public:
	OptionsAutoSaveDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~OptionsAutoSaveDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_AUTO_SAVE };
#endif


   void UpdateAutoSaveDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedAutoSaveOff();
   CButton m_btnAutoSaveOn;
   CButton m_btnAutoSaveOff;
   CEdit m_editAutoSaveMinutes;
   int m_iAutoSaveMinutes;
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedAutoSaveOn();
   virtual void OnOK();
};
