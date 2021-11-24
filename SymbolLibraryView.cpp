// SymbolLibraryView.cpp : implementation of the CSymbolLibraryView class
//

#include "stdafx.h"
#include "PCDraft.h"

#include "mapper.h"
#include "draftcontext.h"
#include "SymbolLibraryDoc.h"
#include "SymbolLibraryView.h"
#include "symbolscrollview.h"
#include "PCDraftDoc.h"
#include "MDArea.h"
#include "MDBitmap.h"
#include "MDText.h"
#include "MDDimLine.h"
#include "SymbolToolView.h"
#include "SymbolToolFrame.h"
#include "LinkVertex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	kFixedHeight	225
#define	kOffset			5

CString	LoadString(UINT nID)
{
	CString theText;
	VERIFY(theText.LoadString(nID));
	return theText;
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryView

IMPLEMENT_DYNCREATE(CSymbolLibraryView, CFormView)

BEGIN_MESSAGE_MAP(CSymbolLibraryView, CFormView)
	//{{AFX_MSG_MAP(CSymbolLibraryView)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_DATUM, OnDatum)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_RESIZE_LESS, OnResizeLess)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	//}}AFX_MSG_MAP
	// Standard printing commands
//	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryView construction/destruction

CSymbolLibraryView::CSymbolLibraryView()
	: CFormView(CSymbolLibraryView::IDD)
{
	//{{AFX_DATA_INIT(CSymbolLibraryView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_pSymbolView=NULL;
	m_bDatumDone=true;
}

CSymbolLibraryView::~CSymbolLibraryView()
{
}

void CSymbolLibraryView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSymbolLibraryView)
	DDX_Control(pDX, IDC_UNITS_SCALE, m_ScaleUnits);
	DDX_Control(pDX, IDC_RENAME, m_Rename);
	DDX_Control(pDX, IDC_FIND, m_Find);
	DDX_Control(pDX, IDC_DATUM, m_Datum);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_FIELD5, m_F5);
	DDX_Control(pDX, IDC_FIELD4, m_F4);
	DDX_Control(pDX, IDC_FIELD3, m_F3);
	DDX_Control(pDX, IDC_FIELD2, m_F2);
	DDX_Control(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}

BOOL CSymbolLibraryView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CSymbolLibraryView::OnInitialUpdate()
{
	if(m_pSymbolView)
	{
		m_List.SetCurSel(-1);
	}
	else
	{
		CFormView::OnInitialUpdate();
		ResizeParentToFit();
		CRect rc;
		GetClientRect(&rc);
		rc.top=225;
		rc.left+=kOffset;
		rc.bottom-=kOffset;
		rc.right-=kOffset;
		m_pSymbolView=new CSymbolScrollView;
      CCreateContext cc;
      cc.m_pCurrentDoc = GetDocument();
		m_pSymbolView->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,rc,this,IDC_SYMBOL, &cc);
	}
	LoadShapeList();
//	m_List.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryView printing

BOOL CSymbolLibraryView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSymbolLibraryView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSymbolLibraryView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

//void CSymbolLibraryView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
//{
	// TODO: add customized printing code here
//}

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryView diagnostics

