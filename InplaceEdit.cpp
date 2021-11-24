// InplaceEdit.cpp: implementation of the CInplaceEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InplaceEdit.h"

#include "DSShape.h"
#include "DraftContext.h"
#include "PCDraftView.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInplaceEdit::CInplaceEdit()
{
   m_bImeComposition = false; // Infinisys Ltd. // Japanese Inline Text
   m_ecompositionstate = e_composition_state_none;

	::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONT),&m_DefStyle.m_LogFont);
	m_DefStyle.m_TextColor=RGB(0,0,0);
	m_nLineSpace=kSingleSpace;
	m_nAlign=kAlignLeft;
	m_nCaretChar=m_nStartChar=m_nEndChar=-1;
	m_pWnd=NULL;
	m_pMapper=NULL;
	m_ptOrg.x=0;
	m_ptOrg.y=0;
	m_BoundRect.SetRectEmpty();
	m_bAutoAdjustBoundRect=false;
	m_rgbBackColor=RGB(255,255,255);

	m_pUndo=NULL;
	m_pRedo=NULL;
	m_bRedo=false;
	m_bTyping=false;
}

CInplaceEdit::~CInplaceEdit()
{
	ClearUndo();
}

void CInplaceEdit::SetTextParams(const TextParams& tp)
{
	m_DefStyle.m_LogFont=tp.m_lf;
	m_nLineSpace=tp.m_nSpacing;
	m_nAlign=tp.m_nJust;
	m_nVAlign=tp.m_nVAlign;
	m_nCase=tp.m_nCase;
}

void CInplaceEdit::GetTextParams(TextParams& tp) const
{
	tp.m_lf=m_DefStyle.m_LogFont;
	tp.m_nSpacing=m_nLineSpace;
	tp.m_nJust=m_nAlign;
	tp.m_nVAlign=m_nVAlign;
	tp.m_nCase=m_nCase;
}

void CInplaceEdit::GetCaretTextParams(TextParams& tp) const
{
	if(m_nStartChar==m_nEndChar && (m_nEndChar==-1 || m_nEndChar==m_StyledText.GetSize()))
	{
		tp.m_lf=m_DefStyle.m_LogFont;
	}
	else
	{
		CStyledTextStyle style;
		TCHAR ch;
		m_StyledText.GetAt(m_nStartChar,ch,style);
		tp.m_lf=style.m_LogFont;
	}
	tp.m_nSpacing=m_nLineSpace;
	tp.m_nJust=m_nAlign;
	tp.m_nVAlign=m_nVAlign;
	tp.m_nCase=m_nCase;
}

void CInplaceEdit::SetDefFont(const LOGFONT& lf)
{
	m_DefStyle.m_LogFont=lf;
	Redraw(true);
}

void CInplaceEdit::SetDefColor(COLORREF color)
{
	m_DefStyle.m_TextColor=color;
}

void CInplaceEdit::GetDefFont(LOGFONT& lf)
{
	lf=m_DefStyle.m_LogFont;
}

void CInplaceEdit::SetLineSpace(int nLineSpace)
{
	m_nLineSpace=nLineSpace;
	Redraw(false);
}

void CInplaceEdit::SetTextAlign(int nAlign)
{
	m_nAlign=nAlign;
	switch (m_nAlign)
	{
	case kAlignLeft: m_ptOrigin.x = m_BoundRect.left;
		break;
	case kAlignCenter: m_ptOrigin.x = (m_BoundRect.left + m_BoundRect.right) / 2;
		break;
	case kAlignRight: m_ptOrigin.x = m_BoundRect.right;
		break;
	}
	Redraw(false);
}

void CInplaceEdit::SetCase(int nCase)
{
	int nStartChar,nEndChar;
	if(m_nStartChar==m_nEndChar)
	{
		nStartChar=0;
		nEndChar=-1;
	}
	else
	{
		nStartChar=m_nStartChar;
		nEndChar=m_nEndChar;
	}
	m_StyledText.SetCase(nCase,nStartChar,nEndChar);
	Redraw(false);
}

void CInplaceEdit::GetSel(int &nStartChar,int &nEndChar)
{
	nStartChar=m_nStartChar;
	nEndChar=m_nEndChar;
}

void CInplaceEdit::SetSel(int nStartChar,int nEndChar,int nCaretChar)
{
	if(nStartChar==nEndChar)
	{
		if(nEndChar==-1) m_nCaretChar=m_nStartChar=m_nEndChar=-1;
		else if(nEndChar<0) m_nCaretChar=m_nStartChar=m_nEndChar=0;
		else if(nEndChar>m_StyledText.GetSize()) m_nCaretChar=m_nStartChar=m_nEndChar=m_StyledText.GetSize();
		else m_nCaretChar=m_nStartChar=m_nEndChar=nEndChar;
	}
	else
	{
		if(nStartChar<0) nStartChar=0;
		if(nEndChar==-1 || nEndChar>m_StyledText.GetSize()) nEndChar=m_StyledText.GetSize();
		m_nStartChar=nStartChar;
		m_nEndChar=nEndChar;
		m_nCaretChar=nCaretChar;
	}
	TextParams tp;
	GetCaretTextParams(tp);
	SetTextParams(tp);
}

void CInplaceEdit::ReplaceSel(LPCTSTR pText, enum_composition_state ecompositionstate)
{
	if(m_nStartChar==-1 && m_nEndChar==-1)
		return;
	AdjustFontCharSet(m_DefStyle.m_LogFont);
   m_ecompositionstate = ecompositionstate;
	int nTextLength=pText?strlen(pText):0;

   m_nCompositionStartChar = m_nStartChar;
   m_nCompositionEndChar = m_nStartChar + nTextLength;

	while(nTextLength--)
	{
		if(m_nStartChar<m_nEndChar)
		{
			m_StyledText.SetAt(m_nStartChar,*pText++,m_DefStyle);
		}
		else
		{
			m_StyledText.InsertAt(m_nStartChar,*pText++,m_DefStyle);
		}
		m_nStartChar++;
	}
   
	while(m_nStartChar<m_nEndChar)
	{
		m_StyledText.RemoveAt(m_nStartChar);
		--m_nEndChar;
	}
   

	m_nCaretChar=m_nEndChar=m_nStartChar;
	Redraw(false);
}

