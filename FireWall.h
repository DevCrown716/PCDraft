// FireWall.h: interface for the CFireWall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIREWALL_H__B8B6E9DA_65B4_46AC_A537_E9A05FE2B67C__INCLUDED_)
#define AFX_FIREWALL_H__B8B6E9DA_65B4_46AC_A537_E9A05FE2B67C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct INetFwProfile;

class CFireWall  
{
public:
	void Initialize ();
	bool Check ();
	bool IsSettingsChanged () const { return m_bSettingsChanged; }

	CFireWall ();
	virtual ~CFireWall();

    void EnableApplication();

private:

	bool IsEnabled () const;
	bool IsExceptionsEnabled () const;
	bool ChangeUser ();
    void EnableExceptions();
	bool IsApplicationEnabled ();
	INetFwProfile* m_pFwProfile;

	bool	m_bSettingsChanged;
	time_t m_lastCheck;
	static const LPWSTR m_strFriendlyName;
	static INT_PTR CALLBACK ChangeUserProc (HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

#endif // !defined(AFX_FIREWALL_H__B8B6E9DA_65B4_46AC_A537_E9A05FE2B67C__INCLUDED_)
