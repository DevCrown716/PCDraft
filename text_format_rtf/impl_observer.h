#pragma once


namespace text_format_rtf
{


   namespace impl
   {


      class observer
      {
      public:


         observer();

         virtual void debug(const char *);
         virtual void warning(int id, const char *) ;
         virtual void error(int id, const char *);

      };

   } // namespace impl


} // namespace text_format_rtf


