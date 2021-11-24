// SymbolLibraryDoc.h : interface of the CSymbolLibraryDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYMBOLLIBRARYDOC_H__D9BAD20D_B870_11D2_98EB_000000000000__INCLUDED_)
#define AFX_SYMBOLLIBRARYDOC_H__D9BAD20D_B870_11D2_98EB_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mdshape.h"

class	CSymLibEntry : public CObject
{
	DECLARE_SERIAL(CSymLibEntry)
public:
	CMDShape* m_pShape;
	int m_nDisplayRes;
	int m_nScale;
	CSymLibEntry() : m_pShape(NULL), m_nDisplayRes(0), m_nScale(0) {}
	CSymLibEntry(CMDShape* pShape,int nDisplayRes,int nScale);
	virtual ~CSymLibEntry();
	virtual void Serialize(CArchive& ar);
};

typedef CTypedPtrArray<CObArray,CSymLibEntry*> CSymLibArray;

#include "PCDraftDoc.h"


class CSymbolLibraryDoc : public CVersionDoc
{
protected: // create from serialization only
	CSymbolLibraryDoc();
	DECLARE_DYNCREATE(CSymbolLibraryDoc)

// Attributes
public:
	CSymLibArray m_Shapes;
	CSymLibEntry* m_pActiveEntry;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolLibraryDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

// Implementation
public:
	int GetObjectCount(bool bOverall);
	virtual ~CSymbolLibraryDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSymbolLibraryDoc)
	afx_msg void OnUpdateFileRevert(CCmdUI* pCmdUI);
	afx_msg void OnFileRevert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMBOLLIBRARYDOC_H__D9BAD20D_B870_11D2_98EB_000000000000__INCLUDED_)
