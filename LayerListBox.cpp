// LayerListBox.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "LayerListBox.h"

#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerListBox

CLayerListBox::CLayerListBox()
{
	m_hActiveLayerIcon=AfxGetApp()->LoadIcon(IDI_ACTIVE_LAYER);
	m_hVisibleLayerIcon=AfxGetApp()->LoadIcon(IDI_VISIBLE_LAYER);
	m_cyText=0;
	m_pActiveLayer=NULL;
}

CLayerListBox::~CLayerListBox()
{
}


BEGIN_MESSAGE_MAP(CLayerListBox, CListBox)
	//{{AFX_MSG_MAP(CLayerListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerListBox message handlers

void CLayerListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	if(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
	{
		int nBackColorIndex,nTextColorIndex;
		if((lpDrawItemStruct->itemState & ODS_SELECTED)!=0)
		{
			nTextColorIndex=COLOR_HIGHLIGHTTEXT;
			nBackColorIndex=COLOR_HIGHLIGHT;
		}
		else
		{
			nTextColorIndex=COLOR_WINDOWTEXT;
			nBackColorIndex=COLOR_WINDOW;
		}
		CGDITextColor theTextColor(pDC,::GetSysColor(nTextColorIndex));
		CGDIBackColor theBackColor(pDC,::GetSysColor(nBackColorIndex));
		int cyItem=lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top;
		CLayer* pLayer=(CLayer*)lpDrawItemStruct->itemData;
		if(m_cyText==0)
		{
			TEXTMETRIC tm;
			pDC->GetTextMetrics(&tm);
			m_cyText=tm.tmHeight;
		}
		pDC->ExtTextOut(lpDrawItemStruct->rcItem.left+cyItem,lpDrawItemStruct->rcItem.top+(cyItem-m_cyText)/2,ETO_OPAQUE,&lpDrawItemStruct->rcItem,pLayer->m_strName ,NULL);
		HICON hIcon=(pLayer==m_pActiveLayer)?m_hActiveLayerIcon:((pLayer->m_bHidden)?NULL:m_hVisibleLayerIcon);
		if(hIcon)
			pDC->DrawIcon(lpDrawItemStruct->rcItem.left+(cyItem-9)/2,lpDrawItemStruct->rcItem.top+(cyItem-8)/2,hIcon);
	}
	if((lpDrawItemStruct->itemAction & ODA_FOCUS)!=0)
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
}

void CLayerListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
}
