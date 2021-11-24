#include "Stdafx.h"
#include "imm_client.h"
#include "imm_context.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(imm_client, CView)

BEGIN_MESSAGE_MAP(imm_client, CView)
   //{{AFX_MSG_MAP(CPCDraftView)
   ON_WM_KEYDOWN()
   ON_WM_KILLFOCUS()
   ON_WM_SETFOCUS()
   ON_MESSAGE(WM_IME_REQUEST, ::imm_client::OnImeRequest)
   ON_MESSAGE(WM_IME_CONTROL, ::imm_client::OnImeControl)
   ON_MESSAGE(WM_IME_SETCONTEXT,  ::imm_client::OnIme)
   ON_MESSAGE(WM_IME_STARTCOMPOSITION,  ::imm_client::OnIme)
   ON_MESSAGE(WM_IME_COMPOSITION,  ::imm_client::OnImeComposition)
   ON_MESSAGE(WM_IME_ENDCOMPOSITION,  ::imm_client::OnIme)
   ON_MESSAGE(WM_IME_NOTIFY,  ::imm_client::OnImeNotify)
   ON_MESSAGE(WM_IME_KEYDOWN,  ::imm_client::OnIme)
   ON_MESSAGE(WM_IME_KEYUP,  ::imm_client::OnIme)
   ON_MESSAGE(WM_IME_SELECT,  ::imm_client::OnIme)
   ON_MESSAGE(WM_INPUTLANGCHANGE, imm_client::OnIme)
   ON_MESSAGE(WM_IME_CHAR, imm_client::OnChar11)
   ON_MESSAGE(WM_CHAR, imm_client::OnChar11)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

imm_client::imm_client()
{
   m_iCandidateList = -1;
   m_bResetting = false;
   m_bImeCancelling = false;
   m_bImeCandidateOpened = false;

}


imm_client::~imm_client()
{


}


bool imm_client::is_text_mode() const
{

   return true;

}


void imm_client::OnDraw(CDC* pDC)  // overridden to draw this view
{



}
//void imm_client::install_message_routing(::channel * pchannel)
//{
//
//   IGUI_MSG_LINK(WM_SETFOCUS, pchannel, this, &::imm_client::_001OnSetFocus);
//   IGUI_MSG_LINK(WM_KILLFOCUS, pchannel, this, &::imm_client::_001OnKillFocus);
//   IGUI_MSG_LINK(WM_IME_SETCONTEXT, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_STARTCOMPOSITION, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_COMPOSITION, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_ENDCOMPOSITION, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_NOTIFY, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_KEYDOWN, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_KEYUP, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_IME_SELECT, pchannel, this, &::imm_client::OnIme);
//   IGUI_MSG_LINK(WM_INPUTLANGCHANGE, pchannel, this, &::imm_client::OnIme);
//
//   IGUI_MSG_LINK(WM_CHAR, pchannel, this, &::imm_client::_011OnChar);
//   IGUI_MSG_LINK(WM_IME_CHAR, pchannel, this, &::imm_client::_011OnChar);
//
//   IGUI_MSG_LINK(WM_KEYDOWN, pchannel, this, &::imm_client::_001OnKeyDown);
//
//
//}





void imm_client::OnKillFocus(CWnd* pNewWnd)
{
}


void imm_client::OnSetFocus(CWnd* pOldWnd)
{
}


LRESULT imm_client::OnChar11(WPARAM wparam, LPARAM lparam)
{

   

   if (GetCurrentMessage()->message == WM_CHAR)
   {

      if ((GetAsyncKeyState(VK_CONTROL) < 0)
         || (GetAsyncKeyState(VK_MENU) < 0))
      {

         return 0;

      }

      if (wparam == '\b')
      {

         return 0;

      }

      if (wparam == '\t')
      {

         return 0;

      }

      wchar_t wsz[2];
      wsz[0] = (wchar_t)wparam;
      wsz[1] = '\0';

      CStringW strChar;

      strChar = wsz;

      //if (!::str::begins_eat_ci(m_strImeComposition, strChar))
      //{

      insert_text(strChar, false);

      //}

   }

   return 0;

}


void imm_client::insert_text(CStringW str, bool bForceNewStep)
{


}


