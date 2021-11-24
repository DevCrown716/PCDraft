#pragma once


namespace text_format_rtf
{


   class std_source :
      public source
   {
   public:


      ::std::istream & m_reader;
      char * buf;


      std_source(::std::istream & reader);


      virtual size_t read(char * buf, size_t chars);
      virtual void open();
      virtual void close();


   };


} // namespace text_format_rtf



