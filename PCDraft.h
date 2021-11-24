// PCDraft.h : main header file for the PCDRAFT application
//

#if !defined(AFX_PCDRAFT_H__F8C83DF4_0F91_11D2_9806_000000000000__INCLUDED_)
#define AFX_PCDRAFT_H__F8C83DF4_0F91_11D2_9806_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"       // main symbols
#include "ViewTemplate.h"
#include "DSShape.h"

/////////////////////////////////////////////////////////////////////////////
// CPCDraftApp:
// See PCDraft.cpp for the implementation of this class
//

#define VK_COMMAND	VK_CONTROL
#define VK_OPTION	VK_MENU

enum WindowAtStartUp
{
	kNew		= 0,
	kOpenDialog	= 1,
	kNone		= 2,
   kWelcomeScreen = 3
};

typedef enum
{
	kUMillimeters,
	kUCentimeters,
	kUDecimeters,
	kUMeters,
	kUDecInches = 0x4000,
	kUDecFeetInches,
	kUDecFeet,
	kUFracInches = 0x0403,
	kUFracFeetInches
} Unit;

typedef enum
{
	kDecimalDegrees,
	kDegreesMinutes,
	kDegMinSeconds
} AngleUnit;

#define	kUEnglishMask		0x4400

#define	kUEnglishDecMask	0x4000			/* 2nd bit set for dec units */
#define	kUEnglishFracMask	0x0400			/* high bit set for fractional */

#ifndef _PERSONAL_EDITION
#define	kMaxDocH	12846L
#define	kMaxDocV	12854L
#else
#define	kMaxDocH	1224L
#define	kMaxDocV	1224L
#endif

#ifdef _PERSONAL_EDITION
	#define	kVersion "6.0.0"
	#define kVersionT _T("6.0.0")
	#define kVersionW L"6.0.0"

#else 
	#define	kVersion "6.0.0"
	#define kVersionT _T("6.0.0")
	#define kVersionW L"6.0.0"
#endif


class CPCDraftDoc;
class CMDShape;
class CNetGuard;
class CLibraryDoc;
class CLibraryFrame;
class CLayerDoc;
class CLayerFrame;
class CMainFrame;

class GetStartedWnd;
class TemplateWnd;


extern	UINT	gNetGuardMsg;

#ifndef _PERSONAL_EDITION
class COpenDWGio;
#endif

class CPCDraftApp: public CWinApp
{
public:
	void init_dwgio();
   bool CheckResources();
   void RefreshAllDocuments();
   int m_nMirrorMethod;
   int m_nTrimMethod;
   static	int gMaxZoom;
   bool m_bDemoMode;
   CString  m_strDDETitle; // Infinisys Ltda.
   CNetGuard* m_pNetGuard;

   // Infinisys Ltda.
   CRecentFileList * GetRecentFileList()
   {
      return m_pRecentFileList;

   }

   void LoadDefaultPalette();
   void SaveApplicationDefaults();
   void LoadApplicationDefaults();
   void SaveDocumentDefaults(CPCDraftDoc* pDoc);
   void LoadDocumentDefaults();
   void DocumentSetDefaults(CPCDraftDoc* pDoc);
   void StoreDocumentDefault(LPCTSTR lpszEntry,CPCDraftDoc* pDoc,void (CPCDraftDoc::*SerializeFunc)(CArchive& ar));
   bool LoadDocumentDefault(LPCTSTR lpszEntry,CPCDraftDoc* pDoc,void (CPCDraftDoc::*SerializeFunc)(CArchive& ar));
   bool FindDrawingDoc(CDocument* pDoc);
   bool CanExportFile();
   bool DoAppDefaults();
   bool HasOpenDocuments();


   void DoCopyAttributes(CMDShape* pShape);

   CRecentFileList * recent_file_list()
   {
      return m_pRecentFileList;
   }

	CPCDraftApp();
	
	UINT m_cfShapeList;

	// appliction default
	int		m_nWindowAtStartUp;
	BOOL m_bShowCursorPosition;
	BOOL m_bToolDisplay;
	BOOL m_bToolSaveLocation;
	BOOL m_bHideLineStyles;
	BOOL m_bZoomLineWeights;
	BOOL m_bCrossCursor;
	BOOL m_bLinkedDimensions;
	BOOL m_bRemoveLinks;
	BOOL m_bClickToStartPoly;
	// document default
	BOOL m_bDimEndMarks;
	BOOL m_bDimFormats;
	BOOL m_bDimStandards;
	BOOL m_bDrawAndPageSetup;
	BOOL m_bFieldNames;
	BOOL m_bFillPatternsColors;
	BOOL m_bHideGridLines;
	BOOL m_bHidePageBreaks;
	BOOL m_bLineSettings;
	BOOL m_bOpenAtCenter;
	BOOL m_bScaleAndLayerSetup;
	BOOL m_bShowRulers;
	BOOL m_bSnapToObject;
	BOOL m_bTextSettings;

	ShapeAttributes m_ShapeAttributes;

