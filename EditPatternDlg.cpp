// EditPatternDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "EditPatternDlg.h"
#include "PCDraftDoc.h"
#include "CustomMenus.h"
#include "ChooseColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditPatternDlg dialog

CEditPatternDlg::CEditPatternDlg(HBITMAP *phDib,CWnd* pParent /*=NULL*/)
	: CDialog(CEditPatternDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditPatternDlg)
	//}}AFX_DATA_INIT
	m_phPattern=phDib;
	m_hUndo=NULL;
	m_hEditPattern=NULL;
	m_nSize=0;
	m_nTool=IDC_TOOL_PEN;
	m_Color=RGB(0,0,0);
}

void CEditPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPatternDlg)
	DDX_Control(pDX, IDC_COLOR, m_color);
	DDX_Control(pDX, IDC_TOOL_PEN, m_toolPen);
	DDX_Control(pDX, IDC_PREVIEW, m_preview);
	DDX_Control(pDX, IDC_SCALE, m_scale);
	DDX_Control(pDX, IDC_INVERT, m_invert);
	DDX_Control(pDX, IDC_FLIP_VERTICAL, m_flipVert);
	DDX_Control(pDX, IDC_FLIP_HORIZONTAL, m_flipHorz);
	DDX_Control(pDX, IDC_BW, m_bw);
	DDX_Control(pDX, IDC_REVERT, m_revert);
	DDX_Control(pDX, IDC_UNDO, m_undo);
//	DDX_Control(pDX, IDC_EDIT, m_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditPatternDlg, CDialog)
	//{{AFX_MSG_MAP(CEditPatternDlg)
	ON_BN_CLICKED(IDC_BW, OnBlackWhite)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_FILL, OnFill)
	ON_BN_CLICKED(IDC_FLIP_HORIZONTAL, OnFlipHorizontal)
	ON_BN_CLICKED(IDC_FLIP_VERTICAL, OnFlipVertical)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	ON_BN_CLICKED(IDC_OTHER, OnOther)
	ON_BN_CLICKED(IDC_PATTERN, OnPattern)
	ON_BN_CLICKED(IDC_REVERT, OnRevert)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_TOOL_PEN, IDC_TOOL_HAND, OnToolsRange)
	ON_COMMAND_RANGE(IDC_PATTERN_SIZE8, IDC_PATTERN_SIZE32, OnSizeRange)
   ON_STN_CLICKED(IDC_EDIT_PLACEHOLDER,&CEditPatternDlg::OnStnClickedEditPlaceholder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPatternDlg message handlers

BOOL CEditPatternDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_edit.SetParentDlg(this);

	m_invert.SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_INVERT,0,NULL,0));
	m_flipHorz.SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_FLIPH,0,NULL,0));
	m_flipVert.SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_FLIPV,0,NULL,0));

	m_toolPen.SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_PEN,0,NULL,0));
	((CButton*)GetDlgItem(IDC_TOOL_THUMB))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_THUMB,0,NULL,0));
	((CButton*)GetDlgItem(IDC_TOOL_BUCKET))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_BUCKET,0,NULL,0));
	((CButton*)GetDlgItem(IDC_TOOL_HAND))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PE_HAND,0,NULL,0));

	m_toolPen.SetCheck(1);
	m_scale.SetCheck(1);

	CRect rc;
	GetDlgItem(IDC_EDIT_PLACEHOLDER)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_edit.Create(AfxRegisterWndClass(CS_PARENTDC|CS_HREDRAW|CS_VREDRAW,NULL,NULL,NULL),NULL,WS_CHILD|WS_VISIBLE,rc,this,IDC_EDIT);

	OnRevert();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEditPatternDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);

	switch(lpDIS->CtlID)
	{
	case IDC_PREVIEW:
		{
			BITMAP bm;
			GetObject(m_hEditPattern,sizeof(BITMAP),&bm);

			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			HGDIOBJ hOldBmp=memDC.SelectObject(m_hEditPattern);
			CGDIBackColor bc(pDC,RGB(255,255,255));
			CGDITextColor tc(pDC,RGB(0,0,0));

			for(int x=rc.left;x<rc.right;x+=bm.bmWidth)
			{
				for(int y=rc.top;y<rc.bottom;y+=bm.bmHeight)
				{
					pDC->BitBlt(x,y,bm.bmWidth,bm.bmHeight,&memDC,0,0,SRCCOPY);
				}
			}
			memDC.SelectObject(hOldBmp);
		}
		break;
	case IDC_COLOR:
		{
			CGDIBackColor(pDC,m_Color);
			pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
		}
		break;
	default:
		CDialog::OnDrawItem(nIDCtl,lpDIS);
		break;
	}
}

