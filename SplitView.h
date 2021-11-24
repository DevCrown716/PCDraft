#pragma once


// CSplitView view


class CSplitView : public CView
{
	DECLARE_DYNCREATE(CSplitView)

   CSplitterWndEx * m_psplit;

   CRect m_rectBorder;

protected:
	CSplitView();           // protected constructor used by dynamic creation
	virtual ~CSplitView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnSize(UINT nType,int cx,int cy);
};


