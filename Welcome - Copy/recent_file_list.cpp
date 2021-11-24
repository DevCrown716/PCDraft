#include "StdAfx.h"
#include "sal.h"
#include "recent_file_list.h"

#include <ShlObj.h>
#include <atlconv.h>

CStringW _GetProfileStringW(CWinApp * papp, LPCTSTR lpszSection, LPCTSTR lpszEntry,
   LPCWSTR lpszDefault)
{
   ASSERT(lpszSection != NULL);
   ASSERT(lpszEntry != NULL);
   if (papp->m_pszRegistryKey != NULL)
   {
      HKEY hSecKey = papp->GetSectionKey(lpszSection);
      if (hSecKey == NULL)
         return lpszDefault;
      CStringW strValue;
      DWORD dwType = REG_NONE;
      DWORD dwCount = 0;
      LONG lResult = RegQueryValueExW(hSecKey, (LPWSTR)(LPCWSTR)(get_uni(lpszEntry)), NULL, &dwType,
         NULL, &dwCount);
      if (lResult == ERROR_SUCCESS)
      {
         ASSERT(dwType == REG_SZ);
         lResult = RegQueryValueExW(hSecKey, (LPWSTR)(LPCWSTR)get_uni(lpszEntry), NULL, &dwType,
            (LPBYTE)strValue.GetBuffer(dwCount / sizeof(WCHAR)), &dwCount);
         strValue.ReleaseBuffer();
      }
      RegCloseKey(hSecKey);
      if (lResult == ERROR_SUCCESS)
      {
         ASSERT(dwType == REG_SZ);
         return strValue;
      }
      return lpszDefault;
   }
   else
   {
      ASSERT(papp->m_pszProfileName != NULL);

      if (lpszDefault == NULL)
         lpszDefault = L"";	// don't pass in NULL
      WCHAR szT[4096];
      DWORD dw = ::GetPrivateProfileStringW(get_uni(lpszSection), get_uni(lpszEntry),
         lpszDefault, szT, _countof(szT), get_uni(papp->m_pszProfileName));
      ASSERT(dw < 4095);
      return szT;
   }
}

UINT AFXAPI AfxGetFileTitle2(LPCWSTR lpszPathName, _Out_writes_(nMax) LPWSTR lpszTitle, UINT nMax);
HRESULT _AfxSHCreateItemFromParsingName(__in PCWSTR pszPath, __in IBindCtx *pbc, __in REFIID riid, __out void **ppv);

UINT AFXAPI AfxGetFileName2(LPCWSTR lpszPathName, _Out_writes_opt_(nMax) LPWSTR lpszTitle, UINT nMax);

/////////////////////////////////////////////////////////////////////////////
// lpszCanon = C:\MYAPP\DEBUGS\C\TESWIN.C
//
// cchMax   b   Result
// ------   -   ---------
//  1- 7    F   <empty>
//  1- 7    T   TESWIN.C
//  8-14    x   TESWIN.C
// 15-16    x   C:\...\TESWIN.C
// 17-23    x   C:\...\C\TESWIN.C
// 24-25    x   C:\...\DEBUGS\C\TESWIN.C
// 26+      x   C:\MYAPP\DEBUGS\C\TESWIN.C

