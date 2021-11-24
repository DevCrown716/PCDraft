//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//
#pragma once

#include <stdio.h> // for size_t


namespace text_format_rtf
{



#define SKIP_ONE_WORD	2

#define CHECK_PARAM_NOT_NULL(x) // FIXME: implementation needed

#ifdef WINDOWS
#define snprintf _snprintf
#endif 




typedef struct {
   int             type;

   int             has_param;
   int             param;

   ::std::string          content;


   void append(const char * psz);
   void append(char ch );

} rtf_part;

enum
{
	PART_TYPE_TEXT,
	PART_TYPE_BLOCK_START,
	PART_TYPE_BLOCK_END,
	PART_TYPE_COMMAND,
	PART_TYPE_HEX_CHAR,
};


typedef struct
{

} internal_processor;



} //  namespace text_format_rtf


