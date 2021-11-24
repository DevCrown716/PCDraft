#pragma once


namespace text_format_rtf
{


   class output 
   {

   public:

      virtual void write(const void *, int len) = 0;
      virtual void append(const char * psz);

      inline void rtf_begin() { append("{\\rtf1"); }

      inline void rtf_end() { append("}"); }



   };



} // namespace text_format_rtf




