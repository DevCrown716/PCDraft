#pragma once


namespace text_format_rtf
{


   class std_logger :
      public observer
   {
   public:


      ::std::ostream & output;

      bool log_errors, log_warnings, log_debug;


      std_logger(::std::ostream & output, bool log_errors = true, bool log_warnings = true, bool log_debug = false);


      virtual void debug(const char *);
      virtual void warning(int id, const char *);
      virtual void error(int id, const char *);

   };


} // namespace text_format_rtf



