//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//
#include "framework.h"
#include "_text_format_rtf.h"
#include <sstream>

std::string trim_right(const char * psz,const char * pszTrim);
std::string ReplaceString(std::string subject, const std::string & search,
   const std::string & replace) {
   size_t pos = 0;
   while ((pos = subject.find(search, pos)) != std::string::npos) {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
   }
   return subject;
}
namespace text_format_rtf
{


   void fonts_stack::register_font(int num, const char * fontname, int charset)
   {
      FONT f;
      f.m_strFamily = trim_right(fontname,";");
      f.m_iCharset = charset;
      m_mapFont[num] = f;
   }

   void fonts_stack::add_font(const char * fontname, int charset)
   {
      if (fontid(fontname) < 0)
      {
         int num = m_mapFont.size();
         m_mapFont[num].m_strFamily = fontname;
         m_mapFont[num].m_iCharset = charset;
      }
   }

   const ::std::string & fonts_stack::fontname(int id)
   {

      if (id < 0)
      {

         id = 0;

      }

      return m_mapFont[id].m_strFamily;

   }

   int fonts_stack::fontcharset(int id)
   {

      if(id < 0)
      {

         id = 0;

      }

      return m_mapFont[id].m_iCharset;

   }

   int fonts_stack::fontid(const char * fontname)
   {

      for(auto & pair : m_mapFont)
      {

         if (pair.second.m_strFamily == fontname)
         {

            return pair.first;

         }

      }

      return -1;

   }


   ::std::string fonts_stack::as_rtf()
   {

      ::std::string str;

      // Create new RTF document font table
      for (auto & pair : m_mapFont)
      {
         ::std::ostringstream strEntry;

         if (!is_mac_mode())
         {
            strEntry << "{";
         }

         int iCodePage = 1252;

#define SHIFTJIS_CHARSET 128

         if (pair.second.m_iCharset == SHIFTJIS_CHARSET)
         {

            iCodePage = 932;

         }

         strEntry << "\\f" << pair.first << "\\fnil\\fcharset"<< pair.second.m_iCharset << "\\cpg65001 ";

         if (is_mac_mode())
         {

            std::string str;

            str = ReplaceString(pair.second.m_strFamily, " ", "");

            if (str.compare("TimesNewRoman") == 0)
            {

               str += "PS";

            }

            str += "MT";

            strEntry  << str << ";";
             
          }
         else
         {

            ::std::string strText;
            
            bool bMultiByteChar = false;
            const char * p = pair.second.m_strFamily.c_str();
            while (*p)
            {
               if (*p & 0x80)
               {
                  bMultiByteChar = true;
                  break;

               }
               p++;
            }
            if (bMultiByteChar)
            {
               const char * p = pair.second.m_strFamily.c_str();
               while (*p)
               {
                  strText += "\\'";
                  char sz[32];
                  int i = *(unsigned char *)p;
                  sprintf(sz, "%02x", i);
                  strText += sz;
                  p++;
               }

            }
            else
            {

               strText = pair.second.m_strFamily;

            }


            strEntry << strText;

            strEntry << "}";

         }

         str += strEntry.str();

      }

      return str;

   }


} // namespace text_format_rtf



