#pragma once


namespace text_format_rtf
{


   enum e_element
   {

      element_paragraph,
      element_character,
      element_section,
      element_table,
      element_table_row,

   };



   enum e_attr
   {
      attr_none = 0,
      attr_bold,
      attr_italic,
      attr_underline,
      attr_double_ul,
      attr_word_ul,
      attr_thick_ul,
      attr_wave_ul,
      attr_dot_ul,
      attr_dash_ul,
      attr_dot_dash_ul,
      attr_2dot_dash_ul,

      attr_fontsize,
      attr_std_fontsize,
      attr_fontface,
      attr_foreground,
      attr_background,
      attr_caps,
      attr_smallcaps,

      attr_shadow,
      attr_outline,
      attr_emboss,
      attr_engrave,

      attr_super,
      attr_sub,
      attr_strike,
      attr_dbl_strike,

      attr_expand,

      /* ATTR_CONDENSE */

   };


   enum e_char
   {

      char_bullet,
      char_left_quote,
      char_right_quote,
      char_left_dbl_quote,
      char_right_dbl_quote,
      char_nonbreaking_space,
      char_emdash,
      char_endash,
      char_lessthan,
      char_greaterthan,
      char_amp,
      char_copyright,
      char_trademark,
      char_nonbreaking_hyphen,
      char_optional_hyphen,
      char_dbl_quote,
      char_line_break,
      char_page_break,
      char_paragraph_begin,
      char_center_begin,	// move to attr_code
      char_right_begin,	// -- || --
      char_justify_begin,	// -- || --
      char_center_end,	// -- || --
      char_right_end,		// -- || --
      char_justify_end,	// -- || --
      char_tab,

      last_chars

   };

   extern const char * std_fonts[];

   enum e_font
   {

      font_nil = 0,
      font_roman,
      font_swiss,
      font_modern,
      font_script,
      font_decor,
      font_tech,

   };


   enum e_pict
   {

      pict_unknown = 0,
      pict_wm,
      pict_mac,
      pict_pm,
      pict_di,
      pict_wb,
      pict_jpeg,
      pict_png,

   };



   enum e_document_summary
   {
      document_summary_number_of_pages,
      document_summary_number_of_words,
      document_summary_number_of_chars,
   };



   enum e_warn
   {
      warn_unknown = 0,
      warn_unknown_command,
      warn_too_long_font_name,
      warn_command_with_param,		// when command should be without param
      warn_command_without_param,	// when command should bw with param
      warn_command_not_found,
   };


   enum e_err
   {
      err_unknown = 0,
      err_out_of_memory,
      err_more_unget_needed,
      err_bad_character,
      err_not_allocated_buf,
      err_too_long_word,
   };


   enum e_charset{
      charset_ansi = 1,
      charset_mac,
      charset_cp437,
      charset_cp850,
   };


   /* Paragraph alignment (kludge)
 */
   enum e_align{
      align_left = 0,
      align_right,
      align_center,
      align_justify
   };
} // namespace text_format_rtf



