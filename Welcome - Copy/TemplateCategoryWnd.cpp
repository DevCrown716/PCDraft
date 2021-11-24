// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateCategoryWnd.h"
#include "TemplateWnd.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"
#include "../PCDraftDoc.h"
#include "../DraftContext.h"
#include "../Layer.h"
#include "FindFileW.h"
#include <tchar.h>
#include "ScrollHelper.h"



bool Contains(const CRect & rectBigger, const CRect & rectSmaller)
{
   
   return rectSmaller.left >= rectBigger.left
      && rectSmaller.right <= rectBigger.right
      && rectSmaller.top >= rectBigger.top
      && rectSmaller.bottom <= rectBigger.bottom;


}

HBITMAP CreateDIB(int x, int y, COLORREF * pdata, int nStride, COLORREF ** p, bool bSwap);;
#undef new
Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);

BEGIN_MESSAGE_MAP(TemplateCategoryWnd, BaseControl)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_KEYDOWN()
END_MESSAGE_MAP()

TemplateCategoryWnd::TemplateCategoryWnd()
{

   m_bBaseControlArrowNavigation = false;

   m_iSel = 0;

   m_bComposited = true;

   m_iHover = -1;

   m_iThumbnailSize = 256;
   m_ptemplatewnd = nullptr;

   m_crBackground = RGB(242, 242, 242);
   m_pfont = nullptr;
   m_pfontLine1 = nullptr;
   m_pfontLine2 = nullptr;


   //m_pbitmap = LoadPNG(MAKEINTRESOURCEA(IDB_PNG_PCDRAFT_DOC48), "PNG", NULL);

}


TemplateCategoryWnd::~TemplateCategoryWnd()
{

   //delete m_pbitmap;

}




void TemplateCategoryWnd::ListCategories(CArray < CStringW > & stra)
{

   WCHAR szPath[MAX_PATH];
   // Get path for each computer, non-user specific and non-roaming data.
   if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA,
      NULL, 0, szPath)))
   {

      CFindFileW find;

      CStringW str(szPath);

      if (find.FindFile(str + "\\Microspot\\Template\\*.*"))
      {

         bool bWorking = true;

         while (bWorking)
         {

            bWorking = find.FindNextFileA();

            if (find.IsDots())
            {



            }
            else if (find.IsDirectory())
            {

               CStringW strCategory(find.GetFileName());

               CArray < CStringW > straTemplates;
               
               ListCategoryTemplates(straTemplates, strCategory, "");

               if (straTemplates.GetCount() > 0)
               {

                  stra.Add(strCategory);

               }

            }

         }

      }

   }

}


////////////////////////////////////////////////////////////////////////////
// CFindFileW implementation

CFindFileW::CFindFileW()
{
   m_pFoundInfo = NULL;
   m_pNextInfo = NULL;
   m_hContext = NULL;
   m_chDirSeparator = '\\';
//   m_pTM = NULL;
}

//CFindFileW::CFindFileW(CAtlTransactionManager* pTM)
//{
//   m_pFoundInfo = NULL;
//   m_pNextInfo = NULL;
//   m_hContext = NULL;
//   m_chDirSeparator = '\\';
//   //m_pTM = pTM;
//}

CFindFileW::~CFindFileW()
{
   Close();
}

void CFindFileW::Close()
{
   if (m_pFoundInfo != NULL)
   {
      delete m_pFoundInfo;
      m_pFoundInfo = NULL;
   }

   if (m_pNextInfo != NULL)
   {
      delete m_pNextInfo;
      m_pNextInfo = NULL;
   }

   if (m_hContext != NULL)
   {
      if (m_hContext != INVALID_HANDLE_VALUE)
         CloseContext();
      m_hContext = NULL;
   }
}

void CFindFileW::CloseContext()
{
   ::FindClose(m_hContext);
   return;
}

