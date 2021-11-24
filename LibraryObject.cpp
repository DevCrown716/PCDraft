// Library - PCDraft 6 - Infinisys Ltd.
#include "StdAfx.h"
#include "LibraryObject.h"
#include "PCDraftDoc.h"
#include "Layer.h"
#include <gdiplus.h>
#include <share.h>
CString GetAppDataPath();
CString acp_to_utf8(CString str);
CString utf8_to_acp(CString str);
void del_dir(const char * pszPath);
bool is_spotlibrary_path(CString strPath);
void get_folder_name(CString & strFolder, const char *psz);
double __double_rate(int i, int iCount);
double __double(int i);
//CStringW get_uni(LPCSTR lpcsz, int iCodePage = CP_UTF8);
bool file_as_mem(CArray < BYTE > & a,const char *psz)
{

   CFILE f(psz,L"rb",_SH_DENYNO);

   try
   {

      CStdioFile file(f);

      a.SetSize(file.GetLength());

      file.Read(a.GetData(),a.GetSize());

   }
   catch(...)
   {

   }

   return true;

}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
bool is_valid_uuid(const char * psz)
{
   
   UUID uuid;
   
   return UuidFromString((RPC_CSTR)(const char *)psz,&uuid) == RPC_S_OK;

}
/* portions derived from IJG code */

#define readbyte(a,b) do if(((a)=getc((b))) == EOF) return 0; while (0)
#define readword(a,b) do { int cc_=0,dd_=0; \
                          if((cc_=getc((b))) == EOF \
        		  || (dd_=getc((b))) == EOF) return 0; \
                          (a) = (cc_<<8) + (dd_); \
                          } while(0)

int scanhead(FILE * infile,int * image_width,int * image_height) {
   int marker=0;
   int dummy=0;
   if(getc(infile) != 0xFF || getc(infile) != 0xD8)
      return 0;

   for(;
      ;) {


      int discarded_bytes=0;
      readbyte(marker,infile);
      while(marker != 0xFF) {
         discarded_bytes++;
         readbyte(marker,infile);
      }
      do readbyte(marker,infile); while(marker == 0xFF);

      if(discarded_bytes != 0) return 0;

      switch(marker) {
      case 0xC0:
      case 0xC1:
      case 0xC2:
      case 0xC3:
      case 0xC5:
      case 0xC6:
      case 0xC7:
      case 0xC9:
      case 0xCA:
      case 0xCB:
      case 0xCD:
      case 0xCE:
      case 0xCF: {
         readword(dummy,infile);	/* usual parameter length count */
         readbyte(dummy,infile);
         readword((*image_height),infile);
         readword((*image_width),infile);
         readbyte(dummy,infile);

         return 1;
         break;
      }
      case 0xDA:
      case 0xD9:
         return 0;
      default: {
         int length;

         readword(length,infile);

         if(length < 2)
            return 0;
         length -= 2;
         while(length > 0) {
            readbyte(dummy,infile);
            length--;
         }
      }
               break;
      }
   }
}

void get_subdir(CString str,CStringArray & straPath, CStringArray & straName);

#pragma comment(lib, "Rpcrt4.lib")

LPWSTR ConvertString(const CString instr);
extern CMutex * g_pmutexLibrary;
CMutex * library_mutex()
{

   return g_pmutexLibrary;

}

