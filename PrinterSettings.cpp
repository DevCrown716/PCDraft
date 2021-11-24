// PrinterSettings.cpp: Implementierung der Klasse CPrinterSettings.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "PrinterSettings.h"
#include <cderr.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CPrinterSettings,CObject,1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CPrinterSettings::CPrinterSettings()
{
   m_hPrinter = NULL;
	m_hDevMode = m_hDevNames = NULL;
	m_hSaveDevMode = NULL;
	m_hSaveDevNames = NULL;
	m_bSavedDevInfo = false;
	m_strPrinterName = PRINTERNAME_UNDEFINED;
}

CPrinterSettings::~CPrinterSettings()
{
   
   ::ClosePrinter(m_hPrinter);

	if (m_hDevMode)  GlobalFree(m_hDevMode);
	if (m_hDevNames) GlobalFree(m_hDevNames);
	// if possible MFC-Printer will go back to m_hSaveDevMode
	RestorePrinter();
}

void CPrinterSettings::operator=(const CPrinterSettings* src)
{
	VERIFY(src != NULL);
	m_hDevMode = CopyHandle(src->m_hDevMode);
	m_hDevNames = CopyHandle(src->m_hDevNames);
	m_strPrinterName = src->m_strPrinterName;
}

void CPrinterSettings::operator=(const CPrinterSettings& src)
{
	*this = &src;
}

//
// check to see if our printer (e.g. m_strprintername)
// is still available
//
BOOL CPrinterSettings::IsPrinterAvailable(LPCTSTR pszDeviceName)
{
	HANDLE hPrinter;
	if (OpenPrinter((char *) pszDeviceName, &hPrinter, NULL) == FALSE)
		return FALSE;
	ClosePrinter(hPrinter);
	return TRUE;
}


//
// Set the MFC-Printer
// with default settings ( previous settings are freed )
//
// This code is from a microsoft sample:
// HOWTO: Implementing an Application-Defined Default Printer 
// Article ID: Q193103 
//
BOOL CPrinterSettings::SetPrinterDevice(LPCTSTR pszDeviceName)
{
	HANDLE hPrinter;

	if (OpenPrinter((char *) pszDeviceName, &hPrinter, NULL) == FALSE)
		return FALSE;

	DWORD dwBytesReturned, dwBytesNeeded;

	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);

	PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR,dwBytesNeeded);

	if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded,&dwBytesReturned) == 0) {
		GlobalFree(p2);
		ClosePrinter(hPrinter);
		return FALSE;
	}
	ClosePrinter(hPrinter);

	// Allocate a global handle for DEVMODE and copy DEVMODE data.
	HGLOBAL  hDevMode = GlobalAlloc(GHND, sizeof(*p2->pDevMode));
	DEVMODE* pDevMode = (DEVMODE*) GlobalLock(hDevMode);

	CopyMemory(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode));
	GlobalUnlock(hDevMode);

	// Compute size of DEVNAMES structure you'll need.
	DWORD drvNameLen = strlen(p2->pDriverName);  // driver name
	DWORD ptrNameLen = strlen(p2->pPrinterName); // printer name
	DWORD porNameLen = strlen(p2->pPortName);    // port name
	DWORD devNameSize = sizeof(DEVNAMES) + ptrNameLen + porNameLen + 	drvNameLen + 3;

	// Allocate a global handle big enough to hold DEVNAMES.
	HGLOBAL   hDevNames = GlobalAlloc(GHND, devNameSize);
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);

	// Copy the DEVNAMES information from PRINTER_INFO_2 structure.
	pDevNames->wDriverOffset = sizeof(DEVNAMES);
	CopyMemory((char*)pDevNames + pDevNames->wDriverOffset,p2->pDriverName, drvNameLen);

	pDevNames->wDeviceOffset = (USHORT) (sizeof(DEVNAMES) + drvNameLen + 1);
	CopyMemory((char*)pDevNames + pDevNames->wDeviceOffset,p2->pPrinterName, ptrNameLen);

	pDevNames->wOutputOffset = (USHORT) (sizeof(DEVNAMES) + drvNameLen + ptrNameLen + 2);
	CopyMemory((char*)pDevNames + pDevNames->wOutputOffset,p2->pPortName, porNameLen);

	pDevNames->wDefault = 0;
	GlobalUnlock(hDevNames);
	GlobalFree(p2);             // free PRINTER_INFO_2.

	m_hDevMode = hDevMode;
	m_hDevNames = hDevNames;

	return TRUE;
}

