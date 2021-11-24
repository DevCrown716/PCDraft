// StarDialog.cpp : implementation file
// Infinisys Ltd
//

#include "stdafx.h"
#include "pcdraft.h"
#include "StarDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStarDialog dialog


CStarDialog::CStarDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CStarDialog::IDD,pParent)
{
   //{{AFX_DATA_INIT(CStarDialog)
   m_nSides = 0;
   m_iThickness = 50;
   //}}AFX_DATA_INIT
}


void CStarDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CStarDialog)
   DDX_Text(pDX,IDC_SIDES,m_nSides);
   DDV_MinMaxInt(pDX,m_nSides,3,64);
   DDX_Text(pDX,IDC_THICKNESS,m_iThickness);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStarDialog,CDialog)
   //{{AFX_MSG_MAP(CStarDialog)
   // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStarDialog message handlers
