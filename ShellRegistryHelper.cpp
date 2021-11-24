#include "stdafx.h"
#include "ShellRegistryHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

static const TCHAR gShellOpenFmt[] = _T("%s\\shell\\open\\%s");
static const TCHAR gShellPrintFmt[] = _T("%s\\shell\\print\\%s");
static const TCHAR gShellPrintToFmt[] = _T("%s\\shell\\printto\\%s");
static const TCHAR gDefaultIconFmt[] = _T("%s\\DefaultIcon");
static const TCHAR gShellNewFmt[] = _T("%s\\ShellNew");

#define DEFAULT_ICON_INDEX 0

static const TCHAR gIconIndexFmt[] = _T(",%d");
static const TCHAR gCommand[] = _T("command");
static const TCHAR gOpenArg[] = _T(" \"%1\"");
static const TCHAR gPrintArg[] = _T(" /p \"%1\"");
static const TCHAR gPrintToArg[] = _T(" /pt \"%1\" \"%2\" \"%3\" \"%4\"");
static const TCHAR gDDEArg[] = _T(" /dde");

static const TCHAR gDDEExec[] = _T("ddeexec");
static const TCHAR gDDEOpen[] = _T("[open(\"%1\")]");
static const TCHAR gDDEPrint[] = _T("[print(\"%1\")]");
static const TCHAR gDDEPrintTo[] = _T("[printto(\"%1\",\"%2\",\"%3\",\"%4\")]");
static const TCHAR gApplication[] = _T("\\Application");


static const TCHAR gShellNewValueName[] = _T("NullFile");
static const TCHAR gShellNewValue[] = _T("");

