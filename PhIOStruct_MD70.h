// MD7.0 - Infinisys Ltd
#pragma once

struct DimFormat;
struct DimStandard;
class CMDShape;
class CLinkVertexList;

#define DEV_DEBUG 0


template < typename T >
class array: public CArray < T >
{
public:

   array & operator = (const array & a)
   {
      if(this == &a)
         return *this;

      RemoveAll();
      Append(a);

      return *this;
   }

   T & last()
   {
      return operator[](GetUpperBound());
   }
   const T & last() const
   {
      return operator[](GetUpperBound());
   }

   bool has_elements()
   {
      return GetCount() > 0;
   }
   bool is_empty()
   {
      return GetCount() <= 0;
   }
};

namespace formatMD70
{


   inline void swap16(void * p)
   {
      char * pch = (char *)p;
      char t;
      t=pch[1]; pch[1]=pch[0]; pch[0]=t;
   }
   inline void swap16(void * p, int count)
   {
      char * pch = (char *)p;
      while(count > 0)
      {
         swap16(pch);
         count--;
         pch += 2;
      }
   }
   inline void swap32(void * p)
   {
      char * pch = (char *)p;
      char t;
      t=pch[3]; pch[3]=pch[0]; pch[0]=t;
      t=pch[2]; pch[2]=pch[1]; pch[1]=t;
   }
   inline void swap32(void * p,int count)
   {
      char * pch = (char *)p;
      while(count > 0)
      {
         swap32(pch);
         count--;
         pch += 4;
      }
   }
   inline void swap64(void *p)
   {
      char * pch = (char *)p;
      char t;
      t=pch[7]; pch[7]=pch[0]; pch[0]=t;
      t=pch[6]; pch[6]=pch[1]; pch[1]=t;
      t=pch[5]; pch[5]=pch[2]; pch[2]=t;
      t=pch[4]; pch[4]=pch[3]; pch[3]=t;
   }


   class memory
   {
   private:


      char *	m_pdata;
      UINT		m_iSize;


   public:


      memory()
      {
         m_pdata = NULL;
         m_iSize = 0;
      }

      memory(int iSize)
      {
         m_pdata = (char *)malloc(iSize);
         m_iSize = iSize;
      }

      memory(void * pdata,int iSize)
      {
         m_pdata = (char *)pdata;
         m_iSize = iSize;
      }

      memory(const memory & mem)
      {
         m_pdata = (char*)malloc((size_t)mem.m_iSize);
         m_iSize = mem.m_iSize;
         memcpy(m_pdata,mem.m_pdata,(size_t)m_iSize);
      }

      memory(memory && mem)
      {
         m_pdata = mem.m_pdata;
         m_iSize = mem.m_iSize;
         mem.m_pdata = NULL;
         mem.m_iSize = 0;
      }

      ~memory()
      {
      }

      bool allocate(size_t iSize)
      {
         destroy();
         m_pdata = (char *)malloc(iSize);
         if(m_pdata == NULL)
         {
            return false;
         }
         m_iSize =(UINT) iSize;
         return true;
      }

      void destroy()
      {
         if(m_pdata != NULL)
         {
            free(m_pdata);
            m_pdata = NULL;
            m_iSize = 0;
         }
      }

      bool read_file(CFile & file,size_t offset,size_t uiLength)
      {

         if(uiLength <= 0)
         {

            return false;

         }

         if(!allocate(uiLength))
         {
            return false;
         }

         file.Seek(offset,CFile::SeekPosition::begin);

         file.Read(m_pdata,(UINT)m_iSize);

         return true;
      }

      char * get_data() { return m_pdata; }

      const char * get_data()const { return m_pdata; }

      UINT get_size()const { return m_iSize; }

      bool read_resource(HINSTANCE hinstance,UINT nID,LPCTSTR lpcszType);
      
   };

   class serialize;

   class complex_type_context
   {
   public:

      serialize * m_pserialize;

      complex_type_context(serialize * pserialize);

      ~complex_type_context();
   };

   class raw_type_context
   {
   public:

      serialize * m_pserialize;

      raw_type_context(serialize * pserialize);

      ~raw_type_context();
   };

   class external_type_context
   {
   public:

      serialize * m_pserialize;

      external_type_context(serialize * pserialize);

      ~external_type_context();
   };

   struct pointer
   {
      union
      {
         char * pch;
         unsigned char * puch;
         short * psh;
         unsigned short* push;
         int  * pi32;
         unsigned int  * pui32;
         long  * pl;
         unsigned long  * pul;
         __int64 * pi64;
         unsigned __int64 * pui64;
         float * pf;
         double * pd;
      };

      char *      pend;
      serialize * m_pserialize;
      bool        m_bComplexOnStack;
      struct copy
      {
         char *   m_p;
         int      m_iSize;
         bool     m_bComplex;
         bool     m_bExternal;

         copy(bool bComplex = false,bool bExternal = false)
         {
            m_p = NULL;
            m_iSize = 0;
            m_bComplex = bComplex;
            m_bExternal = bExternal;
         }
      };

      array < copy >   m_copya;
      bool              m_bSwap;

      pointer(serialize * pserialize)
      {
         m_pserialize = pserialize;
         m_bSwap = true;
         pch = NULL;
         m_bComplexOnStack = false;
      }

      bool read(memory & mem,CFile & file,size_t uiOffset,size_t uiLength)
      {
         if(!mem.read_file(file,uiOffset,uiLength))
         {
            return false;
         }

         pch = mem.get_data();

         pend = pch + uiLength;

         return true;
      }

      void str(CString & strParam)
      {
         long len = i32();

         str(strParam,len);
      }

      void str(CString & str,long len)
      {

         external_type_context externaltypecontext(m_pserialize);
         
         str = CString(pch,len);

         inc_no_copy(len);

      }

      CString str()
      {
         CString strRet;

         str(strRet);

         return strRet;
      }

      char ch()
      {
         char ch = *pch;
         inc_no_copy(sizeof(char));
         return ch;
      }

      unsigned char uch()
      {
         unsigned char uch = *puch;
         inc_no_copy(sizeof(unsigned char));
         return uch;
      }

      short sh()
      {
         swap16();
         short sh = *psh;
         inc_no_copy(sizeof(short));
         return sh;
      }

      unsigned short ush()
      {
         swap16();
         unsigned short ush = *push;
         inc_no_copy(sizeof(unsigned short));
         return ush;
      }

      int i32()
      {
         swap32();
         int i = *pi32;
         inc_no_copy(sizeof(int));
         return i;
      }

      unsigned int ui32()
      {
         swap32();
         unsigned int ui = *pui32;
         inc_no_copy(sizeof(unsigned int));
         return ui;
      }

      __int64 i64()
      {
         swap64();
         __int64 i64 = *pi64;
         inc_no_copy(sizeof(__int64));
         return i64;
      }

      unsigned __int64 ui64()
      {
         swap64();
         unsigned __int64 ui64 = *pui64;
         inc_no_copy(sizeof(unsigned __int64));
         return ui64;
      }

      template < typename T >
      void synch_type(T p)
      {
         pch = (char *)p;
         pend = (char *)(p + 1);
      }

      template < typename T >
      void synch(T & p)
      {
         copy * pcopy = NULL;

         if(m_copya.GetCount() > 0)
         {
            pcopy = &m_copya[m_copya.GetUpperBound()];
         }

         if(m_bComplexOnStack)
         {
            if(pcopy != NULL)
            {
               if(!pcopy->m_bExternal && pcopy->m_p != NULL)
               {
                  int delta = (int) ((char *)(void *) p - (((char *)(void *) pcopy->m_p) + pcopy->m_iSize));

                  ASSERT(delta == 0);

                  if(delta != 0)
                  {
                     // if delta is greater than zero,
                     // something was not serialized.
                     // if something need to be skipped,
                     // please notify serialize class.
                     // if delta is negative,
                     // some serialization was done out
                     // of expected order, or a
                     // class with external storage was used.
                     // for external storage,
                     // the external_type_context class can be used as guard.

                     //pch += delta;

                     throw "error";
                  }
               }

               pcopy->m_p = (char *)(void *)p;

               pcopy->m_iSize = sizeof(*p);

            }
         }
         else if(p == NULL)
         {
            p = (T)(void *) pch;
         }
         else if(pch == NULL)
         {
            pch = (char *)(void *) p;
         }
      }

      
      inline void swap16()
      {
         if(!m_bSwap)
         {
            return;
         }
         ::formatMD70::swap16(pch);
      }

      
      inline void swap32()
      {
         if(!m_bSwap)
         {
            return;
         }
         ::formatMD70::swap32(pch);
      }


      inline void swap64()
      {
         if(!m_bSwap)
         {
            return;
         }
         ::formatMD70::swap64(pch);
      }

      
      inline void inc64()
      {
         inc(sizeof(__int64));
      }
      inline void inc32()
      {
         inc(sizeof(int));
      }
      inline void inc16()
      {
         inc(sizeof(short));
      }

      inline void inc8()
      {
         inc(1);
      }

      inline void inc(size_t s)
      {
         if(m_copya.GetCount() > 0)
         {
            int iLast = (int) (m_copya.GetCount() - 1);

            copy & copy = m_copya[iLast];

            if(copy.m_p != NULL && copy.m_iSize > 0)
            {
               ASSERT(copy.m_iSize == s);

               if(copy.m_iSize == s)
               {
                  memcpy(copy.m_p,pch,s);
               }
            }
         }
         pch += s;
         if(pch > pend)
         {
            throw "error";
         }
      }

      inline void inc_no_copy(size_t s)
      {
         if(m_copya.GetCount() > 0)
         {
            copy & copy = m_copya.last();

            if(copy.m_p != NULL)
            {
               copy.m_p+=s;
            }
         }

         pch += s;
         if(pch > pend)
         {
            throw "error";
         }
      }

      inline void swap(short & sh,int count = 1)
      {
         swap(&sh,count);
      }

      inline void swap(short * psh,int count = 1)
      {
         while(count > 0)
         {
            synch(psh);
            swap16();
            inc16();
            count--;
         }
      }

      inline void swap(unsigned short & ush,int count = 1)
      {
         swap(&ush,count);
      }

      inline void swap(unsigned short * push,int count = 1)
      {
         while(count > 0)
         {
            synch(push);
            swap16();
            inc16();
            count--;
         }
      }

      inline void swap(long & l,int count = 1)
      {
         swap(&l,count);
      }

      inline void swap(long * pl,int count = 1)
      {
         while(count > 0)
         {
            synch(pl);
            swap32();
            inc32();
            count--;
         }
      }

      inline void swap(unsigned long & ul,int count = 1)
      {
         swap(&ul,count);
      }

      inline void swap(unsigned long * pul,int count = 1)
      {
         while(count > 0)
         {
            synch(pul);
            swap32();
            inc32();
            count--;
         }
      }

