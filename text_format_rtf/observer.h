#pragma once


namespace text_format_rtf
{

   
   class observer 
   {
   public:


      virtual void debug(const char *) = 0;
      virtual void warning(int id, const char *) = 0;
      virtual void error(int id, const char *) = 0;

   };


} // namespace text_format_rtf