LRESULT imm_client::OnImeComposition(WPARAM wparam, LPARAM lparam)
{

   if (!is_text_mode())
   {

      return 0;

   }



   if (lparam == 0)
   {

      OutputDebugString("\nWM_IME_COMPOSITION Cancellation...");

      if (m_strImeComposition.GetLength() > 0)
      {

         on_edit_undo();

         m_strImeComposition.Empty();

      }

   }
   else
   {

      CString strFormat;

      strFormat.Format("\nWM_IME_COMPOSITION %d", lparam);

      OutputDebugString(strFormat);

      if ((lparam & GCS_RESULTSTR) != 0)
      {

         if (m_bResetting)
         {

            OutputDebugString("resetting");

         }
         else
         {

            //::output_debug_string("\nWM_IME_COMPOSITION Result String... ");

            imm_context imm(this);

            CStringW strComposition;

            strComposition = imm.get_string(GCS_RESULTSTR);

            if (strComposition.GetLength() <= 0)
            {

               strComposition = imm.get_string(GCS_RESULTREADSTR);

            }

            //::output_debug_string("\nWM_IME_COMPOSITION Composit・String Length = " + __str(strComposition.get_length()));

            on_text_composition(strComposition, e_composition_state_selected);

            on_text_composition_done();

         }

      }

      if ((lparam & GCS_COMPSTR) != 0)
      {

         //::output_debug_string("\nWM_IME_COMPOSITION Compositing... ");

         imm_context imm(this);

         CStringW strComposition;

         strComposition = imm.get_string(GCS_RESULTSTR);

         DWORD dwCount = 0;

         int iList = m_iCandidateList;

         CANDIDATELIST candidatelistDummy;

         ZeroMemory(&candidatelistDummy, sizeof(CANDIDATELIST));

         candidatelistDummy.dwSize = sizeof(CANDIDATELIST);

         DWORD dwSize = ImmGetCandidateListA(imm.m_himc, iList, &candidatelistDummy, 0);

         if (dwSize == 0)
         {

            DWORD dw = GetLastError();

            ::OutputDebugString("\nImmGetCandidateListW Error");

         }
         else
         {

            m_memCandidateList.SetSize(dwSize);

            m_pcandidatelist = (LPCANDIDATELIST)m_memCandidateList.GetData();

            ImmGetCandidateListW(imm.m_himc, iList, m_pcandidatelist, (DWORD)m_memCandidateList.GetSize());

            int iTest = m_pcandidatelist->dwOffset[0];

            auto iTest2 = (byte *)& m_pcandidatelist->dwOffset[m_pcandidatelist->dwCount] - (unsigned char *)m_pcandidatelist;

            auto pwsz = (WCHAR *)(m_memCandidateList.GetData() + m_pcandidatelist->dwOffset[m_pcandidatelist->dwSelection]);

            CString wstr(pwsz);

         }

         if (strComposition.GetLength() > 0)
         {

            on_text_composition(strComposition, e_composition_state_selected);

         }
         else
         {

            strComposition = imm.get_string(GCS_COMPSTR);

            on_text_composition(strComposition, e_composition_state_composing);

         }

      }

   }

   return 0;

}


LRESULT imm_client::OnIme(WPARAM wparam, LPARAM lparam)
{

   if (!is_text_mode())
   {

      return DefWindowProc(GetCurrentMessage()->message, wparam, lparam);

   }





   if (GetCurrentMessage()->message == WM_INPUTLANGCHANGE)
   {

      //::output_debug_string("WM_INPUTLANGCHANGE");

      //get_wnd()->pred([this]()
      //   {

      //      imm_context imm(this);

      //      if (!ImmGetOpenStatus(imm))
      //      {

      //         ImmSetOpenStatus(imm, TRUE);

      //      }
      //
      //      //SendMessage(ImmGetDefaultIMEWnd(get_handle()), WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0);

      //      //   output_debug_string("window already open");

      //      //}

      //      //


      //   });

   }
   else if (GetCurrentMessage()->message == WM_IME_SELECT)
   {

      OutputDebugString("\nWM_IME_SELECT");
      //SetInputLanguage();

   }
   else if (GetCurrentMessage()->message == WM_IME_SETCONTEXT)
   {

      OutputDebugString("\nWM_IME_SETCONTEXT");
      //EnableIME();

      //if (!is_text_mode())
      //{

      //   lparam = lparam & ~ISC_SHOWUIALL;
      //   return TRUE;

      //}


   }
   else if (GetCurrentMessage()->message == WM_IME_KEYDOWN)
   {

      //INFO("WM_IME_KEYDOWN");

   }
   else if (GetCurrentMessage()->message == WM_IME_ENDCOMPOSITION)
   {

      OutputDebugString("\nWM_IME_ENDCOMPOSITION");

   }
   else if (GetCurrentMessage()->message == WM_IME_STARTCOMPOSITION)
   {

      OutputDebugString("\nWM_IME_STARTCOMPOSITION");

      m_bImeCancelling = false;

      on_text_composition_message(TEXT_COMPOSITION_MESSAGE_UPDATE_CANDIDATE_WINDOW_POSITION);

      //pbase->m_bRet = true;

   }

   return 0;

}


