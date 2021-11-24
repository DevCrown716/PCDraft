// ViewTemplate.h: interface for the CViewTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWTEMPLATE_H__6F665011_C99E_11D2_9903_000000000000__INCLUDED_)
#define AFX_VIEWTEMPLATE_H__6F665011_C99E_11D2_9903_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CViewTemplate  
{
public:
	CViewTemplate();
	virtual ~CViewTemplate();
	void Register(UINT nResourceID);
	BOOL CreateView(CDocument* pDocument,CRuntimeClass* pView,CRuntimeClass* pFrame,BOOL bVisible=TRUE,CMDIChildWnd** pNewChildFrame=NULL);
protected:
	UINT	m_nResourceID;
	HMENU	m_hSharedMenu;
	HACCEL	m_hSharedAccel;
};

#endif // !defined(AFX_VIEWTEMPLATE_H__6F665011_C99E_11D2_9903_000000000000__INCLUDED_)
