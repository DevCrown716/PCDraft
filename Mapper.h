// Mapper.h: interface for the CMapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPPER_H__2A348DF7_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
#define AFX_MAPPER_H__2A348DF7_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_

#include "lpoint.h"	// Added by ClassView
#include "pointf.h" // Infinisys Ltd. GDI+
#include "pointd.h" // Infinisys Ltd. GDI+
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMapper : public CObject
{
	DECLARE_DYNAMIC(CMapper)
public:
	CMapper() {}
	virtual ~CMapper() {}

	virtual long NoZoomToZoom(long l) const;
	virtual Coord NoZoomToZoom(const Coord& f) const;
   virtual double NoZoomToZoom(double d) const;// Infinisys Ltd. 2021
   virtual long ZoomToNoZoom(long l) const; // Infinisys Ltd. 2021
   virtual Coord ZoomToNoZoom(const Coord& f) const; // Infinisys Ltd. 2021
   virtual double ZoomToNoZoom(double d) const; // Infinisys Ltd. 2021
   virtual long LPtoDP(const Coord& f) const;
   virtual double LPtoDPF(const Coord& f) const; // Infinisys Ltd. GDI+
   virtual void LPtoDP(pointd & p, const LongPoint& inPoint) const; // Infinisys Ltd. GDI+
   virtual pointd LPtoDPF(const LongPoint& lpt) const; // Infinisys Ltd. GDI+
   virtual CPoint LPtoDP(const LongPoint& lpt) const;
	virtual CRect LPtoDP(const LongRect& lrc) const;
	virtual Coord DPtoLP(long l) const;
   virtual Coord DPtoLP(int i) const;
   virtual Coord DPtoLP(float f) const;// Infinisys Ltd. GDI+
   virtual Coord DPtoLP(double d) const; // Infinisys Ltd. GDI+
   virtual LongPoint DPtoLP(const pointd& lpt) const; // Infinisys Ltd. GDI+
   virtual LongPoint DPtoLP(const CPoint& pt) const;
	virtual LongRect DPtoLP(const CRect& rc) const;
	virtual Coord Scalar2Long(double value) const;
	virtual short ScreenPenW(short nPenW) const;
	virtual Coord PenW(short nPenW) const;
	virtual float fScreenPenW(float fPenW);
};

#endif // !defined(AFX_MAPPER_H__2A348DF7_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
