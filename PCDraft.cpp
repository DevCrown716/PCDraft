// PCDraft.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PCDraft.h"
#include "Welcome/FileDialogW.h"
#define	_AFX_NO_OLE_SUPPORT
// #include <..\atlmfc\src\mfc\afximpl.h>
#include <io.h>
#include "DpiAwareness.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "SymbolLibraryDoc.h"
#include "SymbolLibraryView.h"
#include "SymbolFrame.h"
#include "ScaleUnitsDlg.h"

#include "Welcome/FileStatusW.h"

#include "MDBitmap.h"
#include "tiff.h"

#include "ApplicationDefaultsDlg.h"
#include "DocDefaultsDlg.h"
#include "k32exp.h"

#include "ShellRegistryHelper.h"
#include "SerialNumber.h"
#include "NetGuard.h"

#include "DWGUnitsDlg.h"
#include "CommandLineInfoEx.h"
#include <FreeImage.h>
//namespace QTML
//{
//#include <QTML.h>
//#include <Movies.h>
//#include <QuickTimeComponents.h>
//}

#include "FireWall.h"
#include <gdiplus.h> // Opacity/Gradient - Infinisys Ltd
#include "LibraryDoc.h" // Library - PCDraft 6 - Infinisys Ltd.
#include "LibraryFrame.h" // Library - PCDraft 6 - Infinisys Ltd.
#include "LibraryMainView.h" // Library - PCDraft 6 - Infinisys Ltd.
#include "LayerDoc.h" // Layer - PCDraft 6 - Infinisys Ltd.
#include "LayerFrame.h" // Layer - PCDraft 6 - Infinisys Ltd.
#include "LayerMainView.h" // Layer - PCDraft 6 - Infinisys Ltd.

#include "OptionsAutoSaveDlg.h" // PCDraft 7 - Auto Save

//#include "Welcome/WelcomeDoc.h" // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
//#include "Welcome/WelcomeFrame.h" // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
//#include "Welcome/WelcomeMainView.h" // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "Welcome/GetStartedWnd.h" // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "Welcome/TemplateWnd.h" // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen


Gdiplus::GdiplusStartupInput gdiplusStartupInput; // Opacity/Gradient - Infinisys Ltd
ULONG_PTR           gdiplusToken; // Opacity/Gradient - Infinisys Ltd


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_bEndApp;

// SMK.SSA from afximpl.h
BOOL AFXAPI AfxFullPath(_Pre_notnull_ _Post_z_ LPTSTR lpszPathOut, LPCTSTR lpszFileIn);


int CPCDraftApp::gMaxZoom = 6; // PC Draft 7 - Zoom 64x
const LPCTSTR gURL="URL";
const LPCTSTR gHome="Home";
const LPCTSTR gRegister="Register";
const LPCTSTR gUpgrade="Upgrade";

UINT	gNetGuardMsg = RegisterWindowMessage("NetGuardMsg");
int CFlag::m_nFlag = 0;
/////////////////////////////////////////////////////////////////////////////
// CPCDraftApp

BEGIN_MESSAGE_MAP(CPCDraftApp, CWinApp)
   //{{AFX_MSG_MAP(CPCDraftApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   ON_COMMAND(ID_FILE_SET_DEFAULTS, OnFileSetDefaults)
   ON_COMMAND(ID_FILE_NEW_SYMBOL_LIBRARY, OnFileNewSymbolLibrary)
   ON_COMMAND(ID_FILE_NEW_DRAWING, OnFileNewDrawing)
   ON_COMMAND(ID_FILE_NEW_FROM_TEMPLATE, OnFileNewFromTemplate)
   ON_UPDATE_COMMAND_UI(ID_PREF_HIDE_LINE_STYLE, OnUpdatePrefHideLineStyle)
   ON_COMMAND(ID_PREF_HIDE_LINE_STYLE, OnPrefHideLineStyle)
   ON_UPDATE_COMMAND_UI(ID_PREF_ZOOM_LINE_WEIGHTS, OnUpdatePrefZoomLineWeights)
   ON_COMMAND(ID_PREF_ZOOM_LINE_WEIGHTS, OnPrefZoomLineWeights)
   ON_UPDATE_COMMAND_UI(ID_PREF_CROSS_CURSOR, OnUpdatePrefCrossCursor)
   ON_COMMAND(ID_PREF_CROSS_CURSOR, OnPrefCrossCursor)
   ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
   ON_COMMAND(ID_WELCOME_SCREEN, OnWelcomeScreen)
   ON_COMMAND(ID_FILE_CLOSE_ALL, OnFileCloseAll)
   ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_ALL, OnUpdateFileCloseAll)
   ON_COMMAND(ID_HELP_MANUAL, OnHelpManual)
   ON_COMMAND(ID_HELP_REGISTER, OnHelpRegister)
   ON_COMMAND(ID_HELP_UPGRADE, OnHelpUpgrade)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateDemo)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateDemo)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateDemo)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_ACTIVE_LAYER, OnUpdateDemo)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SLIDES, OnUpdateDemo)
	//}}AFX_MSG_MAP
   // Standard print setup command
   ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
#ifdef _LIBRARY_PALETTE  
   ON_COMMAND(ID_VIEW_LIBRARY,OnViewLibrary) // Library - PCDraft 6 - Infinisys Ltd.
   ON_UPDATE_COMMAND_UI(ID_VIEW_LIBRARY,OnUpdateViewLibrary) // Library - PCDraft 6 - Infinisys Ltd.
#endif
   ON_COMMAND(ID_OPTIONS_AUTO_SAVE, OnOptionsAutoSave)  // PC Draft7 - Auto Save
   ON_UPDATE_COMMAND_UI(ID_OPTIONS_AUTO_SAVE, OnUpdateOptionsAutoSave)  // PC Draft7 - Auto Save
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPCDraftApp construction

CPCDraftApp::CPCDraftApp()
{
   m_bWorkaroundUTF8 = false;
   g_bEndApp = false;
   FreeImage_Initialise();
#ifndef _PERSONAL_EDITION
	m_pdwgio = nullptr;
#endif
	//if(process_set_dpi_awareness_per_monitor_v2())
   //{
   //   OutputDebugString("Process Dpi Awareness Set");
   //}
   //if(thread_set_dpi_awareness_per_monitor_v2())
   //{
   //   OutputDebugString("Process Dpi Awareness Set");
   //}
   m_pLibraryDoc = NULL; // Library - Infinisys Ltd.
   m_pLibraryFrame = NULL; // Library - Infinisys Ltd.
   m_pMainFrame = NULL; 
   m_pGetStartedWnd = NULL; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   m_pTemplateWnd = NULL; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   m_pLayerDoc = NULL; // Layer - Infinisys Ltd.
   m_pLayerFrame = NULL; // Layer - Infinisys Ltd.
   m_nTrimMethod=0;
   m_pNetGuard=NULL;

   //m_bWelcomeScreen = true; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen

   m_ShapeAttributes.m_nFill=1;
   m_ShapeAttributes.m_Fill.m_nFillType=kFillColor;
   m_ShapeAttributes.m_Fill.m_nIndex=PALETTEINDEX(0);	// white color
   m_ShapeAttributes.m_nPen=1;
//	m_ShapeAttributes.m_Pen=pDoc->m_Pen;
   m_ShapeAttributes.m_bInfo=false;
   m_ShapeAttributes.m_nPara=1;
   m_ShapeAttributes.m_Para.m_nFillType=kFillColor;
   m_ShapeAttributes.m_Para.m_nIndex=PALETTEINDEX(0);	// white color
//	m_ShapeAttributes.m_Para=pDoc->m_ParallelAttrs.m_Fill;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPCDraftApp object

CPCDraftApp theApp;
bool bQTInitialized = false;

HANDLE gMutex=NULL;

/////////////////////////////////////////////////////////////////////////////
// CPCDraftApp initialization

static void RegisterFileExt(LPCTSTR lpszExt,LPCTSTR lpszType)
{
   ::RegSetValue(HKEY_CLASSES_ROOT, lpszExt, REG_SZ, lpszType, lstrlen(lpszType) * sizeof(TCHAR));
}

BOOL _SetRegKey(LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL)
{
   if (lpszValueName == NULL)
   {
      if (::RegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ,
                        lpszValue, lstrlen(lpszValue) * sizeof(TCHAR)) != ERROR_SUCCESS)
      {
         TRACE1("Warning: registration database update failed for key '%s'.\n",
                lpszKey);
         return FALSE;
      }
      return TRUE;
   }
   else
   {
      HKEY hKey;

      if(::RegCreateKey(HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS)
      {
         LONG lResult = ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ,
                                        (CONST BYTE*)lpszValue, (lstrlen(lpszValue) + 1) * sizeof(TCHAR));

         if(::RegCloseKey(hKey) == ERROR_SUCCESS && lResult == ERROR_SUCCESS)
            return TRUE;
      }
      TRACE1("Warning: registration database update failed for key '%s'.\n", lpszKey);
      return FALSE;
   }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam)
{
   CPCDraftApp * papp = (CPCDraftApp *)lParam;
   LPCTSTR lpszTitle=papp->m_strDDETitle;
   CString sText;
   GetWindowText(hwnd,sText.GetBufferSetLength(256),255);
   sText.ReleaseBuffer();
   if(sText.Left(lstrlen(lpszTitle))==lpszTitle)
   {
      HWND hwndChild=GetLastActivePopup(hwnd);
      if(IsIconic(hwnd)) ShowWindow(hwnd,SW_RESTORE);
      SetForegroundWindow(hwndChild);
      // Infinisys Ltd.
      COPYDATASTRUCT copydata;
      copydata.dwData = 1000;
      copydata.cbData = (lstrlenW(::GetCommandLineW()) + 1) *sizeof(WCHAR);
      copydata.lpData = (void *)::GetCommandLineW();
      SendMessage(hwnd,WM_COPYDATA,0,(LPARAM)&copydata);
      return FALSE;
   }
   return TRUE;
}

