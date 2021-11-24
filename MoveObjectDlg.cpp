// MoveObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "MoveObjectDlg.h"

#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveObjectDlg dialog


CMoveObjectDlg::CMoveObjectDlg(CPCDraftView* pView,CWnd* pParent /*=NULL*/)
	: CDialog(CMoveObjectDlg::IDD, pParent)
{
	m_pView=pView;
	//{{AFX_DATA_INIT(CMoveObjectDlg)
	m_bMoveCopy = FALSE;
	m_nAbsRel = 0;
	//}}AFX_DATA_INIT
}


void CMoveObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveObjectDlg)
	DDX_Control(pDX, IDC_TOLAYER, m_ToLayer);
	DDX_Control(pDX, IDC_PARAM2_VALUE, m_Value2);
	DDX_Control(pDX, IDC_PARAM1_VALUE, m_Value1);
	DDX_Control(pDX, IDC_PARAM2_LABEL, m_label2);
	DDX_Control(pDX, IDC_PARAM1_LABEL, m_label1);
	DDX_Check(pDX, IDC_MOVE_COPY, m_bMoveCopy);
	DDX_Radio(pDX, IDC_ABSOLUTE, m_nAbsRel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CMoveObjectDlg)
	ON_BN_CLICKED(IDC_TOLAYER, OnTolayer)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_APPLY_REVERSE, OnApplyReverse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveObjectDlg message handlers

BOOL CMoveObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CPCDraftDoc* pDoc=m_pView->GetDocument();
	m_LayerPopup.CreatePopupMenu();
	int n=0;
	m_nToLayer=pDoc->m_nActiveLayer;
   m_ToLayer.SetWindowText(get_cp(get_uni(pDoc->GetActiveLayer()->m_strName), CP_ACP));
	int nScale=pDoc->GetActiveLayer()->m_nScale;
	for(int i=0;i<pDoc->m_Layers.GetSize();i++)
	{
		if(pDoc->m_Layers[i]->m_bHidden) continue;
      m_LayerPopup.AppendMenu(pDoc->m_Layers[i]->m_nScale == nScale?(++n,MF_STRING):(MF_STRING | MF_GRAYED | MF_DISABLED),IDC_TOLAYER + i,get_cp(get_uni(pDoc->m_Layers[i]->m_strName), CP_ACP));
		if(pDoc->m_nActiveLayer==i)
		{
			m_nToLayer=i;
		}
	}
	if(n==1) m_ToLayer.EnableWindow(FALSE);

	m_Value1.SetDocument(pDoc);
	m_Value2.SetDocument(pDoc);
	m_Value1.SetValue(kUnitlessValue,m_lptMove.x);
	m_Value2.SetValue(kUnitlessValue,m_lptMove.y);

	GetDlgItem(IDC_APPLY_REVERSE)->EnableWindow(m_nAbsRel!=0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMoveObjectDlg::OnAbsolute() 
{
   //CButton * pbuttonRelative = (CButton *) GetDlgItem(IDC_RELATIVE);
   //pbuttonRelative->SetCheck(0);
	CString strText;
	strText.LoadString(IDS_ABSOLUTEX);
	m_label1.SetWindowText(strText);
	strText.LoadString(IDS_ABSOLUTEY);
	m_label2.SetWindowText(strText);
	m_Value1.SetValue(kUnitlessValue,m_lptMove.x);
	m_Value2.SetValue(kUnitlessValue,m_lptMove.y);
	GetDlgItem(IDC_APPLY_REVERSE)->EnableWindow(FALSE);
}

void CMoveObjectDlg::OnRelative() 
{
   //CButton * pbuttonRelative = (CButton *)GetDlgItem(IDC_ABSOLUTE);
   //pbuttonRelative->SetCheck(0);

	CString strText;
	strText.LoadString(IDS_RELATIVEX);
	m_label1.SetWindowText(strText);
	strText.LoadString(IDS_RELATIVEY);
	m_label2.SetWindowText(strText);
	m_Value1.SetValue(kUnitlessValue,0);
	m_Value2.SetValue(kUnitlessValue,0);
	GetDlgItem(IDC_APPLY_REVERSE)->EnableWindow(TRUE);
}

void CMoveObjectDlg::CalcMoveParams()
{
	UpdateData();

	if(m_nAbsRel)
	{
		m_Value1.GetValue(m_lptMove.x);
		m_Value2.GetValue(m_lptMove.y);
	}
	else
	{
		WrFixed fValue;
		m_Value1.GetValue(fValue);
		m_lptMove.x=fValue-m_lptMove.x;
		m_Value2.GetValue(fValue);
		m_lptMove.y=fValue-m_lptMove.y;
	}
}

void CMoveObjectDlg::OnOK() 
{
	CalcMoveParams();
	CDialog::OnOK();
}

void CMoveObjectDlg::OnTolayer() 
{
#ifndef _PERSONAL_EDITION
	CPoint pt;
	GetCursorPos(&pt);
	int nCmd=m_LayerPopup.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTBUTTON|TPM_LEFTALIGN,pt.x,pt.y,this);
	if(nCmd)
	{
		m_nToLayer=nCmd-IDC_TOLAYER;
		m_ToLayer.SetWindowText(get_cp(get_uni(m_pView->GetDocument()->m_Layers[m_nToLayer]->m_strName), CP_ACP));
	}
#endif
}

void CMoveObjectDlg::OnApply() 
{
	CalcMoveParams();
	m_pView->DoMoveObject(m_lptMove,m_bMoveCopy,m_nToLayer);
	m_pView->DoCalcMovePt(m_lptMove);
}

void CMoveObjectDlg::OnApplyReverse() 
{
	CalcMoveParams();
	m_lptMove.x=-m_lptMove.x;
	m_lptMove.y=-m_lptMove.y;
	m_pView->DoMoveObject(m_lptMove,m_bMoveCopy,m_nToLayer);
	m_pView->DoCalcMovePt(m_lptMove);
}
