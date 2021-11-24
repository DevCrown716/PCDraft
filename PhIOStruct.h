
#if !defined(AFX_PHIOSTRUCT_H_INCLUDED_)
#define AFX_PHIOSTRUCT_H_INCLUDED_

#pragma pack(1)

#define kPhDocType		'MDDC'
#define kPhPadType		'MPAD'
#define kPhSYMBType		'MSYM'

#define kPhFileHdr		'20'
#define kPhRelease4_0_0	6

typedef char Boolean;
typedef unsigned long Size;

struct PhPoint
{
	short x,y;
};

struct PhRect
{
	short l,t,r,b;
};

typedef short PhUnit;
typedef short PhAngleUnit;

struct PhDocUnits
{
	PhUnit fDefUnits;
	short fDefPlaces;
	PhAngleUnit fDefAngleUnits;
//	char fDummy;
	short fDefAnglePlaces;
	PhPoint fDefResolution;
	short fDefScaleIndex;
	short fDefSnapIndex;
};

struct PhDimStd
{
	short dsType;						/* ANSI, Metric */
	WrFixed dsWitnessExt;
	WrFixed dsWitnessLineGap;
	WrFixed dsDimTextGap;
	WrFixed dsLeaderLen;
	WrFixed dsOutsideLineLen;
	long dsToleranceTextScale;
	long dsToleranceSpaceScale;
	WrFixed dsCircleCenterLen;
	WrFixed dsCircleCenterGap;
	WrFixed dsCenterExt;
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
	short dfTextDisplay;	/* Horizontal, Along Line, Above, Below */
	short dfLeader;			/* None, Left, Right */
	short dfRDSymbol;		/* None, Leading, Trailing */
	short dfTolerance;		/* None, One, Two, Limits */
	Coord dfTolerance1;
	Coord dfTolerance2;
	Handle dfDimLabelHdl;
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

struct PhFillModel
{
	short fFillType;		// old fFillModel short - wash, pat, paint, postscript
	short fFillIndex;		// index of wash, pat, or variant for paintFill()
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

struct PixPat
{
	short patType;
	long patMap;
	long patData;
	long patXData;
	short patXValid;
	long patXMap;
	uchar pat1Data[8];
};

struct BitMap
{
	long baseAddr;
	short rowBytes;
	PhRect bounds;
};

struct PixMap : public BitMap
{
	short pmVersion;
	short packType;
	long packSize;
	long hRes;
	long vRes;
	short pixelType;
	short pixelSize;
	short cmpCount;
	short cmpSize;
	long planeBytes;
	long pmHandle;
	long pmReserved;
};

struct PixPatMap
{
	PixPat pp;
	PixMap pm;
};

struct RGBColor
{
	short red;
	short green;
	short blue;
};

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

struct PhBitmapIOStruct
{
	long fBitmapOffset;	/* 0L if no bitmap */
	long fBitmapLength;	/* 0L if no bitmap */
	ushort fShareCount;
	Boolean fColor;		/* set to true if color PixMap is used. */
	char fUnused;		/* pad for even bytes */
};

typedef short PhAlignment;

enum
{
	kPhPenInvis = 0,
	kPhPenColor,			/* rgb color */
	kPhPenGray,
	kPhPenPat				/* b&w or fullColor pattern */
};

struct PhPenModel
{
	short fPenW;			// 0x8001 for hair, otherwise > 1
	char fAlignment;		// enum type Alignment (GUStructs.h)
	char fPenType;
	ushort fPenStyle;		// contains 2 endcap, 1 dash model indices
	short fColorIndex;		// % value if gray pmo
};

typedef char Style;

#define	kMaxLineSpacing	127
/* line spacing constants */
enum
{
	kPhSingleSpace = -3,
	kPhOne_halfSpace = -2,
	kPhDoubleSpace = -1,
	kPhSpaceOther = kMaxLineSpacing
};
typedef char TSpace;

/* character Case constants */
enum
{
	kPhAsIs = 1,
	kPhUppercase,
	kPhLowercase,
	kPhTitleText
};
typedef char TCase;

struct PhTextParams
{
	short fTextFont;
	short fTextSize;
	Style fTextStyle;
	char fDummy;
	short fTextJust;
	TSpace fTextSpace;
	TCase fTextCase;
	RGBColor fTextColor;
};

typedef short PhLineAttribute;

struct PhParallelAttr
{
	PhFillModel fFillModel;		/* fill model						*/
	Coord fWidth;				/* width between parallel lines		*/
	PhLineAttribute fLineAttr;	/* parallel line attribute			*/
};

/* printing preference */

#define	kPhPrintByCols		0x0001
#define kPhRegMarks			0x0002
#define kPhPrintLastFirst	0x0004
#define kPhPrintPlot		0x0008

typedef ushort DmPrintFlags;		/* bit or of above masks */

enum
{
	kPhNoTrim = 0, kPhManualTrim, kPhAutoTrim
};
typedef char TrimMethod;

struct PhFilletAttr 
{
	Coord fRadius;
	TrimMethod fTrimMethod;		/* kAutoTrim, kManualTrim, kNoTrim */
	Boolean fIsSmallerArc;
};

struct PhChamferAttr 
{
	Coord fLine1SetBack;
	Coord fLine2SetBack;
	TrimMethod fTrimMethod;		/* kAutoTrim, kManualTrim, kNoTrim */
	char fDummy;
};

struct PhActiveEnvirons
{
	short fToolID;
	PhFillModel fFillModel;
	PhPenModel fPenModel;
	PhTextParams fTextParams;	/* TextStructs */
	PhParallelAttr fParallelAttr;	 /* parallel line attribute */
	Boolean fDimVisible;
	Boolean fHasGrid;
	Boolean fHideText;
	Boolean fShowRulers;
	PhPoint fPrevZoomScroll;
	LongPoint fRulerOrigin;
	Boolean fSnapToObject;
	Boolean fStandardRulers;
	Boolean fHidePageBreaks;
	Boolean fPlotterMode;
	DmPrintFlags fPrintFlags;
	Boolean fDimLineFlag;
	char fAngleSnapsToACirclePart1;
	short fDimTextAlgn;	/* Dim Text alignment (above, along, uni) */
	char fPrevZoom;		/* stored in signed char */
	unsigned char fAngleSnapsToACirclePart2;
	PhFilletAttr fFilletAttr;
	PhChamferAttr fChamferAttr;
};

struct TPrInfo
{
	short iDev;		/*Font mgr/QuickDraw device code*/
	short iVRes;	/*Resolution of device, in device coordinates*/
	short iHRes;	/*..note: V before H => compatable with Point.*/
	PhRect rPage;	/*The page (printable) rectangle in device coordinates.*/
};

struct TPrStl {
	short wDev;
	short iPageV;
	short iPageH;
	char/*SInt8*/ bPort;
	char/*TFeed*/ feed;
};

struct TPrXInfo {
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

struct TPrJob {
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

struct PhDocHeader
{
	/* Std Mac Header - always 128 bytes */
	ulong fType;			/* OS Type						*/
	ushort fHeaderID;		/* 'MD'	for Dreams				*/
	ushort fVersion;		/* Version Number				*/
	TPrint fPrintRec;		/* Print Record					*/
};

struct PhDocState : public PhDocHeader
{
	PhRect fWindowRect;		/* WindowRect (in Globals)		*/
	short fZoom;			/* Old zoom						*/
	PhPoint fOldScroll;		/* Current scroll position		*/
	PhPoint fMViewPages;
	PhDocUnits fDocUnits;

