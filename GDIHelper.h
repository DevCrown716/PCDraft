#if !defined(AFX_GDIHELPER_H__INCLUDED_)
#define AFX_GDIHELPER_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

HBITMAP LoadBmpWithAlpha(UINT nBmpID, UINT nMaskID);
HBITMAP LoadBmpWithAlpha(UINT nBmpID, HINSTANCE resInstance);
HBITMAP LoadGradientBmp32(UINT nBmpID, UINT nMaskID, COLORREF clrStart, COLORREF clrEnd);


// helper template function for unused parameters
template <class T>
void Unused(T rT)
{
    static_cast<void>(rT);
};

class CGDI
{
protected:
    CDC* m_pDC;

    CGDI(CDC* pDC) : 
      m_pDC(pDC)
    {
    }

    virtual ~CGDI()
    {
        m_pDC = NULL;
    }
};

// Class to select a stock object
class CGDIStockObject : public CGDI
{
    CGdiObject* m_pLastObject;

public:
    CGDIStockObject(CDC* pDC, int nIndex) : CGDI(pDC),
      m_pLastObject(NULL)
    {
        m_pLastObject = m_pDC->SelectStockObject(nIndex);
    }

    virtual ~CGDIStockObject()
    {
        if (m_pLastObject)
        {
            Unused(m_pDC->SelectObject(m_pLastObject));

            m_pLastObject = NULL;
        }
    }
};


class CGDITextColor : public CGDI
{
    COLORREF m_LastColor;

public:
    CGDITextColor(CDC* pDC, COLORREF theColor) : CGDI(pDC),
      m_LastColor(RGB(0,0,0))
    {
        m_LastColor = m_pDC->SetTextColor(theColor);
    }

    virtual ~CGDITextColor()
    {
        Unused(m_pDC->SetTextColor(m_LastColor));
    }

};

class CGDIBackColor : public CGDI
{
    COLORREF m_LastColor;

public:
    CGDIBackColor(CDC* pDC, COLORREF theColor) : CGDI(pDC),
      m_LastColor(RGB(0,0,0))
    {
        m_LastColor = m_pDC->SetBkColor(theColor);
    }

    virtual ~CGDIBackColor()
    {
        Unused(m_pDC->SetBkColor(m_LastColor));
    }

};

class CGDIBackMode : public CGDI
{
    int m_LastMode;

public:
    CGDIBackMode(CDC* pDC, int theMode) : CGDI(pDC),
	  m_LastMode(0)
    {
        m_LastMode = m_pDC->SetBkMode(theMode);
    }

    virtual ~CGDIBackMode()
    {
        Unused(m_pDC->SetBkMode(m_LastMode));
    }

};

// This class can be used like this:
// CGDIObject<CFont>(pDC, pFont);
// CGDIObject<CPen>(pDC, pPen);

template <class T>
class CGDIObject : public CGDI
{
T* m_pLast;

public:
    CGDIObject(CDC* pDC, T* pObject) : CGDI(pDC), 
	  m_pLast(NULL)
    {
        if (pObject && pObject->GetSafeHandle())
        {
            m_pLast = pDC->SelectObject(pObject);
        }
    }

    virtual ~CGDIObject()
    {
        if (m_pLast)
        {
            Unused(m_pDC->SelectObject(m_pLast));

            m_pLast = NULL;
        }
    }
};

class CGDITextAlign : public CGDI
{
UINT m_Last;

public:
    CGDITextAlign(CDC* pDC, UINT nTextAlign) : CGDI(pDC),
	  m_Last(0)
    {
        m_Last = pDC->SetTextAlign(nTextAlign);
    }

    virtual ~CGDITextAlign()
    {
        Unused(m_pDC->SetTextAlign(m_Last));
    }
};


class CGDIROP : public CGDI
{
int m_Last;

public:
    CGDIROP(CDC* pDC, int nROP) : CGDI(pDC),
	  m_Last(0)
    {
        m_Last = pDC->SetROP2(nROP);
    }

    virtual ~CGDIROP()
    {
        Unused(m_pDC->SetROP2(m_Last));
    }
};

class CGDIMapMode : public CGDI
{
int m_Last;

public:
    CGDIMapMode(CDC* pDC, int nMapMode) : CGDI(pDC),
	  m_Last(0)
    {
        m_Last = pDC->SetMapMode(nMapMode);
    }

    virtual ~CGDIMapMode()
    {
        Unused(m_pDC->SetMapMode(m_Last));
    }
};

class CGDIBitmapDC : public CDC
{
	CBitmap* m_pLastBitmap;
public:
	CGDIBitmapDC(CDC *pDC,CBitmap *pBitmap)
	{
		Unused(CreateCompatibleDC(pDC));
		m_pLastBitmap=SelectObject(pBitmap);
	}

    virtual ~CGDIBitmapDC()
	{
		Unused(SelectObject(m_pLastBitmap));
	}
};

class CGDIArcDirection: public CGDI
{
	int m_nLast;
public:
	CGDIArcDirection(CDC* pDC, int newAttr) : CGDI(pDC),
		m_nLast(0)
	{
		m_nLast=pDC->SetArcDirection(newAttr);
	}

	virtual ~CGDIArcDirection()
	{
		Unused(m_pDC->SetArcDirection(m_nLast));
	}
};

class CGDIClipRgn: public CGDI
{
public:
	CGDIClipRgn(CDC *pDC,CRgn *pRgn) : CGDI(pDC)
	{
		m_pDC->SelectClipRgn(pRgn);
	}
	~CGDIClipRgn()
	{
		m_pDC->SelectClipRgn(NULL);
	}
};

#endif
