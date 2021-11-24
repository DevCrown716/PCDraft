// Edge.h: interface for the CEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDGE_H__2A348DF1_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
#define AFX_EDGE_H__2A348DF1_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_

#include "lpoint.h"	// Added by ClassView
#include "pointf.h" // Infinisys Ltd. GDI+
#include "pointd.h" // Infinisys Ltd. GDI+
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMapper;
class CMDShape;
class CDraftContext;

class CEdge;
typedef CTypedPtrList<CObList,CEdge*> CEdgeList;

class CEdge : public CObject  
{
	DECLARE_SERIAL(CEdge)
public:
	virtual double SubAreaCalc(const LongRatio& ratio);
	enum
	{
		kLineEdge,
		kArcEdge,
		kBezierEdge
	};

   CEdge(short nType=kLineEdge): m_nType(nType),m_iBezierIndex(-1){}
   CEdge(const LongPoint& lptFrom,const LongPoint& lptTo,short nType=kLineEdge): m_lptFrom(lptFrom),m_lptTo(lptTo),m_nType(nType),m_iBezierIndex(-1){}

	virtual void Serialize(CArchive& ar);
	virtual void Draw(CDraftContext* pDC) const;
   virtual void DrawOutlinePath(CDraftContext* pDC) const; // Infinisys Ltd. GDI+
	virtual bool PtAtEdge(const pointd& pt,const CMapper* pMapper) const;
	virtual bool PtAtEdge(const LongPoint& lpt) const;
	virtual void PerpPt(const LongPoint& lptTo,LongPoint* lptFrom) const;
	virtual	LongPoint MidPoint() const { return (m_lptFrom+m_lptTo)>>1; }
	virtual CMDShape* CreateShape() const;
	virtual CEdge* CreateCopy() const;
	virtual void Reverse();
	virtual LongRect GetExtents() const;
	virtual void Offset(const LongPoint& lpt);
	virtual bool IsEqual(CEdge* pEdge);
	virtual int Intersect(CEdge* pEdge,CEdgeList* pList=NULL,CEdgeList* pOtherList=NULL,bool bRevert=false);
	virtual bool Concatenate(CEdge* pEdge);
	virtual void Rotate(Angle angle,const LongPoint& lptAbout);
	virtual void Scale(const LongRect& lrcFrom, const LongRect& lrcTo);
	virtual void Scale(const LongRatio& ratio, double factor=0);
	virtual void Mirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	virtual void Flip(bool bVertical,const LongPoint& lpt);
	virtual bool Snap(LongPoint* lptSnap,const LongRect& lrcProx, short nType) const;

//protected:
	short m_nType;
	LongPoint m_lptTo;
	LongPoint m_lptFrom;
   int m_iBezierIndex;
static void ClearLastEdgeInfo() { gLastEdgeSign=0; }
protected:
	virtual int GetNumPoints() { return 2; }
	virtual const LongPoint& GetPoint(int i) const;
	virtual void SetPoint(int i,const LongPoint& lpt);
	static double lineArea(const LongRatio& ratio, const LongPoint& from, const LongPoint& to);
	static short gLastEdgeSign;
};

typedef CTypedPtrList<CObList,CEdge*> CEdgeList;

#endif // !defined(AFX_EDGE_H__2A348DF1_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
