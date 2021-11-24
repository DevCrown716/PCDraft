/******************************************************

  DXF.h
  Header file contains Common Types and Constants for DXF.

  Copyright © Microspot Ltd., 1993

  Created : 11/25/93

******************************************************/

#ifndef _DXF_H_
#define _DXF_H_

#if _MSC_VER>1000
#pragma once
#endif

#include "DrawPrim.h"

/*---------------------------------------------------------------------*/

//typedef LongRectPtr LFdRectPtr;

#define	kDXFOKAlertID	133
#define kAcadPaletteID	129

//#define kBufSize	1024

#define kNilOff     -1L
#define kEditBufSize  16

/* ACAD Palette */
struct PALMACSYSTEM
{
 WORD palVersion;
 WORD palNumEntries;
 PALETTEENTRY palEntries[16];
};
extern struct PALMACSYSTEM palMacSystem;

/* Error Code For Reading DXF */
enum {
  kDxfErr = 0,
  kDxfNoErr,
  kDxfEOF,
  kDxfOFlow,
  kDxfStopLookAhead
};

/* Group Value Types */
enum {
  kStringType = 0,
  kFloatType,
  kIntegerType
};

/*---------------------------------------------------------------------*/

/*
  Constants for the AutoCad Entity Group Codes.
*/
#define kEntryStart	0
#define kTextValue	1
#define kNameString	2

#define kOtherNameMin	3
#define kOtherNameMax	4

#define kEntityHandle	5
#define kLineTypeName	6
#define kTextStyleName	7
#define kLayerName	8
#define kVarNameId	9

#define kPrimaryXCoord	10
#define kOtherXCoordMin	11
#define kOtherXCoordMax	18

#define kPrimaryYCoord	20
#define kOtherYCoordMin	21
#define kOtherYCoordMax	28

#define kPrimaryZCoord	30
#define kOtherZCoordMin	31
#define kOtherZCoordMax	37

#define kOther2XCoord	12   /* For Trace & Solid */
#define kOther3XCoord	13   /* For Trace & Solid */

#define kOther2YCoord	22   /* For Trace & Solid */
#define kOther3YCoord	23   /* For Trace & Solid */

#define kOther2ZCoord	32   /* For Trace & Solid */
#define kOther3ZCoord	33   /* For Trace & Solid */

#define kOther4XCoord	14   /* For Linear & Angular Dim */
#define kOther4YCoord	24   /* For Linear & Angular Dim */
#define kOther4ZCoord	34   /* For Linear & Angular Dim */

#define kOther5XCoord	15   /* For Diam., Radius & Angular Dim */
#define kOther5YCoord	25   /* For Diam., Radius & Angular Dim */
#define kOther5ZCoord	35   /* For Diam., Radius & Angular Dim */

#define kOther6XCoord	16   /* For Arc Dim */
#define kOther6YCoord	26   /* For Arc Dim */
#define kOther6ZCoord	36   /* For Arc Dim */

#define kEntityElevation	38
#define kEntityThickness	39

#define kOtherFloatValMin	40
#define kFlatVal2	41
#define kFlatVal3	42
#define kFlatVal4	43
#define kFlatVal5	44
#define kFlatVal6	45
#define kFlatVal7	46
#define kFlatVal8	47
#define kOtherFloatValMax	48

#define kTableLength	49

#define kAngleMin	50
#define kAngleMax	51

#define kColorNumber	62
#define kEntitiesFollow	66

#define kOtherIntValMin	70
#define kIntVal0	70
#define kIntVal1	71
#define kHorizAlign	72
#define kNumOfDashCodes	73
#define kIntVal4	74
#define kIntVal5	75
#define kIntVal6	76
#define kIntVal7	77
#define kIntVal8	78
#define kOtherIntValMax	78

#define kXExtrusion	210
#define kYExtrusion	220
#define kZExtrusion	230
#define kComments	999
#define kAsciiString	1000
#define kRegApplName	1001
#define kXDataCtrlStr	1002
#define kXDataLayerName	1003
#define kChunkOfBytes	1004
#define kXDataDBHandle	1005

