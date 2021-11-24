#include "stdafx.h"
#include "WrFixed.h"

#define WINDOWS
#define SUCCESS ((short) 0)
#define FAILURE ((short) -1)

#define ODD(X)	((X) & 1)		/* Returns true if X is odd */
#define EVEN(X) (!((X) & 1))	/* Returns true if X is even */
#define COM(X) (~(X)) 		 	/* XyWrite users:  hidden tilda */

#define	lmemset(p,c,n)		(memset((p),(c),(n)))
#define WARNING_SIZE	400		/* size of hWarnings buffer */
#define MAXKIDS		10		/* any more than this are ignored */
#define MAXVAL 		2000
#define MAX_RECOMMENDED_STRIP	(12*1024L)
#define NBLOCKS		4
#define	T2ENTRIES	104		/* 104 entries in each of the 4 tables */
#define EIGHT		8	/* I know this seems silly, but it is such an important
				 * constant in this code that I thought I'd give it a name
				 */
#define WHITE		0
#define BLACK		1
#define TCLIM		64	/* "terminal code" limit -- actually, first muc */
#define LUTBITS		13	/* maximum number of bits in ccitt code words */
#define LUTLEN		(1<<LUTBITS)
#define ADJUST		(32-LUTBITS)
#define L13			(~(~0<<LUTBITS))	/* hidden tildes */
#define NBLOCKS		4

#ifdef WINDOWS
#define WHITEBYTE	((BYTE)(0xff))
#define BLACKBYTE	((BYTE)0)
#endif /* WINDOWS */

#ifdef MACINTOSH
#define WHITEBYTE	((BYTE)0)
#define BLACKBYTE	((BYTE)(0xff))
#define RESOUshortETYPE	'Tif2'
#define ID_WSTR		1
#define ID_BSTR		2
#define ID_WLEN		3
#define ID_BLEN		4
#endif /* MACINTOSH */

#define SAMPLESPERPIXEL	3
#define PIXELSPERGROUP	3
#define ADDOVERFLOW	12	/* for HorizAdd(). was 9, but that isn't enough for HorizRgbAdd */
#define CHUNKSIZE 32768L

/*-----------------------------------------------------------------------*/
/* Compression type defines:
 */
#define PACKINTOBYTES	1
#define CCITT1D			2
#define LZW				5
#define TIFFPACKBITS	32773	/* MacPaint scheme in TIFF file */

/* photometric interpretation values
 */
#define WHITEZERO		0
#define BLACKZERO		1
#define TIFFRGB			2
#define PALETTECOLOR	3

/* planashortonfiguration values
 */
#define CHUNKY	1	/* RGB RGB RGB */
#define PLANAR	2	/* RRR GGG BBB, in separate planes */

/* predictor values
 */
#define PREDICTOR_NONE		1
#define PREDICTOR_HDIFF		2	/* not defined in TIFF 5.0 */

/*---------------------------------------------------*/

/* TIFF data types
 */
#define TIFFBYTE		1
#define TIFFASCII		2
#define TIFFSHORT		3
#define TIFFLONG		4
#define TIFFRATIONAL	5
#define TIFFSIGNED		6
#define TIFFFLOAT		32768/* manufactured type -- not found in TIFF file */

/* TIFF tag constants
 */
#define TGNEWSUBFILETYPE			254
#define TGOLDSUBFILETYPE			255
#define TGIMAGEWIDTH				256
#define TGIMAGELENGTH				257
#define TGBITSPERSAMPLE				258
#define TGCOMPRESSION				259

#define TGPHOTOMETRICINTERPRETATION	262
#define TGTHRESHHOLDING				263
#define TGCELLWIDTH					264
#define TGCELLLENGTH				265
#define TGFILLORDER					266

#define TGDOCUMENTNAME				269
#define TGIMAGEDESCRIPTION			270
#define TGMAKE						271
#define TGMODEL						272
#define TGSTRIPOFFSETS				273
#define TGORIENTATION				274

