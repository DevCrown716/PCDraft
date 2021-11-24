// LinkVertex.h: interface for the CLinkVertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKVERTEX_H__729DAEE4_BFBE_41BE_9A92_76BA306AC802__INCLUDED_)
#define AFX_LINKVERTEX_H__729DAEE4_BFBE_41BE_9A92_76BA306AC802__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMDShape;
class CPCDraftView;

class CLinkVertex : public CObject  
{
	DECLARE_SERIAL(CLinkVertex)
public:
	CLinkVertex();
	CLinkVertex(const CLinkVertex& lv);
	virtual ~CLinkVertex();
	virtual void Serialize(CArchive& ar);
public:
	long m_nKey;
	union
	{
		CMDShape* m_pShape;
		long m_nShape;
	};
	int m_nVertex;
	bool m_bResolved;	// for importing Mac files
};

class CLinkVertexList : public CTypedPtrList<CObList,CLinkVertex*>
{
public:
	void RemoveSinglePoints();
	bool IsLinkedShape(const CMDShape* pShape, int nLinkVertex, CMDShape** pLinkedShape);
	void DeleteAll();
	void DeleteAllByShapeAndVertex(CMDShape* pShape,int nVertex,bool bIncludeLine=true);
	bool DeleteAllByShape(CMDShape* pShape);
	void ResolveObjectIndex(long nObjectIndex,CMDShape* pShape);
	void FlattenObjectIndex(CMDShape* pShape,long nObjectIndex);
	void TransferResolved(CLinkVertexList& linkVertexList);
	void Copy(const CLinkVertexList& linkVertexList);
	void SwapShapeVertex(CMDShape* pShape,int nVertex1,int nVertex2);
	void GetVertexes(CUIntArray& vertexes,CMDShape* pShape,int nVertex);
	void ReplaceShape(CMDShape* pOldShape,CMDShape* pNewShape);
	void ReplaceShape(CMDShape* pOldShape,int nOldVertex,CMDShape* pNewShape,int nNewVertex);
	void SplitShape(CMDShape* pOldShape,int nSplitVertex,CMDShape* pNewShape1,CMDShape* pNewShape2);
	void AdjustForAddVertex(CMDShape* pShape,int nVertex,int nShift=1);
	void AdjustForSubVertex(CMDShape* pShape,int nVertex,int nShift=1);
protected:
	void RemoveIfSingle(POSITION rpos,int nKey, int count);
};

class CLinkVertexContext : public CTypedPtrArray<CObArray,CMDShape*>
{
public:
	bool Include(CMDShape* pShape);
public:
	CLinkVertexList* m_pList;
	CPCDraftView* m_pView;
};

#endif // !defined(AFX_LINKVERTEX_H__729DAEE4_BFBE_41BE_9A92_76BA306AC802__INCLUDED_)