BOOL CFindFileW::FindFile(LPCWSTR pstrName /* = NULL */,
   DWORD dwUnused /* = 0 */)
{
   UNUSED_ALWAYS(dwUnused);
   Close();

   if (pstrName == NULL)
      pstrName = L"*.*";
   else if (wcslen(pstrName) >= (_countof(((WIN32_FIND_DATAW*)m_pNextInfo)->cFileName)))
   {
      ::SetLastError(ERROR_BAD_ARGUMENTS);
      return FALSE;
   }

   m_pNextInfo = new WIN32_FIND_DATAW;

   WIN32_FIND_DATAW *pFindData = (WIN32_FIND_DATAW *)m_pNextInfo;

   Checked::wcscpy_s(pFindData->cFileName, _countof(pFindData->cFileName), pstrName);

   m_hContext = 
      ::FindFirstFileW(pstrName, (WIN32_FIND_DATAW*)m_pNextInfo);

   if (m_hContext == INVALID_HANDLE_VALUE)
   {
      DWORD dwTemp = ::GetLastError();
      Close();
      ::SetLastError(dwTemp);
      return FALSE;
   }

   LPWSTR pstrRoot = m_strRoot.GetBufferSetLength(_MAX_PATH);
   LPCWSTR pstr = _wfullpath(pstrRoot, pstrName, _MAX_PATH);

   // passed name isn't a valid path but was found by the API
   ASSERT(pstr != NULL);
   if (pstr == NULL)
   {
      m_strRoot.ReleaseBuffer(0);
      Close();
      ::SetLastError(ERROR_INVALID_NAME);
      return FALSE;
   }
   else
   {
      WCHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR];
      _wsplitpath_s(pstrRoot, strDrive, _MAX_DRIVE, strDir, _MAX_DIR, NULL, 0, NULL, 0);
      _wmakepath_s(pstrRoot, _MAX_PATH, strDrive, strDir, NULL, NULL);
      m_strRoot.ReleaseBuffer(-1);
   }
   return TRUE;
}

BOOL CFindFileW::MatchesMask(DWORD dwMask) const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL)
      return (!!(((LPWIN32_FIND_DATAW)m_pFoundInfo)->dwFileAttributes & dwMask));
   else
      return FALSE;
}

BOOL CFindFileW::GetLastAccessTime(FILETIME* pTimeStamp) const
{
   ASSERT(m_hContext != NULL);
   ASSERT(pTimeStamp != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL && pTimeStamp != NULL)
   {
      *pTimeStamp = ((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftLastAccessTime;
      return TRUE;
   }
   else
      return FALSE;
}

BOOL CFindFileW::GetLastWriteTime(FILETIME* pTimeStamp) const
{
   ASSERT(m_hContext != NULL);
   ASSERT(pTimeStamp != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL && pTimeStamp != NULL)
   {
      *pTimeStamp = ((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftLastWriteTime;
      return TRUE;
   }
   else
      return FALSE;
}

BOOL CFindFileW::GetCreationTime(FILETIME* pTimeStamp) const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL && pTimeStamp != NULL)
   {
      *pTimeStamp = ((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftCreationTime;
      return TRUE;
   }
   else
      return FALSE;
}

BOOL CFindFileW::GetLastAccessTime(CTime& refTime) const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL)
   {
      if (CTime::IsValidFILETIME(((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftLastAccessTime))
      {
         refTime = CTime(((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftLastAccessTime);
      }
      else
      {
         refTime = CTime();
      }
      return TRUE;
   }
   else
      return FALSE;
}

BOOL CFindFileW::GetLastWriteTime(CTime& refTime) const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL)
   {
      if (CTime::IsValidFILETIME(((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftLastWriteTime))
      {
         refTime = CTime(((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftLastWriteTime);
      }
      else
      {
         refTime = CTime();
      }
      return TRUE;
   }
   else
      return FALSE;
}

BOOL CFindFileW::GetCreationTime(CTime& refTime) const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   if (m_pFoundInfo != NULL)
   {
      if (CTime::IsValidFILETIME(((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftCreationTime))
      {
         refTime = CTime(((LPWIN32_FIND_DATAW)m_pFoundInfo)->ftCreationTime);
      }
      else
      {
         refTime = CTime();
      }
      return TRUE;
   }
   else
      return FALSE;
}

BOOL CFindFileW::IsDots() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   // return TRUE if the file name is "." or ".." and
   // the file is a directory

   BOOL bResult = FALSE;
   if (m_pFoundInfo != NULL && IsDirectory())
   {
      LPWIN32_FIND_DATAW pFindData = (LPWIN32_FIND_DATAW)m_pFoundInfo;
      if (pFindData->cFileName[0] == '.')
      {
         if (pFindData->cFileName[1] == '\0' ||
            (pFindData->cFileName[1] == '.' &&
               pFindData->cFileName[2] == '\0'))
         {
            bResult = TRUE;
         }
      }
   }

   return bResult;
}

BOOL CFindFileW::FindNextFile()
{
   ASSERT(m_hContext != NULL);

   if (m_hContext == NULL)
      return FALSE;
   if (m_pFoundInfo == NULL)
      m_pFoundInfo = new WIN32_FIND_DATAW;

   ASSERT_VALID(this);

   void* pTemp = m_pFoundInfo;
   m_pFoundInfo = m_pNextInfo;
   m_pNextInfo = pTemp;

   return ::FindNextFileW(m_hContext, (LPWIN32_FIND_DATAW)m_pNextInfo);
}

CStringW CFindFileW::GetFileURL() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   CStringW strResult(L"file://");
   strResult += GetFilePath();
   return strResult;
}

CStringW CFindFileW::GetRoot() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   return m_strRoot;
}

CStringW CFindFileW::GetFilePath() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   CStringW strResult = m_strRoot;
   LPCWSTR pszResult;
   LPCWSTR pchLast;
   pszResult = strResult;
   //pchLast = _wcsdec(pszResult, pszResult + strResult.GetLength());
   pchLast = pszResult + strResult.GetLength()-1;
   ENSURE(pchLast != NULL);
   if ((*pchLast != _T('\\')) && (*pchLast != _T('/')))
      strResult += m_chDirSeparator;
   strResult += GetFileName();
   return strResult;
}

CStringW CFindFileW::GetFileTitle() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   CStringW strFullName = GetFileName();
   CStringW strResult;

   _wsplitpath_s(strFullName, NULL, 0, NULL, 0,
      strResult.GetBuffer(_MAX_FNAME), _MAX_FNAME, NULL, 0);

   strResult.ReleaseBuffer();
   return strResult;
}

CStringW CFindFileW::GetFileName() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   CStringW ret;

   if (m_pFoundInfo != NULL)
      ret = ((LPWIN32_FIND_DATAW)m_pFoundInfo)->cFileName;
   return ret;
}

ULONGLONG CFindFileW::GetLength() const
{
   ASSERT(m_hContext != NULL);
   ASSERT_VALID(this);

   ULARGE_INTEGER nFileSize;

   if (m_pFoundInfo != NULL)
   {
      nFileSize.LowPart = ((LPWIN32_FIND_DATAW)m_pFoundInfo)->nFileSizeLow;
      nFileSize.HighPart = ((LPWIN32_FIND_DATAW)m_pFoundInfo)->nFileSizeHigh;
   }
   else
   {
      nFileSize.QuadPart = 0;
   }

   return nFileSize.QuadPart;
}

#ifdef _DEBUG
void CFindFileW::Dump(CDumpContext& dc) const
{
   CObject::Dump(dc);
   dc << "\nm_hContext = " << (void*)m_hContext;
}

void CFindFileW::AssertValid() const
{
   // if you trip the ASSERT in the else side, you've called
   // a Get() function without having done at least one
   // FindNext() call

   if (m_hContext == NULL)
      ASSERT(m_pFoundInfo == NULL && m_pNextInfo == NULL);
   else
      ASSERT(m_pFoundInfo != NULL && m_pNextInfo != NULL);

}
#endif


IMPLEMENT_DYNAMIC(CFindFileW, CObject)


void TemplateCategoryWnd::ListCategoryTemplates(CArray < CStringW > & stra, CStringW strCategory, CStringW strFilter)
{

   CStringW strLowerFilter(strFilter);

   strLowerFilter.MakeLower();

   WCHAR szPath[MAX_PATH];
   // Get path for each computer, non-user specific and non-roaming data.
   if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA,
      NULL, 0, szPath)))
   {

      CFindFileW find;

      CStringW str(szPath);

      if (find.FindFile(str + "\\Microspot\\Template\\"+strCategory+"\\*.drs"))
      {

         bool bWorking = true;

         while (bWorking)
         {

            bWorking = find.FindNextFileA();

            if (find.IsDots())
            {



            }
            else if (!find.IsDirectory())
            {

               CStringW strName = find.GetFileName();

               CStringW strLowerName(strName);

               strLowerName.MakeLower();

               if (strFilter.IsEmpty()
                  || strLowerName.Find(strLowerFilter) >= 0)
               {


                  stra.Add(find.GetFilePath());

               }

            }

         }

      }


   }

}