#define kXDataXCoord	1010
#define kXDataYCoord	1020
#define kXDataZCoord	1030

#define kXDataXCoord3DPos	1011
#define kXDataYCoord3DPos	1021
#define kXDataZCoord3DPos	1031

#define kXDataXComp3DDisp	1012
#define kXDataYComp3DDisp	1022
#define kXDataZComp3DDisp	1032

#define kXDataXComp3DDir	1013
#define kXDataYComp3DDir	1023
#define kXDataZComp3DDir	1033

#define kXDataFloatPtVal	1040
#define kXDataDistValue	1041
#define kXDataScaleFactor	1042
#define kXData16BitInteger	1070
#define kXData32BitSignLong	1071

/*
  Character constants.
*/
#define kCReturn	0x0D
#define kSpaceCh	0x20
#define kLineFeed	0x0A

#define kStrMin1	0
#define kStrMax1	9
#define kStrMin2	1000
#define kStrMax2	1009

#define kFloatMin1	10
#define kFloatMax1	59
#define kFloatMin2	210
#define kFloatMax2	239
#define kFloatMin3	1010
#define kFloatMax3	1059

#define kIntMin1	60
#define kIntMax1	79
#define kIntMin2	1060
#define kIntMax2	1079

/*------------------------ Header variables ------------------------------*/

#define kHeaderBase	101
#define kACADVER	(kHeaderBase+0)
#define kANGBASE	(kHeaderBase+1)
#define kANGDIR	(kHeaderBase+2)
#define kAUNITS	(kHeaderBase+3)
#define kAUPREC	(kHeaderBase+4)
#define kAXISMODE	(kHeaderBase+5)
#define kAXISUNIT	(kHeaderBase+6)
#define kCECOLOR	(kHeaderBase+7)
#define kCELTYPE	(kHeaderBase+8)
#define kCLAYER	(kHeaderBase+9)
#define kDIMALT	(kHeaderBase+10)
#define kDIMALTD	(kHeaderBase+11)
#define kDIMALTF	(kHeaderBase+12)
#define kDIMAPOST	(kHeaderBase+13)
#define kDIMASO	(kHeaderBase+14)
#define kDIMASZ	(kHeaderBase+15)
#define kDIMBLK	(kHeaderBase+16)
#define kDIMBLK1	(kHeaderBase+17)
#define kDIMBLK2	(kHeaderBase+18)
#define kDIMCEN	(kHeaderBase+19)
#define kDIMCLRD	(kHeaderBase+20)
#define kDIMCLRE	(kHeaderBase+21)
#define kDIMCLRT	(kHeaderBase+22)
#define kDIMDLE	(kHeaderBase+23)
#define kDIMDLI	(kHeaderBase+24)
#define kDIMEXE	(kHeaderBase+25)
#define kDIMEXO	(kHeaderBase+26)
#define kDIMGAP	(kHeaderBase+27)
#define kDIMLFAC	(kHeaderBase+28)
#define kDIMLIM	(kHeaderBase+29)
#define kDIMPOST	(kHeaderBase+30)
#define kDIMRND	(kHeaderBase+31)
#define kDIMSCALE	(kHeaderBase+32)
#define kDIMSE1	(kHeaderBase+33)
#define kDIMSE2	(kHeaderBase+34)
#define kDIMSOXD	(kHeaderBase+35)
#define kADIMSTYLE	(kHeaderBase+36)
#define kDIMTAD	(kHeaderBase+37)
#define kDIMTIH	(kHeaderBase+38)
#define kDIMTIX	(kHeaderBase+39)
#define kDIMTOFL	(kHeaderBase+40)
#define kDIMTOH	(kHeaderBase+41)
#define kDIMTOL	(kHeaderBase+42)
#define kDIMTVP	(kHeaderBase+43)
#define kDIMTXT	(kHeaderBase+44)
#define kDIMZIN	(kHeaderBase+45)
#define kEXTMAX	(kHeaderBase+46)
#define kEXTMIN	(kHeaderBase+47)
#define kFASTZOOM	(kHeaderBase+48) /* Prior to R11 */
#define kFILLMODE	(kHeaderBase+49)
#define kGRIDMODE	(kHeaderBase+50) /* Prior to R11 */
#define kGRIDUNIT	(kHeaderBase+51) /* Prior to R11 */
#define kHANDSEED	(kHeaderBase+52)
#define kINSBASE	(kHeaderBase+53)
#define kLIMMAX	(kHeaderBase+54)
#define kLIMMIN	(kHeaderBase+55)
#define kLTSCALE	(kHeaderBase+56)
#define kLUNITS	(kHeaderBase+57)
#define kLUPREC	(kHeaderBase+58)
#define kMAXACTVP	(kHeaderBase+59)
#define kPEXTMAX	(kHeaderBase+60)
#define kPEXTMIN	(kHeaderBase+61)
#define kPLIMMAX	(kHeaderBase+62)
#define kPLIMMIN	(kHeaderBase+63)
#define kPLINEWID	(kHeaderBase+64)
#define kPUCSNAME	(kHeaderBase+65)
#define kPUCSORG	(kHeaderBase+66)
#define kPUCSXDIR	(kHeaderBase+67)
#define kPUCSYDIR	(kHeaderBase+68)
#define kSNAPANG	(kHeaderBase+69)  /* Prior to R11 */
#define kSNAPBASE	(kHeaderBase+70)  /* Prior to R11 */
#define kSNAPMODE	(kHeaderBase+71)  /* Prior to R11 */
#define kSNAPSTYLE	(kHeaderBase+72)  /* Prior to R11 */
#define kSNAPUNIT	(kHeaderBase+73)  /* Prior to R11 */
#define kSPLINESEGS	(kHeaderBase+74)
#define kTEXTSIZE	(kHeaderBase+75)
#define kTEXTSTTYLE	(kHeaderBase+76)
#define kUCSNAME	(kHeaderBase+77)
#define kUCSORG	(kHeaderBase+78)
#define kUCSXDIR	(kHeaderBase+79)
#define kUCSYDIR	(kHeaderBase+80)
#define kUNITMODE	(kHeaderBase+81)
#define kUSERI1	(kHeaderBase+82)
#define kUSERI2	(kHeaderBase+83)
#define kUSERI3	(kHeaderBase+84)
#define kUSERI4	(kHeaderBase+85)
#define kUSERI5	(kHeaderBase+86)
#define kUSERR1	(kHeaderBase+87)
#define kUSERR2	(kHeaderBase+88)

