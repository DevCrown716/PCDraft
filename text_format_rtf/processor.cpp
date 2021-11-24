//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//

#include "framework.h"
#include "_text_format_rtf.h"


namespace text_format_rtf
{


   // add attribute to last block
   //void processor::attr_push(e_attr eattr, int param)
   //{

   //}


   // remove last attribute from last last block if it is equal to attr
   //int processor::attr_pop(e_attr eattr)
   //{
   //   return 0;
   //}

   // add new block
   //void processor::attrstack_push()
   //{

   //}

   // remove last block and rollback all attributes
   //void processor::attrstack_drop()
   //{

   //}

   // clear last block without rollingback attributes
   //void processor::attr_drop_all()
   //{

   //}

   // rollback all attributes from last block and removes all attributes from last block
   //void processor::attr_pop_all()
   //{

   //}

   // rollback all attributes from last block without removing them
   //void processor::attr_pop_dump()
   //{

   //}

   // remove all possible attribues from last block 
   //void processor::attr_remove(int * tab, int size)
   //{

   //}

   // push attributes from registred style
   //void processor::attr_push_style(enum TextStyleType type, int id)
   //{

   //}


   void processor::charset(int charset_id)
   {
   }

   void processor::codepage(int cp_id)
   {
   }


   char * processor::translate_char(int ch)
   {
      //	short start;
      char * result = NULL;

      /* if we are seeking a character from a symbol font we can
         be below 0x80
      */
      /*
      if(ntable == FONTSYMBOL_TABLE)
      {
         start = op->symbol_first_char;

         if(ch >= start && ch <= op->symbol_last_char)
            result = op->symbol_translation_table[ch - start];
      }
      else
      if (ch >= 0x20 && ch < 0x80) {
         result = op->ascii_translation_table [ch - 0x20];
      }
      else
      if (charset != CHARSET_ANSI &&
          charset != CHARSET_MAC &&
          charset != CHARSET_CP437 &&
          charset != CHARSET_CP850)
         error_handler ("invalid character set value, cannot translate character");
      else
      switch (charset)
      {
      case CHARSET_ANSI:
         if (codepage != NULL && op->unisymbol_print != NULL && codepage->cp) {
            if (codepage->chars[ch - 0x80]) {
               result = op->unisymbol_print(codepage->chars[ch - 0x80]);
            }
         } else {
         start = op->ansi_first_char;
            if (ch >= start &&
                ch <= op->ansi_last_char)
               result = op->ansi_translation_table [ch-start];
         }
         break;
      case CHARSET_MAC:
         start = op->mac_first_char;
         if (ch >= start &&
             ch <= op->mac_last_char)
            result = op->mac_translation_table [ch-start];
         break;
      case CHARSET_CP437:
         start = op->cp437_first_char;
         if (ch >= start &&
             ch <= op->cp437_last_char)
            result = op->cp437_translation_table [ch-start];
         break;
      case CHARSET_CP850:
         start = op->cp850_first_char;
         if (ch >= start &&
             ch <= op->cp850_last_char)
            result = op->cp850_translation_table [ch-start];
         break;
      }
      */
      return result;
   }



   void processor::begin()
   {

   }
   void processor::end()
   {

   }

   void processor::title(const char *)
   {

   }
   void processor::keywords(const char *)
   {

   }
   void processor::author(const char *)
   {

   }
   void processor::summary(e_document_summary type, int iParam)
   {

   }

   void processor::hyperlink_base(const char * url)
   {

   }

   void processor::font_smaller_begin()
   {

   }
   void processor::font_smaller_end()
   {

   }

   //char * processor::translate_char(int)
   //{
   //   
   //   return nullptr;

   //}


   void processor::print(::text_format_rtf::impl::processor * pprocessor, const char * psz)
   {

   }


   void processor::print_symbol(const char *)
   {

   }
   void processor::print_forced_space()
   {

   }
   
   
   int processor::print_unicode(int)
   {

      return 0;

   }


   void processor::hyperlink(const char *)
   {

   }


   void processor::print_char(e_char echar)
   {

   }


   void processor::table_begin()
   {

   }


   void processor::table_end()
   {

   }


   void processor::table_cell_begin()
   {

   }


   void processor::table_cell_end()
   {

   }


   void processor::table_row_begin()
   {

   }


   void processor::table_row_end()
   {

   }

   
   image_manager * processor::image_begin(const image_descriptor & descriptor)
   {

       return nullptr;

   }


   void processor::image_end(image_manager *)
   {

   }

   //void processor::charset(int charset_id)
   //{

   //}
   //void processor::codepage(int cp_id)
   //{

   //}

   //void processor::register_font(int id, const char * font_name)
   //{

   //}
   //void processor::register_color(unsigned char r, unsigned char g, unsigned char b)
   //{

   //}
   //void processor::register_style(const style * style)
   //{

   //}



} // namespace text_format_rtf




