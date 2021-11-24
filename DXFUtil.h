#ifndef _DXFUtil_H_
#define _DXFUtil_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "stdafx.h"
#include "lpoint.h"	
#include "MDShape.h"
#include "MDGroupShape.h"
#include "PCDraft.h"
#include "dxf.h"

#define kDxfBufSize	2048	// buffer size (DXF read)
#define kDxfLayerNameLen  32

/* Shape flags */
#define kNoShape		0x0000

#define kLineShape		0x0010
#define kArcShape		0x0020
#define kCircleShape	0x0040
#define kPolylineShape	0x0080
#define kPolygonShape	0x0100
#define kTextShape		0x0200
#define kDimShape		0x0400

#define kGroupShape		0x1000
#define kInsertShape	0x2000

#define kLnW1	1
#define kLnW2	2
#define kLnW3	3
#define kLnW5	5
#define kLnW7	7
#define kLnW9	9
#define kLnW11	11
#define kLnW13	13

/*
typedef struct DxfLayerStruct {
  CString	fLayerName;
//  LayerHdl	fLayerHdl;
  short		fLayerColor;
  short		fDashIdx;
} DxfLayerStruct;
*/

typedef struct DxfBulgeStruct {
  short	fVertIdx;
  WrFixed	fBulge;
} DxfBulgeStruct;

typedef struct DxfLnWStruct {
  short	fVertIdx;
  short	fLnW;
} DxfLnWStruct;

typedef CArray <LongPoint,LongPoint> CLongPointArray;
typedef CArray <DxfBulgeStruct,DxfBulgeStruct> CDxfBulgeArray;
typedef CArray <DxfLnWStruct,DxfLnWStruct> CDxfLnWArray;
//typedef CArray <DxfLayerStruct,DxfLayerStruct> CLayerArray;

struct DxfParamStruct {
	bool			gRead3DData; 	// Does the DXF contain 3D info????
	short			sSectionId ;
	bool			sSectBegin ;
	short			sBufIdx ;
	bool			sTabBegin ;
	bool			sClosedSh ;
	bool			sIgnoreLayer;
	bool			sIgnoreShape;
	bool			sIgnoreGroup;
	CMDShape*		sShHdl;
	CMDShapeList	sGrShHdl;
	short			sShapeID;
	short			sShape;
	bool			sVertexOk;
	short			sLnWidth;
	CLongPointArray	sPtHdl;

	int		 		sFdPtPtr;

	short			sShPtCnt; /* specifies number of points in the point array */
	short			sMaxPtArryCnt;/* specifies max number of points that can be
								 	 accomodated in the point array */

	double			sCurUnitFactor;

//	GroupNameElem	sGroupNameElem;
	CMDGroupShape	sGrpFList;
	bool			sRdGrpBase;
	double			sGrpBaseXPt;
	double			sGrpBaseYPt;

	double			sXScaleFactor;
	double			sYScaleFactor;

	bool			sInsert;
	CMDGroupShape*	sAddGrpHdl;

	short			sPlTrSolOk;

/* fLayerName field has to be right packed with NULL char */
	bool			sLayerList;
	CString			sCurLayerStr;
	bool			sThisLayerStr;

	bool			sDimensionOk ;
	short			sDefColorIdx; 		/* Set Default Color to Black Color index */
	short			sEntityColorIdx;
	short			sEntityDashIdx ;
	short			sPolyFlag ;
	CDxfBulgeArray	sBulgeListHdl; 	/* Hal for Bulge of Curve Fit */
	CDxfLnWArray 	sLnWListHdl; 		/* Handle for Line Weights of Polygon */

	short			sDefaultDashIdx;	/* Continuous Dash Idx*/

	int				sLStyleIdx;			/* Flag for Setting LStyle to Col */
	short			sDashStrIdx;
	unsigned char	sDashCodeStr[kEditBufSize];

	short			sHorizAlign;
	short			sVertAlign;

	Angle			sRotAngle ;
	CString			sTextStr;

	bool			sExtMaxFound;
	bool			sExtMinFound;
	double			sExtMinX, sExtMinY, sExtMaxX, sExtMaxY;

	bool			sLookAheadForGrp;
	CString			sGrpName;

	short			sFileRefNum;
	char *			sBufPtr;
	bool			sLAGrpFound;
//	LayerHdl		sBlkLayerHdl;

	CRect			sRect;
	bool			sExit;
	bool			bFirstLayer;
};

/*------------------------ Line Style Table ------------------------------*/

/*
   Line Style Variables Tables -- Contain strings that can be
   found within Line Styles of ACAD.
*/

