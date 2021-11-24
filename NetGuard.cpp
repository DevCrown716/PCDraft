// NetGuard.cpp: implementation of the CNetGuard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Winsock2.h>
#include "PCDraft.h"
#include "NetGuard.h"
#include "SerialNumber.h"

#include <afxmt.h>
#include <wsipx.h>
#include <wsnwlink.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	kDefPort	8000

#define MAX_MSGLEN	200

#define	kTimeOut	60000 // 1 minute

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD CNetGuard::dwPlatformId=0;

CNetGuard::CNetGuard() : m_hListenThreadIP (INVALID_HANDLE_VALUE), m_hListenThreadIPX (INVALID_HANDLE_VALUE) , m_bStarted (false)
{
	m_nCopies=0;

	if(0==dwPlatformId)
	{
		OSVERSIONINFO osVer;
		BOOL fResult;
		osVer.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
		fResult=GetVersionEx(&osVer);
		dwPlatformId=osVer.dwPlatformId;
		if(FALSE==fResult)
		{
			TRACE("GetVersionEx error=%d\n",GetLastError ( ));
		}
	}

	m_cFireWall.Initialize ();
}

bool CNetGuard::StartListener ()
{
	if (m_bStarted) return true;
	if (!m_cFireWall.Check ()) return false;

	unsigned long iThread;
	WSADATA wsaData;
	INT iRetVal;
	iRetVal=WSAStartup (MAKEWORD(2,0), &wsaData); //GK: was version 1.1
	m_bStarted=(iRetVal==0);
	if(m_bStarted)
	{
		m_ServerSockIP=socket(AF_INET,SOCK_DGRAM,0);
		if(m_ServerSockIP!=INVALID_SOCKET)
		{
			SOCKADDR_IN saUdpServ;
			saUdpServ.sin_family=AF_INET;
			saUdpServ.sin_addr.s_addr=htonl(INADDR_ANY);
			saUdpServ.sin_port=htons(kDefPort);
			int err=bind(m_ServerSockIP,(SOCKADDR*)&saUdpServ,sizeof(SOCKADDR_IN));
			if(SOCKET_ERROR==err)
			{
				TRACE("bind error=%d\n",WSAGetLastError());
			}
		}
		m_BroadcastSockIP=socket(AF_INET,SOCK_DGRAM,0);
		if(m_BroadcastSockIP!=INVALID_SOCKET)
		{
			// Variable to set the broadcast option with setsockopt ().
			BOOL fBroadcast = TRUE;
			int err=setsockopt(m_BroadcastSockIP,SOL_SOCKET,SO_BROADCAST,(CHAR*)&fBroadcast,sizeof(BOOL));
			if(SOCKET_ERROR==err)
			{
				TRACE("setsockopt error=%d\n",WSAGetLastError());
			}
			//
			// bind to a local socket and an interface.
			//
			SOCKADDR_IN saUdpCli;
			saUdpCli.sin_family=AF_INET;
			saUdpCli.sin_addr.s_addr=htonl(INADDR_ANY);
			saUdpCli.sin_port=htons(0);
		    err=bind(m_BroadcastSockIP,(SOCKADDR*)&saUdpCli,sizeof(SOCKADDR_IN));
			if(SOCKET_ERROR==err)
			{
				TRACE("bind error=%d\n",WSAGetLastError());
			}
		}
		m_ClientSockIP=socket(AF_INET,SOCK_DGRAM,0);
		m_hListenThreadIP=::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ListenThreadFnIP,(void*)this,0,&iThread);
	}
	else
	{
		m_ServerSockIP=INVALID_SOCKET;
		m_ClientSockIP=INVALID_SOCKET;
		m_BroadcastSockIP=INVALID_SOCKET;
		m_hListenThreadIP=INVALID_HANDLE_VALUE;
	}
	if(m_bStarted)
	{

		m_ServerSockIPX=socket(AF_IPX,SOCK_DGRAM,NSPROTO_IPX);
		if(m_ServerSockIPX!=INVALID_SOCKET)
		{
			if(VER_PLATFORM_WIN32_WINDOWS==dwPlatformId)
			{
				// Variable to set the broadcast option with setsockopt ().
				BOOL fBroadcast=TRUE;
				int err=setsockopt(m_ServerSockIPX,SOL_SOCKET,SO_BROADCAST,(CHAR*)&fBroadcast,sizeof(BOOL));
				if(SOCKET_ERROR==err)
				{
					TRACE("setsockopt error=%d\n",WSAGetLastError());
				}
			}

			SOCKADDR_IPX saIpxServ;
			saIpxServ.sa_family=AF_IPX;
			saIpxServ.sa_socket=kDefPort;
			memset(saIpxServ.sa_netnum,0,sizeof (saIpxServ.sa_netnum));
			memset(saIpxServ.sa_nodenum,0,sizeof (saIpxServ.sa_nodenum));
			int err=bind(m_ServerSockIPX,(SOCKADDR*)&saIpxServ,sizeof(SOCKADDR_IPX));
			if(SOCKET_ERROR==err)
			{
				TRACE("bind error=%d\n",WSAGetLastError());
			}
		}
		m_BroadcastSockIPX=socket(AF_IPX,SOCK_DGRAM,NSPROTO_IPX);
		if(m_BroadcastSockIPX!=INVALID_SOCKET)
		{
			// Variable to set the broadcast option with setsockopt ().
			BOOL fBroadcast = TRUE;
			int err=setsockopt(m_BroadcastSockIPX,SOL_SOCKET,SO_BROADCAST,(CHAR*)&fBroadcast,sizeof(BOOL));
			if(SOCKET_ERROR==err)
			{
				TRACE("setsockopt error=%d\n",WSAGetLastError());
			}
			//
			// bind to a local socket and an interface.
			//
			SOCKADDR_IPX saIpxCli;
			saIpxCli.sa_family=AF_IPX;
			saIpxCli.sa_socket=(USHORT)0;
			memset(saIpxCli.sa_netnum,0,sizeof (saIpxCli.sa_netnum));
			memset(saIpxCli.sa_nodenum,0,sizeof (saIpxCli.sa_nodenum));
		    err=bind(m_BroadcastSockIPX,(SOCKADDR*)&saIpxCli,sizeof(SOCKADDR_IPX));
			if(SOCKET_ERROR==err)
			{
				TRACE("bind error=%d\n",WSAGetLastError());
			}
		}
		m_ClientSockIPX=socket(AF_IPX,SOCK_DGRAM,NSPROTO_IPX);
		m_hListenThreadIPX=::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ListenThreadFnIPX,(void*)this,0,&iThread);
	}
	else
	{
		m_ServerSockIPX=INVALID_SOCKET;
		m_ClientSockIPX=INVALID_SOCKET;
		m_BroadcastSockIPX=INVALID_SOCKET;
		m_hListenThreadIPX=INVALID_HANDLE_VALUE;
	}

	AskCopies ();
	return true;
}

