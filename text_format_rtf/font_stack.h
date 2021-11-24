#pragma once


namespace text_format_rtf
{


   class fonts_stack : virtual protected fonts
   {
   public:

      typedef ::std::map <int, ::std::string > fonts_container;

      fonts_container   fonts;


      virtual void register_font(int id, const char * fontname);

      virtual const ::std::string & fontname(int id);


   };





} // namespace text_format_rtf


