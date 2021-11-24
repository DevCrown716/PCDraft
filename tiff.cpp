#include "stdafx.h"
#include "tiff.h"
#include "phmath.h"
#include "gu.h"

		SBITMAPINFO     bmpi;
		bool			isColorMap;
		bool			isGrayScale;

		WrFixed			gHRes 			= short ( 72 ) ,
						gVRes 			= short ( 72 ) ;

		short			gPhotoMetr;

		DWORD 			TiffStart 		= 0L;

static 	char 			defstr[] 		= "(no string avail)";

static struct {
	WORD	tag;
 	char	*str;
} tagstr[] = {
TGNEWSUBFILETYPE,			"NewSubfileType",
	TGOLDSUBFILETYPE,			"OldSubfileType",
	TGIMAGEWIDTH,				"ImageWidth",
	TGIMAGELENGTH,				"ImageLength",
	TGCOMPRESSION,				"Compression",
	TGPHOTOMETRICINTERPRETATION,"PhotometricInterp",
	TGTHRESHHOLDING,			"Threshholding",
	TGCELLWIDTH,				"CellWidth",
	TGCELLLENGTH,				"CellLength",
	TGFILLORDER,				"FillOrder",
	TGSTRIPOFFSETS,				"StripOffsets",
	TGORIENTATION,				"Orientation",
	TGSAMPLESPERPIXEL,			"SamplesPerPixel",
	TGBITSPERSAMPLE,			"BitsPerSample",
	TGROWSPERSTRIP,				"RowsPerStrip",
	TGSTRIPBYTECOUNTS,			"StripByteCounts",
	TGMINSAMPLEVALUE,			"MinSampleValue",
	TGMAXSAMPLEVALUE,			"MaxSampleValue",
	TGXRESOLUTION,				"XResolution",
	TGYRESOLUTION,				"YResolution",
	TGPLANAshortONFIGURATION,		"Planashortonfiguration",
	TGDOCUMENTNAME,				"DocumentName",
	TGPAGENAME,					"PageName",
	TGXPOSITION,				"XPosition",
	TGYPOSITION,				"YPosition",
	TGIMAGEDESCRIPTION,			"ImageDescription",
	TGMAKE,						"Make",
	TGMODEL,					"Model",
	TGFREEOFFSETS,				"FreeOffsets",
	TGFREEBYTECOUNTS,			"FreeByteCounts",
	TGGRAYUNIT,					"GrayUnit",
	TGGRAYCURVE,				"GrayCurve",
	TGRESOLUTIONUNIT,			"ResolutionUnit",
	TGPAGENUMBER,				"PageNumber",
	TGCOLORRESPONSECURVES,		"ColorResponseCurves",
	TGSOFTWARE,					"Software",
	TGDATETIME,					"DateTime",
	TGARTIST,					"Artist",
	TGHOSTCOMPUTER,				"HostComputer",
	TGPREDICTOR,				"Predictor",
	TGWHITEPOINT,				"WhitePoint",
	TGPRIMARYCHROMATICITIES,	"PrimaryChromaticities",
	TGCOLORMAP,					"ColorMap",
	TGHIGHSHADOW,				"HighlightShadow",
	TGTILEWIDTH,				"TileWidth",
	TGTILELENGTH,				"TileLength",
	TGTILEOFFSETS,				"TileOffsets",
	TGTILEBYTECOUNTS,			"TileByteCounts",
	TGKIDS,						"Kids",
};


/***************************** statics *******************************/
/* for addrunbits
 */
#ifdef WINDOWS
BYTE ormask2[] = {'\x0','\x1','\x3','\x7','\xf','\x1f',
 '\x3f','\x7f',(BYTE)'\xff'};
#endif
#ifdef MACINTOSH
 BYTE ormask2[] = {'\0','\1','\3','\7','\17','\37',
 '\77','\177','\377'};
#endif

/* for mhdecomp
 */
 WORD wmask[] = {0x0,0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff};

/* use these greasy redundant globals for debugging info only!!!
 */
 WORD		curStrip;
 WORD		curStripRow;
 DWORD	curRowOffset;	/* wit beginning of strip */

 WORD	runcount;
 WORD	color;
 WORD	outbo;
 BYTE	waitingfortc;
 LPSTR lpOut;


/* WHITE code strings
 */
 BYTE wstr[] = {
/*    0 */	0x35,	0x07,	0x07,	0x08,
/*    4 */	0x0b,	0x0c,	0x0e,	0x0f,
/*    8 */	0x13,	0x14,	0x07,	0x08,
/*   12 */	0x08,	0x03,	0x34,	0x35,
/*   16 */	0x2a,	0x2b,	0x27,	0x0c,
/*   20 */	0x08,	0x17,	0x03,	0x04,
/*   24 */	0x28,	0x2b,	0x13,	0x24,
/*   28 */	0x18,	0x02,	0x03,	0x1a,
/*   32 */	0x1b,	0x12,	0x13,	0x14,
/*   36 */	0x15,	0x16,	0x17,	0x28,
/*   40 */	0x29,	0x2a,	0x2b,	0x2c,
/*   44 */	0x2d,	0x04,	0x05,	0x0a,
/*   48 */	0x0b,	0x52,	0x53,	0x54,
/*   52 */	0x55,	0x24,	0x25,	0x58,
/*   56 */	0x59,	0x5a,	0x5b,	0x4a,
/*   60 */	0x4b,	0x32,	0x33,	0x34,
/*   64 */	0x1b,
/*  128 */	0x12,
/*  192 */	0x17,
/*  256 */	0x37,
/*  320 */	0x36,
/*  384 */	0x37,
/*  448 */	0x64,
/*  512 */	0x65,
/*  576 */	0x68,
/*  640 */	0x67,
/*  704 */	0xcc,
/*  768 */	0xcd,
/*  832 */	0xd2,
/*  896 */	0xd3,
/*  960 */	0xd4,
/* 1024 */	0xd5,
/* 1088 */	0xd6,
/* 1152 */	0xd7,
/* 1216 */	0xd8,
/* 1280 */	0xd9,
/* 1344 */	0xda,
/* 1408 */	0xdb,
/* 1472 */	0x98,
/* 1536 */	0x99,
/* 1600 */	0x9a,
/* 1664 */	0x18,
/* 1728 */	0x9b,
/* 1792 */	0x08,
/* 1856 */	0x0c,
/* 1920 */	0x0d,
/* 1984 */	0x12,
/* 2048 */	0x13,
/* 2112 */	0x14,
/* 2176 */	0x15,
/* 2240 */	0x16,
/* 2304 */	0x17,
/* 2368 */	0x1c,
/* 2432 */	0x1d,
/* 2496 */	0x1e,
/* 2560 */	0x1f
};

/* BLACK code strings
 */
 BYTE bstr[] = {
/*    0 */	0x37,	0x02,	0x03,	0x02,
/*    4 */	0x03,	0x03,	0x02,	0x03,
/*    8 */	0x05,	0x04,	0x04,	0x05,
/*   12 */	0x07,	0x04,	0x07,	0x18,
/*   16 */	0x17,	0x18,	0x08,	0x67,
/*   20 */	0x68,	0x6c,	0x37,	0x28,
/*   24 */	0x17,	0x18,	0xca,	0xcb,
/*   28 */	0xcc,	0xcd,	0x68,	0x69,
/*   32 */	0x6a,	0x6b,	0xd2,	0xd3,
/*   36 */	0xd4,	0xd5,	0xd6,	0xd7,
/*   40 */	0x6c,	0x6d,	0xda, 	0xdb,
/*   44 */	0x54,	0x55,	0x56,	0x57,
/*   48 */	0x64,	0x65,	0x52,	0x53,
/*   52 */	0x24,	0x37,	0x38,	0x27,
/*   56 */	0x28,	0x58,	0x59,	0x2b,
/*   60 */	0x2c,	0x5a,	0x66,	0x67,
/*   64 */	0x0f,
/*  128 */	0xc8,
/*  192 */	0xc9,
/*  256 */	0x5b,
/*  320 */	0x33,
/*  384 */	0x34,
/*  448 */	0x35,
/*  512 */	0x6c,
/*  576 */	0x6d,
/*  640 */	0x4a,
/*  704 */	0x4b,
/*  768 */	0x4c,
/*  832 */	0x4d,
/*  896 */	0x72,
/*  960 */	0x73,
/* 1024 */	0x74,
/* 1088 */	0x75,
/* 1152 */	0x76,
/* 1216 */	0x77,
/* 1280 */	0x52,
/* 1344 */	0x53,
/* 1408 */	0x54,
/* 1472 */	0x55,
/* 1536 */	0x5a,
/* 1600 */	0x5b,
/* 1664 */	0x64,
/* 1728 */	0x65,
/* 1792 */	0x08,
/* 1856 */	0x0c,
/* 1920 */	0x0d,
/* 1984 */	0x12,
/* 2048 */	0x13,
/* 2112 */	0x14,
/* 2176 */	0x15,
/* 2240 */	0x16,
/* 2304 */	0x17,
/* 2368 */	0x1c,
/* 2432 */	0x1d,
/* 2496 */	0x1e,
/* 2560 */	0x1f
};

/* WHITE code lengths
 */
 BYTE wlen[] = {
/*    0 */	0x08,	0x06,	0x04,	0x04,
/*    4 */	0x04,	0x04,	0x04,	0x04,
/*    8 */	0x05,	0x05,	0x05,	0x05,
/*   12 */	0x06,	0x06,	0x06,	0x06,
/*   16 */	0x06,	0x06,	0x07,	0x07,
/*   20 */	0x07,	0x07,	0x07,	0x07,
/*   24 */	0x07,	0x07,	0x07,	0x07,
/*   28 */	0x07,	0x08,	0x08,	0x08,
/*   32 */	0x08,	0x08,	0x08,	0x08,
/*   36 */	0x08,	0x08,	0x08,	0x08,
/*   40 */	0x08,	0x08,	0x08,	0x08,
/*   44 */	0x08,	0x08,	0x08,	0x08,
/*   48 */	0x08,	0x08,	0x08,	0x08,
/*   52 */	0x08,	0x08,	0x08,	0x08,
/*   56 */	0x08,	0x08,	0x08,	0x08,
/*   60 */	0x08,	0x08,	0x08,	0x08,
/*   64 */	0x05,
/*  128 */	0x05,
/*  192 */	0x06,
/*  256 */	0x07,
/*  320 */	0x08,
/*  384 */	0x08,
/*  448 */	0x08,
/*  512 */	0x08,
/*  576 */	0x08,
/*  640 */	0x08,
/*  704 */	0x09,
/*  768 */	0x09,
/*  832 */	0x09,
/*  896 */	0x09,
/*  960 */	0x09,
/* 1024 */	0x09,
/* 1088 */	0x09,
/* 1152 */	0x09,
/* 1216 */	0x09,
/* 1280 */	0x09,
/* 1344 */	0x09,
/* 1408 */	0x09,
/* 1472 */	0x09,
/* 1536 */	0x09,
/* 1600 */	0x09,
/* 1664 */	0x06,
/* 1728 */	0x09,
/* 1792 */	0x0B,
/* 1856 */	0x0B,
/* 1920 */	0x0B,
/* 1984 */	0x0C,
/* 2048 */	0x0C,
/* 2112 */	0x0C,
/* 2176 */	0x0C,
/* 2240 */	0x0C,
/* 2304 */	0x0C,
/* 2368 */	0x0C,
/* 2432 */	0x0C,
/* 2496 */	0x0C,
/* 2560 */	0x0C,
};

/* BLACK code lengths
 */
 BYTE blen[] = {
/*    0 */	0x0A,	0x03,	0x02,	0x02,
/*    4 */	0x03,	0x04,	0x04,	0x05,
/*    8 */	0x06,	0x06,	0x07,	0x07,
/*   12 */	0x07,	0x08,	0x08,	0x09,
/*   16 */	0x0A,	0x0A,	0x0A,	0x0B,
/*   20 */	0x0B,	0x0B,	0x0B,	0x0B,
/*   24 */	0x0B,	0x0B,	0x0C,	0x0C,
/*   28 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   32 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   36 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   40 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   44 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   48 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   52 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   56 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   60 */	0x0C,	0x0C,	0x0C,	0x0C,
/*   64 */	0x0A,
/*  128 */	0x0C,
/*  192 */	0x0C,
/*  256 */	0x0C,
/*  320 */	0x0C,
/*  384 */	0x0C,
/*  448 */	0x0C,
/*  512 */	0x0D,
/*  576 */	0x0D,
/*  640 */	0x0D,
/*  704 */	0x0D,
/*  768 */	0x0D,
/*  832 */	0x0D,
/*  896 */	0x0D,
/*  960 */	0x0D,
/* 1024 */	0x0D,
/* 1088 */	0x0D,
/* 1152 */	0x0D,
/* 1216 */	0x0D,
/* 1280 */	0x0D,
/* 1344 */	0x0D,
/* 1408 */	0x0D,
/* 1472 */	0x0D,
/* 1536 */	0x0D,
/* 1600 */	0x0D,
/* 1664 */	0x0D,
/* 1728 */	0x0D,
/* 1792 */	0x0B,
/* 1856 */	0x0B,
/* 1920 */	0x0B,
/* 1984 */	0x0C,
/* 2048 */	0x0C,
/* 2112 */	0x0C,
/* 2176 */	0x0C,
/* 2240 */	0x0C,
/* 2304 */	0x0C,
/* 2368 */	0x0C,
/* 2432 */	0x0C,
/* 2496 */	0x0C,
/* 2560 */	0x0C,
};

/* main LZW DEcompression control structure
 *
 * the table is an array of TREENODEs, the first (1<<CHARBITS) entries of
 * which represent the characters themselves.
 */
typedef struct {
	HANDLE		hTable;
	HANDLE		hExpCodesBuf;	/* expanded-codes buffer */
} LZWDE, *PLZWDE,  *LPLZWDE;


void swapb (
	LPWORD	lpSrc,			/* assumed to be word-aligned */
	LPWORD	lpDst,			/* assumed to be word-aligned */
	WORD  	count)			/* Number of bytes (assumed to be even),
								 * converted to number of WORDs. */
{
	count >>= 1;		/* convert byte count to WORD count	*/

	if (lpDst <= lpSrc || lpDst >= lpSrc + count)
	{
		while (count--)
		{
			*lpDst++ = ((*lpSrc) << 8) + ((*lpSrc) >> 8);
			++lpSrc;
		}
	}
	else
	{		/* we'll have to go backward */
		lpSrc += (count-1);
		lpDst += (count-1);
		while (count--)
		{
			*lpDst-- = ((*lpSrc) << 8) + ((*lpSrc) >> 8);
			--lpSrc;
		}
	}
}

short byteCmp (
	void			*void1,
	void			*void2,
	unsigned short	n )
{
	char *s1 = (char *)void1;
	char *s2 = (char *)void2;

	for (; --n && (*s1 == *s2); s1++, s2++)
		;

	return (*s1 - *s2);
}

/**[r******************************************************************
 * swapw
 *
 * DESCRIPTION:
 * swap words -- overlapping ranges are handled properly
 *
 * actually, does a 4-byte reversal
 *
 * GLOBAL DATA USED:
 * none
 *
 * FUNCTIONS CALLED:
 * none
 **r]*****************************************************************/

void swapw (
	LPSTR	lpSrc,			/* assumed to be word-aligned */
	LPSTR	lpDst,			/* assumed to be word-aligned */
	WORD  			nbytes)			/* assumed to be multiple of 4 */
{
	WORD dwords;
	union {
		char c[4];
		DWORD dw;
	} dwrd;

	dwords = nbytes/4;

	if (lpDst <= lpSrc || lpDst >= lpSrc + nbytes)
	{
		for (; dwords--; lpSrc += 4)
		{
			dwrd.dw = *(DWORD FAR *)lpSrc;
			*lpDst++ = *(LPSTR)(dwrd.c + 3);
			*lpDst++ = *(LPSTR)(dwrd.c + 2);
			*lpDst++ = *(LPSTR)(dwrd.c + 1);
			*lpDst++ = *(LPSTR)(dwrd.c);
		}
	}
	else
	{		/* we'll have to go backward */
		lpSrc += nbytes - sizeof(DWORD);
		lpDst += nbytes - 1;
		for (; dwords--; lpSrc -= 4)
		{
			dwrd.dw = *(DWORD FAR *)lpSrc;
			*lpDst-- = *(LPSTR)(dwrd.c);
			*lpDst-- = *(LPSTR)(dwrd.c + 1);
			*lpDst-- = *(LPSTR)(dwrd.c + 2);
			*lpDst-- = *(LPSTR)(dwrd.c + 3);
		}
	}
}

