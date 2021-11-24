// Infinisys Ltd. - Gradient


#include "StdAfx.h"
#include "GradientPack.h"
#include "PackedDIB.h"
#include "PackedItem.h"
#include "PCDraftDoc.h"


CArchive & operator << (CArchive & ar,CGradientPack & item);


CPackedItem::CPackedItem()
{
   
   m_etype = packet_type_none;
   
   m_bOwn = false;
   
   m_pdib = NULL;

}

CPackedItem::~CPackedItem()
{
   
   destroy();

}

HBITMAP CPackedItem::CreateBitmap(int iOpacity)
{

   if(m_etype == packed_type_dib)
   {

      return m_pdib->CreateBitmap(iOpacity);

   }
   else if(m_etype == packed_type_gradient)
   {

      return m_pgradient->CreateBitmap(iOpacity);

   }

   return NULL;

}

Gdiplus::Brush * CPackedItem::proper_brush(LPCRECT lpcrect, int iOpacity)
{

   if (m_etype == packed_type_gradient)
   {

      return m_pgradient->proper_brush(lpcrect, iOpacity);

   }
   else if (m_etype == packed_type_dib)
   {

      return m_pdib->proper_brush(lpcrect, iOpacity);

   }
   else
   {

      ASSERT(FALSE);

      return nullptr;

   }

}


int CPackedItem::get_index(CVersionDoc * pdoc)
{

   if(m_etype == packed_type_dib)
   {
      
      return m_pdib - pdoc->m_Patterns;

   }
   else if(m_etype == packed_type_gradient)
   {

      return m_pgradient - pdoc->m_Gradients;

   }

   return -1;

}

void CPackedItem::set_index(CVersionDoc * pdoc,int iIndex)
{

   if(m_etype == packed_type_dib)
   {

      m_pdib = &pdoc->m_Patterns[iIndex];

   }
   else if(m_etype == packed_type_gradient)
   {

      m_pgradient = &pdoc->m_Gradients[iIndex];

   }

}


CArchive & operator << (CArchive & ar,const CPackedItem & item)
{
   
   int iType = item.m_etype;

   ar << iType;

   if(item.m_etype == packed_type_dib)
   {

      ar << *item.m_pdib;

   }
   else
   {

      ar << (CGradientPack &) *item.m_pgradient;

   }

   return ar;

}

CArchive & operator >> (CArchive & ar,CPackedItem & item)
{

   item.destroy();

   int iType;

   ar >> iType;

   item.m_etype = (e_packed_type) iType;

   if(item.m_etype == packed_type_dib)
   {

      item.m_pdib = new CPackedDIB();

      ar >> *item.m_pdib;

   }
   else
   {

      item.m_pgradient = new CGradientPack();

      ar >> *item.m_pgradient;

   }

   item.m_bOwn = true;

   return ar;

}


void CPackedItem::destroy()
{

   if(m_bOwn)
   {

      if(m_etype == packed_type_dib)
      {
         
         if(m_pdib != NULL)
         {

            delete m_pdib;

         }

      }
      else if(m_etype == packed_type_gradient)
      {

         if(m_pgradient != NULL)
         {

            delete m_pgradient;

         }

      }

   }

   m_pdib = NULL;
   m_etype = packet_type_none;
   m_bOwn = false;

}