#ifdef _DEBUG
void CSymbolLibraryView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSymbolLibraryView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSymbolLibraryDoc* CSymbolLibraryView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSymbolLibraryDoc)));
	return (CSymbolLibraryDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSymbolLibraryView message handlers

void CSymbolLibraryView::OnEditPaste() 
{
   
   CSymbolLibraryDoc * pDoc = GetDocument();

	if(OpenClipboard())
	{
		HANDLE hClip=GetClipboardData(((CPCDraftApp*)AfxGetApp())->m_cfShapeList);
		if(hClip)
		{
			CMemFile mf((BYTE*)GlobalLock(hClip),GlobalSize(hClip));
			CArchive ar(&mf,CArchive::load);

			int nDisplayRes,nScale;
			ar >> nDisplayRes >> nScale;

			LongPoint origin;
			ar >> origin;

			// by TSM - merging patern palettes
			int usedCount;
			int used[2048];
            int i = 0;
			for(; i<2048; i++) used[i] = 0;

			ar >> usedCount;

			CPackedItem pack_entry[2048];
			int j;
			for(i=0; i<usedCount; i++) {
				ar >> j;
				ar >> pack_entry[j];
				ASSERT( 0<=j && j<2048);

            if(pack_entry[j].m_etype == packed_type_gradient)
            {

               ASSERT(0 <= j && j < 2048);

               int found = -1;
               for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
               {
                  if(pDoc->m_packeditema[k].m_etype == packed_type_gradient)
                  {
                     if(*pack_entry[j].m_pgradient == pDoc->m_Gradients[pDoc->m_packeditema[k].get_index(pDoc)])
                     {
                        found = k;
                        break;
                     }
                  }
               }
               if(found >= 0) {
                  used[j] = found;
               }
               else {
                  used[j] = pDoc->m_packeditema.GetCount();
                  pDoc->m_Gradients[pDoc->m_numGradients] = *pack_entry[j].m_pgradient;
                  CPackedItem item;
                  item.m_etype = packed_type_gradient;
                  item.m_pgradient = &pDoc->m_Gradients[pDoc->m_numGradients];
                  pDoc->m_packeditema.Add(item);
                  pDoc->m_numGradients++;
               }

            }
		
				/*HBITMAP hDib = pat_entry[j].CreateDIBSection();
				int hash = CalculateDIBHash(hDib);
				int found = 0;
				for(int k=1; k <= pDoc->m_numDIBPatterns; k++) {
					if(hash == pDoc->m_PatternsHash[k-1]) {
						HBITMAP hDib2 = pDoc->m_Patterns[k-1].CreateDIBSection();
						if(!CompareDIBs(hDib, hDib2)) {
							::DeleteObject(hDib2);
							found = k; break;
						}
						::DeleteObject(hDib2);
					}
				}
				if(found) {
					used[j] = found-1;
					::DeleteObject(hDib);
				} else {
					used[j] = pDoc->m_numDIBPatterns;
					pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
					pDoc->m_Patterns[pDoc->m_numDIBPatterns++].FromDIBSection(hDib);
					::DeleteObject(hDib);
				}*/
			}
			// end by TSM

			int nCount, n=0;
			ar >> nCount;

			CMDShapeList groupList;

			while(nCount--)
			{
				CObject* pObject;
				ar >> pObject;
				CMDShape* pShape=(CMDShape*)pObject;
				// by TSM
				/*PenModel pm = pShape->GetPenModel();
				if(pm.m_nPenType == kPenPat)
				{
					pm.m_nPenType=kPenPatEmbed;
					pm.m_hDib=pat_entry[pm.m_nIndex].CreateDIBSection();
					pShape->SetPenModel(pm);
				}
				FillModel fm = pShape->GetFillModel();
				if(fm.m_nFillType == kFillPat)
				{
					fm.m_nFillType=kFillPatEmbed;
					fm.m_hDib=pat_entry[fm.m_nIndex].CreateDIBSection();
					pShape->SetFillModel(fm);
				}*/
				pShape->ProcessShapesAfterSymLibraryCopy(pack_entry, used);
				// end by TSM
				pShape->CalculateExtents();
				if(pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)) || pShape->IsKindOf(RUNTIME_CLASS(CMDDimLine))) continue;
				pShape->DeleteAttached();
				groupList.AddHead(pShape);
			}
			GlobalUnlock(hClip);

			if(groupList.GetCount())
			{
				CMDShape* pShape;
				if(groupList.GetCount()==1)
				{
					pShape=groupList.GetHead();
				}
				else
				{
					CMDGroupShape* pGroupShape=new CMDGroupShape(&groupList);
					pGroupShape->CalculateExtents();
					pGroupShape->SetShapeDatum(kDefaultDatum);
					pShape=pGroupShape;
				}
				GetDocument()->m_pActiveEntry=new CSymLibEntry(pShape,nDisplayRes,nScale);
				GetDocument()->m_Shapes.Add(GetDocument()->m_pActiveEntry);
				CString sName,sF2,sF3,sF4,sF5;
				pShape->GetValues(sName,sF2,sF3,sF4,sF5);
				if(sName.IsEmpty())
				{
					sName.Format(IDS_NEWSYMBOLNAME,GetDocument()->m_Shapes.GetSize());
					pShape->SetValues(sName,sF2,sF3,sF4,sF5);
				}
				GetDocument()->SetModifiedFlag();
				LoadShapeList();
			}
		}
		else
		{
			hClip=::GetClipboardData(CF_DIB);
			if(hClip)
			{
				LPBITMAPINFO lpBmi=LPBITMAPINFO(GlobalLock(hClip));
				ASSERT(lpBmi->bmiHeader.biSize==sizeof(BITMAPINFOHEADER));
				LPVOID lpBits; //=LPSTR(lpBmi+1)+sizeof(RGBQUAD);
				HBITMAP hDib=CreateDIBSection(NULL,lpBmi,DIB_RGB_COLORS,&lpBits,NULL,0);
				DIBSECTION ds;
				::GetObject(hDib,sizeof(DIBSECTION),&ds);
				memcpy(lpBits,LPVOID(LPSTR(lpBmi+1)+sizeof(RGBQUAD)*(lpBmi->bmiHeader.biClrUsed-1)),ds.dsBmih.biSizeImage); //lpBmi->bmiHeader.biSizeImage);
				CRect rc(0,0,lpBmi->bmiHeader.biWidth,lpBmi->bmiHeader.biHeight);
				CMapper theMapper;
				LongRect lrc(0,0,theMapper.DPtoLP(rc.right),theMapper.DPtoLP(rc.bottom));
				CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				GetDocument()->m_pActiveEntry=new CSymLibEntry(pShape,kEngRes,4);
				GetDocument()->m_Shapes.Add(GetDocument()->m_pActiveEntry);
				CString sName,sF2,sF3,sF4,sF5;
				sName.Format(IDS_NEWSYMBOLNAME,GetDocument()->m_Shapes.GetSize());
				pShape->SetValues(sName,sF2,sF3,sF4,sF5);
				GetDocument()->SetModifiedFlag();
				LoadShapeList();
			}
			else
			{
				hClip=::GetClipboardData(CF_TEXT);
				if(hClip)
				{
					TextParams theTextParams;
					CRect rc;
					CStyledText st;
					CStyledTextStyle style;
					style.m_TextColor=RGB(0,0,0);
					style.m_LogFont=theTextParams.m_lf;
					CString theText=(LPCTSTR)::GlobalLock(hClip);
					::GlobalUnlock(hClip);
					if(!theText.IsEmpty())
					{
						theText.Replace('\t',' ');
						theText.Replace("\r\n","\n");
						for(int i=0;i<theText.GetLength();i++)
							st.Add(theText.GetAt(i),style);
					}
					CMapper theMapper;
					{
						CClientDC theDC(this);
						CDraftContext dc(&theDC,&theMapper);
						dc.CalcBoundRect(st,theTextParams,rc, true);
					}
					LongRect lrc=theMapper.DPtoLP(rc);
					lrc.Inset(-3,-3);
					CMDText* pShape=new CMDText(CMDText::kTextAutoSize);
					pShape->SetShapePoint(0,lrc.GetCorner(0));
					pShape->SetShapePoint(1,lrc.GetCorner(1));
					pShape->SetShapePoint(2,lrc.GetCorner(2));
					pShape->SetShapePoint(3,lrc.GetCorner(3));
					pShape->SetText(st);
					pShape->SetTextParams(theTextParams);
					pShape->CalculateExtents();
					pShape->SetShapeDatum(kDefaultDatum);
					GetDocument()->m_pActiveEntry=new CSymLibEntry(pShape,kEngRes,4);
					GetDocument()->m_Shapes.Add(GetDocument()->m_pActiveEntry);
					CString sName,sF2,sF3,sF4,sF5;
					sName.Format(IDS_NEWSYMBOLNAME,GetDocument()->m_Shapes.GetSize());
					pShape->SetValues(sName,sF2,sF3,sF4,sF5);
					GetDocument()->SetModifiedFlag();
					LoadShapeList();
				}
				else
				{
					hClip=::GetClipboardData(CF_ENHMETAFILE);
					if(hClip)
					{
						ENHMETAHEADER hd;
						GetEnhMetaFileHeader((HENHMETAFILE)hClip,sizeof(hd),&hd);
						BITMAPINFOHEADER bmih;
						bmih.biSize=sizeof(BITMAPINFOHEADER);

						HDC hDC=::GetDC(NULL);
						bmih.biWidth=MulDiv(hd.rclFrame.right,GetDeviceCaps(hDC,LOGPIXELSX),2540);
						bmih.biHeight=MulDiv(hd.rclFrame.bottom,GetDeviceCaps(hDC,LOGPIXELSY),2540);
						::ReleaseDC(NULL,hDC);

						bmih.biPlanes=1;
						bmih.biBitCount=32;
						bmih.biCompression=BI_RGB;
						bmih.biSizeImage=0;
						bmih.biXPelsPerMeter=0;
						bmih.biYPelsPerMeter=0;
						bmih.biClrUsed=0;
						bmih.biClrImportant=0;
						LPVOID lpBits;
						HBITMAP hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0);
						CRect rc(0,0,bmih.biWidth,bmih.biHeight);
						CMapper theMapper;
						{
							CWindowDC theWindowDC(AfxGetMainWnd());
							CGDIBitmapDC theDC(&theWindowDC,CBitmap::FromHandle(hDib));
							theDC.FillRect(&rc,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
							PlayEnhMetaFile(theDC.GetSafeHdc(),(HENHMETAFILE)hClip,&rc);
						}
						LongRect lrc(0,0,theMapper.DPtoLP(rc.right),theMapper.DPtoLP(rc.bottom));
						CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
						pShape->CalculateExtents();
						pShape->SetShapeDatum(kDefaultDatum);
						GetDocument()->m_pActiveEntry=new CSymLibEntry(pShape,kEngRes,4);
						GetDocument()->m_Shapes.Add(GetDocument()->m_pActiveEntry);
						CString sName,sF2,sF3,sF4,sF5;
						sName.Format(IDS_NEWSYMBOLNAME,GetDocument()->m_Shapes.GetSize());
						pShape->SetValues(sName,sF2,sF3,sF4,sF5);
						GetDocument()->SetModifiedFlag();
						LoadShapeList();
					}
				}
			}
		}
		CloseClipboard();
	}
}