CNetGuard::~CNetGuard()
{
	m_bStarted = FALSE;
	if (m_hListenThreadIP!=INVALID_HANDLE_VALUE)
	{
		::SetEvent (m_hReadIP);
		WaitForSingleObject (m_hListenThreadIP, kTimeOut); // wait until thread will closed
	}
	if(m_ServerSockIP!=INVALID_SOCKET)
		closesocket(m_ServerSockIP);
	if(m_ClientSockIP!=INVALID_SOCKET)
		closesocket(m_ClientSockIP);
	if(m_BroadcastSockIP!=INVALID_SOCKET)
		closesocket(m_BroadcastSockIP);
	// above should cause listen thread to exit
	if(m_hListenThreadIP!=INVALID_HANDLE_VALUE)
	{
		DWORD dwExitCode=0;
		while(::GetExitCodeThread(m_hListenThreadIP,&dwExitCode) && dwExitCode==STILL_ACTIVE);
		::CloseHandle(m_hListenThreadIP);
	}

	if(m_ServerSockIPX!=INVALID_SOCKET)
		closesocket(m_ServerSockIPX);
	if(m_ClientSockIPX!=INVALID_SOCKET)
		closesocket(m_ClientSockIPX);
	if(m_BroadcastSockIPX!=INVALID_SOCKET)
		closesocket(m_BroadcastSockIPX);
	// above should cause listen thread to exit
	if(m_hListenThreadIPX!=INVALID_HANDLE_VALUE)
	{
		DWORD dwExitCode=0;
		while(::GetExitCodeThread(m_hListenThreadIPX,&dwExitCode) && dwExitCode==STILL_ACTIVE);
		::CloseHandle(m_hListenThreadIPX);
	}

	if(m_bStarted)
		WSACleanup();
}

