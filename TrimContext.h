// TrimContext.h: interface for the CTrimContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIMCONTEXT_H__676B6FA3_F895_11D2_B5E1_B5A9BF4F267F__INCLUDED_)
#define AFX_TRIMCONTEXT_H__676B6FA3_F895_11D2_B5E1_B5A9BF4F267F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDShape.h"

class CMDLineShape;

enum {
	kAutoTrim=0,
	kManualTrim,
	kNoTrim
};

class CTrimContext
{
public:
	CTrimContext();
	virtual ~CTrimContext();

	bool CanTrim(CMDShapeList* pList, bool bBoth=false);
	void Trim();
	bool Have2IntersectLine(CMDShapeList* pList,LongPoint& lpt1,LongPoint& lpt2,LongPoint& lpt3,LongPoint& lpt4);

	enum
	{
		kTrimByQuadrant,
		kTrimByNearest,
		kTrimByDist
	};
	CMDLineShape* TrimLine(const LongPoint &lipt,const LongPoint &ltpt,CMDLineShape *pLine,bool bExtend,int nTrimBy=kTrimByDist);

	int m_nTrimMethod;

	CMDLineShape* m_pLines[2];
	LongPoint m_lipt;

	CMDLineShape* m_pNewLines[2];
};

#endif // !defined(AFX_TRIMCONTEXT_H__676B6FA3_F895_11D2_B5E1_B5A9BF4F267F__INCLUDED_)
