#pragma once


// CStatus
BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
class CStatus : public CStatusBar
{
	DECLARE_DYNAMIC(CStatus)

public:
	CStatus();
	virtual ~CStatus();
   BOOL CreateEx(CWnd* pParentWnd,DWORD dwCtrlStyle,DWORD dwStyle,UINT nID)
   {
      ASSERT_VALID(pParentWnd);   // must have a parent

      // save the style (some of these style bits are MFC specific)
      m_dwStyle = (dwStyle & CBRS_ALL);

      // translate MFC style bits to windows style bits
      dwStyle &= ~CBRS_ALL;
      dwStyle |= CCS_NOPARENTALIGN | CCS_NOMOVEY | CCS_NODIVIDER | CCS_NORESIZE;
         dwStyle |= SBARS_SIZEGRIP;
      dwStyle |= dwCtrlStyle;
#define AFX_WNDCOMMCTL_BAR_REG          0x01000
      // initialize common controls
      VERIFY(AfxEndDeferRegisterClass(AFX_WNDCOMMCTL_BAR_REG));

      // create the HWND
      CRect rect; rect.SetRectEmpty();
      return CWnd::Create(STATUSCLASSNAME,NULL,dwStyle,rect,pParentWnd,nID);
   }


protected:
	DECLARE_MESSAGE_MAP()
};


