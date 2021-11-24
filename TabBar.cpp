#include "stdafx.h"
#include "PCDraftRibbon.h"
#include "MainFrm.h"
#include "ribbon.h"
#include <Propvarutil.h>
using namespace Gdiplus;
Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst);
void copy(Gdiplus::Rect & r,LPCRECT lpcrect);

void copy(Gdiplus::RectF & r,LPCRECT lpcrect);


CTabBar::CTabBar() :
m_font2(L"Segoe UI",13.0,0,Gdiplus::UnitPixel)
{
   m_iPreviousCount = 0;
   LOGFONTW lfw;

   {

      HDC hdc = ::GetDC(NULL);

      {

         Gdiplus::Graphics g(hdc);

         m_font2.GetLogFontW(&g,&lfw);

         m_font.Attach(::CreateFontIndirectW(&lfw));

      }

      ::ReleaseDC(NULL,hdc);

   }

   m_bExpanded = true;
   m_iMargin = 0;

   m_format.SetAlignment(Gdiplus::StringAlignmentNear);
   m_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
   m_format.SetTrimming(Gdiplus::StringTrimmingWord);
   m_format.SetFormatFlags(0);

   m_formatTab.SetAlignment(Gdiplus::StringAlignmentNear);
   m_formatTab.SetLineAlignment(Gdiplus::StringAlignmentCenter);
   m_formatTab.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
   m_formatTab.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

   m_ulRibbonHeight = 1;
   m_rIconMargin.left = 0;
   m_rIconMargin.right = 0;
   m_rIconMargin.top = 15;
   m_rIconMargin.bottom = 5;

   m_rIconBorder.left = 8;
   m_rIconBorder.right = 8;
   m_rIconBorder.top = 5;
   m_rIconBorder.bottom = 5;
   m_rTabBorder.left =20;
   m_rTabBorder.right =20;
   m_rTabBorder.top =2;
   m_rTabBorder.bottom =5;

   m_iTabSel = 0;


   // initialize state
   m_pStringMap = NULL;
   m_hRsrcImageWell = NULL;
   m_hInstImageWell = NULL;
   m_hbmImageWell = NULL;
   m_bDelayedButtonLayout = TRUE;

   // default image sizes
   m_sizeImage.cx = 16;
   m_sizeImage.cy = 15;

   // default button sizes
   m_sizeButton.cx = 23;
   m_sizeButton.cy = 22;

   // top and bottom borders are 1 larger than default for ease of grabbing
   m_cyTopBorder = 3;
   m_cyBottomBorder = 3;
   EnableActiveAccessibility();

   m_iTabScroll = 0;

}



CTabBar::~CTabBar()
{
   if(m_hbmImageWell)
   {
      ::DeleteObject(m_hbmImageWell);
      m_hbmImageWell = NULL;
   }
   delete m_pStringMap;

   m_nCount = 0;
}

BOOL CTabBar::Create(CWnd* pParentWnd,DWORD dwStyle,UINT nID)
{
   return CreateEx(pParentWnd,0,dwStyle,
      CRect(m_cxLeftBorder,m_cyTopBorder,m_cxRightBorder,m_cyBottomBorder),nID);
}

BOOL CTabBar::CreateEx(CWnd* pParentWnd,DWORD dwCtrlStyle,DWORD dwStyle,CRect rcBorders,UINT nID)
{
   ASSERT_VALID(pParentWnd);   // must have a parent
   ASSERT(!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));

   SetBorders(rcBorders);

   // save the style
   m_dwStyle = (dwStyle & CBRS_ALL);
   if(nID == AFX_IDW_TOOLBAR)
      m_dwStyle |= CBRS_HIDE_INPLACE;

   dwStyle &= ~CBRS_ALL;
   dwStyle |= CCS_NOPARENTALIGN | CCS_NOMOVEY | CCS_NODIVIDER | CCS_NORESIZE;
   dwStyle |= dwCtrlStyle;
   m_bNeedLayout = true;
   // create the HWND
   CRect rect; rect.SetRectEmpty();
   if(!CWnd::Create(NULL,NULL,dwStyle,rect,pParentWnd,nID))
      return FALSE;

   // sync up the sizes
   SetSizes(m_sizeButton,m_sizeImage);
   char sz[2];

   sz[0] = 231;
   sz[1] = '\0';
   m_buttonTabLeft.Create(NULL,WS_CHILD | BS_OWNERDRAW,CRect(0,0,16,100),this,ID_TAB_LEFT_ARROW);
   m_buttonTabLeft.EnableWindow();
   m_buttonTabLeft.m_etype = CArrowButton::type_left;
   m_buttonTabLeft.m_bTimer = false;

   m_buttonTabRight.Create(NULL,WS_CHILD | BS_OWNERDRAW,CRect(64,0,64 + 16,100),this,ID_TAB_RIGHT_ARROW);
   m_buttonTabRight.EnableWindow();
   m_buttonTabRight.m_etype = CArrowButton::type_right;
   m_buttonTabRight.m_bTimer = false;
   // Note: Parent must resize itself for control bar to be resized
   SetTimer(1000,100,NULL);
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTabBar

BOOL CTabBar::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(!CControlBar::OnNcCreate(lpCreateStruct))
      return FALSE;

   // if the owner was set before the toolbar was created, set it now
   if(m_hWndOwner != NULL)
      DefWindowProc(TB_SETPARENT,(WPARAM)m_hWndOwner,0);

   DefWindowProc(TB_BUTTONSTRUCTSIZE,(WPARAM)sizeof(TBBUTTON),0);
   return TRUE;
}

void CTabBar::SetOwner(CWnd* pOwnerWnd)
{
   ASSERT_VALID(this);
   if(m_hWnd != NULL)
   {
      ASSERT(::IsWindow(m_hWnd));
      DefWindowProc(TB_SETPARENT,(WPARAM)pOwnerWnd->GetSafeHwnd(),0);
   }
   CControlBar::SetOwner(pOwnerWnd);
}

void CTabBar::SetSizes(SIZE sizeButton,SIZE sizeImage)
{
   ASSERT_VALID(this);

   // sizes must be non-zero and positive
   ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);
   ASSERT(sizeImage.cx > 0 && sizeImage.cy > 0);

   // button must be big enough to hold image
   //   + 7 pixels on x
   //   + 6 pixels on y
   ASSERT(sizeButton.cx >= sizeImage.cx + 7);
   ASSERT(sizeButton.cy >= sizeImage.cy + 6);

   if(::IsWindow(m_hWnd))
   {
      // set the sizes via TB_SETBITMAPSIZE and TB_SETBUTTONSIZE
      //VERIFY(SendMessage(TB_SETBITMAPSIZE,0,MAKELONG(sizeImage.cx,sizeImage.cy)));
      //VERIFY(SendMessage(TB_SETBUTTONSIZE,0,MAKELONG(sizeButton.cx,sizeButton.cy)));
      //if(_afxComCtlVersion >= VERSION_6)
      //{
      //   DWORD dwSize = (DWORD)SendMessage(TB_GETBUTTONSIZE,0,0);
      //   m_sizeButton.cx = LOWORD(dwSize);
      //   m_sizeButton.cy = HIWORD(dwSize);
      //}
      Invalidate();   // just to be nice if called when toolbar is visible
   }
   else
   {
      // just set our internal values for later
      m_sizeButton = sizeButton;
      m_sizeImage = sizeImage;
   }
}

