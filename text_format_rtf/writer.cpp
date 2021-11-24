// Author https://github.com/pangwa/rtfcpp
// ca2-ing by Camilo Sasuke 2019-07-22

#include "framework.h"
#include "_text_format_rtf.h"
#include <iostream>
#include <sstream>


namespace text_format_rtf
{

   int __fwrite(const void * p,int b,int c,output * poutput)
   {

      try
      {

         poutput->write((const char *)p,b * c);

      }
      catch(...)
      {

         return -1;

      }

      return 0;
   }

   writer::writer(output * poutput)
      : m_poutput(poutput)
   {

   }

   writer::~writer()
   {
      if(m_poutput)
      {
         // Write RTF document end part
         std::string rtfText("}");
         __fwrite(rtfText.c_str(),1,rtfText.length(),m_poutput);

         // Close RTF document
         //fclose(m_poutput);
      }
   }


   bool writer::open()
   {

      if(m_poutput == NULL)
      {

         return false;

      }

      // Initialize global params
      init();


      if(!write_header())
      {

         return false;

      }

      if(!write_documentformat())
      {

         return false;

      }

      // Create first RTF document section with default formatting
      write_sectionformat();

      return true;

   }


   ::std::string writer::get_color_table()
   {

      return colors_stack::as_rtf();

   }


   ::std::string writer::get_font_table()
   {

      return fonts_stack::as_rtf();

   }


   // Writes RTF document header
   bool writer::write_header()
   {
      // Set estatus flag
      bool result = true;

      // Standard RTF document header
      std::string rtfText;
      rtfText += "{\\rtf1\\ansi\\ansicpg65001\\deff0{\\fonttbl";
      rtfText += get_font_table();
      rtfText += "}{\\colortbl";
      rtfText += get_color_table();
      rtfText += "}";

      //{\\*\\generator rtflib ver. 1.0;}";
      //rtfText += "\n{\\info{\\author rtflib ver. 1.0}{\\company ETC Company LTD.}}";

      // Writes standard RTF document header part
      if(__fwrite(rtfText.c_str(),1,rtfText.length(),m_poutput) < rtfText.length())
         result = false;

      // Return estatus flag
      return result;
   }

   void writer::init()
   {
      //// Set RTF document default font table
      //m_strFontTable.clear();
      //m_strFontTable += "{\\f0\\froman\\fcharset0\\cpg1252 Times New Roman}";
      //m_strFontTable += "{\\f1\\fswiss\\fcharset0\\cpg1252 Arial}";
      //m_strFontTable += "{\\f2\\fmodern\\fcharset0\\cpg1252 Courier New}";
      //m_strFontTable += "{\\f3\\fscript\\fcharset0\\cpg1252 Cursive}";
      //m_strFontTable += "{\\f4\\fdecor\\fcharset0\\cpg1252 Old English}";
      //m_strFontTable += "{\\f5\\ftech\\fcharset0\\cpg1252 Symbol}";
      //m_strFontTable += "{\\f6\\fbidi\\fcharset0\\cpg1252 Miriam}";

      // Set RTF document default color table
      //m_strColorTable.clear();
      //m_strColorTable += "\\red0\\green0\\blue0;";
      //m_strColorTable += "\\red255\\green0\\blue0;";
      //m_strColorTable += "\\red0\\green255\\blue0;";
      //m_strColorTable += "\\red0\\green0\\blue255;";
      //m_strColorTable += "\\red255\\green255\\blue0;";
      //m_strColorTable += "\\red255\\green0\\blue255;";
      //m_strColorTable += "\\red0\\green255\\blue255;";
      //m_strColorTable += "\\red255\\green255\\blue255;";
      //m_strColorTable += "\\red128\\green0\\blue0;";
      //m_strColorTable += "\\red0\\green128\\blue0;";
      //m_strColorTable += "\\red0\\green0\\blue128;";
      //m_strColorTable += "\\red128\\green128\\blue0;";
      //m_strColorTable += "\\red128\\green0\\blue128;";
      //m_strColorTable += "\\red0\\green128\\blue128;";
      //m_strColorTable += "\\red128\\green128\\blue128;";

      // Set default formatting
      set_defaultformat();
   }

