#if !defined(AFX_FINDREPLACEDLG_H__C4A92263_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_FINDREPLACEDLG_H__C4A92263_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FindReplaceDlg.h : header file
//

class CPCDraftView;
class CMDShape;
class CLayer;

#include "IShapeSelectedNotify.h"

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg dialog

class CFindReplaceDlg : public CDialog, public IShapeSelectedNotify
{
// Construction
public:
	void DoReplace();
	void DoFind();
	CFindReplaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindReplaceDlg)
	enum { IDD = IDD_FIND_REPLACE };
	CButton	m_Undo;
	//}}AFX_DATA

	enum ShapeSelectedType
	{
		kFindShape,
		kReplaceShape
	};

	enum ReplaceType
	{
		kReplaceObjectAndData =	0,
		kReplaceObjectOnly = 1,
		kReplaceDataOnly = 2
	};

	virtual void ShapeSelected(CMDShape* pShape);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindReplaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMDShape* m_pReplaceShape;
	CPCDraftView* m_pActiveView;
	ShapeSelectedType m_nShapeSelectedType;
	CObArray m_FoundShapes;
	CArray<int,int> m_FoundShapesLayerIndex;
	int m_nCurrShapeIndex;
	int m_ReplaceType;
	bool m_bShowMore;

	void SelectLayerShapes(int nLayerIndex,CLayer* pLayer,bool* bUseField,CString* sFieldValue);
	void DoShapeSelect(ShapeSelectedType nType);
	void SetupFoundShapesStatus();
	void SelectValidShapes();
	void SelectReplaceShape();
	CMDShape* FindLayerShape(CLayer* pLayer,CString* sFieldValue);
	void OffsetWindow(UINT nID,int nDelta);

	afx_msg void OnSelectValidShapes();
	afx_msg void OnSelectReplaceShape();
	// Generated message map functions
	//{{AFX_MSG(CFindReplaceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnReplaceUseMouse();
	afx_msg void OnFindUseMouse();
	afx_msg void OnFind();
	afx_msg void OnReplace();
	afx_msg void OnSelchangeReplaceType();
	afx_msg void OnReplaceAll();
	afx_msg void OnUndo();
	afx_msg void OnClose();
	afx_msg void OnResizeOpt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDREPLACEDLG_H__C4A92263_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
