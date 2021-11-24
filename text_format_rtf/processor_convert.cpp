//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//

#include "framework.h"
#include "_text_format_rtf.h"


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>




bool is_lower_char(char ch)
{
   return ch >= 'a' && ch <= 'z';
}
bool is_upper_char(char ch)
{
   return ch >= 'A' && ch <= 'Z';
}
#include "parser.h"

namespace text_format_rtf
{




   void print_with_special_exprs(internal_processor *, const char * s);
   static void process_block(internal_processor * f, rtf_part *);
   void starting_body(internal_processor * filter);

   /*========================================================================
   * Name:	starting_body
   * Purpose:	Switches output stream for writing document contents.
   * Args:	None.
   * Returns:	None.
   *=======================================================================*/


   static int
      compare(rtf_part * part, const char * command)
   {
      return !strcmp(part->content.c_str(), command);
   }

   static int
      get_param(rtf_part * part)
   {
      if (part->has_param == false)
      {
         // ?
      }
      return part->param;
   }

   namespace impl
   {
      /*-------------------------------------------------------------------*/
      /*-------------------------------------------------------------------*/
      /*-------------------------------------------------------------------*/


      /*========================================================================
      * Name:	process_font_table
      * Purpose:	Processes the font table of an RTF file.
      * Args:	Tree of words.
      * Returns:	None.
      *=======================================================================*/

      void processor::process_font_table()
      {

         ::std::string fontname;
         int charset;
         int fontblock_level = 0;
         int fontid = 0;
         rtf_part * part = NULL;

         //	CHECK_PARAM_NOT_NULL(part);
         //	CHECK_PARAM_NOT_NULL(filter);

         part = m_psource->get_next_part();
         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
            {
               fontblock_level++;
               charset = 0;
               fontid = 0;
               fontname = "";
            }
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               if(!fontname.empty())
               {

                  m_pprocessor->register_font(fontid,fontname.c_str(),charset);

                  fontname = "";

               }

               if(fontblock_level == 0)
               {
                  m_psource->push_back_current_part();
                  break;
               }
               fontblock_level--;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {

               if(!fontname.empty())
               {

                  m_pprocessor->register_font(fontid,fontname.c_str(),charset);

                  fontname = "";

               }

               //if(fontblock_level == 1)
               {
                  if(compare(part,"f"))
                     fontid = get_param(part);
                  else if(compare(part,"charset"))
                     charset = get_param(part);
                  else if(compare(part,"fcharset"))
                     charset = get_param(part);
               }
            }
            else if(part->type == PART_TYPE_TEXT)
            {
               //if(fontblock_level == 1)
               {
                  fontname += part->content;
                  //m_pprocessor->register_font(fontid,fontname.c_str(),charset);
                  //fontname = "";

               }
               //if (!fontname.empty())
               //{

               //   auto iFind = fontname.find(';');

               //   if(iFind != ::std::string::npos)
               //   {

               //      fontname = fontname.substr(0,iFind);

               //      if(fontblock_level == 0)
               //      {
               //         // MACOS (only?)

               //         if(fontname.substr(fontname.length() - 2,2) == "MT")
               //         {

               //            fontname = fontname.substr(0,fontname.length() - 2);

               //         }

               //         if(fontname.substr(fontname.length() - 2,2) == "PS")
               //         {

               //            fontname = fontname.substr(0,fontname.length() - 2);

               //         }

               //         bool bUpper = true;

               //         for(int i = 0; i < fontname.length(); i++)
               //         {

               //            if(i > 0)
               //            {
               //               if(is_upper_char(fontname[i]) && !bUpper)
               //               {
               //                  fontname = fontname.substr(0,i) + " " + fontname.substr(i);
               //                  i++;
               //               }
               //            }

               //            bUpper = is_upper_char(fontname[i]);

               //         }

               //      }

               //      m_pprocessor->register_font(fontid,fontname.c_str(), charset);

               //      fontname = "";
               //   }

               //}
            }
            part = m_psource->get_next_part();
         }
      }

      /*========================================================================
      * Name:	process_index_entry
      * Purpose:	Processes an index entry of an RTF file.
      * Args:	Tree of words.
      * Returns:	None.
      *=======================================================================*/

      void processor::process_index_entry()
      {
         //	CHECK_PARAM_NOT_NULL(w);
         m_psource->skip_block_to_end();
      }


      /*========================================================================
      * Name:	process_toc_entry
      * Purpose:	Processes an index entry of an RTF file.
      * Args:	Tree of words, flag to say whether to include a page#.
      * Returns:	None.
      *=======================================================================*/

      void processor::process_toc_entry(int include_page_num)
      {

         //	CHECK_PARAM_NOT_NULL(w);

         m_psource->skip_block_to_end();
      }


      /*========================================================================
      * Name:	process_info_group
      * Purpose:	Processes the \info group of an RTF file.
      * Args:	Tree of words.
      * Returns:	None.
      *=======================================================================*/

      ::std::string & processor::read_to_end(::std::string & result)
      {
         rtf_part * part = NULL;
         int blocks = 0;

         result = "";

         part = m_psource->get_next_part();

         while(part)
         {
            if(part->type == PART_TYPE_TEXT)
               result += part->content;
            else if(part->type == PART_TYPE_BLOCK_START)
               blocks++;
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               if(blocks == 0)
               {
                  m_psource->push_back_current_part();
                  break;
               }
               blocks--;
            }
            part = m_psource->get_next_part();
         }

         if(part == NULL)
         {
            // unexpected end of source.
         }