void CTabBar::SetHeight(int cyHeight)
{
   ASSERT_VALID(this);

   int nHeight = cyHeight;
   //   if(m_dwStyle & CBRS_BORDER_TOP)
   //    cyHeight -= afxData.cyBorder2;
   // if(m_dwStyle & CBRS_BORDER_BOTTOM)
   //  cyHeight -= afxData.cyBorder2;
   m_cyBottomBorder = (cyHeight - m_sizeButton.cy) / 2;
   // if there is an extra pixel, m_cyTopBorder will get it
   m_cyTopBorder = cyHeight - m_sizeButton.cy - m_cyBottomBorder;
   if(m_cyTopBorder < 0)
   {
      TRACE(traceAppMsg,0,"Warning: CTabBar::SetHeight(%d) is smaller than button.\n",
         nHeight);
      m_cyBottomBorder += m_cyTopBorder;
      m_cyTopBorder = 0;  // will clip at bottom
   }

   // recalculate the non-client region
   SetWindowPos(NULL,0,0,0,0,
      SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
   Invalidate();   // just to be nice if called when toolbar is visible
}

struct CTabBarData
{
   WORD wVersion;
   WORD wWidth;
   WORD wHeight;
   WORD wItemCount;
   //WORD aItems[wItemCount]

   WORD* items()
   {
      return (WORD*)(this + 1);
   }
};

BOOL CTabBar::LoadToolBar(LPCTSTR lpszResourceName)
{
   ASSERT_VALID(this);
   ASSERT(lpszResourceName != NULL);

   // determine location of the bitmap in resource fork
   HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName,RT_TOOLBAR);
   HRSRC hRsrc = ::FindResource(hInst,lpszResourceName,RT_TOOLBAR);
   if(hRsrc == NULL)
      return FALSE;

   HGLOBAL hGlobal = LoadResource(hInst,hRsrc);
   if(hGlobal == NULL)
      return FALSE;

   CTabBarData* pData = (CTabBarData*)LockResource(hGlobal);
   if(pData == NULL)
      return FALSE;
   ASSERT(pData->wVersion == 1);

   UINT* pItems = new UINT[pData->wItemCount];
   for(int i = 0; i < pData->wItemCount; i++)
      pItems[i] = pData->items()[i];
   BOOL bResult = SetButtons(pItems,pData->wItemCount);
   delete[] pItems;

   if(bResult)
   {
      // set new sizes of the buttons
      CSize sizeImage(pData->wWidth,pData->wHeight);
      CSize sizeButton(pData->wWidth + 7,pData->wHeight + 7);
      SetSizes(sizeButton,sizeImage);

      // load bitmap now that sizes are known by the toolbar control
      bResult = LoadBitmap(lpszResourceName);
   }

   UnlockResource(hGlobal);
   FreeResource(hGlobal);

   return bResult;
}

BOOL CTabBar::LoadBitmap(LPCTSTR lpszResourceName)
{
   ASSERT_VALID(this);
   ASSERT(lpszResourceName != NULL);

   // determine location of the bitmap in resource fork
   HINSTANCE hInstImageWell = AfxFindResourceHandle(lpszResourceName,RT_BITMAP);
   HRSRC hRsrcImageWell = ::FindResource(hInstImageWell,lpszResourceName,RT_BITMAP);
   if(hRsrcImageWell == NULL)
      return FALSE;

   // load the bitmap
   HBITMAP hbmImageWell;
   hbmImageWell = AfxLoadSysColorBitmap(hInstImageWell,hRsrcImageWell);

   // tell common control toolbar about the new bitmap
   if(!AddReplaceBitmap(hbmImageWell))
      return FALSE;

   // remember the resource handles so the bitmap can be recolored if necessary
   m_hInstImageWell = hInstImageWell;
   m_hRsrcImageWell = hRsrcImageWell;
   return TRUE;
}

BOOL CTabBar::SetBitmap(HBITMAP hbmImageWell)
{
   ASSERT_VALID(this);
   ASSERT(hbmImageWell != NULL);

   // the caller must manage changing system colors
   m_hInstImageWell = NULL;
   m_hRsrcImageWell = NULL;

   // tell common control toolbar about the new bitmap
   return AddReplaceBitmap(hbmImageWell);
}

BOOL CTabBar::AddReplaceBitmap(HBITMAP hbmImageWell)
{
   // need complete bitmap size to determine number of images
   BITMAP bitmap;
   VERIFY(::GetObject(hbmImageWell,sizeof(BITMAP),&bitmap));

   // add the bitmap to the common control toolbar
   BOOL bResult;
   if(m_hbmImageWell == NULL)
   {
      TBADDBITMAP addBitmap;
      addBitmap.hInst = NULL; // makes TBADDBITMAP::nID behave a HBITMAP
      addBitmap.nID = (UINT_PTR)hbmImageWell;
      bResult =  DefWindowProc(TB_ADDBITMAP,
         bitmap.bmWidth / m_sizeImage.cx,(LPARAM)&addBitmap) == 0;
   }
   else
   {
      TBREPLACEBITMAP replaceBitmap;
      replaceBitmap.hInstOld = NULL;
      replaceBitmap.nIDOld = (UINT_PTR)m_hbmImageWell;
      replaceBitmap.hInstNew = NULL;
      replaceBitmap.nIDNew = (UINT_PTR)hbmImageWell;
      replaceBitmap.nButtons = bitmap.bmWidth / m_sizeImage.cx;
      bResult = (BOOL)DefWindowProc(TB_REPLACEBITMAP,0,
         (LPARAM)&replaceBitmap);
   }
   // remove old bitmap, if present
   if(bResult)
   {
      if(m_hbmImageWell != NULL)
      {
         ::DeleteObject(m_hbmImageWell);
      }
      m_hbmImageWell = hbmImageWell;
   }

   return bResult;
}

