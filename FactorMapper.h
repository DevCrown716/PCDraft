// FactorMapper.h: interface for the CFactorMapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORMAPPER_H__2B3F12A4_5FC7_11D3_B6C2_B5060148A774__INCLUDED_)
#define AFX_FACTORMAPPER_H__2B3F12A4_5FC7_11D3_B6C2_B5060148A774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mapper.h"
#include "PCDraftDoc.h"

class CFactorMapper : public CMapper  
{
public:
	CFactorMapper(double factor=1, const LongPoint& lptOrg=LongPoint::kZeroPt) : m_Factor(factor), m_lptOrg(lptOrg) {}

	long NoZoomToZoom(long l) const;
	Coord NoZoomToZoom(const Coord& f) const;
   double NoZoomToZoom(double d) const;  // Infinisys Ltd. 2021
   long ZoomToNoZoom(long l) const; // Infinisys Ltd. 2021
   Coord ZoomToNoZoom(const Coord& f) const; // Infinisys Ltd. 2021
   double ZoomToNoZoom(double d) const; // Infinisys Ltd. 2021
   long LPtoDP(const Coord& f) const;
   double LPtoDPF(const Coord& f) const;
	CPoint LPtoDP(const LongPoint& lpt) const;
	CRect LPtoDP(const LongRect& lrc) const;
   void LPtoDP(pointd & pt, const LongPoint &lpt) const; // Infinisys Ltd. GDI+ Print
   Coord DPtoLP(int i) const; // Infinisys Ltd. GDI+ Print
   Coord DPtoLP(long l) const;
   Coord DPtoLP(float f) const; // Infinisys Ltd. GDI+ Print
   Coord DPtoLP(double d) const; // Infinisys Ltd. GDI+ Print
   virtual LongPoint DPtoLP(const pointd& lpt) const; // Infinisys Ltd. GDI+ Print
   LongPoint DPtoLP(const CPoint& pt) const;
	LongRect DPtoLP(const CRect& rc) const;
	virtual Coord Scalar2Long(double value) const;
	void SetDocument(CPCDraftDoc* pDoc) { m_pDoc=pDoc; }
	CPCDraftDoc* GetDocument() { ASSERT(m_pDoc); return m_pDoc;}

protected:
	double m_Factor;
	LongPoint m_lptOrg;
	CPCDraftDoc* m_pDoc; // Infinisys Ltd. 2021
};

#endif // !defined(AFX_FACTORMAPPER_H__2B3F12A4_5FC7_11D3_B6C2_B5060148A774__INCLUDED_)