	PhDocStds fDocStds;

	LongPoint fRulerOrigin;
	PhActiveEnvirons fActiveEnvirons;
};

struct PhSymDocHeader : public PhDocHeader
{
	short				fActiveSymbol;
	Boolean				fUnScaled;		/* for Unscaled Check Box		*/
	PhRect				fWindowRect;	/* WindowRect (in Globals)		*/
	PhPoint				fMViewPages;
	ushort				fZoom;			/* Old zoom						*/
	PhPoint				fOldScroll;		/* Current scroll position		*/
};

struct PhObjectInfoFieldSet
{
	char fF2Name[32];
	char fF3Name[32];
	char fF4Name[32];
	char fF5Name[32];
};

// ---------------------------------------------------------------------
struct PhFListHeader
{
	short fListID;		/* 2	id of fList - START header */
	long fListType;		/* 4	resType of header */
	short fNumElems;	/* 2	*/
	long fElemSize;		/* 4	*/
	long fInitSize;		/* 4	*/
	long fGrowSize;		/* 4	*/
	short fKeyOff;		/* 2	*/
	short fKeylen;		/* 2	*/
	short fIDData;		/* 2	resID of data if resource based */
	long fDataType;		/* 4	resource type of data array  - END header */
};

#define	kPhMaxLayerNameLen		16		// max name length for layer
#define	kPhActive				1		// layer mode
#define	kPhVisible				2
#define	kPhSelectable			4
#define	kPhScaled				8
#define	kPhNotGrayed			16
#define	kPhFillsOn				32
#define	kPhDeleteThisLayer		64
#define	kPhDummy1				128

struct PhLayerStruct
{
	char fLayerName[kPhMaxLayerNameLen];
	ushort fLayerFlags;
	short fScaleIndex;
	short fSnapIndex;
	long fNumShapes;
	ulong fLayerOffset;
	ulong fLayerLength;
};

#define kMaxViewName	30

struct PhSavedView
{
	char fViewName[kMaxViewName];
	char fCmdKey;
	char fFutureUse;
	PhPoint fOldScroll;
	short fZoomFactor;
};

/*------------------------- object ID ----------------------------------*/
/* object ID ( 1 - 127 )	*/
#define PhIDNoFillModel		0x8000		/* does not have fill model inhibit of fID */

const unsigned short
	PhIDLine			= ( 1 | PhIDNoFillModel ),

