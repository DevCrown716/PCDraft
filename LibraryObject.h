// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "MDShape.h"
#include "PackedItem.h"

class CLibraryPhoto;
class CLibrary;
class CLibraryItem:
   public CObject
{
public:


   CString                    m_strName;
   CString                    m_strUuid;
   CRect                      m_rect;
   CRect                      m_rectDraw;
   CRect                      m_rectLabel;
   Gdiplus::Bitmap *          m_pbitmapThumbnail;
   CString                    m_strPath;
   bool                       m_bNew;
   CPCDraftDoc *              m_pdoc;
   CLibrary *                 m_plibrary;
   CLibraryPhoto *            m_pphoto;
   int                        m_wThumb;
   int                        m_hThumb;

   
   CLibraryItem();
   CLibraryItem(CLibraryPhoto * pphoto);
   CLibraryItem(Gdiplus::Bitmap & bitmapSource, CPCDraftDoc * pdoc);
   CLibraryItem(CLibraryItem * pobj);
   virtual ~CLibraryItem();


   void load();
   void load_bk();
   void save();
   void del();

   Gdiplus::Bitmap * get_thumbnail2();
   CSize get_thumbnail_size();

};


class CLibraryCacheItem
{
public:

   CString                    m_strName;
   int                        m_wThumb;
   int                        m_hThumb;

   CLibraryCacheItem()
   {

      m_wThumb = -1;
      m_hThumb = -1;

   }


};

class CLibrary :
   public CObject
{
public:

   double                     m_dProgress;
   CArray < CLibraryItem * >  m_itema;
   CString                    m_strFilter;
   CArray < int >             m_ia;
   CMutex                     m_mutex;
   CWinThread *               m_pthreadBk;
   bool                       m_bDelete;
   bool                       m_bSave;
   CString                    m_strPath2;
   CString                    m_strPathLoad;
   CString                    m_strPathCache;
   bool                       m_bLibraryOk;
   bool                       m_bCacheModified;
   CMap< CString, LPCTSTR, CLibraryCacheItem, const CLibraryCacheItem & >    m_mapCache;

   
   CLibrary();
   virtual ~CLibrary();


   static UINT AFX_CDECL ThreadProcLoadBk(PVOID p);

   void library_background_load();


   void library_add(CLibraryItem * pitem);
   void get_uuida(CStringArray & stra, const CArray < int > & ia);
   void del(const CStringArray & strUuid);
   void load(CString strPath, bool bAsync, double dProgressBegin = 0.0, double dProgressRange = 1.0);
   void load_spotlibrary(CString strPath,bool bAsync, double dProgressBegin =0.0, double dProgressRange = 1.0);
   void load_folder(CString strPath,bool bAsync, double dProgressBegin = 0.0, double dProgressRange = 1.0);
   void save();

   void load_cache();
   void save_cache();

   void sort_by_name();
   void sort_by_name(int left, int right);

   void defer_save();
   void defer_delete();
   void destroy();

   int uuid_index(CString strUuid);
   int get_count();
   void update_filter(CString str);

   CLibraryItem * uuid_item(CString strUuid);
   CLibraryItem * get_item(int iItem);

};