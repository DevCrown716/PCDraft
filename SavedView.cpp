// SavedView.cpp: implementation of the CSavedView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "SavedView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CSavedView,CObject,1)

//////////////////////////////////////////////////////////////////////

CSavedView::CSavedView()
{
}

CSavedView::~CSavedView()
{
}

void CSavedView::Serialize(CArchive& ar)
{
	if(ar.IsLoading())
	{
		ar >> m_sTitle;
		ar >> m_ptOrg;
		ar >> m_nZoom;
		ar >> m_nShortcut;
	}
	else
	{
		ar << m_sTitle;
		ar << m_ptOrg;
		ar << m_nZoom;
		ar << m_nShortcut;
	}
}

CString CSavedView::GetMenuTitle()
{
	if(m_nShortcut)
	{
		CString temp;
		temp.Format("\tCtrl+%d",m_nShortcut+4);
		return m_sTitle+temp;
	}
	else return m_sTitle;
}
