// ReportFormatsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ReportFormatsDlg.h"
#include "PCDraftDoc.h"
#include "LoadReportDlg.h"
#include "MDReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportFormatsDlg dialog


CReportFormatsDlg::CReportFormatsDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CReportFormatsDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CReportFormatsDlg)
	//}}AFX_DATA_INIT
}


void CReportFormatsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportFormatsDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportFormatsDlg, CDialog)
	//{{AFX_MSG_MAP(CReportFormatsDlg)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportFormatsDlg message handlers

BOOL CReportFormatsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	for(int i=0;i<m_pDoc->m_Reports.GetSize();i++)
	{
		CMDReport* pReport=m_pDoc->m_Reports[i];
		m_List.AddString(pReport->m_sTitle);
	}
	if(m_List.GetCount())
	{
		m_List.SetCurSel(0);
	}
	else m_List.EnableWindow(FALSE);
	OnSelchangeList();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CReportFormatsDlg::OnNew() 
{
	CMDReport* pReport=m_pDoc->AddReport();
	if(pReport)
		AddReport(pReport);
}

void CReportFormatsDlg::OnSelchangeList() 
{
	int sel=m_List.GetCurSel();

	if(sel==LB_ERR)
	{
		m_Name.SetWindowText("");
		m_Name.EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else
	{
		CString str;
		m_List.GetText(sel,str);
		m_Name.EnableWindow();
		m_Name.SetWindowText(str);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);
}

void CReportFormatsDlg::OnDelete() 
{
	CString q;
	int sel=m_List.GetCurSel();
	q.Format(IDS_DELETE_REPORT,m_pDoc->m_Reports[sel]->m_sTitle);
	if(AfxMessageBox(q,MB_YESNO|MB_DEFBUTTON2|MB_ICONWARNING)==IDYES)
	{
		m_pDoc->DeleteReport(sel);
		m_List.DeleteString(sel);
		m_Name.SetWindowText("");
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);
	}
}

void CReportFormatsDlg::OnRename() 
{
	int sel=m_List.GetCurSel();
	CMDReport* pReport=m_pDoc->m_Reports[sel];

	CString sName;
	m_Name.GetWindowText(sName);
	if(sName!=pReport->m_sTitle)
	{
		if(m_pDoc->CheckReportName(sName))
		{
			pReport->m_sTitle=sName;
			m_List.SetRedraw(FALSE);
			m_List.DeleteString(sel);
			m_List.InsertString(sel,pReport->m_sTitle);
			m_List.SetCurSel(sel);
			m_List.SetRedraw();
			m_List.UpdateWindow();
			GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);
			m_pDoc->UpdateAllViews(NULL,kReportTitleChanged);
		}
		else AfxMessageBox(IDS_ALREADY_TAKEN);
	}
}

void CReportFormatsDlg::OnChangeName() 
{
	GetDlgItem(IDC_RENAME)->EnableWindow(m_List.GetCurSel()>=0 && m_Name.GetWindowTextLength()>0);
}

void CReportFormatsDlg::OnOK() 
{
	m_nSelReport=m_List.GetCurSel();
	CDialog::OnOK();
}

void CReportFormatsDlg::OnLoad() 
{
	CString strFilter;
	strFilter.LoadString(IDS_LOADREPORTFILTERS);
	CFileDialog theDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,strFilter);
//	theDlg.m_ofn.nFilterIndex=m_pDoc->m_bSaveAsMacDraft?3:1;
	if(theDlg.DoModal()==IDOK)
	{
		CPCDraftDoc* pNewDoc=new CPCDraftDoc;
		BOOL bResult;
		if(theDlg.GetFileExt().CompareNoCase("drf")==0 || theDlg.GetFileExt().CompareNoCase("drs")==0)
			bResult=pNewDoc->OnOpenDocument(theDlg.GetPathName());
		else // assume MacDraft
			bResult=pNewDoc->OnNewDocument() && pNewDoc->LoadMacDraft(theDlg.GetPathName());
		if(bResult)
		{
			pNewDoc->SetPathName(theDlg.GetPathName(),FALSE);
			CLoadReportDlg theReportDlg;
			theReportDlg.m_pDoc=pNewDoc;
			if(theReportDlg.DoModal()==IDOK)
			{
				CMDReport* pNewReport=pNewDoc->m_Reports[theReportDlg.m_nReportIndex];
				CMDReport* pReport=m_pDoc->AddReport(&pNewReport->m_sTitle);
				if(pReport)
				{
					CMemFile mf;
					{
						CArchive ar(&mf,CArchive::store);
 						pNewReport->Serialize(ar);
					}
					mf.SeekToBegin();
					{
						CArchive ar(&mf,CArchive::load);
						pReport->Serialize(ar);
					}
					AddReport(pReport);
				}
			}
		}
		pNewDoc->OnCloseDocument();
	}
}

void CReportFormatsDlg::AddReport(CMDReport *pReport)
{
	m_List.EnableWindow();
	m_List.SetCurSel(m_List.AddString(pReport->m_sTitle));
	OnSelchangeList();
	m_Name.SetSel(0,-1);
	GotoDlgCtrl(&m_Name);
}
