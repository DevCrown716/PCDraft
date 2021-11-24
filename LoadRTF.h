#pragma once

class CMDText;

#include "text_format_rtf/_text_format_rtf.h"
#include "StyledText.h"



class md_text_rtf_processor :
   public ::text_format_rtf::processor
{
public:

   CMDText *         m_ptext;
   CStyledTextStyle  m_style;
   int               m_iCodePage;


   md_text_rtf_processor();
   virtual ~md_text_rtf_processor();


   virtual void enable_attr(int attr,int param,bool enable);

   virtual void begin();
   virtual void end();
   virtual void title(::std::string title)				;
   virtual void keywords(::std::string keywords)		;
   virtual void author(::std::string author)			;
   virtual void summary(::text_format_rtf::e_document_summary cat,int param);


   virtual void print(::text_format_rtf::impl::processor * pimpl,const char *);
   virtual void print(::std::string)				;
   virtual void print_symbol(::std::string)	;
   virtual void print_forced_space()	;
   virtual int  print_unicode(int)		;
   virtual void hyperlink(::std::string)		;
   virtual void hyperlink_base(::std::string)	;

   virtual void print_char(::text_format_rtf::e_char ch)	;
   virtual void font_smaller_begin()		;
   virtual void font_smaller_end()			;
   virtual void table_begin()					;
   virtual void table_end()					;
   virtual void table_cell_begin()			;
   virtual void table_cell_end()				;
   virtual void table_row_begin()			;
   virtual void table_row_end()				;

   virtual ::text_format_rtf::image_manager * image_begin(const ::text_format_rtf::image_descriptor & image)	;
   virtual void image_end(text_format_rtf::image_manager &)				;


   virtual void codepage(int cp_id);


};