void CInplaceEdit::ReplaceSel(const CStyledText& st)
{
	if(m_nStartChar==-1 && m_nEndChar==-1)
		return;
	TCHAR ch;
	CStyledTextStyle style;
	for(int i=0;i<st.GetSize();i++)
	{
		if(m_nStartChar<m_nEndChar)
		{
			st.GetAt(i,ch,style);
			m_StyledText.SetAt(m_nStartChar,ch,style);
		}
		else
		{
			st.GetAt(i,ch,style);
			m_StyledText.InsertAt(m_nStartChar,ch,style);
		}
		m_nStartChar++;
	}
	while(m_nStartChar<m_nEndChar)
	{
		m_StyledText.RemoveAt(m_nStartChar);
		--m_nEndChar;
	}
	m_nCaretChar=m_nEndChar=m_nStartChar;
   if (!m_bImeComposition)
   {
      Redraw(false);

   }
}

void CInplaceEdit::ReplaceSelFont(const LOGFONT& lf)
{
	if(m_nStartChar==-1 && m_nEndChar==-1)
		return;
	CStyledTextStyle style;
	for(int n=m_nStartChar;n<m_nEndChar;n++)
	{
		m_StyledText.GetStyleAt(n,style);
		style.m_LogFont=lf;
		m_StyledText.SetAt(n,style);
	}
	Redraw(false);
}

void CInplaceEdit::ReplaceSelColor(COLORREF color)
{
	if(m_nStartChar==-1 && m_nEndChar==-1)
		return;
	CStyledTextStyle style;
	for(int n=m_nStartChar;n<m_nEndChar;n++)
	{
		m_StyledText.GetStyleAt(n,style);
		style.m_TextColor=color;
		m_StyledText.SetAt(n,style);
	}
	Redraw(false);
}

void CInplaceEdit::Redraw(bool bCaretOnly)
{
	if(m_pWnd)
	{
		OnKillFocus();
		if(!bCaretOnly)
		{
			CRect rc;
			if(m_bAutoAdjustBoundRect)
			{
				rc=m_BoundRect;
				rc.OffsetRect(-m_ptOrg.x,-m_ptOrg.y);
				rc.InflateRect(3,3);
				m_pWnd->InvalidateRect(&rc);

				CClientDC theDC(m_pWnd);
				CDraftContext theContext(&theDC,m_pMapper);
            CalcBoundRect(&theContext, true);
            if (m_pWnd->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
            {
               LongRect lrcBounds = m_pMapper->DPtoLP(m_BoundRect);

               auto pview = ((CPCDraftView*)m_pWnd);

               auto pdoc = pview->GetDocument();

               bool bEditActive = pview->IsEditActive();

               bool bExpanded = pdoc->ExpandDoc(lrcBounds, false);

               if(bExpanded)
               {

                  pview->m_pbitmapDrawBackground.release();
            
                  pdoc->UpdateMaxZoom();
                  pdoc->UpdateAllViews(NULL, kDocumentSizeChanged);
                  pdoc->SetModifiedFlag();


                  pview->RedrawWindow();

                  pview->m_pbitmapDrawBackground.reset(((CPCDraftView*)m_pWnd)->BitmapFromView());

               }

				}

				rc=m_BoundRect;
            
			}
			else
				rc=m_editRect;
			rc.OffsetRect(-m_ptOrg.x,-m_ptOrg.y);
			rc.InflateRect(3,3);
			//m_pWnd->InvalidateRect(&rc);
         m_pWnd->InvalidateRect(NULL);
			m_pWnd->UpdateWindow();
         //m_pWnd->RedrawWindow(nullptr, nullptr, RDW_NOERASE | RDW_INVALIDATE| RDW_UPDATENOW);
		}
		OnSetFocus();
	}
}


int CInplaceEdit::CalcLineExtent(CDraftContext* pDC,int pos, double & dLineSpacing, double &baseAscent, double &dDescent, double &width, bool bRelaxedBox)
{

	return pDC->CalcLineExtent(m_StyledText,pos,dLineSpacing, baseAscent,dDescent,width,bRelaxedBox, m_bAutoAdjustBoundRect,m_BoundRect.Width());

}


void __copy(Gdiplus::RectF & r, LPCRECT lpcrect);

void CInplaceEdit::DrawBackground(CDraftContext * pDC)
{

         CRect rectBounds(m_bAutoAdjustBoundRect ? m_BoundRect : m_editRect);
   LongRect lrBounds = pDC->GetMapper()->DPtoLP(rectBounds);
   UseRect userect(pDC, lrBounds);
   {
      //CGDIBackColor bk(pDC->GetCDC(),m_rgbBackColor);
      //pDC->GetCDC()->ExtTextOut(0,0,ETO_OPAQUE,m_bAutoAdjustBoundRect?&m_BoundRect:&m_editRect,NULL,0,NULL);

      COLORREF cr = pDC->m_pDoc->translate_color(m_rgbBackColor);

      Gdiplus::SolidBrush br(Gdiplus::Color(255, GetRValue(cr), GetGValue(cr), GetBValue(cr)));
      pDC->graphics_on();
      Gdiplus::RectF r;
      __copy(r, rectBounds);
      pDC->graphics()->FillRectangle(&br, r);
   }

}


void CInplaceEdit::Draw(CDraftContext* pDC)
{
   CRect rectBounds(m_bAutoAdjustBoundRect ? m_BoundRect : m_editRect);
   LongRect lrBounds = pDC->GetMapper()->DPtoLP(rectBounds);
   UseRect userect(pDC, lrBounds);
   {
      //CGDIBackColor bk(pDC->GetCDC(),m_rgbBackColor);
      //pDC->GetCDC()->ExtTextOut(0,0,ETO_OPAQUE,m_bAutoAdjustBoundRect?&m_BoundRect:&m_editRect,NULL,0,NULL);

      COLORREF cr = pDC->m_pDoc->translate_color(m_rgbBackColor);

      Gdiplus::SolidBrush br(Gdiplus::Color(255, GetRValue(cr), GetGValue(cr), GetBValue(cr)));
      pDC->graphics_on();
      Gdiplus::RectF r;
      __copy(r, rectBounds);
      if (r.Width > 4)
      {
         pDC->graphics()->FillRectangle(&br, r);

      }
   }

	if(!m_bAutoAdjustBoundRect && m_nVAlign!=kAlignTop)
	{
		CalcBoundRect(pDC, false);
		int nEditHeight = m_editRect.Height();
		int nBoundHeight = m_BoundRect.Height();
		if(m_nVAlign==kAlignMiddle)
		{
			m_BoundRect.top = m_editRect.top + (nEditHeight - nBoundHeight)/2;
		}
		else
		{
			m_BoundRect.top = m_editRect.top + (nEditHeight - nBoundHeight);
		}
		m_BoundRect.bottom = m_BoundRect.top + nBoundHeight;
		m_BoundRect.left = m_editRect.left;
		m_BoundRect.right = m_editRect.right;
	}
	
	LongRect lrc=m_pMapper->DPtoLP(m_BoundRect);
	lrc.Inset(-3,-3);
	LongPoint theCorners[4];
	for(int i=0;i<4;i++)
		theCorners[i]=lrc.GetCorner(i);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&m_editRect);
   pDC->m_ecompositionstate = m_ecompositionstate;
   pDC->m_nCompositionStartChar = m_nCompositionStartChar;
   pDC->m_nCompositionEndChar = m_nCompositionEndChar;
   pDC->DrawStyledText(255,m_StyledText,theCorners,m_bAutoAdjustBoundRect,m_bAutoAdjustBoundRect?NULL:&rgn,m_nStartChar,m_nEndChar);
}


