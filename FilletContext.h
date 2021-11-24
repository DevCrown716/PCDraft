// FilletContext.h: interface for the CFilletContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLETCONTEXT_H__0BA71594_F891_11D2_B5E0_C87C8D8A317F__INCLUDED_)
#define AFX_FILLETCONTEXT_H__0BA71594_F891_11D2_B5E0_C87C8D8A317F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TrimContext.h"

class CMDShape;
class CMDArcShape;
class CPCDraftView;
class CDraftContext;

class CFilletContext : public CTrimContext
{
public:
	CFilletContext();
	virtual ~CFilletContext();

	bool CanFillet(CMDShapeList* pList);
	int CreateFillet();
	void DrawFillet(CDraftContext* pDC);
	int TestFillet(const LongPoint& lpt);
	CMDShape* FinishCreateFillet(int nFillet);
	void ClearFillet();

	Coord m_fRadius;
	bool m_bSmallerArc;

protected:
	bool makeFillet(const LongPoint& lpt1,const LongPoint& lpt2,const LongPoint& lpt3,const LongPoint& lpt4,LongPoint& lptCenter,LongPoint& lip12,LongPoint& lip34);
	CMDArcShape* createArc(const LongPoint& lptCenter,const LongPoint& lptFrom, const LongPoint& lptTo);

	CMDShape* m_Fillet[4];
	bool m_bCreating;
};

#endif // !defined(AFX_FILLETCONTEXT_H__0BA71594_F891_11D2_B5E0_C87C8D8A317F__INCLUDED_)
