#pragma once


namespace text_format_rtf
{


   class fonts :
      virtual public processor_base
   {
   public:
      
      
      virtual ~fonts() { }
      virtual void register_font(int num, const char * fontname, int charset) = 0;
      virtual const ::std::string & fontname(int id) = 0;
      virtual int fontcharset(int id) = 0;


   };


} // namespace text_format_rtf