      inline void swap(float & f,int count = 1)
      {
         swap(&f,count);
      }

      inline void swap(float * pf,int count = 1)
      {
         while(count > 0)
         {
            synch(pf);
            swap32();
            inc32();
            count--;
         }
      }

      inline void swap(int & i,int count = 1)
      {
         swap(&i,count);
      }

      inline void swap(int * pi32,int count = 1)
      {
         while(count > 0)
         {
            synch(pi32);
            swap32();
            inc32();
            count--;
         }
      }

      inline void swap(unsigned int & ui,int count = 1)
      {
         swap(&ui,count);
      }

      inline void swap(unsigned int * pui32,int count = 1)
      {
         while(count > 0)
         {
            synch(pui32);
            swap32();
            inc32();
            count--;
         }
      }

      inline void swap(__int64 & i,int count = 1)
      {
         swap(&i,count);
      }

      inline void swap(__int64 * pi64,int count = 1)
      {
         while(count > 0)
         {
            synch(pi64);
            swap64();
            inc64();
            count--;
         }
      }

      inline void swap(unsigned __int64 & ui,int count = 1)
      {
         swap(&ui,count);
      }

      inline void swap(unsigned __int64 * pui64,int count = 1)
      {
         while(count > 0)
         {
            synch(pui64);
            swap64();
            inc64();
            count--;
         }
      }

      inline void swap(double & d,int count = 1)
      {
         swap(&d,count);
      }

      inline void swap(double * pd,int count = 1)
      {
         while(count > 0)
         {
            synch(pd);
            swap64();
            inc64();
            count--;
         }
      }
   };

   class serialize;

   class serializable
   {
   public:

      serializable()
      {
      }

      virtual ~serializable()
      {
      }

      virtual void operator()(serialize & serializer) {}
   };

   inline unsigned char byte_clip(float f)
   {
      if(f <= 0.f)
         return 0;
      else if(f >= 255.f)
         return 255;
      return (unsigned char)f;
   }
   const double kRadToAng		= 180.0 / _M_PI; 		/* 180/pi */
   const double kAngToRad		= _M_PI / 180.0;		/* pi/180 */

#pragma pack(1)

#define kPhDocType		'MD70'
#define kPhPadType		'MPAD'
#define kPhSYMBType		'MSYM'

#define kPhFileHdr		0x3230
#define kPhRelease8	   8

   class Coord
   {
   public:

      float m_f;

      Coord() {}
      Coord(float f) { m_f = f; }


      operator WrFixed() const { return WrFixed(m_f * 0.28); }

      Coord & operator = (float f) { m_f = f; return *this; }

      Coord & operator = (const WrFixed & wr) { m_f = (float) (wr.GetRawDouble() / (65536.0 * 0.28)); return *this; }

      bool operator == (const Coord & f) const { return m_f == f.m_f; }
      bool operator != (const Coord & f) const { return !operator == (f); }

      bool operator == (float f) const { return f == m_f; }
   };
   class DCoord
   {
   public:

      double m_d;

      DCoord() {}
      DCoord(double d) { m_d = d; }

      operator WrFixed() const { return WrFixed(m_d * 0.28); }

      DCoord & operator = (double d) { m_d = d; return *this; }

      DCoord & operator = (const WrFixed & wr) { m_d = wr.GetRawDouble()/(65536.0 * 0.28); return *this; }

      DCoord operator - ()const  { return DCoord(-m_d); }
      bool operator == (const DCoord & d) const { return m_d == d.m_d; }
      bool operator != (const DCoord & d) const { return !operator == (d); }

      bool operator == (double d) const { return d == m_d; }
      bool operator >= (double d) const { return d >= m_d; }
      bool operator >= (const DCoord & d) const { return d >= d.m_d; }

   };

   typedef double DAngle;

   const DAngle kAlmostSameCriteria = 400.0 / (65536.0 * 0.28);
   //const DCoord kAlmostSameCriteria = 400.0 / (65536.0 * 0.28);

   typedef short SInt16;
   typedef unsigned short UInt16;
   typedef int SInt32 ;
   typedef unsigned int UInt32;
   typedef __int64 SInt64 ;
   typedef unsigned __int64 UInt64;
   typedef float Angle;
   typedef unsigned long ulong;
   typedef long Alignment;
   typedef short Alignment2;
   typedef unsigned char uchar;
   typedef unsigned short ushort;
   typedef unsigned char Boolean;
   typedef unsigned char UInt8;
   typedef int md_bool;
   typedef long Size;
   typedef SInt32 Fixed; // 16-bit signed integer plus 16-bit fraction
   typedef UInt32 FourCharCode;
   typedef FourCharCode OSType;
   typedef SInt16 OSErr;
   typedef UInt16 Unit ;
   typedef ushort	DmPrintFlags;

   enum eID:SInt32
   {
      IDForce32bits		= -1,

      IDInvalid		 	= 0,

      IDLine2				= 257,
      IDLine				= 1,

      IDFreehand			= 3,
      IDPolygon			= 4,

      IDRectCenter			= 9,
      IDRectDiagonal		= 10,

      IDRndRectangle		= 11,

      IDFreeSpline			= 12,
      IDPolySpline			= 14,

      IDCircle				= 20,
      IDArc				= 21,
      IDEArc				= 22,
      ID3Arc				= 23,
      IDEllipse			= 24,
      //IDBezier				= 30,
      IDCubic				= 31,
      IDBezier				= 32,

      IDText				= 41,

      IDBitmap				= 48,

      IDGroup				= 51,
      IDMetaPoly			= 53,

      IDViewPort			= 60,

      IDHatch				= 61,
      IDSolidHatch			= 62,

      IDHLineDim			= 65,
      IDVLineDim			= 66,
      IDSLineDim			= 67,
      IDPLineDim			= 68,
      IDRRadialDim			= 69,
      IDDRadialDim			= 70,
      IDCenterDim			= 71,
      IDAngularDim			= 72,
      IDArea				= 74,
      IDPerimeter				= 75,
      IDDimString			= 79,

      IDDimPt2Pt			= 90,

//      IDParallelLine		= 100,
//      IDParallelPolygon	= 101,
      IDMultiLine=102,
      IDWall              = 103,

      IDUnknownShape		= 126,

      IDBaseDocObject= 0x01000000,

      IDFirstPattern= 0x01000011,
      IDPixelPattern= 0x01000011,
      IDBlendPattern= 0x01000012,
      IDLastPattern= 0x0100002F,
      IDVersionObject= 0x01000030,

      IDUnknownDocObject= 0x0100007E,
      IDLastDocObject= 0x0100007F

   } ;

   struct ColorScheme
   {
      float red;
      float green;
      float blue;
   };

   struct ColorSchemeInfo
   {
      
      ColorScheme schemea[4];

   };

#define	kMaxLineSpacing	127

#define	kDocState_Size			960

#define kMarkerStartStructs		kDocState_Size
   /// Marker position of Main Structs
#define kMarkerGenericPattern		kMarkerStartStructs
#define kMarkerLayerStruct		kMarkerStartStructs + 8
#define kMarkerCDocObjectInfo		kMarkerStartStructs + 16
#define kMarkerShapes			kMarkerStartStructs + 24
#define kMarkerBitmaps			kMarkerStartStructs + 32
#define kMarkerViews			kMarkerStartStructs + 40
#define kMarkerPalette			kMarkerStartStructs + 48
#define kMarkerDashTable		kMarkerStartStructs + 56
#define kMarkerAllLinks			kMarkerStartStructs + 64
#define kMarkerReports			kMarkerStartStructs + 72
#define kMarkerFlatPageFormat		kMarkerStartStructs + 80
#define kMarkerFlatPrintSettings	kMarkerStartStructs + 88
#define kMarkerReserved_8kMarkStartStructs kMarkerStartStructs + 96
#define kMarkerQuickLookPreview	kMarkerStartStructs + 104
#define kMarkerReserved_7kMarkStartStructs		kMarkerStartStructs + 112
#define kMarkerLayersCustomScaleInfo kMarkerStartStructs + 120
#define kMarkerGuidesInfo		kMarkerStartStructs + 128
#define kMarkerColorSchemeInfo	kMarkerStartStructs + 136
#define kMarkerDocVersionSave		kMarkerStartStructs + 144
#define kMarkerReserved_6		kMarkerStartStructs + 152
#define kMarkerReserved_5		kMarkerStartStructs + 160
#define kMarkerReserved_4		kMarkerStartStructs + 168
#define kMarkerReserved_3		kMarkerStartStructs + 176
#define kMarkerReserved_2		kMarkerStartStructs + 184
#define kMarkerReserved_1		kMarkerStartStructs + 192

#define kMarkerDataStream		kMarkerStartStructs + 200

   // start macdraft 5

#define kPhSelectedMask				0x0001
#define kPhDeletedMask				0x0002
#define kPhVertexEditedMask			0x0004
#define kPhObjLockedMask			0x0008
#define kPhShowDatumMask			0x0010

#define kPhCtrDatumCodeMask			0x0020
#define kPhFreeDatumCodeMask		0x0040
#define kPhSetDatumCodeMask			0x0060
#define kPhDatumCodeShift				5

#define kPhMetaPolyMemberMask		0x0100
#define kPhRunInReverseMask			0x0200

   // 0x0C00 = 0x0800 + 0x0400
#define kPhVertTextAlignMask		0x0C00
#define kPhVertTextAlignCodeShift	10

#define kPhAccurateWidthFlag		0x0400  // Reuse the bits that we had previously
#define kPhSquareEndsFlag			0x0800	// used for text vertical alignment

   // end macdraft 5

   enum Method: long
   {
      eCenterRadius,eDiameter,e3Point,eEllipse
   } ;

   enum Corner: long
   {
      kProportionalCorner=1,kConstantCorner,kEllipticalCorner
   };


   enum Smoothing :short
   {
      kPhUnsmooth,kPhSmooth,kPhVerySmooth
   };

   enum
   {
      // N.B. sometimes also use 'true' !
      kPhCanSelect		= 0x80,	/* these are points that can be dimensioned */
      kPhControlVertex	= 0x00,
      kPhRealVertex		= 0x81,	/* always can select */
      kPhSelectedVertex	= 0x02,
      kPhNumber			= 0x04,
      kPhNoScale			= 0x08,	/* will not change scale */
      kPhNoHighlight		= 0x10
   };

   typedef ushort DimStatus;
   typedef double DimValue;

#define	kPhDimStrAttached		1		/* boolean flags for dimShape and dimStr status */
#define kPhDimVisible			2		/* Also gCurrDimStyle.fStyle... see note below */
#define kPhAreaStr				4
#define	kPhDimStrAbove			8
#define	kPhDimStrOnSlope		16
#define kPhDimNoWitness			32
#define kPhDimEndmarkOut		64
#define kPhAreaNeedsRecalc		128