#define kVIEWCTR	(kHeaderBase+89)  /* Prior to R11 */
#define kVIEWDIR	(kHeaderBase+90)  /* Prior to R11 */
#define kVIEWSIZE	(kHeaderBase+91)  /* Prior to R11 */
#define kWORLDVIEW	(kHeaderBase+92)
#define kHdrEndSec	(kHeaderBase+93)
#define kHdrHeader	(kHeaderBase+94)
#define kHdrSection	(kHeaderBase+95)

// New for DXF Ver13
#define kATTDIA		(kHeaderBase+99)
#define kATTMODE	(kHeaderBase+100)
#define kATTREQ		(kHeaderBase+101)
#define kBLIPMODE	(kHeaderBase+102)
#define kCHAMFERA	(kHeaderBase+103)
#define kCHAMFERB	(kHeaderBase+104)
#define kCOORDS		(kHeaderBase+105)
#define kDIMSAH		(kHeaderBase+106)
#define kDIMSHO		(kHeaderBase+107)
#define kDIMTM		(kHeaderBase+108)
#define kDIMTP		(kHeaderBase+109)
#define kDIMTSZ		(kHeaderBase+110)

#define kDRAGMODE	(kHeaderBase+111)
#define kELEVATION	(kHeaderBase+112)
#define kFILLETRAD	(kHeaderBase+113)
#define kFLATLAND	(kHeaderBase+114)
#define kHANDLING	(kHeaderBase+115)
#define kLIMCHECK	(kHeaderBase+116)
#define kMENU		(kHeaderBase+117)
#define kMIRRTEXT	(kHeaderBase+118)
#define kORTHOMODE	(kHeaderBase+119)
#define kOSMODE		(kHeaderBase+120)

