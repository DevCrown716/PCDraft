#pragma once


// CArrowButton

class CArrowButton : public CButton
{
	DECLARE_DYNAMIC(CArrowButton)

public:

   enum e_type
   {
      type_left,
      type_right
   };

   int         m_iHover;
   e_type      m_etype;
   bool        m_bPressed;
   bool        m_bTimer;


	CArrowButton();
	virtual ~CArrowButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
   virtual void DrawItem(LPDRAWITEMSTRUCT);
   afx_msg void OnPaint();
   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnTimer(UINT_PTR nIDEvent);
};


