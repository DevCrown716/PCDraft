// SearchBox.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "SearchBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchBox

IMPLEMENT_DYNAMIC(CSearchBox,CEdit)

CSearchBox::CSearchBox()
{
   m_rectBtn.SetRectEmpty();
   m_bIsButtonPressed = FALSE;
   m_bIsButtonHighlighted = FALSE;
   m_bIsButtonCaptured = FALSE;
   m_sizeImage = CSize(0,0);
   m_nBrowseButtonWidth = 20;
   m_bDefaultImage = TRUE;
}

CSearchBox::~CSearchBox()
{
}

BEGIN_MESSAGE_MAP(CSearchBox,CEdit)
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_NCCALCSIZE()
   ON_WM_NCPAINT()
   ON_WM_NCHITTEST()
   ON_WM_NCLBUTTONDBLCLK()
   ON_WM_NCMOUSEMOVE()
   ON_WM_CANCELMODE()
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
   ON_WM_RBUTTONUP()
   //ON_WM_PAINT()
   //ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchBox message handlers

void CSearchBox::OnLButtonUp(UINT nFlags,CPoint point)
{
   if(m_bIsButtonCaptured)
   {
      ReleaseCapture();

      m_bIsButtonPressed = FALSE;
      m_bIsButtonCaptured = FALSE;
      m_bIsButtonHighlighted = FALSE;

      RedrawWindow(NULL,NULL,RDW_FRAME | RDW_INVALIDATE);

      if(m_rectBtn.PtInRect(point))
      {
         OnBrowse();
      }

      return;
   }

   CEdit::OnLButtonUp(nFlags,point);
}

