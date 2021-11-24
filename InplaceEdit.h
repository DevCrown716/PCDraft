// InplaceEdit.h: interface for the CInplaceEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPLACEEDIT_H__F1FDFA4D_23C1_11D3_9542_008048DA0F68__INCLUDED_)
#define AFX_INPLACEEDIT_H__F1FDFA4D_23C1_11D3_9542_008048DA0F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "styledtext.h"
#include "lpoint.h"
#include "pointf.h"

struct TextParams;
class CMapper;
class CDraftContext;

class CInplaceEdit : public CObject  
{
public:

   enum_composition_state     m_ecompositionstate; // Inline Japanese - Infinisys Ltd.
   int                        m_nCompositionStartChar; // Inline Japanese - Infinisys Ltd.
   int                        m_nCompositionEndChar; // Inline Japanese - Infinisys Ltd.

   bool m_bImeComposition; // Infinisys Ltd Japanese Text
   CRect m_rectCaret; // Infinisys Ltd Japanese Text
	void SelectAll();
	CInplaceEdit();
	virtual ~CInplaceEdit();

	void SetBackColor(COLORREF rgbBackColor) { m_rgbBackColor=rgbBackColor; }
	void CalcTrueBoundRect(LongRect &lrc);
	void SetOrg(CPoint ptOrg);
	void SetWindow(CWnd* pWnd);
	void SetMapper(CMapper* pMapper);
	void SetBoundRect(const CRect &rc);
	void GetBoundRect(CRect &rc);
	void SetTextParams(const TextParams& tp);
	void GetTextParams(TextParams& tp) const;
	void GetCaretTextParams(TextParams& tp) const;
	void SetDefColor(COLORREF color);
	void SetDefFont(const LOGFONT& lf);
	void GetDefFont(LOGFONT& lf);
	void SetLineSpace(int nLineSpace);
	void SetTextAlign(int nAlign);
	void SetCase(int nCase);
	void GetSel(int &nStartChar,int &nEndChar);
	int GetCaretPos() const { return m_nCaretChar; }
	void SetSel(int nStartChar,int nEndChar,int nCaretChar=-1);
	void ReplaceSel(LPCTSTR pText, enum_composition_state ecompositionstate = e_composition_state_none);
	void ReplaceSelFont(const LOGFONT& lf);
	void ReplaceSelColor(COLORREF color);
	void ReplaceSel(const CStyledText& st);
	int CalcLineExtent(CDraftContext* pDC,int pos,double & lineAscent,double &baseAscent,double &baseDescent,double &width, bool bRelaxed);
   void DrawBackground(CDraftContext* pDC); // Inline Fast Text - Infinisys Ltd.
	void Draw(CDraftContext* pDC);
	int CharFromPos(CDraftContext* pDC,pointf pt);
	void GetCaretDim(CDraftContext* pDC,pointf& pt,CSize& size);
	void OnSetFocus();
	void OnKillFocus();
	void Redraw(bool bCaretOnly);
	void GoHome();
	void GoEnd();
	void GoUp();
	void GoDown();
	void GoLeft();
	void GoRight();
	void DoClick(CPoint point);
	void DoDblClick(CPoint point);
	void DoDelete();
	void DoBackSpace();
	void DoTyping(TCHAR ch);
	void DoImeTyping(LPCTSTR lpstr);
	void DoReturn();
	void GetStyledText(CStyledText& st);
	void SetStyledText(const CStyledText& st);
	void CalcBoundRect(CDraftContext* pDC, bool bRelaxedBox);
	void SetAutoAdjustBoundRect(CPoint ptOrigin);
	static void RegisterClipboardFormat();
	bool CanCopyCut();
	bool CanPaste();
	bool DoCopy();
	void DoCut();
	void DoPaste();
	void CommonCaretInc(int n);
	void CommonCaretDec(int n);
	int WordLeft(int nChar);
	int WordRight(int nChar);
	void MouseActivate(CPoint point);

	void DoUndoRedo();
	int CanUndoRedo();
	void ClearUndo();
   void RestoreState(CMemFile* pFile);
   void SaveState(CMemFile* pFile);
protected:
	void SaveUndo();

protected:
	static UINT gClipboardFormat;
	bool m_bAutoAdjustBoundRect;
	CPoint m_ptOrg;
	CPoint m_ptOrigin;
	CWnd* m_pWnd;
	CMapper* m_pMapper;
	CRect	m_BoundRect;
	CRect	m_editRect;
	CStyledTextStyle m_DefStyle;
	int m_nLineSpace;
	int m_nAlign;
	char m_nVAlign;
	short m_nCase;
	CStyledText m_StyledText;
	int m_nStartChar,m_nEndChar,m_nCaretChar;
   double m_dCaretLineSpacing;
	COLORREF m_rgbBackColor;

	//	undo/redo
	CMemFile* m_pUndo;
	CMemFile* m_pRedo;
	bool m_bRedo;
	bool m_bTyping;
};

#endif // !defined(AFX_INPLACEEDIT_H__F1FDFA4D_23C1_11D3_9542_008048DA0F68__INCLUDED_)
