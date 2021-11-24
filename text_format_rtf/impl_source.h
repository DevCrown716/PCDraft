#pragma once


namespace text_format_rtf
{


   namespace impl
   {


      class source 
      {
      public:

         
         ::text_format_rtf::source * m_psource;


         ::text_format_rtf::observer * m_pobserver; // was "log"

         rtf_part current_part;

         int use_current_part;

         int last_returned_ch;	// =0;
         int ungot_char;         // = -1;
         int ungot_char2;        // = -1;
         int ungot_char3;        //= -1;

         char * buf;
         unsigned long index, buf_size, read_bytes;


         source(::text_format_rtf::observer * pobserver);

         virtual rtf_part * get_next_part();
         virtual rtf_part * get_next_command();
         virtual void skip_block_to_end();
         virtual void skip_block_to_outside();
         virtual void push_back_current_part();


         virtual void my_unget_char(int ch);
         virtual int get_next_char();


      };


   } // namespace impl


} // namespace text_format_rtf

