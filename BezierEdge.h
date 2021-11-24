// BezierEdge.h: interface for the CBezierEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIEREDGE_H__2A348DF4_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
#define AFX_BEZIEREDGE_H__2A348DF4_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Edge.h"
#include "lpoint.h"	// Added by ClassView

class CBezierEdge : public CEdge  
{
	DECLARE_SERIAL(CBezierEdge)
public:
	virtual double SubAreaCalc(const LongRatio &ratio);
	CBezierEdge() : CEdge(kBezierEdge) {}
	CBezierEdge(const LongPoint& lptFrom,const LongPoint& lptFromCtl,const LongPoint& lptToCtl,const LongPoint& lptTo) : CEdge(lptFrom,lptTo,kBezierEdge), m_lptFromCtl(lptFromCtl), m_lptToCtl(lptToCtl) {}

	void Serialize(CArchive& ar);

	virtual void Draw(CDraftContext* pDC) const override;
   virtual void DrawOutlinePath(CDraftContext* pDC) const override;
	bool PtAtEdge(const pointd& pt, const CMapper* pMapper) const;
	CMDShape* CreateShape() const;
	CEdge* CreateCopy() const;
	void Reverse();
	LongRect GetExtents() const;
	void Offset(const LongPoint& lpt);
	bool IsEqual(CEdge* pEdge);
	bool Concatenate(CEdge *pEdge);
	void Scale(const LongRatio& ratio, double factor=0);
	void Rotate(Angle angle,const LongPoint& lptAbout);
	void Mirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void Flip(bool bVertical,const LongPoint& lpt);

	LongPoint m_lptFromCtl;
	LongPoint m_lptToCtl;
protected:
	virtual int GetNumPoints() { return 4; }
	virtual const LongPoint& GetPoint(int i) const;
	virtual void SetPoint(int i,const LongPoint& lpt);
};



#endif // !defined(AFX_BEZIEREDGE_H__2A348DF4_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
