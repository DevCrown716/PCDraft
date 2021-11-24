//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//
#include "framework.h"
#include "_text_format_rtf.h"


namespace text_format_rtf
{

   void colors_stack::register_color(uchar r, uchar g, uchar b)
   {

      m_colora.push_back(color(r, g, b));

   }

   void colors_stack::add_color(uchar r, uchar g, uchar b)
   {

      if (colorid(r, g, b) < 0)
      {

         register_color(r, g, b);

      }

   }

   int colors_stack::colorid(uchar r, uchar g, uchar b)
   {

      auto colorFind = color(r, g, b);

      for (int i = 0; i < m_colora.size(); i++)
      {

         auto & color = m_colora[i];

         if (color == colorFind)
         {

            return i;

         }

      }

      return -1;

   }


   ::std::string colors_stack::as_rtf()
   {

      ::std::string str;

      char color_table_entry[256];
      // Create new RTF document color table
      for(auto & color : m_colora)
      {
         sprintf(color_table_entry, "\\red%d\\green%d\\blue%d;", color.r, color.g, color.b);
         str += color_table_entry;

      }

      return str;

   }

} // namespace text_format_rtf



