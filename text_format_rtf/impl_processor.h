#pragma once

#define DECL_CMD(xxx) int xxx(rtf_part * ppart);


namespace text_format_rtf
{


   namespace impl
   {


      class processor
      {
      public:


         //virtual void internal_init();

         typedef int (processor::* PFN_CMD)(rtf_part *);

         ::text_format_rtf::processor *     m_pprocessor;
         ::text_format_rtf::observer *      m_pobserver;
         impl::source *                      m_psource;
         output *                            m_poutput;


         codepage_info * charset_codepage;
         int   coming_pars_that_are_tabular; // = 0;
         int   have_printed_row_begin; // =FALSE;
         int   have_printed_cell_begin; // =FALSE;
         int   have_printed_row_end; // =FALSE;
         int   have_printed_cell_end; // =FALSE;
         int   within_table; // = FALSE;
         int   within_picture; // = FALSE;
         int   simulate_smallcaps;
         int   simulate_allcaps;
         short numchar_table;

         image_descriptor  m_imageCurrent;
         image_manager * m_pimagemanagerCurrent;

         //enum summary_categories
         //{
         //   nubmer_of_pages,
         //   number_of_words,
         //   number_of_chars,
         //};
         //enum special_characters
         //{
         //   bullet,
         //   left_quote,
         //   right_quote,
         //   left_dbl_quote,
         //   right_dbl_quote,
         //   nonbreaking_space,
         //   emdash,
         //   endash,
         //   lessthan,
         //   greaterthan,
         //   amp,
         //   copyright,
         //   trademark,
         //   nonbreaking_hyphen,
         //   optional_hyphen,
         //   dbl_quote,
         //   line_break,
         //   page_break,
         //   paragraph_begin,
         //   center_begin,	// move to attr_code
         //   right_begin,	// -- || --
         //   justify_begin,	// -- || --
         //   center_end,	// -- || --
         //   right_end,		// -- || --
         //   justify_end,	// -- || --
         //   tab,
         //};

         processor(::text_format_rtf::output * poutput);
         virtual ~processor();

         //virtual void charset(int charset_id);
         //virtual void codepage(int cp_id);

         //virtual void begin();
         //virtual void end();
         //virtual void title(::std::string title);
         //virtual void keywords(::std::string keywords);
         //virtual void author(::std::string author);
         //virtual void summary(e_document_summary cat, int param);

         //virtual char * translate_char(int);

         //virtual void print(::std::string);
         //virtual void print_symbol(::std::string);
         //virtual void print_forced_space();
         //virtual int  print_unicode(int);
         //virtual void hyperlink(::std::string);
         //virtual void hyperlink_base(::std::string);

         //virtual void print_char(e_char ch);
         //virtual void font_smaller_begin();
         //virtual void font_smaller_end();
         //virtual void table_begin();
         //virtual void table_end();
         //virtual void table_cell_begin();
         //virtual void table_cell_end();
         //virtual void table_row_begin();
         //virtual void table_row_end();

         //virtual image_manager * image_begin(const image_descriptor & image);
         //virtual void image_end(image_manager *);

         void rtf_process(::text_format_rtf::source * src, ::text_format_rtf::processor * pprocessor, ::text_format_rtf::observer * log);

         // impl
         void process_font_table();
         void process_index_entry();
         void process_toc_entry(int include_page_num);
         void process_info_group();
         void process_color_table();
         void process_expanded_color_table();
         void process_block(rtf_part * ppart);

         ::std::string & read_to_end(::std::string & result);



