// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once


#include "FileStatusW.h"
#include <strsafe.h>


AFX_STATIC void AFXAPI _AfxFillExceptionInfo(CFileException* pException, LPCWSTR lpszFileName);
BOOL AFXAPI _AfxFullPath2(_Out_writes_z_(_MAX_PATH) LPWSTR lpszPathOut, LPCWSTR lpszFileIn, CFileException* pException);
void AfxGetTempFileName(LPCWSTR lpszPath, LPCWSTR lpszPrefix, LPWSTR lpszTempName, size_t sizeOfTempName);
void AFX_CDECL AfxTimeToFileTime(const CTime& time, LPFILETIME pFileTime);
void AFXAPI AfxGetRoot(LPCWSTR lpszPath, CStringW& strRoot);


class CMirrorFileW : public CFile
{
   // Implementation
public:


   CStringW m_wstrMirrorName;
   BOOL Open(LPCWSTR lpszFileName, UINT nOpenFlags,
      CFileException* pError)
   {
      ASSERT(lpszFileName != NULL);
      m_wstrMirrorName.Empty();

      CFileStatusW status;
      if (nOpenFlags & CFile::modeCreate) //opened for writing
      {
         if (FileGetStatus(lpszFileName, status))
         {
            CStringW strRoot;
            AfxGetRoot(lpszFileName, strRoot);

            DWORD dwSecPerClus, dwBytesPerSec, dwFreeClus, dwTotalClus;
            int nBytes = 0;
            if (GetDiskFreeSpaceW(strRoot, &dwSecPerClus, &dwBytesPerSec, &dwFreeClus,
               &dwTotalClus))
            {
               nBytes = dwFreeClus * dwSecPerClus*dwBytesPerSec;
            }
            if (nBytes > 2 * status.m_size) // at least 2x free space avail
            {
               // get the directory for the file
               WCHAR szPath[_MAX_PATH];
               LPWSTR lpszName;
               GetFullPathNameW(lpszFileName, _MAX_PATH, szPath, &lpszName);
               *lpszName = NULL;

               //let's create a temporary file name
               AfxGetTempFileName(szPath, L"MFC", m_wstrMirrorName.GetBuffer(_MAX_PATH + 1), _MAX_PATH + 1);
               m_wstrMirrorName.ReleaseBuffer();
            }
         }
      }

      if (!m_wstrMirrorName.IsEmpty() &&
         FileOpen(m_wstrMirrorName, nOpenFlags, pError))
      {
         m_strFileName = lpszFileName;
         FILETIME ftCreate, ftAccess, ftModify;
         if (::GetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify))
         {
            AfxTimeToFileTime(status.m_ctime, &ftCreate);
            SetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify);
         }