   enum TextDisplayType: short
   {
      kHorizontal,
      kAlongLine,
      kAbove,
      kBelow
   };

   enum LeaderType: short
   {
      kLeaderNoLeader,
      kLeaderLeft,
      kLeaderRight
   };

   enum RDSymbolType: short
   {
      kNoSymbol,
      kLeading,
      kTrailing
   };

   enum ToleranceType: short
   {
      kNoTolerance,
      kOne,
      kTwo,
      kLimits
   };

   struct PhPoint
   {
      short               y; // BigEndian-LittleEndian
      short               x; // BigEndian-LittleEndian
   };

   struct PhRect
   {
      short               t; // BigEndian-LittleEndian
      short               l; // BigEndian-LittleEndian
      short               b; // BigEndian-LittleEndian
      short               r; // BigEndian-LittleEndian
   };

   enum eTextType
   {
      kCaption = false,
      kParagraph = true,
      kChainedParagraph // next kChainedParagraph or kParagraph text object can hold text which overflows this box
   };

   enum TSpace: long
   {
      kSingleSpace = -3,
      kOne_halfSpace = -2,
      kDoubleSpace = -1,
      kSpaceOther =kMaxLineSpacing,
   };

   enum Just: long
   {
      /* Justification (word alignment) styles */
      teJustLeft                    = 0,
      teJustCenter                  = 1,
      teJustRight                   = -1,
      teForceLeft                   = -2,   /* new names for the Justification (word alignment) styles */
      teFlushDefault                = 0,    /*flush according to the line direction */
      teCenter                      = 1,    /*center justify (word alignment) */
      teFlushRight                  = -1,   /*flush right for all scripts */
      teFlushLeft                   = -2    /*flush left for all scripts */
   };

   typedef short PhUnit;
   typedef short PhAngleUnit;

   struct DCoordPoint
   {
      DCoord         y;
      DCoord         x;
      DCoord         reservedForZ;

      operator LongPoint() { return to_long_point(*this); }

      static ::LongPoint to_long_point(DCoordPoint p)
      {
         ::LongPoint l;

         l.x = p.x;
         l.y = p.y;

         return l;
      }

      DCoordPoint()
      {
         reservedForZ = 0.0;
      }

      DCoordPoint(DCoord xParam,DCoord yParam)
      {
         x = xParam;
         y = yParam;
      }
      DCoordPoint(double xParam,double yParam)
      {
         x = xParam;
         y = yParam;
      }

      DCoordPoint operator -(const DCoordPoint& p2) const
      {
         return DCoordPoint(x.m_d - p2.x.m_d,y.m_d - p2.y.m_d);
      }
      DCoordPoint operator +(const DCoordPoint& p2) const
      {
         return DCoordPoint(x.m_d + p2.x.m_d,y.m_d + p2.y.m_d);
      }
      DCoordPoint operator /(double d) const
      {
         return DCoordPoint(x.m_d / d,y.m_d / d);
      }

      bool operator == (const DCoordPoint & pt) const
      {
         return x == pt.x && y == pt.y;
      }
      bool operator != (const DCoordPoint & pt) const
      {
         return !operator==(pt);
      }

      DCoordPoint operator - () const
      {
         return DCoordPoint(-x, -y);
      }

      DCoordPoint & operator = (const LongPoint & pt) 
      {
         x = pt.x;
         y = pt.y;
         return *this;
      }


      bool operator>=(const DCoordPoint& pt) const
      {
         return x >= pt.x && y >= pt.y;
      }
      bool operator>=(double d) const
      {
         return x >= d && y >= d;
      }
   };

   struct CoordPoint
   {
      Coord y;
      Coord x;

      CoordPoint()
      {
      }

      CoordPoint(Coord xParam,Coord yParam)
      {
         x = xParam;
         y = yParam;
      }

      CoordPoint operator -(const CoordPoint& p2) const
      {
         return CoordPoint(x.m_f - p2.x.m_f,y.m_f - p2.y.m_f);
      }
   };
   struct DCoordRect
   {
      DCoord t;
      DCoord l;
      DCoord b;
      DCoord r;

      DCoordRect() {}

      DCoordRect(DCoord left, DCoord top, DCoord right, DCoord bottom)
      {

         t = top;
         l = left;
         r = right;
         b = bottom;
      
      }

   };
   struct CoordRect
   {
      Coord t;
      Coord l;
      Coord b;
      Coord r;

      CoordRect() {}

      CoordRect(Coord left,Coord top,Coord right,Coord bottom)
      {

         t = top;
         l = left;
         r = right;
         b = bottom;

      }
   };
   struct RGBFloat
   {
      float fRed;
      float fGreen;
      float fBlue;
   };
   struct RGBAFloat: RGBFloat
   {
      float fOpacity;
   };
   struct RGBAPos: RGBAFloat
   {
      float fPos;
   };

   struct PhFillModel
   {
      RGBAFloat   color;
      long        fFillType;
      long        fFillIndex;
      long        fReserved = 0;
   };

#define kDashCodeEntrySize	16
#define kDashCodeEntryCount	16

   struct DashStyle
   {
      uchar	seg[kDashCodeEntrySize] 	;
   };

   typedef DashStyle	DashTable[kDashCodeEntryCount];

   extern DashTable g_dashtableDefault;

   struct RGBColor
   {
      unsigned short	red;
      unsigned short	green;
      unsigned short	blue;
   };

   struct PhPenModel
   {
      PhFillModel   fillModel;
      double      width;
      char        fEndCap[2];
      Alignment   fAlignment;
      DashStyle   fDash;
      long        fPenReserved = 0;
   };

   typedef unsigned char  Style;
   //   typedef long TSpace ;
   //   typedef long TCase ;
   enum
   {
      teHorizJustLeft = 0,
      teHorizJustCenter = 1,
      teHorizJustRight = -1,
      teHorizJustUndefined = -2
   };
   typedef short THorizTextJust ;
   enum
   {
      k_mdTopAlign = 0,// This must be zero for backwards compatibility with top align
      k_mdMiddleAlign,
      k_mdBottomAlign
   } ;

   typedef long TVertTextAlign ;
   typedef long TTextFlipped ;
   //typedef long TrimMethod ;
   /* character Case constants */
   enum TCase: long
   {
      kPhAsIs = 1,
      kPhUppercase,
      kPhLowercase,
      kPhTitleText
   };
   
   
   struct PhTextParams
   {
   
      CString           fTextFontName;
      float	            fTextSize = 0.f;
      md_bool           fUnderline = 0;
      THorizTextJust    fHorizTextJust = teHorizJustLeft; // short
      TVertTextAlign	   fVertTextAlign	= k_mdTopAlign; // Sint32
      TTextFlipped	   fTextFlip = 0;  // Sint32
      enum TSpace       fTextSpace = kSingleSpace; // Sint32
      enum TCase	      fTextCase = kPhAsIs;// Sint32
      // Boolean	      fInPixels = 0;
      char              fReserved2 = 0;
      short	            fReserved3 = 0;
      RGBAFloat	      fTextColor = {};
      // CoordRect	   fMargins	= {};
      short             fReserved4 = 0;

   };


   enum TrimMethod: long
   {
      kPhNoTrim = 0,kPhManualTrim,kPhAutoTrim
   };
   struct PhChamferAttr
   {
      Coord	fLine1SetBack	;
      Coord	fLine2SetBack	;
      enum TrimMethod	fTrimMethod=	kPhNoTrim	;
   };
   struct PhFilletAttr
   {
      Coord	fRadius	;
      TrimMethod	fTrimMethod=	kPhNoTrim;
      Boolean	fIsSmallerArc	= 0;
   };
   struct PhWindowSplit
   {
      long f;
      double	fSplitRatio	;
      short	fVCurScroll	;
      short	fHCurScroll	;
      double	fZoomFactor	;
   };

   struct PhPropListElem
   {
      SInt32	fType	;
      SInt32  	fFlags	;
      UInt64	fDataLength	;
      SInt64	fArea;
      SInt64	fPerimeter;
      SInt32  	fReserved2;
      SInt32   ptrLength;
      array < unsigned char > data;
   };

   //Str255
   //   Field	Type
   //   len	char
   //   String data sequence in ASCII encoding of length = len is written if len>0

   //   String
   //   Field	Type
   //   length	signed long
   //   String data written in kCFStringEncodingUTF8 encoding - UInt8 data sequence length bytes long(if length != 0)

   struct DDBPoint
   {
      DCoordPoint	fLongPt;
      long	fReserved;
      uchar	fFlags;
      char	fUse;
      short	fReserved1;

      DDBPoint() {}

      DDBPoint(DCoordPoint pt,int flags, int use)
      {
         fLongPt = pt;
         fFlags = flags;
         fUse = use;
         fReserved1 = 0;
         fReserved = 0;
      }

      DDBPoint(LongPoint pt,int flags,int use)
      {
         fLongPt = pt;
         fFlags = flags;
         fUse = use;
      }

   };

   struct PhParallelAttr: public PhFillModel
   {
      DCoord	fWidth;
      char	fAlignment;
      char	fEndCap;
      char	fLineJoin;
      char	fParaReserved1 = 0;
      long	fParaReserved2 = 0;
   };

   struct PhDimStd
   {
      short dsType;						/* ANSI, Metric */
      short dsReserved1;
      long dsReserved2;
      DCoord dsWitnessExt;
      DCoord dsWitnessLineGap;
      DCoord dsDimTextGap;
      DCoord dsLeaderLen;
      DCoord dsOutsideLineLen;
      long dsToleranceTextScale;
      long dsToleranceSpaceScale;
      DCoord dsCircleCenterLen;
      DCoord dsCircleCenterGap;
      DCoord dsCenterExt;
   };

   typedef long Handle;

   struct PhDimFormat
   {
      short dfType;			/* Linear, Radial, Angular */
      Boolean dfTextCentered;
      Boolean dfTextFramed;
      Boolean dfArrowsInside;
      Boolean dfUseLabel;
      Boolean dfUseWitness;
      Boolean dfSpare1;
      TextDisplayType dfTextDisplay;	/* Horizontal, Along Line, Above, Below */
      LeaderType dfLeader;			/* None, Left, Right */
      RDSymbolType dfRDSymbol;		/* None, Leading, Trailing */
      ToleranceType dfTolerance;		/* None, One, Two, Limits */
      DCoord dfTolerance1;
      DCoord dfTolerance2;
      SInt64 dfDimLabelID;
      SInt64 dfReserved;

      PhDimFormat()
      {
         dfTextCentered = TRUE;
      }
   };