int CInplaceEdit::CharFromPos(CDraftContext * pDC, pointf pt)
{

   pt.X -= m_BoundRect.left;

	pt.Y -= m_BoundRect.top;

	int pos = m_StyledText.GetFirstTextPart();

	double lastBaseLine = -1;

	double dLastDescent = 0;

   double dLineSpacing;
   
   double dAscent;
   
   double dDescent = 0.0;
   
   double width;

	while(pos != -1)
	{

      dLastDescent = dDescent;

		int nextLinePos = CalcLineExtent(pDC,pos,dLineSpacing,dAscent,dDescent,width, false);
		
      if(lastBaseLine < 0.0)
		{

			lastBaseLine=dLineSpacing;

		}
		else
		{

			double n = dLineSpacing;

			switch(m_nLineSpace)
			{
				case kSingleSpace: break;
				case k1_5Space: n *= 1.5; break;
				case kDoubleSpace: n *= 2.0; break;
				default:
					ASSERT(FALSE);
			}

			lastBaseLine += n;

		}

		if(pt.Y < lastBaseLine)
		{

         bool bLineFeed = false;

			int n;

			int offset,lastOffset;

			switch(m_nAlign)
			{
				case kAlignLeft: offset=0; break;
				case kAlignCenter: offset=(m_BoundRect.Width()-width)/2;  break;
				case kAlignRight: offset=m_BoundRect.Width()-width; break;
				default:
					ASSERT(FALSE);
			}
         
         double dBaseW = 0.0;

         bool bNewLine = true;
			
         while(pos!=-1)
			{
			
            CString theText;
            CString theTextPlus1;
				CStyledTextStyle style;
				n=pos;
				m_StyledText.GetNextTextPart(pos,theText,theTextPlus1, style,bLineFeed);
				pDC->SelectFont(style.m_LogFont);
				lastOffset=offset;

				int nCharLen;
            int nPos = 0;
            Gdiplus::RectF rBounds;
            int nWideChar = 0;
            bool bVertical = style.m_LogFont.lfFaceName[0] == '@';


            for (LPCTSTR p = theText; *p; p = _tcsinc(p))
            {
                  
               nCharLen = _tcsinc(p) - p;
                  
               nWideChar++;

               if (pt.X < offset)
               {
                     
                  pDC->UnselectFont();
                     
                  return abs(pt.X - offset) <= abs(pt.X - lastOffset) ? n : m_StyledText.GetPrevPos(n);

               }

               nPos += nCharLen;

               CStringW wstrText(get_uni(theText, CP_ACP));
                  
               CStringW wstrTextPlus1(wstrText);

               wstrTextPlus1 += L"L";

               int iWideLength = wstrTextPlus1.GetLength();

               pDC->graphics()->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

               Gdiplus::CharacterRange cr;

               cr.First = 0;

               cr.Length = nWideChar;

               Gdiplus::RectF layoutRect(0.f, 0.f, 655350.f, 655350.f);

               Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

               if (bVertical)
               {

                  format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

               }

               format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

               format.SetMeasurableCharacterRanges(1, &cr);

               Gdiplus::Region r;

               pDC->graphics()->MeasureCharacterRanges(wstrTextPlus1, wstrTextPlus1.GetLength(), pDC->font(), layoutRect, &format, 1, &r);

               r.GetBounds(&rBounds, pDC->graphics());

					lastOffset = offset;

					offset = dBaseW + (bVertical ? rBounds.Height + (bNewLine ? rBounds.Y : 0.0) : rBounds.Width + (bNewLine ? rBounds.X : 0.0));

					n += nCharLen;

               bool bOverflow = n >= nextLinePos && nextLinePos > 0;

               if (!m_bAutoAdjustBoundRect && bOverflow)
               {

						pos = n;
							
                  bLineFeed = true;

						break;

					}

				}

            dBaseW += (bVertical ? rBounds.Height + (bNewLine ? rBounds.Y : 0.0) : rBounds.Width + (bNewLine ? rBounds.X : 0.0));

            bNewLine = false;

				pDC->UnselectFont();

            if (bLineFeed)
            {

               break;

            }

            bNewLine = false;

			}

         if (bLineFeed)
         {

            pDC->UnselectFont();

            return abs(pt.X - offset) <= abs(pt.X - lastOffset) ? n : m_StyledText.GetPrevPos(n);

         }
				
		}
      else
      {

         pos = nextLinePos;

      }

	}

	return m_StyledText.GetSize();

}

