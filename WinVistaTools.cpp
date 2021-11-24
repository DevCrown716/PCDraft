#include "stdafx.h"
#include "WinVistaTools.h"


BOOL WinVistaTools::IsVistaOrHigher()
{
	OSVERSIONINFO osver;

	osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	
	if (	::GetVersionEx( &osver ) && 
			osver.dwPlatformId == VER_PLATFORM_WIN32_NT && 
			(osver.dwMajorVersion >= 6 ) )
		return TRUE;

	return FALSE;
}

HRESULT WinVistaTools::IsElevatedProcess( )
{
    if (!IsVistaOrHigher())
	{
		return E_FAIL;
	}

	HRESULT hResult = E_FAIL; // assume an error occured
	HANDLE hToken	= NULL;

	if ( !::OpenProcessToken( 
		::GetCurrentProcess(), 
		TOKEN_QUERY, 
		&hToken ) )
	{
		return hResult;
	}

	DWORD dwReturnLength = 0;
	TOKEN_ELEVATION_TYPE elevationType; 

	if ( ::GetTokenInformation(
		hToken,
		TokenElevationType,
		&elevationType,
		sizeof( elevationType ),
		&dwReturnLength ) )
	{

		hResult = ( elevationType == TokenElevationTypeFull )? S_OK : S_FALSE;
	}

	::CloseHandle( hToken );

	return hResult;
}


bool WinVistaTools::StartElevated(
    HWND    hwnd,
    LPCTSTR pszPath,
    LPCTSTR pszParameters, //   = NULL,
    LPCTSTR pszDirectory)  //   = NULL );
{

    SHELLEXECUTEINFO shex = { 0 };

    shex.cbSize = sizeof(SHELLEXECUTEINFO);
    shex.fMask = SEE_MASK_NOCLOSEPROCESS; // 0;
    shex.hwnd = hwnd;
#ifdef UNICODE
    shex.lpVerb = _T("runas");
#else
    shex.lpVerb = "runas";
#endif

    shex.lpFile = pszPath;
    shex.lpParameters = pszParameters;
    shex.lpDirectory = pszDirectory;
    shex.nShow = SW_NORMAL;

    bool bRet = ::ShellExecuteEx(&shex) != FALSE;

    //if (shex.hProcess)
    //{
    //  BOOL result = FALSE;
    //  while (1)
    //  {
    //      if (::IsProcessInJob(shex.hProcess, NULL, &result))
    //      {
    //          if (!result)
    //              break;
    //          ::Sleep(10);
    //      }else
    //          break;
    //  }
    //}
    ::Sleep(1000);
    return bRet;
}