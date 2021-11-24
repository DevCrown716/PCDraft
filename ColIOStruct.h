
#if !defined(AFX_COLIOSTRUCT_H_INCLUDED_)
#define AFX_COLIOSTRUCT_H_INCLUDED_

#pragma pack(1)

#define kColMDCreator	  	'TFDM'	/* MacDraft Creator type */
#define kColWin        		' NIW'
#define kColMac     		' CAM'
#define kColDocType        	'CDDM'
#define kColSYMBType		'MYSM'

typedef unsigned short Bool;
typedef unsigned long Size;
typedef unsigned char Byte;

struct ColRGBTriple
{
	Byte rgbtBlue;
	Byte rgbtGreen;
	Byte rgbtRed;
};

struct ColPt 
{
	short x,y;
};

struct ColRect 
{
	short left;
	short top;
	short right;
	short bottom;
};

struct ColLongRatio
{
	ulong fN,fD;
};

struct ColSnap
{
	ColLongRatio fSnapRatio;
	LongPoint fSnapResidue;
};

/* possible values for PenModel.fPenType */

enum {
	kColPenInvis = 0,
	kColPenColor,      /* rgb color */
	kColPenGray,
	kColPenPat        /* b&w or fullColor pattern */
};

/*
   On a b&w machine,
  invisible pen is fPenType = kPenInvis, fColorIndex = kNoneIndex
  white pen   fPenType = kPenColor, fColorIndex = kWhiteIndex
  black pen   fPenType = kPenColor, fColorIndex = kBlackIndex
*/

struct ColPenModel
{
	Coord fPenW;
	ushort fUnits       : 8;
	ushort fAlignment   : 2;
	ushort fPenType     : 2;
	ushort fEndCap1     : 4;
	ushort fEndCap2     : 4;
	ushort fDashIndex   : 4;
	ushort fHairline    : 1;
	ushort fHairlineCnt : 3;
	ushort fSpare       : 4;
	ushort fPenIndex;
	ushort fDimTextAlgn :2;     /* Dim Text alignment (above, along, uni) */
};

/* possible values for FillModel.fFillType */
enum {
	kColNoFill  = 0,
	kColFillColor,
	kColFillPat,
	kColFillGray
};

struct ColFillModel
{
	ushort fFillType :2;
	ushort fFillIndex:14;
};

struct ColDocUnits
{
	short fDefUnits;
	short fDefPlaces;
	LongPoint fDefResolution;
	short fDefScaleIndex;
	short fDefSnapIndex;
	Bool fRotDisp;  /* Rotation Display                  */
					   /* 0 is for Decimal Degrees, and     */
					   /* 1 is for Degrees Min & Secs.      */
	short fDefRotPlaces;
	Bool fHidUnits; /* Only for Metric Unit, false no hidden */
};

struct ColLayerIOInfo
{
  	short fNumLayers;
  	long fLayerOff;
};

struct ColActiveEnvirons
{
	short fToolID;       /* for fast access */
	ColFillModel fFillModel;    /* Current fill model */
	ColPenModel fPenModel;     /* Current pen model */
	short fCase;
	short fSpace;
	short fJust;
	Bool fHideText;
	Bool fShowRulers;
	LongPoint fRulerOrigin;
	Bool fStandardRulers;
	Bool fHidePageBreaks;
	Bool fHideGrid;
	Bool fSnapToObject;
	ushort fPrintFlags;
	Bool fDimVisible;
	Bool fDimLineFlag;   /* Dim Line attribute set in menu*/
	short fDimTextAlgn;   /* Dim Text alignment (above, along, uni) */
	ColPt fPrevZoomScroll;
	short fPrevZoom;      /* Previous zoom factor */
	short fZoom;          /* Current zoom factor */
	short fMaxZoom;       /* Maximum zoom factor */
	short fLineWt;        /* Current line weight */
	ushort fhBrFillInfo;	/* Change to ushort because PC handle is 2 bytes */
	ushort fPenIndex;
	short fSpare       : 8;
	short fAlignment   : 8;
	short fDocEvent    : 8;
	short fDocState    : 8;
	ColPt fLogWinOrigin;
	ColPt fViewOrigin;
	LongPoint fZRes;
	ColPt fCursorPoint;
	ColSnap fCurSnap;
};

