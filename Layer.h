#ifndef __LAYER_H__
#define __LAYER_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#include "MDShape.h"
#include "LinkVertex.h"

class CPCDraftDoc;

class CLayer : public CObject  
{
	DECLARE_SERIAL(CLayer)
public:
	CLayer();
   CLayer(const CLayer & layer);
	~CLayer();
	virtual void Serialize(CArchive& ar);

	bool Draw(CDraftContext *pDC,bool bForceFill=false,bool bHideText=false,CMDShape* pActiveShape=NULL);
	CMDShape* ShapeFromPoint(const pointd& pt, CMapper* pMapper,bool bHideText=false,bool bNoGroup=false,CMDShape* pStart=NULL);
	CMDShape* ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex);
	bool ReplaceShape(CMDShape* pShape,CMDShape* pNewShape);
	bool ReplaceShape(CMDShape* pShape,CMDShapeList* pNewShapes);
	bool SetScale(int nScale, CPCDraftDoc* pDoc);
	bool Rescale(int nScale, CPCDraftDoc* pDoc,const LongRatio& oldRatio,const LongRatio& newRatio,double factor=0);
	POSITION TopmostShape(CMDShapeList* pList);
	void SortShapes(CMDShapeList &list,bool bRemoveLocked=true);
	LongRect GetCumulativeExtents(bool bBasic=true);
	void RecalcGroupExtents(CMDShape* pShape);
	long GetVertexKey() { return ++m_nVertexKey; }
	//by TSM
	void MakeSelfContainedShapes(CPCDraftDoc* pDoc);

public:
	int GetObjectCount(bool bOverall);
	void DeleteAllShapes();
	void UpdateDependentShapes(CPCDraftView* pView);
	int m_nSnap;
	int m_nScale;
	bool m_bHidden;
	bool m_bGrayed;
	CString m_strName;
	CMDShapeArray m_ShapeList;
	CLinkVertexList m_LinkVertexList;
	long m_nVertexKey;
static bool bIgnoreShapeList;
};

#endif
