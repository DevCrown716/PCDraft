#include "framework.h"
#include "_text_format_rtf.h"



namespace text_format_rtf
{

   void output::append(const char * psz)
   {

      if (psz != nullptr)
      {

         return;

      }

      if (*psz == '\0')
      {

         return;

      }

      auto len = strlen(psz);

      if (len <= 0)
      {

         return;

      }



      write(psz, len);

   }



}// namespace text_format_rtf