// OpenDWGio.cpp: implementation of the COpenDWGio class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifndef _PERSONAL_EDITION
#include <assert.h>
#include "resource.h"
#include "ProgressDlg.h"
#include <OdaCommon.h>
#include <DbDatabase.h>
#include <DbLayerTable.h>
#include <DbLayerTableRecord.h>

#include <DbSymbolTable.h>
#include <DbSymbolTableRecord.h>
#include <DbBlockTableRecord.h>
#include <DbLinetypeTableRecord.h>
//entities
#include <DbBlockReference.h>
#include <DbText.h>
#include <DbMText.h>
#include <DbEntity.h>
#include <DbDimension.h>
#include <DbTrace.h>
// curves
#include <DbCircle.h>
#include <DbEllipse.h>
#include <DbArc.h>
#include <DbLine.h>
#include <DbPoint.h>
#include <DbPolyLine.h>
#include <Db2dVertex.h>
#include <Db3dPolyLine.h>
#include <Db3dPolyLineVertex.h>
#include <DbSpline.h>
#include <DbLeader.h>
#include <DbSolid.h>
#include <DbFace.h>
#include <DbFaceRecord.h>
#include <DbRay.h>
#include <DbXline.h>
#include <DbRegion.h>
#include <DbHatch.h>
#include <DbImage.h>
#include <DbPolyFaceMesh.h>
#include <DbPolygonMesh.h>
#include <DbFcf.h>
#include <Db3dSolid.h>
#include <DbMline.h>
#include <DbFrame.h>
#include <DbBody.h>
#include <DbMInsertBlock.h>

//dimentions
#include <DbRotatedDimension.h>
#include <DbAlignedDimension.h>
#include <DbDiametricDimension.h>
#include <DbRadialDimension.h>
#include <Db3PointAngularDimension.h>
#include <Db2LineAngularDimension.h>

#include <DbTextStyleTableRecord.h>
#include <DbAttributeDefinition.h>
#include <DbAttribute.h>
//#include <DbProxyEntity.h>
#include <DbHostAppServices.h>


#include "OpenDWGio.h"
#include "ExHostAppServices.h"
#include "ExSystemServices.h"
#include "RxDynamicModule.h"
#include "OdModuleNames.h"
#include "diagnostics.h"
//#include "AECAppStaticModules.h"


#include <GeMatrix3d.h>
#include <GePoint3d.h>
#include <GeScale3d.h>
#include <GeCircArc2d.h>
#include <GeCircArc3d.h>
#include <GeEllipArc2d.h>
#include <GeEllipArc3d.h>
#include <GeExtents3d.h>

#include "Layer.h"

#include "ColUtils.h"
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

#include "BadDWGObjectDlg.h"
#include "PasswordDlg.h"
#ifndef _TOOLKIT_IN_DLL_
#include "Architecture\include\AECAppStaticModules.h" // uncomment for Architecture objects support in statically linked version
#endif


#ifndef _TOOLKIT_IN_DLL_

//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ColladaModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ExCustObjsModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ExCommandsModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(RasterExportModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(DbCommandsModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdOleItemHandlerModuleImpl);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(WinGDIModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(WinOpenGLModule);
//#if !defined(WINDIRECTX_DISABLED)
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(WinDirectXModule);
//#endif //WINDIRECTX_DISABLED
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(BitmapModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ExRasterModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModelerModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdPsPlotStyleServicesImpl);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdGripPointsModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(SvgExportModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdPlotSettingsValidatorModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(Dwf7ImportModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(TD_DynBlocksModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdRasterProcessingServicesImpl);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ExDynamicBlocksModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(AutoSurfServicesModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(DwfUnderlayModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdRecomputeDimBlockModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdCurveFunctionsModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdAveModuleImpl);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ExFieldEvaluatorModule);
////ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdTrueTypeEngineModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(TD_IdViewObjModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModlerCommandsModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdPdfExportModule);
//ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(TD_3DSolidHistoryModule);


ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdRxThreadPoolImpl); // uncomment for multicore/multicpu support in statically linked version

//ODRX_DECLARE_ARCH_STATIC_MODULES_ENTRY_POINTS(); // uncomment for Architecture objects support in statically linked version

ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdDgnModule);
ODRX_BEGIN_STATIC_MODULE_MAP()
ODRX_DEFINE_STATIC_APPLICATION(L"TG_Db", OdDgnModule)
ODRX_END_STATIC_MODULE_MAP()

#endif

#define DEFAULTCONVERTC2W(CHAR_BUFF,WCHAR_BUFF, SIZE) MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,(LPCSTR)CHAR_BUFF, (int)SIZE, WCHAR_BUFF, (int)SIZE)


/************************************************************************/
/* Simple trace function                                                */
/************************************************************************/
static void myTrace(const OdChar* debugString)
{
#ifndef NDEBUG
	OutputDebugStringW((LPCWSTR)debugString);
#endif
}

/************************************************************************/
/* Displays MessageBox for Assertion failure                            */
/************************************************************************/
static void myAssert(const char* expr, const char* fileName, int nLine)
{
	USES_CONVERSION;
	char sMsg[MAX_PATH * 3];
	sprintf(sMsg, "expression: \"%s\"\n\nfile: \"%s\"\nline: %d",
		expr, fileName, nLine);

	switch (MessageBox(NULL, A2CT(sMsg), _T("Assertion failed!"), MB_ICONWARNING | MB_ABORTRETRYIGNORE))
	{
	case IDABORT:
		exit(0);
		break;
	case IDRETRY:
		DebugBreak();
		break;
	case IDIGNORE:
	default:
		break;
	}
}

static bool myCheckAssertGroup(const char* pGroup)
{
	return !pGroup || !*pGroup; // asserts of base group are enabled only (default)
	//return true; // asserts of all groups are enabled
}

const double	kMaxValue = 32767.0;

class UserBreak
{
};

class CProgressMeter: public CProgressDlg, public OdDbHostAppProgressMeter
{
public:
	CProgressMeter () : m_nLimit (1) {
		m_nCaptionID = IDS_READINGDWG;
	};
   virtual ~CProgressMeter()
   {
      stop();

   }
  // SMK.SSA
  // virtual void start(const char* displayString = NULL);
  virtual void start(const OdString& displayString = OdString::kEmpty);
  virtual void stop();
  virtual void meterProgress();
  virtual void setLimit(int max);
private:
// implementation data
	long m_nLimit;
};

class COutProgressMeter: public CProgressMeter
{
public:
	COutProgressMeter () {
		m_nCaptionID = IDS_WRITINGDWG;
	};
};

//#ifdef _MSC_VER
//#pragma message ("Compiling password protected r18 drawings loading support...")
//#include "WinNTCrypt.cpp"
//#endif
class CHostAppServicesEx: public ExHostAppServices, public ExSystemServices
{
public:
	bool                  m_bPartial;
	bool                  m_bRecover;
	bool                  m_bLoading;
	// ODA_MT_DBIO_BEGIN
	bool                  m_bUseMTLoading;
	// ODA_MT_DBIO_END


	ODRX_USING_HEAP_OPERATORS(ExSystemServices);


	CHostAppServicesEx();
	OdDbDatabasePtr openFile(LPCTSTR lpszPathName);
	void setPartialOption(bool val) { m_bPartial = val; }
	void setRecoverOption(bool val) { m_bRecover = val; }
	// ODA_MT_DBIO_BEGIN
	void setMTLoadingOption(bool val) { m_bUseMTLoading = val; }
	// ODA_MT_DBIO_END
	virtual OdDbHostAppProgressMeter* newProgressMeter () { return &m_cProgressMeter; }
	virtual ~CHostAppServicesEx () {}
	virtual bool ttfFileNameByDescriptor(const OdTtfDescriptor& descr, OdString& fileName) {return FALSE;}
	virtual OdHatchPatternManager* patternManager() {return NULL;}
    virtual OdGsDevicePtr gsBitmapDevice(OdRxObject* pViewObj = NULL,
        OdDbBaseDatabase* pDb = NULL,
        OdUInt32 flags = 0) {
        return OdGsDevicePtr();
    }
private:
	CProgressMeter m_cProgressMeter;
	private:
#ifdef _MSC_VER
	bool getPassword(const OdString& dwgName, bool isXref, OdPassword& password)
	{
		CPasswordDlg pwdDlg;
        // SMK.SSA
		pwdDlg.m_sFileName = (LPCWSTR)dwgName;
		if (pwdDlg.DoModal()==IDOK)
		{
			password = pwdDlg.m_password;
			return true;
		}
		return false;
	}
#endif
};

class COutHostAppServicesEx: public OdDbHostAppServices2, public ExSystemServices
{
public:
	using ExSystemServices::operator new;
	using ExSystemServices::operator delete;
	virtual OdDbHostAppProgressMeter* newProgressMeter () { return &m_cProgressMeter; }
	virtual ~COutHostAppServicesEx () {}
	virtual bool ttfFileNameByDescriptor(const OdTtfDescriptor& descr, OdString& fileName) {return FALSE;}
	virtual OdHatchPatternManager* patternManager() {return NULL;}
    virtual OdGsDevicePtr gsBitmapDevice(OdRxObject* pViewObj = NULL,
        OdDbBaseDatabase* pDb = NULL,
        OdUInt32 flags = 0) {
        return OdGsDevicePtr();
    }

private:
	COutProgressMeter m_cProgressMeter;
	private:
#ifdef _MSC_VER
	bool getPassword(const OdString& dwgName, bool isXref, OdPassword& password)
	{
		CPasswordDlg pwdDlg;
        // SMK.SSA
		pwdDlg.m_sFileName = (LPCWSTR)dwgName;
		if (pwdDlg.DoModal()==IDOK)
		{
			password = pwdDlg.m_password;
			return true;
		}
		return false;
	}
#endif
};

void CProgressMeter::setLimit (int nMax)
{
	m_nLimit = nMax < 1 ? 1 : nMax;
	if (GetSafeHwnd ())
	{
		SetRange (0, m_nLimit);
	}

}


CHostAppServicesEx::CHostAppServicesEx() :
	m_bPartial(false),
	m_bRecover(false),
	m_bLoading(false),
	m_bUseMTLoading(false)
{


}
// SMK.SSA
// void CProgressMeter::start (const char* displayString/* = NULL*/)
void CProgressMeter::start(const OdString& displayString/* = OdString::kEmpty*/)
{
	Create ();
	SetRange (0, m_nLimit);
	SetStep (1);
	SetPos (0);
}

void CProgressMeter::meterProgress ()
{
	if (GetSafeHwnd ())
	{
		StepIt ();
		if (CheckCancelButton ()) throw UserBreak();
	}
}

void CProgressMeter::stop ()
{
	if (GetSafeHwnd ())
	{
		DestroyWindow ();
	}
}

bool gUserCanceled = false;
bool m_bOverflowError = false;


class oda
{
public:

	OdStaticRxObject<CHostAppServicesEx> m_cAppServices;
	OdDbDatabasePtr m_pDb;

	oda()
	{

      init();

	}

	bool init()
	{

		try
		{

			//m_cAppServices.disableProgressMeterOutput(true);
			::odrxInitialize(&m_cAppServices);
			odInitializeDbCore(&m_cAppServices);
			odInitialize(&m_cAppServices);  


#ifndef _TOOLKIT_IN_DLL_
			ODRX_INIT_STATIC_MODULE_MAP();
#endif

			m_cAppServices.setRecomputeDimBlocksRequired(false);

		}
		catch ( OdError& e)
		{
			int iCode = e.code();
         CString str;
         str = get_utf8((LPCWSTR)e.description().c_str());
			if (e.code() == eDecryptionError) {
				AfxMessageBox("Error!");
			}
			else {
				OdString str = m_cAppServices.getErrorDescription(e.code());
				CString strUtf8 = get_utf8((LPCWSTR)str.c_str());
				// SMK.SSA
				// AfxMessageBox (cAppServices.getErrorDescription(e.code()).c_str());
				AfxMessageBox(strUtf8);
			}
			if (m_pDb.get() != NULL)
			{
				m_pDb.release(); // Delete the database before odUnitialize() is called.
			}

         return false;

		}
		catch (UserBreak)
		{
			gUserCanceled = true;
			if (m_pDb.get() != NULL)
			{
				m_pDb.release(); // Delete the database before odUnitialize() is called.
			}

         return false;

		}
		catch (...)
		{
			AfxMessageBox("Unknown error during open");
			if (m_pDb.get() != NULL)
			{
				m_pDb.release(); // Delete the database before odUnitialize() is called.
			}
			
			return false;

		}

		return true;

	}

   void term()
   {

      odUninitialize();

   }