void TemplateCategoryWnd::ListAllTemplates(CArray < CStringW > & stra, CStringW strFilter)
{

   WCHAR szPath[MAX_PATH];
   // Get path for each computer, non-user specific and non-roaming data.
   if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA,
      NULL, 0, szPath)))
   {

      CFindFileW find;

      CStringW str(szPath);

      if (find.FindFile(str + "\\Microspot\\Template\\*.*"))
      {

         bool bWorking = true;

         while (bWorking)
         {

            bWorking = find.FindNextFileA();

            if (find.IsDots())
            {



            }
            else if (find.IsDirectory())
            {

               CStringW strCategory(find.GetFileName());

               CArray < CStringW > straTemplates;

               ListCategoryTemplates(straTemplates, strCategory, strFilter);

               if (straTemplates.GetCount() > 0)
               {

                  stra.Append(straTemplates);

               }

            }

         }

      }

   }

}


void TemplateCategoryWnd::UpdateList(const CStringW & strFilter)
{

   {

      CArray < CStringW > straCategories;

      ListCategories(straCategories);

      m_ptemplatewnd->m_combobox.GetList()->m_straItem.RemoveAll();

      CStringW str;

      str.LoadString(IDS_TEMPLATE_ALL_TEMPLATES);

      m_ptemplatewnd->m_combobox.GetList()->m_straItem.Add(str);

      for (int i = 0; i < straCategories.GetCount(); i++)
      {

         CStringW strCategory = straCategories[i];

         m_ptemplatewnd->m_combobox.GetList()->m_straItem.Add(strCategory);

      }

   }

   CStringW strCategory = m_ptemplatewnd->GetCategory();

   CArray < CStringW > straTemplates;

   if (strCategory.IsEmpty())
   {

      ListAllTemplates(straTemplates, strFilter);

   }
   else
   {

      ListCategoryTemplates(straTemplates, strCategory, strFilter);

   }

   m_itema.RemoveAll();

   if(strCategory.IsEmpty() && strFilter.IsEmpty())
   {

      Item item;

      m_itema.Add(item);

   }


   for (int i = 0; i < straTemplates.GetCount(); i++)
   {

      Item item;

      item.m_wstrPath = straTemplates[i];

      m_itema.Add(item);

   }

   //CRecentFileList* pMRUFiles = ((CPCDraftApp*)AfxGetApp())->GetRecentFileList();

   //Group group;

   //CTime timeNow = CTime::GetCurrentTime();

   //int iSize = pMRUFiles->GetSize();

   //for (int i = 0; i < iSize; i++)
   //{

   //   CString strPath = pMRUFiles->operator[](i);

   //   if (MatchDocument(strPath, strFilter))
   //   {

   //      CFile  file;

   //      file.Open(strPath, CFile::modeRead);

   //      if (file.operator HANDLE() != INVALID_HANDLE_VALUE
   //         && file.operator HANDLE() != nullptr)
   //      {

   //         Item item;

   //         item.m_strPath = strPath;

   //         FILETIME ft;

   //         GetFileTime(file, NULL, &ft, NULL);

   //         CTime time(ft);

   //         CTimeSpan span = timeNow - time;

   //         CString strGroup;

   //         int iHours = span.GetTotalHours();

   //         if (iHours < 24)
   //         {

   //            strGroup = "Today";

   //         }
   //         else if (iHours < 48)
   //         {

   //            strGroup = "Yesteday";

   //         }
   //         else if (iHours < 24 * 7)
   //         {

   //            strGroup = "Last Week";

   //         }
   //         else if (iHours < 24 * 30)
   //         {

   //            strGroup = "Last Month";

   //         }
   //         else
   //         {

   //            strGroup = "Old Files";

   //         }

   //         if (group.m_title.m_strTitle != strGroup)
   //         {

   //            if (group.m_itema.GetCount() > 0)
   //            {

   //               m_groupa.Add(group);

   //               group.m_itema.RemoveAll();

   //            }

   //            group.m_title.m_strTitle = strGroup;

   //         }

   //         group.m_itema.Add(item);

   //      }

   //   }

   //}


   //if (group.m_itema.GetCount() > 0)
   //{

   //   m_groupa.Add(group);

   //}

   if (m_itema.IsEmpty())
   {

      m_iSel = -1;

   }
   else
   {

      m_iSel = 0;

   }

   m_iMaxColumnCount = 0;

   layout();

   Invalidate();

}


