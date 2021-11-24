#pragma once


#include "Imm.h"


class imm_context
{
public:


   HIMC                                m_himc;
   CView *                             m_pview;


   imm_context(CView * pview);
   ~imm_context();


   operator HIMC () const { return m_himc; }

   bool close();

   bool is_opened() const;

   void clear_composition();

   bool close_candidate(int iIndex = 0);

   CStringW get_string(int iStr = GCS_COMPSTR);
   void get_attr(CArray < byte > & ba, int iStr = GCS_COMPATTR);
   void get_clause(CArray < int > & ia, int iStr = GCS_COMPCLAUSE);

   CStringW _get_candidate(int iList);

   CStringW get_candidate();


};