void CSymbolLibraryView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsClipboardFormatAvailable(((CPCDraftApp*)AfxGetApp())->m_cfShapeList) || IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_ENHMETAFILE) || IsClipboardFormatAvailable(CF_TEXT));
}

void CSymbolLibraryView::OnRename() 
{
	int n=m_List.GetCurSel();
	if(n>=0)
	{
		int i;
		CSymLibEntry* pEntry=GetDocument()->m_Shapes.GetAt(i=m_List.GetItemData(n));
		CMDShape* pShape=pEntry->m_pShape;
		CString sName,sF2,sF3,sF4,sF5;
		m_Name.GetWindowText(sName);
		m_F2.GetWindowText(sF2);
		m_F3.GetWindowText(sF3);
		m_F4.GetWindowText(sF4);
		m_F5.GetWindowText(sF5);
		pShape->SetValues(sName,sF2,sF3,sF4,sF5);
		GetDocument()->SetModifiedFlag();
		m_List.SetRedraw(FALSE);
		m_List.DeleteString(n);
		n=m_List.AddString(sName);
		m_List.SetItemData(n,i);
		m_List.SetCurSel(n);
		m_List.SetRedraw(TRUE);
	}
}

void CSymbolLibraryView::LoadShapeList(bool bSelect)
{
	int nCurSel=m_List.GetCurSel()-1;
	if(nCurSel<0) nCurSel=0;
	m_List.ResetContent();
	CSymbolLibraryDoc* pDoc=GetDocument();
	int nActive=0;
	for(int i=0;i<pDoc->m_Shapes.GetSize();i++)
	{
		CSymLibEntry* pEntry=pDoc->m_Shapes.GetAt(i);
		CString sName,sF2,sF3,sF4,sF5;
		pEntry->m_pShape->GetValues(sName,sF2,sF3,sF4,sF5);
		int n=m_List.AddString(sName);
		if(pEntry==GetDocument()->m_pActiveEntry)
			nActive=n;
		m_List.SetItemData(n,i);
	}
	bool bEnabled;
	if(pDoc->m_Shapes.GetSize())
	{
		if(bSelect) m_List.SetCurSel(nActive);
		else m_List.SetCurSel(nCurSel);
		bEnabled=true;
	}
	else
	{
		if(!m_bDatumDone)
			OnDatum();
		m_pSymbolView->SetShape(NULL);
		m_Name.SetWindowText("");
		m_F2.SetWindowText("");
		m_F3.SetWindowText("");
		m_F4.SetWindowText("");
		m_F5.SetWindowText("");
		bEnabled=false;
	}
	OnSelchangeList();
	m_Datum.EnableWindow(bEnabled);
//	m_Rename.EnableWindow(bEnabled); //  enabled/disabled indirectly via OnNameChange handler
//	m_Find.EnableWindow(bEnabled);
	m_List.EnableWindow(bEnabled);
	m_Name.EnableWindow(bEnabled);
	m_F2.EnableWindow(bEnabled);
	m_F3.EnableWindow(bEnabled);
	m_F4.EnableWindow(bEnabled);
	m_F5.EnableWindow(bEnabled);
	if(bEnabled)
	{
		m_Name.SetFocus();
		m_Name.SetSel(0,-1);
	}
}