short ExtractUserCount(const CString FromThisSerialNumber); // Declared in serial number.cpp

void CNetGuard::AskCopies ()
{
	if(m_sSerialNumber.IsEmpty())
	{
		m_sSerialNumber=AfxGetApp()->GetProfileString ("Registration","SerialNumber");
		FormatTheSerialNumber (m_sSerialNumber);
	}
	CString theNetAsk;
	theNetAsk.Format ("?%s",m_sSerialNumber);
	SendBroadcast (theNetAsk, ExtractUserCount(m_sSerialNumber));
}

void CNetGuard::CheckFireWall ()
{
	if (m_bStarted && m_cFireWall.Check ())// && m_cFireWall.IsSettingsChanged ())
	{
		AskCopies ();
	}
}

long WINAPI CNetGuard::ListenThreadFnIP(CNetGuard *pGuard)
{
	TRACE("ListenThreadFnIP up\n");
	::CoInitialize (NULL);
	// IP address structures needed to bind to a local port and get the sender's
	// information.
	SOCKADDR_IN saUdpCli;
	INT err,nSize;
	CHAR achBuffer[MAX_MSGLEN];

	pGuard->m_hReadIP = ::CreateEvent (NULL, FALSE, FALSE, NULL);
	WSAEventSelect (pGuard->m_ServerSockIP, pGuard->m_hReadIP, FD_READ);
	int nTimeOut = kTimeOut;
	while(pGuard->m_bStarted)
	{
		//
		// receive a datagram on the bound port number.
		//
		if (WaitForSingleObject (pGuard->m_hReadIP, nTimeOut) == WAIT_TIMEOUT)
		{
			if (CFlag::Free ())
			{
				pGuard->CheckFireWall ();
				nTimeOut = kTimeOut;
			}
			else
			{
				// To avoid crash on exit FireWall is not checked. but time sceduled to 0,1 second
				nTimeOut = 100;
			}
			continue;
		}
		if (!pGuard->m_bStarted)
		{
			break;
		}

		nSize=sizeof(SOCKADDR_IN);

		err=recvfrom(pGuard->m_ServerSockIP,achBuffer,MAX_MSGLEN,0,(SOCKADDR*)&saUdpCli,&nSize);
		if(SOCKET_ERROR==err)
		{
			TRACE("recvfrom error=%d\n",WSAGetLastError());
			break;
		}
		achBuffer[err]='\0';

		bool bIgnore=false;
		char hostname[MAX_COMPUTERNAME_LENGTH];
		gethostname(hostname,MAX_COMPUTERNAME_LENGTH);
		hostent* he=gethostbyname(hostname);
		if(he && he->h_length==4)
		{
			char** p=he->h_addr_list;
			while(*p)
			{
				if(saUdpCli.sin_addr.S_un.S_addr==*(u_long*)*p)
				{
					bIgnore=true;
					break;
				}
				p++;
			}
		}
		if(!bIgnore)
			pGuard->OnReceive((SOCKADDR*)&saUdpCli,achBuffer);
	}

	TRACE("ListenThreadFnIP shut down\n");
	::CoUninitialize ();
	::WSAEventSelect (pGuard->m_ServerSockIP, pGuard->m_hReadIP, 0);
	::CloseHandle (pGuard->m_hReadIP);
	::ExitThread (0);
	return 0;
}

