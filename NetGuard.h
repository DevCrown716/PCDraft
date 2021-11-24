// NetGuard.h: interface for the CNetGuard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETGUARD_H__D8CD741F_2646_11D4_9981_000021D1C7AF__INCLUDED_)
#define AFX_NETGUARD_H__D8CD741F_2646_11D4_9981_000021D1C7AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>
#include "firewall.h"

class CNetGuard  
{
public:
	CNetGuard();
	virtual ~CNetGuard();
	virtual void OnReceive(SOCKADDR* psa,LPCTSTR pBuffer);
	void SendBroadcast(LPCTSTR pszData,int nCopies);
	void Send(SOCKADDR* psa,LPCTSTR pszData);
	bool StartListener ();
protected:
	void CheckFireWall ();
	void AskCopies ();

	CFireWall	m_cFireWall;
	CStringArray m_strCopies;
	int m_nCopies;
	bool m_bStarted;
	SOCKET m_ServerSockIP;
	SOCKET m_ClientSockIP;
	SOCKET m_BroadcastSockIP;
	HANDLE m_hListenThreadIP;
	HANDLE m_hReadIP;

	SOCKET m_ServerSockIPX;
	SOCKET m_ClientSockIPX;
	SOCKET m_BroadcastSockIPX;
	HANDLE m_hListenThreadIPX;
	CString m_sSerialNumber;
static DWORD dwPlatformId;
static long WINAPI ListenThreadFnIP(CNetGuard *pGuard);
static long WINAPI ListenThreadFnIPX(CNetGuard *pGuard);
};

#endif // !defined(AFX_NETGUARD_H__D8CD741F_2646_11D4_9981_000021D1C7AF__INCLUDED_)
