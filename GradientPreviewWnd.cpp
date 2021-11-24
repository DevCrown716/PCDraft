// EditPatternWnd.cpp : implementation file
// Infinisys Ltd
//

#include "stdafx.h"
#include "pcdraft.h"
#include "GradientPreviewWnd.h"
#include "GradientDlg.h"
#include "GradientPack.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void __copy(Gdiplus::RectF & r, LPCRECT lpcrect);


/////////////////////////////////////////////////////////////////////////////
// CGradientPreviewWnd

CGradientPreviewWnd::CGradientPreviewWnd()
{
   m_bTrack=false;
   m_bToggle=false;
   m_nConstraint=kNone;
   m_hThumb=NULL;
}

CGradientPreviewWnd::~CGradientPreviewWnd()
{
}


BEGIN_MESSAGE_MAP(CGradientPreviewWnd,CWnd)
   //{{AFX_MSG_MAP(CGradientPreviewWnd)
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_ERASEBKGND()
   ON_WM_SETCURSOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGradientPreviewWnd message handlers

void CGradientPreviewWnd::SetParentDlg(CGradientDlg *parentDlg)
{
   m_pParentDlg=parentDlg;
   m_BlendColor=-1;
   UpdateColor();
}

BOOL CGradientPreviewWnd::OnEraseBkgnd(CDC* pDC)
{
   return TRUE;
   CRect rc;
   GetClientRect(&rc);

   CGDIBackColor bc(pDC,GetSysColor(COLOR_BTNFACE));
   pDC->ExtTextOut(0,0,ETO_OPAQUE | ETO_CLIPPED,&rc,NULL,0,NULL);

   return CWnd::OnEraseBkgnd(pDC);
}

void CGradientPreviewWnd::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   CRect rc;
   GetClientRect(&rc);
   Gdiplus::GraphicsPath path;
   Gdiplus::RectF r;
   __copy(r, rc);
   path.AddRectangle(r);
   Gdiplus::Graphics g(dc.GetSafeHdc());

   ::std::unique_ptr < Gdiplus::Brush > brush;
   
   brush.reset(m_pParentDlg->m_ppack->proper_brush( rc, 255));

   g.FillPath(brush.get(), &path);

}


void CGradientPreviewWnd::defer_update_center(CPoint pt)
{
   if(!m_pParentDlg->m_ppack->m_bRadial)
   {
      return;
   }
   CRect rectClient;

   GetClientRect(rectClient);

   CPoint ptCenter = rectClient.CenterPoint();

   m_pParentDlg->m_ppack->m_dX = (double)(pt.x - ptCenter.x) / (double)rectClient.Width();
   m_pParentDlg->m_ppack->m_dY = (double)(pt.y - ptCenter.y) / (double)rectClient.Height();

   m_pParentDlg->defer_redraw();

}

void CGradientPreviewWnd::OnLButtonDown(UINT nFlags,CPoint pt)
{

   if(m_pParentDlg->m_ppack->m_bRadial)
   {
      defer_update_center(pt);
      m_bTrack = true;
      SetCapture();
   }
   //m_nEditX=m_nX;
   //m_nEditY=m_nY;
   //m_pParentDlg->SaveUndo();
   //if(ApplyTool())
   //{
   //   if(GetAsyncKeyState(VK_SHIFT)<0) m_nConstraint=kTest;
   //   m_bTrack=true;
   //   SetCapture();
   //}


}

void CGradientPreviewWnd::OnLButtonUp(UINT nFlags,CPoint pt)
{
   if(m_bTrack)
   {
         defer_update_center(pt);
      ReleaseCapture();
      m_bTrack=false;
   }
   //m_bToggle=false;
   //m_nConstraint=kNone;
}

