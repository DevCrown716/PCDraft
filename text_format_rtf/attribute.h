#pragma once


namespace text_format_rtf
{


   class attribute
   {
   public:


      e_attr      m_eattr;
      int         m_iParam;

      attribute(e_attr eattr = attr_none, int iParam = 0) :
         m_eattr(eattr),
         m_iParam(iParam)
      {

      }

   };


} // namespace text_format_rtf