void CSearchBox::OnMouseMove(UINT nFlags,CPoint point)
{
   if(m_bIsButtonCaptured)
   {
      BOOL bIsButtonPressed = m_rectBtn.PtInRect(point);
      if(bIsButtonPressed != m_bIsButtonPressed)
      {
         m_bIsButtonPressed = bIsButtonPressed;
         RedrawWindow(NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
      }

      return;
   }

   if(m_bIsButtonHighlighted)
   {
      if(!m_rectBtn.PtInRect(point))
      {
         m_bIsButtonHighlighted = FALSE;
         ReleaseCapture();

         RedrawWindow(NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
      }
   }

   CEdit::OnMouseMove(nFlags,point);
}

void CSearchBox::OnNcCalcSize(BOOL bCalcValidRects,NCCALCSIZE_PARAMS FAR* lpncsp)
{
   CEdit::OnNcCalcSize(bCalcValidRects,lpncsp);

   lpncsp->rgrc[0].right -= m_nBrowseButtonWidth;
}

void CSearchBox::OnNcPaint()
{
   CEdit::OnNcPaint();

   CWindowDC dc(this);

   CRect rectClient;
   GetWindowRect(rectClient);
   ScreenToClient(rectClient);


   m_rectBtn = rectClient;
   m_rectBtn.left = m_rectBtn.right - m_nBrowseButtonWidth;
   m_rectBtn.OffsetRect(- rectClient.left,-rectClient.top);
   m_rectBtn.DeflateRect(1,1);


   OnDrawBrowseButton(&dc,m_rectBtn,m_bIsButtonPressed,m_bIsButtonHighlighted);

}

LRESULT CSearchBox::OnNcHitTest(CPoint point)
{
   CPoint ptClient = point;
   ScreenToClient(&ptClient);

   if(m_rectBtn.PtInRect(ptClient))
   {
      return HTCAPTION;
   }

   return CEdit::OnNcHitTest(point);
}

void CSearchBox::OnDrawBrowseButton(CDC* pDC,CRect rect,BOOL bIsButtonPressed,BOOL bHighlight)
{
   ASSERT_VALID(pDC);


   int iImage = 0;

   if(m_ImageBrowse.GetSafeHandle() != NULL)
   {
      CPoint ptImage;
      ptImage.x = rect.CenterPoint().x - m_sizeImage.cx / 2;
      ptImage.y = rect.CenterPoint().y - m_sizeImage.cy / 2;

      pDC->FillSolidRect(rect,GetSysColor(COLOR_WINDOW));
      m_ImageBrowse.Draw(pDC,iImage,ptImage,ILD_NORMAL);

      
   }
}

void CSearchBox::OnChangeLayout()
{
   ASSERT_VALID(this);
   ENSURE(GetSafeHwnd() != NULL);

   m_nBrowseButtonWidth = max(20,m_sizeImage.cx + 8);

   SetWindowPos(NULL,0,0,0,0,SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);

      GetWindowRect(m_rectBtn);
      m_rectBtn.left = m_rectBtn.right - m_nBrowseButtonWidth;

      ScreenToClient(&m_rectBtn);
}

void CSearchBox::OnNcLButtonDblClk(UINT /*nHitTest*/,CPoint /*point*/)
{
}

void CSearchBox::OnBrowse()
{
   

   SetFocus();
}


void CSearchBox::SetBrowseButtonImage(HICON hIcon,BOOL bAutoDestroy)
{
   if(m_ImageBrowse.GetSafeHandle() != NULL)
   {
      m_ImageBrowse.DeleteImageList();
   }

   if(hIcon == NULL)
   {
      m_sizeImage = CSize(0,0);
      return;
   }

   ICONINFO info;
   ::GetIconInfo(hIcon,&info);

   BITMAP bmp;
   ::GetObject(info.hbmColor,sizeof(BITMAP),(LPVOID)&bmp);

   m_sizeImage.cx = bmp.bmWidth;
   m_sizeImage.cy = bmp.bmHeight;

   ::DeleteObject(info.hbmColor);
   ::DeleteObject(info.hbmMask);

   UINT nFlags = ILC_MASK;

   switch(bmp.bmBitsPixel)
   {
   case 4:
   default:
      nFlags |= ILC_COLOR4;
      break;

   case 8:
      nFlags |= ILC_COLOR8;
      break;

   case 16:
      nFlags |= ILC_COLOR16;
      break;

   case 24:
      nFlags |= ILC_COLOR24;
      break;

   case 32:
      nFlags |= ILC_COLOR32;
      break;
   }

   m_ImageBrowse.Create(bmp.bmWidth,bmp.bmHeight,nFlags,0,0);
   m_ImageBrowse.Add(hIcon);

   m_bDefaultImage = FALSE;

   if(bAutoDestroy)
   {
      ::DestroyIcon(hIcon);
   }
}

void CSearchBox::SetBrowseButtonImage(HBITMAP hBitmap,BOOL bAutoDestroy)
{
   if(m_ImageBrowse.GetSafeHandle() != NULL)
   {
      m_ImageBrowse.DeleteImageList();
   }

   if(hBitmap == NULL)
   {
      m_sizeImage = CSize(0,0);
      return;
   }

   BITMAP bmp;
   ::GetObject(hBitmap,sizeof(BITMAP),(LPVOID)&bmp);

   m_sizeImage.cx = bmp.bmWidth;
   m_sizeImage.cy = bmp.bmHeight;

   UINT nFlags = ILC_MASK;

   switch(bmp.bmBitsPixel)
   {
   case 4:
   default:
      nFlags |= ILC_COLOR4;
      break;

   case 8:
      nFlags |= ILC_COLOR8;
      break;

   case 16:
      nFlags |= ILC_COLOR16;
      break;

   case 24:
      nFlags |= ILC_COLOR24;
      break;

   case 32:
      nFlags |= ILC_COLOR32;
      break;
   }

   m_ImageBrowse.Create(bmp.bmWidth,bmp.bmHeight,nFlags,0,0);

   HBITMAP hbmpCopy = (HBITMAP) ::CopyImage(hBitmap,IMAGE_BITMAP,0,0,0);
   m_ImageBrowse.Add(CBitmap::FromHandle(hbmpCopy),RGB(192,192,192));

   ::DeleteObject(hbmpCopy);

   m_bDefaultImage = FALSE;

   if(bAutoDestroy)
   {
      ::DeleteObject(hBitmap);
   }
}

void CSearchBox::SetBrowseButtonImage(UINT uiBmpResId)
{
   if(m_ImageBrowse.GetSafeHandle() != NULL)
   {
      m_ImageBrowse.DeleteImageList();
   }

   if(uiBmpResId == 0)
   {
      m_sizeImage = CSize(0,0);
      return;
   }

   CMFCToolBarImages images;
   if(!images.Load(uiBmpResId))
   {
      ASSERT(FALSE);
      return;
   }

   SetBrowseButtonImage((HBITMAP) ::CopyImage(images.GetImageWell(),IMAGE_BITMAP,0,0,0),TRUE /* bAutoDestroy */);
   m_bDefaultImage = FALSE;
}

void CSearchBox::SetInternalImage()
{
   if(m_ImageBrowse.GetSafeHandle() != NULL)
   {
      m_ImageBrowse.DeleteImageList();
   }

   UINT uiImageListResID = GetGlobalData()->Is32BitIcons() ? IDB_AFXBARRES_BROWSE32 : IDB_AFXBARRES_BROWSE;

   LPCTSTR lpszResourceName = MAKEINTRESOURCE(uiImageListResID);
   ENSURE(lpszResourceName != NULL);

   HBITMAP hbmp = (HBITMAP) ::LoadImage(
      AfxFindResourceHandle(lpszResourceName,RT_BITMAP),lpszResourceName,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);

   if(hbmp == NULL)
   {
      TRACE(_T("Can't load bitmap: %x\n"),uiImageListResID);
      return;
   }

   BITMAP bmpObj;
   ::GetObject(hbmp,sizeof(BITMAP),&bmpObj);

   UINT nFlags = ILC_MASK;

   switch(bmpObj.bmBitsPixel)
   {
   case 4:
   default:
      nFlags |= ILC_COLOR4;
      break;

   case 8:
      nFlags |= ILC_COLOR8;
      break;

   case 16:
      nFlags |= ILC_COLOR16;
      break;

   case 24:
      nFlags |= ILC_COLOR24;
      break;

   case 32:
      nFlags |= ILC_COLOR32;
      break;
   }

   m_ImageBrowse.Create(16,16,nFlags,0,0);
   m_ImageBrowse.Add(CBitmap::FromHandle(hbmp),RGB(255,0,255));

   m_sizeImage = CSize(16,16);
   m_bDefaultImage = TRUE;
}

void CSearchBox::OnAfterUpdate()
{
   if(GetOwner() == NULL)
   {
      return;
   }

   GetOwner()->PostMessage(EN_CHANGE,GetDlgCtrlID(),(LPARAM)GetSafeHwnd());
   GetOwner()->PostMessage(EN_UPDATE,GetDlgCtrlID(),(LPARAM)GetSafeHwnd());
}

void CSearchBox::OnNcMouseMove(UINT nHitTest,CPoint point)
{
   if(!m_bIsButtonCaptured)
   {
      CPoint ptClient = point;
      ScreenToClient(&ptClient);

      if(m_rectBtn.PtInRect(ptClient))
      {
         SetCapture();
         m_bIsButtonHighlighted = TRUE;

         RedrawWindow(NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
      }
   }

   CEdit::OnNcMouseMove(nHitTest,point);
}

void CSearchBox::OnCancelMode()
{
   CEdit::OnCancelMode();

   if(IsWindowEnabled())
   {
      ReleaseCapture();
   }

   m_bIsButtonPressed = FALSE;
   m_bIsButtonCaptured = FALSE;
   m_bIsButtonHighlighted = FALSE;

   RedrawWindow(NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
}

void CSearchBox::OnLButtonDown(UINT nFlags,CPoint point)
{

   if(m_rectBtn.PtInRect(point))
   {
      SetFocus();

      m_bIsButtonPressed = TRUE;
      m_bIsButtonCaptured = TRUE;

      SetCapture();

      RedrawWindow(NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
      return;
   }

   CEdit::OnLButtonDown(nFlags,point);
}

void CSearchBox::OnRButtonDown(UINT nFlags,CPoint point)
{
   if(m_rectBtn.PtInRect(point))
   {
      return;
   }

   CEdit::OnRButtonDown(nFlags,point);
}

void CSearchBox::OnRButtonUp(UINT nFlags,CPoint point)
{
   if(m_rectBtn.PtInRect(point))
   {
      return;
   }

   CEdit::OnRButtonUp(nFlags,point);
}

BOOL CSearchBox::PreTranslateMessage(MSG* pMsg)
{
   switch(pMsg->message)
   {
   case WM_SYSKEYDOWN:
      if((pMsg->wParam == VK_DOWN || pMsg->wParam == VK_RIGHT))
      {
         OnBrowse();
         return TRUE;
      }
      break;
   }

   return CEdit::PreTranslateMessage(pMsg);
}



void CSearchBox::OnPaint()
{
   CPaintDC dc(this); // device context for painting
   // TODO: Add your message handler code here
   // Do not call CEdit::OnPaint() for painting messages
}


BOOL CSearchBox::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
   return CEdit::OnEraseBkgnd(pDC);
}
