#pragma once



class CSearchBox: public CEdit
{
   DECLARE_DYNAMIC(CSearchBox)

   // Construction
public:
   CSearchBox();

   int  m_nBrowseButtonWidth;
   BOOL m_bIsButtonPressed;
   BOOL m_bIsButtonHighlighted;
   BOOL m_bIsButtonCaptured;
   BOOL m_bDefaultImage;

   CRect   m_rectBtn;
   CSize   m_sizeImage;
   CImageList m_ImageBrowse;

public:

   void SetBrowseButtonImage(HICON hIcon,BOOL bAutoDestroy = TRUE);
   void SetBrowseButtonImage(HBITMAP hBitmap,BOOL bAutoDestroy = TRUE);
   void SetBrowseButtonImage(UINT uiBmpResId);

   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual void OnBrowse();
   virtual void OnDrawBrowseButton(CDC* pDC,CRect rect,BOOL bIsButtonPressed,BOOL bIsButtonHot);
   virtual void OnChangeLayout();
   virtual void OnAfterUpdate();

   virtual ~CSearchBox();

   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg void OnNcCalcSize(BOOL bCalcValidRects,NCCALCSIZE_PARAMS FAR* lpncsp);
   afx_msg void OnNcPaint();
   afx_msg void OnNcLButtonDblClk(UINT nHitTest,CPoint point);
   afx_msg void OnNcMouseMove(UINT nHitTest,CPoint point);
   afx_msg void OnCancelMode();
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnRButtonUp(UINT nFlags,CPoint point);
   afx_msg LRESULT OnNcHitTest(CPoint point);

   DECLARE_MESSAGE_MAP()

   void SetInternalImage();
   afx_msg void OnPaint();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

