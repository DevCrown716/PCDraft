// MetaContext.h: interface for the CMetaContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_METACONTEXT_H__E1D2BC83_198B_11D3_B634_CF2FE6F9277F__INCLUDED_)
#define AFX_METACONTEXT_H__E1D2BC83_198B_11D3_B634_CF2FE6F9277F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Edge.h"

class CMDSHape;

enum
{
	kIntersect,kInside,kOutside,kBorder
};

class CMetaContext  
{
public:
	CMetaContext();
	virtual ~CMetaContext();

	void AddEdges(CMDShape* pShape,bool bAtEnd,bool bReverse);
	void DeleteEdges();
	bool IsPoly();
	CMDShape* CreateShape(CMDShape* pSrcShape=NULL, bool bConcatenate=true);

	CEdgeList m_newEdges;
};

#endif // !defined(AFX_METACONTEXT_H__E1D2BC83_198B_11D3_B634_CF2FE6F9277F__INCLUDED_)
