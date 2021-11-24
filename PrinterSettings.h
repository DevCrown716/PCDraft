// PrinterSettings.h: Interface of CPrinterSettings.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTIT_H__2B9969C1_0872_11D3_9A02_0000B45414CC__INCLUDED_)
#define AFX_PRINTIT_H__2B9969C1_0872_11D3_9A02_0000B45414CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winspool.h>	// for: ClosePrinter,GetPrinter,PRINTER_INFO_2


#define PRINTERNAME_UNDEFINED			"printer not set..."


struct page_size
{
   int m_iWidth;
   int m_iHeight;
   int m_iPageSize;
   int m_iOrientation;

   CSize get_size() const { return { m_iWidth, m_iHeight }; }

   static page_size * from_index(int iPageSize, int iOrientation);
   static page_size * from_size(int cx, int cy);


};


class CPrinterSettings : public CObject  
{
	DECLARE_SERIAL(CPrinterSettings)
public:
	CPrinterSettings();
	virtual ~CPrinterSettings();
// for use e.g. in CArray
public:
	void operator=(const CPrinterSettings* src);
	void operator=(const CPrinterSettings& src);
	virtual void Serialize(CArchive& ar);

// general interface
public:
   // PC Draft 7 - page size
   bool defer_set_page_size(int cx, int cy);
	// get the settings for the default MFC Printer
	bool CopyDefaultMfcPrinter(void);
	// Set the default MFC Printer to this
	void SetThisPrinter(void);
	// Set the default MFC Printer by Name
	BOOL SetPrinterDevice(LPCTSTR pszDeviceName);
	// Show the common dialog and get the settings
	BOOL PrinterSetup( CWnd* pWnd);
	// restore previous printer settings (if safed )
	void RestorePrinter(void);
	// check to see if our printer is still available
	BOOL IsPrinterAvailable(LPCTSTR pszDeviceName);
	// retrieve the human readable printername :)
	CString GetPrinterName(void) const { return m_strPrinterName; }
	void SetCustomPageSize(int nWidth,int nHeight);
   //BOOL ClearSetCustomPageSize(int nWidth, int nHeight);
   CSize GetCustomPageSize(); // MD70 - Infinisys Ltd.
   void SetPageSize(int iPageSize); // MD6.2.1 - Infinisys Ltd.
   int GetPageSize(); // MD6.2.1 - Infinisys Ltd.
   void SetPageScale(int iPageScale); // MD6.2.1 - Infinisys Ltd.
   int GetPageScale(); // MD6.2.1 - Infinisys Ltd.
   short GetQuality(); // Bitmap Printing - Infinisys Ltd.
   short GetPageOrientation();
	void SetPageOrientation(short bOrientation);
private:	// but not useless:  :o)
   HANDLE   m_hPrinter;
	CString DevmodePrinterName(void);
	HANDLE CopyHandle(HANDLE h); 
	HANDLE m_hDevMode;
	HANDLE m_hDevNames;
	HANDLE m_hSaveDevMode;
	HANDLE m_hSaveDevNames;
	bool m_bSavedDevInfo;
	CString m_strPrinterName;
};

#endif // !defined(AFX_PRINTIT_H__2B9969C1_0872_11D3_9A02_0000B45414CC__INCLUDED_)