AFX_STATIC void AFXAPI _AfxAbbreviateName(_Inout_z_ LPWSTR lpszCanon, int cchMax, BOOL bAtLeastName)
{
   ENSURE_ARG(AfxIsValidString(lpszCanon));

   int cchFullPath, cchFileName, cchVolName;
   const WCHAR* lpszCur;
   const WCHAR* lpszBase;
   const WCHAR* lpszFileName;

   lpszBase = lpszCanon;
   cchFullPath = AtlStrLen(lpszCanon);

   cchFileName = AfxGetFileName2(lpszCanon, NULL, 0) - 1;
   lpszFileName = lpszBase + (cchFullPath - cchFileName);

   // If cchMax is more than enough to hold the full path name, we're done.
   // This is probably a pretty common case, so we'll put it first.
   if (cchMax >= cchFullPath)
      return;

   // If cchMax isn't enough to hold at least the basename, we're done
   if (cchMax < cchFileName)
   {
      if (!bAtLeastName)
         lpszCanon[0] = L'\0';
      else
         Checked::wcscpy_s(lpszCanon, cchFullPath + 1, lpszFileName);
      return;
   }

   // Calculate the length of the volume name.  Normally, this is two characters
   // (e.g., "C:", "D:", etc.), but for a UNC name, it could be more (e.g.,
   // "\\server\share").
   //
   // If cchMax isn't enough to hold at least <volume_name>\...\<base_name>, the
   // result is the base filename.

   lpszCur = lpszBase + 2;                 // Skip "C:" or leading "\\"

   if (lpszBase[0] == '\\' && lpszBase[1] == '\\') // UNC pathname
   {
      // First skip to the '\' between the server name and the share name,
      while (*lpszCur != '\\')
      {
         lpszCur++;
         ASSERT(*lpszCur != '\0');
      }
   }
   // if a UNC get the share name, if a drive get at least one directory
   ASSERT(*lpszCur == '\\');
   // make sure there is another directory, not just c:\filename.ext
   if (cchFullPath - cchFileName > 3)
   {
      ++lpszCur;
      while (*lpszCur != '\\')
      {
         ++lpszCur;
         ASSERT(*lpszCur != '\0');
      }
   }
   ASSERT(*lpszCur == '\\');

   cchVolName = int(lpszCur - lpszBase);
   if (cchMax < cchVolName + 5 + cchFileName)
   {
      Checked::wcscpy_s(lpszCanon, cchFullPath + 1, lpszFileName);
      return;
   }

   // Now loop through the remaining directory components until something
   // of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
   //
   // Assert that the whole filename doesn't fit -- this should have been
   // handled earlier.

   ASSERT(cchVolName + AtlStrLen(lpszCur) > cchMax);
   while (cchVolName + 4 + AtlStrLen(lpszCur) > cchMax)
   {
      do
      {
         lpszCur++;
         ASSERT(*lpszCur != '\0');
      } while (*lpszCur != '\\');
   }

   // Form the resultant string and we're done.
   int cch;
   if (cchVolName >= 0 && cchVolName < cchMax)
      cch = cchVolName;
   else cch = cchMax;
   Checked::memcpy_s(lpszCanon + cch, cchFullPath + 1 - cch, L"\\...", sizeof(L"\\..."));
   Checked::wcscat_s(lpszCanon, cchFullPath + 1, lpszCur);
}

/////////////////////////////////////////////////////////////////////////////
// recent_file_list

#pragma warning(disable: 4267)
recent_file_list::recent_file_list(UINT nStart, LPCWSTR lpszSection,
   LPCWSTR lpszEntryFormat, 
   LPCWSTR lpszTimeEntryFormat,int nSize, int nMaxDispLen)
{
   ENSURE_ARG(nSize >= 0);
   m_arrNames.SetSize(nSize);
   m_timea.SetSize(nSize);

   m_nSize = nSize;
   m_nStart = nStart;

   m_strSectionName = lpszSection;
   m_strEntryFormat = lpszEntryFormat;
   m_strTimeEntryFormat = lpszTimeEntryFormat;

   m_nMaxDisplayLength = nMaxDispLen;
}

recent_file_list::~recent_file_list()
{
   
}

BOOL AFXAPI AfxFullPath(_Pre_notnull_ _Post_z_ LPWSTR lpszPathOut, LPCWSTR lpszFileIn);