struct ColIODoc
{
	ulong fDocType;
	ulong fCreator;
	ulong fPlatform;
	ushort fOSVers;
	ushort fApplVers;
	char fName[64];
	LongRect fDefPageSize;
	LongRect fDocSize;
	long fPrintInfoOff;
	Size fDocBytes;
	ColRect fWinPos;
	ushort fUpdateDeltas;
	LongPoint fDupDeltas;
	ColDocUnits fDocUnits;
	ColLayerIOInfo fLayerInfo;
	short fNumColors;
	short fNumPats;
	long fScaleTableOff;
	long fDashTableOff;
	long fBitmapListOff;
	ColActiveEnvirons fActiveEnvirons;
	long fViewsOff;
	long fFontInfoOff;
	long fMissFontOff;
	ColPt fViewPages;
	long fColorPalOff;
	long fPatListOff;
	ColPt fAspectRatio;
};

#define	kColMaxLayerNameLen	16	/* max name length for layer */
#define kColActive           1	/* .... .... .... ...1 - bit 1 */
#define kColVisible          2	/* .... .... .... ..1. - bit 2 */
#define kColSelectable       4	/* .... .... .... .1.. - bit 3 */
#define kColScaled           8	/* .... .... .... 1... - bit 4 */
#define kColNotGrayed        16	/* .... .... ...1 .... - bit 5 */
#define kColFillsOn          32	/* .... .... ..1. .... - bit 6 */
#define kColDeleteThisLayer  64	/* .... .... .1.. .... - bit 7 */

struct ColLayerIO
{
	char fLayerName[kColMaxLayerNameLen];
	ushort fLayerFlags;
	short fScaleIndex;
	short fSnapIndex;
	long fNumShapes;
	long fShapesOff;
	long fFirstShape;
	long fLastShape;
};

struct ColIOShapeHeader
{
	ulong fBlkSize;
	ulong fSpare;
	long fNextAvail;
	long fHoleBytes;
	ushort fHoleListHdl;
};

/* Shape ID's (fID) */

#define  ColIDLine         0x81  /* 0x01 | 0x80  NoFill type. */

#define  ColIDGroup        0x82  /* 0x02 | 0x80  NoFill type. */
#define  ColIDInvalGroup   0x83  /* 0x03 | 0x80  NoFill type. */

#define  ColIDFreehand     0x43  /* 0x03 | 0x40  Uniform points type. */
#define  ColIDPolygon      0x44  /* 0x04 | 0x40  Uniform points type. */

#define  ColIDFreeSpline   0x45  /* 0x05 | 0x40  Uniform points type. */
#define  ColIDPolySpline   0x46  /* 0x06 | 0x40  Uniform points type. */

#define  ColIDBezier       0x47  /* 0x07 | 0x40  Uniform points type. */
#define  ColIDCubic        0x48  /* 0x08 | 0x40  Uniform points type. */

#define  ColIDCircle       0x01
#define  ColIDEllipse      0x02
#define  ColIDArc          0x03
#define  ColIDEArc         0x04
#define  ColID3Arc         0x05
#define  ColIDRectangle    0x06
#define  ColIDRndRectangle 0x07
#define  ColIDText         0x08
#define  ColIDBitmap       0x09

#define  ColIDDimString    0x21  /* 0x01 | 0x20   Dimension type. */

#define  ColIDDimPt2Pt     0xA2  /* 0x02 | 0x20| 0x80  */
#define  ColIDArea         0xA4  /* 0x03 | 0x20| 0x80  */

#define  kNoFillMask    0x80  /* No fill model, mask.          */
#define  kUniPointMask  0x40  /* All points are uniform, mask  */
#define  kDimShapeMask  0x20  /* Dimension object, mask.       */

enum  ColMethod
{
	kColCenterRadius, kColDiameter, kCol3Point, kColEllipse
};