	~oda()
	{

      term();

	}

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenDWGio::COpenDWGio () : m_pCurBlock (0),
		m_gDWGScale(1),
		m_g100mmDWGScale(1),
		m_bShowBadObjectAlerts(true),
		m_nBlockColor(0)
{

	//AfxOleInit();

//#if !defined(PDFMODULEVI_ENABLED) // PDF conflicts with...
//	// This will cause OdaMfcApp to crash if it will perform illegal FP operation
//	_clear87();
//	_control87( //              off on
//				//_EM_INVALID       |
//		_EM_DENORMAL |
//		//_EM_ZERODIVIDE    |
//		//_EM_OVERFLOW      |
//		_EM_UNDERFLOW |
//		_EM_INEXACT,
//		_MCW_EM
//	);
//#endif
	//::odSetTraceFunc(myTrace);
	//::odSetAssertFunc(myAssert);
	//::odSetCheckAssertGroupFunc(myCheckAssertGroup);

	m_poda = new oda();

}


COpenDWGio::~COpenDWGio ()
{

	delete m_poda;

}


bool COpenDWGio::Import (CPCDraftDoc* pDoc, LPCTSTR strFileName)
{

	try
	{

		if (pDoc == NULL) 
		{

         return false; // no document for initialization

		}

		m_pDoc = pDoc;
		// Create a database and load the drawing into it.
		m_poda->m_pDb = m_poda->m_cAppServices.readFile (strFileName, true) ;
		
		if (m_poda->m_pDb.get () == NULL)
		{

         return false; // error or loading cancelled

		}

		DoDocument (m_poda->m_pDb); // Do something with the database.

		if(m_bOverflowError) AfxMessageBox(IDS_SCALETOOBIG,MB_ICONERROR|MB_OK);
	
		m_poda->m_pDb.release (); // Delete the database before odUnitialize() is called.
		
      if(gUserCanceled) return false;

	}
	catch (const OdError& e)
	{
      m_poda->m_cAppServices.newProgressMeter()->stop();
      int iCode = e.code();
      CString str;
      str = get_utf8((LPCWSTR)e.description().c_str());

		if(e.code() == eDecryptionError) {
			AfxMessageBox("Error!");
		} else {
			OdString str = m_poda->m_cAppServices.getErrorDescription(e.code());
			CString strUtf8 = get_utf8((LPCWSTR) str.c_str());
		// SMK.SSA
		// AfxMessageBox (cAppServices.getErrorDescription(e.code()).c_str());
		AfxMessageBox (strUtf8);
		}
		if (m_poda->m_pDb.get() != NULL)
		{
			m_poda->m_pDb.release (); // Delete the database before odUnitialize() is called.
		}
      return false;

	}
	catch(UserBreak)
	{
      m_poda->m_cAppServices.newProgressMeter()->stop();
      gUserCanceled = true;
		if (m_poda->m_pDb.get() != NULL)
		{
			m_poda->m_pDb.release (); // Delete the database before odUnitialize() is called.
		}
      return false;

	}
	catch(...)
	{
      m_poda->m_cAppServices.newProgressMeter()->stop();
      AfxMessageBox ("Unknown error during open");
		if (m_poda->m_pDb.get() != NULL)
		{
			m_poda->m_pDb.release (); // Delete the database before odUnitialize() is called.
		}
      return false;

	}
	return true;
}


bool COpenDWGio::Export (CPCDraftDoc* pDoc, LPCTSTR strFileName)
{

   try
	{
	

		m_pDoc = pDoc;
		char strPath[MAX_PATH+1];

		VERIFY(GetTempPath(MAX_PATH,strPath));
		char strTempFile[MAX_PATH];
		VERIFY(GetTempFileName(strPath,"dxf",0,strTempFile));

		if(!m_pDoc->ExportDXF(strTempFile))
		{

         ::DeleteFile(strTempFile);

			return false;

		}

      OdDbDatabasePtr pDb = m_poda->m_cAppServices.readFile(strTempFile, false);

		if (pDb.get () == NULL)
		{

         return false; // error or loading cancelled

		}

		pDb->writeFile(odSystemServices()->createFile (strFileName, Oda::kFileWrite, Oda::kShareDenyNo, Oda::kCreateAlways), 
            OdDb::kDwg, OdDb::kDHL_CURRENT);
		//	OdDb::kDwg, OdDb::vAC12);

		::DeleteFile (strTempFile);

	
		pDb.release (); // Delete the database before odUnitialize() is called.
		
      if(gUserCanceled) return false;

	}
	catch (OdError& e)
	{
      m_poda->m_cAppServices.newProgressMeter()->stop();
		// SMK.SSA
		// AfxMessageBox (cAppServices.getErrorDescription(e.code()).c_str());
		AfxMessageBox (m_poda->m_cAppServices.getErrorDescription(e.code()));
		
      return false;

	}
	catch(UserBreak)
	{
      m_poda->m_cAppServices.newProgressMeter()->stop();
		gUserCanceled = true;

      return false;

	}
	catch(...)
	{
		
      m_poda->m_cAppServices.newProgressMeter()->stop();
      AfxMessageBox ("Unknown error during open");
		
      return false;

	}
	return true;

}


void COpenDWGio::SetTransformation (OdDbBlockTableRecord* pEntityTable)
{
	double minX = -100;
	double minY = -100;
	double maxX = 700;
	double maxY = 500;
	
	OdGeExtents3d ext;
	//OdDbExtents ext;
	//ext.addBlockExt(pEntityTable);
	/*OdResult err = pEntityTable->getGeomExtents(ext);
	if(err == eOk) {
	//if(ext.isValidExtents()) {
		minX = ext.minPoint().x;
		minY = ext.minPoint().y ;
		maxX = ext.maxPoint().x ;
		maxY = ext.maxPoint().y ;
	} else */{
		OdDbObjectIteratorPtr iter = pEntityTable->newIterator();
		iter->start();
		if(!iter->done()) 
		{
			OdDbEntityPtr pEntity = iter->entity ();
			OdResult err = pEntity->getGeomExtents (ext);				
			while ((err != eOk) && (!iter->done())) {
				iter->step();
				if(iter->done()) break;
				pEntity = iter->entity ();
				err = pEntity->getGeomExtents (ext);	
			}

			if( err == eOk) {	
				OdGePoint3d maxPt = ext.maxPoint ();
				OdGePoint3d minPt = ext.minPoint ();
				for (; !iter->done(); iter->step())
				{
					pEntity = iter->entity ();
					err = pEntity->getGeomExtents (ext);
					if( err == eOk) {	
						if(ext.maxPoint().x > maxPt.x) maxPt.x = ext.maxPoint().x;
						if(ext.maxPoint().y > maxPt.y) maxPt.y = ext.maxPoint().y;
						if(ext.minPoint().x < minPt.x) minPt.x = ext.minPoint().x;
						if(ext.minPoint().y < minPt.y) minPt.y = ext.minPoint().y;
					}
				}
				minX = minPt.x;
				minY = minPt.y ;
				maxX = maxPt.x ;
				maxY = maxPt.y ;
			}		  	
		}
	}
/*	if ( ( Abs ( minX ) > 1E10 ) || ( Abs ( minY ) > 1E10 ) || ( Abs ( maxX ) > 1E10 ) || ( Abs ( maxY ) > 1E10 ) )
	{
		minX = adhd->limmin [ sXCoord ] ;
		minY = adhd->limmin [ sYCoord ] ;
		maxX = adhd->limmax [ sXCoord ] ;
		maxY = adhd->limmax [ sYCoord ] ;
	}*/
	
	if(m_bMetric)
	{
		// "sUnits" DWG units per metre
  		if (m_pDoc->m_nUnit < kUFracInches ) // metric
  			m_gDWGScale *= 100 * kMetRes ; //kColMetRes
 		else // metric drawing being converted to imperial
  		if (m_pDoc->m_nUnit == kUDecFeet) // decimal feet
  			m_gDWGScale *= 100 * 10 * kEngDecRes / ( 2.54 * 12 ) ; // kColEngDecRes
  		else
  			m_gDWGScale *= 100 * kEngFactor / 2.54 ; // kColEngFactor
	}
	else
	{
		// "sUnits" DWG units per foot
  		if (m_pDoc->m_nUnit< kUFracInches )
  			m_gDWGScale *= 12 * 2.54 * kMetRes ; // imperial drawing being convertred to metric
  		else
	  	if (m_pDoc->m_nUnit == kUDecFeet)
	 		m_gDWGScale *= 10 * kEngDecRes ; // decimal feet
	  	else
	  		m_gDWGScale *= 12 * kEngFactor ; // 12 for per foot 
	}

	LongRatio longRatio;
	m_pDoc->GetScaleRatio (m_pDoc->GetLayer(0)->m_nScale, longRatio);
	if (longRatio.fD == 0xFFFF) // 'Automatic' scaling
	{
		double sizeX = maxX-minX;
		double sizeY = maxY-minY;
		if(Abs(sizeY) > Abs(sizeX)) sizeX = sizeY;
		m_pDoc->AutomaticScale (Abs(sizeX)* m_gDWGScale/ m_sUnits);
		m_pDoc->GetScaleRatio (m_pDoc->GetLayer(0)->m_nScale, longRatio);
	}

	m_gDWGScale *= double (longRatio.fN) / (double (longRatio.fD) * m_sUnits);
	m_g100mmDWGScale = m_gDWGScale * 72 / ( 25.4 * 100 ) ; // value in 1/100 mm ( Inaccurate but probably good enough - RPH )

	m_cTransform = ScaleMat3x2 ( m_gDWGScale , -m_gDWGScale ) ;
	m_cTransform = TranslateMat3x2 (-minX ,	-maxY) * m_cTransform ;
	if (( maxX <= minX ) || ( maxY <= minY ))
	{
		m_cTransform [ 2 ] [ 0 ] = 0.0 ;
		m_cTransform [ 2 ] [ 1 ] = 0.0 ;
	}
}

void COpenDWGio::DoDocument (OdDbDatabase* pDb)
{
	// 790081
	// _crtBreakAlloc(790081);
	// Tune document parameters

	OdDbBlockTableRecordPtr pEntityTable = pDb->getModelSpaceId().safeOpenObject ();
	//OdDbBlockTableRecordPtr pEntityTable = pDb->getActiveLayoutBTRId().safeOpenObject ();

	SetTransformation (pEntityTable);

	// Create layers
	OdDbSymbolTableIteratorPtr pLayers =
		OdDbLayerTablePtr(pDb->getLayerTableId().safeOpenObject())->newIterator();	


	int i = 0;
    for (pLayers->start(); !pLayers->done(); pLayers->step())
	{
		OdDbLayerTableRecordPtr pLayerRecord = pLayers->getRecord();
		DoLayer (pLayerRecord, i++);
		//pLayerRecord.release();
	}
	m_pDoc->m_nActiveLayer=0;
	if(m_pDoc->GetLayer(0)->m_bHidden)
	{
		m_pDoc->m_bActiveLayerHidden=true;
		m_pDoc->GetLayer(0)->m_bHidden=false;
	}
	m_lastNon0DbLayer = NULL;
	m_curBlock = NULL;
	m_curDbLayer = NULL;
//	firstNon0Layer = false;

	// Create drawing objects
	OdDbObjectIteratorPtr iter = pEntityTable->newIterator();
    for (iter->start(); !iter->done(); iter->step())
    {
        OdDbEntityPtr pEntity = iter->entity ();
		//DoEntity (pEntity.detach());
		DoEntity (pEntity);
		// pEntity.release();
		if(gUserCanceled) return;
    }
}


//void COpenDWGio::DoLayer (OdDbLayerTableRecord* pDbLayer, int index)

void COpenDWGio::DoLayer (OdDbLayerTableRecord* pDbLayer, int index)
{
 	if(index) // add a new layer to the document
	{
        // SMK.SSA
		// CString str = pDbLayer->getName ().c_str ();
        CString str = (LPCWSTR)pDbLayer->getName();
		CLayer *pLayer = m_pDoc->AddLayerSorted (&str);
		pLayer->m_bHidden = pDbLayer->isOff () || pDbLayer->isFrozen ();
		pLayer->m_bGrayed = pDbLayer->isFrozen ();
	}
	else // change the existing layers info
	{
		if (!pDbLayer->getName ().isEmpty ()) // do not change if blank name
		{
			// SMK.SSA
			// m_pDoc->GetLayer(0)->m_strName = pDbLayer->getName();
			m_pDoc->GetLayer(0)->m_strName = (const char*)pDbLayer->getName();
		}
	}
}



void COpenDWGio::DoEntity (OdDbEntity* pEntity)
{
	// SMK.SSA
	// CLayer *pLayer = GetLayer (pEntity->layer ().c_str ());
	CLayer *pLayer = GetLayer (pEntity->layer ());
	if (pLayer == NULL) return;
	OdDbLayerTableRecordPtr pDbLayer = pEntity->layerId().safeOpenObject ();
	m_curDbLayer = pDbLayer;

	CMDShape* pShape = NULL;
	if ((pShape = DoArc				(pEntity)) == NULL)
	if ((pShape = DoCircle			(pEntity)) == NULL)
	if ((pShape = DoEllipse			(pEntity)) == NULL)
	if ((pShape = DoSpline			(pEntity)) == NULL)
	if ((pShape = DoPolyline		(pEntity, pLayer)) == NULL)
	if ((pShape = Do2dPolyline		(pEntity)) == NULL)
	if ((pShape = DoLine			(pEntity)) == NULL)
	if ((pShape = DoPoint			(pEntity)) == NULL)
	if ((pShape = DoMInsertBlock	(pEntity)) == NULL)
	if ((pShape = DoBlockReference	(pEntity, pLayer)) == NULL)
	if ((pShape = DoText			(pEntity, pLayer)) == NULL)
	if ((pShape = DoMText			(pEntity, pLayer)) == NULL)
	if ((pShape = DoDimension		(pEntity)) == NULL)
	if ((pShape = DoLeader			(pEntity, pLayer)) == NULL)
	if ((pShape = DoTrace			(pEntity, pLayer)) == NULL)
	if ((pShape = DoSolid			(pEntity, pLayer)) == NULL)
	if ((pShape = DoFace			(pEntity, pLayer)) == NULL)
	if ((pShape = Do3dPolyline		(pEntity)) == NULL)
	if ((pShape = DoRay				(pEntity)) == NULL)
	if ((pShape = DoXline			(pEntity)) == NULL)
	if ((pShape = DoRegion			(pEntity)) == NULL)
	if ((pShape = DoHatch			(pEntity)) == NULL)
//	if ((pShape = DoArcAlignedText	(pEntity)) == NULL)
	if ((pShape = DoImage			(pEntity)) == NULL)
	if ((pShape = DoPolygonMesh		(pEntity)) == NULL)
	if ((pShape = DoPolyFaceMesh	(pEntity)) == NULL)
	if ((pShape = DoFcf				(pEntity)) == NULL)
	if ((pShape = Do3dSolid			(pEntity)) == NULL)
	if ((pShape = DoFrame			(pEntity)) == NULL)
	if ((pShape = DoBody			(pEntity)) == NULL)
	if ((pShape = DoMline			(pEntity)) == NULL)
//	if ((pShape = DoProxyEntity		(pEntity)) == NULL)
		return;
	pShape->CalculateExtents ();
	SetObjectAttributes (pShape, pEntity);
	AddObject (pLayer, pShape);
	//m_curDbLayer = 0;
}


bool COpenDWGio::AddObject(CLayer *layer, CMDShape *shapeHdl)
{
	assert (layer);
	assert (shapeHdl);

	shapeHdl->SetShapeDatum (kDefaultDatum);

	if (m_pCurBlock)
	{
		assert (shapeHdl != m_pCurBlock);
		m_pCurBlock->AddMember (shapeHdl);
	}
	else
		//layer->m_ShapeList.AddTail (shapeHdl);
		layer->m_ShapeList.AddHead (shapeHdl);

	return true;
	
}

bool COpenDWGio::RemoveObject (CLayer *layer, CMDShape *shapeHdl)
{	
	POSITION pos=layer->m_ShapeList.Find (shapeHdl);
	if(pos)
	{
		shapeHdl->DeleteLinks (&layer->m_LinkVertexList);
		layer->m_ShapeList.RemoveAt(pos);
		delete shapeHdl;
		return true;
	}
	
	return false;
}


bool COpenDWGio::AddGroupObject (CLayer *layerHdl, CMDGroupShape *groupShape)
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
		if (m_pCurBlock)
		{
			assert (groupShape != m_pCurBlock);
			m_pCurBlock->AddMember (groupShape);
		}
		else
			layerHdl->m_ShapeList.AddTail (groupShape);
			//layerHdl->m_ShapeList.AddHead (groupShape);
		
		return true;
	}
	else
		delete groupShape;
	return false;
}



CLayer *COpenDWGio::GetLayer (LPCTSTR name) const
{
	for(int i=0; i<m_pDoc->GetLayersCount(); i++)
	{
		CLayer *pLayer = m_pDoc->GetLayer(i);
		if(pLayer->m_strName == name)
			return pLayer;
	}
	return m_pDoc->GetLayer (0);
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
} //* getColLnW */

LongPoint COpenDWGio::GetLongPoint (const OdGePoint3d& pt, bool checkBounds) const
{
	LPoint2D pt2D (pt.x, pt.y);
	pt2D *= m_cTransform;
	if(checkBounds)
		if((pt2D.x<=-kMaxValue) || (pt2D.x>=kMaxValue) || (pt2D.y<=-kMaxValue) || (pt2D.y>=kMaxValue))
		{
			m_bOverflowError=true;
			gUserCanceled=true; // to avoid additional error message
		}
	return LongPoint (pt2D.x, pt2D.y);
}

LongPoint COpenDWGio::GetLongPoint (const OdGePoint2d& pt, bool checkBounds) const
{
	LPoint2D pt2D (pt.x, pt.y);
	pt2D *= m_cTransform;
	if(checkBounds)
		if((pt2D.x<=-kMaxValue) || (pt2D.x>=kMaxValue) || (pt2D.y<=-kMaxValue) || (pt2D.y>=kMaxValue))
		{
			m_bOverflowError=true;
			gUserCanceled=true; // to avoid additional error message
		}
	return LongPoint (pt2D.x, pt2D.y);
}

LongPoint COpenDWGio::GetLongPoint (const OdGeVector3d& pt, bool checkBounds) const
{
	LVector2D vec2D (pt.x, pt.y);
	vec2D *= m_cTransform;
	if(checkBounds)
		if((vec2D.x<=-kMaxValue) || (vec2D.x>=kMaxValue) || (vec2D.y<=-kMaxValue) || (vec2D.y>=kMaxValue))
		{
			m_bOverflowError=true;
			gUserCanceled=true; // to avoid additional error message
		}
	return LongPoint (vec2D.x, vec2D.y);
}


