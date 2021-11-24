#ifndef __OFFSETARCSKETCHER_H__
#define __OFFSETARCSKETCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "sketcher.h"

class CMDArcShape;

class COffsetArcSketcher : public CSketcher  
{
public:
	COffsetArcSketcher(CMDArcShape* pBaseShape) : m_pBaseShape(pBaseShape) {}

	ContinueMode QueryContinue();
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnCancelSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	void OnUpdatePt(const LongPoint& ptUpdate);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
protected:
	LongPoint m_ptCenter;
	LongPoint m_ptRadius;
	LongPoint m_pt[3];
	CMDArcShape* m_pBaseShape;
};

#endif
