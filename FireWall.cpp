// FireWall.cpp: implementation of the CFireWall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "FireWall.h"
#include <afxmt.h>
#include "WinVistaTools.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#include "netfw.h"
#include <objbase.h>
#include <oleauto.h>

#ifdef _PERSONAL_EDITION
	const LPWSTR CFireWall::m_strFriendlyName = L"PC Draft " kVersionW L" PE";
#else 
	const LPWSTR CFireWall::m_strFriendlyName = L"PC Draft " kVersionW;
#endif

// BOOL CALLBACK CFireWall::ChangeUserProc (HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

#define kDuration 60

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CFireWall::CFireWall() : m_pFwProfile (NULL) , m_lastCheck (0), m_bSettingsChanged (false)
{

}

CFireWall::~CFireWall()
{

}


void CFireWall::Initialize ()
{
    HRESULT hr = S_OK;
    INetFwMgr* pFwMgr = NULL;
    INetFwPolicy* pFwPolicy = NULL;

	if (m_pFwProfile) return;

	CLSID NetFwCls;

	hr = CLSIDFromProgID (L"HNetCfg.FwMgr", &NetFwCls);
    if (FAILED(hr))
    {
		return; // FileWall not installed
    }
    // Create an instance of the firewall settings manager.
    hr = CoCreateInstance (NetFwCls, NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwMgr), (void**)&pFwMgr);
    if (FAILED(hr))
    {
		return; // Firewall is not accessible by some reason
		//AfxThrowOleException (hr);
    }

    // Retrieve the local firewall policy.
    hr = pFwMgr->get_LocalPolicy (&pFwPolicy);
    if (FAILED (hr))
    {
		pFwMgr->Release ();
		return;
		//AfxThrowOleException (hr);
    }

    // Retrieve the firewall profile currently in effect.
    hr = pFwPolicy->get_CurrentProfile (&m_pFwProfile);
    if (FAILED(hr))
    {
		pFwPolicy->Release ();
		pFwMgr->Release ();
		//AfxThrowOleException (hr);
    }
}