   struct PhDimFormat2
   {
      short dfType;			/* Linear, Radial, Angular */
      Boolean dfTextCentered;
      Boolean dfTextFramed;
      Boolean dfArrowsInside;
      Boolean dfUseLabel;
      Boolean dfUseWitness;
      Boolean dfSpare1;
      TextDisplayType dfTextDisplay;	/* Horizontal, Along Line, Above, Below */
      LeaderType dfLeader;			/* None, Left, Right */
      RDSymbolType dfRDSymbol;		/* None, Leading, Trailing */
      ToleranceType dfTolerance;		/* None, One, Two, Limits */
      DCoord dfTolerance1;
      DCoord dfTolerance2;
      CString dfDimLabel;

      PhDimFormat2()
      {
         dfTextCentered = TRUE;
         dfType = 0;
         dfTextFramed = 0;
         dfArrowsInside = 0;
         dfUseLabel = 0;
         dfUseWitness = 0;
         dfSpare1 = 0;
         dfTextDisplay = kHorizontal;
         dfLeader = kLeaderNoLeader;
         dfRDSymbol = kNoSymbol;
         dfTolerance = kNoTolerance;
         dfTolerance1 = 0;
         dfTolerance2 = 0;
      }
   };


   struct PhDocStds
   {
      PhDimStd fEnglishDimStd;
      PhDimStd fMetricDimStd;
      PhDimFormat fEnglishDimFormat[3];
      PhDimFormat fMetricDimFormat[3];
   };

   enum
   {
      kPhNoFill	= 0,
      kPhFillColor,
      kPhFillPat,
      kPhFillGray
   };

   struct PhFillDescript
   {
      short fColor;
      short fResID;
      union
      {
         char fPattern[8];
         struct
         {
            long fPixPat;
            short fEditSize;
         } fColorPat;
      } fData;
   };

   struct PhGenericPatternHeader
   {
      UInt64      numItems;
      UInt64      sizeVersionNumber;
      UInt64      idOfVersionNumber;
      UInt64      versionNumber;
   };

   struct PatternBase
   {
      UInt64   Size;
      UInt64   Id;
   };

   struct PixelPattern: public PatternBase
   {
      UInt32   fBitsPerChannel;
      UInt32   fWidth;
      UInt32   fHeight;
      float    fRotation;
      //fWidth * fHeight times write fPixels data
      CArray < unsigned long > data;
   };

   struct BlendPattern: public PatternBase
   {
      float   fRotation;
      UInt32  fLinearNotRadial;
      float  fCenterX;
      float  fCenterY;
      CArray < RGBAPos > data;
   };

   struct PhObjectInfo
   {
      short fVersion;
      short fSetNumber;
      //	uchar fText[1];
   };

   //struct PixPat
   //{
   //   short patType;
   //   long patMap;
   //   long patData;
   //   long patXData;
   //   short patXValid;
   //   long patXMap;
   //   uchar pat1Data[8];
   //};

   //struct BitMap
   //{
   //   long baseAddr;
   //   short rowBytes;
   //   PhRect bounds;
   //};

   //struct PixMap: public BitMap
   //{
   //   short pmVersion;
   //   short packType;
   //   long packSize;
   //   long hRes;
   //   long vRes;
   //   short pixelType;
   //   short pixelSize;
   //   short cmpCount;
   //   short cmpSize;
   //   long planeBytes;
   //   long pmHandle;
   //   long pmReserved;
   //};

   //struct PixPatMap
   //{
   //   PixPat pp;
   //   PixMap pm;
   //};

   struct ColorSpec
   {
      short value;
      RGBColor rgb;
   };

   struct ColorTable
   {
      long ctSeed;
      short ctFlags;
      short ctSize;
      ColorSpec ctTable[1];
   };


   typedef short PhAlignment;

   enum
   {
      kPhPenInvis = 0,
      kPhPenColor,			/* rgb color */
      kPhPenGray,
      kPhPenPat				/* b&w or fullColor pattern */
   };



   struct NonColorBitmap
   {
      Size	                     theLong;
      short	                     rowBytes;
      PhRect	                  bounds;
      array < unsigned char >   data;

   };

   struct ColorBitmap
   {
      Size	                        theLong	;
      short	                        rowBytes	;
      PhRect	                     bounds	;//	Data of sizeof(Rect) bytes(8 bytes)
      short	                        pmVersion	;
      short	                        packType	;
      SInt32	                     packSize	;//	length of pixel data
      Fixed	                        hRes	;	//horiz. resolution(ppi)
      Fixed	                        vRes	;	//vert. resolution(ppi)
      short	                        pixelType	;
      short	                        pixelSize	;	//bits in pixel
      short	                        cmpCount	;	//components in pixel
      short	                        cmpSize	;	//bits per component
      OSType	                     pixelFormat	;//	fourCharCode representation
      Size	                        pmTable	;//	(if 0 -> pmTable =0 else theLong is the size of pmTable
      Size	                        pmExt	;
      array < unsigned char >      data	;	//Chunk of data of baseAddr size
      array < unsigned char >      table	;	//Chunk of data of pmTable size

   };

   struct PhBitmapIOStruct
   {
      long fBitmapOffset;	/* 0L if no bitmap */
      long fBitmapLength;	/* 0L if no bitmap */
      ushort fShareCount;
      Boolean fColor;		/* set to true if color PixMap is used. */
      char fUnused;		/* pad for even bytes */
      SInt32         imageDataSize;
      array < unsigned char >      imageData	;	//Chunk of data of baseAddr size

   };

   //struct PhTextParams
   //{
   //   short fTextFont;
   //   short fTextSize;
   //   Style fTextStyle;
   //   char fDummy;
   //   short fTextJust;
   //   TSpace fTextSpace;
   //   TCase fTextCase;
   //   RGBColor fTextColor;
   //};

   typedef short PhLineAttribute;

   /* printing preference */

#define	kPhPrintByCols		0x0001
#define kPhRegMarks			0x0002
#define kPhPrintLastFirst	0x0004
#define kPhPrintPlot		0x0008

   typedef ushort DmPrintFlags;		/* bit or of above masks */



   //struct PhFilletAttr
   //{
   //   Coord fRadius;
   //   TrimMethod fTrimMethod;		/* kAutoTrim, kManualTrim, kNoTrim */
   //   Boolean fIsSmallerArc;
   //};

   //struct PhChamferAttr
   //{
   //   Coord fLine1SetBack;
   //   Coord fLine2SetBack;
   //   TrimMethod fTrimMethod;		/* kAutoTrim, kManualTrim, kNoTrim */
   //   char fDummy;
   //};

   struct PhActiveEnvirons
   {
      PhFillModel	fFillModel	;
      PhPenModel	fPenModel	;
      PhTextParams	fTextParams	;
      PhParallelAttr	fParallelAttr	;
      CPoint	fPrevZoomScroll	;
      CoordPoint	fRulerOriginReserved	;
      DmPrintFlags	fPrintFlags	= 0;
      short	fAngleSnapsToACircle	= 0;
      short	fDimTextAlgn	= 0;
      Boolean fDimVisible= 0;
      Boolean fHasGrid= 0;
      Boolean fHideText= 0;
      Boolean fShowRulers= 0;
      Boolean fSnapToObject= 0;
      Boolean	fStandardRulers	= 0;
      Boolean	fHidePageBreaks	= 0;
      Boolean	fDimLineFlag	= 0;
      char	fPrevZoom	= 0;
      int      fPenShape= 0;
      short fDashMenuItem= 0;
      PhFilletAttr fFilletAttr;
      PhChamferAttr fChamferAttr;
      SInt16	fColorUsageSettings= 0	;
      SInt16	fReserved00	= 0;
      //SInt16	fReserved01	= 0;
      char	fReserved01	= 0;
      SInt16   fHideMetrics= 0;
      Boolean	fPrintWithBGColor	= 0;
      Boolean	fShowGuides	= 0;
      Boolean	fSmartSnap	= 0;
      Boolean	fSnapToGuides	= 0;
      char	fDimEndCap_1	= 0;
      char	fDimEndCap_2	= 0;
      SInt16	fDimType_2= 0	;
      char   fGridType= 0;
      double   fTrunk= 0.;
      CPoint   screenViewRectOrigin={};
      double   fZoomFactor= 0.;

      //PhWindowSplit	fTrunk	;
   };

   struct TPrInfo
   {
      short iDev;		/*Font mgr/QuickDraw device code*/
      short iVRes;	/*Resolution of device, in device coordinates*/
      short iHRes;	/*..note: V before H => compatable with Point.*/
      PhRect rPage;	/*The page (printable) rectangle in device coordinates.*/
   };

   struct TPrStl
   {
      short wDev;
      short iPageV;
      short iPageH;
      char/*SInt8*/ bPort;
      char/*TFeed*/ feed;
   };

   struct TPrXInfo
   {
      short iRowBytes;
      short iBandV;
      short iBandH;
      short iDevBytes;
      short iBands;
      char/*SInt8*/ bPatScale;
      char/*SInt8*/ bUlThick;
      char/*SInt8*/ bUlOffset;
      char/*SInt8*/ bUlShadow;
      char/*TScan*/ scan;
      char/*SInt8*/ bXInfoX;
   };

   struct TPrJob
   {
      short iFstPage;				/*Page Range.*/
      short iLstPage;
      short iCopies;				/*No. copies.*/
      char/*SInt8*/ bJDocLoop;	/*The Doc style: Draft, Spool, .., and ..*/
      Boolean fFromUsr;			/*Printing from an User's App (not PrApp) flag*/
      long/*PrIdleUPP*/ pIdleProc;/*The Proc called while waiting on IO etc.*/
      long/*StringPtr*/ pFileName;/*Spool File Name: NIL for default.*/
      short iFileVol;				/*Spool File vol, set to 0 initially*/
      char/*SInt8*/ bFileVers;	/*Spool File version, set to 0 initially*/
      char/*SInt8*/ bJobX;		/*An eXtra byte.*/
   };

   struct TPrint
   {
      short iPrVersion;	/*(2) Printing software version*/
      TPrInfo prInfo;		/*(14) the PrInfo data associated with the current style.*/
      PhRect rPaper;		/*(8) The paper rectangle [offset from rPage]*/
      TPrStl prStl;		/*(8)  This print request's style.*/
      TPrInfo prInfoPT;	/*(14)  Print Time Imaging metrics*/
      TPrXInfo prXInfo;	/*(16)  Print-time (expanded) Print info record.*/
      TPrJob prJob;		/*(20) The Print Job request (82)  Total of the above; 120-82 = 38 bytes needed to fill 120*/
      short printX[19];	/*Spare to fill to 120 bytes!*/
   };

   struct PhStdMacHeader
   {
      /* Std Mac Header - always 128 bytes */
      ulong fType;			/* OS Type						*/
      ushort fHeaderID;		/* 'MD'	for Dreams				*/
      ushort fVersion;		/* Version Number				*/
      long fUpgradeVersionDS; /* For future use */
      PhRect fWindowRect;  /* Window Rect in Global Coordinates */
      PhPoint fMViewPages; /* Number of Pages horizontally & vertically */
   };