	PhIDFreehand		= 3,
	PhIDPolygon			= 4,

	PhIDRectCenter		= 9,
	PhIDRectDiagonal	= 10,

	PhIDRndRectangle	= 11,

	PhIDFreeSpline		= 13,
	PhIDPolySpline		= 14,

	PhIDCircle			= 20,
	PhIDArc				= 21,
	PhIDEArc			= 22,
	PhID3Arc			= 23,
	PhIDEllipse			= 24,
	PhIDBezier			= 30,
	PhIDCubic			= 31,

	PhIDText			= 40,

	PhIDBitmap			= 48,

	PhIDGroup			= ( 51 | PhIDNoFillModel ),
	PhIDInvalGroup		= ( 52 | PhIDNoFillModel ),
	PhIDMetaPoly		= 53,
	PhIDInvalMetaPoly	= 54,

/* Note dim objects ids are in closed range [80,99], put no other types in
	between.
*/
	PhIDHLineDim		= (81  | PhIDNoFillModel ),
	PhIDVLineDim		= (82  | PhIDNoFillModel ),
	PhIDSLineDim		= (83  | PhIDNoFillModel ),
	PhIDPLineDim		= (84  | PhIDNoFillModel ),
	PhIDRRadialDim		= (85  | PhIDNoFillModel ),
	PhIDDRadialDim		= (86  | PhIDNoFillModel ),
	PhIDCenterDim		= (87  | PhIDNoFillModel ),
	PhIDAngularDim		= (88  | PhIDNoFillModel ),

	PhIDDimPt2Pt		= ( 90 | PhIDNoFillModel ), 	/* keep this first */
	PhIDDimString		= 99,
	PhIDArea			= ( 94 | PhIDNoFillModel ),

/* range test for dependent text (attribute) objects */
// all future attribute text objects > 94
	PhIDFirstAttrObj	= (PhIDArea & ~PhIDNoFillModel),

	PhIDFirstDimObj		= 80,
	PhIDLastDimObj		= 99,

	PhIDParallelLine	= ( 100 | PhIDNoFillModel ),
	PhIDParallelPolygon	= 101,

