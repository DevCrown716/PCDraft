// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "FileStatusW.h"


BOOL AFXAPI AfxFullPath(_Pre_notnull_ _Post_z_ LPWSTR lpszPathOut, LPCWSTR lpszFileIn);


BOOL PASCAL FileGetStatus(LPCWSTR lpszFileName, CFileStatusW& rStatus, CAtlTransactionManager* pTM)
{
   ASSERT(lpszFileName != NULL);

   if (lpszFileName == NULL)
   {
      return FALSE;
   }

   if (wcslen(lpszFileName) >= _MAX_PATH)
   {
      ASSERT(FALSE); // MFC requires paths with length < _MAX_PATH
      return FALSE;
   }

   // attempt to fully qualify path first
   if (!AfxFullPath(rStatus.m_szFullName, lpszFileName))
   {
      rStatus.m_szFullName[0] = '\0';
      return FALSE;
   }

   WIN32_FILE_ATTRIBUTE_DATA fileAttributeData;

   //if (pTM != NULL)
   //{
   //   if (!pTM->GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fileAttributeData))
   //      return FALSE;
   //}
   //else
   {
      if (!GetFileAttributesExW(lpszFileName, GetFileExInfoStandard, &fileAttributeData))
         return FALSE;
   }

   // strip attribute of NORMAL bit, our API doesn't have a "normal" bit.
   rStatus.m_attribute = (fileAttributeData.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL);

   rStatus.m_size = fileAttributeData.nFileSizeHigh;
   rStatus.m_size <<= 32;
   rStatus.m_size |= fileAttributeData.nFileSizeLow;

   // convert times as appropriate
   if (CTime::IsValidFILETIME(fileAttributeData.ftCreationTime))
   {
      rStatus.m_ctime = CTime(fileAttributeData.ftCreationTime);
   }
   else
   {
      rStatus.m_ctime = CTime();
   }

   if (CTime::IsValidFILETIME(fileAttributeData.ftLastAccessTime))
   {
      rStatus.m_atime = CTime(fileAttributeData.ftLastAccessTime);
   }
   else
   {
      rStatus.m_atime = CTime();
   }

   if (CTime::IsValidFILETIME(fileAttributeData.ftLastWriteTime))
   {
      rStatus.m_mtime = CTime(fileAttributeData.ftLastWriteTime);
   }
   else
   {
      rStatus.m_mtime = CTime();
   }

   if (rStatus.m_ctime.GetTime() == 0)
      rStatus.m_ctime = rStatus.m_mtime;

   if (rStatus.m_atime.GetTime() == 0)
      rStatus.m_atime = rStatus.m_mtime;

   return TRUE;
}