void CSymbolLibraryView::OnSelchangeList() 
{
	if(!m_bDatumDone)
		OnDatum();
	int n=m_List.GetCurSel();
	if(n>=0)
	{
		CSymLibEntry* pEntry=GetDocument()->m_Shapes.GetAt(m_List.GetItemData(n));
		GetDocument()->m_pActiveEntry=pEntry;
		CMDShape* pShape=pEntry->m_pShape;
		CString sName,sF2,sF3,sF4,sF5;
		pShape->GetValues(sName,sF2,sF3,sF4,sF5);
		m_Name.SetWindowText(sName);
		m_F2.SetWindowText(sF2);
		m_F3.SetWindowText(sF3);
		m_F4.SetWindowText(sF4);
		m_F5.SetWindowText(sF5);
		m_pSymbolView->SetShape(pShape);
		CString theScaleUnits;
		theScaleUnits.Format("%s %s",LoadString(pEntry->m_nDisplayRes==kMetRes?IDS_METRIC_UNITS:IDS_ENGLISH_UNITS),LoadString((pEntry->m_nDisplayRes==kMetRes?IDS_METRIC_SCALE_FIRST:IDS_ENGLISH_SCALE_FIRST)+pEntry->m_nScale-1));
		m_ScaleUnits.SetWindowText(theScaleUnits);
		GetDlgItem(IDC_UNSCALED)->EnableWindow(TRUE);
		m_Name.SetFocus();
		m_Name.SetSel(0,-1);
	}
	else
	{
		m_ScaleUnits.SetWindowText("");
		GetDlgItem(IDC_UNSCALED)->EnableWindow(FALSE);
	}
}

