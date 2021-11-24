#include "stdafx.h"

#define AD_PROTOTYPES
#define AD_VM_PC
#include "OpenDWG.h"

#include "PCDraftDoc.h"
#include "Layer.h"

#include "MDLineShape.h"
#include "MDCircle.h"
#include "MDArcShape.h"
#include "MDGroupShape.h"
#include "MDPolygonShape.h"
#include "MDBezierShape.h"
#include "MDEllipse.h"
#include "MDRectangle.h"
#include "MDCubicShape.h"
#include "MDText.h"
#include "MDLineDim.h"
#include "DocMapper.h"
#include "DraftContext.h"
#include "MDDimStr.h"
#include "MDAngularDim.h"
#include "MDRadialDim.h"
#include "MDAngularDim.h"

#include "phmath.h"
#include "colutils.h"

#include <assert.h>

#include "ProgressDlg.h"
#include "BadDWGObjectDlg.h"
#include "OpenDWGio.h"

const double	kMaxValue = 32767.0;
#ifndef _PERSONAL_EDITION
extern bool gUserCanceled;
extern bool m_bOverflowError;
#endif

//static void LoadoMeter(short percent);
static CProgressDlg s_dlgProgress;

struct DWGAttributeLink
{
	CMDText*	AttrDefObject ;
	CLayer*		AttrDefLayer ;
 	char		tag [ 513 ] ;
};

/*
struct LPoint2D
{
	double x ;
	double y ;

	double& operator [] ( unsigned long L ) { return ( ( double* ) ( &x ) ) [ L ] ; }
	const double& operator [] ( unsigned long L ) const { return ( ( double* ) ( &x ) ) [ L ] ; }
} ;

struct LVector2D
{
	double x ;
	double y ;

	double& operator [] ( unsigned long L ) { return ( ( double* ) ( &x ) ) [ L ] ; }
	const double& operator [] ( unsigned long L ) const { return ( ( double* ) ( &x ) ) [ L ] ; }
} ;

struct LMatrix3x2
{
	double X0 , Y0 ; // Scale / Rotation
	double X1 , Y1 ; // Scale / Rotation
	double X2 , Y2 ; // Offset

	LPoint2D& operator [] ( unsigned long L ) { return ( ( LPoint2D* ) ( &X0 ) ) [ L ] ; }
	const LPoint2D& operator [] ( unsigned long L ) const { return ( ( LPoint2D* ) ( &X0 ) ) [ L ] ; }
} ;

LPoint2D operator * ( const LPoint2D& V , const LMatrix3x2& M )
{
	LPoint2D Result ;
	Result.x = V.x * M [ 0 ] [ 0 ] + V.y * M [ 1 ] [ 0 ] + M [ 2 ] [ 0 ] ;
	Result.y = V.x * M [ 0 ] [ 1 ] + V.y * M [ 1 ] [ 1 ] + M [ 2 ] [ 1 ] ;
	return Result ;
}

void operator *= ( LPoint2D& R , const LMatrix3x2& M )
{
	LPoint2D Result ;
	Result.x = R.x * M [ 0 ] [ 0 ] + R.y * M [ 1 ] [ 0 ] + M [ 2 ] [ 0 ] ;
	Result.y = R.x * M [ 0 ] [ 1 ] + R.y * M [ 1 ] [ 1 ] + M [ 2 ] [ 1 ] ;

	R.x = Result.x ;
	R.y = Result.y ;
}

LVector2D operator * ( const LVector2D& V , const LMatrix3x2& M )
{
	LVector2D Result ;
	Result.x = V.x * M [ 0 ] [ 0 ] + V.y * M [ 1 ] [ 0 ] ;
	Result.y = V.x * M [ 0 ] [ 1 ] + V.y * M [ 1 ] [ 1 ] ;
	return Result ;
}

void operator *= ( LVector2D& R , const LMatrix3x2& M )
{
	LPoint2D Result ;
	Result.x = R.x * M [ 0 ] [ 0 ] + R.y * M [ 1 ] [ 0 ] ;
	Result.y = R.x * M [ 0 ] [ 1 ] + R.y * M [ 1 ] [ 1 ] ;

	R.x = Result.x ;
	R.y = Result.y ;
}

LMatrix3x2 operator * ( const LMatrix3x2& A , const LMatrix3x2& B )
{
	LMatrix3x2 Result ;
	Result [ 0 ] [ 0 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 0 ] [ 1 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 0 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 1 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 2 ] [ 0 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 0 ] + B [ 2 ] [ 0 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	Result [ 2 ] [ 1 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 1 ] + B [ 2 ] [ 1 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	return Result ;
}

void operator *= ( LMatrix3x2& A , const LMatrix3x2& B )
{
	LMatrix3x2 Result ;
	Result [ 0 ] [ 0 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 0 ] [ 1 ] = A [ 0 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 0 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 0 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 0 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 0 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 1 ] [ 1 ] = A [ 1 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 1 ] [ 1 ] * B [ 1 ] [ 1 ] ; // A [ 1 ] [ 2 ] is 0 though not stored 
	Result [ 2 ] [ 0 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 0 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 0 ] + B [ 2 ] [ 0 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	Result [ 2 ] [ 1 ] = A [ 2 ] [ 0 ] * B [ 0 ] [ 1 ] + A [ 2 ] [ 1 ] * B [ 1 ] [ 1 ] + B [ 2 ] [ 1 ] ; // A [ 2 ] [ 2 ] is 1 though not stored 
	A = Result ;
}

bool IsMirrored ( const LMatrix3x2& M )
{
	return M [ 0 ] [ 0 ] * M [ 1 ] [ 1 ] - M [ 0 ] [ 1 ] * M [ 1 ] [ 0 ] < 0 ; // cos^2 + sin^2 == 1, leaving xScale * yScale. RPH
}		

LMatrix3x2 ScaleMat3x2 ( const double X , const double Y )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = X ;
	M [ 0 ] [ 1 ] = 0.0 ;
	
	M [ 1 ] [ 0 ] = 0.0 ;
	M [ 1 ] [ 1 ] = Y ;
	
	M [ 2 ] [ 0 ] = 0.0 ;
	M [ 2 ] [ 1 ] = 0.0 ;
	return M ;
}

LMatrix3x2 TranslateMat3x2 ( const LVector2D v )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = 1.0 ;
	M [ 0 ] [ 1 ] = 0.0 ;
	
	M [ 1 ] [ 0 ] = 0.0 ;
	M [ 1 ] [ 1 ] = 1.0 ;
	
	M [ 2 ] [ 0 ] = v.x ;
	M [ 2 ] [ 1 ] = v.y ;
	return M ;
}

LMatrix3x2 TranslateMat3x2 ( double x , double y )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = 1.0 ;
	M [ 0 ] [ 1 ] = 0.0 ;
	
	M [ 1 ] [ 0 ] = 0.0 ;
	M [ 1 ] [ 1 ] = 1.0 ;
	
	M [ 2 ] [ 0 ] = x ;
	M [ 2 ] [ 1 ] = y ;
	return M ;
}

LMatrix3x2 RotateMat3x2 ( const double Radians )
{
	LMatrix3x2 M ;
	M [ 0 ] [ 0 ] = cos ( Radians ) ;
	M [ 0 ] [ 1 ] = -sin ( Radians ) ;
	
	M [ 1 ] [ 0 ] = sin ( Radians ) ;
	M [ 1 ] [ 1 ] = cos ( Radians ) ;
	
	M [ 2 ] [ 0 ] = 0.0 ;
	M [ 2 ] [ 1 ] = 0.0 ;
	return M ;
}

LMatrix3x2 RotateMat3x2 ( const LPoint2D Pivot , const double Radians )
{
	// Could be optimised easily
	return TranslateMat3x2 ( -Pivot.x , -Pivot.y ) * RotateMat3x2 ( Radians ) * TranslateMat3x2 ( Pivot.x , Pivot.y ) ;
}
*/

class CDWGImport
{
public:
	CDWGImport(CPCDraftDoc *pDoc):
		m_pDoc(pDoc),
//		handle(0),
		gDoingInserts(false),
		gGroupNesting(0),
		gDWGScale(1),
		g100mmDWGScale(1),


		gXScale(1),
		gYScale(1),
		gRotation(0),
		gXOffset(0),
		gYOffset(0),

		gBlockColor(1), //2),

		sCurIndex(0),

		gLayer(0),
		gPolyLine(0),
		gBezierLine(0),
		sCurGroup(0),
		m_bShowBadObjectAlerts(true)
//		m_bOverflowError(false)
		{
			gTransform [ 0 ] [ 0 ] = 1.0 ;
			gTransform [ 0 ] [ 1 ] = 0.0 ;
			gTransform [ 1 ] [ 0 ] = 0.0 ;
			gTransform [ 1 ] [ 1 ] = 1.0 ;
			gTransform [ 2 ] [ 0 ] = 0.0 ;
			gTransform [ 2 ] [ 1 ] = 0.0 ;

		}
	bool ImportFile(const char *szPath, double sUnits, bool bMetric);
private:
	CPCDraftDoc *m_pDoc;
	//AD_DB_HANDLE handle;
	//PAD_DWGHDR adhd;
	//PAD_ENT_HDR adenhd;
	//PAD_ENT aden;
	//PAD_TB adtb;
	//PAD_XD adxd;


	bool gDoingInserts;
	bool gAddOrigin;
	unsigned long gGroupNesting;
	double gDWGScale;
	double g100mmDWGScale;
//	LongBoundRect sDWGBounds;
	double gXScale;
	double gYScale;
	double gRotation;
	double gXOffset;
	double gYOffset;

	double gMinDWG;
	double gMaxDWG;

	LMatrix3x2	gTransform;


	short gBlockColor;
	long sCurIndex;
	CList<LongPoint,LongPoint&> gInsertOriginList;
	CTypedPtrList<CObList,CMDGroupShape*> sGroupList;
	CList<short,short&> sGroupColorList;
	CArray<DWGAttributeLink,DWGAttributeLink&> gDWGAttributeLinks;

	CLayer *gLayer;
	CMDPolygonShape *gPolyLine;
	CMDCubicShape *gBezierLine;

	CMDGroupShape *sCurGroup;

	bool DoImport(const char *szPath,double sUnits, bool bMetric);

	LongPoint GetLongValueNoTest (const double* pt);
	LongPoint GetLongValue(const double *pt);
	LongPoint GetLongPoint(const double *d);
	Coord GetScaledValue(const double &d);
	double RadiansToDegrees(double radians);

	//CLayer *GetCorrectLayer(PAD_LAY adlayer);
	//short GetDWGColor(short entDWGColorIndex, PAD_LAY adlayer);
	//void SetObjectAttributes(CMDShape* theShape,PAD_ENT_HDR adenhd,PAD_LAY adlayer,bool toFill=false);
	bool AddGroupObject (CLayer *layerHdl,CMDGroupShape *groupShape);
	//bool IterateInsert( AD_OBJHANDLE blockheaderobjhandle);
	bool AddObject(CLayer *layer,CMDShape *shapeHdl);
	bool RemoveObject(CLayer *layer,CMDShape *shapeHdl);

//	void OffsetObjects(CMDShape* shapeHdl, const Coord& x, const Coord& y);

	//bool DoDWGHeader(PAD_DWGHDR adheader);
	//bool DoNewLayer(PAD_LAY adlayer, char on, char frozen, char viewportfrozen, char locked, unsigned long index);
	//bool DoEntity(PAD_ENT_HDR adenhd,PAD_ENT aden);
	//bool DoLine(PAD_ENT_HDR adenhd,PAD_LAY adlayer,double *pt0,double *pt1, double lineWidth = 0 , short endCaps = 0 , Coord offset = 0 );
	//bool DoCircle(PAD_ENT_HDR adenhd,PAD_LAY adlayer, double* pt0, const double& radius);
	//bool Do3PtCircle(PAD_ENT_HDR adenhd,PAD_LAY adlayer, double* pt0, double* pt1, double* pt2);
	//bool Arc(PAD_ENT_HDR adenhd,double *center,double r,double stangle,double endangle,PAD_LAY adlayer);
	//bool DoArc(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoBlock(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoEndBlk(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoInsert(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoATTDEF(PAD_ENT_HDR adenhd, PAD_ENT aden, PAD_LAY adlayer);
	//bool DoATTRIB (PAD_ENT_HDR adenhd, PAD_ENT aden, PAD_LAY adlayer);
	//bool DoPolyline(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoVertex(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoSEQEND(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoTrace(PAD_ENT_HDR adenhd,PAD_LAY adlayer,double *pt0,double *pt1,double *pt2,double *pt3);
	//bool DoEllipse(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoSpline(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoLwpline(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoDimension(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//int CreateText (const char* text,CStyledText &stText,CStyledTextStyle Style);
	//void InitText (short entcolor, double textHeight, PAD_LAY adlayer, TextParams& mySettings);
	//bool DoMacDraftText( PAD_ENT_HDR adenhd,double* pt0, double* rotPt,char* text,double &height,long justification,long vertalignment,double &angle, bool italicText,PAD_LAY adlayer, CMDText** object = NULL );
	//bool DoTEXT(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoMTEXT(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoLEADER(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoTOLERANCE(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool DoPROXY(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer);
	//bool ReadGrBlobInfo( PAD_ENT_HDR adenhd, AD_VMADDR grblob, PAD_LAY adlayer);

	bool m_bShowBadObjectAlerts;
	CDWordArray m_BadObjects;
	bool BadObjectAlert(UINT nID);
//	bool m_bOverflowError;
};

//bool gUserCanceled=false;
const int sXCoord=0;
const int sYCoord=1;
const double kRadsToDeg=180.0/pi;
const double kTextScaleFactor			= 100.0 / 72.0; // don't ask me why, it just seems to be about the right factor