bool TemplateCategoryWnd::MatchDocument(const CString & strPath, const CString & strFilter)
{

   if (strFilter.IsEmpty())
   {

      return true;

   }


   int iFind = strPath.ReverseFind('\\');

   if (iFind < 0)
   {


      return false;
   }

   CString strTitle = strPath.Mid(iFind + 1);

   strTitle.MakeLower();

   CString strFilterLower(strFilter);

   strFilterLower.MakeLower();

   if (strTitle.Find(strFilterLower) < 0)
   {

      return false;

   }

   return true;

}


void TemplateCategoryWnd::OnLButtonDown(UINT nFlags, CPoint point)
{

   int iItem = hit_test(point);

   if (iItem >= 0)
   {

      m_iHover = iItem;

      m_iSel = iItem;

      Invalidate();

   }

   BaseControl::OnLButtonUp(nFlags, point);

}



void TemplateCategoryWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

   int iItem = hit_test(point);

   if(iItem >= 0 && m_iSel == iItem)
   {

      OnClick(iItem);

      return;

   }

   BaseControl::OnLButtonUp(nFlags, point);

}


void TemplateCategoryWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void TemplateCategoryWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonDblClk(nFlags, point);

}


int TemplateCategoryWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
      return -1;

   enable_scroll_helper();

   //EnableScrollBar(SB_BOTH, TRUE);

   //EnableScrollBarCtrl(SB_BOTH);

   //ModifyStyle(0, WWS_VSCROLL, SWP_FRAMECHANGED);

   //EnableScrollBar(SB_VERT, TRUE);

   //m_buttonNew.Create(NULL, "New", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1000);
   //m_buttonRecent.Create(NULL, "Recent", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1001);
   //m_buttonOpen.Create(NULL, "Open", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_OPEN);

   //m_buttonWhatsNew.Create(NULL, "What's New", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1000);
   //m_buttonHelp.Create(NULL, "Help & Support", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1001);
   //m_buttonCommunity.Create(NULL, "Community", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1002);

   // TODO:  Add your specialized creation code here

   UpdateList();

   return 0;
}


void TemplateCategoryWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   for (int iItem = 0; iItem < m_itema.GetCount(); iItem++)
   {

      Item & item = m_itema[iItem];

      OnDrawItem(iItem, pdc, pgraphics, item);

   }


   CRect rectClient;

   GetClientRect(rectClient);

   //pdc->FillSolidRect(rectClient, RGB(242, 242, 242));

   //Gdiplus::Graphics g(pdc->GetSafeHdc());

   //Gdiplus::RectF rectSrc(0, 0,
   //   m_pbitmap->GetWidth(),
   //   m_pbitmap->GetHeight());

   //Gdiplus::RectF rectDst(
   //   (rectClient.Width() - rectSrc.Width * size_scaler()) / 2,
   //   40 * size_scaler(),
   //   rectSrc.Width * size_scaler(),
   //   rectSrc.Height * size_scaler());

   //pgraphics->DrawImage(m_pbitmap, rectDst, rectSrc, Gdiplus::UnitPixel);

   //int left = 25;

   //Gdiplus::Pen pen(Gdiplus::Color(255, 190, 70, 42), 4.0);

   //pgraphics->DrawLine(&pen, left, m_xSeparator, rectClient.Width() - left, m_xSeparator);
}


int TemplateCategoryWnd::hit_test(const CPoint & pointParam)
{

   CPoint point(pointParam);

   point.y += m_scrollHelper->GetScrollPos().cy;
   point.x += m_scrollHelper->GetScrollPos().cx;

   for (int i = 0; i < m_itema.GetCount(); i++)
   {

      Item & item = m_itema[i];

      if (item.m_rect.PtInRect(point))
      {

         return i;

      }

   }

   return -1;

}


void TemplateCategoryWnd::OnSize(UINT nType, int cx, int cy)
{
   layout();

   BaseControl::OnSize(nType, cx, cy);


}


void TemplateCategoryWnd::layout()
{
   // TODO: Add your message handler code here

   CRect rectClient;

   GetClientRect(rectClient);

   int cx = rectClient.Width();

   int cy = rectClient.Height();

   //m_iThumbnailSize = 200 * size_scaler();

   m_iThumbnailSize = 200;

   int iButtonHeight = m_iThumbnailSize * 3/2;
   int iButtonWidth = m_iThumbnailSize * 3/2;

   int y = 0;

   int xStart = 20;

   int x = xStart;

   CRect rectTotal;

   int iColumn = 0;

   for (int i = 0; i < m_itema.GetCount(); i++)
   {

      Item & item = m_itema[i];

      item.m_rect.left = x;
      item.m_rect.right = x + iButtonWidth;
      item.m_rect.top = y;
      item.m_rect.bottom = y + iButtonHeight;

      x += iButtonWidth;

      if (i == 0)
      {

         rectTotal = item.m_rect;

      }
      else
      {

         rectTotal.UnionRect(rectTotal, item.m_rect);

      }

      iColumn++;

      if (iColumn > m_iMaxColumnCount)
      {

         m_iMaxColumnCount = iColumn;

      }

      if (x + iButtonWidth > cx)
      {

         y += iButtonHeight;
         x = xStart;
         iColumn = 0;

      }


   }

   if (m_scrollHelper)
   {
      m_scrollHelper->SetDisplaySize(rectTotal.Width(), rectTotal.Height());

   }

}


