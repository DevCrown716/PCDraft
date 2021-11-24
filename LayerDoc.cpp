// Layer - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftView.h"
#include "PCDraftDoc.h"
#include "LayerDoc.h"
#include "LayerListView.h"
#include "Layer.h"
#include "LayerFrame.h"
#include "LayerHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString get_utf8(LPCWSTR lpcwsz);

IMPLEMENT_DYNCREATE(CLayerDoc, CDocument)

CLayerDoc::CLayerDoc()
{
   m_iLayerSel = -1;
   m_playerheader = NULL;
   m_pDoc = NULL;
}

BOOL CLayerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CLayerDoc::~CLayerDoc()
{
}


BEGIN_MESSAGE_MAP(CLayerDoc, CDocument)
   ON_COMMAND(ID_LAYER_SHOW,&CLayerDoc::OnLayerShow)
   ON_COMMAND(ID_LAYER_HIDE,&CLayerDoc::OnLayerHide)
   ON_COMMAND(ID_LAYER_GRAY,&CLayerDoc::OnLayerGray)
   ON_COMMAND(ID_LAYER_MERGE,&CLayerDoc::OnLayerMerge)
   ON_COMMAND(ID_LAYER_ADD,&CLayerDoc::OnLayerAdd)
   ON_COMMAND(ID_LAYER_DELETE,&CLayerDoc::OnLayerDelete)
   ON_COMMAND(ID_LAYER_RENAME,&CLayerDoc::OnLayerRename)
   ON_UPDATE_COMMAND_UI(ID_LAYER_SHOW,&CLayerDoc::OnUpdateLayerShow)
   ON_UPDATE_COMMAND_UI(ID_LAYER_HIDE,&CLayerDoc::OnUpdateLayerHide)
   ON_UPDATE_COMMAND_UI(ID_LAYER_GRAY,&CLayerDoc::OnUpdateLayerGray)
   ON_UPDATE_COMMAND_UI(ID_LAYER_MERGE,&CLayerDoc::OnUpdateLayerMerge)
   ON_UPDATE_COMMAND_UI(ID_LAYER_ADD,&CLayerDoc::OnUpdateLayerAdd)
   ON_UPDATE_COMMAND_UI(ID_LAYER_DELETE,&CLayerDoc::OnUpdateLayerDelete)
   ON_UPDATE_COMMAND_UI(ID_LAYER_RENAME,&CLayerDoc::OnUpdateLayerRename)
END_MESSAGE_MAP()


// CLayerDoc diagnostics

#ifdef _DEBUG
void CLayerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CLayerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CLayerDoc serialization

void CLayerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CLayerDoc commands

CString CLayerDoc::GetDefaultLayerPath()
{

   TCHAR path[MAX_PATH];

   HRESULT hr = SHGetFolderPath(NULL,CSIDL_PERSONAL,NULL,SHGFP_TYPE_CURRENT,path);

   CString str(path);

   str.TrimRight("/");

   str.TrimRight("\\");

   str += "\\Microspot";

   if(!PathIsDirectory(str))
   {
      ::CreateDirectory(str,NULL);
   }

   str += "\\Layer.spotlibrary";
   if(!PathIsDirectory(str))
   {
      ::CreateDirectory(str,NULL);

   }

   return str;

}


CString CLayerDoc::GetDefaultPhotoLayerPath()
{


   PWSTR pwsz = NULL;

   if(FAILED(SHGetKnownFolderPath(FOLDERID_Pictures,KF_FLAG_CREATE,NULL,&pwsz)))
   {

      return "";

   }

   CString str = get_utf8(pwsz);

   CoTaskMemFree(pwsz);

   str.TrimRight("/");

   str.TrimRight("\\");

   return str;

}


CString CLayerDoc::GetAppDataPath()
{

   PWSTR pwsz = NULL;

   if(FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData,KF_FLAG_CREATE, NULL, &pwsz)))
   {
      
      return "";

   }

   CString str = get_utf8(pwsz);

   CoTaskMemFree(pwsz);

   str.TrimRight("/");

   str.TrimRight("\\");

   str += "\\Microspot";

   if(!PathIsDirectory(str))
   {
      ::CreateDirectory(str,NULL);
   }

   str += "\\Layer";
   if(!PathIsDirectory(str))
   {
      ::CreateDirectory(str,NULL);

   }

   return str;

}


