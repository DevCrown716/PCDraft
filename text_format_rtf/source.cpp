#include "framework.h"
#include "_text_format_rtf.h"

int digit_to_int(char ch)
{

   if(ch >= '0' && ch <= '9')
   {
      return ch - '0';
   }
   else if(ch >= 'A' && ch <= 'Z')
   {
      return ch - 'A' + 10;
   }
   else if(ch >= 'a' && ch <= 'z')
   {
      return ch - 'a' + 10;
   }
   return -1;
}

namespace text_format_rtf
{


   namespace impl
   {

      
      source::source(::text_format_rtf::observer * pobserver)
      {
         m_pobserver = pobserver;

      }


      void source::my_unget_char(int ch)
      {

         if (ungot_char >= 0 && ungot_char2 >= 0 && ungot_char3 >= 0)
         {

            m_pobserver->error(err_more_unget_needed, "More than 3 ungot chars");

         }

         ungot_char3 = ungot_char2;
         ungot_char2 = ungot_char;
         ungot_char = ch;
      }

      int source::get_next_char()
      {
         
         int ch;

         if (ungot_char >= 0) 
         {

            ch = ungot_char;

            ungot_char = ungot_char2;

            ungot_char2 = ungot_char3;

            ungot_char3 = -1;

            last_returned_ch = ch;

            return ch;

         }

         if (index >= buf_size)
         {
         
            if (!buf)
            {
            
               buf_size = 256;
               
               buf = (char *)malloc(buf_size);
               
               if (!buf)
               {
                  
                  // rdr->observer->error(rdr->observer, ERR_OUT_OF_MEMORY, "Cannot allocate buffer for reading");
                  
                  return EOF;
               
               }
          
            }

            if((read_bytes = (unsigned long)m_psource->read(buf,buf_size)) == 0)
            {

               return EOF;

            }

            index = 0;
           
         }

         if(read_bytes == index)
         {

            return EOF;

         }

         ch = buf[index++];

         last_returned_ch = ch;

         return ch;

      }


      void
         source::push_back_current_part()
      {
         use_current_part = true;
      }

      void source::skip_block_to_end()
      {
         skip_block_to_outside();
         push_back_current_part();
      }

      void source::skip_block_to_outside()
      {
         int blocks = 0;
         rtf_part * part;
         part = get_next_part();
         while (part)
         {
            if (part->type == PART_TYPE_BLOCK_START)
               blocks++;
            else if (part->type == PART_TYPE_BLOCK_END)
            {
               if (blocks == 0)
                  return;
               blocks--;
            }
            part = get_next_part();
         }
      }

      rtf_part * source::get_next_command()
      {
         rtf_part * part = NULL;
         part = get_next_part();
         while (part)
         {
            if (part->type == PART_TYPE_COMMAND)
               return part;
            else if (part->type == PART_TYPE_BLOCK_START)
               skip_block_to_outside();	// anyway it should not happen.
            else if (part->type == PART_TYPE_BLOCK_END)
               break;
            part = get_next_part();
         }
         return NULL;
      }



      rtf_part * source::get_next_part()
      {
         int ch, ch2;
         rtf_part * part;
         int is_control_word = 0;

         int have_whitespace = 0;
         int need_unget = 0;
         int negation = 1;

         if (use_current_part)
         {
            use_current_part = false;
            return &current_part;
         }

         part = &current_part;

         part->param = 0;
         part->has_param = 0;
         part->type = PART_TYPE_TEXT;

         part->content = "";

         do {
            ch = get_next_char();
         } while (ch == '\n');

         if (ch == ' ')
         {
            // Compress multiple space chars down to one.

            while (ch == ' ') {
               ch = get_next_char();
               have_whitespace = true;
            }
            if (have_whitespace) {
               my_unget_char(ch);
               part->append(' ');
               return part;
            }
         }

         switch (ch)
         {
         case EOF:
            return 0;

         case '\\':
            ch2 = get_next_char();

            // Look for two-character command words.
            switch (ch2)
            {
            case '\n':
               part->append("par");
               part->type = PART_TYPE_COMMAND;
               return part;
            case '~':
            case '{':
            case '}':
            case '\\':
            case '_':
            case '-':
               part->append('\\');
               part->append(ch2);
               return part;

            case '\'':
               // Preserve \'## expressions (hex char exprs) for later. 
               // append( & part->word, "\\\'");
               char chHi = get_next_char();
               char chLo = get_next_char();
               char chMerge = digit_to_int(chHi) << 4 | digit_to_int(chLo);
               part->append(chMerge);
               part->type = PART_TYPE_TEXT;
               ch = get_next_char();
               goto text_processing;
            }

            part->type = PART_TYPE_COMMAND;
            is_control_word = 1;
            // append_char( & part->word, ch );
            ch = ch2;
            break;

         case '\t':
            // In RTF, a tab char is the same as \tab.
            part->append("\\tab");
            return part;

         case '{':
            part->type = PART_TYPE_BLOCK_START;
            return part;
         case '}':
            part->type = PART_TYPE_BLOCK_END;
            return part;
         case ';':
            part->append(ch);
            return part;
         }

         text_processing:

         while (ch != EOF)
         {
            if(ch == '\\')
            {
               char ch2 = get_next_char();
               if(ch2 == '\'')
               {
                  part->append(digit_to_int(get_next_char()) << 4 | digit_to_int(get_next_char()));
                  ch = get_next_char();
                  continue;
               }
               my_unget_char(ch2);
               need_unget = true;
               break;
            }
            // Several chars always ends a word, and we need to save them.

            if (ch == '\t' || ch == '{' || ch == '}' ) {
               need_unget = true;
               break;
            }

            // A newline always ends a command word; we don't save it. 
            //  A newline is ignored if this is not a command word.

            if (ch == '\n') {
               if (is_control_word)
                  break;
               ch = get_next_char();
               continue;
            }

            // A semicolon always ends a command word; we do save it. 
            // A semicolon never ends a regular word.

            if (ch == ';') {
               if (is_control_word) {
                  need_unget = true;
                  break;
               }
            }

            // In this parser, a space character terminates
            // any word, and if it does not follow a command,
            // then it is a word in itself.

            if (ch == ' ') {
               if (!is_control_word)
                  need_unget = true;
               break;
            }

            // Identify a control word's numeric parameter.
            if (is_control_word) {
               if (!part->has_param && (isdigit(ch) || ch == '-'))
                  part->has_param = true;
               else
                  if (part->has_param && !isdigit(ch)) {
                     if (ch != ' ')
                        need_unget = true;
                     break;
                  }
            }

            if (part->has_param)
            {
               if (ch == '-')
                  negation = -1;
               else
                  part->param = (part->param * 10) + (ch - '0');
            }
            else
               part->append(ch);

            if (part->type == PART_TYPE_TEXT && part->content.size() >= 256)
            {
               // part->unfinised = true;
               break;
            }

            ch = get_next_char();
         }

         if (need_unget)
            my_unget_char(ch);

         part->param *= negation;

         return part;
      }


   } // namespace impl



} // namespace text_format_rtf