#define kPDMODE		(kHeaderBase+121)
#define kPDSIZE		(kHeaderBase+122)
#define kQTEXTMODE	(kHeaderBase+123)
#define kREGENMODE	(kHeaderBase+124)
#define kSKETCHINC	(kHeaderBase+125)
#define kSKPOLY		(kHeaderBase+126)
#define kSPLFRAME	(kHeaderBase+127)
#define kSPLINETYPE	(kHeaderBase+128)
#define kSURFTAB1	(kHeaderBase+129)
#define kSURFTAB2	(kHeaderBase+130)

#define kSURFTYPE	(kHeaderBase+131)
#define kSURFU		(kHeaderBase+132)
#define kSURFV		(kHeaderBase+133)
#define kTDCREATE	(kHeaderBase+134)
#define kTDINDWG	(kHeaderBase+135)
#define kTDUPDATE	(kHeaderBase+136)
#define kTDUSERTIMER (kHeaderBase+137)
#define kTHICKNESS	(kHeaderBase+138)
#define kTRACEWID	(kHeaderBase+139)
#define kUSRTIMER	(kHeaderBase+140)

/*------------------------ Types of Table  ------------------------------*/

#define kTablesBase	501
#define kAPPID	(kTablesBase+0)
#define kDIMSTYLE	(kTablesBase+1)
#define kENDSEC	(kTablesBase+2)
#define kENDTAB	(kTablesBase+3)
#define kLTYPE	(kTablesBase+4)
#define kLAYER	(kTablesBase+5)
#define kSECTION	(kTablesBase+6)
#define kSTYLE	(kTablesBase+7)
#define kTABLE	(kTablesBase+8)
#define kTABLES	(kTablesBase+9)
#define kVIEW	(kTablesBase+10)
#define kUCS	(kTablesBase+11)
#define kVPORT	(kTablesBase+12)

/*------------------------ Entities Types ------------------------------*/

#define kEntitiesBase   601
#define kDxfArc	(kEntitiesBase+0)
#define kDxfAttdef	(kEntitiesBase+1)
#define kDxfAttrib	(kEntitiesBase+2)
#define kDxfBlock	(kEntitiesBase+3)
#define kDxfCircle	(kEntitiesBase+4)
#define kDxfDiameter	(kEntitiesBase+5)
#define kDxfDimension	(kEntitiesBase+6)
#define kDxfEndblk	(kEntitiesBase+7)
#define kDxfLine	(kEntitiesBase+8)
#define kDxfInsert	(kEntitiesBase+9)
#define kDxfLinear	(kEntitiesBase+10)
#define kDxfOrdinate	(kEntitiesBase+11)
#define kDxfPoint	(kEntitiesBase+12)
#define kDxfPolyline	(kEntitiesBase+13)
#define kDxfRadius	(kEntitiesBase+14)
#define kDxfText	(kEntitiesBase+15)
#define kDxfSolid	(kEntitiesBase+16)
#define kDxfTrace	(kEntitiesBase+17)
#define kDxfVertex	(kEntitiesBase+18)
#define kDxfViewport	(kEntitiesBase+19)
#define kDxfBLOCKS	(kEntitiesBase+20)
#define kDxfEndSec	(kEntitiesBase+21)
#define kDxfEntities	(kEntitiesBase+22)
#define kDxfSection	(kEntitiesBase+23)
#define kDxfSeqEnd	(kEntitiesBase+24)

// DXF Ver 13 - 3D entities....
#define kDxf3DLine	(kEntitiesBase+25)
#define kDxf3DFace	(kEntitiesBase+26)

/*------------------------ Line Style  Types ------------------------------*/