void CInplaceEdit::SetBoundRect(const CRect &rc)
{
	m_BoundRect=rc;
	m_editRect = rc;
	m_bAutoAdjustBoundRect=false;
}

void CInplaceEdit::CalcTrueBoundRect(LongRect &lrc)
{
	lrc=m_pMapper->DPtoLP(m_BoundRect);
	LongPoint lpt=lrc.GetCorner(0);
	CPCDraftDoc* pDoc=((CPCDraftView*)m_pWnd)->GetDocument();
	int nZoomFactor=pDoc->m_nZoomFactor;
	pDoc->m_nZoomFactor=0;
	CClientDC theDC(m_pWnd);
	CDraftContext theContext(&theDC,m_pMapper);
	CalcBoundRect(&theContext, false);
	lrc=m_pMapper->DPtoLP(m_BoundRect);
	pDoc->m_nZoomFactor=nZoomFactor;
	lrc.Set(lpt,Width(lrc),Height(lrc));
}

void CInplaceEdit::GetBoundRect(CRect &rc)
{
	rc = m_bAutoAdjustBoundRect?m_BoundRect:m_editRect;
}

void CInplaceEdit::SetWindow(CWnd* pWnd)
{
	m_pWnd=pWnd;
}

void CInplaceEdit::SetMapper(CMapper* pMapper)
{
	m_pMapper=pMapper;
}

void CInplaceEdit::OnSetFocus()
{
	if(m_pWnd && m_nCaretChar!=-1)
	{
		pointf pt;
		CSize size;
		{
			CClientDC theDC(m_pWnd);
			{
				CDraftContext theContext(&theDC,m_pMapper);
				GetCaretDim(&theContext,pt,size);
			}
		}
		CreateCaret(m_pWnd->GetSafeHwnd(),NULL,size.cx,size.cy);
		pt.X-=m_ptOrg.x;
		pt.Y-=m_ptOrg.y;
		CWnd::SetCaretPos(pt);
      m_rectCaret.left = pt.X;
      m_rectCaret.top = pt.Y;
      m_rectCaret.right = m_rectCaret.left + size.cx;
      m_rectCaret.bottom = m_rectCaret.top + size.cy;
		m_pWnd->ShowCaret();
	}
}

void CInplaceEdit::OnKillFocus()
{
	if(m_pWnd && m_nCaretChar!=-1)
	{
		m_pWnd->HideCaret();
		DestroyCaret();
	}
}


