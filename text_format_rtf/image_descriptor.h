#pragma once


namespace text_format_rtf
{

   class image_descriptor 
   {
   public:

      int                  width;
      int                  height;
      int                  bits_per_pixel;	// =1;
      e_pict               m_epict;
      int                  m_iWmfMappingMode;
      ::std::string               m_strWMFMappingMode;
      image_manager *      m_pmanager;

   };


} // namespace text_format_rtf