BOOL AFXAPI AfxComparePath(LPCWSTR lpszPath1, LPCWSTR lpszPath2)
{
#pragma warning(push)
#pragma warning(disable:4068)
#pragma prefast(push)
#pragma prefast(disable:400, "lstrcmpi is ok here as we are backing it up with further comparison")
	// use case insensitive compare as a starter
	if (lstrcmpiW(lpszPath1, lpszPath2) != 0)
    {
		return FALSE;
    }
#pragma prefast(pop)
#pragma warning(pop)

	// on non-DBCS systems, we are done
	if (!GetSystemMetrics(SM_DBCSENABLED))
		return TRUE;

	// on DBCS systems, the file name may not actually be the same
	// in particular, the file system is case sensitive with respect to
	// "full width" roman characters.
	// (ie. fullwidth-R is different from fullwidth-r).
	int nLen = AtlStrLen(lpszPath1);
	if (nLen != AtlStrLen(lpszPath2))
		return FALSE;
	ASSERT(nLen < _MAX_PATH);

	// need to get both CT_CTYPE1 and CT_CTYPE3 for each filename
	LCID lcid = GetThreadLocale();
	WORD aCharType11[_MAX_PATH];
	VERIFY(GetStringTypeExW(lcid, CT_CTYPE1, lpszPath1, -1, aCharType11));
	WORD aCharType13[_MAX_PATH];
	VERIFY(GetStringTypeExW(lcid, CT_CTYPE3, lpszPath1, -1, aCharType13));
	WORD aCharType21[_MAX_PATH];
	VERIFY(GetStringTypeExW(lcid, CT_CTYPE1, lpszPath2, -1, aCharType21));
#ifdef _DEBUG
	WORD aCharType23[_MAX_PATH];
	VERIFY(GetStringTypeExW(lcid, CT_CTYPE3, lpszPath2, -1, aCharType23));
#endif

	// for every C3_FULLWIDTH character, make sure it has same C1 value
	int i = 0;
	for (LPCWSTR lpsz = lpszPath1; *lpsz != 0; lpsz++)
	{
		// check for C3_FULLWIDTH characters only
		if (aCharType13[i] & C3_FULLWIDTH)
		{
#ifdef _DEBUG
			ASSERT(aCharType23[i] & C3_FULLWIDTH); // should always match!
#endif

			// if CT_CTYPE1 is different then file system considers these
			// file names different.
			if (aCharType11[i] != aCharType21[i])
				return FALSE;
		}
		++i; // look at next character type
	}
	return TRUE; // otherwise file name is truly the same
}


// Operations
void recent_file_list::Add(LPCWSTR lpszPathName)
{
   //ASSERT(AfxIsValidString(lpszPathName));

   // fully qualify the path name
   WCHAR szTemp[_MAX_PATH];
   if (lpszPathName == NULL || wcslen(lpszPathName) >= _MAX_PATH)
   {
      ASSERT(FALSE);
      // MFC requires paths with length < _MAX_PATH
      // No other way to handle the error from a void function
      AfxThrowFileException(CFileException::badPath);
   }

   AfxFullPath(szTemp, lpszPathName);

   // update the MRU list, if an existing MRU string matches file name
   int iMRU;
   for (iMRU = 0; iMRU < m_nSize - 1; iMRU++)
   {
      if (AfxComparePath(m_arrNames[iMRU], szTemp))
         break;      // iMRU will point to matching entry
   }
   // move MRU strings before this one down
   for (; iMRU > 0; iMRU--)
   {
      ASSERT(iMRU > 0);
      ASSERT(iMRU < m_nSize);
      m_arrNames[iMRU] = m_arrNames[iMRU - 1];
      m_timea[iMRU] = m_timea[iMRU - 1];
   }
   // place this one at the beginning
   m_arrNames[0] = szTemp;
   m_timea[0] = CTime::GetCurrentTime();

   // any addition to the MRU should also be added to recent docs
   SHAddToRecentDocs(SHARD_PATHW, lpszPathName);

   WriteList();
}

void recent_file_list::Add(LPCWSTR lpszPathName, LPCWSTR lpszAppID)
{
   CWinApp* pApp = AfxGetApp();
   if (pApp == NULL || !pApp->IsWindows7())
   {
      Add(lpszPathName);
      return;
   }

   CStringW strAppID = lpszAppID == NULL ? L"" : lpszAppID;

   ASSERT(AfxIsValidString(lpszPathName));

   Add(lpszPathName);

   HRESULT hr = S_OK;
   CComPtr<IShellItem> psi = NULL;

#ifdef UNICODE
   hr = _AfxSHCreateItemFromParsingName(lpszPathName, NULL, IID_IShellItem, reinterpret_cast<void**>(&psi));
#else
   {
      USES_CONVERSION;
      LPOLESTR lpWPath = (LPOLESTR) lpszPathName;
      hr = _AfxSHCreateItemFromParsingName(lpWPath, NULL, IID_IShellItem, (LPVOID*)&psi);
   }
#endif

   ENSURE(SUCCEEDED(hr));

   Add(psi, strAppID);
}