/* For Solid, Trace, or PLine Entity */
enum {
  kPLineOk = 0,
  kSolidOk,
  kTraceOk,
  k3DFaceOk
};

/* For Solid, Trace, or PLine Entity */
enum {
  kPlFg = 0,
  kDonutFg = 1,
  kCurveFitFg = 2,
  kSplineFitFg = 4
};

/* For Text Vertical Alignment Group Code 73 */
enum {
  kTxtBaseline,
  kTxtBottom,
  kTxtMiddle,
  kTxtTop
};

/* For Text Horizontal Alignment Group Code 72 */
enum {
  kTxtLeft,
  kTxtCent,
  kTxtRight,
  kTxtAligned,
  kTxtHMiddle,
  kTxtFit
};

#define IDNoFillModel		0x0000		/* does not have fill model inhibit of fID */

const int
	IDLine				= ( 1 | IDNoFillModel ),

	IDFreehand			= 3,
	IDPolygon			= 4,

	IDRectCenter		= 9,
	IDRectDiagonal		= 10,

	IDRndRectangle		= 11,

	IDFreeSpline		= 13,
	IDPolySpline		= 14,

	IDCircle			= 20,
	IDArc				= 21,
	IDEArc				= 22,
	ID3Arc				= 23,
	IDEllipse			= 24,
	IDBezier			= 30,
	IDCubic				= 31,

	IDText				= 40,

	IDBitmap			= 48,

	IDGroup				= ( 51 | IDNoFillModel ),
	IDInvalGroup		= ( 52 | IDNoFillModel ),
	IDMetaPoly			= 53,
	IDInvalMetaPoly		= 54,



/* Note dim objects ids are in closed range [80,99], put no other types in
	between.
*/

	IDHLineDim			= (81  | IDNoFillModel ),
	IDVLineDim			= (82  | IDNoFillModel ),
	IDSLineDim			= (83  | IDNoFillModel ),
	IDPLineDim			= (84  | IDNoFillModel ),
	IDRRadialDim		= (85  | IDNoFillModel ),
	IDDRadialDim		= (86  | IDNoFillModel ),
	IDCenterDim			= (87  | IDNoFillModel ),
	IDAngularDim		= (88  | IDNoFillModel ),


	IDDimPt2Pt			= ( 90 | IDNoFillModel ), 	/* keep this first */
	IDDimString			= 99,
	IDArea				= ( 94 | IDNoFillModel ),

/* range test for dependent text (attribute) objects */
// all future attribute text objects > 94
	IDFirstAttrObj		= (IDArea & ~IDNoFillModel),

	IDFirstDimObj		= 80,
	IDLastDimObj		= 99,

	IDParallelLine		= ( 100 | IDNoFillModel ),
	IDParallelPolygon	= 101,

	IDLastItem		 	= 127,


	IDInvalid		 	= 0;

/*------------------------- handle -------------------------------------*/

enum
	{
/* resize/reshape constants	*/
	 kHdlSize			= 2			/* for each direction	*/
	,kHandleSlop		= 4			/* port of handle that extend outside extents */

/*------------------------- min & max ----------------------------------*/

	,kMaxPoint			= 300			/* max points for objects	*/

/*------------------------- misc constant ------------------------------*/


	,kNoIndex			= -1				/* no Index	*/
	,kNoKeyPressed	 	= -1				/* CAUTION: use short	*/
	};

/*------------------------- typedef enum -------------------------------*/

typedef enum {
	kCenterRadius, kDiameter, k3Point, kEllipse
} Method;

typedef enum {
	kProportionalCorner=1,kConstantCorner,kEllipticalCorner
} Corner;

typedef enum {
	kUnsmooth, kSmooth, kVerySmooth
} Smoothing;

/*------------------------- typedef ------------------------------------*/

enum {
	kNoneIndex = 0,
	kWhiteIndex = 1,
	kBlackIndex = 2
};

/* possible values for PenModel.fPenType */

//enum {
//	kPenInvis = 0,
//	kPenColor,			/* rgb color */
//	kPenGray,
//	kPenPat				/* b&w or fullColor pattern */
//};

/*
	on a b&w machine,
		invisible pen is	fPenType = kPenInvis, fColorIndex = kNoneIndex
		white pen			fPenType = kPenColor, fColorIndex = kWhiteIndex
		black pen			fPenType = kPenColor, fColorIndex = kBlackIndex
*/

extern HeaderTab		gHeaderTab;
extern TablesTab		gTablesTab;
extern EntitiesTab   	gEntitiesTab;
extern LineStyleTab  	gLineStyleTab;

void findStrFromTab (short		pStrId,	CString&	pStrReturn, short pWhichTab);


#endif
