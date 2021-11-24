// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "MDShape.h"
#include "PackedItem.h"


class CLibraryPhoto:
   public CObject
{
public:


   CString                    m_strName;
   CRect                      m_rect;
   CRect                      m_rectDraw;
   Gdiplus::Bitmap *          m_pbitmap;
   Gdiplus::Bitmap *          m_pbitmapThumbnail;
   CString                    m_strPath;


   CLibraryPhoto();
   CLibraryPhoto(Gdiplus::Bitmap & bitmapSource);
   CLibraryPhoto(CLibraryPhoto * pobj);
   virtual ~CLibraryPhoto();

   
   void load_thumbnail();
   void load();
   //void save();

};


class CPhotoLibrary: public CObject
{
public:

   CArray < CLibraryPhoto * >    m_itema;
   CString                       m_strFilter;
   CMutex                        m_mutex;
   CString                       m_strPath;
   CArray < int >                m_ia;

   CPhotoLibrary();
   virtual ~CPhotoLibrary();

   static UINT AFX_CDECL ThreadProcLoadBk(PVOID p);

   virtual void library_add(CLibraryPhoto * pitem);

   virtual void load();
   virtual int unfiltered_name_index(CString strName);

   virtual void destroy();
   virtual int get_count();
   virtual void update_filter(CString strFilter, int & iSel);


   CLibraryPhoto * get_item(int iItem);
   CLibraryPhoto * unfiltered_name_item(CString strName);


};