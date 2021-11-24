// Library - PCDraft 6 - Infinisys Ltd.
#include "StdAfx.h"
#include "LibraryPhoto.h"
#include "Layer.h"
#include <gdiplus.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void get_subdir(CString str,CStringArray & straPath,CStringArray & straName);
void get_files(CString str,CStringArray & straPath,CStringArray & straName);

LPWSTR ConvertString(const CString instr);

int GetEncoderClsid(const WCHAR* format,CLSID* pClsid);
CLibraryPhoto::CLibraryPhoto()
{

   m_pbitmap = NULL;
   m_pbitmapThumbnail = NULL;

}


CLibraryPhoto::~CLibraryPhoto()
{

   if(m_pbitmapThumbnail)
   {

      delete m_pbitmapThumbnail;

   }
   if(m_pbitmap == NULL)
   {

      delete m_pbitmap;

   }

}
UINT AFX_CDECL CPhotoLibrary::ThreadProcLoadBk(PVOID p)
{

   CPhotoLibrary * plibrary = (CPhotoLibrary *)p;

   for(int i = 0; i < plibrary->get_count(); i++)
   {

      plibrary->get_item(i)->load_thumbnail();

   }

   return 0;

}

void CLibraryPhoto::load_thumbnail()
   {

      if(m_pbitmapThumbnail == NULL)
      {

         return;

      }

      Gdiplus::Graphics g(m_pbitmapThumbnail);

      Gdiplus::RectF r;

      g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);

      g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

      r.Width  = m_pbitmapThumbnail->GetWidth();

      r.Height = m_pbitmapThumbnail->GetHeight();

      r.X      = 0;

      r.Y      = 0;

      Gdiplus::Color color(255,255,255);

      Gdiplus::SolidBrush br(color);

      g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

      g.FillRectangle(&br,r);

      g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

      g.DrawImage(m_pbitmap,r,0,0,m_pbitmap->GetWidth(),m_pbitmap->GetHeight(),Gdiplus::UnitPixel);

      g.Flush();

   }


void CLibraryPhoto::load()
{

   if(m_pbitmapThumbnail != NULL)
   {

      delete m_pbitmapThumbnail;

   }

   if(m_pbitmap != NULL)
   {

      delete m_pbitmap;

   }

   m_strPath.TrimRight("/");

   m_strPath.TrimRight("\\");

   m_strName = m_strPath;

   int iFindSlash = max(m_strName.ReverseFind('\\'),m_strName.ReverseFind('/')) + 1;

   m_strName = m_strName.Mid(iFindSlash);

   int iFindExtension = m_strName.ReverseFind('.');

   if(iFindExtension >= 0)
   {

      m_strName = m_strName.Left(iFindExtension);

   }


   CString strThumb(m_strPath);

   LPWSTR pwsz = ConvertString(strThumb);

   m_pbitmap = Gdiplus::Bitmap::FromFile(pwsz);

   if(m_pbitmap == NULL || m_pbitmap->GetWidth() == 0 || m_pbitmap->GetHeight() == 0)
   {
      delete pwsz;

      TRACE("file \"%S\" is not picture", pwsz);

      return;
   }

   int w = m_pbitmap->GetWidth();

   int h = m_pbitmap->GetHeight();

   double dRate = min(600.0 / w,600.0 / h);

   if(dRate > 1.0)
   {

      dRate = 1.0;

   }

   int wNew = w * dRate;

   int hNew = h * dRate;
#undef new
   m_pbitmapThumbnail = new Gdiplus::Bitmap(wNew,hNew);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   

   delete pwsz;

}


CLibraryPhoto * CPhotoLibrary::get_item(int iItem)
{

   if(!m_strFilter.IsEmpty())
   {

      return m_itema.ElementAt(m_ia[iItem]);

   }

   return m_itema.ElementAt(iItem);

}
int CPhotoLibrary::get_count()
{
   if(!m_strFilter.IsEmpty())
   {

      return m_ia.GetCount();

   }

   return m_itema.GetCount();

}