void CGradientPreviewWnd::OnMouseMove(UINT nFlags,CPoint pt)
{
   if(m_bTrack)
   {
      defer_update_center(pt);
   }
   
   //int x=pt.x / (m_nSide + 1);
   //int y=pt.y / (m_nSide + 1);

   //if(m_nConstraint == kVertical) x=m_nEditX;
   //if(m_nConstraint == kHorizontal) y=m_nEditY;

   //if(x != m_nX || y != m_nY)
   //{
   //   m_nX=x;
   //   m_nY=y;
   //   if(m_bTrack)
   //   {
   //      if(m_nConstraint == kTest) m_nConstraint=(m_nX == m_nEditX)?kVertical:kHorizontal;

   //      ApplyTool();
   //   }
   //}
}

void CGradientPreviewWnd::UpdateEditInfo()
{
   //m_nEditSize=m_pParentDlg->m_nSize;
   ////	m_nSide=m_nEditSize*3/4-1;
   //switch(m_nEditSize)
   //{
   //case 8:
   //   m_nSide=23;
   //   break;
   //case 16:
   //   m_nSide=11;
   //   break;
   //case 32:
   //   m_nSide=5;
   //   break;
   //}
   //Invalidate();
}

bool CGradientPreviewWnd::ApplyTool()
{
   //bool bNeedCapture=true;

   //CDC memDC;
   //memDC.CreateCompatibleDC(NULL);
   //HGDIOBJ hOldBmp=memDC.SelectObject(m_pParentDlg->m_hEditPattern);

   //if(m_pParentDlg->m_nTool == IDC_TOOL_HAND)
   //{
   //   CDC srcDC;
   //   srcDC.CreateCompatibleDC(&memDC);
   //   HGDIOBJ hOldSrcBmp=srcDC.SelectObject(m_pParentDlg->m_hUndo);
   //   int dx=(m_nX - m_nEditX) % m_nEditSize;
   //   int dy=(m_nY - m_nEditY) % m_nEditSize;

   //   memDC.BitBlt(dx,dy,m_nEditSize,m_nEditSize,&srcDC,0,0,SRCCOPY);
   //   if(dx) memDC.BitBlt(dx>0?dx - m_nEditSize:m_nEditSize + dx,dy,m_nEditSize,m_nEditSize,&srcDC,0,0,SRCCOPY);
   //   if(dy)
   //   {
   //      dy=dy>0?dy - m_nEditSize:m_nEditSize + dy;
   //      memDC.BitBlt(dx,dy,m_nEditSize,m_nEditSize,&srcDC,0,0,SRCCOPY);
   //      if(dx) memDC.BitBlt(dx>0?dx - m_nEditSize:m_nEditSize + dx,dy,m_nEditSize,m_nEditSize,&srcDC,0,0,SRCCOPY);
   //   }
   //   srcDC.SelectObject(hOldSrcBmp);
   //}
   //else
   //{
   //   if(m_nX >= 0 && m_nX<m_nEditSize && m_nY >= 0 && m_nY<m_nEditSize)
   //   {
   //      COLORREF ptColor=memDC.GetPixel(m_nX,m_nY);
   //      if(!m_bTrack && ptColor == m_pParentDlg->m_Color) m_bToggle=true;

   //      if(GetAsyncKeyState(VK_OPTION)<0)
   //      {
   //         if(m_pParentDlg->m_nTool != IDC_TOOL_THUMB)
   //         {
   //            m_pParentDlg->SetColor(ptColor);
   //         }
   //         m_BlendColor=ptColor;
   //         UpdateThumbColor();
   //         bNeedCapture=false;
   //      }
   //      else
   //      {
   //         if(m_pParentDlg->m_nTool == IDC_TOOL_BUCKET)
   //         {
   //            memDC.SelectObject(hOldBmp);
   //            m_pParentDlg->Colorize(m_pParentDlg->m_Color);
   //            memDC.SelectObject(m_pParentDlg->m_hEditPattern);

   //            CBrush br(m_pParentDlg->m_Color);
   //            CGDIObject<CBrush> obr(&memDC,&br);
   //            memDC.ExtFloodFill(m_nX,m_nY,ptColor,FLOODFILLSURFACE);
   //            bNeedCapture=false;
   //         }
   //         else
   //         {
   //            if(GetAsyncKeyState(VK_COMMAND)<0) ptColor=RGB(255,255,255);
   //            else
   //            {
   //               if(m_pParentDlg->m_nTool == IDC_TOOL_THUMB)
   //               {
   //                  AverageColor(m_BlendColor,ptColor);
   //                  AverageColor(ptColor,m_BlendColor);
   //                  UpdateThumbColor();
   //               }
   //               else ptColor=(m_bToggle)?RGB(255,255,255):m_pParentDlg->m_Color;
   //            }
   //            memDC.SelectObject(hOldBmp);
   //            m_pParentDlg->Colorize(ptColor);
   //            memDC.SelectObject(m_pParentDlg->m_hEditPattern);

   //            memDC.SetPixel(m_nX,m_nY,ptColor);
   //         }
   //      }
   //   }
   //}
   //memDC.SelectObject(hOldBmp);
   //Invalidate(FALSE);
   //m_pParentDlg->m_preview.Invalidate();
   //return bNeedCapture;
   return false;
}

