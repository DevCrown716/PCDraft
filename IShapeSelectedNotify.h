#if !defined(ISHAPESELECTEDNOTIFY_H__INCLUDED_)
#define ISHAPESELECTEDNOTIFY_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMDShape;

class IShapeSelectedNotify
{
public:
	virtual void ShapeSelected(CMDShape* pShape)=0;
};

#endif