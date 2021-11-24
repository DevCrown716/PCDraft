// ArcEdge.h: interface for the CArcEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCEDGE_H__2A348DF3_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
#define AFX_ARCEDGE_H__2A348DF3_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Edge.h"
#include "lpoint.h"
#include "MDShape.h"

class CArcEdge : public CEdge  
{
	DECLARE_SERIAL(CArcEdge)
public:
	double SubAreaCalc(const LongRatio& ratio);
	CArcEdge() : CEdge(kArcEdge) {}
	CArcEdge(const LongPoint& lptCenter,const LongPoint& Radii,const LongPoint& lptFrom,const LongPoint& lptTo,int nDirection,Angle rotAngle=0) : CEdge(lptFrom,lptTo,kArcEdge), m_lptCenter(lptCenter), m_Radii(Radii), m_nDirection(nDirection),m_RotAngle(rotAngle) {}

	void Serialize(CArchive& ar);

	void Flatten(CMDShapeList *pList);
	void Draw(CDraftContext* pDC) const;
   void DrawOutlinePath(CDraftContext* pDC) const;
	bool PtAtEdge(const pointd& pt, const CMapper* pMapper) const;
	bool PtAtEdge(const LongPoint& lpt) const;
	void PerpPt(const LongPoint& lptTo,LongPoint* lptFrom) const;
	LongPoint MidPoint() const;
	CMDShape* CreateShape() const;
	CEdge* CreateCopy() const;
	void Reverse();
	LongRect GetExtents() const;
	void Offset(const LongPoint& lpt);
	bool IsEqual(CEdge* pEdge);
	int Intersect(CEdge* pEdge,CEdgeList* pList=NULL,CEdgeList* pOtherList=NULL,bool bRevert=false);
	bool Concatenate(CEdge *pEdge);
	void Scale(const LongRect& lrcFrom,const LongRect& lrcTo);
	void Scale(const LongRatio& ratio, double factor=0);
	void Rotate(Angle angle,const LongPoint& lptAbout);
	void Mirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void Flip(bool bVertical,const LongPoint& lpt);
	bool Snap(LongPoint* lptSnap,const LongRect& lrcProx, short nType) const;

static bool SolveBinomial (double *d, double *e, double *f, double *x1, double *x2);
static bool AngleBetween(Angle angle,Angle startAngle,Angle endAngle);

	LongPoint m_lptCenter;
	LongPoint m_Radii;
	int m_nDirection;
	Angle m_RotAngle;
protected:
	virtual int GetNumPoints() { return 3; }
	virtual const LongPoint& GetPoint(int i) const;
	virtual void SetPoint(int i,const LongPoint& lpt);

private:
	bool CheckAngles(const LongPoint& lpt) const;
	LongPoint ArcPoint(Angle angle) const;
};

#endif // !defined(AFX_ARCEDGE_H__2A348DF3_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