void CSymbolLibraryView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	if(m_pSymbolView)
		m_pSymbolView->MoveWindow(kOffset,kFixedHeight,cx-2*kOffset,cy-kFixedHeight-kOffset);
}

void CSymbolLibraryView::DoClear()
{
	int n=m_List.GetCurSel();
	ASSERT(n>=0);
	int i=m_List.GetItemData(n);
	delete GetDocument()->m_Shapes.GetAt(i);
	GetDocument()->m_Shapes.RemoveAt(i);
	GetDocument()->SetModifiedFlag();
	LoadShapeList(false);
}

BOOL CSymbolLibraryView::DoCopy()
{
	int n=m_List.GetCurSel();
	ASSERT(n>=0);
	CSymLibEntry* pEntry=GetDocument()->m_Shapes.GetAt(m_List.GetItemData(n));
	CMDShape* pShape=pEntry->m_pShape;
	if(OpenClipboard() && EmptyClipboard())
	{
		HANDLE hClip;
		{
			CMemFile mf;
			CArchive ar(&mf,CArchive::store);
         ar.m_pDocument = GetDocument();
         GetDocument()->m_iArchiveVersion = 2;
			ar << pEntry->m_nDisplayRes << pEntry->m_nScale;
			
			LongPoint lptOrigin=pShape->GetShapeDatum();
			ar << lptOrigin;
			// by TSM
			//ar << int(0);
			// end by TSM

         int used[2048];
         int i = 0;
         for(; i<2048; i++) used[i] = 0;

         CMDShape* pNewShape=pShape->CreateCopy();
         pShape->ProcessShapesForCopy(GetDocument(),&used[0]);
         delete pNewShape;
         // by TSM
         int usedCount=0;
         for(i=0; i<2048; i++)
            if(used[i]) usedCount++;
         ar << usedCount;
         for(i=0; i<2048; i++)
            if(used[i]) {
               ar << i << GetDocument()->m_packeditema[i];
            }


			ar << int(1);	// count
			ar << pShape;

         CLinkVertexList theLinkVertexList; // Infinisys Ltd.
         theLinkVertexList.Serialize(ar);  // Infinisys Ltd.

			ar.Close();
			DWORD len=mf.GetLength();
			BYTE* data=mf.Detach();
			hClip=GlobalAlloc(GMEM_DDESHARE,len);
			CopyMemory(GlobalLock(hClip),data,len);
			GlobalUnlock(hClip);
			free(data);
		}
		SetClipboardData(((CPCDraftApp*)AfxGetApp())->m_cfShapeList,hClip);

#ifndef _DEMO
		if(((CPCDraftApp*)AfxGetApp())->m_bDemoMode)
#endif
			SetClipboardData(CF_BITMAP,LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_DEMO_CLIPBOARD)));
