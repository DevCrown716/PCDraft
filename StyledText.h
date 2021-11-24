// StyledText.h: interface for the CStyledText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STYLEDTEXT_H__64531B16_2489_11D3_9543_008048DA0F68__INCLUDED_)
#define AFX_STYLEDTEXT_H__64531B16_2489_11D3_9543_008048DA0F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <afxtempl.h>

enum enum_composition_state
{
   e_composition_state_none,
   e_composition_state_composing,
   e_composition_state_selected,
};


struct PhIOContext;

struct	CStyledTextStyle
{
	COLORREF m_TextColor;
	LOGFONT	m_LogFont;
};

struct	LongRatio;

class CStyledText : public CObject  
{
DECLARE_SERIAL(CStyledText)
public:
	CStyledText();
	virtual ~CStyledText();

	virtual void Serialize(CArchive& ar);

	bool operator ==(const CStyledText &st);

	bool IsEmpty();
	int GetStyleIndex(const CStyledTextStyle& style);
	int Add(TCHAR ch,const CStyledTextStyle& style);
	void InsertAt(int n,TCHAR ch,const CStyledTextStyle& style);
	void SetAt(int n,TCHAR ch,const CStyledTextStyle& style);
	void SetAt(int n,const CStyledTextStyle& style);
	void GetAt(int n,TCHAR &ch,CStyledTextStyle& style) const;
	void GetStyleAt(int n,CStyledTextStyle& style) const;
	TCHAR GetCharAt(int n) const { return m_Text.GetAt(n); }
	void RemoveAt(int n,int nCount=1);
	void RemoveAll();
	void Copy(const CStyledText& st);
	int GetSize() const { return (int) m_Text.GetSize(); }
	LPSTR GetTextPtr() { return LPSTR(m_Text.GetData()); }
   LPCSTR GetTextPtr() const { return LPCSTR(m_Text.GetData()); }
	int GetFirstTextPart() const;
	void GetNextTextPart(int& pos,CString& text,CString & textPlus1,CStyledTextStyle& style,bool& bLineFeed, bool bUtf8 = false) const;
	int GetPrevPos(int pos) const;
	int GetNextPos(int pos) const;

	void SetCase(int nCase,int nStartChar=0,int nEndChar=-1);
	void DoScale(const LongRatio& ratio,double factor);
	void MakeSelfContained(CPalette& pal);
protected:
	CArray<CStyledTextStyle, const CStyledTextStyle &> m_Style;
	CArray<short, short> m_StyleIndex;
	CArray<TCHAR, TCHAR> m_Text;
};

#endif // !defined(AFX_STYLEDTEXT_H__64531B16_2489_11D3_9543_008048DA0F68__INCLUDED_)
