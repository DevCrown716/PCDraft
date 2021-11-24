// ObjectInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ObjectInfoDlg.h"
#include "PCDraftDoc.h"
#include "MDDimStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectInfoDlg dialog

CObjectInfoDlg::CObjectInfoDlg(CPCDraftDoc* pDoc,CMDShape* pSingleShape,CWnd* pParent /*=NULL*/)
	: CDialog(CObjectInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectInfoDlg)
	m_sLabel1 = pDoc->m_sFieldNames[0]+':';
	m_sLabel2 = pDoc->m_sFieldNames[1]+':';
	m_sLabel3 = pDoc->m_sFieldNames[2]+':';
	m_sLabel4 = pDoc->m_sFieldNames[3]+':';
	m_sLabel5 = pDoc->m_sFieldNames[4]+':';
	m_sValue1 = _T("");
	m_sValue2 = _T("");
	m_sValue3 = _T("");
	m_sValue4 = _T("");
	m_sValue5 = _T("");
	//}}AFX_DATA_INIT
	m_nCount=1;
	m_pShapes=NULL;
	m_pSingleShape=pSingleShape;
	m_pSingleShape->GetValues(m_sValue1,m_sValue2,m_sValue3,m_sValue4,m_sValue5);
}

CObjectInfoDlg::CObjectInfoDlg(CPCDraftDoc* pDoc,CMDShapeList* pShapes,CWnd* pParent /*=NULL*/)
	: CDialog(CObjectInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectInfoDlg)
	m_sLabel1 = pDoc->m_sFieldNames[0];
	m_sLabel2 = pDoc->m_sFieldNames[1];
	m_sLabel3 = pDoc->m_sFieldNames[2];
	m_sLabel4 = pDoc->m_sFieldNames[3];
	m_sLabel5 = pDoc->m_sFieldNames[4];
	m_sValue1 = _T("");
	m_sValue2 = _T("");
	m_sValue3 = _T("");
	m_sValue4 = _T("");
	m_sValue5 = _T("");
	//}}AFX_DATA_INIT
	m_nCount=0;
	m_pShapes=pShapes;
	m_pSingleShape=NULL;
	POSITION pos=m_pShapes->GetHeadPosition();
	while(pos)
	{
		CMDShape* pShape=m_pShapes->GetNext(pos);
		if(!pShape->IsLocked() && !m_nCount++)
		{
			m_pSingleShape=pShape;
		}
	}
	if(m_nCount==1) m_pSingleShape->GetValues(m_sValue1,m_sValue2,m_sValue3,m_sValue4,m_sValue5);
}

void CObjectInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectInfoDlg)
	DDX_Text(pDX, IDC_FIELD1_LABEL, m_sLabel1);
	DDX_Text(pDX, IDC_FIELD2_LABEL, m_sLabel2);
	DDX_Text(pDX, IDC_FIELD3_LABEL, m_sLabel3);
	DDX_Text(pDX, IDC_FIELD4_LABEL, m_sLabel4);
	DDX_Text(pDX, IDC_FIELD5_LABEL, m_sLabel5);
	DDX_Text(pDX, IDC_FIELD1_VALUE, m_sValue1);
	DDX_Text(pDX, IDC_FIELD2_VALUE, m_sValue2);
	DDX_Text(pDX, IDC_FIELD3_VALUE, m_sValue3);
	DDX_Text(pDX, IDC_FIELD4_VALUE, m_sValue4);
	DDX_Text(pDX, IDC_FIELD5_VALUE, m_sValue5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectInfoDlg message handlers

void CObjectInfoDlg::OnOK() 
{
	CDialog::OnOK();

	if(m_nCount==1)
	{
		if(!m_pSingleShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)))
			m_pSingleShape->SetValues(m_sValue1,m_sValue2,m_sValue3,m_sValue4,m_sValue5);
	}
	else
	{
		CString sValues[5];
		POSITION pos=m_pShapes->GetHeadPosition();
		while(pos)
		{
			CMDShape* pShape=m_pShapes->GetNext(pos);
			if(!pShape->IsLocked() && !pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)))
			{
				pShape->GetValues(sValues);
				pShape->SetValues(m_sValue1.IsEmpty()?sValues[0]:m_sValue1,m_sValue2.IsEmpty()?sValues[1]:m_sValue2,m_sValue3.IsEmpty()?sValues[2]:m_sValue3,m_sValue4.IsEmpty()?sValues[3]:m_sValue4,m_sValue5.IsEmpty()?sValues[4]:m_sValue5);
			}
		}
	}
}
