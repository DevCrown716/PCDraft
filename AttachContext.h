// AttachContext.h: interface for the CAttachContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTACHCONTEXT_H__ACD01863_FA1B_11D2_B5E4_B8E7A751587F__INCLUDED_)
#define AFX_ATTACHCONTEXT_H__ACD01863_FA1B_11D2_B5E4_B8E7A751587F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MetaContext.h"
#include "MDShape.h"

class CAttachContext : public CMetaContext
{
public:
	CAttachContext() {}

	bool CanAttach(CMDShapeList* pList);
	CMDShape* Attach() { return CreateShape(NULL,false); }
	bool FindNextCoincide(CMDShapeList* pList,LongPoint& lptFirst,LongPoint& lptLast);
	bool IsValidShape(CMDShape* pShape);
};

#endif // !defined(AFX_ATTACHCONTEXT_H__ACD01863_FA1B_11D2_B5E4_B8E7A751587F__INCLUDED_)
