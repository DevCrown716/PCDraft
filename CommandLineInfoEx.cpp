// CommandLineInfoEx.cpp: implementation of the CCommandLineInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandLineInfoEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandLineInfoEx::CCommandLineInfoEx()
    :m_bEnableFilewall(false)
{

}

CCommandLineInfoEx::~CCommandLineInfoEx()
{

}

void CCommandLineInfoEx::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
    if (bFlag){
        if (::CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT),
            NORM_IGNORECASE, pszParam, -1, "enablefirewall", -1) - CSTR_EQUAL == 0)
        {
            m_bEnableFilewall = true;
        }else
        ParseParamFlag(pszParam);
    }
	else
	{
		ParseParamNotFlag(pszParam);
		m_arrFileNames.Add(pszParam);
	}

	ParseLast(bLast);
}