short COpenDWGio::GetColor (const OdCmColor& color, bool zeroLayer) const
{
	static short sLastDWGColorIndex = -1;
	static short sLastColorIndex = 1;
	assert(sLastColorIndex <= 255);

	OdCmColor c;
	
	if (color.isByBlock ())
	{
		sLastDWGColorIndex = -1;
		sLastColorIndex = m_nBlockColor;
		return m_nBlockColor;
	}
	else if (color.isByLayer ())
	{
		if(zeroLayer) {
			if(m_lastNon0DbLayer) {
				c = m_lastNon0DbLayer->color ();
			} else {
				c = m_curDbLayer->color ();
			}
		} else {
			if (Abs(m_curDbLayer->colorIndex ()) == sLastDWGColorIndex)
			{
				return sLastColorIndex;
			}
			c = m_curDbLayer->color ();
		}
	}
	else // normal color
	{
		c = color;
	}
	
	
	if (c.colorIndex () == sLastDWGColorIndex) {
		return sLastColorIndex;
	}
	else
		sLastDWGColorIndex = c.colorIndex ();


	if (sLastDWGColorIndex == 255) // white ?? 179, 179, 179
		sLastColorIndex = 255;//0; //1;
	else
	switch (sLastDWGColorIndex)
	{
		case 7:  /* ACAD White if background is of dark color, else it is black */
			sLastColorIndex = 0; //2;
			break;
		default:
		{
			//COLORREF cr = RGB (c.red (), c.green (), c.blue ());
			//sLastColorIndex = m_pDoc->m_Palette.GetNearestPaletteIndex (cr);				 
			sLastColorIndex = c.colorIndex();

		}
	}
	return sLastColorIndex;
}

COLORREF from_index_toRGB(short colorIndex)
{
	OdCmEntityColor cEnt;
	assert(colorIndex<=255);
	return RGB (cEnt.mLUT[colorIndex][0],cEnt.mLUT[colorIndex][1],cEnt.mLUT[colorIndex][2]);
	//return PALETTEINDEX(colorIndex);
}

void COpenDWGio::SetObjectAttributes (CMDShape* pShape, const OdDbEntity* pEntity, bool blockRef, bool toFill)
{
	bool isZeroLayer = pEntity->layer() == "0";
	short colorIndex = GetColor (pEntity->color (), isZeroLayer);
	//COLORREF colorRGB = GetRGBColor (pEntity->color ());
	
	PenModel penModel = pShape->GetPenModel ();

	double thickness;
	bool hairline = false;
	OdDb::LineWeight lineweight = pEntity->lineWeight ();
	OdDbLinetypeTableRecordPtr pLineType;
	OdString linetype = pEntity->linetype();
	switch (lineweight)
	{
	case OdDb::kLnWtByBlock:
		if(m_curBlock) {
			lineweight = m_curBlock->lineWeight ();
			if(lineweight == OdDb::kLnWtByLayer) {
				if(m_lastNon0DbLayer != NULL) {
					lineweight = m_lastNon0DbLayer->lineWeight ();
				} else {
					lineweight = m_curDbLayer->lineWeight ();
				}
			}
			if (lineweight < 0)
			{
				lineweight = OdDb::kLnWt000;
				//lineweight = OdDb::kLnWt005;
				hairline = true;
			}
			break;
		}
	case OdDb::kLnWtByLayer:
		if(isZeroLayer && (m_lastNon0DbLayer != NULL)) {
			lineweight = m_lastNon0DbLayer->lineWeight ();
		} else {
			lineweight = m_curDbLayer->lineWeight ();
		}
		if (lineweight < 0)
		{
			lineweight = OdDb::kLnWt000;
			//lineweight = OdDb::kLnWt005;
			hairline = true;
			break;
		}
	case OdDb::kLnWtByLwDefault:
		hairline = true;
		break;
	default:
		lineweight = pEntity->lineWeight ();
	}
	if(linetype.makeUpper() == "BYBLOCK") {
		if(m_curBlock) {
			pLineType = m_curBlock->linetypeId ().openObject ();
			linetype = m_curBlock->linetype();
			if(linetype.makeUpper() == "BYLAYER") {
				if(m_lastNon0DbLayer != NULL) pLineType = m_lastNon0DbLayer->linetypeObjectId ().openObject ();
				else pLineType = m_curDbLayer->linetypeObjectId ().openObject ();
			}
		} else pLineType = m_curDbLayer->linetypeObjectId ().openObject ();
		
	} else if(linetype.makeUpper() == "BYLAYER") {
		if(isZeroLayer && (m_lastNon0DbLayer != NULL)) pLineType = m_lastNon0DbLayer->linetypeObjectId ().openObject ();
		else pLineType = m_curDbLayer->linetypeObjectId ().openObject ();
	} else pLineType = pEntity->linetypeId ().openObject ();
	
	thickness = lineweight * m_g100mmDWGScale * 0.01;
	penModel.m_nWidth = (uchar)getColLnW((short)thickness);
	if(hairline)
		penModel.m_nWidth |= 0x80;

	if(pLineType.get())
	{
		double patternLength = pLineType->patternLength ();
		double dashLength = patternLength > 0 ? pLineType->dashLengthAt (0) : 0;

		switch (pLineType->numDashes())
		{
			case 0:
				penModel.m_nStyle = 0;
				break;
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
				if (dashLength < patternLength * 0.7)
					penModel.m_nStyle = 1;
				else
				if (dashLength < patternLength * 0.8)
					penModel.m_nStyle = 2;
				else
				if (dashLength < patternLength * 0.9)
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
				if (dashLength < patternLength * 0.7)
					penModel.m_nStyle=5;
				else if (dashLength < patternLength * 0.8)
					penModel.m_nStyle=6;
				else if (dashLength < patternLength * 0.9)
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
				if (dashLength < patternLength * 0.7)
					penModel.m_nStyle=9;
				else
				if (dashLength < patternLength * 0.8)
					penModel.m_nStyle = 10;
				else
				if (dashLength < patternLength * 0.9)
					penModel.m_nStyle = 11;
				else
					penModel.m_nStyle = 12;
				break;
			}
			case 8:
			{
				// must be dash dot dot dot
				// 13 = 1 dash dot dot dot length
				// 14 = 2 dash dot dot dot lengths
				// 15 = 4 dash dot dot dot lengths
				if (dashLength < patternLength * 0.7)
					penModel.m_nStyle = 13;
				else
				if (dashLength < patternLength * 0.8)
					penModel.m_nStyle = 14;
				else
				if (dashLength < patternLength * 0.9)
					penModel.m_nStyle = 15;
				break;
			}
			default:
			{
				if (pLineType->numDashes () > 8) // lots of dash dots
					penModel.m_nStyle = 15;
			}
		}
	}
	//To remove

	FillModel fillModel;

	if (toFill)
	{
		fillModel.m_nFillType=kFillColor;
		fillModel.m_crColor= from_index_toRGB(colorIndex); //PALETTEINDEX (colorIndex);
		penModel.m_crColor=from_index_toRGB(colorIndex); //PALETTEINDEX (colorIndex);
//		OdCmColor col = pEntity->color();
		if(colorIndex == 255) 
		{
			fillModel.m_crColor = RGB(179, 179, 179);
			penModel.m_crColor=RGB(179, 179, 179);
		} /*else {
			fillModel.m_crColor = RGB(col.red(), col.green(), col.blue());
			penModel.m_crColor=RGB(col.red(), col.green(), col.blue());
		}*/
	}
	else
	{
		if(colorIndex == 255) 
		{
			penModel.m_crColor=RGB(179, 179, 179);
		} else {
			penModel.m_crColor= from_index_toRGB(colorIndex);//PALETTEINDEX (colorIndex);
		}
		fillModel.m_nFillType=kNoFill;
		fillModel.m_crColor=PALETTEINDEX (0);
		//fillModel.m_nIndex=0;
	}

	if (pEntity->visibility () == OdDb::kInvisible )
	{
		fillModel.m_nFillType = kNoFill;
		penModel.m_nPenType = kPenInvis;
		penModel.m_nIndex = kNoneIndex;
	}

	pShape->SetPenModel (penModel);
	if(!blockRef) pShape->SetFillModel (fillModel);
}