void CEditPatternDlg::CopyDIB(HBITMAP& hDstDib,HBITMAP& hSrcDib,CopyDIBParams *pp)
{
	DIBSECTION ds;
	GetObject(hSrcDib,sizeof(DIBSECTION),&ds);
	if(pp)
	{
		if(pp->nFlags&CDPF_SIZE)
		{
			ds.dsBmih.biWidth=pp->nNewSize;
			ds.dsBmih.biHeight=pp->nNewSize;
			ds.dsBmih.biSizeImage=0;
			if(m_scale.GetCheck())
			{
				pp->nFlags|=CDPF_COLORS;
				pp->bNewBW=false;
			}
		}
		if(pp->nFlags&CDPF_COLORS)
		{
			ds.dsBmih.biBitCount=pp->bNewBW?1:32;
			ds.dsBmih.biSizeImage=0;
		}
	}
	if(ds.dsBmih.biBitCount==1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
	LPVOID lpBits;
	HDC hdc=::GetDC(NULL);
	hDstDib=CreateDIBSection(hdc,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
	if(pp)
	{
		HDC hSrcDC,hDstDC;
		hSrcDC=CreateCompatibleDC(hdc);
		hDstDC=CreateCompatibleDC(hdc);
		HGDIOBJ hSrcOldBmp=SelectObject(hSrcDC,hSrcDib);
		HGDIOBJ hDstOldBmp=SelectObject(hDstDC,hDstDib);
		if(((pp->nFlags&CDPF_SIZE) && m_scale.GetCheck()) || (pp->nFlags&(CDPF_FLIPV|CDPF_FLIPH)))
		{
			SetStretchBltMode(hDstDC,(pp->nFlags&CDPF_SIZE)?HALFTONE:COLORONCOLOR);
			StretchBlt(hDstDC,0,0,ds.dsBmih.biWidth,ds.dsBmih.biHeight,hSrcDC,pp->nFlags&CDPF_FLIPH?m_nSize-1:0,pp->nFlags&CDPF_FLIPV?m_nSize-1:0,pp->nFlags&CDPF_FLIPH?-m_nSize:m_nSize,pp->nFlags&CDPF_FLIPV?-m_nSize:m_nSize,pp->nFlags&CDPF_ROP?pp->dwROP:SRCCOPY);
		}
		else
		{
			for(int x=0;x<ds.dsBmih.biWidth;x+=m_nSize)
			{
				for(int y=0;y<ds.dsBmih.biHeight;y+=m_nSize)
				{
					BitBlt(hDstDC,x,y,m_nSize,m_nSize,hSrcDC,0,0,pp->nFlags&CDPF_ROP?pp->dwROP:SRCCOPY);
				}
			}
		}
		SelectObject(hSrcDC,hSrcOldBmp);
		SelectObject(hDstDC,hDstOldBmp);
		DeleteDC(hDstDC);
		DeleteDC(hSrcDC);
	}
	else
	{
		if(lpBits) memcpy(lpBits,ds.dsBm.bmBits,ds.dsBmih.biSizeImage);
	}
	::ReleaseDC(NULL,hdc);
}

void CEditPatternDlg::OnToolsRange(UINT nID)
{
	if(m_nTool==IDC_TOOL_THUMB && m_nTool==int(nID)) m_edit.UpdateColor();
	else m_nTool=nID;
}

void CEditPatternDlg::OnSizeRange(UINT nID)
{
	SaveUndo(false);

	CopyDIBParams cp;
	cp.nFlags=CDPF_SIZE;
	switch(nID)
	{
	case IDC_PATTERN_SIZE8:
		cp.nNewSize=8;
		break;
	case IDC_PATTERN_SIZE16:
		cp.nNewSize=16;
		break;
	case IDC_PATTERN_SIZE32:
		cp.nNewSize=32;
		break;
	}
	CopyDIB(m_hEditPattern,m_hUndo,&cp);
	UpdateEditInfo();
}

void CEditPatternDlg::OnBlackWhite() 
{
	m_bw.EnableWindow(FALSE);
	SaveUndo(false);
	CopyDIBParams cp;
	cp.nFlags=CDPF_COLORS;
	cp.bNewBW=true;
	CopyDIB(m_hEditPattern,m_hUndo,&cp);
	UpdateEditInfo();
	SetColor(RGB(0,0,0));
}

void CEditPatternDlg::OnClear() 
{
	SaveUndo(false);
	CopyDIBParams cp;
	cp.nFlags=CDPF_ROP;
	cp.dwROP=WHITENESS;
	CopyDIB(m_hEditPattern,m_hUndo,&cp);
	UpdateEditInfo();
//	SetColor(RGB(0,0,0));
}

void CEditPatternDlg::OnFill() 
{
	SaveUndo();

	Colorize(m_Color);
	
	CRect rc(0,0,m_nSize,m_nSize);
	CDC memDC;
	memDC.CreateCompatibleDC(NULL);
	HGDIOBJ hOldBmp=memDC.SelectObject(m_hEditPattern);
	CGDIBackColor bc(&memDC,m_Color);
	memDC.ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
	memDC.SelectObject(hOldBmp);
	UpdateEditInfo();
}

void CEditPatternDlg::OnFlipVertical() 
{
	SaveUndo(false);
	CopyDIBParams cp;
	cp.nFlags=CDPF_FLIPV;
	CopyDIB(m_hEditPattern,m_hUndo,&cp);
	UpdateEditInfo();
}

void CEditPatternDlg::OnFlipHorizontal() 
{
	SaveUndo(false);
	CopyDIBParams cp;
	cp.nFlags=CDPF_FLIPH;
	CopyDIB(m_hEditPattern,m_hUndo,&cp);
	UpdateEditInfo();
}

void CEditPatternDlg::OnInvert() 
{
	SaveUndo(false);
	CopyDIBParams cp;
	cp.nFlags=CDPF_ROP;
	cp.dwROP=NOTSRCCOPY;
	CopyDIB(m_hEditPattern,m_hUndo,&cp);
	UpdateEditInfo();
}

void CEditPatternDlg::OnColor() 
{
	CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();

	if(pDoc)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CColorMenu theMenu;
		theMenu.Create(&pDoc->m_Palette,IDS_PATTERNCOLOR);
		int cmd=theMenu.Track(this);
		if(cmd>0)
		{
			PALETTEENTRY palEntry;
			pDoc->m_Palette.GetPaletteEntries(cmd-1,1,&palEntry);
			SetColor(RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue));
			m_edit.UpdateColor();
		}
	}
}