#define kLineStyleBase	701
#define kDxfContinuous	(kLineStyleBase+0)
#define kDxfCustomLn01	(kLineStyleBase+1)
#define kDxfCustomLn02	(kLineStyleBase+2)
#define kDxfCustomLn03	(kLineStyleBase+3)
#define kDxfCustomLn04	(kLineStyleBase+4)
#define kDxfCustomLn05	(kLineStyleBase+5)
#define kDxfCustomLn06	(kLineStyleBase+6)
#define kDxfCustomLn07	(kLineStyleBase+7)
#define kDxfCustomLn08	(kLineStyleBase+8)
#define kDxfCustomLn09	(kLineStyleBase+9)
#define kDxfCustomLn10	(kLineStyleBase+10)
#define kDxfCustomLn11	(kLineStyleBase+11)
#define kDxfCustomLn12	(kLineStyleBase+12)
#define kDxfCustomLn13	(kLineStyleBase+13)
#define kDxfCustomLn14	(kLineStyleBase+14)
#define kDxfCustomLn15	(kLineStyleBase+15)



#define kDxfOutMaxPolyPts	300
#define kBufLen				512
#define kACADPalEntries		16
#define kFitAlign			5
#define kDxfAlign			65
#define kCustom				"CUSTOM"
#define kSolidLine			"Solid Line"
#define kVerStr				"MacDraft 4.0"
#define kACVerCode			"AC1006"  /* AutoCad Release 10 */
#define kCustGrpName		"GR"

/*------------------------ Table Def --------------------------------------*/

#define kMaxHeaderTab	141
#define kMaxTablesTab	13
#define kMaxEntitiesTab	27

#define kMaxLineStyleTab	16

const Coord kLOrdEndmarkOutset = short ( kDimLineOut ) ;

enum {
  kHeaderTab = 1,
  kTablesTab,
  kBlockTab,
  kEntitiesTab,
  kLineStyleTab
};

typedef struct  DXFTable {
	char	fVarStr[15];
	short	fVarID;
} DXFTable, *DXFTablePtr;

typedef DXFTable HeaderTab[kMaxHeaderTab];
typedef DXFTable TablesTab[kMaxTablesTab];
typedef DXFTable EntitiesTab[kMaxEntitiesTab];
typedef DXFTable LineStyleTab[kMaxLineStyleTab];

#define kArrow				1
#define kOval				3
#define kSlash				4
#define kX					5
#define kVArrow				6

#define DashingMask			0x0f00

#define kClose				8
#define kOpen				4
#define kFill				2
#define kFrame				1

#define kDXF8BitIntLowRangeStart	280
#define kDXF8BitIntLowRangeEnd		289

#define kDXF16BitIntLowRangeStart	60
#define kDXF16BitIntLowRangeEnd		79
#define kDXF16BitIntMidRangeStart	170
#define kDXF16BitIntMidRangeEnd		175
#define kDXF16BitIntUpperRangeStart	1060
#define kDXF16BitIntUpperRangeEnd	1070

#define kDXF32BitIntLowRangeStart	90
#define kDXF32BitIntLowRangeEnd		99
#define kDXF32BitIntUpperRangeEnd	1071

#define kDXFStringLowRangeStart -5
#define kDXFStringLowRangeEnd 9
#define kDXFStringIndivid1 100
#define kDXFStringIndivid2 102
#define kDXFStringIndivid3 105
#define kDXFStringMidRangeStart 300
#define kDXFStringMidRangeEnd 369
#define kDXFStringUpperRangeStart 999
#define kDXFStringUpperRangeEnd 1009

#define kDXFRealLowRangeStart		10
#define kDXFRealLowRangeEnd			59
#define kDXFRealMidRangeStart		140
#define kDXFRealMidRangeEnd			147
#define kDXFRealMidRange2Start		210
#define kDXFRealMidRange2End		230
#define kDXFRealUpperRangeStart		1010
#define kDXFRealUpperRangeEnd		1059


//const WrFixed kFixedPie = WrFixedFromFixed ( 0x32440L ) ; /* Fixed '¦' (3.1416015625) (see IM: IV-64) */
//const WrFixed kFixedPie2 = WrFixedFromFixed ( 0x19220L ) ; /* Fixed '¦'/2 */
//const WrFixed kFixedPie4 = WrFixedFromFixed ( 0xC910L ) ; /* Fixed '¦'/4 */


/*---------------------------------------------------------------------*/

#endif