void CLayerDoc::OnLayerAdd()
{
   CLayer* pLayer=m_pDoc->AddLayer();
   //int n;
   //m_LayerList.m_pActiveLayer=pLayer;
   //m_LayerList.SetItemDataPtr(n=m_LayerList.AddString(NULL),pLayer);
   //m_LayerList.SelItemRange(FALSE,0,m_LayerList.GetCount() - 1);
   //m_LayerList.SetSel(n);
   //m_LayerList.SetCaretIndex(n);
   m_playerlistview->RedrawWindow();
   m_pDoc->LayersChanged();
}


void CLayerDoc::OnUpdateLayerAdd(CCmdUI *pCmdUI)
{

   pCmdUI->Enable(is_add_enabled());

}


void CLayerDoc::OnLayerDelete()
{
   
   if(m_playerlistview->get_selected_count() >= 1 && m_pDoc->GetLayersCount() > 1)
   {

      for(int i = 0; i < get_item_count(); i++)
      {

         if(m_playerlistview->is_selected(i))
         {

            if(m_pDoc->GetLayer(i)->m_ShapeList.GetCount() != 0)
            {

               m_playerframe->m_bIgnoreActivate=true;

               CString thePrompt;

               thePrompt.Format(IDS_QUERY_DELETE_LAYER,m_pDoc->GetLayer(i)->m_strName);

               if(AfxMessageBox(thePrompt,MB_YESNO | MB_DEFBUTTON2) == IDNO)
               {

                  m_playerframe->m_bIgnoreActivate=false;

                  continue;

               }

               m_playerframe->m_bIgnoreActivate=false;
            }

            m_pDoc->DeleteLayer(i);

            m_playerlistview->set_selected(i,false);

            i--;

         }

      }

      m_playerlistview->unselect_all();

      m_playerlistview->OnSelchangeLayerList();

      m_pDoc->LayersChanged();

   }
   else
   {
      m_playerframe->m_bIgnoreActivate=true;
      AfxMessageBox(IDS_CANT_DELETE_LAYER,MB_OK | MB_ICONSTOP);
      m_playerframe->m_bIgnoreActivate=false;
   }
}



void CLayerDoc::OnUpdateLayerDelete(CCmdUI *pCmdUI)
{

   pCmdUI->Enable(is_delete_enabled());

}


void CLayerDoc::OnLayerRename()
{

   //CLayerFrame * pframe = (CLayerFrame *)GetParentFrame();

   //pframe->OnFileRenameitem();

}



void CLayerDoc::OnUpdateLayerRename(CCmdUI *pCmdUI)
{

   //pCmdUI->Enable(m_plist->is_rename_enabled());

}




void CLayerDoc::OnLayerShow()
{
   for(int i = 0; i < m_playerlistview->m_iaSel.GetCount(); i++)
   {
      m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]]->m_bHidden = false;
      m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]]->m_bGrayed = false;
   }
   m_playerlistview->RedrawWindow();
}


void CLayerDoc::OnUpdateLayerShow(CCmdUI *pCmdUI)
{

   pCmdUI->Enable(is_show_enabled());

}


void CLayerDoc::OnLayerHide()
{
   for(int i = 0; i < m_playerlistview->m_iaSel.GetCount(); i++)
   {
      if(m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]] != m_pDoc->GetActiveLayer())
      {
         m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]]->m_bHidden = true;
         m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]]->m_bGrayed = false;

      }
   }
   m_playerlistview->RedrawWindow();
}


void CLayerDoc::OnUpdateLayerHide(CCmdUI *pCmdUI)
{

   pCmdUI->Enable(is_hide_enabled());

}