BOOL CGradientPreviewWnd::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message)
{
   HCURSOR hCur=NULL;

   //if(m_pParentDlg->m_nTool != IDC_TOOL_HAND && GetAsyncKeyState(VK_OPTION)<0) hCur=AfxGetApp()->LoadCursor(IDC_ATTRIB_PICK);
   //else
   //{
   //   switch(m_pParentDlg->m_nTool)
   //   {
   //   case IDC_TOOL_PEN:
   //      hCur=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
   //      break;
   //   case IDC_TOOL_THUMB:
   //      hCur=m_hThumb;
   //      break;
   //   case IDC_TOOL_BUCKET:
   //      hCur=AfxGetApp()->LoadCursor(IDC_BUCKET);
   //      break;
   //   case IDC_TOOL_HAND:
   //      hCur=AfxGetApp()->LoadCursor(IDC_HAND_DRAFT);
   //      break;
   //   }
   //}
   //if(hCur)
   //{
   //   SetCursor(hCur);
   //   return TRUE;
   //}
   //else
      return CWnd::OnSetCursor(pWnd,nHitTest,message);
}

void CGradientPreviewWnd::UpdateThumbColor()
{
   ICONINFO ii;
   COLORMAP cm;
   cm.from=RGB(255,255,255);
   cm.to=m_BlendColor;
   ii.hbmColor=CreateMappedBitmap(AfxGetResourceHandle(),IDB_THUMB,0,&cm,1);
   ii.hbmMask=LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_THUMB_MASK));
   ii.xHotspot=7;
   ii.yHotspot=7;
   ii.fIcon=FALSE;
   HCURSOR hThumb=CreateIconIndirect(&ii);
   DeleteObject(ii.hbmMask);
   DeleteObject(ii.hbmColor);

   //if(m_pParentDlg->m_nTool == IDC_TOOL_THUMB && GetAsyncKeyState(VK_OPTION) >= 0) SetCursor(hThumb);
   //if(m_hThumb) DestroyCursor(m_hThumb);
   m_hThumb=hThumb;
}

void CGradientPreviewWnd::UpdateColor()
{
   //if(m_BlendColor != m_pParentDlg->m_Color)
   //{
   //   m_BlendColor=m_pParentDlg->m_Color;
   //   UpdateThumbColor();
   //}
}

void CGradientPreviewWnd::AverageColor(COLORREF src,COLORREF &dst)
{
   dst=RGB((GetRValue(src) + GetRValue(dst)) / 2,(GetGValue(src) + GetGValue(dst)) / 2,(GetBValue(src) + GetBValue(dst)) / 2);
}