#define TGSAMPLESPERPIXEL			277
#define TGROWSPERSTRIP				278
#define TGSTRIPBYTECOUNTS			279
#define TGMINSAMPLEVALUE			280
#define TGMAXSAMPLEVALUE			281
#define TGXRESOLUTION				282
#define TGYRESOLUTION				283
#define TGPLANAshortONFIGURATION		284
#define TGPAGENAME					285
#define TGXPOSITION					286
#define TGYPOSITION					287
#define TGFREEOFFSETS				288
#define TGFREEBYTECOUNTS			289
#define	TGGRAYUNIT					290
#define	TGGRAYCURVE					291

#define TGRESOLUTIONUNIT			296		/* 87-12-11 */
#define	TGPAGENUMBER				297

#define	TGCOLORRESPONSECURVES		301

#define	TGSOFTWARE					305
#define	TGDATETIME					306

#define TGARTIST					315
#define TGHOSTCOMPUTER				316

#define TGPREDICTOR					317		/* 88-09-19 */
#define	TGWHITEPOINT				318
#define	TGPRIMARYCHROMATICITIES		319
#define	TGCOLORMAP					320

#define TGHIGHSHADOW				321		/* 89-11-16 */
#define TGTILEWIDTH			322
#define TGTILELENGTH		323
#define TGTILEOFFSETS		324
#define TGTILEBYTECOUNTS	325
#define TGKIDS				330

/*---------------------------------------------------*/
// Image errors 

#define IM_BASE	0

#define	IM_BUG				(IM_BASE +  1)	/* supposedly impossible situation - almost certainly my problem */
#define	IM_MEM_FULL			(IM_BASE +  2)	/* can't allocate enough memory */
#define	IM_MEM_FAIL			(IM_BASE +  3)	/* can't lock allocated memory */
#define	IM_NO_WIDTH			(IM_BASE +  4)	/* no ImageWidth */
#define	IM_NO_LENGTH		(IM_BASE +  5)	/* no ImageLength */
#define	IM_NO_OFFSETS		(IM_BASE +  6)	/* no StripOffsets */
#define	IM_BAD_SPP			(IM_BASE +  7)	/* unsupported SamplesPerPixel */
#define	IM_BAD_COMPR		(IM_BASE +  8)	/* unsupported compression type */
#define	IM_BAD_PHOTO		(IM_BASE +  9)	/* unsupported photometricInterpretation */
#define	IM_BAD_PREDICT		(IM_BASE + 10)	/* unsupported Predictor */
#define	IM_BAD_PLANAR		(IM_BASE + 11)	/* unsupported Planashortonfiguration */
#define	IM_BAD_BPS			(IM_BASE + 12)	/* unsupported BitsPerSample */
#define	IM_BAD_NUM_OFF		(IM_BASE + 13)	/* wrong number of StripOffsets */
#define	IM_UNK_FORMAT		(IM_BASE + 14)	/* unknown format */
#define	IM_BAD_FILLORDER	(IM_BASE + 15)	/* unsupported FillOrder */
#define	IM_BAD_WIDTH		(IM_BASE + 16)	/* bad ImageWidth - like 0 */
#define	IM_BAD_LENGTH		(IM_BASE + 17)	/* bad ImageLength - like 0 */
#define	IM_PRED_MISMATCH	(IM_BASE + 18)	/* cannot use this predictor with this bit depth */