#define def_var(T) \
ASSERT (pEntity); \
OdDb##T* p##T = static_cast <OdDb##T*> (pEntity->queryX (OdDb##T::desc ())); \
if (p##T == NULL) return NULL;

CMDShape* COpenDWGio::DoBlockReference (OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(BlockReference);
	CMDGroupShape* shape = new CMDGroupShape;
	//SetObjectAttributes (shape, pEntity);
	if (shape)
	{
		// Save previous group values
		LMatrix3x2 cParentTransform = m_cTransform;
		CMDGroupShape*	pParentBlock = m_pCurBlock;
		short nParentColor = m_nBlockColor; 
		OdDbLayerTableRecord* pBlockDbLayer = m_curDbLayer;
		OdDbLayerTableRecord* pBlockDbLayerNon0 = m_lastNon0DbLayer;
		OdDbBlockReference* pBlock = m_curBlock;
		bool non0LayerChanged = false;
		double old_gDWGScale = m_gDWGScale;

		// Initialize current group values
		m_nBlockColor = GetColor (pBlockReference->color ());
		m_pCurBlock =  shape;
		OdGePoint3d pt = pBlockReference->position ();
		double angle =	pBlockReference->rotation ();
		OdGeScale3d scale = pBlockReference->scaleFactors ();
		m_curBlock = pBlockReference;

		if(pEntity->layer() != "0"){
			OdDbLayerTableRecordPtr pLay = pEntity->layerId().openObject ();
			m_lastNon0DbLayer = pLay;
			non0LayerChanged = true;
		}

		m_cTransform = ScaleMat3x2 (scale.sx, scale.sy) * RotateMat3x2 (-angle) * TranslateMat3x2 (pt.x, pt.y) * m_cTransform;
		m_gDWGScale *= (scale.sx + scale.sy)/2.0;
		
		OdDbBlockTableRecordPtr pEntityTable = pBlockReference->blockTableRecord().openObject ();
		pt = pEntityTable->origin ();
		m_cTransform = TranslateMat3x2 (-pt.x, -pt.y) * m_cTransform;
		// Create drawing objects in the group
		OdDbObjectIteratorPtr iter = pEntityTable->newIterator();
		for (iter->start(); !iter->done(); iter->step())
		{
			OdDbEntityPtr pEntity = iter->entity ();
			DoEntity (pEntity);
			//pEntity.release();
		}
		
		// Restore previous group values
		m_cTransform = cParentTransform;

		iter = pBlockReference->attributeIterator();
		for (iter->start(); !iter->done(); iter->step())
		{
			OdDbEntityPtr pEntity = pBlockReference->openAttribute (iter->objectId(), OdDb::kForRead);
			DoEntity (pEntity);
		}

		m_nBlockColor = nParentColor;
		m_pCurBlock = pParentBlock;
		m_curDbLayer = pBlockDbLayer;
		m_gDWGScale = old_gDWGScale;
		m_curBlock = pBlock;
		if(non0LayerChanged)
			m_lastNon0DbLayer = pBlockDbLayerNon0;
	}
	//SetObjectAttributes (shape, pEntity, true);
	AddGroupObject (pLayer, shape);
	return NULL;
}

CMDShape* COpenDWGio::DoMInsertBlock (OdDbEntity* pEntity)
{
	def_var(MInsertBlock);
	CMDGroupShape* shape = new CMDGroupShape;
	if (shape)
	{
		// Save previous group values
		LMatrix3x2 cParentTransform = m_cTransform;
		CMDGroupShape*	pParentBlock = m_pCurBlock;
		short nParentColor = m_nBlockColor; 
		OdDbLayerTableRecord* pBlockDbLayer = m_curDbLayer;
		OdDbLayerTableRecord* pBlockDbLayerNon0 = m_lastNon0DbLayer;
		bool non0LayerChanged = false;
		double old_gDWGScale = m_gDWGScale;
		OdDbBlockReference* pBlock = m_curBlock;

		if(pEntity->layer() != "0"){
			OdDbLayerTableRecordPtr pLay = pEntity->layerId().openObject ();
			m_lastNon0DbLayer = pLay;
			non0LayerChanged = true;
		}
		

		// Initialize current group values
		
		OdGePoint3d pt = pMInsertBlock->position ();
		OdGePoint3d newpt = pt;
		double angle =	pMInsertBlock->rotation ();
		OdGeScale3d scale = pMInsertBlock->scaleFactors ();
		m_curBlock = pMInsertBlock;

		m_gDWGScale *= (scale.sx + scale.sy)/2.0;

		OdUInt16 cols = pMInsertBlock->columns();
		OdUInt16 rows = pMInsertBlock->rows();
		double colSp = pMInsertBlock->columnSpacing();
		double rowSp = pMInsertBlock->rowSpacing();

		OdUInt16 i,j;
		LMatrix3x2 newTransform = RotateMat3x2 (-angle);
		for(i = 0; i < cols; i++)
			for(j = 0; j < rows; j++)
			{
				m_nBlockColor = GetColor (pMInsertBlock->color ());
				m_pCurBlock =  shape;
				pt = newpt + OdGeVector3d( i * colSp, j * rowSp, 0);
				LPoint2D pt2D (pt.x, pt.y);
				pt2D *= newTransform;
				m_cTransform = ScaleMat3x2 (scale.sx, scale.sy) * RotateMat3x2 (-angle) * TranslateMat3x2 (pt2D.x, pt2D.y) * cParentTransform;
				OdDbBlockTableRecordPtr pEntityTable = pMInsertBlock->blockTableRecord().openObject ();
				pt = pEntityTable->origin ();
				m_cTransform = TranslateMat3x2 (-pt.x, -pt.y) * m_cTransform;
				// Create drawing objects in the group
				OdDbObjectIteratorPtr iter = pEntityTable->newIterator();
				for (iter->start(); !iter->done(); iter->step())
				{
					OdDbEntityPtr pEntity = iter->entity ();
					DoEntity (pEntity);
				}
				// Restore previous group values
				m_cTransform = cParentTransform;
				iter = pMInsertBlock->attributeIterator();
				for (iter->start(); !iter->done(); iter->step())
				{
					OdDbEntityPtr pEntity = pMInsertBlock->openAttribute (iter->objectId(), OdDb::kForRead);
					DoEntity (pEntity);
				}
				m_nBlockColor = nParentColor;
				m_pCurBlock = pParentBlock;
				m_curDbLayer = pBlockDbLayer;
				m_curBlock = pBlock;
			}
		m_gDWGScale = old_gDWGScale;
		if(non0LayerChanged)
			m_lastNon0DbLayer = pBlockDbLayerNon0;
	}
	return shape;
}

bool isPlanar(const OdGeVector3d& normal)
{
	if( Abs(normal.x) + Abs(normal.y) > 1e-10) return false;
	return true;
}

// Curves
CMDShape* COpenDWGio::DoArc (OdDbEntity* pEntity)
{
	def_var(Arc);
	LongPoint ptCenter = GetLongPoint (pArc->center (), false);
	double radius = pArc->radius ();
	bool isPl = isPlanar(pArc->normal());
	
	//LVector2D s_v = m_cTransform.GetScale ();
	CMDArcShape *newHdl;
	//if (Coord (s_v.x) != Coord(s_v.y))
	if(m_cTransform.IsUniformRotateMatrix() && isPl )
		newHdl = new CMDArcShape (CMDArcShape::kRadius);
	else 
		newHdl = new CMDArcShape (CMDArcShape::kElliptic);
	if (newHdl)
	{
		newHdl->m_nArcDirection = AD_CLOCKWISE;

//		double startAng;
//		pArc->getStartParam (startAng);
//		double endAng;
//		pArc->getEndParam (endAng);
		if (m_cTransform.IsMirrored ())
		{
		//		startAng *= -1;
		//		endAng *= -1;
			newHdl->m_nArcDirection = AD_COUNTERCLOCKWISE;
		}
		if(pArc->normal().z < 0) {
			if(newHdl->m_nArcDirection == AD_CLOCKWISE) newHdl->m_nArcDirection = AD_COUNTERCLOCKWISE;
			else newHdl->m_nArcDirection = AD_CLOCKWISE;
//			startAng = _M_PI -startAng;
//			endAng = _M_PI -endAng;
		}
		newHdl->SetShapePoint (0, ptCenter);

		OdGePoint3d apt;
		pArc->getStartPoint(apt);
		LongPoint pt = GetLongPoint(apt);
		newHdl->SetShapePoint (1, pt);

		pArc->getEndPoint(apt);
		pt = GetLongPoint(apt);
		newHdl->SetShapePoint (2, pt);

		if (!m_cTransform.IsUniformRotateMatrix() || !isPl) {
			double angle;
			LVector2D v;
		
			if(isPl)
				CalculateCircleParams(radius, v, angle);
			else {
				double x = pArc->normal().x;
				double y = pArc->normal().y;
				double z = pArc->normal().z;
				double norm = sqrt(x*x + y*y + z*z);
				double norm1 = pArc->radius() / sqrt( x*x + y*y );
				double norm2 = norm1 * z / norm;
				OdGeVector3d ptMinorAxis(x * norm2, y * norm2, 0);
				OdGeVector3d ptMajorAxis(y * norm1, -x * norm1, 0);
				CalculateEllipseParams(ptMajorAxis, ptMinorAxis, v, angle);
			}
			newHdl->SetShapePoint (3, ptCenter + LongPoint(v.x, v.y));
			newHdl->SetRotation (m360Ang (angle * kRadToAng));
		} else {
			newHdl->SetRotation (m360Ang (m_cTransform.GetAngle () * kRadToAng));
		}
	}
	return newHdl;
}

bool COpenDWGio::CalculateCircleParams(const double radius, double& a, double& b, double& angle)
{
	// Inverse matrix
	double det = ( m_cTransform [ 0 ] [ 0 ] ) * ( m_cTransform [ 1 ] [ 1 ] ) - ( m_cTransform [ 0 ] [ 1 ] ) * ( m_cTransform [ 1 ] [ 0 ] );
	if ( Abs( det ) < 1e-10) return false;
	double i11 = m_cTransform [ 1 ] [ 1 ] / det;
	double i22 = m_cTransform [ 0 ] [ 0 ] / det;
	double i12 = - m_cTransform [ 1 ] [ 0 ] / det;
	double i21 = - m_cTransform [ 0 ] [ 1 ] / det;
	double a11, a22, a12;
	a11 = i11 * i11 + i21 * i21;
	a22 = i12 * i12 + i22 * i22;
	a12 = i11 * i12 + i21 * i22;
	det = a11 * a22 - a12 * a12;
	double diskr = ( a11 - a22 ) * ( a11 - a22 ) + 4 * a12 * a12;
	double minor = ( a11 + a22 - sqrt(diskr)) / 2.0;
	double major = ( a11 + a22 + sqrt(diskr)) / 2.0;
	//if ( Abs( minor ) < 1e-10) return false;
	//if ( Abs( major ) < 1e-10) return false;
	double A = det * radius * radius;
	angle = - atan2(2 * a12, a11 - a22) / 2.0;
	a = sqrt(A/minor/minor/major);
	b = sqrt(A/minor/major/major);
	return true;
}

// Parameters for Arc
bool COpenDWGio::CalculateCircleParams(const double radius, LVector2D& v, double& angle)
{
	double a,b;
	if(CalculateCircleParams(radius, a, b, angle)) {
		v.x = a;
		v.y = b;
		LMatrix3x2 rot = RotateMat3x2 ( _M_PI/2 + angle );
		v *= rot;
		return true;
	}
	return false;
}

// Parameters for Circle
bool COpenDWGio::CalculateCircleParams(const double radius, LVector2D& v, LVector2D& v1, double& angle)
{
	double a,b;
	if(CalculateCircleParams(radius, a, b, angle)) {
		v.x = a;
		v.y = b;
		v1.x = v.x;
		v1.y = -v.y;
		LMatrix3x2 rot = RotateMat3x2 ( _M_PI/2 + angle );
		v *= rot;
		v1 *= rot;
		return true;
	}
	return false;
}

bool COpenDWGio::CalculateEllipseParams(const OdGeVector3d& majorAxx, const OdGeVector3d& minorAxx, double& a, double& b, double& angle) const
{
	// Inverse matrix
	double det = ( m_cTransform [ 0 ] [ 0 ] ) * ( m_cTransform [ 1 ] [ 1 ] ) - ( m_cTransform [ 0 ] [ 1 ] ) * ( m_cTransform [ 1 ] [ 0 ] );
	if ( Abs( det ) < 1e-10) return false;
	double i11 = m_cTransform [ 1 ] [ 1 ] / det;
	double i22 = m_cTransform [ 0 ] [ 0 ] / det;
	double i12 = - m_cTransform [ 1 ] [ 0 ] / det;
	double i21 = - m_cTransform [ 0 ] [ 1 ] / det;
	// Equation of the original ellipse
	double o11, o12, o22, odet, odetsq;
	odet = majorAxx.x * minorAxx.y - majorAxx.y * minorAxx.x;
	odetsq = odet * odet;
	if(Abs(odet) < 1e-10) return false; // Ellipse is singular
	// Major and minor axes lengths
	//omajsq = majorAxx.x * majorAxx.x + majorAxx.y * majorAxx.y;
	//ominsq = minorAxx.x * minorAxx.x + minorAxx.y * minorAxx.y;
	// Calculating original coeffitients
	o11 = (minorAxx.y * minorAxx.y + majorAxx.y * majorAxx.y) / odetsq;
	o22 = (minorAxx.x * minorAxx.x + majorAxx.x * majorAxx.x) / odetsq;
	o12 = - (minorAxx.x * minorAxx.y + majorAxx.x * majorAxx.y) / odetsq;

	// Calculating coefficients of the transformed ellipse
	double a11, a22, a12;
	a11 = o11 * i11 * i11 + o22 * i21 * i21 + 2 * o12 * i11 * i21;
	a22 = o11 * i12 * i12 + o22 * i22 * i22 + 2 * o12 * i12 * i22;
	a12 = o11 * i11 * i12 + o22 * i21 * i22 + o12 * ( i11 * i22 + i21 * i12);
	det = a11 * a22 - a12 * a12;
	double diskr = ( a11 - a22 ) * ( a11 - a22 ) + 4 * a12 * a12;
	double minor = ( a11 + a22 - sqrt(diskr)) / 2.0;
	double major = ( a11 + a22 + sqrt(diskr)) / 2.0;
	//if ( Abs( minor ) < 1e-10) return false;
	//if ( Abs( major ) < 1e-10) return false;
	double A = det;
	angle = - atan2(2 * a12, a11 - a22) / 2.0;
	a = sqrt(A/minor/minor/major);
	b = sqrt(A/minor/major/major);
	return true;
}

bool COpenDWGio::CalculateEllipseParams(const OdGeVector3d& majorAxx, const OdGeVector3d& minorAxx, LVector2D& v, LVector2D& v1, double& angle) const
{
	double a,b;
	if(CalculateEllipseParams(majorAxx, minorAxx, a, b, angle)) {
		v.x = a;
		v.y = b;
		v1.x = v.x;
		v1.y = -v.y;
		LMatrix3x2 rot = RotateMat3x2 ( _M_PI/2 + angle );
		v *= rot;
		v1 *= rot;
		return true;
	}
	return false;
}

bool COpenDWGio::CalculateEllipseParams(const OdGeVector3d& majorAxx, const OdGeVector3d& minorAxx, LVector2D& v, double& angle) const
{
	double a,b;
	if(CalculateEllipseParams(majorAxx, minorAxx, a, b, angle)) {
		v.x = a;
		v.y = b;
		LMatrix3x2 rot = RotateMat3x2 ( _M_PI/2 + angle );
		v *= rot;
		return true;
	}
	return false;
}

CMDShape* COpenDWGio::DoCircle (OdDbEntity* pEntity)
{
	def_var(Circle);
	CMDShape *newHdl = NULL;
	OdGePoint3d pt = pCircle->center ();
	LVector2D v = m_cTransform.GetScale ();

	LongPoint ptCenter = GetLongPoint(pt);

	if (!m_cTransform.IsUniformRotateMatrix())
	{
		newHdl = new CMDEllipse(CMDRectangle::kRectDiagonal);
		if (newHdl)
		{
			double angle;
			LVector2D v;
			LVector2D v1;
			CalculateCircleParams(pCircle->radius(), v, v1, angle);
			newHdl->SetShapePoint(0,ptCenter + LongPoint(v.x,v.y));
			newHdl->SetShapePoint(1,ptCenter + LongPoint( v1.x, v1.y));
			newHdl->SetShapePoint(2,ptCenter + LongPoint( -v.x, -v.y));
			newHdl->SetShapePoint(3,ptCenter + LongPoint( -v1.x,-v1.y));
			//newHdl->SetRotation (90 + m360Ang (m_cTransform.GetAngle () * kRadToAng));
			newHdl->SetRotation (m360Ang (angle * kRadToAng));
			//newHdl->SetRotation (m360Ang (m_cTransform.GetAngle () * kRadToAng));
		}
	}
	else
	{
		if(isPlanar(pCircle->normal())) {
			newHdl = new CMDCircle (CMDCircle::kRadius);
			if (newHdl)
			{
				newHdl->SetShapePoint (0, GetLongPoint(pt));
				pt.x += pCircle->radius ();
				newHdl->SetShapePoint (1, GetLongPoint(pt));
			}
		} else { // rotated in 3D-space
			newHdl = new CMDEllipse(CMDRectangle::kRectDiagonal);
			if (newHdl)
			{
				double angle;
				double x = pCircle->normal().x;
				double y = pCircle->normal().y;
				double z = Abs(pCircle->normal().z);
				double norm = sqrt(x*x + y*y + z*z);
				double norm1 = pCircle->radius() / sqrt( x*x + y*y );
				double norm2 = norm1 * z / norm;
				OdGeVector3d minor(x * norm2, y * norm2, 0);
				OdGeVector3d major(-y * norm1, x * norm1, 0);
				LVector2D v;
				LVector2D v1;
				CalculateEllipseParams(major, minor, v, v1, angle);
				newHdl->SetShapePoint(0,ptCenter + LongPoint(v.x,v.y));
				newHdl->SetShapePoint(1,ptCenter + LongPoint( v1.x, v1.y));
				newHdl->SetShapePoint(2,ptCenter + LongPoint( -v.x, -v.y));
				newHdl->SetShapePoint(3,ptCenter + LongPoint( -v1.x,-v1.y));
				//newHdl->SetRotation (90 + m360Ang (m_cTransform.GetAngle () * kRadToAng));
				newHdl->SetRotation (m360Ang (angle * kRadToAng));
				//newHdl->SetRotation (m360Ang (m_cTransform.GetAngle () * kRadToAng));
			}
		}
	}
	return newHdl;
}



CMDShape* COpenDWGio::DoLine (OdDbEntity* pEntity)
{
	def_var(Line);

	CMDLineShape *newHdl = new CMDLineShape;

	if (newHdl)
	{
		newHdl->SetShapePoint (0, GetLongPoint (pLine->startPoint ()));
		newHdl->SetShapePoint (1, GetLongPoint (pLine->endPoint ()));
	}

	return newHdl;
}

CMDShape* COpenDWGio::DoPoint (OdDbEntity* pEntity)
{
	def_var(Point);
	CMDLineShape *newHdl = new CMDLineShape;
	if (newHdl)
	{
		newHdl->SetShapePoint (0, GetLongPoint (pPoint->position ()));
		newHdl->SetShapePoint (1, GetLongPoint (pPoint->position ()));
	}
	return newHdl;
}

CMDShape* COpenDWGio::Do2dPolyline (OdDbEntity* pEntity)
{
	def_var(2dPolyline);

	OdDbObjectIteratorPtr iter = p2dPolyline->vertexIterator();
	int count = 0;
    for (iter->start(); !iter->done(); iter->step()) {
		OdDb2dVertexPtr pt_ptr = iter->entity();
		OdDb::Vertex2dType tp = pt_ptr->vertexType();
		if((pt_ptr->vertexType() == OdDb::k2dVertex) || (pt_ptr->vertexType() == OdDb::k2dSplineFitVertex)) count++;
	}

	//if(p2dPolyline->isClosed()) count--;

	CMDPolygonShape* shape = NULL; 

	switch (p2dPolyline->polyType ())
	{
		case OdDb::k2dSimplePoly:
			{
				CMDPolygonShape* newHdl = new CMDPolygonShape (CMDPolygonShape::kPolygon,count, p2dPolyline->isClosed ());
				long i=0;
				iter = p2dPolyline->vertexIterator();
				for (iter->start(); !iter->done(); iter->step())
				{
					OdDb2dVertexPtr pt_ptr = iter->entity();
					newHdl->SetShapePoint (i, GetLongPoint (pt_ptr->position()));
					++i;
				}
				return newHdl;
			}
			break;
		case OdDb::k2dFitCurvePoly:
			BadObjectAlert(IDS_DWGBEZIER);
			return NULL;
			break;
		case OdDb::k2dQuadSplinePoly:
			BadObjectAlert (IDS_DWGQUADRATICBSPLINE);
			return NULL;
			break;
		case OdDb::k2dCubicSplinePoly:
			{
				//if(p2dPolyline->isClosed()) count--;
				CMDCubicShape* newHdl = new CMDCubicShape (count, p2dPolyline->isClosed ());
				long i=0;
				iter = p2dPolyline->vertexIterator();
				iter->start();

				// Need to skip first vertex
				//if(p2dPolyline->isClosed ()) 
				iter->step();
				for (; !iter->done(); iter->step())
				{
					OdDb2dVertexPtr pt_ptr = iter->entity();
					if((i == 0)&&(p2dPolyline->isClosed() == false)) newHdl->SetShapePoint (i++, GetLongPoint (pt_ptr->position())); 
					else if(pt_ptr->vertexType() == OdDb::k2dSplineFitVertex) newHdl->SetShapePoint (i++, GetLongPoint (pt_ptr->position()));
				}
				return newHdl;
			}
			break;
	}
	return NULL;
}


CMDShape* COpenDWGio::DoPolyline (OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(Polyline);

	long count = pPolyline->numVerts ();
	long totcount = count + ((pPolyline->isClosed()) ? 1 : 0);

	OdGePoint3d pt;
	if (count == 0)
	{
		return NULL;
	}
	if (count == 1)
	{
		CMDLineShape *line = new CMDLineShape;
		pPolyline->getPointAt (0, pt);
		line->SetShapePoint (0, GetLongPoint (pt));
		line->SetShapePoint (1, GetLongPoint (pt));
		return line;
	}
	if (totcount == 2 && (pPolyline->segType (0) == 0))
	{
		double thickness, startWidth, endWidth;
		if(pPolyline->hasWidth()) {
			pPolyline->getWidthsAt (0, startWidth, endWidth);
			thickness = (startWidth + endWidth) * m_gDWGScale * 0.5 + 0.5;
		} else {
			thickness = pPolyline->getConstantWidth() * m_gDWGScale + 0.5;
		}
		CMDLineShape *line = new CMDLineShape;
		pPolyline->getPointAt (0, pt);
		line->SetShapePoint (0, GetLongPoint (pt));
		pPolyline->getPointAt (1, pt);
		line->SetShapePoint (1, GetLongPoint (pt));
		SetObjectAttributes(line, pEntity);
		PenModel penModel = line->GetPenModel();
		penModel.m_nWidth = (uchar)getColLnW((short)thickness);
		line->SetPenModel (penModel);
		line->CalculateExtents ();
		AddObject (pLayer, line);
		return NULL;
	}
	if (pPolyline->isOnlyLines() && !pPolyline->hasWidth())
	{
		double thickness, startWidth, endWidth;
		CMDPolygonShape* newHdl = new CMDPolygonShape (CMDPolygonShape::kPolygon,count, pPolyline->isClosed ());
		if(pPolyline->hasWidth()) {
			pPolyline->getWidthsAt (0, startWidth, endWidth);
			thickness = (startWidth + endWidth) * m_gDWGScale * 0.5 + 0.5;
		} else {
			thickness = pPolyline->getConstantWidth() * m_gDWGScale + 0.5;
		}
		SetObjectAttributes(newHdl, pEntity);
		PenModel penModel = newHdl->GetPenModel();
		penModel.m_nWidth = (uchar)getColLnW((short)thickness);
		newHdl->SetPenModel (penModel);
		for (long i = 0; i < count; i++)
		{
			pPolyline->getPointAt (i, pt);
			newHdl->SetShapePoint (i, GetLongPoint (pt));
		}
		newHdl->CalculateExtents ();
		AddObject (pLayer, newHdl);
		return NULL;
	}
	else
	{
		double startWidth = 0;
		double endWidth = 0;
	//	double buldge;
		double thickness;
		CMDGroupShape* newHdl = new CMDGroupShape;
		SetObjectAttributes(newHdl, pEntity);
		for (long i = 0; i < totcount - 1; i++)
		{
			//buldge = pPolyline->getBulgeAt (i);
			if(pPolyline->hasWidth()) {
				pPolyline->getWidthsAt (i, startWidth, endWidth);
				thickness = (startWidth + endWidth) * m_gDWGScale * 0.5 + 0.5;
			} else {
				thickness = pPolyline->getConstantWidth() * m_gDWGScale + 0.5;
			}
			switch (pPolyline->segType (i))
			{
			case OdDbPolyline::kLine:
				{
				CMDLineShape *line = new CMDLineShape;
				pPolyline->getPointAt (i, pt);
				line->SetShapePoint (0, GetLongPoint (pt));
				if(i != count-1) 
					pPolyline->getPointAt (i + 1, pt);
				else 
					pPolyline->getPointAt (0, pt);
				line->SetShapePoint (1, GetLongPoint (pt));
				SetObjectAttributes(line, pEntity);
				PenModel penModel = line->GetPenModel();
				penModel.m_nWidth = (uchar)getColLnW((short)thickness);
				line->SetPenModel (penModel);
				newHdl->AddMember (line);
				}
				break;
			case OdDbPolyline::kArc:
				{
					CMDArcShape *newArc;

					OdGeCircArc3d arc;
					pPolyline->getArcSegAt (i, arc);
					OdGeCircArc2d arc1;
					pPolyline->getArcSegAt (i, arc1);

					bool isPl = isPlanar(arc.normal());

					if (!m_cTransform.IsUniformRotateMatrix() || !isPl)
						newArc = new CMDArcShape (CMDArcShape::kElliptic);
					else 
						newArc = new CMDArcShape (CMDArcShape::kRadius);
					
					LongPoint ptCenter = GetLongPoint (arc.center (), false);
					double radius = arc.radius ();

					if (newArc)
					{
						newArc->m_nArcDirection = AD_CLOCKWISE;
						//double startAng =  arc.startAngFromXAxis();
						//double endAng =  arc.endAngFromXAxis();

						if(arc1.isClockWise()) 
						{
							/*startAng =  -arc.startAngFromXAxis();
							endAng =  -arc.endAngFromXAxis();*/
							newArc->m_nArcDirection = AD_COUNTERCLOCKWISE;
						} 

						if (m_cTransform.IsMirrored ())
						{
							//		startAng *= -1;
							//		endAng *= -1;
							if(newArc->m_nArcDirection == AD_COUNTERCLOCKWISE) newArc->m_nArcDirection = AD_CLOCKWISE;
							else newArc->m_nArcDirection = AD_COUNTERCLOCKWISE;
						}
						if(pPolyline->normal().z < 0) {
							if(newArc->m_nArcDirection == AD_CLOCKWISE) newArc->m_nArcDirection = AD_COUNTERCLOCKWISE;
							else newArc->m_nArcDirection = AD_CLOCKWISE;
							/*startAng = _M_PI -startAng;
							endAng = _M_PI -endAng;*/
						}
						newArc->SetShapePoint (0, ptCenter);
						LongPoint pt;
						//LongPoint spt;
						pt = GetLongPoint(arc.startPoint());
						newArc->SetShapePoint (1, pt);

						pt = GetLongPoint(arc.endPoint());
						newArc->SetShapePoint (2, pt);

						if (!m_cTransform.IsUniformRotateMatrix() || !isPl ) {
							double angle;
							LVector2D v;
			
							if(isPl)
								CalculateCircleParams(radius, v, angle);
							else {
								double x = arc.normal().x;
								double y = arc.normal().y;
								double z = arc.normal().z;
								double norm = sqrt(x*x + y*y + z*z);
								double norm1 = arc.radius() / sqrt( x*x + y*y );
								double norm2 = norm1 * z / norm;
								OdGeVector3d ptMinorAxis(x * norm2, y * norm2, 0);
								OdGeVector3d ptMajorAxis(y * norm1, -x * norm1, 0);
								CalculateEllipseParams(ptMajorAxis, ptMinorAxis, v, angle);
							}
							newArc->SetShapePoint (3, ptCenter + LongPoint(v.x, v.y));
							newArc->SetRotation (m360Ang (angle * kRadToAng));
						
							//newArc->SetShapePoint (3, ptCenter + GetLongPoint (OdGeVector3d(radius, radius, 0)));
						} else 
							newArc->SetRotation (m360Ang (m_cTransform.GetAngle () * kRadToAng));

						SetObjectAttributes(newArc, pEntity);
						PenModel penModel = newArc->GetPenModel();
						penModel.m_nWidth = (uchar)getColLnW((short)thickness);
						newArc->SetPenModel (penModel);
						newHdl->AddMember (newArc);
					}
				}
				break;
			case OdDbPolyline::kCoincident:
			case OdDbPolyline::kPoint:
				{
				CMDLineShape *line = new CMDLineShape;
				pPolyline->getPointAt (i, pt);
				line->SetShapePoint (0, GetLongPoint (pt));
				line->SetShapePoint (1, GetLongPoint (pt));
				newHdl->AddMember (line);
				}
			case OdDbPolyline::kEmpty:
				break;
			}
		}
		/*if(pPolyline->isClosed ()) {
			CMDLineShape *line = new CMDLineShape;
			pPolyline->getPointAt (0, pt);
			line->SetShapePoint (0, GetLongPoint (pt));
			pPolyline->getPointAt (count-1, pt);
			line->SetShapePoint (1, GetLongPoint (pt));
			newHdl->AddMember (line);
		}*/
		newHdl->CalculateExtents ();
		AddObject (pLayer, newHdl);
		return NULL;
	}

	return NULL;
}



CMDShape* COpenDWGio::DoSpline (OdDbEntity* pEntity)
{
	def_var(Spline);
	register long i;
	
	CMDPolygonShape* shape;
	OdGePoint3d pt;

/*
	long numCtlPoints = pSpline->numControlPoints ();


	if (numCtlPoints)
	{
		long numPoints = (pSpline->numFitPoints () - 1) * 3 + 1;
		numPoints = 10;
		if ( pSpline->isClosed())
			--numPoints;
		shape = new CMDBezierShape (numPoints);

		long fit = 0;
		long ctl = 0;
		pSpline->getFitPointAt (0, pt);
		shape->SetShapePoint (0, GetLongPoint (pt));

		for (i = 1 ; i < numPoints-2; i+=3)
		{
//			if ((i % 3) == 0)
//			{
//				pSpline->getFitPointAt (fit++, pt);
//			}
//			else
//			{
//				if (ctl & 1)
//					pSpline->getControlPointAt (ctl++, pt);
//			}
//		
			pSpline->getFitPointAt (i/3 + 1, pt);
			pSpline->getControlPointAt (ctl+ 2, pt);
			shape->SetShapePoint (i, GetLongPoint (pt));
			pSpline->getControlPointAt (ctl+ 1, pt);
			shape->SetShapePoint (i+1, GetLongPoint (pt));
			ctl+=2;
			pSpline->getFitPointAt (i/3 + 1, pt);
			shape->SetShapePoint (i+2, GetLongPoint (pt));
		}
	}
*/

	long numPoints = pSpline->numFitPoints ();
	if (numPoints)
	{
 		if ( pSpline->isClosed())
			--numPoints;

		//shape = new CMDCubicShape (numPoints * 4 + 1, pSpline->isClosed ());
		shape = new CMDCubicShape (numPoints, pSpline->isClosed ());
		/*double t;
		double start, end;
		pSpline->getStartParam(start);
		pSpline->getEndParam(end);*/
		for (i = 0 ; i < numPoints; ++i)
		{
			//t = (i * (end - start)) / (4 * numPoints) + start;
			pSpline->getFitPointAt (i, pt);
			//pSpline->getPointAtParam (t, pt);
			shape->SetShapePoint (i, GetLongPoint (pt));
		}
		/*pSpline->getPointAtParam (end-1e-8, pt);
		shape->SetShapePoint (i, GetLongPoint (pt));*/
	}
	else
	{
		numPoints = pSpline->numControlPoints ();
		shape = new CMDBezierShape (numPoints);
		for (i = 0 ; i < numPoints; ++i)
		{
			pSpline->getControlPointAt (i, pt);
			shape->SetShapePoint (i, GetLongPoint (pt, false));
		}
	}
	

	return shape;
}


CMDShape* COpenDWGio::DoLeader (OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(Leader);

	//if ( ( aden->leader.annohandle [ 0 ] == 0 ) && aden->leader.ldblob )
	//{
		// It seems these  are always black/white, but they don't come through that way. Override.
		//short oldColour = adenhd->entcolor ;
		//adenhd->entcolor = 7 ;

		bool bRet = true;
		bool first = true;
		
		OdGePoint3d pt0;
		OdGePoint3d pt1;
		if(pLeader->numVertices() == 0) return NULL;
		if(pLeader->isSplined()) {
			CMDCubicShape* newHdl = new CMDCubicShape(pLeader->numVertices(), false);
			for ( long i = 0; i < pLeader->numVertices() ; ++i )
				newHdl->SetShapePoint (i, GetLongPoint (pLeader->vertexAt(i), false));
			SetObjectAttributes(newHdl, pEntity);
			PenModel penModel = newHdl->GetPenModel();
			penModel.m_nEndCaps = (UCHAR)(( pLeader->hasArrowHead()) ? kVArrow : 0) ;
			newHdl->SetPenModel (penModel);
			newHdl->CalculateExtents ();
			AddObject (pLayer, newHdl);
		} else {
			CMDGroupShape* newHdl = new CMDGroupShape;
			pt0 = pLeader->vertexAt(0);
			for ( long i = 1; i < pLeader->numVertices() ; ++i )
			{
				pt1 = pLeader->vertexAt(i);
		
				if (bRet) {
					CMDLineShape *line = new CMDLineShape;
					if(!line)
						bRet = false;
					else {
						line->SetShapePoint (0, GetLongPoint (pt0));
						line->SetShapePoint (1, GetLongPoint (pt1));

						SetObjectAttributes(line, pEntity);
						PenModel penModel = line->GetPenModel();
						penModel.m_nEndCaps = (UCHAR)(( pLeader->hasArrowHead() && first ) ? kVArrow : 0) ;
						line->SetPenModel (penModel);
						//AddObject (pLayer, line);
						newHdl->AddMember(line);
					}
				}
				pt0 = pt1;
				first = false ;
			}

			if ( bRet ) {
				CMDLineShape *line = new CMDLineShape;
				if(!line)
					bRet = false;
				else {
					line->SetShapePoint (0, GetLongPoint (pt0));
					if(pLeader->hasHookLine()) {
						OdGeVector3d annoVec = pLeader->annotationXDir();
						double width = pLeader->annoWidth() + 4;
						if(pLeader->isHookLineOnXDir()) 
							pt1 -= width * annoVec; // Special, for extra bit on 'leaders' next to the text
						else 
							pt1 += width * annoVec; // Special, for extra bit on 'leaders' next to the text
					}
					LongPoint p = GetLongPoint (pt1) ;

					line->SetShapePoint (1, p);
					SetObjectAttributes(line, pEntity);
					PenModel penModel = line->GetPenModel();
					penModel.m_nEndCaps = (UCHAR)(( pLeader->hasArrowHead() && first ) ? kVArrow : 0) ;
					line->SetPenModel (penModel);
					//AddObject (pLayer, line);
					newHdl->AddMember(line);
				}
			}
			newHdl->CalculateExtents ();
			AddObject (pLayer, newHdl);
		}
	return NULL;
}


CMDShape* COpenDWGio::DoEllipse (OdDbEntity* pEntity)
{
	def_var(Ellipse);
	
	LongPoint ptCenter = GetLongPoint (pEllipse->center (), false);
	OdGeVector3d ptMajorAxis = pEllipse->majorAxis ();
	OdGeVector3d ptMinorAxis = pEllipse->minorAxis ();

	if (pEllipse->isClosed ())
	{
		CMDEllipse* newHdl = new CMDEllipse(CMDRectangle::kRectDiagonal);
		if (newHdl)
		{
			double angle;
			LVector2D v;
			LVector2D v1;
			CalculateEllipseParams(ptMajorAxis, ptMinorAxis, v, v1, angle);
			newHdl->SetShapePoint(0,ptCenter + LongPoint(v.x,v.y));
			newHdl->SetShapePoint(1,ptCenter + LongPoint( v1.x, v1.y));
			newHdl->SetShapePoint(2,ptCenter + LongPoint( -v.x, -v.y));
			newHdl->SetShapePoint(3,ptCenter + LongPoint( -v1.x,-v1.y));
			//newHdl->SetRotation (90 + m360Ang (m_cTransform.GetAngle () * kRadToAng));
			newHdl->SetRotation (m360Ang (angle * kRadToAng));
			return newHdl; 
		}
	}
	else // can not merge these two after the new as they are derived from different roots.
	{
		CMDArcShape *newHdl = new CMDArcShape (CMDArcShape::kElliptic);
		if (newHdl)
		{
			newHdl->m_nArcDirection = AD_CLOCKWISE;

//			double startAng = 0;
//			pEllipse->getStartParam (startAng);
//			double endAng = pi;
//			pEllipse->getEndParam (endAng);
			if (m_cTransform.IsMirrored ())
			{
//				startAng *= -1;
//				endAng *= -1;
				newHdl->m_nArcDirection = AD_COUNTERCLOCKWISE;
			}

			if(pEllipse->normal().z < 0) {
				if(newHdl->m_nArcDirection == AD_CLOCKWISE) newHdl->m_nArcDirection = AD_COUNTERCLOCKWISE;
				else newHdl->m_nArcDirection = AD_CLOCKWISE;
			}

			newHdl->SetShapePoint (0, ptCenter);
			
			OdGePoint3d ept;
			pEllipse->getStartPoint(ept);
			LongPoint pt = GetLongPoint(ept);
			newHdl->SetShapePoint (1, pt);

			pEllipse->getEndPoint(ept);
			pt = GetLongPoint(ept);
			newHdl->SetShapePoint (2, pt);
			double angle;
			LVector2D v;
			CalculateEllipseParams(ptMajorAxis, ptMinorAxis, v, angle);
			newHdl->SetShapePoint (3, ptCenter + LongPoint (v.x, v.y));
			newHdl->SetRotation (m360Ang (angle * kRadToAng));
		//}
			return newHdl;
		}
	}

	return NULL;
}

static void getAngDimLineEndPt (
	LongPointPtr		startPt,
	LongPointPtr		centerPt,
	LongPointPtr		lineP1,
	LongPointPtr		lineP2,
	LongPointPtr		endPt)
{
	LongPoint			linePt;

	linePt = *startPt;

	// Buggy code
	if ( Abs (lineP1->x - lineP2->x) > Abs (lineP1->y - lineP2->y))
	{
		if( Abs (lineP1->x - centerPt->x) > Abs (lineP2->x - centerPt->x)) 
			linePt = *lineP1;
		else 
			linePt = *lineP2;
		/*if (startPt->x > centerPt->x) {
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
		}*/
	}
	else
	{
		if( Abs (lineP1->y - centerPt->y) > Abs (lineP2->y - centerPt->y)) 
			linePt = *lineP1;
		else 
			linePt = *lineP2;
		/*if (startPt->y > centerPt->y)
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
		}*/
	}

	*endPt = linePt;
} /* getAngDimLineEndPt */


LongPoint COpenDWGio::getAngDimLineEndsPt (const double arcPt_x, const double arcPt_y,
								 const double line1sPt_x, const double line1sPt_y,
								 const double line1ePt_x, const double line1ePt_y,
								 const double line2sPt_x, const double line2sPt_y,
								 const double line2ePt_x, const double line2ePt_y)
{
	double a,b,c, delta, delta1, delta2;	
	a = line1sPt_y - line1ePt_y;
	b = - (line1sPt_x - line1ePt_x);
	c = line1ePt_y * line1sPt_x - line1sPt_y * line1ePt_x;

	delta = a * arcPt_x + b * arcPt_y + c; 
	delta1 = a * line2sPt_x + b * line2sPt_y + c; 
	delta2 = a * line2ePt_x + b * line2ePt_y + c; 
	if(delta * delta1 <= 0.0) return GetLongPoint(OdGePoint2d(line2ePt_x, line2ePt_y));
	if(delta * delta2 <= 0.0) return GetLongPoint(OdGePoint2d(line2sPt_x, line2sPt_y));
	if(Abs(delta1) > Abs(delta2)) return GetLongPoint(OdGePoint2d(line2sPt_x, line2sPt_y));
	return GetLongPoint(OdGePoint2d(line2ePt_x, line2ePt_y));
} /* getAngDimLineEndPt */


CMDShape* COpenDWGio::DoDimension (OdDbEntity* pEntity)
{
	def_var(Dimension);

	DimStandard std;
	short dsType= (m_pDoc->m_nUnit & kUEnglishMask) ? kDsEnglish:kDsMetric;
	std.FillDefaults (dsType);
	DimFormat fmt;		
	  
	unsigned char EndCap1,EndCap2;
	EndCap1=EndCap2=6;

	CMDAssocDim *pNewShape = 0;
	LongPoint startPt,endPt,edgePt1,anchorPt;

	if (pDimension->isKindOf (OdDbAlignedDimension::desc ()) ||
		pDimension->isKindOf (OdDbRotatedDimension::desc ()))

	{
		fmt.FillDefaults (kLinear, dsType);

		CMDLineDim::Mode mode = CMDLineDim::kSlope;
		CMDLineDim* newHdl;
		OdDbAlignedDimensionPtr pLine = pDimension->queryX (OdDbAlignedDimension::desc ());
		if (pLine.get ())
		{
			startPt = GetLongPoint (pLine->xLine1Point ());
			endPt = GetLongPoint (pLine->xLine2Point ());
			edgePt1 = GetLongPoint ( pLine->dimLinePoint ());
			anchorPt = GetLongPoint (pLine->textPosition ());

				LongPoint edgePt2;
				edgePt2.x = edgePt1.x + (anchorPt.x - edgePt1.x) * FixedTwo;
				edgePt2.y = edgePt1.y + (anchorPt.y - edgePt1.y) * FixedTwo;

				if (edgePt1.y == edgePt2.y) // horizontal
					newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, CMDLineDim::kHoriz);
				else if (edgePt1.x == edgePt2.x) // vertical
					newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, CMDLineDim::kVert);
				else
					newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, mode, mtAngle(startPt,endPt));
		}
		OdDbRotatedDimensionPtr pRotated = pDimension->queryX (OdDbRotatedDimension::desc ());
		if (pRotated.get ())
		{
			startPt = GetLongPoint (pRotated->xLine1Point ());
			endPt = GetLongPoint (pRotated->xLine2Point ());
			edgePt1 = GetLongPoint ( pRotated->dimLinePoint ());
			anchorPt = GetLongPoint (pRotated->textPosition ());
			mode = CMDLineDim::kPerp;
			double rot = pRotated->rotation();
			if((Abs(rot) < 1e-9) || (Abs(rot - _M_PI) < 1e-9)) {
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kHoriz);
				anchorPt.y = edgePt1.y;
			}
			else if((Abs(rot - _M_PI/2) < 1e-9)||(Abs(rot - 3*_M_PI/2) < 1e-9)) {
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kVert);
				anchorPt.x = edgePt1.x;
			}
			else newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, mode, mtAngle(startPt,endPt));				
			/*if (endPt.y == edgePt1.y && startPt.y == edgePt1.y)
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kHoriz);
			else if (endPt.x == edgePt1.x && startPt.x == edgePt1.x)
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kVert);
			else if (endPt.x == edgePt1.x && endPt.y == edgePt1.y) {
				if(Abs(rot) < 1) newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kHoriz);
				else newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kVert);
			}
			else if (endPt.y == edgePt1.y)// || startPt.y == edgePt1.y)
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kVert);
			else if (endPt.x == edgePt1.x )//|| startPt.x == edgePt1.x)
				newHdl=new CMDLineDim(std,fmt,EndCap1,EndCap2,CMDLineDim::kHoriz);
			else
			{
				LongPoint edgePt2;
				edgePt2.x = edgePt1.x + (anchorPt.x - edgePt1.x) * FixedTwo;
				edgePt2.y = edgePt1.y + (anchorPt.y - edgePt1.y) * FixedTwo;

				if (edgePt1.y == edgePt2.y) // horizontal
					newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, CMDLineDim::kHoriz);
				else if (edgePt1.x == edgePt2.x) // vertical
					newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, CMDLineDim::kVert);
				else
					newHdl=new CMDLineDim (std, fmt, EndCap1, EndCap2, mode, mtAngle(startPt,endPt));				
			}*/
		}


		//CMDLineDim* newHdl;

		

		newHdl->SetShapePoint(0,startPt);
		newHdl->SetShapePoint(1,endPt);
