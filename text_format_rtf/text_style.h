#pragma once


namespace text_format_rtf
{

   class style 
   {
   public:


      int            id;
      e_element      m_eelement;
      ::std::string         m_strName;
      int            rsid;

      int            bold;
      int            italic;
      int            fontsize;
      int            fontface;


      inline bool operator == (const style & another) const
      {
         return m_eelement == another.m_eelement && id == another.id;
      }
      inline bool operator < (const style & another) const
      {
         return m_eelement < another.m_eelement || (m_eelement == another.m_eelement && id < another.id);
      }
      inline bool operator > (const style & another) const
      {
         return m_eelement > another.m_eelement || (m_eelement == another.m_eelement && id > another.id);
      }

   };



} // namespace text_format_rtf