void TemplateCategoryWnd::OnDrawItem(int iItem, CDC * pdc, Gdiplus::Graphics * pgraphics, Item & item)
{

   CRect rectClient(item.m_rect);

   //{

   //   Gdiplus::Color colorBorder;

   //   if (GetFocus() == this)
   //   {

   //      colorBorder = Gdiplus::Color(255, 200, 80, 52);

   //   }
   //   else
   //   {

   //      colorBorder = Gdiplus::Color(255, 192, 192, 192);

   //   }

   //   Gdiplus::Pen pen(colorBorder, 1.0);

   //   Gdiplus::Color colorFill;

   //   if (GetFocus() == this)
   //   {

   //      colorFill = Gdiplus::Color(255, 255, 200, 182);

   //   }
   //   else if (m_bSetHover)
   //   {

   //      colorFill = Gdiplus::Color(255, 242, 242, 242);

   //   }
   //   else
   //   {

   //      colorFill = Gdiplus::Color(255, 255, 255, 255);

   //   }

   //   int iRadius = 11 * size_scaler();

   //   Gdiplus::SolidBrush brush(colorFill);

   //   Gdiplus::GraphicsPath path;

   //   Gdiplus::RectF rect1(item.m_rect.left+(Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f, item.m_rect.top+ 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect1, -90.0f, 90.0f);

   //   Gdiplus::RectF rect2(item.m_rect.left + (Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f, item.m_rect.top + (Gdiplus::REAL)(rectClient.Height() - iRadius) - 2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect2, 0.0f, 90.0f);

   //   Gdiplus::RectF rect3(item.m_rect.left + 1.0f, item.m_rect.top + (Gdiplus::REAL)(rectClient.Height() - iRadius) - 2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect3, 90.0f, 90.0f);

   //   Gdiplus::RectF        rect4(item.m_rect.left + 1.0f, item.m_rect.top + 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect4, 180.0f, 90.0f);

   //   path.CloseFigure();

   //   pgraphics->FillPath(&brush, &path);

   //   pgraphics->DrawPath(&pen, &path);

   //}



   if (m_pfontLine1 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Semibold", &installedFontCollection);

      m_pfontLine1 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (15.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   if (m_pfontLine2 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Semibold", &installedFontCollection);

      m_pfontLine2 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (12.0f * font_scaler()),Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   CStringW str(item.m_wstrPath);

   CStringW strLine1;

   CStringW strLine2;


   int iLeft = rectClient.left + 30;

   if (str.IsEmpty())
   {

      str.LoadString(IDS_WELCOME_BLANK_DOCUMENT);

      strLine1 = str;

   }
   else
   {


      auto iFind = str.ReverseFind('\\');

      if (iFind >= 0)
      {

         strLine2 = str.Left(iFind);

         strLine1 = str.Mid(iFind + 1);

      }
      else
      {

         strLine1 = str;

      }

      strLine1 = strLine1.Left(strLine1.GetLength() - 4);

   }

   
   int iLineHeight1 = 0;

   Gdiplus::RectF rectF1(iLeft, 0, rectClient.Width(), rectClient.Height());

   CStringW wstrLine1 = strLine1;

   Gdiplus::StringFormat format;

   format.SetLineAlignment(::Gdiplus::StringAlignmentNear);

   if (!strLine1.IsEmpty())
   {

      Gdiplus::RectF        rectOut;

      pgraphics->MeasureString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &rectOut);

      iLineHeight1 = rectOut.Height;

   }


   rectF1.Y = rectClient.bottom - iLineHeight1-10;
   rectF1.Height = iLineHeight1;





   Gdiplus::REAL spacing;

   Gdiplus::Rect rIcon;


   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192), 1.0);

      rIcon.Width = m_iThumbnailSize * size_scaler();
      rIcon.Height = m_iThumbnailSize * size_scaler();

      if (!item.m_pbitmap && rIcon.Width > 0 && rIcon.Height > 0)
      {

         CDC dc;
         dc.CreateCompatibleDC(pdc);
         item.load_bk();
         //item.m_pdoc->m_fZoomFactor = 0.2;

         CSize size(item.m_pdoc->GetTotalDevSize());

         if (size.cx > 0 && size.cy > 0)
         {


            //double dW = (double)size.cx  / (double)rIcon.Width;
            //double dH = (double)size.cy / (double)rIcon.Height;

            //double d = max(dW, dH);

            //rIcon.Width = size.cx / d;
            //rIcon.Height = size.cy / d;


            dc.OffsetViewportOrg(0, 0);

            CBitmap bitmap;
            COLORREF * pdib = NULL;

            Gdiplus::Bitmap * pbitmap2 = NULL;

            Gdiplus::Bitmap * pbitmap = NULL;

            HBITMAP hbitmap = CreateDIB(size.cx, size.cy, NULL, 0, &pdib, false);

            if (hbitmap == NULL)
            {

            }

            bitmap.Attach(hbitmap);


            dc.SelectObject(bitmap);

            dc.FillSolidRect(0, 0, size.cx, size.cy, RGB(255, 255, 255));

            item.generate_thumbnail(&dc);

            BITMAP b;

            bitmap.GetBitmap(&b);

#undef new

            pbitmap = new Gdiplus::Bitmap(b.bmWidth, b.bmHeight);
            //pbitmap = new Gdiplus::Bitmap((HBITMAP) bmp.GetSafeHandle(), NULL);

#ifdef _DEBUG
#endif

            Gdiplus::Rect r2;

            r2.X = 0;

            r2.Y = 0;

            r2.Width = pbitmap->GetWidth();

            r2.Height = pbitmap->GetHeight();

            Gdiplus::BitmapData data;

            pbitmap->LockBits(&r2, 0, PixelFormat32bppARGB, &data);

            int bA = 255;

            if (bA == 255)
            {

               for (int i = 0; i < r2.Height; i++)
               {

                  memcpy(&((byte *)data.Scan0)[data.Stride * i], &((COLORREF*)b.bmBits)[i * r2.Width], r2.Width * sizeof(COLORREF));

               }

            }
            else
            {

               for (int i = 0; i < r2.Height; i++)
               {

                  for (int j = 0; j < r2.Width; j++)
                  {

                     BYTE * ps = (BYTE*)&((COLORREF*)b.bmBits)[i * r2.Width + j];
                     BYTE * pd = (BYTE*)&((byte *)data.Scan0)[data.Stride * i + j * sizeof(COLORREF)];

                     pd[0] = ps[0] * bA / 255;
                     pd[1] = ps[1] * bA / 255;
                     pd[2] = ps[2] * bA / 255;
                     pd[3] = ps[3] * bA / 255;
                     //pd[0] =pd[0] * bA / 255;
                     //pd[1] =pd[1] * bA / 255;
                     //pd[2] =pd[2] * bA / 255;
                     //pd[3] =pd[3] * bA / 255;

                  }

               }

            }

            pbitmap->UnlockBits(&data);

            double dW = (double)rIcon.Width / (double)b.bmWidth;
            double dH = (double)rIcon.Height / (double)b.bmHeight;
            double d = min(dW, dH);

            int cx = d * b.bmWidth;
            int cy = d * b.bmHeight;

            int iMargin = 10;

            cx += iMargin*2;
            cy += iMargin*2;

            pbitmap2 = new Gdiplus::Bitmap(cx, cy);

            Gdiplus::Graphics g(pbitmap2);

            Gdiplus::RectF r;
            r.X = 0;
            r.Y = 0;
            r.Width = cx;
            r.Height = cy;
            Gdiplus::SolidBrush b1(Gdiplus::Color(255, 255, 255, 255));
            g.FillRectangle(&b1, r);
            Gdiplus::RectF r1(r);
            r1.X += iMargin;
            r1.Y += iMargin;
            r1.Width -= iMargin;
            r1.Height -= iMargin;
            g.DrawImage(pbitmap, r1);

            Gdiplus::Pen p1(Gdiplus::Color(255, 128, 128, 128), 1.0f);
            r.Width--;
            r.Height--;

            g.DrawRectangle(&p1, r);

            item.m_pbitmap = pbitmap2;

         }

      }

      if (item.m_pbitmap)
      {

         rIcon.Width = item.m_pbitmap->GetWidth() * size_scaler();
         rIcon.Height = item.m_pbitmap->GetHeight() * size_scaler();

      }

      rIcon.Y = rectClient.bottom - rIcon.Height - iLineHeight1 -20;

      spacing = rIcon.Y * 1.25f;

      rIcon.X = rectClient.left + (rectClient.Width() - rIcon.Width) / 2.0f;

      
      Gdiplus::Rect rect1(rIcon);

      if(iItem == m_iHover || iItem == m_iSel)
      {
      
         rect1.X -= 10;
         rect1.Y -= 10;
         rect1.Width += 20;
         rect1.Height = rectF1.Y + rectF1.Height + 10 - rect1.Y;

         if (iItem == m_iSel)
         {

            Gdiplus::SolidBrush bBackground(Gdiplus::Color(255, 255, 200, 182));

            pgraphics->FillRectangle(&bBackground, rect1);

         }
         else
         {

            Gdiplus::SolidBrush bBackground(Gdiplus::Color(255, 182, 182, 182));

            pgraphics->FillRectangle(&bBackground, rect1);

         }

      }

      if (!strLine1.IsEmpty())
      {


         Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
         //rectF1.X += item.m_rect.left;
         //rectF1.Y += rIcon.Height;

         rectF1.X = rIcon.X;

         pgraphics->DrawString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &brush);

      }

      if (item.m_pbitmap)
      {

         pgraphics->DrawImage(item.m_pbitmap, rIcon);

      }
      else
      {

         pgraphics->DrawRectangle(&pen, rIcon);

      }

      if (iItem == m_iSel)
      {

         Gdiplus::Pen pen(Gdiplus::Color(255, 255, 180, 122), 2.0);

         pgraphics->DrawRectangle(&pen, rect1);

      }


   }


}


 TemplateCategoryWnd::Item::Item()
{

   m_pdoc = nullptr;
   m_pbitmap = nullptr;

}

 TemplateCategoryWnd::Item::~Item()
 {

    if (m_pdoc != NULL)
    {

       try
       {

          m_pdoc->DeleteContents();

       }
       catch (...)
       {

       }

       try
       {

          delete m_pdoc;

       }
       catch (...)
       {

       }

    }



 }