BOOL CPCDraftApp::InitInstance()
{

   AfxEnableControlContainer();
   // Initialize GDI+
   GdiplusStartup(&gdiplusToken,&gdiplusStartupInput,NULL); // Opacity/Gradient - Infinisys Ltd

   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfoEx cmdInfo;
   ParseCommandLine(cmdInfo);

   ::CoInitialize(NULL);

   if(cmdInfo.m_bEnableFilewall)
   {
      CFireWall cFireWall;
      try
      {
         cFireWall.Initialize();
         cFireWall.EnableApplication();
      }
      catch(...)
      {
      }
      ::CoUninitialize();
      return FALSE;
   }


   if(!CheckResources()) return FALSE;

   CString strAppTitle;
   strAppTitle.LoadString(IDS_DDE_TITLE);
   m_strDDETitle = strAppTitle;
   // use mutex to allow only one copy of program
   gMutex=CreateMutex(NULL,TRUE,strAppTitle);
   if(GetLastError() == ERROR_ALREADY_EXISTS)
   {
      // locate and activate running copy
      EnumWindows(EnumWindowsProc,(LPARAM)this);
      return FALSE;
   }

   // Standard initialization
#ifdef _AFXDLL
   Enable3dControls();			// Call this when using MFC in a shared DLL
#else

   // SMK.SSA warning C4996: 'CWinApp::Enable3dControlsStatic': CWinApp::Enable3dControlsStatic is no longer needed. You should remove this call.
   // Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

   AfxOleInit();

   // Change the registry key under which our settings are stored.
   SetRegistryKey(_T("Microspot"));

   m_cfShapeList=RegisterClipboardFormat("PCDraft Shape List");

   LoadStdProfileSettings(50);  // Load standard INI file options (including MRU)
   LoadApplicationDefaults();
   LoadDocumentDefaults();

   VERIFY(m_imgLayerBar.Create(IDB_LAYER_BAR,13,0,RGB(255,0,0)));
   VERIFY(m_imgReport.Create(IDB_REPORT,7,0,RGB(255,0,0)));

   // Register document templates

   m_pDrawingDocTemplate=new CMultiDocTemplate(
      IDR_PCDRAFTTYPE,
      RUNTIME_CLASS(CPCDraftDoc),
      RUNTIME_CLASS(CChildFrame), // custom MDI child frame
      RUNTIME_CLASS(CPCDraftView));
   AddDocTemplate(m_pDrawingDocTemplate);

   m_pSymbolDocTemplate=new CMultiDocTemplate(
      IDR_SYMBOL_LIBRARY,
      RUNTIME_CLASS(CSymbolLibraryDoc),
      RUNTIME_CLASS(CSymbolFrame),
      RUNTIME_CLASS(CSymbolLibraryView));
   AddDocTemplate(m_pSymbolDocTemplate);


// Library - Infinisys Ltd.
   m_pLibraryDocTemplate = new CSingleDocTemplate(
      IDR_LIBRARY,
      RUNTIME_CLASS(CLibraryDoc),
      RUNTIME_CLASS(CLibraryFrame),
      RUNTIME_CLASS(CLibraryMainView));
   AddDocTemplate(m_pLibraryDocTemplate);

//// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
//   m_pWelcomeDocTemplate = new CSingleDocTemplate(
//      IDR_WELCOME,
//      RUNTIME_CLASS(CWelcomeDoc),
//      RUNTIME_CLASS(CWelcomeFrame),
//      RUNTIME_CLASS(CWelcomeMainView));
//   AddDocTemplate(m_pWelcomeDocTemplate);

// Layer - Infinisys Ltd.
   m_pLayerDocTemplate = new CSingleDocTemplate(
      IDR_LAYER,
      RUNTIME_CLASS(CLayerDoc),
      RUNTIME_CLASS(CLayerFrame),
      RUNTIME_CLASS(CLayerMainView));
   AddDocTemplate(m_pLayerDocTemplate);

   m_ReportViewTemplate.Register(IDR_REPORTYPE);



   // create main MDI Frame window
   CMainFrame* pMainFrame = new CMainFrame;
   if(!pMainFrame->LoadFrame(IDR_MAINFRAME))
      return FALSE;
   m_pMainWnd = pMainFrame;
   m_pMainFrame = pMainFrame;

   EnableShellOpen();
   RegisterFile(m_pDrawingDocTemplate,1,kRegisterOpen | kRegisterPrint,strAppTitle);
   //	RegisterFileExt(m_pDrawingDocTemplate,".mdd");
   RegisterFile(IDR_PCMACDRAFTTYPE,1,kRegisterOpen | kRegisterPrint,strAppTitle);

   RegisterFile(IDR_STATIONARYTYPE,3,kRegisterOpen | kRegisterPrint,strAppTitle);
   RegisterFile(m_pSymbolDocTemplate,2,kRegisterOpen,strAppTitle);



   if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
      cmdInfo.m_nShellCommand=CCommandLineInfo::FileNothing;

   // Dispatch commands specified on the command line
   if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
   {
      BOOL bOpen = FALSE;
      for(int i = 0; i < cmdInfo.m_arrFileNames.GetSize(); i++)
         if(OpenDocumentFile(cmdInfo.m_arrFileNames.GetAt(i)) != NULL)
            bOpen = TRUE;
      if(!bOpen)
         return FALSE;
   }
   else if(!ProcessShellCommand(cmdInfo))
      return FALSE;

   //{
   //   BYTE* pWP;
   //   UINT nWP;
   //   if(GetProfileBinary("MainWindow","Placement",&pWP,&nWP) && LPWINDOWPLACEMENT(pWP)->showCmd != SW_SHOWMINIMIZED)
   //   {
   //      if(m_nCmdShow != SW_SHOWNORMAL) LPWINDOWPLACEMENT(pWP)->showCmd=m_nCmdShow;
   //      pMainFrame->SetWindowPlacement((LPWINDOWPLACEMENT)pWP);
   //      delete[]pWP;
   //   }
   //   else
   //      pMainFrame->ShowWindow(m_nCmdShow == SW_SHOWNORMAL?SW_SHOWMAXIMIZED:m_nCmdShow);
   //}
   //pMainFrame->UpdateWindow();


   // ShowMainFrame();

   //using namespace QTML;
   ////	long version;
   //if((InitializeQTML(0) == noErr) && (EnterMovies() == noErr) /*&& (Gestalt(gestaltQuickTime, &version)==noErr) && (version>=0x04000000)*/)
   //{
   //   ComponentDescription cd;
   //   cd.componentType = GraphicsExporterComponentType;
   //   cd.componentSubType = 0;
   //   cd.componentManufacturer = 0;
   //   cd.componentFlags = 0;
   //   cd.componentFlagsMask = graphicsExporterIsBaseExporter;
   //   if(FindNextComponent(0,&cd) != 0) bQTInitialized=true;
   //   else
   //   {
   //      ExitMovies();
   //      TerminateQTML();
   //   }
   //}


#ifdef _DEMO
   m_bDemoMode=true;
#else

   //	m_pNetGuard=new CNetGuard();
   m_bDemoMode=!GetSerialNumber();
   if(m_bDemoMode)
   {
      //		delete m_pNetGuard;
      m_pNetGuard=NULL;
   }
   else
   {
      m_pNetGuard=new CNetGuard();
      if(cmdInfo.m_nShellCommand != CCommandLineInfo::FileDDE &&
         m_pNetGuard && !m_pNetGuard->StartListener())
      {
         return FALSE;
      }
   }
#endif

   if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNothing)
   {
      switch(m_nWindowAtStartUp)
      {
      case kNew:
      {

         if (false)
         {
            
            ShowMainFrame();

            OnFileNewDrawing();

         }
         else
         {
            
            ShowTemplateWnd();

         }

      }

         break;
      case kOpenDialog:
         ShowMainFrame();
         OnFileOpen();
         break;
      case kNone:
         ShowMainFrame();
         break;
      case kWelcomeScreen:
         //OnFileNewDrawing();
         OnGetStartedWnd();
         break;
      }
   }

   {

// Library - Infinisys Ltd.
   m_pLibraryDoc = (CLibraryDoc *)m_pLibraryDocTemplate->OpenDocumentFile(NULL,FALSE);

   POSITION pos = m_pLibraryDoc->GetFirstViewPosition();

   CView * pView = m_pLibraryDoc->GetNextView(pos);

   CLibraryFrame * pLibraryFrame = (CLibraryFrame *)pView->GetParentFrame();

   pLibraryFrame->m_hWndOwner = m_pMainFrame->GetSafeHwnd();

   pLibraryFrame->m_pMainFrame = m_pMainFrame;

   pLibraryFrame->SetOwner(m_pMainFrame);

}

   {
// Layer - Infinisys Ltd.
      m_pLayerDoc = (CLayerDoc *)m_pLayerDocTemplate->OpenDocumentFile(NULL,FALSE);

      POSITION pos = m_pLayerDoc->GetFirstViewPosition();

      CView * pView = m_pLayerDoc->GetNextView(pos);

      CLayerFrame * pLayerFrame = (CLayerFrame *)pView->GetParentFrame();

      pLayerFrame->m_hWndOwner = m_pMainFrame->GetSafeHwnd();

      pLayerFrame->m_pMainFrame = m_pMainFrame;

      pLayerFrame->SetOwner(m_pMainFrame);

   }

   return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialog Data
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   CString	m_sSerialNumber;
   CString	m_sName;
   CString	m_sVersion;
   CString	m_sObjects;
   CString	m_sSelectable;
   CString	m_sMemFree;
   CString	m_sDemo;
   //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CAboutDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CFont m_boldFont;
   //{{AFX_MSG(CAboutDlg)
   afx_msg void OnGotoWebPage();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   m_sSerialNumber = _T("");
   m_sName = _T("");
   m_sVersion = _T("Version ") kVersionT; // gk+ 09/17/07
// gk- 09/17/07
//#ifdef _PERSONAL_EDITION
//	m_sVersion = _T("Version 5.0.7");
//#else
//	m_sVersion = _T("Version 5.0.7");
//#endif
//
   m_sObjects = _T("");
   m_sSelectable = _T("");
   m_sMemFree = _T("");
   m_sDemo = _T("");
   //}}AFX_DATA_INIT

   CMDIFrameWnd *pFrame=(CMDIFrameWnd*)AfxGetMainWnd();
   CMDIChildWnd *pChild=(CMDIChildWnd*)pFrame->GetActiveFrame();
   CView *pView=pChild->GetActiveView();
   if(pView)
   {
      CDocument* pDoc=pView->GetDocument();
      if(pDoc->IsKindOf(RUNTIME_CLASS(CPCDraftDoc)))
      {
         m_sObjects.Format(IDS_OBJECTS,((CPCDraftDoc*)pDoc)->GetObjectCount(true));
         m_sSelectable.Format(IDS_SELECTABLE,((CPCDraftDoc*)pDoc)->GetObjectCount(false));
      }
      else if(pDoc->IsKindOf(RUNTIME_CLASS(CSymbolLibraryDoc)))
      {
         m_sObjects.Format(IDS_COMPOSING,((CSymbolLibraryDoc*)pDoc)->GetObjectCount(true));
         m_sSelectable.Format(IDS_SYMBOLS,((CSymbolLibraryDoc*)pDoc)->GetObjectCount(false));
      }
   }

   MEMORYSTATUS ms;
   GlobalMemoryStatus(&ms);
   char c='K';
   auto nMemFree=ms.dwAvailPhys/1024;
   if(nMemFree>9999)
   {
      nMemFree/=1024;
      c='M';
   }
   m_sMemFree.Format(IDS_MEMFREE,nMemFree,c);

