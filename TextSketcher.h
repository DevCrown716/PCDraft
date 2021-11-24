// TextSketcher.h: interface for the CTextSketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTSKETCHER_H__6F7CA213_D71A_11D2_991A_000000000000__INCLUDED_)
#define AFX_TEXTSKETCHER_H__6F7CA213_D71A_11D2_991A_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RubberBandSketcher.h"

class CTextSketcher : public CRubberBandSketcher  
{
public:
	bool OnCompleteSketch(CDC* pDC,const LongPoint& pt);

};

#endif // !defined(AFX_TEXTSKETCHER_H__6F7CA213_D71A_11D2_991A_000000000000__INCLUDED_)