void CInplaceEdit::GetCaretDim(CDraftContext* pDC,pointf& pt,CSize& size)
{

   double dDescent = 0.0;
   double offset;
   double dLastDescent;


   offset = pDC->GetMapper()->LPtoDPF(3);
   dLastDescent = offset;


	if(m_nCaretChar!=-1)
	{

		int pos=m_StyledText.GetFirstTextPart();
		double lastBaseLine=-1;
      double dLineSpacing;
      double dAscent;
      double width;



      while(pos!=-1)
		{

         dLastDescent = dDescent;

			int newPos=CalcLineExtent(pDC,pos,dLineSpacing,dAscent,dDescent,width, false);

			if(lastBaseLine< 0.0)
			{

				lastBaseLine=dLineSpacing;
			}
			else
			{
				double n = dLineSpacing;
				switch(m_nLineSpace)
				{
					case kSingleSpace: break;
					case k1_5Space: n*=1.5; break;
					case kDoubleSpace: n*=2.0; break;
					default:
						ASSERT(FALSE);
				}
				lastBaseLine+=n;
			}
			int n;
			switch(m_nAlign)
			{
				case kAlignLeft: offset=0; break;
				case kAlignCenter: offset=(m_BoundRect.Width()-width)/2;  break;
				case kAlignRight: offset=m_BoundRect.Width()-width; break;
				default:
					ASSERT(FALSE);
			}

         offset = pDC->GetMapper()->LPtoDPF(3);
         dLastDescent = offset;

         double dBaseW = 0.0;
         bool bNewLine = true;
			while(pos!=-1)
			{

				CString theText;
            CString theTextPlus1;
				CStyledTextStyle style;
				bool bLineFeed;
				n=pos;
				m_StyledText.GetNextTextPart(pos,theText,theTextPlus1,style,bLineFeed);
				pDC->SelectFont(style.m_LogFont);
				int nCharLen;
            int nPos = 0;
            const char * pStart = theText;
            Gdiplus::RectF rBounds;


            bool bVertical = style.m_LogFont.lfFaceName[0] == '@';

            pDC->graphics()->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
            CStringW wstrText(get_uni(theText, CP_ACP));
            CStringW wstrTextPlus1(wstrText);
            wstrTextPlus1 += L"L";

            int nWideChar = 0;
				for(LPCTSTR p = theText; *p; p = _tcsinc(p))
				{
					nCharLen = _tcsinc(p) - p;
					if(n==m_nCaretChar)
					{
						pt.X= (double)m_BoundRect.left+offset;
						pt.Y=(double)m_BoundRect.top+lastBaseLine-dLineSpacing+dLastDescent;
						size.cx=1;
						size.cy=dLineSpacing;
						pDC->UnselectFont();
                  m_dCaretLineSpacing = dLineSpacing;
						return;
					}
               
               nPos += nCharLen;
               
               nWideChar++;
               
               n += nCharLen;

               auto pNext = _tcsinc(p);

               bool bOverflow = n >= newPos && newPos > 0;

               if ((!m_bAutoAdjustBoundRect && bOverflow) || n == m_nCaretChar || (!pNext || !*pNext))
               {

                  Gdiplus::CharacterRange cr;

                  cr.First = 0;

                  cr.Length = nWideChar;

                  Gdiplus::RectF layoutRect(0.f, 0.f, 65535.f, 65535.f);

                  Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

                  if (bVertical)
                  {

                     format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

                  }

                  format.SetMeasurableCharacterRanges(1, &cr);

                  Gdiplus::Region r;

                  pDC->graphics()->MeasureCharacterRanges(wstrTextPlus1, wstrTextPlus1.GetLength(), pDC->font(), layoutRect, &format, 1, &r);

                  r.GetBounds(&rBounds, pDC->graphics());

                  offset = dBaseW + (bVertical ? rBounds.Height + (bNewLine ? rBounds.Y:0.0): rBounds.Width+(bNewLine ? rBounds.X : 0.0));

               }

					if(!m_bAutoAdjustBoundRect && bOverflow)
					{

						pos=n;

						bLineFeed=true;

						break;

					}
				}

            dBaseW += (bVertical ? rBounds.Height + (bNewLine ? rBounds.Y : 0.0) : rBounds.Width + (bNewLine ? rBounds.X : 0.0));
            bNewLine = false;

				pDC->UnselectFont();

            if (bLineFeed)
            {

               break;

            }

			}

			if(n==m_nCaretChar)
			{
				pt.X= (double)m_BoundRect.left+offset;
				pt.Y=(double)m_BoundRect.top+lastBaseLine-dLineSpacing;
				size.cx=1;
				size.cy=dLineSpacing+dDescent;
				return;
			}
		}
	}
	switch(m_nAlign)
	{
		case kAlignLeft: pt.X=0; break;
		case kAlignCenter: pt.X=m_BoundRect.Width()/2;  break;
		case kAlignRight: pt.X=m_BoundRect.Width(); break;
		default:
			ASSERT(FALSE);
	}
	pt.X+=m_BoundRect.left + offset;
	pt.Y=m_BoundRect.top + dLastDescent;
	pDC->SelectFont(m_DefStyle.m_LogFont);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	pDC->UnselectFont();
	size.cx=1;
	size.cy=tm.tmAscent+tm.tmDescent+tm.tmExternalLeading;
}

void CInplaceEdit::GoHome()
{
	bool bCaretOnly=m_nStartChar==m_nEndChar;
	int nEnd=GetKeyState(VK_SHIFT)<0?m_nEndChar:0;
	SetSel(0,nEnd,0);
	Redraw(bCaretOnly && m_nStartChar==m_nEndChar);
}

void CInplaceEdit::GoEnd()
{
	bool bCaretOnly=m_nStartChar==m_nEndChar;
	int n=m_StyledText.GetSize();
	int nStart=GetKeyState(VK_SHIFT)<0?m_nStartChar:n;
	SetSel(nStart,n,n);
	Redraw(bCaretOnly && m_nStartChar==m_nEndChar);
}

void CInplaceEdit::CommonCaretInc(int n)
{
	if(n>m_StyledText.GetSize()) n=m_StyledText.GetSize();
	int nStart,nEnd;
	if(GetKeyState(VK_SHIFT)<0)
	{
		if(m_nCaretChar==m_nEndChar)
		{
			nStart=m_nStartChar;
			nEnd=n;
		}
		else
		{
			nStart=n;
			nEnd=m_nEndChar;
		}
	}
	else
		nStart=nEnd=n;
	bool bCaretOnly=m_nStartChar==m_nEndChar;
	SetSel(nStart,nEnd,n);
	Redraw(bCaretOnly && m_nStartChar==m_nEndChar);
}

void CInplaceEdit::CommonCaretDec(int n)
{
	if(n<0) n=0;
	int nStart,nEnd;
	if(GetKeyState(VK_SHIFT)<0)
	{
		if(m_nCaretChar==m_nStartChar)
		{
			nStart=n;
			nEnd=m_nEndChar;
		}
		else
		{
			nStart=m_nStartChar;
			nEnd=n;
		}
	}
	else
		nStart=nEnd=n;
	bool bCaretOnly=m_nStartChar==m_nEndChar;
	SetSel(nStart,nEnd,n);
	Redraw(bCaretOnly && m_nStartChar==m_nEndChar);
}


void CInplaceEdit::GoUp()
{

   if (m_nCaretChar <= 0)
   {

      return;

   }

	int n = m_nCaretChar;

	{

		CClientDC theDC(m_pWnd);

		{

			CDraftContext theContext(&theDC, m_pMapper);
			
         pointf pt, newPt;
			
         CSize size;
			
         GetCaretDim(&theContext, pt, size);

			newPt = pt;
         
         newPt.Y -= m_dCaretLineSpacing;

			//while(m_nCaretChar>0)
			//{
			//	m_nCaretChar = m_StyledText.GetPrevPos(m_nCaretChar);
			//	GetCaretDim(&theContext,newPt,size);
			//	if(pt.y!=newPt.y)
			//		break;
			//}

			m_nCaretChar = n;

			if(pt.Y != newPt.Y)
			{

				pt.Y = newPt.Y;

				n = CharFromPos(&theContext, pt);

			}
         else
         {

            n = m_StyledText.GetPrevPos(n);

         }

		}

	}

	CommonCaretDec(n);

}


