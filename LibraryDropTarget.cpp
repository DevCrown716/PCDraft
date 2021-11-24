// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include <afxole.h>
#include "LibraryDropTarget.h" 
#include "LibraryObject.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DROPEFFECT drop_target::OnDragOver(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point)
{
   return DROPEFFECT_NONE;
}

DROPEFFECT drop_target::OnDragEnter(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point)
{
   return DROPEFFECT_NONE;
}

CLibraryDropTarget::CLibraryDropTarget()
{
   
   m_ptarget = NULL;
   m_pwndDrop = NULL;
   m_bDisableDrop = false;
   m_iaAcceptFormat.Add(CF_OWNERDISPLAY);

}


CLibraryDropTarget::~CLibraryDropTarget()
{

}


DROPEFFECT CLibraryDropTarget::OnDragEnter(CWnd* pWnd,COleDataObject * pDataObject,DWORD dwKeyState,CPoint point)
{

   if(m_ptarget != NULL)
   {

      return m_ptarget->OnDragEnter(pWnd,pDataObject,dwKeyState,point);

   }

   for(int i = 0; i < m_iaAcceptFormat.GetCount(); i++)
   {
      if(pDataObject->IsDataAvailable(m_iaAcceptFormat[i]))
      {
         return DROPEFFECT_COPY;
      }
   }


   return DROPEFFECT_NONE;
}


void CLibraryDropTarget::OnDragLeave(CWnd* pWnd)
{

   COleDropTarget:: OnDragLeave(pWnd);

}


DROPEFFECT CLibraryDropTarget::OnDragOver(CWnd* pWnd,COleDataObject * pDataObject,DWORD dwKeyState,CPoint point)
{

   if(m_ptarget != NULL)
   {

      return m_ptarget->OnDragOver(pWnd,pDataObject,dwKeyState,point);

   }

   if(m_bDisableDrop)
   {

      return DROPEFFECT_NONE;

   }

   for(int i = 0; i < m_iaAcceptFormat.GetCount(); i++)
   {
      if(pDataObject->IsDataAvailable(m_iaAcceptFormat[i]))
      {
         return DROPEFFECT_COPY;
      }
   }
   
   return DROPEFFECT_NONE;

}

void get_hdrop(CStringArray & stra, HDROP hDrop)
{

   UINT uNumFiles = DragQueryFile(hDrop,-1,NULL,0);

   for(UINT uFile = 0; uFile < uNumFiles; uFile++)
   {
      // Get the next filename from the HDROP info.
      UINT uLen = DragQueryFileW(hDrop,uFile,NULL,0);

      if(uLen > 0)
      {
         CStringW wstr;

         LPWSTR lpsz = wstr.GetBufferSetLength(uLen + 1);

         if(DragQueryFileW(hDrop,uFile,lpsz,uLen + 1) > 0)
         {

            stra.Add(get_cp(lpsz));

         }

      }

      //   // Get the index of the file's icon in the system image list and
      //   // it's type description.

      //   SHGetFileInfo(szNextFile,0,&rFileInfo,sizeof(rFileInfo),
      //      SHGFI_SYSICONINDEX | SHGFI_ATTRIBUTES |
      //      SHGFI_TYPENAME);

      //   // Set up an LVITEM for the file we're about to insert.

      //   ZeroMemory(&rItem,sizeof(LVITEM));

      //   rItem.mask    = LVIF_IMAGE | LVIF_TEXT;
      //   rItem.iItem   = nIndex;
      //   rItem.pszText = szNextFile;
      //   rItem.iImage  = rFileInfo.iIcon;

      //   // If the file has the hidden attribute set, its attributes in
      //   // the SHFILEINFO struct will include SFGAO_GHOSTED.  We'll mark
      //   // the item as "cut" in our list to give it the same ghosted look.

      //   if(rFileInfo.dwAttributes & SFGAO_GHOSTED)
      //   {
      //      rItem.mask |= LVIF_STATE;
      //      rItem.state = rItem.stateMask = LVIS_CUT;
      //   }

      //   // Insert it into the list!

      //   m_pList->InsertItem(&rItem);

      //   // Set column #1 to the file's type description.

      //   m_pList->SetItemText(nIndex,1,rFileInfo.szTypeName);

      //   // Get the file size, and put that in column #2.

      //   hFind = FindFirstFile(szNextFile,&rFind);

      //   if(INVALID_HANDLE_VALUE != hFind)
      //   {
      //      StrFormatByteSize(rFind.nFileSizeLow,szFileLen,64);
      //      FindClose(hFind);
      //   }

      //   m_pList->SetItemText(nIndex,2,szFileLen);

      //   nIndex++;
      //}
   }   // end for


}

BOOL CLibraryDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect,CPoint point)
{
   
   if(m_bDisableDrop)
   {

      return FALSE;

   }

   if(dropEffect & DROPEFFECT_COPY)
   {

      HGLOBAL hGlobal = pDataObject->GetGlobalData(CF_HDROP);

      if(hGlobal != NULL)
      {

         

         HDROP hDrop = (HDROP)GlobalLock(hGlobal);


         m_straFiles.RemoveAll();
         
         get_hdrop(m_straFiles,hDrop);

         GlobalUnlock(hGlobal);

         m_pwndDrop->PostMessage(WM_APP + 1026, 0,(LPARAM)(CStringArray *)  &m_straFiles);

         return TRUE;

      }

      hGlobal = pDataObject->GetGlobalData(CF_OWNERDISPLAY);

      if(hGlobal != NULL)
      {

      LPCSTR psz = (LPCSTR)GlobalLock(hGlobal);


      ASSERT(psz != NULL);

      CLibraryItem * pitem = (CLibraryItem *)strtoull(psz,NULL,10);

      if(pitem != NULL)
      {

         m_pwndDrop->PostMessage(WM_APP + 1024,MAKELPARAM(point.x,point.y),(LPARAM)pitem);

      }

      GlobalUnlock(hGlobal);

      return TRUE;
   }


   }

   return TRUE;

}


BOOL CLibraryDropTarget::Initialize(CWnd* pwnd)
{
   
   BOOL success = Register(pwnd);
   
   if(!success)
      return FALSE;

   m_pwndDrop = pwnd;

   return TRUE;


}