/*-----------------------------------------------------------------------*/


/************************ mostly local procedures *************************/

/* -------------------------------------------------------------------------------- */
/********************************************************************
 *	GlobalAlloc
 *
 *	DESCRIPTION:
 *	Allocate a block from the global heap.  The new block is relocatable
 *	(must be locked before it can be used) and non-purgable.  Handles to
 *	blocks are valid only to the application that allocated them and cannot
 *	be passed from one application to another.
 *	Depending on the environment, the actual number of bytes allocated may
 *	be slightly larger than the requested.
 *
 *	PARAMETERS:
 *	IN	DWORD	Number of bytes to allocate.
 *
 *	RETURN:
 *	HANDLE		Handle of allocated block.
 *
 *	SIDE EFFECTS:
 *	Memory is allocated from the global heap.  It should later be freed with
 *	a call to free().
 *******************************************************************/
//#ifdef	WINDOWS
//#include "malloc.h"
//#define GlobalAlloc(sz) 		(HANDLE)_fmalloc(sz)
//#endif	/* WINDOWS */

//#ifdef	MACINTOSH
//extern	Handle	GlobalAlloc			(Size) ;

//extern	Ptr	MMPtrAlloc			(Size) ;

//#endif	/* MACINTOSH */


/********************************************************************
 *	free
 *
 *	DESCRIPTION:
 *	Free a block allocated by GlobalAlloc().
 *	Do not use on blocks allocated by MMFixAlloc().
 *
 *	PARAMETERS:
 *	IN	HANDLE		Handle to free.
 *
 *	RETURN:
 *	None.
 *
 *	SIDE EFFECTS:
 *	Memory is freed in the global heap.
 *******************************************************************/
#ifdef	MACINTOSH
#define free(hndl) 			DisposeHandle((hndl))
#endif	/* MACINTOSH */



/*-----------------------------------------------------------------------*/


short GetItLockIt (
	DWORD		dwbytes,
	HANDLE		*ph,
	BYTE 	**plp)
{
		short err = SUCCESS;

		if (!(*ph = GlobalAlloc (GMEM_MOVEABLE, dwbytes))) {
			err = IM_MEM_FULL;
			goto cu0;
		}
		if (!(*plp = (BYTE  *) GlobalLock (*ph))) {
			GlobalFree (*ph);
			*ph = HNULL;
			err = IM_MEM_FAIL;
			goto cu0;
		}
cu0:	return err;
}

/*-----------------------------------------------------------------------*/

void UnlockItFreeIt ( HANDLE	h)
{
		GlobalUnlock (h);
		GlobalFree (h);
}

/*-----------------------------------------------------------------------*/

/* get the sizeof a TIFF data type
 */
short GtTiffSizeof (
	WORD n,		/* TIFFBYTE or ... */
	LPWORD lp)	/* output */
{
	short err = SUCCESS;

	switch (n) {
	case TIFFBYTE:
	case TIFFASCII:
		*lp = 1;
		break;
	case TIFFSHORT:
		*lp = 2;
		break;
	case TIFFLONG:
		*lp = 4;
		break;
	case TIFFRATIONAL:
		*lp = 8;
		break;
	case TIFFSIGNED:
		*lp = 2;
		break;
	case TIFFFLOAT:		/* manufactured type -- not found in TIFF file */
		*lp = sizeof(float);
		break;
	default:
		*lp = 1;
		err = IM_BUG;
		break;
	}

	return err;
}

/*-----------------------------------------------------------------------*/

short VRead (
	/*LPDLOC	lpD,		/* pointer to a structure that tells
				 	 * where to go for the information
					 */
	CFile*  pFile,
	DWORD	pos,	/* byte position, with respect to the beginning
				 * of the "file", to start reading data from
				 */
	DWORD	BytesToRead,
	LPSTR	lpBuf)	/* where to put the data */
{
		
		short	err = SUCCESS;
		long	mdBytes = BytesToRead;

		/* DBMSG(("VRead: top.  %u\n", BytesToRead)); */
		try {
		
			pFile->Seek(pos, CFile::begin);
			pFile->Read(lpBuf, BytesToRead);
			return err;
		}

		catch (CException* e) {
			e->Delete();
			return -1;
		}
}

/*-----------------------------------------------------------------------*/
/* get data -- handles file/table and byte-order problems
 * 64K max
 */
short GtData (
	CFile* pFile,		/* data location - open file or locked-down table */
	WORD	order,		/* byte order -- INTELTIFF or MOTOROLATIFF */
	DWORD	pos,		/* file/table position, with respect to its beginning */
	DWORD	n,			/* number of data elements to read */
	WORD	dtype,		/* data type: TIFFSHORT, etc */
	LPSTR	lpData)		/* where to put the data */
{
		short		err;
		WORD	tsize;
		DWORD	BytesToRead;

		if (n == 0)
			goto done;

		/* read the data
		 */
		if ((err = GtTiffSizeof (dtype, &tsize)) != 0) {
			return err;
		}
		BytesToRead = tsize * n;
		if ((err = VRead (pFile, pos, BytesToRead, lpData)) != 0) {
			return err;
		}

		/* change the byte order, if necessary
		 */
		if (order != MOTOROLATIFF && order != INTELTIFF) {
			return IM_BUG;	/* should have been caught earlier */
		}
#ifdef WINDOWS
		if (order == MOTOROLATIFF)
#endif
#ifdef MACINTOSH
		if (order == INTELTIFF)
#endif
			{
			if (dtype == TIFFSHORT)
				swapb ((LPWORD)lpData, (LPWORD)lpData, BytesToRead);
			else if (dtype == TIFFLONG)
				swapw (lpData, lpData, BytesToRead);
			else if (dtype == TIFFRATIONAL)
				swapw (lpData, lpData, BytesToRead);
			else if (dtype == TIFFSIGNED)
				swapb ((LPWORD)lpData, (LPWORD)lpData, BytesToRead);
			}

done:	return SUCCESS;
}

/*-----------------------------------------------------------------------*/
/* get a possibly >64K chunk of data, by calling GtData repeatedly
 */
short GtHugeData (
	CFile* pFile,		/* data location - open file or locked-down table */
	WORD	order,		/* INTELTIFF or MOTOROLATIFF */
	DWORD	pos,		/* file/table position, with respect to its beginning */
	DWORD	dwN,		/* number of data elements to read */
	WORD	dtype,		/* data type: TIFFSHORT, etc */
	LPSTR	lpData) 		/* where to put the data */
{
		short		err = SUCCESS;
		WORD	tsize;
		DWORD	ElementsLeft;
		DWORD	ElementsToRead;
		LPSTR	lpOut = lpData;
		DWORD	ElementsPeshorthunk;

		/* DBMSG(("GtHugeData: top\n")); */

		/* get size of elements
		 */
		if ((err = GtTiffSizeof (dtype, &tsize)) != 0) {
			return err;
		}

		/* calculate number of elements per chunk
		 */
		ElementsPeshorthunk = CHUNKSIZE / tsize;
		/* DBMSG((" ElementsPeshorthunk=%lu\n",ElementsPeshorthunk)); */

		/* read repeatedly
		 */
		ElementsLeft = dwN;
		while (ElementsLeft > 0L) {
			ElementsToRead = min (ElementsPeshorthunk, ElementsLeft);
			/* DBMSG((" ElementsToRead=%lu\n",ElementsToRead)); */

			if ((err = GtData (pFile, order, pos, ElementsToRead, dtype, lpOut)) != 0) {
				goto cu;
			}
			lpOut += ElementsToRead * tsize;
			pos += ElementsToRead * tsize;		/* RFB 90-04-05 */
			ElementsLeft -= ElementsToRead;
			/* DBMSG((" ElementsLeft=%lu\n",ElementsLeft)); */
		}
		/* DBMSG(("GtHugeData: bottom\n")); */
cu:		return err;
}

/*---------------------------------------------*/

/* copies tiff field information into a buffer.
 * This helps save some
 * code in places where the character string could either be in the 4-byte
 * Value spot or in a handle.
 *
 * best for things like character strings that aren't too large, since it
 * starts to get expensive to keep multiple copies around.
 */
short TField2Buf (
	LPTFIELD	pT,		/* soushorte */
	LPVOID		lp,		/* destination */
	WORD		max)	/* size of destination */
{
		short		err = SUCCESS;
		WORD	TypeSize;
		LPVOID	lpInBuf;
		DWORD	dwBytes;

		if (!pT->Texists) {
			err = IM_BUG;
			goto cu0;
		}
		if ((err = GtTiffSizeof (pT->Ttype, &TypeSize)) != 0) {
			goto cu0;
		}
		dwBytes = pT->Tlength * (DWORD)TypeSize;
		if (dwBytes > (DWORD)max) {
			dwBytes = (DWORD)max;
		}

		/* if <= 4 bytes, just copy
		 */
		if (dwBytes <= 4) {
			memcpy ((LPSTR)lp, (LPSTR)&pT->val.Tchar[0], (WORD)dwBytes);
		}

		/* otherwise lock and copy
		 */
		else {
			if ((lpInBuf = (LPVOID) GlobalLock(pT->Thandle)) == (LPVOID)NULL) {
				err = IM_MEM_FAIL;
				goto cu0;
			}
			memcpy ((LPSTR)lp, (LPSTR)lpInBuf, (WORD)dwBytes);
			GlobalUnlock (pT->Thandle);
		}

cu0:

		return err;
}

/*-----------------------------------------------------------------------*/
/* get the max TIFFLONG (DWORD) value in a TIFF-type array
 */
short GtMaxTLong (
	LPIMAG	x,
	WORD	field,		/* e.g., X_STRIPBYTECOUNTS. must be TIFFLONG */
	LPDWORD			lpMaxTLong)	/* OUT: max TIFFLONG value */
{
		short		err = SUCCESS;
		BOOL	bLocked = false;
		DWORD	TLong;
		DWORD	MaxTLong;
		DWORD	Count;
		DWORD	ii;
		LPDWORD	lpTLongs;

		/* DBMSG(("GtMaxTLong: top\n")); */

		/* not TIFFLONG?
		 */
		if (x->tf[field].Ttype != TIFFLONG) {
			err = IM_BUG;
			goto cu0;
		}

		/* do they exist?
		 */
		if (x->tf[field].Texists) {
			if (x->tf[field].Talloc) {
				if (!(lpTLongs = (LPDWORD)GlobalLock (x->tf[field].Thandle))) {
					err = MM_LOCK_FAILED;
					goto cu0;
				}
				bLocked = true;
			}
			else {
				lpTLongs = (LPDWORD)&x->tf[field].val.Tdword;
			}
		}
		else {
			err = IM_NO_BYTECOUNTS;
			goto cu0;
		}

		/* find the largest
		 */
		MaxTLong = 0;
		Count = x->tf[field].Tlength;
		/* DBMSG(("# of TLongs = %lu\n", Count)); */
		for (ii = 0; ii < Count; ii++) {
			TLong = *lpTLongs++;
			/* DBMSG(("TLong=%lu\n", TLong)); */
			if (TLong > MaxTLong) {
				MaxTLong = TLong;
			}
		}

		/* save the returned value
		 */
		/* DBMSG((" MaxTLong=%lu\n",MaxTLong)); */
		*lpMaxTLong = MaxTLong;

		/* clean up and return
		 */
//cu1:
		if (bLocked) {
			GlobalUnlock (x->tf[field].Thandle);
		}
cu0:

		return err;
} /* end GtMaxTLong */

/*-----------------------------------------------------------------------*/

/* get TIFF directory entry
 */
short GtTiffEntry (
	CFile* pFile,
	WORD	order,
	DWORD	EntryOffset,
	LPDIRENTRY	lpDe)
{
		short err;
		DWORD BytesToRead;
		WORD tsize;
		WORD wTmp;

		/* get the 2 words beginning with deTag
		 */
		if ((err = GtData (pFile, order, EntryOffset, 2, TIFFSHORT, (LPSTR)&lpDe->deTag)) != 0) {
			return err;
		}

		/* get the 2 dwords, beginning with deLength
		 */
		if ((err = GtData (pFile, order, EntryOffset + 4L, 2, TIFFLONG, (LPSTR)&lpDe->deLength)) != 0) {
			return err;
		}

		/* fix up deVal, if it's not really a LONG
		 */
#ifdef WINDOWS
		if (order == MOTOROLATIFF)
#endif
#ifdef MACINTOSH
		if (order == INTELTIFF)
#endif
		{
			if ((err = GtTiffSizeof (lpDe->deType, &tsize)) != 0) {
				return err;
			}
			BytesToRead = (DWORD)tsize * lpDe->deLength;
			if (BytesToRead <= 4) {
				if (tsize == 2) {	/* swap words */
					wTmp = * (WORD *) &lpDe->deVal;
					* (WORD *) &lpDe->deVal = * ((WORD *) &lpDe->deVal + 1);
					* ((WORD *) &lpDe->deVal + 1) = wTmp;
				}
				else if (tsize == 1) {	/* swap bytes */
					swapw ((LPSTR)&lpDe->deVal, (LPSTR)&lpDe->deVal, 4);
				}
			}
		}

		/* return
		 */
		return SUCCESS;
}

/*-----------------------------------------------------------------------*/
/* Fill a Tiff Field structure
 * Note: the caller should probably not totally die upon getting an error.
 *  data types are possible, for one thing.  Just don't set the
 * existence flag for the field.
 */
short FillTField (
	CFile*				pFile,
	WORD				order,
	DIRENTRY	*pDe,
	DWORD				EntryOffset,
	TFIELD		*pTF)
{
		short		err = SUCCESS;
		HANDLE	h;
		LPSTR	lp;
		WORD	TypeSize;
		DWORD	BytesToRead;

		/* copy tag, type, and length from DIRENTRY structure
		 */
		pTF->Texists = true;
		pTF->Ttag = pDe->deTag;
		pTF->Ttype = pDe->deType;
		pTF->Tlength = pDe->deLength;

		/* record the offset, for possible later modify-in-place action
		 */
		pTF->Tentryoffset = EntryOffset;

		/* calculate needed space
		 */
		if ((err = GtTiffSizeof (pTF->Ttype, &TypeSize)) != 0) {
			goto cu0;
		}
		BytesToRead = (DWORD)TypeSize * pTF->Tlength;

		/* if <= 4 bytes, we're almost done.  else we have to do some
		 * work.
		 */
		pTF->Talloc = false;	/* just to be safe */
		if (BytesToRead <= 4) {
			pTF->val.Tdword = pDe->deVal;
		}
		else {

			/* allocate and lock a buffer
			 */
			if (!(h = GlobalAlloc (GMEM_MOVEABLE,BytesToRead))) {
				err = IM_MEM_FULL;
				goto cu0;
			}
			if (!(lp = (LPSTR)GlobalLock (h))) {
				err = IM_MEM_FAIL;
				GlobalFree (h);
				goto cu0;
			}

			/* read the data
			 */
			if ((err = GtHugeData (pFile, order, pDe->deVal, pDe->deLength, pDe->deType, lp)) != 0) {
				GlobalUnlock (h);
				GlobalFree (h);
				goto cu0;
			}

			/* make sure that the val union contains the first N values from
			 * the memory buffer, so that we can use things like p->iBitsPerSample
			 * constructs even if there are 3 BitsPerSample values.
			 */
			memcpy ((LPSTR) &pTF->val.Tchar[0], lp, 4);

			/* unlock the buffer
			 */
			GlobalUnlock (h);

			/* stuff the handle into the TFIELD structure
			 */
			pTF->Thandle = h;
			pTF->Talloc = true;

		} /* end of greater-than-4-byte case */

		/* return
		 */
cu0:	return err;
}

