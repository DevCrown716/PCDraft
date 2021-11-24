//   ca2 by Camilo Sasuke 2019-07-22
//
#pragma once


namespace text_format_rtf
{



   class processor_base
   {
   public:


      bool     m_bMacMode;


      processor_base();


      virtual bool is_mac_mode() const;


   };


}