// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString get_utf8(LPCWSTR lpcwsz);
// CLibraryDoc

IMPLEMENT_DYNCREATE(CLibraryDoc, CDocument)


CMutex * g_pmutexLibrary;

CLibraryDoc::CLibraryDoc()
{
   m_iLibrarySel = -1;
   g_pmutexLibrary = new CMutex;
}

BOOL CLibraryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CLibraryDoc::~CLibraryDoc()
{
   delete g_pmutexLibrary;
}


BEGIN_MESSAGE_MAP(CLibraryDoc, CDocument)
END_MESSAGE_MAP()


// CLibraryDoc diagnostics

#ifdef _DEBUG
void CLibraryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CLibraryDoc serialization

void CLibraryDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CLibraryDoc commands

CString CLibraryDoc::GetDefaultLibraryPath()
{

   PWSTR path = NULL;

   HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData,KF_FLAG_CREATE,NULL,&path);

   if(FAILED(hr))
   {

      return "";

   }

   CStringW wstr(path);

   ::CoTaskMemFree(path);

   wstr.TrimRight(L"/");

   wstr.TrimRight(L"\\");

   wstr += "\\Microspot";

   if(!PathIsDirectoryW(wstr))
   {

      ::CreateDirectoryW(wstr,NULL);

   }

   wstr += "\\Library";

   if(!PathIsDirectoryW(wstr))
   {

      ::CreateDirectoryW(wstr,NULL);

   }

   return get_utf8(wstr);

}


CString CLibraryDoc::GetDefaultPhotoLibraryPath()
{


   PWSTR pwsz = NULL;

   if(FAILED(SHGetKnownFolderPath(FOLDERID_Pictures,KF_FLAG_CREATE,NULL,&pwsz)))
   {

      return "";

   }

   CString str = get_utf8(pwsz);

   CoTaskMemFree(pwsz);

   str.TrimRight("/");

   str.TrimRight("\\");

   return str;

}


CString GetAppDataPath()
{

   PWSTR pwsz = NULL;

   if(FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData,KF_FLAG_CREATE, NULL, &pwsz)))
   {
      
      return "";

   }

   CStringW wstr = pwsz;

   CoTaskMemFree(pwsz);

   wstr.TrimRight(L"/");

   wstr.TrimRight(L"\\");

   wstr += L"\\Microspot";

   if(!PathIsDirectoryW(wstr))
   {
      ::CreateDirectoryW(wstr,NULL);
   }

   return get_utf8(wstr);

}

void CLibraryDoc::library_remove()
{

   m_straLibrary.RemoveAt(library_sel());

   library_save();

   UpdateAllViews(NULL,LIBRARY_HINT_UPDATE_TREE);

}

void CLibraryDoc::library_save()
{

   CStdioFile file(GetAppDataPath() + "\\Library.txt",CFile::modeWrite | CFile::modeCreate | CFile::typeText);

   for(int i = 0; i < m_straLibrary.GetCount(); i++)
   {

      CString strPath = m_straLibrary[i];

      file.WriteString(strPath + "\n");


   }

}
void CLibraryDoc::library_load()
{

   m_straLibrary.RemoveAll();

   if(!PathFileExists(GetAppDataPath() + "\\Library.txt"))
   {

      return;

   }

   try
   {

      CStdioFile file(GetAppDataPath() + "\\Library.txt",CFile::modeRead | CFile::typeText);

      CString strPath;

      while(file.ReadString(strPath))
      {

         if(PathIsDirectoryW(get_uni(strPath)))
         {

            m_straLibrary.Add(strPath);

         }

      }

   }
   catch(...)
   {
   }

}


int CLibraryDoc::library_count()
{
   
   return m_straLibrary.GetCount();

}


CString CLibraryDoc::library_path(int iIndex)
{

   if(iIndex < 0)
   {
      return GetDefaultLibraryPath();
   }

   return m_straLibrary[iIndex];

}

void CLibraryDoc::library_sel(int iIndex)
{
   m_iLibrarySel = iIndex;
}

int CLibraryDoc::library_sel()
{
   return m_iLibrarySel;
}

CString CLibraryDoc::library_name(int iIndex)
{

   CString strName = library_path(iIndex);

   CString strExt = ".spotlibrary";

   if(strName.Right(strExt.GetLength()).CompareNoCase(strExt) == 0)
   {

      strName = strName.Left(strName.GetLength() - strExt.GetLength());

   }

   int iFind1 = strName.ReverseFind('\\');

   int iFind2 = strName.ReverseFind('\\');

   int iFind = max(iFind1,iFind2);

   strName = strName.Mid(iFind + 1);

   return strName;

}


void CLibraryDoc::library_add(CString strPath, bool bCreate)
{

   strPath.TrimRight("\\");
   strPath.TrimRight("/");

   for(int i = 0; i < m_straLibrary.GetCount(); i++)
   {

      if(m_straLibrary[i].CompareNoCase(strPath) == 0)
      {

         return;

      }

   }

   CString strExt = ".spotlibrary";

   if(strPath.Right(strExt.GetLength()).CompareNoCase(strExt) == 0)
   {

      if(!PathIsDirectoryW(get_uni(strPath)))
      {

         CreateDirectoryW(get_uni(strPath),NULL);

      }

      if(!PathIsDirectoryW(get_uni(strPath)))
      {

         CreateDirectoryW(get_uni(strPath + "\\SpotLibrary_Items"),NULL);

      }

   }

   m_straLibrary.Add(strPath);

   library_save();

}