//
// Show the printer common dialog
// Accept and store all settings
//
// This code is predominantly from a microsoft sample:
// HOWTO: Implementing an Application-Defined Default Printer 
// Article ID: Q193103 
//
BOOL CPrinterSettings::PrinterSetup(CWnd* pWnd)
{
	// Ask the user which printer to use.
	ASSERT(pWnd);
loop:
	CPageSetupDialog psd(PSD_DISABLEMARGINS|PSD_DISABLEPAGEPAINTING,pWnd);

	psd.m_psd.hInstance=AfxGetResourceHandle();
	psd.m_psd.lpPageSetupTemplateName=MAKEINTRESOURCE(IDD_PAGE_SETUP);
	psd.m_psd.Flags|=PSD_ENABLEPAGESETUPTEMPLATE;

	// Force the CPrintDialog to use our device mode & name.
	if(m_hDevMode)
		psd.m_psd.hDevMode  = CopyHandle(m_hDevMode);
	if(m_hDevNames)
		psd.m_psd.hDevNames = CopyHandle(m_hDevNames);

	// Display the dialog box and let the user make their selection.
	BOOL bResult;
	if (psd.DoModal() == IDOK)	{
		// The user clicked OK
		// (and POSSIBLY changed printers).
		// In any case, the CPrintDialog logic made a copy of the original
		// DEVMODE/DEVNAMES that we passed it and applied the user's
		// changes to that copy and discarded the original copy we passed
		// it. (NOTE: If the user had clicked CANCEL instead, the original
		// values we passed would have been returned unchanged).
		if(m_hDevMode)
			GlobalFree(m_hDevMode);                      // Free old copies.
		if(m_hDevNames)
			GlobalFree(m_hDevNames);                     // Free old copies.
		if(psd.m_psd.hDevMode)
			m_hDevMode  = CopyHandle(psd.m_psd.hDevMode);  // Save new copies.
		if(psd.m_psd.hDevNames)
			m_hDevNames = CopyHandle(psd.m_psd.hDevNames); // Save new copies.
		bResult=TRUE;
	}
	else
	{
		if(CommDlgExtendedError()==PDERR_PRINTERNOTFOUND)
		{
			if(CopyDefaultMfcPrinter()) // if no default printer then user already prompted about that
			{
				GlobalFree(psd.m_psd.hDevMode);   // Because DoModal was called,
				GlobalFree(psd.m_psd.hDevNames);  // we need to free these.
				goto loop;
			}
		}
		bResult=FALSE;
	}
	// Regardless of whether the user clicked OK or CANCEL,
	// we need to ALWAYS do a GlobalFree of CPrintDialog's
	// m_pd.hDevMode and m_pd.hDevNames upon return from
	// DoModal in order to prevent a resource leak.
	GlobalFree(psd.m_psd.hDevMode);   // Because DoModal was called,
	GlobalFree(psd.m_psd.hDevNames);  // we need to free these.
	
	DevmodePrinterName();
	return bResult;
}

//
// This code is from a microsoft sample:
// HOWTO: Implementing an Application-Defined Default Printer 
// Article ID: Q193103 
//
HANDLE CPrinterSettings::CopyHandle(HANDLE h)
{
	// Return a handle to a copy of the data
	// that the passed handle was for.
	if (!h) return NULL;
	DWORD   dwLen = GlobalSize(h);
	HANDLE hCopy = GlobalAlloc(GHND, dwLen);
	if(hCopy) {
		BYTE* lpCopy = (BYTE*)GlobalLock(hCopy);
		BYTE* lp     = (BYTE*)GlobalLock(h);
		CopyMemory(lpCopy,lp,dwLen);
		GlobalUnlock(hCopy);
		GlobalUnlock(h);
	}
	return hCopy;
}

//
// retrieve our printername
//
CString CPrinterSettings::DevmodePrinterName()
{
	if(!m_hDevMode) {
		m_strPrinterName = PRINTERNAME_UNDEFINED;
	} else {
		DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
      if(pDevMode == NULL)
      {
         m_strPrinterName = PRINTERNAME_UNDEFINED;
      }
      else
      {
         m_strPrinterName = (LPCTSTR)(pDevMode->dmDeviceName);
      }
		GlobalUnlock(m_hDevMode);
	}
	return m_strPrinterName;
}


