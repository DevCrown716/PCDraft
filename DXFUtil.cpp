#include "stdafx.h"
#include "DXFUtil.h"

/*------------------------ Header variables ------------------------------*/

/*
   Header Variables Tables -- Contain strings that can be
   found within Header Section.
*/

// All commented headers are now obselete - DXF Ver 13.
struct PALMACSYSTEM palMacSystem =
{
 0x300,   16,
 {
  {   0,   0,   0, 0},
  { 255,   0,   0, 0},
  { 255, 255,   0, 0},
  {   0, 255,   0, 0},
  {   0, 255, 255, 0},
  {   0,   0, 255, 0},
  { 255,   0, 255, 0},
  { 255, 255, 255, 0},
  { 128, 128, 128, 0},
  { 255,   0,  50, 0},
  {  64,   0,   0, 0},
  {   0, 128,   0, 0},
  {   0, 128, 128, 0},
  {   0,   0, 128, 0},
  { 255,   0, 173, 0},
  { 192, 192, 192, 0},
 }
};


LineStyleTab  gLineStyleTab = {
	"CONTINUOUS",   kDxfContinuous,
	"XXXXXXXXXX",   kDxfCustomLn01,
	"XXXXXXXXXX",   kDxfCustomLn02,
	"XXXXXXXXXX",   kDxfCustomLn03,
	"XXXXXXXXXX",   kDxfCustomLn04,
	"XXXXXXXXXX",   kDxfCustomLn05,
	"XXXXXXXXXX",   kDxfCustomLn06,
	"XXXXXXXXXX",   kDxfCustomLn07,
	"XXXXXXXXXX",   kDxfCustomLn08,
	"XXXXXXXXXX",   kDxfCustomLn09,
	"XXXXXXXXXX",   kDxfCustomLn10,
	"XXXXXXXXXX",   kDxfCustomLn11,
	"XXXXXXXXXX",   kDxfCustomLn12,
	"XXXXXXXXXX",   kDxfCustomLn13,
	"XXXXXXXXXX",   kDxfCustomLn14,
	"XXXXXXXXXX",   kDxfCustomLn15,
};  /* LineStylesTab */


