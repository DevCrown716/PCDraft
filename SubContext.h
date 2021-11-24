// SubContext.h: interface for the CSubContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCONTEXT_H__15E3FED3_12D1_11D3_B623_D154FE12267F__INCLUDED_)
#define AFX_SUBCONTEXT_H__15E3FED3_12D1_11D3_B623_D154FE12267F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MetaContext.h"
#include "MDShape.h"

class CSubContext : public CMetaContext
{
public:
	CSubContext();
	virtual ~CSubContext();

	bool CanSub(CMDShape* pSuber,CMDShapeList *pSubList);
	void Sub(CMDShape* pSuber,CMDShapeList *pSubList,CMDShapeList *pNewList);
	bool IsValidShape(CMDShape* pShape);
	bool SubShape(CMDShape* pShape,CMDShape* pSubShape,CMDShapeList* pList);

	int posType(const LongPoint& lpt,CMDShape* pShape);
	int edgeType(CEdge* pEdge,CMDShape* pShape);
	CEdge* getNextEdge(POSITION& pos,int nBase,bool bReverse);
	CEdge* getEdgeByPoint(const LongPoint& lpt,POSITION& pos,int& nBase,bool& bReverse);

protected:
	bool SubShape(CMDShape *pBase, CMDShape *pSuber);
	CMDShape* GetLineCutterShape(const CMDShape* pBase, const LongPoint& lpt1, const LongPoint& lpt2,bool bSide) const;

	CMDShape* m_Shape[2];
	CEdgeList m_Edges[2];
	CMDShapeList m_newShape;
};

#endif // !defined(AFX_SUBCONTEXT_H__15E3FED3_12D1_11D3_B623_D154FE12267F__INCLUDED_)