#ifndef _DEMO
		else SetClipboardData(CF_BITMAP,ShapeToBitmap(pShape));
#endif
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

HBITMAP CSymbolLibraryView::ShapeToBitmap(CMDShape* pShape)
{
	LongRect lrcExtents=pShape->CalculateBasicExtents();
	pShape->ExtendForPen(lrcExtents);

	CMapper theMapper;
	CRect rcExtents=theMapper.LPtoDP(lrcExtents);
	rcExtents.right++;
	rcExtents.bottom++;

	CWindowDC theWindowDC(AfxGetMainWnd());
	CBitmap theBmp;
	theBmp.CreateCompatibleBitmap(&theWindowDC,Width(rcExtents),Height(rcExtents));
	{
		CGDIBitmapDC theDC(&theWindowDC,&theBmp);
		theDC.SetViewportOrg(-rcExtents.left,-rcExtents.top);

		CRgn theClipRgn;
		theClipRgn.CreateRectRgnIndirect(&rcExtents);
		theClipRgn.OffsetRgn(-rcExtents.left,-rcExtents.top);
		theDC.SelectClipRgn(&theClipRgn);

		theDC.FillRect(&rcExtents,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

		CDraftContext dc(&theDC,&theMapper, NULL, GetDocument());
		pShape->DrawShape(&dc);
	}
	return (HBITMAP)theBmp.Detach();
}

void CSymbolLibraryView::OnEditCopy() 
{
	DoCopy();
}

void CSymbolLibraryView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_List.GetCurSel()>=0);
}