void recent_file_list::Add(IShellItem* pItem, LPCWSTR lpszAppID)
{
   CWinApp* pApp = AfxGetApp();
   if (pApp == NULL || !pApp->IsWindows7())
   {
      return;
   }

   ASSERT(pItem != NULL);

   if (pItem == NULL)
   {
      return;
   }

   CStringW strAppID = lpszAppID == NULL ? L"" : lpszAppID;

   SHARDAPPIDINFO info;

#ifdef UNICODE
   info.pszAppID = strAppID;
#else
   {
      USES_CONVERSION;
      info.pszAppID = strAppID;
   }
#endif

   info.psi = pItem;
   SHAddToRecentDocs(SHARD_APPIDINFO, &info);
}

void recent_file_list::Add(IShellLink* pLink, LPCWSTR lpszAppID)
{
   CWinApp* pApp = AfxGetApp();
   if (pApp == NULL || !pApp->IsWindows7())
   {
      return;
   }

   ASSERT(pLink != NULL);

   if (pLink == NULL)
   {
      return;
   }

   CStringW strAppID = lpszAppID == NULL ? L"" : lpszAppID;

   SHARDAPPIDINFOLINK info;

#ifdef UNICODE
   info.pszAppID = strAppID;
#else
   {
      USES_CONVERSION;
      info.pszAppID = strAppID;
   }
#endif

   info.psl = pLink;
   if (strAppID.GetLength() == 0)
      SHAddToRecentDocs(SHARD_LINK, &info);
   else
      SHAddToRecentDocs(SHARD_APPIDINFOLINK, &info);
}

void recent_file_list::Add(PIDLIST_ABSOLUTE pidl, LPCWSTR lpszAppID)
{
   CWinApp* pApp = AfxGetApp();
   if (pApp == NULL || !pApp->IsWindows7())
   {
      return;
   }

   CStringW strAppID = lpszAppID == NULL ? L"" : lpszAppID;

   SHARDAPPIDINFOIDLIST info;

#ifdef UNICODE
   info.pszAppID = strAppID;
#else
   {
      USES_CONVERSION;
      info.pszAppID = strAppID;
   }
#endif

   info.pidl = pidl;
   SHAddToRecentDocs(SHARD_APPIDINFOIDLIST, &info);
}

void recent_file_list::Remove(int nIndex)
{
   ENSURE_ARG(nIndex >= 0 && nIndex < m_nSize);

   m_arrNames[nIndex].Empty();
   int iMRU;
   for (iMRU = nIndex; iMRU < m_nSize - 1; iMRU++)
   {
      m_arrNames[iMRU] = m_arrNames[iMRU + 1];
      m_timea[iMRU] = m_timea[iMRU + 1];
   }

   ASSERT(iMRU < m_nSize);
   m_arrNames[iMRU].Empty();
}

BOOL recent_file_list::GetDisplayName(CStringW& strName, int nIndex,
   LPCWSTR lpszCurDir, int nCurDir, BOOL bAtLeastName) const
{
   ENSURE_ARG(lpszCurDir == NULL || AfxIsValidString(lpszCurDir, nCurDir));

   ENSURE_ARG(nIndex < m_nSize);
   if (lpszCurDir == NULL || m_arrNames[nIndex].IsEmpty())
      return FALSE;

   int nLenName = m_arrNames[nIndex].GetLength();
   LPWSTR lpch = strName.GetBuffer(nLenName + 1);
   if (lpch == NULL)
   {
      AfxThrowMemoryException();
   }
   Checked::wcsncpy_s(lpch, nLenName + 1, m_arrNames[nIndex], _TRUNCATE);
   // nLenDir is the length of the directory part of the full path
   int nLenDir = nLenName - (AfxGetFileName2(lpch, NULL, 0) - 1);
   BOOL bSameDir = FALSE;
   if (nLenDir == nCurDir)
   {
      WCHAR chSave = lpch[nLenDir];
      lpch[nCurDir] = 0;  // terminate at same location as current dir
      bSameDir = ::AfxComparePath(lpszCurDir, lpch);
      lpch[nLenDir] = chSave;
   }
   // copy the full path, otherwise abbreviate the name
   if (bSameDir)
   {
      // copy file name only since directories are same
      WCHAR szTemp[_MAX_PATH];
      AfxGetFileTitle2(lpch + nCurDir, szTemp, _countof(szTemp));
      Checked::wcsncpy_s(lpch, nLenName + 1, szTemp, _TRUNCATE);
   }
   else if (m_nMaxDisplayLength != -1)
   {
      // strip the extension if the system calls for it
      WCHAR szTemp[_MAX_PATH];
      AfxGetFileTitle2(lpch + nLenDir, szTemp, _countof(szTemp));
      Checked::wcsncpy_s(lpch + nLenDir, nLenName + 1 - nLenDir, szTemp, _TRUNCATE);

      // abbreviate name based on what will fit in limited space
      _AfxAbbreviateName(lpch, m_nMaxDisplayLength, bAtLeastName);
   }
   strName.ReleaseBuffer();
   return TRUE;
}