void CLayerDoc::OnLayerGray()
{
   for(int i = 0; i < m_playerlistview->m_iaSel.GetCount(); i++)
   {
      if(m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]] != m_pDoc->GetActiveLayer())
      {
         m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]]->m_bHidden = false;
         m_pDoc->m_Layers[m_playerlistview->m_iaSel[i]]->m_bGrayed = true;

      }
   }

   m_playerlistview->RedrawWindow();

}


void CLayerDoc::OnUpdateLayerGray(CCmdUI *pCmdUI)
{

   pCmdUI->Enable(is_gray_enabled());

}




void CLayerDoc::OnLayerMerge()
{
   m_playerframe->m_bIgnoreActivate=true;

   int n=m_playerlistview->get_selected_count();
   CCmpArray < int > indexes;

   indexes.Append(m_playerlistview->m_iaSel);
   //m_playerlistview->GetSelItems(n,pIndexes);

   int nScale=m_pDoc->GetLayer(indexes[0])->m_nScale;
   int i = 1;
   for(; i<n; i++) if(nScale != m_pDoc->GetLayer(indexes[i])->m_nScale) break;

   if(i<n) AfxMessageBox(IDS_CANT_MERGE_LAYERS,MB_OK | MB_ICONEXCLAMATION);
   else if(AfxMessageBox(IDS_LAYER_MERGE_CONFIRM,MB_OKCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON2) == IDOK)
   {
      m_pDoc->MergeLayer(n,indexes.GetData());
      //LoadListBox();
//      OnSelchangeLayerList();
      m_pDoc->LayersChanged();
      m_playerlistview->unselect_all();
      m_playerlistview->RedrawWindow();
   }
   //delete[]pIndexes;
   m_playerframe->m_bIgnoreActivate=false;
}

void CLayerDoc::OnUpdateLayerMerge(CCmdUI *pCmdUI)
{

   pCmdUI->Enable(is_merge_enabled());

}