/*-----------------------------------------------------------------------*/
short TypeConvert (
	TFIELD	*pTF,
	WORD	totype)		/* TIFFBYTE, etc */
{
		short		err = SUCCESS;

		WORD	fromtype;
		DWORD	dwLength;

		WORD	SshortSize;
		DWORD	SshortBytes;
		DWORD	SshortVal;
		LPSTR	lpSshort;

		WORD	DstSize;
		DWORD	DstBytes;
		DWORD	DstVal;
		LPSTR	lpDst;
		HANDLE	hDst = (HANDLE)NULL;

		/* DBMSG(("TypeConvert: top\n")); */

		/* shorthands:
		 */
		fromtype = pTF->Ttype;
		dwLength = pTF->Tlength;

		/* DBMSG(("tfield:\n"));
		 * DBMSG((" Talloc=%u Ttag=%u Ttype=%u Tlength=%lu val=%lx\n",
		 *  pTF->Talloc,pTF->Ttag,pTF->Ttype,pTF->Tlength,pTF->val.Tdword));
		 */

		/* if the same type, do nothing
		 */
		if (totype == fromtype) {
			goto done;
		}

		/* calculate number of soushorte bytes
		 */
		if ((err = GtTiffSizeof (fromtype, &SshortSize)) != 0) {
			goto done;
		}
		SshortBytes = (DWORD)SshortSize * dwLength;

		/* point to soushorte data
		 */
		if (SshortBytes <= (DWORD)4) {
			if (pTF->Talloc) {
				err = IM_BUG;	/* programming error */
				goto done;
			}
			SshortVal = pTF->val.Tdword;
			lpSshort = (LPSTR)&SshortVal;
		} else {
			if (!pTF->Talloc) {
				err = IM_BUG;	/* programming error */
				goto done;
			}
			if (pTF->Thandle == (HANDLE)NULL) {
				err = IM_BUG;	/* programming error */
				goto done;
			}
			if ((lpSshort = (LPSTR)GlobalLock(pTF->Thandle)) == (LPSTR)NULL) {
				err = IM_MEM_FAIL;
				goto done;
			}
		}

		/* calculate number of destination bytes
		 *
		 * In the case of ASCII soushorte and integer or
		 * floating point destination, we are going to
		 * allocate too much space, and quite possibly
		 * allocate when the result would fit into 4
		 * bytes, so make sure we take that into consideration
		 * below.
		 */
		if ((err = GtTiffSizeof (totype, &DstSize)) != 0) {
			if (pTF->Talloc) GlobalUnlock (pTF->Thandle);
			goto done;
		}
		DstBytes = (DWORD)DstSize * dwLength;

		/* point to destination data
		 */
		if (DstBytes <= (DWORD)4)
			lpDst = (LPSTR)&DstVal;
		else {
			if ((err = GetItLockIt (DstBytes, &hDst, (LPBYTE *)&lpDst)) != 0) {
				if (pTF->Talloc) GlobalUnlock (pTF->Thandle);
				goto done;
			}
		}

		/* convert depending on soushorte and destination type
		 */
		{
			/* if TIFFRATIONAL, convert to floating point or
			 * integer. Note that I am relaxing the definition
			 * of RATIONAL to include negative numbers.
			 */
			if (fromtype == TIFFRATIONAL &&
			 (totype == TIFFFLOAT || totype == TIFFSIGNED ||
			 totype == TIFFSHORT)) {
			 	long 	*lpSshortPtr;	/* assumption: long & DWORD are same length */
			 	LPSTR		lpDstPtr;
				DWORD		dwCnt;
				float				FloatVal;
				short				ShortVal;

				/* set up
				 */
				lpSshortPtr = (long  *)lpSshort;
				lpDstPtr = lpDst;

				/* for each rational...
				 */
				for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {

					/* calculate a float equivalent
					 */
					if (*(lpSshortPtr +1) == (DWORD)0) {
						FloatVal = (float)0.0;
					} else {
						FloatVal = (float)*lpSshortPtr / (float)*(lpSshortPtr+1);
					}

					/* translate if necessary, and store
					 * into the destination array
					 */
					if (totype == TIFFFLOAT) {
						*((float  *)lpDstPtr) = FloatVal;
					} else {
						if (FloatVal >= (float)0.0) {
							ShortVal = (short)(FloatVal + .5);
						} else {
							/* is this right? */
							ShortVal = (short)(FloatVal - .5);
						}
					}
					if (totype == TIFFSHORT) {
						*((LPWORD)lpDstPtr) = (WORD)ShortVal;
					} else if (totype == TIFFSIGNED) {
						*((short  *)lpDstPtr) = ShortVal;
					}

					/* increment the pointers
					 */
					lpSshortPtr++;
					lpDstPtr += DstSize;
				}

				/* make sure that the val section contains the first N values from
				 * the destination array
				 */
				if (DstBytes > (DWORD)4) {
					memcpy ((LPSTR) &pTF->val.Tdword, (LPSTR)lpDst, 4);
				}

				/* set the new type
				 */
				pTF->Ttype = totype;

			} /* end of rational to float/short/WORD section */

			/* else if an unsigned integer (TIFFBYTE, TIFFSHORT, or TIFFLONG),
			 * do the appropriate conversion.  I probably should check for
			 * problems when converting to something smaller.
			 */
			else if ((fromtype == TIFFBYTE || fromtype == TIFFSHORT ||
			 fromtype == TIFFLONG) &&
			 (totype == TIFFBYTE || totype == TIFFSHORT ||
			 totype == TIFFLONG)) {
			 	LPSTR		lpSshortPtr = lpSshort;
			 	LPSTR		lpDstPtr = lpDst;
				DWORD		dwCnt;
				WORD		TiffShort;
				DWORD		TiffLong;

				if (fromtype == TIFFBYTE && totype == TIFFSHORT) {
					for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {
						*((LPWORD)lpDstPtr) = (WORD)*((LPBYTE)lpSshortPtr);
						lpSshortPtr += SshortSize;
						lpDstPtr += DstSize;
					}
				} else if (fromtype == TIFFBYTE && totype == TIFFLONG) {
					for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {
						*((LPDWORD)lpDstPtr) = (DWORD)*((LPBYTE)lpSshortPtr);
						lpSshortPtr += SshortSize;
						lpDstPtr += DstSize;
					}
				} else if (fromtype == TIFFSHORT && totype == TIFFBYTE) {
					for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {
						TiffShort = *((LPWORD)lpSshortPtr);
						if (TiffShort > MAXBYTE) {
							TiffShort = MAXBYTE;
						}
						*((LPBYTE)lpDstPtr) = (BYTE)TiffShort;
						lpSshortPtr += SshortSize;
						lpDstPtr += DstSize;
					}
				} else if (fromtype == TIFFSHORT && totype == TIFFLONG) {
					for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {
						*((LPDWORD)lpDstPtr) = (DWORD)*((LPWORD)lpSshortPtr);
						lpSshortPtr += SshortSize;
						lpDstPtr += DstSize;
					}
				} else if (fromtype == TIFFLONG && totype == TIFFBYTE) {
					for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {
						TiffLong = *((LPDWORD)lpSshortPtr);
						if (TiffLong > MAXWORD) {
							TiffLong = MAXWORD;
						}
						*((LPBYTE)lpDstPtr) = (BYTE)TiffLong;
						lpSshortPtr += SshortSize;
						lpDstPtr += DstSize;
					}
				} else if (fromtype == TIFFLONG && totype == TIFFSHORT) {
					for (dwCnt = (DWORD)0; dwCnt < dwLength; dwCnt++) {
						TiffLong = *((LPDWORD)lpSshortPtr);
						if (TiffLong > MAXWORD) {
							TiffLong = MAXWORD;
						}
						*((LPWORD)lpDstPtr) = (WORD)TiffLong;
						lpSshortPtr += SshortSize;
						lpDstPtr += DstSize;
					}
				}

				/* make sure that the val section contains the first N values from
				 * the destination array, so that I can have a 3-valued BitsPerSample
				 * (foshorted to be the same) but still get "the" BitsPerSample value
				 * by using the p->iBitsPerSample contruct
				 */
				if (DstBytes > (DWORD)4) {
					memcpy ((LPSTR) &pTF->val.Tdword, (LPSTR)lpDst, 4);
				}

				/* set the new type
				 */
				pTF->Ttype = totype;
			} /* end of unsigned section */

			/* else if none of the above cases, give up.
			 */
			else {
				if (pTF->Talloc) GlobalUnlock (pTF->Thandle);
				if (hDst) {
					UnlockItFreeIt (hDst);
					hDst = (HANDLE) NULL;
				}
				err = IM_BUG;	/* programming error */
				goto done;
			}

		} /* end of conversion section */

		/* if neither the soushorte nor destination is allocated,
		 * just copy into the 4-byte value
		 */
		if (!pTF->Talloc && !hDst) {
			pTF->val.Tdword = DstVal;
		}

		/* else if soushorte and destination are both allocated,
		 * free the old buffer, and store the new destination handle
		 */
		else if (pTF->Talloc && hDst) {
			UnlockItFreeIt (pTF->Thandle);
			pTF->Thandle = hDst;
			GlobalUnlock (hDst);
		}

		/* else if destination is allocated, but not the soushorte,
		 * just store the new destination handle
		 */
		else if (!pTF->Talloc && hDst) {
			pTF->Thandle = hDst;
			GlobalUnlock (pTF->Thandle);
			pTF->Talloc = true;
		}

		/* else if soushorte is allocated, but not the destination,
		 * unlock and free the buffer, and copy the value.
		 */
		else if (pTF->Talloc && !hDst) {
			UnlockItFreeIt (pTF->Thandle);
			pTF->Talloc = false;
			pTF->val.Tdword = DstVal;
		}

		/* return
		 */
done:	return err;
}

/*-----------------------------------------------------------------------*/

/* check for bad values, convert from one format to another
 * if necessary, and store the information in the appropriate
 * TFIELD structure in the IMAG structure
 */
short NicefyTField (
	TFIELD	*pTF,
	IMAG	*x)
{
		short		err = SUCCESS;
		WORD	Tag;

		Tag = pTF->Ttag;
		/* DBMSG(("Nicefy: unsigned tag=%u   signed tag=%d\n",
		 *  pTF->Ttag, pTF->Ttag));
		 */

		if (Tag == TGNEWSUBFILETYPE) {
			if ((err = TypeConvert (pTF, TIFFLONG)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_NEWSUBFILETYPE] = *pTF;	/* structure copy */

		} else if (Tag == TGIMAGEWIDTH) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_IMAGEWIDTH] = *pTF;	/* structure copy */

		} else if (Tag == TGIMAGELENGTH) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_IMAGELENGTH] = *pTF;	/* structure copy */

		} else if (Tag == TGBITSPERSAMPLE) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_BITSPERSAMPLE] = *pTF;	/* structure copy */

		} else if (Tag == TGCOMPRESSION) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_COMPRESSION] = *pTF;	/* structure copy */

		} else if (Tag == TGPHOTOMETRICINTERPRETATION) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_PHOTOMETRICINTERP] = *pTF;	/* structure copy */

		} else if (Tag == TGSTRIPOFFSETS) {
			if ((err = TypeConvert (pTF, TIFFLONG)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_STRIPOFFSETS] = *pTF;	/* structure copy */

		} else if (Tag == TGSAMPLESPERPIXEL) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_SAMPLES] = *pTF;		/* structure copy */

		} else if (Tag == TGSTRIPBYTECOUNTS) {
			if ((err = TypeConvert (pTF, TIFFLONG)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_STRIPBYTECOUNTS] = *pTF;	/* structure copy */

		} else if (Tag == TGROWSPERSTRIP) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_ROWSPERSTRIP] = *pTF;	/* structure copy */

		} else if (Tag == TGXRESOLUTION) {
			
			if ((err = TypeConvert (pTF, TIFFFLOAT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_XRESOLUTION] = *pTF;	// structure copy 
			if (x->fXResolution == (float)0.0)
				x->fXResolution = (float)300.0;

		} else if (Tag == TGYRESOLUTION) {
			if ((err = TypeConvert (pTF, TIFFFLOAT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_YRESOLUTION] = *pTF;	// structure copy 
			if (x->fYResolution == (float)0.0)
				x->fYResolution = (float)300.0;

		} else if (Tag == TGPLANAshortONFIGURATION) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_PLANAR] = *pTF;		/* structure copy */

		} else if (Tag == TGGRAYUNIT) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_GRAYUNIT] = *pTF;	/* structure copy */

		} else if (Tag == TGGRAYCURVE) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_GRAYCURVE] = *pTF;	/* structure copy */

		} else if (Tag == TGRESOLUTIONUNIT) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_RESOLUTIONUNIT] = *pTF;	/* structure copy */

		} else if (Tag == TGPREDICTOR) {
			if ((err = TypeConvert (pTF, TIFFSHORT)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_PREDICTOR] = *pTF;	/* structure copy */

		} else if (Tag == TGKIDS) {
			if ((err = TypeConvert (pTF, TIFFLONG)) != 0)
				goto cu0;
			pTF->Texists = true;
			x->tf[X_KIDS] = *pTF;	/* structure copy */

		} else {
			/* DBMSG(("GtTiffInfo: unrecognized tag %d\n",de.deTag)); */
			/*
			 * no big deal, probably.
			 */
		}

cu0:	return err;
}


/*----------------------------------------------------------------------------*/
/* mhbuild - build an 8k lookup table for CCITT Modified Huffman 1D coding
 */
short MhBuildLut (
	BYTE	inlen,		/* number of elements in lpCode[] and lpLen[] */
	LPBYTE	lpCode,		/* right-justified code string values */
	LPBYTE	lpLen,		/* number of bits in each code string */
	WORD	lutbits,	/* the lut is 2**lutbits long */
	LPBYTE	lpLut)		/* the lut to be filled */
{
		short				err = SUCCESS;
		WORD			nlutentries;
		BYTE  *lpPtr;
		WORD	ii;
		BYTE			inindex;
		WORD	count;

		/* initialize the lut to an impossible index value (255)
		 */
		nlutentries = 1 << (lutbits);
		lmemset ((LPSTR)lpLut, (BYTE)'\377', nlutentries);

		/* for each code string
		 */
		for (inindex = 0; inindex < inlen; inindex++) {
				/* calculate starting lut address
				 */
				lpPtr = lpLut + ((WORD)lpCode[inindex] << (lutbits - lpLen[inindex]));

				/* calculate number of lut entries to fill
				 */
				count = 1 << (lutbits - lpLen[inindex]);

				/* for each entry
				 */
				for (ii = 0; ii < count; ii++) {

					/* put in the index of the current string
					 */
					*lpPtr++ = inindex;
				}
		}
		return err;
}

/*-----------------------------------------------------------------------*/

/* addblackbits
 */
short addblackbits (void)
{
		short			err = SUCCESS;
		WORD			nbytebits;
		WORD			remainder;
		WORD			wholebytes;
		WORD			BitsLeft = runcount;

		/* add to current output byte
		 */
		remainder = EIGHT - outbo;
		nbytebits = (BitsLeft < remainder) ? BitsLeft : remainder;
#ifdef WINDOWS
		*lpOut &= ~(ormask2[nbytebits] <<
		 (EIGHT - (outbo + nbytebits)));
#endif
#ifdef MACINTOSH

		*lpOut |= ormask2[nbytebits] <<
		 (EIGHT - (outbo + nbytebits));

#endif
		outbo += nbytebits;
		BitsLeft -= nbytebits;
		if (outbo == EIGHT) {
			lpOut++;
			outbo = 0;
		}

		/* write out any whole bytes
		 */
		wholebytes = BitsLeft >> 3;		/* BitsLeft / 8 */
		if (wholebytes) {
			lmemset (lpOut, BLACKBYTE, wholebytes);
			lpOut += wholebytes;
			BitsLeft &= 7;				/* BitsLeft %= 8 */
		}

		/* add to current output byte
		 */
		if (BitsLeft) {
#ifdef WINDOWS
			*lpOut &= ~(ormask2[BitsLeft]
			 << (EIGHT - (outbo + BitsLeft)));
#endif
#ifdef MACINTOSH

			*lpOut |= ormask2[BitsLeft]
			 << (EIGHT - (outbo + BitsLeft));

#endif
			outbo += BitsLeft;
		}

		return err;
}

/*-----------------------------------------------------------------------*/

/* MhDecomp
 *
 * This routine expects to be given one compressed row of data.
 *
 * It returns SUCCESS if successful,
 *  else IM_1D_XXX
 *
 * Notes:
 * 1.  Assumes that it should start in WHITE mode.
 * 2.  Uses a precomputed 13-bit lookup table.  See mhbuild.c.
 */

