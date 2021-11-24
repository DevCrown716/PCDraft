//    LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//    ca2-ing by Camilo Sasuke 2019-07-22
//
//
//    Definitions of C++ interface classes for librtf.
//
//    Autors :
//       Tomek : Tomasz Hławiczka : tomek /at/ tru /./ pl
//
//    Changes :
//	   28 Sep 2006, Tomek: Implementation.
//	   3  Oct 2006, Tomek: StyleSheet support.
//

#pragma once

#define  _CRT_SECURE_NO_WARNINGS
#include <string>
#include <istream>
#include <map>
#include <vector>
#include <list>
#include <stack>
#include <set>


namespace text_format_rtf
{

   class image_manager;
   namespace impl
   {

      class processor;
   } // namespace impl

} // namespace text_format_rtf



#include "const.h"
#include "image_descriptor.h"
#include "text_style.h"

#include "processor_base.h"

#include "observer.h"
#include "source.h"
#include "output.h"

#include "fonts.h"

#include "parser.h"

#include "image_manager.h"
#include "codepage_info.h"



#include "attribute.h"
#include "attributes.h"
#include "attributes_stack.h"

#include "colors.h"
#include "colors_stack.h"

#include "fonts_stack.h"



#include "std_source.h"
#include "std_logger.h"
#include "std_output.h"

#include "processor.h"



#include "impl_source.h"
#include "impl_observer.h"
#include "impl_processor.h"

#include "impl_processor.h"


#include "writer_defs.h"

#include "writer.h"


extern const char * std_fonts[];


//
//#include <ctype.h>
//#include <stdio.h>
//#include <stdlib.h>


//
//namespace text_format_rtf
//{
//
//
//
//   // Function for reading rtf source.
//   //extern void         rtf_process(source * src, processor * proc, observer * log);
//
//   //// Functions for creating rtf output.

//
//   //CLASS_DECL_TEXT_FORMAT_RTF void         destroy_rtf_processor(processor * proc);
//
//
//
//
//
//
//
//      //class processor;
//      //class source;
//      //class observer;
//
//      //namespace parsers
//      //{
//      //   class rtf
//      //   {
//      //   public:
//      //      explicit rtf(source & src, processor & proc, observer & log);
//
//      //   protected:
//      //      void exec(source & src, processor & proc, observer & log);
//      //   };
//      //}
//
//
//      //struct image_descriptor
//      //{
//      //   enum types {
//      //      unknown = 0, //PICT_UNKNOWN,
//      //      wm = 1, //PICT_WM,
//      //      mac = 2, //PICT_MAC,
//      //      pm = 3, //PICT_PM,
//      //      di = 4, //PICT_DI,
//      //      wb = 5, //PICT_WB,
//      //      jpeg = 6, //PICT_JPEG,
//      //      png = 7, //PICT_PNG,
//      //   };
//      //   int width;
//      //   int height;
//      //   int depth;
//      //   types type;
//      //   int wmetafile_type;
//      //};
//
//
//} // namespace text_format_rtf
//
//
//
//
//
//



//CLASS_DECL_TEXT_FORMAT_RTF __pointer(::text_format_rtf::processor)  create_rtf_processor(::text_format_rtf::output * poutput);