#ifdef _DEMO

   CFont font;
   font.CreateStockObject(DEFAULT_GUI_FONT);
   LOGFONT lf;
   font.GetLogFont(&lf);
   lf.lfWeight=FW_BOLD;
   m_boldFont.CreateFontIndirect(&lf);

   m_sDemo.LoadString(IDS_DEMO);
   m_sSerialNumber.LoadString(IDS_DEMO_VERSION);
   m_sName.LoadString(IDS_DEMO_VERSION);
#else
   m_sSerialNumber=AfxGetApp()->GetProfileString("Registration","SerialNumber");
   if(IsBadSerialNumber(m_sSerialNumber)) m_sSerialNumber="";
   else m_sName=AfxGetApp()->GetProfileString("Registration","Name");
#endif
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   DDX_Text(pDX, IDC_SERIAL, m_sSerialNumber);
   DDX_Text(pDX, IDC_NAME, m_sName);
   DDX_Text(pDX, IDC_VERSION, m_sVersion);
   DDX_Text(pDX, IDC_OBJECTS, m_sObjects);
   DDX_Text(pDX, IDC_SELECTABLE, m_sSelectable);
   DDX_Text(pDX, IDC_MEMFREE, m_sMemFree);
   DDX_Text(pDX, IDC_DEMO, m_sDemo);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
   //{{AFX_MSG_MAP(CAboutDlg)
   ON_BN_CLICKED(IDC_GOTO_WEB_PAGE, OnGotoWebPage)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   GetDlgItem(IDC_DEMO)->SetFont(&m_boldFont,FALSE);

   return TRUE;
}

void CAboutDlg::OnGotoWebPage()
{
   CString strDefault;
   strDefault.LoadString(IDS_HOMEURL);
   CString sURL=AfxGetApp()->GetProfileString(gURL,gHome,strDefault);
   if(::ShellExecute(NULL,"open","IEXPLORE",sURL,"",SW_SHOWNORMAL)<=HINSTANCE(32))
      AfxMessageBox(IDS_CANT_OPEN_WEB_PAGE);
   EndDialog(IDOK);
}

// App command to run the dialog
void CPCDraftApp::OnAppAbout()
{
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftApp commands

bool CPCDraftApp::DoAppDefaults()
{
   CApplicationDefaultsDlg theDlg;
   theDlg.m_bCrossCursor=m_bCrossCursor;
   theDlg.m_nWindowAtStartUp=m_nWindowAtStartUp;
   theDlg.m_bHideLineStyle=m_bHideLineStyles;
   theDlg.m_bShowCursorPosition=m_bShowCursorPosition;
   theDlg.m_bToolDisplay=m_bToolDisplay;
   theDlg.m_bToolSaveLocation=m_bToolSaveLocation;
   theDlg.m_bZoomLineWeights=m_bZoomLineWeights;
   theDlg.m_bLinkedDimensions=m_bLinkedDimensions;
   theDlg.m_bRemoveLinks=m_bRemoveLinks;
   if(theDlg.DoModal()==IDOK)
   {
      m_bCrossCursor=theDlg.m_bCrossCursor;
      m_nWindowAtStartUp=theDlg.m_nWindowAtStartUp;
      m_bHideLineStyles=theDlg.m_bHideLineStyle;
      m_bShowCursorPosition=theDlg.m_bShowCursorPosition;
      m_bToolDisplay=theDlg.m_bToolDisplay;
      m_bToolSaveLocation=theDlg.m_bToolSaveLocation;
      m_bZoomLineWeights=theDlg.m_bZoomLineWeights;
      m_bLinkedDimensions=theDlg.m_bLinkedDimensions;
      m_bRemoveLinks=theDlg.m_bRemoveLinks;
      SaveApplicationDefaults();
      return true;
   }
   return false;
}

void CPCDraftApp::OnFileSetDefaults()
{
   DoAppDefaults();
}

const	LPCTSTR	gApplicationDefaults = "ApplicationDefaults";
const	LPCTSTR	gCrossCursor = "CrossCursor";
const	LPCTSTR	gWindowAtStartUp = "WindowAtStartUp2";
const	LPCTSTR	gHideLineStyles = "HideLineStyles";
const	LPCTSTR	gShowCursorPosition = "ShowCursorPosition";
const	LPCTSTR	gToolDisplay = "ToolDisplay";
const	LPCTSTR	gToolSaveLocation = "ToolSaveLocation";
const	LPCTSTR	gZoomLineWeights = "ZoomLineWeights";
const	LPCTSTR	gLinkedDimensions = "LinkedDimensions";
const	LPCTSTR	gRemoveLinks = "RemoveLinks";
const	LPCTSTR	gClickToStartPoly = "ClickToStartPoly";
const	LPCTSTR	gAutoSave = "AutoSave"; // PC Draft 7 - Auto Save
const	LPCTSTR	gAutoSaveMinutes = "AutoSaveMinutes"; // PC Draft 7 - Auto Save
//const	LPCTSTR	gShowWelcomeScreen = "ShowWelcomeScreen"; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen


void CPCDraftApp::LoadApplicationDefaults()
{
   m_bCrossCursor=GetProfileInt(gApplicationDefaults,gCrossCursor,1);
   m_nWindowAtStartUp=GetProfileInt(gApplicationDefaults,gWindowAtStartUp,kWelcomeScreen);
   m_bHideLineStyles=GetProfileInt(gApplicationDefaults,gHideLineStyles,0);
   m_bShowCursorPosition=GetProfileInt(gApplicationDefaults,gShowCursorPosition,0);
   m_bToolDisplay=GetProfileInt(gApplicationDefaults,gToolDisplay,0);
   m_bToolSaveLocation=GetProfileInt(gApplicationDefaults,gToolSaveLocation,0);
   m_bZoomLineWeights=GetProfileInt(gApplicationDefaults,gZoomLineWeights,0);
   m_bLinkedDimensions=GetProfileInt(gApplicationDefaults,gLinkedDimensions,1);
#ifdef _PERSONAL_EDITION
   m_bRemoveLinks=true;
   m_bClickToStartPoly=true;
#else
   m_bRemoveLinks=GetProfileInt(gApplicationDefaults,gRemoveLinks,0);
   m_bClickToStartPoly=GetProfileInt(gApplicationDefaults,gClickToStartPoly,0);
#endif
   m_bAutoSave = GetProfileInt(gApplicationDefaults, gAutoSave, 0);
   m_iAutoSaveMinutes = GetProfileInt(gApplicationDefaults, gAutoSaveMinutes, 5);
   //m_bWelcomeScreen = GetProfileInt(gShowWelcomeScreen, gCrossCursor, 1);
   LoadDefaultPalette();
}

void CPCDraftApp::SaveApplicationDefaults()
{
   WriteProfileInt(gApplicationDefaults,gCrossCursor,m_bCrossCursor);
   WriteProfileInt(gApplicationDefaults,gWindowAtStartUp,m_nWindowAtStartUp);
   WriteProfileInt(gApplicationDefaults,gHideLineStyles,m_bHideLineStyles);
   WriteProfileInt(gApplicationDefaults,gShowCursorPosition,m_bShowCursorPosition);
   WriteProfileInt(gApplicationDefaults,gToolDisplay,m_bToolDisplay);
   WriteProfileInt(gApplicationDefaults,gToolSaveLocation,m_bToolSaveLocation);
   WriteProfileInt(gApplicationDefaults,gZoomLineWeights,m_bZoomLineWeights);
   WriteProfileInt(gApplicationDefaults,gLinkedDimensions,m_bLinkedDimensions);
#ifndef _PERSONAL_EDITION
   WriteProfileInt(gApplicationDefaults,gRemoveLinks,m_bRemoveLinks);
   WriteProfileInt(gApplicationDefaults,gClickToStartPoly,m_bClickToStartPoly);
#endif
   WriteProfileInt(gApplicationDefaults, gAutoSave, m_bAutoSave);
   WriteProfileInt(gApplicationDefaults, gAutoSaveMinutes, m_iAutoSaveMinutes);

}


void CPCDraftApp::OnGetStartedWnd() // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
{

   if (m_pGetStartedWnd)
   {

      return;

   }

   m_pGetStartedWnd = new GetStartedWnd;

   m_pGetStartedWnd->Create();

}


bool CPCDraftApp::CreateAndShowTemplateWnd()
{

   if (m_pTemplateWnd != nullptr)
   {

      return false;

   }

   m_pTemplateWnd = new TemplateWnd;

   m_pTemplateWnd->Create();

   return true;

}


void CPCDraftApp::OnTemplateWnd() // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
{

   CreateAndShowTemplateWnd();

}


void CPCDraftApp::LoadDefaultPalette()
{
}

const	LPCTSTR	gDocumentDefaults = "DocumentDefaults";
LPCTSTR	gDimEndMarks = "DimEndMarks";
LPCTSTR	gDimFormats = "DimFormats";
LPCTSTR	gDimStandards = "DimStandards";
LPCTSTR	gDrawAndPageSetup = "DrawAndPageSetup";
LPCTSTR	gFieldNames = "FieldNames";
LPCTSTR	gFillPatternsColors = "FillPatternsColors";
LPCTSTR	gHideGridLines = "HideGridLines";
LPCTSTR	gHidePageBreaks = "HidePageBreaks";
LPCTSTR	gLineSettings = "LineSettings";
LPCTSTR	gOpenAtCenter = "OpenAtCenter";
LPCTSTR	gScaleAndLayerSetup = "ScaleAndLayerSetup";
LPCTSTR	gShowRulers = "ShowRulers";
LPCTSTR	gSnapToObject = "SnapToObject";
LPCTSTR	gTextSettings = "TextSettings";

void CPCDraftApp::StoreDocumentDefault(LPCTSTR lpszEntry,CPCDraftDoc* pDoc,void (CPCDraftDoc::*SerializeFunc)(CArchive& ar))
{
   CMemFile mf;
   CArchive ar(&mf,CArchive::store);
   (pDoc->*SerializeFunc)(ar);
   ar.Close();
   auto  len=mf.GetLength();
   BYTE* data=mf.Detach();
   WriteProfileBinary(gDocumentDefaults,CString(lpszEntry)+"Data",data,(UINT) len);
   ::free(data);
}

bool CPCDraftApp::LoadDocumentDefault(LPCTSTR lpszEntry,CPCDraftDoc* pDoc,void (CPCDraftDoc::*SerializeFunc)(CArchive& ar))
{
   BYTE* pData;
   UINT nBytes;
   if(GetProfileBinary(gDocumentDefaults,CString(lpszEntry)+"Data",&pData,&nBytes))
   {
      CMemFile mf(pData,nBytes);
      CArchive ar(&mf,CArchive::load);
      (pDoc->*SerializeFunc)(ar);
      ar.Close();
      delete []pData;
      return true;
   }
   return false;
}

void CPCDraftApp::SaveDocumentDefaults(CPCDraftDoc* pDoc)
{
   WriteProfileInt(gDocumentDefaults,gDimEndMarks,m_bDimEndMarks);
   if (m_bDimEndMarks)
   {
      StoreDocumentDefault(gDimEndMarks, pDoc, &CPCDraftDoc::SerializeDimEndmarks);
   }
   WriteProfileInt(gDocumentDefaults,gDimFormats,m_bDimFormats);
   if(m_bDimFormats)
      StoreDocumentDefault(gDimFormats, pDoc, &CPCDraftDoc::SerializeDimFormats);
   WriteProfileInt(gDocumentDefaults,gDimStandards,m_bDimStandards);
   if(m_bDimStandards)
      StoreDocumentDefault(gDimStandards, pDoc, &CPCDraftDoc::SerializeDimStandards);
   WriteProfileInt(gDocumentDefaults,gDrawAndPageSetup,m_bDrawAndPageSetup);
   if(m_bDrawAndPageSetup)
      StoreDocumentDefault(gDrawAndPageSetup, pDoc, &CPCDraftDoc::SerializeDrawAndPageSetup);
   WriteProfileInt(gDocumentDefaults,gFieldNames,m_bFieldNames);
   if(m_bFieldNames)
      StoreDocumentDefault(gFieldNames, pDoc, &CPCDraftDoc::SerializeFieldNames);
   WriteProfileInt(gDocumentDefaults,gFillPatternsColors,m_bFillPatternsColors);
   if(m_bFillPatternsColors)
      StoreDocumentDefault(gFillPatternsColors, pDoc, &CPCDraftDoc::SerializeFillPatternsColors);
   WriteProfileInt(gDocumentDefaults,gHideGridLines,m_bHideGridLines);
   WriteProfileInt(gDocumentDefaults,gHidePageBreaks,m_bHidePageBreaks);
   WriteProfileInt(gDocumentDefaults,gLineSettings,m_bLineSettings);
   if(m_bLineSettings)
      StoreDocumentDefault(gLineSettings, pDoc, &CPCDraftDoc::SerializeLineSettings);
   WriteProfileInt(gDocumentDefaults,gOpenAtCenter,m_bOpenAtCenter);
   WriteProfileInt(gDocumentDefaults,gScaleAndLayerSetup,m_bScaleAndLayerSetup);
   if(m_bScaleAndLayerSetup)
      StoreDocumentDefault(gScaleAndLayerSetup, pDoc, &CPCDraftDoc::SerializeScaleAndLayerSetup);
   WriteProfileInt(gDocumentDefaults,gShowRulers,m_bShowRulers);
   WriteProfileInt(gDocumentDefaults,gSnapToObject,pDoc->m_bSnapToObject);
   WriteProfileInt(gDocumentDefaults,gTextSettings,m_bTextSettings);
   if(m_bTextSettings)
      StoreDocumentDefault(gTextSettings, pDoc, &CPCDraftDoc::SerializeTextSettings);
}

void CPCDraftApp::LoadDocumentDefaults()
{
   m_bDimEndMarks=GetProfileInt(gDocumentDefaults,gDimEndMarks,0);
   m_bDimFormats=GetProfileInt(gDocumentDefaults,gDimFormats,0);
   m_bDimStandards=GetProfileInt(gDocumentDefaults,gDimStandards,0);
   m_bDrawAndPageSetup=GetProfileInt(gDocumentDefaults,gDrawAndPageSetup,0);
   m_bFieldNames=GetProfileInt(gDocumentDefaults,gFieldNames,0);
   m_bFillPatternsColors=GetProfileInt(gDocumentDefaults,gFillPatternsColors,0);
   m_bHideGridLines=GetProfileInt(gDocumentDefaults,gHideGridLines,0);
   m_bHidePageBreaks=GetProfileInt(gDocumentDefaults,gHidePageBreaks,0);
   m_bLineSettings=GetProfileInt(gDocumentDefaults,gLineSettings,0);
   m_bOpenAtCenter=GetProfileInt(gDocumentDefaults,gOpenAtCenter,0);
   m_bScaleAndLayerSetup=GetProfileInt(gDocumentDefaults,gScaleAndLayerSetup,0);
   m_bShowRulers=GetProfileInt(gDocumentDefaults,gShowRulers,1);
   m_bSnapToObject=GetProfileInt(gDocumentDefaults,gSnapToObject,1);
   m_bTextSettings=GetProfileInt(gDocumentDefaults,gTextSettings,0);
}

void CPCDraftApp::DocumentSetDefaults(CPCDraftDoc* pDoc)
{
   CDocDefaultsDlg theDlg;
   /*	theDlg.m_bDimEndMarks=m_bDimEndMarks;
   	theDlg.m_bDimFormats=m_bDimFormats;
   	theDlg.m_bDimStandards=m_bDimStandards;
   	theDlg.m_bDrawAndPageSetup=m_bDrawAndPageSetup;
   	theDlg.m_bFieldNames=m_bFieldNames;
   	theDlg.m_bFillPatternsColors=m_bFillPatternsColors;
   	theDlg.m_bLineSettings=m_bLineSettings;
   	theDlg.m_bScaleAndLayerSetup=m_bScaleAndLayerSetup;
   	theDlg.m_bSnapToObject=m_bSnapToObject;
   	theDlg.m_bTextSettings=m_bTextSettings;
   */	theDlg.m_bOpenAtCenter=m_bOpenAtCenter;
   theDlg.m_bHidePageBreaks=m_bHidePageBreaks;
   theDlg.m_bHideGridLines=m_bHideGridLines;
   theDlg.m_bShowRulers=m_bShowRulers;
   if(theDlg.DoModal()==IDOK)
   {
      if(theDlg.m_bDimEndMarks) m_bDimEndMarks=true;
      if(theDlg.m_bDimFormats) m_bDimFormats=true;
      if(theDlg.m_bDimStandards) m_bDimStandards=true;
      if(theDlg.m_bDrawAndPageSetup) m_bDrawAndPageSetup=true;
      if(theDlg.m_bFieldNames) m_bFieldNames=true;
      if(theDlg.m_bFillPatternsColors) m_bFillPatternsColors=true;
      if(theDlg.m_bLineSettings) m_bLineSettings=true;
      if(theDlg.m_bScaleAndLayerSetup) m_bScaleAndLayerSetup=true;
      if(theDlg.m_bSnapToObject) m_bSnapToObject=true;
      if(theDlg.m_bTextSettings) m_bTextSettings=true;
      m_bOpenAtCenter=theDlg.m_bOpenAtCenter;
      m_bHidePageBreaks=theDlg.m_bHidePageBreaks;
      m_bHideGridLines=theDlg.m_bHideGridLines;
      m_bShowRulers=theDlg.m_bShowRulers;
      SaveDocumentDefaults(pDoc);
   }
}


void CPCDraftApp::OnFileNewSymbolLibrary()
{
   if (m_pNetGuard && !m_pNetGuard->StartListener ()) return;
   m_pSymbolDocTemplate->OpenDocumentFile(NULL);
}

void CPCDraftApp::OnFileNewDrawing()
{
   if (m_pNetGuard && !m_pNetGuard->StartListener ()) return;
   m_pDrawingDocTemplate->OpenDocumentFile(NULL);
}


void CPCDraftApp::OnFileNewFromTemplate()
{
   
   ShowTemplateWnd();

}


void CPCDraftApp::OnUpdatePrefHideLineStyle(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_bHideLineStyles);
}

