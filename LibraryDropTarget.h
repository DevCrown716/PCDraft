// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include <afxole.h>


class drop_target
{
public:

   virtual DROPEFFECT OnDragEnter(CWnd * pWnd,COleDataObject * pDataObject,DWORD dwKeyState,CPoint point);
   virtual DROPEFFECT OnDragOver(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point);
};


class CLibraryDropTarget: public COleDropTarget
{
public:


   drop_target *        m_ptarget;
   CWnd *               m_pwndDrop;
   bool                 m_bDisableDrop;
   CStringArray         m_straFiles;
   CArray < int >       m_iaAcceptFormat;

   CLibraryDropTarget();
   virtual ~CLibraryDropTarget();


   BOOL Initialize(CWnd* wnd);

   virtual DROPEFFECT OnDragEnter(CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point);
   virtual DROPEFFECT OnDragOver(CWnd * pWnd, COleDataObject* pDataObject,DWORD dwKeyState,CPoint point);
   virtual void OnDragLeave(CWnd * pWnd);

   virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

};
