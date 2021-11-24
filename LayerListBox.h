#if !defined(AFX_LAYERLISTBOX_H__DBCA0447_5C8B_11D2_9863_000000000000__INCLUDED_)
#define AFX_LAYERLISTBOX_H__DBCA0447_5C8B_11D2_9863_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LayerListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayerListBox window

class CLayer;

class CLayerListBox : public CListBox
{
// Construction
public:
	CLayerListBox();

// Attributes
public:
	CLayer* m_pActiveLayer;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLayerListBox();

protected:
	HICON m_hActiveLayerIcon;
	HICON m_hVisibleLayerIcon;
	int m_cyText;

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERLISTBOX_H__DBCA0447_5C8B_11D2_9863_000000000000__INCLUDED_)
