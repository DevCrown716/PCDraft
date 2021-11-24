// Sketcher.h: interface for the CSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKETCHER_H__C476E0A9_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_SKETCHER_H__C476E0A9_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lpoint.h"
#include "pointf.h" // Infinisys Ltd. GDI+
#include "pointd.h" // Infinisys Ltd. GDI+

class CPCDraftView;
class CDraftContext;

class CSketcher  
{
public:
	enum ContinueMode { Cancel, Complete, Continue, ContinueAddPt, ContinueUpdatePt };
	enum SketchMode { NoScroll = 0x1, NoSnap=0x2, NoSnapToDocument = 0x4 };

   CSketcher(); 
   virtual ~CSketcher();

	// call this in OnLButtonDown or OnRButtonDown
	virtual bool DoSketch(CPCDraftView* pView, CPoint& ptStart, LPRECT lpRectClip=NULL,UINT nSketchMode=0);
protected:
	// begin sketching
	virtual void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	// cancel sketching
	virtual void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	// called when sketching is complete
	virtual bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	// called when mouse or key message received
	virtual CSketcher::ContinueMode QueryContinue();
	// draw sketcher stuff
	virtual void OnDrawSketcher(CDC * pDC) {};
	// default implementation calls OnDrawSketcher
	virtual void OnEraseSketcher(CDC * pDC) { OnDrawSketcher(pDC); }
	/* used to adjust new point position before drawing figure
		for constraint drawing for example */
	virtual LongPoint AdjustPosition(LongPoint pt) { return pt; }
	virtual void OnAddPt(const LongPoint& ptAdd) {};
	virtual void OnUpdatePt(const LongPoint& ptUpdate) {};
protected:
	CPCDraftView* m_pView;
   CPen* m_pPen;
	CPen* m_pOldPen;
	CBrush* m_pOldBrush;
	MSG m_msg;
	int m_nSketchMode;
	Angle m_Angle;
};

#endif // !defined(AFX_SKETCHER_H__C476E0A9_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