	PhIDLastItem		= 127,
	PhIDInvalid		 	= 0;

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

enum
{
	kPhCenterRadius, kPhDiameter, kPh3Point, kPhEllipse
};
typedef short PhMethod;

enum
{
	kPhProportionalCorner=1,kPhConstantCorner,kPhEllipticalCorner
};
typedef short PhCorner;

enum
{
	kPhUnsmooth, kPhSmooth, kPhVerySmooth
};
typedef short PhSmoothing;

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

enum // These are the flags which get stored in fMoreFlags. Values would ideally be private to this file and its .c file
{
	kPhPointUse			= 0x01,
	kPhPointUseBar		= 0xFE,
	
	kPhPointComplex		= 0x02,
	kPhPointComplexBar	= 0xFD
};

enum // Values put into fComplexType field of PhComplexPtPicData
{
	kPhComplexPtLinkPoint,
	kPhComplexPtPerpDimPoint,
	kPhComplexPtArcDimPoint,
	kPhComplexPtOnLinePoint,
	kPhComplexPtAngularDimPoint
};

struct PhLinkResInfo
{
	long fObjectIndex ;
	long fPointIndex ;
	long fFlags ; // Currently bottom bit holds fNotifyOnly - rest are zero
};

struct PhComplexPtPicData
{
	ulong fSignature;
	short fKind;
	short fSpare1S;		// Currently zero ( STDIDD_PICCOMMENT_DATA is 6 bytes, hence this is a short )
	long fSpare2L;		// Currently zero
	long fSpare3L;		// Have padded to a multiple of 3 longs so can view 'Link' in Resorcerer in 6 columns
	long fVersion;		// Currently zero
	long fComplexType;	// values in enum in DBPointMgr.h