unsigned short g_clut_32000[256][4] = {
//	 0x0000, 0x03F0, 0x0000, 0x00FF,
	 0x0000, 0x0000, 0x0000, 0x0000,
	 0x0001, 0xFFFF, 0x0000, 0x0000,
	 0x0002, 0xFFFF, 0xFFFF, 0x0000,
	 0x0003, 0x0000, 0xFFFF, 0x0000,
	 0x0004, 0x0000, 0xFFFF, 0xFFFF,
	 0x0005, 0x0000, 0x0000, 0xFFFF,
	 0x0006, 0xFFFF, 0x0000, 0xFFFF,
	 0x0007, 0xFFFF, 0xFFFF, 0xFFFF,
	 0x0008, 0x76C8, 0x76C8, 0x76C8,
	 0x0009, 0x9916, 0x9916, 0x9916,
	 0x000A, 0xFFFF, 0x0000, 0x0000,
	 0x000B, 0xFFFF, 0x8000, 0x8000,
	 0x000C, 0xA666, 0x0000, 0x0000,
	 0x000D, 0xA666, 0x5333, 0x5333,
	 0x000E, 0x8000, 0x0000, 0x0000,
	 0x000F, 0x8000, 0x4000, 0x4000,
	 0x0010, 0x4CCD, 0x0000, 0x0000,
	 0x0011, 0x4CCD, 0x2666, 0x2666,
	 0x0012, 0x2666, 0x0000, 0x0000,
	 0x0013, 0x2666, 0x1333, 0x1333,
	 0x0014, 0xFFFF, 0x4000, 0x0000,
	 0x0015, 0xFFFF, 0x9FFF, 0x8000,
	 0x0016, 0xA666, 0x2999, 0x0000,
	 0x0017, 0xA666, 0x6803, 0x5333,
	 0x0018, 0x8000, 0x2000, 0x0000,
	 0x0019, 0x8000, 0x5000, 0x4000,
	 0x001A, 0x4CCD, 0x1333, 0x0000,
	 0x001B, 0x4CCD, 0x3000, 0x2666,
	 0x001C, 0x2666, 0x099A, 0x0000,
	 0x001D, 0x2666, 0x1803, 0x1333,
	 0x001E, 0xFFFF, 0x8000, 0x0000,
	 0x001F, 0xFFFF, 0xBFFF, 0x8000,
	 0x0020, 0xA666, 0x5333, 0x0000,
	 0x0021, 0xA666, 0x7CCC, 0x5333,
	 0x0022, 0x8000, 0x4000, 0x0000,
	 0x0023, 0x8000, 0x6000, 0x4000,
	 0x0024, 0x4CCD, 0x2666, 0x0000,
	 0x0025, 0x4CCD, 0x3999, 0x2666,
	 0x0026, 0x2666, 0x1333, 0x0000,
	 0x0027, 0x2666, 0x1CCD, 0x1333,
	 0x0028, 0xFFFF, 0xBFFF, 0x0000,
	 0x0029, 0xFFFF, 0xDFFF, 0x8000,
	 0x002A, 0xA666, 0x7CCC, 0x0000,
	 0x002B, 0xA666, 0x919C, 0x5333,
	 0x002C, 0x8000, 0x6000, 0x0000,
	 0x002D, 0x8000, 0x7000, 0x4000,
	 0x002E, 0x4CCD, 0x3999, 0x0000,
	 0x002F, 0x4CCD, 0x4333, 0x2666,
	 0x0030, 0x2666, 0x1CCD, 0x0000,
	 0x0031, 0x2666, 0x219D, 0x1333,
	 0x0032, 0xFFFF, 0xFFFF, 0x0000,
	 0x0033, 0xFFFF, 0xFFFF, 0x8000,
	 0x0034, 0xA666, 0xA666, 0x0000,
	 0x0035, 0xA666, 0xA666, 0x5333,
	 0x0036, 0x8000, 0x8000, 0x0000,
	 0x0037, 0x8000, 0x8000, 0x4000,
	 0x0038, 0x4CCD, 0x4CCD, 0x0000,
	 0x0039, 0x4CCD, 0x4CCD, 0x2666,
	 0x003A, 0x2666, 0x2666, 0x0000,
	 0x003B, 0x2666, 0x2666, 0x1333,
	 0x003C, 0xBFFF, 0xFFFF, 0x0000,
	 0x003D, 0xDFFF, 0xFFFF, 0x8000,
	 0x003E, 0x7CCC, 0xA666, 0x0000,
	 0x003F, 0x919C, 0xA666, 0x5333,
	 0x0040, 0x6000, 0x8000, 0x0000,
	 0x0041, 0x7000, 0x8000, 0x4000,
	 0x0042, 0x3999, 0x4CCD, 0x0000,
	 0x0043, 0x4333, 0x4CCD, 0x2666,
	 0x0044, 0x1CCD, 0x2666, 0x0000,
	 0x0045, 0x219D, 0x2666, 0x1333,
	 0x0046, 0x8000, 0xFFFF, 0x0000,
	 0x0047, 0xBFFF, 0xFFFF, 0x8000,
	 0x0048, 0x5333, 0xA666, 0x0000,
	 0x0049, 0x7CCC, 0xA666, 0x5333,
	 0x004A, 0x4000, 0x8000, 0x0000,
	 0x004B, 0x6000, 0x8000, 0x4000,
	 0x004C, 0x2666, 0x4CCD, 0x0000,
	 0x004D, 0x3999, 0x4CCD, 0x2666,
	 0x004E, 0x1333, 0x2666, 0x0000,
	 0x004F, 0x1CCD, 0x2666, 0x1333,
	 0x0050, 0x4000, 0xFFFF, 0x0000,
	 0x0051, 0x9FFF, 0xFFFF, 0x8000,
	 0x0052, 0x2999, 0xA666, 0x0000,
	 0x0053, 0x6803, 0xA666, 0x5333,
	 0x0054, 0x2000, 0x8000, 0x0000,
	 0x0055, 0x5000, 0x8000, 0x4000,
	 0x0056, 0x1333, 0x4CCD, 0x0000,
	 0x0057, 0x3000, 0x4CCD, 0x2666,
	 0x0058, 0x099A, 0x2666, 0x0000,
	 0x0059, 0x1803, 0x2666, 0x1333,
	 0x005A, 0x0000, 0xFFFF, 0x0000,
	 0x005B, 0x8000, 0xFFFF, 0x8000,
	 0x005C, 0x0000, 0xA666, 0x0000,
	 0x005D, 0x5333, 0xA666, 0x5333,
	 0x005E, 0x0000, 0x8000, 0x0000,
	 0x005F, 0x4000, 0x8000, 0x4000,
	 0x0060, 0x0000, 0x4CCD, 0x0000,
	 0x0061, 0x2666, 0x4CCD, 0x2666,
	 0x0062, 0x0000, 0x2666, 0x0000,
	 0x0063, 0x1333, 0x2666, 0x1333,
	 0x0064, 0x0000, 0xFFFF, 0x4000,
	 0x0065, 0x8000, 0xFFFF, 0x9FFF,
	 0x0066, 0x0000, 0xA666, 0x2999,
	 0x0067, 0x5333, 0xA666, 0x6803,
	 0x0068, 0x0000, 0x8000, 0x2000,
	 0x0069, 0x4000, 0x8000, 0x5000,
	 0x006A, 0x0000, 0x4CCD, 0x1333,
	 0x006B, 0x2666, 0x4CCD, 0x3000,
	 0x006C, 0x0000, 0x2666, 0x099A,
	 0x006D, 0x1333, 0x2666, 0x1803,
	 0x006E, 0x0000, 0xFFFF, 0x8000,
	 0x006F, 0x8000, 0xFFFF, 0xBFFF,
	 0x0070, 0x0000, 0xA666, 0x5333,
	 0x0071, 0x5333, 0xA666, 0x7CCC,
	 0x0072, 0x0000, 0x8000, 0x4000,
	 0x0073, 0x4000, 0x8000, 0x6000,
	 0x0074, 0x0000, 0x4CCD, 0x2666,
	 0x0075, 0x2666, 0x4CCD, 0x3999,
	 0x0076, 0x0000, 0x2666, 0x1333,
	 0x0077, 0x1333, 0x2666, 0x1CCD,
	 0x0078, 0x0000, 0xFFFF, 0xBFFF,
	 0x0079, 0x8000, 0xFFFF, 0xDFFF,
	 0x007A, 0x0000, 0xA666, 0x7CCC,
	 0x007B, 0x5333, 0xA666, 0x919C,
	 0x007C, 0x0000, 0x8000, 0x6000,
	 0x007D, 0x4000, 0x8000, 0x7000,
	 0x007E, 0x0000, 0x4CCD, 0x3999,
	 0x007F, 0x2666, 0x4CCD, 0x4333,
	 0x0080, 0x0000, 0x2666, 0x1CCD,
	 0x0081, 0x1333, 0x2666, 0x219D,
	 0x0082, 0x0000, 0xFFFF, 0xFFFF,
	 0x0083, 0x8000, 0xFFFF, 0xFFFF,
	 0x0084, 0x0000, 0xA666, 0xA666,
	 0x0085, 0x5333, 0xA666, 0xA666,
	 0x0086, 0x0000, 0x8000, 0x8000,
	 0x0087, 0x4000, 0x8000, 0x8000,
	 0x0088, 0x0000, 0x4CCD, 0x4CCD,
	 0x0089, 0x2666, 0x4CCD, 0x4CCD,
	 0x008A, 0x0000, 0x2666, 0x2666,
	 0x008B, 0x1333, 0x2666, 0x2666,
	 0x008C, 0x0000, 0xBFFF, 0xFFFF,
	 0x008D, 0x8000, 0xDFFF, 0xFFFF,
	 0x008E, 0x0000, 0x7CCC, 0xA666,
	 0x008F, 0x5333, 0x919C, 0xA666,
	 0x0090, 0x0000, 0x6000, 0x8000,
	 0x0091, 0x4000, 0x7000, 0x8000,
	 0x0092, 0x0000, 0x3999, 0x4CCD,
	 0x0093, 0x2666, 0x4333, 0x4CCD,
	 0x0094, 0x0000, 0x1CCD, 0x2666,
	 0x0095, 0x1333, 0x219D, 0x2666,
	 0x0096, 0x0000, 0x8000, 0xFFFF,
	 0x0097, 0x8000, 0xBFFF, 0xFFFF,
	 0x0098, 0x0000, 0x5333, 0xA666,
	 0x0099, 0x5333, 0x7CCC, 0xA666,
	 0x009A, 0x0000, 0x4000, 0x8000,
	 0x009B, 0x4000, 0x6000, 0x8000,
	 0x009C, 0x0000, 0x2666, 0x4CCD,
	 0x009D, 0x2666, 0x3999, 0x4CCD,
	 0x009E, 0x0000, 0x1333, 0x2666,
	 0x009F, 0x1333, 0x1CCD, 0x2666,
	 0x00A0, 0x0000, 0x4000, 0xFFFF,
	 0x00A1, 0x8000, 0x9FFF, 0xFFFF,
	 0x00A2, 0x0000, 0x2999, 0xA666,
	 0x00A3, 0x5333, 0x6803, 0xA666,
	 0x00A4, 0x0000, 0x2000, 0x8000,
	 0x00A5, 0x4000, 0x5000, 0x8000,
	 0x00A6, 0x0000, 0x1333, 0x4CCD,
	 0x00A7, 0x2666, 0x3000, 0x4CCD,
	 0x00A8, 0x0000, 0x099A, 0x2666,
	 0x00A9, 0x1333, 0x1803, 0x2666,
	 0x00AA, 0x0000, 0x0000, 0xFFFF,
	 0x00AB, 0x8000, 0x8000, 0xFFFF,
	 0x00AC, 0x0000, 0x0000, 0xA666,
	 0x00AD, 0x5333, 0x5333, 0xA666,
	 0x00AE, 0x0000, 0x0000, 0x8000,
	 0x00AF, 0x4000, 0x4000, 0x8000,
	 0x00B0, 0x0000, 0x0000, 0x4CCD,
	 0x00B1, 0x2666, 0x2666, 0x4CCD,
	 0x00B2, 0x0000, 0x0000, 0x2666,
	 0x00B3, 0x1333, 0x1333, 0x2666,
	 0x00B4, 0x4000, 0x0000, 0xFFFF,
	 0x00B5, 0x9FFF, 0x8000, 0xFFFF,
	 0x00B6, 0x2999, 0x0000, 0xA666,
	 0x00B7, 0x6803, 0x5333, 0xA666,
	 0x00B8, 0x2000, 0x0000, 0x8000,
	 0x00B9, 0x5000, 0x4000, 0x8000,
	 0x00BA, 0x1333, 0x0000, 0x4CCD,
	 0x00BB, 0x3000, 0x2666, 0x4CCD,
	 0x00BC, 0x099A, 0x0000, 0x2666,
	 0x00BD, 0x1803, 0x1333, 0x2666,
	 0x00BE, 0x8000, 0x0000, 0xFFFF,
	 0x00BF, 0xBFFF, 0x8000, 0xFFFF,
	 0x00C0, 0x5333, 0x0000, 0xA666,
	 0x00C1, 0x7CCC, 0x5333, 0xA666,
	 0x00C2, 0x4000, 0x0000, 0x8000,
	 0x00C3, 0x6000, 0x4000, 0x8000,
	 0x00C4, 0x2666, 0x0000, 0x4CCD,
	 0x00C5, 0x3999, 0x2666, 0x4CCD,
	 0x00C6, 0x1333, 0x0000, 0x2666,
	 0x00C7, 0x1CCD, 0x1333, 0x2666,
	 0x00C8, 0xBFFF, 0x0000, 0xFFFF,
	 0x00C9, 0xDFFF, 0x8000, 0xFFFF,
	 0x00CA, 0x7CCC, 0x0000, 0xA666,
	 0x00CB, 0x919C, 0x5333, 0xA666,
	 0x00CC, 0x6000, 0x0000, 0x8000,
	 0x00CD, 0x7000, 0x4000, 0x8000,
	 0x00CE, 0x3999, 0x0000, 0x4CCD,
	 0x00CF, 0x4333, 0x2666, 0x4CCD,
	 0x00D0, 0x1CCD, 0x0000, 0x2666,
	 0x00D1, 0x219D, 0x1333, 0x2666,
	 0x00D2, 0xFFFF, 0x0000, 0xFFFF,
	 0x00D3, 0xFFFF, 0x8000, 0xFFFF,
	 0x00D4, 0xA666, 0x0000, 0xA666,
	 0x00D5, 0xA666, 0x5333, 0xA666,
	 0x00D6, 0x8000, 0x0000, 0x8000,
	 0x00D7, 0x8000, 0x4000, 0x8000,
	 0x00D8, 0x4CCD, 0x0000, 0x4CCD,
	 0x00D9, 0x4CCD, 0x2666, 0x4CCD,
	 0x00DA, 0x2666, 0x0000, 0x2666,
	 0x00DB, 0x2666, 0x1333, 0x2666,
	 0x00DC, 0xFFFF, 0x0000, 0xBFFF,
	 0x00DD, 0xFFFF, 0x8000, 0xDFFF,
	 0x00DE, 0xA666, 0x0000, 0x7CCC,
	 0x00DF, 0xA666, 0x5333, 0x919C,
	 0x00E0, 0x8000, 0x0000, 0x6000,
	 0x00E1, 0x8000, 0x4000, 0x7000,
	 0x00E2, 0x4CCD, 0x0000, 0x3999,
	 0x00E3, 0x4CCD, 0x2666, 0x4333,
	 0x00E4, 0x2666, 0x0000, 0x1CCD,
	 0x00E5, 0x2666, 0x1333, 0x219D,
	 0x00E6, 0xFFFF, 0x0000, 0x8000,
	 0x00E7, 0xFFFF, 0x8000, 0xBFFF,
	 0x00E8, 0xA666, 0x0000, 0x5333,
	 0x00E9, 0xA666, 0x5333, 0x7CCC,
	 0x00EA, 0x8000, 0x0000, 0x4000,
	 0x00EB, 0x8000, 0x4000, 0x6000,
	 0x00EC, 0x4CCD, 0x0000, 0x2666,
	 0x00ED, 0x4CCD, 0x2666, 0x3999,
	 0x00EE, 0x2666, 0x0000, 0x1333,
	 0x00EF, 0x2666, 0x1333, 0x1CCD,
	 0x00F0, 0xFFFF, 0x0000, 0x4000,
	 0x00F1, 0xFFFF, 0x8000, 0x9FFF,
	 0x00F2, 0xA666, 0x0000, 0x2999,
	 0x00F3, 0xA666, 0x5333, 0x6803,
	 0x00F4, 0x8000, 0x0000, 0x2000,
	 0x00F5, 0x8000, 0x4000, 0x5000,
	 0x00F6, 0x4CCD, 0x0000, 0x1333,
	 0x00F7, 0x4CCD, 0x2666, 0x3000,
	 0x00F8, 0x2666, 0x0000, 0x099A,
	 0x00F9, 0x2666, 0x1333, 0x1803,
	 0x00FA, 0x547B, 0x547B, 0x547B,
	 0x00FB, 0x76C8, 0x76C8, 0x76C8,
	 0x00FC, 0x9916, 0x9916, 0x9916,
	 0x00FD, 0xBB64, 0xBB64, 0xBB64,
	 0x00FE, 0xDDB1, 0xDDB1, 0xDDB1,
	 0x00FF, 0xFFFF, 0xFFFF, 0xFFFF
};
#if 0
LongPoint CDWGImport::GetLongValueNoTest (const double* pt)
{
	assert (pt);

	LPoint2D xy = *( (LPoint2D*) pt) * gTransform ;

	LongPoint lPt;
	lPt.x = xy.x ;
	lPt.y = xy.y ;

	return lPt;
}

LongPoint CDWGImport::GetLongValue(const double* pt)
{
	assert (pt);

	LPoint2D xy = *( (LPoint2D*) pt) * gTransform ;

	if((xy.x<=-kMaxValue) || (xy.x>=kMaxValue) || (xy.y<=-kMaxValue) || (xy.y>=kMaxValue))
	{
		m_bOverflowError=true;
		gUserCanceled=true; // to avoid additional error message
	}

	LongPoint lPt;
	lPt.x = xy.x ;
	lPt.y = xy.y ;

	return lPt;
}

LongPoint CDWGImport::GetLongPoint(const double *d)
{
//	LongPoint lPt=
	return GetLongValue(d);

//	sDWGBounds|=lPt;
//	return lPt;
}

Coord CDWGImport::GetScaledValue(const double &d)
{
	double result = d * gDWGScale;
	if ( result < gMinDWG )
		gMinDWG = result ;
	if ( result > gMaxDWG )
		gMaxDWG = result ;

	if (result > kMaxValue || result < -kMaxValue)
	{
		m_bOverflowError=true;
		gUserCanceled=true; // to avoid additional error message
	}
	return result;
}

double CDWGImport::RadiansToDegrees(double radians)
{
	radians *= kRadsToDeg;
	if (gXScale < 0.0)
		radians = 180.0 - radians;
	if (gYScale < 0.0)
		radians = -radians;
	radians += gRotation;
	
	if ((radians > -0.1) && (radians < 0.1))
		radians = 0.0;
	else if ((radians > 359.9) && (radians < 360.1))
		radians = 0.0;
	else if ((radians > 89.9) && (radians < 90.1))
		radians = 90.0;
	else if ((radians > 179.9) && (radians < 180.1))
		radians = 180.0;
	else if ((radians > 269.9) && (radians < 270.1))
		radians = 270.0;
	else if ((radians > -90.1) && (radians < -89.9))
		radians = -90.0;
	else if ((radians > -180.1) && (radians < -179.9))
		radians = -180.0;
	else if ((radians > -270.1) && (radians < -269.9))
		radians = -270.0;
	else if ((radians > -360.1) && (radians < -359.9))
		radians = 0.0;
	return radians;
}

CLayer *CDWGImport::GetCorrectLayer(PAD_LAY adlayer)
{
	if(gLayer)
		return gLayer;
	for(int i=0;i<m_pDoc->GetLayersCount();i++)
	{
		CLayer *pLayer=m_pDoc->GetLayer(i);
		if(pLayer->m_strName==adlayer->name)
			return pLayer;
	}
	return m_pDoc->GetLayer(0);
}

short CDWGImport::GetDWGColor (short entDWGColorIndex, PAD_LAY adlayer)
{
	static short sLastDWGColorIndex = -1;
	static short sLastMacDraftColorIndex = 1; //2;
	
	if (entDWGColorIndex == AD_COLOR_BYBLOCK)
	{
		sLastDWGColorIndex = -1;
		sLastMacDraftColorIndex = gBlockColor;
		return gBlockColor;
	}
	else if (entDWGColorIndex == AD_COLOR_BYLAYER)
	{
		if (Abs(adlayer->color) == sLastDWGColorIndex)
			return sLastMacDraftColorIndex;
		else
			sLastDWGColorIndex = Abs(adlayer->color); // Because negated if layer is hidden ( 'off' )
	}
	else // normal color
	{
		if (entDWGColorIndex == sLastDWGColorIndex)
			return sLastMacDraftColorIndex;
		else
			sLastDWGColorIndex = entDWGColorIndex;
	}
	if (sLastDWGColorIndex == 255) // white
		sLastMacDraftColorIndex = 0; //1;
	else
	switch (sLastDWGColorIndex)
	{
		case 7:  /* ACAD White if background is of dark color, else it is black */
			sLastMacDraftColorIndex = 1; //2;
			break;
		default:
		{
			COLORREF cr=RGB(g_clut_32000[sLastDWGColorIndex][1]>>8,
				g_clut_32000[sLastDWGColorIndex][2]>>8,
				g_clut_32000[sLastDWGColorIndex][3]>>8);
							
			sLastMacDraftColorIndex = m_pDoc->m_Palette.GetNearestPaletteIndex(cr);				 

		}
	}
	return sLastMacDraftColorIndex;
}

short getColLnW(short pAcadLnW)
{
	short   vColLnW;

  	if ( pAcadLnW <= kLnW1 )
		vColLnW = kLnW1;
  	else if ( pAcadLnW <= kLnW2 )
		vColLnW = kLnW2;
  	else if ( pAcadLnW <= kLnW3 )
		vColLnW = kLnW3;
  	else if ( pAcadLnW <= kLnW5 )
		vColLnW = kLnW5;
  	else if ( pAcadLnW <= kLnW7 )
		vColLnW = kLnW7;
  	else if ( pAcadLnW <= kLnW9 )
		vColLnW = kLnW9;
 	else if ( pAcadLnW <= kLnW11 )
		vColLnW = kLnW11;
  	else
		vColLnW = kLnW13;

	return vColLnW;
} /* getColLnW */

