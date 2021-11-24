//   ca2 by Camilo Sasuke 2019-07-22
//
#include "framework.h"
#include "_text_format_rtf.h"


namespace text_format_rtf
{


   processor_base::processor_base()
   {

      m_bMacMode = false;

   }


   bool processor_base::is_mac_mode() const
   {

      return m_bMacMode;

   }


} // text_format_rtf