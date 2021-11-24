///////////////////////////////////////////////
//
// Infinisys Ltd
//
#pragma once

class CVersionDoc;

enum e_packed_type
{

   packet_type_none,
   packed_type_dib,
   packed_type_gradient,

};

class CGradientPack;

class CPackedItem
{
public:

   bool                    m_bOwn;
   e_packed_type           m_etype;
   
   union
   {

      CPackedDIB *      m_pdib;
      CGradientPack *   m_pgradient;

   };

   CPackedItem();
   ~CPackedItem();

   HBITMAP CreateBitmap(int iOpacity);

   int get_index(CVersionDoc * pdoc);
   void set_index(CVersionDoc * pdoc,int iIndex);
   void destroy();

   Gdiplus::Brush * proper_brush(LPCRECT lpcrect, int iOpacity);

   CGradientPack * detach_gradient();



};


CArchive & operator << (CArchive & ar, const CPackedItem & item);
CArchive & operator >> (CArchive & ar, CPackedItem & item);