// Some strange dimmensions have the default text position attribute and wrong actual 
// text position property. Need to uncommnet following code, but don't work well on other dimensions.
//		if(pDimension->isUsingDefaultTextPosition()) {
//			newHdl->SetShapePoint(7, edgePt1);
//		} else 
		newHdl->SetShapePoint(2,anchorPt);
		pNewShape=newHdl;
	}
	else
	if (pDimension->isKindOf (OdDbDiametricDimension::desc ()) ||
		pDimension->isKindOf (OdDbRadialDimension::desc ()))
	{
		fmt.FillDefaults(kRadial,dsType);
		OdDbDiametricDimensionPtr pDiameter = pDimension->queryX (OdDbDiametricDimension::desc ());
		LongPoint ptEnd;
		LongPoint ptCenter;
		if (pDiameter.get ())
		{
			ptEnd=GetLongPoint (pDiameter->chordPoint (), false);
			ptCenter=Average (GetLongPoint(pDiameter->farChordPoint(), false),ptEnd);
		}
		else
		{
			OdDbRadialDimensionPtr pRadius = pDimension->queryX (OdDbRadialDimension::desc ());
			if (pRadius.get ())
			{
				ptCenter = GetLongPoint (pRadius->center (), false);
				ptEnd=GetLongPoint (pRadius->chordPoint (), false);
			}
		}

		Coord radius = Pythag (ptCenter - ptEnd);
		pNewShape = new CMDRadialDim (std, fmt, EndCap1, EndCap2,
			pDimension->isKindOf (OdDbDiametricDimension::desc ()), ptCenter, radius,
			GetLongPoint( pDimension->textPosition ()));
	}
	else 
	if (pDimension->isKindOf (OdDb3PointAngularDimension::desc ()) ||
		pDimension->isKindOf (OdDb2LineAngularDimension::desc ()))
	{
		fmt.FillDefaults(kAngular,dsType);

		LongPoint lp1, lp2, lp3, lp4, refPt1, refPt2, centerPt;
		OdGePoint3d pt1, pt2, pt3, pt4, apt;
		OdDb3PointAngularDimensionPtr pAngular3pt = pDimension->queryX (OdDb3PointAngularDimension::desc ());
		if (pAngular3pt.get())
		{
			lp1 = GetLongPoint (pAngular3pt->xLine1Point ());// 13
			lp3 = GetLongPoint (pAngular3pt->xLine2Point ()); //14
			lp2 = lp4 = GetLongPoint ( pAngular3pt->centerPoint (), false); // 15 center point
			pt1 = pAngular3pt->xLine1Point ();
			pt3 = pAngular3pt->xLine1Point ();
			pt2 = pt3 = pAngular3pt->centerPoint ();
			apt = pAngular3pt->arcPoint (); //14
		}
		else
		{									
			OdDb2LineAngularDimensionPtr pLineAngular = pDimension->queryX (OdDb2LineAngularDimension::desc ());
			lp1 = GetLongPoint (pLineAngular->xLine1Start ());
			lp2 = GetLongPoint (pLineAngular->xLine1End ());
			lp3 = GetLongPoint (pLineAngular->xLine2Start ()); // 10
			lp4 = GetLongPoint (pLineAngular->xLine2End ());
			pt1 = pLineAngular->xLine1Start ();
			pt2 = pLineAngular->xLine1End ();
			pt3 = pLineAngular->xLine2Start ();
			pt4 = pLineAngular->xLine2End ();
			apt = pLineAngular->arcPoint (); //14
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
//			LongPoint tpt = GetLongPoint (pDimension->textPosition ());
			//getAngDimLineEndPt (&lp1, &centerPt, &lp1, &lp2, &refPt1);
			//getAngDimLineEndPt (&lp3, &centerPt, &lp3, &lp4, &refPt2);
			//getAngDimLineEndPt (&tpt, &centerPt, &lp1, &lp2, &refPt1);
			//getAngDimLineEndPt (&tpt, &centerPt, &lp3, &lp4, &refPt2);
			refPt1 = getAngDimLineEndsPt(apt.x, apt.y, pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);
			refPt2 = getAngDimLineEndsPt(apt.x, apt.y, pt3.x, pt3.y, pt4.x, pt4.y, pt1.x, pt1.y, pt2.x, pt2.y);
		}
		
		pNewShape = new CMDAngularDim (std,fmt,6,6,true);
		pNewShape->SetShapePoint (0, refPt1); 
		pNewShape->SetShapePoint (1, refPt2); 
		pNewShape->SetShapePoint (2, GetLongPoint (pDimension->textPosition ()));
		pNewShape->SetShapePoint (12,centerPt);
	}	

	if(pNewShape)
	{
		//pNewShape->SetRotation (m360Ang (m_cTransform.GetAngle () * kRadToAng));
		CDocMapper mapper(m_pDoc);
		pNewShape->ReCalc(&mapper);
		CPCDraftView* pView=NULL;
		POSITION pos=m_pDoc->GetFirstViewPosition();
		if(pos) pView=(CPCDraftView*)m_pDoc->GetNextView (pos); // 1 view assumed
		if(pView) pNewShape->UpdateDependentShapes(pView);
	}
	else
	{
		BadObjectAlert (IDS_DWGDIMENSION);
	}

	return pNewShape;
}


