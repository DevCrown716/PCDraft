// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen#include "stdafx.h"
#include "stdafx.h"
#include "../pcdraft.h"
#include "FileDialogExW.h"
#include "FileStatusW.h"

#include <dlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int AFXAPI AfxMessageBoxW(LPCWSTR lpszText, UINT nType, UINT nIDHelp = 0)
{
   CPCDraftApp* pApp = (CPCDraftApp *)AfxGetApp();
   if (pApp != NULL)
   {
      return pApp->DoMessageBoxW(lpszText, nType, nIDHelp);
   }
   else
   {
      return CPCDraftApp::ShowAppMessageBoxW(NULL, lpszText, nType, nIDHelp);
   }
}

int AFXAPI AfxMessageBoxW(UINT nIDPrompt, UINT nType, UINT nIDHelp)
{
   CStringW string;
   if (!string.LoadString(nIDPrompt))
   {
      TRACE(traceAppMsg, 0, "Error: failed to load message box prompt string 0x%04x.\n",
         nIDPrompt);
      ASSERT(FALSE);
   }
   if (nIDHelp == (UINT)-1)
      nIDHelp = nIDPrompt;
   return AfxMessageBoxW(string, nType, nIDHelp);
}




/////////////////////////////////////////////////////////////////////////////
// CFileDialogExW

IMPLEMENT_DYNAMIC(CFileDialogExW, CFileDialogW)

CFileDialogExW::CFileDialogExW(BOOL bOpenFileDialog, LPCWSTR lpszDefExt, LPCWSTR lpszFileName,
   DWORD dwFlags, LPCWSTR lpszFilter, CWnd* pParentWnd) :
   CFileDialogW(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, 0)
{
}

BEGIN_MESSAGE_MAP(CFileDialogExW, CFileDialogW)
   //{{AFX_MSG_MAP(CFileDialogExW)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFileDialogExW::OnFileNameOK()
{
   if (!m_bOpenFileDialog)
   {
      if (GetFileExt().IsEmpty())
      {
         UpdateFileExt();
         if (m_ofn.Flags & OFN_OVERWRITEPROMPT)
         {
            CStringW thePathName = GetPathName();
            CFileStatusW theStatus;
            if (FileGetStatus(thePathName, theStatus))
            {
               CStringW thePrompt;
               thePrompt.Format(IDS_OVERWRITEPROMPT, thePathName);
               return AfxMessageBoxW(thePrompt, MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) == IDYES ? 0 : 1;
            }
         }
      }
   }
   return 0;
}

void CFileDialogExW::UpdateFileExt()
{
   if (!m_bOpenFileDialog)
   {
      CStringW theFileName = GetFileTitle();
      LPCWSTR p = m_ofn.lpstrFilter, pszExt;
      for (DWORD i = 0; i < m_ofn.nFilterIndex; i++)
      {
         p += wcslen(p) + 1;
         pszExt = p + 1;
         p += wcslen(p) + 1;
      }
      theFileName += pszExt;
      SetControlText(edt1, theFileName);
   }
}

void CFileDialogExW::OnTypeChange()
{
   UpdateFileExt();
}
