// ChamferContext.h: interface for the CChamferContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAMFERCONTEXT_H__8197A0C1_F7F1_11D2_A08F_00A0C9A0C319__INCLUDED_)
#define AFX_CHAMFERCONTEXT_H__8197A0C1_F7F1_11D2_A08F_00A0C9A0C319__INCLUDED_

#include "lpoint.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TrimContext.h"

class CChamferContext : public CTrimContext
{
public:
	CChamferContext();
	virtual ~CChamferContext();

	bool CanChamfer(CMDShapeList* pList);
	CMDLineShape* CreateChamfer();
	bool FindSetBack(Coord dist,const LongPoint& lipt,const LongPoint& lptFar,LongPoint& lptSetBack);

	Coord m_fLine1SetBack;
	Coord m_fLine2SetBack;
	
	LongPoint m_lptFar1;
	LongPoint m_lptFar2;
};

#endif // !defined(AFX_CHAMFERCONTEXT_H__8197A0C1_F7F1_11D2_A08F_00A0C9A0C319__INCLUDED_)