short MhDecomp (
	LPBYTE		lpWlen,			/* white code lengths */
	LPBYTE		lpBlen,			/* black code lengths */
	LPSTR		lpWCodeLut,		/* white code lut */
	LPSTR		lpBCodeLut,		/* black code lut */
	LPSTR 	*plpSshort,		/* compressed data */
	LPSTR		lpDst,			/* decompressed data is put here */
	WORD		DstPixels,		/* expected number of pixels (columns) in the row */
	bool		wantdata)		/* make it true if you want data */
{
		WORD			lutindex = 0;
		LPSTR			lpSshort = *plpSshort;
		WORD			bitsneeded = LUTBITS;
		WORD			grab;
		WORD			bitsleftinbyte;
		short			err = SUCCESS;
		WORD			inbo;		/* input bit offset */
		BYTE			strindex;	/* string (and len) index */
		WORD			len;		/* length of this code word, in bits */
		WORD			outso;
		short			add;
		WORD			too;

		/* initializations
		 */
		inbo = 0;
		outso = 0;
		runcount = 0;
		color = WHITE;
		outbo = 0;
		waitingfortc = '\0';
		lpOut = lpDst;

		/* loop on each code word
		 */
		while (outso < DstPixels || waitingfortc == '\1') {

			/* grab the next 13 bits (i.e., build the lut index)
			 * -- version C
			 * this version doesn't start from scratch each time --
			 * it retains as much information as possible in lutindex.
			 * it required several other changes, so it is not trivial to
			 * go back to versions A or B.  The meaning of lpSshort and inbo
			 * are correspondingly slightly changed.
			 *
			 * since I "look ahead" 13 bits, it would be possible to read
			 * data that I'm not supposed to -- shouldn't be a problem in
			 * any reasonable operating system.
			 */
			while (bitsneeded) {
				bitsleftinbyte = 8 - inbo;
				grab = min (bitsleftinbyte, bitsneeded);
				lutindex <<= grab;
				lutindex |= ((WORD)*lpSshort >> (bitsleftinbyte - grab))
				 & wmask[grab];
				bitsneeded -= grab;
				inbo += grab;
				if (inbo == 8) {
					lpSshort++;
					inbo = 0;
				}
			}
			lutindex &= L13;
			/*
			DBMSG(("%x %d",lutindex,lutindex));
			 */

			/* get the string index (same as len index),
			 * which also gives the run length
			 */
			if (color == WHITE) {
				strindex = (BYTE)lpWCodeLut[lutindex];
				if (strindex == (BYTE)'\377')
					err = IM_1D_BADCODE;
				else
					/* len = (WORD)wlen[strindex]; */
					len = (WORD)lpWlen[strindex];
			}
			else {
				strindex = (BYTE)lpBCodeLut[lutindex];
				if (strindex == (BYTE)'\377')
					err = IM_1D_BADCODE;
				else
					/* len = (WORD)blen[strindex]; */
					len = (WORD)lpBlen[strindex];
			}
			if (err) {
				goto cu0b;
			}
			/*
			DBMSG((" %d %d\n",strindex,len));
			*/

			/* if a muc, just add to the current run count
			 */
			if (strindex >= TCLIM) {	/* muc */
				runcount += TCLIM * (strindex - TCLIM + 1);
				waitingfortc = '\1';
			}

			/* else if a tc,
			 *  add to the current run count,
			 * 	and add to the output
			 */
			else {
				runcount += (WORD)strindex;
				/* check for coding error
				 */
				if (outso + runcount > DstPixels) {
					err = IM_1D_OVERFLOW;
					goto cu0b;
				}

				if (wantdata) {
					if (color == BLACK) {
						if ((err = addblackbits ()) != 0) {
							goto cu0b;
						}
					}
					else {	/* color == WHITE */
						outbo += runcount;
						lpOut += outbo>>3;	/* i.e., /8 */
						outbo %= EIGHT;
					}
				}

				/* update a few other variables
				 */
				color = (color == WHITE) ? BLACK : WHITE;
				outso += runcount;
				runcount = 0;
				waitingfortc = '\0';
			}

			/* update the number of bits that my barrel shifter will want,
			 * in order to always hold the next 13 bits.
			 */
			bitsneeded = len;

		} /* end of code word loop */

		/* adjust lpSshort.  I have to compensate for the fact that lpSshort and
		 * inbo point to the next place to fill my 13-bit "barrel shifter",
		 * lutindex, and not the actual 1-bit-past-the-end of the last code
		 * word.
		 *
		 * I couldn't find a closed form solution, after a few
		 * hours work.  I'm sure there is one, but it might well be nearly
		 * incomprehensible, and the efficiency is unimportant here, since
		 * the code is only executed once per row.
		 */
		add = 1;
		too = LUTBITS - len;
		do {
			if (inbo-- == 0) {
				add--;
				inbo = 7;
			}
		} while (too--);
		*plpSshort = lpSshort + add;

cu0b:	return err;
}

/***************************** External Routines *********************/

/* build the lookup tables, and so on
 */
short OpenTiff2 (
	CFile* pFile,
	LPIMAG x)
{
		short		err = SUCCESS;
		LPSTR	lpWLut,lpBLut;
		WORD	ncodes;
		WORD	rowbytes;
		LPBYTE	lpWstr, lpBstr, lpWlen, lpBlen;

		lpWstr = (LPBYTE) wstr;
		lpBstr = (LPBYTE) bstr;
		lpWlen = (LPBYTE) wlen;
		lpBlen = (LPBYTE) blen;

		/*	x->hT2Wlen = (short)hWlen; */
		x->lpT2Wlen = lpWlen;

		/*	x->hT2Blen = (short)hBlen; */
		x->lpT2Blen = lpBlen;

		/* allocate and lock both code lookup table buffers
		 */
		if (!(x->hWCodeLut = GlobalAlloc (GMEM_MOVEABLE,LUTLEN))) {
			err = MM_GLOBAL_FULL;
			goto cu0d;
		}
		if (!(lpWLut = (LPSTR)GlobalLock (x->hWCodeLut))) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}
		if (!(x->hBCodeLut = GlobalAlloc (GMEM_MOVEABLE,LUTLEN))) {
			err = MM_GLOBAL_FULL;
			goto cu2;
		}
		if (!(lpBLut = (LPSTR)GlobalLock (x->hBCodeLut))) {
			err = MM_LOCK_FAILED;
			goto cu3;
		}

		/* build the code lookup tables
		 */
		/* ncodes = wstrlen; */
		ncodes = T2ENTRIES;
		if ((err = MhBuildLut ((BYTE)ncodes, lpWstr, lpWlen, LUTBITS,  (BYTE  *)lpWLut)) != 0) {
			goto cu4;
		}
		if ((err = MhBuildLut ((BYTE)ncodes, lpBstr, lpBlen, LUTBITS, (BYTE  *)lpBLut)) != 0) {
			goto cu4;
		}

		/* allocate enough memory for a full uncompressed row of data
		 */
		rowbytes = (x->iImageWidth+7)/8;
		if (!(x->hUnRow = GlobalAlloc (GMEM_MOVEABLE, rowbytes))) {
			goto cu4;
		}

		/* set up a few state variables
		 */
		x->NextRow = MAXWORD;
		x->CurStrip = MAXWORD;
		/* x->CurStripSize = 0L; */

		/* unlock the luts and return
		 */
//cu8:	;
//cu7:	/* if (err) GlobalUnlock (x->hVfr[0]); */
//cu6:	;
//cu5:	;
cu4:	GlobalUnlock (x->hBCodeLut);
cu3:	;
cu2:	GlobalUnlock (x->hWCodeLut);
cu1:	;
cu0d:
//cu0:
		return err;
}

/*-----------------------------------------------------------------------*/

/* Tiff2DeStrip - CCITT 1D Decompress-a-Strip
 *
 * unpack a whole strip.  called from TIFFBC.C.  be sure to call OpenTiff2 first; it works
 * for both Tiff2DeStrip and RdTiff2Row.
 *
 * some day, we should be able to kill RdTiff2Row, and keep just this routine.
 */
short Tiff2DeStrip (
	LPIMAG	x,
	WORD	RowsInThisStrip)	/* number of rows in this strip */
{
		short				err = SUCCESS;
		LPSTR			lpSshort;
		LPSTR	lpDst;
		LPSTR	lpWLut, lpBLut;
		WORD	StripRow;

		/* lock the strip buffers
		 */
		if ((lpSshort = (LPSTR)GlobalLock (x->hCmStrip)) == LPNULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}
		if ((lpDst = (LPSTR)GlobalLock (x->hUnStrip)) == LPNULL) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}

		/* lock the decompression luts
		 */
		if (!(lpWLut = (LPSTR)GlobalLock (x->hWCodeLut))) {
			err = MM_LOCK_FAILED;
			goto cu2;
		}
		if (!(lpBLut = (LPSTR)GlobalLock (x->hBCodeLut))) {
			err = MM_LOCK_FAILED;
			goto cu3;
		}


		for (StripRow = 0; StripRow < RowsInThisStrip; StripRow++, lpDst += x->BytesPerRow) {

			lmemset (lpDst, WHITEBYTE, x->BytesPerRow);

			if ((err = MhDecomp (x->lpT2Wlen, x->lpT2Blen, lpWLut, lpBLut,  &lpSshort, lpDst, x->iImageWidth, /* wantdata */ true)) != 0) {
				goto cu4;
			}

		} /* end of loop */

cu4:	GlobalUnlock (x->hBCodeLut);
cu3:	GlobalUnlock (x->hWCodeLut);
cu2:	GlobalUnlock (x->hUnStrip);
cu1:	GlobalUnlock (x->hCmStrip);
cu0:
		return err;
}

/*-----------------------------------------------------------------------*/

WORD AldCalcMask (WORD	BitDepth)
{
		WORD	Mask;

		Mask = (WORD)COM(0);
		Mask <<= BitDepth;	/* shift in low-order zeros */
		Mask = COM(Mask);

		return Mask;
} /* AldCalcMask */

/*-----------------------------------------------------------------------*/

/* expand the codes in the chunk to 16 bits per code,
 * low-order-justified,
 * so that I don't have to make a function call every
 * time I want to get the next code.
 *
 * there must be an EOI code at the end of the chunk, or this routine
 * will get stuck in an infinite loop.
 *
 * assumes that data has been stored "as bytes", but we will
 * access a word at a time, so the caller
 * must be careful to pass even addresses to this routine.
 * this routine swaps bytes if executing
 * on an Intel-type machine.
 *
 * This routine was inspired by Eric R's word-at-a-time
 * variable-bit i/o routines.
 *
 */
short LzwExpandCodes (
	HANDLE	hCmChunk,			/* the input non-expanded codes */
	DWORD	dwChunkByteCount,	/* number of bytes in hCmChunk */
	WORD 	*lpNumCodes,			/* OUT: number of codes, including the EOI */
	HANDLE	hExpCodes)			/* where to put the expanded codes */
{
		short		err = SUCCESS;
		WORD	ChunkByteCount = (WORD)dwChunkByteCount;
		WORD	BitsLeft = 16;
		WORD	diff;
		WORD	Code;
		WORD	Mask;
		WORD	ComprSize;
		WORD	NextOne;
		WORD	NextBoundary;
		LPWORD	lpCmChunk;
		LPWORD	lpCmChunkPtr;
		LPWORD	lpExpCodes;
		LPWORD	lpExpCodesPtr;

		/* DBMSG(("LzwExpandCodes: top\n")); */

		/* lock stuff
		 */
		if ((lpCmChunk = (LPWORD) GlobalLock (hCmChunk)) == (LPWORD)NULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}
		lpCmChunkPtr = lpCmChunk;

		if ((lpExpCodes = (LPWORD) GlobalLock (hExpCodes)) == (LPWORD)NULL) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}
		lpExpCodesPtr = lpExpCodes;

		/* swap bytes on Intel-type machine
		 */
		if (ODD(ChunkByteCount))
			ChunkByteCount++;
		if (LowToHigh()) { /* not tested under Windows as of 88-5-11 */
			swapb ((LPWORD)lpCmChunk, (LPWORD)lpCmChunk, ChunkByteCount);
		}

		/* other setup
		 */
		NextOne = EOICODE + 1;	/* so that we know when we are about
								 * to cross over a bit boundary.
								 * used similarly to "Empty" in
								 * compression routines
								 */
		ComprSize = CHARBITS + 1;
		NextBoundary = 1 << ComprSize;
		Mask = AldCalcMask(ComprSize);

		/* DBMSG(("Codes:\n")); */
		do {	/* TODO: figure out a safer way - VJH - In Think also */

			/* break into cases to grab the next code
			 */
			if (BitsLeft > ComprSize) {
				/* set and stay in same word */
				BitsLeft -= ComprSize;
				Code = (*lpCmChunkPtr >> BitsLeft) & Mask;

			} else if (BitsLeft < ComprSize) {
				/* Code is across a word boundary */
				diff = ComprSize - BitsLeft;
				Code = (*lpCmChunkPtr++ << diff) & Mask;
				if ( ((char*)lpCmChunkPtr) - ((char*)lpCmChunk) >= ChunkByteCount) {
					err = CM_DECOMPRESSION;
					goto cu2;
				}
				BitsLeft = 16 - diff;
				Code |= (*lpCmChunkPtr >> BitsLeft);

			} else {	/* equal */
				/* set and move on to the next word */
				Code = *lpCmChunkPtr++ & Mask;
				BitsLeft = 16;
			}

			/* store the result
			 */
			*lpExpCodesPtr++ = Code;

			/* check for CLEAR code
			 */
			if (Code == CLEAshortODE) {
				NextOne = EOICODE + 1;
				ComprSize = CHARBITS + 1;
				NextBoundary = 1 << ComprSize;
				Mask = AldCalcMask(ComprSize);
			}

			/* if at bit boundary, adjust compression size
			 */
			else if (++NextOne == NextBoundary) {
				ComprSize++;
				if (ComprSize > MAXCODEWIDTH) {
					/* RFB 90-04-20 fix case where EOICODE coincides with
					 * a full table.
					 */
					if (Code == EOICODE) {
						continue;	/* to end of loop (we're done) */
					}
					err = CM_DECOMPRESSION;
					goto cu2;
				}
				NextBoundary <<= 1;
				Mask = AldCalcMask(ComprSize);
			}

		} while (Code != EOICODE);

		/* store output information
		 * (caution: word arithmetic)
		 */
		*lpNumCodes = lpExpCodesPtr - lpExpCodes;

		/* unlock stuff and return
		 */
cu2:	GlobalUnlock (hExpCodes);
cu1:	GlobalUnlock (hCmChunk);
cu0:	/* DBMSG(("LzwExpandCodes: bottom\n")); */
		return err;
}

/*-----------------------------------------------------------------------*/

/* decompress an entire chunk
 *
 * assumptions:
 * 1. the input codes have already been expanded to 16-bit codes.
 * 2. the first code in a chunk is CLEAR.
 */