#define	IM_FADING_COMPR		(IM_BASE + 19)	/* this compression type is not recommended */
#define	IM_BAD_TTYPE		(IM_BASE + 20)	/* bad tiff type (not BYTE or ASCII or ...) */
#define	IM_BAD_NUM_BITS		(IM_BASE + 21)	/* wrong number of BitsPerSample values */
#define	IM_LARGE_STRIP		(IM_BASE + 22)	/* the strip is larger than the recommended 10K */
#define	IM_BAD_NUM_COUNTS	(IM_BASE + 23)	/* wrong number of StripByteCounts */
#define	IM_NO_BYTECOUNTS	(IM_BASE + 24)	/* no StripByteCounts */	/* fatal if LZW */
#define	IM_BAD_NEXT_IFD		(IM_BASE + 25)	/* the (2nd) next-ifd-pointer points past EOF */
#define	IM_PB_BITSNOTONE	(IM_BASE + 26)	/* PackBits: bit depth is greater than 1 */
#define	IM_NO_PHOTO			(IM_BASE + 27)	/* no PhotometricInterpretation field */
#define	IM_FADING_BITDEPTH	(IM_BASE + 28)	/* not-recommended bit depth (probably 6) */
#define	IM_BAD_ROWSPERSTRIP	(IM_BASE + 29)	/* bad RowsPerStrip (probably 0) */
#define	IM_NO_COMPR			(IM_BASE + 30)	/* uncompressed */
#define IM_COLOR_CLASH		(IM_BASE + 31)	/* PhotometricInterpretation does not match SamplesPerPixel */
#define	IM_NO_NEWSUBFILETYPE (IM_BASE + 32)	/* no NewSubfileType */
#define IM_PACKBITS_OVER	(IM_BASE + 33)	/* PackBits overflow */
#define	IM_1D_BADCODE		(IM_BASE + 34)
#define IM_1D_OVERFLOW		(IM_BASE + 35)

#define kUnCompressMsg		(IM_BASE + 36)	/* Not an error, but a message so we can read the data directly. */

#define IM_LGrayFile		(IM_BASE + 37)

/*---------------------------------------------------*/

	/* Substitute for prototype in IMAGELIB's edgeordr.h */
#ifdef MACINTOSH
#define LowToHigh()		((BOOL)(false))
#endif
#ifdef WINDOWS
#define LowToHigh()		((BOOL)(true))
#endif


/* other LZW definitions
 */
#define MAXCODEWIDTH		12				/* maximum code width, in bits */
#define MAXTABENTRIES	(1<<MAXCODEWIDTH)	/* max # of table entries */
#define CHARBITS	8	/* phony bit depth; we always compress and decompress bytes, in
 						 * this version
 						 */
#define CLEAshortODE		256
#define EOICODE			257

/* TAG FIELD INDICES: (not to be confused with actual tag values -- see
 * tiff.h for those).  Also note that I only need define the things that
 * I currently care about, to save a little space.
 *
 * this must be kept in numerical order by tag (see tiff.h), as of 88-10-12,
 * since I am writing them out in the order they appear in the IMAG array.
 */
#define X_NEWSUBFILETYPE		0
#define X_IMAGEWIDTH			1
#define X_IMAGELENGTH			2
#define X_BITSPERSAMPLE			3
#define X_COMPRESSION			4
#define X_PHOTOMETRICINTERP		5
#define X_STRIPOFFSETS			6
#define X_SAMPLES				7	/* SamplesPerPixel */
#define X_ROWSPERSTRIP			8
#define X_STRIPBYTECOUNTS		9
#define X_XRESOLUTION			10
#define X_YRESOLUTION			11
#define X_PLANAR				12	/* Planashortonfiguration */
#define	X_GRAYUNIT				13
#define	X_GRAYCURVE				14
#define X_RESOLUTIONUNIT		15
#define X_COLOshortURVES			16
#define	X_PREDICTOR				17	/* 88-09-19 */
#define X_KIDS					18

#define NTFIELDS				19	/* KEEP THIS CURRENT!!! (one more than largest value) */

/* shorthand ways of getting to field data.  Note:  assumes something has
 * converted if necessary to my standard data type for that field.  See
 * tiff.c for the code that determines the current standard type for each field.
 */
#define dwNewSubfileType			tf[X_NEWSUBFILETYPE].val.Tdword	/* 88-09-12 */

