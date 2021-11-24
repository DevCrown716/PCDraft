#pragma once


namespace text_format_rtf
{


   class image_manager 
   {
   public:


      virtual void put(const unsigned char * ch, unsigned long size) = 0;

   };



} // namespace text_format_rtf