	// Followed by data specific to fLinkType
};

struct PhLinkedPtPicData : public PhComplexPtPicData
{
	PhLinkResInfo fLinks[1]; // Dynamic
};

struct PhDBPoint
{
	char fUse;
	uchar fFlags; // true if it is a vertex point
	LongPoint fLongPt;
};

struct PhDSShape
{
	long fPrev;
	ushort fID;				// which shape
	long fOwnerGroup;		// 0L if not in group
	Angle fRot;				// transformation
	long fRef;				// attribute
	PhPenModel fPenModel;	// how to draw it
	LongPoint fDatum;		// datum point
	ushort fShapeFlags;		// flags including datum code
	long fObjectInfo;
};

struct PhDSFillShape : public PhDSShape
{
	PhFillModel fFillModel;
};

struct PhDSLine : public PhDSShape
{
	short fConAngle;	// constrained angle
	PhDBPoint fP1;		// first endpoint
	PhDBPoint fP2;		// second endpoint
};

struct PhDSParallelShape : public PhDSShape
{
	PhParallelAttr fParallelAttr;	// parallel attribute
};

struct PhDSFillParallelShape : public PhDSFillShape
{
	PhParallelAttr fParallelAttr;	// parallel attribute
};

struct PhDSParallelLine : public PhDSParallelShape
{
	short fConAngle;		// constrained angle
	PhDBPoint fPoint[6];	// line end points
};

struct PhDSParallelPolygon : public PhDSFillParallelShape
{
	Boolean fClosed;
	char fDummy;
	short fCount;			// original line count
	PhDBPoint fPoint[1];	// fPoint1[fCount*3],then fPoint2[fCount*3]
};

struct PhDSPolygon : public PhDSFillShape
{
	Boolean fClosed;
	char fDummy;
	short fCount;			// point count
	PhDBPoint fPoint[1];
};

struct PhDSRectangle : public PhDSFillShape
{
	PhDBPoint			fPoint[4];
};

struct PhDSRndRectangle : public PhDSFillShape
{
	PhDBPoint fPoint[4];
	char fCornerType;
	char fDummy;
	PhDBPoint fDiagonals;
};

struct PhDSCircle : public PhDSFillShape
{
	char fMethod;
	char fDummy;
	PhDBPoint fCenter;
	Coord fRadius;
	PhDBPoint fPoint[3];
};

struct PhDSArc : public PhDSFillShape
{
	char fMethod;
	char fDummy;
	PhDBPoint fCenter;
	Coord fRadius;
	Angle fStartAng,fEndAng;
	PhDBPoint fPoint[3];
};

struct PhDSEArc : public PhDSFillShape
{
	char fMethod;
	char fDummy;
	PhDBPoint fCenter;
	Coord fHRadius,fVRadius;
	Angle fStartAng,fEndAng;
	PhDBPoint fPoint[3];
};

struct PhDSEllipse : public PhDSFillShape
{
	char fMethod;
	char fDummy;
	PhDBPoint fCenter;
	Coord fHRadius,fVRadius;
};

struct PhDSText  : public PhDSFillShape
{
	Boolean fParagraph;		// paragraph/caption flag
	Boolean fObjOpen;
	long fZoomTEH;
	long fNoZoomCopy;		// non-zoomed style info
	short fZoomFactor;		// text's zoom factor
	TSpace fTextSpace;		// text's line spacing
	char fDummy;
	short fTextJust;		// text's justification
	PhDBPoint fPoints[4];
};

struct ScrpSTElement
{
	long scrpStartChar;
	short scrpHeight;
	short scrpAscent;
	short scrpFont;
//	short scrpFace;
	char scrpFace;
	char scrpDummy;
	short scrpSize;
	RGBColor scrpColor;
};

struct StScrpRec
{
	short scrpNStyles;
	ScrpSTElement scrpStyleTab[1];
};

struct PhTextData
{
	/* From TEHandle:  */
	PhRect fDestRect, fViewRect;
	short fTELength;
	/* Size of block:  */
	Size fNZScrpSize;
};

struct PhDSBitmap  : public PhDSFillShape
{
	PhDBPoint fCorner[4];
	PhRect fSrcRect;
	short fMapHdlIndex;	 // index into the document bitmap list FList
};

struct PhDSGroup : public PhDSShape
{
	short fNumMembers;	// to save calc
	long fMembers[1];	// dynamic array
};

struct PhDSMetaPoly : public PhDSFillShape
{
	LongPoint fStartPoint;	// the first point to draw
	Boolean fClosed;
	char fDummy;
	short fNumValidMembers;	// valid, ordered, and connected
	short fNumClosedMembers;// valid and non-intersecting
	short fNumMembers;		// to save calc
	long fMembers[1];		// dynamic array
};

typedef ushort DimStatus;
typedef double	DimValue;

#define	kPhDimStrAttached		1		/* boolean flags for dimShape and dimStr status */
#define kPhDimVisible			2		/* Also gCurrDimStyle.fStyle... see note below */
#define kPhAreaStr				4
#define	kPhDimStrAbove			8
#define	kPhDimStrOnSlope		16
#define kPhDimNoWitness			32
#define kPhDimEndmarkOut		64
#define kPhAreaNeedsRecalc		128

#define kDimStrSize		24
#define kDimTolStrSize	12

struct PhDSDimStr : public PhDSFillShape
{
	DimStatus fStatus;
	short fFont;
	short fFontSize;
	PhDBPoint fAnchorPt;		// midpt of dimension line
	PhDBPoint fStrOffset;		// dimStr offsets
	long fObjHdl;				// fObjHdl is now DSShape
	char fString[kDimStrSize];	// the string
};

struct PhDSDimShape : public PhDSLine
{
	DimStatus fStatus;	// kAttached/kNotAttached
//	DimValue fValue;	// the dimension value
	char fValue[12];
	long fDimStrHdl;	// the dependent string record
};

struct PhDSArea : public PhDSShape
{
	DimStatus fStatus;
//	DimValue fValue;	// the value
	char fValue[12];
	long fShapeHdl;		// the dependent shape
	long fDimStrHdl;	// the area string
};

struct PhDSAssocDim : public PhDSFillShape
{
	DimStatus fStatus;
	short fFont;
	short fFontSize;
//	DimValue fValue;				// dimension value !!! note after font stuff
	char fValue[12];
	PhDimFormat format;				// local dimension format
	PhDimStd standard;				// local dimension standards
	LongRect textExtent;			// extent of dimension text
	LongRect dimStrDraw;			// extent of drawn text
	LongRect tol1Draw, tol2Draw;	// extent of tolerance strings
	char dimStr[kDimStrSize];		// string is in this dimension
	char tol1Str[kDimTolStrSize];	// tol 1 string
	char tol2Str[kDimTolStrSize];	// tol 2 string
};

struct PhDSLineDim : public PhDSAssocDim	// same data struct for Linear and Radial dimensions
{
	PhDBPoint refPt1, refPt2;
	PhDBPoint anchorPt;	// point to the left/center of the dimension string
	PhDBPoint witnessStart1, witnessEnd1, witnessStart2, witnessEnd2;
	PhDBPoint dimLineStart, dimLineEnd;
	PhDBPoint extLineStart, extLineEnd;
	PhDBPoint leaderLineStart, leaderLineEnd;
};

struct PhDSAngularDim : public PhDSAssocDim	// Angular dimension
{
	PhDBPoint refPt1;		// make sure that the order of the DBPoints is:
							// refPt1, refPt2, dimLineStart, dimLineEnd,
							// anchorPt, extPt, centerPt, witnessStart1,
							// witnessEnd1, witnessStart2, witnessEnd2.
	PhDBPoint refPt2;
	PhDBPoint dimLineStart;	// the arrow point of the first selected line
	PhDBPoint dimLineEnd;	// the arrow point of the second selected line
	PhDBPoint anchorPt;
	PhDBPoint extPt;		// the end point of the extension line
	PhDBPoint centerPt;		// the center of the arc
	PhDBPoint witnessStart1;
	PhDBPoint witnessEnd1;
	PhDBPoint witnessStart2;
	PhDBPoint witnessEnd2;