//CMDShape* COpenDWGio::DoText (OdDbEntity* pEntity)
//{
//	def_var(Text);
/*	
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
*/
 // return NULL;

//}


/*

bool COpenDWGio::DoRay (OdDbEntity*);
bool COpenDWGio::Do2dPolyline (OdDbEntity*);
bool COpenDWGio::DoXline (OdDbEntity*);

//Entities
bool COpenDWGio::DoCurve (OdDbEntity*);
bool COpenDWGio::DoRegion (OdDbEntity*);
bool COpenDWGio::DoHatch (OdDbEntity*);
bool COpenDWGio::DoFace (OdDbEntity*);
bool COpenDWGio::DoArcAlignedText (OdDbEntity*);
bool COpenDWGio::DoVertex (OdDbEntity*);
bool COpenDWGio::DoRText (OdDbEntity*);
bool COpenDWGio::DoImage (OdDbEntity*);
bool COpenDWGio::DoText (OdDbEntity*);
bool COpenDWGio::DoPolygonMesh (OdDbEntity*);
bool COpenDWGio::DoFaceRecord (OdDbEntity*);
bool COpenDWGio::DoPolyFaceMesh (OdDbEntity*);
bool COpenDWGio::DoShape (OdDbEntity*);
bool COpenDWGio::DoProxyEntity (OdDbEntity*);
bool COpenDWGio::DoTrace (OdDbEntity*);
bool COpenDWGio::DoFcf (OdDbEntity*);
bool COpenDWGio::DoSequenceEnd (OdDbEntity*);
bool COpenDWGio::Do3dSolid (OdDbEntity*);
bool COpenDWGio::DoFrame (OdDbEntity*);
bool COpenDWGio::DoBody (OdDbEntity*);
bool COpenDWGio::DoMline (OdDbEntity*);
bool COpenDWGio::DoSolid (OdDbEntity*);
bool COpenDWGio::DoEntityWithGrData (OdDbEntity*);
*/

const double kTextScaleFactor			= 100.0 / 72.0; // don't ask me why, it just seems to be about the right factor
                                                        // Possible - 1 inch is a 72 text size points