   // Sets default RTF document formatting
   void writer::set_defaultformat()
   {
      // Set default RTF document formatting properties
      RTF_DOCUMENT_FORMAT df ={RTF_DOCUMENTVIEWKIND_PAGE,100,12240,15840,1800,1800,1440,1440,false,0,false};
      set_documentformat(&df);

      // Set default RTF section formatting properties
      RTF_SECTION_FORMAT sf ={RTF_SECTIONBREAK_CONTINUOUS,false,true,12240,15840,1800,1800,1440,1440,0,720,720,false,720,720,false,1,720,false};
      set_sectionformat(&sf);

      // Set default RTF paragraph formatting properties
      RTF_PARAGRAPH_FORMAT pf ={RTF_PARAGRAPHBREAK_NONE,false,true,RTF_PARAGRAPHALIGN_LEFT,0,0,0,0,0,0,"",false,false,false,false,false,false};
      pf.BORDERS.borderColor = 0;
      pf.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
      pf.BORDERS.borderSpace = 0;
      pf.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
      pf.BORDERS.borderWidth = 0;
      pf.CHARACTER.animatedCharacter = false;
      pf.CHARACTER.backgroundColor = 0;
      pf.CHARACTER.boldCharacter = false;
      pf.CHARACTER.capitalCharacter = false;
      pf.CHARACTER.doublestrikeCharacter = false;
      pf.CHARACTER.embossCharacter = false;
      pf.CHARACTER.engraveCharacter = false;
      pf.CHARACTER.expandCharacter = 0;
      pf.CHARACTER.fontNumber = 0;
      pf.CHARACTER.fontSize = 24;
      pf.CHARACTER.foregroundColor = 0;
      pf.CHARACTER.italicCharacter = false;
      pf.CHARACTER.kerningCharacter = 0;
      pf.CHARACTER.outlineCharacter = false;
      pf.CHARACTER.scaleCharacter = 100;
      pf.CHARACTER.shadowCharacter = false;
      pf.CHARACTER.smallcapitalCharacter = false;
      pf.CHARACTER.strikeCharacter = false;
      pf.CHARACTER.subscriptCharacter = false;
      pf.CHARACTER.superscriptCharacter = false;
      pf.CHARACTER.underlineCharacter = 0;
      pf.NUMS.numsChar = char(0x95);
      pf.NUMS.numsLevel = 11;
      pf.NUMS.numsSpace = 360;
      pf.SHADING.shadingBkColor = 0;
      pf.SHADING.shadingFillColor = 0;
      pf.SHADING.shadingIntensity = 0;
      pf.SHADING.shadingType = RTF_PARAGRAPHSHADINGTYPE_FILL;
      pf.TABS.tabKind = RTF_PARAGRAPHTABKIND_NONE;
      pf.TABS.tabLead = RTF_PARAGRAPHTABLEAD_NONE;
      pf.TABS.tabPosition = 0;
      set_paragraphformat(&pf);

      // Set default RTF table row formatting properties
      RTF_TABLEROW_FORMAT rf ={RTF_ROWTEXTALIGN_LEFT,0,0,0,0,0,0};
      set_tablerowformat(&rf);

      // Set default RTF table cell formatting properties
      RTF_TABLECELL_FORMAT cf ={RTF_CELLTEXTALIGN_CENTER,0,0,0,0,RTF_CELLTEXTDIRECTION_LRTB,false};
      cf.SHADING.shadingBkColor = 0;
      cf.SHADING.shadingFillColor = 0;
      cf.SHADING.shadingIntensity = 0;
      cf.SHADING.shadingType = RTF_PARAGRAPHSHADINGTYPE_FILL;
      cf.borderBottom.border = false;
      cf.borderBottom.BORDERS.borderColor = 0;
      cf.borderBottom.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
      cf.borderBottom.BORDERS.borderSpace = 0;
      cf.borderBottom.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
      cf.borderBottom.BORDERS.borderWidth = 5;
      cf.borderLeft.border = false;
      cf.borderLeft.BORDERS.borderColor = 0;
      cf.borderLeft.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
      cf.borderLeft.BORDERS.borderSpace = 0;
      cf.borderLeft.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
      cf.borderLeft.BORDERS.borderWidth = 5;
      cf.borderRight.border = false;
      cf.borderRight.BORDERS.borderColor = 0;
      cf.borderRight.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
      cf.borderRight.BORDERS.borderSpace = 0;
      cf.borderRight.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
      cf.borderRight.BORDERS.borderWidth = 5;
      cf.borderTop.border = false;
      cf.borderTop.BORDERS.borderColor = 0;
      cf.borderTop.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
      cf.borderTop.BORDERS.borderSpace = 0;
      cf.borderTop.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
      cf.borderTop.BORDERS.borderWidth = 5;
      set_tablecellformat(&cf);
   }


   // Sets new RTF document font table
   //void writer::set_fonttable(char * fonts)
   //{
   //   // Clear old font table
   //   m_strFontTable = "";

   //   // Set separator list
   //   char separator[] = ";";

   //   // Create new RTF document font table
   //   int font_number = 0;
   //   char font_table_entry[1024];
   //   char * token = strtok(fonts, separator);
   //   while (token != NULL)
   //   {
   //      // Format font table entry
   //      sprintf(font_table_entry, "{\\f%d\\fnil\\fcharset0\\cpg1252 %s}", font_number, token);
   //      m_strFontTable += font_table_entry;
   //      //strcat( m_strFontTable, font_table_entry );

   //      // Get next font
   //      token = strtok(NULL, separator);
   //      font_number++;
   //   }
   //}


   // Sets new RTF document color table
   //void writer::set_colortable(char * colors)
   //{
   //   // Clear old color table
   //   m_strColorTable.clear();
   //   //strcpy( m_strColorTable, "" );

   //   // Set separator list
   //   char separator[] = ";";

   //   // Create new RTF document color table
   //   int color_number = 0;
   //   char color_table_entry[1024];
   //   char * token = strtok(colors, separator);
   //   while (token != NULL)
   //   {
   //      // Red
   //      sprintf(color_table_entry, "\\red%s", token);
   //      m_strColorTable += color_table_entry;
   //      //strcat( m_strColorTable, color_table_entry );

   //      // Green
   //      token = strtok(NULL, separator);
   //      if (token != NULL)
   //      {
   //         sprintf(color_table_entry, "\\green%s", token);
   //         m_strColorTable += color_table_entry;
   //         //strcat( m_strColorTable, color_table_entry );
   //      }

   //      // Blue
   //      token = strtok(NULL, separator);
   //      if (token != NULL)
   //      {
   //         sprintf(color_table_entry, "\\blue%s;", token);
   //         m_strColorTable += color_table_entry;
   //         //strcat( m_strColorTable, color_table_entry );
   //      }

   //      // Get next color
   //      token = strtok(NULL, separator);
   //      color_number++;
   //   }
   //}


   // Sets RTF document formatting properties
   void writer::set_documentformat(RTF_DOCUMENT_FORMAT * df)
   {
      // Set new RTF document formatting properties
      memcpy(&m_documentformat,df,sizeof(RTF_DOCUMENT_FORMAT));
   }