HeaderTab  gHeaderTab = {
	"$ACADVER",	kACADVER,
	"$ANGBASE",	kANGBASE,
	"$ANGDIR",	kANGDIR,
	"$AUNITS",	kAUNITS,
	"$AUPREC",	kAUPREC,
	"$AXISMODE",kAXISMODE,
	"$AXISUNIT",kAXISUNIT,
	"$CECOLOR",	kCECOLOR,
	"$CELTYPE",	kCELTYPE,
	"$CLAYER",	kCLAYER,
	"$DIMALT",	kDIMALT,
	"$DIMALTD",	kDIMALTD,
	"$DIMALTF",	kDIMALTF,
	"$DIMAPOST",kDIMAPOST,
	"$DIMASO",	kDIMASO,
	"$DIMASZ",	kDIMASZ,
	"$DIMBLK",	kDIMBLK,
	"$DIMBLK1",	kDIMBLK1,
	"$DIMBLK2",	kDIMBLK2,
	"$DIMCEN",	kDIMCEN,
	"$DIMCLRD",	kDIMCLRD,		//	Not needed in DXF Ver13
	"$DIMCLRE",	kDIMCLRE,		//	Not needed in DXF Ver13
	"$DIMCLRT",	kDIMCLRT,		//	Not needed in DXF Ver13
	"$DIMDLE",	kDIMDLE,
	"$DIMDLI",	kDIMDLI,
	"$DIMEXE",	kDIMEXE,
	"$DIMEXO",	kDIMEXO,
	"$DIMGAP",	kDIMGAP,
	"$DIMLFAC",	kDIMLFAC,
	"$DIMLIM",	kDIMLIM,
	"$DIMPOST",	kDIMPOST,
	"$DIMRND",	kDIMRND,
	"$DIMSCALE",kDIMSCALE,
	"$DIMSE1",	kDIMSE1,
	"$DIMSE2",	kDIMSE2,
	"$DIMSOXD",	kDIMSOXD,
	"$DIMSTYLE",kADIMSTYLE,		//	Not needed in DXF Ver13
	"$DIMTAD",	kDIMTAD,
	"$DIMTIH",	kDIMTIH,
	"$DIMTIX",	kDIMTIX,
	"$DIMTOFL",	kDIMTOFL,
	"$DIMTOH",	kDIMTOH,
	"$DIMTOL",	kDIMTOL,
	"$DIMTVP",	kDIMTVP,
	"$DIMTXT",	kDIMTXT,
	"$DIMZIN",	kDIMZIN,
	"$EXTMAX",	kEXTMAX,
	"$EXTMIN",	kEXTMIN,
	"$FASTZOOM",kFASTZOOM,
	"$FILLMODE",kFILLMODE,
	"$GRIDMODE",kGRIDMODE,
	"$GRIDUNIT",kGRIDUNIT,
	"$HANDSEED",kHANDSEED,
	"$INSBASE",	kINSBASE,
	"$LIMMAX",	kLIMMAX,
	"$LIMMIN",	kLIMMIN,
	"$LTSCALE",	kLTSCALE,
	"$LUNITS",	kLUNITS,
	"$LUPREC",	kLUPREC,
	"$MAXACTVP",kMAXACTVP,		//	Not needed in DXF Ver13
	"$PEXTMAX",	kPEXTMAX,		//	Not needed in DXF Ver13
	"$PEXTMIN",	kPEXTMIN,		//	Not needed in DXF Ver13
	"$PLIMMAX",	kPLIMMAX,		//	Not needed in DXF Ver13
	"$PLIMMIN",	kPLIMMIN,		//	Not needed in DXF Ver13
	"$PLINEWID",kPLINEWID,
	"$PUCSNAME",kPUCSNAME,		//	Not needed in DXF Ver13
	"$PUCSORG",	kPUCSORG,		//	Not needed in DXF Ver13
	"$PUCSXDIR",kPUCSXDIR,		//	Not needed in DXF Ver13
	"$PUCSYDIR",kPUCSYDIR,		//	Not needed in DXF Ver13
	"$SNAPANG",	kSNAPANG,
	"$SNAPBASE",kSNAPBASE,
	"$SNAPMODE",kSNAPMODE,
	"$SNAPSTYLE",	kSNAPSTYLE,
	"$SNAPUNIT",	kSNAPUNIT,
	"$SPLINESEGS",  kSPLINESEGS,
	"$TEXTSIZE",	kTEXTSIZE,
	"$TEXTSTTYLE",  kTEXTSTTYLE,
	"$UCSNAME",	kUCSNAME,
	"$UCSORG",	kUCSORG,
	"$UCSXDIR",	kUCSXDIR,
	"$UCSYDIR",	kUCSYDIR,
	"$UNITMODE",kUNITMODE,
	"$USERI1",	kUSERI1,
	"$USERI2",	kUSERI2,
	"$USERI3",	kUSERI3,
	"$USERI4",	kUSERI4,
	"$USERI5",	kUSERI5,
	"$USERR1",	kUSERR1,
	"$USERR2",	kUSERR2,
	"$VIEWCTR",	kVIEWCTR,
	"$VIEWDIR",	kVIEWDIR,
	"$VIEWSIZE",kVIEWSIZE,
	"$WORLDVIEW",	kWORLDVIEW,
	"ENDSEC",	kHdrEndSec,
	"HEADER",	kHdrHeader,
	"SECTION",	kHdrSection, // 95???
	"TABLES",	kTABLES,
	"BLOCKS",	kDxfBLOCKS,
	"ENTITIES",	kDxfEntities,
	// New for DXF ver 13
	"$ATTDIA", kATTDIA,
	"$ATTMODE", kATTMODE,
	"$ATTREQ", kATTREQ,
	"$BLIPMODE", kBLIPMODE,
	"$CHAMFERA", kCHAMFERA,
	"$CHAMFERB", kCHAMFERB,
	"$COORDS", kCOORDS,
	"$DIMSAH", kDIMSAH,
	"$DIMSHO", kDIMSHO,
	"$DIMTM", kDIMTM,
	"$DIMTP", kDIMTP,
	"$DIMTSZ", kDIMTSZ,
	"$DRAGMODE", kDRAGMODE,
	"$ELEVATION", kELEVATION,
	"$FILLETRAD", kFILLETRAD,
	"$FLATLAND", kFLATLAND,
	"$HANDLING", kHANDLING,
	"$LIMCHECK", kLIMCHECK,
	"$MENU", kMENU,
	"$MIRRTEXT", kMIRRTEXT,
	"$ORTHOMODE", kORTHOMODE,
	"$OSMODE", kOSMODE,
	"$PDMODE", kPDMODE,
	"$PDSIZE", kPDSIZE,
	"$QTEXTMODE", kQTEXTMODE,
	"$REGENMODE", kREGENMODE,
	"$SKETCHINC", kSKETCHINC,
	"$SKPOLY", kSKPOLY,
	"$SPLFRAME", kSPLFRAME,
	"$SPLINETYPE", kSPLINETYPE,
	"$SURFTAB1", kSURFTAB1,
	"$SURFTAB2", kSURFTAB2,
	"$SURFTYPE", kSURFTYPE,
	"$SURFU", kSURFU,
	"$SURFV", kSURFV,
	"$TDCREATE", kTDCREATE,
	"$TDINDWG", kTDINDWG,
	"$TDUPDATE", kTDUPDATE,
	"$TDUSERTIMER", kTDUSERTIMER,
	"$THICKNESS", kTHICKNESS,
	"$TRACEWID", kTRACEWID,
	"$USRTIMER", kUSRTIMER,
}; /* HeaderTab */