long WINAPI CNetGuard::ListenThreadFnIPX(CNetGuard *pGuard)
{
	// IP address structures needed to bind to a local port and get the sender's
	// information.
	SOCKADDR_IPX saIpxCli;
	INT err,nSize;
	CHAR achBuffer[MAX_MSGLEN];
	while(1)
	{
		//
		// receive a datagram on the bound port number.
		//
		nSize=sizeof(SOCKADDR_IPX);
		err=recvfrom(pGuard->m_ServerSockIPX,achBuffer,MAX_MSGLEN,0,(SOCKADDR*)&saIpxCli,&nSize);
		if(SOCKET_ERROR==err)
		{
			TRACE("recvfrom IPX error=%d\n",WSAGetLastError());
			break;
		}
		achBuffer[err]='\0';

		bool bIgnore=false;
		int cAdapters, cbOpt=sizeof(cAdapters), cbAddr=sizeof(SOCKADDR_IPX);
		if(VER_PLATFORM_WIN32_WINDOWS==dwPlatformId) cAdapters=1; // if we got here assume at least 1 adapter for Win9x
		else
		{
			// Get the number of adapters => cAdapters. - this won't work under Win9x - confirmed by Microsoft
			err=getsockopt(pGuard->m_ServerSockIPX, NSPROTO_IPX,IPX_MAX_ADAPTER_NUM,(char*)&cAdapters,&cbOpt);
			if(SOCKET_ERROR==err)
			{
				TRACE("getsockopt error=%d\n",WSAGetLastError());
				break;
			}
		}
		// At this point cAdapters is the number of installed adapters.
		while(cAdapters>0)
		{
			IPX_ADDRESS_DATA IpxData;
			memset( &IpxData, 0, sizeof(IpxData));

			// Specify which adapter to check.
			IpxData.adapternum=cAdapters-1;
			cbOpt=sizeof(IpxData);

			// Get information for the current adapter.
			err=getsockopt(pGuard->m_ServerSockIPX,NSPROTO_IPX,IPX_ADDRESS,(char*)&IpxData,&cbOpt);
			if(SOCKET_ERROR==err)
			{
				TRACE("getsockopt error=%d\n",WSAGetLastError());
				break;
			}

			if(0==memcmp(saIpxCli.sa_netnum,IpxData.netnum,4) && 0==memcmp(saIpxCli.sa_nodenum,IpxData.nodenum,6))
			{
				bIgnore=true;
				break;
			}

			// IpxData contains the address for the current adapter.
			cAdapters--;
		}

		if(!bIgnore)
			pGuard->OnReceive((SOCKADDR*)&saIpxCli,achBuffer);
	}
	return 0;
}

void CNetGuard::OnReceive(SOCKADDR* psa,LPCTSTR pBuffer)
{

	TRACE("IP=%s Message=%s\n", inet_ntoa (((sockaddr_in*)psa)->sin_addr), pBuffer);
	if(*pBuffer=='?')
	{
		if(m_sSerialNumber.IsEmpty())
		{
			m_sSerialNumber=AfxGetApp()->GetProfileString("Registration","SerialNumber");
			FormatTheSerialNumber(m_sSerialNumber);
		}
		if(m_sSerialNumber==(pBuffer+1))
		{
			CString theComputerName;
			DWORD dwLen=MAX_COMPUTERNAME_LENGTH+1;
			GetComputerName(theComputerName.GetBufferSetLength(MAX_COMPUTERNAME_LENGTH+1),&dwLen);
			theComputerName.ReleaseBuffer();
			CString theUserName=AfxGetApp()->GetProfileString("Registration","Name");
			CString theAnswer;
			theAnswer.Format("#%s\t%s\t%s",m_sSerialNumber,theComputerName,theUserName);
			Send(psa,theAnswer);
		}
	}
	else if(*pBuffer=='#')
	{
		for(int i=m_strCopies.GetSize();--i>=0;)
			if(m_strCopies.GetAt(i)==pBuffer)
				return;
		m_strCopies.Add(pBuffer);
		if(--m_nCopies<=0)
		{
			CWnd* pMainWnd=AfxGetMainWnd();
			if(pMainWnd)
			{
				static bool gGuardMsgSent=false;
				CCriticalSection cs;
				if(!gGuardMsgSent)
				{
static CHAR achMsg[MAX_MSGLEN];
					strcpy(achMsg,pBuffer+1);
					gGuardMsgSent=true;
					cs.Unlock();
					pMainWnd->PostMessage(gNetGuardMsg,0,(LPARAM)(achMsg));
				}
			}
		}
	}
}