bool CPrinterSettings::CopyDefaultMfcPrinter()
{
	bool bResult=false;
	if (m_hDevMode)  GlobalFree(m_hDevMode);
	if (m_hDevNames) GlobalFree(m_hDevNames);
	// Get MFC's printer
	PRINTDLG pd;
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd) )
	{
		// Make a copy
		m_hDevNames = CopyHandle(pd.hDevNames);
		m_hDevMode = CopyHandle(pd.hDevMode);
		DevmodePrinterName();

		DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
		GlobalUnlock(m_hDevMode);

		DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);
		GlobalUnlock(m_hDevNames);
		bResult=true;
	}
	else
	{	// no default printer, creating own defaults
		m_hDevMode=GlobalAlloc(GHND,sizeof(DEVMODE));
		DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
		lstrcpy(LPSTR(pDevMode->dmDeviceName),"dummy");
		pDevMode->dmSpecVersion=DM_SPECVERSION;
		pDevMode->dmSize=sizeof(DEVMODE);
		pDevMode->dmFields=DM_ORIENTATION|DM_PAPERSIZE|DM_COPIES|DM_YRESOLUTION|DM_TTOPTION|DM_FORMNAME;
		pDevMode->dmOrientation=DMORIENT_PORTRAIT;
		pDevMode->dmPaperSize=DMPAPER_A4;
		pDevMode->dmCopies=1;
		pDevMode->dmYResolution=300;
		pDevMode->dmTTOption=DMTT_DOWNLOAD;
		lstrcpy(LPSTR(pDevMode->dmFormName),"A4");
		GlobalUnlock(m_hDevMode);

		m_hDevNames=GlobalAlloc(GHND,sizeof(DEVNAMES)+21);
		DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);
		pDevNames->wDriverOffset=0x8;
		pDevNames->wDeviceOffset=0x11;
		pDevNames->wOutputOffset=0x17;
		memcpy(pDevNames+1,"winspool\0dummy\0FILE:",21);
		GlobalUnlock(m_hDevNames);
	}
	return bResult;
}

void CPrinterSettings::SetThisPrinter()
{
	PRINTDLG pd;
	// Save MFC's printer and select ours instead.
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd))
	{
		m_hSaveDevNames = CopyHandle(pd.hDevNames);
		m_hSaveDevMode = CopyHandle(pd.hDevMode);
	}
	m_bSavedDevInfo = true;
    AfxGetApp()->SelectPrinter(CopyHandle(m_hDevNames),CopyHandle(m_hDevMode),TRUE);
}

void CPrinterSettings::RestorePrinter()
{
	// Restore previous MFC printer if possible
	if(m_bSavedDevInfo)
	{
		AfxGetApp()->SelectPrinter(m_hSaveDevNames,m_hSaveDevMode,TRUE);
		m_bSavedDevInfo = false;
	}
	m_hSaveDevNames = NULL;
	m_hSaveDevMode = NULL;
}

void CPrinterSettings::Serialize(CArchive& ar)
{
	DWORD dwLen;
	LPBYTE lpCopy;
	if(ar.IsLoading())
	{
		m_hSaveDevMode=NULL;
		m_hSaveDevNames=NULL;
		m_bSavedDevInfo=false;
		ar >> dwLen;
		if(dwLen!=0)
		{
			m_hDevMode=GlobalAlloc(GHND, dwLen);
			lpCopy=(LPBYTE)GlobalLock(m_hDevMode);
			ar.Read(lpCopy,dwLen);
			GlobalUnlock(m_hDevMode);
		}
		else
			m_hDevMode=NULL;
		ar >> dwLen;
		if(dwLen!=0)
		{
			m_hDevNames=GlobalAlloc(GHND, dwLen);
			lpCopy=(LPBYTE)GlobalLock(m_hDevNames);
			ar.Read(lpCopy,dwLen);
			GlobalUnlock(m_hDevNames);
		}
		else
			m_hDevNames=NULL;
		DevmodePrinterName();
	}
	else
	{
		if(m_hDevMode)
		{
			dwLen=GlobalSize(m_hDevMode);
			ar << dwLen;
			LPBYTE lpCopy=(LPBYTE)GlobalLock(m_hDevMode);
			ar.Write(lpCopy,dwLen);
			GlobalUnlock(m_hDevMode);
		}
		else
		{
			dwLen=0;
			ar << dwLen;
		}
		if(m_hDevNames)
		{
			dwLen=GlobalSize(m_hDevNames);
			ar << dwLen;
			LPBYTE lpCopy=(LPBYTE)GlobalLock(m_hDevNames);
			ar.Write(lpCopy,dwLen);
			GlobalUnlock(m_hDevNames);
		}
		else
		{
			dwLen=0;
			ar << dwLen;
		}
	}
}

void CPrinterSettings::SetCustomPageSize(int nWidth,int nHeight)
{
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
	pDevMode->dmFields|=(DM_PAPERSIZE|DM_PAPERLENGTH|DM_PAPERWIDTH|DM_FORMNAME);
	pDevMode->dmPaperSize=DMPAPER_USER;
	pDevMode->dmPaperWidth=nWidth;
	pDevMode->dmPaperLength=nHeight;
   CString strFormName;
   strFormName.Format("%dx%d", nWidth, nHeight);
	lstrcpy(LPSTR(pDevMode->dmFormName), strFormName); //Custom");
	GlobalUnlock(m_hDevMode);
}