         DECL_CMD(cmd_ignore);
         DECL_CMD(cmd_optional_hyphen);
         DECL_CMD(cmd_nonbreaking_hyphen);
         DECL_CMD(cmd_nonbreaking_space);
         DECL_CMD(cmd_ansi);
         DECL_CMD(cmd_ansicpg);
         DECL_CMD(cmd_b);
         DECL_CMD(cmd_bullet);
         DECL_CMD(cmd_bin);
         DECL_CMD(cmd_caps);
         DECL_CMD(cmd_cb);
         DECL_CMD(cmd_cf);
         DECL_CMD(cmd_colortbl);
         DECL_CMD(cmd_cs);
         DECL_CMD(cmd_dn);
         DECL_CMD(cmd_emdash);
         DECL_CMD(cmd_endash);
         DECL_CMD(cmd_emboss);
         DECL_CMD(cmd_expand);
         DECL_CMD(cmd_expandedcolortbl);
         DECL_CMD(cmd_f);
         DECL_CMD(cmd_fdecor);
         DECL_CMD(cmd_fmodern);
         DECL_CMD(cmd_fnil);
         DECL_CMD(cmd_fonttbl);
         DECL_CMD(cmd_froman);
         DECL_CMD(cmd_fs);
         DECL_CMD(cmd_fscript);
         DECL_CMD(cmd_fswiss);
         DECL_CMD(cmd_ftech);
         DECL_CMD(cmd_field);
         DECL_CMD(cmd_footer);
         DECL_CMD(cmd_footerf);
         DECL_CMD(cmd_footerl);
         DECL_CMD(cmd_footerr);
         DECL_CMD(cmd_generator);
         DECL_CMD(cmd_highlight);
         DECL_CMD(cmd_header);
         DECL_CMD(cmd_headerf);
         DECL_CMD(cmd_headerl);
         DECL_CMD(cmd_headerr);
         DECL_CMD(cmd_i);
         DECL_CMD(cmd_info);
         DECL_CMD(cmd_intbl);
         DECL_CMD(cmd_engrave);
         DECL_CMD(cmd_jpegblip);
         DECL_CMD(cmd_ldblquote);
         DECL_CMD(cmd_line);
         DECL_CMD(cmd_lquote);
         DECL_CMD(cmd_mac);
         DECL_CMD(cmd_macpict);
         DECL_CMD(cmd_nosupersub);
         DECL_CMD(cmd_outl);
         DECL_CMD(cmd_page);
         DECL_CMD(cmd_par);
         DECL_CMD(cmd_pc);
         DECL_CMD(cmd_pca);
         DECL_CMD(cmd_pich);
         DECL_CMD(cmd_pict);
         DECL_CMD(cmd_picw);
         DECL_CMD(cmd_pgdsctbl);
         DECL_CMD(cmd_plain);
         DECL_CMD(cmd_pngblip);
         DECL_CMD(cmd_pnmetafile);
         DECL_CMD(cmd_pntxta);
         DECL_CMD(cmd_pntxtb);
         DECL_CMD(cmd_rdblquote);
         DECL_CMD(cmd_rquote);
         DECL_CMD(cmd_rtf);
         DECL_CMD(cmd_s);
         DECL_CMD(cmd_sect);
         DECL_CMD(cmd_scaps);
         DECL_CMD(cmd_super);
         DECL_CMD(cmd_sub);
         DECL_CMD(cmd_shad);
         DECL_CMD(cmd_strike);
         DECL_CMD(cmd_striked);
         DECL_CMD(cmd_strikedl);
         DECL_CMD(cmd_stylesheet);
         DECL_CMD(cmd_shp);
         DECL_CMD(cmd_tab);
         DECL_CMD(cmd_tc);
         DECL_CMD(cmd_tcn);
         DECL_CMD(cmd_themedata);
         DECL_CMD(cmd_u);
         DECL_CMD(cmd_ul);
         DECL_CMD(cmd_up);
         DECL_CMD(cmd_uld);
         DECL_CMD(cmd_uldash);
         DECL_CMD(cmd_uldashd);
         DECL_CMD(cmd_uldashdd);
         DECL_CMD(cmd_uldb);
         DECL_CMD(cmd_ulnone);
         DECL_CMD(cmd_ulth);
         DECL_CMD(cmd_ulthd);
         DECL_CMD(cmd_ulthdash);
         DECL_CMD(cmd_ulw);
         DECL_CMD(cmd_ulwave);
         DECL_CMD(cmd_wbmbitspixel);
         DECL_CMD(cmd_wmetafile);
         DECL_CMD(cmd_xe);


         DECL_CMD(cmd_ignore_all_parts);


         void begin_table();
         void print_with_special_exprs(const char * s);
         void end_table();

         void starting_paragraph_align(int align);
         void ending_paragraph_align(int align);

         void starting_text();

      };



   } // namespace impl


} // namespace text_format_rtf



#undef DECL_CMD