#ifndef __OFFSCREENDC_H__
#define __OFFSCREENDC_H__

#if _MSC_VER > 1000
#pragma once
#endif

class COffScreenDC : public CDC  
{
public:
	COffScreenDC(CDC* pDC);
	~COffScreenDC();
//	CDC override
//	int	 GetClipBox(LPRECT lpRect);
//
	BOOL Flush();
protected:
	CDC*	 m_pDC;
	CRect	 m_rcClipBox;
	CBitmap* m_pOldBitmap;
	bool m_bAttached;

static CBitmap bmOffScreen;
};

#endif