BOOL CTabBar::SetButtons(const UINT* lpIDArray,int nIDCount)
{
   ASSERT_VALID(this);
   ASSERT(nIDCount >= 1);  // must be at least one of them
   ASSERT(lpIDArray == NULL ||
      AfxIsValidAddress(lpIDArray,sizeof(UINT) * nIDCount,FALSE));

   // delete all existing buttons
   int nCount = (int)DefWindowProc(TB_BUTTONCOUNT,0,0);
   while(nCount--)
      VERIFY(DefWindowProc(TB_DELETEBUTTON,0,0));

   TBBUTTON button; memset(&button,0,sizeof(TBBUTTON));
   button.iString = -1;
   if(lpIDArray != NULL)
   {
      // add new buttons to the common control
      int iImage = 0;
      for(int i = 0; i < nIDCount; i++)
      {
         button.fsState = TBSTATE_ENABLED;
         if((button.idCommand = *lpIDArray++) == 0)
         {
            // separator
            button.fsStyle = TBSTYLE_SEP;
            // width of separator includes 8 pixel overlap
            if((GetStyle() & TBSTYLE_FLAT))
               button.iBitmap = 6;
            else
               button.iBitmap = 8;
         }
         else
         {
            // a command button with image
            button.fsStyle = TBSTYLE_BUTTON;
            button.iBitmap = iImage++;
         }
         if(!DefWindowProc(TB_ADDBUTTONS,1,(LPARAM)&button))
            return FALSE;
      }
   }
   else
   {
      // add 'blank' buttons
      button.fsState = TBSTATE_ENABLED;
      for(int i = 0; i < nIDCount; i++)
      {
         ASSERT(button.fsStyle == TBSTYLE_BUTTON);
         if(!DefWindowProc(TB_ADDBUTTONS,1,(LPARAM)&button))
            return FALSE;
      }
   }
   m_nCount = (int)DefWindowProc(TB_BUTTONCOUNT,0,0);
   m_bDelayedButtonLayout = TRUE;

   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CTabBar attribute access

void CTabBar::_GetButton(int nIndex,TBBUTTON* pButton) const
{
   CTabBar* pBar = (CTabBar*)this;
   VERIFY(pBar->DefWindowProc(TB_GETBUTTON,nIndex,(LPARAM)pButton));
   // TBSTATE_ENABLED == TBBS_DISABLED so invert it
   pButton->fsState ^= TBSTATE_ENABLED;
}

void CTabBar::_SetButton(int nIndex,TBBUTTON* pButton)
{
   // get original button state
   TBBUTTON button;
   VERIFY(DefWindowProc(TB_GETBUTTON,nIndex,(LPARAM)&button));

   // prepare for old/new button comparsion
   button.bReserved[0] = 0;
   button.bReserved[1] = 0;
   // TBSTATE_ENABLED == TBBS_DISABLED so invert it
   pButton->fsState ^= TBSTATE_ENABLED;
   pButton->bReserved[0] = 0;
   pButton->bReserved[1] = 0;

   // nothing to do if they are the same
   if(memcmp(pButton,&button,sizeof(TBBUTTON)) != 0)
   {
      // don't redraw everything while setting the button
      DWORD dwStyle = GetStyle();
      ModifyStyle(WS_VISIBLE,0);
      VERIFY(DefWindowProc(TB_DELETEBUTTON,nIndex,0));

      // Force a recalc of the toolbar's layout to work around a comctl bug
      if(pButton->iString < -1){
         int iTextRows;
         iTextRows = (int)::SendMessage(m_hWnd,TB_GETTEXTROWS,0,0);
         ::SendMessage(m_hWnd,WM_SETREDRAW,FALSE,0);
         ::SendMessage(m_hWnd,TB_SETMAXTEXTROWS,iTextRows + 1,0);
         ::SendMessage(m_hWnd,TB_SETMAXTEXTROWS,iTextRows,0);
         ::SendMessage(m_hWnd,WM_SETREDRAW,TRUE,0);
      }
      VERIFY(DefWindowProc(TB_INSERTBUTTON,nIndex,(LPARAM)pButton));
      ModifyStyle(0,dwStyle & WS_VISIBLE);

      // invalidate appropriate parts
      if(((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
         ((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
      {
         // changing a separator
         Invalidate();
      }
      else
      {
         // invalidate just the button
         CRect rect;
         if(DefWindowProc(TB_GETITEMRECT,nIndex,(LPARAM)&rect))
            InvalidateRect(rect);
      }
   }
}

int CTabBar::CommandToIndex(UINT nIDFind) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   CTabBar* pBar = (CTabBar*)this;
   return (int)pBar->DefWindowProc(TB_COMMANDTOINDEX,nIDFind,0);
}

UINT CTabBar::GetItemID(int nIndex) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   return button.idCommand;
}

void CTabBar::GetItemRect(int nIndex,LPRECT lpRect) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // handle any delayed layout
   if(m_bDelayedButtonLayout)
      ((CTabBar*)this)->Layout();

   // now it is safe to get the item rectangle
   CTabBar* pBar = (CTabBar*)this;
   if(!pBar->DefWindowProc(TB_GETITEMRECT,nIndex,(LPARAM)lpRect))
      SetRectEmpty(lpRect);
}

void CTabBar::Layout()
{
   ASSERT(m_bDelayedButtonLayout);

   m_bDelayedButtonLayout = FALSE;

   BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
   if((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
      ((CTabBar*)this)->CalcDynamicLayout(0,LM_HORZ | LM_MRUWIDTH | LM_COMMIT);
   else if(bHorz)
      ((CTabBar*)this)->CalcDynamicLayout(0,LM_HORZ | LM_HORZDOCK | LM_COMMIT);
   else
      ((CTabBar*)this)->CalcDynamicLayout(0,LM_VERTDOCK | LM_COMMIT);
}

UINT CTabBar::GetButtonStyle(int nIndex) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   return MAKELONG(button.fsStyle,button.fsState);
}

void CTabBar::SetButtonStyle(int nIndex,UINT nStyle)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   if(button.fsStyle != (BYTE)LOWORD(nStyle) || button.fsState != (BYTE)HIWORD(nStyle))
   {
      button.fsStyle = (BYTE)LOWORD(nStyle);
      button.fsState = (BYTE)HIWORD(nStyle);
      _SetButton(nIndex,&button);
      m_bDelayedButtonLayout = TRUE;
   }
}

#define CX_OVERLAP  0

CSize CTabBar::CalcSize(TBBUTTON* pData,int nCount)
{
   ASSERT(pData != NULL && nCount > 0);

   CPoint cur(0,0);
   CSize sizeResult(0,0);

   DWORD dwExtendedStyle = DWORD(DefWindowProc(TB_GETEXTENDEDSTYLE,0,0));

   for(int i = 0; i < nCount; i++)
   {
      //  The IE4 version of COMCTL32.DLL calculates the separation
      //	on a TBSTYLE_WRAP button as 100% of the value in iBitmap compared
      //	to the other versions which calculate it at 2/3 of that value.
      //	This is actually a bug which should be fixed in IE 4.01, so we
      //	only do the 100% calculation specifically for IE4.
      int cySep = pData[i].iBitmap;
      if(!(GetStyle() & TBSTYLE_FLAT))
         cySep = cySep * 2 / 3;

      if(pData[i].fsState & TBSTATE_HIDDEN)
         continue;

      int cx = m_sizeButton.cx;
      if(pData[i].fsStyle & TBSTYLE_SEP)
      {
         // a separator represents either a height or width
         if(pData[i].fsState & TBSTATE_WRAP)
            sizeResult.cy = max(cur.y + m_sizeButton.cy + cySep,sizeResult.cy);
         else
            sizeResult.cx = max(cur.x + pData[i].iBitmap,sizeResult.cx);
      }
      else
      {
         // check for dropdown style, but only if the buttons are being drawn
         if((pData[i].fsStyle & TBSTYLE_DROPDOWN) &&
            (dwExtendedStyle & TBSTYLE_EX_DRAWDDARROWS))
         {
         }
         sizeResult.cx = max(cur.x + cx,sizeResult.cx);
         sizeResult.cy = max(cur.y + m_sizeButton.cy,sizeResult.cy);
      }

      if(pData[i].fsStyle & TBSTYLE_SEP)
         cur.x += pData[i].iBitmap;
      else
         cur.x += cx - CX_OVERLAP;

      if(pData[i].fsState & TBSTATE_WRAP)
      {
         cur.x = 0;
         cur.y += m_sizeButton.cy;
         if(pData[i].fsStyle & TBSTYLE_SEP)
            cur.y += cySep;
      }
   }
   return sizeResult;
}

int CTabBar::WrapToolBar(TBBUTTON* pData,int nCount,int nWidth)
{
   ASSERT(pData != NULL && nCount > 0);

   int nResult = 0;
   int x = 0;
   for(int i = 0; i < nCount; i++)
   {
      pData[i].fsState &= ~TBSTATE_WRAP;

      if(pData[i].fsState & TBSTATE_HIDDEN)
         continue;

      int dx,dxNext;
      if(pData[i].fsStyle & TBSTYLE_SEP)
      {
         dx = pData[i].iBitmap;
         dxNext = dx;
      }
      else
      {
         dx = m_sizeButton.cx;
         dxNext = dx - CX_OVERLAP;
      }

      if(x + dx > nWidth)
      {
         BOOL bFound = FALSE;
         for(int j = i; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
         {
            // Find last separator that isn't hidden
            // a separator that has a command ID is not
            // a separator, but a custom control.
            if((pData[j].fsStyle & TBSTYLE_SEP) &&
               (pData[j].idCommand == 0) &&
               !(pData[j].fsState & TBSTATE_HIDDEN))
            {
               bFound = TRUE; i = j; x = 0;
               pData[j].fsState |= TBSTATE_WRAP;
               nResult++;
               break;
            }
         }
         if(!bFound)
         {
            for(int j = i - 1; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
            {
               // Never wrap anything that is hidden,
               // or any custom controls
               if((pData[j].fsState & TBSTATE_HIDDEN) ||
                  ((pData[j].fsStyle & TBSTYLE_SEP) &&
                  (pData[j].idCommand != 0)))
                  continue;

               bFound = TRUE; i = j; x = 0;
               pData[j].fsState |= TBSTATE_WRAP;
               nResult++;
               break;
            }
            if(!bFound)
               x += dxNext;
         }
      }
      else
         x += dxNext;
   }
   return nResult + 1;
}

void  CTabBar::SizeToolBar(TBBUTTON* pData,int nCount,int nLength,BOOL bVert)
{
   ASSERT(pData != NULL && nCount > 0);

   if(!bVert)
   {
      int nMin,nMax,nTarget,nCurrent,nMid;

      // Wrap ToolBar as specified
      nMax = nLength;
      nTarget = WrapToolBar(pData,nCount,nMax);

      // Wrap ToolBar vertically
      nMin = 0;
      nCurrent = WrapToolBar(pData,nCount,nMin);

      if(nCurrent != nTarget)
      {
         while(nMin < nMax)
         {
            nMid = (nMin + nMax) / 2;
            nCurrent = WrapToolBar(pData,nCount,nMid);

            if(nCurrent == nTarget)
               nMax = nMid;
            else
            {
               if(nMin == nMid)
               {
                  WrapToolBar(pData,nCount,nMax);
                  break;
               }
               nMin = nMid;
            }
         }
      }
      CSize size = CalcSize(pData,nCount);
      WrapToolBar(pData,nCount,size.cx);
   }
   else
   {
      CSize sizeMax,sizeMin,sizeMid;

      // Wrap ToolBar vertically
      WrapToolBar(pData,nCount,0);
      sizeMin = CalcSize(pData,nCount);

      // Wrap ToolBar horizontally
      WrapToolBar(pData,nCount,32767);
      sizeMax = CalcSize(pData,nCount);

      while(sizeMin.cx < sizeMax.cx)
      {
         sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
         WrapToolBar(pData,nCount,sizeMid.cx);
         sizeMid = CalcSize(pData,nCount);

         if(nLength < sizeMid.cy)
         {
            if(sizeMin == sizeMid)
            {
               WrapToolBar(pData,nCount,sizeMax.cx);
               return;
            }
            sizeMin = sizeMid;
         }
         else if(nLength > sizeMid.cy)
         {
            if(sizeMax == sizeMid)
            {
               WrapToolBar(pData,nCount,sizeMin.cx);
               return;
            }
            sizeMax = sizeMid;
         }
         else
            return;
      }
   }
}

struct _AFX_CONTROLPOS
{
   int nIndex,nID;
   CRect rectOldPos;
};

CSize CTabBar::CalcLayout(DWORD dwMode,int nLength)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));
   if(dwMode & LM_HORZDOCK)
      ASSERT(dwMode & LM_HORZ);

   int nCount;
   TBBUTTON* pData = NULL;
   CSize sizeResult(0,0);

   //BLOCK: Load Buttons
   {
      nCount = int(DefWindowProc(TB_BUTTONCOUNT,0,0));
      if(nCount != 0)
      {
         int i;
         pData = new TBBUTTON[nCount];
         for(i = 0; i < nCount; i++)
            _GetButton(i,&pData[i]);
      }
   }

   if(nCount > 0)
   {
      if(!(m_dwStyle & CBRS_SIZE_FIXED))
      {
         BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

         if(bDynamic && (dwMode & LM_MRUWIDTH))
            SizeToolBar(pData,nCount,m_nMRUWidth);
         else if(bDynamic && (dwMode & LM_HORZDOCK))
            SizeToolBar(pData,nCount,32767);
         else if(bDynamic && (dwMode & LM_VERTDOCK))
            SizeToolBar(pData,nCount,0);
         else if(bDynamic && (nLength != -1))
         {
            CRect rect; rect.SetRectEmpty();
            CalcInsideRect(rect,(dwMode & LM_HORZ));
            BOOL bVert = (dwMode & LM_LENGTHY);
            int nLen = nLength + (bVert ? rect.Height() : rect.Width());

            SizeToolBar(pData,nCount,nLen,bVert);
         }
         else if(bDynamic && (m_dwStyle & CBRS_FLOATING))
            SizeToolBar(pData,nCount,m_nMRUWidth);
         else
            SizeToolBar(pData,nCount,(dwMode & LM_HORZ) ? 32767 : 0);
      }

      sizeResult = CalcSize(pData,nCount);

      if(dwMode & LM_COMMIT)
      {
         _AFX_CONTROLPOS* pControl = NULL;
         int nControlCount = 0;
         BOOL bIsDelayed = m_bDelayedButtonLayout;
         m_bDelayedButtonLayout = FALSE;
         int i;

         for(i = 0; i < nCount; i++)
            if((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
               nControlCount++;

         if(nControlCount > 0)
         {
            pControl = new _AFX_CONTROLPOS[nControlCount];
            int nControlAlloc=nControlCount;
            nControlCount = 0;

            for(i = 0; i < nCount && nControlCount < nControlAlloc; i++)
            {
               if((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
               {
                  pControl[nControlCount].nIndex = i;
                  pControl[nControlCount].nID = pData[i].idCommand;

                  CRect rect;
                  GetItemRect(i,&rect);
                  ClientToScreen(&rect);
                  pControl[nControlCount].rectOldPos = rect;

                  nControlCount++;
               }
            }
         }

         if((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
            m_nMRUWidth = sizeResult.cx;
         for(i = 0; i < nCount; i++)
            _SetButton(i,&pData[i]);

         if(nControlCount > 0)
         {
            for(i = 0; i < nControlCount; i++)
            {
               CWnd* pWnd = GetDlgItem(pControl[i].nID);
               if(pWnd != NULL)
               {
                  CRect rect;
                  pWnd->GetWindowRect(&rect);
                  CPoint pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
                  GetItemRect(pControl[i].nIndex,&rect);
                  pt = rect.TopLeft() + pt;
                  pWnd->SetWindowPos(NULL,pt.x,pt.y,0,0,SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
               }
            }
            delete[] pControl;
         }
         m_bDelayedButtonLayout = bIsDelayed;
      }
      delete[] pData;
   }

   //BLOCK: Adjust Margins
   {
      CRect rect; rect.SetRectEmpty();
      CalcInsideRect(rect,(dwMode & LM_HORZ));
      sizeResult.cy -= rect.Height();
      sizeResult.cx -= rect.Width();

      CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH),(dwMode & LM_HORZ));
      sizeResult.cx = max(sizeResult.cx,size.cx);
      sizeResult.cy = max(sizeResult.cy,size.cy);
   }
   return sizeResult;
}

CSize CTabBar::CalcFixedLayout(BOOL bStretch,BOOL bHorz)
{
   DWORD dwMode = bStretch ? LM_STRETCH : 0;
   dwMode |= bHorz ? LM_HORZ : 0;
   CSize size = CalcLayout(dwMode);
   size.cy = m_ulRibbonHeight;
   return size;
}

CSize CTabBar::CalcDynamicLayout(int nLength,DWORD dwMode)
{
   if((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
      ((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
   {
      return CalcFixedLayout(dwMode & LM_STRETCH,dwMode & LM_HORZDOCK);
   }
   return CalcLayout(dwMode,nLength);
}

void CTabBar::GetButtonInfo(int nIndex,UINT& nID,UINT& nStyle,int& iImage) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   nID = button.idCommand;
   nStyle = MAKELONG(button.fsStyle,button.fsState);
   iImage = button.iBitmap;
}

void CTabBar::SetButtonInfo(int nIndex,UINT nID,UINT nStyle,int iImage)
{
   ASSERT_VALID(this);

   TBBUTTON button;
   _GetButton(nIndex,&button);
   TBBUTTON save;

   Checked::memcpy_s(&save,sizeof(TBBUTTON),&button,sizeof(TBBUTTON));
   button.idCommand = nID;
   button.iBitmap = iImage;
   button.fsStyle = (BYTE)LOWORD(nStyle);
   button.fsState = (BYTE)HIWORD(nStyle);
   if(memcmp(&save,&button,sizeof(save)) != 0)
   {
      _SetButton(nIndex,&button);
      m_bDelayedButtonLayout = TRUE;
   }
}

INT_PTR CTabBar::OnToolHitTest(CPoint point,TOOLINFO* pTI) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // check child windows first by calling CControlBar
   INT_PTR nHit = CControlBar::OnToolHitTest(point,pTI);
   if(nHit != -1)
      return nHit;

   // now hit test against CTabBar buttons
   CTabBar* pBar = (CTabBar*)this;
   int nButtons = (int)pBar->DefWindowProc(TB_BUTTONCOUNT,0,0);
   for(int i = 0; i < nButtons; i++)
   {
      CRect rect;
      TBBUTTON button;
      if(pBar->DefWindowProc(TB_GETITEMRECT,i,(LPARAM)&rect))
      {
         ++rect.bottom;
         ++rect.right;
         if(rect.PtInRect(point) &&
            pBar->DefWindowProc(TB_GETBUTTON,i,(LPARAM)&button) &&
            !(button.fsStyle & TBSTYLE_SEP))
         {
            UINT_PTR uHit = GetItemID(i);
            if(pTI != NULL)
            {
               pTI->hwnd = m_hWnd;
               pTI->rect = rect;
               pTI->uId = uHit;
               pTI->lpszText = LPSTR_TEXTCALLBACK;
            }
            // found matching rect, return the ID of the button
            return uHit != 0 ? static_cast<INT_PTR>(uHit) : static_cast<INT_PTR>(-1);
         }
      }
   }
   return -1;
}

BOOL CTabBar::SetButtonText(int nIndex,LPCTSTR lpszText)
{
   // attempt to lookup string index in map
   INT_PTR nString = -1;
   void* p;
   if(m_pStringMap != NULL && m_pStringMap->Lookup(lpszText,p))
      nString = (INT_PTR)p;

   // add new string if not already in map
   if(nString == -1)
   {
      // initialize map if necessary
      if(m_pStringMap == NULL)
         m_pStringMap = new CMapStringToPtr;

      // add new string to toolbar list
      CString strTemp(lpszText,AtlStrLen(lpszText) + 1);
      nString = (INT_PTR)DefWindowProc(TB_ADDSTRING,0,(LPARAM)(LPCTSTR)strTemp);
      if(nString == -1)
         return FALSE;

      // cache string away in string map
      m_pStringMap->SetAt(lpszText,(void*)nString);
      ASSERT(m_pStringMap->Lookup(lpszText,p));
   }

   // change the toolbar button description
   TBBUTTON button;
   _GetButton(nIndex,&button);
   _SetButton(nIndex,&button);

   return TRUE;
}

CString CTabBar::GetButtonText(int nIndex) const
{
   CString strResult;
   GetButtonText(nIndex,strResult);
   return strResult;
}

void CTabBar::GetButtonText(int nIndex,CString& rString) const
{
   if(m_pStringMap != NULL)
   {
      // get button information (need button.iString)
      TBBUTTON button;
      _GetButton(nIndex,&button);

      // look in map for matching iString
      POSITION pos = m_pStringMap->GetStartPosition();
      CString str; void* p;
      while(pos)
      {
         m_pStringMap->GetNextAssoc(pos,str,p);
         if((INT_PTR)p == button.iString)
         {
            rString = str;
            return;
         }
      }
   }
   rString.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CTabBar message handlers

BEGIN_MESSAGE_MAP(CTabBar,CControlBar)
   ON_WM_NCHITTEST()
   ON_WM_NCPAINT()
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
   ON_WM_NCCALCSIZE()
   ON_WM_WINDOWPOSCHANGING()
   ON_WM_NCCREATE()
   ON_MESSAGE(TB_SETBITMAPSIZE,&CTabBar::OnSetBitmapSize)
   ON_MESSAGE(TB_SETBUTTONSIZE,&CTabBar::OnSetButtonSize)
   ON_MESSAGE(WM_SETTINGCHANGE,&CTabBar::OnPreserveZeroBorderHelper)
   ON_MESSAGE(WM_SETFONT,&CTabBar::OnPreserveZeroBorderHelper)
   ON_MESSAGE(TB_SETDISABLEDIMAGELIST,&CTabBar::OnPreserveSizingPolicyHelper)
   ON_MESSAGE(TB_SETHOTIMAGELIST,&CTabBar::OnPreserveSizingPolicyHelper)
   ON_MESSAGE(TB_SETIMAGELIST,&CTabBar::OnPreserveSizingPolicyHelper)
   ON_WM_SYSCOLORCHANGE()
   ON_WM_LBUTTONDOWN()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_LBUTTONUP()
   ON_WM_TIMER()
   ON_WM_SIZE()
   ON_COMMAND(ID_TAB_LEFT_ARROW,&CTabBar::OnTabLeftArrow)
   ON_COMMAND(ID_TAB_RIGHT_ARROW,&CTabBar::OnTabRightArrow)
END_MESSAGE_MAP()

BOOL CTabBar::OnEraseBkgnd(CDC*)
{
   return TRUE;
}

LRESULT CTabBar::OnNcHitTest(CPoint)
{
   return HTCLIENT;
}

void CTabBar::OnNcCalcSize(BOOL /*bCalcValidRects*/,NCCALCSIZE_PARAMS* lpncsp)
{
   // calculate border space (will add to top/bottom, subtract from right/bottom)
   CRect rect; rect.SetRectEmpty();
   //BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
   //CControlBar::CalcInsideRect(rect,bHorz);

   // adjust non-client area for border space
   lpncsp->rgrc[0].left += rect.left;
   lpncsp->rgrc[0].top += rect.top;
   // previous versions of COMCTL32.DLL had a built-in 2 pixel border
   lpncsp->rgrc[0].right += rect.right;
   lpncsp->rgrc[0].bottom += rect.bottom;
}

void CTabBar::OnBarStyleChange(DWORD dwOldStyle,DWORD dwNewStyle)
{
   // a dynamically resizeable toolbar can not have the CBRS_FLOAT_MULTI
   ASSERT(!((dwNewStyle & CBRS_SIZE_DYNAMIC) &&
      (m_dwDockStyle & CBRS_FLOAT_MULTI)));

   // a toolbar can not be both dynamic and fixed in size
   ASSERT(!((dwNewStyle & CBRS_SIZE_FIXED) &&
      (dwNewStyle & CBRS_SIZE_DYNAMIC)));

   // CBRS_SIZE_DYNAMIC can not be disabled once it has been enabled
   ASSERT(((dwOldStyle & CBRS_SIZE_DYNAMIC) == 0) ||
      ((dwNewStyle & CBRS_SIZE_DYNAMIC) != 0));

   if(m_hWnd != NULL &&
      ((dwOldStyle & CBRS_BORDER_ANY) != (dwNewStyle & CBRS_BORDER_ANY)))
   {
      // recalc non-client area when border styles change
      SetWindowPos(NULL,0,0,0,0,
         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
   }
   m_bDelayedButtonLayout = TRUE;
}

void CTabBar::OnNcPaint()
{
   //EraseNonClient();
}

void CTabBar::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
   // not necessary to invalidate the borders
   DWORD dwStyle = m_dwStyle;
   m_dwStyle &= ~(CBRS_BORDER_ANY);
   CControlBar::OnWindowPosChanging(lpWndPos);
   m_dwStyle = dwStyle;

   // If we can resize while floating
   if(dwStyle & CBRS_SIZE_DYNAMIC)
   {
      // And we are resizing
      if(lpWndPos->flags & SWP_NOSIZE)
         return;

      // Then redraw the buttons
      Invalidate();
   }
}

bool CTabBar::get_tab_rect(LPRECT lprect,int i)
{

   if(i < m_iTabScroll)
   {
      return false;
   }
   CDC dc;

   dc.CreateCompatibleDC(NULL);

   Graphics g(dc.GetSafeHdc());

   CRect rectClient;

   GetClientRect(rectClient);

   rectClient.DeflateRect(12,0,12,0);

   CRect r = rectClient;

   int j = 0;
   int x = 25;
   int iTabHeight = 0;
   CArray<RectF> boxa;
   boxa.SetSize(m_taba.size());
   int iTotalW = 0;
   int iFullTotalW = 0;
   for(;j < m_taba.size(); j++)
   {
      //CStringW wstr(get_uni(m_stra[j], CP_OEMCP));
      CStringW wstr(get_uni(m_stra[j]));
      g.MeasureString(wstr,wstr.GetLength(),&m_font2,PointF(0,0),&boxa[j]);
      iTotalW += ceil(boxa[j].Width);
      iFullTotalW += ceil(boxa[j].Width);
      iFullTotalW +=m_rTabBorder.left + m_rTabBorder.right;
      iTabHeight = max(ceil(boxa[j].Height),iTabHeight);
   }
   bool bScroll = false;
   m_iMargin = (m_rTabBorder.left + m_rTabBorder.right) / 2;
   if(rectClient.Width() - 64 < iFullTotalW)
   {
      ///m_iMargin = (rectClient.Width() - 64 - iTotalW) / (m_taba.size() * 2);
      bScroll = true;
   }
   if(bScroll && m_iPreviousCount < m_taba.size())
   {
      m_iPreviousCount = m_taba.size();
      m_iTabScroll= m_iTabSel-1;
      if(m_iTabScroll < 0)
      {
         m_iTabScroll = 0;
      }
   }
   int right = 0;
   int xTotal = 0;
   int right2 = 0;
   for(j=0;j < m_taba.size(); j++)
   {
      if(j >= m_iTabScroll && j < i)
      {
         x += ceil(boxa[j].Width);
         x +=m_iMargin * 2;
         right2 = x + ceil(boxa[j].Width) + m_iMargin * 2;
         if(right2 > rectClient.right)
         {
            return false;
         }
      }
      xTotal += ceil(boxa[j].Width);
      right = x + ceil(boxa[j].Width) + m_iMargin * 2;
   }
   //bool bScroll = right >rectClient.right;
      iTabHeight += m_rTabBorder.top + m_rTabBorder.bottom;
   r.bottom = r.top + iTabHeight - 3;
   m_iTabHeight = iTabHeight;
   m_ulRibbonHeight = 24;
   if(i < m_taba.size())
   {
      lprect->left = x;
      lprect->right = x + ceil(boxa[i].Width) + m_iMargin * 2;
      lprect->top = r.top;
      lprect->bottom = r.bottom;
   }
   if(bScroll)
   {

            if(m_iTabScroll <= 0)
            {

               if(m_buttonTabLeft.IsWindowVisible())
               m_buttonTabLeft.ShowWindow(SW_HIDE);

            }
            else
            {

               x += 10;

               if(!m_buttonTabLeft.IsWindowVisible())
               m_buttonTabLeft.SetWindowPos(&CWnd::wndTop,0,0,10,m_iTabHeight,SWP_SHOWWINDOW);

            }

            if(m_iTabScroll >= m_taba.size())
            {

               if(m_buttonTabRight.IsWindowVisible())
               m_buttonTabRight.ShowWindow(SW_HIDE);

            }
            else
            {

               if(!m_buttonTabRight.IsWindowVisible())
               m_buttonTabRight.SetWindowPos(&CWnd::wndTop,rectClient.right - 10,0,10,m_iTabHeight,SWP_SHOWWINDOW);

            }



   }
   else
   {

      m_iTabScroll = 0;

      if(m_buttonTabLeft.IsWindowVisible())
         m_buttonTabLeft.ShowWindow(SW_HIDE);

      if(m_buttonTabRight.IsWindowVisible())
         m_buttonTabRight.ShowWindow(SW_HIDE);

   }
   return  true;
}





void CTabBar::OnPaint()
{

   if(!::IsWindow(GetSafeHwnd()))
   {

      return;

   }

   CPaintDC dcPaint(this);



   if(m_bNeedLayout)
   {

      m_bNeedLayout = false;
      layout();

   }
   if(&m_font2 == NULL)
   {
      return;
   }

   CMemDC dcMem(dcPaint,this);

   CDC &dc = dcMem.GetDC();

   CRect rectClient;

   GetClientRect(rectClient);

   CRect r = rectClient;

   CRect rTab;

   get_tab_rect(rTab,0);

   r.bottom = m_ulRibbonHeight;

   dc.FillSolidRect(rectClient,GetSysColor(COLOR_BTNFACE));

   dc.SetBkMode(TRANSPARENT);

   if(m_iMargin <= 1)
   {
      return;
   }

   Gdiplus::Graphics g(dc.GetSafeHdc());

   Gdiplus::SolidBrush brTabTextSel(Gdiplus::Color(255,0,0,0));

   Gdiplus::SolidBrush brTabBackSel(Gdiplus::Color(255,250,250,250));

   Gdiplus::Pen pen(Gdiplus::Color(255,0,0,0));

   Gdiplus::SolidBrush brTabText(Gdiplus::Color(255,80,80,80));

   Gdiplus::SolidBrush brTabBack(Gdiplus::Color(255,200,200,200));

   Gdiplus::RectF r2;

   CStringW wstr;

   int overflow = 5;

   int iLeft = 30;
   int iRightMargin = 0;

   for(int i = m_iTabScroll; i <m_iTabSel; i++)
   {

      if(!get_tab_rect(rTab,i))
      {
         continue;
      }

      copy(r2,rTab);
      CString str;
      // CStringW wstr(get_uni(m_stra[i], CP_OEMCP));
      CStringW wstr(get_uni(m_stra[i], CP_ACP));

         GraphicsPath path;

         path.StartFigure();
         path.AddLine(rTab.left-overflow,rTab.bottom,rTab.left + overflow,rTab.top);
         path.AddLine(rTab.left + overflow,rTab.top, rTab.right-overflow,rTab.top);
         path.AddLine(rTab.right-overflow,rTab.top, rTab.right + overflow, rTab.bottom);
         //path.CloseFigure();

         g.FillPath(&brTabBack,&path);
         g.DrawPath(&pen,&path);
         r2.X += iLeft;
         r2.Width -= iLeft + iRightMargin;
         g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_formatTab,&brTabText);
         copy(r2,rTab);

         r2.X += 10;
         Gdiplus::Font f(L"Marlett",10.0);

         r2.Y +=1;
         g.DrawString(L"r",1,&f,r2,&m_format,&brTabText);


   }

   for(int i = m_taba.size()-1; i > m_iTabSel ; i--)
   {

      if(!get_tab_rect(rTab,i))
         continue;

      copy(r2,rTab);
      CString str;
      //CStringW wstr(get_uni(m_stra[i],CP_OEMCP));
      CStringW wstr(get_uni(m_stra[i], CP_ACP));

         GraphicsPath path;

         path.StartFigure();
         path.AddLine(rTab.left - overflow,rTab.bottom,rTab.left + overflow,rTab.top);
         path.AddLine(rTab.left + overflow,rTab.top,rTab.right - overflow,rTab.top);
         path.AddLine(rTab.right - overflow,rTab.top,rTab.right + overflow,rTab.bottom);
         //path.CloseFigure();

         g.FillPath(&brTabBack,&path);
         g.DrawPath(&pen,&path);
         r2.X += iLeft;
         r2.Width -= iLeft + iRightMargin;
         g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_formatTab,&brTabText);
         copy(r2,rTab);

         r2.X += 10;
         Gdiplus::Font f(L"Marlett",10.0);

         r2.Y +=1;
         g.DrawString(L"r",1,&f,r2,&m_format,&brTabText);


   }

   if(m_iTabSel >= 0 && m_iTabSel < m_taba.size())
   {
      if(get_tab_rect(rTab,m_iTabSel))
      {
         copy(r2,rTab);

         CString str;

         if(::IsWindow(m_taba[m_iTabSel]->GetSafeHwnd()))
         {
            m_taba[m_iTabSel]->GetWindowText(str);
         }
         //CStringW wstr(get_uni(str, CP_OEMCP));
         CStringW wstr(get_uni(str, CP_ACP));

         GraphicsPath path;

         path.StartFigure();
         path.AddLine(rTab.left - overflow,rTab.bottom,rTab.left + overflow,rTab.top);
         path.AddLine(rTab.left + overflow,rTab.top,rTab.right - overflow,rTab.top);
         path.AddLine(rTab.right - overflow,rTab.top,rTab.right + overflow,rTab.bottom);
         //path.CloseFigure();
         r2.X += iLeft;
         r2.Width -= iLeft + iRightMargin;
         g.FillPath(&brTabBackSel,&path);
         g.DrawPath(&pen,&path);

         g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_formatTab,&brTabTextSel);
         copy(r2,rTab);
         r2.X += 10;
         Gdiplus::Font f(L"Marlett",10.0);

         r2.Y +=1;
         g.DrawString(L"r",1,&f,r2,&m_format,&brTabTextSel);

      }

   }
}

LRESULT CTabBar::OnSetButtonSize(WPARAM,LPARAM lParam)
{
   return OnSetSizeHelper(m_sizeButton,lParam);
}

LRESULT CTabBar::OnSetBitmapSize(WPARAM,LPARAM lParam)
{
   return OnSetSizeHelper(m_sizeImage,lParam);
}

LRESULT CTabBar::OnSetSizeHelper(CSize& size,LPARAM lParam)
{
   //  The IE4 version of COMCTL32.DLL supports a zero border, but 
   //	only if TBSTYLE_TRANSPARENT is on during the the TB_SETBITMAPSIZE
   //	and/or TB_SETBUTTONSIZE messages.  In order to enable this feature
   //	all the time (so we get consistent border behavior, dependent only
   //	on the version of COMCTL32.DLL) we turn on TBSTYLE_TRANSPARENT 
   //	whenever these messages go through.  It would be nice that in a
   //	future version, the system toolbar would just allow you to set
   //	the top and left borders to anything you please.

   //  In comctl32 version 6.00 when using XP Look combined with TBSTYLE_EX_DRAWDDARROWS 
   //	style minimal sizing policy was changed to be
   //			button.height >= image.height + 13
   //			button.width >= image.width + 16
   //	this force buttons to be bigger then usual
   //	To override this behavior we should remove TBSTYLE_EX_DRAWDDARROWS prior to sizing operations

   BOOL bModify = FALSE;
   DWORD dwStyle = 0;
   DWORD dwStyleEx = 0;
   //if(_afxComCtlVersion >= VERSION_IE4)
   //{
   //   dwStyle = GetStyle();
   //   bModify = ModifyStyle(0,TBSTYLE_TRANSPARENT | TBSTYLE_FLAT);
   //   if((_afxComCtlVersion >= VERSION_6) && ::IsWindow(GetToolBarCtrl()))
   //   {
   //      dwStyleEx = GetToolBarCtrl().SetExtendedStyle(GetToolBarCtrl().GetExtendedStyle() &~ TBSTYLE_EX_DRAWDDARROWS);
   //   }
   //}

   LRESULT lResult = Default();
   if(lResult)
   {
      size = DWORD(lParam);
   }

   if(bModify)
   {
      SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
   }

   return lResult;
}

LRESULT CTabBar::OnPreserveZeroBorderHelper(WPARAM,LPARAM)
{
   BOOL bModify = FALSE;
   DWORD dwStyle = 0;
   DWORD dwStyleEx = 0;
   //if(_afxComCtlVersion >= VERSION_IE4)
   //{
   //   dwStyle = GetStyle();
   //   bModify = ModifyStyle(0,TBSTYLE_TRANSPARENT | TBSTYLE_FLAT);
   //   if((_afxComCtlVersion >= VERSION_6) && ::IsWindow(GetToolBarCtrl()))
   //   {
   //      dwStyleEx = GetToolBarCtrl().SetExtendedStyle(GetToolBarCtrl().GetExtendedStyle() &~ TBSTYLE_EX_DRAWDDARROWS);
   //   }
   //}

   LRESULT lResult = Default();

   if(bModify)
   {
      SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
   }

   return lResult;
}

LRESULT CTabBar::OnPreserveSizingPolicyHelper(WPARAM,LPARAM)
{
   //	NOTE: in comctl32 version 6.00 when using XP Look combined with TBSTYLE_EX_DRAWDDARROWS 
   //	style minimal sizing policy was changed to be
   //			button.height >= image.height + 13
   //			button.width >= image.width + 16
   //	this force buttons to be bigger then usual
   //	To override this behavior we should remove TBSTYLE_DROPDOWN from all buttons prior to setimagelist 
   //  operations

   UINT uiButtonNum = 0;
   DWORD* pdwStyles = NULL;
   //if(_afxComCtlVersion >= VERSION_6)
   //{
   //   uiButtonNum = GetToolBarCtrl().GetButtonCount();
   //   pdwStyles = new DWORD[uiButtonNum];
   //   ASSERT(pdwStyles);
   //   for(UINT i =0; i < uiButtonNum; i++)
   //   {
   //      pdwStyles[i] = GetButtonStyle(i);
   //      SetButtonStyle(i,pdwStyles[i] & ~TBSTYLE_DROPDOWN);
   //   }
   //}

   LRESULT lResult = Default();

   if(pdwStyles)
   {
      for(UINT i =0; i < uiButtonNum; i++)
      {
         SetButtonStyle(i,pdwStyles[i]);
      }
      delete[] pdwStyles;
   }

   return lResult;
}

void CTabBar::OnSysColorChange()
{
   // re-color bitmap for toolbar
   if(m_hInstImageWell != NULL && m_hbmImageWell != NULL)
   {
      HBITMAP hbmNew;
      hbmNew = AfxLoadSysColorBitmap(m_hInstImageWell,m_hRsrcImageWell);
      if(hbmNew != NULL)
         AddReplaceBitmap(hbmNew);
   }
}

// Active Accessibility support
HRESULT CTabBar::get_accName(VARIANT varChild,BSTR *pszName)
{
   if(varChild.lVal == CHILDID_SELF)
   {
      CString strText;
      GetWindowText(strText);

      *pszName = strText.AllocSysString();
      return S_OK;
   }
   else if(varChild.lVal != 0)
   {
      CString szFullText;
      CString strTipText;

      UINT nID;
      UINT nStyle;
      int nImage;
      GetButtonInfo(varChild.lVal - 1,nID,nStyle,nImage);
      // don't handle the message if no string resource found
      if(szFullText.LoadString((UINT)nID) != 0)
      {
         // this is the command id, not the button index
         AfxExtractSubString(strTipText,szFullText,1,'\n');
         *pszName = strTipText.AllocSysString();
         return S_OK;
      }
   }
   return CControlBar::get_accName(varChild,pszName);
}


void CTabBar::OnUpdateCmdUI(CFrameWnd* pTarget,BOOL bDisableIfNoHndler)
{

   m_windowa.Refresh();
   
   restart:
   for(auto it = m_windowa.begin(); it != m_windowa.end(); it++)
   {
      if(std::find(m_taba.begin(),m_taba.end(),*it) == m_taba.end())
      {
         m_taba.push_back(*it);
         m_bNeedLayout  = true;
         goto restart;
      }
   }
   for(auto it = m_taba.begin(); it != m_taba.end(); it++)
   {
      if(std::find(m_windowa.begin(),m_windowa.end(),*it) == m_windowa.end())
      {
         m_taba.erase(it);
         m_bNeedLayout  = true;
         goto restart;
      }
   }
   CStringArray stra;
   CString str;
   for(auto it = m_taba.begin(); it != m_taba.end(); it++)
   {
      (*it)->GetWindowText(str);
      stra.Add(str);
   }

   int iTabSel = -1;

   CMainFrame * pframe = (CMainFrame *)GetParentFrame();

   auto it = std::find(m_taba.begin(),m_taba.end(),(CWnd *) pframe->MDIGetActive());

   if(it != m_taba.end())
   {
         
      iTabSel = it - m_taba.begin();

   }


   bool bDifferentText = false;

   if(m_stra.GetCount() == stra.GetCount())
   {

      for(int i = 0; i < m_stra.GetCount(); i++)
      {

         if(m_stra[i] != stra[i])
         {

            bDifferentText = true;

            m_stra.Copy(stra);

            break;

         }

      }

   }
   else
   {
      bDifferentText = true;
      m_stra.Copy(stra);

   }

   if(m_bNeedLayout || iTabSel != m_iTabSel || bDifferentText)
   {

      m_iTabSel = iTabSel;

      RedrawWindow();

   }

}

/////////////////////////////////////////////////////////////////////////////
// CTabBar diagnostics

#ifdef _DEBUG
void CTabBar::AssertValid() const
{
   // Note: CControlBar::AssertValid is not called because it checks for
   //  m_nCount and m_pData to be in sync, which they are not in CTabBar.

   ASSERT(m_hbmImageWell == NULL || ::GetObjectType(m_hbmImageWell) == OBJ_BITMAP);

   if(m_hInstImageWell != NULL && m_hbmImageWell != NULL)
      ASSERT(m_hRsrcImageWell != NULL);
}

void CTabBar::Dump(CDumpContext& dc) const
{
   CControlBar::Dump(dc);

   dc << "m_hbmImageWell = " << (void*)m_hbmImageWell;
   dc << "\nm_hInstImageWell = " << (void*)m_hInstImageWell;
   dc << "\nm_hRsrcImageWell = " << (void*)m_hRsrcImageWell;
   dc << "\nm_sizeButton = " << m_sizeButton;
   dc << "\nm_sizeImage = " << m_sizeImage;

   if(dc.GetDepth() > 0)
   {
      CTabBar* pBar = (CTabBar*)this;
      int nCount = int(pBar->DefWindowProc(TB_BUTTONCOUNT,0,0));
      for(int i = 0; i < nCount; i++)
      {
         TBBUTTON button;
         _GetButton(i,&button);
         dc << "\ntoolbar button[" << i << "] = {";
         dc << "\n\tnID = " << button.idCommand;
         dc << "\n\tnStyle = " << MAKELONG(button.fsStyle,button.fsState);
         if(button.fsStyle & TBSTYLE_SEP)
            dc << "\n\tiImage (separator width) = " << button.iBitmap;
         else
            dc << "\n\tiImage (bitmap image index) = " << button.iBitmap;
         dc << "\n}";
      }
   }

   dc << "\n";
}
#endif


IMPLEMENT_DYNAMIC(CTabBar,CControlBar)

/////////////////////////////////////////////////////////////////////////////


void CTabBar::OnLButtonDown(UINT nFlags,CPoint point)
{
   CRect rTab;
   CRect rClose;
   for(int i = 0; i < m_taba.size() + 2; i++)
   {
      get_tab_rect(rTab,i);
      rClose = rTab;
      rClose.left += 5;
      rClose.right = rClose.left + 18;
      if(rClose.PtInRect(point))
      {
         CWnd * pwnd = m_taba[i];
         m_taba.erase(m_taba.begin() +i);
         pwnd->SendMessageA(WM_CLOSE);
         break;
      }
      if(rTab.PtInRect(point))
      {
         set_cur_tab(i);
         break;
      }
   }
   Invalidate();
   SetCapture();
   CControlBar::OnLButtonDown(nFlags,point);
}


void CTabBar::OnMouseMove(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   TRACKMOUSEEVENT tme ={};
   tme.cbSize = sizeof(tme);
   tme.dwFlags = TME_LEAVE;
   TrackMouseEvent(&tme);


   CControlBar::OnMouseMove(nFlags,point);

}


void CTabBar::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_iGroupHover = -1;
   m_iItemHover = -1;
   Invalidate();
   CControlBar::OnMouseLeave();
}


void CTabBar::OnLButtonUp(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   ReleaseCapture();
   int iGroupHover;
   int iItemHover;

   CControlBar::OnLButtonUp(nFlags,point);
}





void CTabBar::OnTimer(UINT_PTR nIDEvent)
{
   // TODO: Add your message handler code here and/or call default
   if(nIDEvent == 1000)
   {
      CPoint point;
      ::GetCursorPos(&point);
      ScreenToClient(&point);
      OnMouseMove(0,point);
   }
   CControlBar::OnTimer(nIDEvent);
}


void CTabBar::set_cur_tab(int iTab)
{

   if(iTab < 0 || iTab >= m_taba.size())
   {

      return;

   }

   CMainFrame * pframe = (CMainFrame *)GetParentFrame();
   pframe->MDIActivate(m_taba[iTab]);
   

   CRect rTab;
   get_tab_rect(rTab,iTab);
   m_bNeedLayout = true;
   RedrawWindow();
   
}

void CTabBar::OnSize(UINT nType,int cx,int cy)
{
   CControlBar::OnSize(nType,cx,cy);

   m_bNeedLayout = true;


}
#undef new
void CTabBar::layout()
{
   CRect rectClient;
   GetClientRect(rectClient);
   if(rectClient.IsRectEmpty())
      return;
   CDC dc;
   dc.CreateCompatibleDC(NULL);


   CRect rItem;
   CRect rImage;
   CRect rText;

   CRect r = rectClient;

   CRect rTab;
   get_tab_rect(rTab,0);

   if(m_iMargin < 1)
   {
      if(m_ulRibbonHeight > 1)
      {
         m_ulRibbonHeight = 1;
         GetParentFrame()->RecalcLayout();
      }
   }
   else
   {
      if(m_ulRibbonHeight == 1)
      {
         if(m_bExpanded)
         {
            m_ulRibbonHeight = 24;
         }
         else
         {
            m_ulRibbonHeight = 24;


         }
         GetParentFrame()->RecalcLayout();
      }
   }

   r.bottom = m_iTabHeight;



   Gdiplus::Graphics g(dc.GetSafeHdc());

   CStringW wstr;

   wstr = L"WApyg";

   Gdiplus::RectF r2;

   r2.X = 0.0f;
   r2.Y = 0.0f;
   r2.Width = 200.0f;
   r2.Height = 200.0f;

   Gdiplus::RectF box;

   Gdiplus::Status s = g.MeasureString(wstr,wstr.GetLength(),&m_font2,r2,&m_format,&box);

   m_fMaxTextHeight = box.Height * 2.f;

   if(&m_font2 == NULL)
   {
   }

}


#include <algorithm> 
CChildFrameList::CChildFrameList()
{
}

CChildFrameList::~CChildFrameList()
{
}

int CChildFrameList::Refresh()
{
   clear();
   CWinApp* pApp = AfxGetApp();
   POSITION pos = pApp->GetFirstDocTemplatePosition();
   while(pos != NULL)
   {
      CDocTemplate* doc_template = (
         CDocTemplate*)pApp->GetNextDocTemplate(pos);
      if(doc_template != NULL)
      {
         POSITION doc_pos = doc_template->GetFirstDocPosition();
         while(doc_pos != NULL)
         {
            CDocument* doc = doc_template->GetNextDoc(doc_pos);
            if(doc != NULL)
            {
               POSITION view_pos = doc->GetFirstViewPosition();
               while(view_pos != NULL)
               {
                  CView* view = doc->GetNextView(view_pos);
                  if(view && ::IsWindow(view->GetSafeHwnd()))
                  {
                     CWnd* frame = FindFrame(view);
                     if(frame != NULL)
                     {
                        if(std::find(begin(),end(),frame) ==
                           end())
                        {
                           push_back(frame);
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return size();
}

CWnd* CChildFrameList::FindFrame(CWnd* child)
{
   if(child)
   {
      if(child->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
      {
         return child;
      }
      else
      {
         CWnd* parent = child->GetParent();
         while(parent != NULL)
         {
            if(parent->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
            {
               return parent;
            }
            parent = parent->GetParent();
         }
      }
   }
   return NULL;
}





void CTabBar::OnTabLeftArrow()
{
   m_iTabScroll -= 1;
   if(m_iTabScroll < 0)
   {
      m_iTabScroll =  0;
   }
   RedrawWindow();
}


void CTabBar::OnTabRightArrow()
{

   m_iTabScroll += 1;

   if(m_iTabScroll >= m_taba.size())
   {

      m_iTabScroll =  m_taba.size()-1;

   }

   RedrawWindow();

}