void CDWGImport::SetObjectAttributes(CMDShape* theShape,PAD_ENT_HDR adenhd,PAD_LAY adlayer,bool toFill)
{
	assert (handle);
	assert (theShape);
	assert (adenhd);
	assert (adlayer);

	short colorIndex = GetDWGColor (adenhd->entcolor, adlayer);
	//colorIndex=2;
	
	PenModel penModel=theShape->GetPenModel();

	double thickness;
	AD_OBJHANDLE* lineTypeHandle;
	if(adenhd->lineweight<0) // use layers line weight and line type
	{
		thickness = adlayer->lineweight;
		lineTypeHandle = &adlayer->linetypeobjhandle;
	}
	else
	{
		thickness = adenhd->lineweight;
		lineTypeHandle = &adenhd->entltypeobjhandle;
	}
	thickness *= g100mmDWGScale;
	penModel.m_nWidth = (uchar)getColLnW((short)thickness);

	AD_LTYPE lineType;
	if(adSeekLinetype(handle,*lineTypeHandle,&lineType))
	{
		if (lineType.numlinetypesegs) // dashed in some way
		{
			switch (lineType.numlinetypesegs)
			{
				case 1:
					break;
					
				case 2:
				case 3:
				{
					// must be dash-space but how long a dash
					// 1 = 1 dash length
					// 2 = 2 dash lengths
					// 3 = 4 dash lengths
					// 4 = 8 dash lengths
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.7)
						penModel.m_nStyle=1;
					else
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.8)
						penModel.m_nStyle=2;
					else
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.9)
						penModel.m_nStyle=3;
					else
						penModel.m_nStyle=4;
					break;
				}
				case 4:
				case 5:
				{
					// must be dash dot
					// 5 = 1 dash dot length
					// 6 = 2 dash dot lengths
					// 7 = 4 dash dot lengths
					// 8 = 8 dash dot lengths
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.7)
						penModel.m_nStyle=5;
					else if ((lineType.segdata [0].val) < lineType.patternlength * 0.8)
						penModel.m_nStyle=6;
					else if ((lineType.segdata [0].val) < lineType.patternlength * 0.9)
						penModel.m_nStyle=7;
					else
						penModel.m_nStyle=8;
					break;
				}
				case 6:
				case 7:
				{
					// must be dash dot dot
					// 9 = 1 dash dot dot length
					// 10 = 2 dash dot dot lengths
					// 11 = 4 dash dot dot lengths
					// 12 = 8 dash dot dot lengths
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.7)
						penModel.m_nStyle=9;
					else
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.8)
						penModel.m_nStyle=10;
					else
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.9)
						penModel.m_nStyle=11;
					else
						penModel.m_nStyle=12;
					break;
				}
				case 8:
				{
					// must be dash dot dot dot
					// 13 = 1 dash dot dot dot length
					// 14 = 2 dash dot dot dot lengths
					// 15 = 4 dash dot dot dot lengths
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.7)
						penModel.m_nStyle=13;
					else
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.8)
						penModel.m_nStyle=14;
					else
					if ((lineType.segdata [0].val) < lineType.patternlength * 0.9)
						penModel.m_nStyle=15;
					break;
				}
				default:
				{
					if (lineType.numlinetypesegs > 8) // lots of dash dots
						penModel.m_nStyle=15;
				}
			}
		}
	}
	
	FillModel fillModel;

	if (toFill)
	{
		fillModel.m_nFillType=kFillColor;
		fillModel.m_crColor=PALETTEINDEX(colorIndex);
	}
	else
	{
		penModel.m_crColor=PALETTEINDEX(colorIndex);
		fillModel.m_nFillType=kNoFill;
		fillModel.m_crColor=PALETTEINDEX(0);
		//fillModel.m_nIndex=0;
	}

	if (adEntIsInvisible (adenhd->entflags))
	{
		fillModel.m_nFillType = kNoFill;
		penModel.m_nPenType = kPenInvis;
		penModel.m_nIndex = kNoneIndex;
	}

	theShape->SetPenModel(penModel);
	theShape->SetFillModel(fillModel);
}

bool CDWGImport::IterateInsert( AD_OBJHANDLE blockheaderobjhandle)
{
	AD_VMADDR entlist=adEntityList(handle,blockheaderobjhandle);
	adStartEntityGet (entlist);

	short returnval;
	do
	{					 
		AD_ENT_HDR adenhd;
		AD_ENT aden;

		if (!(returnval=adGetEntity(entlist,&adenhd,&aden)))
			continue;
		if(!DoEntity ( &adenhd, &aden))
			return false;
	}			
	while (returnval == 1);
	return true;
}

bool CDWGImport::AddObject(CLayer *layer,CMDShape *shapeHdl)
{
	assert (layer);
	assert (shapeHdl);

	shapeHdl->SetShapeDatum(kDefaultDatum);

	if (sCurGroup)
	{
		assert (shapeHdl != sCurGroup);
		sCurGroup->AddMember(shapeHdl);
	}
	else
		layer->m_ShapeList.AddTail(shapeHdl);

	return true;
}

bool CDWGImport::RemoveObject(CLayer *layer,CMDShape *shapeHdl)
{
	POSITION pos=layer->m_ShapeList.Find(shapeHdl);
	if(pos)
	{
		shapeHdl->DeleteLinks(&layer->m_LinkVertexList);
		layer->m_ShapeList.RemoveAt(pos);
		delete shapeHdl;
		return true;
	}
	return false;
}

bool CDWGImport::AddGroupObject (CLayer *layerHdl,CMDGroupShape *groupShape)
{
	assert (layerHdl);
	assert (groupShape);
	if (!layerHdl)
		return false;
	if (!groupShape)
		return false;
	
	long vNumMem = groupShape->GetMembersCount();
	assert (vNumMem >= 0);

	if (vNumMem)
	{
		groupShape->CalculateExtents (); // re-calculate the extents
		if (sCurGroup)
		{
			assert (groupShape != sCurGroup);
			sCurGroup->AddMember(groupShape);
		}
		else
			layerHdl->m_ShapeList.AddTail(groupShape);
		
		return true;
	}
	else
		delete groupShape;
	return false;
}

void LoadoMeter(short percent)
{
	if (s_dlgProgress.CheckCancelButton())
	{
		gUserCanceled=true;
		adStopLoading();
	}
	else
	{
		s_dlgProgress.SetPos(percent);
	}

	TRACE("Load meter function error # %d\n", percent);
}

bool CDWGImport::DoImport(const char *szPath, double sUnits, bool bMetric)
{
	//open file
	s_dlgProgress.Create();
	CString sTitle;
	sTitle.LoadString(IDS_READINGDWG);
	s_dlgProgress.SetWindowText(sTitle);
	s_dlgProgress.ShowWindow(SW_SHOW);
	s_dlgProgress.SetRange(0,100);
	adSetAd2LoadometerFn(LoadoMeter);
	handle=adLoadFile((void*)szPath,AD_PRELOAD_ALL,0);
	adSetAd2LoadometerFn(NULL);
	s_dlgProgress.DestroyWindow();

	if(!handle)
	{
		TRACE("Cannot open file\n");
		return false;
	}

	CWaitCursor wc;
	//header
	PAD_DWGHDR adhd=adHeaderPointer (handle);
	if (!adhd) return false;

	double minX = adhd->extmin [ sXCoord ] ;
	double minY = adhd->extmin [ sYCoord ] ;
	double maxX = adhd->extmax [ sXCoord ] ;
	double maxY = adhd->extmax [ sYCoord ] ;
	
	if ( ( Abs ( minX ) > 1E10 ) || ( Abs ( minY ) > 1E10 ) || ( Abs ( maxX ) > 1E10 ) || ( Abs ( maxY ) > 1E10 ) )
	{
		minX = adhd->limmin [ sXCoord ] ;
		minY = adhd->limmin [ sYCoord ] ;
		maxX = adhd->limmax [ sXCoord ] ;
		maxY = adhd->limmax [ sYCoord ] ;
	}
	
	if(bMetric)
	{
		// "sUnits" DWG units per metre
  		if (m_pDoc->m_nUnit< kUFracInches ) // metric
  			gDWGScale *= 100 * kMetRes ; //kColMetRes
 		else // metric drawing being converted to imperial
  		if (m_pDoc->m_nUnit == kUDecFeet) // decimal feet
  			gDWGScale *= 100 * 10 * kEngDecRes / ( 2.54 * 12 ) ; // kColEngDecRes
  		else
  			gDWGScale *= 100 * kEngFactor / 2.54 ; // kColEngFactor
	}
	else
	{
		// "sUnits" DWG units per foot
  		if (m_pDoc->m_nUnit< kUFracInches )
  			gDWGScale *= 12 * 2.54 * kMetRes ; // imperial drawing being convertred to metric
  		else
	  	if (m_pDoc->m_nUnit == kUDecFeet)
	 		gDWGScale *= 10 * kEngDecRes ; // decimal feet
	  	else
	  		gDWGScale *= 12 * kEngFactor ; // 12 for per foot 
	}

	LongRatio longRatio;
	m_pDoc->GetScaleRatio(m_pDoc->GetLayer(0)->m_nScale,longRatio);
	if( longRatio.fD==0xFFFF) // 'Automatic' scaling
	{
		double sizeX=maxX-minX;
		double sizeY=maxY-minY;
		if(Abs(sizeY)>Abs(sizeX)) sizeX=sizeY;
		m_pDoc->AutomaticScale(Abs(sizeX)*gDWGScale/sUnits);
		m_pDoc->GetScaleRatio(m_pDoc->GetLayer(0)->m_nScale,longRatio);
	}

	gDWGScale *= double (longRatio.fN) / (double (longRatio.fD)*sUnits);
	gTransform = ScaleMat3x2 ( gDWGScale , -gDWGScale ) ;
	g100mmDWGScale = gDWGScale * 72 / ( 25.4 * 100 ) ; // value in 1/100 mm ( Inaccurate but probably good enough - RPH )

	gTransform = TranslateMat3x2 (	-minX ,	-maxY ) * gTransform ;
	if (( maxX <= minX ) || ( maxY <= minY ))
	{
		gTransform [ 2 ] [ 0 ] = 0.0 ;
		gTransform [ 2 ] [ 1 ] = 0.0 ;
	}

	//layers
	{
		adStartLayerGet(handle);
		long numLayers=adNumLayers(handle);
		for(int i=0;i<numLayers;i++)
		{
			adGetLayer(handle,&adtb->lay);
			char on, frozen, vpfrozen, locked;
			adGetLayerState(handle,adtb->lay.objhandle, &on, &frozen, &vpfrozen, &locked);
			DoNewLayer(&adtb->lay,on,frozen,vpfrozen,locked,i);
		}
//		m_pDoc->SetActiveLayer(0);
		m_pDoc->m_nActiveLayer=0;
		if(m_pDoc->GetLayer(0)->m_bHidden)
		{
			m_pDoc->m_bActiveLayerHidden=true;
			m_pDoc->GetLayer(0)->m_bHidden=false;
		}
	}

	//entities
	AD_OBJHANDLE pspaceblkobjhandle;
	if(adGetBlockHandle(handle,pspaceblkobjhandle,AD_MODELSPACE_HANDLE))
	{					  
		AD_VMADDR entlist=adEntityList(handle,pspaceblkobjhandle);
		adStartEntityGet(entlist);
		short returnval;
		do
		{
			if(!(returnval=adGetEntity(entlist,adenhd,aden)))
				continue;
			if(!DoEntity(adenhd, aden))
				return false;
		}while (returnval == 1);
	}

	//blocks
	{
		bool bRes=true;
		adStartBlockheaderGet(handle);
		for (int i=0;i<adNumBlockheaders(handle)-2;i++) 
		{
			adGetBlockheader(handle,&adtb->blkh);   // gives block name, which we need
			adStartEntityGet(adtb->blkh.entitylist);
			short returnval;
			do
			{                          // read block def ent, then rest of block
				returnval=adGetEntity(adtb->blkh.entitylist,adenhd,aden);    // read next entity
				if(returnval)            // and the block has not been purged
				{
					bRes=DoEntity(adenhd,aden);
					if(!bRes)
						break;
				}
			}while (returnval && (adenhd->enttype != AD_ENT_ENDBLK));
		}
		if(!bRes)
			return false;
	}

	//
	gDoingInserts=false;
	//UnwindGroups();
	//close
	return true;
}


bool CDWGImport::ImportFile(const char *szPath, double sUnits, bool bMetric)
{
	//COpenDWG OpenDWG;
	gUserCanceled=false;
	m_bOverflowError = false;

	adAcceptR14BetaFiles();
	adConvertPlinesOnLoad(0);
	adSetupDwgRead();
	adSetupDxfRead();//??
	//structs
	adhd=new AD_DWGHDR;
	adenhd=new AD_ENT_HDR;
	aden=new AD_ENT;
	adtb=new AD_TB;
	adxd=new AD_XD;

	bool bRes=DoImport(szPath, sUnits, bMetric);
	if((bRes || gUserCanceled) && handle) adCloseFile(handle);

	if(m_bOverflowError) AfxMessageBox(IDS_SCALETOOBIG,MB_ICONERROR|MB_OK);

	delete adhd;
	delete adenhd;
	delete aden;
	delete adtb;
	delete adxd;

	return bRes;
}

bool CDWGImport::DoDWGHeader(PAD_DWGHDR adheader)
{
	double minX = adheader->extmin [ sXCoord ] ;
	double minY = adheader->extmin [ sYCoord ] ;
	double maxX = adheader->extmax [ sXCoord ] ;
	double maxY = adheader->extmax [ sYCoord ] ;
	
	if ( ( Abs ( minX ) > 1E10 )
	|| ( Abs ( minY ) > 1E10 )
	|| ( Abs ( maxX ) > 1E10 )
	|| ( Abs ( maxY ) > 1E10 ) )
		{
		minX = adheader->limmin [ sXCoord ] ;
		minY = adheader->limmin [ sYCoord ] ;
		maxX = adheader->limmax [ sXCoord ] ;
		maxY = adheader->limmax [ sYCoord ] ;
		}
	
	
	gTransform = TranslateMat3x2 (	-minX ,
									-maxY ) * gTransform ;
	
	if (( maxX <= minX ) ||
		( maxY <= minY ))
		{
		gTransform [ 2 ] [ 0 ] = 0.0 ;
		gTransform [ 2 ] [ 1 ] = 0.0 ;
		}

	return true;
}

bool CDWGImport::DoNewLayer(PAD_LAY adlayer, char on, char frozen, char viewportfrozen, char locked, unsigned long index)
{
  	if(index) // add a new layer to the document
	{
		CString str=adlayer->name;
		CLayer *pLayer=m_pDoc->AddLayer(&str);
		pLayer->m_bHidden=(on==0);
		pLayer->m_bGrayed=(frozen!=0);
	}
	else // change the existing layers info
	{
		if (*adlayer->name) // do not change if blank name
		{
			m_pDoc->GetLayer(0)->m_strName=adlayer->name;
		}
	}
	return true;
}

bool CDWGImport::DoEntity( PAD_ENT_HDR adenhd, PAD_ENT aden)
{
	AD_LAY layer;
	short ret = adSeekLayer (handle, adenhd->entlayerobjhandle, &layer);
	PAD_LAY layerPtr;
	if (!ret)
		layerPtr = 0;
	else
		layerPtr = &layer;
	bool bRet=true;

	switch(adenhd->enttype)
	{
	case AD_ENT_BLOCK:
		bRet=DoBlock(adenhd,aden,layerPtr);
		break;
	case AD_ENT_ENDBLK:
		bRet=DoEndBlk(adenhd,aden,layerPtr);
		break;
	default:
		if(!((!gDoingInserts && gGroupNesting)||
			(adEntIsInPaperspace(adenhd->entflags))||
			(adEntIsInvisible(adenhd->entflags))
			))
		{
			switch (adenhd->enttype)
			{
			case AD_ENT_LINE:
				bRet=DoLine(adenhd,layerPtr,aden->line.pt0,aden->line.pt1);
				break;
			case AD_ENT_POINT:
				bRet=DoLine(adenhd,layerPtr,aden->line.pt0,aden->line.pt0);
				break;
			case AD_ENT_CIRCLE:
				bRet=DoCircle(adenhd,layerPtr,aden->circle.pt0,aden->circle.radius);
				break;
			case AD_ENT_SHAPE:
				bRet=BadObjectAlert(IDS_DWGSHAPE);
				break;
			case AD_ENT_ELLIPSE:
				bRet=DoEllipse(adenhd,aden,layerPtr);
				break;
			case AD_ENT_SPLINE:
				bRet=DoSpline(adenhd,aden,layerPtr);
				break;
			case AD_ENT_TEXT:
				bRet=DoTEXT(adenhd,aden,layerPtr);
				break;
			case AD_ENT_ARC:
				bRet=DoArc(adenhd,aden,layerPtr);
				break;
			case AD_ENT_TRACE:
				bRet=DoTrace(adenhd,layerPtr,aden->trace.pt0,aden->trace.pt1,aden->trace.pt2,aden->trace.pt3);
				break;
			case AD_ENT_REGION:
				bRet=BadObjectAlert(IDS_DWGREGION);
				break;
			case AD_ENT_SOLID:
				bRet=DoTrace(adenhd,layerPtr,aden->solid.pt0,aden->solid.pt1,aden->solid.pt2,aden->solid.pt3);
				break;
			case AD_ENT_INSERT:
				bRet=DoInsert(adenhd,aden,layerPtr);
				break;
			case AD_ENT_ATTDEF:
				bRet=DoATTDEF(adenhd,aden,layerPtr);
				break;
			case AD_ENT_ATTRIB:
				bRet=DoATTRIB(adenhd,aden,layerPtr);
				break;
			case AD_ENT_SEQEND:
				bRet=DoSEQEND(adenhd,aden,layerPtr);
				break;
			case AD_ENT_POLYLINE:
				bRet=DoPolyline(adenhd,aden,layerPtr);
				break;
			case AD_ENT_VERTEX:
				bRet=DoVertex(adenhd,aden,layerPtr);
				break;
			case AD_ENT_LINE3D:
				bRet=DoLine(adenhd,layerPtr,aden->line3d.pt0,aden->line3d.pt1);
				break;
			case AD_ENT_FACE3D:
				bRet=DoTrace(adenhd,layerPtr,aden->face3d.pt0,aden->face3d.pt1,aden->face3d.pt2,aden->face3d.pt3);
				break;
			case AD_ENT_DIMENSION:
				bRet=DoDimension(adenhd,aden,layerPtr);
				break;
			case AD_ENT_VIEWPORT:
				// no alert for viewports
				break;
			case AD_ENT_SOLID3D:
				bRet=BadObjectAlert(IDS_DWG3DSOLID);
				break;
			case AD_ENT_RAY:
				bRet=BadObjectAlert(IDS_DWGRAY);
				break;
			case AD_ENT_XLINE:
				bRet=BadObjectAlert(IDS_DWGCONSTRUCTIONLINE);
				break;
			case AD_ENT_MTEXT:
				bRet=DoMTEXT(adenhd,aden,layerPtr);
				break;
			case AD_ENT_LEADER:
				bRet=DoLEADER(adenhd,aden,layerPtr);
				break;
			case AD_ENT_TOLERANCE:
				bRet=DoTOLERANCE(adenhd,aden,layerPtr);
				break;
			case AD_ENT_MLINE:
				bRet=BadObjectAlert(IDS_DWGMULTILINE);
				break;
			case AD_ENT_BODY:
				bRet=BadObjectAlert(IDS_DWGBODY);
				break;
			default:
				if(adenhd->enttype==adLwplineEnttype(handle))
				{
					bRet=DoLwpline(adenhd,aden,layerPtr);
				}
				else if(adenhd->enttype==adHatchEnttype(handle))
				{
					bRet=BadObjectAlert(IDS_DWGHATCH);
				}
				else if(adenhd->enttype==adImageEnttype(handle))
				{
					bRet=BadObjectAlert(IDS_DWGIMAGE);
				}
				else if(adenhd->enttype==adOle2frameEnttype(handle))
				{
					bRet=BadObjectAlert(IDS_DWGOLEOBJECT);
				}
				else //if(AD_IS_A_PROXYENT(adenhd->enttype))
				{
					bRet=DoPROXY(adenhd,aden,layerPtr);
				}
				break;
			}
		}
	}
	return (bRet && !m_bOverflowError);
}