#define iImageWidth					tf[X_IMAGEWIDTH].val.Tword[0]
#define iImageLength				tf[X_IMAGELENGTH].val.Tword[0]
#define iBitsPerSample				tf[X_BITSPERSAMPLE].val.Tword[0]	/* assumes BitsPerSample values are equal! */
#define iSamples					tf[X_SAMPLES].val.Tword[0]			/* SamplesPerPixel */
#define iCompression				tf[X_COMPRESSION].val.Tword[0]
#define iPhotometricInterpretation	tf[X_PHOTOMETRICINTERP].val.Tword[0]
#define hSTRIPOffsets				tf[X_STRIPOFFSETS].Thandle
#define iRowsPerStrip				tf[X_ROWSPERSTRIP].val.Tword[0]
#define fXResolution				tf[X_XRESOLUTION].val.Tfloat
#define fYResolution				tf[X_YRESOLUTION].val.Tfloat
#define iPlanar						tf[X_PLANAR].val.Tword[0]
#define	iGrayUnit					tf[X_GRAYUNIT].val.Tword[0]
#define	hGrayCurve					tf[X_GRAYCURVE].Thandle
#define iResolutionUnit				tf[X_RESOLUTIONUNIT].val.Tword[0]
#		define RES_UNIT_NO_UNIT			1
#		define RES_UNIT_INCH			2
#		define RES_UNIT_CM				3
#define hStripByteCounts			tf[X_STRIPBYTECOUNTS].Thandle
#define hColoshorturves				tf[X_COLOshortURVES].Thandle
#define iPredictor					tf[X_PREDICTOR].val.Tword[0]

/* shorthands for existence flags
 */
#define eNewSubfileType				tf[X_NEWSUBFILETYPE].Texists
#define eImageWidth					tf[X_IMAGEWIDTH].Texists
#define eImageLength				tf[X_IMAGELENGTH].Texists
#define eSamples					tf[X_SAMPLES].Texists
#define eCompression				tf[X_COMPRESSION].Texists
#define ePhotometricInterpretation	tf[X_PHOTOMETRICINTERP].Texists
#define eStripOffsets				tf[X_STRIPOFFSETS].Texists
#define eXResolution				tf[X_XRESOLUTION].Texists
#define eYResolution				tf[X_YRESOLUTION].Texists
#define ePlanar						tf[X_PLANAR].Texists
#define	eGrayUnit					tf[X_GRAYUNIT].Texists
#define	eGrayCurve					tf[X_GRAYCURVE].Texists
#define eResolutionUnit				tf[X_RESOLUTIONUNIT].Texists
#define eStripByteCounts			tf[X_STRIPBYTECOUNTS].Texists
#define	eColoshorturves				tf[X_COLOshortURVES].Texists
#define ePredictor					tf[X_PREDICTOR].Texists
#define eKids						tf[X_KIDS].Texists

/* macros for computing BytesPerRow and Word-Aligned BytesPerRow
 *  w=width of row in pixels
 *  b=BitsPerSample
 *  s=SamplesPerPixel
 */
#define UBPR(w,b,s)		(((w)*(b)*(s)+7)>>3)
#define UWABPR(w,b,s)	((((w)*(b)*(s)+15)>>4)<<1)

/* NULL shorthands:
 */
#define HNULL	((HANDLE)NULL)
#define BFNULL	((BYTE  *)NULL)
#define LPNULL	((LPSTR)NULL)



/********************* structure definitions *******************/

/*  pointer typedefs
 */
typedef	short 	*LPSHORT;
typedef long 	*LPLONG;
typedef int 	*LPSINT;
typedef short   DC;
typedef DWORD   Handle;
typedef char    *Ptr;

typedef LPVOID	LPDLOC;	/* to allow imaging routines to pass a hidden
								 * data location structure, for eventual
								 * use by the vio.c routines.
								 */


/* Tiff Field Structure.  An array of these things is the main part of
 * the IMAG structure, below.
 */
typedef struct {
	BOOL	Texists;
	BOOL	Talloc;		/* true if Thandle is being used */
	WORD	Ttag;
	WORD	Ttype;
	DWORD	Tlength;
	HANDLE	Thandle;	/* for things > 4 bytes, except for TIFF RATIONAL */
	union {
		BYTE	Tbyte[4];
		char	Tchar[4];
		WORD	Tword[2];	/* from TIFF SHORT */
		short	Tsigned[2];	/* from TIFF SIGNED */
		DWORD	Tdword;		/* from TIFF LONG */
		float	Tfloat;		/* from TIFF RATIONAL */
	} val;
	DWORD	Tentryoffset;	/* file/memory offset of the TIFF directory
							 * entry.
							 */
} TFIELD,  *LPTFIELD;


