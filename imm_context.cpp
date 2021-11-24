#include "stdafx.h"
#include "imm_context.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

imm_context::imm_context(CView * pview)
{

   m_pview = pview;

   HWND hwnd = pview->GetSafeHwnd();

   m_himc = ImmGetContext(hwnd);

}


imm_context::~imm_context()
{

   HWND hwnd = m_pview->GetSafeHwnd();

   ImmReleaseContext(hwnd, m_himc);

}



bool imm_context::close()
{

   return ImmSetOpenStatus(m_himc, false) != FALSE;

}


bool imm_context::is_opened() const
{

   return ImmGetOpenStatus(m_himc) != FALSE;

}


void imm_context::clear_composition()
{

   ImmNotifyIME(m_himc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
   ImmNotifyIME(m_himc, NI_CLOSECANDIDATE, 0, 0);
   ImmSetCompositionStringW(m_himc, SCS_SETSTR, L"", sizeof(wchar_t), L"", sizeof(wchar_t));
   close();

}


bool imm_context::close_candidate(int iIndex)
{

   return ImmNotifyIME(m_himc, NI_CLOSECANDIDATE, 0, (DWORD)iIndex) != FALSE;

}


CStringW imm_context::get_string(int iStr)
{

   int iLen = ImmGetCompositionStringW(m_himc, iStr, nullptr, 0);

   CStringW wstr;

   auto point = wstr.GetBufferSetLength(iLen);

   ImmGetCompositionStringW(m_himc, iStr, point, iLen);

   wstr.ReleaseBufferSetLength(iLen /sizeof(WCHAR));

   return wstr;

}

void imm_context::get_attr(CArray < byte> & ba, int iStr)
{

   int iLen = ImmGetCompositionStringW(m_himc, iStr, nullptr, 0);

   ba.SetSize(iLen);

   ImmGetCompositionStringW(m_himc, iStr, ba.GetData(), ba.GetCount());

}

void imm_context::get_clause(CArray < int> & ia, int iStr)
{

   int iLen = ImmGetCompositionStringW(m_himc, iStr, nullptr, 0);

   ia.SetSize(iLen / 4);
   ImmGetCompositionStringW(m_himc, iStr, (WCHAR *)ia.GetData(), iLen);

}


CStringW imm_context::_get_candidate(int iList)
{

   CByteArray mem;

   DWORD dwCount = 0;

   DWORD dwSize = ImmGetCandidateListW(m_himc, iList, 0, 0);

   if (dwSize == 0)
   {

      DWORD dw = GetLastError();

      //output_debug_string("error " + __str(dw));

   }

   mem.SetSize(dwSize);

   LPCANDIDATELIST pc = (LPCANDIDATELIST)mem.GetData();

   if (!pc)
   {

      return "";

   }

   ImmGetCandidateListW(m_himc, iList, pc, (DWORD)mem.GetSize());

   int iTest = pc->dwOffset[0];

   auto iTest2 = (byte *)& pc->dwOffset[pc->dwCount] - (unsigned char *)pc;

   auto pwsz = (WCHAR *)(mem.GetData() + pc->dwOffset[pc->dwSelection]);

   return pwsz;

}


CStringW imm_context::get_candidate()
{

   for (int i = 3; i >= 0; i--)
   {

      CStringW str = _get_candidate((int)i);

      if (str.GetLength() > 0)
      {

         return str;

      }

   }

   return "";

}