         return result;
      }



      void processor::process_info_group()
      {
         int block_level = 0;
         ::std::string tmp_str;

         rtf_part * part = m_psource->get_next_part();
         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
               block_level++;
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               if(block_level == 0)
               {
                  m_psource->push_back_current_part();
                  break;
               }
               block_level--;
            }
            else if(part->type == PART_TYPE_COMMAND && block_level == 1)
            {
               if(compare(part,"title"))
                  m_pprocessor->title(read_to_end(tmp_str).c_str());
               else if(compare(part,"keywords"))
                  m_pprocessor->keywords(read_to_end(tmp_str).c_str());
               else if(compare(part,"author"))
                  m_pprocessor->author(read_to_end(tmp_str).c_str());
               else if(compare(part,"comments"))
               {
               }
               else if(compare(part,"nofpages"))
                  m_pprocessor->summary(document_summary_number_of_pages,get_param(part));
               else if(compare(part,"nofwords"))
                  m_pprocessor->summary(document_summary_number_of_words,get_param(part));
               else if(compare(part,"nofchars"))
                  m_pprocessor->summary(document_summary_number_of_chars,get_param(part));
               else if(compare(part,"creatim"))	// creation date
               {
               }
               else if(compare(part,"printim"))	// last print date
               {
               }
               else if(compare(part,"buptim")) // last backup date
               {
               }
               else if(compare(part,"revtime"))	// revision date
               {
               }
               else if(compare(part,"hlinkbase"))
                  m_pprocessor->hyperlink_base(read_to_end(tmp_str).c_str());
            }

            part = m_psource->get_next_part();
         }
      }


      int processor::cmd_generator(rtf_part * part)
      {
         ::std::string tmp;

         read_to_end(tmp);

         // info(LIBRTF_INFO_GENERATOR, tmp );

         return false;
      }

      /*========================================================================
      * Name:	process_color_table
      * Purpose:	Processes the color table of an RTF file.
      * Args:	Tree of words.
      * Returns:	None.
      *=======================================================================*/

      void processor::process_color_table()
      {
         int r,g,b;
         rtf_part * part;

         //	CHECK_PARAM_NOT_NULL(w);

         /* Sometimes, RTF color tables begin with a semicolon,
         * i.e. an empty color entry. This seems to indicate that color 0
         * will not be used, so here I set it to black.
         */
         r = g = b = 0;

         part = m_psource->get_next_part();

         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
               m_psource->skip_block_to_outside();
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               m_psource->push_back_current_part();
               break;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {
               if(compare(part,"red"))
                  r = get_param(part);
               else if(compare(part,"green"))
                  g = get_param(part);
               else if(compare(part,"blue"))
                  b = get_param(part);
            }
            else if(part->type == PART_TYPE_TEXT)
            {
               if(compare(part,";"))
               {
                  while(r > 255) r >>= 8;
                  while(g > 255) g >>= 8;
                  while(b > 255) b >>= 8;
                  m_pprocessor->register_color(r,g,b);
                  r = g = b = 0;
               }
            }

            part = m_psource->get_next_part();
         }
      }

      void processor::process_expanded_color_table()
      {
         ///int r, g, b;
         rtf_part * part;

         //	CHECK_PARAM_NOT_NULL(w);

         /* Sometimes, RTF color tables begin with a semicolon,
         * i.e. an empty color entry. This seems to indicate that color 0
         * will not be used, so here I set it to black.
         */
         //r = g = b = 0;

         part = m_psource->get_next_part();

         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
               m_psource->skip_block_to_outside();
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               m_psource->push_back_current_part();
               break;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {
               //if (compare(part, "red"))
               //   r = get_param(part);
               //else if (compare(part, "green"))
               //   g = get_param(part);
               //else if (compare(part, "blue"))
               //   b = get_param(part);
            }
            else if(part->type == PART_TYPE_TEXT)
            {
               //if (compare(part, ";"))
               //{
               //   while (r > 255) r >>= 8;
               //   while (g > 255) g >>= 8;
               //   while (b > 255) b >>= 8;
               //   m_pprocessor->register_color(r, g, b);
               //   r = g = b = 0;
               //}
            }

            part = m_psource->get_next_part();
         }

      }

      /*========================================================================
      * Name:	cmd_cf
      * Purpose:	Executes the \cf command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_cf(rtf_part * w)
      {

         m_pprocessor->attr_push(attr_foreground,get_param(w));
         return false;
      }

      /*========================================================================
      * Name:	cmd_cs
      * Purpose:	Executes the \cs command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_cs(rtf_part * w)
      {
         if(w->has_param)
            m_pprocessor->attr_push_style(element_section,get_param(w));
         else
         {
            // \cs without param should be only in \stylesheet block
         }
         return false;
      }


      /*========================================================================
      * Name:	cmd_cb
      * Purpose:	Executes the \cb command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_cb(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_background,get_param(w));
         return false;
      }


      /*========================================================================
      * Name:	cmd_fs
      * Purpose:	Executes the \fs command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_fs(rtf_part * w)
      {

         /* Note, fs20 means 10pt */
         m_pprocessor->attr_push(attr_fontsize,get_param(w));

         return false;
      }


      /*========================================================================
      * Name:	cmd_field
      * Purpose:	Interprets fields looking for hyperlinks.
      * Comment:	Because hyperlinks are put in \field groups,
      *		we must interpret all \field groups, which is
      *		slow and laborious.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_field(rtf_part * w)
      {

         // 	CHECK_PARAM_NOT_NULL(w);

         /*	Specification :
         {\field <fieldmod>? <fieldinst> <fieldrslt> }
         {\* \fldinst <para>+ <fldalt>? }
         {\fldrslt <para>+ }
         <fieldmod> : \flddirty \fldedit \fldlock \fldpriv
         */

         rtf_part * part = m_psource->get_next_part();

         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_END)
            {
               m_psource->push_back_current_part();
               break;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {
               if(compare(part,"flddirty"))
               {
                  // TODO: A formatting change has been made to the field result since the field was last updated.
               }
               else if(compare(part,"fldedit"))
               {
                  // TODO: Text has been added to, or removed from, the field result since the field was last updated.
               }
               else if(compare(part,"fldlock"))
               {
                  // TODO: Field is locked and cannot be updated.
               }
               else if(compare(part,"fldpriv"))
               {
                  // TODO: Result is not in a form suitable for display (for example, binary data used by fields whose result is a picture).
               }
               else
               {
                  // Unknown commmand.
               }
            }
            else if(part->type == PART_TYPE_BLOCK_START)
            {
               part = m_psource->get_next_command();

               if(part)
               {
                  if(compare(part,"fldrslt"))
                  {
                     process_block(part);
                  }
                  else if(compare(part,"*"))
                  {
                     part = m_psource->get_next_command();
                     if(part)
                     {
                        if(compare(part,"fldinst"))
                        {
                           part = m_psource->get_next_part();
                           while(part)
                           {
                              if(part->type == PART_TYPE_BLOCK_START)
                                 m_psource->skip_block_to_outside();
                              else if(part->type == PART_TYPE_BLOCK_END)
                                 break;
                              else if(part->type == PART_TYPE_TEXT)
                              {
                                 if(compare(part,"SYMBOL"))
                                 {
                                    // TODO:
                                 }
                                 else if(compare(part,"SHAPE"))
                                 {
                                    // TODO:
                                 }
                                 else if(compare(part,"HYPERLINK"))
                                 {
                                    // FIXME:
                                    m_pprocessor->hyperlink("FIXME: URL");
                                 }
                              }
                              part = m_psource->get_next_part();
                           }
                        }
                        else
                           m_psource->skip_block_to_outside();
                     }
                  }
                  else
                     m_psource->skip_block_to_outside();
               }
            }
            part = m_psource->get_next_part();
         }
         return true;
      }



      /*========================================================================
      * Name:	cmd_f
      * Purpose:	Executes the \f command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_f(rtf_part * w)
      {

         m_pprocessor->attr_push(attr_fontface,get_param(w));
         return false;
      }


      /*========================================================================
      * Name:	cmd_highlight
      * Purpose:	Executes the \cf command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_highlight(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_background,get_param(w));
         return false;
      }



      /*========================================================================
      * Name:	cmd_tab
      * Purpose:	Executes the \tab command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_tab(rtf_part * w)
      {
         /* Tab presents a genuine problem
         * since some output formats don't have
         * an equivalent. As a kludge fix, I shall
         * assume the font is fixed width and that
         * the tabstops are 8 characters apart.
         */
         m_pprocessor->print_char(char_tab);
         /* FIXME
         int need= 8-(total_chars_this_line%8);
         total_chars_this_line += need;
         while(need>0) {
         filter->print_forced_space(filter);
         need--;
         }
         printf("\n");
         */
         return false;
      }


      /*========================================================================
      * Name:	cmd_plain
      * Purpose:	Executes the \plain command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_plain(rtf_part * w) {
         m_pprocessor->attr_pop_all();
         return false;
      }


      /*========================================================================
      * Name:	cmd_fnil
      * Purpose:	Executes the \fnil command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_fnil(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_nil);
         return false;
      }



      /*========================================================================
      * Name:	cmd_froman
      * Purpose:	Executes the \froman command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::
         cmd_froman(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_roman);
         return false;
      }


      /*========================================================================
      * Name:	cmd_fswiss
      * Purpose:	Executes the \fswiss command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::
         cmd_fswiss(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_swiss);
         return false;
      }


      /*========================================================================
      * Name:	cmd_fmodern
      * Purpose:	Executes the \fmodern command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::
         cmd_fmodern(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_modern);
         return false;
      }


      /*========================================================================
      * Name:	cmd_fscript
      * Purpose:	Executes the \fscript command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_fscript(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_script);
         return false;
      }

      /*========================================================================
      * Name:	cmd_fdecor
      * Purpose:	Executes the \fdecor command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int
         processor::cmd_fdecor(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_decor);
         return false;
      }

      /*========================================================================
      * Name:	cmd_ftech
      * Purpose:	Executes the \ftech command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int
         processor::cmd_ftech(rtf_part * w) {
         m_pprocessor->attr_push(attr_fontface,font_tech);
         return false;
      }

      /*========================================================================
      * Name:	cmd_expand
      * Purpose:	Executes the \expand command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int
         processor::cmd_expand(rtf_part * w) {
         if(w->has_param) {
            int param = get_param(w);
            if(!param)
               m_pprocessor->attr_pop(attr_expand);
            else
               m_pprocessor->attr_push(attr_expand,param);
         }
         return false;
      }

      int
         processor::cmd_expandedcolortbl(rtf_part * w) {

         process_expanded_color_table();
         return true;

      }


      /*========================================================================
      * Name:	cmd_emboss
      * Purpose:	Executes the \embo command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_emboss(rtf_part * w) {
         if(w->has_param && !get_param(w))
            m_pprocessor->attr_pop(attr_emboss);
         else
            m_pprocessor->attr_push(attr_emboss,get_param(w));
         return false;
      }


      /*========================================================================
      * Name:	cmd_engrave
      * Purpose:	Executes the \impr command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_engrave(rtf_part * w) {
         if(w->has_param && !get_param(w))
            m_pprocessor->attr_pop(attr_engrave);
         else
            m_pprocessor->attr_push(attr_engrave,get_param(w));
         return false;
      }

      /*========================================================================
      * Name:	cmd_caps
      * Purpose:	Executes the \caps command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_caps(rtf_part * w) {
         if(w->has_param && !get_param(w))
            m_pprocessor->attr_pop(attr_caps);
         else
            m_pprocessor->attr_push(attr_caps,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_scaps
      * Purpose:	Executes the \scaps command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_scaps(rtf_part * w) {
         if(w->has_param && !get_param(w))
            m_pprocessor->attr_pop(attr_smallcaps);
         else
            m_pprocessor->attr_push(attr_smallcaps,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_bullet
      * Purpose:	Executes the \bullet command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_bullet(rtf_part * w)
      {

         m_pprocessor->print_char(char_bullet);

         //	if (op->chars.bullet) {
         //		printf(op->chars.bullet);
         //		++total_chars_this_line; /* \tab */
         //	}
         return false;
      }

      /*========================================================================
      * Name:	cmd_ldblquote
      * Purpose:	Executes the \ldblquote command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_ldblquote(rtf_part * w) {

         m_pprocessor->print_char(char_dbl_quote);

         //	if (op->chars.left_dbl_quote) {
         //		printf(op->chars.left_dbl_quote);
         //		++total_chars_this_line; /* \tab */
         //	}
         return false;
      }


      /*========================================================================
      * Name:	cmd_rdblquote
      * Purpose:	Executes the \rdblquote command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_rdblquote(rtf_part * w) {

         m_pprocessor->print_char(char_right_dbl_quote);
         return false;
      }


      /*========================================================================
      * Name:	cmd_lquote
      * Purpose:	Executes the \lquote command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_lquote(rtf_part * w) {

         m_pprocessor->print_char(char_left_dbl_quote);
         return false;
      }


      /*========================================================================
      * Name:	cmd_nonbreaking_space
      * Purpose:	Executes the nonbreaking space command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_nonbreaking_space(rtf_part * w) {

         m_pprocessor->print_char(char_nonbreaking_space);
         return false;
      }


      /*========================================================================
      * Name:	cmd_nonbreaking_hyphen
      * Purpose:	Executes the nonbreaking hyphen command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_nonbreaking_hyphen(rtf_part * w) {

         m_pprocessor->print_char(char_nonbreaking_hyphen);
         return false;
      }


      /*========================================================================
      * Name:	cmd_optional_hyphen
      * Purpose:	Executes the optional hyphen command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_optional_hyphen(rtf_part * w) {

         m_pprocessor->print_char(char_optional_hyphen);
         return false;
      }


      /*========================================================================
      * Name:	cmd_emdash
      * Purpose:	Executes the \emdash command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_emdash(rtf_part * w) {

         m_pprocessor->print_char(char_emdash);
         return false;
      }


      /*========================================================================
      * Name:	cmd_endash
      * Purpose:	Executes the \endash command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_endash(rtf_part * w) {

         m_pprocessor->print_char(char_endash);
         return false;
      }


      /*========================================================================
      * Name:	cmd_rquote
      * Purpose:	Executes the \rquote command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_rquote(rtf_part * w) {

         m_pprocessor->print_char(char_right_quote);
         return false;
      }


      /*========================================================================
      * Name:	cmd_par
      * Purpose:	Executes the \par command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_par(rtf_part * w) {

         m_pprocessor->print_char(char_line_break);
         return false;
      }


      int processor::cmd_pntxta(rtf_part * w) {

         m_psource->skip_block_to_end();
         return false;
      }


      int
         processor::cmd_pntxtb(rtf_part * w) {

         m_psource->skip_block_to_end();
         return false;
      }

      /*========================================================================
      * Name:	cmd_line
      * Purpose:	Executes the \line command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_line(rtf_part * w) {

         m_pprocessor->print_char(char_line_break);
         return false;
      }


      /*========================================================================
      * Name:	cmd_page
      * Purpose:	Executes the \page command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_page(rtf_part * w) {

         m_pprocessor->print_char(char_page_break);
         return false;
      }


      /*========================================================================
      * Name:	cmd_intbl
      * Purpose:	Executes the \intbl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_intbl(rtf_part * w) {
         //	++coming_pars_that_are_tabular;
         return false;
      }


      /*========================================================================
      * Name:	cmd_ulnone
      * Purpose:	Executes the \ulnone command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ulnone(rtf_part * w)
      {

         static e_attr underlines[] ={attr_underline,
            attr_dot_ul,attr_dash_ul,attr_dot_dash_ul,attr_2dot_dash_ul,
            attr_word_ul,attr_wave_ul,attr_thick_ul,attr_double_ul};

         m_pprocessor->attr_remove(underlines,sizeof(underlines) / sizeof(underlines[0]));

         return false;
      }

      /*========================================================================
      * Name:	cmd_ul
      * Purpose:	Executes the \ul command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ul(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0) {
            cmd_ulnone(w);
         }
         else {
            m_pprocessor->attr_push(attr_underline,0);
         }
         return false;
      }

      /*========================================================================
      * Name:	cmd_uld
      * Purpose:	Executes the \uld command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_uld(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_double_ul,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_uldb
      * Purpose:	Executes the \uldb command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_uldb(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_dot_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_uldash
      * Purpose:	Executes the \uldash command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_uldash(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_dash_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_uldashd
      * Purpose:	Executes the \cmd_uldashd command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_uldashd(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_dot_dash_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_uldashdd
      * Purpose:	Executes the \uldashdd command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_uldashdd(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_2dot_dash_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_ulw
      * Purpose:	Executes the \ulw command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ulw(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_word_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_ulth
      * Purpose:	Executes the \ulth command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ulth(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_thick_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_ulthd
      * Purpose:	Executes the \ulthd command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ulthd(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_thick_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_ulthdash
      * Purpose:	Executes the \ulthdash command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ulthdash(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_thick_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_ulwave
      * Purpose:	Executes the \ulwave command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ulwave(rtf_part * w)
      {
         m_pprocessor->attr_push(attr_wave_ul,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_strike
      * Purpose:	Executes the \strike command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_strike(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_strike);
         else
            m_pprocessor->attr_push(attr_strike,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_strikedl
      * Purpose:	Executes the \strikedl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_strikedl(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_dbl_strike);
         else
            m_pprocessor->attr_push(attr_dbl_strike,0);
         return false;
      }


      /*========================================================================
      * Name:	cmd_striked
      * Purpose:	Executes the \striked command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_striked(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_dbl_strike);
         else
            m_pprocessor->attr_push(attr_dbl_strike,0);
         return false;
      }


      int processor::cmd_pgdsctbl(rtf_part * w)
      {
         m_psource->skip_block_to_end();
         return false;
      }

      /*========================================================================
      * Name:	cmd_rtf
      * Purpose:	Executes the \rtf command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_rtf(rtf_part * w)
      {
         return false;
      }


      /*========================================================================
      * Name:	cmd_up
      * Purpose:	Executes the \up command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_up(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_super);
         else
            m_pprocessor->attr_push(attr_super,0);
         return false;
      }

#if 1 /* daved - 0.19.4 */
      /*========================================================================
      * Name:	cmd_u
      * Purpose:	Processes a Unicode character
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, always false
      *=======================================================================*/

      int processor::cmd_u(rtf_part * w)
      {

         if(m_pprocessor->print_unicode(get_param(w)))
            return(SKIP_ONE_WORD);
         return false;
      }
