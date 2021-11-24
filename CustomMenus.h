#if !defined(AFX_CUSTOMMENUS_H__B60B1099_65B3_11D2_B499_F110F5064965__INCLUDED_)
#define AFX_CUSTOMMENUS_H__B60B1099_65B3_11D2_B499_F110F5064965__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomMenus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolMenu 

class CToolMenu : public CMenu
{
public:
	int TrackPopupMenu(CPoint& pt,CWnd* pWnd);
// Operations

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	CToolMenu(CBitmap* pBitmap);

	CBitmap* m_bitmap;
};

/////////////////////////////////////////////////////////////////////////////
// CColorMenu 

class CColorMenu : public CMenu
{
public:
	CPalette* m_pPalette;
	CString m_sTitle;
	int m_nBase;
// Operations

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	CColorMenu();
	void Create(CPalette* pPalette,UINT nTitle,int nBase=ID_COLORS_BASE);
	int Track(CWnd* pWnd);
	void CmdTrack(CWnd *pWnd);
};

/////////////////////////////////////////////////////////////////////////////
// CPatternMenu 
// Pattern/Gradient - Infinisys Ltd
//

class CPackedDIB;

class CPatternOnlyMenu: public CMenu
{
public:
   int m_iColumns;
   CString m_sTitle;
   int m_nBase;
   int   m_nCount;

   // Operations

   // Implementation
   CPatternOnlyMenu();
   void	Create(CPackedDIB * pitem,int nCount,UINT nTitle,int nBase);
   virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   int Track(CWnd* pWnd);
   void CmdTrack(CWnd *pWnd);
};


/////////////////////////////////////////////////////////////////////////////
// CPatternMenu 

class CPackedItem;

class CPatternMenu : public CMenu
{
public:
   int m_iColumns;
	CString m_sTitle;
	int m_nBase;
   int   m_nCount;

// Operations

// Implementation
	CPatternMenu();
   void	Create(CPackedItem * pitem, int nCount, UINT nTitle,int nBase); // Pattern/Gradient - Infinisys Ltd
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	int Track(CWnd* pWnd);
	void CmdTrack(CWnd *pWnd);
};


/////////////////////////////////////////////////////////////////////////////
// CColorPatternGradientMenu - Interface Update - Infinisys Ltd.

class CColorPatternGradientMenu: public CMenu
{
public:
   int m_iColumnCount;
   CPalette* m_pPalette;
   CString m_sTitleColor;
   CString m_sTitlePatternGradient;
   int m_nBaseColor;
   int m_nBasePatternGradient;
   int m_nCountPatternGradient;
   // Operations

   // Implementation
   CColorPatternGradientMenu();
   void Create(CPalette* pPalette,UINT nTitleColor, int nBaseColor,CPackedItem * pitem,int nCount,UINT nTitlePatternGradient,int nBasePatternGradient);
   virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   int Track(CWnd* pWnd);
   void CmdTrack(CWnd *pWnd);
};

/////////////////////////////////////////////////////////////////////////////
// CEndmarksMenu 

class CEndmarksMenu : public CMenu
{
public:
	CBitmap m_bitmap;
// Operations

// Implementation
	CEndmarksMenu();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	int Track(CWnd* pWnd);
};

/////////////////////////////////////////////////////////////////////////////
// CLineStyleMenu 

class CLineStyleMenu : public CMenu
{
public:
	int m_nCmdBase;
// Operations

// Implementation
	CLineStyleMenu();
	void Create(unsigned char* styles,int cmdBase,bool withSolid=true);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	int Track(CWnd* pWnd);
	void CmdTrack(CWnd* pWnd);
};

/////////////////////////////////////////////////////////////////////////////
// CBitmapMenu 

class CBitmapMenu : public CMenu
{
public:
	int m_cy;
	int m_cx;
	CBitmap m_bitmap;
	int m_nCmdBase;
// Operations

// Implementation
	CBitmapMenu(int nResID,int nItemNum,int nCmdBase=1);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	int Track(CWnd* pWnd);
	void CmdTrack(CWnd* pWnd);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMMENUS_H__B60B1099_65B3_11D2_B499_F110F5064965__INCLUDED_)
