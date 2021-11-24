#ifndef __OFFSETCIRCLESKETCHER_H__
#define __OFFSETCIRCLESKETCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "sketcher.h"

class CMDCircle;

class COffsetCircleSketcher : public CSketcher  
{
public:
	COffsetCircleSketcher(CMDCircle* pBaseShape) : m_pBaseShape(pBaseShape) {}

	ContinueMode QueryContinue();
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	LongPoint m_ptCenter;
	LongPoint m_ptRadius;
	CMDCircle* m_pBaseShape;
};

#endif
