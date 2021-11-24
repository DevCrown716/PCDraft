// FindReplaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "FindReplaceDlg.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg dialog


CFindReplaceDlg::CFindReplaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindReplaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindReplaceDlg)
	//}}AFX_DATA_INIT
	m_bShowMore=false;
}


void CFindReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindReplaceDlg)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindReplaceDlg, CDialog)
	//{{AFX_MSG_MAP(CFindReplaceDlg)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_REPLACE_USE_MOUSE, OnReplaceUseMouse)
	ON_BN_CLICKED(IDC_FIND_USE_MOUSE, OnFindUseMouse)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_REPLACE, OnReplace)
	ON_CBN_SELCHANGE(IDC_REPLACE_TYPE, OnSelchangeReplaceType)
	ON_BN_CLICKED(IDC_REPLACE_ALL, OnReplaceAll)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RESIZE_OPT, OnResizeOpt)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ALL_LAYERS, OnSelectValidShapes)
	ON_EN_CHANGE(IDC_FIELD1_FIND, OnSelectValidShapes)
	ON_EN_CHANGE(IDC_FIELD2_FIND, OnSelectValidShapes)
	ON_EN_CHANGE(IDC_FIELD3_FIND, OnSelectValidShapes)
	ON_EN_CHANGE(IDC_FIELD4_FIND, OnSelectValidShapes)
	ON_EN_CHANGE(IDC_FIELD5_FIND, OnSelectValidShapes)
	ON_BN_CLICKED(IDC_FIELD1_USE, OnSelectValidShapes)
	ON_BN_CLICKED(IDC_FIELD2_USE, OnSelectValidShapes)
	ON_BN_CLICKED(IDC_FIELD3_USE, OnSelectValidShapes)
	ON_BN_CLICKED(IDC_FIELD4_USE, OnSelectValidShapes)
	ON_BN_CLICKED(IDC_FIELD5_USE, OnSelectValidShapes)
	ON_EN_CHANGE(IDC_FIELD1_REPLACE, OnSelectReplaceShape)
	ON_EN_CHANGE(IDC_FIELD2_REPLACE, OnSelectReplaceShape)
	ON_EN_CHANGE(IDC_FIELD3_REPLACE, OnSelectReplaceShape)
	ON_EN_CHANGE(IDC_FIELD4_REPLACE, OnSelectReplaceShape)
	ON_EN_CHANGE(IDC_FIELD5_REPLACE, OnSelectReplaceShape)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg message handlers

