// AddContext.h: interface for the CAddContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDCONTEXT_H__9E277627_FBB6_11D2_B5E7_BF3C23BA337F__INCLUDED_)
#define AFX_ADDCONTEXT_H__9E277627_FBB6_11D2_B5E7_BF3C23BA337F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MetaContext.h"
#include "MDShape.h"

class CAddContext : public CMetaContext
{
public:
	CAddContext();
	virtual ~CAddContext();

	bool CanAdd(CMDShape* pAdder,CMDShapeList *pAddList);
	void Add(CMDShape* pAdder,CMDShapeList *pAddList,CMDShapeList *pNewList);
	bool IsValidShape(CMDShape* pShape);
	bool AddShape(CMDShape* pShape,CMDShape* pSubShape,CMDShapeList* pList);

	int posType(const LongPoint& lpt,CMDShape* pShape);
	int edgeType(CEdge* pEdge,CMDShape* pShape);
	CEdge* getNextEdge(POSITION& pos,int nBase,bool bReverse);
	CEdge* getEdgeByPoint(const LongPoint& lpt,POSITION& pos,int& nBase,bool& bReverse);

protected:
	bool AddShape(CMDShape *pBase, CMDShape *pAdder);

	CMDShape* m_Shape[2];
	CEdgeList m_Edges[2];
	CMDShapeList m_newShape;
};

#endif // !defined(AFX_ADDCONTEXT_H__9E277627_FBB6_11D2_B5E7_BF3C23BA337F__INCLUDED_)