enum ColCorner
{
	kColProportionalCorner=1, kColConstantCorner, kColEllipticalCorner
};

/* kDefaultDatum means datum is anchored to a shape point. */

enum ColDatumCode
{
	kColDefaultDatum, kColCenterDatum, kColFreeDatum
};

enum  ColDimText
{
	kColAlongLine = 1, kColNextToLine, kColHorToLine
};

struct ColDSShape
{
	long fNext;					/* Next shape in BtoF order  */             \
	long fPrev;					/* Prev. shape in BtoF order  */            \
	long fOwnerGroup;			/* 0L if not in a group     */              \
	long fDepShapeOff;			/* First dependent shape offset    */       \
	ushort fShapeSize;			/* size of shape in bytes */                \
	LongRect fExtent;			/* Unzoomed long extent     */              \
	Angle fRot;					/* Rotation      */
	ColPenModel fPenModel;		/* How to draw it       */
	ushort fID            :8;	/* Shape ID   */
	ushort fConstrained   :1;
	ushort fDatumType     :3;
	ushort fLocked        :1;
	ushort fClosed        :1;
	ushort fDeleted       :1;
	ushort fSelected      :1;
	LongPoint fDatum;			/* Datum point       */
};

struct ColLine : public ColDSShape
{
	LongPoint fPts[2];
};

struct ColFillShape : public ColDSShape
{
	ColFillModel fFillModel;
};

struct ColDSRect : public ColFillShape
{
	LongPoint fPts[4];
};

struct ColDSRndRect : public ColFillShape
{
	short fCornerType;
	LongPoint fDiagonals;
	LongPoint fPts[4];
};

struct ColDSCircle : public ColFillShape
{
	short fMethod;
	WrFixed fRadius;
	LongPoint fPts[4];
};

struct ColDSArc : public ColFillShape
{
	short fMethod;
	WrFixed fRadius,fVRadius;
	LongPoint fPts[4];
};

struct ColDSEllipse : public ColFillShape
{
	short fMethod;
	WrFixed fHRadius,fVRadius;
	LongPoint fPts[1];
};

/*
  For Polygon and Freehand :
    fCount is the count of total number of points.
    For closed shapes the last point is the first point (No duplicates).

  For Bezier and Cubic spline:
    fCount is the count of total number of points.
    For closed curves the last point is a duplicate of the first point.
    Num. of Segs. =  ( fCount -1 ) /3

  For Smoothed Shapes ( FreeSpline and PolySpline ) :
    fCount is the number of bezier points as above.
    After the bezier points (fCount points) there are n polygon points.
    if shape is closed
      n = fCount / 3;
    if shape is open FreeSpline
      n = fCount / 3 + 1;
    if shape is open PolySpline
      n = fCount / 3 + 2;

    For closed curves the last point is a duplicate of the first point.
    Num. of Segs. =  ( fCount -1 ) /3
*/

struct ColDSInvarShape : public ColFillShape
{
	short fCount;
	LongPoint fPts[1];
};

/* Text Structures */

#define kColSingleSpace	0
#define kCol1_1_2Space	1
#define kColDblSpace	2

#define kColLeftJus		0
#define kColCenterJus	1
#define kColRightJus	2

#define kColUppercase	0
#define kColLowercase	1
#define kColTitleText	2

struct ColTextInfo
{
  short fTxFont;
  short fTxFace;
  short fTxMode;
  short fTxSize;
};

struct IOLogFont
{
	ushort fFontRot  : 12;
	ushort fFontType : 4;
	short lfWidth;
	short lfEscapement;
	short lfOrientation;
	uchar lfCharSet;
	uchar lfOutPrecision;
	uchar lfClipPrecision;
	uchar lfQuality;
	uchar lfPitchAndFamily;
//	uchar lfDummy;
	char lfFaceName[LF_FACESIZE];
};

struct IOFontInfo
{
	short fNumFonts;
	short fCurFontIdx;
	short fZmFontSize;
	short fCurFontSize;
	short fCurFontStyle;
	short fShRot;
	ushort fCurFontHdl;
	IOLogFont fIddFont[1];
};