int COpenDWGio::CreateText(const char* text,CStyledText &stText,CStyledTextStyle Style)
{
	int nCount=0;
	
	const char* textEnd = text ;
	while ( *textEnd != 0 )
		++textEnd ;
	--textEnd ;

	bool styleAllowedAnywhere = true ;
	long styled = 0 ;
	CList<CStyledTextStyle,CStyledTextStyle&> textParamStack;
	CStyledTextStyle Style1 = Style;
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
			if (!textParamStack.IsEmpty())
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

			case 'm': // ??unicode eg \M+Nxxxx
			case 'M':
				++text;
				if (*text == '+') // could not work out if there is a + or not, so skip if there
					++text;
				++text;
				// next 4 chars are the unicode value for the char
				{
					char tmp[3];
					tmp[0] = *text++;
					tmp[1] = *text++;
					tmp[2] = 0;
					int mbcs_char;
					sscanf(tmp, "%2x", &mbcs_char);
					stText.Add((TCHAR)mbcs_char,Style);
					nCount++;
					tmp[0] = *text++;
					tmp[1] = *text;
					sscanf(tmp, "%2x", &mbcs_char);
					stText.Add((TCHAR)mbcs_char,Style);
					nCount++;
				}
				break;

			case 'l': // underline ?
				Style.m_LogFont.lfUnderline=0;
				break;
			case 'L': // underline ?
				Style.m_LogFont.lfUnderline=1;
				break;

			case 'P': // paragraph
			case 'p':
				stText.Add('\n',Style);
				break;

			case 'C': // Color?
				{
					++text;
					OdString fontColor = "";
					while ( ( text < textEnd ) && ( *text != ';' ) )
					{
						fontColor += *text;
						++text ;
					}
					// SMK.SSA
					// unsigned char col = (unsigned char)atoi(fontColor.c_str());
					unsigned char col = (unsigned char)atoi(fontColor);
					if(col == 7) col = 0;
					if(col == 255)
						Style.m_TextColor = RGB( 179, 179, 179 );
					else 
						Style.m_TextColor = RGB( OdCmEntityColor::mLUT[col][0], OdCmEntityColor::mLUT[col][1], OdCmEntityColor::mLUT[col][2]);
				}
				break;

			case '\\':
			case '{':
			case '}':
				stText.Add(*text,Style);
				nCount++;
				break;

			case 'F': // ?? diffent format ?? what does '//Fcomplex.shx;' mean ??? RPH
			case 'f': // font
				{
					//if(styled == 0) break;

					++text ;
					CString fontName ;
					while ( ( text < textEnd ) && ( *text != ';' ) && ( *text != '|' ) )
					{
						fontName+=*text;
						++text ;
					}
					CString shx( _T(".shx"));
					CString ext = fontName.Right(4);
					ext.MakeLower();
					if(ext == shx) {
						fontName = fontName.Left(fontName.ReverseFind('.'));
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
							//Style.m_LogFont.lfWeight=(*text++ == '0')?FW_NORMAL:FW_BOLD;
							Style.m_LogFont.lfWeight=(*text == '0')?FW_NORMAL:FW_BOLD;
							break;
						case 'i' :
							++text;
							//Style.m_LogFont.lfItalic=(*text++ == '0')?0:1;
							Style.m_LogFont.lfItalic=(*text == '0')?0:1;
							break;
						case 'c' : // Not tested - no sample file
						case 'C' : // May be color ??
							++text;
							//Style.m_LogFont.??=(*text++ == '0')?0:1;
							//++text ;
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
				stText.Add('°',Style);
				nCount++;
				break;
			case 'p': // + or - symbol
			case 'P':
				//stText.Add('±',Style); Dennis
#ifdef _JAPANESE
				stText.Add('}', Style);
#else
				stText.Add('±', Style);
#endif
				nCount++;
				break;
			case 'c': // ¯ symbol
			case 'C':
				Style1 = Style;
				strncpy(Style1.m_LogFont.lfFaceName,"Symbol",LF_FACESIZE);
				stText.Add((unsigned char)0xC6,Style1);
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
				// 127 is Esymbol
				short num = *text++ - '0';
				num *= 10;
				num += *text++ - '0';
				num *= 10;
				num += *text - '0'; // do not increment as is incremented at end
				if (num == 127)
					stText.Add('E',Style);//*p++ = 'E;
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


/*CMDShape* COpenDWGio::DoMText( OdDbEntity* pEntity) // MultiLine text
{
	def_var(MText);

	return NULL;
}*/

//bool COpenDWGio::DoMacDraftText( PAD_ENT_HDR adenhd,double* pt0, double* rotPt,char* text,double &height,long justification,long vertalignment,double &angle, bool italicText,PAD_LAY adlayer, CMDText** object/*=NULL */)
CMDShape* COpenDWGio::DoText( OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(Text);
	
	TextParams mySettings;

	double textHeight = pText->height ();
	LVector2D v (textHeight,0);
	v *= m_cTransform;
	textHeight *= kTextScaleFactor;
	//if (textHeight < 0)
	//textHeight = 9.0;
	//textHeight *= m_gDWGScale;
	//textHeight *= m_gDWGScale * gYScale;
	//if (textHeight < 1.0)
	//	textHeight = 1.0;

	CWindowDC hDC(NULL);
	// Need a little fix - if font size is too small, should substitute 1 or 2 as the smallest possible font.
	mySettings.m_lf.lfHeight=-MulDiv((int) (v.length () * kTextScaleFactor),GetDeviceCaps(hDC,LOGPIXELSY),72);
	if(mySettings.m_lf.lfHeight > -2) mySettings.m_lf.lfHeight = -2;

	// Default font for Autocad - Txt.ttf or Bigfont for large charsets
#ifdef _JAPANESE
	strncpy(mySettings.m_lf.lfFaceName,"BigFont",LF_FACESIZE);
#else
	strncpy(mySettings.m_lf.lfFaceName,"Txt",LF_FACESIZE);
#endif

	OdDb::TextHorzMode justification = pText->horizontalMode ();
	OdDb::TextVertMode vertalignment = pText->verticalMode ();

	//LongPoint leftPt = GetLongPoint (pText->alignmentPoint ());
	// Position point
	LongPoint leftPt;
	// Rotation point
	//LongPoint rotPt = GetLongPoint (pText->alignmentPoint ());

	if((justification == OdDb::kTextLeft && vertalignment == OdDb::kTextBase) ||
		justification == OdDb::kTextAlign ||
		justification == OdDb::kTextFit)
		leftPt = GetLongPoint (pText->position ());
	else 
		leftPt = GetLongPoint (pText->alignmentPoint ());
	if(justification == OdDb::kTextCenter  || justification == OdDb::kTextMid)	
	{
		mySettings.m_nJust=kAlignCenter;
	}
	else if (justification == OdDb::kTextRight)
	{
		mySettings.m_nJust=kAlignRight;
	}
	else
	{
		mySettings.m_nJust=kAlignLeft;
	}

	if ( vertalignment == OdDb::kTextVertMid || justification == OdDb::kTextMid )
		mySettings.m_nVAlign = kAlignMiddle;
	else if (vertalignment == OdDb::kTextTop)
		mySettings.m_nVAlign = kAlignTop;
	else // AD_TEXT_VALIGN_BASELINE or AD_TEXT_VALIGN_BOTTOM
		mySettings.m_nVAlign = kAlignBottom;

	CStyledText stText;
	CStyledTextStyle stsText;

	// Tries to read Autocad style record
	OdDbTextStyleTableRecordPtr txtst_ptr = pText->textStyle().openObject();

	bool bold, italic;
	int charset, pitch;
	OdString typeFace;
	txtst_ptr->font(typeFace, bold, italic, charset, pitch);
	mySettings.m_lf.lfWeight = (bold)?FW_BOLD:FW_NORMAL;
	mySettings.m_lf.lfItalic = (italic)?1:0;
	//mySettings.m_lf.lfCharSet = charset;
	mySettings.m_lf.lfPitchAndFamily = pitch;
	// SMK.SSA if(typeFace.getLength()) strncpy(mySettings.m_lf.lfFaceName, typeFace.getBuffer(LF_FACESIZE), LF_FACESIZE);
	if(typeFace.getLength()) strncpy(mySettings.m_lf.lfFaceName, (const char*)typeFace.getBuffer(LF_FACESIZE), LF_FACESIZE);
	else {
		OdString fileName = txtst_ptr->fileName();
		CString shx( _T(".shx"));
        // SMK.SSA
		// CString fontName(fileName.c_str());
        CString fontName((LPCWSTR)fileName);
		CString ext = fontName.Right(4);
		ext.MakeLower();
		if(ext == shx) {
			fontName = fontName.Left(fontName.ReverseFind('.'));
		}
		if(fileName.getLength()) strncpy(mySettings.m_lf.lfFaceName, fontName, LF_FACESIZE);
	}

	stsText.m_LogFont=mySettings.m_lf;
/*	stsText.m_TextColor=RGB(g_clut_32000[adenhd->entcolor][1]>>8,
				g_clut_32000[adenhd->entcolor][2]>>8,
				g_clut_32000[adenhd->entcolor][3]>>8);
*/
	CMDText *temp = new CMDText();
	SetObjectAttributes(temp, pEntity);
	PenModel penModel = temp->GetPenModel ();
	stsText.m_TextColor = penModel.m_crColor;
	delete temp;
	/*if(pText->color().colorIndex() == 255)
		stsText.m_TextColor = RGB (179, 179, 179);
	else 
		stsText.m_TextColor = RGB (pText->color ().red (),pText->color ().green (),pText->color ().blue ());
	*/
	OdString str = pText->textString ();
	if(pText->isKindOf (OdDbAttribute::desc ())) {
		OdDbAttributePtr pAttr = pText->queryX (OdDbAttribute::desc ());
		if(pAttr->isInvisible()) return NULL;
	}
	if(pText->isKindOf (OdDbAttributeDefinition::desc())) {
		OdDbAttributeDefinitionPtr pAttr = pText->queryX (OdDbAttributeDefinition::desc ());
		if(!pAttr->isConstant()) return NULL;
	}
	
	// SMK.SSA
	// if(CreateText(str.c_str (),stText,stsText))
	if(CreateText(str,stText,stsText))
	{
		CMDText *newHdl=new CMDText();
		newHdl->SetTextParams(mySettings);
		newHdl->SetText(stText);
		//SetObjectAttributes(newHdl, pEntity);

		LongPoint boxSize;
		{
			CWindowDC cdc(NULL);
			CDocMapper mapper(m_pDoc);
			CDraftContext dc(&cdc,&mapper,m_pDoc);
			CRect rc;
			dc.CalcBoundRect(stText,mySettings,rc, true);
			boxSize.x=rc.right-rc.left+6;
			boxSize.y=rc.bottom-rc.top+6;
		}

//		LongPoint lPt = GetLongPoint (pText->position ());
//		LongPoint lPt2 = lPt + boxSize; //GetLongPoint (pText->alignmentPoint ());
//		lPt = mtRotPoint ( lPt , TSinCos ( RadiansToDegrees ( angle ) ) , GetLongPoint ( rotPt ) ) ;
		LongRect viewRect;
		viewRect.left = viewRect.right = leftPt.x;
		viewRect.top = viewRect.bottom = leftPt.y;

		if ( mySettings.m_nJust == kAlignCenter)
		{
			//viewRect.left = viewRect.right = (leftPt.x + rightPt.x) / FixedTwo;
			Coord x = boxSize.x / FixedTwo ;
			viewRect.left -= x ;
			viewRect.right += x ;
		}
		else if ( mySettings.m_nJust == kAlignRight )
		{
			//viewRect.left = viewRect.right = rightPt.x;
			viewRect.left -= boxSize.x ;
		}
		else
		{
			//viewRect.left = viewRect.right = leftPt.x;
			viewRect.right += boxSize.x ;
		}

		if ( mySettings.m_nVAlign == kAlignMiddle )
		{
			//viewRect.top = viewRect.bottom = (leftPt.y + rightPt.y) / FixedTwo;
			Coord y = boxSize.y / FixedTwo ;
			viewRect.top -= y ;
			viewRect.bottom += y ;
		}
		else if ( mySettings.m_nVAlign == kAlignBottom )
		{
			//viewRect.top = viewRect.bottom = rightPt.y;
			viewRect.top -= boxSize.y ;
		}
		else
		{
			//viewRect.top = viewRect.bottom = leftPt.y;
			viewRect.bottom += boxSize.y ;
		}
		

//		SetObjectAttributes (newHdl, adenhd, adlayer);
		Angle aAngle;
		//if(m_cTransform.IsMirrored())
			aAngle= m360Ang ( (pText->rotation () + m_cTransform.GetAngle() )* kRadToAng + 180);
/*		else 
			aAngle= m360Ang ( (pText->rotation () + m_cTransform.GetAngle() )* kRadToAng );*/
		for(int i=0;i<4;i++)
		{
			newHdl->SetShapePoint(i, mtRotPoint(viewRect.GetCorner(i), aAngle, leftPt));
		}
		newHdl->SetRotation(aAngle);
 
//		newHdl->CalculateExtents();
//		AddObject (layer, newHdl);

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
		CWindowDC hDC(NULL);
		CFont* pOldFont,*pFont = new CFont();
		pFont->CreateFontIndirect(&stsText.m_LogFont);
		pOldFont=hDC.SelectObject(pFont);
		TEXTMETRIC tm;
		hDC.GetTextMetrics(&tm);
		hDC.SelectObject(pOldFont);
		delete pFont;

		/*if( mySettings.m_nVAlign ==  kAlignMiddle )
			y = kBotRightMargin + short ( tm.tmDescent / 6 ); // +/- ? */
		if( mySettings.m_nVAlign ==  kAlignBottom && vertalignment == OdDb::kTextBase )
			y = kBotRightMargin + short ( tm.tmDescent ); // +/- ? // maybe 3/4?
		/*else if( mySettings.m_nVAlign ==  kAlignBottom )
			y = kBotRightMargin + short ( tm.tmDescent / 4 ); // +/- ? 
		if( mySettings.m_nVAlign ==  kAlignTop )
			y = kBotRightMargin - short ( tm.tmDescent / 2 ); // +/- ? */
		//y-=short(tm.tmInternalLeading);

		if ( x != FixedZero | y != FixedZero )
			newHdl->DoShapeOffset(LongPoint(x, y));


		//if (object) *object = newHdl;
		newHdl->CalculateExtents ();
		if(pText->isKindOf (OdDbAttribute::desc ()))
			SetObjectAttributes (newHdl, pEntity, true);
		AddObject (pLayer, newHdl);
	}
	return NULL;
}

//bool CDWGImport::DoMTEXT(PAD_ENT_HDR adenhd,PAD_ENT aden,PAD_LAY adlayer)
CMDShape* COpenDWGio::DoMText( OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(MText);
	
	TextParams mySettings;

//	CLayer *layer = GetCorrectLayer (adlayer);
//	if (!layer)
//		return true;


//	InitText (adenhd->entcolor, aden->mtext.ht, adlayer, mySettings);
	double textHeight = pMText->textHeight ();
	LVector2D v (textHeight,0);
	v *= m_cTransform;
	double tH = v.length() * kTextScaleFactor;
	
	CWindowDC hDC(NULL);
	mySettings.m_lf.lfHeight=-MulDiv((int)(tH),GetDeviceCaps(hDC,LOGPIXELSY),72);
	if(mySettings.m_lf.lfHeight > -2) mySettings.m_lf.lfHeight = -2;

	// Default font for Autocad - Txt.ttf or Bigfont for large charsets
#ifdef _JAPANESE
	strncpy(mySettings.m_lf.lfFaceName,"BigFont",LF_FACESIZE);
#else
	strncpy(mySettings.m_lf.lfFaceName,"Txt",LF_FACESIZE);
#endif

	if(pMText->lineSpacingFactor() < 1.2) mySettings.m_nSpacing = kSingleSpace;
	else if(pMText->lineSpacingFactor() < 1.7) mySettings.m_nSpacing = k1_5Space;
	else mySettings.m_nSpacing = kDoubleSpace;

	OdDb::TextHorzMode justification = pMText->horizontalMode ();
	OdDb::TextVertMode vertalignment = pMText->verticalMode ();
	
//	LongPoint leftPt = GetLongPoint (pMText->position ());
//	LongPoint rightPt = GetLongPoint (pMText->alignmentPoint ());

	switch (pMText->attachment())
	{
		case OdDbMText::kMiddleLeft:
		case OdDbMText::kBottomLeft:
			mySettings.m_nJust=kAlignLeft;
			break;
		case OdDbMText::kTopCenter:
		case OdDbMText::kMiddleCenter:
		case OdDbMText::kBottomCenter:
			mySettings.m_nJust=kAlignCenter;
			break;
		case OdDbMText::kTopRight:
		case OdDbMText::kMiddleRight:
		case OdDbMText::kBottomRight:
			mySettings.m_nJust=kAlignRight;
			break;
		default:
			mySettings.m_nJust=kAlignLeft;
			break;
	}

	switch (pMText->attachment())
	{
		case OdDbMText::kTopLeft:
		case OdDbMText::kTopCenter:
		case OdDbMText::kTopRight:
			mySettings.m_nVAlign=kAlignTop;
			break;
		case OdDbMText::kMiddleLeft:
		case OdDbMText::kMiddleCenter:
		case OdDbMText::kMiddleRight:
			mySettings.m_nVAlign=kAlignMiddle;
			break;
		case OdDbMText::kBottomLeft:
		case OdDbMText::kBottomCenter:
		case OdDbMText::kBottomRight:
			mySettings.m_nVAlign=kAlignBottom;
			break;
		default:
			mySettings.m_nVAlign=kAlignTop;
			break;
	}

	// Tries to read Autocad style record
	OdDbTextStyleTableRecordPtr txtst_ptr = pMText->textStyle().openObject();

	bool bold, italic;
	int charset, pitch;
	OdString typeFace;
	txtst_ptr->font(typeFace, bold, italic, charset, pitch);
	mySettings.m_lf.lfWeight = (bold)?FW_BOLD:FW_NORMAL;
	mySettings.m_lf.lfItalic = (italic)?1:0;
	//mySettings.m_lf.lfCharSet = charset;
	mySettings.m_lf.lfPitchAndFamily = pitch;
	// SMK.SSA
	// if(typeFace.getLength()) strncpy(mySettings.m_lf.lfFaceName, typeFace.getBuffer(LF_FACESIZE), LF_FACESIZE);
	if(typeFace.getLength()) strncpy(mySettings.m_lf.lfFaceName, (const char*) typeFace.getBuffer(LF_FACESIZE), LF_FACESIZE);
	else {
		OdString fileName = txtst_ptr->fileName();
		CString shx( _T(".shx"));
        // SMK.SSA
		// CString fontName(fileName.c_str());
        CString fontName((LPCWSTR)fileName);
		CString ext = fontName.Right(4);
		ext.MakeLower();
		if(ext == shx) {
			fontName = fontName.Left(fontName.ReverseFind('.'));
		}
		if(fileName.getLength()) strncpy(mySettings.m_lf.lfFaceName, fontName, LF_FACESIZE);
	}


	CStyledText stText;
	CStyledTextStyle stsText;
	stsText.m_LogFont=mySettings.m_lf;
//	stsText.m_TextColor=RGB(g_clut_32000[adenhd->entcolor][1]>>8,
//				g_clut_32000[adenhd->entcolor][2]>>8,
//				g_clut_32000[adenhd->entcolor][3]>>8);;
	CMDText *temp = new CMDText();
	SetObjectAttributes(temp, pEntity);
	PenModel penModel = temp->GetPenModel ();
	stsText.m_TextColor = penModel.m_crColor;
	delete temp;
	/*if(pMText->color().colorIndex() == 255)
		stsText.m_TextColor = RGB (179, 179, 179);
	else 
		stsText.m_TextColor = RGB (pMText->color ().red (),pMText->color ().green (),pMText->color ().blue ());
	*/

	CString strText = (const char*) pMText->contents();
	/*if (aden->mtext.ldblob == AD_VMNULL) // text held in aden->mtext.textstr
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
	}*/
	
	if(CreateText (strText,stText,stsText))
	{
		LongPoint boxSize;
		CMDText::TextType nType=CMDText::kTextAutoSize;
		//if (aden->mtext.boxht || aden->mtext.boxwid) // one of these can be zero
		if(pMText->actualHeight() || pMText->actualWidth())
		{
			nType=CMDText::kTextUserSize;
			//LVector2D v (pMText->actualHeight(), pMText->actualWidth());
			//v *= m_cTransform;
			textHeight *= kTextScaleFactor;
			//if (aden->mtext.boxwid)
			if(pMText->actualWidth()) {
				LVector2D v (pMText->actualWidth(), 0);
				v *= m_cTransform;
				boxSize.x = v.length() + 2*tH + 6;
			}
			else {
				// work out the width from textPtr??
				LVector2D v (pMText->actualHeight(), 0);
				v *= m_cTransform;
				boxSize.x = v.length() + 2*tH + 6;
			}
				
			if (pMText->actualHeight()) {
				LVector2D v (0, pMText->actualHeight());
				v *= m_cTransform;
				boxSize.y = v.length() + tH * 0.5 + 6;
			}
			else {
				LVector2D v (0, pMText->actualWidth());
				v *= m_cTransform;
				boxSize.y = v.length() + tH * 0.5 + 6;
			}
		}
		else
		{
			CWindowDC cdc(NULL);
			CDocMapper mapper(m_pDoc);
			CDraftContext dc(&cdc,&mapper,m_pDoc);
			CRect rc;
			if(nType==CMDText::kTextAutoSize)
			{
				dc.CalcBoundRect(stText,mySettings,rc, true);
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

		OdGePoint3d pt0 = pMText->location();
		LongPoint lPt = GetLongPoint (pt0);
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
		//SetObjectAttributes(newHdl, pEntity);
        int i = 0;
		for(;i<4;i++)
			newHdl->SetShapePoint(i,viewRect.GetCorner(i));
		
		OdGeVector3d odv3d = pMText->direction();
		//xdir!!
		if ( Abs ( odv3d.x ) == 1.0 )
			odv3d.y = 0.0 ; // The are some E minus 17 numbers which come through, probably erroneously
		else
		if ( Abs ( odv3d.y ) == 1.0 )
			odv3d.x = 0.0 ; // The are some E minus 17 numbers which come through, probably erroneously

		/*CWindowDC hDC(NULL);
		CFont* pOldFont,*pFont = new CFont();
		pFont->CreateFontIndirect(&stsText.m_LogFont);
		pOldFont=hDC.SelectObject(pFont);
		TEXTMETRIC tm;
		hDC.GetTextMetrics(&tm);
		hDC.SelectObject(pOldFont);
		delete pFont;

		Coord y = -short(tm.tmInternalLeading)+short(tm.tmExternalLeading);

		if (y != FixedZero )
			newHdl->DoShapeOffset(LongPoint(0, y));
		*/
		
		LVector2D textVec ;
		textVec.x = odv3d.x ;
		textVec.y = odv3d.y ;
		 
		textVec *= m_cTransform ;
		
		Angle angle = m360Ang ( atan2 ( -textVec.y , textVec.x ) * kRadToAng ) ;
		//Angle angle = m360Ang( pMText->rotation() * kRadToAng );
		for(i=0;i<4;i++)
			newHdl->SetShapePoint(i,mtRotPoint(newHdl->GetShapePoint(i),angle,lPt));
		newHdl->SetRotation(angle);

		newHdl->CalculateExtents();
		AddObject (pLayer, newHdl);
	}
	return NULL;
}

//bool CDWGImport::DoTrace(PAD_ENT_HDR adenhd,PAD_LAY adlayer,double *pt0,double *pt1,double *pt2,double *pt3)
CMDShape* COpenDWGio::DrawTrace(OdDbEntity* pEntity, CLayer* pLayer, OdGePoint3d& pt0, OdGePoint3d& pt1, OdGePoint3d& pt2, OdGePoint3d& pt3)
{
	CMDPolygonShape *newHdl=new CMDPolygonShape(CMDPolygonShape::kPolygon,4,true);

	newHdl->SetShapePoint(0,GetLongPoint(pt0));
	newHdl->SetShapePoint(1,GetLongPoint(pt1));
	newHdl->SetShapePoint(3,GetLongPoint(pt2));
	newHdl->SetShapePoint(2,GetLongPoint(pt3));

	newHdl->CalculateExtents();
	SetObjectAttributes (newHdl, pEntity, false, true);
	AddObject (pLayer, newHdl);

	return newHdl;
}

CMDShape* COpenDWGio::DoTrace(OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(Trace);
	
	OdGePoint3d pt0, pt1, pt2, pt3;
	pTrace->getPointAt(0, pt0);
	pTrace->getPointAt(1, pt1);
	pTrace->getPointAt(2, pt2);
	pTrace->getPointAt(3, pt3);

	CMDShape *newHdl= DrawTrace(pEntity, pLayer, pt0, pt1, pt2, pt3);

	return NULL;
}

CMDShape* COpenDWGio::DoSolid(OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(Solid);
	
	OdGePoint3d pt0, pt1, pt2, pt3;
	pSolid->getPointAt(0, pt0);
	pSolid->getPointAt(1, pt1);
	pSolid->getPointAt(2, pt2);
	pSolid->getPointAt(3, pt3);

	CMDShape *newHdl= DrawTrace(pEntity, pLayer, pt0, pt1, pt2, pt3);

	return NULL;
}

CMDShape* COpenDWGio::DoFace(OdDbEntity* pEntity, CLayer* pLayer)
{
	def_var(Face);
	
	OdGePoint3d pt0, pt1, pt2, pt3;
	pFace->getVertexAt(0, pt0);
	pFace->getVertexAt(1, pt1);
	pFace->getVertexAt(2, pt2);
	pFace->getVertexAt(3, pt3);

	CMDShape *newHdl= DrawTrace(pEntity, pLayer, pt0, pt1, pt2, pt3);

	return NULL;
}

CMDShape* COpenDWGio::Do3dPolyline (OdDbEntity* pEntity)
{
	def_var(3dPolyline);

	OdDbObjectIteratorPtr iter = p3dPolyline->vertexIterator();
	int count = 0;
    for (iter->start(); !iter->done(); iter->step()) {
		OdDb3dPolylineVertexPtr pt_ptr = iter->entity();
		OdDb::Vertex3dType tp = pt_ptr->vertexType();
		if((pt_ptr->vertexType() == OdDb::k3dSimpleVertex) || (pt_ptr->vertexType() == OdDb::k3dFitVertex)) count++;
	}

	CMDPolygonShape* shape = NULL; 

	switch (p3dPolyline->polyType ())
	{
		case OdDb::k3dSimplePoly:
			{
				CMDPolygonShape* newHdl = new CMDPolygonShape (CMDPolygonShape::kPolygon,count, p3dPolyline->isClosed ());
				long i=0;
				iter = p3dPolyline->vertexIterator();
				for (iter->start(); !iter->done(); iter->step())
				{
					OdDb3dPolylineVertexPtr pt_ptr = iter->entity();
					newHdl->SetShapePoint (i, GetLongPoint (pt_ptr->position()));
					++i;
				}
				return newHdl;
			}
			break;
		case OdDb::k3dQuadSplinePoly:
			{
				BadObjectAlert (IDS_DWGQUADRATICBSPLINE);
				return NULL;
			}
			break;
		case OdDb::k3dCubicSplinePoly:
			{
				//if(p3dPolyline->isClosed()) count--;
				CMDCubicShape* newHdl = new CMDCubicShape (count, p3dPolyline->isClosed ());
				long i=0;
				iter = p3dPolyline->vertexIterator();
				iter->start();

				// Need to skip first vertex
				//if(p3dPolyline->isClosed ()) 
				iter->step();
				for (; !iter->done(); iter->step())
				{
					OdDb3dPolylineVertexPtr pt_ptr = iter->entity();
					if((i == 0)&&(p3dPolyline->isClosed() == false)) newHdl->SetShapePoint (i++, GetLongPoint (pt_ptr->position())); 
					else if(pt_ptr->vertexType() == OdDb::k3dFitVertex) newHdl->SetShapePoint (i++, GetLongPoint (pt_ptr->position()));
				}
				return newHdl;
			}
			break;
		}
	return NULL;
}

void COpenDWGio::BadObjectAlert(UINT nID)
{
	if(m_bShowBadObjectAlerts)
	{
		for(int i=0;i<m_BadObjects.GetSize();i++)
		{
			if(m_BadObjects[i]==nID)
				return;
		}
		m_BadObjects.Add(nID);
		CString strObject;
		strObject.LoadString(nID);
		CBadDWGObjectDlg theDlg;
		
		theDlg.m_strText.Format(IDS_BADDWGOBJECT,strObject);

		if(IDCANCEL==theDlg.DoModal())
		{
			gUserCanceled=true;
			return;
		}
		if(theDlg.m_bDontShow) m_bShowBadObjectAlerts=false;
	}
	return;
}

CMDShape* COpenDWGio::DoRay (OdDbEntity* pEntity)
{
	def_var(Ray);
	BadObjectAlert(IDS_DWGRAY);
	return NULL;
}

CMDShape* COpenDWGio::DoXline (OdDbEntity* pEntity)
{
	def_var(Xline);
	BadObjectAlert(IDS_DWGCONSTRUCTIONLINE);
	return NULL;
}

CMDShape* COpenDWGio::DoRegion (OdDbEntity* pEntity)
{
	def_var(Region);
	BadObjectAlert(IDS_DWGREGION);
	return NULL;
}

CMDShape* COpenDWGio::DoHatch (OdDbEntity* pEntity)
{
	def_var(Hatch);
	BadObjectAlert(IDS_DWGHATCH);
	return NULL;
}


/*CMDShape* COpenDWGio::DoArcAlignedText (OdDbEntity* pEntity)
{
	def_var(ArcAlignedText);
	BadObjectAlert(IDS_DWGARCALIGNEDTEXT);
	return NULL;
}*/

CMDShape* COpenDWGio::DoImage (OdDbEntity* pEntity)
{
	def_var(Image);
	BadObjectAlert(IDS_DWGIMAGE);
	return NULL;
}

CMDShape* COpenDWGio::DoPolyFaceMesh (OdDbEntity* pEntity)
{
	def_var(PolyFaceMesh);
	BadObjectAlert(IDS_DWGARCALIGNEDTEXT);
	return NULL;
}

CMDShape* COpenDWGio::DoPolygonMesh (OdDbEntity* pEntity)
{
	def_var(PolygonMesh);
	BadObjectAlert(IDS_DWGPOLYGONMASH);
	return NULL;
}

CMDShape* COpenDWGio::DoFcf (OdDbEntity* pEntity)
{
	def_var(Fcf);
	BadObjectAlert(IDS_DWGTOLERANCE);
	return NULL;
}

CMDShape* COpenDWGio::Do3dSolid (OdDbEntity* pEntity)
{
	def_var(3dSolid);
	BadObjectAlert(IDS_DWG3DSOLID);
	return NULL;
}

CMDShape* COpenDWGio::DoFrame (OdDbEntity* pEntity)
{
	def_var(Frame);
	BadObjectAlert(IDS_DWGFRAME);
	return NULL;
}

CMDShape* COpenDWGio::DoBody (OdDbEntity* pEntity)
{
	def_var(Body);
	BadObjectAlert(IDS_DWGBODY);
	return NULL;
}

CMDShape* COpenDWGio::DoMline (OdDbEntity* pEntity)
{
	def_var(Mline);
	BadObjectAlert(IDS_DWGMULTILINE);
	return NULL;
}

/*CMDShape* COpenDWGio::DoProxyEntity(OdDbEntity* pEntity)
{
	def_var(ProxyEntity);
	OdTypedIdsArray arr;
	pProxyEntity->getReferences(arr);
	OdDbTypedId ent;
	int size = arr.size();
	for(int i=0; i<size; i++)
	{
		ent = arr[i];
		OdDbEntityPtr entity = ent.safeOpenObject();
		DoEntity(entity);
	}
	return NULL;
}*/

/*void reportError(LPCTSTR szContextMsg, const OdError& e)
{
	reportError(szContextMsg, e.code());
}

void reportError(LPCTSTR szContextMsg, unsigned int eCode)
{
	MessageBox(NULL, getErrorDescription(eCode), szContextMsg, MB_OK|MB_ICONERROR);
}*/


void CPCDraftApp::init_dwgio()
{

	m_pdwgio = new COpenDWGio;

	//m_pdwgio->init();
}


#endif
