#pragma once


#define TEXT_COMPOSITION_MESSAGE_UPDATE_CANDIDATE_WINDOW_POSITION 256
#define TEXT_COMPOSITION_MESSAGE_UPDATE_CARET 512


#include "styledtext.h"
#include <imm.h>

class imm_client :
   public CScrollView
{
public:

   DECLARE_DYNCREATE(imm_client)

   bool                                         m_bResetting;
   CString                                      m_strImeComposition;
   bool                                         m_bImeCandidateOpened;
   bool                                         m_bImeCancelling;
   int                                          m_iCandidateList;
   CByteArray                                   m_memCandidateList;
   LPCANDIDATELIST                              m_pcandidatelist;

   
   imm_client();
   virtual ~imm_client();

   virtual bool is_text_mode() const;

   virtual void OnDraw(CDC* pDC);  // overridden to draw this view

   DECLARE_MESSAGE_MAP()
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg LRESULT OnIme(WPARAM wparam, LPARAM lparam);
   afx_msg LRESULT OnImeNotify(WPARAM wparam, LPARAM lparam);
   afx_msg LRESULT OnImeComposition(WPARAM wparam, LPARAM lparam);
   afx_msg LRESULT OnImeRequest(WPARAM wparam, LPARAM lparam);
   afx_msg LRESULT OnImeControl(WPARAM wparam, LPARAM lparam);
   afx_msg LRESULT OnChar11(WPARAM wparam, LPARAM lparam);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnSetFocus(CWnd* pOldWnd);

   virtual int on_text_composition_message(int iMessage) ;

   virtual void on_text_composition(CStringW wstr, enum_composition_state ecompositionstate);
   
   virtual void on_text_composition_done();

   virtual void insert_text(CStringW str, bool bForceNewStep);

   virtual bool is_text_composition_active() const;

   virtual void on_edit_undo();

   virtual void get_text_composition_area(LPRECT lprect);
};

