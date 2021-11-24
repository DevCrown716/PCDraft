#include "framework.h"
#include "_text_format_rtf.h"



namespace text_format_rtf
{



   std_source::std_source(::std::istream & reader)
      : m_reader(reader), buf(NULL)
   {

   }


   size_t std_source::read(char * buf, size_t chars)
   {

       m_reader.read(buf, chars);

       return m_reader.gcount();

   }


   void std_source::open()
   {

   }


   void std_source::close()
   {

   }


   /*parsers::rtf::rtf(source & src, processor & proc, observer & log)
   {
      exec(src, proc, log);
   }

   void parsers::rtf::exec(source & src, processor & proc, observer & log)
   {
      ((bridge &)src).internal_init();
      ((bridge &)proc).internal_init();
      ((bridge &)log).internal_init();

      ::rtf_process(src.c< ::source >(), proc.c< processor >(), log.c < observer >());
   }*/

   //processor::processor()
   //{
   //}

   //processor::~processor()
   //{
   //}

   //void processor::internal_init()
   //{
   //   register_object < processor >(this);
   //   processor * cobject = c < processor >();

   //   use(attr_push);
   //   use(attrstack_push);
   //   use(attrstack_drop);
   //   use(attr_pop);
   //   use(attr_drop_all);
   //   use(attr_pop_all);
   //   use(attr_pop_dump);
   //   use(attr_remove);
   //   use(attr_push_style);

   //   use(register_font);
   //   use(register_color);
   //   use(register_style);

   //   use(begin);
   //   use(end);
   //   use(title);
   //   use(keywords);
   //   use(author);
   //   use(summary);
   //   use(translate_char);
   //   use(print);
   //   use(print_symbol);
   //   use(print_forced_space);
   //   use(print_unicode);
   //   use(hyperlink);
   //   use(hyperlink_base);
   //   use(print_char);
   //   use(font_smaller_begin);
   //   use(font_smaller_end);
   //   use(table_begin);
   //   use(table_end);
   //   use(table_cell_begin);
   //   use(table_cell_end);
   //   use(table_row_begin);
   //   use(table_row_end);
   //   use(image_begin);
   //   use(image_end);

   //   use(charset);
   //   use(codepage);

   //}



} // namespace text_format_rtf



