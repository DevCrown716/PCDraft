// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDialogEx.h : header file
//
#include  "FileDialogW.h"

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx dialog

class CFileDialogExW : public CFileDialogW
{
   DECLARE_DYNAMIC(CFileDialogExW)

public:
   CFileDialogExW(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
      LPCWSTR lpszDefExt = NULL,
      LPCWSTR lpszFileName = NULL,
      DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      LPCWSTR lpszFilter = NULL,
      CWnd* pParentWnd = NULL);

protected:
   void UpdateFileExt();
   virtual void OnTypeChange();
   virtual BOOL OnFileNameOK();
   //{{AFX_MSG(CFileDialogEx)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

