// SymbolLibraryDoc.cpp : implementation of the CSymbolLibraryDoc class
//

#include "stdafx.h"
#include "PCDraft.h"

#include "SymbolLibraryDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSymLibEntry,CObject,1)

CSymLibEntry::CSymLibEntry(CMDShape* pShape,int nDisplayRes,int nScale) : m_pShape(pShape), m_nDisplayRes(nDisplayRes), m_nScale(nScale)
{
}

CSymLibEntry::~CSymLibEntry()
{
	if(m_pShape)
		delete m_pShape;
}

void CSymLibEntry::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_pShape;
		ar << m_nDisplayRes;
		ar << m_nScale;
	}
	else
	{
		ar >> m_pShape;
		ar >> m_nDisplayRes;
		ar >> m_nScale;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryDoc

IMPLEMENT_DYNCREATE(CSymbolLibraryDoc, CDocument)

BEGIN_MESSAGE_MAP(CSymbolLibraryDoc, CDocument)
	//{{AFX_MSG_MAP(CSymbolLibraryDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateFileRevert)
	ON_COMMAND(ID_FILE_REVERT, OnFileRevert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryDoc construction/destruction

CSymbolLibraryDoc::CSymbolLibraryDoc()
{
	// TODO: add one-time construction code here
	m_pActiveEntry=NULL;
}

CSymbolLibraryDoc::~CSymbolLibraryDoc()
{
}

BOOL CSymbolLibraryDoc::OnNewDocument()
{
	CPCDraftApp *pApp=(CPCDraftApp*)AfxGetApp();

	if(!pApp->CheckResources() || !CDocument::OnNewDocument())
		return FALSE;

	SetTitle(GetDefaultTitle());

	return TRUE;
}

// Infinisys Ltd
BOOL CSymbolLibraryDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
#ifdef _DEBUG
   if(IsModified())
      TRACE(traceAppMsg,0,"Warning: OnOpenDocument replaces an unsaved document.\n");
#endif

   ENSURE(lpszPathName);

   int iCount = 1;

   for(int iTry = 0; iTry < iCount; iTry++)
   {

      CFileException* pfe = new CFileException;
      CFile* pFile = GetFile(lpszPathName,
         CFile::modeRead | CFile::shareDenyWrite,pfe);
      if(pFile == NULL)
      {
         TRY
         {
            ReportSaveLoadException(lpszPathName,pfe,
            FALSE,AFX_IDP_FAILED_TO_OPEN_DOC);
         }
            END_TRY

         pfe->Delete();
         return FALSE;
      }

      pfe->Delete();
      

      DeleteContents();
      SetModifiedFlag();  // dirty during de-serialize

      CArchive loadArchive(pFile,CArchive::load | CArchive::bNoFlushOnDelete);
      loadArchive.m_pDocument = this;
      loadArchive.m_bForceFlat = FALSE;
      TRY
      {
         
         CWaitCursor wait;
         if(pFile->GetLength() != 0)
         {
            if(iTry == 1)
            {
               m_iArchiveVersion = 1;
               m_Shapes.Serialize(loadArchive);

            }
            else
            {
               Serialize(loadArchive);     // load me

            }
         }

         if(iTry <= 0 && m_iArchiveVersion < 0)
         {
            ReleaseFile(pFile,TRUE);
            DeleteContents();   // remove failed contents
            iCount++;
            continue;
         }

         loadArchive.Close();
         ReleaseFile(pFile,FALSE);
      }
         CATCH_ALL(e)
      {
         ReleaseFile(pFile,TRUE);
         DeleteContents();   // remove failed contents

         TRY
         {
            ReportSaveLoadException(lpszPathName,e,
            FALSE,AFX_IDP_FAILED_TO_OPEN_DOC);
         }
            END_TRY
            pfe->Delete();
            
         return FALSE;
      }
      END_CATCH_ALL
   }
   m_iArchiveVersion = 2;

      SetModifiedFlag(FALSE);     // start off with unmodified

   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryDoc serialization

void CSymbolLibraryDoc::Serialize(CArchive& ar)
{
   // Infinisys Ltd
   ar.m_pDocument = this; // Infinisys Ltd
   if(ar.IsStoring())
   {
      m_iArchiveVersion = 2;
      ar << m_iArchiveVersion;
   }
   else
   {
      ar >> m_iArchiveVersion;
      if(m_iArchiveVersion < 0) // Old file Version - Infinisys Ltd
      {
         return;
      }
   }
   m_Shapes.Serialize(ar);
   if(m_iArchiveVersion >= 2)
   {
      if(ar.IsStoring())
      {
         int iCount = m_packeditema.GetCount();
         ar << iCount;
         for(int i = 0; i < iCount; i++)
         {
            ar << m_packeditema[i];
         }
      }
      else
      {
         int iCount = m_packeditema.GetCount();
         ar >> iCount;
         for(int i = 0; i < iCount; i++)
         {
            CPackedItem item;
            ar >> item;

            if(item.m_etype == packed_type_gradient)
            {
               m_Gradients[m_numGradients] = *item.m_pgradient;
               item.destroy();
               item.m_etype = packed_type_gradient;
               item.m_pgradient =& m_Gradients[m_numGradients];
               m_numGradients++;
            }

            m_packeditema.Add(item);
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryDoc diagnostics

#ifdef _DEBUG
void CSymbolLibraryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSymbolLibraryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryDoc commands

void CSymbolLibraryDoc::DeleteContents() 
{
	for(int i=0;i<m_Shapes.GetSize();i++)
		delete m_Shapes.GetAt(i);
	m_Shapes.RemoveAll();

	CDocument::DeleteContents();
}

BOOL CSymbolLibraryDoc::SaveModified() 
{
#ifdef _DEMO
	return TRUE;
#else
	if(((CPCDraftApp*)AfxGetApp())->m_bDemoMode) return TRUE;
	return CDocument::SaveModified();
#endif
}

int CSymbolLibraryDoc::GetObjectCount(bool bOverall)
{
	if(bOverall)
	{
		int nCount=0;
		for(int i=0;i<m_Shapes.GetSize();i++)
			nCount+=m_Shapes.GetAt(i)->m_pShape->GetObjectCount();
		return nCount;
	}
	return m_Shapes.GetSize();
}

void CSymbolLibraryDoc::OnUpdateFileRevert(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!GetPathName().IsEmpty() && IsModified());
}

void CSymbolLibraryDoc::OnFileRevert() 
{
	CString text;
	text.Format("Revert \"%s\" to last version saved?\n",GetTitle());
	CFileStatus st;
	CFile::GetStatus(GetPathName(),st);
	if(AfxMessageBox(text+st.m_mtime.Format("(was last saved at %X on %A, %B %d, %Y)"),MB_OKCANCEL)==IDOK)
	{
		DeleteContents();
		OnOpenDocument(GetPathName());
		SendInitialUpdate();
	}
}