LRESULT imm_client::OnImeRequest(WPARAM wparam, LPARAM lparam)
{

   if (is_text_mode())
   {

      return DefWindowProc(WM_IME_REQUEST, wparam, lparam);

   }

   return 0;

}


LRESULT imm_client::OnImeControl(WPARAM wparam, LPARAM lparam)
{

   if (is_text_mode())
   {

      return DefWindowProc(WM_IME_CONTROL, wparam, lparam);

   }

   return 0;

}


LRESULT imm_client::OnImeNotify(WPARAM wparam, LPARAM lparam)
{

   if (!is_text_mode())
   {

      if (wparam == IMN_OPENCANDIDATE)
      {

         imm_context imm(this);

         imm.close();

      }

      return 0;

   }


   if (wparam == IMN_SETCANDIDATEPOS)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_SETCANDIDATEPOS");

   }
   else if (wparam == IMN_SETCOMPOSITIONFONT)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_SETCOMPOSITIONFONT");

   }
   else if (wparam == IMN_SETCOMPOSITIONWINDOW)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_SETCOMPOSITIONWINDOW");

   }
   else if (wparam == IMN_SETSTATUSWINDOWPOS)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_SETSTATUSWINDOWPOS");

   }
   else if (wparam == IMN_OPENCANDIDATE)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_OPENCANDIDATE");

      m_bImeCandidateOpened = true;

      if (wparam & 1)
      {

         m_iCandidateList = 0;

      }
      else if (wparam & 2)
      {

         m_iCandidateList = 1;

      }
      else if (wparam & 4)
      {

         m_iCandidateList = 2;

      }
      else if (wparam & 8)
      {

         m_iCandidateList = 3;

      }
      else
      {

         m_iCandidateList = -1;

      }

      //imm_context imm(this);

   }
   else if (wparam == IMN_CHANGECANDIDATE)
   {

      int iLparam = lparam;
      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_CHANGECANDIDATE");

      m_bImeCandidateOpened = true;

   }
   else if (wparam == IMN_CLOSECANDIDATE)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_CLOSECANDIDATE");

      m_bImeCandidateOpened = false;

      m_iCandidateList = -1;

   }
   else if (wparam == IMN_OPENSTATUSWINDOW)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_OPENSTATUSWINDOW");

   }
   else if (wparam == IMN_CLOSESTATUSWINDOW)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_CLOSESTATUSWINDOW");

   }
   else if (wparam == IMN_SETSTATUSWINDOWPOS)
   {

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_SETSTATUSWINDOWPOS");

   }
   else if (wparam == IMN_SETCONVERSIONMODE)
   {

      imm_context imm(this);

      DWORD dwConversion = 0;

      DWORD dwSentence = 0;

      ImmGetConversionStatus(imm, &dwConversion, &dwSentence);

      ::OutputDebugString("\nWM_IME_NOTIFY" " > " "IMN_SETCONVERSIONMODE");

      if (dwConversion == IME_CMODE_ALPHANUMERIC)
      {

         //str += "alphanumeric mode; ";

      }
      else
      {
         if (dwConversion & IME_CMODE_NATIVE)
         {

            //str += "native mode; ";

         }

         if (dwConversion & IME_CMODE_KATAKANA)
         {

            //str += "katakana mode; ";

         }
         if (dwConversion & IME_CMODE_LANGUAGE)
         {

            //str += "language mode; ";

         }

         if (dwConversion & IME_CMODE_FULLSHAPE)
         {

            //str += "full shape mode; ";

         }
         if (dwConversion & IME_CMODE_ROMAN)
         {

            //str += "roman mode; ";

         }
         if (dwConversion & IME_CMODE_CHARCODE)
         {

            //str += "char code mode; ";

         }
         if (dwConversion & IME_CMODE_HANJACONVERT)
         {

            //str += "hanja mode; ";

         }
         if (dwConversion & IME_CMODE_NATIVESYMBOL)
         {

            //str += "native symbol mode; ";

         }

      }

      //output_debug_string(str);

   }
   else
   {

      CString strFormat;

      strFormat.Format("\nWM_IME_NOTIFY" " > " "%d", wparam);

      ::OutputDebugString(strFormat);

   }

   return 0;

}