   struct PhDocUnits
   {
      PhUnit fDefUnits;
      short fDefPlaces;
      short fDefAngleUnits;
      short fDefAnglePlaces;
      PhPoint fDefResolution;
      short fDefScaleIndex;
      short fDefSnapIndex;
   };

   struct PhDocStateBase: public PhStdMacHeader
   {
      PhDocUnits fDocUnits;
      PhDocStds fDocStds;
      DCoordPoint fRulerOrigin;
      PhActiveEnvirons fActiveEnvirons;
   };

   struct OffsetHeader
   {
      UInt32 uiOffset = 0;
      UInt32 uiLength = 0;

      bool read(struct pointer & pointer,memory & mem,CFile & file,int iMarker);
      bool write(struct pointer & pointer,memory & mem,CFile * pfileDst,int iMarker,CFile & fileSrc,int iLen = -1);
      bool write(struct pointer & pointer,CFile * pfileDst,int iMarker);
   };

   struct PhDocState: public PhDocStateBase
   {
      char   reserved[960 - sizeof(PhDocStateBase)];
   };

   struct PhPaletteEntry: public serializable
   {

      RGBFloat    ciRGB;
      short       ciUsage;
      short       ciTolerance;
      short       ciDataFields[3];

      virtual void operator()(serialize & serialize);


   };

   struct PhGuideEntry : public serializable
   {

      int            fType;
      float          fLineValue;

      virtual void operator()(serialize & serialize);

   };

   struct PhGuidesInfo : public serializable
   {

      UInt32                  numGuides;
      CArray < PhGuideEntry > guides;

      virtual void operator()(serialize & serialize);

   };

   struct PhMDShape: public serializable
   {
      eID            fID;
      SInt32           lObjectSize;
      SInt32           lInfoLocation;
      DCoordPoint	   cDatum;
      UInt64	      the64;
      UInt64	      cOwnerGroup;
      SInt64	      cRef;
      SInt32         theLong;//fObjectProps;
      UInt64         the64ObjectInfo; // the64ObjectInfo
      //UInt64	      fObjectInfo;
      //DAngle	      fRot;
      DAngle	      theAngle;
      PhPenModel	   cPenModel;
      SInt32         cShapeFlags;
      SInt32         fClassification;
      SInt64         cShapeReserved1 = 0;
      SInt32         cShapeReserved2 = 0;
      SInt32         cShapeReserved3 = 0;
      //SInt32         cShapeReserved4 = 0;

      virtual void operator()(serialize & serialize);

      static PhMDShape * create(eID eid);
   };

   struct PhMDShapeOk: public serializable
   {
      DCoordPoint	   fDatum;
      UInt64	      fPrev; // fPrev???
      UInt64	      fOwnerGroup;
      long	         fObjectProps;
      UInt64	      cRef;
      UInt64	      fObjectInfo;
      DAngle	      fRot;
      PhPenModel	   cPenModel;
      long	         fShapeFlags;
      long	         fClassification;
      SInt64         cShapeReserved1;
      ulong	         cShapeReserved2;
      ulong	         cShapeReserved3;

      virtual void operator()(serialize & serialize);

      static PhMDShapeOk * create(eID eid);
   };

   struct PhMDFillShape: public PhMDShape
   {
      PhFillModel	cFillModel;
      Boolean	cEvenOddFill;
      long	cFillShapeReserved = 0;
      char	cFillShapeReserved1 = 0;
      short	cFillShapeReserved2 = 0;

      virtual void operator()(serialize & serialize);
   };

   struct PhMDPolygon: public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	vertexCount	; //DDBPoint structs written vertexCount times
      CArray < DDBPoint > data;
      SInt64	cReserved	;
      char	cReserved1	;
      short	cReserved2	;
      long	cReserved3	;

