// SavedView.h: interface for the CSavedView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVEDVIEW_H__1DA82654_EC51_11D2_B5B8_9ABF4588277F__INCLUDED_)
#define AFX_SAVEDVIEW_H__1DA82654_EC51_11D2_B5B8_9ABF4588277F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSavedView : public CObject  
{
	DECLARE_SERIAL(CSavedView)
public:
	CString GetMenuTitle();
	CSavedView();
	virtual ~CSavedView();

	virtual void Serialize(CArchive& ar);

	CString m_sTitle;
	CPoint m_ptOrg;
	int m_nZoom;
	int m_nShortcut;
};

typedef CTypedPtrArray<CObArray,CSavedView*> ViewArray;

#endif // !defined(AFX_SAVEDVIEW_H__1DA82654_EC51_11D2_B5B8_9ABF4588277F__INCLUDED_)