void imm_client::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{


   if (nChar == VK_ESCAPE)
   {

      if (m_bImeCandidateOpened)
      {

         imm_context imm(this);

         imm.close_candidate();

      }

      if (m_strImeComposition.GetLength() > 0)
      {

         m_bImeCancelling = true;

         imm_context imm(this);

         ::ImmNotifyIME(imm, NI_COMPOSITIONSTR, CPS_CANCEL, 0);

         on_edit_undo();

         m_strImeComposition.Empty();

         //pkey->m_bRet = true;

         return;

      }

   }

}


bool imm_client::is_text_composition_active() const
{

   return m_bImeCandidateOpened;

}


void imm_client::on_edit_undo()
{


}


void imm_client::get_text_composition_area(LPRECT lprect)
{


}


void imm_client::on_text_composition(CStringW wstr, enum_composition_state ecompositionstate)
{
   
}


void imm_client::on_text_composition_done()
{
   m_bResetting = true;
   try
   {
      imm_context imm(this);
      ImmSetCompositionStringW(imm, SCS_SETSTR, L"", 0, NULL, 0);
   }
   catch (...)
   {


   }
   m_bResetting = false;
 
}


int imm_client::on_text_composition_message(int iMessage)
{

   if (iMessage == TEXT_COMPOSITION_MESSAGE_UPDATE_CARET)
   {

      //strsize iBeg;
      //strsize iEnd;

      //_001GetSel(iBeg, iEnd);

      //i32 x;
      //i32 iLine = SelToLineX(iEnd, x);




      //int y = (iLine)* m_iLineHeight - get_viewport_offset().y;
      //int y2 = y + m_iLineHeight;
      // ::point point(x, y);
      //CRect r;
      //get_client_rect(rect);
      //rect.left = x;
      //rect.top = y;
      //rect.bottom = y2;
      //_001ClientToScreen(rect);
      //get_wnd()->_001ScreenToClient(rect);


      //SetCaretPos(rect.left, rect.top);
      //ShowCaret(get_handle());
   }
   else if (iMessage == TEXT_COMPOSITION_MESSAGE_UPDATE_CANDIDATE_WINDOW_POSITION)
   {

      imm_context imm(this);

      if (!imm)
      {

         return 0;

      }

      CRect rect;

      get_text_composition_area(rect);

      //rect.bottom = rect.top + 30;

      COMPOSITIONFORM com = {};

      //com.dwStyle = CFS_FORCE_POSITION;
      //com.ptCurrentPos = rect.TopLeft();
      //com.ptCurrentPos.y -= 100;
      CRect rect2(rect);
      //rect2.OffsetRect(0, 30);
      com.dwStyle = CFS_RECT;
      com.rcArea = rect2;

      //ShowCaret(get_handle());

      //auto pstyle = get_style(pgraphics);

      //auto pfont = get_font(pstyle);

      //if (pfont)
      //{

      //   LOGFONTW lf = {};

      //   if (pfont->GetLogFont(&lf))
      //   {

      //      if (ImmSetCompositionFontW(imm, &lf))
      //      {

      //         output_debug_string("set ime composition font (Unicode) pos");

      //      }



      //   }

      //}

      if (::ImmSetCompositionWindow(imm, &com))
      {
         //output_debug_string("set ime composition window pos");
      }

      CANDIDATEFORM can = {};

      can.dwStyle = CFS_CANDIDATEPOS;

      can.ptCurrentPos = CPoint(rect.left, rect.bottom);

      if (::ImmSetCandidateWindow(imm, &can))
      {

//         output_debug_string("set ime candidate window pos");

      }

      return 1;

   }

   return 0;

}