BOOL CFindReplaceDlg::OnInitDialog() 
{
	CString strText;
	CComboBox* pCB=(CComboBox*)GetDlgItem(IDC_REPLACE_TYPE);
	for(int i=IDS_REPLACEFIRST;i<=IDS_REPLACELAST;i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}

	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_FIELD1_USE))->SetCheck(1);
	((CComboBox*)GetDlgItem(IDC_REPLACE_TYPE))->SetCurSel(0);

	m_ReplaceType=kReplaceObjectAndData;

	m_pActiveView=NULL;
	CMDIFrameWnd *pFrame=(CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild=(CMDIChildWnd*)pFrame->GetActiveFrame();
	CView *pView=pChild->GetActiveView();
	if(pView && pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
	{
		CMDShapeList& theSelection=((CPCDraftView*)pView)->GetSelection();
		if(theSelection.GetCount()==1)
		{
			CMDShape* pShape=theSelection.GetHead();
			CString sValues[5];
			pShape->GetValues(sValues);
			SetDlgItemText(IDC_FIELD1_FIND,sValues[0]);
			SetDlgItemText(IDC_FIELD2_FIND,sValues[1]);
			SetDlgItemText(IDC_FIELD3_FIND,sValues[2]);
			SetDlgItemText(IDC_FIELD4_FIND,sValues[3]);
			SetDlgItemText(IDC_FIELD5_FIND,sValues[4]);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFindReplaceDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	m_pActiveView=NULL;
	if(nState)
	{
		CMDIFrameWnd *pFrame=(CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd *pChild=(CMDIChildWnd*)pFrame->GetActiveFrame();
		CView *pView=pChild->GetActiveView();
		if(pView && pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
		{
			m_pActiveView=(CPCDraftView*)pView;
			CPCDraftDoc* pDoc=m_pActiveView->GetDocument();
			SetDlgItemText(IDC_FIELD1_LABEL,pDoc->m_sFieldNames[0]);
			SetDlgItemText(IDC_FIELD2_LABEL,pDoc->m_sFieldNames[1]);
			SetDlgItemText(IDC_FIELD3_LABEL,pDoc->m_sFieldNames[2]);
			SetDlgItemText(IDC_FIELD4_LABEL,pDoc->m_sFieldNames[3]);
			SetDlgItemText(IDC_FIELD5_LABEL,pDoc->m_sFieldNames[4]);
			SelectValidShapes();
			m_Undo.EnableWindow(FALSE);
		}
		else
			SelectValidShapes();
	}
}

void CFindReplaceDlg::ShapeSelected(CMDShape* pShape)
{
	ShowWindow(SW_SHOW);
	BringWindowToTop();
	CString sValues[5];
	pShape->GetValues(sValues);
	bool bInvalidShape=sValues[0].IsEmpty() && sValues[1].IsEmpty() && sValues[2].IsEmpty() && sValues[3].IsEmpty()&& sValues[4].IsEmpty();
	if(m_nShapeSelectedType==kFindShape)
	{
		if(bInvalidShape)
		{
			AfxMessageBox(IDS_INVALID_FIND_SHAPE,MB_ICONSTOP|MB_OK);
			return;
		}
		SetDlgItemText(IDC_FIELD1_FIND,sValues[0]);
		SetDlgItemText(IDC_FIELD2_FIND,sValues[1]);
		SetDlgItemText(IDC_FIELD3_FIND,sValues[2]);
		SetDlgItemText(IDC_FIELD4_FIND,sValues[3]);
		SetDlgItemText(IDC_FIELD5_FIND,sValues[4]);
		SelectValidShapes();
	}
	else
	{
		if(bInvalidShape)
		{
			AfxMessageBox(IDS_INVALID_REPLACE_SHAPE,MB_ICONSTOP|MB_OK);
			return;
		}
		SetDlgItemText(IDC_FIELD1_REPLACE,sValues[0]);
		SetDlgItemText(IDC_FIELD2_REPLACE,sValues[1]);
		SetDlgItemText(IDC_FIELD3_REPLACE,sValues[2]);
		SetDlgItemText(IDC_FIELD4_REPLACE,sValues[3]);
		SetDlgItemText(IDC_FIELD5_REPLACE,sValues[4]);
		SelectReplaceShape();
		SetupFoundShapesStatus();
	}
}

void CFindReplaceDlg::OnReplaceUseMouse() 
{
	DoShapeSelect(kReplaceShape);
}

void CFindReplaceDlg::OnFindUseMouse() 
{
	DoShapeSelect(kFindShape);
}

void CFindReplaceDlg::DoShapeSelect(ShapeSelectedType nType)
{
	if(m_pActiveView)
	{
		CPCDraftView* pActiveView=m_pActiveView;
		m_nShapeSelectedType=nType;
		ShowWindow(SW_HIDE);
		pActiveView->GetParentFrame()->ActivateFrame();
		pActiveView->DoShapeSelect(this);
	}
}

void CFindReplaceDlg::SelectLayerShapes(int nLayerIndex,CLayer* pLayer,bool* bUseField,CString* sFieldValue)
{
	POSITION pos=pLayer->m_ShapeList.GetTailPosition();
	CString sValues[5];
	while(pos!=NULL)
	{
		CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
		if(pShape->IsLocked()) continue;
		pShape->GetValues(sValues);
		bool bOk=false;
        int i = 0;
		for(;i<5;i++)
		{
			if(bUseField[i])
			{
				if(sValues[i]==sFieldValue[i])
				{
					bOk=true;
				}
				else
					break;
			}
		}
		if(i==5 && bOk)
		{
			m_FoundShapes.Add(pShape);
			m_FoundShapesLayerIndex.Add(nLayerIndex);
		}
	}
}

CMDShape* CFindReplaceDlg::FindLayerShape(CLayer* pLayer,CString* sFieldValue)
{
	POSITION pos=pLayer->m_ShapeList.GetTailPosition();
	CString sValues[5];
	while(pos!=NULL)
	{
		CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
		pShape->GetValues(sValues);
		if(sValues[0]==sFieldValue[0]
		&& sValues[1]==sFieldValue[1]
		&& sValues[2]==sFieldValue[2]
		&& sValues[3]==sFieldValue[3]
		&& sValues[4]==sFieldValue[4])
		{
			return pShape;
		}
	}
	return NULL;
}

void CFindReplaceDlg::SelectReplaceShape()
{
	m_pReplaceShape=NULL;
	if(m_pActiveView)
	{
		bool bAllLayers=((CButton*)GetDlgItem(IDC_ALL_LAYERS))->GetCheck()?true:false;
		CString sValues[5];
		GetDlgItemText(IDC_FIELD1_REPLACE,sValues[0]);
		GetDlgItemText(IDC_FIELD2_REPLACE,sValues[1]);
		GetDlgItemText(IDC_FIELD3_REPLACE,sValues[2]);
		GetDlgItemText(IDC_FIELD4_REPLACE,sValues[3]);
		GetDlgItemText(IDC_FIELD5_REPLACE,sValues[4]);
		if(!sValues[0].IsEmpty()
		|| !sValues[1].IsEmpty()
		|| !sValues[2].IsEmpty()
		|| !sValues[3].IsEmpty()
		|| !sValues[4].IsEmpty())
		{
			CPCDraftDoc* pDoc=m_pActiveView->GetDocument();
			if(bAllLayers)
			{
				for(int i=0;i<pDoc->GetLayersCount();i++)
				{
					m_pReplaceShape=FindLayerShape(pDoc->GetLayer(i),sValues);
					if(m_pReplaceShape)
						break;
				}
			}
			else
			{
				m_pReplaceShape=FindLayerShape(pDoc->GetActiveLayer(),sValues);
			}
		}
	}
}

void CFindReplaceDlg::SelectValidShapes()
{
	m_FoundShapes.RemoveAll();
	m_FoundShapesLayerIndex.RemoveAll();
	if(m_pActiveView)
	{
		bool bAllLayers=((CButton*)GetDlgItem(IDC_ALL_LAYERS))->GetCheck()?true:false;
		bool bUseField[5];
		bUseField[0]=((CButton*)GetDlgItem(IDC_FIELD1_USE))->GetCheck()?true:false;
		bUseField[1]=((CButton*)GetDlgItem(IDC_FIELD2_USE))->GetCheck()?true:false;
		bUseField[2]=((CButton*)GetDlgItem(IDC_FIELD3_USE))->GetCheck()?true:false;
		bUseField[3]=((CButton*)GetDlgItem(IDC_FIELD4_USE))->GetCheck()?true:false;
		bUseField[4]=((CButton*)GetDlgItem(IDC_FIELD5_USE))->GetCheck()?true:false;
		CString sValues[5];
		GetDlgItemText(IDC_FIELD1_FIND,sValues[0]);
		GetDlgItemText(IDC_FIELD2_FIND,sValues[1]);
		GetDlgItemText(IDC_FIELD3_FIND,sValues[2]);
		GetDlgItemText(IDC_FIELD4_FIND,sValues[3]);
		GetDlgItemText(IDC_FIELD5_FIND,sValues[4]);
		CString sResult;
		for(int i=0;i<5;i++)
			if(bUseField[i])
				sResult+=sValues[i];
		if(!sResult.IsEmpty())
		{
			CPCDraftDoc* pDoc=m_pActiveView->GetDocument();
			if(bAllLayers)
			{
				for(int i=0;i<pDoc->GetLayersCount();i++)
				{
					SelectLayerShapes(i,pDoc->GetLayer(i),bUseField,sValues);
				}
			}
			else
			{
				SelectLayerShapes(pDoc->m_nActiveLayer,pDoc->GetActiveLayer(),bUseField,sValues);
			}
		}
	}
	m_nCurrShapeIndex=-1;
	SelectReplaceShape();
	SetupFoundShapesStatus();
}

void CFindReplaceDlg::SetupFoundShapesStatus()
{
	CString theFound;
	int nCount= (int)m_FoundShapes.GetSize();
	if(m_nCurrShapeIndex>0) nCount-=m_nCurrShapeIndex;
	BOOL bEnabled=nCount!=0;
	if(bEnabled)
	{
		theFound.Format(IDS_FOUND,nCount);
	}
	else
	{
		theFound.LoadString(IDS_NONE_FOUND);
	}
	SetDlgItemText(IDC_FOUND,theFound);
	GetDlgItem(IDC_FIND)->EnableWindow(bEnabled);
	BOOL bReplaceStatus=((m_ReplaceType==kReplaceObjectAndData || m_ReplaceType==kReplaceObjectOnly) && m_pReplaceShape!=NULL) || m_ReplaceType==kReplaceDataOnly;
	GetDlgItem(IDC_REPLACE)->EnableWindow(bEnabled && m_nCurrShapeIndex>=0 && bReplaceStatus);
	GetDlgItem(IDC_REPLACE_ALL)->EnableWindow(bEnabled && bReplaceStatus);
}

void CFindReplaceDlg::OnSelectReplaceShape() 
{
	SelectReplaceShape();
	SetupFoundShapesStatus();
}

void CFindReplaceDlg::OnSelectValidShapes() 
{
	if(m_pActiveView)
		SelectValidShapes();
}

void CFindReplaceDlg::DoFind()
{
	++m_nCurrShapeIndex;
	if(m_nCurrShapeIndex<m_FoundShapes.GetSize())
	{
		m_pActiveView->SelectNone();
		m_pActiveView->GetDocument()->SetActiveLayer(m_FoundShapesLayerIndex.GetAt(m_nCurrShapeIndex));
		CMDShape* pShape=(CMDShape*)m_FoundShapes.GetAt(m_nCurrShapeIndex);
		m_pActiveView->EnsureVisible(pShape);
		m_pActiveView->SelectShape(pShape,false);
	}
	SetupFoundShapesStatus();
}

void CFindReplaceDlg::OnFind() 
{
	if(!GetDlgItem(IDC_FIND)->IsWindowEnabled())
		return;
	if(!m_pActiveView) return;
	m_pActiveView->SaveUndo(0);
	m_Undo.EnableWindow(FALSE);
	DoFind();
}

void CFindReplaceDlg::DoReplace()
{
	CMDShape* pShape=(CMDShape*)m_FoundShapes.GetAt(m_nCurrShapeIndex);

	CString sValues[5];
	pShape->GetValues(sValues);

	CString sValue;
	GetDlgItemText(IDC_FIELD1_REPLACE,sValue);
	if(m_ReplaceType!=kReplaceDataOnly || !sValue.IsEmpty()) sValues[0]=sValue;
	GetDlgItemText(IDC_FIELD2_REPLACE,sValue);
	if(m_ReplaceType!=kReplaceDataOnly || !sValue.IsEmpty()) sValues[1]=sValue;
	GetDlgItemText(IDC_FIELD3_REPLACE,sValue);
	if(m_ReplaceType!=kReplaceDataOnly || !sValue.IsEmpty()) sValues[2]=sValue;
	GetDlgItemText(IDC_FIELD4_REPLACE,sValue);
	if(m_ReplaceType!=kReplaceDataOnly || !sValue.IsEmpty()) sValues[3]=sValue;
	GetDlgItemText(IDC_FIELD5_REPLACE,sValue);
	if(m_ReplaceType!=kReplaceDataOnly || !sValue.IsEmpty()) sValues[4]=sValue;

	switch(m_ReplaceType)
	{
		case kReplaceObjectOnly:
			pShape->GetValues(sValues);
		case kReplaceObjectAndData:
			if(m_pReplaceShape!=pShape)
			{
				ASSERT(m_pReplaceShape!=NULL);
				m_pActiveView->SelectNone();
				m_pActiveView->InvalidateShape(pShape,true);
				pShape->ShowArea(false,m_pActiveView);
				CMDShape* pNewShape=m_pReplaceShape->CreateCopy();
				pNewShape->DeleteAttached(); // do not copy area if any
				pNewShape->DoShapeMove(pShape->GetShapeDatum());
				m_pActiveView->GetDocument()->GetActiveLayer()->ReplaceShape(pShape,pNewShape);
				m_pActiveView->SelectShape(pNewShape,true);
				pShape=pNewShape;
			}
		case kReplaceDataOnly:
			pShape->SetValues(sValues);
			break;
		default:
			ASSERT(FALSE);
	}
	DoFind();
	m_pActiveView->GetDocument()->SetModifiedFlag();
}

void CFindReplaceDlg::OnReplace() 
{
	if(!GetDlgItem(IDC_REPLACE)->IsWindowEnabled())
		return;
	if(!m_pActiveView) return;
	m_pActiveView->SaveUndo(IDS_REPLACE);
	m_Undo.EnableWindow();
	DoReplace();
}

void CFindReplaceDlg::OnSelchangeReplaceType() 
{
	m_ReplaceType=((CComboBox*)GetDlgItem(IDC_REPLACE_TYPE))->GetCurSel();
	SetupFoundShapesStatus();
}

void CFindReplaceDlg::OnReplaceAll() 
{
	if(!GetDlgItem(IDC_REPLACE_ALL)->IsWindowEnabled())
		return;
	if(!m_pActiveView) return;
	m_pActiveView->SaveUndo(IDS_REPLACE);
	m_Undo.EnableWindow();
	if(m_nCurrShapeIndex<0)
		DoFind();
	while(m_nCurrShapeIndex<m_FoundShapes.GetSize())
		DoReplace();
}

void CFindReplaceDlg::OnUndo() 
{
	if(!GetDlgItem(IDC_UNDO)->IsWindowEnabled())
		return;
	if(!m_pActiveView) return;
	m_pActiveView->DoEditUndo();
	SelectValidShapes();
}

void CFindReplaceDlg::OnClose() 
{
	m_bShowMore=false;
	DestroyWindow();
}

void CFindReplaceDlg::OffsetWindow(UINT nID,int nDelta)
{
	CRect rc;
	CWnd* pWnd=GetDlgItem(nID);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	pWnd->SetWindowPos(NULL,rc.left,rc.top+nDelta,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

void CFindReplaceDlg::OnResizeOpt() 
{
	static UINT gCtrls[] = { IDC_FIND_STATIC,IDC_REPLACE1_STATIC,IDC_REPLACE2_STATIC,
							 IDC_ALL_LAYERS,IDC_FIND_USE_MOUSE,IDC_REPLACE_USE_MOUSE,
							 IDC_FIELD1_LABEL,IDC_FIELD1_USE,IDC_FIELD1_FIND,IDC_FIELD1_REPLACE,
							 IDC_FIELD2_LABEL,IDC_FIELD2_USE,IDC_FIELD2_FIND,IDC_FIELD2_REPLACE,
							 IDC_FIELD3_LABEL,IDC_FIELD3_USE,IDC_FIELD3_FIND,IDC_FIELD3_REPLACE,
							 IDC_FIELD4_LABEL,IDC_FIELD4_USE,IDC_FIELD4_FIND,IDC_FIELD4_REPLACE,
							 IDC_FIELD5_LABEL,IDC_FIELD5_USE,IDC_FIELD5_FIND,IDC_FIELD5_REPLACE,
							 IDC_REPLACE_TYPE, 0 };
	UINT nBitmapID;
	int nDelta;
	CRect rc(0,0,0,112);
	MapDialogRect(&rc);
	if(m_bShowMore)
	{
		nDelta=rc.bottom;
		m_bShowMore=false;
		nBitmapID=IDB_RESIZE_LESS;
	}
	else
	{
		nDelta=-rc.bottom;
		m_bShowMore=true;
		nBitmapID=IDB_RESIZE_MORE;
	}
	OffsetWindow(IDC_FIND,nDelta);
	OffsetWindow(IDC_FOUND,nDelta);
	OffsetWindow(IDC_REPLACE,nDelta);
	OffsetWindow(IDC_REPLACE_ALL,nDelta);
	OffsetWindow(IDC_UNDO,nDelta);
	for(int i=0;gCtrls[i];i++)
		GetDlgItem(gCtrls[i])->ShowWindow((!m_bShowMore)?SW_SHOW:SW_HIDE);
	HBITMAP hOldBitmap=((CStatic*)GetDlgItem(IDC_RESIZE_OPT))->SetBitmap(CreateMappedBitmap(AfxGetInstanceHandle(),nBitmapID,0,NULL,0));
	if(hOldBitmap)
		DeleteObject(hOldBitmap);
	GetWindowRect(&rc);
	SetWindowPos(NULL,0,0,rc.Width(),rc.Height()+nDelta,SWP_NOZORDER|SWP_NOMOVE);
}