#endif

      /*========================================================================
      * Name:	cmd_dn
      * Purpose:	Executes the \dn command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_dn(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_sub);
         else
            m_pprocessor->attr_push(attr_sub,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_nosupersub
      * Purpose:	Executes the \nosupersub command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_nosupersub(rtf_part * w)
      {
         m_pprocessor->attr_pop(attr_super);
         m_pprocessor->attr_pop(attr_sub);
         return false;
      }

      /*========================================================================
      * Name:	cmd_super
      * Purpose:	Executes the \super command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_super(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_super);
         else
            m_pprocessor->attr_push(attr_super,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_sub
      * Purpose:	Executes the \sub command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_sub(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_sub);
         else
            m_pprocessor->attr_push(attr_sub,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_shad
      * Purpose:	Executes the \shad command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_shad(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_shadow);
         else
            m_pprocessor->attr_push(attr_shadow,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_b
      * Purpose:	Executes the \b command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_b(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0) {
            m_pprocessor->attr_pop(attr_bold);
         }
         else
            m_pprocessor->attr_push(attr_bold,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_i
      * Purpose:	Executes the \i command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_i(rtf_part * w)
      {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_italic);
         else
            m_pprocessor->attr_push(attr_italic,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_s
      * Purpose:	Executes the \s command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/
      int processor::cmd_s(rtf_part * w) {
         if(w->has_param)
            m_pprocessor->attr_push_style(element_paragraph,get_param(w));
         else
         {
            // \s without param should be only in \stylesheet block
         }

         return false;
      }

      /*========================================================================
      * Name:	cmd_sect
      * Purpose:	Executes the \sect command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_sect(rtf_part * w) {
         /* XX kludge */
         m_pprocessor->print_char(char_paragraph_begin);
         //	if (op->paragraph_begin) {
         //		printf(op->paragraph_begin);
         //	}
         return false;
      }

      /*========================================================================
      * Name:	cmd_shp
      * Purpose:	Executes the \shp command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_shp(rtf_part * w) {

         // FIXME:
         //	if (op->comment_begin) {
         //		printf(op->comment_begin);
         //		printf("Drawn Shape (ignored--not implemented yet)");
         //		printf(op->comment_end);	/* daved 0.20.2 */
         //	}

         return false;
      }

      /*========================================================================
      * Name:	cmd_outl
      * Purpose:	Executes the \outl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_outl(rtf_part * w) {
         if(w->has_param && get_param(w) == 0)
            m_pprocessor->attr_pop(attr_outline);
         else
            m_pprocessor->attr_push(attr_outline,0);
         return false;
      }

      /*========================================================================
      * Name:	cmd_ansi
      * Purpose:	Executes the \ansi command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ansi(rtf_part * w) {
         m_pprocessor->charset(charset_ansi);
         return false;
      }

      /*========================================================================
      * Name:	cmd_ansicpg
      * Purpose:	Executes the \ansicpg command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ansicpg(rtf_part * w)
      {

         m_pprocessor->codepage(get_param(w));
         /*
         int i;
         for (i = 0; i < sizeof(codepages) / sizeof(CodepageInfo); i ++) {
         charset_codepage = &codepages[i];
         if (charset_codepage->cp == param) {
         // FIXME:
         //if (op->comment_begin) {
         //	printf(op->comment_begin);
         //	printf("document uses ANSI codepage %d character set", param);
         //	printf(op->comment_end);
         //}
         break;
         }
         }
         */
         //	FIXME:
         //	if ((charset_codepage == NULL || charset_codepage->cp == 0) && op->comment_begin) {
         //		printf(op->comment_begin);
         //		printf("document uses default ANSI codepage character set");
         //		printf(op->comment_end);
         //	}
         return false;
      }

      /*========================================================================
      * Name:	cmd_pc
      * Purpose:	Executes the \pc command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_pc(rtf_part * w) {
         m_pprocessor->charset(charset_cp437);
         return false;
      }

      /*========================================================================
      * Name:	cmd_pca
      * Purpose:	Executes the \pca command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_pca(rtf_part * w) {
         m_pprocessor->charset(charset_cp850);
         return false;
      }

      /*========================================================================
      * Name:	cmd_mac
      * Purpose:	Executes the \mac command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_mac(rtf_part * w) {
         m_pprocessor->charset(charset_mac);
         return false;
      }

      /*========================================================================
      * Name:	cmd_colortbl
      * Purpose:	Executes the \colortbl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_colortbl(rtf_part * w) {

         process_color_table();

         return true;
      }

      /*========================================================================
      * Name:	cmd_fonttbl
      * Purpose:	Executes the \fonttbl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_fonttbl(rtf_part * w) {

         process_font_table();

         return true;
      }

      /*========================================================================
      * Name:	cmd_header
      * Purpose:	Executes the \header command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_header(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_headerl
      * Purpose:	Executes the \headerl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_headerl(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_headerr
      * Purpose:	Executes the \headerr command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_headerr(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_headerf
      * Purpose:	Executes the \headerf command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_headerf(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_footer
      * Purpose:	Executes the \footer command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_footer(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_footerl
      * Purpose:	Executes the \footerl command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_footerl(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_footerr
      * Purpose:	Executes the \footerr command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_footerr(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_footerf
      * Purpose:	Executes the \footerf command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_footerf(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_ignore
      * Purpose:	Dummy function to get rid of subgroups
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_ignore(rtf_part * w) {
         return true;
      }

      /*========================================================================
      * Name:	cmd_info
      * Purpose:	Executes the \info command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_info(rtf_part * w) {
         process_info_group();
         return true;
      }

      /*========================================================================
      * Name:	cmd_pict
      * Purpose:	Executes the \pict command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_pict(rtf_part * w) {
         within_picture = true;
         m_imageCurrent.width = 0;
         m_imageCurrent.height = 0;
         m_imageCurrent.m_epict = pict_wb;
         return false;
      }

      /*========================================================================
      * Name:	cmd_bin
      * Purpose:	Executes the \bin command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_bin(rtf_part * w) {
         return false;
      }


      /*========================================================================
      * Name:	cmd_macpict
      * Purpose:	Executes the \macpict command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_macpict(rtf_part * w) {
         m_imageCurrent.m_epict = pict_mac;
         return false;
      }

      /*========================================================================
      * Name:	cmd_jpegblip
      * Purpose:	Executes the \jpegblip command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_jpegblip(rtf_part * w) {
         m_imageCurrent.m_epict = pict_jpeg;
         return false;
      }

      /*========================================================================
      * Name:	cmd_pngblip
      * Purpose:	Executes the \pngblip command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_pngblip(rtf_part * w) {
         m_imageCurrent.m_epict = pict_png;
         return false;
      }

      /*========================================================================
      * Name:	cmd_pnmetafile
      * Purpose:	Executes the \pnmetafile command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_pnmetafile(rtf_part * w) {
         m_imageCurrent.m_epict = pict_pm;
         return false;
      }

      /*========================================================================
      * Name:	cmd_wmetafile
      * Purpose:	Executes the \wmetafile command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_wmetafile(rtf_part * w) {
         m_imageCurrent.m_epict = pict_wm;
         if(within_picture) {
            m_imageCurrent.m_iWmfMappingMode = get_param(w);
            switch(get_param(w))
            {
            case 1: m_imageCurrent.m_strWMFMappingMode = "MM_TEXT"; break;
            case 2: m_imageCurrent.m_strWMFMappingMode = "MM_LOMETRIC"; break;
            case 3: m_imageCurrent.m_strWMFMappingMode = "MM_HIMETRIC"; break;
            case 4: m_imageCurrent.m_strWMFMappingMode = "MM_LOENGLISH"; break;
            case 5: m_imageCurrent.m_strWMFMappingMode = "MM_HIENGLISH"; break;
            case 6: m_imageCurrent.m_strWMFMappingMode = "MM_TWIPS"; break;
            case 7: m_imageCurrent.m_strWMFMappingMode = "MM_ISOTROPIC"; break;
            case 8: m_imageCurrent.m_strWMFMappingMode = "MM_ANISOTROPIC"; break;
            default: m_imageCurrent.m_strWMFMappingMode = "default:MM_TEXT"; break;
            }
         }
         return false;
      }

      /*========================================================================
      * Name:	cmd_wbmbitspixel
      * Purpose:	Executes the \wbmbitspixel command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_wbmbitspixel(rtf_part * w) {
         if(within_picture)
            m_imageCurrent.bits_per_pixel = get_param(w);
         return false;
      }

      /*========================================================================
      * Name:	cmd_picw
      * Purpose:	Executes the \picw command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_picw(rtf_part * w) {
         if(within_picture)
            m_imageCurrent.width = get_param(w);
         return false;
      }

      /*========================================================================
      * Name:	cmd_pich
      * Purpose:	Executes the \pich command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_pich(rtf_part * w) {
         if(within_picture)
            m_imageCurrent.height = get_param(w);
         return false;
      }


      /*========================================================================
      * Name:	cmd_xe
      * Purpose:	Executes the \xe (index entry) command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_xe(rtf_part * w) {
         process_index_entry();
         return true;
      }

      /*========================================================================
      * Name:	cmd_tc
      * Purpose:	Executes the \tc (TOC entry) command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_tc(rtf_part * w) {
         process_toc_entry(true);
         return true;
      }

      /*========================================================================
      * Name:	cmd_tcn
      * Purpose:	Executes the \tcn (TOC entry, no page #) command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_tcn(rtf_part * w) {
         process_toc_entry(false);
         return true;
      }


      int processor::cmd_themedata(rtf_part * w) {

         ///int r, g, b;
         rtf_part * part;

         //	CHECK_PARAM_NOT_NULL(w);

         /* Sometimes, RTF color tables begin with a semicolon,
         * i.e. an empty color entry. This seems to indicate that color 0
         * will not be used, so here I set it to black.
         */
         //r = g = b = 0;

         part = m_psource->get_next_part();

         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
               m_psource->skip_block_to_outside();
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               m_psource->push_back_current_part();
               break;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {
               //if (compare(part, "red"))
               //   r = get_param(part);
               //else if (compare(part, "green"))
               //   g = get_param(part);
               //else if (compare(part, "blue"))
               //   b = get_param(part);
            }
            else if(part->type == PART_TYPE_TEXT)
            {
               //if (compare(part, ";"))
               //{
               //   while (r > 255) r >>= 8;
               //   while (g > 255) g >>= 8;
               //   while (b > 255) b >>= 8;
               //   m_pprocessor->register_color(r, g, b);
               //   r = g = b = 0;
               //}
            }

            part = m_psource->get_next_part();
         }

         return true;
      }

      int processor::cmd_ignore_all_parts(rtf_part * w) {

         ///int r, g, b;
         rtf_part * part;

         //	CHECK_PARAM_NOT_NULL(w);

         /* Sometimes, RTF color tables begin with a semicolon,
         * i.e. an empty color entry. This seems to indicate that color 0
         * will not be used, so here I set it to black.
         */
         //r = g = b = 0;

         part = m_psource->get_next_part();

         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
               m_psource->skip_block_to_outside();
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               m_psource->push_back_current_part();
               break;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {
               //if (compare(part, "red"))
               //   r = get_param(part);
               //else if (compare(part, "green"))
               //   g = get_param(part);
               //else if (compare(part, "blue"))
               //   b = get_param(part);
            }
            else if(part->type == PART_TYPE_TEXT)
            {
               //if (compare(part, ";"))
               //{
               //   while (r > 255) r >>= 8;
               //   while (g > 255) g >>= 8;
               //   while (b > 255) b >>= 8;
               //   m_pprocessor->register_color(r, g, b);
               //   r = g = b = 0;
               //}
            }

            part = m_psource->get_next_part();
         }

         return true;
      }

      /*========================================================================
      * Name:	cmd_stylesheet
      * Purpose:	Executes the \stylesheet command.
      * Args:	rtf_part, paragraph align info, and numeric param if any.
      * Returns:	Flag, true only if rest of rtf_parts on line should be ignored.
      *=======================================================================*/

      int processor::cmd_stylesheet(rtf_part * part) {

         /* Specification :
         <stylesheet> 	'{' \stylesheet <style>+ '}'
         <style>			'{' <styledef>?<keycode>? <formatting> <additive>? <based>? <next>? <autoupd>? <link>? <locked>? <hidden>? <semihidden>? <personal>? <compose>? <reply>? <styleid>? <stylename>? ';' '}'
         <styledef>		\s  |\*\cs  | \ds | \ts\tsrowd
         <keycode>		'{' \keycode <keys> '}'
         <keys>			( \shift? & \ctrl? & \alt?) <key>
         <key>			\fn | #PCDATA
         <additive>		\additive
         <based>			\sbasedon
         <next>			\snext
         <autoupd>		\sautoupd
         <hidden>		\shidden
         <link>			\slinkN
         <locked>		\slocked
         <personal>		\spersonal
         <compose>		\scompose
         <reply>			\sreply
         <formatting>	(<brdrdef> | <parfmt> | <apoctl> | <tabdef> | <shading> | <chrfmt>)+
         <styleid>		\styrsidN
         <semihidden>	\ssemihidden
         <stylename>	#PCDATA
         */

         int block_level = 0;

         part = m_psource->get_next_part();

         style current_style;
         ::std::string stylename;

         while(part)
         {
            if(part->type == PART_TYPE_BLOCK_START)
            {
               block_level++;
               if(block_level == 1)
               {
                  current_style.bold = -1;
                  current_style.fontface = -1;
                  current_style.fontsize = -1;
                  current_style.italic = -1;
                  current_style.id = 0;
                  current_style.rsid = -1;
                  stylename = "";
                  current_style.m_eelement = element_paragraph;
               }
            }
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               if(block_level == 0)
               {
                  m_psource->push_back_current_part();
                  break;
               }
               else
               {
                  current_style.m_strName = stylename;
                  m_pprocessor->register_style(current_style);
                  stylename = "";
               }
               block_level--;
            }
            else if(part->type == PART_TYPE_COMMAND)
            {
               if(block_level == 2) // <keycode>
               {
                  // if( !strcmp("\\keycode", style ) )
                  {
                     /* This group is specified within the description of a style
                     in the style sheet in the RTF header. The syntax for this group
                     is '{\*’\keycode <keys>'}' where <keys> are the characters used
                     in the key code. For example, a style, Normal, may be defined
                     {\s0 {\*\keycode \shift\ctrl n}Normal;} within the RTF style sheet.
                     See the Special Character control words for the characters outside
                     the alphanumeric range that may be used.

                     \alt	The ALT modifier key. Used to describe shortcut key codes for styles.
                     \shift	The SHIFT modifier key. Used to describe shortcut key codes for styles.
                     \ctrl	The CTRL modifier key. Used to describe shortcut key codes for styles.
                     \fnN	Specifies a function key where N is the function key number. Used to describe shortcut-key codes for styles.

                     */
                     // TODO: 
                  }
                  m_psource->skip_block_to_end();
               }
               if(block_level == 1) // <style>
               {
                  // <styledef>
                  if(compare(part,"s") && part->has_param)
                  {
                     // Designates paragraph style.
                     current_style.id = part->param;
                     current_style.m_eelement = element_paragraph;
                  }
                  else if(compare(part,"cs") && part->has_param /* was_star */)
                  {
                     /*	Designates character style. Like \s, \cs is not a destination control word.
                     However, it is important to treat it like one inside the style sheet;
                     that is, \cs must be prefixed with \* and must appear as the first item
                     inside a group. Doing so ensures that readers that do not understand
                     character styles will skip the character style information correctly.
                     When used in body text to indicate that a character style was applied,
                     do not include the \* prefix.
                     */
                     current_style.id = part->param;
                     current_style.m_eelement = element_character;
                  }
                  else if(compare(part,"ds") && part->has_param)
                  {
                     // Designates section style.
                     // TODO:
                     current_style.id = part->param;
                     current_style.m_eelement = element_section;
                  }
                  else if(compare(part,"ts") && part->has_param)
                  {
                     // Designates table style, in the same style as \cs for placement and prefixes.
                     // TODO:
                     current_style.id = part->param;
                     current_style.m_eelement = element_table;
                  }
                  else if(compare(part,"tsrowd"))
                  {
                     // Like \trowd but for table style definitions.
                     // TODO:
                     current_style.m_eelement = element_table_row;
                  }
                  // </styledef>
                  else if(compare(part,"additive"))
                  {
                     // Used in a character style definition ('{\*'\cs'}'). Indicates that character style attributes are to be added to the current paragraph style attributes, rather than setting the paragraph attributes to only those defined in the character style definition.
                  }
                  else if(compare(part,"sbasedon"))
                  {
                     // Defines the number of the style the current style is based on (the default is 222—no style).
                  }
                  else if(compare(part,"snext"))
                  {
                     // Defines the style to be used in the next paragraph after the paragraph marked by this style. If it is omitted, the next style is the current style.
                  }
                  else if(compare(part,"sautoupd"))
                  {
                     // Automatically update styles.
                  }
                  else if(compare(part,"shidden"))
                  {
                     // Style does not appear in the Styles drop-down list in the Style dialog box  (on the Format menu, click Styles).
                  }
                  else if(compare(part,"slink") && part->has_param)
                  {
                     // The style is linked to the style whose stylesheet index is denoted by N.  A paragraph style is linked to a character style when they share the same font properties and the character style is updated when the paragraph style changes.  Normally rtf_part will suppress the display of the linked character style in most style lists.
                  }
                  else if(compare(part,"slocked"))
                  {
                     // The style is locked. It cannot be used in the current document if protection is on.
                  }
                  else if(compare(part,"spersonal"))
                  {
                     // Style is a personal e-mail style.
                  }
                  else if(compare(part,"scompose"))
                  {
                     // Style is the e-mail compose style.
                  }
                  else if(compare(part,"sreply"))
                  {
                     // Style is the e-mail reply style.
                  }
                  else if(compare(part,"styrsid") && part->has_param)
                  {
                     // Tied to the rsid table, N is the rsid of the author who implemented the style.
                     current_style.rsid = part->param;
                  }
                  else if(compare(part,"ssemihidden"))
                  {
                     // Style does not appear in drop-down menus.
                  }
                  else if(compare(part,"b"))
                  {
                     // bold
                     current_style.bold = 1;
                  }
                  else if(compare(part,"i"))
                  {
                     // italic
                     current_style.italic = 1;
                  }
                  else if(compare(part,"fs") && part->has_param)
                  {
                     // font size
                     current_style.fontsize = part->param;
                  }
                  else if(compare(part,"f") && part->has_param)
                  {
                     // font
                     current_style.fontface = part->param;
                  }
                  else
                  {
                     // TODO: Error: Unknown command
                  }
               }
            }
            else if(part->type == PART_TYPE_TEXT)
            {
               if(block_level == 1)
                  stylename += part->content;
            }
            part = m_psource->get_next_part();
         }
         return false;
      }

      class HashItem
      {
      public:

         const char * m_name;

         char                 m_needed_param;	// 2 = no, 1 = yes, 0 = don't check

         processor::PFN_CMD   m_func;

         char * m_debug_print;

      };


      /* All of the possible commands that RTF might recognize. */
      static HashItem hashArray_other[] ={
         {"*",0,&processor::cmd_ignore,NULL},
         {"-",0,&processor::cmd_optional_hyphen,"optional hyphen"},
         {"_",0,&processor::cmd_nonbreaking_hyphen,"nonbreaking hyphen"},
         {"~",0,&processor::cmd_nonbreaking_space,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_a[] ={
         {"ansi",0,&processor::cmd_ansi,NULL},
         {"ansicpg",0,&processor::cmd_ansicpg,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_b[] ={
         {"b",0,&processor::cmd_b,NULL},
         {"bullet",0,&processor::cmd_bullet,NULL},
         {"bin",0,&processor::cmd_bin,"picture is binary"},
#if 0
         {"bgbdiag",0,NULL,NULL},
         {"bgcross",0,NULL,NULL},
         {"bgdcross",0,NULL,NULL},
         {"bgfdiag",0,NULL,NULL},
         {"bghoriz",0,NULL,NULL},
         {"bgkbdiag",0,NULL,NULL},
         {"bgkcross",0,NULL,NULL},
         {"bgkdcross",0,NULL,NULL},
         {"bgkfdiag",0,NULL,NULL},
         {"bgkhoriz",0,NULL,NULL},
         {"bgkvert",0,NULL,NULL},
         {"bgvert",0,NULL,NULL},
         {"brdrcf",0,NULL,NULL},
         {"brdrdb",0,NULL,NULL},
         {"brdrdot",0,NULL,NULL},
         {"brdrhair",0,NULL,NULL},
         {"brdrs",0,NULL,NULL},
         {"brdrsh",0,NULL,NULL},
         {"brdrth",0,NULL,NULL},
         {"brdrw",0,NULL,NULL},
#endif
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_c[] ={
         {"caps",0,&processor::cmd_caps,NULL},
         {"cb",1,&processor::cmd_cb,NULL},
         {"cf",1,&processor::cmd_cf,NULL},
         {"colortbl",0,&processor::cmd_colortbl,"color table"},
         {"cols",0,NULL,"columns (not implemented)"},
         {"column",0,NULL,"column break (not implemented)"},
         {"cs",1,&processor::cmd_cs,"Section style"},
         {"cbpat",0,NULL,"Paragraph Shading"},
         {"cellx",0,NULL,"Table Definitions"},
         {"cfpat",0,NULL,NULL},
         {"cgrid",0,NULL,NULL},
         {"charrsid",0,NULL,"Revision Mark (ignore)"},
         {"clbgbcross",0,NULL,NULL},
         {"clbgbdiag",0,NULL,NULL},
         {"clbgbkbdiag",0,NULL,NULL},
         {"clbgbkcross",0,NULL,NULL},
         {"clbgbkdcross",0,NULL,NULL},
         {"clbgbkfdiag",0,NULL,NULL},
         {"clbgbkhor",0,NULL,NULL},
         {"clbgbkvert",0,NULL,NULL},
         {"clbgdcross",0,NULL,NULL},
         {"clbgfdiag",0,NULL,NULL},
         {"clbghoriz",0,NULL,NULL},
         {"clbgvert",0,NULL,NULL},
         {"clbrdrb",0,NULL,NULL},
         {"clbrdrl",0,NULL,NULL},
         {"clbrdrr",0,NULL,NULL},
         {"clbrdrt",0,NULL,NULL},
         {"clcbpat",0,NULL,NULL},
         {"clcfpat",0,NULL,NULL},
         {"clmgf",0,NULL,NULL},
         {"clmrg",0,NULL,NULL},
         {"clshdng",0,NULL,NULL},
         {"colorschememapping",0,&processor::cmd_ignore_all_parts,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_d[] ={
         {"deff",0,NULL,"Default Font"},
         {"dn",0,&processor::cmd_dn,NULL},
         {"datastore",0,&processor::cmd_ignore_all_parts,NULL},
#if 0
         {"dibitmap",0,NULL,NULL},
#endif
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_e[] ={
         {"emdash",0,&processor::cmd_emdash,NULL},
         {"endash",0,&processor::cmd_endash,NULL},
         {"embo",0,&processor::cmd_emboss,NULL},
         {"expand",0,&processor::cmd_expand,NULL},
         {"expandedcolortbl",0,&processor::cmd_expandedcolortbl,NULL},
         {"expnd",0,&processor::cmd_expand,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_f[] ={
         {"f",1,&processor::cmd_f,NULL},
         {"fdecor",0,&processor::cmd_fdecor,NULL},
         {"fmodern",0,&processor::cmd_fmodern,NULL},
         {"fnil",0,&processor::cmd_fnil,NULL},
         {"fonttbl",0,&processor::cmd_fonttbl,"font table"},
         {"froman",0,&processor::cmd_froman,NULL},
         {"fs",1,&processor::cmd_fs,NULL},
         {"fscript",0,&processor::cmd_fscript,NULL},
         {"fswiss",0,&processor::cmd_fswiss,NULL},
         {"ftech",0,&processor::cmd_ftech,NULL},
         {"field",0,&processor::cmd_field,NULL},
         {"footer",0,&processor::cmd_footer,NULL},
         {"footerf",0,&processor::cmd_footerf,NULL},
         {"footerl",0,&processor::cmd_footerl,NULL},
         {"footerr",0,&processor::cmd_footerr,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_g[] ={
         {"generator",0,&processor::cmd_generator,NULL},
      };
      static HashItem hashArray_h[] ={
         {"highlight",0,&processor::cmd_highlight,NULL},
         {"header",0,&processor::cmd_header,NULL},
         {"headerf",0,&processor::cmd_headerf,NULL},
         {"headerl",0,&processor::cmd_headerl,NULL},
         {"headerr",0,&processor::cmd_headerr,NULL},
         {"hl",0,&processor::cmd_ignore,"hyperlink within object"},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_i[] ={
         {"i",0,&processor::cmd_i,NULL},
         {"info",0,&processor::cmd_info,NULL},
         {"insrsid",0,NULL,"Revision Mark (ignore)"},
         {"intbl",0,&processor::cmd_intbl,NULL},
         {"impr",0,&processor::cmd_engrave,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_j[] ={
         {"jpegblip",0,&processor::cmd_jpegblip,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_l[] ={
         {"ldblquote",0,&processor::cmd_ldblquote,NULL},
         {"line",0,&processor::cmd_line,NULL},
         {"lquote",0,&processor::cmd_lquote,NULL},
         {"latentstyles",0,&processor::cmd_ignore_all_parts,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_m[] ={
         {"mac",0,&processor::cmd_mac,NULL},
         {"macpict",0,&processor::cmd_macpict,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_n[] ={
         {"nosupersub",0,&processor::cmd_nosupersub,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_o[] ={
         {"outl",0,&processor::cmd_outl,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_p[] ={
         {"page",0,&processor::cmd_page,NULL},
         {"par",0,&processor::cmd_par,NULL},
         {"pc",0,&processor::cmd_pc,NULL},
         {"pca",0,&processor::cmd_pca,NULL},
         {"pich",1,&processor::cmd_pich,NULL},
         {"pict",2,&processor::cmd_pict,"picture"},
         {"picw",1,&processor::cmd_picw,NULL},
         {"pgdsctbl",0,&processor::cmd_pgdsctbl,"hgw"},
         {"plain",0,&processor::cmd_plain,NULL},
         {"pngblip",0,&processor::cmd_pngblip,NULL},
         {"pnmetafile",0,&processor::cmd_pnmetafile,NULL},
         {"pntxta",0,&processor::cmd_pntxta,"hgw"},
         {"pntxtb",0,&processor::cmd_pntxtb,"hgw"},
#if 0
         {"piccropb",0,NULL,NULL},
         {"piccropl",0,NULL,NULL},
         {"piccropr",0,NULL,NULL},
         {"piccropt",0,NULL,NULL},
         {"pichgoal",0,NULL,NULL},
         {"pichgoal",0,NULL,NULL},
         {"picscaled",0,NULL,NULL},
         {"picscalex",0,NULL,NULL},
         {"picwgoal",0,NULL,NULL},
#endif
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_r[] ={
         {"rdblquote",0,&processor::cmd_rdblquote,NULL},
         {"rquote",0,&processor::cmd_rquote,NULL},
         {"rtf",0,&processor::cmd_rtf,NULL},
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_s[] ={
         {"s",1,&processor::cmd_s,"style"},
         {"sect",0,&processor::cmd_sect,"section break"},
         {"scaps",0,&processor::cmd_scaps,NULL},
         {"super",0,&processor::cmd_super,NULL},
         {"sub",0,&processor::cmd_sub,NULL},
         {"shad",0,&processor::cmd_shad,NULL},
         {"strike",0,&processor::cmd_strike,NULL},
         {"striked",0,&processor::cmd_striked,NULL},
         {"strikedl",0,&processor::cmd_strikedl,NULL},
         {"stylesheet",0,&processor::cmd_stylesheet,"style sheet"},
         {"shp",0,&processor::cmd_shp,"drawn shape"},
#if 0
         {"shading",NULL,NULL},
#endif
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_t[] ={
         {"tab",0,&processor::cmd_tab,NULL},
         {"tc",0,&processor::cmd_tc,"TOC entry"},
         {"tcn",0,&processor::cmd_tcn,"TOC entry"},
         {"themedata",0,&processor::cmd_themedata,NULL},
#if 0
         {"tcf",0,NULL,NULL},
         {"tcl",0,NULL,NULL},
         {"trgaph",0,NULL,NULL},
         {"trleft",0,NULL,NULL},
         {"trowd",0,NULL,NULL},
         {"trqc",0,NULL,NULL},
         {"trql",0,NULL,NULL},
         {"trqr",0,NULL,NULL},
         {"trrh",0,NULL,NULL},
#endif
         {NULL,0,NULL,NULL}
      };
      static HashItem hashArray_u[] ={
#if 1 /* daved - 0.19.4 */
         {"u",0,&processor::cmd_u,NULL},
#endif
         {"ul",0,&processor::cmd_ul,NULL},
         {"up",0,&processor::cmd_up,NULL},
         {"uld",0,&processor::cmd_uld,NULL},
         {"uldash",0,&processor::cmd_uldash,NULL},
         {"uldashd",0,&processor::cmd_uldashd,NULL},
         {"uldashdd",0,&processor::cmd_uldashdd,NULL},
         {"uldb",0,&processor::cmd_uldb,NULL},
         {"ulnone",0,&processor::cmd_ulnone,NULL},
         {"ulth",0,&processor::cmd_ulth,NULL},
         {"ulthd",0,&processor::cmd_ulthd,NULL},
         {"ulthdash",0,&processor::cmd_ulthdash,NULL},
         {"ulw",0,&processor::cmd_ulw,NULL},
         {"ulwave",0,&processor::cmd_ulwave,NULL},
         {NULL,0,NULL,NULL}
      };

      static HashItem hashArray_v[] ={
         {"v",0,NULL,"Hidden Text"},
         {NULL,0,NULL,NULL}
      };

      static HashItem hashArray_w[] ={
         {"wbmbitspixel",1,&processor::cmd_wbmbitspixel,NULL},
         {"wmetafile",0,&processor::cmd_wmetafile,NULL},
         {"wgrffmtfilter",0,&processor::cmd_ignore_all_parts,NULL},
#if 0
         {"wbitmap",0,NULL,NULL},
         {"wbmplanes",0,NULL,NULL},
         {"wbmwidthbytes",0,NULL,NULL},
#endif
         {NULL,0,NULL,NULL}
      };

      static HashItem hashArray_x[] ={
         {"xe",0,&processor::cmd_xe,"index entry"},
         {"xmlns",0,&processor::cmd_ignore_all_parts,NULL},
         {NULL,0,NULL,NULL}
      };

      static HashItem * hash[26] ={
         hashArray_a,
         hashArray_b,
         hashArray_c,
         hashArray_d,
         hashArray_e,
         hashArray_f,
         hashArray_g,
         hashArray_h,
         hashArray_i,
         hashArray_j,
         NULL,
         hashArray_l,
         hashArray_m,
         hashArray_n,
         hashArray_o,
         hashArray_p,
         NULL,
         hashArray_r,
         hashArray_s,
         hashArray_t,
         hashArray_u,
         hashArray_v,
         hashArray_w,
         hashArray_x,
         NULL,
         NULL,
      };


      /*-------------------------------------------------------------------*/
      /*-------------------------------------------------------------------*/



      /*-------------------------------------------------------------------*/
      /*-------------------------------------------------------------------*/


      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/
      void processor::print_with_special_exprs(const char * s)
      {

         m_pprocessor->print(this,s);

      }


      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/

      void processor::begin_table()
      {
         within_table = true;
         have_printed_row_begin = false;
         have_printed_cell_begin = false;
         have_printed_row_end = false;
         have_printed_cell_end = false;
         m_pprocessor->attrstack_push();

         m_pprocessor->table_begin();
      }



      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/

      void processor::end_table()
      {
         if(within_table) {
            if(!have_printed_cell_end) {
               m_pprocessor->attr_pop_dump();
               m_pprocessor->table_cell_end();
            }
            if(!have_printed_row_end) {
               m_pprocessor->table_row_end();
            }
            m_pprocessor->table_end();

            within_table = false;
            have_printed_row_begin = false;
            have_printed_cell_begin = false;
            have_printed_row_end = false;
            have_printed_cell_end = false;
         }
      }



      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/

      void processor::starting_text()
      {
         if(within_table) {
            if(!have_printed_row_begin) {
               m_pprocessor->table_row_begin();
               have_printed_row_begin = true;
               have_printed_row_end = false;
               have_printed_cell_begin = false;
            }
            if(!have_printed_cell_begin) {
               m_pprocessor->table_cell_begin();

               // FIXME: there is no another way to set style in every cell ?
               //filter->attrstack_express_all(filter);
               have_printed_cell_begin = true;
               have_printed_cell_end = false;
            }
         }
      }




      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/

      void processor::starting_paragraph_align(int align)
      {
         switch(align)
         {
         case align_center:
            m_pprocessor->print_char(char_center_begin);
            break;
         case align_left:
            break;
         case align_right:
            m_pprocessor->print_char(char_right_begin);
            break;
         case align_justify:
            m_pprocessor->print_char(char_justify_begin);
            break;
         }
      }



      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/

      void processor::ending_paragraph_align(int align)
      {
         switch(align) {
         case align_center:
            m_pprocessor->print_char(char_center_end);
            break;
         case align_left:
            break;
         case align_right:
            m_pprocessor->print_char(char_right_end);
            break;
         case align_justify:
            m_pprocessor->print_char(char_justify_end);
            break;
         }
      }


      /*========================================================================
      * Name:
      * Purpose:	Recursive routine to produce the output in the target
      *		format given on a tree of words.
      * Args:	rtf_part* (the tree).
      * Returns:	None.
      *=======================================================================*/

      void processor::process_block(rtf_part * part)
      {
         int is_cell_group = false;
         int paragraph_begined = false;
         int paragraph_align = align_left;

         CHECK_PARAM_NOT_NULL(w);

         if(!coming_pars_that_are_tabular && within_table) {
            end_table();
         }
         else if(coming_pars_that_are_tabular && !within_table) {
            begin_table();
         }

         m_pprocessor->attrstack_push();

         while(part)
         {
            if(part->type == PART_TYPE_TEXT)
            {

               const char * s = part->content.c_str();



               starting_text();

               if(!paragraph_begined) {
                  starting_paragraph_align(paragraph_align);
                  paragraph_begined = true;
               }

               /*----------------------------------------*/
               if(within_picture) {
                  if(!m_imageCurrent.m_pmanager)
                  {

                     m_imageCurrent.m_pmanager = m_pprocessor->image_begin(m_imageCurrent);

                  }

                  if(s[0] != ' ') {
                     const char * s2;
                     if(m_imageCurrent.width && m_imageCurrent.height)
                        // && m_imageCurrent.bits_per_pixel)
                     {
                        unsigned char buf[512];
                        unsigned long index = 0;

                        s2 = s;
                        while(*s2) {
                           unsigned int tmp,value;
                           tmp = tolower(*s2++);
                           if(tmp > '9') tmp -= ('a' - 10);
                           else tmp -= '0';
                           value = 16 * tmp;
                           tmp = tolower(*s2++);
                           if(tmp > '9') tmp -= ('a' - 10);
                           else tmp -= '0';
                           value += tmp;

                           buf[index++] = value;

                           if(index == sizeof(buf))
                           {
                              if(m_imageCurrent.m_pmanager)
                                 m_imageCurrent.m_pmanager->put(buf,index);
                              index = 0;
                           }

                        }
                        if(index)
                        {
                           if(m_imageCurrent.m_pmanager)
                              m_imageCurrent.m_pmanager->put(buf,index);
                        }
                     }
                  }
               }
               /*----------------------------------------*/
               else
                  print_with_special_exprs(s);
            }
            else if(part->type == PART_TYPE_COMMAND)
               /*----Paragraph alignment----------------------------------------------------*/
            {
               const char * s = part->content.c_str();

               if(!strcmp("ql",s))
                  paragraph_align = align_left;
               else if(!strcmp("qr",s))
                  paragraph_align = align_right;
               else if(!strcmp("qj",s))
                  paragraph_align = align_justify;
               else if(!strcmp("qc",s))
                  paragraph_align = align_center;
               else if(!strcmp("pard",s))
               {
                  /* Clear out all font attributes.
                  */
                  m_pprocessor->attr_pop_all();

                  ending_paragraph_align(paragraph_align);

                  // loading default paragraph style.
                  m_pprocessor->attr_push_style(element_character,0);
                  m_pprocessor->attr_push_style(element_paragraph,0);

                  paragraph_align = align_left;
                  paragraph_begined = false; // ?
               }
               else if(!strcmp(s,"cell"))
               {
                  /*----Table keywords---------------------------------------------------------*/
                  is_cell_group = true;
                  if(!have_printed_cell_begin) {
                     /* Need this with empty cells */
                     m_pprocessor->table_cell_begin();
                     // Using of this same style is needed.	
                     // attrstack_express_all();
                  }
                  m_pprocessor->attr_pop_dump();
                  m_pprocessor->table_cell_end();
                  have_printed_cell_begin = false;
                  have_printed_cell_end = true;
               }
               else if(!strcmp(s,"row")) {
                  if(within_table) {
                     m_pprocessor->table_row_end();
                     have_printed_row_begin = false;
                     have_printed_row_end = true;
                  }
                  else {
                     // if (debug_mode) {
                     // FIXME: 
                     //	printf(op->comment_begin);
                     //	printf("end of table row");
                     //	printf(op->comment_end);
                     // }
                  }
               }

               /* else if (*s == '\'')
               {
               // ----Special chars---------------------------------------------------------

               // \'XX is a hex char code expression
               int ch = h2toi (&s[1]);
               char *s2;

               s2 = translate_char( f, ch );

               if (!s2 || !*s2) {
               //	FIXME
               //	printf(op->comment_begin);
               //	printf("char 0x%02x",ch);
               //	printf(op->comment_end);
               } else {
               print_char( f, CHAR_WORD_BEGIN);
               print( s2);
               print_char( f, CHAR_WORD_END );
               }
               } */
               else
               {
                  /*----Search the RTF command hash-------------------------------------------*/
                  int ch;
                  int index = 0;
                  HashItem * hip;
                  const char * p;
                  int match = false;  /* Pacify gcc,  st001906 - 0.19.6 */

                  /* Look for a parameter */
                  p = s;

                  /* Generate a hash index */
                  ch = tolower(*s);
                  if(ch >= 'a' && ch <= 'z')
                     hip = hash[ch - 'a'];
                  else
                     hip = hashArray_other;

                  if(hip)
                  {
                     while(1)
                     {
                        char * debug;
                        match = false;

                        match = !strcmp(s,hip[index].m_name);

                        if(!match)
                        {
                           index++;
                           if(!hip[index].m_name)
                              break;
                           continue;
                        }

                        if(hip[index].m_func)
                        {
                           int terminate_group;

                           if(hip[index].m_needed_param)
                           {
                              int func_param = hip[index].m_needed_param;
                              if(part->has_param && func_param == 2)
                              {
                                 char buf[128];
                                 _snprintf(buf,sizeof(buf),"Command \'%s\' should be without param.",s);
                                 m_pobserver->warning(warn_command_without_param,buf);
                                 break;
                              }
                              else if(part->has_param == false && func_param == 1)
                              {
                                 char buf[128];
                                 _snprintf(buf,sizeof(buf),"Command \'%s\' should be with param.",s);
                                 m_pobserver->warning(warn_command_with_param,buf);
                                 break;
                              }
                           }

                           terminate_group = (this->*hip[index].m_func)(part);

                           /*
                           if (terminate_group == SKIP_ONE_WORD)
                           w=w->next;
                           else if (terminate_group)
                           while(w)
                           w=w->next;
                           */
                        }

                        debug = hip[index].m_debug_print;

                        // if (debug && debug_mode) {
                        // FIXME
                        //	printf(op->comment_begin);
                        //	printf("%s", debug);
                        //	printf(op->comment_end);
                        // }
                        break;
                     }
                  }
                  if(!match)
                  {
                     char buf[128];
                     _snprintf(buf,sizeof(buf) - 1,"Command not found in hash table : %s",s);
                     m_pobserver->warning(warn_command_not_found,buf);
                  }
               }
            }
            else if(part->type == PART_TYPE_BLOCK_START)
            {
               if(!paragraph_begined) {
                  starting_paragraph_align(paragraph_align);
                  paragraph_begined = true;
               }

               process_block(m_psource->get_next_part());
            }
            else if(part->type == PART_TYPE_BLOCK_END)
            {
               break;
            }
            part = m_psource->get_next_part();
         }

         if(m_imageCurrent.m_pmanager) {
            m_pprocessor->image_end(m_imageCurrent.m_pmanager);
            m_imageCurrent.m_pmanager = NULL;
            within_picture = false;
         }

         /* Undo font attributes UNLESS we're doing table cells
         * since they would appear between </td> and </tr>.
         */
         if(!is_cell_group)
            m_pprocessor->attr_pop_all();
         else
            m_pprocessor->attr_drop_all();

         /* Undo paragraph alignment
         */
         if(paragraph_begined)
            ending_paragraph_align(paragraph_align);

         m_pprocessor->attrstack_drop();
      }


      /*========================================================================
      * Name:
      * Purpose:
      * Args:	None.
      * Returns:	None.
      *=======================================================================*/
      void processor::rtf_process(::text_format_rtf::source * src,::text_format_rtf::processor * pprocessor,::text_format_rtf::observer * pobserver)
      {

         impl::source isrc(pobserver);

         m_pprocessor = pprocessor;

         isrc.m_psource = src;

         isrc.current_part.content = "";

         isrc.use_current_part = 0;
         isrc.last_returned_ch = 0;
         isrc.ungot_char = -1;
         isrc.ungot_char2 = -1;
         isrc.ungot_char3 = -1;
         isrc.buf = 0;
         isrc.index = 0;
         isrc.buf_size = 0;
         isrc.read_bytes = 0;

         m_pobserver = pobserver;
         charset_codepage = 0;
         coming_pars_that_are_tabular = 0;
         within_table = 0;
         have_printed_row_begin = 0;
         have_printed_cell_begin = 0;
         have_printed_row_end = 0;
         have_printed_cell_end = 0;
         within_picture = 0;
         m_imageCurrent.m_pmanager = 0;
         simulate_smallcaps = 0;
         simulate_allcaps = 0;
         numchar_table = 0;
         m_psource = &isrc;

         m_imageCurrent.width = 0;
         m_imageCurrent.height = 0;
         m_imageCurrent.bits_per_pixel = 0;
         m_imageCurrent.m_epict = pict_unknown;
         m_imageCurrent.m_iWmfMappingMode = 0;
         m_imageCurrent.m_strWMFMappingMode = "";

         m_pprocessor->begin();

         within_table = false;
         simulate_allcaps = false;

         process_block(isrc.get_next_part());

         end_table();

         m_pprocessor->end();

         isrc.current_part.content = "";
      }

   } // namespace impl

} // namespace text_format_rtf