bool contains_no_case(CString str,CString strFind);
void CPhotoLibrary::update_filter(CString strFilter, int & iSel)
{

   m_ia.RemoveAll();

   if(!strFilter.IsEmpty())
   {

      if(!m_strFilter.IsEmpty() && iSel >= 0 && iSel < m_ia.GetCount())
      {

         iSel = m_ia[iSel];

      }
      else
      {

         iSel = -1;

      }

      bool bFoundSel = false;

      for(int i = 0; i < m_itema.GetCount(); i++)
      {

         if(contains_no_case(m_itema.ElementAt(i)->m_strName,strFilter))
         {
            m_ia.Add(i);

            if(i == iSel)
            {

               iSel = m_ia.GetUpperBound();

               bFoundSel = true;

            }
         }


      }

      if(!bFoundSel)
      {

         iSel = -1;

      }

   }
   m_strFilter = strFilter;


   
}



CPhotoLibrary::CPhotoLibrary()
{

}


CPhotoLibrary::~CPhotoLibrary()
{

   destroy();

}



void CPhotoLibrary::destroy()
{

   for(int i = 0; i < m_itema.GetCount(); i++)
   {

      try
      {

         delete m_itema.ElementAt(i);

      }
      catch(...)
      {

      }

   }

   m_itema.RemoveAll();

}

int CPhotoLibrary::unfiltered_name_index(CString strName)
{

   for(int i = 0; i < m_itema.GetCount(); i++)
   {

      try
      {

         if(m_itema[i]->m_strName.CompareNoCase(strName) == 0)
         {

            return i;

         }

      }
      catch(...)
      {

      }

   }

   return -1;

}

CLibraryPhoto * CPhotoLibrary::unfiltered_name_item(CString strName)
{

   int iItem = unfiltered_name_index(strName);

   if(iItem < 0)
   {

      return NULL;

   }

   return m_itema[iItem];

}

void CPhotoLibrary::library_add(CLibraryPhoto * pitem)
{

   Gdiplus::Bitmap  * pbitmapSource = pitem->m_pbitmapThumbnail;

   pitem->m_pbitmapThumbnail = NULL;

   int w = pbitmapSource->GetWidth();

   int h = pbitmapSource->GetHeight();

   if(w <= 0 || h <= 0)
   {

      throw "invalid bitmap";

   }

   double dRate = min(300.0 / w,300.0 / h);

   if(dRate > 1.0)
   {

      dRate = 1.0;

   }

   int wNew = w * dRate;

   int hNew = h * dRate;
#undef new
   pitem->m_pbitmapThumbnail = new Gdiplus::Bitmap(wNew,hNew);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   {

      Gdiplus::Graphics g(pitem->m_pbitmapThumbnail);

      Gdiplus::RectF r;

      g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);

      g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

      r.Width  = wNew;

      r.Height = hNew;

      r.X      = 0;

      r.Y      = 0;

      Gdiplus::Color color(255,255,255);

      Gdiplus::SolidBrush br(color);

      g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

      g.FillRectangle(&br,r);

      g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

      g.DrawImage(pbitmapSource,r,0,0,w,h,Gdiplus::UnitPixel);

      g.Flush();

   }

   delete pbitmapSource;

   m_itema.Add(pitem);

}


void CPhotoLibrary::load()
{

   m_strPath.TrimRight("/");

   m_strPath.TrimRight("\\");

   if(!PathIsDirectory(m_strPath))
   {

      return;

   }

   CString str(m_strPath);

   CStringArray straPath;

   CStringArray straName;

   get_files(str,straPath,straName);

   for(int i = 0; i < straPath.GetCount();i++)
   {

      CString strPath = straPath[i];

      CString strName = straName[i];

      CLibraryPhoto * pitem = unfiltered_name_item(strName);

      bool bAdd = false;

      bool bDelete = false;

      if(pitem == NULL)
      {

         pitem = new CLibraryPhoto();

         pitem->m_strName = strName;

         pitem->m_strPath = strPath;

         bAdd = true;

      }

      try
      {

         pitem->load();

      }
      catch(...)
      {

         if(bAdd)
         {

            bDelete = true;

         }

      }

      if(bDelete)
      {

         delete pitem;

      }
      else if(bAdd)
      {

         m_itema.Add(pitem);

      }

   }

//   AfxBeginThread(ThreadProcLoadBk,this,THREAD_PRIORITY_BELOW_NORMAL);

}




