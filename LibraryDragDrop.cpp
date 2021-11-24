// Library - PCDraft 6 - Infinisys Ltd.
#include "StdAfx.h"
#include "LibraryDragDrop.h"
#include "LibraryObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLibraryDragDrop::CLibraryDragDrop()
{
}

CLibraryDragDrop::~CLibraryDragDrop()
{
}


DROPEFFECT CLibraryDragDrop::StartDragging(CLibraryItem * pitem,CPoint* MousePos)
{

   CWnd * pwnd = m_pwndDrop;

   DROPEFFECT effect = DROPEFFECT_NONE;

   m_bDisableDrop = true;

   try
   {

      CRect rect(pitem->m_rectDraw);

      effect= CLibraryDragSource::StartDragging(pwnd,pitem,MousePos);

   }
   catch(...)
   {

      effect = DROPEFFECT_NONE;

   }

   m_bDisableDrop = false;

   return effect;

}