   // Writes RTF document formatting properties
   bool writer::write_documentformat()
   {
      // Set estatus flag
      bool result = true;

      // RTF document text
      char rtfText[1024];
      strcpy(rtfText,"");

      sprintf(rtfText,"\\viewkind%d\\viewscale%d\\paperw%d\\paperh%d\\margl%d\\margr%d\\margt%d\\margb%d\\gutter%d",
         m_documentformat.viewKind,m_documentformat.viewScale,m_documentformat.paperWidth,m_documentformat.paperHeight,
         m_documentformat.marginLeft,m_documentformat.marginRight,m_documentformat.marginTop,m_documentformat.marginBottom,m_documentformat.gutterWidth);

      if(m_documentformat.facingPages)
         strcat(rtfText,"\\facingp");
      if(m_documentformat.readOnly)
         strcat(rtfText,"\\annotprot");

      // Writes RTF document formatting properties
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText))
         result = false;

      // Return estatus flag
      return result;
   }


   // Sets RTF section formatting properties
   void writer::set_sectionformat(RTF_SECTION_FORMAT * sf)
   {
      // Set new RTF section formatting properties
      memcpy(&m_sectionformat,sf,sizeof(RTF_SECTION_FORMAT));
   }


   // Writes RTF section formatting properties
   bool writer::write_sectionformat()
   {
      // Set estatus flag
      bool result = true;

      // RTF document text
      char rtfText[1024];
      strcpy(rtfText,"");

      // Format new section
      char text[1024] = "",pgn[100] = "";
      if(m_sectionformat.newSection)
         strcat(text,"\\sect");
      if(m_sectionformat.defaultSection)
         strcat(text,"\\sectd");
      if(m_sectionformat.showPageNumber)
      {
         sprintf(pgn,"\\pgnx%d\\pgny%d",m_sectionformat.pageNumberOffsetX,m_sectionformat.pageNumberOffsetY);
         strcat(text,pgn);
      }

      // Format section break
      char sbr[100] = "";
      switch(m_sectionformat.sectionBreak)
      {
         // Continuous break
      case RTF_SECTIONBREAK_CONTINUOUS:
         strcat(sbr,"\\sbknone");
         break;

         // Column break
      case RTF_SECTIONBREAK_COLUMN:
         strcat(sbr,"\\sbkcol");
         break;

         // Page break
      case RTF_SECTIONBREAK_PAGE:
         strcat(sbr,"\\sbkpage");
         break;

         // Even-page break
      case RTF_SECTIONBREAK_EVENPAGE:
         strcat(sbr,"\\sbkeven");
         break;

         // Odd-page break
      case RTF_SECTIONBREAK_ODDPAGE:
         strcat(sbr,"\\sbkodd");
         break;
      }

      // Format section columns
      char cols[100] = "";
      if(m_sectionformat.cols == true)
      {
         // Format columns
         sprintf(cols,"\\cols%d\\colsx%d",m_sectionformat.colsNumber,m_sectionformat.colsDistance);

         if(m_sectionformat.colsLineBetween)
            strcat(cols,"\\linebetcol");
      }

      sprintf(rtfText,"\n%s%s%s\\pgwsxn%d\\pghsxn%d\\marglsxn%d\\margrsxn%d\\margtsxn%d\\margbsxn%d\\guttersxn%d\\headery%d\\footery%d",
         text,sbr,cols,m_sectionformat.pageWidth,m_sectionformat.pageHeight,m_sectionformat.pageMarginLeft,m_sectionformat.pageMarginRight,
         m_sectionformat.pageMarginTop,m_sectionformat.pageMarginBottom,m_sectionformat.pageGutterWidth,m_sectionformat.pageHeaderOffset,m_sectionformat.pageFooterOffset);

      // Writes RTF section formatting properties
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText))
         result = false;

      // Return estatus flag
      return result;
   }


   // Starts new RTF section
   int writer::start_section()
   {
      // Set estatus flag
      int estatus = RTF_SUCCESS;

      // Set new section flag
      m_sectionformat.newSection = true;

      // Starts new RTF section
      if(!writer::write_sectionformat())
         estatus = RTF_SECTIONFORMAT_ERROR;

      // Return estatus flag
      return estatus;
   }


   // Sets RTF paragraph formatting properties
   void writer::set_paragraphformat(RTF_PARAGRAPH_FORMAT * pf)
   {
      // Set new RTF paragraph formatting properties
      memcpy(&m_paragraphformat,pf,sizeof(RTF_PARAGRAPH_FORMAT));
   }


   // Writes RTF paragraph formatting properties
   bool writer::write_paragraphformat()
   {
      // Set estatus flag
      bool result = true;

      // RTF document text
      char rtfText[1024 * 6];
      strcpy(rtfText,"");

      // Format new paragraph
      char text[1024 * 3] = "";
      if(m_paragraphformat.newParagraph)
         strcat(text,"\\par");
      if(m_paragraphformat.defaultParagraph)
         strcat(text,"\\pard");
      if(m_paragraphformat.tableText)
      {
         strcat(text,"\\intbl");
      }
      else if(m_paragraphformat.defaultParagraph || m_paragraphformat.newParagraph)
      {
         strcat(text,"\\plain");
      }

      switch(m_paragraphformat.paragraphBreak)
      {
         // No break
      case RTF_PARAGRAPHBREAK_NONE:
         break;

         // Page break;
      case RTF_PARAGRAPHBREAK_PAGE:
         strcat(text,"\\page");
         break;

         // Column break;
      case RTF_PARAGRAPHBREAK_COLUMN:
         strcat(text,"\\column");
         break;

         // Line break;
      case RTF_PARAGRAPHBREAK_LINE:
         strcat(text,"\\line");
         break;
      }

      // Format aligment
      switch(m_paragraphformat.paragraphAligment)
      {
         // Left aligned paragraph
      case RTF_PARAGRAPHALIGN_LEFT:
         strcat(text,"\\ql");
         break;

         // Center aligned paragraph
      case RTF_PARAGRAPHALIGN_CENTER:
         strcat(text,"\\qc");
         break;

         // Right aligned paragraph
      case RTF_PARAGRAPHALIGN_RIGHT:
         strcat(text,"\\qr");
         break;

         // Justified aligned paragraph
      case RTF_PARAGRAPHALIGN_JUSTIFY:
         strcat(text,"\\qj");
         break;
      }

      // Format tabs
      if(m_paragraphformat.paragraphTabs == true)
      {
         // Set tab kind
         switch(m_paragraphformat.TABS.tabKind)
         {
            // No tab
         case RTF_PARAGRAPHTABKIND_NONE:
            break;

            // Centered tab
         case RTF_PARAGRAPHTABKIND_CENTER:
            strcat(text,"\\tqc");
            break;

            // Flush-right tab
         case RTF_PARAGRAPHTABKIND_RIGHT:
            strcat(text,"\\tqr");
            break;

            // Decimal tab
         case RTF_PARAGRAPHTABKIND_DECIMAL:
            strcat(text,"\\tqdec");
            break;
         }

         // Set tab leader
         switch(m_paragraphformat.TABS.tabLead)
         {
            // No lead
         case RTF_PARAGRAPHTABLEAD_NONE:
            break;

            // Leader dots
         case RTF_PARAGRAPHTABLEAD_DOT:
            strcat(text,"\\tldot");
            break;

            // Leader middle dots
         case RTF_PARAGRAPHTABLEAD_MDOT:
            strcat(text,"\\tlmdot");
            break;

            // Leader hyphens
         case RTF_PARAGRAPHTABLEAD_HYPH:
            strcat(text,"\\tlhyph");
            break;

            // Leader underline
         case RTF_PARAGRAPHTABLEAD_UNDERLINE:
            strcat(text,"\\tlul");
            break;

            // Leader thick line
         case RTF_PARAGRAPHTABLEAD_THICKLINE:
            strcat(text,"\\tlth");
            break;

            // Leader equal sign
         case RTF_PARAGRAPHTABLEAD_EQUAL:
            strcat(text,"\\tleq");
            break;
         }

         // Set tab position
         char tb[32];
         sprintf(tb,"\\tx%d",m_paragraphformat.TABS.tabPosition);
         strcat(text,tb);
      }

      // Format bullets and numbering
      if(m_paragraphformat.paragraphNums == true)
      {
         char nums[1024];
         sprintf(nums,"{\\*\\pn\\pnlvl%d\\pnsp%d\\pntxtb %c}",m_paragraphformat.NUMS.numsLevel,m_paragraphformat.NUMS.numsSpace,m_paragraphformat.NUMS.numsChar);
         strcat(text,nums);
      }

      // Format paragraph borders
      if(m_paragraphformat.paragraphBorders == true)
      {
         char border[1024] = "";

         // Format paragraph border kind
         switch(m_paragraphformat.BORDERS.borderKind)
         {
            // No border
         case RTF_PARAGRAPHBORDERKIND_NONE:
            break;

            // Border top
         case RTF_PARAGRAPHBORDERKIND_TOP:
            strcat(border,"\\brdrt");
            break;

            // Border bottom
         case RTF_PARAGRAPHBORDERKIND_BOTTOM:
            strcat(border,"\\brdrb");
            break;

            // Border left
         case RTF_PARAGRAPHBORDERKIND_LEFT:
            strcat(border,"\\brdrl");
            break;

            // Border right
         case RTF_PARAGRAPHBORDERKIND_RIGHT:
            strcat(border,"\\brdrr");
            break;

            // Border box
         case RTF_PARAGRAPHBORDERKIND_BOX:
            strcat(border,"\\box");
            break;
         }

         // Format paragraph border type
         char * br = writer::get_bordername(m_paragraphformat.BORDERS.borderType);
         strcat(border,br);

         // Set paragraph border width
         char brd[100];
         sprintf(brd,"\\brdrw%d\\brsp%d",m_paragraphformat.BORDERS.borderWidth,m_paragraphformat.BORDERS.borderSpace);
         strcat(border,brd);
         strcat(text,border);

         // Set paragraph border color
         char brdcol[100];
         sprintf(brdcol,"\\brdrcf%d",m_paragraphformat.BORDERS.borderColor);
         strcat(text,brdcol);
      }

      // Format paragraph shading
      if(m_paragraphformat.paragraphShading == true)
      {
         char shading[100];
         sprintf(shading,"\\shading%d",m_paragraphformat.SHADING.shadingIntensity);

         // Format paragraph shading
         char * sh = writer::get_shadingname(m_paragraphformat.SHADING.shadingType,false);
         strcat(text,sh);

         // Set paragraph shading color
         char shcol[100];
         sprintf(shcol,"\\cfpat%d\\cbpat%d",m_paragraphformat.SHADING.shadingFillColor,m_paragraphformat.SHADING.shadingBkColor);
         strcat(text,shcol);
      }

      // Format paragraph font
      char font[1024] = "";
      char part[256] = "";
      //sprintf(font,"\\animtext%d\\expndtw%d\\kerning%d\\charscalex%d\\f%d\\fs%d\\cf%d",m_paragraphformat.CHARACTER.animatedCharacter,
        // m_paragraphformat.CHARACTER.expandCharacter,m_paragraphformat.CHARACTER.kerningCharacter,m_paragraphformat.CHARACTER.scaleCharacter,
         //m_paragraphformat.CHARACTER.fontNumber,m_paragraphformat.CHARACTER.fontSize,m_paragraphformat.CHARACTER.foregroundColor);

      if(m_paragraphformat.CHARACTER.animatedCharacter)
      {
         sprintf(part,"\\animtext%d", m_paragraphformat.CHARACTER.animatedCharacter);
         strcat(font,part);
      }
      if(m_paragraphformat.CHARACTER.expandCharacter)
      {
         sprintf(part,"\\expndtw%d",m_paragraphformat.CHARACTER.expandCharacter);
         strcat(font,part);
      }
      if(m_paragraphformat.CHARACTER.kerningCharacter)
      {
         sprintf(part,"\\kerning%d",m_paragraphformat.CHARACTER.kerningCharacter);
         strcat(font,part);
      }
      if(m_paragraphformat.CHARACTER.scaleCharacter)
      {
         sprintf(part,"\\charscalex%d",m_paragraphformat.CHARACTER.scaleCharacter);
         strcat(font,part);
      }
      sprintf(part,"\\f%d\\fs%d\\cf%d",
        m_paragraphformat.CHARACTER.fontNumber,m_paragraphformat.CHARACTER.fontSize,m_paragraphformat.CHARACTER.foregroundColor);
      strcat(font,part);

      if(m_paragraphformat.CHARACTER.boldCharacter)
         strcat(font,"\\b");
      //else
        // strcat(font,"\\b0");
      if(m_paragraphformat.CHARACTER.capitalCharacter)
         strcat(font,"\\caps");
      //else
        // strcat(font,"\\caps0");
      if(m_paragraphformat.CHARACTER.doublestrikeCharacter)
         strcat(font,"\\striked1");
      //else
        // strcat(font,"\\striked0");
      if(m_paragraphformat.CHARACTER.embossCharacter)
         strcat(font,"\\embo");
      if(m_paragraphformat.CHARACTER.engraveCharacter)
         strcat(font,"\\impr");
      if(m_paragraphformat.CHARACTER.italicCharacter)
         strcat(font,"\\i");
      //else
        // strcat(font,"\\i0");
      if(m_paragraphformat.CHARACTER.outlineCharacter)
         strcat(font,"\\outl");
      //else
         //strcat(font,"\\outl0");
      if(m_paragraphformat.CHARACTER.shadowCharacter)
         strcat(font,"\\shad");
      //else
        // strcat(font,"\\shad0");
      if(m_paragraphformat.CHARACTER.smallcapitalCharacter)
         strcat(font,"\\scaps");
//      else
  //       strcat(font,"\\scaps0");
      if(m_paragraphformat.CHARACTER.strikeCharacter)
         strcat(font,"\\strike");
    //  else
      //   strcat(font,"\\strike0");
      if(m_paragraphformat.CHARACTER.subscriptCharacter)
         strcat(font,"\\sub");
      if(m_paragraphformat.CHARACTER.superscriptCharacter)
         strcat(font,"\\super");

      switch(m_paragraphformat.CHARACTER.underlineCharacter)
      {
         // None underline
      case 0:
         //strcat(font,"\\ulnone");
         break;

         // Continuous underline
      case 1:
         strcat(font,"\\ul");
         break;

         // Dotted underline
      case 2:
         strcat(font,"\\uld");
         break;

         // Dashed underline
      case 3:
         strcat(font,"\\uldash");
         break;

         // Dash-dotted underline
      case 4:
         strcat(font,"\\uldashd");
         break;

         // Dash-dot-dotted underline
      case 5:
         strcat(font,"\\uldashdd");
         break;

         // Double underline
      case 6:
         strcat(font,"\\uldb");
         break;

         // Heavy wave underline
      case 7:
         strcat(font,"\\ulhwave");
         break;

         // Long dashed underline
      case 8:
         strcat(font,"\\ulldash");
         break;

         // Thick underline
      case 9:
         strcat(font,"\\ulth");
         break;

         // Thick dotted underline
      case 10:
         strcat(font,"\\ulthd");
         break;

         // Thick dashed underline
      case 11:
         strcat(font,"\\ulthdash");
         break;

         // Thick dash-dotted underline
      case 12:
         strcat(font,"\\ulthdashd");
         break;

         // Thick dash-dot-dotted underline
      case 13:
         strcat(font,"\\ulthdashdd");
         break;

         // Thick long dashed underline
      case 14:
         strcat(font,"\\ulthldash");
         break;

         // Double wave underline
      case 15:
         strcat(font,"\\ululdbwave");
         break;

         // Word underline
      case 16:
         strcat(font,"\\ulw");
         break;

         // Wave underline
      case 17:
         strcat(font,"\\ulwave");
         break;
      }

      char txt[20] = "";

      std::string strText;

      bool bMultiByteChar = false;
      const char * p = m_paragraphformat.paragraphText;
      while (*p)
      {
         if (*p & 0x80)
         {
            bMultiByteChar = true;
            break;

         }
         p++;
      }

      //if (fontcharset(m_paragraphformat.CHARACTER.fontNumber) == 128)
      if (bMultiByteChar)
      {
         const char * p = m_paragraphformat.paragraphText;
         while (*p)
         {
            strText += "\\'";
            char sz[32];
            int i = *(unsigned char *)p;
            sprintf(sz, "%02x", i);
               strText += sz;
               p++;
         }
         
      }
      else
      {

         strText = m_paragraphformat.paragraphText;
      }


      // Set paragraph tabbed text
      if(m_paragraphformat.tabbedText == false)
      {
         sprintf(rtfText,"\n%s\\fi%d\\li%d\\ri%d\\sb%d\\sa%d\\sl%d%s %s",text,
            m_paragraphformat.firstLineIndent,m_paragraphformat.leftIndent,m_paragraphformat.rightIndent,m_paragraphformat.spaceBefore,
            m_paragraphformat.spaceAfter,m_paragraphformat.dLineSpacing,font, strText.c_str());
      }
      else
         sprintf(rtfText,"\\tab %s", strText.c_str());

      // Writes RTF paragraph formatting properties
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText) == 0)
         result = false;

      // Return estatus flag
      return result;
   }


   // Starts new RTF paragraph
   int writer::start_paragraph(const char * text,bool newPar)
   {
      // Set estatus flag
      int estatus = RTF_SUCCESS;

      // Copy paragraph text
      m_paragraphformat.paragraphText = new char[strlen(text) * 2];
      strcpy(m_paragraphformat.paragraphText,text);

      // Set new paragraph
      m_paragraphformat.newParagraph = newPar;

      // Starts new RTF paragraph
      if(!writer::write_paragraphformat())
         estatus = RTF_PARAGRAPHFORMAT_ERROR;

      // Return estatus flag
      return estatus;
   }


   // Gets RTF document formatting properties
   RTF_DOCUMENT_FORMAT * writer::get_documentformat()
   {
      // Get current RTF document formatting properties
      return &m_documentformat;
   }

   // Gets RTF section formatting properties
   RTF_SECTION_FORMAT * writer::get_sectionformat()
   {
      // Get current RTF section formatting properties
      return &m_sectionformat;
   }


   // Gets RTF paragraph formatting properties
   RTF_PARAGRAPH_FORMAT * writer::get_paragraphformat()
   {
      // Get current RTF paragraph formatting properties
      return &m_paragraphformat;
   }


   // Loads image from file
   int writer::load_image(char * image,int width,int height)
   {
      return -1;
   }


   // Converts binary data to hex
   char * writer::bin_hex_convert(unsigned char * binary,int size)
   {
      char * result = new char[2 * size];

      char part1,part2;
      for(int i = 0; i < size; i++)
      {
         part1 = binary[i] / 16;
         if(part1 < 10)
            part1 += 48;
         else
         {
            if(part1 == 10)
               part1 = 'a';
            if(part1 == 11)
               part1 = 'b';
            if(part1 == 12)
               part1 = 'c';
            if(part1 == 13)
               part1 = 'd';
            if(part1 == 14)
               part1 = 'e';
            if(part1 == 15)
               part1 = 'f';
         }

         part2 = binary[i] % 16;
         if(part2 < 10)
            part2 += 48;
         else
         {
            if(part2 == 10)
               part2 = 'a';
            if(part2 == 11)
               part2 = 'b';
            if(part2 == 12)
               part2 = 'c';
            if(part2 == 13)
               part2 = 'd';
            if(part2 == 14)
               part2 = 'e';
            if(part2 == 15)
               part2 = 'f';
         }

         result[2 * i] = part1;
         result[2 * i + 1] = part2;
      }

      strcat(result,"\0");

      return result;
   }


   // Starts new RTF table row
   int writer::start_tablerow()
   {
      // Set estatus flag
      int estatus = RTF_SUCCESS;

      char tblrw[1024] = "";
      // Format table row aligment
      switch(m_tablerowformat.rowAligment)
      {
         // Left align
      case RTF_ROWTEXTALIGN_LEFT:
         strcat(tblrw,"\\trql");
         break;

         // Center align
      case RTF_ROWTEXTALIGN_CENTER:
         strcat(tblrw,"\\trqc");
         break;

         // Right align
      case RTF_ROWTEXTALIGN_RIGHT:
         strcat(tblrw,"\\trqr");
         break;
      }

      // Writes RTF table data
      char rtfText[1024] = "";
      sprintf(rtfText,"\n\\trowd\\trgaph115%s\\trleft%d\\trrh%d\\trpaddb%d\\trpaddfb3\\trpaddl%d\\trpaddfl3\\trpaddr%d\\trpaddfr3\\trpaddt%d\\trpaddft3",
         tblrw,m_tablerowformat.rowLeftMargin,m_tablerowformat.rowHeight,m_tablerowformat.marginTop,m_tablerowformat.marginBottom,m_tablerowformat.marginLeft,m_tablerowformat.marginRight);
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText))
         estatus = RTF_TABLE_ERROR;

      // Return estatus flag
      return estatus;
   }


   // Ends RTF table row
   int writer::end_tablerow()
   {
      // Set estatus flag
      int estatus = RTF_SUCCESS;

      // Writes RTF table data
      char rtfText[1024];
      sprintf(rtfText,"\n\\trgaph115\\row\\pard");
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText))
         estatus = RTF_TABLE_ERROR;

      // Return estatus flag
      return estatus;
   }


   // Starts new RTF table cell
   int writer::start_tablecell(int rightMargin)
   {
      // Set estatus flag
      int estatus = RTF_SUCCESS;

      char tblcla[20] = "";
      // Format table cell text aligment
      switch(m_tablecellformat.textVerticalAligment)
      {
         // Top align
      case RTF_CELLTEXTALIGN_TOP:
         strcpy(tblcla,"\\clvertalt");
         break;

         // Center align
      case RTF_CELLTEXTALIGN_CENTER:
         strcpy(tblcla,"\\clvertalc");
         break;

         // Bottom align
      case RTF_CELLTEXTALIGN_BOTTOM:
         strcpy(tblcla,"\\clvertalb");
         break;
      }

      char tblcld[20];
      // Format table cell text direction
      switch(m_tablecellformat.textDirection)
      {
         // Left to right, top to bottom
      case RTF_CELLTEXTDIRECTION_LRTB:
         strcpy(tblcld,"\\cltxlrtb");
         break;

         // Right to left, top to bottom
      case RTF_CELLTEXTDIRECTION_RLTB:
         strcpy(tblcld,"\\cltxtbrl");
         break;

         // Left to right, bottom to top
      case RTF_CELLTEXTDIRECTION_LRBT:
         strcpy(tblcld,"\\cltxbtlr");
         break;

         // Left to right, top to bottom, vertical
      case RTF_CELLTEXTDIRECTION_LRTBV:
         strcpy(tblcld,"\\cltxlrtbv");
         break;

         // Right to left, top to bottom, vertical
      case RTF_CELLTEXTDIRECTION_RLTBV:
         strcpy(tblcld,"\\cltxtbrlv");
         break;
      }

      char tbclbrb[1024] = "",tbclbrl[1024] = "",tbclbrr[1024] = "",tbclbrt[1024] = "";
      // Format table cell border
      if(m_tablecellformat.borderBottom.border == true)
      {
         // Bottom cell border
         char tbclbt[20];
         strcpy(tbclbt,"\\clbrdrb");

         char * border = writer::get_bordername(m_tablecellformat.borderBottom.BORDERS.borderType);

         sprintf(tbclbrb,"%s%s\\brdrw%d\\brsp%d\\brdrcf%d",tbclbt,border,m_tablecellformat.borderBottom.BORDERS.borderWidth,
            m_tablecellformat.borderBottom.BORDERS.borderSpace,m_tablecellformat.borderBottom.BORDERS.borderColor);
      }
      if(m_tablecellformat.borderLeft.border == true)
      {
         // Left cell border
         char tbclbt[20] = "";
         strcpy(tbclbt,"\\clbrdrl");

         char * border = writer::get_bordername(m_tablecellformat.borderLeft.BORDERS.borderType);

         sprintf(tbclbrl,"%s%s\\brdrw%d\\brsp%d\\brdrcf%d",tbclbt,border,m_tablecellformat.borderLeft.BORDERS.borderWidth,
            m_tablecellformat.borderLeft.BORDERS.borderSpace,m_tablecellformat.borderLeft.BORDERS.borderColor);
      }
      if(m_tablecellformat.borderRight.border == true)
      {
         // Right cell border
         char tbclbt[20];
         strcpy(tbclbt,"\\clbrdrr");

         char * border = writer::get_bordername(m_tablecellformat.borderRight.BORDERS.borderType);

         sprintf(tbclbrr,"%s%s\\brdrw%d\\brsp%d\\brdrcf%d",tbclbt,border,m_tablecellformat.borderRight.BORDERS.borderWidth,
            m_tablecellformat.borderRight.BORDERS.borderSpace,m_tablecellformat.borderRight.BORDERS.borderColor);
      }
      if(m_tablecellformat.borderTop.border == true)
      {
         // Top cell border
         char tbclbt[20];
         strcpy(tbclbt,"\\clbrdrt");

         char * border = writer::get_bordername(m_tablecellformat.borderTop.BORDERS.borderType);

         sprintf(tbclbrt,"%s%s\\brdrw%d\\brsp%d\\brdrcf%d",tbclbt,border,m_tablecellformat.borderTop.BORDERS.borderWidth,
            m_tablecellformat.borderTop.BORDERS.borderSpace,m_tablecellformat.borderTop.BORDERS.borderColor);
      }

      // Format table cell shading
      char shading[100] = "";
      if(m_tablecellformat.cellShading == true)
      {
         char * sh = writer::get_shadingname(m_tablecellformat.SHADING.shadingType,true);

         // Set paragraph shading color
         sprintf(shading,"%s\\clshdgn%d\\clcfpat%d\\clcbpat%d",sh,m_tablecellformat.SHADING.shadingIntensity,m_tablecellformat.SHADING.shadingFillColor,m_tablecellformat.SHADING.shadingBkColor);
      }

      // Writes RTF table data
      char rtfText[1024];
      sprintf(rtfText,"\n\\tcelld%s%s%s%s%s%s%s\\cellx%d",tblcla,tblcld,tbclbrb,tbclbrl,tbclbrr,tbclbrt,shading,rightMargin);
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText))
         estatus = RTF_TABLE_ERROR;

      // Return estatus flag
      return estatus;
   }


   // Ends RTF table cell
   int writer::end_tablecell()
   {
      // Set estatus flag
      int estatus = RTF_SUCCESS;

      // Writes RTF table data
      char rtfText[1024];
      strcpy(rtfText,"\n\\cell ");
      if(__fwrite(rtfText,1,strlen(rtfText),m_poutput) < strlen(rtfText))
         estatus = RTF_TABLE_ERROR;

      // Return estatus flag
      return estatus;
   }


   // Gets RTF table row formatting properties
   RTF_TABLEROW_FORMAT * writer::get_tablerowformat()
   {
      // Get current RTF table row formatting properties
      return &m_tablerowformat;
   }


   // Sets RTF table row formatting properties
   void writer::set_tablerowformat(RTF_TABLEROW_FORMAT * rf)
   {
      // Set new RTF table row formatting properties
      memcpy(&m_tablerowformat,rf,sizeof(RTF_TABLEROW_FORMAT));
   }


   // Gets RTF table cell formatting properties
   RTF_TABLECELL_FORMAT * writer::get_tablecellformat()
   {
      // Get current RTF table cell formatting properties
      return &m_tablecellformat;
   }


   // Sets RTF table cell formatting properties
   void writer::set_tablecellformat(RTF_TABLECELL_FORMAT * cf)
   {
      // Set new RTF table cell formatting properties
      memcpy(&m_tablecellformat,cf,sizeof(RTF_TABLECELL_FORMAT));
   }


   // Gets border name
   char * writer::get_bordername(int border_type)
   {
      char * border = new char[20];

      switch(border_type)
      {
         // Single-thickness border
      case RTF_PARAGRAPHBORDERTYPE_STHICK:
         strcpy(border,"\\brdrs");
         break;

         // Double-thickness border
      case RTF_PARAGRAPHBORDERTYPE_DTHICK:
         strcpy(border,"\\brdrth");
         break;

         // Shadowed border
      case RTF_PARAGRAPHBORDERTYPE_SHADOW:
         strcpy(border,"\\brdrsh");
         break;

         // Double border
      case RTF_PARAGRAPHBORDERTYPE_DOUBLE:
         strcpy(border,"\\brdrdb");
         break;

         // Dotted border
      case RTF_PARAGRAPHBORDERTYPE_DOT:
         strcpy(border,"\\brdrdot");
         break;

         // Dashed border
      case RTF_PARAGRAPHBORDERTYPE_DASH:
         strcpy(border,"\\brdrdash");
         break;

         // Hairline border
      case RTF_PARAGRAPHBORDERTYPE_HAIRLINE:
         strcpy(border,"\\brdrhair");
         break;

         // Inset border
      case RTF_PARAGRAPHBORDERTYPE_INSET:
         strcpy(border,"\\brdrinset");
         break;

         // Dashed border (small)
      case RTF_PARAGRAPHBORDERTYPE_SDASH:
         strcpy(border,"\\brdrdashsm");
         break;

         // Dot-dashed border
      case RTF_PARAGRAPHBORDERTYPE_DOTDASH:
         strcpy(border,"\\brdrdashd");
         break;

         // Dot-dot-dashed border
      case RTF_PARAGRAPHBORDERTYPE_DOTDOTDASH:
         strcpy(border,"\\brdrdashdd");
         break;

         // Outset border
      case RTF_PARAGRAPHBORDERTYPE_OUTSET:
         strcpy(border,"\\brdroutset");
         break;

         // Triple border
      case RTF_PARAGRAPHBORDERTYPE_TRIPLE:
         strcpy(border,"\\brdrtriple");
         break;

         // Wavy border
      case RTF_PARAGRAPHBORDERTYPE_WAVY:
         strcpy(border,"\\brdrwavy");
         break;

         // Double wavy border
      case RTF_PARAGRAPHBORDERTYPE_DWAVY:
         strcpy(border,"\\brdrwavydb");
         break;

         // Striped border
      case RTF_PARAGRAPHBORDERTYPE_STRIPED:
         strcpy(border,"\\brdrdashdotstr");
         break;

         // Embossed border
      case RTF_PARAGRAPHBORDERTYPE_EMBOSS:
         strcpy(border,"\\brdremboss");
         break;

         // Engraved border
      case RTF_PARAGRAPHBORDERTYPE_ENGRAVE:
         strcpy(border,"\\brdrengrave");
         break;
      }

      return border;
   }


   // Gets shading name
   char * writer::get_shadingname(int shading_type,bool cell)
   {
      char * shading = new char[20];

      if(cell == false)
      {
         switch(shading_type)
         {
            // Fill shading
         case RTF_PARAGRAPHSHADINGTYPE_FILL:
            strcpy(shading,"");
            break;

            // Horizontal background pattern
         case RTF_PARAGRAPHSHADINGTYPE_HORIZ:
            strcpy(shading,"\\bghoriz");
            break;

            // Vertical background pattern
         case RTF_PARAGRAPHSHADINGTYPE_VERT:
            strcpy(shading,"\\bgvert");
            break;

            // Forward diagonal background pattern
         case RTF_PARAGRAPHSHADINGTYPE_FDIAG:
            strcpy(shading,"\\bgfdiag");
            break;

            // Backward diagonal background pattern
         case RTF_PARAGRAPHSHADINGTYPE_BDIAG:
            strcpy(shading,"\\bgbdiag");
            break;

            // Cross background pattern
         case RTF_PARAGRAPHSHADINGTYPE_CROSS:
            strcpy(shading,"\\bgcross");
            break;

            // Diagonal cross background pattern
         case RTF_PARAGRAPHSHADINGTYPE_CROSSD:
            strcpy(shading,"\\bgdcross");
            break;

            // Dark horizontal background pattern
         case RTF_PARAGRAPHSHADINGTYPE_DHORIZ:
            strcpy(shading,"\\bgdkhoriz");
            break;

            // Dark vertical background pattern
         case RTF_PARAGRAPHSHADINGTYPE_DVERT:
            strcpy(shading,"\\bgdkvert");
            break;

            // Dark forward diagonal background pattern
         case RTF_PARAGRAPHSHADINGTYPE_DFDIAG:
            strcpy(shading,"\\bgdkfdiag");
            break;

            // Dark backward diagonal background pattern
         case RTF_PARAGRAPHSHADINGTYPE_DBDIAG:
            strcpy(shading,"\\bgdkbdiag");
            break;

            // Dark cross background pattern
         case RTF_PARAGRAPHSHADINGTYPE_DCROSS:
            strcpy(shading,"\\bgdkcross");
            break;

            // Dark diagonal cross background pattern
         case RTF_PARAGRAPHSHADINGTYPE_DCROSSD:
            strcpy(shading,"\\bgdkdcross");
            break;
         }
      }
      else
      {
         switch(shading_type)
         {
            // Fill shading
         case RTF_CELLSHADINGTYPE_FILL:
            strcpy(shading,"");
            break;

            // Horizontal background pattern
         case RTF_CELLSHADINGTYPE_HORIZ:
            strcpy(shading,"\\clbghoriz");
            break;

            // Vertical background pattern
         case RTF_CELLSHADINGTYPE_VERT:
            strcpy(shading,"\\clbgvert");
            break;

            // Forward diagonal background pattern
         case RTF_CELLSHADINGTYPE_FDIAG:
            strcpy(shading,"\\clbgfdiag");
            break;

            // Backward diagonal background pattern
         case RTF_CELLSHADINGTYPE_BDIAG:
            strcpy(shading,"\\clbgbdiag");
            break;

            // Cross background pattern
         case RTF_CELLSHADINGTYPE_CROSS:
            strcpy(shading,"\\clbgcross");
            break;

            // Diagonal cross background pattern
         case RTF_CELLSHADINGTYPE_CROSSD:
            strcpy(shading,"\\clbgdcross");
            break;

            // Dark horizontal background pattern
         case RTF_CELLSHADINGTYPE_DHORIZ:
            strcpy(shading,"\\clbgdkhoriz");
            break;

            // Dark vertical background pattern
         case RTF_CELLSHADINGTYPE_DVERT:
            strcpy(shading,"\\clbgdkvert");
            break;

            // Dark forward diagonal background pattern
         case RTF_CELLSHADINGTYPE_DFDIAG:
            strcpy(shading,"\\clbgdkfdiag");
            break;

            // Dark backward diagonal background pattern
         case RTF_CELLSHADINGTYPE_DBDIAG:
            strcpy(shading,"\\clbgdkbdiag");
            break;

            // Dark cross background pattern
         case RTF_CELLSHADINGTYPE_DCROSS:
            strcpy(shading,"\\clbgdkcross");
            break;

            // Dark diagonal cross background pattern
         case RTF_CELLSHADINGTYPE_DCROSSD:
            strcpy(shading,"\\clbgdkdcross");
            break;
         }
      }

      return shading;
   }


   std::string         writer::encodeWString(const std::wstring & str)
   {
      std::ostringstream sstr;
      for(size_t i = 0; i < str.length(); i++)
      {
         if(str[i] <= 0x7f)
            sstr << (char)str[i];
         else
            sstr << "\\u" << str[i] << "?";//\uN and append the ansi representation of this char
      }
      return sstr.str();
   }


} // namespace text_format_rtf


