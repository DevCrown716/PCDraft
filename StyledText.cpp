// StyledText.cpp: implementation of the CStyledText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"

#include "StyledText.h"
#include "dsshape.h"
#include "lpoint.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CStyledText,CObject,1)

CStyledText::CStyledText()
{
	
}

CStyledText::~CStyledText()
{

}

void CStyledText::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	m_Style.Serialize(ar);
	m_StyleIndex.Serialize(ar);
	m_Text.Serialize(ar);
}

int CStyledText::GetStyleIndex(const CStyledTextStyle &style)
{
	for(int i=0;i<m_Style.GetSize();i++)
		if(memcmp(&m_Style.ElementAt(i),&style,sizeof(CStyledTextStyle))==0)
			return i;
	return m_Style.Add(style);
}

void CStyledText::RemoveAll()
{
	m_Style.RemoveAll();
	m_StyleIndex.RemoveAll();
	m_Text.RemoveAll();
}

int CStyledText::Add(TCHAR ch,const CStyledTextStyle& style)
{
	m_StyleIndex.Add(GetStyleIndex(style));
	return m_Text.Add(ch);
}

void CStyledText::InsertAt(int n,TCHAR ch,const CStyledTextStyle& style)
{
	m_StyleIndex.InsertAt(n,GetStyleIndex(style));
	m_Text.InsertAt(n,ch);
}
	
void CStyledText::SetAt(int n,TCHAR ch,const CStyledTextStyle& style)
{
	m_StyleIndex.SetAt(n,GetStyleIndex(style));
	m_Text.SetAt(n,ch);
}

void CStyledText::SetAt(int n,const CStyledTextStyle& style)
{
	m_StyleIndex.SetAt(n,GetStyleIndex(style));
}

void CStyledText::GetAt(int n,TCHAR &ch,CStyledTextStyle& style) const
{
	ch=m_Text.GetAt(n);
	style=m_Style.GetData()[m_StyleIndex.GetAt(n)];
}

void CStyledText::GetStyleAt(int n,CStyledTextStyle& style) const
{
	style=m_Style.GetData()[m_StyleIndex.GetAt(n)];
}

void CStyledText::RemoveAt(int n,int nCount)
{
	m_StyleIndex.RemoveAt(n,nCount);
	m_Text.RemoveAt(n,nCount);
}

void CStyledText::Copy(const CStyledText& st)
{
	RemoveAll();
	for(int i=0;i<st.GetSize();i++)
	{
		CStyledTextStyle style;
		TCHAR ch;
		st.GetAt(i,ch,style);
		Add(ch,style);
	}
}

int CStyledText::GetFirstTextPart() const
{
	if(m_Text.GetSize()==0)
		return -1;
	return 0;
}

void CStyledText::GetNextTextPart(int& pos,CString& text, CString & theTextPlus1, CStyledTextStyle& style,bool& bLineFeed, bool bUtf8) const
{
	text.Empty();
	if(pos>=m_Text.GetSize())
	{
		pos=m_Text.GetSize()-1;
		int styleIndex=m_StyleIndex.GetAt(pos);
		style=m_Style.GetData()[styleIndex];
		pos=-1;
		bLineFeed=true;
		return;
	}
	int styleIndex=m_StyleIndex.GetAt(pos);
	style=m_Style.GetData()[styleIndex];
	while(pos<m_Text.GetSize() && m_Text[pos]!='\n' && styleIndex==m_StyleIndex.GetAt(pos))
	{
		text+=m_Text[pos];
      const char * inc = _tcsinc(&m_Text[pos]);
      if (inc && *inc && pos + 1 < m_Text.GetSize())
      {
         theTextPlus1 = text + CString(inc, _tcsinc(inc) - inc);
      }
      else
      {
         theTextPlus1 = text;
      }
		pos++;
	}
	if(pos<m_Text.GetSize() && m_Text[pos]=='\n')
	{
		++pos;
		bLineFeed=true;
	}
	else
		bLineFeed=false;
}


int CStyledText::GetPrevPos(int pos) const
{
	ASSERT(pos >= 0 && pos <= m_Text.GetSize());

	const TCHAR* pData = m_Text.GetData();
    if (pData == NULL)
        return -1;
	int prevPos =  _tcsdec(pData, pData + pos) - pData;
	if (prevPos < 0)
		return -1;
	return prevPos;
}


int CStyledText::GetNextPos(int pos) const
{
	ASSERT(pos >= 0 && pos <= m_Text.GetSize());

	const TCHAR* pData = m_Text.GetData();
    if (pData == NULL)
        return -1;
	int nextPos = _tcsinc(pData + pos) - pData;
	if (nextPos > m_Text.GetSize())
		return -1;
	return nextPos;
}


bool CStyledText::IsEmpty()
{
	return m_Text.GetSize()==0;
}

bool CStyledText::operator ==(const CStyledText &st)
{
	return false;
}

void CStyledText::SetCase(int nCase,int nStartChar,int nEndChar)
{
	int i;
	BYTE ch;
	bool bNeedCaps;
	if(nStartChar<0)
		nStartChar=0;
	if(nEndChar<0 || nEndChar>=m_Text.GetSize())
		nEndChar=m_Text.GetSize()-1;
	switch(nCase)
	{
		case kCaseUpper:
			for(i=nStartChar;i<=nEndChar;i++)
			{
				ch=m_Text[i];
				if(islower(ch))
					m_Text[i]=toupper(ch);
			}
			break;
		case kCaseLower:
			for(i=nStartChar;i<=nEndChar;i++)
			{
				ch=m_Text[i];
				if(isupper(ch))
					m_Text[i]=tolower(ch);
			}
			break;
		case kCaseTitle:
			bNeedCaps=true;
			for(i=nStartChar;i<=nEndChar;i++)
			{
				ch=m_Text[i];
				if(isspace(ch)) bNeedCaps=true;
				else
				{
					if(bNeedCaps)
					{
						m_Text[i]=toupper(ch);
						bNeedCaps=false;
					}
					else m_Text[i]=tolower(ch);
				}
			}
			break;
		default:
			ASSERT(FALSE);
	}
}

void CStyledText::DoScale(const LongRatio& ratio,double factor)
{
	for(int i=0;i<m_Style.GetSize();i++)
	{
		CStyledTextStyle& style=m_Style.ElementAt(i);
		double nHeight=style.m_LogFont.lfHeight;
		if(factor) nHeight*=factor;
		nHeight=(nHeight*ratio.fN)/ratio.fD;
		style.m_LogFont.lfHeight=(long)nHeight;
		if(style.m_LogFont.lfHeight==0) style.m_LogFont.lfHeight=1;
	}
}

void CStyledText::MakeSelfContained(CPalette& pal)
{
	for(int i=0;i<m_Style.GetSize();i++)
	{
		CStyledTextStyle& style=m_Style.ElementAt(i);
		if(style.m_TextColor & PALETTEINDEX(0))
		{
			PALETTEENTRY pe;
			pal.GetPaletteEntries(style.m_TextColor&0xFF,1,&pe);
			style.m_TextColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
		}
	}
}