         DWORD dwLength = 0;
         PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
         if (GetFileSecurityW(lpszFileName, DACL_SECURITY_INFORMATION,
            NULL, dwLength, &dwLength))
         {
            pSecurityDescriptor = (PSECURITY_DESCRIPTOR) new BYTE[dwLength];
            if (::GetFileSecurityW(lpszFileName, DACL_SECURITY_INFORMATION,
               pSecurityDescriptor, dwLength, &dwLength))
            {
               SetFileSecurityW(m_wstrMirrorName, DACL_SECURITY_INFORMATION, pSecurityDescriptor);
            }
            delete[](BYTE*)pSecurityDescriptor;
         }
         return TRUE;
      }
      m_wstrMirrorName.Empty();
      return FileOpen(lpszFileName, nOpenFlags, pError);
   }


   void FileRemove(LPCWSTR lpszFileName)
   {
      BOOL bRes = ::DeleteFile((LPTSTR)lpszFileName);
      if (!bRes)
         CFileException::ThrowOsError((LONG)::GetLastError(), get_utf8(lpszFileName));

   }

   void Abort()
   {
      CFile::Abort();
      if (!m_wstrMirrorName.IsEmpty())
         FileRemove(m_wstrMirrorName);
   }

   BOOL FileOpen(LPCWSTR lpszFileName, UINT nOpenFlags, CFileException* pException)
   {
      ASSERT_VALID(this);
      ASSERT(AfxIsValidString(lpszFileName));

      ASSERT(pException == NULL ||
         AfxIsValidAddress(pException, sizeof(CFileException)));
      ASSERT((nOpenFlags & typeText) == 0);   // text mode not supported

      // shouldn't open an already open file (it will leak)
      ASSERT(m_hFile == INVALID_HANDLE_VALUE);

      // CFile objects are always binary and CreateFile does not need flag
      nOpenFlags &= ~(UINT)typeBinary;

      m_bCloseOnDelete = FALSE;

      m_hFile = INVALID_HANDLE_VALUE;
      m_strFileName.Empty();

      WCHAR szTemp[_MAX_PATH];
      if (lpszFileName != NULL && SUCCEEDED(StringCchLengthW(lpszFileName, _MAX_PATH, NULL)))
      {
         if (_AfxFullPath2(szTemp, lpszFileName, pException) == FALSE)
            return FALSE;
      }
      else
      {
         // user passed in a buffer greater then _MAX_PATH
         if (pException != NULL)
         {
            pException->m_cause = CFileException::badPath;
            pException->m_strFileName = lpszFileName;
         }
         return FALSE; // path is too long
      }

      m_strFileName = get_utf8(szTemp);
      ASSERT(shareCompat == 0);

      // map read/write mode
      ASSERT((modeRead | modeWrite | modeReadWrite) == 3);
      DWORD dwAccess = 0;
      switch (nOpenFlags & 3)
      {
      case modeRead:
         dwAccess = GENERIC_READ;
         break;
      case modeWrite:
         dwAccess = GENERIC_WRITE;
         break;
      case modeReadWrite:
         dwAccess = GENERIC_READ | GENERIC_WRITE;
         break;
      default:
         ASSERT(FALSE);  // invalid share mode
      }

      // map share mode
      DWORD dwShareMode = 0;
      switch (nOpenFlags & 0x70)    // map compatibility mode to exclusive
      {
      default:
         ASSERT(FALSE);  // invalid share mode?
      case shareCompat:
      case shareExclusive:
         dwShareMode = 0;
         break;
      case shareDenyWrite:
         dwShareMode = FILE_SHARE_READ;
         break;
      case shareDenyRead:
         dwShareMode = FILE_SHARE_WRITE;
         break;
      case shareDenyNone:
         dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ;
         break;
      }

      // Note: typeText and typeBinary are used in derived classes only.

      // map modeNoInherit flag
      SECURITY_ATTRIBUTES sa;
      sa.nLength = sizeof(sa);
      sa.lpSecurityDescriptor = NULL;
      sa.bInheritHandle = (nOpenFlags & modeNoInherit) == 0;

      // map creation flags
      DWORD dwCreateFlag;
      if (nOpenFlags & modeCreate)
      {
         if (nOpenFlags & modeNoTruncate)
            dwCreateFlag = OPEN_ALWAYS;
         else
            dwCreateFlag = CREATE_ALWAYS;
      }
      else
         dwCreateFlag = OPEN_EXISTING;

      // special system-level access flags

      // Random access and sequential scan should be mutually exclusive
      ASSERT((nOpenFlags&(osRandomAccess | osSequentialScan)) != (osRandomAccess |
         osSequentialScan));

      DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;
      if (nOpenFlags & osNoBuffer)
         dwFlags |= FILE_FLAG_NO_BUFFERING;
      if (nOpenFlags & osWriteThrough)
         dwFlags |= FILE_FLAG_WRITE_THROUGH;
      if (nOpenFlags & osRandomAccess)
         dwFlags |= FILE_FLAG_RANDOM_ACCESS;
      if (nOpenFlags & osSequentialScan)
         dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;

      // attempt file creation
      HANDLE hFile = // m_pTM != NULL ? m_pTM->CreateFile(lpszFileName, dwAccess, dwShareMode, &sa, dwCreateFlag, dwFlags, NULL) :
         ::CreateFileW(lpszFileName, dwAccess, dwShareMode, &sa, dwCreateFlag, dwFlags, NULL);
      if (hFile == INVALID_HANDLE_VALUE)
      {
         _AfxFillExceptionInfo(pException, lpszFileName);
         return FALSE;
      }
      m_hFile = hFile;
      m_bCloseOnDelete = TRUE;

      return TRUE;
   }
   void FileRename(LPCWSTR lpszOldName, LPCWSTR lpszNewName)
   {
      //BOOL bRes = (pTM != NULL) ? pTM->MoveFile(lpszOldName, lpszNewName) : ::MoveFile((LPTSTR)lpszOldName, (LPTSTR)lpszNewName);

      BOOL bRes = ::MoveFileW((LPWSTR)lpszOldName, (LPWSTR)lpszNewName);
      if (!bRes)
         CFileException::ThrowOsError((LONG)::GetLastError(), get_utf8(lpszOldName));
   }


   void Close()
   {
      // Note that CMirrorFile does not have a virtual destructor, so Close will not
      // be implicitly called when the object is destroyed (as it is for CFile).
      // CMirrorFile::Close must be called explicitly in order to close an opened file.
      CString strName = m_strFileName; //file close empties string
      CFile::Close();
      if (!m_wstrMirrorName.IsEmpty())
      {
         CStringW wstrName(get_uni(strName));
         // m_wstrMirrorName;
         if (!ReplaceFileW(wstrName, m_wstrMirrorName, NULL, 0, NULL, NULL))
         {
            FileRemove(wstrName);
            FileRename(m_wstrMirrorName, wstrName);
         }
      }
   }

};