/*
   The STYLE RUN ELEMENT structure. The list of style runs is attached
   to the DSText structure.
*/
struct ColSRElem
{
	short fStartChar;	/* Starting character position.              */
	short fNumOfChars;	/* Number of chars in this run.              */
	short fStyleIdx;	/* Index to the style table of the document. */
	short fSize;		/* Font size.                                */
	short fStyle;		/* Font style.                               */
	ushort fPenIndex;	/* Pen Color index.                          */
};

/*
   The LINE ELEMENT structure. The list of line element is also attached
   to the DSText structure.
*/
struct ColLElem
{
	short fStartChar;	/* Starting character position.        */
	short fNumOfChars;	/* Number of characters in the line.   */
	short fLineAsc;		/* Ascent of biggest font on the line. */
	short fLineHt;		/* Maximum height in line =            */
						/* Ascent + Descent + ExternalLeading  */
	short fLineWidth;	/* Width of line.                      */
};

struct ColDSText  : public ColFillShape
{
	ColPt fStPt;		/* Initial base point for text object */
	Bool fParagraph;	/* paragraph/caption flag */
	Bool fObjOpen;		/* flag for text open/closed */
	short fZoomFactor;	/* text's zoom factor */
	short fTextSpace;	/* text's line spacing */
	short fTextJust;	/* text's justification */
	LongPoint fPts[4];	/* non-zoomed four corner coordinates */
	ColRect fViewRect;	/* zoomed short view rect */
	short fShRot;
	short fSelStart;
	short fSelEnd;
	short fNumStyles;		/* number of style runs */
	short fStyleRunsSize;	/* current size of fStyleRuns dynamic array */
	short fNumLines;		/* number of lines */
	short fNumLinesSize;	/* current size of fLHElem dynamic array */
	short fTextLen;			/* number of chars */
	short fTextSize;		/* current size of fText dynamic array */
//	uchar fSRElem[1];		/* dynamic array of style runs */
//	uchar fLElem[1];		/* dynamic array of line elements */
//	uchar fText[1];			/* dynamic array of chars */
};

struct ColBitmapIO
{
	short fShareCount;
	ulong fSize;
	ushort fNumColors;       /* Non-zero for Color, zero otherwise */
	long fBmpInfoOff;
};

struct ColBitmapRec
{
	short fWidth;
	short fHeight;
	uchar fPlanes;
	uchar fBitCount;
	uchar fBits[1];   /* Dynamic Array of Bits */
};

struct ColDSBitmap  : public ColFillShape
{
	short fMapHdlIndex;	/* index into the document bitmap list FList */
	ColRect fSrcRect;
	ColRect fBounds;
};

struct ColDSGroup : public ColDSShape
{
	LongPoint fPts[4];
	short fNumMembers;	/* to save calc */
	long fMemberOff[1];	/* dynamic array */
};

/*------------------------- annotation data structures -------------------*/

#define kColDimStrSize    24

/*
  1 All Annotation objects contain DimStatus as their first field
  2 If a double precision value is also required the 2nd field is DimValue
  3 The remaining fields are unique to that dim objects
*/

struct ColDSDimStr : public ColFillShape
{
	ushort fStatus : 1;
	ushort fPtToPt : 1;
	ushort fMoved  : 1;
	ushort fSpare  : 13;
	short fFont;
	short fFontSize;
	LongPoint fPts[4];				/* non-zoomed four corner coordinates */
	LongPoint fAnchorPt;			/* midpt of dimension line */
	LongPoint fStrOffset;			/* dimStr offsets */
	uchar fString[kColDimStrSize];	/* the string */
	/*short*/ double fValue;		/* short double takes 64 bits on MAC */
};

#define kColMaxViewName  30
struct ColSavedView
{
  char      fViewName[kColMaxViewName];
  char      fCmdKey;
  char      fFutureUse;
  ColPt     fOldScroll;
  short     fZoomFactor;
  short     fMenuID;
};

#pragma pack()

#endif
