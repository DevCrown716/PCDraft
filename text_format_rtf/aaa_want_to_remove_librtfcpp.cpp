//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//

/** Description :
 *   Implementation of C++ interface classes for librtf.
 *
 *  Autors :
 *      Tomek : Tomasz Hławiczka : tomek /at/ tru /./ pl
 *
 *  Changes :
 *	28 Sep 2006, Tomek: Implementation
 *
 */
#include "framework.h"
#include "_text_format_rtf.h"
//#include <ctype.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string>
//#include <iostream>
//#include <errno.h>
//#include <fstream>
//#include <list>


namespace text_format_rtf
{


//
//#define impl0(C,R,FUNC) R cpp_##FUNC ( c_def_##C * f ) { \
//	return ( reinterpret_cast<C *> ( f->object )->FUNC() ); \
//}
//#define impl1(C,R,FUNC,PA) R cpp_##FUNC ( c_def_##C * f, PA a ) { \
//	return ( reinterpret_cast<C *> ( f->object )->FUNC(a) ); \
//}
//#define impl2(C,R,FUNC,PA,PB) R cpp_##FUNC ( c_def_##C * f, PA a, PB b) { \
//	return ( reinterpret_cast<C *> ( f->object )->FUNC(a,b) ); \
//}
//#define impl3(C,R,FUNC,PA,PB,PC) R cpp_##FUNC ( c_def_##C * f, PA a, PB b, PC c ) { \
//	return reinterpret_cast<C *> ( f->object )->FUNC(a, b, c); \
//}
//#define impl4(C,R,FUNC,PA,PB,PC,PD) R cpp_##FUNC ( c_def_##C * f, PA a, PB b, PC c, PD d ) { \
//	return reinterpret_cast<C *> ( f->object )->FUNC(a, b, c, d); \
//}
//#define use(func) cobject->func = & cpp_##func;
//
//#define duse(func) std::cerr << #func "\t= " << (void*) cobject->func << "\n";

   //typedef unsigned char uchar;
   //typedef processor	c_def_processor;
   //typedef source	c_def_source;
   //typedef observer	c_def_observer;
   //typedef image_manager	c_def_image_mgr;

   //impl2(processor, void, attr_push, int, int);
   //impl0(processor, void, attrstack_push);
   //impl0(processor, void, attrstack_drop);
   //impl1(processor, int, attr_pop, int);
   //impl0(processor, void, attr_drop_all);
   //impl0(processor, void, attr_pop_all);
   //impl0(processor, void, attr_pop_dump);
   //// impl2(processor, void,  attr_push_style	, m_attributestack::style::types, int );
   //impl0(processor, void, begin);
   //impl0(processor, void, end);
   //impl1(processor, void, title, const char *);
   //impl1(processor, void, keywords, const char *);
   //impl1(processor, void, author, const char *);
   //// impl2(processor, void,  summary            , enum document_summary, int );
   //impl1(processor, char *, translate_char, int);
   //impl1(processor, void, print, const char *);
   //impl1(processor, void, print_symbol, const char *);
   //impl0(processor, void, print_forced_space);
   //impl1(processor, int, print_unicode, int);
   //impl1(processor, void, hyperlink, const char *);
   //impl1(processor, void, hyperlink_base, const char *);
   ////impl1(processor, void,  print_char			, enum special_char );
   //impl0(processor, void, font_smaller_begin);
   //impl0(processor, void, font_smaller_end);
   //impl0(processor, void, table_begin);
   //impl0(processor, void, table_end);
   //impl0(processor, void, table_cell_begin);
   //impl0(processor, void, table_cell_end);
   //impl0(processor, void, table_row_begin);
   //impl0(processor, void, table_row_end);
   //impl2(processor, void, attr_remove, int *, int);
   //impl2(processor, void, register_font, int, const char *);
   //impl3(processor, void, register_color, uchar, uchar, uchar);
   //// impl1(processor, void,  register_style		, const ::TextStyle * );
   //impl1(processor, void, charset, int);
   //impl1(processor, void, codepage, int);

   //impl2(image_manager, void, put, const uchar *, unsigned long);

   //impl1(observer, void, debug, const char *);
   //impl2(observer, void, warning, int, const char *);
   //impl2(observer, void, error, int, const char *);

   //impl0(source, void, open);
   //impl0(source, void, close);
   //impl2(source, size_t, read, char *, size_t);


   //image_manager * processor::image_begin(const image_descriptor * desc)
   //{

   //   image_descriptor img;

   //   img.width = desc->width;
   //   img.height = desc->height;
   //   img.depth = desc->bits_per_pixel;
   //   img.type = static_cast<image_descriptor::types> (desc->type);
   //   img.wmetafile_type = desc->wmetafile_type;

   //   image_manager * mgr = reinterpret_cast<processor *> (proc->object)->image_begin(img);
   //   if (mgr == NULL)
   //      return NULL;

   //   ((bridge *)mgr)->internal_init();

   //   return mgr->c< image_manager >();
   //}

   //void cpp_image_end(processor * proc, image_manager * mgr)
   //{
   //   // TODO: mgr cannot be NULL !
   //   image_manager * cpp_mgr = reinterpret_cast<image_manager *> (mgr->object);
   //   // TODO: cpp_mgr cannot be NULL !
   //   reinterpret_cast<processor *> (proc->object)->image_end(*cpp_mgr);
   //}

   //void cpp_register_style(processor * proc, const TextStyle * style)
   //{
   //   m_attributestack::style cpp_style;
   //   cpp_style.bold = style->bold;
   //   cpp_style.id = style->id;
   //   cpp_style.italic = style->italic;
   //   if (style->stylename)
   //      cpp_style.name = style->stylename;
   //   cpp_style.rsid = style->rsid;
   //   cpp_style.type = static_cast<m_attributestack::style::types> (style->type);
   //   cpp_style.font.size = style->fontsize;
   //   cpp_style.font.face = style->fontface;


   //   reinterpret_cast<processor *> (proc->object)->register_style(cpp_style);
   //}

   //void cpp_attr_push_style(processor * proc, TextStyleType type, int style_id)
   //{
   //   reinterpret_cast<processor *> (proc->object)->attr_push_style(
   //      static_cast <m_attributestack::style::types> (type), style_id
   //   );
   //}

   //void cpp_summary(processor * proc, document_summary category, int param)
   //{
   //   reinterpret_cast<processor *> (proc->object)->summary(
   //      static_cast<processor::summary_categories> (category), param
   //   );
   //}

   //void cpp_print_char(processor * proc, special_char ch)
   //{
   //   reinterpret_cast<processor *> (proc->object)->print_char(
   //      static_cast<processor::special_characters> (ch));
   //}

   
   //observer::observer()
   //{

   //}


   //void observer::internal_init()
   //{
   //   register_object < observer >(this);

   //   observer * cobject = c < observer >();

   //   use(debug);
   //   use(error);
   //   use(warning);
   //}

   //source::source()
   //   : bridge()
   //{
   //}

   //void source::internal_init()
   //{
   //   register_object < source >(this);

   //   source * cobject = c < source >();

   //   use(read);
   //   use(open);
   //   use(close);
   //}

   //image_manager::image_manager()
   //{
   //}

   //void image_manager::internal_init()
   //{
   //   register_object < ::image_manager >(this);

   //   ::image_manager * cobject = c < ::image_manager >();

   //   use(put);
   //}




} // namespace text_format_rtf