void CLayerDoc::Refresh()
{
   static UINT gCtrlID[] ={IDC_ADD,IDC_ARRANGE,IDC_DELETE,IDC_EDIT_LAYER,IDC_GRAYED,IDC_HIDDEN,IDC_LAYER_LIST,IDC_LAYER_NAME,IDC_MERGE,IDC_RENAME,IDC_SCALE,0};
   CMDIFrameWnd *pFrame=(CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
   CMDIChildWnd *pChild=(CMDIChildWnd*)pFrame->GetActiveFrame();
   CView *pView=pChild->GetActiveView();
   if(pView)
   {
      if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
      {
         m_pDoc=(CPCDraftDoc*)pView->GetDocument();
      }
      else
      {
         if(!((CPCDraftApp*)AfxGetApp())->FindDrawingDoc(m_pDoc))
            m_pDoc=NULL;
      }
   }
   else
   {
      m_pDoc=NULL;
   }
   if(m_pDoc)
   {
      CString strText;
      strText.Format(IDS_LAYER_SETUP,(LPCTSTR)m_pDoc->GetTitle());
      //AfxSetWindowText(GetSafeHwnd(),strText);
      //bool bReloadScale;
      //UINT nScaleFirst,nScaleLast;
      //if(m_pDoc->m_bEnglishUnits)
      //{
      //   nScaleFirst=IDS_ENGLISH_SCALE_FIRST;
      //   nScaleLast=IDS_ENGLISH_SCALE_LAST;
      //   bReloadScale=m_nLastScale != 1;
      //   m_nLastScale=1;
      //}
      //else
      //{
      //   nScaleFirst=IDS_METRIC_SCALE_FIRST;
      //   nScaleLast=IDS_METRIC_SCALE_LAST;
      //   bReloadScale=m_nLastScale != 2;
      //   m_nLastScale=2;
      //}

      //if(bReloadScale)
      //{
      //   m_Scale.ResetContent();
      //   for(UINT i=nScaleFirst;i <= nScaleLast;i++)
      //   {
      //      CString str;
      //      VERIFY(str.LoadString(i));
      //      m_Scale.AddString(str);
      //   }
      //}

      //if(!m_bLastDocument)
      //{
      //   for(int i=0;gCtrlID[i];i++)
      //   {
      //      GetDlgItem(gCtrlID[i])->EnableWindow(TRUE);
      //   }
      //   m_bLastDocument=true;
      //}

      //LoadListBox(true);
      //OnSelchangeLayerList();
      //m_playerlistview->OnUpdateItemCount();
      m_playerheader->Refresh();
   }
   else
   {
      //CString strText;
      //strText.LoadString(IDS_LAYER_SETUP_NODOC);
      //AfxSetWindowText(GetSafeHwnd(),strText);
      //m_LayerName.SetWindowText("");
      //m_playerlistview->ResetContent();
      //m_Scale.ResetContent();
      //for(int i=0;gCtrlID[i];i++)
      //{
      //   GetDlgItem(gCtrlID[i])->EnableWindow(FALSE);
      //}
      //m_bLastDocument=false;
   }
   m_playerlistview->Refresh();
}

bool CLayerDoc::is_show_enabled()
{

   return m_playerlistview->is_show_enabled();

}


bool CLayerDoc::is_hide_enabled()
{

   return m_playerlistview->is_hide_enabled();

}


bool CLayerDoc::is_gray_enabled()
{

   return m_playerlistview->is_gray_enabled();

}

bool CLayerDoc::is_rename_enabled()
{

   return m_playerlistview->is_rename_enabled();

}







bool CLayerDoc::is_add_enabled()
{

   return m_playerlistview->is_add_enabled();

}


bool CLayerDoc::is_delete_enabled()
{

   return m_playerlistview->is_delete_enabled();

}


bool CLayerDoc::is_merge_enabled()
{

   return m_playerlistview->is_merge_enabled();

}



e_layer_show CLayerDoc::get_layer_show(int iItem)
{

   if(m_pDoc->m_Layers[iItem]->m_bHidden)
   {

      return layer_show_hidden;

   }
   else if(m_pDoc->m_Layers[iItem]->m_bGrayed)
   {

      return layer_show_grayed;

   }
   else
   {

      return layer_show_visible;

   }

}


void CLayerDoc::set_layer_show(int iItem, e_layer_show eshow)
{

   if(eshow == layer_show_visible)
   {

      m_pDoc->m_Layers[iItem]->m_bHidden = false;

      m_pDoc->m_Layers[iItem]->m_bGrayed = false;

   }
   else if(eshow == layer_show_grayed)
   {

      m_pDoc->m_Layers[iItem]->m_bHidden = false;

      m_pDoc->m_Layers[iItem]->m_bGrayed = true;

   }
   else if(eshow == layer_show_hidden)
   {

      m_pDoc->m_Layers[iItem]->m_bHidden = true;

      m_pDoc->m_Layers[iItem]->m_bGrayed = false;

   }

}






INT_PTR CLayerDoc::get_item_count()
{

   if(m_pDoc == NULL)
   {

      return 0;

   }

   return m_pDoc->m_Layers.GetCount();

}



e_layer_state CLayerDoc::get_layer_state(int iItem)
{

   if(m_pDoc->m_Layers[iItem] == m_pDoc->GetActiveLayer())
   {

      return layer_state_active;

   }
   else
   {

      return layer_state_locked;

   }

}



CString CLayerDoc::get_layer_name(int iItem)
{

   return m_pDoc->m_Layers[iItem]->m_strName;

}


int CLayerDoc::get_layer_scale(int iItem)
{

   return m_pDoc->m_Layers[iItem]->m_nScale;

}

CString CLayerDoc::get_layer_scale_text(int iItem)
{

   int nScaleFirst;

   if(m_pDoc->m_bEnglishUnits)
   {

      nScaleFirst=IDS_ENGLISH_SCALE_FIRST;

   }
   else
   {

      nScaleFirst=IDS_METRIC_SCALE_FIRST;

   }

   CString str;

   VERIFY(str.LoadString(nScaleFirst + get_layer_scale(iItem) - 1));

   return str;

}


CString CLayerDoc::get_layer_shape_count_text(int iItem)
{

   CString str;

   str.Format("%d",m_pDoc->m_Layers[iItem]->GetObjectCount(true));

   return str;

}