void CInplaceEdit::GoDown()
{

   if (m_nCaretChar == m_StyledText.GetSize())
   {

      return;

   }

	int n=m_nCaretChar;

	{

		CClientDC theDC(m_pWnd);

		{

			CDraftContext theContext(&theDC, m_pMapper);

			pointf pt, newPt;

			CSize size;

			GetCaretDim(&theContext, pt, size);

			newPt = pt;

         newPt.Y += m_dCaretLineSpacing;

			//while(m_nCaretChar<m_StyledText.GetSize())
			//{
			//	m_nCaretChar = m_StyledText.GetNextPos(m_nCaretChar);
			//	GetCaretDim(&theContext,newPt,size);
			//	if(pt.y!=newPt.y)
			//		break;
			//}

			m_nCaretChar = n;

			if(pt.Y != newPt.Y)
			{

				pt.Y = newPt.Y;

				n = CharFromPos(&theContext,pt);

			}
         else
         {

            n = m_StyledText.GetNextPos(n);

         }

		}

	}

	CommonCaretInc(n);

}


int CInplaceEdit::WordLeft(int nChar)
{

	nChar = m_StyledText.GetPrevPos(nChar);

	LPTSTR str = m_StyledText.GetTextPtr();

	while(nChar > 0)
	{

      if (!_istspace(_tcsnextc(str + nChar)))
      {

         break;

      }

		nChar = m_StyledText.GetPrevPos(nChar);

	}

	while(nChar > 0)
	{

		if(_istspace(_tcsnextc(str+nChar)))
		{

			nChar = m_StyledText.GetNextPos(nChar);

			break;

		}

		nChar = m_StyledText.GetPrevPos(nChar);

	}

	return nChar<0?0:nChar;

}


int CInplaceEdit::WordRight(int nChar)
{
	LPTSTR str = m_StyledText.GetTextPtr();
	while(nChar<m_StyledText.GetSize())
	{
		if(_istspace(_tcsnextc(str+nChar)))
			break;
		nChar = m_StyledText.GetNextPos(nChar);
	}
	while(nChar<m_StyledText.GetSize())
	{
		if(!_istspace(_tcsnextc(str+nChar)))
			break;
		nChar = m_StyledText.GetNextPos(nChar);
	}
	return nChar>m_StyledText.GetSize()?m_StyledText.GetSize():nChar;
}

void CInplaceEdit::GoLeft()
{
	if (m_nCaretChar > 0)
		CommonCaretDec(GetKeyState(VK_CONTROL)<0 ? WordLeft(m_nCaretChar) : m_StyledText.GetPrevPos(m_nCaretChar));
}

void CInplaceEdit::GoRight()
{
	if (m_nCaretChar < m_StyledText.GetSize())
		CommonCaretInc(GetKeyState(VK_CONTROL)<0 ? WordRight(m_nCaretChar) : m_StyledText.GetNextPos(m_nCaretChar));
}

void CInplaceEdit::DoBackSpace()
{
	SaveUndo();
	if(m_nStartChar==m_nEndChar)
	{
		if(m_nCaretChar>0)
		{
			int nPrevPos = m_StyledText.GetPrevPos(m_nCaretChar);
			SetSel(nPrevPos, m_nCaretChar, nPrevPos);
			ReplaceSel("");
		}
	}
	else
	{
		ReplaceSel("");
	}
}

void CInplaceEdit::DoDelete()
{
	SaveUndo();
	if(m_nStartChar==m_nEndChar)
	{
		if(m_nCaretChar>=0 && m_nCaretChar<m_StyledText.GetSize())
		{
			SetSel(m_nCaretChar,m_StyledText.GetNextPos(m_nCaretChar),m_nCaretChar);
			ReplaceSel("");
		}
	}
	else
	{
		ReplaceSel("");
	}
}

void CInplaceEdit::DoTyping(TCHAR ch)
{
	if(!m_bTyping)
	{
		SaveUndo();
		m_bTyping=true;
	}
	ReplaceSel(CString(ch));
}

void CInplaceEdit::DoReturn()
{
	SaveUndo();
	ReplaceSel("\n");
}

void CInplaceEdit::DoDblClick(CPoint point)
{
	CClientDC theDC(m_pWnd);
	{
		CDraftContext theContext(&theDC,m_pMapper);
		int pos=CharFromPos(&theContext,point);
		int nStartChar=WordLeft(pos);
		int nEndChar=WordRight(pos);
		SetSel(nStartChar,nEndChar,nEndChar);
		Redraw(false);
		m_pWnd->SetCapture();
		MSG msg;
		while(1)
		{
			GetMessage(&msg,NULL,0,0);
			m_pWnd->ScreenToClient(&msg.pt);
			switch(msg.message)
			{
				case WM_MOUSEMOVE:
					{
						CPoint pt(msg.pt);
						int newPos=CharFromPos(&theContext,pt+m_ptOrg);
						int nStart,nEnd;
						if(newPos<m_nStartChar || newPos>m_nEndChar)
						{
							if(newPos<m_nStartChar)
							{
								nStart=WordLeft(newPos);
								nEnd=nEndChar;
							}
							else
							{
								nStart=nStartChar;
								nEnd=WordRight(newPos);
							}
						}
						else
						{
							if(newPos<nStartChar)
							{
								nStart=WordLeft(newPos);
								nEnd=m_nEndChar;
							}
							else if(newPos>nEndChar)
							{
								nStart=m_nStartChar;
								nEnd=WordRight(newPos);
							}
							else
							{
								nStart=nStartChar;
								nEnd=nEndChar;
							}
						}
						if(nStart!=m_nStartChar || nEnd!=m_nEndChar)
						{
							SetSel(nStart,nEnd,nEnd);
							Redraw(false);
						}
					}
					break;
				case WM_LBUTTONUP:
					ReleaseCapture();
					return;
			}
		}
	}
}