void CEditPatternDlg::OnOther() 
{
	CChooseColorDlg theDlg(m_Color,this);
	if(theDlg.DoModal()==IDOK)
	{
		SetColor(theDlg.GetColor());
		m_edit.UpdateColor();
	}
}

void CEditPatternDlg::OnPattern() 
{
	CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();

	if(pDoc)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CPatternOnlyMenu theMenu;
      theMenu.Create(pDoc->m_Patterns,pDoc->m_numDIBPatterns,IDS_FILLPATTERN,ID_PATTERNS_BASE); // Pattern/Gradient - Infinisys Ltd
		int cmd=theMenu.Track(this);
		if(cmd>0)
		{
			SaveUndo();

			HBITMAP hDib=pDoc->m_Patterns[cmd-1].CreateDIBSection();
			BITMAP bm;
			GetObject(hDib,sizeof(BITMAP),&bm);
			Colorize(bm.bmBitsPixel==1?m_Color:RGB(128,128,128));

			CDC memDC;
			memDC.CreateCompatibleDC(NULL);
			HGDIOBJ hOldBmp=memDC.SelectObject(m_hEditPattern);
			CDC mixDC;
			mixDC.CreateCompatibleDC(&memDC);
			HGDIOBJ hOldMixBmp=mixDC.SelectObject(hDib);

			for(int x=0;x<m_nSize;x++)
			{
				int mixX=x%bm.bmWidth;
				for(int y=0;y<m_nSize;y++)
				{
					int mixY=y%bm.bmHeight;
					COLORREF mixColor=mixDC.GetPixel(mixX,mixY);
					if(mixColor!=RGB(255,255,255))
					{
						if(mixColor==RGB(0,0,0)) mixColor=m_Color;
						memDC.SetPixel(x,y,mixColor);
					}
				}
			}

			mixDC.SelectObject(hOldMixBmp);
			memDC.SelectObject(hOldBmp);
			::DeleteObject(hDib);
			UpdateEditInfo();
		}
	}
}

