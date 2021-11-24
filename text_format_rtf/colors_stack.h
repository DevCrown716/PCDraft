#pragma once


namespace text_format_rtf
{


   class colors_stack : virtual protected colors
   {
   public:

      struct color {
         color(uchar r = 0, uchar g = 0, uchar b = 0) : r(r), g(g), b(b) { }
         uchar r, g, b;

         bool operator == (const color & c) const { return r == c.r && g == c.g && b == c.b; }
      };

      
      typedef ::std::vector< color > color_array;
      color_array   m_colora;

      virtual void register_color(uchar, uchar, uchar);
      virtual void add_color(uchar, uchar, uchar);

      int colorid(uchar, uchar, uchar);

      ::std::string as_rtf();

   };


} // namespace text_format_rtf