int GetEncoderClsid(const WCHAR* format,CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num,&size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num,size,pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if(wcscmp(pImageCodecInfo[j].MimeType,format) == 0)
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

CString new_uuid()
{

   CString strUuid;

   UUID uuid;

   UuidCreate(&uuid);

   char * pszUuid = NULL;

   UuidToStringA(&uuid,(RPC_CSTR*)&pszUuid);

   strUuid = pszUuid;

   RpcStringFreeA((RPC_CSTR*)&pszUuid);

   return strUuid;

}

CLibraryItem::CLibraryItem()
{

   m_plibrary = NULL;
   m_pdoc = NULL;
   m_bNew = true;
   m_pbitmapThumbnail = NULL;
   m_pphoto = NULL;
   m_wThumb = -1;
   m_hThumb = -1;

}


CLibraryItem::CLibraryItem(CLibraryPhoto * pphoto)
{
   m_plibrary = NULL;
   m_pphoto = pphoto;
}


CLibraryItem::CLibraryItem(CLibraryItem * pitem) :
CLibraryItem(*pitem->m_pbitmapThumbnail,pitem->m_pdoc)
{

   m_plibrary = NULL;
   m_bNew = true;
   m_pdoc = NULL;
   m_pphoto = NULL;
}


CLibraryItem::CLibraryItem(Gdiplus::Bitmap & bitmapSource, CPCDraftDoc * pdoc)
{
   
   m_plibrary = NULL;
   m_pphoto = NULL;

   m_bNew = true;
   
   m_pdoc = pdoc;
   
   m_pbitmapThumbnail = NULL;

   int w = bitmapSource.GetWidth();

   int h = bitmapSource.GetHeight();

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
   m_pbitmapThumbnail = new Gdiplus::Bitmap(wNew,hNew);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   {

      Gdiplus::Graphics g(m_pbitmapThumbnail);

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

      g.DrawImage(&bitmapSource,r,0,0,w,h,Gdiplus::UnitPixel);

      g.Flush();

   }

}


CLibraryItem::~CLibraryItem()
{


   try
   {

      if(m_pbitmapThumbnail)
      {

         delete m_pbitmapThumbnail;

      }

   }
   catch(...)
   {


   }

   if(m_pdoc != NULL)
   {

      try
      {

         m_pdoc->DeleteContents();

      }
      catch(...)
      {

      }

      try
      {

         delete m_pdoc;

      }
      catch(...)
      {

      }

   }


}

// StartDragging - begins dragging a bitmap. 
// Returns TRUE if successful, or FALSE otherwise. 
// hwnd - handle to the window in which the bitmap is dragged. 
// ptCur - coordinates of the cursor. 
// himl - handle to the image list. 
// 
// Global variables 
//     g_rcImage - bounding rectangle of the image to drag. 
//     g_nImage - index of the image. 
//     g_ptHotSpot - location of the image's hot spot. 
//     g_cxBorder and g_cyBorder - width and height of border. 
//     g_cyCaption and g_cyMenu - height of title bar and menu bar. 

// MoveTheImage - drags an image to the specified coordinates. 
// Returns TRUE if successful, or FALSE otherwise. 
// ptCur - new coordinates for the image. 


// StopDragging - ends a drag operation and draws the image 
// at its final location. 
// Returns TRUE if successful, or FALSE otherwise. 
// hwnd - handle to the window in which the bitmap is dragged. 
// himl - handle to the image list. 
// ptCur - coordinates of the cursor. 
// 
// Global variable 
//     g_ptHotSpot - location of the image's hot spot. 
void CLibraryItem::load()
{
   
   if(!PathIsDirectoryW(get_uni(m_strPath)))
   {

      throw "invalid path: path should be a existing directory";

      return;

   }

   m_bNew = false;

   bool bCache = false;

   if (m_plibrary != NULL)
   {

      auto & cacheitem = m_plibrary->m_mapCache[m_strUuid];

      if (cacheitem.m_strName.GetLength() > 0)
      {

         m_strName = cacheitem.m_strName;

         bCache = true;

      }

   }

   if (!bCache)
   {

      try
      {

         CString strText(m_strPath);

         strText += "\\SpotLibraryItem_Name.txt";

         if (PathFileExistsW(get_uni(strText)))
         {

            CFILE f(strText, L"r", _SH_DENYNO);

            CStdioFile file(f);

            file.ReadString(m_strName);

            if (m_plibrary)
            {

               auto & cacheitem = m_plibrary->m_mapCache[m_strUuid];

               cacheitem.m_strName = m_strName;

               m_plibrary->m_bCacheModified = true;


            }

         }

      }
      catch (...)
      {

      }

   }


}

CSize CLibraryItem::get_thumbnail_size()
{

   if(m_wThumb >= 0 && m_hThumb >= 0)
   {

      return CSize(m_wThumb,m_hThumb);

   }
   if(m_pbitmapThumbnail != NULL)
   {

      m_wThumb = m_pbitmapThumbnail->GetWidth();

      m_hThumb = m_pbitmapThumbnail->GetHeight();

   }
   else if (m_plibrary != NULL)
   {

      auto & cacheitem = m_plibrary->m_mapCache[m_strUuid];

      if (cacheitem.m_wThumb < 0 || cacheitem.m_hThumb < 0)
      {

         m_plibrary->m_bCacheModified = true;

         m_strPath.TrimRight("/");

         m_strPath.TrimRight("\\");

         CString strThumb(m_strPath);

         strThumb += "\\SpotLibraryItem_Thumbnail.jpg";

         CFILE f(strThumb, L"rb", _SH_DENYNO);

         if (f != NULL)
         {

            scanhead(f, &cacheitem.m_wThumb, &cacheitem.m_hThumb);

            fclose(f);

         }

      }

      m_wThumb = cacheitem.m_wThumb;

      m_hThumb = cacheitem.m_hThumb;

   }

   return CSize(m_wThumb,m_hThumb);
}


Gdiplus::Bitmap * CLibraryItem::get_thumbnail2()
{

   if(m_pbitmapThumbnail == NULL)
   {

      m_strPath.TrimRight("/");

      m_strPath.TrimRight("\\");

      CString strThumb(m_strPath);

      strThumb += "\\SpotLibraryItem_Thumbnail.jpg";

      CArray < BYTE > data;

      file_as_mem(data,strThumb);

      IStream * ptr = NULL;

      ptr = SHCreateMemStream(data.GetData(),data.GetCount());

      m_pbitmapThumbnail = Gdiplus::Bitmap::FromStream(ptr);

      ptr->Release();

   }

   return m_pbitmapThumbnail;

}


void CLibraryItem::load_bk()
{

   if(m_pdoc == NULL)
   {

      m_pdoc = new CPCDraftDoc();

      m_pdoc->m_bUpdateViews = false;
      m_pdoc->OnNewDocument();

      CString strMd60(m_strPath);

      strMd60 += "\\SpotLibraryItem_MD60.md60";
      m_pdoc->LoadMacDraft60(strMd60, true);

      m_pdoc->SetModifiedFlag(FALSE);

      if(m_pdoc->m_Layers.GetCount() <= 0)
      {
         throw "invalid file";
         return;
      }

   }

}

void CLibraryItem::del()
{

   if(!PathIsDirectoryW(get_uni(m_strPath)))
   {

      return;

   }

   del_dir(m_strPath);

}


void CLibraryItem::save()
{


   if(!PathIsDirectoryW(get_uni(m_strPath)))
   {
      
      throw "invalid path: path should be a existing directory";
      
      return;

   }

   m_strPath.TrimRight("/");

   m_strPath.TrimRight("\\");

   if(m_strName.IsEmpty())
   {

      if(m_pdoc != NULL)
      {

         POSITION pos = m_pdoc->m_Layers[0]->m_ShapeList.GetHeadPosition();

         if(pos != NULL)
         {

            CString stra[5];

            m_pdoc->m_Layers[0]->m_ShapeList.GetNext(pos)->GetValues(
               stra[0],
               stra[1],
               stra[2],
               stra[3],
               stra[4]);

            m_strName = acp_to_utf8(stra[0]);

         }

      }

   }

   try
   {


      CString strText(m_strPath);

      strText += "\\SpotLibraryItem_Name.txt";

      CFILE f(strText,L"w",_SH_DENYNO);

      CStdioFile file(f);

      file.Write(m_strName,m_strName.GetLength());

   }
   catch(...)
   {
   }

   CString strThumb(m_strPath);

   strThumb += "\\SpotLibraryItem_Thumbnail.jpg";

   if(!PathFileExistsW(get_uni(strThumb)))
   {

      CLSID clsid;

      int result = GetEncoderClsid(L"image/jpeg",&clsid);

      if(result < 0)
      {

         printf("The JPEG encoder is not installed.\n");

      }

      CStringW wstr = get_uni(strThumb);

      LPCWSTR pwsz = wstr;

      Gdiplus::EncoderParameters encoderParameters;

      ULONG             quality;

      // Before we call Image::Save, we must initialize an
      // EncoderParameters object. The EncoderParameters object
      // has an array of EncoderParameter objects. In this
      // case, there is only one EncoderParameter object in the array.
      // The one EncoderParameter object has an array of values.
      // In this case, there is only one value (of type ULONG)
      // in the array. We will let this value vary from 0 to 100.

      encoderParameters.Count = 1;
      encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
      encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
      encoderParameters.Parameter[0].NumberOfValues = 1;

      // Save the image as a JPEG with quality level 0.
      quality = 100;
      encoderParameters.Parameter[0].Value = &quality;

      m_pbitmapThumbnail->Save(pwsz,&clsid,&encoderParameters);

      //delete[] pwsz;

   }

   if(m_pdoc == NULL)
   {

      return;

   }

   CString strMd60(m_strPath);

   strMd60 += "\\SpotLibraryItem_MD60.md60";

   if(!PathFileExistsW(get_uni(strMd60)))
   {

      m_pdoc->SaveMacDraft60(strMd60, true);

   }


}


CLibrary::CLibrary()
{

   m_bCacheModified = false;
   
   m_bDelete = false;

   m_bSave = false;

   m_pthreadBk = NULL;

   m_bLibraryOk = false;

}


CLibrary::~CLibrary()
{

   destroy();

}

void CLibrary::defer_delete()
{
   
   if(m_pthreadBk == NULL)
   {

      CSingleLock sl(library_mutex(), TRUE);

      delete this;

   }
   else
   {
      
      m_bDelete = true;

   }

}

void CLibrary::defer_save()
{

   CSingleLock sl(&m_mutex,TRUE);

   if(m_pthreadBk == NULL)
   {

      save();

   }
   else
   {

      m_bSave = true;

   }

}


UINT AFX_CDECL CLibrary::ThreadProcLoadBk(PVOID p)
{

   CLibrary * plibrary = (CLibrary *) p;

   plibrary->library_background_load();

   CSingleLock sl(&plibrary->m_mutex,TRUE);

   if(plibrary->m_bSave)
   {

      plibrary->save();

      plibrary->m_bSave = false;

   }
   
   if(plibrary->m_bDelete)
   {

      CSingleLock sl(library_mutex(), TRUE);

      delete plibrary;

   }
   else
   {

      plibrary->m_pthreadBk = NULL;

   }

   return 0;

}



void CLibrary::destroy()
{

   {

      CSingleLock sl(&m_mutex, TRUE);

      save_cache();

      for (int i = 0; i < m_itema.GetCount(); i++)
      {

         try
         {

            delete m_itema.ElementAt(i);

         }
         catch (...)
         {

         }

      }

      m_itema.RemoveAll();

   }

}


int CLibrary::uuid_index(CString strUuid)
{

   for(int i = 0; i < m_itema.GetCount(); i++)
   {

      try
      {

         if(m_itema.ElementAt(i)->m_strUuid.CompareNoCase(strUuid) == 0)
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

CLibraryItem * CLibrary::uuid_item(CString strUuid)
{

   int iItem = uuid_index(strUuid);

   if(iItem < 0)
   {

      return NULL;

   }

   return m_itema.ElementAt(iItem);

}


void CLibrary::get_uuida(CStringArray & stra, const CArray < int > & ia)
{

   for(int i = 0; i < ia.GetCount(); i++)
   {

      stra.Add(get_item(ia[i])->m_strUuid);

   }

}


void CLibrary::del(const CStringArray & straUuid)
{

   for(int i = 0; i < straUuid.GetCount(); i++)
   {

      int iIndex = uuid_index(straUuid[i]);

      CLibraryItem * pitem = uuid_item(straUuid[i]);

      pitem->del();

      m_itema.RemoveAt(iIndex);

   }

}

void CLibrary::library_add(CLibraryItem * pitem)
{

   pitem->m_strUuid = new_uuid();

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


void CLibrary::load(CString strPath,bool bAsync, double dProgressBegin, double dProgressRange)
{

   m_dProgress = dProgressBegin;

   strPath.TrimRight("/");

   strPath.TrimRight("\\");

   if(!PathIsDirectoryW(get_uni(strPath)))
   {

      return;

   }

   if (m_strPathCache.IsEmpty())
   {

      try
      {

         CString str(strPath);

         str.Replace(":", "_");

         m_strPathCache = GetAppDataPath() + "\\" + str + ".cache";

         load_cache();

      }
      catch (...)
      {

      }

   }

   if(is_spotlibrary_path(strPath))
   {

      load_spotlibrary(strPath,bAsync, dProgressBegin, dProgressRange);

   }
   else
   {

      load_folder(strPath,bAsync, dProgressBegin, dProgressRange);

   }

}


void CLibrary::sort_by_name(int start,int end)
{

   if(start >= end)
   {

      return;

   }

   int piv_pos;
   {
      int i = start + 1;
      
      CStringW piv = get_uni(m_itema[start]->m_strName);
      for(int j =start + 1; j <= end ; j++)
      {
      /*rearrange the array by putting elements which are less than pivot
      on one side and which are greater that on other. */

         if(get_uni(m_itema[j]->m_strName).CollateNoCase(piv) < 0) 
         {
            CLibraryItem * pI = m_itema[i];
            m_itema[i] = m_itema[j];
            m_itema[j] = pI;

         i += 1;
      }
   }
      //put the pivot element in its proper place.
      CLibraryItem * pS = m_itema[start];
      m_itema[start] = m_itema[i-1];
      m_itema[i-1] = pS;

   piv_pos = i - 1;                      //return the position of the pivot
}


   sort_by_name(start,piv_pos - 1);    //sorts the left side of pivot.

   sort_by_name(piv_pos + 1,end) ; //sorts the right side of pivot.

}
void CLibrary::sort_by_name()
{

   OutputDebugString("Going to sort");

   sort_by_name(0,(int)m_itema.GetUpperBound());

}


void CLibrary::load_folder(CString strPath,bool bAsync, double dProgressBegin, double dProgressRange)
{

   strPath.TrimRight("/");

   strPath.TrimRight("\\");

   if(!PathIsDirectoryW(get_uni(strPath)))
   {

      return;

   }

   CStringArray straPath;

   CStringArray straName;

   get_subdir(strPath,straPath,straName);

   for(int i = 0; i < straPath.GetCount(); i++)
   {

      CString strPath = straPath[i];

      load(strPath,bAsync, dProgressBegin + dProgressRange * __double_rate(i, (int)straPath.GetCount()), dProgressRange / __double((int)straPath.GetCount()));

   }

}

void CLibrary::load_cache()
{


   CFILE f(m_strPathCache, L"rb", _SH_DENYNO);

   if (f.m_pf)
   {

      CStdioFile file(f);

      CArchive ar(&file, CArchive::load);

      int iVersion = -1;

      ar >> iVersion;

      if (iVersion == 1)
      {

         int iCount;

         ar >> iCount;

         for (int i = 0; i < iCount; i++)
         {

            CString strUuid;

            ar >> strUuid;

            CLibraryCacheItem & cacheitem = m_mapCache[strUuid];

            ar >> cacheitem.m_strName;
            ar >> cacheitem.m_wThumb;
            ar >> cacheitem.m_hThumb;

         }

      }

   }


}


void CLibrary::save_cache()
{

   CSingleLock sl(&m_mutex, TRUE);

   if (m_bCacheModified)
   {

      if (!m_strPathCache.IsEmpty())
      {

         CString strFolder;

         get_folder_name(strFolder, m_strPathCache);

         if (!PathIsDirectoryW(get_uni(strFolder)))
         {

            SHCreateDirectoryExW(NULL, get_uni(strFolder), NULL);

         }

         CFILE f(m_strPathCache, L"wb", _SH_DENYNO);

         if (f.m_pf)
         {

            CStdioFile file(f);

            CArchive ar(&file, CArchive::store);

            int iVersion = 1;

            ar << iVersion;

            int iCount = m_mapCache.GetCount();

            ar << iCount;

            for (auto ppair = m_mapCache.PGetFirstAssoc(); ppair != nullptr; ppair = m_mapCache.PGetNextAssoc(ppair))
            {

               ar << ppair->key;
               ar << ppair->value.m_strName;
               ar << ppair->value.m_wThumb;
               ar << ppair->value.m_hThumb;

            }

         }

      }

   }

   m_bCacheModified = false;

}


void CLibrary::load_spotlibrary(CString strPath, bool bAsync, double dProgressBegin, double dProgressRange)
{

   strPath.TrimRight("/");

   strPath.TrimRight("\\");

   if(!PathIsDirectoryW(get_uni(strPath)))
   {

      return;

   }

   CString str(strPath);

   str += "\\SpotLibrary_Items";

   if(!PathIsDirectoryW(get_uni(str)))
   {

      return;

   }

   CStringArray straPath;

   CStringArray straName;

   get_subdir(str,straPath,straName);

   for(int i = 0; i < straPath.GetCount(); i++)
   {

      CString strPath = straPath[i];

      CString strUuid = straName[i];

      if(!is_valid_uuid(strUuid))
      {

         continue;

      }

      CLibraryItem * pitem = uuid_item(strUuid);

      bool bDelete = false;

      bool bAdd = false;

      if(pitem == NULL)
      {

         pitem = new CLibraryItem();

         pitem->m_plibrary = this;

         pitem->m_strUuid = strUuid;

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

         pitem->get_thumbnail2();

         m_itema.Add(pitem);

      }

      m_dProgress = dProgressBegin + dProgressRange * __double_rate(i, (int) straPath.GetCount());

   }

   m_strPathLoad = strPath;

   if(bAsync)
   {

      //m_pthreadBk = AfxBeginThread(ThreadProcLoadBk,this);

   }
   else
   {

      library_background_load();

   }

}


void CLibrary::library_background_load()
{

   return;

   for(int i = 0; i < m_itema.GetCount(); )
   {

      CLibraryItem * pitem = m_itema.ElementAt(i);

      try
      {

         if(!pitem->m_bNew)
         {

            pitem->load_bk();

         }

         i++;

      }
      catch(...)
      {

         try
         {

            delete pitem;

         }
         catch(...)
         {

         }

         m_itema.RemoveAt(i);

      }

   }

}


void CLibrary::save()
{

   if(!is_spotlibrary_path(m_strPath2))
   {

      return;

   }

   if(!PathIsDirectoryW(get_uni(m_strPath2)))
   {
      
      if(!CreateDirectoryW(get_uni(m_strPath2),NULL))
      {

         throw "unable to create library folder";

         return;

      }

   }

   for(int i = 0; i < m_itema.GetCount(); i++)
   {

      CLibraryItem * pobj = m_itema.ElementAt(i);

      if(!is_valid_uuid(pobj->m_strUuid))
      {

         pobj->m_strUuid = new_uuid();

      }

   }

   m_strPath2.TrimRight("/");

   m_strPath2.TrimRight("\\");

   CString str(m_strPath2);

   str += "\\SpotLibrary_Items";

   if(!PathIsDirectoryW(get_uni(str)))
   {

      CreateDirectoryW(get_uni(str),NULL);

   }

   CString strItem;

   for(int i = 0; i < m_itema.GetCount(); i++)
   {

      CLibraryItem * pobj = m_itema.ElementAt(i);

      strItem = str + "\\" + pobj->m_strUuid;

      if(!PathIsDirectoryW(get_uni(strItem)))
      {

         CreateDirectoryW(get_uni(strItem),NULL);

      }

      pobj->m_strPath = strItem;

      pobj->save();

   }

}



CLibraryItem * CLibrary::get_item(int iItem)
{

   if(!m_strFilter.IsEmpty())
   {

      return m_itema.ElementAt(m_ia[iItem]);

   }

   return m_itema.ElementAt(iItem);

}
int CLibrary::get_count()
{
   if(!m_strFilter.IsEmpty())
   {

      return m_ia.GetCount();

   }

   return m_itema.GetCount();

}

bool contains_no_case(CString str,CString strFind)
{

   str.MakeLower();

   strFind.MakeLower();

   return str.Find(strFind) >= 0;

}


void CLibrary::update_filter(CString strFilter)
{

   m_ia.RemoveAll();

   if(!strFilter.IsEmpty())
   {

      //if(!m_strFilter.IsEmpty() && iSel >= 0 && iSel < m_ia.GetCount())
      //{

      //   iSel = m_ia[iSel];

      //}
      //else
      //{

      //   iSel = -1;

      //}

      bool bFoundSel = false;

      for(int i = 0; i < m_itema.GetCount(); i++)
      {

         if(contains_no_case(m_itema.ElementAt(i)->m_strName,m_strFilter))
         {
            m_ia.Add(i);

            //if(i == iSel)
            //{

            //   iSel = m_ia.GetUpperBound();

            //   bFoundSel = true;

            //}
         }


      }

      //if(!bFoundSel)
      //{

      //   iSel = -1;

      //}

   }

   m_strFilter = strFilter;


}