void CInplaceEdit::MouseActivate(CPoint point)
{
	CClientDC theDC(m_pWnd);
	{
		CDraftContext theContext(&theDC,m_pMapper);
		int pos=CharFromPos(&theContext,point);
		bool bCaretOnly=m_nStartChar==m_nEndChar;
		SetSel(pos,pos);
		Redraw(bCaretOnly);
	}
}

void CInplaceEdit::DoClick(CPoint point)
{
	CClientDC theDC(m_pWnd);
	{
		CDraftContext theContext(&theDC,m_pMapper);
		int pos=CharFromPos(&theContext,point);
		bool bCaretOnly=m_nStartChar==m_nEndChar;
		SetSel(pos,pos);
		Redraw(bCaretOnly);
		m_pWnd->SetCapture();
		MSG msg;
		while(1)
		{
			GetMessage(&msg,NULL,0,0);
			m_pWnd->ScreenToClient(&msg.pt);
			switch(msg.message)
			{
				case WM_MOUSEMOVE:
					{
						CPoint pt(msg.pt);
						int newPos=CharFromPos(&theContext,pt+m_ptOrg);
						if(newPos!=pos)
						{
							int nStart,nEnd;
							if(newPos<pos)
							{
								nStart=newPos;
								nEnd=pos;
							}
							else
							{
								nStart=pos;
								nEnd=newPos;
							}
							SetSel(nStart,nEnd,newPos);
							Redraw(false);
						}
					}
					break;
				case WM_LBUTTONUP:
					ReleaseCapture();
					return;
			}
		}
	}
}

void CInplaceEdit::GetStyledText(CStyledText& st)
{
	st.Copy(m_StyledText);
}

void CInplaceEdit::SetStyledText(const CStyledText& st)
{
	m_StyledText.Copy(st);
}

void CInplaceEdit::CalcBoundRect(CDraftContext* pDC, bool bRelaxedBox)
{
	if (m_bAutoAdjustBoundRect)
		m_BoundRect.SetRect(0,0,0,0);
	else
	{
		m_BoundRect.left = m_editRect.left;
		m_BoundRect.right = m_editRect.right;
		m_BoundRect.top = 0;
		m_BoundRect.bottom =0;
	}

	int pos=m_StyledText.GetFirstTextPart();
	int lastBaseLine=-1;
   double dLineSpacing;
	int dLastDescent=0;
	while(pos!=-1)
	{
		double baseAscent,dDescent,width;
		pos=CalcLineExtent(pDC,pos,dLineSpacing,baseAscent, dDescent,width, bRelaxedBox);
		if(lastBaseLine==-1)
		{
			lastBaseLine=dLineSpacing;
		}
		else
		{
			int n=dLastDescent+dLineSpacing;
			switch(m_nLineSpace)
			{
				case kSingleSpace: break;
				case k1_5Space: n=(n*3)/2; break;
				case kDoubleSpace: n=n*2; break;
				default:
					ASSERT(FALSE);
			}
			lastBaseLine+=n;
		}
		dLastDescent=dDescent;
		if(width>m_BoundRect.right) m_BoundRect.right=width;
	}
	if(lastBaseLine==-1)
	{
		pDC->SelectFont(m_DefStyle.m_LogFont);
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		pDC->UnselectFont();
		m_BoundRect.right=1;
		m_BoundRect.bottom=tm.tmAscent+tm.tmDescent+tm.tmExternalLeading;
	}
	else
	{
		m_BoundRect.bottom=ceil(lastBaseLine+dLastDescent);
	}

	if (m_bAutoAdjustBoundRect)
	{
		int cx;
		switch (m_nAlign)
		{
		case kAlignLeft: cx = m_ptOrigin.x;
			break;
		case kAlignCenter: cx = m_ptOrigin.x - m_BoundRect.Width() / 2;
			break;
		case kAlignRight: cx = m_ptOrigin.x - m_BoundRect.Width();
			break;
		}

		m_BoundRect.OffsetRect(cx, m_ptOrigin.y);
	}
}

void CInplaceEdit::SetAutoAdjustBoundRect(CPoint ptOrigin)
{
	m_ptOrigin=ptOrigin;
	m_bAutoAdjustBoundRect=true;
}

bool CInplaceEdit::CanCopyCut()
{
	return m_nStartChar!=m_nEndChar;
}

UINT CInplaceEdit::gClipboardFormat;

void CInplaceEdit::RegisterClipboardFormat()
{
	gClipboardFormat=::RegisterClipboardFormat("PCDraft_InplaceEdit_StyledText");
}

bool CInplaceEdit::CanPaste()
{
	return ::IsClipboardFormatAvailable(gClipboardFormat) || ::IsClipboardFormatAvailable(CF_TEXT);
}

