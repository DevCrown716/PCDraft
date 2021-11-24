// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once


struct CFileStatusW
{
   CTime m_ctime;          // creation date/time of file
   CTime m_mtime;          // last modification date/time of file
   CTime m_atime;          // last access date/time of file
   ULONGLONG m_size;       // logical size of file in bytes
   DWORD m_attribute;      // logical OR of CFile::Attribute enum values
   WCHAR m_szFullName[_MAX_PATH]; // absolute path name

};


BOOL PASCAL FileGetStatus(LPCWSTR lpszFileName, CFileStatusW& rStatus, CAtlTransactionManager* pTM = nullptr);
