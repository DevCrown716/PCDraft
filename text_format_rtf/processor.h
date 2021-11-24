//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//
#pragma once


namespace text_format_rtf
{



   class processor :
      virtual public fonts_stack,
      virtual public colors_stack,
      virtual public attributes_stack
   {
   public:

      // add attribute to last block
      //virtual void attr_push(e_attr eattr, int param) ;

      // remove last attribute from last last block if it is equal to attr
      //virtual int attr_pop(e_attr eattr);

      // add new block
      //virtual void attrstack_push();

      // remove last block and rollback all attributes
      //virtual void attrstack_drop();

      // clear last block without rollingback attributes
      //virtual void attr_drop_all();

      // rollback all attributes from last block and removes all attributes from last block
      //virtual void attr_pop_all();

      // rollback all attributes from last block without removing them
      //virtual void attr_pop_dump();

      // remove all possible attribues from last block 
      //virtual void attr_remove(e_attr * peattr, int size);

      // push attributes from registred style
      //virtual void attr_push_style(e_element eelement, int id);

      virtual void begin();
      virtual void end();

      virtual void title(const char *);
      virtual void keywords(const char *);
      virtual void author(const char *);
      virtual void summary(e_document_summary type, int param);

      virtual void hyperlink_base(const char * url);

      virtual void font_smaller_begin();
      virtual void font_smaller_end();
      virtual char * translate_char(int);

      virtual void print(::text_format_rtf::impl::processor * pimpl, const char *);
      virtual void print_symbol(const char *);
      virtual void print_forced_space();
      virtual int  print_unicode(int);
      virtual void hyperlink(const char *);
      virtual void print_char(e_char);

      virtual void table_begin();
      virtual void table_end();
      virtual void table_cell_begin();
      virtual void table_cell_end();
      virtual void table_row_begin();
      virtual void table_row_end();

      virtual image_manager * image_begin(const image_descriptor & descriptor);
      virtual void image_end(image_manager *);

      virtual void charset(int charset_id);
      virtual void codepage(int cp_id);

      //virtual void register_font(int id, const char * font_name);
      //virtual void register_color(unsigned char r, unsigned char g, unsigned char b);
      //virtual void register_style(const style * style);


   };


} // namespace text_format_rtf