//*************************************************
//  Check if PCDraft is enabled in Firewall (every kDuration seconds)
//  if fw settings were changed since last time then m_bSettingsChanged set TRUE
//	RETURNS FALSE if PCDraft can not be enabled
//*************************************************
bool CFireWall::Check ()
{
//#ifdef _DEBUG
//	return true;
//#endif

	bool bCheck = m_pFwProfile != NULL ;// && (m_lastCheck == 0 || time (0) > m_lastCheck); 
	bool bUserChanged = false;
	m_bSettingsChanged = false;
//	static time_t tt = time (0) + 60;
	if (!bCheck)
	{
		return true;
	}
    bool IsStartedInUAC = false;
	while (bCheck)
	{
		try 
		{
			ASSERT (m_lastCheck = time (0));
			TRACE("check %s\n",ctime (&m_lastCheck));
//			m_lastCheck = time (0) + kDuration;
	/*		if (!IsApplicationEnabled () && !bUserChanged)
			{
				if (AfxMessageBox (IDS_FIREWALL_ASK_CHANGE, MB_OKCANCEL ) != IDOK)
					break;
			}*/

            EnableApplication ();

			if (m_bSettingsChanged)
			{
				Sleep (100); // Firewall should apply changes
			}

			bCheck = false;
			if (bUserChanged)
			{
				RevertToSelf (); // restore current user to avoid security violation
			}
//			if (m_lastCheck < tt)
			return true;
		}
		catch (COleException* e)
		{
			SCODE sc = e->m_sc;
			e->Delete();

			if (sc != E_ACCESSDENIED)
			{
				return false; // some COM error, try later
			}

            if (!IsStartedInUAC && WinVistaTools::IsVistaOrHigher() && WinVistaTools::IsElevatedProcess() != S_OK)
            { 
                // Check if UAC enabled
                CString strAppName;
                GetModuleFileName(::GetModuleHandle(NULL), strAppName.GetBuffer(_MAX_PATH), _MAX_PATH);
                strAppName.ReleaseBuffer();

                WinVistaTools::StartElevated(NULL, strAppName, "/enablefirewall");
                IsStartedInUAC = true;
                continue;
            }
		}

		bUserChanged = ChangeUser ();

		if (!bUserChanged)
		{
			break;
		}
	}

	HWND hWnd = AfxGetMainWnd ()->GetSafeHwnd ();
	while (::IsWindow (hWnd)) // if there are unsaved files user can cancel action then we cycle until application will be closed
	{
		::SendMessage (hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	}

	return false;
}


//*************************************************
// IsEnabled checks Firewall status
// TRUE - FW is On
// FALSE - FW is Off
//*************************************************
bool CFireWall::IsEnabled () const
{
    VARIANT_BOOL fwEnabled = VARIANT_FALSE;

	if (m_pFwProfile)
	{
		// Get the current state of the firewall.
		HRESULT hr = m_pFwProfile->get_FirewallEnabled (&fwEnabled);
		if (FAILED (hr))
		{
			AfxThrowOleException (hr);
		}
	}
	TRACE("Firefall is %s\n",fwEnabled == VARIANT_TRUE ? "on" : "off");

    return fwEnabled == VARIANT_TRUE;
}


//*************************************************
// IsExceptionsEnabled checks Firewall status
// TRUE - FW exception list is On
// FALSE - FW exception list is Off
//*************************************************
bool CFireWall::IsExceptionsEnabled () const
{
    VARIANT_BOOL fwNotEnabled = VARIANT_FALSE;

	if (m_pFwProfile)
	{
		// Get the current state of the exception list.
		HRESULT hr = m_pFwProfile->get_ExceptionsNotAllowed (&fwNotEnabled);
		if (FAILED (hr))
		{
			AfxThrowOleException (hr);
		}
	}
	TRACE("Exceptions is %s\n",fwNotEnabled == VARIANT_TRUE ? "off" : "on");
    return fwNotEnabled == VARIANT_FALSE;
}


void CFireWall::EnableExceptions()
{
	if (IsExceptionsEnabled ()) return;

	HRESULT hr = m_pFwProfile->put_ExceptionsNotAllowed (VARIANT_FALSE);
	if (FAILED (hr))
	{
		AfxThrowOleException (hr);
	}

	m_bSettingsChanged = true;
}



bool CFireWall::ChangeUser ()
{


	if (AfxMessageBox (IDS_FIREWALL_ALERT, MB_YESNO|MB_ICONINFORMATION) == IDNO)
	{
		return false;
	}

	if (!::DialogBox (GetModuleHandle (NULL), MAKEINTRESOURCE (IDD_CHANGE_USER), AfxGetMainWnd ()->m_hWnd, ChangeUserProc))
	{
		return false;
	}
	return true;

}

bool CFireWall::IsApplicationEnabled ()
{

    INetFwAuthorizedApplication* pFwApp = NULL;
    INetFwAuthorizedApplications* pFwApps = NULL;
	VARIANT_BOOL fwEnabled;

	if (!IsEnabled ()) return true; // firewall is off

	if (!IsExceptionsEnabled ()) return false;


	// Retrieve the authorized application collection.
    HRESULT hr = m_pFwProfile->get_AuthorizedApplications (&pFwApps);

    if (FAILED(hr))
    {
		AfxThrowOleException (hr);
    }
	// Allocate a BSTR for the process image file name.

	CString strAppName;
	GetModuleFileName (::GetModuleHandle (NULL), strAppName.GetBuffer (_MAX_PATH),_MAX_PATH);
	strAppName.ReleaseBuffer ();
	TRACE("Enable application %s starts.\n", (LPCTSTR)strAppName);
	BSTR bstrFileName = strAppName.AllocSysString ();
	if (SysStringLen (bstrFileName) == 0)
	{
		pFwApps->Release ();
		AfxThrowOleException (E_OUTOFMEMORY);
	}
    // Attempt to retrieve the authorized application.
    hr = pFwApps->Item (bstrFileName, &pFwApp);
    ::SysFreeString (bstrFileName);
    if (SUCCEEDED(hr))
    {
        // Find out if the authorized application is enabled.
        hr = pFwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr))
		{
			pFwApp->Release ();
			pFwApps->Release ();
			AfxThrowOleException (hr);
		}

		pFwApp->Release ();
		pFwApps->Release ();
		return 	fwEnabled == VARIANT_TRUE;
	}        
	pFwApps->Release ();
 	return false;
}