bool CDWGImport::DoLine(PAD_ENT_HDR adenhd,PAD_LAY adlayer,double *pt0,double *pt1, double lineWidth, short endCaps, Coord offset)
{
	CLayer *pLayer=GetCorrectLayer(adlayer);
	if(!pLayer)
		return true;

	CMDLineShape *newHdl = new CMDLineShape;
	if(!newHdl)
		return false;

	newHdl->SetShapePoint(0,GetLongPoint(pt0));
	LongPoint p = GetLongPoint (pt1) ;
	p.x += offset ; // Special, for extra bit on 'leaders' next to the text
	newHdl->SetShapePoint(1,p);

	newHdl->CalculateExtents();

	SetObjectAttributes(newHdl,adenhd,adlayer);

	PenModel penModel = newHdl->GetPenModel();
	if (lineWidth)
	{
		double scaledWidth = lineWidth * gDWGScale;
		penModel.m_nWidth = (UCHAR)getColLnW((short)scaledWidth);
	}
	penModel.m_nEndCaps = (UCHAR)endCaps ;
	newHdl->SetPenModel (penModel);

	AddObject(pLayer,newHdl);

	return true;
}

//version 2
//DON'T know how to transform to version 3
bool CDWGImport::DoCircle(PAD_ENT_HDR adenhd,PAD_LAY adlayer, double* pt0, const double& radius)
{
	CLayer *pLayer=GetCorrectLayer(adlayer);
	if(!pLayer)
		return true;

	CMDCircle *newHdl=new CMDCircle(CMDCircle::kRadius);

	LongPoint p=GetLongPoint(pt0);
	newHdl->SetShapePoint(0,p);
	
	p.x+=GetScaledValue(radius * gXScale);
	newHdl->SetShapePoint(1,p);

	newHdl->CalculateExtents();

	SetObjectAttributes(newHdl,adenhd,adlayer);

	AddObject(pLayer,newHdl);

	return true;
} 

bool CDWGImport::Do3PtCircle(PAD_ENT_HDR adenhd,PAD_LAY adlayer, double* pt0, double* pt1, double* pt2)
{
	CLayer *pLayer=GetCorrectLayer(adlayer);
	if(!pLayer)
		return true;

	CMDCircle *newHdl=new CMDCircle(CMDCircle::kThreePoint);
	newHdl->SetShapePoint(0,GetLongPoint(pt0));
	newHdl->SetShapePoint(1,GetLongPoint(pt1));
	newHdl->SetShapePoint(2,GetLongPoint(pt2));
	
	newHdl->CalculateExtents();

	SetObjectAttributes(newHdl,adenhd,adlayer);

	AddObject(pLayer,newHdl);

	return true;
}

void DxfCalcArcPoints ( LongPointPtr	centerPt,
						LongPoint				rad ,
						Angle					startAng,
						Angle					endAng,
						LongPointPtr			p1,
						LongPointPtr			p2)
{
	Angle						midAngle;

	midAngle = mtFixAngle ((mtFixAngle(endAng - startAng) >>1) + startAng);

	p1->x = centerPt->x + mtFixMulDbl (rad.x, mtCos (startAng) );
	p1->y = centerPt->y + mtFixMulDbl (rad.y, mtSin (startAng) );

	p2->x = centerPt->x + mtFixMulDbl (rad.x, mtCos (endAng) );
	p2->y = centerPt->y + mtFixMulDbl (rad.y, mtSin (endAng) );

} /* calcArcPoints */

bool CDWGImport::Arc(PAD_ENT_HDR adenhd,double *center,double r,double stangle,double endangle,PAD_LAY adlayer)
{
	CLayer *pLayer=GetCorrectLayer(adlayer);
	if(!pLayer)
		return true;

	double scaledRadius = gXScale;
	if(scaledRadius < 0) // negative scale, make positive
		scaledRadius = -scaledRadius;
	if(gYScale < 0) // negative YScale
	{
		if (scaledRadius < -gYScale)
			scaledRadius = -gYScale;
	}
	else if (gYScale > scaledRadius)// positive YScale
		scaledRadius = gYScale;

	scaledRadius*=r;
	Coord radius=GetScaledValue(scaledRadius);

	double stang,endang;
//	if (gXScale * gYScale < 0)
	if (!IsMirrored ( gTransform ))
	{
		stang=RadiansToDegrees (endangle);
		endang=RadiansToDegrees (stangle);
	}
	else
	{
		stang=RadiansToDegrees (stangle);
		endang=RadiansToDegrees (endangle);
	}

	LongPoint pt0,pt1,ptCenter;
	ptCenter=GetLongPoint(center);
	DxfCalcArcPoints(&ptCenter,LongPoint(radius,radius),Angle(stang),Angle(endang),&pt0,&pt1);

	CMDArcShape *newHdl=new CMDArcShape(CMDArcShape::kRadius);
	newHdl->m_nArcDirection=AD_COUNTERCLOCKWISE;
	newHdl->SetShapePoint(0,ptCenter);
	newHdl->SetShapePoint(1,pt0);
	newHdl->SetShapePoint(2,pt1);
	SetObjectAttributes(newHdl,adenhd,adlayer);
	newHdl->CalculateExtents();
	AddObject(pLayer,newHdl);

	return true;
}

bool CDWGImport::DoArc(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	return Arc(adenhd,aden->arc.pt0,aden->arc.radius,aden->arc.stang,aden->arc.endang,adlayer);
}

bool CDWGImport::DoBlock(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	if(gDoingInserts)
	{
		if(gAddOrigin)
		{
		
			// gTransform gets restored in DoInsert
			gTransform = TranslateMat3x2 (	-aden->block.base [ sXCoord ] ,
												-aden->block.base [ sYCoord ] ) * gTransform ;
				
			gAddOrigin=false;
		}
		return true;
	}
	gGroupNesting++;
	return true;
}

bool CDWGImport::DoEndBlk(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	if(gDoingInserts)
		return true;
	if(gGroupNesting)
		gGroupNesting--;
	return true;
}

static unsigned char gBlockHeaderObjHandle [ 8 ] = {0, 0, 0, 0, 0, 0, 0, 0};

bool CDWGImport::DoInsert(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	CMDGroupShape* origGroup = sCurGroup;
	long i;
	bool same = true;
	for (i = 0; i < 8; ++i)
	{
		if (aden->insert.blockheaderobjhandle [i] != gBlockHeaderObjHandle [i])
		{
			same = false;
			break;
		}
	}
	if (same)
		return true; // stops recursion
	
	//LayerHdl oldLayer = gLayer;
	unsigned long oldLayerNesting = gGroupNesting;

	LMatrix3x2 oldTransform = gTransform ;

	double oldXScale = gXScale;
	double oldYScale = gYScale;
	double oldRotation = gRotation;
	unsigned char oldBlockHeaderObjHandle [ 8 ];
	for (i = 0; i < 8; ++i)
		oldBlockHeaderObjHandle [i] = gBlockHeaderObjHandle [i];

	CLayer *gLayer = GetCorrectLayer (adlayer); // no need to test gLayer for nil as will return gLayer if not
	if (!gLayer)
		return true;
	CLayer *curLayer = gLayer;
	
	AD_ENT_HDR entityHeader;
	AD_ENT entity;

	if(!adSeekEntity (handle, 0, aden->insert.blockheaderobjhandle, &entityHeader, &entity) || (entityHeader.enttype != AD_ENT_BLOCK))
	{
		entity.block.base[sXCoord] = 0.0;
		entity.block.base[sYCoord] = 0.0;
	}

	//if (aden->insert.attribsfollow) {}
	

//	LongPoint pt=GetLongPoint(aden->insert.pt0); // must do before updating the globals

	gRotation += aden->insert.rotang*kRadsToDeg;
	double* scale = &aden->insert.xscale; // luckily they come in the correct order
	gXScale*=scale [sXCoord];
	gYScale*=scale [sYCoord];


	gTransform =	ScaleMat3x2 ( scale [ sXCoord ] , scale [ sYCoord ] )
				  * RotateMat3x2 ( -aden->insert.rotang )
				  * TranslateMat3x2 (	aden->insert.pt0 [ sXCoord ] ,
										aden->insert.pt0 [ sYCoord ] ) * gTransform ;
			
	
	// got to copy all the data as aden is a common pointer used for all objects and will be destroyed by IterateInsert
	Coord rowDist = GetScaledValue (aden->insert.rowdist * gXScale);
	Coord colDist = GetScaledValue (aden->insert.coldist * gYScale);
	for (i = 0; i < 8; ++i)
		gBlockHeaderObjHandle [i] = aden->insert.blockheaderobjhandle [i];
	long numCols = aden->insert.numcols;
	long numRows = aden->insert.numrows;

// set up new group
	CMDGroupShape* shapeHdl = new CMDGroupShape ();

	//shapeHdl->IMDGroupShape (0);?
	sGroupList.AddTail(shapeHdl);
	//shapeHdl->SetOwnerGroup (sCurGroup); ???
	sCurGroup = shapeHdl; // so objects get added to this group

	short oldColor = gBlockColor; 
	gBlockColor = GetDWGColor (adenhd->entcolor, adlayer);
	sGroupColorList.AddTail(gBlockColor);
	
// read the group objects
	bool oldDoingInsert = gDoingInserts;
	gDoingInserts = true;
	++gGroupNesting;
	gAddOrigin = true;

	bool bRet=IterateInsert( gBlockHeaderObjHandle);

	--gGroupNesting;
	gDoingInserts = oldDoingInsert;
	
	assert (gGroupNesting == oldLayerNesting);
	gGroupNesting = oldLayerNesting;

	sCurGroup = 0;
	sGroupList.RemoveTail();
	if(!sGroupList.IsEmpty())
		sCurGroup=sGroupList.GetTail();

	assert (sCurGroup == origGroup);
	sCurGroup = origGroup;

	gBlockColor = 1; //2;
	sGroupColorList.RemoveTail();
	if(!sGroupColorList.IsEmpty())
		gBlockColor=sGroupColorList.GetTail();
	assert (gBlockColor == oldColor);
	gBlockColor = oldColor;
	
	assert (gLayer == curLayer);
	gLayer = curLayer;
	
// now insert the group with the correct offset
	if ((numCols <= 1) && (numRows <= 1))
	{
		AddGroupObject (gLayer, shapeHdl);
	}
	else
	{
		Coord xOffset = FixedZero;
		for (i = 0; i < numCols; ++i)
		{
			Coord yOffset = FixedZero;
			for (long j = 0; j < numRows; ++j)
			{
				CMDGroupShape* newShape=(CMDGroupShape*)(shapeHdl->CreateCopy());
				if(AddGroupObject (gLayer, newShape))
					newShape->DoShapeOffset(LongPoint(xOffset, yOffset));

				yOffset += rowDist;
			}
			xOffset += colDist;
		}

		delete shapeHdl;
	}

	gGroupNesting = oldLayerNesting;
	sCurGroup	= origGroup;
	//gLayer		= oldLayer;
	gXScale		= oldXScale;
	gYScale		= oldYScale;
	gRotation	= oldRotation;
	gTransform = oldTransform;

	for (i = 0; i < 8; ++i)
		gBlockHeaderObjHandle [i] = oldBlockHeaderObjHandle [i];
	return bRet;
}

bool CDWGImport::DoATTDEF(PAD_ENT_HDR adenhd, PAD_ENT aden, PAD_LAY adlayer)
{
	CMDText* textObject = NULL;
	bool bRet;
	bRet = DoMacDraftText(adenhd, aden->attdef.pt0, aden->attdef.pt0, aden->attdef.defaultval, aden->attdef.tdata.height,
							aden->attdef.tdata.justification, aden->attdef.tdata.vertalignment,
							aden->attdef.tdata.rotang, aden->attdef.tdata.oblique != 0.0, adlayer,&textObject ) ;
	if ( bRet && textObject )
	{
		CLayer *layer = GetCorrectLayer (adlayer);
		DWGAttributeLink link ;
		link.AttrDefObject = textObject ;
		link.AttrDefLayer = layer ;
		memcpy(&link.tag ,&aden->attdef.tag , 513 ) ;
//		gDWGAttributeLinks.Append ( link ) ;
	}
							
	return bRet; 
}

bool CDWGImport::DoATTRIB (PAD_ENT_HDR adenhd, PAD_ENT aden, PAD_LAY adlayer)
{
	for ( long i = 0 ; i < gDWGAttributeLinks.GetSize() ; ++i )
	{
		DWGAttributeLink link = gDWGAttributeLinks [ i ] ;
		if (!strncmp(link.tag,aden->attrib.tag,513))
		{
			gDWGAttributeLinks.RemoveAt( i ) ;
			long length=strlen(aden->attrib.attval);
			if ( length == 0 )
			{
/*				CMDShape* ownerGroup = link.AttrDefObject->GetOwnerGroup () ;
				if ( ownerGroup )
				{
					link.AttrDefObject->SetOwnerGroup ( nil ) ;
					short number = ownerGroup->GetNumGroupMembers ();
					for ( long k = number - 1 ; k >= 0 ; --k )
					{
						if ( ownerGroup->GetGroupMember ( k ) == link.AttrDefObject )
						{
							for ( long n = number - 2 ; n >= k ; --n )
								ownerGroup->SetGroupMember ( n , ownerGroup->GetGroupMember ( n + 1 ) );
							ownerGroup->SetNumGroupMembers ( number - 1 ) ;
							break ;
						}
					}
				}
*/				RemoveObject ( link.AttrDefLayer , link.AttrDefObject );
				return true ;
			}
				
/*			reOpenText ( link.AttrDefLayer , link.AttrDefObject ) ;
			TEHandle newZTEH = link.AttrDefObject->GetZoomTEH();
			if (newZTEH)
			{
				::TESetSelect ( 0 , 0 , newZTEH ) ;
				::TEInsert ( & aden->attrib.attval [ 0 ] , length , newZTEH ) ;
				::TESetSelect ( length , 32767 , newZTEH ) ;
				::TEDelete ( newZTEH ) ;
				::TESetSelect ( 0 , 32767 , newZTEH ) ;
			}
			TEHandle newNZTEH = (TEHandle)link.AttrDefObject->GetNoZoomCopy();
			if (newNZTEH)
			{
				::TESetSelect ( 0 , 0 , newNZTEH ) ;
				::TEInsert ( & aden->attrib.attval [ 0 ] , length , newNZTEH ) ;
				::TESetSelect ( length , 32767 , newNZTEH ) ;
				::TEDelete ( newNZTEH ) ;
				::TESetSelect ( 0 , 32767 , newNZTEH ) ;
			}
			if ( capTextExtents ( link.AttrDefLayer , link.AttrDefObject , kCreateTables ) )
				calcLineSpacing ( link.AttrDefObject ) ;
			closeText ( link.AttrDefLayer , link.AttrDefObject ) ;
*/			return true ;
		}
	}
	
	return true; // Well actually if we got here we didn't find the tag, but not to worry
		
//	return BadObjectAlert (kAttributeDWGObject);
/*
	LayerHdl layer = GetCorrectLayer (adlayer);
	if (!layer)
		return noErr;

	aden->attrib.pt0[0];
	aden->attrib.pt0[1];
	aden->attrib.tdata.height;
	aden->attrib.attval;
	aden->attrib.tag;
	aden->attrib.attflag;
	RadiansToDegrees (aden->attrib.tdata.rotang);
	aden->attrib.tdata.widthfactor;
	aden->attrib.tdata.oblique;

	aden->attrib.tdata.shapefileobjhandle;

	aden->attrib.tdata.generationflag;
	aden->attrib.tdata.justification;
	if (aden->attrib.tdata.vertalignment != 0)
		{
		aden->attrib.tdata.vertalignment;
		}
	if ((aden->attrib.tdata.justification != 0) || (aden->attrib.tdata.vertalignment != 0))
		{
		aden->attrib.tdata.secondtextloc[0];
		aden->attrib.tdata.secondtextloc[1];
		}
	return DoReadExtendedData (adenhd);
*/
}

bool CDWGImport::DoPolyline(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	assert(!(gPolyLine||gBezierLine));

	sCurIndex = 0;
	short fitType = aden->pline.fittype;
	if (fitType == AD_PLINE_SURFTYPE_NONE || !(aden->pline.polyflag&(AD_PLINE_CURVEFIT|AD_PLINE_SPLINEFIT))) // no curve
	{
		gPolyLine = new CMDPolygonShape (CMDPolygonShape::kPolygon,0,aden->pline.polyflag & AD_PLINE_CLOSED); // assume triangle but will grow later
		SetObjectAttributes(gPolyLine, adenhd, adlayer/*,gPolyLine->IsClosed ()*/);
	}
	else if(fitType==AD_PLINE_SURFTYPE_CUBIC)
	{
		gBezierLine = new CMDCubicShape(2,false);
		SetObjectAttributes(gBezierLine, adenhd, adlayer);
	}
	else if (fitType == AD_PLINE_SURFTYPE_BEZIER)
		return BadObjectAlert(IDS_DWGBEZIER);
	else // if (fitType == AD_PLINE_SURFTYPE_QUAD)
		return BadObjectAlert (IDS_DWGQUADRATICBSPLINE);

	return true;	
}

