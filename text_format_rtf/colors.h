#pragma once

typedef unsigned char uchar;


namespace text_format_rtf
{



   class colors 
   {
   public:
      
  
      virtual ~colors() { }
      
      virtual void register_color(uchar r, uchar g, uchar b) = 0;

   };


} // namespace text_format_rtf
