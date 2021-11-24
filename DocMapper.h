// DocMapper.h: interface for the CDocMapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCMAPPER_H__2A348DF8_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
#define AFX_DOCMAPPER_H__2A348DF8_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mapper.h"
#include "lpoint.h"	// Added by ClassView

class CPCDraftDoc;
class CPCDraftView;

class CDocMapper : public CMapper  
{
	DECLARE_DYNAMIC(CDocMapper)
public:
   // Created Local ZoomLineWeights Flags
   BOOL m_bZoomLineWeights; // Infinisys Ltd. 2021

	virtual float fScreenPenW(float fPenW);
   CDocMapper(); 
   CDocMapper(CPCDraftDoc* pDoc); 

	void SetDocument(CPCDraftDoc* pDoc) { m_pDoc=pDoc; }
	CPCDraftDoc* GetDocument() { ASSERT(m_pDoc); return m_pDoc;}

	long NoZoomToZoom(long l) const;
	Coord NoZoomToZoom(const Coord& f) const;
   double NoZoomToZoom(double d) const; // Infinisys Ltd. 2021
   long ZoomToNoZoom(long l) const; // Infinisys Ltd. 2021
   Coord ZoomToNoZoom(const Coord& f) const; // Infinisys Ltd. 2021
   double ZoomToNoZoom(double d) const; // Infinisys Ltd. 2021
   long LPtoDP(const Coord& f) const;
   double LPtoDPF(const Coord& f) const; // Infinisys Ltd. GDI+
   virtual pointd LPtoDPF(const LongPoint& lpt) const; // Infinisys Ltd. GDI+
	CPoint LPtoDP(const LongPoint& lpt) const;
	CRect LPtoDP(const LongRect& lrc) const;
   virtual void LPtoDP(pointd & p, const LongPoint& inPoint) const override;// Infinisys Ltd. GDI+
	Coord DPtoLP(long l) const;
   Coord DPtoLP(int l) const override;
   virtual Coord DPtoLP(float f) const override;// Infinisys Ltd. GDI+
   virtual Coord DPtoLP(double d) const override; // Infinisys Ltd. GDI+
	LongPoint DPtoLP(const CPoint& pt) const;
	LongRect DPtoLP(const CRect& rc) const;
   virtual LongPoint DPtoLP(const pointd& lpt) const override; // Infinisys Ltd. GDI+
	Coord Scalar2Long(double value) const;
	short ScreenPenW(short nPenW) const;
	Coord PenW(short nPenW) const;
protected:
	CPCDraftDoc* m_pDoc;
};

#endif // !defined(AFX_DOCMAPPER_H__2A348DF8_D543_11D2_B58B_D9E08AF72B7F__INCLUDED_)
