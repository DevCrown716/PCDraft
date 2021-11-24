#include "framework.h"
#include "_text_format_rtf.h"



namespace text_format_rtf
{


   std_output::std_output(::std::ostream & output ) :
      m_output(output)
   {


   }


   void std_output::write(const void * p, int len)
   {

      int iStringLen = strnlen_s((const char *) p, len);

      if (iStringLen < len)
      {


         len = iStringLen;

      }

      m_output.write((const char *) p, len);


   }



} // namespace text_format_rtf