short LzwDecodeChunk (
	HANDLE				hExpCodes,		/* input expanded codes */
	HANDLE				hTable,			/* the LZW table */
	WORD				/*NumCodes*/,		/* number of codes */
	LPBYTE				lpUnChunk,		/* output uncompressed bytes */
	DWORD				dwOutExpected)	/* number of bytes expected in output */
{
		short		err = SUCCESS;
		LPTREENODE	lpTab;
		LPBYTE		lpOutPtr;
		LPWORD		lpExpCodes;	/* the input codes, an expanded (16-bit) version of the compressed chunk */
		WORD		Code;
		WORD		Old;
		WORD		StringToWrite;
		WORD		Empty;
		DWORD				dwOutSo = 0L;
		BYTE		FirstChar;
		BYTE		OutStringLength;


		/* lock down the table
		 */
		if ((lpTab = (LPTREENODE) GlobalLock (hTable))
		 == (LPTREENODE)NULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}

		/* lock down the expanded input code buffer
		 */
		if ((lpExpCodes = (LPWORD) GlobalLock (hExpCodes)) == (LPWORD)NULL) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}

		/* if the first code is not a clear code, give up
		 */
		if (*lpExpCodes != CLEAshortODE) {
			err = CM_LZW_INITCLEAR;
			goto cu3;
		}

		/* get the next code, while there are codes to be gotten...
		 */
		/* DBMSG(("Codes:\n")); */
		while ((Code = *lpExpCodes++) != EOICODE) {


			/* if 'Clear'...
			 */
			if (Code == CLEAshortODE) {

				/* do the clear
				 */
				Empty = EOICODE + 1;

				/* get the next code, to prime the pump.
				 * output the code to the charstream,
				 * i.e., the (expanded-pixel) output buffer.
				 * (we assume code = char for roots, which is true for our data).
				 * initialize "old-code": <old> = <code>
				 *
				 * make sure we don't get mixed up by a multiple-
				 * Cleashortode situation, which shouldn't ever happen,
				 * but why take the chance...
				 */
				while ((Code = *lpExpCodes++) == CLEAshortODE) {
					;
				}

				/* Code = *lpExpCodes++; */
				if (Code == EOICODE) {
					break;
				}
				/* DBMSG((" %u\n",Code)); */
				/* DBMSG((" %u at %u\n", Code, (WORD)lpUnChunk & 0xffff )); */
				*lpUnChunk++ = (BYTE)Code;
				dwOutSo++;
				Old = Code;
				FirstChar = (BYTE)Code;

				/* continue to the very bottom of the loop
				 */
				continue;

			} /* end of clear-handler */

			/* otherwise, we have a normal code, so...
			 */
			else {

				/* TODO MAYBE: add a special case for roots? - VJH - In Think also
				 */

				/* if <code> exists in the string table...
				 *
				 * described this way in the LZW paper:
				 *
				 * "output the string for <code> to the charstream"
				 * "add the correct entry to our table"
				 *		"get string [...] corresponding to <old>"
				 *		"get first character K of string corresponding to <code>"
				 *		"add [...]K to the string table"
				 * "<old> = <code>"
				 *
				 * we do it a little differently...
				 */
				if (Code < Empty) {
					StringToWrite = Code;
					OutStringLength = (lpTab + Code)->StringLength;	/* Old to Code, 5-5, 3pm */
					lpUnChunk += OutStringLength;
					lpOutPtr = lpUnChunk;
				}

				/* else if <code> does not exist in the string table...
				 *
				 * described this way in the paper:
				 *
				 * "get string [...] corresponding to <old>"
				 * "get K, the first character of [...]"
				 * "output [...]K to the charstream"
				 * "add it to the string table"
				 * "<old> = <code>"
				 *
				 * we do it a little differently, but with the same effect:
				 */
				else if (Code == Empty) {
					/* DBMSG((" eq\n")); */
#ifdef DEBUG
					if (Old == CLEAshortODE) {
						err = CM_DECOMPRESSION;
						goto cu3;
					}
#endif
					StringToWrite = Old;
					OutStringLength = (lpTab + Old)->StringLength + 1;
					lpUnChunk += OutStringLength;
					lpOutPtr = lpUnChunk;
					/* DBMSG((" %u at %u\n", (WORD)FirstChar, ((WORD)lpOutPtr - 1) & 0xffff )); */
					*--lpOutPtr = FirstChar;

				} else {
					err = CM_DECOMPRESSION;
					goto cu3;
				}

				/* write out the rest of the string, by walking up the tree
				 */
				{
					LPTREENODE	lpNode;
					WORD		TabIndex = StringToWrite;
					LPBYTE		lpOutPtr2;

					/* DBMSG(("OutStringLength=%u\n", OutStringLength)); */
					dwOutSo += (DWORD)OutStringLength;

					lpOutPtr2 = lpOutPtr;
					do {
						lpNode = lpTab + TabIndex;
						/* DBMSG((" %u at %u\n", (WORD)lpNode->Suffix, ((WORD)lpOutPtr - 1) & 0xffff )); */
						*--lpOutPtr = lpNode->Suffix;
						TabIndex = lpNode->Parent;
					} while (TabIndex != MAXWORD);
					lpOutPtr = lpOutPtr2;

					/* keep the first char around, so that when we need
					 * the first char of <old>, it will be available
					 */
					FirstChar = lpNode->Suffix;
				}

				/* add the correct entry to our table
				 */
				{
					LPTREENODE	lpNode;

					lpNode = lpTab + Empty++;		/* our new table entry */
					lpNode->Suffix = FirstChar;
					lpNode->StringLength = (lpTab + Old)->StringLength + 1;
					lpNode->Parent = Old;			/* parent is always Old */
					/* DBMSG(("adding entry %u: Suffix=%u StringLength=%u Parent=%u\n",
					 *  (Empty-1), FirstChar, lpNode->StringLength, lpNode->Parent));
					 */

				} /* end of entry-adding */

				/* <old> = <code>
				 */
				Old = Code;

				/* check for overflow
				 */
				if (dwOutSo > dwOutExpected) {
					err = CM_DECOMPRESSION;
					goto cu3;
				}
				if (Empty >= MAXTABENTRIES) {
					err = CM_DECOMPRESSION;
					goto cu3;
				}

			} /* end of normal-code section */

		} /* end of main code loop */

#ifdef DEBUG
		/* we're done with the chunk, so check the output
		 * count to make sure everything looks ok.
		 */
		if (dwOutSo != dwOutExpected) {
			err = CM_DECOMPRESSION;
			goto cu3;
		}

#endif /* DEBUG */

		/* store local things back
		 */

		/* unlock and return
		 */
cu3:	/* GlobalUnlock (hUnChunk) */;
		GlobalUnlock (hExpCodes);
cu1:	GlobalUnlock (hTable);
cu0:	;
		/* DBMSG(("LzwDecodeChunk: bottom\n")); */
		return err;
}


/*****************************/

/* Lzw Decompression "OPEN" routine: allocate buffers, do some
 * preliminary calculations, and so on, so that we don't have to
 * do it for every chunk
 */
short LzwDeOpen (
	DWORD		dwMaxOutBytes,		/* maximum output (i.e. uncompressed ) bytes per chunk */
	LPHANDLE	phTable,			/* OUT: table allocated by this routine */
	LPHANDLE	phExpCodesBuf)		/* OUT: place to put the expanded codes */
{
		short					err = SUCCESS;
		HANDLE				hTable;

		/* DBMSG(("LzwDeOpen: top\n")); */

		/* allocate the string table
		 */
		if (!(hTable = GlobalAlloc (GMEM_MOVEABLE, sizeof(TREENODE) * MAXTABENTRIES))) {
			err = MM_GLOBAL_FULL;
			goto cu2;
		}

		/* initialize the string table
		 */
		{
			WORD	nRoots = 1<<CHARBITS;
			WORD	ii;
			LPTREENODE	lpNode;

			if ((lpNode = (LPTREENODE)GlobalLock (hTable)) == (LPTREENODE)NULL) {
				err = MM_LOCK_FAILED;
				goto cu3;
			}
			(lpNode + CLEAshortODE)->StringLength = 1;	/* useful to avoid special case for <old> */
			for (ii = 0; ii < nRoots; ii++, lpNode++) {
				lpNode->Suffix = (BYTE)ii;
				lpNode->StringLength = 1;
				lpNode->Parent = MAXWORD;	/* signals the top of the tree */
			}
			GlobalUnlock (hTable);
		}

		/* calculate the maximum string length.  the worst case, of course,
		 * is when the input string is made up of all the same character.
		 * if the input buffer is made up of 1 character, the max output string is
		 * 1 character.  if input is 3 characters, the max output string is
		 * 2 characters.  if input is 6 characters, the max output string is 3
		 * characters.  if input is 10 characters, the max output string is 4
		 * characters.  so, we can use the old sum of an arithmetic sequence
		 * formula:  Sum = n*(n+1)/2.  We want n, given s, so using the
		 * quadratic formula we have n = (-1 + sqrt(1 + 8*Sum)) / 2.
		 * our "Sum" is the length of the input data, which is the number
		 * of pixels in a chunk.
		 *
		 * allocate a MaxStringLen-byte buffer to hold a reversed string, which
		 * is the way we first get it.
		 *
		 * FLASH: don't need it, because I'm not reversing any more.
		 */

		/* allocate the expanded-codes buffer.  I need to know how many codes
		 * I have, and then I need a word for each code.  Now, how do I know
		 * how many codes I have?  I guess I don't.  I could make people store it
		 * in the TIFF file, but that's kind of a pain...So what is the worst case?
		 * The worst case is probably one code per input character.
		 * (UPDATE: out input characters are always 8-bit bytes, now.)
		 * Actually, the worst case is infinite, since you could string an infinite
		 * number of Cleashortodes together, but I guess we can safely assume that
		 * that won't happen.
		 */
		/* IMBUGFIX: add 1 for Cleashortode, 88-10-28 */
		if (!(*phExpCodesBuf = GlobalAlloc (GMEM_MOVEABLE, (dwMaxOutBytes+1) * sizeof(WORD)))) {
			err = MM_GLOBAL_FULL;
			goto cu3;
		}

		/* store return values
		 */
		*phTable = hTable;

		if (err) GlobalFree (*phExpCodesBuf);
cu3:	if (err) GlobalFree (hTable);
cu2:	;
		;
		return err;
}

/*-----------------------------------------------------------------------*/

/* decompress a chunk, referenced by pointer instead of handle
 * (the new, preferred interface)
 */
short LzwDePChunk (
	HANDLE	hCmChunk,			/* the compressed chunk */
	DWORD	dwChunkByteCount,	/* number of bytes in the compressed chunk */
	HANDLE	hExpCodesBuf,		/* work buffer for the expanded codes */
	HANDLE	hTable,				/* buffer to hold the decompression table */
	DWORD	dwOutExpected,		/* number of output (uncompressed) characters (bytes) expected */
	LPBYTE	lpUnChunk)			/* where to put the uncompressed chunk data */
{
		short		err = SUCCESS;
		WORD	nCodes;	/* number of codes, including the EOI */

		/* DBMSG(("LzwDeChunk: top\n")); */

		/* expand the codes in the chunk to 16 bits per code
		 */
		if ((err = LzwExpandCodes (hCmChunk, dwChunkByteCount, &nCodes, hExpCodesBuf)) != 0) {
			goto cu0;
		}

		/* Decode the entire chunk
		 */
		if ((err = LzwDecodeChunk (hExpCodesBuf, hTable, nCodes, lpUnChunk, dwOutExpected)) != 0) {
			goto cu0;
		}

cu0:	/* DBMSG(("LzwDeChunk: bottom\n")); */
		return err;
}

/*-----------------------------------------------------------------------*/

/* decompress a chunk
 */
short LzwDeChunk (
	HANDLE	hCmChunk,			/* the compressed chunk */
	DWORD	dwChunkByteCount,	/* number of bytes in the compressed chunk */
	HANDLE	hExpCodesBuf,		/* work buffer for the expanded codes */
	HANDLE	hTable,				/* buffer to hold the decompression table */
	DWORD	dwOutExpected,		/* number of output (uncompressed) characters (bytes) expected */
	HANDLE	hUnChunk)			/* where to put the uncompressed chunk data */
{
		short		err = SUCCESS;
		LPBYTE	lpUnChunk;

		/* lock down the output (uncompressed) buffer
		 */
		if ((lpUnChunk = (LPBYTE) GlobalLock (hUnChunk)) == (LPBYTE)NULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}

		/* call the more general pointer-based version
		 */
		err = LzwDePChunk (hCmChunk, dwChunkByteCount, hExpCodesBuf, hTable,
		 dwOutExpected, lpUnChunk);

		/* clean up and return
		 */
		GlobalUnlock (hUnChunk);
cu0:	return err;
}

/*-----------------------------------------------------------------------*/

/* CLOSE:  free up buffers, and so on.
 */
short LzwDeClose (
	HANDLE	hExpCodesBuf,
	HANDLE	hTable)
{
		short			err = SUCCESS;

		/* DBMSG(("LzwDeClose: top\n")); */

		GlobalFree (hExpCodesBuf);
		GlobalFree (hTable);

		/* DBMSG(("LzwDeClose: bottom\n")); */
		return err;
}

/*-----------------------------------------------------------------------*/

/* Lzw Decompression "OPEN" routine: allocate buffers, do some
 * preliminary calculations, and so on, so that we don't have to
 * do it for every strip
 *
 * tiffbc.c allocates the main strip input and output buffers, hCmStrip
 * and hUnStrip, so we don't have to worry about that here.
 *
 * tifftile.c calls this, too. it also allocates its input and output
 * tile buffers.
 */
short ImLzwDeOpen (
	LPIMAG	 x,	/* for the main strip input and output buffer handles */
	DWORD dwMaxOutBytesPerBuf) /* maximum output (i.e. uncompressed ) bytes per "chunk" */
{
		short					err = SUCCESS;
		LPLZWDE	lpDD;	/* LzwDecompression structure */

		/* DBMSG(("ImLzwDeOpen: top\n")); */

		/* allocate our main LZW DEcompression structure,
		 * and lock it
		 */
		if ((x->hDe = GlobalAlloc (GMEM_MOVEABLE, sizeof(LZWDE))) == HNULL) {
			err = MM_GLOBAL_FULL;
			goto cu0;
		}
		if ((lpDD = (LPLZWDE)GlobalLock (x->hDe)) == (LPLZWDE)NULL) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}

		/* allocate the table and the expanded-codes buffer
		 */
		if ((err = LzwDeOpen (dwMaxOutBytesPerBuf, &lpDD->hTable, &lpDD->hExpCodesBuf)) != 0) {
			goto cu2;
		}

cu2:	GlobalUnlock (x->hDe);
cu1:	if (err) GlobalFree (x->hDe);
cu0:	/* DBMSG(("ImLzwDeOpen: bottom\n")); */
		return err;
}

/*-----------------------------------------------------------------------*/

/* decompress an entire strip/tile
 */
short ImLzwDeStrip (
	LPIMAG	x,
	DWORD	dwStripByteCount,	/* number of bytes in the compressed strip (or tile) */
	DWORD	dwOutExpected,		/* number of bytes in the uncompressed strip/tile */
	HANDLE	hOut)				/* where to put the uncompressed strip/tile */
{
		short		err = SUCCESS;
		LZWDE	dd;

		/* DBMSG(("ImLzwDeStrip: top\n")); */

		/* lock down the main LZW DEcompression structure, copy
		 * it to a local version, and unlock it
		 */
		{
			LPLZWDE	lpDD;

			if ((lpDD = (LPLZWDE) GlobalLock (x->hDe))
			 == (LPLZWDE)NULL) {
				err = MM_LOCK_FAILED;
				goto cu0;
			}
			dd = *lpDD;
			GlobalUnlock (x->hDe);
		}

		/* call the non-image-specific routine
		 */
		if ((err = LzwDeChunk (x->hCmStrip, dwStripByteCount, dd.hExpCodesBuf, dd.hTable, dwOutExpected, hOut /* x->hUnStrip */)) != 0) {
			goto cu0;
		}

cu0:	/* DBMSG(("ImLzwDeStrip: bottom\n")); */
		return err;
}

/*-----------------------------------------------------------------------*/

/* CLOSE:  free up buffers, and so on.
 */
short ImLzwDeClose (
	LPIMAG	x)
{
		short			err = SUCCESS;
		LPLZWDE	lpDD;

		/* DBMSG(("ImLzwDeClose: top\n")); */

		/* lock down the main LZW DEcompression structure
		 */
		if ((lpDD = (LPLZWDE) GlobalLock (x->hDe))
		 == (LPLZWDE)NULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}

		/* call the non-image-specific version
		 */
		if ((err = LzwDeClose (lpDD->hExpCodesBuf, lpDD->hTable)) != 0) {
			goto cu1;
		}

cu1:	GlobalUnlock (x->hDe);

cu0:	/* DBMSG(("ImLzwDeClose: bottom\n")); */
		return err;
}

/*-----------------------------------------------------------------------*/

void InitImag (
	IMAG *p)
{
		/* zero out the structure, mainly for the existence fields and handles
		 */
		lmemset (p, '\0', sizeof(IMAG));

		/* fill in simple defaults
		 */
#ifdef WINDOWS
		p->iFileType = INTELTIFF;
#endif
#ifdef MACINTOSH
		p->iFileType = MOTOROLATIFF;
#endif

		/* fill in some defaults
		 */
		p->iBitsPerSample = 1;
		p->iSamples = 1;
		p->iPredictor = PREDICTOR_NONE;
		p->iRowsPerStrip = MAXWORD;
		p->iPhotometricInterpretation = WHITEZERO;
		p->iCompression = PACKINTOBYTES;
}

/* -------------------------------------------------------------------------------- */

/* set up the buffering routine, and so on
 */