void CEditPatternDlg::OnRevert() 
{
	if(m_hUndo)
	{
		DeleteObject(m_hUndo);
		m_hUndo=NULL;
		m_undo.EnableWindow(FALSE);
	}

	if(m_hEditPattern)
	{
		((CButton*)GetDlgItem(IDC_PATTERN_SIZE8+(m_nSize/16)))->SetCheck(0);
		DeleteObject(m_hEditPattern);
	}

	CopyDIB(m_hEditPattern,*m_phPattern);
	UpdateEditInfo();

	((CButton*)GetDlgItem(IDC_PATTERN_SIZE8+(m_nSize/16)))->SetCheck(1);
}

void CEditPatternDlg::SaveUndo(bool bAsCopy)
{
	if(m_hUndo) DeleteObject(m_hUndo);
	else m_undo.EnableWindow();
	if(!m_revert.IsWindowEnabled()) m_revert.EnableWindow();
	if(bAsCopy) CopyDIB(m_hUndo,m_hEditPattern);
	else m_hUndo=m_hEditPattern;
}

void CEditPatternDlg::OnUndo() 
{
	if(m_hUndo)
	{
		HBITMAP hTemp=m_hEditPattern;
		m_hEditPattern=m_hUndo;
		m_hUndo=hTemp;
		((CButton*)GetDlgItem(IDC_PATTERN_SIZE8+(m_nSize/16)))->SetCheck(0);
		UpdateEditInfo();
		((CButton*)GetDlgItem(IDC_PATTERN_SIZE8+(m_nSize/16)))->SetCheck(1);
	}
}

void CEditPatternDlg::UpdateEditInfo()
{
	BITMAP bm;
	GetObject(m_hEditPattern,sizeof(BITMAP),&bm);
	m_bIsBW=(bm.bmBitsPixel==1);
	ASSERT(bm.bmWidth==bm.bmHeight);
	m_nSize=bm.bmWidth;
	m_preview.Invalidate();
	m_edit.UpdateEditInfo();
	m_bw.EnableWindow(!m_bIsBW);
}

void CEditPatternDlg::OnOK() 
{
	DeleteObject(*m_phPattern);
	*m_phPattern=m_hEditPattern;

	CDialog::OnOK();
}

void CEditPatternDlg::Colorize(COLORREF color)
{
	if(m_bIsBW && color!=RGB(0,0,0) && color!=RGB(255,255,255))
	{
		HBITMAP hTemp=m_hEditPattern;
		CopyDIBParams cp;
		cp.nFlags=CDPF_COLORS;
		cp.bNewBW=false;
		CopyDIB(m_hEditPattern,hTemp,&cp);
		DeleteObject(hTemp);
		m_bw.EnableWindow();
		m_bIsBW=false;
	}
}


void CEditPatternDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;

	m_color.GetClientRect(&rc);
	m_color.ClientToScreen(&rc);
	ScreenToClient(&rc);

	CGDIBackColor bc(&dc,m_Color);
	dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
}

void CEditPatternDlg::SetColor(COLORREF color)
{
	m_Color=color;
	CRect rc;
	m_color.GetClientRect(&rc);
	m_color.ClientToScreen(&rc);
	ScreenToClient(&rc);
	InvalidateRect(&rc,FALSE);
}

BOOL CEditPatternDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(pMsg->wParam==0x5A && GetAsyncKeyState(VK_CONTROL)<0)
		{
			OnUndo();
			return TRUE;
		}
		break;

	case WM_SYSKEYDOWN:
		if(pMsg->wParam==0x12)
		{
			CPoint pt;
			GetCursorPos(&pt);
			if(WindowFromPoint(pt)==&m_edit)
			{
				m_edit.OnSetCursor(this,HTCLIENT,WM_MOUSEMOVE);
				return TRUE;
			}
		}
		break;

	case WM_SYSKEYUP:
		if(pMsg->wParam==0x12)
		{
			CPoint pt;
			GetCursorPos(&pt);
			if(WindowFromPoint(pt)==&m_edit)
			{
				m_edit.OnSetCursor(this,HTCLIENT,WM_MOUSEMOVE);
				return TRUE;
			}
		}
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}



void CEditPatternDlg::OnStnClickedEditPlaceholder()
{
   // TODO: Add your control notification handler code here
}
