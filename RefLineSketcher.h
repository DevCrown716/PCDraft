// RefLineSketcher.h: interface for the CRefLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFLINESKETCHER_H__C476E0B8_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_REFLINESKETCHER_H__C476E0B8_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineSketcher.h"

class CRefLineSketcher : public CLineSketcher  
{
public:
	CRefLineSketcher() {}
	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC,const LongPoint& pt);
};

#endif // !defined(AFX_REFLINESKETCHER_H__C476E0B8_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