bool CDWGImport::DoVertex(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	if (gPolyLine)
	{
		gPolyLine->SetNumShapePoints(sCurIndex+1);
		gPolyLine->SetShapePoint(sCurIndex,GetLongPoint(aden->vertex.pt0));
		++sCurIndex;
	}
	else if (gBezierLine)
	{
		if (aden->vertex.vertexflag & AD_VERTEX_SPLINEVERTEX)
		{
			//gBezierLine->SetNumShapePoints ((sCurIndex + 1) * 3);
			gBezierLine->SetNumShapePoints ((sCurIndex + 1));
			/*pt.SetPoint (GetLongPoint (aden->vertex.pt0));
			gBezierLine->SetShapeDBPoint (3 * sCurIndex, pt);*/
			//gBezierLine->SetShapePoint(3*sCurIndex,GetLongPoint (aden->vertex.pt0));
			gBezierLine->SetShapePoint(sCurIndex,GetLongPoint (aden->vertex.pt0));
			++sCurIndex;
		}
	}
	return true;
}

bool CDWGImport::DoSEQEND(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return false;
	if (gPolyLine) // end polyline
	{
		gPolyLine->CalculateExtents ();
		//relocDatum (layer, gPolyLine);??
		AddObject (layer, gPolyLine);
		gPolyLine = NULL; // do not delete it as it has been added to the list, just null it out so we know that it is now finished
	}
	else if(gBezierLine)
	{
		if (aden->pline.polyflag & AD_PLINE_CLOSED)
		{
			gBezierLine->SetClosed(true);
/*			gBezierLine->SetNumShapePoints (sCurIndex * 3);
			gBezierLine->SetShapePoint (sCurIndex * 3, gBezierLine->GetShapePoint(0));
*/		} 
		//cuRebuild (layer, gBezierLine);	 ??
		gBezierLine->CalculateExtents ();
		AddObject (layer, gBezierLine);
		gBezierLine = NULL; // do not delete it as it has been added to the list, just null it out so we know that it is now finished
	}
	sCurIndex = 0;
	
	return true;
}

bool CDWGImport::DoTrace(PAD_ENT_HDR adenhd,PAD_LAY adlayer,double *pt0,double *pt1,double *pt2,double *pt3)
{
	CMDPolygonShape *newHdl=new CMDPolygonShape(CMDPolygonShape::kPolygon,4,true);

	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return true;

	newHdl->SetShapePoint(0,GetLongPoint(pt0));
	newHdl->SetShapePoint(1,GetLongPoint(pt1));
	newHdl->SetShapePoint(2,GetLongPoint(pt3));
	newHdl->SetShapePoint(3,GetLongPoint(pt2));

	newHdl->CalculateExtents();
	//relocDatum (layer, newHdl);??
	
	SetObjectAttributes (newHdl, adenhd, adlayer, true);
	
	// now add it to the doc
	AddObject (layer, newHdl);
	
	return true;
}

//version 3 not finished
bool CDWGImport::DoEllipse(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	assert (handle);
	assert (adenhd);
	assert (aden);
	assert (adlayer);

	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return true;
	
	LongPoint centerlPt = GetLongPoint (aden->ellipse.pt0);
	
	LVector2D xy = *( (LVector2D*) &aden->ellipse.pt1offset ) * gTransform ;
	LongPoint radiilPt ;
	radiilPt.x = xy.x ;
	radiilPt.y = xy.y ;

	Angle angle = m360Ang ( ( atan2 ( xy.x , xy.y ) ) * kRadToAng + k90 ) ;

	Coord distance = Pythag (radiilPt);
	radiilPt.x = distance ;
	radiilPt.y = distance * aden->ellipse.minortomajorratio ;

	CMDShape* newShape=0;
	if ((aden->ellipse.startparam == aden->ellipse.endparam)
	|| ( Abs ( aden->ellipse.startparam - aden->ellipse.endparam ) >= pi * 2 ) )
	{
		CMDEllipse* newHdl = new CMDEllipse(CMDRectangle::kRectDiagonal);
		newShape = newHdl;

		newHdl->SetShapePoint(0,centerlPt+mtRotPoint(LongPoint(-radiilPt.x,-radiilPt.y),angle));
		newHdl->SetShapePoint(1,centerlPt+mtRotPoint(LongPoint( radiilPt.x, -radiilPt.y),angle));
		newHdl->SetShapePoint(2,centerlPt+mtRotPoint(LongPoint( radiilPt.x, radiilPt.y),angle));
		newHdl->SetShapePoint(3,centerlPt+mtRotPoint(LongPoint( -radiilPt.x, radiilPt.y),angle));
		newHdl->SetRotation ( angle );
		newHdl->CalculateExtents ();
	}
	else // can not merge these two after the new as they are derived from different roots.
	{
		angle = m360Ang ( ( -atan2 ( xy.x , -xy.y ) ) * kRadToAng + k90 ) ; // Don't ask why its different, I don't know. RPH

		CMDArcShape *newHdl=new CMDArcShape(CMDArcShape::kElliptic);
		newShape = newHdl;
		LongPoint pt0,pt1,pt2,ptCenter;

		Angle fStartAng, fEndAng;
		if ( !IsMirrored ( gTransform ) ) // The opposite way round because of different coordinate system. h == -y
		{
			fStartAng = RadiansToDegrees ( aden->ellipse.endparam ) ;
			fEndAng = RadiansToDegrees ( aden->ellipse.startparam ) ;
		} else {
			fStartAng = RadiansToDegrees ( aden->ellipse.startparam ) ;
			fEndAng = RadiansToDegrees ( aden->ellipse.endparam ) ;
		}
		if ( adenhd->extrusion [ 2 ] < 0 ) // Angles are around the extrusion vector, which should be parallel to z axis for ellipses
		{
			Angle temp = fStartAng ;
			fStartAng = k360 - fEndAng ;
			fEndAng = k360 - temp ;
		}


		LongPoint				p1, p2, p3 , rad ;
	
		DxfCalcArcPoints (&centerlPt, radiilPt, fStartAng, fEndAng,	&p1, &p2);

		if ( angle != FixedZero )
		{
			TSinCos sinCos ( angle ) ;
			p1 = mtRotPoint ( p1, sinCos , centerlPt );
			p2 = mtRotPoint ( p2, sinCos , centerlPt);
			radiilPt = mtRotPoint ( radiilPt, sinCos);
		}

		newHdl->SetShapePoint(0,centerlPt);
		newHdl->SetShapePoint(1,p1);
		newHdl->SetShapePoint(2,p2);
		newHdl->SetShapePoint(3,centerlPt+radiilPt);

		newHdl->SetRotation ( angle );
		newHdl->CalculateExtents ();
	}
	if(!newShape)
		return true;

	SetObjectAttributes(newShape, adenhd, adlayer);
	AddObject(layer, newShape);

	return true;
}

bool CDWGImport::DoSpline(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	assert (handle);
	assert (adenhd);
	assert (aden);
	assert (adlayer);

	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return true;

	register long i;
	
	double knot;
	double fitpt [3];
	long numPoints;
	CMDPolygonShape* shape;
	PAD_BLOB_CTRL bcptr = adStartBlobRead (aden->spline.ldblob);
	for (i = 0; i < aden->spline.numknots; ++i)
		adReadBlobDouble (bcptr, &knot);
		
	if (aden->spline.numfitpts)
	{
		for (i = 0; i < (short)aden->spline.numctlpts; ++i)
		{
			double ctlpt [3];
			adReadBlob3Double (bcptr, ctlpt);
			if (aden->spline.flag & AD_SPLINE_RATIONAL) // rational, has weights
			{
				double weight;
				adReadBlobDouble (bcptr, &weight);
			}
		}
		
		numPoints = aden->spline.numfitpts;
		if ((aden->spline.flag & AD_SPLINE_CLOSED))
			--numPoints;
	}
	else
		numPoints = aden->spline.numctlpts;

	if (aden->spline.numfitpts)
		shape = new CMDCubicShape(numPoints,aden->spline.flag & AD_SPLINE_CLOSED);
	else
		shape = new CMDBezierShape(numPoints);
	for (i = 0 ; i < numPoints; ++i)
	{		
		adReadBlob3Double (bcptr, fitpt);
		shape->SetShapePoint(i, GetLongPoint (fitpt));
	}
	adEndBlobRead (bcptr);

	shape->CalculateExtents();
	SetObjectAttributes(shape, adenhd, adlayer);
	AddObject(layer,shape);
	return true;
}

bool CDWGImport::DoLwpline(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	if (aden->lwpline.ldblob)
	{
		PAD_BLOB_CTRL bcptr = adStartBlobRead (aden->lwpline.ldblob);
		if (bcptr)
		{
			CLayer *layer = GetCorrectLayer (adlayer);
			if (!layer)
			{
				adEndBlobRead (bcptr);
				return true;
			}

			long width = 0;
			if (aden->lwpline.flag & AD_LWPLINE_HAS_CONSTANT_WIDTH)
				width = (long)aden->lwpline.constantwidth;

			const long hasBulges = aden->lwpline.flag & AD_LWPLINE_HAS_BULGES;
			const long hasWidths = aden->lwpline.flag & AD_LWPLINE_HAS_WIDTHS;
			
			double tempdouble[ 3 ] = { 0.0, 0.0, 0.0 };
			double tempwidth[ 3 ] = { 0.0, 0.0, 0.0 };
			double center[ 3 ] = { 0.0, 0.0, 0.0 };
			double tempbulge = 0.0;

	
			double radius = 0.0;
			double startAng = 0.0;
			double endAng = 0.0;
			double incl = 0.0;
				
			LongPoint pt;
			const double kPiOver2 = pi * 0.5;
			if ( aden->lwpline.numpoints > 2 )
			{
				// can not do it as a polygon shape, have to do it as lines and arcs
				if ( hasBulges )
				{
					bool noErr=true;

					double prevdouble[ 3 ] = { 0.0, 0.0, 0.0 };
					double startdouble [ 3 ] ;

					// Get the info for the first vertex
					adReadBlob2Double( bcptr, prevdouble );
					for ( long tt = 2 ; tt >= 0 ; --tt )
						startdouble [ tt ] = prevdouble [ tt ] ;

					if ( hasBulges )  // true
						adReadBlobDouble( bcptr, &tempbulge );

					if ( hasWidths )
						adReadBlob2Double( bcptr, tempwidth );

					long numPoints = aden->lwpline.numpoints ; // In case it gets changed. RPH
					if ( aden->lwpline.flag & AD_LWPLINE_IS_CLOSED )
						++numPoints ;
					for ( long il = 1; il < numPoints ; ++il ) // We've already read the first one.
					{
						if ( il < aden->lwpline.numpoints )
							adReadBlob2Double( bcptr, tempdouble );  // next vertex, used to calculate centre
						else
							for ( long tt = 2 ; tt >= 0 ; --tt )
								tempdouble [ tt ] = startdouble [ tt ] ; // We are closing the polyline - maybe with a bulge
						
						incl = 4.0 * atan( tempbulge );
						
						if ( incl == 0.0 )
							noErr = DoLine(adenhd, adlayer , prevdouble, tempdouble);
						else
						{
							adCalcPlineArcCenter( prevdouble, tempdouble, incl, &center[ 0 ], &center[ 1 ], &radius );
						//	startAng	= -atan2( tempdouble[ sXCoord ] - center[ sXCoord ], tempdouble[ sYCoord ] - center[ sYCoord ] ) + kPiOver2;
						//	endAng		= -atan2( prevdouble[ sXCoord ] - center[ sXCoord ], prevdouble[ sYCoord ] - center[ sYCoord ] ) + kPiOver2;
							endAng		= -atan2( tempdouble[ sXCoord ] - center[ sXCoord ], tempdouble[ sYCoord ] - center[ sYCoord ] ) + kPiOver2;
							startAng	= -atan2( prevdouble[ sXCoord ] - center[ sXCoord ], prevdouble[ sYCoord ] - center[ sYCoord ] ) + kPiOver2;
							
							if ( Abs ( endAng - startAng ) > pi )
							{
								if ( endAng > startAng )
									endAng -= pi * 2 ;
								else 
									endAng += pi * 2 ;
							}
								
							if ( Abs ( incl ) > pi )
							{
								if ( endAng > startAng )
									endAng -= pi * 2 ;
								else 
									endAng += pi * 2 ;
							}

							//endAng = startAng + incl ;
						//	if ( ( incl < 0 ) ^ ( endAng >= startAng ) )
							if ( endAng < startAng ) // Strangely, it seems all MacDraft arcs must be anti-clockwise
								noErr = Arc( adenhd,  center, radius, endAng, startAng, adlayer/*, width */);
							else
								noErr= Arc( adenhd, center, radius, startAng, endAng, adlayer/*, width */);
						}
							
						if ( !noErr )
							break;
							
						if ( il < ( numPoints - 1 ) )
						{
							prevdouble[ 0 ] = tempdouble[ 0 ];
							prevdouble[ 1 ] = tempdouble[ 1 ];
							prevdouble[ 2 ] = tempdouble[ 2 ];
							
							if ( hasBulges )  // true
								adReadBlobDouble( bcptr, &tempbulge );
								
							if ( hasWidths )
								adReadBlob2Double( bcptr, tempwidth );
						}
					}
						
					adEndBlobRead( bcptr );
					return noErr;
				}
				else //hasBulges
				{
					bool prevPoint = false;
					long totalPoints = aden->lwpline.numpoints; // do not make unsigned due to while loop test
					CMDPolygonShape* newHdl;
					do
					{
						long numPoints = totalPoints;
						//if (numPoints > kMaxPolyPoints)
						//	numPoints = kMaxPolyPoints;
						if (prevPoint)
							++numPoints;
						
						newHdl=new CMDPolygonShape(CMDPolygonShape::kPolygon,numPoints,(aden->lwpline.flag & AD_LWPLINE_IS_CLOSED)!=0);
						
						long il = 0;
						if (prevPoint)
						{
							newHdl->SetShapePoint (il, pt);
							++il;
						}
						
						for (; il < numPoints; ++il)
						{
							adReadBlob2Double (bcptr, tempdouble);
							if (hasBulges)
								adReadBlobDouble (bcptr, &tempbulge);
							if (hasWidths)
								adReadBlob2Double (bcptr, tempwidth);
							pt=GetLongPoint (tempdouble);
							newHdl->SetShapePoint (il, pt);
						}
						
						newHdl->SetClosed ((aden->lwpline.flag & AD_LWPLINE_IS_CLOSED) != 0);
						newHdl->CalculateExtents ();
						SetObjectAttributes (newHdl, adenhd, adlayer);
						
						if (width)
						{
							double scaledWidth = width * gDWGScale;
							if (scaledWidth > kMaxValue)
								scaledWidth = kMaxValue;
							PenModel penModel = newHdl->GetPenModel();
							penModel.m_nWidth=(UCHAR)getColLnW((short)scaledWidth);
							newHdl->SetPenModel (penModel);
						}
						
						// now add it to the doc
						AddObject(layer,newHdl);
						
						totalPoints-=numPoints;
						prevPoint = true;
					} while (totalPoints > 0); // will go negative if gone round twice
					
					adEndBlobRead (bcptr);
					return true;
				}
			} //points>2
			else // just a line or an arc
			{
				adReadBlob2Double(bcptr, tempdouble);
				if (hasBulges)
					adReadBlobDouble(bcptr, &tempbulge);
				if (hasWidths)
					adReadBlob2Double(bcptr, tempwidth);
				if (aden->lwpline.numpoints == 2)
					adReadBlob2Double (bcptr, tempwidth);
				else
				{
					tempwidth [0] = tempdouble [0];
					tempwidth [1] = tempdouble [1];
					tempwidth [2] = tempdouble [2];
				}
				adEndBlobRead (bcptr);
				
				if (hasBulges) // it is actually an arc
				{
					incl = 4.0 * atan (tempbulge);
					if (incl == 0.0)
						return DoLine(adenhd,adlayer,tempdouble,tempwidth /*, width*/);//!!
					else
					{
						adCalcPlineArcCenter (tempdouble, tempwidth, incl, &center [0], &center [1], &radius);
						startAng = -atan2 ( tempdouble [sXCoord] - center [sXCoord], tempdouble [sYCoord] - center [sYCoord]) + kPiOver2;
						endAng = -atan2 ( tempwidth [sXCoord] - center [sXCoord], tempwidth [sYCoord] - center [sYCoord]) + kPiOver2;	 
						if (incl < 0)
							return Arc (adenhd, center, radius, endAng, startAng, adlayer/*, width*/);
						else
							return Arc (adenhd, center, radius, startAng, endAng, adlayer/*, width*/);
					}
				}
				else
					return DoLine( adenhd, adlayer,tempdouble, tempwidth/*, width*/);
			}
		}
	}
	return false;
}

void getAngDimLineEndPt (
	LongPointPtr		startPt,
	LongPointPtr		centerPt,
	LongPointPtr		lineP1,
	LongPointPtr		lineP2,
	LongPointPtr		endPt)
{
	LongPoint			linePt;

	linePt = *startPt;

	if ( Abs (lineP1->x - lineP2->x) > Abs (lineP1->y - lineP2->y))
	{
		if (startPt->x > centerPt->x) {
			if ((lineP1->x > centerPt->x) && (lineP1->x > linePt.x))
				linePt = *lineP1;
			if ((lineP2->x > centerPt->x) && (lineP2->x > linePt.x))
				linePt = *lineP2;
		}
		else
		{
			if ((lineP1->x < centerPt->x) && (lineP1->x < linePt.x))
				linePt = *lineP1;
			if ((lineP2->x < centerPt->x) && (lineP2->x < linePt.x))
				linePt = *lineP2;
		}
	}
	else
	{
		if (startPt->y > centerPt->y)
		{
			if ((lineP1->y > centerPt->y) && (lineP1->y > linePt.y))
				linePt = *lineP1;
			if ((lineP2->y > centerPt->y) && (lineP2->y > linePt.y))
				linePt = *lineP2;
		}
		else
		{
			if ((lineP1->y < centerPt->y) && (lineP1->y < linePt.y))
				linePt = *lineP1;
			if ((lineP2->y < centerPt->y) && (lineP2->y < linePt.y))
				linePt = *lineP2;
		}
	}

	*endPt = linePt;
} /* getAngDimLineEndPt */


