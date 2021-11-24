//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//

#include "framework.h"
#include "_text_format_rtf.h"


#include <stdlib.h>


//CLASS_DECL_TEXT_FORMAT_RTF __pointer(::text_format_rtf::processor)
//create_rtf_processor(::text_format_rtf::output * poutput)
//{
//   //processor * proc = NULL;
////
//  //    proc = (processor *)malloc(sizeof(processor));
//
//    //  proc->object = (void *)out;
//
////      proc->begin = &rtf_begin;
//      //proc->end = &rtf_end;
//
//   return __new(::text_format_rtf::impl::processor(poutput));
//}


namespace text_format_rtf
{


   namespace impl
   {

      processor::processor(::text_format_rtf::output * poutput) :
         m_poutput(poutput)
      {


      }

      processor::~processor()
      {


      }


   } // namespace impl


} // namespace text_format_rtf



//
//namespace text_format_rtf
//{
//
//#undef OUT
//
//#define OUT(P) ((output*) P->object)
//
//   class processor_impl
//   {
//
//   }
//
//
//   void
//      destroy_rtf_processor(processor * proc)
//   {
//      // free( proc->object);
//      free(proc);
//   }
//
//
//} // namespace text_format_rtf
//
//
