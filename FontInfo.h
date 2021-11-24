#pragma once


class CFontInfo {
public:
   CMapStringToString m_mapLocToEngFontName;
   bool m_bInit;
   int createFontNameMaps();
};


struct NAMERECORD {
   USHORT platformID;
   USHORT encodingID;
   USHORT languageID;
   USHORT nameID;
   USHORT length;
   USHORT offset;
};

#define SWAPBYTES(w)  ( ((USHORT)((w)[0]) << 8) | (w)[1])