bool CInplaceEdit::DoCopy()
{
	if(m_pWnd->OpenClipboard() && ::EmptyClipboard())
	{
		CStyledText st;
		CString t;
		LPTSTR str = m_StyledText.GetTextPtr();
		for(int i=m_nStartChar;i<m_nEndChar;)
		{
			TCHAR ch;
			CStyledTextStyle style;
			int nCharLen = _tcsinc(str) - str;
			if (nCharLen == 1)
			{
				m_StyledText.GetAt(i,ch,style);
				st.Add(ch,style);
				if(ch==_T('\n'))
					t+=_T('\r');
				t+=ch;
				i++;
			}
			else
			{
				ASSERT(nCharLen == 2);

				m_StyledText.GetAt(i,ch,style);
				st.Add(ch,style);
				t+=ch;
				i++;
				
				m_StyledText.GetAt(i,ch,style);
				st.Add(ch,style);
				t+=ch;
				i++;
				
			}
         str += nCharLen;
		}
		CMemFile mf;
		CArchive ar(&mf,CArchive::store);
		st.Serialize(ar);
		ar.Close();
		DWORD len=mf.GetLength();
		BYTE* data=mf.Detach();

		HANDLE hClip=GlobalAlloc(GMEM_DDESHARE,len);
		CopyMemory(GlobalLock(hClip),data,len);
		GlobalUnlock(hClip);
		SetClipboardData(gClipboardFormat,hClip);

		hClip=GlobalAlloc(GMEM_DDESHARE,t.GetLength()+1);
		CopyMemory(GlobalLock(hClip),(LPCTSTR)t,t.GetLength()+1);
		GlobalUnlock(hClip);
		free(data);
		SetClipboardData(CF_TEXT,hClip);

		::CloseClipboard();
		return true;
	}
	return false;
}

void CInplaceEdit::DoCut()
{
	SaveUndo();
	if(DoCopy())
		ReplaceSel("");
}

void CInplaceEdit::DoPaste()
{
	SaveUndo();
	if(m_pWnd->OpenClipboard())
	{
		HANDLE hClip=GetClipboardData(gClipboardFormat);
		if(hClip)
		{
			CStyledText st;
			CMemFile mf((BYTE*)GlobalLock(hClip),GlobalSize(hClip));
			CArchive ar(&mf,CArchive::load);
			st.Serialize(ar);
			GlobalUnlock(hClip);
			ReplaceSel(st);
		}
		else
		{
         hClip = GetClipboardData(CF_UNICODETEXT);
         if (hClip)
         {
            LPCWSTR p = (LPCWSTR)GlobalLock(hClip);
            CString t = get_cp(p, CP_ACP);
            GlobalUnlock(hClip);
            ReplaceSel(t);
         }
         else
         {
            hClip = GetClipboardData(CF_TEXT);
            if (hClip)
            {
               CString t;
               LPCTSTR p = (LPCTSTR)GlobalLock(hClip);
               t = p;
               //while (*p != _T('\0'))
               //{
               //   int nCharLen = _tcsinc(p) - p;
               //   if (nCharLen == 1)
               //   {
               //      TCHAR ch = *p++;
               //      if (ch != _T('\r'))
               //         t += ch;
               //   }
               //   else
               //   {
               //      ASSERT(nCharLen == 2);
               //      t += *p++;
               //      t += *p++;
               //   }
               //}
               GlobalUnlock(hClip);
               ReplaceSel(t);
            }
         }
		}
		::CloseClipboard();
	}
}

void CInplaceEdit::SetOrg(CPoint ptOrg)
{
	m_ptOrg=ptOrg;
}

void CInplaceEdit::SelectAll()
{
	m_nStartChar=0;
	m_nCaretChar=m_nEndChar=m_StyledText.GetSize();
	Redraw(false);
}

void CInplaceEdit::SaveState(CMemFile *pFile)
{
	CArchive ar(pFile,CArchive::store);
	ar << m_BoundRect;
	ar << m_nAlign << m_nCase << m_nVAlign << m_nLineSpace;
	ar << m_nStartChar << m_nEndChar << m_nCaretChar;
	ar.Write(&m_DefStyle,sizeof(m_DefStyle));
	m_StyledText.Serialize(ar);
	ar.Close();
}

void CInplaceEdit::RestoreState(CMemFile *pFile)
{
   //	InvalidateBounds();
   CRect rc = m_BoundRect;
   rc.OffsetRect(-m_ptOrg.x, -m_ptOrg.y);
   rc.InflateRect(3, 3);
   m_pWnd->InvalidateRect(&rc);
   //	InvalidateBounds();
   pFile->SeekToBegin();
   CArchive ar(pFile, CArchive::load);
   ar >> m_BoundRect;
   ar >> m_nAlign >> m_nCase >> m_nVAlign >> m_nLineSpace;
   ar >> m_nStartChar >> m_nEndChar >> m_nCaretChar;
   ar.Read(&m_DefStyle, sizeof(m_DefStyle));
   m_StyledText.Serialize(ar);
   ar.Close();
   
   if (!m_bImeComposition)
   {

      Redraw(false);

   }

}

void CInplaceEdit::ClearUndo()
{
	if(m_pUndo)
	{
		delete m_pUndo;
		m_pUndo=NULL;
	}
	if(m_pRedo)
	{
		delete m_pRedo;
		m_pRedo=NULL;
	}
	m_bRedo=false;
	m_bTyping=false;
}

void CInplaceEdit::SaveUndo()
{
	ClearUndo();
	m_pUndo=new CMemFile;
	SaveState(m_pUndo);
}

void CInplaceEdit::DoUndoRedo() 
{
	if(m_bRedo)	RestoreState(m_pRedo);
	else
	{
		if(m_pRedo==NULL)
		{
			m_pRedo=new CMemFile;
			SaveState(m_pRedo);
		}
		RestoreState(m_pUndo);
		m_bTyping=false;
	}
	m_bRedo=!m_bRedo;
}

int CInplaceEdit::CanUndoRedo() 
{
	return m_pUndo?(m_bRedo?IDS_REDO:IDS_UNDO):0;
}

#ifndef _UNICODE
void CInplaceEdit::DoImeTyping(LPCTSTR lpstr)
{
	if(!m_bTyping)
	{
		SaveUndo();
		m_bTyping=true;
	}
	ReplaceSel(lpstr);
}
#endif