void recent_file_list::UpdateMenu(CCmdUI* pCmdUI)
{
   ENSURE_ARG(pCmdUI != NULL);
   //ASSERT(m_arrNames != NULL);

   CMenu* pMenu = pCmdUI->m_pMenu;
   if (m_strOriginal.IsEmpty() && pMenu != NULL)
      pMenu->GetMenuString(pCmdUI->m_nID, get_utf8(m_strOriginal), MF_BYCOMMAND);

   if (m_arrNames[0].IsEmpty())
   {
      // no MRU files
      if (!m_strOriginal.IsEmpty())
         pCmdUI->SetText(get_utf8(m_strOriginal));
      pCmdUI->Enable(FALSE);
      return;
   }

   if (pCmdUI->m_pMenu == NULL)
      return;

   int iMRU;
   for (iMRU = 0; iMRU < m_nSize; iMRU++)
      pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);

   WCHAR szCurDir[_MAX_PATH];
   DWORD dwDirLen = GetCurrentDirectoryW(_MAX_PATH, szCurDir);
   if (dwDirLen == 0 || dwDirLen >= _MAX_PATH)
      return;	// Path too long

   int nCurDir = AtlStrLen(szCurDir);
   ASSERT(nCurDir >= 0);
   szCurDir[nCurDir] = '\\';
   szCurDir[++nCurDir] = '\0';

   CStringW strName;
   CStringW strTemp;
   for (iMRU = 0; iMRU < m_nSize; iMRU++)
   {
      if (!GetDisplayName(strName, iMRU, szCurDir, nCurDir))
         break;

      // double up any '&' characters so they are not underlined
      LPCWSTR lpszSrc = strName;
      LPWSTR lpszDest = strTemp.GetBuffer(strName.GetLength() * 2);
      while (*lpszSrc != 0)
      {
         if (*lpszSrc == '&')
            *lpszDest++ = '&';
         if (_istlead(*lpszSrc))
            *lpszDest++ = *lpszSrc++;
         *lpszDest++ = *lpszSrc++;
      }
      *lpszDest = 0;
      strTemp.ReleaseBuffer();

      // insert mnemonic + the file name
      WCHAR buf[10];
      int nItem = ((iMRU + m_nStart) % _AFX_MRU_MAX_COUNT) + 1;


      // number &1 thru &9, then 1&0, then 11 thru ...
      if (nItem > 10)
        swprintf_s(buf, _countof(buf), L"%d ", nItem);
      else if (nItem == 10)
         Checked::wcscpy_s(buf, _countof(buf), L"1&0 ");
      else
         swprintf_s(buf, _countof(buf), L"&%d ", nItem);

      pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
         MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
         get_utf8(buf) + get_utf8(strTemp));
   }

   // update end menu count
   pCmdUI->m_nIndex--; // point to last menu added
   pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();

   pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

