#include "StdAfx.h"
#include "LoadRTF.h"
#include "MDText.h"
#include <strstream>

bool is_ansi(const char *psz)
{

   while (*psz)
   {
      if (*psz < 0)
      {

         return false;

      }
         
         psz++;
   }

   return true;

}

CString get_font_english_name(CString str);
std::string trim_right(const char * psz,const char * pszTrim)
{

   CString str(psz);

   str.TrimRight(pszTrim);

   return str;

}

void debug_append_to_file(const char * psz)
{
   CFile f;
   if(f.Open(_T("C:\\archive\\test.txt"),CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
   {
      f.SeekToEnd();
      f.Write(psz, strlen(psz));
   }
   f.Close();
}


md_text_rtf_processor::md_text_rtf_processor()
{

   memset(&m_style,0,sizeof(m_style));
   m_bMacMode = true;
   m_iCodePage = 0;

   m_style.m_LogFont.lfWeight = FW_NORMAL;

}


md_text_rtf_processor::~md_text_rtf_processor()
{
   
   //m_ptext->SetText(m_styledtext);

}


   void md_text_rtf_processor::enable_attr(int attr,int param,bool enable)
   {

   }

   void md_text_rtf_processor::begin()
   {
   }
   void md_text_rtf_processor::end()
   {

   }
   void md_text_rtf_processor::title(::std::string title)
   {

   }
   void md_text_rtf_processor::keywords(::std::string keywords)
   {
   }
   void md_text_rtf_processor::author(::std::string author)
   {
   }
   void md_text_rtf_processor::summary(::text_format_rtf::e_document_summary cat,int param)
   {
   }

   static int CALLBACK EnumFontFamExProcIsInstalled(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* /*lpntme*/, int /*FontType*/, LPARAM lParam)
   {
      LPARAM* l = (LPARAM*)lParam;
      *l = TRUE;
      return TRUE;
   }



   bool IsFontInstalled(LPCTSTR lpszFont, int iCharSet)
   {


      CString strFont = get_font_english_name(lpszFont);


      // Get the screen DC
      CDC dc;
      if (!dc.CreateCompatibleDC(NULL))
      {
         
         return false;

      }
      
      LOGFONT lf = { 0 };
      // Any character set will do
      lf.lfCharSet = iCharSet;

      // Set the facename to check for
      strncpy(lf.lfFaceName, strFont, sizeof(lf.lfFaceName));
      
      LPARAM lParam = 0;
      // Enumerate fonts
      ::EnumFontFamiliesEx(dc.GetSafeHdc(), NULL, (FONTENUMPROC)&EnumFontFamExProcIsInstalled, (LPARAM)&lParam, 0);

      return lParam ? true : false;

   }


   //bool IsInstalled(LPCTSTR lpszFont, int iCharSet)
   //{
   //   CStringW wstr(get_uni(lpszFont, CP_ACP));
   //   // Get the screen DC
   //   CDC dc;
   //   if(!dc.CreateCompatibleDC(NULL))
   //   {
   //      return false;
   //   }
   //   LOGFONTW lf ={0};
   //   // Any character set will do
   //   lf.lfCharSet = iCharSet;
   //   // Set the facename to check for
   //   wcsncpy(lf.lfFaceName,wstr, sizeof(lf.lfFaceName));
   //   LPARAM lParam = (LPARAM) lf.lfFaceName;
   //   // Enumerate fonts
   //   ::EnumFontFamiliesW(dc.GetSafeHdc(),NULL,(FONTENUMPROCW)&EnumFontFamExProcIsInstalled,(LPARAM)&lParam);
   //   return lParam ? true : false;
   //}

   int xxx = 0;

   void md_text_rtf_processor::print(::text_format_rtf::impl::processor * pimpl,const char * psz1)
   {

      CString str(psz1);

      if(str.Find("IMMEDIA") >= 0)
      {

         xxx=1;
         OutputDebugString("IMMEDIA");

      }
      else if(xxx == 1 && str.Find("EMERGENCY") >= 0)
      {
         xxx=2;
         OutputDebugString("EMERGENCY");
      }

      {

         //debug_append_to_file(str);

         CStringW wstrUni = get_uni(str,CP_ACP);

         OutputDebugStringW(wstrUni);

      }

      int iColor = get_attr(::text_format_rtf::attr_foreground);

      if(iColor >= 0 && iColor < m_colora.size())
      {

         m_style.m_TextColor = RGB(m_colora[iColor].r,m_colora[iColor].g,m_colora[iColor].b);

      }

      int iFont = get_attr(::text_format_rtf::attr_fontface);

      auto fontfamily = fontname(iFont);

      m_style.m_LogFont.lfCharSet = fontcharset(iFont);

      if(m_style.m_LogFont.lfCharSet == 128)
      {
          
         if(!IsFontInstalled(fontfamily.c_str(), m_style.m_LogFont.lfCharSet))
         {

            fontfamily = "MS Mincho";

         }

      }

      m_style.m_LogFont.lfOutPrecision = OUT_TT_PRECIS;

      m_style.m_LogFont.lfClipPrecision = CLIP_STROKE_PRECIS;

      m_style.m_LogFont.lfPitchAndFamily = VARIABLE_PITCH;

      m_style.m_LogFont.lfQuality = DRAFT_QUALITY;

      //auto tp = m_ptext->GetTextParams();

      //tp.m_lf = m_style.m_LogFont;

      //m_ptext->SetTextParams(tp);

      strncpy(m_style.m_LogFont.lfFaceName,fontfamily.c_str(),sizeof(m_style.m_LogFont.lfFaceName));

      CStringW wstr = get_uni(fontfamily.c_str(), CP_ACP);
      CString strFontFamilyLower(fontfamily.c_str());
      if (is_ansi(fontfamily.c_str()))
      {
         strFontFamilyLower.MakeLower();
      }

      if(get_attr(::text_format_rtf::attr_bold) >= 0 || strFontFamilyLower.Find("bold") >= 0)
      {

         m_style.m_LogFont.lfWeight = FW_BOLD;

      }
      else
      {

         m_style.m_LogFont.lfWeight = FW_NORMAL;

      }

      if (get_attr(::text_format_rtf::attr_italic) >= 0 || strFontFamilyLower.Find("italic") >= 0)
      {

         m_style.m_LogFont.lfItalic = TRUE;

      }
      else
      {

         m_style.m_LogFont.lfItalic = FALSE;

      }

      if (get_attr(::text_format_rtf::attr_underline) >= 0)
      {

         m_style.m_LogFont.lfUnderline = TRUE;

      }
      else
      {

         m_style.m_LogFont.lfUnderline = FALSE;

      }


      int iDpiReference = 96;
      //HDC hDC=GetDC(NULL);
      int iRtfSize = get_attr(::text_format_rtf::attr_fontsize);
      //iDpiReference = GetDeviceCaps(hDC,LOGPIXELSY);
         //iRtfSize--;
      m_style.m_LogFont.lfHeight= -MulDiv(iRtfSize,iDpiReference, 144);
      //ReleaseDC(NULL,hDC);
      CStringW wstr2(get_uni(str, CP_ACP));
      for(int i = 0; i < str.GetLength(); i++)
      {
         m_ptext->GetText().Add(str[i],m_style);
      }

   }


   void md_text_rtf_processor::print(::std::string str)
   {

      for(int i = 0; i < str.size(); i++)
      {
         
         m_ptext->GetText().Add(str[i],m_style);

      }

   }


   void md_text_rtf_processor::print_symbol(::std::string)
   {

   }


   void md_text_rtf_processor::print_forced_space()
   {

   }
   
   
   int md_text_rtf_processor::print_unicode(int)
   {
      
      return 0;

   }


   void md_text_rtf_processor::hyperlink(::std::string)
   {

   }


   void md_text_rtf_processor::hyperlink_base(::std::string)
   {

   }


   void md_text_rtf_processor::print_char(::text_format_rtf::e_char echar)
   {

      if (echar == ::text_format_rtf::char_line_break)
      {

         int iColor = get_attr(::text_format_rtf::attr_foreground);

         if (iColor >= 0 && iColor < m_colora.size())
         {

            m_style.m_TextColor = RGB(m_colora[iColor].r, m_colora[iColor].g, m_colora[iColor].b);

         }

         auto fontfamily = fontname(get_attr(::text_format_rtf::attr_fontface));

         strncpy(m_style.m_LogFont.lfFaceName, fontfamily.c_str(), sizeof(m_style.m_LogFont.lfFaceName));

         CString strFontFamilyLower(fontfamily.c_str());

         if (is_ansi(fontfamily.c_str()))
         {

            strFontFamilyLower.MakeLower();

         }

         if(get_attr(::text_format_rtf::attr_bold) >= 0 || strFontFamilyLower.Find("bold") >= 0)
         {

            m_style.m_LogFont.lfWeight = FW_BOLD;

         }
         else
         {

            m_style.m_LogFont.lfWeight = FW_NORMAL;

         }

         int iDpiReference = 96;
         //HDC hDC=GetDC(NULL);
         int iRtfSize = get_attr(::text_format_rtf::attr_fontsize);
         //iDpiReference = GetDeviceCaps(hDC,LOGPIXELSY);
         m_style.m_LogFont.lfHeight= -MulDiv(iRtfSize,iDpiReference,144);
         //ReleaseDC(NULL,hDC);

         m_ptext->GetText().Add('\n',m_style);

      }

   }


   void md_text_rtf_processor::font_smaller_begin()
   {
   }
   void md_text_rtf_processor::font_smaller_end()
   {
   }
   void md_text_rtf_processor::table_begin()
   {
   }
   void md_text_rtf_processor::table_end()
   {
   }
   void md_text_rtf_processor::table_cell_begin()
   {
   }
   void md_text_rtf_processor::table_cell_end()
   {
   }
   void md_text_rtf_processor::table_row_begin()
   {
   }
   void md_text_rtf_processor::table_row_end()
   {
   }


   void md_text_rtf_processor::codepage(int cp_id)
   {
      m_iCodePage = cp_id;
   }

   text_format_rtf::image_manager * md_text_rtf_processor::image_begin(const ::text_format_rtf::image_descriptor & image)
   {
      return nullptr;
   }
   void md_text_rtf_processor::image_end(text_format_rtf::image_manager &)
   {
   }