short OpenBC (
	CFile* pFile,
	LPIMAG x)
{
		short				err = SUCCESS;
		DWORD			StripSize;
		DWORD			MaxByteCount;
		DWORD			SafetyMargin;

		/* DBMSG(("OpenBC: top\n")); */

		/* calculate the uncompressed strip buffer size
		 */
		/* DBMSG(("OpenBC: BytesPerRow = %u\n",x->BytesPerRow)); */
		StripSize = (DWORD)x->iRowsPerStrip * (DWORD)x->BytesPerRow;
		/* DBMSG((" StripSize=%lu\n",StripSize)); */

		/* calculate an overflow amount, for safety when using
		 * UnpackBits
		 */
		SafetyMargin = 128;

		/* Calculate the maximum strip byte count. */

		if (x->iCompression == 1) {
				err = kUnCompressMsg;
				goto cu0;	
		}



		if ((err = GtMaxTLong(x, X_STRIPBYTECOUNTS, (LPDWORD)&MaxByteCount)) != 0) {
			if (x->iCompression == 1)
				err = kUnCompressMsg;
			goto cu0;
		}

		if ( (x->iCompression == 1) && (x->tf[X_STRIPOFFSETS].Tlength <= 1) &&
												(MaxByteCount >= StripSize) )	{

			x->tf[X_STRIPBYTECOUNTS].val.Tdword = MaxByteCount = StripSize;
			err = kUnCompressMsg;
			goto cu0;
		}

		/* allocate an uncompressed strip buffer
		 */
		if (!(x->hUnStrip = GlobalAlloc (GMEM_MOVEABLE, StripSize + SafetyMargin))) {
			err = MM_GLOBAL_FULL;
			goto cu0;
		}

		/* allocate a compressed strip buffer
		 */
		if (ODD(MaxByteCount))
			MaxByteCount++;

		if (!(x->hCmStrip = GlobalAlloc (GMEM_MOVEABLE, MaxByteCount))) {
			err = MM_GLOBAL_FULL;
			goto cu2;
		}

		/* we have not decompressed a strip yet, so set the
		 * current strip to an impossible (by convention, at least!)
		 * value
		 */
		x->CurStrip = MAXWORD;

		/* allocate enough memory for a full uncompressed row of data,
		 * expanded to 8 bits,
		 * plus extra for LZW un-differencing overrun (see HorizAdd())
		 */
		if (x->iPredictor == PREDICTOR_HDIFF) {
			WORD ExpSize;

			ExpSize = x->iImageWidth * x->iSamples;
			if (!(x->hUnRow = GlobalAlloc (GMEM_MOVEABLE, ExpSize + ADDOVERFLOW))) {
				err = MM_GLOBAL_FULL;
				goto cu3;
			}

			/* allocate enough memory for a full uncompressed row of data,
			 * at it's native size,
			 * plus extra for algorithm overrun
			 */
			if (!(x->hBuf1 = GlobalAlloc (GMEM_MOVEABLE, x->BytesPerRow + ADDOVERFLOW))) {
				err = MM_GLOBAL_FULL;
				goto cu4;
			}
		} /* end HDIFF */

		/* do any necessary decompression-specific setup
		 */
		{
			if (x->iCompression == LZW) {
				if ((err = ImLzwDeOpen (x, StripSize)) != 0) {
					goto cu5;
				}
			} else if (x->iCompression == CCITT1D) {
				if ((err = OpenTiff2 (pFile, x)) != 0) {
					goto cu5;
				}
			}
		}

		/* return
		 */
//cu6:
		if (err) {
			if (x->iCompression == LZW) {
				ImLzwDeClose(x);
			}
		}
cu5:	if (err) {
			GlobalFree (x->hBuf1);
			x->hBuf1 = HNULL;
		}
cu4:	if (err) {
			GlobalFree (x->hUnRow);
			x->hUnRow = HNULL;
		}
cu3:	if (err) {
			GlobalFree (x->hCmStrip);
			x->hCmStrip = HNULL;
		}
cu2:	;
//cu1:
		if (err) {
			GlobalFree (x->hUnStrip);
			x->hUnStrip = HNULL;
		}
cu0:	/* DBMSG(("OpenBC: bottom\n")); */

		return err;
}	/* OpenBC */

/* -------------------------------------------------------------------------------- */

/* close - perform any necessary cleanup
 */
void CloseBC (IMAG *x)
{
		short err = SUCCESS;

		/* DBMSG(("CloseBC: top\n")); */

		if (x->iCompression == LZW) {
			if ((err = ImLzwDeClose (x)) != 0) {
			}
		}

		/* DBMSG(("CloseBC: bottom\n")); */
		return;
}	/* CloseBC */

/*-----------------------------------------------------------------------*/

void CloseImag (
	IMAG *p)
{
		WORD	ii;

		/* free allocated field data:
		 */
		for (ii = 0; ii < NTFIELDS; ii++) {
			if (p->tf[ii].Talloc) {
				GlobalFree (p->tf[ii].Thandle);
				p->tf[ii].Talloc = false;
				p->tf[ii].Thandle = HNULL;	/* probably unnecessary */
				p->tf[ii].Texists = false;	/* probably unnecessary */
			}
		}

		/* some generic buffers
		 */
		if (p->hUnRow) { GlobalFree (p->hUnRow); p->hUnRow = HNULL; }
		if (p->hBuf1) { GlobalFree (p->hBuf1); p->hBuf1 = HNULL; }

		/* TIFF 1D
		 *
		 * Note: hRowData is used also by some other import filters, such
		 * as the Windows 2.x Paint import.
		 */
		if (p->hWCodeLut) { GlobalFree (p->hWCodeLut); p->hWCodeLut = HNULL; }
		if (p->hBCodeLut) { GlobalFree (p->hBCodeLut); p->hBCodeLut = HNULL; }
		/* if (p->hStrip) { free (p->hStrip); p->hStrip = HNULL; } */


		/* some stuff for LZW decompression.  note that I should really have an
		 * ID of some sort at the beginning of the hDe glob, so that I know
		 * that I should call a certain decompression-close routine.  but the
		 * following code is sufficient for now, since I only use hDe for
		 * LZW decompression.  SEC  89-01-20.
		 */
		/* DBMSG((" compression=%u\n",p->iCompression)); */
		if ((p->iCompression == LZW)
		 && p->hDe) {	/* last test is to make sure my compression-testing
			 					 * software doesn't try to free this stuff, since
			 					 * it doesn't use it.  see imxtest.c, imx3.c.
			 					 */
		 	/* DBMSG((" in LZW case\n")); */
			CloseBC (p);
			if (p->hDe)  { GlobalFree (p->hDe);  p->hDe  = HNULL; }
		}

		/* these critters used to be used only for LZW decompression, but now
		 * they're used for several kinds of decompression, since TiffBC.c was
		 * enhanced to handle more than just LZW...and other modules also reference
		 * them, I believe, such as tiff2.c.  Thanks for the bug fix pointer,
		 * Rusty!  SEC  89-01-20.
		 */
		if (p->hUnStrip) { GlobalFree (p->hUnStrip); p->hUnStrip = HNULL; }
		if (p->hCmStrip) { 
			GlobalFree (p->hCmStrip); 
			p->hCmStrip = HNULL; 
		}

		return;
}
/*-----------------------------------------------------------------------*/

/* "decompress" an uncompressed strip by copying from the "compressed" buffer
 * to the "uncompressed" buffer
 *
 * too trivial to rate its own module
 */
short UnDeStrip (
	HANDLE	hCmStrip,			/* copy from here */
	DWORD	dwStripByteCount,	/* number of compressed (and uncompressed) bytes to copy */
	HANDLE	hUnStrip)			/* copy to here */
{
		short		err = SUCCESS;
		LPSTR	lpCmStrip;
		LPSTR	lpUnStrip;

		/* I could easily fix the following restriction, but we don't want strips that
		 * large anyway.  Some day, when images get so large that 32K is not big enough
		 * for a row of image data, we'll have to convert this stuff to DWORDS.
		 * Note that there is a parallel test in tiffdiag.c.
		 */

		if ((lpCmStrip = (LPSTR)GlobalLock (hCmStrip)) == LPNULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}
		if ((lpUnStrip = (LPSTR)GlobalLock (hUnStrip)) == LPNULL) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}

		memcpy (lpCmStrip, lpUnStrip, dwStripByteCount);

//cu2:
		GlobalUnlock (hUnStrip);
cu1:	GlobalUnlock (hCmStrip);
cu0:
		return err;
}	/* UnDeStrip */

/* -------------------------------------------------------------------------------- */


/* PbDeStrip - PackBits Decompress-a-Strip
 *
 * unpack a whole strip.  called from TIFFBC.C.
 *
 * some day, we should be able to kill everything in this module except for this routine.
 */
short PbDeStrip (
	HANDLE		hCmStrip,			/* in:  compressed strip */
	HANDLE		hUnStrip,			/* out: uncompressed strip. make sure it is 128 bytes too big. */
	WORD		BytesPerRow,		/* number of uncompressed bytes per row */
	WORD		RowsInThisStrip)	/* number of rows in this strip */
{
		short		err = SUCCESS;
		LPSTR	lpCmStrip;
		LPSTR	lpUnStrip;
		LPSTR	lpSshort, lpPrevSshort;
		LPSTR	lpDst, lpPrevDst;

		if ((lpCmStrip = (LPSTR)GlobalLock (hCmStrip)) == LPNULL) {
			err = MM_LOCK_FAILED;
			goto cu0;
		}
		if ((lpUnStrip = (LPSTR)GlobalLock (hUnStrip)) == LPNULL) {
			err = MM_LOCK_FAILED;
			goto cu1;
		}

		lpSshort = lpCmStrip;
		lpDst = lpUnStrip;
		while (RowsInThisStrip--) {

			lpPrevSshort = lpSshort;
			lpPrevDst = lpDst;

			if (err = UnpackBits (lpSshort, lpDst, BytesPerRow)) {
				goto cu2;
			}
			if (lpDst - lpPrevDst != BytesPerRow) {
				err = IM_PACKBITS_OVER;
				goto cu2;
			}
		} /* end of loop */

cu2:	GlobalUnlock (hUnStrip);
cu1:	GlobalUnlock (hCmStrip);
cu0:

		return err;
}

/*-----------------------------------------------------------------------*/

/* horizontal addition
 *
 * assumes 8-bit samples
 *
 * also, we'll eventually need a version that can handle
 * multiple-sample-per-pixel data
 */
void HorizAdd (
	WORD 	ImWidth,	/* number of pixels per row */
	LPBYTE	lpRow)		/* must be 8 bytes too long (not 7) */
{
		WORD		nGroups = (ImWidth+7) >> 3;
		BYTE		LastVal;

		/* we'll do groups of 8
		 */
		LastVal = *lpRow++;
		while (nGroups--) {
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
			LastVal += *lpRow;
			*lpRow++ = LastVal;
		}

		return;
}

/*-----------------------------------------------------------------------*/


/* horizontal addition
 *
 * assumes 24-bit RGBRGB data (multiple-sample-per-pixel)
 *
 * WARNING: requires 12-byte overflow, since lpR is 3 more than lpL. (see imadd.h).
 */
void HorizRgbAdd (
	WORD 	ImWidth,	/* number of pixels per row */
	LPBYTE	lpRow)		/* must be SAMPLESPERPIXEL * PIXELSPERGROUP bytes too long */
{
		LPBYTE		lpL = lpRow;
		LPBYTE		lpR = lpRow + 3;
		WORD		nGroups = (ImWidth + PIXELSPERGROUP -1 ) / PIXELSPERGROUP;

		while (nGroups--) {		/* not tested as of 88-09-20 */
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
			*lpR++ += *lpL++;
		}

		return;
} /* HorizRgbAdd */


/*-----------------------------------------------------------------------*/

void baP4P8Hi (
	WORD		p,
	LPBYTE	frombuf,
	LPBYTE	tobuf)
{
		while (p--) {
			*tobuf++  =  *frombuf & 0xf0;
			*tobuf++  = (*frombuf++ << 4) & 0xf0;
		}
}

/*-----------------------------------------------------------------------*/

/* note -- this routine still works if frombuf and tobuf point to the
 * same buffer
 *
 * soushorte data is high-order justified.
 */
short P8HiToP4 (
	WORD		n,					/* number of pixels */
	BYTE 	*frombuf,	/* 8-bit pixels */
	BYTE 	*tobuf)		/* 4-bit pixels */
{
		short				err = SUCCESS;
		WORD	wb;		/* number of whole output bytes */
		WORD			r;		/* remainder -- 0 or 1 */

		wb = n>>1;
		r = n&1;

		while (wb--) {
			*tobuf    =  *frombuf++       & 0xf0;
			*tobuf++ |= (*frombuf++ >> 4) & 0xf;
		}

		if (r == 0) goto done;
		*tobuf = *frombuf++ & 0xf0;

done:	return err;
}

/*-----------------------------------------------------------------------*/

short P4toP8Hi (
	WORD				n,
	WORD				startbit,	/* 0 or 4 */
	BYTE 	*frombuf,
	BYTE 	*tobuf)
{
		short				err = SUCCESS;
		WORD	p;	/* number of pairs */
		WORD			r;	/* remainder */

		if (n == 0)
			goto done;

		if (startbit == 4) {
			*tobuf++ = (*frombuf++ << 4) & 0xf0;
			n--;
		} else if (startbit == 0) {
		} else {
			err = IM_BUG;
			goto done;
		}

		p = n>>1;	/* n/2 */
		r = n&1;	/* n%2 */

		baP4P8Hi (p,frombuf,tobuf);
		frombuf += p;
		tobuf += (p<<1);

		if (r == 0) goto done;
		*tobuf++  =  *frombuf & 0xf0;

done:	;
		return err;
}

/*-----------------------------------------------------------------------*/

/* un-do horizontal differencing predictor
 *
 */
short UnHorDiff (
	WORD	BitsPerSample,
	WORD	SamplesPerPixel,	/* 3 for RGB, else 1 */
	DWORD	PixelsWide,			/* number of pixels in a tile/strip row */
	LPBYTE	lpIn,				/* input data row */
	LPBYTE	lpExp,				/* data expanded into 8-bit samples */
							/* must be at least 7 pixels too large */
	LPBYTE	lpOut)				/* output, not expanded */
{
		short		err = SUCCESS;
		DWORD	SamplesPerRow;

		SamplesPerRow = SamplesPerPixel * PixelsWide;

		/* convert to 8 bits if necessary, which is required by the add routines
		 */
		switch (BitsPerSample) {
		case 4:
			/* P4toP8Hi doesn't overrun, as of 89-11-20 */
			if ((err = P4toP8Hi ((WORD)SamplesPerRow, 0, lpIn, lpExp)) != 0) {
				goto cu0;
			}
			break;
		case 8:
			/* copy into another buffer. we might need overflow space for
			 * some algorithms...
			 */
			memcpy ((LPSTR)lpExp, (LPSTR)lpIn, (WORD)SamplesPerRow);
			break;
		default:
			err = IM_BUG;
			goto cu0;
		}

		/* add, to reverse the differencing predictor
		 *
		 * warning: these routines overrun lpExp!
		 */
		if (SamplesPerPixel == 3) {
			HorizRgbAdd ((WORD)PixelsWide, lpExp);
		} else {
			HorizAdd ((WORD)PixelsWide, lpExp);
		}

		/* convert from expanded to the output bit depth
		 */
		switch (BitsPerSample) {
		case 4:
			/* P8HiToP4 does not overrun, as of 89-11-20 */
			if ((err = P8HiToP4 ((WORD)SamplesPerRow, lpExp, lpOut)) != 0) {
				goto cu0;
			}
			break;
		case 8:
			memcpy (lpOut, lpExp, (WORD)SamplesPerRow);
			break;
		default:
			err = IM_BUG;
			goto cu0;
		}

cu0:

		return err;
} /* end of UnHorDiff */

/*-----------------------------------------------------------------------*/

/* get (perhaps part of) a row of data, translated into
 * basic uncompressed (type 1) format
 */