	Coord		fRadius;
	Angle		fStartAngle1, fArcAngle1;
	short		fCaps1;
	Angle		fStartAngle2, fArcAngle2;
	short		fCaps2;
};

struct PhDSCenterDim : public PhDSAssocDim
{
	PhDBPoint centerPt;
	Coord hRad, vRad;
};

struct PhObjectInfo
{
	short fVersion;
	short fSetNumber;
//	uchar fText[1];
};

#define kPhMaxPropTypes		8
/*--- Object Property ( property that can be in selected list ---*/
#define kPhShapeToDimLink	101
#define kPhShapeToAreaLink	102
/*--- Attribute Property ( non-visual, cannot be in selcted list ---*/
#define kPhAttrProperty		0x8000		/* this bit is set for attribute property */
#define kPhObjectName		(kPhAttrProperty | 201 )		/* name of an object */

struct PhPropListElem
{
	short fType;
	long fData;
};

struct PhPropList : public PhFListHeader
{
	PhPropListElem fData[1];/* dynamic array */
};

// -------------------------------------------------------------------------

struct FontInfo
{
	short ascent;
	short descent;
	short widMax;
	short leading;
};

struct PhReportCell
{
	short row;
	short col;
};

struct FSSpec
{
	short vRefNum;
	long parID;
	char name[64];
};

struct PhReportLinkInfo
{
	long fLinkType;
	FSSpec fLinkFileSpec;
	short fLastLinkError;
	short fUnitType;
	char reservedfiller[32];
};

struct PhReportScrap
{
	long nextScrap;			/* for accumulating clipboard UNUSED MD 4.1 */
	short cellCount;		/* number of cells in scrap */
	short textLen;			/* current cell scrap UNUSED MD 4.1  */
	long textHdl;			/* current cell scrap UNUSED MD 4.1  */
	long textScrapHdl;		/* current cell scrap UNUSED MD 4.1  */
	PhReportCell scrapSize;	/* extended scrap dimension */
	long cellListHdl;		/* extended scrap data */
};

struct PhReport
{
	short cClassCodes;
	short cVersion;					/* structure version number			*/
	long cOwnerDoc;					/* owner document handle			*/
	char cfTitle[64];				/* report title						*/
	long cPrintInfoHdl;				/* printer information record hdl	*/
	ushort cColumnPageBreak;		/* use bits to indicate breaks		*/
	PhTextParams cDefaultTextParams;/* default text parameters			*/
	FontInfo cFontInfo;				/* default text justification		*/
	short cRowHeight;				/* row height (changes with font)	*/

	short cFunctionWidth;			/* width of function column			*/
	short cDefaultColumnWidth;		/* default width for all columns	*/
	short cfColumnWidths[6];		/* report column dynamic widths		*/