      virtual void operator()(serialize & serialize);
   };

   struct PhMDMultiLineShape: public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	vertexCount	;
      long	ml_count	;
      CArray < DDBPoint > data;
      PhParallelAttr	fParallelAttr	;
      SInt64	fFillParallelShapeReserved	;
      long	theLong	;
      RGBColor ml_colors[1];
      virtual void operator()(serialize & serialize);
   };

   struct PhMDHatch: PhMDPolygon
   {
      virtual void operator()(serialize & serialize);
   };

   struct PhMDCircle: public PhMDFillShape
   {
      Method      method;
      DCoord      radius;
      CArray < DDBPoint > data;
      SInt64 fReserved;
      char fReserved1;
      short fReserved2;
      long fReserved3;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDRectangle: public PhMDFillShape
   {
      DDBPoint fPoint[4];
      SInt64	cReserved;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDRndRectangle: public PhMDRectangle
   {
      Corner	cCornerType	;
      DDBPoint	cDiagonals	;
      long	cReserved	;
      char	cReserved1	;
      short	cReserved2	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDEllipse: public PhMDFillShape
   {
      Method      method;
      DDBPoint	fCenter	;
      CoordPoint	radius	;
      SInt64	cReserved	;
      char	cReserved1	;
      short	cReserved2	;
      long	cReserved3	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDGroup: public PhMDShape
   {
      SInt64	itemsCount	;
      CArray < SInt64 >   pointers;
      SInt64	fReserved	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDParallelPolygon: public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	vertexCount	;
      CArray < DDBPoint > data	;
      PhParallelAttr	fParallelAttr	;
      SInt64	fFillParallelShapeReserved	= 0;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDWall : public PhMDFillShape
   {
      UInt64	segmentsCount;
      CArray < CPoint > segments	;
      UInt64	pointsCount;
      CArray < DDBPoint > data	;
      PhParallelAttr	fParallelAttr	;
      SInt64	fFillParallelShapeReserved	= 0;
      virtual void operator()(serialize & serialize);
   };


   struct PhMDLine: public PhMDShape
   {
      short	cConAngle	;
      double	mHeadArrowSize	;
      DDBPoint	fP1	;
      DDBPoint	fP2	;
      short	cReserved1	;
      long	cReserved2	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDDim: public PhMDLine
   {
      DimStatus	fStatus	;
      DimValue	cValue	;
      UInt64	cDimStrHdl	;
      SInt64	cReserved	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDDimension: public PhMDFillShape
   {
      DimStatus	fStatus	;
      CString	   fShapeFont	;
      float	      fShapeFontSize	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDAssocDim: public PhMDDimension
   {
      DimValue	      cValue;
      PhDimFormat2	   format;
//      PhDimFormat	   format;
      PhDimStd	      standard;
      CoordRect	   cTextExtent;
      CoordRect	   cDimStrDraw;
      CoordRect	   tol1Draw;
      CoordRect	   tol2Draw;
      CString	      dimStr;
      CString	      tol1Str	;
      CString	      tol2Str	;
      SInt64	      cReserved	= 0;
      //Size	         handleSize	; //String	DimLabel(if handleSize > 0)	;
      //CString        dimLabel;

      virtual void operator()(serialize & serialize);
   };

   struct PhMDLineDim: public PhMDAssocDim
   {
      DDBPoint	refPt1	;
      DDBPoint	refPt2	;
      DDBPoint	anchorPt	;
      DDBPoint	witnessStart1	;
      DDBPoint	witnessEnd1	;
      DDBPoint	witnessStart2	;
      DDBPoint	witnessEnd2	;
      DDBPoint	dimLineStart	;
      DDBPoint	dimLineEnd	;
      DDBPoint	extLineStart	;
      DDBPoint	extLineEnd	;
      DDBPoint	leaderLineStart	;
      DDBPoint	leaderLineEnd	;
      long	theLong	= 0;
      double	mHeadArrowSize	;
      double	mLineDimention	;
      SInt64	fReserved	= 0;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDCenterDim: public PhMDAssocDim
   {
      DDBPoint	cCenterPt	;
      DCoordPoint	cRadii	;
      SInt64	fReserved	= 0;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDAngularDim: public PhMDAssocDim
   {
      DDBPoint	ref1DBPoint	;
      DDBPoint	ref2DBPoint	;
      DDBPoint	lineStartDBPoint	;
      DDBPoint	lineEndDBPoint	;
      DDBPoint	anchorDBPoint	;
      DDBPoint	extDBPoint	;
      DDBPoint	centerDBPoint	;
      DDBPoint	witnessStart1DBPoint	;
      DDBPoint	witnessEnd1DBPoint	;
      DDBPoint	witnessStart2DBPoint	;
      DDBPoint	witnessEnd2DBPoint	;
      DCoord	cRadius	;
      DAngle	cStartAngle1	;
      DAngle	cArcAngle1	;
      short	cCaps1	;
      DAngle	cStartAngle2	;
      DAngle	cArcAngle2	;
      short	cCaps2	;
      double	mHeadArrowSize	;
      SInt64	fReserved	= 0;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDDimStr: public PhMDDimension
   {
      DDBPoint	cAnchorPt	;
      DDBPoint	cStrOffset	;
      SInt64	cObjHdl	;
      CString	 str;
      SInt64	cReserved	= 0;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDBitmap: public PhMDFillShape
   {
      DDBPoint data[4]	;
      PhRect	cSrcRect	;
      long	cMapHdlIndex	;
      Angle	fBitMapAngle	;
      SInt64	fReserved	= 0;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDArea: public PhMDDimension
   {
      DimValue	cAreaValue	;
      SInt64	cShapeHdl	;
      DDBPoint	cPosOrOffset	;
      CString	DimString	;
      short	cReserved1	= 0;
      long	cReserved2	= 0;
      SInt64	cReserved3	= 0;
      SInt64	cReserved4	= 0;
      virtual void operator()(serialize & serialize);
   };

   //struct PhMDTextScript : 
   //   public serializable
   //{
   //   SInt32	   scrpStartChar	;
   //   SInt16	scrpHeight	;
   //   SInt16	scrpAscent	;
   //   SInt16	scrpFont	;
   //   char	scrpFace	;
   //   char	theFaceChar	;
   //   SInt16	scrpSize	;
   //   RGBColor	scrpColor	;
   //   virtual void operator()(serialize & serialize);
   //};

   struct PhMDText: public  PhMDFillShape
   {
      //long	fInPixels	;
      //DCoordRect	fMargins	;
      //uchar	cParagraph	;
      //Boolean	cObjOpen	;
      //double	cfZoomFactor	;
      //TSpace	cTextSpace	;
      //short	cTextJust	;
      //DCoordRect	destRect	;
      //DCoordRect	viewRect	;
      //UInt64	teLength	;
      //DDBPoint data[4];
      //CString chars;

      //UInt64	scrCount	;
      ////If scrCount != 0 then the following fields(colored) are written scrCount times	;
      //CArray<PhMDTextScript> scripta;

      //SInt64	fReserved	;
      //short	fReserved2	;

      uchar cParagraph;	//either kCaption,kParagraph
      THorizTextJust cTextJust;//	short	Text justification - THorizTextJust enum

      DDBPoint data[4];
      UInt64 textDataSize;
      CString m_strRtf;

      virtual void operator()(serialize & serialize);
   };

   struct PhMDSpecificBezier: public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	numPoints	;
      CArray<DDBPoint> data; // DDBPoint written numPoints times
      //. If two points in a row have flag RealVertex it means bezier MoveTo element. That is beginning of subpath of the path.
      virtual void operator()(serialize & serialize);
   };

   struct PhMDCubic:
      public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	vertexCount	;
      CArray <DDBPoint> data	; //  DDBPoint written(3 * vertexCount + 1) times
      virtual void operator()(serialize & serialize);
   };

   struct PhMDFreeSpline:
      public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	vertexCount	;
      CArray < DDBPoint > data;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDPolySpline:
      public PhMDFillShape
   {
      Boolean	cClosed	;
      SInt64	vertexCount	;
      CArray < DDBPoint > data;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDProtoArc:
      public PhMDFillShape
   {
      DCoordPoint	Radii	;
      DAngle	cStartAng	;
      DAngle	cEndAng	;
      Boolean	cBackwards	;
      DDBPoint	StartPt	;
      DDBPoint	EndPt	;
      DDBPoint	ThirdPt	;
      Method	method	;
      DDBPoint	fCenter	;
      SInt64	cProtoReserved	;
      short	cProtoReserved1	;
      long	cProtoReserved2	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDArc: public PhMDProtoArc
   {
      SInt64	cReserved	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDParallelLine: public
      PhMDShape
   {
      PhParallelAttr	fParallelAttr	;
      SInt64	fFillParallelShapeReserved	;
      short	cConAngle	;
      DDBPoint data[6]	;
      long	cReserved	;
      short	cReserved1	;
      virtual void operator()(serialize & serialize);
   };

   struct PhMDViewPort:
      public PhMDFillShape
   {
      DCoordPoint	fModelCentre	;
      DCoord	fModelHeight	;
      long	fViewport_type	;
      long	numPoints	;
      CArray < DDBPoint > data	;
      UInt64	cReserved1	;
      UInt64	fReserved2	;
      virtual void operator()(serialize & serialize);
   };

   //struct CMDGroupShape;
   //struct CMDGroupShape;

   //PhMDMultiLineShape multiline;
   //PhMDHatch hatch;
   //PhMDCircle circle;
   //PhMDRectangle rect;
   //PhMDRndRectangle rndrect;
   //PhMDPolygonShape polygon;
   //PhMDEllipse ellipse;
   //PhMDGroupShape group[];
   //PhMDMetaPolyShape poly[];
   //PhMDParallelPolygon parallelpolygon;
   //PhMDLineShape line;
   //PhMDDimShape dim;
   //PhMDLineDim linedim;
   //PhMDCenterDim centerdim;
   //PhMDAngularDim angulardim;
   //PhMDDimStr dimstr;
   //PhMDBitmap bitmap;
   //PhMDArea area;
   //PhMDText text;
   //PhMDSpecificBezierShape bezier;
   //PhMDCubicShape cubic;
   //PhMDFreeSplineShape freespline;
   //PhMDPolySplineShape polyspline;
   //PhMDEArcShape arc;
   //PhMDParallelLine parallelline;
   //PhMDViewPort  viewport;
   //PhMDUnknownObject unknown;
   ////   };
   ////};

   struct PhMDMetaPoly: public PhMDFillShape
   {
      DCoordPoint	cStartPoint	;
      Boolean	cClosed	;
      long	cNumValidMembers	;
      long	cNumClosedMembers	;
      SInt64	itemsCount	;
      CArray < SInt64 > shapes;
      SInt64	fReserved	;
      char	fReserved1	;
      short	fReserved2	;
      virtual void operator()(serialize & serialize);
   };

   struct PhShapeInfo
   {
      CString	fF1Text	;
      CString	fF2Text	;
      CString	fF3Text	;
      CString	fF4Text	;
      CString	fF5Text	;
      virtual void operator()(serialize & serialize);
   };

   struct PhShapeProp
   {
      UInt64	propCount	;
      PhPropListElem data[1];
   };
   //#define	kPhMaxLayerNameLen		16		// max name length for layer
#define	kPhActive				1		// layer mode
#define	kPhVisible				2
#define	kPhSelectable			4
#define	kPhScaled				8
#define	kPhNotGrayed			16
#define	kPhFillsOn				32
#define	kPhDeleteThisLayer		64
#define	kPhDummy1				128

   struct LayerStruct: public serializable
   {
      UInt64	fNumShapes	;
      UInt64	fLayerOffset	;
      UInt64	fLayerLength	;
      SInt32	fReserved1	;
      SInt32	fReserved2	;
      SInt32	fReserved3	;
      SInt32	fReserved4	;
      SInt32	fReserved5	;
      SInt32	fReserved6	;
      ushort	fReserved7	;
      uchar    fReserved8;
      uchar    fAdditionalFlags;
      SInt32	fLayerActiveFlag;
      SInt32   fScaleIndex;
      LongRatio   fCustomRatio;
      SInt32	fSnapIndex	;
      ushort	fLayerFlags	;
      CString	fCFRLayerName	;

      virtual void operator()(serialize & serialize);
   };

   const Angle	 k5				= float(5);
   const Angle	 k15			= float(15);
   const Angle	 k20			= float(20);
   const Angle	 k30			= float(30);
   const Angle	 k45			= float(45);
   const Angle	 k70			= float(70);
   const Angle	 k90			= float(90);
   const Angle	 k135			= float(135);
   const Angle	 k180			= float(180);
   const Angle	 k225			= float(225);
   const Angle	 k270			= float(270);
   const Angle	 k315			= float(315);
   const Angle	 k360			= float(360);
   const Angle	 k450			= float(450);
   struct TSinCos
   {
      TSinCos() {} ;
      TSinCos(double pAngle)
      {
         if(pAngle == 0.0)
         {
            fSin=0.0;
            fCos=1.0;
         }
         else if(pAngle == 90.0)
         {
            fSin=1.0;
            fCos=0.0;
         }
         else if(pAngle == 180.0)
         {
            fSin=0.0;
            fCos=-1.0;
         }
         else if(pAngle == 270.0)
         {
            fSin=-1.0;
            fCos=0.0;
         }
         else
         {
            double radian=pAngle*pi / 180.0;
            fSin=sin(radian);
            fCos=cos(radian);
         }
      }

      double fSin ;
      double fCos ;
   };

   inline CoordPoint mtRotPoint(const CoordPoint& pPoint,const TSinCos& sinCos)
   {
      CoordPoint result;

      double h = pPoint.x.m_f;
      double v = pPoint.y.m_f;

      result.x.m_f = (float) (h * sinCos.fCos - v * sinCos.fSin);
      result.y.m_f = (float) (h * sinCos.fSin + v * sinCos.fCos);

      return result;
   }

   inline CoordPoint mtRotPoint(const CoordPoint& pPoint,const TSinCos& sinCos,const CoordPoint& pRefPoint)
   {
      CoordPoint result;

      double h = pPoint.x.m_f - pRefPoint.x.m_f;
      double v = pPoint.y.m_f - pRefPoint.y.m_f;

      result.x.m_f = (float) (h * sinCos.fCos - v * sinCos.fSin + pRefPoint.x.m_f);
      result.y.m_f = (float) (h * sinCos.fSin + v * sinCos.fCos + pRefPoint.y.m_f);

      return result;
   }

   inline CoordPoint mtRotPoint(const CoordPoint& pPoint,Angle angle)
   {
      if(angle == 0.f)
         return pPoint;
      TSinCos sinCos(angle);
      return mtRotPoint(pPoint,sinCos);
   }

   inline CoordPoint mtRotPoint(const CoordPoint& pPoint,Angle angle,const CoordPoint&	pRefPoint)
   {
      if(angle == 0.f)
         return pPoint;
      TSinCos sinCos(angle);
      return mtRotPoint(pPoint,sinCos,pRefPoint);
   }
   inline Angle mtATan2(const CoordPoint & p)
   {
      if(p.x == 0.f && p.y == 0.f)
         return 0.f;
      Angle vAngle = (float)(::atan2(p.y.m_f,p.x.m_f) * kRadToAng);
      vAngle = 360.0f - vAngle;
      if(vAngle >= 360.0f)
         vAngle -= 360.0f;
      return vAngle;
   }

   inline Angle mtAngle(const CoordPoint& p1,const CoordPoint& p2)
   {
      return mtATan2(p2 - p1);
   }

   inline Angle mtFixAngle(Angle pAngle)
   {
      while(pAngle < 0.f)
         pAngle+=360.f;
      while(pAngle >= 360.f)
         pAngle-=360.f;
      return pAngle;
   }

   inline DCoordPoint mtRotPoint(const DCoordPoint& pPoint,const TSinCos& sinCos)
   {
      DCoordPoint result;

      double h = pPoint.x.m_d;
      double v = pPoint.y.m_d;

      result.x.m_d = h * sinCos.fCos - v * sinCos.fSin;
      result.y.m_d = h * sinCos.fSin + v * sinCos.fCos;

      return result;
   }

   inline DCoordPoint mtRotPoint(const DCoordPoint& pPoint,const TSinCos& sinCos,const DCoordPoint& pRefPoint)
   {
      DCoordPoint result;

      double h = pPoint.x.m_d - pRefPoint.x.m_d;
      double v = pPoint.y.m_d - pRefPoint.y.m_d;

      result.x.m_d = h * sinCos.fCos - v * sinCos.fSin + pRefPoint.x.m_d;
      result.y.m_d = h * sinCos.fSin + v * sinCos.fCos + pRefPoint.y.m_d;

      return result;
   }

   inline DCoordPoint mtRotPoint(const DCoordPoint& pPoint,DAngle angle)
   {
      if(angle == 0.0)
         return pPoint;
      TSinCos sinCos(angle);
      return mtRotPoint(pPoint,sinCos);
   }

   inline DCoordPoint mtRotPoint(const DCoordPoint& pPoint,DAngle angle,const DCoordPoint&	pRefPoint)
   {
      if(angle == 0.0)
         return pPoint;
      TSinCos sinCos(angle);
      return mtRotPoint(pPoint,sinCos,pRefPoint);
   }

   inline DCoordPoint Average(const DCoordPoint& p1,const DCoordPoint& p2)
   {
      return p1 + ((p2 - p1) / 2);
   }

   inline DAngle mtATan2NoFix(const DCoordPoint & p)
   {
      if(p.x == 0. && p.y == 0.)
         return 0.;
      DAngle vAngle = -(double)(::atan2(p.y.m_d,p.x.m_d) * kRadToAng);
      return vAngle;
   }

   inline DAngle mtATan2(const DCoordPoint & p)
   {
      if(p.x == 0. && p.y == 0.)
         return 0.;
      DAngle vAngle = (double)(::atan2(p.y.m_d,p.x.m_d) * kRadToAng);
      vAngle = 360.0 - vAngle;
      if(vAngle >= 360.0)
         vAngle -= 360.0;
      return vAngle;
   }

   inline DAngle mtAngle(const DCoordPoint& p1,const DCoordPoint& p2)
   {
      return mtATan2(p2 - p1);
   }
   inline DAngle mtAngleNoFix(const DCoordPoint& p1,const DCoordPoint& p2)
   {
      return mtATan2NoFix(p2 - p1);
   }

   inline DAngle mtFixAngle(DAngle pAngle)
   {
      while(pAngle < 0.)
         pAngle+=360.;
      while(pAngle > 360.)
         pAngle-=360.;
      return pAngle;
   }
   inline DAngle mtFixAnglePi(DAngle pAngle)
   {
      while(pAngle > 180.0)
         pAngle-=360.;
      while(pAngle < -180.0)
         pAngle+=360.;
      return pAngle;
   }


   class no_swap_context
   {
   public:

      serialize * m_pserialize;
      bool        m_bBefore;

      no_swap_context(serialize * pserialize);

      ~no_swap_context();
   };

   class serialize
   {
   public:

      PhIOContext * m_piocontext;
      CFile *     m_pfile;
      bool        m_bWrite;
      pointer      m_pointer;

      LongRatio   m_ratio;

      serialize():
         m_pointer(this)
      {
         m_piocontext = NULL;
         m_ratio.fD = 1;
         m_ratio.fN = 1;
         m_bWrite = false;
      }

      serialize(CFile * pfile):
         m_pointer(this),
         m_pfile(pfile)
      {
         m_piocontext = NULL;
         m_bWrite = pfile != NULL;
         m_ratio.fD = 1;
         m_ratio.fN = 1;
      }

      void operator()(CPackedDIB * pdib);

      void operator()(CGradientPack * pgradient);

      void operator()(CPackedItem & item);

      void operator()(serializable & serializable);

      void operator()(serializable * pserializable);

      void operator()(char & ch)
      {
         if(m_bWrite)
         {
            m_pfile->Write(&ch,sizeof(ch));
         }
         else
         {
            ch = m_pointer.ch();
         }
      }

      template < typename OFFSET, typename LENGTH >
      void write(OFFSET & offset,LENGTH & length, CFile * pfileSrc)
      {

         memory mem;

         mem.allocate(pfileSrc->GetLength());

         pfileSrc->SeekToBegin();

         pfileSrc->Read(mem.get_data(),mem.get_size());

         offset = (OFFSET) m_pfile->GetPosition();

         length = mem.get_size();

         m_pfile->Write(mem.get_data(),mem.get_size());

      }

      void operator()(unsigned char & uch)
      {
         if(m_bWrite)
         {
            m_pfile->Write(&uch,sizeof(uch));
         }
         else
         {
            uch = m_pointer.uch();
         }
      }

      void operator()(short & sh)
      {
         if(m_bWrite)
         {
            short shSave = sh;
            if(m_pointer.m_bSwap)
            {
               swap16(&shSave);
            }
            m_pfile->Write(&shSave,sizeof(shSave));
         }
         else
         {
            m_pointer.swap(sh);
         }
      }

      void operator()(unsigned short & ush)
      {
         if(m_bWrite)
         {
            unsigned short ushSave = ush;
            if(m_pointer.m_bSwap)
            {
               swap16(&ushSave);
            }
            m_pfile->Write(&ushSave,sizeof(ushSave));
         }
         else
         {
            m_pointer.swap(ush);
         }
      }

      void operator()(int & i)
      {
         if(m_bWrite)
         {
            int iSave = i;
            if(m_pointer.m_bSwap)
            {
               swap32(&iSave);
            }
            m_pfile->Write(&iSave,sizeof(iSave));
         }
         else
         {
            m_pointer.swap(i);
         }
      }

      void operator()(unsigned int & ui)
      {
         if(m_bWrite)
         {
            unsigned int uiSave = ui;
            if(m_pointer.m_bSwap)
            {
               swap32(&uiSave);
            }
            m_pfile->Write(&uiSave,sizeof(uiSave));
         }
         else
         {
            m_pointer.swap(ui);
         }
      }

      void operator()(long & l)
      {
         if(m_bWrite)
         {
            long lSave = l;
            if(m_pointer.m_bSwap)
            {
               swap32(&lSave);
            }
            m_pfile->Write(&lSave,sizeof(lSave));
         }
         else
         {
            m_pointer.swap(l);
         }
      }

      void operator()(unsigned long & ul)
      {
         if(m_bWrite)
         {
            unsigned long ulSave = ul;
            if(m_pointer.m_bSwap)
            {
               swap32(&ulSave);
            }
            m_pfile->Write(&ulSave,sizeof(ulSave));
         }
         else
         {
            m_pointer.swap(ul);
         }
      }

      void operator()(__int64 & i)
      {
         if(m_bWrite)
         {
            __int64 iSave = i;
            if(m_pointer.m_bSwap)
            {
               swap64(&iSave);
            }
            m_pfile->Write(&iSave,sizeof(iSave));
         }
         else
         {
            m_pointer.swap(i);
         }
      }

      void operator()(unsigned __int64 & ui)
      {
         if(m_bWrite)
         {
            unsigned __int64 uiSave = ui;
            if(m_pointer.m_bSwap)
            {
               swap64(&uiSave);
            }
            m_pfile->Write(&uiSave,sizeof(uiSave));
         }
         else
         {
            m_pointer.swap(ui);
         }
      }

      void operator()(float & f)
      {
         if(m_bWrite)
         {
            float fSave = f;
            if(m_pointer.m_bSwap)
            {
               swap32(&fSave);
            }
            m_pfile->Write(&fSave,sizeof(fSave));
         }
         else
         {
            m_pointer.swap(f);
         }
      }

      void operator()(double & d)
      {
         if(m_bWrite)
         {
            double dSave = d;
            if(m_pointer.m_bSwap)
            {
               swap64(&dSave);
            }
            m_pfile->Write(&dSave,sizeof(dSave));
         }
         else
         {
            m_pointer.swap(d);
         }
      }

      template < typename T >
      void operator()(CArray < T > & a,int iSize)
      {
         serialize_array(a,iSize);
      }

      template < typename ARRAY >
      void serialize_array(ARRAY & a,int iSize)
      {
         if(m_bWrite)
         {

            ASSERT(iSize == a.GetSize());

            serialize_static_array(a.GetData(),iSize);

         }
         else
         {
            a.SetSize(iSize);

            serialize_static_array(a.GetData(),iSize);
         }
      }

      template < typename SIZE >
      void serialize_string(CString & str,SIZE & s)
      {

         external_type_context externaltypecontext(this);

         if(m_bWrite)
         {

            s = str.GetLength();

            operator()(s);

            if(s > 0)
            {

               m_pfile->Write((const char *)str,s);

            }

         }
         else
         {

            operator()(s);

            if(s > 0)
            {

               int iRead = m_pfile->Read((char *)str.GetBufferSetLength(s),s);

               if(iRead != s)
               {

                  throw "error";

               }

               str.ReleaseBufferSetLength(iRead);

            }


         }

      }


      template < typename ARRAY >
      void serialize_static_array(ARRAY * pa,int iSize)
      {
         external_type_context externaltypecontext(this);

         for(int i = 0; i < iSize; i++)
         {
            operator()(pa[i]);
         }
      }

      void operator()(CString & str);
      void operator()(CString & str,long l);

      void s1(CString & str);

      void operator()(CPoint & p)
      {
         operator()(p.x);
         operator()(p.y);

      }

      void operator()(DCoord & pd);
      void operator()(DCoordPoint & point);
      void operator()(DCoordRect & rect);

      void operator()(DDBPoint & ppoint);

      void operator()(Coord & ppoint);
      void operator()(CoordPoint & ppoint);
      void operator()(CoordRect & prect);
      void operator()(LongRatio & pratio);

      void operator()(RGBColor & rgb);
      void operator()(RGBFloat & prgba);
      void operator()(RGBAFloat & prgba);
      void operator()(RGBAPos & prgba);

      void operator()(PhRect & prect);
      void operator()(PhPoint & prect);

//      void operator()(PhMDTextScript & script);

      void operator()(PhTextParams * ptextParams);
      void operator()(PhParallelAttr * pparallelAttr);
      void operator()(PhChamferAttr * pattr);
      void operator()(PhFilletAttr * pattr);
      void operator()(PhWindowSplit * psplit);
      void operator()(PhDocUnits * pdocUnits);
      void operator()(PhDimStd * pdimStd);
      void operator()(PhDimFormat * pdimFormat);
      void operator()(PhDimFormat2 * pdimFormat); // MD70 - Infinisys Ltd.
      void operator()(PhDocStds * pdocStds);
      void operator()(PhActiveEnvirons * pactiveEnvirons);
      void operator()(PhStdMacHeader * pheader);
      void operator()(PhDocState * pstate);
      void operator()(PhFillModel * pstate);
      void operator()(PhPenModel * pstate);

      void operator()(PhGenericPatternHeader * pheader);
      void operator()(PatternBase * pbase);
      void operator()(PixelPattern * ppattern);
      void operator()(BlendPattern * ppattern);
   };

   inline void serialize::operator()(serializable & serializable)
   {
      serializable(*this);
   }

   inline void serialize::operator()(serializable * pserializable)
   {
      pserializable->operator()(*this);
   }

   inline complex_type_context::complex_type_context(serialize * pserialize)
   {
      m_pserialize = pserialize;
      m_pserialize->m_pointer.m_copya.Add(pointer::copy(true));
      m_pserialize->m_pointer.m_bComplexOnStack = true;
   }

   inline complex_type_context::~complex_type_context()
   {
      pointer::copy copy = m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()];
      ASSERT(copy.m_bComplex);
      m_pserialize->m_pointer.m_copya.RemoveAt(m_pserialize->m_pointer.m_copya.GetUpperBound());
      if(m_pserialize->m_pointer.m_copya.GetCount() > 0)
      {
         m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()].m_p = NULL;
         m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()].m_iSize = 0;
      }
      bool bComplexOnStack = false;
      for(int i = 0; i < m_pserialize->m_pointer.m_copya.GetCount(); i++)
      {
         if(m_pserialize->m_pointer.m_copya[i].m_bComplex)
         {
            bComplexOnStack = true;
            break;
         }
      }
      m_pserialize->m_pointer.m_bComplexOnStack = bComplexOnStack;
   }

   inline raw_type_context::raw_type_context(serialize * pserialize)
   {
      m_pserialize = pserialize;
      pointer::copy copy(false);
      //if(m_pserialize->m_pointer.m_copya.has_elements() && m_pserialize->m_pointer.m_copya.last().m_p != NULL)
      //{
      //  copy.m_p = m_pserialize->m_pointer.m_copya.last().m_p + m_pserialize->m_pointer.m_copya.last().m_iSize;
      //copy.m_iSize = 0;
      //}
      m_pserialize->m_pointer.m_copya.Add(copy);
   }

   inline raw_type_context::~raw_type_context()
   {
      pointer::copy copy = m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()];
      ASSERT(!copy.m_bComplex);
      m_pserialize->m_pointer.m_copya.RemoveAt(m_pserialize->m_pointer.m_copya.GetUpperBound());
      if(m_pserialize->m_pointer.m_copya.GetCount() > 0 && copy.m_p != NULL)
      {
         m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()].m_p = copy.m_p + copy.m_iSize;
         m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()].m_iSize = 0;
      }
   }

   inline external_type_context::external_type_context(serialize * pserialize)
   {
      m_pserialize = pserialize;
      m_pserialize->m_pointer.m_copya.Add(pointer::copy(false,true));
   }

   inline external_type_context::~external_type_context()
   {
      pointer::copy copy = m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()];
      ASSERT(copy.m_bExternal);
      m_pserialize->m_pointer.m_copya.RemoveAt(m_pserialize->m_pointer.m_copya.GetUpperBound());
      if(m_pserialize->m_pointer.m_copya.GetCount() > 0)
      {
         m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()].m_p = NULL;
         m_pserialize->m_pointer.m_copya[m_pserialize->m_pointer.m_copya.GetUpperBound()].m_iSize = 0;
      }
   }

   inline no_swap_context::no_swap_context(serialize * pserialize)
   {
      m_pserialize = pserialize;
      m_bBefore = m_pserialize->m_pointer.m_bSwap;
      m_pserialize->m_pointer.m_bSwap = false;
   }

   inline no_swap_context::~no_swap_context()
   {
      m_pserialize->m_pointer.m_bSwap = m_bBefore;
   }

   struct PhIOContext
   {
#if DEV_DEBUG
      CMDShapeArray groupaDebug;
      UInt64Array groupa64Debug;
      CString strDebug;
#endif
      serialize   serialize;
      ULONGLONG ullBeg;
      CFile *f;
      CLayer * player = NULL;
      CMDShape* pGroup;
      PhMDShape* pPhShape;
      CDWordArray embedDIBs;
      CWordArray embedDIBsIndex;
      int m_numDIBPatterns;
      CDWordArray bitmaps;
      CDWordArray metaMembers;
      CMap<short,short,CString,CString> fontMap;
      CMap<short,short,int,int> fontCharset;
      CStringArray fontInUse;
      CArray < int > fontCS;
      CPalette *pPalette;
      CPCDraftDoc* pDoc;
      long nObjectIndex;
      CLinkVertexList* pLinkVertexList;
      union
      {
         BYTE cCharset[400];
         DWORD dwProperty[100];
      };
      int nCount;

      PhIOContext(CFile* pf): f(pf),pGroup(NULL),nCount(0),serialize(pf) { serialize.m_piocontext = this; }
      short UseFont(const CString& sFont,int iCS);
      void ConvertPenModel(const PenModel& pm,PhPenModel& phpm);
      void ConvertPhPenModel(const PhPenModel& phpm,PenModel& pm);
      void ConvertFillModel(const FillModel& fm,PhFillModel& phfm);
      void ConvertPhFillModel(const PhFillModel& phfm,FillModel& fm);
      void ConvertParaAttr(const ParallelAttrs& pa,PhParallelAttr& phpa);
      void ConvertPhParaAttr(const PhParallelAttr& phpa,ParallelAttrs& pa);
      void ConvertTextParams(const TextParams& tp,PhTextParams& phtp);
      void ConvertPhTextParams(const PhTextParams& phtp,TextParams& tp);
      void ConvertDimStd(const DimStandard& std,PhDimStd& phstd);
      void ConvertPhDimStd(const PhDimStd& phstd,DimStandard& std);
      void ConvertDimFmt(const DimFormat& fmt,PhDimFormat& phfmt);
      void ConvertPhDimFmt(const PhDimFormat& phfmt,DimFormat& fmt);// MD70 - Infinisys Ltd.
      void ConvertDimFmt(const DimFormat& fmt,PhDimFormat2& phfmt);// MD70 - Infinisys Ltd.
      void ConvertPhDimFmt(const PhDimFormat2& phfmt,DimFormat& fmt);
      void ConvertLogFont(const LOGFONT& lf,long& pFont,long& pSize,Style& pStyle);
      void ConvertPhFont(short nFont,short nSize,Style nStyle,LOGFONT& lf);
      void ConvertLogFont(const LOGFONT& lf,CString & strFontName, float & fSize,Style& pStyle);// MD70 - Infinisys Ltd.
      void ConvertFont(CString strFontName, float fSize,LOGFONT& lf); // MD70 - Infinisys Ltd.
      void ConvertC2P(const CString& str,char* pBuf,int nMaxSize);
   };

#define	kPhTextRow			1
#define kPhCriteriaRow		2
#define	kPhFunctionRow		3

   class FlatPageFormat:public serializable
   {
   public:
      Size theSize; //     If theSize != 0,the following info is stored
      SInt32 handleLength;
      //Chunk of data  handleLength bytes long(if handleLength != 0) (serialized in MacOS PMPageFormat  flattened data,undocumented)
      array < unsigned char > data;
      virtual void operator()(serialize & serialize);
   };
      
   class FlatPrintSettings:public serializable
      {
      public:
         Size theSize; //     If theSize != 0,the following info is stored
         SInt32 handleLength;
         // Chunk of data  handleLength bytes long(if handleLength != 0) (serialized in MacOS PMPrintSettings  flattened data,undocumented)
         array < unsigned char > data;
         virtual void operator()(serialize & serialize);
   };


   class PhReportRowInfo :public serializable
   {
   public:
     
      uchar fRowFlags;
      uchar fRowType;
      short fRowID;
      long theLong;

      virtual void operator()(serialize & serialize);

   };

   class PhTextRow:public serializable
   {
   public:
      Size size;
      Size sizeText;
      ushort fChanged;
      uchar fRowType;
      uchar fSpare;
      char	fJust[6];
      ushort fUseStyle;
      char unused[28];
      CString	Str[6];

         virtual void operator()(serialize & serialize);

   };

   class PhCriteriaRow:public serializable
   {
   public:

      Size size;
      ushort fChanged;
      uchar fRowType;
      uchar fSpare;
         short fFieldRefNum[6];

         virtual void operator()(serialize & serialize);

   };

   class PhFunctionRow:public serializable
   {
   public:

      Size size;
      Size sizeText;
      ushort fChanged;
      uchar fRowType;
      uchar fSpare;
      char fJust[6];
      double fResult;
      short dummy;
      ushort fUseStyle;
      char unused[18];
      CString  Str[6];

      virtual void operator()(serialize & serialize);
   };



   class PhReport:public serializable
   {
   public:
      short fVersion;
      CString       fCFRTitle ;
      PhTextParams fDefaultTextParams;
      short fDefaultColumnWidth;
      short fColumnWidths[6];
      short fRowHeight;
      short fNumRows;
      long  fLinkInfo_fLinkType;
      SInt16 fLinkInfo_fLinkFileSpec_vRefNum;
      UInt32 fLinkInfo_fLinkFileSpec_parID;
      unsigned char linkNameLength;
      CString fLinkInfo_fLinkFileSpec_name;
      OSErr fLinkInfo_fLastLinkError;
      short fLinkInfo_fUnitType;
      char reservedfiller[32];
      FlatPageFormat	FlatPageFormat;
         FlatPrintSettings	FlatPrintSettings;
         long   rowInfoCount;
         array < PhReportRowInfo > rowinfoa;
         long         theLong;
         array < serializable * > rowa;


         virtual ~PhReport();

         virtual void operator()(serialize & serialize);

   };


#pragma pack()

      //   extern unsigned long reportRowType[3];

   inline const char * shape_text(eID eid)
   {

      switch(eid)
      {
      case ::formatMD70::IDLine:
         return "Line";
      case ::formatMD70::IDLine2:
         return "Line2";
      case ::formatMD70::IDFreehand:
         return "Freehand";
      case ::formatMD70::IDPolygon:
         return "Polygon";
      case ::formatMD70::IDRectCenter:
         return "RectangleCenter";
      case ::formatMD70::IDRectDiagonal:
         return "RectangleDiagonal";
      case ::formatMD70::IDRndRectangle:
         return "RndRectangle";
      case ::formatMD70::IDFreeSpline:
         return "FreeSpline";
      case ::formatMD70::IDPolySpline:
         return "PolySpline";
      case ::formatMD70::IDCircle:
         return "Circle";
      case ::formatMD70::IDArc:
         return "Arc";
      case ::formatMD70::ID3Arc:
         return "3Arc";
      case ::formatMD70::IDEArc:
         return "EArc";
      case ::formatMD70::IDEllipse:
         return "Ellipse";
      case ::formatMD70::IDBezier:
         return "SpecificBezier";
      case ::formatMD70::IDCubic:
         return "Cubic";
      case ::formatMD70::IDText:
         return "Text";
      case ::formatMD70::IDWall:
         return "Wall";
      case ::formatMD70::IDBitmap:
         return "Bitmap";
      case ::formatMD70::IDGroup:
         return "Group";
      case ::formatMD70::IDMetaPoly:
         return "MetaPoly";
      case ::formatMD70::IDHLineDim:
         return "HLineDim";
      case ::formatMD70::IDVLineDim:
         return "VLineDim";
      case ::formatMD70::IDSLineDim:
         return "SLineDim";
      case ::formatMD70::IDPLineDim:
         return "PLineDim";
      case ::formatMD70::IDRRadialDim:
         return "RRadialDim";
      case ::formatMD70::IDDRadialDim:
         return "DRadialDim";
      case ::formatMD70::IDCenterDim:
         return "CenterDim";
      case ::formatMD70::IDAngularDim:
         return "AngularDim";
      case ::formatMD70::IDDimPt2Pt:
         return "DimPt2Pt";
      case ::formatMD70::IDDimString:
         return "DimString";
      case ::formatMD70::IDArea:
         return "Area";
//      case ::formatMD70::IDParallelLine:
  //       return "ParallelLine";
    //  case ::formatMD70::IDParallelPolygon:
      //   return "ParallelPolygon";
      case ::formatMD70::IDHatch:
         return "Hatch";
      default:
         return "(unknown shape)";
      };

   }



} // namespace formatMD70