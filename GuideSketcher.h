// LineSketcher.h: interface for the CGuideSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIDESKETCHER_H__C476E0AA_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_GUIDESKETCHER_H__C476E0AA_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"
#include "PCDraftDoc.h"

class CGuideSketcher : public CSketcher
{
public:

   CGuideSketcher(bool bVertical, Coord coord =0.0): m_guide(bVertical, coord),m_bInDoc(false) {  }

   CGuide m_guide;
   bool     m_bInDoc;

protected:
   // CSketcher override
   void OnBeginSketch(CDC* pDC, const LongPoint& pt);
   void OnDrawSketcher(CDC * pDC);
   void OnUpdatePt(const LongPoint& ptUpdate);
   bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
   CSketcher::ContinueMode QueryContinue();
};

#endif // !defined(AFX_GUIDESKETCHER_H__C476E0AA_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
