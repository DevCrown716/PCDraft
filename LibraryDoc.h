// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#define LIBRARY_HINT_UPDATE_TREE 20180328

class CLibraryTreeView;

class library_tree
{
public:

   bool                       m_bSpotLibrary;
   CString                    m_strName;
   CString                    m_strPath;
   int                        m_iCountChildrenLibrary;

   CArray < library_tree >    m_treea;

   library_tree() {}
   library_tree(const library_tree & tree) :
      m_strName(tree.m_strName),
      m_strPath(tree.m_strPath),
      m_bSpotLibrary(tree.m_bSpotLibrary),
      m_iCountChildrenLibrary(tree.m_iCountChildrenLibrary)
   {

      m_treea.Append(tree.m_treea);

   }


   library_tree & operator =(const library_tree & tree)
   {

      if (this != &tree)
      {

         m_strName = tree.m_strName;

         m_strPath = tree.m_strPath;

         m_bSpotLibrary = tree.m_bSpotLibrary;

         m_iCountChildrenLibrary = tree.m_iCountChildrenLibrary;

         m_treea.RemoveAll();

         m_treea.Append(tree.m_treea);

      }

      return *this;

   }

};

CString GetAppDataPath();
class CLibraryDoc : public CDocument
{
public:
   
   
   CStringArray            m_straLibrary;
   CStringArray            m_straPath;
   int                     m_iLibrarySel;
   CArray < HTREEITEM >    m_itemaLibrary;
   CArray < HTREEITEM >    m_itema;
   CLibraryTreeView *      m_ptreeview;
   library_tree            m_tree;

   CLibraryDoc();
	virtual ~CLibraryDoc();

   virtual void Serialize(CArchive& ar);   // overridden for document i/o

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
   virtual BOOL OnNewDocument();

   CString GetDefaultLibraryPath();
   CString GetDefaultPhotoLibraryPath();
  

   void library_remove();
   void library_save();
   void library_load();
   int library_count();
   CString library_path(int iIndex);
   CString library_name(int iIndex);
   void library_add(CString strPath, bool bCreate = false);
   int library_sel();
   void library_sel(int iSel);

   DECLARE_DYNCREATE(CLibraryDoc)
   DECLARE_MESSAGE_MAP()

};
