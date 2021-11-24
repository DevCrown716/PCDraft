// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "../PCDraft.h"
#include "WelcomeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString get_utf8(LPCWSTR lpcwsz);
// CWelcomeDoc

IMPLEMENT_DYNCREATE(CWelcomeDoc, CDocument)


CMutex * g_pmutexWelcome;

CWelcomeDoc::CWelcomeDoc()
{
   m_iWelcomeSel = -1;
   g_pmutexWelcome = new CMutex;
}

BOOL CWelcomeDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;
   return TRUE;
}

CWelcomeDoc::~CWelcomeDoc()
{
   delete g_pmutexWelcome;
}


BEGIN_MESSAGE_MAP(CWelcomeDoc, CDocument)
END_MESSAGE_MAP()


// CWelcomeDoc diagnostics

#ifdef _DEBUG
void CWelcomeDoc::AssertValid() const
{
   CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CWelcomeDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CWelcomeDoc serialization

void CWelcomeDoc::Serialize(CArchive& ar)
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


// CWelcomeDoc commands

CString CWelcomeDoc::GetDefaultWelcomePath()
{

   PWSTR path = NULL;

   HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, NULL, &path);

   if (FAILED(hr))
   {

      return "";

   }

   CStringW wstr(path);

   ::CoTaskMemFree(path);

   wstr.TrimRight(L"/");

   wstr.TrimRight(L"\\");

   wstr += "\\Microspot";

   if (!PathIsDirectoryW(wstr))
   {

      ::CreateDirectoryW(wstr, NULL);

   }

   wstr += "\\Welcome";

   if (!PathIsDirectoryW(wstr))
   {

      ::CreateDirectoryW(wstr, NULL);

   }

   return get_utf8(wstr);

}


CString CWelcomeDoc::GetDefaultPhotoWelcomePath()
{


   PWSTR pwsz = NULL;

   if (FAILED(SHGetKnownFolderPath(FOLDERID_Pictures, KF_FLAG_CREATE, NULL, &pwsz)))
   {

      return "";

   }

   CString str = get_utf8(pwsz);

   CoTaskMemFree(pwsz);

   str.TrimRight("/");

   str.TrimRight("\\");

   return str;

}



void CWelcomeDoc::library_remove()
{

   m_straWelcome.RemoveAt(library_sel());

   library_save();

   UpdateAllViews(NULL, LIBRARY_HINT_UPDATE_TREE);

}

void CWelcomeDoc::library_save()
{

   CStdioFile file(GetAppDataPath() + "\\Welcome.txt", CFile::modeWrite | CFile::modeCreate | CFile::typeText);

   for (int i = 0; i < m_straWelcome.GetCount(); i++)
   {

      CString strPath = m_straWelcome[i];

      file.WriteString(strPath + "\n");


   }

}
void CWelcomeDoc::library_load()
{

   m_straWelcome.RemoveAll();

   if (!PathFileExists(GetAppDataPath() + "\\Welcome.txt"))
   {

      return;

   }

   try
   {

      CStdioFile file(GetAppDataPath() + "\\Welcome.txt", CFile::modeRead | CFile::typeText);

      CString strPath;

      while (file.ReadString(strPath))
      {

         if (PathIsDirectoryW(get_uni(strPath)))
         {

            m_straWelcome.Add(strPath);

         }

      }

   }
   catch (...)
   {
   }

}


int CWelcomeDoc::library_count()
{

   return m_straWelcome.GetCount();

}


CString CWelcomeDoc::library_path(int iIndex)
{

   if (iIndex < 0)
   {
      return GetDefaultWelcomePath();
   }

   return m_straWelcome[iIndex];

}

void CWelcomeDoc::library_sel(int iIndex)
{
   m_iWelcomeSel = iIndex;
}

int CWelcomeDoc::library_sel()
{
   return m_iWelcomeSel;
}

CString CWelcomeDoc::library_name(int iIndex)
{

   CString strName = library_path(iIndex);

   CString strExt = ".spotlibrary";

   if (strName.Right(strExt.GetLength()).CompareNoCase(strExt) == 0)
   {

      strName = strName.Left(strName.GetLength() - strExt.GetLength());

   }

   int iFind1 = strName.ReverseFind('\\');

   int iFind2 = strName.ReverseFind('\\');

   int iFind = max(iFind1, iFind2);

   strName = strName.Mid(iFind + 1);

   return strName;

}


void CWelcomeDoc::library_add(CString strPath, bool bCreate)
{

   strPath.TrimRight("\\");
   strPath.TrimRight("/");

   for (int i = 0; i < m_straWelcome.GetCount(); i++)
   {

      if (m_straWelcome[i].CompareNoCase(strPath) == 0)
      {

         return;

      }

   }

   CString strExt = ".spotlibrary";

   if (strPath.Right(strExt.GetLength()).CompareNoCase(strExt) == 0)
   {

      if (!PathIsDirectoryW(get_uni(strPath)))
      {

         CreateDirectoryW(get_uni(strPath), NULL);

      }

      if (!PathIsDirectoryW(get_uni(strPath)))
      {

         CreateDirectoryW(get_uni(strPath + "\\SpotWelcome_Items"), NULL);

      }

   }

   m_straWelcome.Add(strPath);

   library_save();

}
