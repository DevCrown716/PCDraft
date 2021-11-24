//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//

#include "framework.h"
#include "_text_format_rtf.h"

#include <stdio.h>
#include <string.h>
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

   string &
      append(string & str, const char * toadd)
   {

      str += toadd;
      return str;
   }

   void
      string_init(string & str)
   {
      str.Empty();
   }

   void
      string_destroy(string & str)
   {
   }

   void
      string_reset(string & str)
   {
      str.Empty();
   }



} // namespace text_format_rtf