	char cFieldNames[4][32];		/* field names used in this report	*/

	short cfNumRows;				/* current number of rows in report */
	long cfRowHdl;					/* row data information				*/

	PhReportCell cStartSelection;	/* row and column of beg selection	*/
	PhReportCell cEndSelection;		/* row and column of end selection	*/
	Boolean cAllowObjInfoSearch;	/* prevent obj info search on text	*/
	char cDummy;
	long cSelectRgn;				/* the selection region				*/

	ushort cFlags;					/* report status flags				*/
	long cChangedFlags;				/* flag field showing update needs	*/
	PhReportLinkInfo cfLinkInfo;	/* information for external linking	*/

	PhReportScrap cReportScrap;
	long cReportDoc;				/* visible report doc */
	long cReportFormatDoc;			/* visible report format editor */
};

#define kPhEmptyRow		0
#define	kPhTextRow		1
#define kPhCriteriaRow	2
#define	kPhFunctionRow	3

struct PhRowInfo
{
	uchar fRowFlags;
	uchar fRowType;
	short fRowID; /* number corresponding to row portion of excel name	*/
	long fRowData;
};

struct PhRowData
{
	ushort fChanged;				/* changed flags		*/
	uchar fRowType;
	uchar fSpare;
};

struct PhTextRowData : public PhRowData
{
	char fJust[6];
	ushort fUseStyle;
//	unsigned char fRowText[12];		/* packed character data */
};

struct PhCriteriaRowData : public PhRowData
{
	short fFieldRefNum[6];
};

struct PhFunctRowData : public PhRowData
{
	char fJust[6];
	double fResult;					/* last updated value		*/
	short dummy[2];
	ushort fUseStyle;
//	unsigned char fRowText[12];		/* packed char data			*/
};

// ---------------------------------------------------------------------------
union uptr
{
	char* pc;
	short* ps;
	long* pl;

	uptr() {}
	uptr(char* p) { pc=p; }
	uptr(short* p) { ps=p; }
	uptr(long* p) { pl=p; }
	void skip(int count) { pc+=count; }
	short swaps();
	void swaps(int count);
	long swapl();
	void swapl(int count);
	void swaplpt(int count);
	void swapPhDocState();
	void swapDBPoint(int count);
	void swapDSShape();
	void swapRGB(int count);
	char* p2c(int count=0);
	char* p2c_nt(int count=0);
};

struct DimFormat;
struct DimStandard;
class CMDShape;
class CLinkVertexList;

struct PhIOContext
{
	CFile *f;
	CMDShape* pGroup;
	PhDSFillShape* pPhShape;
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

	PhIOContext(CFile* pf) : f(pf), pGroup(NULL), nCount(0) {}
	short UseFont(const CString& sFont, int iCS);
	void ConvertPenModel(const PenModel& pm, PhPenModel& phpm);
	void ConvertPhPenModel(const PhPenModel& phpm, PenModel& pm);
	void ConvertFillModel(const FillModel& fm, PhFillModel& phfm);
	void ConvertPhFillModel(const PhFillModel& phfm, FillModel& fm);
	void ConvertParaAttr(const ParallelAttrs& pa, PhParallelAttr& phpa);
	void ConvertPhParaAttr(const PhParallelAttr& phpa, ParallelAttrs& pa);
	void ConvertTextParams(const TextParams& tp, PhTextParams& phtp);
	void ConvertPhTextParams(const PhTextParams& phtp, TextParams& tp);
	void ConvertDimStd(const DimStandard& std, PhDimStd& phstd);
	void ConvertPhDimStd(const PhDimStd& phstd, DimStandard& std);
	void ConvertDimFmt(const DimFormat& fmt, PhDimFormat& phfmt);
	void ConvertPhDimFmt(const PhDimFormat& phfmt, DimFormat& fmt);
	void ConvertLogFont(const LOGFONT& lf, short& pFont, short& pSize,Style& pStyle);
	void ConvertPhFont(short nFont, short nSize, Style nStyle,LOGFONT& lf);
	void ConvertC2P(const CString& str,char* pBuf,int nMaxSize);
};

#pragma pack()

#endif