BOOL _WriteProfileStringW(CWinApp * papp, LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCWSTR lpszValue)
{
   ASSERT(lpszSection != NULL);
   if (papp->m_pszRegistryKey != NULL)
   {
      LONG lResult;
      if (lpszEntry == NULL) //delete whole section
      {
         HKEY hAppKey = papp->GetAppRegistryKey();
         if (hAppKey == NULL)
            return FALSE;
         lResult = ::RegDeleteKey(hAppKey, lpszSection);
         RegCloseKey(hAppKey);
      }
      else if (lpszValue == NULL)
      {
         HKEY hSecKey = papp->GetSectionKey(lpszSection);
         if (hSecKey == NULL)
            return FALSE;
         // necessary to cast away const below
         lResult = ::RegDeleteValue(hSecKey, (LPTSTR)lpszEntry);
         RegCloseKey(hSecKey);
      }
      else
      {
         HKEY hSecKey =papp->GetSectionKey(lpszSection);
         if (hSecKey == NULL)
            return FALSE;
         lResult = RegSetValueExW(hSecKey, get_uni(lpszEntry), NULL, REG_SZ,
            (LPBYTE)lpszValue, (static_cast<DWORD>(wcslen(lpszValue)) + 1) * sizeof(WCHAR));
         RegCloseKey(hSecKey);
      }
      return lResult == ERROR_SUCCESS;
   }
   else
   {
      ASSERT(papp->m_pszProfileName != NULL);
      ASSERT(AtlStrLen(papp->m_pszProfileName) < 4095); // can't read in bigger
      return ::WritePrivateProfileStringW(get_uni(lpszSection), get_uni(lpszEntry), lpszValue,
         get_uni(papp->m_pszProfileName));
   }
}


void recent_file_list::WriteList()
{
   //ASSERT(m_arrNames != NULL);
   ASSERT(!m_strSectionName.IsEmpty());
   ASSERT(!m_strEntryFormat.IsEmpty());
   ASSERT(!m_strTimeEntryFormat.IsEmpty());
   int nLen = max(m_strEntryFormat.GetLength(), m_strTimeEntryFormat.GetLength()) + 10;
   LPWSTR pszEntry = new WCHAR[nLen];
   CWinApp* pApp = AfxGetApp();
   _WriteProfileStringW(pApp, get_utf8(m_strSectionName), NULL, NULL);
   for (int iMRU = 0; iMRU < m_nSize; iMRU++)
   {
      swprintf_s(pszEntry, nLen, m_strEntryFormat, iMRU + 1);
      if (!m_arrNames[iMRU].IsEmpty())
      {
         _WriteProfileStringW(pApp, get_utf8(m_strSectionName), get_utf8(pszEntry),
            m_arrNames[iMRU]);
      }
      swprintf_s(pszEntry, nLen, m_strTimeEntryFormat, iMRU + 1);
      if (!m_arrNames[iMRU].IsEmpty())
      {
         pApp->WriteProfileBinary(get_utf8(m_strSectionName), get_utf8(pszEntry),
            (LPBYTE) &m_timea[iMRU], sizeof(CTime));
      }
   }
   delete[] pszEntry;
}

void recent_file_list::ReadList()
{
   //ASSERT(m_arrNames != NULL);
   ASSERT(!m_strSectionName.IsEmpty());
   ASSERT(!m_strEntryFormat.IsEmpty());
   ASSERT(!m_strTimeEntryFormat.IsEmpty());
   int nLen = max(m_strEntryFormat.GetLength(), m_strTimeEntryFormat.GetLength()) + 10;
   LPWSTR pszEntry = new WCHAR[nLen];
   CWinApp* pApp = AfxGetApp();
   for (int iMRU = 0; iMRU < m_nSize; iMRU++)
   {
      swprintf_s(pszEntry, nLen, m_strEntryFormat, iMRU + 1);
      m_arrNames[iMRU] = _GetProfileStringW(pApp,
         get_utf8(m_strSectionName), get_utf8(pszEntry), L"");
      swprintf_s(pszEntry, nLen, m_strTimeEntryFormat, iMRU + 1);
      UINT u = 0;
      LPBYTE p = nullptr;
      if (pApp->GetProfileBinary(
         get_utf8(m_strSectionName), get_utf8(pszEntry), &p, &u))
      {

         if (p && u == sizeof(CTime))
         {
            m_timea[iMRU] = *((CTime*)p);

         }
         delete p;

       }
   }
   delete[] pszEntry;
}

/////////////////////////////////////////////////////////////////////////////
