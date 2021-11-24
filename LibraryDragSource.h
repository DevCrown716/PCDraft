// Library - PCDraft 6 - Infinisys Ltd.
#pragma once

#include <afxole.h>


#include "MDShape.h"


class CLibraryItem;
class CLayer;


class CLibraryDragSource : 
   public COleDropSource
{
public:
   

   bool              m_bDrag;
   CLibraryItem *    m_pitem;
   CBitmap *         m_pbitmap;
   HICON             m_hcursor;
   

   CLibraryDragSource();
   virtual ~CLibraryDragSource();

   virtual DROPEFFECT StartDragging(CWnd * pwnd,CLibraryItem * pitem,CPoint* MousePos);
   
   virtual void CompleteMove();

   virtual void OnMouseMove(POINT pt);

   virtual SCODE GiveFeedback(DROPEFFECT dropEffect);


};




