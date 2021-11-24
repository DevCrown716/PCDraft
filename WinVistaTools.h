#if !defined(_WIN_VISTA_TOOLS_H_)
#define _WIN_VISTA_TOOLS_H_

#pragma once


class WinVistaTools
{
public:

    static BOOL IsVistaOrHigher(); // Check is Vista

    static HRESULT IsElevatedProcess(); // Check If Is Elevated mode

    static bool StartElevated(HWND hwnd, LPCTSTR pszPath, LPCTSTR pszParameters = NULL, LPCTSTR pszDirectory = NULL);

};

#endif // _WIN_VISTA_TOOLS_H_