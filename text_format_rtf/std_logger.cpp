#include "framework.h"
#include "_text_format_rtf.h"



namespace text_format_rtf
{


   std_logger::std_logger(::std::ostream & output, bool log_errors, bool log_warnings, bool log_debug)
      : output(output),
      log_errors(log_errors),
      log_warnings(log_warnings),
      log_debug(log_debug)
   {

   }


   void std_logger::debug(const char * str)
   {

      output << "DEBUG   : " << str << "\n";

   }


   void std_logger::warning(int id, const char * str)
   {

      output << "WARNING : " << str << "\n";

   }


   void std_logger::error(int id, const char * str)
   {

      output << "ERROR   : " << str << "\n";

   }


} // namespace text_format_rtf