//BOOL CPrinterSettings::ClearSetCustomPageSize(int nWidth, int nHeight)
//{
//
//   PDEVMODEA pDevMode = NULL;
//
//   PRINTER_DEFAULTS pd = { NULL, NULL, PRINTER_ACCESS_USE };
//
//   //start print job
//
//   LPSTR pszPrinterName = (char *) (const char*) m_strPrinterName;
//
//   if (OpenPrinter(pszPrinterName, &m_hPrinter, &pd))
//   {
//      //get default printer DEVMODE
//      int nSize = DocumentProperties(NULL, m_hPrinter, pszPrinterName, NULL, NULL, 0);
//      pDevMode = (PDEVMODEA)LocalAlloc(LPTR, nSize);
//      if (DocumentProperties(NULL, m_hPrinter, pszPrinterName, pDevMode, NULL, DM_OUT_DEFAULT))
//      {
//
//         pDevMode->dmFields |= (DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH | DM_FORMNAME);
//         pDevMode->dmPaperSize = 0;
//         pDevMode->dmPaperWidth = nWidth;
//         pDevMode->dmPaperLength = nHeight;
//         CString strFormName;
//         strFormName.Format("%dx%d", nWidth, nHeight);
//         lstrcpy(LPSTR(pDevMode->dmFormName), strFormName);
//         if (DocumentProperties(NULL, m_hPrinter, pszPrinterName, pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER))
//         {
//
//            TRACE("Set Document Properties");
//
//         }
//
//         HDC hDC = CreateDC("", pszPrinterName, NULL, pDevMode);
//
//         int iW = ::GetDeviceCaps(hDC, HORZSIZE);
//         int iH = ::GetDeviceCaps(hDC, VERTSIZE);
//
//         LocalFree(pDevMode);
//
//      }
//
//   }
//
//   return TRUE;
//
//}


CSize CPrinterSettings::GetCustomPageSize()
{
   CSize size;
   DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
   if((pDevMode->dmFields & DM_PAPERLENGTH) && (pDevMode->dmFields & DM_PAPERWIDTH))
   {
      size = CSize(pDevMode->dmPaperWidth,pDevMode->dmPaperLength);
   }
   lstrcpy(LPSTR(pDevMode->dmFormName),""); //Custom");
   GlobalUnlock(m_hDevMode);
   return size;
}

const char * get_page_size_form_name(int iPageSize)
{
   switch(iPageSize)
   {
   case DMPAPER_A3:
      return "A3";
   case DMPAPER_A4:
         return "A4";
   case DMPAPER_A5:
      return "A5";
   case DMPAPER_B5:
      return "B5";
   case DMPAPER_ENV_B5:
      return "iso B5";
   case DMPAPER_TABLOID_EXTRA:
      return "Arch B";
   case DMPAPER_LEGAL:
      return "Legal";
   case DMPAPER_LETTER:
      return "Letter";
   case DMPAPER_B5_EXTRA:
      return "B5";
   case DMPAPER_TABLOID:
      return "Tabloid";
   case DMPAPER_JENV_CHOU3:
      return "Japanese Envelope Choukei 3";
   case DMPAPER_B_PLUS:
      return "13x19";
   case DMPAPER_ENV_DL:
      return "Envelope DL";
   case DMPAPER_ENV_10:
      return "Envelope #10";
   default:
         return "Letter";
   }

}
// MD6.2.1 - Infinisys Ltd.
void CPrinterSettings::SetPageSize(int iPageSize)
{
   DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
   pDevMode->dmFields|=DM_PAPERSIZE;
   pDevMode->dmPaperSize=iPageSize;
   pDevMode->dmFields &=~(DM_PAPERLENGTH | DM_PAPERWIDTH | DM_FORMNAME);
   pDevMode->dmPaperWidth=0;
   pDevMode->dmPaperLength=0;
   lstrcpy(LPSTR(pDevMode->dmFormName),get_page_size_form_name(iPageSize));
   GlobalUnlock(m_hDevMode);
   {
      DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
      GlobalUnlock(m_hDevMode);
   }

   }
// MD6.2.1 - Infinisys Ltd.
int CPrinterSettings::GetPageSize()
{
   int iPageSize = DMPAPER_LETTER;
   DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
   if(pDevMode != NULL && pDevMode->dmFields & DM_PAPERSIZE)
   {
      iPageSize = pDevMode->dmPaperSize;
   }
   GlobalUnlock(m_hDevMode);
   return iPageSize;
}
// MD6.2.1 - Infinisys Ltd.
void CPrinterSettings::SetPageScale(int iPageScale)
{
   DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
   pDevMode->dmFields|=(DM_SCALE);
   pDevMode->dmScale=iPageScale;
   GlobalUnlock(m_hDevMode);
}
// MD6.2.1 - Infinisys Ltd.
int CPrinterSettings::GetPageScale()
{
   DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
   int iPageScale = 100;
   if(pDevMode != NULL && pDevMode->dmFields & DM_SCALE)
   {
      iPageScale = pDevMode->dmScale;
   }
   GlobalUnlock(m_hDevMode);
   return iPageScale;
}

