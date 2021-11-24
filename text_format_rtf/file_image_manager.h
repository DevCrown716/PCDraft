#pragma once


namespace text_format_rtf
{

class file_image_manager : public image_manager
{
public:
   explicit file_image_manager(const ::std::string & f)
   {
      output->open_for_writing(f);
   }
   virtual ~file_image_manager()
   {
      output->close();
   }

   virtual void put(const unsigned char * ch, unsigned long size)
   {
      output->write((const char *)ch, size);
   }

   ::file_sp output;
};







} // namespace text_format_rtf