/* structure containing all available information about an image
 * this structure is never to be written to disk directly; use the
 * TIFF routines.
 */
typedef struct {
	WORD	iFileType;
#define			INTELTIFF	 (0x4949)
#define			MOTOROLATIFF (0x4d4d)

	WORD	iVersion;
#define			VERSION42	42

	/* THE REAL TIFF FIELDS:
	 */
	TFIELD	tf[NTFIELDS];

	/* extra stuff
	 */
	BOOL	bStripsOk;
	WORD	BytesPerRow;
	HANDLE	hUnStrip;
	HANDLE	hCmStrip;
	WORD	CurStrip;
	HANDLE	hUnRow;
	HANDLE	hBuf1;
	WORD	nStrips;
	HANDLE	hDe;

	LPBYTE	lpT2Wlen;
	LPBYTE	lpT2Blen;
	HANDLE	hWCodeLut;
	HANDLE	hBCodeLut;

	WORD	NextRow;

} IMAG,  * LPIMAG;



/* TIFF "header" (8 bytes)
 * note: GtTiffHdr plays a little loose with this structure.
 */
typedef struct {
		WORD	thByteOrder;
		WORD	thVersion;
		DWORD	thIfdOffset;
} TIFFHDR,  * LPTIFFHDR;

/* IFD entry
 * note: GtTiffEntry plays a little loose with this structure.
 */
typedef struct {
		WORD  deTag;
		WORD  deType;
		DWORD deLength;
		DWORD deVal;
} DIRENTRY,  * LPDIRENTRY;

typedef struct {

	WORD		dlWhere;
#define				INFILE		1

	short		dlFRefNum;
} DLOC;

typedef short RC;

/* LZW Decompression tree node structure.
 *
 * This is a funny "tree", with pointers pointing up but not down.
 */
typedef struct {
	BYTE	Suffix;		/* character to be added to the current string */
	BYTE	StringLength; /* # of characters in the string */
	WORD	Parent;		/* offset of parent treenode */
} TREENODE,  * LPTREENODE;

#define MM_BASE		1000
#define CM_BASE		2000

/****** MEMORY MANAGEMENT ERRORS ************************/

#define MM_LOCAL_FULL		((RC)(MM_BASE + 0))	/* No room in local heap		*/
#define MM_GLOBAL_FULL		((RC)(MM_BASE + 1))	/* No room in global heap		*/
#define MM_LOCK_FAILED		((RC)(MM_BASE + 2))	/* Can't lock a handle			*/
#define MM_CANT_GET_PTR		((RC)(MM_BASE + 3))	/* MMGetPointer failed			*/

typedef struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[256];
} SBITMAPINFO;

/******* COMPRESSION ERRORS ******************************************/

#define CM_COMPRESSION		((RC)(CM_BASE + 0))	/* Generic compression error	*/
#define CM_DECOMPRESSION 	((RC)(CM_BASE + 1))	/* Generic decompression error	*/
#define	CM_LZW_INITCLEAR	((RC)(CM_BASE + 2))	/* LZW import: no initial clear code */


