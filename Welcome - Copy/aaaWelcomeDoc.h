// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once


#define LIBRARY_HINT_UPDATE_TREE 20180328


class CWelcomeTreeView;





CString GetAppDataPath();


class CWelcomeDoc : public CDocument
{
public:


   CStringArray            m_straWelcome;
   CStringArray            m_straPath;
   int                     m_iWelcomeSel;


   CWelcomeDoc();
   ~CWelcomeDoc() override;


   virtual void Serialize(CArchive& ar);   // overridden for document i/o


#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

   virtual BOOL OnNewDocument();

   CString GetDefaultWelcomePath();
   CString GetDefaultPhotoWelcomePath();


   void library_remove();
   void library_save();
   void library_load();
   int library_count();
   CString library_path(int iIndex);
   CString library_name(int iIndex);
   void library_add(CString strPath, bool bCreate = false);
   int library_sel();
   void library_sel(int iSel);

   DECLARE_DYNCREATE(CWelcomeDoc)
   DECLARE_MESSAGE_MAP()

};