short RdBCRow (
	CFile*			pFile,
	LPIMAG			x,
	DC				row,			/* which row to read */
	WORD 			rowstartbyte,	/* read starting at this byte, with respect
								 * to the beginning of the row
								 */
	WORD			rowbytes,		/* number of bytes to read */
	LPSTR			lpBuf)			/* where to put the data */
{
		short			err = SUCCESS;
		short			err2 = SUCCESS;
		WORD		NeededStrip;
		LPDWORD		lpOffsets    = (LPDWORD) NULL;
		LPDWORD		lpByteCounts = (LPDWORD) NULL;
		LPBYTE		lpCmStrip = (LPBYTE) NULL;	/* compressed strip */
		LPBYTE		lpUnStrip = (LPBYTE) NULL;	/* uncompressed strip */
		LPBYTE		lpRowPtr;
		DWORD		dwStripOffset;
		DWORD		dwStripByteCount;
		DWORD		dwStripRows;
		DWORD		dwOutExpected;
		WORD		StripRow;

		/* DBMSG(("RdBCRow: top.  row=%d\n", row)); */

		/* calculate which strip the requested row belongs to
		 */
		NeededStrip = (WORD)row / x->iRowsPerStrip;

		/* if we need to decompress a new strip, do it
		 */
		if (NeededStrip != x->CurStrip) {

			/* DBMSG((" reading strip %u\n",NeededStrip)); */

			/* get the strip offset
			 */
			if (x->eStripOffsets) {
				if (x->tf[X_STRIPOFFSETS].Talloc) {
					if (!(lpOffsets = (LPDWORD)GlobalLock (x->hSTRIPOffsets))) {
						err = MM_LOCK_FAILED;
						goto cu0;
					}
					dwStripOffset = lpOffsets[NeededStrip];
					GlobalUnlock (x->hSTRIPOffsets);
				}
				else {
					lpOffsets = (DWORD  *)&x->tf[X_STRIPOFFSETS].val.Tdword;
					dwStripOffset = lpOffsets[NeededStrip];
				}
			}
			else {
				err = IM_BUG;	/* should have been caught earlier */
				goto cu0;
			}

			/* get the strip byte count
			 */
			if (x->eStripByteCounts) {
				if (x->tf[X_STRIPBYTECOUNTS].Talloc) {
					if (!(lpByteCounts = (DWORD  *)GlobalLock (x->hStripByteCounts))) {
						err = MM_LOCK_FAILED;
						goto cu0;
					}
					dwStripByteCount = lpByteCounts[NeededStrip];
					GlobalUnlock (x->hStripByteCounts);
				}
				else {
					lpByteCounts = (DWORD  *)&x->tf[X_STRIPBYTECOUNTS].val.Tdword;
					dwStripByteCount = lpByteCounts[NeededStrip];
				}
			}
			else {
				err = IM_NO_BYTECOUNTS;
				goto cu0;
			}

			/* lock the compressed-strip buffer
			 */
			if (!(lpCmStrip = (LPBYTE) GlobalLock (x->hCmStrip))) {
				err = MM_LOCK_FAILED;
				goto cu0;
			}

			/* read the compressed data into the compressed-strip buffer
			 */
			if ((err = GtHugeData (pFile, x->iFileType, dwStripOffset, dwStripByteCount, TIFFBYTE, (LPSTR)lpCmStrip)) != 0) {
			 	GlobalUnlock (x->hCmStrip);
			 	goto cu0;
			}

			/* unlock the compressed strip
			 */
			GlobalUnlock (x->hCmStrip);

			/* calculate the number of rows in this strip.  might be useful
			 * for the decompressor function
			 */
			if (NeededStrip == x->nStrips - 1) {
				dwStripRows = (DWORD)x->iImageLength - (DWORD)x->iRowsPerStrip
				 * (DWORD)(x->nStrips - 1);
			} else {
				dwStripRows = (DWORD)x->iRowsPerStrip;
			}

			/* calculate output (uncompressed) bytes expected
			 */
			dwOutExpected = (DWORD)x->BytesPerRow * dwStripRows;
			/* DBMSG((" dwStripRows=%lu\n", dwStripRows)); */

			/* call the strip-decompression routine
			 * 90-04-06: ignore errors from these routines,
			 * so that we get better diagnostic visual output,
			 * and so that we don't fail multiple times per strip,
			 * which can be VERY slow.
			 */
			if (x->iCompression == PACKINTOBYTES) {
				if ((err2 = UnDeStrip (x->hCmStrip, dwStripByteCount, x->hUnStrip)) != 0) {
				}
			} else if (x->iCompression == LZW) {
				if ((err2 = ImLzwDeStrip (x, dwStripByteCount, dwOutExpected, x->hUnStrip)) != 0) {
				}
			} else if (x->iCompression == CCITT1D) {
				if ((err2 = Tiff2DeStrip (x, (WORD)dwStripRows)) != 0) {
				}
			} else if (x->iCompression == TIFFPACKBITS) {
				if ((err2 = PbDeStrip (x->hCmStrip, x->hUnStrip, x->BytesPerRow, (WORD)dwStripRows)) != 0) {
				}
			} else {
				err = IM_BUG;
				goto cu0;
			}

			/* redefine the current strip
			 */
			x->CurStrip = NeededStrip;

			/* 90-05-23  don't ignore errors in the TiffRead program.
			 * the resulting garbage may be confusing for people.
			 */
			if (err2) {
				err = err2;
				goto cu0;
			}

		} /* end of decompress-new-strip */

		/* copy a row to the user's buffer
		 */
		{

			/* calculate which row we want with respect to the
			 * current strip
			 */
			StripRow    = (WORD)row % x->iRowsPerStrip;
			/* DBMSG((" StripRow=%u\n",StripRow)); */

			/* lock the uncompressed (output) strip buffer
			 */
			if (!(lpUnStrip = (LPBYTE) GlobalLock (x->hUnStrip))) {
				err = MM_LOCK_FAILED;
				goto cu0;
			}

			/* calculate the pointer to the row we want
			 */

			lpRowPtr = lpUnStrip + ( (DWORD)StripRow * x->BytesPerRow);

			/* if the horizontal predictor has been used, we'll have to do some
			 * work to undo it.
			 */
			if (x->iPredictor == PREDICTOR_HDIFF) {
				LPBYTE	lpExpRowBuf;
				LPBYTE	lpOutRowBuf;

				/* lock the expanded-row buffer
				 */
				if (!(lpExpRowBuf = (LPBYTE) GlobalLock (x->hUnRow))) {
					err = MM_LOCK_FAILED;
					goto cu1;
				}

				/* lock the output row buffer
				 */
				if (!(lpOutRowBuf = (LPBYTE) GlobalLock (x->hBuf1))) {
					err = MM_LOCK_FAILED;
					goto cu2;
				}

				/* expand to 8 bits, un-difference, and convert back to original
				 * depth
				 */
				if ((err = UnHorDiff (x->iBitsPerSample, x->iSamples, (DWORD)x->iImageWidth, lpRowPtr, lpExpRowBuf, lpOutRowBuf)) != 0) {
					goto cu3;
				}

				/* copy the requested part of the row into the caller's buffer
				 */
				memcpy (lpBuf, lpOutRowBuf+rowstartbyte, rowbytes);

cu3:			/* unlock the output-row buffer
				 */
				if (x->hBuf1) GlobalUnlock (x->hBuf1);

cu2:			/* unlock the expanded-row buffer
				 */
				if (x->hUnRow) GlobalUnlock (x->hUnRow);

			} /* end of horiz predictor section */

			/* otherwise (no horiz predictor) just copy
			 */
			else {
				memcpy (lpBuf, lpRowPtr+rowstartbyte, rowbytes);
			}

			/* clean up and return
			 */

			/* unlock the uncompressed (output) strip buffer
			 */
cu1:		GlobalUnlock (x->hUnStrip);

		} /* end of copy-to-users-buffer section */

		/* cleanup
		 */
cu0:	/* DBMSG(("RdBCRow: bottom\n")); */

		return err;
}	/* RdBCRow */


/* -------------------------------------------------------------------------------- */

/*	This function checks the image structure for bad values. */

short CheckTiff (
	LPIMAG	x)
{
		short		err = SUCCESS;

		x->bStripsOk = true;

		/* ImageWidth,ImageLength
		 */
		if (!x->eImageWidth) {
			err = IM_NO_WIDTH;
			goto cu0;
		}
		if (!x->eImageLength) {
			err = IM_NO_LENGTH;
			goto cu0;
		}
		if (x->iImageWidth == 0) {
			err = IM_BAD_WIDTH;
			goto cu0;
		}
		if (x->iImageLength == 0) {
			err = IM_BAD_LENGTH;
			goto cu0;
		}

		if (!x->eStripOffsets) {
			err = IM_NO_OFFSETS;
			goto cu0;
		}

		/* check bit depth */
		{
			WORD	Bits;

			Bits = x->iBitsPerSample;
			if ( ! (Bits == 1 || Bits == 4 || Bits == 6 || Bits == 8)) {
				err = IM_BAD_BPS;
				goto cu0;
			}
		}

		/* if Samples > 1, replicate BitsPerSample, to keep everything kosher
		 */
		if (x->iSamples != 1 && x->iSamples != 3) {
			err = IM_BAD_SPP;
			goto cu0;
		}

		/* planashortonfiguration.  */
		if (x->ePlanar && x->iSamples > 1 && x->iPlanar != CHUNKY) {
			err = IM_BAD_PLANAR;
			goto cu0;
		}

		/* photometric */
		{
			WORD	Photo;

			Photo = x->iPhotometricInterpretation;
			if (Photo != WHITEZERO && Photo != BLACKZERO && Photo != TIFFRGB
			 && Photo != PALETTECOLOR) {
				err = IM_BAD_PHOTO;
				goto cu0;
			}
		}

		/* Predictor */
		{
			WORD	Pred;

			Pred = x->iPredictor;
			if (Pred == PREDICTOR_NONE) {
			} else if (Pred == PREDICTOR_HDIFF) {
				if (x->iBitsPerSample != 8) {
					err = IM_PRED_MISMATCH;
					goto cu0;
				}
			} else	if ( (! Pred)  &&  (x->iCompression ==  1L) ) {
				x->iPredictor = PREDICTOR_NONE;
				err = kUnCompressMsg;
			}
			else	{
				err = IM_BAD_PREDICT;
				goto cu0;
			}
		}

		if ( err == kUnCompressMsg )	{
				WORD		NumStrips;

			x->iRowsPerStrip = 1;
			NumStrips = x->iImageLength;
			x->tf[X_STRIPOFFSETS].Tlength =
						x->tf[X_STRIPBYTECOUNTS].Tlength = NumStrips;
		}

		/* Check strip size. */
		{
			DWORD	dwBytesPerRow, dwStripBytes;

			dwBytesPerRow = UBPR(x->iImageWidth, x->iBitsPerSample, x->iSamples);
			dwStripBytes = (DWORD)x->iRowsPerStrip * dwBytesPerRow;
			x->BytesPerRow = (WORD)dwBytesPerRow;
		}

		/* fatal error if advertised number of offsets or counts does not
		 * equal the computed number
		 */
		{
			WORD	NumStrips;

			if (x->iRowsPerStrip == 0) {
				err = IM_BAD_ROWSPERSTRIP;
				x->bStripsOk = false;
				goto cu0;
			}

			/*	If the iRowsPerStrip equals the default (0xFFFF), then there is
				only one strip.													*/
			if ( x->iRowsPerStrip == MAXWORD )	{
				x->iRowsPerStrip = x->iImageLength;
				if ( x->iCompression == 1 )
					err = kUnCompressMsg;
			}

			NumStrips = (x->iImageLength + x->iRowsPerStrip - 1) / x->iRowsPerStrip;

			if (x->eStripOffsets) {
				if (NumStrips != (WORD)x->tf[X_STRIPOFFSETS].Tlength) {
					err = IM_BAD_NUM_OFF;
					x->bStripsOk = false;
					goto cu0;
				}
			}
			if (x->eStripByteCounts) {
				if (NumStrips != (WORD)x->tf[X_STRIPBYTECOUNTS].Tlength) {
					err = IM_BAD_NUM_COUNTS;
					x->bStripsOk = false;
					goto cu0;
				}
			}
		}

		/* return
		 */
cu0:

		return err;
} /* CheckTiff */

/*----------------------------------------------------------------------------*/
/* read IFD information into an IMAG structure
 */
short GtIfdInfo (
	CFile*			pFile,
	DWORD			pos,	/* position of the IFD with respect to the beginning of the file */
	WORD			ByteOrder,	/* motorola or intel */
	WORD			Version,	/* 42 */
	LPIMAG	x)		/* this is where the IFD information will be put */
{
		short 			err = SUCCESS;
		DIRENTRY	de;
		TFIELD		tf;
		WORD		IfdEntries;
		DWORD		EntryOffset;
		WORD		ii;

		/* DBMSG(("GtIfdInfo: top\n")); */

		/* initialize the structure
		 */
		InitImag (x);

		/* add byte order and version
		 */
		x->iFileType = ByteOrder;
		x->iVersion = Version;

		/* read the number of directory entries
		 */
		if ((err = GtData (pFile, ByteOrder, pos, 1, TIFFSHORT, (LPSTR)&IfdEntries)) != 0) {
			goto cu1;
		}

		/* loop through the entries
		 */
		EntryOffset = pos + sizeof(IfdEntries);
		for (ii = 0; ii < IfdEntries; ii++, EntryOffset += sizeof(de)) {

			/* read the entry.
			 * don't choke if there is a problem with a particular field.
			 */
			if ((err = GtTiffEntry (pFile, ByteOrder, EntryOffset, &de)) != 0) {
				err = SUCCESS;
				continue;
			}

			/* convert to a TFIELD structure, reading big fields as necessary
			 */
			if ((err = FillTField (pFile, ByteOrder, &de, EntryOffset, &tf)) != 0) {
				err = SUCCESS;
				continue;
			}

			/* check for bad values, convert from one format to another
			 * if necessary, and store the information in the appropriate
			 * TFIELD structure in the IMAG structure
			 */
			if ((err = NicefyTField ((LPTFIELD)&tf, x)) != 0) {
				err = SUCCESS;
				continue;
			}

		} /* end of direntry loop */

		/* check the offset of the next IFD
		 */
		{
			DWORD	NextIFD;
			WORD	Dummy;

			/* try to read the offset
			 */
			if ((err = GtData (pFile, ByteOrder, EntryOffset, 1, TIFFLONG, (LPSTR)&NextIFD)) != 0) {
				/* ImWarn (IM_BAD_NEXT_IFD); */
				err = SUCCESS;	/* not fatal, for now */
			}

			/* try to read the field count of the next IFD. if error, we know
			 * that the TIFF writer forgot to write a next-IFD-location value
			 * of 0 after the last-valid IFD.  If no error, the file still might
			 * be bad -- we won't know until we try to parse the next IFD, which
			 * we're not going to bother with until this routine wants to care about
			 * TIFF files with multiple IFDs.
			 */
			else {
				if ((err = GtData (pFile, ByteOrder, NextIFD, 1, TIFFSHORT, (LPSTR)&Dummy)) != 0) {
					/* ImWarn(x->hWarnings, IM_BAD_NEXT_IFD); */
					err = SUCCESS;	/* not fatal, for now */
				}
			}
		}

		/* miscellaneous correctness checks
		 *
		 * also computes some useful values such as x->BytesPerRow and
		 * x->nStrips
		 */
		if ((err = CheckTiff (x)) != 0) {
			goto cu1;
		}

		/* call the compression open routine
		 */
		{
			WORD	Compr;
			BOOL			bPreferredCompression;

			Compr = x->iCompression;
			bPreferredCompression = (Compr == PACKINTOBYTES || Compr == CCITT1D
			 || Compr == TIFFPACKBITS || Compr == LZW);

			/* is it nice?
			 */
			if (bPreferredCompression) {
				if (x->bStripsOk) {
					err = OpenBC (pFile, x);
					goto cu1;
				} else {
					err = IM_BAD_COMPR;
					goto cu1;
				}
			}
			else {
				err = IM_BAD_COMPR;
				goto cu1;
			}
		} /* end of compression-opening section */

		/* clean up and return
		 */
cu1:	;
//cu0:
		return err;
}

/* -------------------------------------------------------------------------------- */

