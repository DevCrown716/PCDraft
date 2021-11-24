#pragma once



namespace text_format_rtf
{


   class source
   {
   public:

      virtual size_t read(char * buf, size_t size) = 0;
      virtual void open() = 0;
      virtual void close() = 0;

   };


} // namespace text_format_rtf