void CPCDraftApp::OnPrefHideLineStyle()
{
   m_bHideLineStyles=!m_bHideLineStyles;
   RefreshAllDocuments();
}

void CPCDraftApp::OnUpdatePrefZoomLineWeights(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_bZoomLineWeights);
}

void CPCDraftApp::OnPrefZoomLineWeights()
{
   m_bZoomLineWeights=!m_bZoomLineWeights;
   RefreshAllDocuments();
}

void CPCDraftApp::OnUpdatePrefCrossCursor(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_bCrossCursor);
}

void CPCDraftApp::OnPrefCrossCursor()
{
   m_bCrossCursor=!m_bCrossCursor;
}

void CPCDraftApp::OnFileOpen()
{
   if (m_pNetGuard && !m_pNetGuard->StartListener ()) return;
   CFlag cFlag;
   CFileDialogW dlgFile(TRUE);

   CStringW title;
   VERIFY(title.LoadString(AFX_IDS_OPENFILE));

   dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

   CStringW strFilter,strFilterName;
   CStringW strDefault;
   CStringW fileName;

   VERIFY(strFilterName.LoadString(IDS_MACFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   //strFilter += _T("*.p55; *.mdd; *.md60; *.md70"); // .p55 format - Infinisys Ltd
   strFilter += _T("*.md70; *.p55; *.mdd; *.md60"); // .p55 format - Infinisys Ltd
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;

   //strFilter += strFilterName;
   //strFilter += (TCHAR)'\0';   // next string please
   //strFilter += _T("*.md60"); // .md60 format - Infinisys Ltd
   //strFilter += (TCHAR)'\0';   // last string
   //dlgFile.m_ofn.nMaxCustFilter++;

   // override the default extension
   //dlgFile.m_ofn.lpstrDefExt = _T("p55"); // .p55 format - Infinisys Ltd
   dlgFile.m_ofn.lpstrDefExt = L"md70"; // .p55 format - Infinisys Ltd
   //dlgFile.m_ofn.lpstrDefExt = _T("md60"); // .p55 format - Infinisys Ltd
   dlgFile.m_ofn.nFilterIndex = dlgFile.m_ofn.nMaxCustFilter;


   // do for all doc template
   POSITION pos = GetFirstDocTemplatePosition();
   BOOL bFirst = TRUE;
   while (pos != NULL)
   {
      CDocTemplate* pTemplate = GetNextDocTemplate(pos);
      CStringW strFilterExt;
      if(pTemplate->GetDocString(get_utf8(strFilterExt),CDocTemplate::filterExt) && !strFilterExt.IsEmpty() &&
            pTemplate->GetDocString(get_utf8(strFilterName),CDocTemplate::filterName) && !strFilterName.IsEmpty())
      {
         // a file based document template - add to filter list
         ASSERT(strFilterExt[0]=='.');
         /*			if(bFirst)
         			{
         				// set the default extension
         				strDefault = ((LPCTSTR)strFilterExt) + 1;  // skip the '.'
         				dlgFile.m_ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(strDefault);
         				dlgFile.m_ofn.nFilterIndex = dlgFile.m_ofn.nMaxCustFilter + 1;  // 1 based number
         			}
         */
         // add to filter
         strFilter += strFilterName;
         ASSERT(!strFilter.IsEmpty());  // must have a file type name
         strFilter += (WCHAR)'\0';  // next string please
         strFilter += (WCHAR)'*';
         strFilter += strFilterExt;
         strFilter += (WCHAR)'\0';  // next string please
         dlgFile.m_ofn.nMaxCustFilter++;
      }

      bFirst = FALSE;
   }

   // append other supported formats
   VERIFY(strFilterName.LoadString(IDS_STATIONERYFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   strFilter += L"*.drs";
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;

   VERIFY(strFilterName.LoadString(IDS_WINDRAFTFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   strFilter += L"*.dft";
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;
#ifndef _PERSONAL_EDITION
   VERIFY(strFilterName.LoadString(IDS_DXFFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   strFilter += L"*.dxf";
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;

   VERIFY(strFilterName.LoadString(IDS_DWGFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   strFilter += L"*.dwg";
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;
#endif
   VERIFY(strFilterName.LoadString(IDS_BITMAPFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   strFilter += L"*.bmp";
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;

   // append the "*.*" all files filter
   VERIFY(strFilterName.LoadString(AFX_IDS_ALLFILTER));
   strFilter += strFilterName;
   strFilter += (WCHAR)'\0';   // next string please
   strFilter += L"*.*";
   strFilter += (WCHAR)'\0';   // last string
   dlgFile.m_ofn.nMaxCustFilter++;

   dlgFile.m_ofn.lpstrFilter = strFilter;
   //if (m_bWorkaroundUTF8)
   //{
   dlgFile.m_ofn.lpstrTitle = title;//get_uni(title, CP_UTF8);
   //}
   //else
   //{
   //   dlgFile.m_ofn.lpstrTitle = get_uni(title, CP_ACP);
   //}
   dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

   auto nResult = dlgFile.DoModal();
   fileName.ReleaseBuffer();
   if(nResult==IDOK)
   {
      m_bWorkaroundUTF8 = true;
      OpenDocumentFile(get_utf8(fileName));
   }

}

// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
void CPCDraftApp::OnWelcomeScreen()
{
   
   ShowGetStartedWnd();

}


void CPCDraftApp::RefreshAllDocuments()
{
   POSITION pos=m_pDrawingDocTemplate->GetFirstDocPosition();
   while(pos)
   {
      CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->GetNextDoc(pos);
      pDoc->UpdateAllViews(NULL,kRefresh);
   }
   pos=m_pSymbolDocTemplate->GetFirstDocPosition();
   while(pos)
   {
      CSymbolLibraryDoc* pDoc=(CSymbolLibraryDoc*)m_pSymbolDocTemplate->GetNextDoc(pos);
      pDoc->UpdateAllViews(NULL,kRefresh);
   }
}

void AFXAPI AfxGetRoot2(LPCSTR lpszPath, CString& strRoot)
{
   ASSERT(lpszPath != NULL);

   LPSTR lpszRoot = strRoot.GetBuffer(_MAX_PATH);
   memset(lpszRoot, 0, _MAX_PATH);
   Checked::tcsncpy_s(lpszRoot, _MAX_PATH, lpszPath, _TRUNCATE);
   PathStripToRoot(lpszRoot);
   strRoot.ReleaseBuffer();
}


AFX_STATIC void AFXAPI _AfxFillExceptionInfo(CFileException* pException, LPCTSTR lpszFileName);
// turn a file, relative path or other into an absolute path
BOOL AFXAPI _AfxFullPath4(_Out_writes_z_(_MAX_PATH) LPTSTR lpszPathOut, LPCTSTR lpszFileIn, CFileException* pException)
// lpszPathOut = buffer of _MAX_PATH
// lpszFileIn = file, relative path or absolute path
// (both in ANSI character set)
// pException - pointer to exception object - can be NULL.
{
   pException;
   ENSURE(lpszPathOut);
   ENSURE(lpszFileIn);
   ASSERT(AfxIsValidAddress(lpszPathOut, _MAX_PATH));

   // first, fully qualify the path name
   LPTSTR lpszFilePart;
   //DWORD dwRet = GetFullPathName(lpszFileIn, _MAX_PATH, lpszPathOut, &lpszFilePart);
   lpszFilePart = (LPTSTR) strrchr(lpszFileIn, '\\');
   if (lpszFilePart == 0)
   {
#ifdef _DEBUG
      if (lpszFileIn != NULL && lpszFileIn[0] != '\0')
         TRACE(traceAppMsg, 0, _T("Warning: could not parse the path '%Ts'.\n"), lpszFileIn);
#endif
      Checked::tcsncpy_s(lpszPathOut, _MAX_PATH, lpszFileIn, _TRUNCATE); // take it literally
      _AfxFillExceptionInfo(pException, lpszFileIn);
      return FALSE;
   }
//   else if (dwRet >= _MAX_PATH)
//   {
//#ifdef _DEBUG
//      if (lpszFileIn[0] != '\0')
//         TRACE1("Warning: could not parse the path '%Ts'. Path is too long.\n", lpszFileIn);
//#endif
//      // GetFullPathName() returned a path greater than _MAX_PATH
//      if (pException != NULL)
//      {
//         pException->m_cause = CFileException::badPath;
//         pException->m_strFileName = lpszFileIn;
//      }
//      return FALSE; // long path won't fit in buffer
//   }
   strcpy(lpszPathOut, lpszFileIn);
//   lpszFilePart = '\0';
   lpszFilePart++;

   CString strRoot;
   // determine the root name of the volume
   AfxGetRoot2(lpszPathOut, strRoot);

   if (!::PathIsUNC(strRoot))
   {
      // get file system information for the volume
      DWORD dwFlags, dwDummy;
      if (!GetVolumeInformation(strRoot, NULL, 0, NULL, &dwDummy, &dwFlags,
         NULL, 0))
      {
         TRACE(traceAppMsg, 0, _T("Warning: could not get volume information '%Ts'.\n"),
            (LPCTSTR)strRoot);
         _AfxFillExceptionInfo(pException, lpszFileIn);
         return FALSE;   // preserving case may not be correct
      }

      // not all characters have complete uppercase/lowercase
      if (!(dwFlags & FS_CASE_IS_PRESERVED))
         CharUpper(lpszPathOut);

      // assume non-UNICODE file systems, use OEM character set
      if (!(dwFlags & FS_UNICODE_STORED_ON_DISK))
      {
         WIN32_FIND_DATA data;
         HANDLE h = FindFirstFile(lpszFileIn, &data);
         if (h != INVALID_HANDLE_VALUE)
         {
            FindClose(h);
            if (lpszFilePart != NULL && lpszFilePart > lpszPathOut)
            {
               int nFileNameLen = AtlStrLen(data.cFileName);
               int nIndexOfPart = (int)(lpszFilePart - lpszPathOut);
               if ((nFileNameLen + nIndexOfPart) < _MAX_PATH)
               {
                  Checked::tcscpy_s(lpszFilePart, _MAX_PATH - nIndexOfPart, data.cFileName);
               }
               else
               {
                  // the path+filename of the file is too long
                  if (pException != NULL)
                  {
                     pException->m_cause = CFileException::badPath;
                     pException->m_strFileName = lpszFileIn;
                  }
                  return FALSE; // Path doesn't fit in the buffer.
               }
            }
            else
            {
               _AfxFillExceptionInfo(pException, lpszFileIn);
               return FALSE;
            }
         }
      }
   }

   return TRUE;
}
//Backward compatible wrapper that calls the new exception throwing func.
BOOL AFXAPI AfxFullPath3(_Pre_notnull_ _Post_z_ LPTSTR lpszPathOut, LPCTSTR lpszFileIn)
{
   return _AfxFullPath4(lpszPathOut, lpszFileIn, NULL);
}

const char * find_last(const char * p)
{

   while (*p)
   {

      p++;
   }

   return p;

}

const char * reverse_find_char(const char * p, char ch)
{
   auto pStart = p;
   p = find_last(p);

   while (*p != ch)
   {

      p--;

      if (p < pStart)
      {
         return nullptr;
      }

   }

   return p;

}


CDocument* CPCDraftApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
   if (m_pNetGuard && !m_pNetGuard->StartListener ()) return NULL;
   CFlag cFlag;
   // check for zero-length file
   if (m_bWorkaroundUTF8)
   {
      CFileStatusW fs;

      CStringW wstr(get_uni(lpszFileName));

      FileGetStatus(wstr, fs);
      if (fs.m_size == 0)
      {
         m_bWorkaroundUTF8 = false;
         AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
         return NULL;
      }

   }
   else
   {

      CFileStatus fs;

      CFile::GetStatus(lpszFileName, fs);
      if (fs.m_size == 0)
      {
         m_bWorkaroundUTF8 = false;
         AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
         return NULL;
      }


   }

   // find the highest confidence
   POSITION pos = GetFirstDocTemplatePosition();
   CDocTemplate::Confidence bestMatch = CDocTemplate::yesAttemptForeign;
   CDocTemplate* pBestTemplate = NULL;
   CDocument* pOpenDocument = NULL;

   TCHAR szPath[_MAX_PATH];
   ASSERT(lstrlen(lpszFileName) < _countof(szPath));
   TCHAR szTemp[_MAX_PATH];
   if (lpszFileName[0] == '\"')
      ++lpszFileName;
   lstrcpyn(szTemp, lpszFileName, _MAX_PATH);
   LPTSTR lpszLast = (char *) reverse_find_char(szTemp, '\"');
   if (lpszLast != NULL)
      *lpszLast = 0;
   AfxFullPath3(szPath, szTemp);

   while (pos != NULL)
   {
      CDocTemplate* pTemplate = GetNextDocTemplate(pos);

      CDocTemplate::Confidence match;
      ASSERT(pOpenDocument == NULL);
      match = pTemplate->MatchDocType(szPath, pOpenDocument);
      if (match > bestMatch)
      {
         bestMatch = match;
         pBestTemplate = pTemplate;
      }
      if (match == CDocTemplate::yesAlreadyOpen)
         break;      // stop here
   }

   if (pOpenDocument != NULL)
   {
      if (m_bWorkaroundUTF8 && pOpenDocument->IsKindOf(RUNTIME_CLASS(CPCDraftDoc)))
      {

         CPCDraftDoc * pDoc = (CPCDraftDoc *) pOpenDocument;
         pDoc->m_bUtf8Name = true;
         m_bWorkaroundUTF8 = false;

      }
      POSITION pos = pOpenDocument->GetFirstViewPosition();
      if (pos != NULL)
      {
         CView* pView = pOpenDocument->GetNextView(pos); // get first one
         ASSERT_VALID(pView);
         CFrameWnd* pFrame = pView->GetParentFrame();
         if (pFrame != NULL)
            pFrame->ActivateFrame();
         else
            TRACE0("Error: Can not find a frame for document to activate.\n");
         CFrameWnd* pAppFrame;
         if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
         {
            ASSERT_KINDOF(CFrameWnd, pAppFrame);
            pAppFrame->ActivateFrame();
         }
      }
      else
      {
         TRACE0("Error: Can not find a view for document to activate.\n");
      }
      return pOpenDocument;
   }
   //if(pBestTemplate==NULL)
   {
      LPCTSTR lpszDot= reverse_find_char(szPath,'.');
      if(lpszDot != NULL)
      {
#ifndef _PERSONAL_EDITION
         			/*if(lstrcmpi(lpszDot, ".dxf") == 0)
                  {

                  CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
                  if(!pDoc) return NULL;
                  CScaleUnitsDlg theDlg(pDoc);
                  if(IDOK==theDlg.DoModal() && pDoc->ImportDXF(szPath))
                  return pDoc;
                  pDoc->OnCloseDocument();
                  return NULL;
                  }*/
                  			if(lstrcmpi(lpszDot,".dwg") == 0 || lstrcmpi(lpszDot,".dxf") == 0)
                  {
                     CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
                     if(!pDoc) return NULL;
                     pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
                     m_bWorkaroundUTF8 = false;
                     CScaleUnitsDlg theDlg(pDoc,true);
                     if(IDOK == theDlg.DoModal())
                     {
                        CDWGUnitsDlg theUnitsDlg(pDoc);
                        if(IDOK == theUnitsDlg.DoModal())
                        {
                           switch(pDoc->ImportDWG(szPath,theUnitsDlg.m_Units,theUnitsDlg.m_nMetric?true:false))
                           {
                           case 1:	// success
                              return pDoc;
                           case 0: // fail
                              AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
                              break;
                              // default: canceled
                           };
                        }
                     }

                     pDoc->OnCloseDocument();
                     return NULL;
                  }
#endif
      if(lstrcmpi(lpszDot,".dft") == 0)
      {
         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);

         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;
         if(pDoc->ImportWinDraft(szPath))
            return pDoc;
         pDoc->OnCloseDocument();
         AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
         return NULL;
      }
      if(lstrcmpi(lpszDot,".drs") == 0)
      {
         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(pDoc)
         {
            pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
            m_bWorkaroundUTF8 = false;

            pDoc->OnOpenDocument(szPath);
            pDoc->SendInitialUpdate();
            ((CFrameWnd*)(((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive()))->OnUpdateFrameTitle(TRUE);
         }
         return pDoc;
      }
      if(lstrcmpi(lpszDot,".bmp") == 0)
      {

         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;

         HBITMAP hDib=(HBITMAP)::LoadImage(NULL,lpszFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE | LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
         if(hDib)
         {
            CLayer* pLayer=pDoc->GetActiveLayer();
            if(pLayer)
            {
               BITMAP bmp;
               ::GetObject(hDib,sizeof(BITMAP),&bmp);
               CRect rc(0,0,bmp.bmWidth,bmp.bmHeight);
               LongRect lrc(0,0,(int)bmp.bmWidth,(int)bmp.bmHeight);
               CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
               pShape->CalculateExtents();
               pShape->SetShapeDatum(kDefaultDatum);
               pDoc->CreateShape(pShape);
               pDoc->LayersChanged();
               pDoc->CheckDocSize(true);
               return pDoc;
            }
            DeleteObject(hDib);
         }
         pDoc->OnCloseDocument();
         return NULL;
      }
      if(lstrcmpi(lpszDot,".tif") == 0)
      {

         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;

         HBITMAP hBitmap;
         int width,height;
         if(readTiff(lpszFileName,hBitmap,width,height))
         {
            CLayer* pLayer=pDoc->GetActiveLayer();
            if(pLayer)
            {
               CRect rc(0,0,width,height);
               LongRect lrc(0,0,width,height);
               CMDBitmap* pShape=new CMDBitmap(rc,lrc,hBitmap);
               pShape->CalculateExtents();
               pShape->SetShapeDatum(kDefaultDatum);
               pDoc->CreateShape(pShape);
               pDoc->LayersChanged();
               pDoc->CheckDocSize(true);
               return pDoc;
            }
            DeleteObject(hBitmap);
         }
         else AfxMessageBox(IDS_INCOMPATIBLE_TIFF);
         pDoc->OnCloseDocument();
         return NULL;
      }
      }
      // assuming MacDraft
      if(lstrcmpi(lpszDot,".mdd") == 0)
      {
         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;

         if(pDoc->LoadMacDraft(szPath))
         {
            pDoc->SetModifiedFlag(FALSE);
            pDoc->SetPathName(szPath);
            pDoc->m_bSaveAsMacDraft=true;
            ((CFrameWnd*)(((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive()))->OnUpdateFrameTitle(TRUE);
            return pDoc;
         }
         pDoc->OnCloseDocument();
         //			return NULL;
      }
      if(lstrcmpi(lpszDot,".p55") == 0)
      {
         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;

         if(pDoc->LoadPCDraft55(szPath))
         {
            pDoc->SetModifiedFlag(FALSE);
            pDoc->SetPathName(szPath);
            pDoc->m_bSaveAsMacDraft=true;
            ((CFrameWnd*)(((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive()))->OnUpdateFrameTitle(TRUE);
            return pDoc;
         }
         pDoc->OnCloseDocument();
         //			return NULL;
      }
      if(lstrcmpi(lpszDot,".md60") == 0)
      {
         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;

         if(pDoc->LoadMacDraft60(szPath, pDoc->m_bUtf8Name))
         {
            pDoc->SetModifiedFlag(FALSE);
            pDoc->SetPathName(szPath);
            pDoc->m_bSaveAsMacDraft=true;
            ((CFrameWnd*)(((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive()))->OnUpdateFrameTitle(TRUE);
            return pDoc;
         }
         pDoc->OnCloseDocument();
         //			return NULL;
      }
      else if(lstrcmpi(lpszDot,".md70") == 0)
      {
         CPCDraftDoc* pDoc=(CPCDraftDoc*)m_pDrawingDocTemplate->OpenDocumentFile(NULL);
         if(!pDoc) return NULL;
         pDoc->m_bUtf8Name = m_bWorkaroundUTF8;
         m_bWorkaroundUTF8 = false;

         if(pDoc->LoadMacDraft70(szPath, pDoc->m_bUtf8Name))
         {
            pDoc->SetModifiedFlag(FALSE);
            pDoc->SetPathName(szPath);
            pDoc->m_bSaveAsMacDraft=true;
            ((CFrameWnd*)(((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive()))->OnUpdateFrameTitle(TRUE);
            return pDoc;
         }
         pDoc->OnCloseDocument();
         //			return NULL;
      }
      m_bWorkaroundUTF8 = false;

      if(pBestTemplate != NULL)
      {
         return pBestTemplate->OpenDocumentFile(szPath);
      }
      else
      {
         AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
         return NULL;
      }
   }
   
}

void CPCDraftApp::OnFileCloseAll()
{
   if(SaveAllModified())
   {
      CloseAllDocuments(FALSE);
   }
}

void CPCDraftApp::OnUpdateFileCloseAll(CCmdUI* pCmdUI)
{
   POSITION pos=GetFirstDocTemplatePosition();
   while(pos)
   {
      if(GetNextDocTemplate(pos)->GetFirstDocPosition())
      {
         pCmdUI->Enable();
         return;
      }
   }
   pCmdUI->Enable(FALSE);
}

bool CPCDraftApp::HasOpenDocuments()
{
   POSITION pos=GetFirstDocTemplatePosition();
   while(pos)
      if(GetNextDocTemplate(pos)->GetFirstDocPosition())
         return true;
   return false;
}

BOOL CPCDraftApp::OnIdle(LONG lCount)
{
   if(lCount==1)
   {
      ((CMainFrame*)m_pMainWnd)->DoIdleUpdate();
   }
   return CWinApp::OnIdle(lCount);
}

void CPCDraftApp::OnHelpManual()
{
   char szPath[MAX_PATH];
   GetModuleFileName(AfxGetInstanceHandle(),szPath,sizeof(szPath));
   char* p=strrchr(szPath,'\\');
   if(p) *p='\0'; else szPath[0]='\0';
   CString strManual;
   strManual.LoadString(IDS_MANUAL);
   CString strPath;
   strPath.Format("%s\\%s",szPath,LPCTSTR(strManual));
   CString strMessage;
   if(_access(strPath,0)==0)
   {
      if(::ShellExecute(NULL,"open",strPath,"","",SW_SHOWNORMAL)<=HINSTANCE(32))
         strMessage.Format(IDS_ACROBAT_NOT_FOUND,strManual);
      else
         return;
   }
   else
      strMessage.Format(IDS_MANUAL_NOT_FOUND,strManual);
   AfxMessageBox(strMessage);
}

void CPCDraftApp::OnHelpRegister()
{
   CString strDefault;
   strDefault.LoadString(IDS_REGISTERURL);
   CString sURL=GetProfileString(gURL,gRegister,strDefault);
   if(::ShellExecute(NULL,"open","IEXPLORE",sURL,"",SW_SHOWNORMAL)<=HINSTANCE(32))
      AfxMessageBox(IDS_CANT_OPEN_WEB_PAGE);
}

void CPCDraftApp::OnHelpUpgrade()
{
   CString strDefault;
   strDefault.LoadString(IDS_UPGRADEURL);
   CString sURL=GetProfileString(gURL,gUpgrade,strDefault);
   if(::ShellExecute(NULL,"open","IEXPLORE",sURL,"",SW_SHOWNORMAL)<=HINSTANCE(32))
      AfxMessageBox(IDS_CANT_OPEN_WEB_PAGE);
}

bool CPCDraftApp::FindDrawingDoc(CDocument* pDoc)
{
   POSITION pos=m_pDrawingDocTemplate->GetFirstDocPosition();
   while(pos!=NULL)
   {
      if(pDoc==m_pDrawingDocTemplate->GetNextDoc(pos))
         return true;
   }
   return false;
}

bool CPCDraftApp::CheckResources()
{

   //if(GetFreeSystemResources(GFSR_SYSTEMRESOURCES)<10)
   //{
   //   AfxMessageBox(IDS_LOW_RESOURCES);
   //   return false;
   //}

   MEMORYSTATUS ms;
   GlobalMemoryStatus(&ms);
   if((ms.dwAvailVirtual * 100 / ms.dwTotalVirtual)<10 || ms.dwAvailVirtual<0x400000)
   {
      AfxMessageBox(IDS_LOW_MEMORY);
      return false;
   }

   return true;
}

void CPCDraftApp::DoCopyAttributes(CMDShape *pShape)
{
   if(pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
   {
      if(m_ShapeAttributes.m_nFill>=0) m_ShapeAttributes.m_nFill-=2;
      if(m_ShapeAttributes.m_nPen>=0) m_ShapeAttributes.m_nPen-=2;
      if(m_ShapeAttributes.m_nPara>=0) m_ShapeAttributes.m_nPara-=2;
   }
   else
   {
      if(m_ShapeAttributes.m_nFill<0) m_ShapeAttributes.m_nFill+=2;
      m_ShapeAttributes.m_Fill=pShape->GetFillModel();
      if(m_ShapeAttributes.m_nPen<0) m_ShapeAttributes.m_nPen+=2;
      m_ShapeAttributes.m_Pen=pShape->GetPenModel();
      if(pShape->IsParallel())
      {
         if(m_ShapeAttributes.m_nPara<0) m_ShapeAttributes.m_nPara+=2;
         m_ShapeAttributes.m_Para=pShape->GetParallelAttrs().m_Fill;
      }
   }
   pShape->GetValues(m_ShapeAttributes.m_sInfo);
}

/////////////////////////////////////////////////////////////////////////////
// Utilities

HBITMAP CopyDIB(HBITMAP hDib)
{

   DIBSECTION ds;
   GetObject(hDib,sizeof(DIBSECTION),&ds);
   LPVOID lpBits;
   HBITMAP hDibCopy=CreateDIBSection(NULL,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
   ASSERT(hDibCopy);
   memcpy(lpBits,ds.dsBm.bmBits,ds.dsBmih.biSizeImage);
   if(ds.dsBmih.biBitCount<=8)
   {
      CWindowDC dcScreen(NULL);
      CDC memDC;
      memDC.CreateCompatibleDC(&dcScreen);

      HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
      LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
      GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
      memDC.SelectObject(hDibCopy);
      SetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
      GlobalFree(GlobalHandle(lpColors));
      memDC.SelectObject(hOldBmp);
   }
   return hDibCopy;
}

void ReadDIB(CArchive& ar, HBITMAP& hDib)
{
   DIBSECTION ds;
   memset(&ds,0,sizeof(DIBSECTION));
   ar.Read(&ds.dsBmih,sizeof(ds.dsBmih));
   if(ds.dsBmih.biSize>sizeof(ds.dsBmih))
   {
      ds.dsBmih.biSize=sizeof(ds.dsBmih);
      ar.Read(ds.dsBitfields,3*sizeof(DWORD));
   }
   LPVOID lpBits;
   hDib=CreateDIBSection(NULL,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
   ar.Read(lpBits,ds.dsBmih.biSizeImage);
   if(ds.dsBmih.biBitCount<=8)
   {
      CWindowDC dcScreen(NULL);
      CDC memDC;
      memDC.CreateCompatibleDC(&dcScreen);
      HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
      lpBits=GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
      ar.Read(lpBits,ds.dsBmih.biClrUsed*sizeof(RGBQUAD));
      SetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,(RGBQUAD*)lpBits);
      GlobalFree(GlobalHandle(lpBits));
      memDC.SelectObject(hOldBmp);
   }
}

void WriteDIB(CArchive& ar, HBITMAP hDib)
{
   DIBSECTION ds;
   GetObject(hDib,sizeof(DIBSECTION),&ds);
   if(ds.dsBmih.biCompression==BI_BITFIELDS)
      ds.dsBmih.biSize+=3*sizeof(DWORD);
   ar.Write(&ds.dsBmih,ds.dsBmih.biSize);
   ar.Write(ds.dsBm.bmBits,ds.dsBmih.biSizeImage);
   if(ds.dsBmih.biBitCount<=8)
   {
      CWindowDC dcScreen(NULL);
      CDC memDC;
      memDC.CreateCompatibleDC(&dcScreen);
      HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
      LPRGBQUAD lpColors=(LPRGBQUAD)GlobalLock(GlobalAlloc(GHND,ds.dsBmih.biClrUsed*sizeof(RGBQUAD)));
      GetDIBColorTable(memDC,0,ds.dsBmih.biClrUsed,lpColors);
      ar.Write(lpColors,ds.dsBmih.biClrUsed*sizeof(RGBQUAD));
      GlobalFree(GlobalHandle(lpColors));
      memDC.SelectObject(hOldBmp);
   }
}



struct dib
{

   COLORREF * m_pcolorref;
   int w;
   int h;

   dib()
   {
      m_pcolorref = NULL;
      w = 0;
      h = 0;
   }
   dib(const dib & d)
   {
      m_pcolorref = d.m_pcolorref;
      w = d.w;
      h = d.h;
   }
   dib(dib && d)
   {
      m_pcolorref = d.m_pcolorref;
      w = d.w;
      h = d.h;
      d.m_pcolorref = NULL;
   }

   ~dib()
   {
      if(m_pcolorref != NULL)
      {
         free(m_pcolorref);
      }
   }


};

dib HBITMAP_to_dib(HBITMAP &hBitmap)
{
   dib dib;
   BITMAP TemporaryBitmap;
   ZeroMemory(&TemporaryBitmap,sizeof(BITMAP));
   GetObject(hBitmap,sizeof(BITMAP),&TemporaryBitmap);

   HDC MemoryDC = CreateCompatibleDC(NULL);

   BITMAPINFO BitmapInfo;
   ZeroMemory(&BitmapInfo,sizeof(BITMAPINFO));
   BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   BitmapInfo.bmiHeader.biWidth = TemporaryBitmap.bmWidth;
   BitmapInfo.bmiHeader.biHeight = TemporaryBitmap.bmHeight;
   BitmapInfo.bmiHeader.biPlanes = 1;
   BitmapInfo.bmiHeader.biBitCount = 32;
   BitmapInfo.bmiHeader.biCompression = BI_RGB;

   RGBQUAD* pArgb = NULL;
   HBITMAP hTemporaryBitmap = CreateDIBSection(MemoryDC,&BitmapInfo,DIB_RGB_COLORS,(void**)&pArgb,NULL,0);

   GetDIBits(MemoryDC,hBitmap,0,BitmapInfo.bmiHeader.biHeight,pArgb,&BitmapInfo,DIB_RGB_COLORS);

   for(int i = 0; i < BitmapInfo.bmiHeader.biWidth * BitmapInfo.bmiHeader.biHeight; i++)
      pArgb[i].rgbReserved = 255;
   dib.w = BitmapInfo.bmiHeader.biWidth;
   dib.h = BitmapInfo.bmiHeader.biHeight;
   dib.m_pcolorref = (COLORREF *)malloc(BitmapInfo.bmiHeader.biWidth * BitmapInfo.bmiHeader.biHeight * 4);
   memcpy(dib.m_pcolorref,pArgb,dib.w * dib.h * 4);
   DeleteDC(MemoryDC);

   DeleteObject(hTemporaryBitmap);
   return dib;
}

int CalculateDIBHash(HBITMAP hDib)
{
   dib dib = HBITMAP_to_dib(hDib);
   if(dib.m_pcolorref == NULL)
   {
      return 0;
   }
   int area = dib.w * dib.h;
   int i;
   int res = 0;
   COLORREF * p = (COLORREF *)dib.m_pcolorref;
   for(i=0; i<area; i++)
   {
      res = res ^ *p;
      p++;
   }

   return res;
}

// 0 - if DIBs are equal
int CompareDIBs(HBITMAP hDib1, HBITMAP hDib2)
{

   dib dib1 = HBITMAP_to_dib(hDib1);
   dib dib2 = HBITMAP_to_dib(hDib2);

   if(dib1.m_pcolorref == NULL)
   {
      return 1;
   }
   if(dib2.m_pcolorref == NULL)
   {
      return 2;
   }

   if(dib1.w != dib2.w || dib1.h != dib2.h)
   {
      return 1;
   }

   return memcmp(dib1.m_pcolorref,dib2.m_pcolorref,dib1.w * dib2.w * 4);

}


////We need this pragma to make a compiler even don't think about to modify a code in release mode
//#pragma optimize ("", off)
//WORD GetFreeSystemResources(WORD type)
//{
////SAFE macro just returns 99 if something goes wrong (say under NT)
//#define DUMMY_RES 99
//#define	SAFE(a) if(!(a)) return DUMMY_RES
//
//   //Decarations  "c" style
//   typedef HINSTANCE (WINAPI* LOADLIB16)(char*);
//   typedef LONG (WINAPI* GETPROC16)(HINSTANCE, LPCSTR);
//   typedef BOOL (WINAPI* FREELIB16)(HINSTANCE);
//   FARPROC QT_Thunk;
//
//   LOADLIB16 pLoadLib16;
//   GETPROC16 pGetProc16;
//   FREELIB16 pFreeLib16;
//   HINSTANCE hInst;
//   DWORD pGetRes;
//   WORD user_fsr;
//   WORD _type;
//   HMODULE hKernel;
//
//   //Usuall Windows API calls
//   SAFE( hKernel = GetModuleHandle("KERNEL32"));
//   SAFE( QT_Thunk = GetProcAddress(hKernel, "QT_Thunk"));
//
//   //Andrew Schulman's
//   SAFE( pLoadLib16 = (LOADLIB16) GetK32ProcAddress(LOADLIBRARY16_ORD));
//   SAFE( pGetProc16 = (GETPROC16) GetK32ProcAddress(GETPROCADDRESS16_ORD));
//   SAFE( pFreeLib16 = (FREELIB16) GetK32ProcAddress(FREELIBRARY16_ORD));
//
//   //Usuall for "c"-style calls - by pointer to function
//   SAFE( hInst = (*pLoadLib16)("user"));
//   (*pFreeLib16)(hInst);
//   SAFE( pGetRes = (*pGetProc16)(hInst,"GetFreeSystemResources"));
//
//   //How to call 16-bit dll API from 32-bit .exe using QT_Thunk function
//   /*Actually it is
//
//   		__asm mov     edx, *pGetRes
//   		return (*QT_Thunk)(type);
//
//   		but compiler may produce a differnt code.
//   		To make QT_Thunk work we need exact code bellow
//   */
//
//   //_type = type;//for inline asm only
//   //if (pGetRes)
//   //   _asm
//   //{
//   //   push    _type
//   //   mov     edx, [pGetRes]
//   //   call    QT_Thunk
//   //   mov     [user_fsr], ax
//   //}
//   //return user_fsr;
//
//
//   /*There is another approach in MSVC documentation - "Thunk Compiler" chapter.
//   I tried to learn about "Thunk Compiler" several times  and finally understood nothing.*/
//
//#undef DUMMY_RES
//#undef SAFE
//}
//#pragma optimize ("", on)

int CPCDraftApp::ExitInstance()
{

   // release mutex
   if(gMutex) ReleaseMutex(gMutex);
   if(m_pNetGuard)	delete m_pNetGuard;
   //if(bQTInitialized)
   //{
   //   using namespace QTML;
   //   // Terminate QuickTime.
   //   ExitMovies();
   //   // Terminate QuickTime Media Layer.
   //   TerminateQTML();
   //}


   Gdiplus::GdiplusShutdown(gdiplusToken); // Opacity/Gradient - Infinisys Ltd
   return CWinApp::ExitInstance();
}

void CPCDraftApp::OnUpdateDemo(CCmdUI* pCmdUI)
{
   if(m_bDemoMode) pCmdUI->Enable(FALSE);
}

bool CPCDraftApp::CanExportFile()
{
   //if(bQTInitialized  && !m_bDemoMode)
   //{
   //   using namespace QTML;
   //   ComponentDescription cd;
   //   memset(&cd, 0, sizeof(cd));
   //   cd.componentType = GraphicsImporterComponentType;
   //   cd.componentSubType = kQTFileTypePicture;
   //   return FindNextComponent(NULL, &cd) != 0;
   //}
   return !m_bDemoMode;
}

CString GetDefaultTitle()
{
   static int nUntitledCount=0;
   CStringW wstrDocName;
   VERIFY(wstrDocName.LoadString(AFX_IDS_UNTITLED));
   CString strDocName = get_cp(wstrDocName, CP_ACP);
   CString strTitle;
   strTitle.Format("%s-%d",(LPCTSTR)strDocName,++nUntitledCount);
   return strTitle;
}

void AdjustFontCharSet(LOGFONT& lf)
{

   if (lf.lfCharSet == 0)
   {
      HKL hKL = GetKeyboardLayout(0);
      CHARSETINFO cs;
      TCHAR szLocaleData[7];
      DWORD dwCodePage;
      GetLocaleInfo(LOWORD(hKL), LOCALE_IDEFAULTANSICODEPAGE, szLocaleData, 6);
      dwCodePage = _ttol(szLocaleData);

      VERIFY(TranslateCharsetInfo(&dwCodePage, &cs, TCI_SRCCODEPAGE));

      lf.lfCharSet = (BYTE)cs.ciCharset;

   }

   /*	HFONT hFont=CreateFontIndirect(&lf);
   	HDC hdc=::GetDC(NULL);
   	HFONT hOldFont=(HFONT)::SelectObject(hdc,hFont);
   	::GetTextFace(hdc,LF_FACESIZE,lf.lfFaceName);
   //	::GetObject(hFont,sizeof(lf),&lf);
   	::SelectObject(hdc,hOldFont);
   	::DeleteObject(hFont);*/
}


// Library - PCDraft 6 - Infinisys Ltd.
void CPCDraftApp::OnViewLibrary()
{

   if(m_pLibraryFrame == NULL)
   {

      POSITION pos = m_pLibraryDoc->GetFirstViewPosition();

      CView * pView = m_pLibraryDoc->GetNextView(pos);

      CLibraryFrame * pLibraryFrame = (CLibraryFrame *)pView->GetParentFrame();

      m_pLibraryFrame = pLibraryFrame;

      m_pLibraryFrame->SetWindowPos(&CWnd::wndTop,100,100,320,450,SWP_SHOWWINDOW);

   }
   else if(!m_pLibraryFrame->IsWindowVisible())
   {

      m_pLibraryFrame->ShowWindow(SW_RESTORE);

      m_pLibraryFrame->ActivateFrame();

   }
   else
   {

      m_pLibraryFrame->ShowWindow(SW_HIDE);

   }



}

// Library - PCDraft 6 - Infinisys Ltd.
void CPCDraftApp::OnUpdateViewLibrary(CCmdUI* pCmdUI)
{

   if(m_pLibraryFrame == NULL || !m_pLibraryFrame->IsWindowVisible())
   {

      pCmdUI->SetCheck(0);

   }
   else
   {

      pCmdUI->SetCheck(1);

   }

   pCmdUI->Enable();

}


void CPCDraftApp::OnOptionsAutoSave()
{

   OptionsAutoSaveDialog dialog;

   dialog.DoModal();

}


void CPCDraftApp::OnUpdateOptionsAutoSave(CCmdUI* pCmdUI)
{

#ifdef _DEMO

   pCmdUI->Enable(FALSE);

#else

   pCmdUI->Enable(!m_bDemoMode); 
	
#endif

}

int CPCDraftApp::DoMessageBoxW(LPCWSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
   return ShowAppMessageBoxW(this, lpszPrompt, nType, nIDPrompt);
}

// Helper for message boxes; can work when no CWinApp can be found
int CPCDraftApp::ShowAppMessageBoxW(CWinApp *pApp, LPCWSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
   // disable windows for modal dialog
   DoEnableModeless(FALSE);
   HWND hWndTop;
   HWND hWnd = CWnd::GetSafeOwner_(NULL, &hWndTop);

   // re-enable the parent window, so that focus is restored 
   // correctly when the dialog is dismissed.
   if (hWnd != hWndTop)
      EnableWindow(hWnd, TRUE);

   // set help context if possible
   DWORD* pdwContext = NULL;

   DWORD dwWndPid = 0;
   GetWindowThreadProcessId(hWnd, &dwWndPid);

   if (hWnd != NULL && dwWndPid == GetCurrentProcessId())
   {
      // use app-level context or frame level context
      LRESULT lResult = ::SendMessage(hWnd, WM_HELPPROMPTADDR, 0, 0);
      if (lResult != 0)
         pdwContext = (DWORD*)lResult;
   }
   // for backward compatibility use app context if possible
   //if (pdwContext == NULL && pApp != NULL)
     // pdwContext = &pApp->m_dwPromptContext;

   DWORD dwOldPromptContext = 0;
   if (pdwContext != NULL)
   {
      // save old prompt context for restoration later
      dwOldPromptContext = *pdwContext;
      if (nIDPrompt != 0)
         *pdwContext = HID_BASE_PROMPT + nIDPrompt;
   }

   // determine icon based on type specified
   if ((nType & MB_ICONMASK) == 0)
   {
      switch (nType & MB_TYPEMASK)
      {
      case MB_OK:
      case MB_OKCANCEL:
         nType |= MB_ICONEXCLAMATION;
         break;

      case MB_YESNO:
      case MB_YESNOCANCEL:
         nType |= MB_ICONQUESTION;
         break;

      case MB_ABORTRETRYIGNORE:
      case MB_RETRYCANCEL:
         // No default icon for these types, since they are rarely used.
         // The caller should specify the icon.
         break;
      }
   }

#ifdef _DEBUG
   if ((nType & MB_ICONMASK) == 0)
      TRACE(traceAppMsg, 0, "Warning: no icon specified for message box.\n");
#endif

   TCHAR szAppName[_MAX_PATH];
   szAppName[0] = '\0';
   LPCTSTR pszAppName;
   if (pApp != NULL)
      pszAppName = pApp->m_pszAppName;
   else
   {
      pszAppName = szAppName;
      DWORD dwLen = GetModuleFileName(NULL, szAppName, _MAX_PATH);
      if (dwLen == _MAX_PATH)
         szAppName[_MAX_PATH - 1] = '\0';
   }

   int nResult = MessageBoxW(hWnd, lpszPrompt, get_uni(pszAppName, CP_ACP), nType);

   // restore prompt context if possible
   if (pdwContext != NULL)
      *pdwContext = dwOldPromptContext;

   // re-enable windows
   if (hWndTop != NULL)
      ::EnableWindow(hWndTop, TRUE);
   DoEnableModeless(TRUE);

   return nResult;
}




void CPCDraftApp::ShowMainFrame()
{

   try
   {

      if (m_pGetStartedWnd)
      {

         m_pGetStartedWnd->Hide();

      }

   }
   catch (...)
   {


   }


   try
   {

      if (m_pTemplateWnd)
      {

         m_pTemplateWnd->Hide();

      }

   }
   catch (...)
   {


   }

   {
      BYTE* pWP;
      UINT nWP;
      if (GetProfileBinary("MainWindow", "Placement", &pWP, &nWP) && LPWINDOWPLACEMENT(pWP)->showCmd != SW_SHOWMINIMIZED)
      {
         if (m_nCmdShow != SW_SHOWNORMAL) LPWINDOWPLACEMENT(pWP)->showCmd = m_nCmdShow;
         m_pMainFrame->SetWindowPlacement((LPWINDOWPLACEMENT)pWP);
         delete[]pWP;
      }
      else
         m_pMainFrame->ShowWindow(m_nCmdShow == SW_SHOWNORMAL ? SW_SHOWMAXIMIZED : m_nCmdShow);
   }
   m_pMainFrame->UpdateWindow();


}


void CPCDraftApp::ShowGetStartedWnd()
{

   OnGetStartedWnd();

   if (m_pTemplateWnd && m_pTemplateWnd->IsWindowVisible())
   {

      m_pTemplateWnd->Hide();

   }

   if (m_pMainFrame->IsWindowVisible())
   {

      m_pGetStartedWnd->ShowWindow(SW_SHOWNORMAL);

      m_pGetStartedWnd->BringWindowToTop();

      m_pGetStartedWnd->layout();

      m_pGetStartedWnd->m_pParentWnd = m_pMainFrame;

      m_pGetStartedWnd->DoModal();

      m_pGetStartedWnd->ShowWindow(SW_HIDE);

      m_pMainWnd->BringWindowToTop();

   }
   else
   {

      m_pGetStartedWnd->ShowWindow(SW_SHOWNORMAL);

      m_pGetStartedWnd->layout();

   }

}


void CPCDraftApp::ShowTemplateWnd()
{

   bool bCreate = CreateAndShowTemplateWnd();

   if (m_pGetStartedWnd->IsWindowVisible())
   {

      m_pGetStartedWnd->Hide();

   }

   if (m_pMainFrame->IsWindowVisible())
   {

      m_pTemplateWnd->ShowWindow(SW_SHOWNORMAL);

      m_pTemplateWnd->BringWindowToTop();

      m_pTemplateWnd->layout();

      m_pTemplateWnd->m_pParentWnd = m_pMainFrame;

      m_pTemplateWnd->DoModal();

      m_pTemplateWnd->ShowWindow(SW_HIDE);

      m_pMainWnd->BringWindowToTop();

   }
   else
   {

      m_pTemplateWnd->ShowWindow(SW_SHOWNORMAL);

      m_pTemplateWnd->layout();

   }

}