/*------------------------ Types of Table  ------------------------------*/

/*
   Tables Variables Tables -- Contain strings that can be
   found within Tables Section.
*/
TablesTab  gTablesTab = {
	"APPID",	kAPPID,
	"DIMSTYLE", kDIMSTYLE,
	"ENDSEC",	kENDSEC,
	"ENDTAB",	kENDTAB,
	"LTYPE",	kLTYPE,
	"LAYER",	kLAYER,
	"SECTION",  kSECTION,
	"STYLE",	kSTYLE,
	"TABLE",	kTABLE,
	"TABLES",	kTABLES,
	"VIEW",		kVIEW,
	"UCS",		kUCS,
	"VPORT",	kVPORT
};  /* TablesTab */

/*------------------------ Entities Types ------------------------------*/

/*
   Entities Variables Tables -- Contain strings that can be
   found within Block & Entities Section.
*/
EntitiesTab  gEntitiesTab = {
	"ARC",		kDxfArc,
	"ATTDEF",	kDxfAttdef,
	"ATTRIB",	kDxfAttrib,
	"BLOCK",	kDxfBlock,
	"CIRCLE",	kDxfCircle,
	"DIAMETER", kDxfDiameter,
	"DIMENSION",kDxfDimension,
	"ENDBLK",	kDxfEndblk,
	"LINE",		kDxfLine,
	"INSERT",	kDxfInsert,
	"LINEAR",	kDxfLinear,
	"ORDINATE", kDxfOrdinate,
	"POINT",	kDxfPoint,
	"POLYLINE", kDxfPolyline,
	"RADIUS",	kDxfRadius,
	"TEXT",		kDxfText,
	"SOLID",	kDxfSolid,
	"TRACE",	kDxfTrace,
	"VERTEX",	kDxfVertex,
	"VIEWPORT", kDxfViewport,
	"BLOCKS",	kDxfBLOCKS,
	"ENDSEC",	kDxfEndSec,
	"ENTITIES", kDxfEntities,
	"SECTION",	kDxfSection,
	"SEQEND",	kDxfSeqEnd,
	// 3D DXF......
	"3DLINE",	kDxf3DLine,
	"3DFACE",	kDxf3DFace
};  /* EntitiesTab */

/*
  Given the string id, it return the string found in pStrReturn.
  If the string is not found, pStrReturn will be NULL.
*/

void  findStrFromTab (
	short		pStrId,
	CString&	pStrReturn,
	short		pWhichTab )
{
	short		vIdx;
	DXFTablePtr	vTabPtr = NULL;

  	switch (pWhichTab) {
		case  kHeaderTab:
			vTabPtr = (DXFTablePtr)gHeaderTab;
			vIdx = pStrId - kHeaderBase;
			break;
		case  kTablesTab:
			vTabPtr = (DXFTablePtr)gTablesTab;
			vIdx = pStrId - kTablesBase;
			break;
		case  kEntitiesTab:
			vTabPtr = (DXFTablePtr)gEntitiesTab;
			vIdx = pStrId - kEntitiesBase;
			break;
		case kLineStyleTab:
			vTabPtr = (DXFTablePtr)gLineStyleTab;
			vIdx = pStrId - kLineStyleBase;
			break;
		default:
		/* No Tables exist */
		pStrReturn = "";
		return;
		break;
  	}

  	pStrReturn = (char *)vTabPtr[vIdx].fVarStr;
} /* findStrFromTab */