/* 	This function first checks if the color table has white as the first entry and
	black as the last entry as per the IM guidelines.
	If not then both the color table and the image is inverted.
*/
/*
bool fixInvertedData (
	Handle			aMapHdl,
    Ptr     		totalBytes)
{
	CTabHandle			colorTabHdl = (*aMapHdl)->pmTable,
						tmpColorTabHdl;
	Ptr					ptr;
	short				idx, numEntries;
	RGBColor			black, white;
	short				err;

	SETRGB (&black, 0x0000, 0x0000, 0x0000);
	SETRGB (&white, 0xFFFF, 0xFFFF, 0xFFFF);
	numEntries = (*colorTabHdl)->ctSize ;

	if ( byteCmp((Ptr)(&(*colorTabHdl)->ctTable[0].rgb), (Ptr)&qd.black, sizeof(RGBColor)) &&
			byteCmp((Ptr)(&(*colorTabHdl)->ctTable[numEntries].rgb), (Ptr)&qd.white, sizeof(RGBColor)))
		return false;

	// Save a temporary copy of the color table. 
	tmpColorTabHdl = colorTabHdl;
	if (( err = HandToHand ((Handle *)&tmpColorTabHdl) ) != 0)
		return false;

	// Invert the color table. 
	(*colorTabHdl)->ctTable[0].rgb = white;
	(*colorTabHdl)->ctTable[numEntries].rgb = black;
	for (idx = 1; idx < numEntries; idx++)
		(*colorTabHdl)->ctTable[idx].rgb =
							(*tmpColorTabHdl)->ctTable[numEntries - idx].rgb;
	DisposeHandle ((Handle)tmpColorTabHdl);

	// Invert the image data, notice that this works at all depths. 
	ptr = (*aMapHdl)->baseAddr;
	totalBytes = ptr + (long)totalBytes;
	while (ptr < totalBytes)	{
		*ptr = ~(*ptr);
		ptr++;
	}
	return true;
}	// fixInvertedData 
*/
/* -------------------------------------------------------------------------------- */

/*  This function creates greyscale "color" table according to bits per sample. */
/*
CTabHandle createGreyTabHdl (short bitsPerSamp)
{
		CTabHandle		grayTabHdl;
		Size			byteCnt;
		short			idx, numEntries, gray = 0, grayOffset;

	numEntries = 1 << bitsPerSamp;
	grayOffset = (65536 / (numEntries - 1));

	byteCnt = sizeof (ColorTable) + sizeof (ColorSpec) * (numEntries -1);
	if ( ! (grayTabHdl = (CTabHandle)GlobalAlloc (byteCnt)) )
		return 0L;
	(*grayTabHdl)->ctSize = numEntries - 1;

	if ( ! gPhotoMetr )	{
		gray = 65535;
		grayOffset = - grayOffset;
	}

	for (idx = 0; idx < numEntries; idx++, gray += grayOffset)	{
		(*grayTabHdl)->ctTable[idx].value = idx;
		(*grayTabHdl)->ctTable[idx].rgb.red =
				(*grayTabHdl)->ctTable[idx].rgb.green =
						(*grayTabHdl)->ctTable[idx].rgb.blue = gray;
	}

	return (grayTabHdl);
}	// createGreyTabHdl 
*/

/* -------------------------------------------------------------------------------- */

/* get tag string
 */

void GtTagString (
	WORD	tag,
	char	**ps)
{
		int tablen;
		int ii;

		tablen = sizeof (tagstr) / sizeof (tagstr[0]);
		for (ii = 0; ii < tablen; ii++) {
			if (tag == tagstr[ii].tag) {
				*ps = tagstr[ii].str;
				return;
			}
		}
		*ps = defstr;
}	/* GtTagString */

/* -------------------------------------------------------------------------------- */

/* dump an entry
 */

short dumpentry (
	CFile*		pFile,
	WORD		ByteOrder,	/* INTELTIFF vs MOTOROLATIFF */
	DWORD		pos,
	LPDIRENTRY	lpde)
{
		short			err;
		WORD		tsize;
		WORD		BytesToRead;
		char		*bufptr;
		union {
			char	bytes[MAXVAL];
			DWORD	dword;
		} buf;
		WORD		maxitems;
		char		*s;
		DWORD		valpos;
		DWORD		ValBuf;
		double		resRatio;

		/* get the non byte reversed value
		 */
		if ((err = VRead (pFile, pos + 8L, sizeof(ValBuf), (LPSTR)&ValBuf)) != 0) {
			 return err;
		}

		/* dump the basic entry
		 */
		GtTagString (lpde->deTag, &s);

		/* print out the value intelligently
		 */
		if ((err = GtTiffSizeof (lpde->deType, &tsize)) != 0) {
			return err;
		}
		BytesToRead = tsize * lpde->deLength;
		maxitems = MAXVAL / tsize;
		maxitems = (lpde->deLength < (DWORD) maxitems) ?
		 (WORD)(lpde->deLength) : maxitems;
		/* careful here: we can't just use deVal to grab data out of, since
		 * may already have been byte-reversed!
		 */
		if (BytesToRead <= 4)
			valpos = pos + 8L;	/* deVal starts on byte 8, wit de */
		else
			valpos = lpde->deVal;
		if ((err = GtData (pFile, ByteOrder, valpos, maxitems, lpde->deType, buf.bytes)) != 0) {
			return err;
		}

		bufptr = buf.bytes;

		switch (lpde->deTag)	{

			case TGXRESOLUTION:
				resRatio = *(LPDWORD)bufptr;
				bufptr += 4;
				resRatio /= *(LPDWORD)bufptr;
				gHRes = mtRealToFix (resRatio);
				break;

			case TGYRESOLUTION:
				resRatio = *(LPDWORD)bufptr;
				bufptr += 4;
				resRatio /= *(LPDWORD)bufptr;
				gVRes = mtRealToFix (resRatio);
				break;

			case TGPHOTOMETRICINTERPRETATION:
				gPhotoMetr = *(short *)bufptr;
				break;

			case TGCOLORMAP:
				{
//						Size	byteCnt;
						short	*rPtr, *gPtr, *bPtr;
						short	idx, numEntries;

					isColorMap = true;

					numEntries = lpde->deLength / 3;

					swapb((unsigned short*)bufptr, (unsigned short*)bufptr,lpde->deLength*2);
					//swapw(bufptr, bufptr,lpde->deLength);

					rPtr = (short *)bufptr;
					gPtr = rPtr + numEntries;
					bPtr = gPtr + numEntries;

					bmpi.bmiHeader.biClrUsed = 0;//numEntries - 1;

					for (idx = 0; idx < numEntries; idx++, rPtr++, gPtr++, bPtr++) {
						bmpi.bmiColors[idx].rgbRed   = *rPtr;
						bmpi.bmiColors[idx].rgbGreen = *gPtr;
						bmpi.bmiColors[idx].rgbBlue  = *bPtr;
						bmpi.bmiColors[idx].rgbReserved = 0;
					}
				}
				break;

		}

		return SUCCESS;
}	/* dumpentry */


/* -------------------------------------------------------------------------------- */

/* process sibling ifd's
 */
short DoSibs (
	CFile*			pFile,
	WORD			ByteOrder,
	WORD			Version,
	LPDWORD			pLocation,
	HBITMAP&		hBitmap, 
	int&			width, 
	int&			height)	/* IN/OUT */
{
	short					err = SUCCESS;
	DWORD					location = *pLocation;
	DWORD					IfdStart = *pLocation;
  	DIRENTRY				de;
 	WORD					entries;
	WORD					entry;
	DWORD					dwtemp;
 	HANDLE					hRowBuf;
 	LPBYTE					lpRowBuf;
// 	double					resFactor;
	long					bufsize;

	Handle					aMapHdl;
	bool					unCompressed = false;
	bool					lastImagIsGood = false;

	aMapHdl = 0L;
	char* gptr; 
	IMAG	imag;
	IMAG	*p = &imag;

	isColorMap = false;
	
	memset(&bmpi.bmiColors,0,4*256);
	
	do {
 		/* if ifd location is 0, quit */
 		if (location == 0L)	break;

 		/* read the number of entries, and dump it */
 		if ((err = GtData (pFile, ByteOrder, location, 1, TIFFSHORT, (LPSTR)&entries)) != 0)
 			break;

 		if (entries == 0)	break;

 		location += 2;

		gPhotoMetr = 0;

		/* loop through the entries  */
 		for (entry = 0; entry < entries; entry++) {
 			/* read the entry  */
 			if ((err = GtTiffEntry (pFile, ByteOrder, location, (LPDIRENTRY)&de)) != 0)
 				goto cu0;
			if ((err = dumpentry (pFile, ByteOrder, location, &de)) != 0)
				goto cu0;
 			/* adjust the current location */
 			location += sizeof (DIRENTRY);
		} /* end of entry loop */

 		/* read the location of the next ifd */
 		if ((err = GtData(pFile, ByteOrder, (DWORD)location, 1, TIFFLONG, (LPSTR)&dwtemp)) != 0)
 			goto cu0;

 		location = dwtemp;

	 	/* process children, and check for errors.
	 	 * We go through some of the same work again, since
	 	 * this part of the program was an afterthought.
	 	 */
		long	totalBytes;

		/* read the header, and set up for reading data  */

 		if ((err = GtIfdInfo (pFile, IfdStart, ByteOrder, Version, p)) != 0)
 			if (err == kUnCompressMsg)
 				unCompressed = true;
 			else
 				goto b0;

			/* Disallow L-compressed gray files on B&W machines. */
/*			if ( ! ( gConfig.hasColorToolbox  ||  gColorTabHdl  ||
						(p->iBitsPerSample == 1)  ||  gPhotoMetr == 2  ||
												p->iCompression != LZW) )	{
				err = IM_LGrayFile;
	 			goto b0;
			}
*/
			height = p->iImageLength;
			width = p->iImageWidth;
		    bufsize = height*width;

			bmpi.bmiHeader.biSize = 40;
			bmpi.bmiHeader.biBitCount = p->iBitsPerSample * p->iSamples;
			bmpi.bmiHeader.biHeight = height;
			bmpi.bmiHeader.biWidth = width;
			bmpi.bmiHeader.biPlanes = 1;

			bmpi.bmiHeader.biCompression = 0;
			bmpi.bmiHeader.biSizeImage = 0;
			bmpi.bmiHeader.biXPelsPerMeter = 0;
			bmpi.bmiHeader.biYPelsPerMeter = 0;
			bmpi.bmiHeader.biClrUsed = 0;
			bmpi.bmiHeader.biClrImportant = 0;

			if (gPhotoMetr != 2 && !isColorMap) {

				short grNumEntries = 1 << p->iBitsPerSample;
				short grayOffset = (65536 / (grNumEntries - 1)), idx, gray = 0;

				if ( ! gPhotoMetr )	{
					gray = 65535;
					grayOffset = - grayOffset;
				}

				for (idx = 0; idx < grNumEntries; idx++, gray += grayOffset)	{
					bmpi.bmiColors[idx].rgbBlue = bmpi.bmiColors[idx].rgbRed = bmpi.bmiColors[idx].rgbGreen = gray;
					bmpi.bmiColors[idx].rgbReserved = 0;
				}

				isGrayScale = true;


			}
						
			hBitmap=CreateDIBSection(NULL,(LPBITMAPINFO)&bmpi,DIB_RGB_COLORS,(void**)&gptr,NULL,0);

	 		/* dump the first few rows of data */
	 		{
	 				WORD				rownum;
	 				WORD				BytesPerRow;
//					Ptr					ptr;
					short				idx;
					long				filePos;
					long				blankSize = 0L;
					WORD				evenRowBytes = 0;

				totalBytes = p->iImageLength;

				//memset(&aPixmap,sizeof(PixMap),0);

	 			/* calculate number of bytes per row  */
	 			BytesPerRow = UBPR(p->iImageWidth,p->iBitsPerSample,p->iSamples);

				
				if ( BytesPerRow % 4)
	 				evenRowBytes = 4 - BytesPerRow % 4 + BytesPerRow;
	 			else
	 				evenRowBytes = BytesPerRow;


				totalBytes *= evenRowBytes;

				if (gPhotoMetr == 2)
					blankSize = totalBytes / 3;

	 			/* allocate space for a row buffer  */
	 			if ((err = GetItLockIt ((DWORD)BytesPerRow, (LPHANDLE)&hRowBuf, (LPBYTE *)&lpRowBuf)) != 0)
	 				goto b1;

				if (unCompressed)
					filePos = p->tf[X_STRIPOFFSETS].val.Tdword;

	 			/* read and dump each row */
		 		for (rownum = 0; rownum < p->iImageLength; rownum++) {

		 			//stillWaiting();
		 			/* read and uncompress the data */
					if (unCompressed)	{
						if (( err = VRead (pFile, filePos, BytesPerRow, (LPSTR)lpRowBuf) ) != 0)
			 				goto b2;

						filePos += BytesPerRow;
					}
			 		else if ((err = RdBCRow (pFile, p, (DC)rownum, 0, evenRowBytes/*BytesPerRow*/, (LPSTR)lpRowBuf)) != 0)
			 			goto b2;

					if (gPhotoMetr == 2 && !isGrayScale)	{
						for (idx = 0; idx < BytesPerRow; idx += 3)	{
							*(((unsigned char*)gptr) + idx + (height - rownum - 1) * evenRowBytes + (isColorMap?2:0)) = *(lpRowBuf + idx + 2);
							*(((unsigned char*)gptr) + idx + (height - rownum - 1) * evenRowBytes + 1) = *(lpRowBuf + idx + 1);
							*(((unsigned char*)gptr) + idx + (height - rownum - 1) * evenRowBytes + (isColorMap?0:2)) = *(lpRowBuf + idx);
						}
					}
					else
					    memcpy((unsigned char*)gptr + (height - rownum - 1) * evenRowBytes, lpRowBuf, BytesPerRow);
			 	}
			 }

			lastImagIsGood = true;

			gPhotoMetr = 0;


b2:			UnlockItFreeIt (hRowBuf);
b1:			CloseImag(p);
b0:			;
//	 	}

 	} while (1); /* end of sibling ifd loop */
//	stillWaiting();
cu0:
	*pLocation = location;

	if ( lastImagIsGood )
		err = 0;

	return err;
}	/* DoSibs */


/* get TIFF 8-byte header
 * currently only probably portable.  depends somewhat on compiler's
 * structure organization.
 */
short GtTiffHdr (CFile* pFile, LPTIFFHDR lpHdr)
{
		short err;

		/* get the first word -- the byte order.
		 * first, set dlOrder to either valid value, since we will immediately
		 * change it.  sort of a chicken and egg situation.
		 */
		/* pDloc->dlOrder = INTELTIFF; */
		if ((err = GtData (pFile, INTELTIFF, (DWORD) 0, 1, TIFFSHORT, (LPSTR)&lpHdr->thByteOrder)) != 0) {
			return err;
		}
		/* *pOrder = lpHdr->thByteOrder; */

		/* get the version
		 */
		if ((err = GtData (pFile, lpHdr->thByteOrder, (DWORD) 2, 1, TIFFSHORT, (LPSTR)&lpHdr->thVersion)) != 0) {
			return err;
		}

		/* get the double word (IFD offset)
		 */
		if ((err = GtData (pFile, lpHdr->thByteOrder, (DWORD)4, 1, TIFFLONG, (LPSTR)&lpHdr->thIfdOffset)) != 0) {
			return err;
		}

		/* return
		 */
		return SUCCESS;
}

/*-----------------------------------------------------------------------*/

bool readTiff (const char* pFileName,  HBITMAP& hBitmap, int& width, int& height)
{

	CFile file;
	TIFFHDR th;
	DWORD location;
	CFileException e;
	short err;
	if (!file.Open(pFileName,CFile::modeRead, &e))
		return false;

	isGrayScale = false;

	/* read the 8-byte header, and dump it */
	if ((err = GtTiffHdr (&file, &th)) == 0) {
		if (th.thByteOrder == INTELTIFF  ||  th.thByteOrder == MOTOROLATIFF)  {
			location = th.thIfdOffset;
			/* do sibling ifd's	*/
			if ((err = DoSibs (&file, th.thByteOrder, th.thVersion, (LPDWORD)&location,  hBitmap, width, height )) == 0) {
				return true;
			}
		}
	}
	return false;
}	/* readTiff */

/* -------------------------------------------------------------------------------- */

short UnpackBits (LPSTR &lpSshort, LPSTR &lpDst, short BytesPerRow)
{
	long totalLength=0;
	long length;

	while (totalLength<BytesPerRow) {
		length = *((signed char*)lpSshort);
		lpSshort++;
		if (length & 0x80) {
			if (length!=0x80) {
			  length = ((~length) & 0xFF) + 2;
			  while ( (length-- ) && totalLength++<BytesPerRow)
			    	*(lpDst++) = *lpSshort;
			}
			lpSshort++;
		}
		else {
			length = (length & 0xFF) + 1;
			while ( length-- && totalLength++<BytesPerRow)
				*(lpDst++) = *(lpSshort++);
		}
		
	}

	return 0;
}