void CNetGuard::Send(SOCKADDR* psa,LPCTSTR pszData)
{
	int err=0;
	switch(psa->sa_family)
	{
	case AF_INET:
		{
			((SOCKADDR_IN*)psa)->sin_port=htons(kDefPort);
			err=sendto(m_ClientSockIP,pszData,lstrlen(pszData),0,psa,sizeof(SOCKADDR_IN));
		}
		break;
	case AF_IPX:
		{
			((SOCKADDR_IPX*)psa)->sa_socket=kDefPort;
			err=sendto(m_ClientSockIPX,pszData,lstrlen(pszData),0,psa,sizeof(SOCKADDR_IN));
		}
		break;
	}
	if(SOCKET_ERROR==err)
	{
		TRACE("sendto error=%d\n",WSAGetLastError());
	}
}

void CNetGuard::SendBroadcast(LPCTSTR pszData,int nCopies)
{
	m_nCopies=nCopies;
	m_strCopies.RemoveAll();

	{
		CString theComputerName;
		DWORD dwLen=MAX_COMPUTERNAME_LENGTH+1;
		GetComputerName(theComputerName.GetBufferSetLength(MAX_COMPUTERNAME_LENGTH+1),&dwLen);
		theComputerName.ReleaseBuffer();
		CString theUserName=AfxGetApp()->GetProfileString("Registration","Name");
		CString theAnswer;
		theAnswer.Format("#%s\t%s\t%s",pszData+1,theComputerName,theUserName);
		m_strCopies.Add(theAnswer);
	}

	CHAR achMessage[MAX_MSGLEN];
	lstrcpy(achMessage,pszData);

	// Fill an IP address structure, to send an IP broadcast. The 
	// packet will be broadcasted to the specified port.
	SOCKADDR_IN saUdpServ;
	saUdpServ.sin_family=AF_INET;
	saUdpServ.sin_addr.s_addr=htonl(INADDR_BROADCAST);
	saUdpServ.sin_port=htons(kDefPort);
	int err=sendto(m_BroadcastSockIP,achMessage,lstrlen(achMessage),0,(SOCKADDR*)&saUdpServ,sizeof(SOCKADDR_IN));
    if(SOCKET_ERROR==err)
    {
		TRACE("sendto error=%d\n",WSAGetLastError());
	}

    // Fill an IPX address structure, to send an IPX broadcast. The
	// packet will be broadcasted to the specified socket.
	SOCKADDR_IPX saIpxServ;
	saIpxServ.sa_family=AF_IPX;
	saIpxServ.sa_socket=kDefPort;
	memset(saIpxServ.sa_netnum,0,sizeof (saIpxServ.sa_netnum));
	memset(saIpxServ.sa_nodenum,0xFF,sizeof (saIpxServ.sa_nodenum));
	err=sendto(m_BroadcastSockIPX,achMessage,lstrlen(achMessage),0,(SOCKADDR*)&saIpxServ,sizeof(SOCKADDR_IPX));
    if(SOCKET_ERROR==err)
    {
		TRACE("sendto IPX error=%d\n",WSAGetLastError());
	}
}