bool CDWGImport::DoDimension(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{

	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return true;

	DimStandard std;
	short dsType=(m_pDoc->m_nUnit&kUEnglishMask)?(kDsEnglish):(kDsMetric);
	std.FillDefaults(dsType);
	DimFormat fmt;		
	  
	unsigned char EndCap1,EndCap2;
	EndCap1=EndCap2=6;

	char flag = aden->dim.dimflag & 0x07;
	CMDAssocDim *pNewShape=0;
	if ((flag == AD_DIMTYPE_ROTATED) || (flag == AD_DIMTYPE_ALIGNED))
	{
		fmt.FillDefaults(kLinear,dsType);

		CMDLineDim* newHdl;
		LongPoint edgePt1 = GetLongPoint (aden->dim.dimlinedefpt);
		LongPoint anchorPt = GetLongPoint (aden->dim.dimtextmidpt);

		LongPoint startPt = GetLongPoint (aden->dim.defpt2);
		LongPoint endPt = GetLongPoint (aden->dim.defpt3);

		if (endPt.y == edgePt1.y || startPt.y == edgePt1.y)
			newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kVert);
		else if (endPt.x == edgePt1.x || startPt.x == edgePt1.x)
			newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kHoriz);
		else
		{
			LongPoint edgePt2;
			edgePt2.x = edgePt1.x + (anchorPt.x - edgePt1.x) * FixedTwo;
			edgePt2.y = edgePt1.y + (anchorPt.y - edgePt1.y) * FixedTwo;

			if (edgePt1.y == edgePt2.y) // horizontal
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kHoriz);
			else if (edgePt1.x == edgePt2.x) // vertical
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kVert);
			else
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,(flag == AD_DIMTYPE_ROTATED)?CMDLineDim::kPerp:CMDLineDim::kSlope,mtAngle(startPt,endPt));
		}

		newHdl->SetShapePoint(0,startPt);
		newHdl->SetShapePoint(1,endPt);
		newHdl->SetShapePoint(2,anchorPt);
		pNewShape=newHdl;
	}
	else if ((flag == AD_DIMTYPE_RADIUS) || (flag == AD_DIMTYPE_DIAMETER))
	{
		fmt.FillDefaults(kRadial,dsType);

		bool bDiameter=(flag==AD_DIMTYPE_DIAMETER);

		//LongPoint ptCenter=bDiameter ?GetLongPoint(aden->dim.dimlinedefpt):
		LongPoint ptEnd=GetLongPoint(aden->dim.defpt4);
		LongPoint ptCenter=bDiameter?Average(GetLongPoint(aden->dim.dimlinedefpt),ptEnd):GetLongPoint(aden->dim.dimlinedefpt);
		Coord radius;
		radius=Pythag(ptCenter-ptEnd);
		CMDRadialDim *pDim=new CMDRadialDim(std,fmt,EndCap1,EndCap2,
			bDiameter,ptCenter,
			radius,
			GetLongPoint(aden->dim.dimtextmidpt));
		pNewShape=pDim;
	}
	else if ((flag == AD_DIMTYPE_ANGULAR) || (flag == AD_DIMTYPE_ANGULAR3PT))
	{
		fmt.FillDefaults(kAngular,dsType);

		LongPoint lp1, lp2, lp3, lp4, refPt1, refPt2, centerPt;
		if (flag == AD_DIMTYPE_ANGULAR3PT)
		{
			lp1 = GetLongPoint (aden->dim.defpt2);
			lp3 = GetLongPoint (aden->dim.defpt3);
			lp2 = lp4 = GetLongPoint (aden->dim.defpt4); // center point
		}
		else
		{									
			lp1 = GetLongPoint (aden->dim.defpt2);
			lp2 = GetLongPoint (aden->dim.defpt3);
			lp3 = GetLongPoint (aden->dim.dimlinedefpt);
			lp4 = GetLongPoint (aden->dim.defpt4);
		}

		if (!guLongLnLnPt (lp1, lp2, lp3, lp4, &centerPt))
		{
			refPt1 = lp1;
			refPt2 = lp3;
			if (Abs (lp2.x - refPt2.x) > Abs (refPt1.x - refPt2.x))
				refPt1 = lp2;
			if (Abs (lp4.x - refPt1.x) > Abs (refPt1.x - refPt2.x))
				refPt2 = lp4;
			centerPt = Average (refPt1, refPt2);
		}
		else
		{
			getAngDimLineEndPt (&lp1, &centerPt, &lp1, &lp2, &refPt1);
			getAngDimLineEndPt (&lp3, &centerPt, &lp3, &lp4, &refPt2);
		}
		
		CMDAngularDim *pDim=new CMDAngularDim(std,fmt,6,6,true);
		pDim->SetShapePoint(0,refPt1);
		pDim->SetShapePoint(1,refPt2);
		pDim->SetShapePoint(2,GetLongPoint(aden->dim.dimtextmidpt));
		pDim->SetShapePoint(12,centerPt);

		pNewShape=pDim;
	}	
	else
		return BadObjectAlert (IDS_DWGDIMENSION);

	if(pNewShape)
	{
		{
			CDocMapper mapper(m_pDoc);
			pNewShape->ReCalc(&mapper);
		}
		CPCDraftView* pView=NULL;
		POSITION pos=m_pDoc->GetFirstViewPosition();
		if(pos) pView=(CPCDraftView*)m_pDoc->GetNextView(pos); // 1 view assumed
		if(pView) pNewShape->UpdateDependentShapes(pView);
/*		{
			CDC cdc;
			cdc.CreateCompatibleDC(NULL);
			CDocMapper mapper(m_pDoc);
			CDraftContext dc(&cdc,&mapper);
			pNewShape->m_pDimStr->SetValues(&dc,aden->dim.dimtext);
		}
*/
		SetObjectAttributes (pNewShape, adenhd, adlayer);
		pNewShape->CalculateExtents();
		AddObject(layer,pNewShape);
	}
	else
		return BadObjectAlert (IDS_DWGDIMENSION);

	return true;

}