short GtTiffSizeof ( WORD n, LPWORD lp);
short VRead (CFile* pFile, DWORD pos, DWORD	BytesToRead, LPSTR	lpBuf);
short GtData (CFile* pFile, WORD order, DWORD pos, DWORD n, WORD dtype, LPSTR lpData);
short GtHugeData (CFile* pFile, WORD order, DWORD pos, DWORD dwN, WORD dtype, LPSTR	lpData);
short TField2Buf (LPTFIELD	pT, LPVOID lp, WORD max);
short GtMaxTLong (LPIMAG x, WORD field, LPDWORD lpMaxTLong);
short GtTiffEntry (CFile* pFile,WORD order, DWORD EntryOffset, LPDIRENTRY lpDe);
short FillTField (CFile*,WORD,DIRENTRY *,DWORD,TFIELD *);
short TypeConvert (TFIELD *, WORD);
short NicefyTField (TFIELD *, IMAG *);
short MhBuildLut (BYTE, LPBYTE, LPBYTE, WORD, LPBYTE);
short addblackbits ();
short MhDecomp (LPBYTE, LPBYTE, LPSTR, LPSTR, LPSTR  *, LPSTR, WORD, bool);
short OpenTiff2(CFile* pFile, LPIMAG x);
short Tiff2DeStrip (LPIMAG	x, WORD	RowsInThisStrip);
WORD AldCalcMask (WORD);
short LzwExpandCodes (HANDLE, DWORD, WORD  *, HANDLE);
short LzwDecodeChunk (HANDLE, HANDLE, WORD, LPBYTE, DWORD);
short LzwDeOpen (DWORD dwMaxOutBytes, LPHANDLE phTable, LPHANDLE phExpCodesBuf);
short LzwDePChunk (HANDLE hCmChunk, DWORD dwChunkByteCount, HANDLE hExpCodesBuf, HANDLE	hTable, DWORD dwOutExpected, LPBYTE lpUnChunk);
short LzwDeChunk (HANDLE hCmChunk, DWORD dwChunkByteCount, HANDLE hExpCodesBuf, HANDLE hTable, DWORD dwOutExpected, HANDLE hUnChunk);
short LzwDeClose (HANDLE hExpCodesBuf, HANDLE hTable);
short ImLzwDeOpen (LPIMAG x, DWORD dwMaxOutBytesPerBuf);
short ImLzwDeStrip (LPIMAG	x, DWORD dwStripByteCount, DWORD dwOutExpected, HANDLE hOut);
short ImLzwDeClose (LPIMAG	x) ;
void InitImag (IMAG *p) ;
short OpenBC (LPDLOC pDloc,LPIMAG x) ;
void CloseBC (IMAG *x) ;
void CloseImag (IMAG *p) ;
short UnDeStrip (HANDLE	hCmStrip, DWORD dwStripByteCount, HANDLE hUnStrip);
short PbDeStrip (HANDLE hCmStrip, HANDLE hUnStrip, WORD BytesPerRow, WORD RowsInThisStrip);
void HorizAdd (WORD ImWidth, LPBYTE lpRow);
void HorizRgbAdd (WORD ImWidth, LPBYTE lpRow);
void baP4P8Hi (WORD p, LPBYTE frombuf,LPBYTE tobuf);
short P8HiToP4 (WORD n,BYTE *frombuf, BYTE *tobuf);
short P4toP8Hi (WORD n, WORD startbit, BYTE *frombuf,BYTE *tobuf);
short UnHorDiff (WORD BitsPerSample, WORD SamplesPerPixel, DWORD PixelsWide, LPBYTE lpIn, LPBYTE lpExp, LPBYTE lpOut);
short RdBCRow (CFile* pFile,LPIMAG	x, DC row, WORD rowstartbyte, WORD rowbytes, LPSTR lpBuf);
short CheckTiff (LPIMAG	x) ;
short GtIfdInfo (CFile* pFile, DWORD pos, WORD ByteOrder, WORD Version, LPIMAG x);
bool fixInvertedData ( Handle , Ptr );
HANDLE createGreyTabHdl (short bitsPerSamp);
void GtTagString (WORD, char **);
short dumpentry (CFile*, WORD, DWORD, LPDIRENTRY);
short DoSibs (CFile*, WORD, WORD, LPDWORD, HBITMAP& hBitmap, int& width, int& height);
short GtTiffHdr (CFile* pFile, LPTIFFHDR);
bool readTiff (const char* pFileName,  HBITMAP& hBitmap, int& width, int& height);
short UnpackBits (LPSTR &, LPSTR  &, short);
