// ArcSketcher.h: interface for the CArcSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCSKETCHER_H__C476E0B3_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_ARCSKETCHER_H__C476E0B3_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CArcSketcher : public CSketcher  
{
public: 
	enum Mode {   kRadius, kElliptic, kThreePoint }; 
	CArcSketcher(Mode nMode) : m_nMode(nMode), m_nStage(0) {} 

// CSketcher override 
	ContinueMode QueryContinue(); 
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	LongPoint AdjustPosition(LongPoint pt); 
	void OnDrawSketcher(CDC * pDC);
	void OnAddPt(const LongPoint& ptAdd);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);

protected: 
	Mode m_nMode; 
	int m_nStage; 
	LongPoint m_pt[4]; 
	int m_nArcDirection;
	bool m_bSnapLength;
};

#endif // !defined(AFX_ARCSKETCHER_H__C476E0B3_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