	CImageList m_imgLayerBar;
	CImageList m_imgReport;

   bool m_bWorkaroundUTF8; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   BOOL m_bAutoSave; // PC Draft 7 - Auto Save
   int m_iAutoSaveMinutes; // PC Draft 7 - Auto Save

   //bool m_bWelcomeScreen; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen

   static int ShowAppMessageBoxW(CWinApp *pApp, LPCWSTR lpszPrompt, UINT nType, UINT nIDPrompt);
   int DoMessageBoxW(LPCWSTR lpszPrompt, UINT nType, UINT nIDPrompt);
   void ShowMainFrame(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   void ShowGetStartedWnd(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   void ShowTemplateWnd(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCDraftApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	CMultiDocTemplate* m_pDrawingDocTemplate;
	CMultiDocTemplate* m_pSymbolDocTemplate;
	CViewTemplate m_ReportViewTemplate;

   CSingleDocTemplate * m_pLibraryDocTemplate; // Library - PCDraft 6 - Infinisys Ltd..
   CLibraryDoc * m_pLibraryDoc; // Library - PCDraft 6 - Infinisys Ltd..
   CLibraryFrame * m_pLibraryFrame; // Library - PCDraft 6 - Infinisys Ltd..
   CMainFrame* m_pMainFrame; // Library - PCDraft 6 - Infinisys Ltd.
   
   CSingleDocTemplate * m_pWelcomeDocTemplate; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   // CWelcomeDoc * m_pWelcomeDoc; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   // CWelcomeFrame* m_pWelcomeFrame; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   GetStartedWnd* m_pGetStartedWnd; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   TemplateWnd* m_pTemplateWnd; // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen

   CSingleDocTemplate * m_pLayerDocTemplate; // Layer - PCDraft 6 - Infinisys Ltd..
   CLayerDoc * m_pLayerDoc; // Layer - PCDraft 6 - Infinisys Ltd..
   CLayerFrame * m_pLayerFrame; // Layer - PCDraft 6 - Infinisys Ltd..
#ifndef _PERSONAL_EDITION
   COpenDWGio * m_pdwgio; // PCDraft 7 - Infinisys Ltd..
#endif

#ifndef _PERSONAL_EDITION
   COpenDWGio * dwgio();
#endif

	//{{AFX_MSG(CPCDraftApp)
   afx_msg void OnGetStartedWnd(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   afx_msg void OnTemplateWnd(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
   afx_msg bool CreateAndShowTemplateWnd(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
	afx_msg void OnAppAbout();
	afx_msg void OnFileSetDefaults();
	afx_msg void OnFileNewSymbolLibrary();
	afx_msg void OnFileNewDrawing();
   afx_msg void OnFileNewFromTemplate(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
	afx_msg void OnUpdatePrefHideLineStyle(CCmdUI* pCmdUI);
	afx_msg void OnPrefHideLineStyle();
	afx_msg void OnUpdatePrefZoomLineWeights(CCmdUI* pCmdUI);
	afx_msg void OnPrefZoomLineWeights();
	afx_msg void OnUpdatePrefCrossCursor(CCmdUI* pCmdUI);
	afx_msg void OnPrefCrossCursor();
	afx_msg void OnFileOpen();
	afx_msg void OnFileCloseAll();
   afx_msg void OnWelcomeScreen(); // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
	afx_msg void OnUpdateFileCloseAll(CCmdUI* pCmdUI);
	afx_msg void OnHelpManual();
	afx_msg void OnHelpRegister();
	afx_msg void OnHelpUpgrade();
	afx_msg void OnUpdateDemo(CCmdUI* pCmdUI);
   afx_msg void OnViewLibrary(); // Library - PCDraft 6 - Infinisys Ltd.
   afx_msg void OnUpdateViewLibrary(CCmdUI* pCmdUI); // Library - PCDraft 6 - Infinisys Ltd.
   afx_msg void OnOptionsAutoSave(); // PC Draft7 - Auto Save
   afx_msg void OnUpdateOptionsAutoSave(CCmdUI* pCmdUI); // PC Draft7 - Auto Save
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// Utilities

class CFlag
{
public:
	CFlag () { m_nFlag++; }
	~CFlag () {	m_nFlag--; }
	static bool Free () { return m_nFlag == 0; }
private:
	static int m_nFlag;
};


CString GetDefaultTitle();

HBITMAP CopyDIB(HBITMAP hDib);
void ReadDIB(CArchive& ar, HBITMAP& hDib);
void WriteDIB(CArchive& ar, HBITMAP hDib);
int CalculateDIBHash(HBITMAP hDib);
int CompareDIBs(HBITMAP hDib1, HBITMAP hDib2);

//WORD GetFreeSystemResources(WORD type);
void AdjustFontCharSet(LOGFONT& lf);

// From 16-bit windows.h
#define GFSR_SYSTEMRESOURCES 0
#define GFSR_GDIRESOURCES 1
#define GFSR_USERRESOURCES 2

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCDRAFT_H__F8C83DF4_0F91_11D2_9806_000000000000__INCLUDED_)
