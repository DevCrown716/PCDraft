#include "framework.h"
#include "_text_format_rtf.h"


namespace text_format_rtf
{


   void rtf_part::append(const char * toadd)
   {
      content += toadd;
   }

   void rtf_part::append(char onechar)
   {

      char buf[2] = { onechar, '\0' };

      append(buf);

   }


} // namespace text_format_rtf