void TemplateCategoryWnd::Item::load_bk()
{

   m_pdoc = new CPCDraftDoc();

   m_pdoc->m_bUpdateViews = false;

   m_pdoc->OnNewDocument();

   CStringW strDrs(m_wstrPath);

   if (!strDrs.IsEmpty())
   {

      m_pdoc->OnOpenDocument(get_utf8(strDrs));

      m_pdoc->SetModifiedFlag(FALSE);

      if (m_pdoc->m_Layers.GetCount() <= 0)
      {
         throw "invalid file";
         return;
      }

   }

   m_Mapper.SetDocument(m_pdoc);

}








void TemplateCategoryWnd::Item::generate_thumbnail(CDC * pDC)
{

   //GetSelectionExtents(&rectSelection);

   CPCDraftDoc* pDoc = m_pdoc;
   ASSERT_VALID(pDoc);


   CDraftContext dc(pDC, &m_Mapper, pDoc);
   dc.m_bCheckRectVisible = false;

   m_Mapper.m_bZoomLineWeights = ((CPCDraftApp*)AfxGetApp())->m_bZoomLineWeights;


   for (int i = 0; i < pDoc->m_Layers.GetSize(); i++)
   {
      CLayer* pLayer = pDoc->GetLayer(i);
      if (pLayer->Draw(&dc, pDoc->m_nActiveLayer == i, pDoc->m_bHideText, nullptr))
      {
         break;
      }
   }

}