void CFireWall::EnableApplication()
{

	m_bSettingsChanged = false;

	if (m_pFwProfile == NULL ||
		!IsEnabled ())
	{
		return;
	}

    EnableExceptions();

    INetFwAuthorizedApplication* pFwApp = NULL;
    INetFwAuthorizedApplications* pFwApps = NULL;
	VARIANT_BOOL fwEnabled;
	
	// Retrieve the authorized application collection.
    HRESULT hr = m_pFwProfile->get_AuthorizedApplications (&pFwApps);

    if (FAILED(hr))
    {
		AfxThrowOleException (hr);
    }

	// Allocate a BSTR for the process image file name.

	CString strAppName;
	GetModuleFileName (::GetModuleHandle (NULL), strAppName.GetBuffer (_MAX_PATH),_MAX_PATH);
	strAppName.ReleaseBuffer ();
	TRACE("Enable application %s starts.\n", (LPCTSTR)strAppName);
	BSTR bstrFileName = strAppName.AllocSysString ();
	if (SysStringLen (bstrFileName) == 0)
	{
		pFwApps->Release ();
        AfxThrowOleException(E_OUTOFMEMORY);
	}
    // Attempt to retrieve the authorized application.
    hr = pFwApps->Item (bstrFileName, &pFwApp);
    if (SUCCEEDED(hr))
    {
	    SysFreeString (bstrFileName);
        // Find out if the authorized application is enabled.
        hr = pFwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr))
		{
			pFwApp->Release ();
			pFwApps->Release ();
			AfxThrowOleException (hr);
		}
		if  (fwEnabled == VARIANT_TRUE)
		{
			pFwApp->Release ();
			pFwApps->Release ();
			return;
		}
        
		hr = pFwApp->put_Enabled (VARIANT_TRUE);
		pFwApp->Release ();
		pFwApps->Release ();

        if (FAILED(hr))
		{
            AfxThrowOleException(hr);
		}
		m_bSettingsChanged = true;
		return;
    }
	
	
	
	CLSID NetFwAuthorizedApplicationCls;

	hr = CLSIDFromProgID (L"HNetCfg.FwAuthorizedApplication", &NetFwAuthorizedApplicationCls);
	if (FAILED(hr))
	{
	    SysFreeString (bstrFileName);
		pFwApps->Release ();
        AfxThrowOleException(hr);
	}

    // Create an instance of an authorized application.
    hr = CoCreateInstance(
            NetFwAuthorizedApplicationCls,
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwAuthorizedApplication),
            (void**)&pFwApp
            );

	if (FAILED(hr))
	{
	    SysFreeString (bstrFileName);
		pFwApps->Release ();
        AfxThrowOleException(hr);
	}

    // Set the process image file name.
    hr = pFwApp->put_ProcessImageFileName (bstrFileName);
    SysFreeString (bstrFileName);

    if (FAILED (hr))
    {
		pFwApp->Release ();
		pFwApps->Release ();
        AfxThrowOleException(hr);
    }

    // Allocate a BSTR for the application friendly name.
    BSTR bstrFriendlyName = SysAllocString (m_strFriendlyName);
    if (SysStringLen (bstrFriendlyName) == 0)
    {
        AfxThrowOleException(E_OUTOFMEMORY);
    }

    // Set the application friendly name.
    hr = pFwApp->put_Name (bstrFriendlyName);
    SysFreeString (bstrFriendlyName);

    if (SUCCEEDED (hr))
    {
	   // Add the application to the collection.
		hr = pFwApps->Add (pFwApp);
	}

	pFwApps->Release ();
	pFwApp->Release ();

    if (FAILED(hr))
    {
        AfxThrowOleException(hr);
    }

	TRACE("Application is added.\n");
	m_bSettingsChanged = true;

}




INT_PTR CALLBACK CFireWall::ChangeUserProc (HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch (message)
	{
	case WM_CLOSE:

        EndDialog(hwndDlg, FALSE);
		return TRUE;

	case WM_COMMAND:
		if (wParam == IDOK)
		{
			const int buf_size = 255;
			char strUser [buf_size + 1];
			char strPassword [buf_size + 1];

			strUser[buf_size] = 0;
			strUser [0] = 0;
			strPassword[buf_size] = 0;
			strPassword [0] = 0;

			GetDlgItemText (hwndDlg, IDC_USER, strUser, buf_size);
			GetDlgItemText (hwndDlg, IDC_PASSWORD, strPassword, buf_size);

			HANDLE hToken = 0;
			if (LogonUser (
					strUser,						// user name
					NULL,							// domain or server
					strPassword,					// password
					LOGON32_LOGON_INTERACTIVE,      // type of logon operation
					LOGON32_PROVIDER_DEFAULT,		// logon provider
					&hToken							// receive tokens handle
				) 
				&& ::ImpersonateLoggedOnUser (hToken)) 
			{
				EndDialog (hwndDlg, TRUE);
			}
			else
			{
				int shift [] = {5, 0, -5, 0};
				RECT r;
				GetWindowRect (hwndDlg, &r);
				for (int i = 0; i < 5 * 4; i++)
				{
					SetWindowPos (hwndDlg, 0, r.left + shift [i % 4], r.top, 0, 0, SWP_NOREPOSITION|SWP_NOSIZE);
					Sleep (10);
				}
			}

			if (hToken)
			{
				CloseHandle (hToken);
			}

		}
		else if (wParam == IDCANCEL)
		{
			EndDialog(hwndDlg, FALSE); 
			return TRUE; 
		}
    } 
    return FALSE; 
} 
