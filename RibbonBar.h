#pragma once

#include "RibbonProperties.h"
#include "ArrowButton.h"

// Declaration of CRibbonBar class which inherits from CToolBar. 
// This placeholder bar helps the MFC CMainFrame to calculate the view layout whenever the Ribbon is shown or hidden.
class CRibbonBar: public CControlBar
{
public:
   DECLARE_DYNAMIC(CRibbonBar)

   class CItem
   {
   public:


      int               m_iId;
      bool              m_bEnabled;
      bool              m_bChecked;
      Gdiplus::Image *  m_pimage1;
      Gdiplus::Image *  m_pimageLarge;
      Gdiplus::Image *  m_pimageSmall;
      CStringW          m_wstrName;
      CRect             m_rItem;
      CRect             m_rImage;
      CRect             m_rText;
      int               m_x;
      int               m_y;
      int               m_iLastMaxW;
      int               m_iRow;
      bool              m_bPreferTwoLines;
      bool              m_bPopup;
      bool              m_bAlwaysEnabledPopup;
      bool              m_bResizeSmaller;


      CItem();
      CItem(const CItem & i);
      ~CItem();


      CItem & operator =(const CItem & i);

      CStringW get_draw_name();
      bool get_prefer_two_lines();

      void ClearLayout();

   };

   class CGroup
   {
   public:


      CGroup(){ m_iMaxRowCount= 3; }

      CGroup(const CGroup & g)
      {
         operator = (g);
      }
      CGroup & operator =(const CGroup & g)
      {
         if(this != &g)
         {
            m_iMaxRowCount = g.m_iMaxRowCount;
            m_wstrName = g.m_wstrName;
            m_itema.Copy(g.m_itema);

         }
         return *this;

      }

      int m_iSep;

      int m_iMaxRowCount;

      CStringW m_wstrName;

      CArray < CItem >     m_itema;
      void ClearLayout()
      {

         for(int i = 0; i < m_itema.GetCount(); i++)
         {
            m_itema[i].ClearLayout();
         }

      }

   };

   class CTab
   {
   public:

      CTab(){}

      CTab(const CTab & t)
      {
         operator =(t);
      }

      CTab & operator =(const CTab & t)
      {
         if(this != &t)
         {
            m_wstrName = t.m_wstrName;
            m_groupa.Copy(t.m_groupa);

         }
         return *this;

      }

      CStringW m_wstrName;

      CArray < CGroup > m_groupa;

      void ClearLayout()
      {

         for(int i = 0; i < m_groupa.GetCount(); i++)
         {
            m_groupa[i].ClearLayout();
         }

      }

   };

   int              m_iFullHeight;
   int              m_iTabScrollMax;
   int              m_iTabScroll;
   int              m_iScrollMax;
   int              m_iScroll;
   CArrowButton     m_buttonTabLeft;
   CArrowButton     m_buttonTabRight;
   CArrowButton     m_buttonLeft;
   CArrowButton     m_buttonRight;

   int m_iGroupSmall;
   CRibbonProperties m_properties;
   int m_iMargin;
   bool  m_bExpanded;
   bool  m_bFocus;
   HICON m_hiconHelp;
   HICON m_hiconExpand;
   HICON m_hiconCollapse;
   Gdiplus::StringFormat m_format;
   Gdiplus::StringFormat m_formatTab;
   int m_iGroupHover;
   int m_iItemHover;
   int m_iGroupLButtonDown;
   int m_iItemLButtonDown;
   int m_iTabSel;
   CArray < CTab >   m_taba;
   CRect m_rTabBorder;
   CRect m_rIconBorder;
   CRect m_rIconMargin;
   int m_iTabHeight;
   Gdiplus::Font m_font2;
   CFont m_font;
   int m_xStart;
   float               m_fMaxTextHeight;

   bool m_bNeedLayout;

   CRibbonBar();
   virtual BOOL Create(CWnd* pParentWnd,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
      UINT nID = AFX_IDW_TOOLBAR);
   virtual BOOL CreateEx(CWnd* pParentWnd,DWORD dwCtrlStyle = TBSTYLE_FLAT,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
      CRect rcBorders = CRect(0,0,0,0),
      UINT nID = AFX_IDW_TOOLBAR);
   void layout_scroll();
   void ShowProperties(bool bShow);
   void get_tab_rect(LPRECT lprect,int i);
   void get_item_rect(LPRECT lprectItem,LPRECT lprectImage, LPRECT lprecText, int iGroup, int iItem);
   void calc_item_rect(Gdiplus::Graphics & g, LPRECT lprectItem,LPRECT lprectImage,LPRECT lprecText,int iGroup,int iItem);
   bool hit_test(CPoint pt,int & iGroup,int & iItem);
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
   virtual ~CRibbonBar();
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
   afx_msg void OnTabLeftArrow();
   afx_msg void OnTabRightArrow();
   afx_msg void OnRibbonLeftArrow();
   afx_msg void OnRibbonRightArrow();

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


   
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
};