static BOOL MySetRegKey(LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL)
{
	if (lpszValueName == NULL)
	{
		if (::RegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ,
			  lpszValue, lstrlen(lpszValue) * sizeof(TCHAR)) != ERROR_SUCCESS)
		{
			TRACE1("Warning: registration database update failed for key '%s'.\n",
				lpszKey);
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		HKEY hKey;

		if(::RegCreateKey(HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS)
		{
			LONG lResult = ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ,
				(CONST BYTE*)lpszValue, (lstrlen(lpszValue) + 1) * sizeof(TCHAR));

			if(::RegCloseKey(hKey) == ERROR_SUCCESS && lResult == ERROR_SUCCESS)
				return TRUE;
		}
		TRACE1("Warning: registration database update failed for key '%s'.\n", lpszKey);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
void RegisterFileExt(UINT nID,LPCTSTR lpszExt)
{
	CString strFullString;
	VERIFY(strFullString.LoadString(nID));
	RegisterFileExt(strFullString,lpszExt);
}

void RegisterFile(UINT nID,int nIconIndex,int nFlags,LPCTSTR lpszAppName)
{
	CString strFullString;
	VERIFY(strFullString.LoadString(nID));
	RegisterFile(strFullString,nIconIndex,nFlags,lpszAppName);
}

void RegisterFileExt(LPCTSTR lpszFullString,LPCTSTR lpszExt)
{
	CString theFileType;
	if(AfxExtractSubString(theFileType,lpszFullString,CDocTemplate::regFileTypeId))
	{
		::RegSetValue(HKEY_CLASSES_ROOT, lpszExt, REG_SZ, theFileType, theFileType.GetLength() * sizeof(TCHAR));
	}
}

void RegisterFile(LPCTSTR lpszFullString,int nIconIndex,int nFlags,LPCTSTR lpszAppName)
{
	TCHAR szLongPathName[_MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szLongPathName, _MAX_PATH);
	CString strTemp, strPathName=szLongPathName;
//	strPathName="\""+strPathName+"\"";	// add quotation

	CString strOpenCommandLine = strPathName;
	CString strPrintCommandLine = strPathName;
	CString strPrintToCommandLine = strPathName;
	CString strDefaultIconCommandLine = strPathName;

	CString strIconIndex;
/*	HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(), strPathName, nIconIndex);
	if (hIcon != NULL)
	{
*/		strIconIndex.Format(gIconIndexFmt, nIconIndex);
/*		DestroyIcon(hIcon);
	}
	else
	{
		strIconIndex.Format(gIconIndexFmt, DEFAULT_ICON_INDEX);
	}
*/	strDefaultIconCommandLine += strIconIndex;

	CString strFilterExt, strFileTypeId, strFileTypeName;
	if (AfxExtractSubString(strFileTypeId, lpszFullString, CDocTemplate::regFileTypeId) && !strFileTypeId.IsEmpty())
	{
		// enough info to register it
		if (!AfxExtractSubString(strFileTypeName, lpszFullString, CDocTemplate::regFileTypeName))
			strFileTypeName = strFileTypeId;    // use id name

		ASSERT(strFileTypeId.Find(' ') == -1);  // no spaces allowed

		// first register the type ID of our server
		if (!MySetRegKey(strFileTypeId, strFileTypeName))
			return;       // just skip it

		// path\DefaultIcon = path,1
		strTemp.Format(gDefaultIconFmt, (LPCTSTR)strFileTypeId);
		if (!MySetRegKey(strTemp, strDefaultIconCommandLine))
			return;       // just skip it

		// If MDI Application
		if (!AfxExtractSubString(strTemp, lpszFullString, CDocTemplate::windowTitle) ||
			strTemp.IsEmpty())
		{
			if(nFlags & kRegisterOpen)
			{
				// path\shell\open\ddeexec = [open("%1")]
				strTemp.Format(gShellOpenFmt, (LPCTSTR)strFileTypeId,
					(LPCTSTR)gDDEExec);
				if (!MySetRegKey(strTemp, gDDEOpen))
					return;
				if(lpszAppName)
				{
					strTemp=strTemp+gApplication;
					MySetRegKey(strTemp, lpszAppName);
				}
			}

			if(nFlags & kRegisterPrint)
			{
				// path\shell\print\ddeexec = [print("%1")]
				strTemp.Format(gShellPrintFmt, (LPCTSTR)strFileTypeId,
					(LPCTSTR)gDDEExec);
				if (!MySetRegKey(strTemp, gDDEPrint))
					return;       // just skip it
				if(lpszAppName)
				{
					strTemp=strTemp+gApplication;
					MySetRegKey(strTemp, lpszAppName);
				}

				// path\shell\printto\ddeexec = [printto("%1","%2","%3","%4")]
				strTemp.Format(gShellPrintToFmt, (LPCTSTR)strFileTypeId,
					(LPCTSTR)gDDEExec);
				if (!MySetRegKey(strTemp, gDDEPrintTo))
					return;       // just skip it
				if(lpszAppName)
				{
					strTemp=strTemp+gApplication;
					MySetRegKey(strTemp, lpszAppName);
				}
			}

			// path\shell\open\command = path /dde
			// path\shell\print\command = path /dde
			// path\shell\printto\command = path /dde
			strOpenCommandLine += gDDEArg;
			strPrintCommandLine += gDDEArg;
			strPrintToCommandLine += gDDEArg;
		}
		else
		{
			// path\shell\open\command = path filename
			// path\shell\print\command = path /p filename
			// path\shell\printto\command = path /pt filename printer driver port
			strOpenCommandLine += gOpenArg;
			strPrintCommandLine += gPrintArg;
			strPrintToCommandLine += gPrintToArg;
		}

		if(nFlags & kRegisterOpen)
		{
			// path\shell\open\command = path filename
			strTemp.Format(gShellOpenFmt, (LPCTSTR)strFileTypeId,
				(LPCTSTR)gCommand);
			if (!MySetRegKey(strTemp, strOpenCommandLine))
				return;       // just skip it
		}

		if(nFlags & kRegisterPrint)
		{
			// path\shell\print\command = path /p filename
			strTemp.Format(gShellPrintFmt, (LPCTSTR)strFileTypeId,
				(LPCTSTR)gCommand);
			if (!MySetRegKey(strTemp, strPrintCommandLine))
				return;       // just skip it

			// path\shell\printto\command = path /pt filename printer driver port
			strTemp.Format(gShellPrintToFmt, (LPCTSTR)strFileTypeId,
				(LPCTSTR)gCommand);
			if (!MySetRegKey(strTemp, strPrintToCommandLine))
				return;       // just skip it
		}

		AfxExtractSubString(strFilterExt, lpszFullString, CDocTemplate::filterExt);
		if (!strFilterExt.IsEmpty())
		{
			ASSERT(strFilterExt[0] == '.');

			LONG lSize = _MAX_PATH * 2;
			LONG lResult = ::RegQueryValue(HKEY_CLASSES_ROOT, strFilterExt,
				strTemp.GetBuffer(lSize), &lSize);
			strTemp.ReleaseBuffer();

			if (lResult != ERROR_SUCCESS || strTemp.IsEmpty() ||
				strTemp == strFileTypeId)
			{
				// no association for that suffix
				if (!MySetRegKey(strFilterExt, strFileTypeId))
					return;

				if(nFlags & kRegisterNew)
				{
					strTemp.Format(gShellNewFmt, (LPCTSTR)strFilterExt);
					(void)MySetRegKey(strTemp, gShellNewValue, gShellNewValueName);
				}
			}
		}
	}
}

void RegisterFileExt(CDocTemplate* pTemplate,LPCTSTR lpszExt)
{
	CString theFileType;
	if(pTemplate->GetDocString(theFileType,CDocTemplate::regFileTypeId))
	{
		::RegSetValue(HKEY_CLASSES_ROOT, lpszExt, REG_SZ, theFileType, theFileType.GetLength() * sizeof(TCHAR));
	}
}

void RegisterFile(CDocTemplate* pTemplate,int nIconIndex,int nFlags,LPCTSTR lpszAppName)
{
	TCHAR szLongPathName[_MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szLongPathName, _MAX_PATH);
	CString strTemp, strPathName=szLongPathName;
//	strPathName="\""+strPathName+"\"";	// add quotation

	CString strOpenCommandLine = strPathName;
	CString strPrintCommandLine = strPathName;
	CString strPrintToCommandLine = strPathName;
	CString strDefaultIconCommandLine = strPathName;

	CString strIconIndex;
/*	HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(), strPathName, nIconIndex);
	if (hIcon != NULL)
	{
*/		strIconIndex.Format(gIconIndexFmt, nIconIndex);
/*		DestroyIcon(hIcon);
	}
	else
	{
		strIconIndex.Format(gIconIndexFmt, DEFAULT_ICON_INDEX);
	}
*/	strDefaultIconCommandLine += strIconIndex;

	CString strFilterExt, strFileTypeId, strFileTypeName;
	if (pTemplate->GetDocString(strFileTypeId, CDocTemplate::regFileTypeId) && !strFileTypeId.IsEmpty())
	{
		// enough info to register it
		if (!pTemplate->GetDocString(strFileTypeName,
		   CDocTemplate::regFileTypeName))
			strFileTypeName = strFileTypeId;    // use id name

		ASSERT(strFileTypeId.Find(' ') == -1);  // no spaces allowed

		// first register the type ID of our server
		if (!MySetRegKey(strFileTypeId, strFileTypeName))
			return;       // just skip it

		// path\DefaultIcon = path,1
		strTemp.Format(gDefaultIconFmt, (LPCTSTR)strFileTypeId);
		if (!MySetRegKey(strTemp, strDefaultIconCommandLine))
			return;       // just skip it

		// If MDI Application
		if (!pTemplate->GetDocString(strTemp, CDocTemplate::windowTitle) ||
			strTemp.IsEmpty())
		{
			if(nFlags & kRegisterOpen)
			{
				// path\shell\open\ddeexec = [open("%1")]
				strTemp.Format(gShellOpenFmt, (LPCTSTR)strFileTypeId,
					(LPCTSTR)gDDEExec);
				if (!MySetRegKey(strTemp, gDDEOpen))
					return;
				if(lpszAppName)
				{
					strTemp=strTemp+gApplication;
					MySetRegKey(strTemp, lpszAppName);
				}
			}

			if(nFlags & kRegisterPrint)
			{
				// path\shell\print\ddeexec = [print("%1")]
				strTemp.Format(gShellPrintFmt, (LPCTSTR)strFileTypeId,
					(LPCTSTR)gDDEExec);
				if (!MySetRegKey(strTemp, gDDEPrint))
					return;       // just skip it
				if(lpszAppName)
				{
					strTemp=strTemp+gApplication;
					MySetRegKey(strTemp, lpszAppName);
				}

				// path\shell\printto\ddeexec = [printto("%1","%2","%3","%4")]
				strTemp.Format(gShellPrintToFmt, (LPCTSTR)strFileTypeId,
					(LPCTSTR)gDDEExec);
				if (!MySetRegKey(strTemp, gDDEPrintTo))
					return;       // just skip it
				if(lpszAppName)
				{
					strTemp=strTemp+gApplication;
					MySetRegKey(strTemp, lpszAppName);
				}
			}

			// path\shell\open\command = path /dde
			// path\shell\print\command = path /dde
			// path\shell\printto\command = path /dde
			strOpenCommandLine += gDDEArg;
			strPrintCommandLine += gDDEArg;
			strPrintToCommandLine += gDDEArg;
		}
		else
		{
			// path\shell\open\command = path filename
			// path\shell\print\command = path /p filename
			// path\shell\printto\command = path /pt filename printer driver port
			strOpenCommandLine += gOpenArg;
			strPrintCommandLine += gPrintArg;
			strPrintToCommandLine += gPrintToArg;
		}

		if(nFlags & kRegisterOpen)
		{
			// path\shell\open\command = path filename
			strTemp.Format(gShellOpenFmt, (LPCTSTR)strFileTypeId,
				(LPCTSTR)gCommand);
			if (!MySetRegKey(strTemp, strOpenCommandLine))
				return;       // just skip it
		}

		if(nFlags & kRegisterPrint)
		{
			// path\shell\print\command = path /p filename
			strTemp.Format(gShellPrintFmt, (LPCTSTR)strFileTypeId,
				(LPCTSTR)gCommand);
			if (!MySetRegKey(strTemp, strPrintCommandLine))
				return;       // just skip it

			// path\shell\printto\command = path /pt filename printer driver port
			strTemp.Format(gShellPrintToFmt, (LPCTSTR)strFileTypeId,
				(LPCTSTR)gCommand);
			if (!MySetRegKey(strTemp, strPrintToCommandLine))
				return;       // just skip it
		}

		pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt);
		if (!strFilterExt.IsEmpty())
		{
			ASSERT(strFilterExt[0] == '.');

			LONG lSize = _MAX_PATH * 2;
			LONG lResult = ::RegQueryValue(HKEY_CLASSES_ROOT, strFilterExt,
				strTemp.GetBuffer(lSize), &lSize);
			strTemp.ReleaseBuffer();

			if (lResult != ERROR_SUCCESS || strTemp.IsEmpty() ||
				strTemp == strFileTypeId)
			{
				// no association for that suffix
				if (!MySetRegKey(strFilterExt, strFileTypeId))
					return;

				if(nFlags & kRegisterNew)
				{
					strTemp.Format(gShellNewFmt, (LPCTSTR)strFilterExt);
					(void)MySetRegKey(strTemp, gShellNewValue, gShellNewValueName);
				}
			}
		}
	}
}
