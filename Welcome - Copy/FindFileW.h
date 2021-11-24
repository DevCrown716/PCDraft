#pragma once
class CFindFileW : public CObject
{
public:
   //CFindFileW();

   /// <summary>
   /// CFindFileW constructor</summary>
   /// <param name="pTM">Pointer to CAtlTransactionManager object</param>
   CFindFileW();
   virtual ~CFindFileW();

   // Attributes
public:
   ULONGLONG GetLength() const;
   virtual CStringW GetFileName() const;
   virtual CStringW GetFilePath() const;
   virtual CStringW GetFileTitle() const;
   virtual CStringW GetFileURL() const;
   virtual CStringW GetRoot() const;

   virtual BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;
   virtual BOOL GetLastAccessTime(FILETIME* pTimeStamp) const;
   virtual BOOL GetCreationTime(FILETIME* pTimeStamp) const;
   virtual BOOL GetLastWriteTime(CTime& refTime) const;
   virtual BOOL GetLastAccessTime(CTime& refTime) const;
   virtual BOOL GetCreationTime(CTime& refTime) const;

   virtual BOOL MatchesMask(DWORD dwMask) const;

   virtual BOOL IsDots() const;
   // these aren't virtual because they all use MatchesMask(), which is
   BOOL IsReadOnly() const;
   BOOL IsDirectory() const;
   BOOL IsCompressed() const;
   BOOL IsSystem() const;
   BOOL IsHidden() const;
   BOOL IsTemporary() const;
   BOOL IsNormal() const;
   BOOL IsArchived() const;

   // Operations
   void Close();
   virtual BOOL FindFile(LPCWSTR pstrName = NULL, DWORD dwUnused = 0);
   virtual BOOL FindNextFile();

protected:
   virtual void CloseContext();

   // Implementation
protected:
   void* m_pFoundInfo;
   void* m_pNextInfo;
   HANDLE m_hContext;
   CStringW m_strRoot;
   TCHAR m_chDirSeparator;     // not '\\' for Internet classes

   /// <summary>
   /// Pointer to CAtlTransactionManager object</summary>
   //CAtlTransactionManager* m_pTM;

#ifdef _DEBUG
   void Dump(CDumpContext& dc) const;
   void AssertValid() const;
#endif

   DECLARE_DYNAMIC(CFindFileW)
};


// CFindFileW
inline BOOL CFindFileW::IsReadOnly() const
{
   return MatchesMask(FILE_ATTRIBUTE_READONLY);
}
inline BOOL CFindFileW::IsDirectory() const
{
   return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
}
inline BOOL CFindFileW::IsCompressed() const
{
   return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
}
inline BOOL CFindFileW::IsSystem() const
{
   return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
}
inline BOOL CFindFileW::IsHidden() const
{
   return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
}
inline BOOL CFindFileW::IsTemporary() const
{
   return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
}
inline BOOL CFindFileW::IsNormal() const
{
   return MatchesMask(FILE_ATTRIBUTE_NORMAL);
}
inline BOOL CFindFileW::IsArchived() const
{
   return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
}