int CDWGImport::CreateText(const char* text,CStyledText &stText,CStyledTextStyle Style)
{
	int nCount=0;
	
	const char* textEnd = text ;
	while ( *textEnd != 0 )
		++textEnd ;
	--textEnd ;

	bool styleAllowedAnywhere = false ;
	long styled = 0 ;
	CList<CStyledTextStyle,CStyledTextStyle&> textParamStack;
	if ( ( *text == '{') && ( *textEnd == '}' ) )
	{
		++text ;
		--textEnd ;
		styled = 1 ;
	}

	while ( text <= textEnd )
	{
		while ( *text == '}' && styleAllowedAnywhere && styled > 0 )
		{
//			InsertTextChunk ( newZTEH , newNZTEH , insertPtr , insertLen , settings ) ;
//			insertLen = 0 ;

			--styled ;
			Style = textParamStack.RemoveTail();
			// N.B. textParamStack.GetCount should be same value as styled

			++text ;
		}

		if ( *text == '{' && styleAllowedAnywhere )
		{
			++text ;
			if (*text != '\\')
				--text ;
			else
			{
				++styled ;
				textParamStack.AddTail(Style);
			}
		}

		if ('\\' == *text )
		{
			++text;
			switch (*text)
			{
			case 'A': // I don't know what it means, but there are lots off blank string with just \A1; - RPH
				++text;
				if ( *text == '1' )
				{
					++text;
					if ( *text == ';' )
						styleAllowedAnywhere = true ;
				}
				while (*text++ != ';'); // read chars until ';'
				--text ; // So we don't skip over the final null character after end of switch. - RPH
				break;

			case 'I': // end style ?
			case 'i':
				Style.m_LogFont.lfUnderline=0;
				break;

			case 'H': // font height
			case 'h':
				++text;
				while (*text++ != ';');	// read chars until ';'
				--text;
				break;

			case 'S': // Could this be superior/inferior ?
			case 's':
				++text;
				while ( *text != '#' && *text != ';' )
				{
					stText.Add(*text++,Style);
					nCount++;
				}

				if ( *text == '#' )		
				{
					stText.Add('/',Style);
					nCount++;
					text++;
				}
					
				while ( *text != ';' )
				{
					stText.Add(*text++,Style);
					nCount++;
				}

				break;

			case 'u': // unicode eg \U+xxxx
			case 'U':
				++text;
				if (*text == '+') // could not work out if there is a + or not, so skip if there
					++text;
				// next 4 chars are the unicode value for the char
				++text; // skip next 4 chars
				++text;
				++text; // only 3 ++ as last one is just before the continue below
				break;

			case 'l':
			case 'L': // underline ?
				Style.m_LogFont.lfUnderline=1;
				break;

			case 'P': // paragraph
			case 'p':
				stText.Add('\n',Style);
				break;

			case '\\':
			case '{':
			case '}':
				stText.Add(*text,Style);
				nCount++;
				break;

			case 'F': // ?? diffent format ?? what does '//Fcomplex.shx;' mean ??? RPH
			case 'f':
				{
					if(styled == 0) break;

					++text ;
					CString fontName ;
					while ( ( text < textEnd ) && ( *text != ';' ) && ( *text != '|' ) )
					{
						fontName+=*text;
						++text ;
					}
					strncpy(Style.m_LogFont.lfFaceName,fontName,LF_FACESIZE);
						
					if ( *text != ';' )
						++text ;
					while ( ( text < textEnd ) && ( *text != ';' ) )
					{
						switch ( *text )
						{
						case 'b' :
							++text;
							Style.m_LogFont.lfWeight=(*text++ == '0')?FW_NORMAL:FW_BOLD;
							break;
						case 'i' :
							++text;
							Style.m_LogFont.lfItalic=(*text++ == '0')?0:1;
							break;
						case 'c' : // Not tested - no sample file
							++text;
							//Style.m_LogFont.??=(*text++ == '0')?0:1;
							++text ;
							break;
						}
						++text ;
					}
					break;
				}
			} //switch
			++text; // skip char
		}
		else if (*text == '%') // meta character or single %?
		{
			++text;
			if (*text != '%')
			{
				// only single % so is correct char	
				stText.Add('%',Style);
				nCount++;
				continue;
			} 
			 //*text=='%'
			 // definitely second meta character
			++text;
			switch (*text)
			{
			case 'u':
			case 'U':
				Style.m_LogFont.lfUnderline=1; //!Style.m_LogFont.lfUnderline;
				break;
			case 'd': // degrees symbol
			case 'D':
				stText.Add('・,Style);
				nCount++;
				break;
			case 'p': // + or - symbol
			case 'P':
				//stText.Add('ｱ',Style); Dennis
#ifdef _JAPANESE
				stText.Add('±', Style);
#else
				stText.Add('ｱ', Style);
#endif
				nCount++;
				break;
			case 'c': // ｯ symbol
			case 'C':
				stText.Add('n',Style);
				nCount++;
				break;
			case '%': // % symbol
				stText.Add('%',Style);
				nCount++;
				break;
			case '0': // special character of number
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': 	
				{
				// 127 is ・symbol
				short num = *text++ - '0';
				num *= 10;
				num += *text++ - '0';
				num *= 10;
				num += *text - '0'; // do not increment as is incremented at end
				if (num == 127)
					stText.Add('・,Style);//*p++ = '・;
				else
					stText.Add((char)num,Style);//*p++ = (char)num;
				}
				nCount++;
				break;
			} // switch
			++text; // skip char
		}//if
		else
		{
			stText.Add(*text++,Style);
			nCount++;
		}
	}//while
	return nCount;
}

void CDWGImport::InitText (short entcolor, double textHeight, PAD_LAY adlayer, TextParams& mySettings)
{
	textHeight *= kTextScaleFactor;
	if (textHeight < 0)
		textHeight = 9.0;
	textHeight *= gDWGScale * gYScale;
	if (textHeight < 1.0)
		textHeight = 1.0;

	CWindowDC hDC(NULL);
	mySettings.m_lf.lfHeight=-MulDiv((int)textHeight,GetDeviceCaps(hDC,LOGPIXELSY),72);
}

bool CDWGImport::DoMacDraftText( PAD_ENT_HDR adenhd,double* pt0, double* rotPt,char* text,double &height,long justification,long vertalignment,double &angle, bool italicText,PAD_LAY adlayer, CMDText** object/*=NULL */)
{
	assert (handle);
	assert (adenhd);
	assert (adlayer);
	assert (pt0);
	assert (text);

	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return true;
	
	TextParams mySettings;
	InitText (adenhd->entcolor, height, adlayer, mySettings);

	if(justification == AD_TEXT_JUST_CENTER	|| justification == AD_TEXT_JUST_MIDDLE )
		mySettings.m_nJust=kAlignCenter;
	else if (justification == AD_TEXT_JUST_RIGHT)
		mySettings.m_nJust=kAlignRight;
	else
		mySettings.m_nJust=kAlignLeft;

	if ( vertalignment == AD_TEXT_VALIGN_MIDDLE	|| justification == AD_TEXT_JUST_MIDDLE )
		mySettings.m_nVAlign = kAlignMiddle;
	else if (vertalignment == AD_TEXT_VALIGN_TOP)
		mySettings.m_nVAlign = kAlignTop;
	else // AD_TEXT_VALIGN_BASELINE or AD_TEXT_VALIGN_BOTTOM
		mySettings.m_nVAlign = kAlignBottom;

	CStyledText stText;
	CStyledTextStyle stsText;
	stsText.m_LogFont=mySettings.m_lf;
	stsText.m_TextColor=RGB(g_clut_32000[adenhd->entcolor][1]>>8,
				g_clut_32000[adenhd->entcolor][2]>>8,
				g_clut_32000[adenhd->entcolor][3]>>8);
	if(CreateText(text,stText,stsText))
	{
		CMDText *newHdl=new CMDText();
		newHdl->SetTextParams(mySettings);
		newHdl->SetText(stText);

		LongPoint boxSize;
		{
			CWindowDC cdc(NULL);
			CDocMapper mapper(m_pDoc);
			CDraftContext dc(&cdc,&mapper,m_pDoc);
			CRect rc;
			dc.CalcBoundRect(stText,mySettings,rc);
			boxSize.x=rc.right-rc.left+6;
			boxSize.y=rc.bottom-rc.top+6;
		}

		LongPoint lPt = GetLongPoint (pt0);
//		lPt = mtRotPoint ( lPt , TSinCos ( RadiansToDegrees ( angle ) ) , GetLongPoint ( rotPt ) ) ;
		LongRect viewRect;
		viewRect.left = viewRect.right = lPt.x;
		viewRect.top = viewRect.bottom = lPt.y;

		if ( mySettings.m_nJust == kAlignCenter)
		{
			Coord x = boxSize.x / FixedTwo ;
			viewRect.left -= x ;
			viewRect.right += x ;
		}
		else if ( mySettings.m_nJust == kAlignRight )
			viewRect.left -= boxSize.x ;
		else
			viewRect.right += boxSize.x ;

		if ( mySettings.m_nVAlign == kAlignMiddle )
		{
			Coord y = boxSize.y / FixedTwo ;
			viewRect.top -= y ;
			viewRect.bottom += y ;
		}
		else if ( mySettings.m_nVAlign == kAlignBottom )
			viewRect.top -= boxSize.y ;
		else
			viewRect.bottom += boxSize.y ;

		for(int i=0;i<4;i++)
			newHdl->SetShapePoint(i,viewRect.GetCorner(i));
		
		SetObjectAttributes (newHdl, adenhd, adlayer);

		Angle aAngle=RadiansToDegrees (angle); 
		for(i=0;i<4;i++)
			newHdl->SetShapePoint(i,mtRotPoint(newHdl->GetShapePoint(i),aAngle,lPt));
		newHdl->SetRotation(aAngle);

		newHdl->CalculateExtents();
		AddObject (layer, newHdl);

		Coord x = FixedZero ;
		Coord y = FixedZero; //FixedTwo ;

		LongRect bounds = newHdl->GetExtents ();
/*		if (mySettings.m_nJust== kAlignCenter) // need to move half text width over
			x = ( bounds.right - bounds.left ) * FixedHalf ;
		else if (mySettings.m_nJust== kAlignRight) // need to move text width over
			x = bounds.right - bounds.left;

		if (vertalignment == AD_TEXT_VALIGN_MIDDLE)
			y = FixedHalf * (bounds.bottom - kBotRightMargin - lPt.y); //mySettings.fTextSize ;
		else if (vertalignment == AD_TEXT_VALIGN_BOTTOM)
			y = bounds.bottom - kBotRightMargin - lPt.y ;
		else*/
		if( mySettings.m_nVAlign == kAlignBottom && vertalignment == AD_TEXT_VALIGN_BASELINE )
		{
			CWindowDC hDC(NULL);
			CFont* pOldFont,*pFont = new CFont();
			pFont->CreateFontIndirect(&stsText.m_LogFont);
			pOldFont=hDC.SelectObject(pFont);
			TEXTMETRIC tm;
			hDC.GetTextMetrics(&tm);
			hDC.SelectObject(pOldFont);
			delete pFont;

			y = kBotRightMargin + short ( tm.tmDescent ); // +/- ? 
		}

		if ( x != FixedZero | y != FixedZero )
			newHdl->DoShapeOffset(LongPoint(x, y));

		if (object) *object = newHdl;
	}
	return true;
}

bool CDWGImport::DoTEXT(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	double definingPoint [ 3 ] ;
	if ( (aden->text.tdata.justification == AD_TEXT_JUST_LEFT && aden->text.tdata.vertalignment == AD_TEXT_VALIGN_BASELINE)
	|| aden->text.tdata.justification == AD_TEXT_JUST_ALIGNED
	|| aden->text.tdata.justification == AD_TEXT_JUST_FIT)
	{
		definingPoint [ 0 ] = aden->text.pt0 [ 0 ] ;
		definingPoint [ 1 ] = aden->text.pt0 [ 1 ] ;
	}
	else
	{
		definingPoint [ 0 ] = aden->text.tdata.secondtextloc [ 0 ] ;
		definingPoint [ 1 ] = aden->text.tdata.secondtextloc [ 1 ] ;
	}

//	if ( aden->text.tdata.vertalignment != AD_TEXT_VALIGN_BOTTOM )
//		definingPoint [ 1 ] = aden->text.pt0 [ 1 ] ;
//	else
//		definingPoint [ 1 ] = aden->text.tdata.secondtextloc [ 1 ] ;
		
	definingPoint [ 2 ] = aden->text.pt0 [ 2 ] ;

	return DoMacDraftText ( adenhd, definingPoint , aden->text.pt0 , aden->text.textstr, aden->text.tdata.height,
							aden->text.tdata.justification, aden->text.tdata.vertalignment, aden->text.tdata.rotang, aden->text.tdata.oblique != 0.0, adlayer);
}

bool CDWGImport::DoMTEXT(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	CLayer *layer = GetCorrectLayer (adlayer);
	if (!layer)
		return true;

	TextParams mySettings;

	InitText (adenhd->entcolor, aden->mtext.ht, adlayer, mySettings);

	switch (aden->mtext.attachpt)
	{
		case AD_MTEXT_ATTACH_TOPLEFT:
		case AD_MTEXT_ATTACH_MIDDLELEFT:
		case AD_MTEXT_ATTACH_BOTTOMLEFT:
			mySettings.m_nJust=kAlignLeft;
			break;
		case AD_MTEXT_ATTACH_TOPCENTER:
		case AD_MTEXT_ATTACH_MIDDLECENTER:
		case AD_MTEXT_ATTACH_BOTTOMCENTER:
			mySettings.m_nJust=kAlignCenter;
			break;
		case AD_MTEXT_ATTACH_TOPRIGHT:
		case AD_MTEXT_ATTACH_MIDDLERIGHT:
		case AD_MTEXT_ATTACH_BOTTOMRIGHT:
			mySettings.m_nJust=kAlignRight;
			break;
		default:
			mySettings.m_nJust=kAlignLeft;
			break;
	}

	switch (aden->mtext.attachpt)
	{
		case AD_MTEXT_ATTACH_TOPLEFT:
		case AD_MTEXT_ATTACH_TOPCENTER:
		case AD_MTEXT_ATTACH_TOPRIGHT:
			mySettings.m_nVAlign=kAlignTop;
			break;
		case AD_MTEXT_ATTACH_MIDDLELEFT:
		case AD_MTEXT_ATTACH_MIDDLECENTER:
		case AD_MTEXT_ATTACH_MIDDLERIGHT:
			mySettings.m_nVAlign=kAlignMiddle;
			break;
		case AD_MTEXT_ATTACH_BOTTOMLEFT:
		case AD_MTEXT_ATTACH_BOTTOMCENTER:
		case AD_MTEXT_ATTACH_BOTTOMRIGHT:
			mySettings.m_nVAlign=kAlignBottom;
			break;
		default:
			mySettings.m_nVAlign=kAlignTop;
			break;
	}

	CStyledText stText;
	CStyledTextStyle stsText;
	stsText.m_LogFont=mySettings.m_lf;
	stsText.m_TextColor=RGB(g_clut_32000[adenhd->entcolor][1]>>8,
				g_clut_32000[adenhd->entcolor][2]>>8,
				g_clut_32000[adenhd->entcolor][3]>>8);;


	CString strText;
	if (aden->mtext.ldblob == AD_VMNULL) // text held in aden->mtext.textstr
		strText=aden->mtext.textstr;
	else
	{
		PAD_BLOB_CTRL bcptr = 0;
		bcptr = adStartBlobRead (aden->mtext.ldblob);
		while(adReadMtextBlock (bcptr, aden->mtext.textstr))
		{
			strText+=aden->mtext.textstr;
		}
		adEndBlobRead (bcptr);
	}

	if(CreateText (strText,stText,stsText))
	{
		LongPoint boxSize;
		CMDText::TextType nType=CMDText::kTextAutoSize;
		if (aden->mtext.boxht || aden->mtext.boxwid) // one of these can be zero
		{
			nType=CMDText::kTextUserSize;
			if (aden->mtext.boxwid)
				boxSize.x = GetScaledValue (aden->mtext.boxwid * gXScale);
			else
			{
				// work out the width from textPtr??
				boxSize.x = GetScaledValue (aden->mtext.boxht * gXScale);
			}
				
			if (aden->mtext.boxht)
				boxSize.y = GetScaledValue (aden->mtext.boxht * gYScale);
			else
				boxSize.y = GetScaledValue (aden->mtext.boxwid * gYScale);
		}

		{
			CWindowDC cdc(NULL);
			CDocMapper mapper(m_pDoc);
			CDraftContext dc(&cdc,&mapper,m_pDoc);
			CRect rc;
			if(nType==CMDText::kTextAutoSize)
			{
				dc.CalcBoundRect(stText,mySettings,rc);
				boxSize.x=rc.right-rc.left+6;
				boxSize.y=rc.bottom-rc.top+6;
			}
			else
			{
				dc.CalcBoundRect(stText,mySettings,rc,false,boxSize.x.GetLong()-6);
				if(boxSize.y<(rc.bottom-rc.top+6))
					boxSize.y=(rc.bottom-rc.top+6);
			}
		}

		LongPoint lPt = GetLongPoint (aden->mtext.pt0);
		LongRect viewRect;
		viewRect.left = viewRect.right = lPt.x;
		viewRect.top = viewRect.bottom = lPt.y;
		if ( mySettings.m_nJust == kAlignCenter)
		{
			Coord x = boxSize.x / FixedTwo ;
			viewRect.left -= x ;	
			viewRect.right += x ;	
		}
		else if ( mySettings.m_nJust == kAlignRight )
			viewRect.left -= boxSize.x ;	
		else
			viewRect.right += boxSize.x ;	

		if ( mySettings.m_nVAlign == kAlignMiddle )
		{
			Coord y = boxSize.y / FixedTwo ;
			viewRect.top -= y ;	
			viewRect.bottom += y ;	
		}
		else if ( mySettings.m_nVAlign == kAlignBottom )
			viewRect.top -= boxSize.y ;	
		else
			viewRect.bottom += boxSize.y ;

		CMDText *newHdl=new CMDText(nType);
		newHdl->SetTextParams(mySettings);
		newHdl->SetText(stText);

		for(int i=0;i<4;i++)
			newHdl->SetShapePoint(i,viewRect.GetCorner(i));
		SetObjectAttributes (newHdl, adenhd, adlayer);
		
		//xdir!!
		if ( Abs ( aden->mtext.xdir [ sXCoord ] ) == 1.0 )
			aden->mtext.xdir [ sYCoord ] = 0.0 ; // The are some E minus 17 numbers which come through, probably erroneously
		else
		if ( Abs ( aden->mtext.xdir [ sYCoord ] ) == 1.0 )
			aden->mtext.xdir [ sXCoord ] = 0.0 ; // The are some E minus 17 numbers which come through, probably erroneously
		
		LVector2D textVec ;
		textVec.x = aden->mtext.xdir [ sXCoord ] ;
		textVec.y = aden->mtext.xdir [ sYCoord ] ;
		 
		textVec *= gTransform ;
		
		Angle angle = m360Ang ( atan2 ( -textVec.y , textVec.x ) * kRadToAng ) ;
		for(i=0;i<4;i++)
			newHdl->SetShapePoint(i,mtRotPoint(newHdl->GetShapePoint(i),angle,lPt));
		newHdl->SetRotation(angle);

		newHdl->CalculateExtents();
		AddObject(layer, newHdl);
	}
	return true;
}

bool CDWGImport::DoLEADER ( PAD_ENT_HDR adenhd, PAD_ENT aden, PAD_LAY adlayer)
{
	if ( ( aden->leader.annohandle [ 0 ] == 0 ) && aden->leader.ldblob )
	{
		// It seems these  are always black/white, but they don't come through that way. Override.
		short oldColour = adenhd->entcolor ;
		adenhd->entcolor = 7 ;

		bool bRet = true;
		bool first = true ;
		
		PAD_BLOB_CTRL bcptr = adStartBlobRead (aden->leader.ldblob);
		double pt0 [3];
		double pt1 [3];
		adReadBlob3Double (bcptr, pt0);
		for ( long i = aden->leader.numpoints - 1 ; i > 0 ; --i )
		{
			adReadBlob3Double (bcptr, pt1);
			
			if (bRet)
				bRet = DoLine(adenhd , adlayer , pt0 , pt1 , 0 , ( aden->leader.arrowheadon && first ) ? kVArrow : 0 ) ;
			
			for ( long j = 2 ; j >= 0 ; --j )
				pt0 [ j ] = pt1 [ j ] ;
			first = false ;
		}
			
		adEndBlobRead (bcptr);

		if ( bRet )
			bRet = DoLine(adenhd , adlayer, pt0 , pt1 , 0 , 0 , Coord ( short ( 4 ) ) ) ;
		
		adenhd->entcolor = oldColour ;
		
		return bRet;
	}
	return BadObjectAlert (IDS_DWGLEADER);
}

bool CDWGImport::DoTOLERANCE( PAD_ENT_HDR adenhd, PAD_ENT aden, PAD_LAY adlayer)
{
	return BadObjectAlert(IDS_DWGTOLERANCE);
}

bool CDWGImport::DoPROXY(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
{
	if (aden->image.grblob != AD_VMNULL)
		return ReadGrBlobInfo( adenhd, aden->proxyent.grblob, adlayer);
		
	return true;
}

bool CDWGImport::ReadGrBlobInfo( PAD_ENT_HDR adenhd, AD_VMADDR grblob, PAD_LAY adlayer)
{
	if (!grblob)
		return true;
	
	double pt [3];
	int i;
	//unsigned long j;
	//long nummeshedges, nummeshfaces, nummeshvertices, numshelledges, numshellfaces, rows, cols;
	//double *vptr;
	//long *lptr;

	bool bRet=true;
	PAD_BLOB_CTRL bcptr = adStartBlobRead (grblob);
	adStartGrblobDataRead (bcptr);
	AD_GR_DATA grdata;
	while (adReadGrblobData (bcptr, &grdata))
	{
		if (grdata.grtype == AD_GRENT_CIRCLE)
		{
			bRet = DoCircle (adenhd, adlayer, grdata.grbody.circle.pt0, grdata.grbody.circle.radius);
			// grdata.grbody.circle.normal[0];
			// grdata.grbody.circle.normal[1];
			// grdata.grbody.circle.normal[2];
		}
		else
		if (grdata.grtype == AD_GRENT_CIRCLE3PT)
		{
			bRet = Do3PtCircle (adenhd, adlayer, grdata.grbody.circle3pt.pt0, grdata.grbody.circle3pt.pt1, grdata.grbody.circle3pt.pt2);
		}
		else
		if (grdata.grtype == AD_GRENT_CIRCULARARC)
		{
			bRet = BadObjectAlert(IDS_DWGCIRCULARARC);
/*
			// center
			grdata.grbody.circulararc.pt0[0];
			grdata.grbody.circulararc.pt0[1];
			grdata.grbody.circulararc.pt0[2];
			// radius
			grdata.grbody.circulararc.radius;
			// normal
			grdata.grbody.circulararc.normal[0];
			grdata.grbody.circulararc.normal[1];
			grdata.grbody.circulararc.normal[2];
			// start vector
			grdata.grbody.circulararc.startvector[0];
			grdata.grbody.circulararc.startvector[1];
			grdata.grbody.circulararc.startvector[2];
			
			grdata.grbody.circulararc.sweepangle;
			grdata.grbody.circulararc.arctype;
*/
		}
/*		else
		if (grdata.grtype == AD_GRENT_CIRCULARARC3PT)
		{
			err = DoMacDraft3PtArc (handle, list, adenhd, grdata.grbody.circulararc3pt.pt0, grdata.grbody.circulararc3pt.pt1, grdata.grbody.circulararc3pt.pt2, adlayer);
			// grdata.grbody.circulararc.arctype;
		}
*/		else
		if ((grdata.grtype == AD_GRENT_POLYLINE) ||	// open not on a plane
			(grdata.grtype == AD_GRENT_POLYGON))	// closed and on a plane
		{
			CLayer *pLayer=GetCorrectLayer(adlayer);
			if(!pLayer)
				return true;
			else
			{
				long numPoints;
				if (grdata.grtype == AD_GRENT_POLYGON)
					numPoints = grdata.grbody.polygon.numpts;
				else
					numPoints = grdata.grbody.pline.numpts;
				CMDPolygonShape* poly = new CMDPolygonShape (CMDPolygonShape::kPolygon, numPoints, grdata.grtype == AD_GRENT_POLYGON);

				for (i = 0; i < numPoints; ++i)
				{
					adReadGrblobVertexPt (bcptr, pt);
					poly->SetShapePoint (i, GetLongPoint(pt));
				}
				poly->CalculateExtents ();
//				relocDatum (layer, poly);
				
				SetObjectAttributes ( poly, adenhd, adlayer, grdata.grtype == AD_GRENT_POLYGON);
					
				// now add it to the doc
				AddObject (pLayer, poly);
			}
		}
		else
		if (grdata.grtype == AD_GRENT_MESH)
		{
			bRet = BadObjectAlert (IDS_DWGMESH);
		/*
			PAD_GR_MESH md;
			md = &grdata.grbody.mesh;
			rows = md->rows;
			cols = md->cols;
			md->edgecolorid = NULL;
			md->edgelayerid = NULL;
			md->edgelinetypeid = NULL;
			md->edgegsmarker = NULL;
			md->edgevisibility = NULL;
			md->facecolorid = NULL;
			md->facelayerid = NULL;
			md->facegsmarker = NULL;
			md->facenormal = NULL;
			md->facevisibility = NULL;
			md->vertexnormal = NULL;
			md->vertexorientation = 0L;

			md->vertex = (double *)NewPtr((short)(rows * cols) * 3 * sizeof (double));
			vptr = md->vertex;
			for (i = 0; i < (unsigned short)rows; ++i)
				{
				for (j = 0; j<(unsigned short)cols; ++j)
					{
					ReadGrblobVertexPt (bcptr, vptr);
					// vertex
					vptr[0];
					vptr[1];
					vptr[2];
					
					vptr += 3;
					}
				}

			// see if there is edge specific data
			ReadPrimFlags (bcptr, &md->edgeprimflag);

			if (adHasPrimTraits (md->edgeprimflag))
				{
				nummeshedges = (rows - 1) * cols + (cols - 1) * rows;

				if (adPrimsHaveColors (md->edgeprimflag))
					{
					md->edgecolorid = (short *)NewPtr((unsigned short)nummeshedges*sizeof(short));
					ReadPrimColors (bcptr, nummeshedges, md->edgecolorid);
					for (i = 0; i < (unsigned short)nummeshedges; ++i)
						md->edgecolorid [i];
					}
				if (adPrimsHaveLayers (md->edgeprimflag))
					{
					md->edgelayerid = (short *)NewPtr((unsigned short)nummeshedges*sizeof(short));
					ReadPrimLayerids (bcptr, nummeshedges, md->edgelayerid);
					for (i = 0; i < (unsigned short)nummeshedges; ++i)
						md->edgelayerid[i];
					}
				if (adPrimsHaveLinetypes (md->edgeprimflag))
					{
					md->edgelinetypeid = (short *)NewPtr((unsigned short)nummeshedges*sizeof(short));
					ReadPrimLinetypeids (bcptr, nummeshedges, md->edgelinetypeid);
					for (i = 0; i < (unsigned short)nummeshedges; ++i)
						md->edgelinetypeid [i];
					}
				if (adPrimsHaveMarkers (md->edgeprimflag))
					{
					md->edgegsmarker = (long *)NewPtr((unsigned short)nummeshedges*sizeof(long));
					ReadPrimMarkers (bcptr, nummeshedges, md->edgegsmarker);
					for (i = 0; i < (unsigned short)nummeshedges; ++i)
						md->edgegsmarker[i];
					}
				if (adPrimsHaveVisibilities (md->edgeprimflag))
					{
					md->edgevisibility = (long *)NewPtr((unsigned short)nummeshedges*sizeof(long));
					ReadPrimVisibilities (bcptr, nummeshedges, md->edgevisibility);
					for (i = 0; i < (unsigned short)nummeshedges; ++i)
						md->edgevisibility [i];
					}
				} // edges have data

			// see if there is face specific data
			ReadPrimFlags (bcptr, &md->faceprimflag);

			if (adHasPrimTraits (md->faceprimflag))
				{
				nummeshfaces = (rows - 1 ) * (cols - 1);

				if (adPrimsHaveColors (md->faceprimflag))
					{
					md->facecolorid = (short *)NewPtr ((unsigned short)nummeshfaces * sizeof (short));
					ReadPrimColors (bcptr, nummeshfaces, md->facecolorid);
					for (i = 0; i < (unsigned short)nummeshfaces; ++i)
						md->facecolorid [i];
					}
				if (adPrimsHaveLayers (md->faceprimflag))
					{
					md->facelayerid = (short *)NewPtr ((unsigned short)nummeshfaces * sizeof (short));
					ReadPrimLayerids (bcptr, nummeshfaces, md->facelayerid);
					for (i = 0; i < (unsigned short)nummeshfaces; ++i)
						md->facelayerid [i];
					}
				if (adPrimsHaveMarkers (md->faceprimflag))
					{
					md->facegsmarker = (long *)NewPtr((unsigned short)nummeshfaces * sizeof (long));
					ReadPrimMarkers (bcptr, nummeshfaces, md->facegsmarker);
					for (i = 0; i < (unsigned short)nummeshfaces; ++i)
						md->facegsmarker [i];
					}
				if (adPrimsHaveNormals (md->faceprimflag))
					{
					md->facenormal = (double *)NewPtr ((unsigned short)nummeshfaces * 3 * sizeof (double));
					ReadPrimNormals (bcptr, nummeshfaces, md->facenormal);
					vptr = md->facenormal;
					for (i = 0; i < (unsigned short)nummeshfaces; ++i)
						{
						vptr [0];
						vptr [1];
						vptr [2];
						vptr += 3;
						}
					}
				if (adPrimsHaveVisibilities (md->faceprimflag))
					{
					md->facevisibility = (long *)NewPtr ((unsigned short)nummeshfaces * sizeof (long));
					ReadPrimVisibilities (bcptr, nummeshfaces, md->facevisibility);
					for (i = 0; i < (unsigned short)nummeshfaces; ++i)
						md->facevisibility[i];
					}
				}

			// see if there is vertex specific data
			ReadPrimFlags (bcptr,&md->vertprimflag);
			if (adHasPrimTraits (md->vertprimflag))
				{
				nummeshvertices = rows * cols;
				if (adPrimsHaveNormals (md->vertprimflag))
					{
					md->vertexnormal = (double *)NewPtr ((unsigned short)nummeshvertices * 3 * sizeof (double));
					ReadPrimNormals (bcptr, nummeshvertices, md->vertexnormal);
					vptr = md->vertexnormal;
					for (i = 0; i < (unsigned short)nummeshvertices; ++i)
						{
						vptr[0];
						vptr[1];
						vptr[2];
						vptr += 3;
						}
					}
				if (adPrimsHaveOrientation (md->vertprimflag))
					{
					ReadPrimOrientation (bcptr, &md->vertexorientation);
					}
				}
			// now free everything
			if (md->edgecolorid)
				DisposePtr ((Ptr)md->edgecolorid);
			if (md->edgelayerid)
				DisposePtr ((Ptr)md->edgelayerid);
			if (md->edgelinetypeid)
				DisposePtr ((Ptr)md->edgelinetypeid);
			if (md->edgegsmarker)
				DisposePtr ((Ptr)md->edgegsmarker);
			if (md->edgevisibility)
				DisposePtr ((Ptr)md->edgevisibility);
			if (md->facecolorid)
				DisposePtr ((Ptr)md->facecolorid);
			if (md->facelayerid)
				DisposePtr ((Ptr)md->facelayerid);
			if (md->facegsmarker)
				DisposePtr ((Ptr)md->facegsmarker);
			if (md->facenormal)
				DisposePtr ((Ptr)md->facenormal);
			if (md->facevisibility)
				DisposePtr ((Ptr)md->facevisibility);
			if (md->vertexnormal)
				DisposePtr ((Ptr)md->vertexnormal);
			if (md->vertex)
				DisposePtr ((Ptr)md->vertex);
		*/
		} // end of mesh
		else
		if (grdata.grtype == AD_GRENT_SHELL)
		{
			bRet = BadObjectAlert (IDS_DWGSHELL);
		/*
			PAD_GR_SHELL sd;
			sd = &grdata.grbody.shell;
			sd->faceentry = NULL;
			sd->edgecolorid = NULL;
			sd->edgelayerid = NULL;
			sd->edgelinetypeid = NULL;
			sd->edgegsmarker = NULL;
			sd->edgevisibility = NULL;
			sd->facecolorid = NULL;
			sd->facelayerid = NULL;
			sd->facegsmarker = NULL;
			sd->facenormal = NULL;
			sd->facevisibility = NULL;
			sd->vertexnormal = NULL;
			sd->vertexorientation = 0L;
			sd->vertex = (double*)NewPtr ((short)(sd->numpts) * 3 * sizeof (double));
			vptr=sd->vertex;
			for (i = 0; i < (unsigned short)sd->numpts; ++i)
				{
				ReadGrblobVertexPt (bcptr, vptr);
				vptr [0];
				vptr [1];
				vptr [2];
				vptr += 3;
				}
			ReadBlobLong (bcptr, &sd->numfaceentries);
			long facecount;
			numshellfaces = facecount = numshelledges = 0L;
			sd->faceentry = (long *)NewPtr ((short)sd->numfaceentries * sizeof (long));
			ReadGrshellFaceEntries (bcptr, sd->numfaceentries, sd->faceentry);
			lptr = sd->faceentry;
			facecount = 0L;
			numshellfaces = 0L;
			for (i = 0; i < (unsigned short)sd->numfaceentries; ++i, ++lptr)
				{
				// face entry
				*lptr;
				if (facecount == 0L)
					{
					facecount = *lptr;
					if (facecount < 0L)
						facecount = -facecount;
					else
						++numshellfaces;
					numshelledges += facecount;
					}
				else
					--facecount;
				}
			// see if there is edge specific data
			ReadPrimFlags (bcptr, &sd->edgeprimflag);

			if (adHasPrimTraits (sd->edgeprimflag))
				{
				// numshelledges computed above
				if (adPrimsHaveColors (sd->edgeprimflag))
					{
					sd->edgecolorid = (short *)NewPtr ((unsigned short)numshelledges * sizeof (short));
					ReadPrimColors (bcptr, numshelledges, sd->edgecolorid);
					for (i = 0; i < (unsigned short)numshelledges; ++i)
						sd->edgecolorid [i];
					}
				if (adPrimsHaveLayers (sd->edgeprimflag))
					{
					sd->edgelayerid = (short *)NewPtr ((unsigned short)numshelledges * sizeof (short));
					ReadPrimLayerids (bcptr, numshelledges, sd->edgelayerid);
					for (i = 0; i < (unsigned short)numshelledges; ++i)
						sd->edgelayerid [i];
					}
				if (adPrimsHaveLinetypes (sd->edgeprimflag))
					{
					sd->edgelinetypeid = (short *)NewPtr ((unsigned short)numshelledges * sizeof (short));
					ReadPrimLinetypeids (bcptr, numshelledges, sd->edgelinetypeid);
					for (i = 0; i < (unsigned short)numshelledges; ++i)
						sd->edgelinetypeid [i];
					}
				if (adPrimsHaveMarkers (sd->edgeprimflag))
					{
					sd->edgegsmarker = (long *)NewPtr ((unsigned short)numshelledges * sizeof (long));
					ReadPrimMarkers (bcptr, numshelledges, sd->edgegsmarker);
					for (i = 0; i < (unsigned short)numshelledges; ++i)
						sd->edgegsmarker [i];
					}
				if (adPrimsHaveVisibilities (sd->edgeprimflag))
					{
					sd->edgevisibility = (long *)NewPtr ((unsigned short)numshelledges * sizeof (long));
					ReadPrimVisibilities (bcptr, numshelledges, sd->edgevisibility);
					for (i = 0; i < (unsigned short)numshelledges; ++i)
						sd->edgevisibility [i];
					}
				} // edges have data

			// see if there is face specific data
			ReadPrimFlags (bcptr, &sd->faceprimflag);

			if (adHasPrimTraits (sd->faceprimflag))
				{
				if (adPrimsHaveColors (sd->faceprimflag))
					{
					sd->facecolorid = (short *)NewPtr ((unsigned short)numshellfaces*sizeof (short));
					ReadPrimColors (bcptr, numshellfaces, sd->facecolorid);
					for (i = 0; i < (unsigned short)numshellfaces; ++i)
						sd->facecolorid [i];
					}
				if (adPrimsHaveLayers (sd->faceprimflag))
					{
					sd->facelayerid = (short *)NewPtr ((unsigned short)numshellfaces * sizeof (short));
					ReadPrimLayerids (bcptr, numshellfaces, sd->facelayerid);
					for (i = 0; i < (unsigned short)numshellfaces; ++i)
						sd->facelayerid [i];
					}
				if (adPrimsHaveMarkers (sd->faceprimflag))
					{
					sd->facegsmarker = (long *)NewPtr ((unsigned short)numshellfaces * sizeof (long));
					ReadPrimMarkers (bcptr, numshellfaces, sd->facegsmarker);
					for (i = 0; i < (unsigned short)numshellfaces; ++i)
						sd->facegsmarker [i];
					}
				if (adPrimsHaveNormals (sd->faceprimflag))
					{
					sd->facenormal = (double *)NewPtr ((unsigned short)numshellfaces * 3 * sizeof (double));
					ReadPrimNormals(bcptr,numshellfaces,sd->facenormal);
					vptr = sd->facenormal;
					for (i = 0; i < (unsigned short)numshellfaces; ++i)
						{
						vptr [0];
						vptr [1];
						vptr [2];
						vptr += 3;
						}
					}
				if (adPrimsHaveVisibilities (sd->faceprimflag))
					{
					sd->facevisibility = (long *)NewPtr ((unsigned short)numshellfaces * sizeof (long));
					ReadPrimVisibilities (bcptr, numshellfaces, sd->facevisibility);
					for (i = 0; i < (unsigned short)numshellfaces; ++i)
						sd->facevisibility[i];
					}
				}

			// see if there is vertex specific data
			ReadPrimFlags (bcptr, &sd->vertprimflag);
			if (adHasPrimTraits (sd->vertprimflag))
				{
				if (adPrimsHaveNormals (sd->vertprimflag))
					{
					sd->vertexnormal = (double *)NewPtr ((unsigned short)sd->numpts * 3 * sizeof (double));
					ReadPrimNormals (bcptr, sd->numpts, sd->vertexnormal);
					vptr = sd->vertexnormal;
					for (i = 0; i < (unsigned short)sd->numpts; ++i)
						{
						//vertex normal
						vptr [0];
						vptr [1];
						vptr [2];
						vptr += 3;
						}
					}
				if ( adPrimsHaveOrientation (sd->vertprimflag))
					{
					ReadPrimOrientation (bcptr, &sd->vertexorientation);
					sd->vertexorientation;
					}
				}
			// now free everything
			if (sd->edgecolorid)
				DisposePtr ((Ptr)sd->edgecolorid);
			if (sd->edgelayerid)
				DisposePtr ((Ptr)sd->edgelayerid);
			if (sd->edgelinetypeid)
				DisposePtr ((Ptr)sd->edgelinetypeid);
			if (sd->edgegsmarker)
				DisposePtr ((Ptr)sd->edgegsmarker);
			if (sd->edgevisibility)
				DisposePtr ((Ptr)sd->edgevisibility);
			if (sd->facecolorid)
				DisposePtr ((Ptr)sd->facecolorid);
			if (sd->facelayerid)
				DisposePtr ((Ptr)sd->facelayerid);
			if (sd->facegsmarker)
				DisposePtr ((Ptr)sd->facegsmarker);
			if (sd->facenormal)
				DisposePtr ((Ptr)sd->facenormal);
			if (sd->facevisibility)
				DisposePtr ((Ptr)sd->facevisibility);
			if (sd->vertexnormal)
				DisposePtr ((Ptr)sd->vertexnormal);
			if (sd->faceentry)
				DisposePtr ((Ptr)sd->faceentry);
			if (sd->vertex)
				DisposePtr ((Ptr)sd->vertex);
		*/
		} // end of shell
		else
		if (grdata.grtype == AD_GRENT_TEXT)
		{
			double zero = 0.0;
			bRet = DoMacDraftText ( adenhd, grdata.grbody.text.pt0, grdata.grbody.text.pt0, grdata.grbody.text.textstr, grdata.grbody.text.height, AD_TEXT_JUST_LEFT, AD_TEXT_VALIGN_TOP, zero, grdata.grbody.text.oblique != 0.0, adlayer);
			// grdata.grbody.text.normal[0];
			// grdata.grbody.text.normal[1];
			// grdata.grbody.text.normal[2];
			
			// grdata.grbody.text.direction[0];
			// grdata.grbody.text.direction[1];
			// grdata.grbody.text.direction[2];
			
			// grdata.grbody.text.widthfactor;
		}
		else
		if (grdata.grtype == AD_GRENT_TEXT2)
		{
			double zero = 0.0;
			bRet = DoMacDraftText ( adenhd, grdata.grbody.text2.pt0, grdata.grbody.text2.pt0, grdata.grbody.text2.textstr, grdata.grbody.text2.height, AD_TEXT_JUST_LEFT, AD_TEXT_VALIGN_TOP, zero, grdata.grbody.text2.oblique != 0.0, adlayer);

			// grdata.grbody.text2.normal[0];
			// grdata.grbody.text2.normal[1];
			// grdata.grbody.text2.normal[2];
			
			// grdata.grbody.text2.direction[0];
			// grdata.grbody.text2.direction[1];
			// grdata.grbody.text2.direction[2];
			
			// grdata.grbody.text2.raw;
			// grdata.grbody.text2.widthfactor;
			
			// grdata.grbody.text2.xdir[0];
			// grdata.grbody.text2.xdir[1];
			// grdata.grbody.text2.xdir[2];
		}
		else
		if (grdata.grtype == AD_GRENT_XLINE)
		{
			bRet = DoLine (adenhd, adlayer, grdata.grbody.xline.pt0, grdata.grbody.xline.pt1);
		}
		else
		if (grdata.grtype == AD_GRENT_RAY)
		{
			bRet = DoLine (adenhd, adlayer, grdata.grbody.ray.pt0, grdata.grbody.ray.pt1);
		}
	/*
		else
		if (grdata.grtype == AD_GRENT_SUBENT_COLOR)
			{
			// subentity color
			grdata.grbody.colorindex;
			}
		else
		if (grdata.grtype == AD_GRENT_SUBENT_LAYER)
			{
			// subentity layerindex
			grdata.grbody.layerindex;
			}
		else
		if (grdata.grtype == AD_GRENT_SUBENT_LINETYPE)
			{
			// subentity linetypeindex
			grdata.grbody.linetypeindex;
			}
		else
		if (grdata.grtype == AD_GRENT_SUBENT_MARKER)
			{
			// subentity markerindex
			grdata.grbody.markerindex;
			}
		else
		if (grdata.grtype == AD_GRENT_SUBENT_FILLON)
			{
			// subentity fillon
			grdata.grbody.fillon;
			}
	*/
		if (!bRet)
			break;
	}
	adEndBlobRead (bcptr);
	return bRet;
}

bool CDWGImport::BadObjectAlert(UINT nID)
{
	if(m_bShowBadObjectAlerts)
	{
		for(int i=0;i<m_BadObjects.GetSize();i++)
		{
			if(m_BadObjects[i]==nID)
				return true;
		}
		m_BadObjects.Add(nID);
		CString strObject;
		strObject.LoadString(nID);
		CBadDWGObjectDlg theDlg;
		theDlg.m_strText.Format(IDS_BADDWGOBJECT,strObject);
		if(IDCANCEL==theDlg.DoModal())
		{
//			gUserCanceled=true;
			return false;
		}
		if(theDlg.m_bDontShow) m_bShowBadObjectAlerts=false;
	}
	return true;
}
#endif //0
#ifndef _PERSONAL_EDITION

COpenDWGio * CPCDraftApp::dwgio()
{
	if (m_pdwgio == nullptr)
	{

      m_pdwgio = new COpenDWGio();
		
	}

	return m_pdwgio;
}
//-------------------------------------------------------------------//
int CPCDraftDoc::ImportDWG(const char *szPath, double sUnits, bool bMetric)
{
	CPCDraftApp* papp = (CPCDraftApp*)AfxGetApp();
	
	gUserCanceled=false;
	m_bOverflowError = false;
	papp->dwgio()->SetMode (sUnits, bMetric);
	if (papp->dwgio()->Import (this, szPath)) {
		if(m_bOverflowError) AfxMessageBox(IDS_SCALETOOBIG,MB_ICONERROR|MB_OK);
		LongRect extents = GetCumulativeExtents();
		if(extents.left.GetLong() < -1) extents.left = Coord(-1);
		if(extents.right.GetLong() > 4096) extents.right = Coord(4096);
		if(extents.top.GetLong() < -1) extents.top = Coord(-1);
		if(extents.bottom.GetLong() > 4096) extents.bottom = Coord(4096);

		if(!extents.IsEmpty()) // have shapes
		{
			Offset(LongPoint(-extents.left,-extents.top));
			m_NumPage.x = (extents.right.GetLong()-extents.left.GetLong()) / m_PageSize.cx + 1;
			m_NumPage.y = (extents.bottom.GetLong()-extents.top.GetLong()) / m_PageSize.cy + 1;
		}
	
		UpdateAllViews(NULL,kDocumentSizeChanged);
		UpdateFrameCounts();
		m_bComplete=true;
		UpdateAllViews(NULL,kRefresh);
		SetModifiedFlag();

		return 1;
	} else {
		return gUserCanceled?-1:0;
	}
/* Old implementation of the import*/
/*	m_bComplete=false;
	CDWGImport dwg(this);
	bool bRes=dwg.ImportFile(szPath, sUnits, bMetric);
	if(bRes)
	{
		LongRect extents;
		extents = GetCumulativeExtents();
		
		if(!extents.IsEmpty()) // have shapes
		{
			Offset(LongPoint(-extents.left,-extents.top));
			m_NumPage.x = (extents.right.GetLong()-extents.left.GetLong()) / m_PageSize.cx + 1;
			m_NumPage.y = (extents.bottom.GetLong()-extents.top.GetLong()) / m_PageSize.cy + 1;
		}
		
		UpdateAllViews(NULL,kDocumentSizeChanged);
		UpdateFrameCounts();
		m_bComplete=true;
		UpdateAllViews(NULL,kRefresh);
		SetModifiedFlag();
		return 1;
	}
	return gUserCanceled?-1:0;
*/
}
#endif
#ifndef _PERSONAL_EDITION

bool CPCDraftDoc::ExportDWG(const char* szFileName)
{
#if 1
	CPCDraftApp* papp = (CPCDraftApp*)AfxGetApp();
	//COpenDWGio cDWGio;
	gUserCanceled=false;
	return papp->dwgio()->Export (this, szFileName);

#else
	//old implementation
	char strPath[MAX_PATH+1];
	VERIFY(GetTempPath(MAX_PATH,strPath));
	char strTempFile[MAX_PATH];
	VERIFY(GetTempFileName(strPath,"dxf",0,strTempFile));

	//char *strTempFile="c:\\dxf.dxf";
	if(!ExportDXF(strTempFile))
	{
		::DeleteFile(strTempFile);
		return false;
	}

	{
		COpenDWG OpenDWG;

		adAcceptR14BetaFiles();
		adConvertPlinesOnLoad(0);
		adSetupDwgRead();
		adSetupDxfRead();//??

		s_dlgProgress.Create();
		CString sTitle;
		sTitle.LoadString(IDS_WRITINGDWG);
		s_dlgProgress.SetWindowText(sTitle);
		s_dlgProgress.GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		s_dlgProgress.GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
		s_dlgProgress.ShowWindow(SW_SHOW);
		s_dlgProgress.SetRange(0,100);
		adSetAd2LoadometerFn(LoadoMeter);
		AD_DB_HANDLE Handle=adLoadFile((void*)strTempFile,AD_PRELOAD_ALL,0);
		adSetAd2LoadometerFn(NULL);
		s_dlgProgress.DestroyWindow();
		::DeleteFile(strTempFile);
		if(!Handle)
			return FALSE;
		if(!adSaveFile(Handle,(void*)szFileName,AD_DWG,AD_ACAD25,0,6,0,0))
			return false;
		return true;
	}
#endif
}

#endif
