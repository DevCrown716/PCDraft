// EllipticSketcher.h: interface for the CEllipticSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELLIPTICSKETCHER_H__C476E0AE_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_ELLIPTICSKETCHER_H__C476E0AE_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RectSketcher.h"

class CEllipticSketcher : public CRectSketcher  
{
public:
	enum Mode { kEllipseCenter, kEllipseDiagonal };
	CEllipticSketcher(Mode nMode) :
		CRectSketcher(nMode==kEllipseDiagonal?kRectDiagonal:kRectCenter), m_nMode(nMode) {}
protected:
	// CSketcher override
	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	Mode m_nMode;
};

#endif // !defined(AFX_ELLIPTICSKETCHER_H__C476E0AE_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