void TemplateCategoryWnd::OnMouseMove(UINT nFlags, CPoint point)
{

   int iHover = hit_test(point);

   int iHoverOld = m_iHover;

   if (iHover != m_iHover)
   {

      if (m_iHover < 0 && iHover >= 0)
      {

         TRACKMOUSEEVENT tme = {};
         tme.cbSize = sizeof(tme);
         tme.dwFlags = TME_LEAVE;
         TrackMouseEvent(&tme);


      }

      m_iHover = iHover;

      if (iHoverOld >= 0 && iHoverOld < m_itema.GetCount())
      {

         InvalidateRect(m_itema[iHoverOld].m_rect - m_scrollHelper->GetScrollPos());

      }

      if (iHover >= 0 && iHover < m_itema.GetCount())
      {

         InvalidateRect(m_itema[iHover].m_rect - m_scrollHelper->GetScrollPos());

      }

   }
   

   BaseControl::OnMouseMove(nFlags, point);
}


void TemplateCategoryWnd::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnMouseLeave();

   m_iHover = -1;

   Invalidate();

}


void TemplateCategoryWnd::ensure_item_visible(int iItem)
{

   CRect rect;

   GetClientRect(rect);

   rect.OffsetRect(m_scrollHelper->m_scrollPos);

   if (!Contains(rect, m_itema[iItem].m_rect))
   {

      m_scrollHelper->m_scrollPos.cx = 0;
      m_scrollHelper->m_scrollPos.cy = m_itema[iItem].m_rect.top;

   }



}


void TemplateCategoryWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnKeyDown(nChar, nRepCnt, nFlags);


   if (nChar == VK_RETURN || nChar == VK_SPACE)
   {

      OnClick(m_iSel);

   }
   else if (nChar == VK_LEFT)
   {

      if (m_iSel > 0)
      {

         m_iSel--;

         ensure_item_visible(m_iSel);

         Invalidate();

      }

   }
   else if (nChar == VK_RIGHT)
   {

      if (m_iSel < m_itema.GetUpperBound())
      {

         m_iSel++;

         ensure_item_visible(m_iSel);

         Invalidate();

      }

   }
   else if (nChar == VK_UP)
   {

      if (m_iSel - m_iMaxColumnCount >= 0)
      {

         m_iSel-= m_iMaxColumnCount;

         ensure_item_visible(m_iSel);

         Invalidate();

      }

   }
   else if (nChar == VK_DOWN)
   {

      if (m_iSel + m_iMaxColumnCount <= m_itema.GetUpperBound())
      {

         m_iSel+= m_iMaxColumnCount;

         ensure_item_visible(m_iSel);

         Invalidate();

      }

   }

}


void TemplateCategoryWnd::OnClick(int iItem)
{

   if (iItem < 0)
   {

      return;

   }

   if (iItem >= m_itema.GetCount())
   {

      return;

   }

   Item & item = m_itema[iItem];

   if (((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd)
   {

      ((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd->ShowWindow(SW_HIDE);

   }

   if (((CPCDraftApp*)AfxGetApp())->m_pGetStartedWnd)
   {

      ((CPCDraftApp*)AfxGetApp())->m_pGetStartedWnd->ShowWindow(SW_HIDE);

   }

   ((CPCDraftApp*)AfxGetApp())->ShowMainFrame();

   if (item.m_wstrPath.IsEmpty())
   {

      ((CPCDraftApp*)AfxGetApp())->OnFileNewDrawing();

   }
   else
   {

      ((CPCDraftApp*)AfxGetApp())->m_bWorkaroundUTF8 = true;

      ((CPCDraftApp*)AfxGetApp())->OpenDocumentFile(get_utf8(item.m_wstrPath));

   }

}