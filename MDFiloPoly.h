#pragma once

class CDraftContext;

class CMDFiloPoly
{
public:

   bool m_bP1Set; // Infinisys Ltd. 2021

   CMDFiloPoly(void);		// Constructor
   CMDFiloPoly(bool frameIt);		// Constructor
   virtual			~CMDFiloPoly(void);		// Destructor

   void setFrameIt(bool bSet) { fFrameIt = bSet; }
   virtual void	DrawNow(CDraftContext* pDC) const;
   virtual void	DrawLineNow(CDraftContext* pDC) const;
   virtual void	Reset(void);
   virtual void	PushPoints(const pointd& newHeadPt, const pointd& newTailPt, CDraftContext* pDC);
   //virtual void	PushTailPoint(const pointd& newTailPt, CDraftContext* pDC);
   virtual void	SetOffset(const point& newOffset);
   bool has_points() const {return fHeadPoints.GetCount() > 0 || fTailPoints.GetCount() > 0; }
private:
   pointd			fLastHeadPoint;
   pointd			fLastTailPoint;
   pointd			polyOffset;
   bool			fFrameIt;

   CArray<pointd, pointd>	fHeadPoints;
   CArray<pointd, pointd>	fTailPoints;

   virtual void	CheckPointsAndPurge(CDraftContext* pDC);
};