void CSymbolLibraryView::OnEditCut() 
{
	if(DoCopy())
		DoClear();
}

void CSymbolLibraryView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_List.GetCurSel()>=0);
}

void CSymbolLibraryView::OnEditClear() 
{
	DoClear();	
}

void CSymbolLibraryView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_List.GetCurSel()>=0);
}

void CSymbolLibraryView::OnFind() 
{
	CString sName;
	m_Name.GetWindowText(sName);
	m_List.SelectString(-1,sName);
	OnSelchangeList();
}

void CSymbolLibraryView::OnDatum() 
{
	UINT nID;
	if(m_bDatumDone)
	{
		nID=IDS_DONE;
		m_bDatumDone=false;
	}
	else
	{
		nID=IDS_DATUM;
		m_bDatumDone=true;
	}
	GetDlgItem(IDC_CENTER)->EnableWindow(m_bDatumDone?FALSE:TRUE);
	m_Datum.SetWindowText(LoadString(nID));
	m_pSymbolView->HideHandles(m_bDatumDone);
}

void CSymbolLibraryView::OnCenter() 
{
	m_pSymbolView->CenterShapeDatum();
}

void CSymbolLibraryView::OnDblclkList() 
{
	OnDatum();
}

void CSymbolLibraryView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu theMenu;
	theMenu.LoadMenu(IDR_PCDRAFT_POPUP);
	CMenu* pPopup=theMenu.GetSubMenu(0);
	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON|TPM_CENTERALIGN,point.x,point.y,AfxGetMainWnd());
}

void CSymbolLibraryView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch(lHint)
	{
		case kRefresh:
			m_pSymbolView->Invalidate(TRUE);
			break;
	}
}

void CSymbolLibraryView::OnResizeLess() 
{
	CCreateContext theContext;
	theContext.m_pCurrentDoc=GetDocument();
	theContext.m_pNewViewClass=RUNTIME_CLASS(CSymbolToolView);
	CFrameWnd* pNewFrame=(CFrameWnd*)(RUNTIME_CLASS(CSymbolToolFrame)->CreateObject());
	if(pNewFrame->LoadFrame(IDR_SYMBOL_LIBRARY,MFS_SYNCACTIVE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,AfxGetMainWnd(),&theContext))
	{
		CRect rc;
		GetParentFrame()->GetWindowRect(&rc);
		pNewFrame->SetMenu(NULL);
		pNewFrame->SetWindowPos(NULL,rc.left,rc.top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
		pNewFrame->InitialUpdateFrame(GetDocument(),TRUE);
		pNewFrame->SetWindowText(GetDocument()->GetTitle());
		GetParentFrame()->DestroyWindow();
	}
}

BOOL CSymbolLibraryView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && m_List.GetCount())
	{
		switch(pMsg->wParam)
		{
		case VK_DOWN:
			{
				int nCurSel=m_List.GetCurSel()+1;
				if(nCurSel==m_List.GetCount()) nCurSel=0;
				m_List.SetCurSel(nCurSel);
				OnSelchangeList();
				return TRUE;
			}
			break;
		case VK_UP:
			{
				int nCurSel=m_List.GetCurSel()-1;
				if(nCurSel<0) nCurSel=m_List.GetCount()-1;
				m_List.SetCurSel(nCurSel);
				OnSelchangeList();
				return TRUE;
			}
			break;
		}
	}
	return CFormView::PreTranslateMessage(pMsg);
}

void CSymbolLibraryView::OnChangeName() 
{
	BOOL bEnabled=m_Name.GetWindowTextLength()>0;
	m_Rename.EnableWindow(bEnabled);
	m_Find.EnableWindow(bEnabled);
}
