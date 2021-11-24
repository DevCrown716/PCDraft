#include "StdAfx.h"
#include "FontInfo.h"




//callback function
int CALLBACK MyEnumFontsProc(CONST LOGFONT *lplf, CONST TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData)
{
   for (int j = 0; j < LF_FACESIZE; j++)
   {
      if (lplf->lfFaceName[j] == 0)
         return 1;
      else if (lplf->lfFaceName[j] < 0 || lplf->lfFaceName[j]>128)
         break;
   }

   HDC hdc = GetDC(NULL);
   HFONT hfont = CreateFontIndirect(lplf);
   if (hfont) {
      HFONT hfontOld = (HFONT)::SelectObject(hdc, hfont);
      if (hfontOld) {
         DWORD dw = ::GetFontData(hdc, 'eman', 0, NULL, 0);
         if (dw != GDI_ERROR) {
            LPBYTE lpv = (BYTE *)malloc(dw);
            if (lpv) {
               dw = ::GetFontData(hdc, 'eman', 0, lpv, dw);
               if (dw != GDI_ERROR) {
                  UINT offset = 0;
                  USHORT format, count, stringOffset;
                  NAMERECORD * rgNR;

                  format = SWAPBYTES(&lpv[offset]);
                  offset += sizeof(USHORT);
                  count = SWAPBYTES(&lpv[offset]);
                  offset += sizeof(USHORT);
                  stringOffset = SWAPBYTES(&lpv[offset]);
                  offset += sizeof(USHORT);

                  rgNR = (struct NAMERECORD *)calloc(count, sizeof(NAMERECORD));

                  for (int i = 0; i < count; i++) {
                     rgNR[i].platformID = SWAPBYTES(&lpv[offset]);
                     offset += sizeof(USHORT);
                     rgNR[i].encodingID = SWAPBYTES(&lpv[offset]);
                     offset += sizeof(USHORT);
                     rgNR[i].languageID = SWAPBYTES(&lpv[offset]);
                     offset += sizeof(USHORT);
                     rgNR[i].nameID = SWAPBYTES(&lpv[offset]);
                     offset += sizeof(USHORT);
                     rgNR[i].length = SWAPBYTES(&lpv[offset]);
                     offset += sizeof(USHORT);
                     rgNR[i].offset = SWAPBYTES(&lpv[offset]);
                     offset += sizeof(USHORT);

                     // The name string is at
                     // lpv + stringOffset + rgNR[i].offset
                     // The name string length is rgNR[i].length

                     // The bytes in both cases must each be run
                     // through something like the SWAPBYTES macro
                     // because all font information is big endian
                     // regardless of endian-ness of the platform).

                     //http://www.microsoft.com/typography/otspec/name.htm
                     if (rgNR[i].languageID == 0 && rgNR[i].nameID == 4)//Full font name
                     {
                        CString strEngName((LPCSTR)(lpv + stringOffset + rgNR[i].offset), rgNR[i].length);
                        CString strLogName(lplf->lfFaceName);
                        CFontInfo *pFontInfo = (CFontInfo*)lpData;
                        CString strOut;
                        if (pFontInfo->m_mapLocToEngFontName.Lookup(strLogName, strOut) == 0)
                        {
                           pFontInfo->m_mapLocToEngFontName[strLogName] = strEngName;
                        }
                        break;
                     }
                  }
                  free(rgNR);
               }
               free(lpv);
            }
         }
         ::SelectObject(hdc, hfontOld);
      }
      ::DeleteObject(hfont);
   }
   ReleaseDC(NULL, hdc);
   return 1;
}


CFontInfo g_fontinfo;


CString get_font_english_name(CString str)
{

   g_fontinfo.createFontNameMaps();

   CStringW wstr = get_uni(str, CP_ACP);

   CString strEnglishName = g_fontinfo.m_mapLocToEngFontName[str];

   const char * pszEnglishName = strEnglishName;

   if (pszEnglishName == nullptr || *pszEnglishName == '\0')
   {
   
      return str;

   }
 
   if (str[0] == '@')
   {

      return "@" + strEnglishName;

   }
   
   return strEnglishName;

}


int CFontInfo::createFontNameMaps()
{
   if (m_bInit)
   {

      return 1;
   }
   m_bInit = true;
   HDC dc = ::GetDC(NULL);
   if (!dc)
      return 0;
   EnumFonts(dc, NULL, MyEnumFontsProc, (LPARAM)this);

   ::ReleaseDC(NULL, dc);
   return m_mapLocToEngFontName.GetCount();
}
