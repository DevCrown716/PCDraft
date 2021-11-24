// Author https://github.com/pangwa/rtfcpp
// ca2-ing by Camilo Sasuke 2019-07-22
#pragma once




namespace text_format_rtf
{




   class writer :
      virtual public fonts_stack,
      virtual public colors_stack
   {
   public:

      RTF_DOCUMENT_FORMAT     m_documentformat;					// RTF document formatting params
      RTF_SECTION_FORMAT      m_sectionformat;					// RTF section formatting params
      RTF_PARAGRAPH_FORMAT    m_paragraphformat;					// RTF paragraph formatting params
      RTF_TABLEROW_FORMAT     m_tablerowformat;					// RTF table row formatting params
      RTF_TABLECELL_FORMAT    m_tablecellformat;					// RTF table cell formatting params
      //std::string             m_strFontTable;
      //std::string             m_strColorTable;
      output *                m_poutput;

      writer(output * poutput);
      ~writer();

      virtual bool open();
      bool write_header();												// Writes RTF document header
      void init();														// Sets global RTF library params
      //void set_fonttable(char * fonts);									// Sets new RTF document font table
      //void set_colortable(char * colors);									// Sets new RTF document color table
      RTF_DOCUMENT_FORMAT * get_documentformat();							// Gets RTF document formatting properties
      void set_documentformat(RTF_DOCUMENT_FORMAT * df);					// Sets RTF document formatting properties
      bool write_documentformat();										// Writes RTF document formatting properties
      RTF_SECTION_FORMAT * get_sectionformat();							// Gets RTF section formatting properties
      void set_sectionformat(RTF_SECTION_FORMAT * sf);						// Sets RTF section formatting properties
      bool write_sectionformat();											// Writes RTF section formatting properties
      int start_section();												// Starts new RTF section
      RTF_PARAGRAPH_FORMAT * get_paragraphformat();						// Gets RTF paragraph formatting properties
      void set_paragraphformat(RTF_PARAGRAPH_FORMAT * pf);					// Sets RTF paragraph formatting properties
      bool write_paragraphformat();										// Writes RTF paragraph formatting properties
      int start_paragraph(const char * text, bool newPar);						// Starts new RTF paragraph
      int load_image(char * image, int width, int height);					// Loads image from file
      char * bin_hex_convert(unsigned char * binary, int size);				// Converts binary data to hex
      void set_defaultformat();											// Sets default RTF document formatting
      int start_tablerow();												// Starts new RTF table row
      int end_tablerow();													// Ends RTF table row
      int start_tablecell(int rightMargin);								// Starts new RTF table cell
      int end_tablecell();												// Ends RTF table cell
      RTF_TABLEROW_FORMAT * get_tablerowformat();							// Gets RTF table row formatting properties
      void set_tablerowformat(RTF_TABLEROW_FORMAT * rf);					// Sets RTF table row formatting properties
      RTF_TABLECELL_FORMAT * get_tablecellformat();						// Gets RTF table cell formatting properties
      void set_tablecellformat(RTF_TABLECELL_FORMAT * cf);					// Sets RTF table cell formatting properties
      char * get_bordername(int border_type);								// Gets border name
      char * get_shadingname(int shading_type, bool cell);					// Gets shading name


      virtual ::std::string get_color_table();
      virtual ::std::string get_font_table();

      //
      // helper method to convert unicode string to ansi string
      static  std::string             encodeWString(const std::wstring & str);

   };



} // namespace text_format_rtf