// Bitmap printing - Infinisys Ltd.
short CPrinterSettings::GetQuality()
{
   
   DEVMODE* pDev=(DEVMODE*)GlobalLock(m_hDevMode);

   short shQuality = pDev->dmPrintQuality;

   GlobalUnlock(m_hDevMode);

   return shQuality;

}


short CPrinterSettings::GetPageOrientation()
{
   short nOrientation = DMORIENT_PORTRAIT;
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
   if(pDevMode != NULL && pDevMode->dmFields & DM_ORIENTATION)
   {
      nOrientation=pDevMode->dmOrientation;
   }
	GlobalUnlock(m_hDevMode);
	return nOrientation;
}

void CPrinterSettings::SetPageOrientation(short nOrientation)
{
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
	pDevMode->dmFields|=DM_ORIENTATION;
	pDevMode->dmOrientation=nOrientation;
	GlobalUnlock(m_hDevMode);
}



page_size g_pagesizea[] = {
{1163, 814, DMPAPER_A3, DMORIENT_LANDSCAPE},
{ 595, 842, DMPAPER_A4, DMORIENT_PORTRAIT },
{ 612, 719, DMPAPER_LETTER, DMORIENT_PORTRAIT },
{ 612, 1009, DMPAPER_LEGAL, DMORIENT_PORTRAIT },
{ 420, 595, DMPAPER_A5, DMORIENT_PORTRAIT },
{ 612, 791, 1, DMORIENT_PORTRAIT },
{ 791, 612, 1, DMORIENT_LANDSCAPE },
{ 612, 791, 2, DMORIENT_PORTRAIT },
{ 791, 612, 2, DMORIENT_LANDSCAPE },
{ 791, 1225, 3, DMORIENT_PORTRAIT },
{ 1225, 791, 3, DMORIENT_LANDSCAPE },
{ 1225, 791, 4, DMORIENT_PORTRAIT },
{ 791, 1225, 4, DMORIENT_LANDSCAPE },
{ 612, 1009, 5, DMORIENT_PORTRAIT },
{ 1009, 612, 5, DMORIENT_LANDSCAPE },
{ 397, 612, 6, DMORIENT_PORTRAIT },
{ 612, 397, 6, DMORIENT_LANDSCAPE },
{ 522, 757, 7, DMORIENT_PORTRAIT },
{ 757, 522, 7, DMORIENT_LANDSCAPE },
{ 842, 1191, 8, DMORIENT_PORTRAIT },
{ 1191, 842, 8, DMORIENT_LANDSCAPE },
{ 595, 842, 9, DMORIENT_PORTRAIT },
{ 842, 595, 9, DMORIENT_LANDSCAPE },
{ 595, 842, 10, DMORIENT_PORTRAIT },
{ 842, 595, 10, DMORIENT_LANDSCAPE },
{ 420, 595, 11, DMORIENT_PORTRAIT },
{ 595, 420, 11, DMORIENT_LANDSCAPE },
{ 729, 1032, 12, DMORIENT_PORTRAIT },
{ 1032, 729, 12, DMORIENT_LANDSCAPE },
{ 516, 729, 13, DMORIENT_PORTRAIT },
{ 729, 516, 13, DMORIENT_LANDSCAPE },
{ 612, 935, 14, DMORIENT_PORTRAIT },
{ 935, 612, 14, DMORIENT_LANDSCAPE },
{ 609, 780, 15, DMORIENT_PORTRAIT },
{ 780, 609, 15, DMORIENT_LANDSCAPE },
{ 720, 1009, 16, DMORIENT_PORTRAIT },
{ 1009, 720, 16, DMORIENT_LANDSCAPE },
{ 791, 1225, 17, DMORIENT_PORTRAIT },
{ 1225, 791, 17, DMORIENT_LANDSCAPE },
{ 612, 791, 18, DMORIENT_PORTRAIT },
{ 791, 612, 18, DMORIENT_LANDSCAPE },
{ 278, 638, 19, DMORIENT_PORTRAIT },
{ 638, 278, 19, DMORIENT_LANDSCAPE },
{ 298, 683, 20, DMORIENT_PORTRAIT },
{ 683, 298, 20, DMORIENT_LANDSCAPE },
{ 323, 746, 21, DMORIENT_PORTRAIT },
{ 746, 323, 21, DMORIENT_LANDSCAPE },
{ 343, 791, 22, DMORIENT_PORTRAIT },
{ 791, 343, 22, DMORIENT_LANDSCAPE },
{ 360, 828, 23, DMORIENT_PORTRAIT },
{ 828, 360, 23, DMORIENT_LANDSCAPE },
{ 1225, 1585, 24, DMORIENT_PORTRAIT },
{ 1585, 1225, 24, DMORIENT_LANDSCAPE },
{ 1585, 2449, 25, DMORIENT_PORTRAIT },
{ 2449, 1585, 25, DMORIENT_LANDSCAPE },
{ 2449, 3169, 26, DMORIENT_PORTRAIT },
{ 3169, 2449, 26, DMORIENT_LANDSCAPE },
{ 312, 624, 27, DMORIENT_PORTRAIT },
{ 624, 312, 27, DMORIENT_LANDSCAPE },
{ 459, 649, 28, DMORIENT_PORTRAIT },
{ 649, 459, 28, DMORIENT_LANDSCAPE },
{ 918, 1298, 29, DMORIENT_PORTRAIT },
{ 1298, 918, 29, DMORIENT_LANDSCAPE },
{ 649, 918, 30, DMORIENT_PORTRAIT },
{ 918, 649, 30, DMORIENT_LANDSCAPE },
{ 323, 459, 31, DMORIENT_PORTRAIT },
{ 459, 323, 31, DMORIENT_LANDSCAPE },
{ 323, 649, 32, DMORIENT_PORTRAIT },
{ 649, 323, 32, DMORIENT_LANDSCAPE },
{ 709, 1001, 33, DMORIENT_PORTRAIT },
{ 1001, 709, 33, DMORIENT_LANDSCAPE },
{ 499, 709, 34, DMORIENT_PORTRAIT },
{ 709, 499, 34, DMORIENT_LANDSCAPE },
{ 499, 354, 35, DMORIENT_PORTRAIT },
{ 354, 499, 35, DMORIENT_LANDSCAPE },
{ 312, 652, 36, DMORIENT_PORTRAIT },
{ 652, 312, 36, DMORIENT_LANDSCAPE },
{ 278, 541, 37, DMORIENT_PORTRAIT },
{ 541, 278, 37, DMORIENT_LANDSCAPE },
{ 261, 468, 38, DMORIENT_PORTRAIT },
{ 468, 261, 38, DMORIENT_LANDSCAPE },
{ 1071, 791, 39, DMORIENT_PORTRAIT },
{ 791, 1071, 39, DMORIENT_LANDSCAPE },
{ 612, 865, 40, DMORIENT_PORTRAIT },
{ 865, 612, 40, DMORIENT_LANDSCAPE },
{ 612, 935, 41, DMORIENT_PORTRAIT },
{ 935, 612, 41, DMORIENT_LANDSCAPE },
{ 709, 1001, 42, DMORIENT_PORTRAIT },
{ 1001, 709, 42, DMORIENT_LANDSCAPE },
{ 283, 420, 43, DMORIENT_PORTRAIT },
{ 420, 283, 43, DMORIENT_LANDSCAPE },
{ 649, 791, 44, DMORIENT_PORTRAIT },
{ 791, 649, 44, DMORIENT_LANDSCAPE },
{ 720, 791, 45, DMORIENT_PORTRAIT },
{ 791, 720, 45, DMORIENT_LANDSCAPE },
{ 1080, 791, 46, DMORIENT_PORTRAIT },
{ 791, 1080, 46, DMORIENT_LANDSCAPE },
{ 624, 624, 47, DMORIENT_PORTRAIT },
{ 624, 624, 47, DMORIENT_LANDSCAPE },
{ 683, 865, 50, DMORIENT_PORTRAIT },
{ 865, 683, 50, DMORIENT_LANDSCAPE },
{ 683, 1080, 51, DMORIENT_PORTRAIT },
{ 1080, 683, 51, DMORIENT_LANDSCAPE },
{ 865, 1295, 52, DMORIENT_PORTRAIT },
{ 1295, 865, 52, DMORIENT_LANDSCAPE },
{ 666, 913, 53, DMORIENT_PORTRAIT },
{ 913, 666, 53, DMORIENT_LANDSCAPE },
{ 612, 791, 54, DMORIENT_PORTRAIT },
{ 791, 612, 54, DMORIENT_LANDSCAPE },
{ 595, 842, 55, DMORIENT_PORTRAIT },
{ 842, 595, 55, DMORIENT_LANDSCAPE },
{ 683, 865, 56, DMORIENT_PORTRAIT },
{ 865, 683, 56, DMORIENT_LANDSCAPE },
{ 643, 1009, 57, DMORIENT_PORTRAIT },
{ 1009, 643, 57, DMORIENT_LANDSCAPE },
{ 865, 1380, 58, DMORIENT_PORTRAIT },
{ 1380, 865, 58, DMORIENT_LANDSCAPE },
{ 612, 913, 59, DMORIENT_PORTRAIT },
{ 913, 612, 59, DMORIENT_LANDSCAPE },
{ 595, 935, 60, DMORIENT_PORTRAIT },

{ 935, 595, 60, DMORIENT_LANDSCAPE },

{ 420, 595, 61, DMORIENT_PORTRAIT },
{ 595, 420, 61, DMORIENT_LANDSCAPE },

{ 516, 729, 62, DMORIENT_PORTRAIT },

{ 729, 516, 62, DMORIENT_LANDSCAPE },
{ 913, 1261, 63, DMORIENT_PORTRAIT },
{ 1261, 913, 63, DMORIENT_LANDSCAPE },
{ 493, 666, 64, DMORIENT_PORTRAIT },
{ 666, 493, 64, DMORIENT_LANDSCAPE },
{ 570, 782, 65, DMORIENT_PORTRAIT },
{ 782, 570, 65, DMORIENT_LANDSCAPE },
{ 1191, 1684, 66, DMORIENT_PORTRAIT },
{ 1684, 1191, 66, DMORIENT_LANDSCAPE },
{ 842, 1191, 67, DMORIENT_PORTRAIT },
{ 1191, 842, 67, DMORIENT_LANDSCAPE },
{ 913, 1261, 68, DMORIENT_PORTRAIT },
{ 1261, 913, 68, DMORIENT_LANDSCAPE },
{ 567, 420, 69, DMORIENT_PORTRAIT },
{ 420, 567, 69, DMORIENT_LANDSCAPE },
{ 298, 420, 70, DMORIENT_PORTRAIT },
{ 420, 298, 70, DMORIENT_LANDSCAPE },
{ 680, 941, 71, DMORIENT_PORTRAIT },
{ 941, 680, 71, DMORIENT_LANDSCAPE },
{ 612, 785, 72, DMORIENT_PORTRAIT },
{ 785, 612, 72, DMORIENT_LANDSCAPE },
{ 340, 666, 73, DMORIENT_PORTRAIT },
{ 666, 340, 73, DMORIENT_LANDSCAPE },
{ 255, 581, 74, DMORIENT_PORTRAIT },
{ 581, 255, 74, DMORIENT_LANDSCAPE },
{ 791, 612, 75, DMORIENT_PORTRAIT },
{ 612, 791, 75, DMORIENT_LANDSCAPE },
{ 1191, 842, 76, DMORIENT_PORTRAIT },
{ 842, 1191, 76, DMORIENT_LANDSCAPE },
{ 842, 595, 77, DMORIENT_PORTRAIT },
{ 595, 842, 77, DMORIENT_LANDSCAPE },
{ 595, 420, 78, DMORIENT_PORTRAIT },
{ 420, 595, 78, DMORIENT_LANDSCAPE },
{ 1032, 729, 79, DMORIENT_PORTRAIT },
{ 729, 1032, 79, DMORIENT_LANDSCAPE },
{ 729, 516, 80, DMORIENT_PORTRAIT },
{ 516, 729, 80, DMORIENT_LANDSCAPE },
{ 420, 283, 81, DMORIENT_PORTRAIT },
{ 283, 420, 81, DMORIENT_LANDSCAPE },
{ 420, 567, 82, DMORIENT_PORTRAIT },
{ 567, 420, 82, DMORIENT_LANDSCAPE },
{ 420, 298, 83, DMORIENT_PORTRAIT },
{ 298, 420, 83, DMORIENT_LANDSCAPE },
{ 941, 680, 84, DMORIENT_PORTRAIT },
{ 680, 941, 84, DMORIENT_LANDSCAPE },
{ 785, 612, 85, DMORIENT_PORTRAIT },
{ 612, 785, 85, DMORIENT_LANDSCAPE },
{ 666, 340, 86, DMORIENT_PORTRAIT },
{ 340, 666, 86, DMORIENT_LANDSCAPE },
{ 581, 255, 87, DMORIENT_PORTRAIT },
{ 255, 581, 87, DMORIENT_LANDSCAPE },
{ 363, 516, 88, DMORIENT_PORTRAIT },
{ 516, 363, 88, DMORIENT_LANDSCAPE },
{ 516, 363, 89, DMORIENT_PORTRAIT },
{ 363, 516, 89, DMORIENT_LANDSCAPE },
{ 865, 791, 90, DMORIENT_PORTRAIT },
{ 791, 865, 90, DMORIENT_LANDSCAPE },
{ 298, 666, 91, DMORIENT_PORTRAIT },
{ 666, 298, 91, DMORIENT_LANDSCAPE },
{ 666, 298, 92, DMORIENT_PORTRAIT },
{ 298, 666, 92, DMORIENT_LANDSCAPE },
{ 533, 737, 93, DMORIENT_PORTRAIT },
{ 737, 533, 93, DMORIENT_LANDSCAPE },
{ 369, 522, 94, DMORIENT_PORTRAIT },
{ 522, 369, 94, DMORIENT_LANDSCAPE },
{ 397, 575, 95, DMORIENT_PORTRAIT },
{ 575, 397, 95, DMORIENT_LANDSCAPE },
{ 289, 468, 96, DMORIENT_PORTRAIT },
{ 468, 289, 96, DMORIENT_LANDSCAPE },
{ 289, 499, 97, DMORIENT_PORTRAIT },
{ 499, 289, 97, DMORIENT_LANDSCAPE },
{ 354, 499, 98, DMORIENT_PORTRAIT },
{ 499, 354, 98, DMORIENT_LANDSCAPE },
{ 312, 590, 99, DMORIENT_PORTRAIT },
{ 590, 312, 99, DMORIENT_LANDSCAPE },
{ 312, 624, 100, DMORIENT_PORTRAIT },
{ 624, 312, 100, DMORIENT_LANDSCAPE },
{ 340, 652, 101, DMORIENT_PORTRAIT },
{ 652, 340, 101, DMORIENT_LANDSCAPE },
{ 454, 652, 102, DMORIENT_PORTRAIT },
{ 652, 454, 102, DMORIENT_LANDSCAPE },
{ 340, 876, 103, DMORIENT_PORTRAIT },
{ 876, 340, 103, DMORIENT_LANDSCAPE },
{ 649, 918, 104, DMORIENT_PORTRAIT },
{ 918, 649, 104, DMORIENT_LANDSCAPE },
{ 918, 1298, 105, DMORIENT_PORTRAIT },
{ 1298, 918, 105, DMORIENT_LANDSCAPE },
{ 737, 533, 106, DMORIENT_PORTRAIT },
{ 533, 737, 106, DMORIENT_LANDSCAPE },
{ 522, 369, 107, DMORIENT_PORTRAIT },
{ 369, 522, 107, DMORIENT_LANDSCAPE },
{ 575, 397, 108, DMORIENT_PORTRAIT },
{ 397, 575, 108, DMORIENT_LANDSCAPE },
{ 468, 289, 109, DMORIENT_PORTRAIT },
{ 289, 468, 109, DMORIENT_LANDSCAPE },
{ 499, 289, 110, DMORIENT_PORTRAIT },
{ 289, 499, 110, DMORIENT_LANDSCAPE },
{ 499, 354, 111, DMORIENT_PORTRAIT },
{ 354, 499, 111, DMORIENT_LANDSCAPE },
{ 590, 312, 112, DMORIENT_PORTRAIT },
{ 312, 590, 112, DMORIENT_LANDSCAPE },
{ 624, 312, 113, DMORIENT_PORTRAIT },
{ 312, 624, 113, DMORIENT_LANDSCAPE },
{ 652, 340, 114, DMORIENT_PORTRAIT },
{ 340, 652, 114, DMORIENT_LANDSCAPE },
{ 652, 454, 115, DMORIENT_PORTRAIT },
{ 454, 652, 115, DMORIENT_LANDSCAPE },
{ 876, 340, 116, DMORIENT_PORTRAIT },
{ 340, 876, 116, DMORIENT_LANDSCAPE },
{ 918, 649, 117, DMORIENT_PORTRAIT },
{ 649, 918, 117, DMORIENT_LANDSCAPE },
{ 1298, 918, 118, DMORIENT_PORTRAIT },
{ 918, 1298, 118, DMORIENT_LANDSCAPE },
{0, 0, 0,0 }
};

// PC Draft 7 - 2020
bool CPrinterSettings::defer_set_page_size(int cx, int cy)
{

   auto ppagesize = page_size::from_size(cx, cy);

   if(ppagesize)
   {
   
      SetPageSize(ppagesize->m_iPageSize);
      SetPageOrientation(ppagesize->m_iOrientation);

      return true;

   }
   else
   {

      SetCustomPageSize(cx, cy);

   }

   return false;

}


page_size* page_size::from_index(int iPageSize, int iOrientation)
{

   int i = 0;

   while (g_pagesizea[i].m_iWidth > 0)
   {

      if (g_pagesizea[i].m_iPageSize == iPageSize
         && g_pagesizea[i].m_iOrientation == iOrientation)
      {

         return &g_pagesizea[i];

      }

      i++;

   }

   return nullptr;

}


page_size * page_size::from_size(int cx, int cy)
{

   int i = 0;

   while (g_pagesizea[i].m_iWidth > 0)
   {

      if (g_pagesizea[i].m_iWidth == cx
         && g_pagesizea[i].m_iHeight == cy)
      {

         return &g_pagesizea[i];

      }

      i++;

   }

   return nullptr;

}