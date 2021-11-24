#pragma once


#include <vector> 
class CChildFrameList: public std::vector< CWnd* >
{
public:
   CChildFrameList();
   virtual ~CChildFrameList();
   int Refresh();
   CWnd* FindFrame(CWnd* child);
};

class CTabBar: public CControlBar
{
public:
   DECLARE_DYNAMIC(CTabBar)

   int m_iPreviousCount;
   int m_iMargin;
   bool  m_bExpanded;
   CFont m_font;
   Gdiplus::Font m_font2;
   Gdiplus::StringFormat m_format;
   Gdiplus::StringFormat m_formatTab;
   int m_iGroupHover;
   int m_iItemHover;
   int m_iGroupLButtonDown;
   int m_iItemLButtonDown;
   int m_iTabSel;
   CChildFrameList  m_windowa;
   CChildFrameList  m_taba;
   CStringArray      m_stra;
   CRect m_rTabBorder;
   CRect m_rIconBorder;
   CRect m_rIconMargin;
   int m_iTabHeight;
   int m_xStart;
   float               m_fMaxTextHeight;
   int m_iTabScroll;

   bool m_bNeedLayout;

   CArrowButton     m_buttonTabLeft;
   CArrowButton     m_buttonTabRight;


   CTabBar();
   virtual BOOL Create(CWnd* pParentWnd,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
      UINT nID = AFX_IDW_TOOLBAR);
   virtual BOOL CreateEx(CWnd* pParentWnd,DWORD dwCtrlStyle = TBSTYLE_FLAT,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
      CRect rcBorders = CRect(0,0,0,0),
      UINT nID = AFX_IDW_TOOLBAR);
   bool get_tab_rect(LPRECT lprect,int i);
   void SetSizes(SIZE sizeButton,SIZE sizeImage);
   // button size should be bigger than image
   void SetHeight(int cyHeight);
   // call after SetSizes, height overrides bitmap size
   BOOL LoadToolBar(LPCTSTR lpszResourceName);
   BOOL LoadToolBar(UINT nIDResource);
   BOOL LoadBitmap(LPCTSTR lpszResourceName);
   BOOL LoadBitmap(UINT nIDResource);
   BOOL SetBitmap(HBITMAP hbmImageWell);
   BOOL SetButtons(const UINT* lpIDArray,int nIDCount);
   // lpIDArray can be NULL to allocate empty buttons

   // Attributes
public:
   void set_cur_tab(int iTab);
   void layout();
   // standard control bar things
   int CommandToIndex(UINT nIDFind) const;
   UINT GetItemID(int nIndex) const;
   virtual void GetItemRect(int nIndex,LPRECT lpRect) const;
   UINT GetButtonStyle(int nIndex) const;
   void SetButtonStyle(int nIndex,UINT nStyle);

   // for changing button info
   void GetButtonInfo(int nIndex,UINT& nID,UINT& nStyle,int& iImage) const;
   void SetButtonInfo(int nIndex,UINT nID,UINT nStyle,int iImage);
   BOOL SetButtonText(int nIndex,LPCTSTR lpszText);
   CString GetButtonText(int nIndex) const;

   void GetButtonText(int nIndex,CString& rString) const;


   // Implementation
public:
   virtual ~CTabBar();
   virtual CSize CalcFixedLayout(BOOL bStretch,BOOL bHorz);
   virtual CSize CalcDynamicLayout(int nLength,DWORD nMode);
   virtual INT_PTR OnToolHitTest(CPoint point,TOOLINFO* pTI) const;
   virtual void OnUpdateCmdUI(CFrameWnd* pTarget,BOOL bDisableIfNoHndler);
   void SetOwner(CWnd* pOwnerWnd);
   BOOL AddReplaceBitmap(HBITMAP hbmImageWell);
   virtual void OnBarStyleChange(DWORD dwOldStyle,DWORD dwNewStyle);
   virtual HRESULT get_accName(VARIANT varChild,BSTR *pszName);

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

public:
   HRSRC m_hRsrcImageWell; // handle to loaded resource for image well
   HINSTANCE m_hInstImageWell; // instance handle to load image well from
   HBITMAP m_hbmImageWell; // contains color mapped button images
   BOOL m_bDelayedButtonLayout; // used to manage when button layout should be done

   CSize m_sizeImage;  // current image size
   CSize m_sizeButton; // current button size

   CMapStringToPtr* m_pStringMap;  // used as CMapStringToUInt

   // implementation helpers
   void _GetButton(int nIndex,TBBUTTON* pButton) const;
   void _SetButton(int nIndex,TBBUTTON* pButton);
   CSize CalcLayout(DWORD nMode,int nLength = -1);
   CSize CalcSize(TBBUTTON* pData,int nCount);
   int WrapToolBar(TBBUTTON* pData,int nCount,int nWidth);
   void SizeToolBar(TBBUTTON* pData,int nCount,int nLength,BOOL bVert = FALSE);
   void Layout(); // called for delayed button layout

   afx_msg LRESULT OnNcHitTest(CPoint);
   afx_msg void OnNcPaint();
   afx_msg void OnPaint();
   afx_msg void OnNcCalcSize(BOOL,NCCALCSIZE_PARAMS*);
   afx_msg void OnWindowPosChanging(LPWINDOWPOS);
   afx_msg void OnSysColorChange();
   afx_msg LRESULT OnSetButtonSize(WPARAM,LPARAM);
   afx_msg LRESULT OnSetBitmapSize(WPARAM,LPARAM);
   afx_msg LRESULT OnPreserveZeroBorderHelper(WPARAM,LPARAM);
   afx_msg LRESULT OnPreserveSizingPolicyHelper(WPARAM,LPARAM);
   afx_msg BOOL OnNcCreate(LPCREATESTRUCT);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   LRESULT OnSetSizeHelper(CSize& size,LPARAM lParam);
   void SetRibbonHeight(UINT ui);
   UINT m_ulRibbonHeight;

   /////////////////////////////////////////////////////////////////////////////
   DECLARE_MESSAGE_MAP()

   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnTabLeftArrow();
   afx_msg void OnTabRightArrow();



};
