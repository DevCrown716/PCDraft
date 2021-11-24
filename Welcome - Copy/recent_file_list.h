#pragma once


/*============================================================================*/
// recent_file_list

#define AFX_ABBREV_FILENAME_LEN 30

class recent_file_list
{
   // Constructors
public:
   recent_file_list(UINT nStart, LPCWSTR lpszSection,
      LPCWSTR lpszEntryFormat,LPCWSTR lpszTimeEntryFormat
      , int nSize,
      int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);

   // Attributes
   int GetSize() const;
   CStringW& operator[](int nIndex);
   CTime get_time(int nIndex);

   // Operations
   virtual void Remove(int nIndex);
   virtual void Add(LPCWSTR lpszPathName);

   /// <summary>
   /// Adds a file to the MRU file list.</summary>
   /// <param name="lpszPathName"> Pathname to be added to the list.</param>
   /// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
   virtual void Add(LPCWSTR lpszPathName, LPCWSTR lpszAppID);
   virtual BOOL GetDisplayName(CStringW& strName, int nIndex,
      LPCWSTR lpszCurDir, int nCurDir, BOOL bAtLeastName = TRUE) const;
   virtual void UpdateMenu(CCmdUI* pCmdUI);
   virtual void ReadList();    // reads from registry or ini file
   virtual void WriteList();   // writes to registry or ini file

   /// <summary>
   /// Adds a file to the MRU file list.</summary>
   /// <param name="pItem"> Specifies a pointer to Shell Item to be added to the list.</param>
   /// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
   void Add(IShellItem* pItem, LPCWSTR lpszAppID);

   /// <summary>
   /// Adds a file to the MRU file list.</summary>
   /// <param name="pLink"> Specifies a pointer to Shell Link to be added to the list.</param>
   /// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
   void Add(IShellLink* pLink, LPCWSTR lpszAppID);

   /// <summary>
   /// Adds a file to the MRU file list.</summary>
   /// <param name="pidl"> Specifies the IDLIST for the shell item that should be added to the recent docs folder.</param>
   /// <param name="lpszAppID"> Specifies Application User Model ID for the application.</param>
   void Add(PIDLIST_ABSOLUTE pidl, LPCWSTR lpszAppID);

   // Implementation
   virtual ~recent_file_list();

   int m_nSize;                // contents of the MRU list
   CArray < CStringW > m_arrNames;
   CArray < CTime > m_timea;
   CStringW m_strSectionName;   // for saving
   CStringW m_strEntryFormat;
   CStringW m_strTimeEntryFormat;
   UINT m_nStart;              // for displaying
   int m_nMaxDisplayLength;
   CStringW m_strOriginal;      // original menu item contents
};

AFX_INLINE int recent_file_list::GetSize() const
{
   return m_nSize;
}
AFX_INLINE CStringW& recent_file_list::operator[](int nIndex)
{
   ENSURE_ARG(nIndex >= 0 && nIndex < m_nSize); return m_arrNames[nIndex];
}

AFX_INLINE CTime recent_file_list::get_time(int nIndex)
{
   ENSURE_ARG(nIndex >= 0 && nIndex < m_nSize); return m_timea[nIndex];
}
