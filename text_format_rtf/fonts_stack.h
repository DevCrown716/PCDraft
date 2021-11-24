#pragma once


namespace text_format_rtf
{

   class FONT
   {
   public:

      ::std::string     m_strFamily;
      int m_iCharset;
   };

   class fonts_stack : virtual protected fonts
   {
   public:



      typedef ::std::map <int, FONT > font_map;

      font_map   m_mapFont;


      virtual void register_font(int id, const char * fontname, int charset);
      virtual void add_font(const char * fontname, int charset);

      virtual const ::std::string & fontname(int id);
      virtual int fontcharset(int id);

      virtual int fontid(const char * fontname);

      ::std::string as_rtf();


   };





} // namespace text_format_rtf


