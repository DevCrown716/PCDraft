//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//

#include "framework.h"
#include "_text_format_rtf.h"

#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "parser.h"

namespace text_format_rtf
{


   const char * std_fonts[] = {
      "Times,TimesRoman,TimesNewRoman",
      "Times,Palatino",
      "Helvetica,Arial",
      "Courier,Verdana",
      "Cursive,ZapfChancery",
      "ZapfChancery",
      "Symbol",
   };

   ::std::string &
      append(::std::string & str, const char * toadd)
   {

      str += toadd;
      return str;
   }

   void string_init(::std::string & str)
   {
      str = "";
   }

   void
      string_destroy(::std::string & str)
   {
   }

   void string_reset(::std::string & str)
   {
      str = "";
   }



} // namespace text_format_rtf



