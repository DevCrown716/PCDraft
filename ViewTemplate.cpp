// ViewTemplate.cpp: implementation of the CViewTemplate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "ViewTemplate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewTemplate::CViewTemplate()
{
	m_hSharedMenu=NULL;
	m_hSharedAccel=NULL;
}

void CViewTemplate::Register(UINT nResourceID)
{
	m_nResourceID=nResourceID;
	m_hSharedMenu=::LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(nResourceID));
	m_hSharedAccel=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(nResourceID));
}

BOOL CViewTemplate::CreateView(CDocument* pDocument,CRuntimeClass* pView,CRuntimeClass* pFrame,BOOL bVisible,CMDIChildWnd** pNewChildFrame)
{
	CCreateContext theContext;
	theContext.m_pCurrentDoc=pDocument;
	theContext.m_pNewViewClass=pView;
	CMDIChildWnd* pNewFrame=(CMDIChildWnd*)pFrame->CreateObject();
	if(pNewFrame->LoadFrame(m_nResourceID,WS_OVERLAPPEDWINDOW,AfxGetMainWnd(),&theContext))
	{
		pNewFrame->SetHandles(m_hSharedMenu,m_hSharedAccel);
		pNewFrame->InitialUpdateFrame(pDocument,bVisible);
		if(!bVisible)
			pNewFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);
		if(pNewChildFrame)
			*pNewChildFrame=pNewFrame;
		return TRUE;
	}
	return FALSE;
}

CViewTemplate::~CViewTemplate()
{
	if(m_hSharedMenu)
		::DestroyMenu(m_hSharedMenu);
	// Accel auto destroy
}
