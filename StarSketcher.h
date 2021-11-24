// StarSketcher.h: interface for the CStarSketcher class.
// Infinisys Ltd
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Sketcher.h"

class CStarSketcher: public CSketcher
{
public:
   enum Mode { kDiameter,kCenterVertex };
   CStarSketcher(Mode nMode,int nSides, int iThickness);
   ~CStarSketcher();
   // CSketcher override
   void OnBeginSketch(CDC* pDC,const LongPoint& pt);
   void OnDrawSketcher(CDC * pDC);
   bool OnCompleteSketch(CDC* pDC,const LongPoint& pt);
   void OnUpdatePt(const LongPoint& ptUpdate);
protected:
   Mode m_nMode;
   int	 m_nSides;
   int   m_iThickness;
   LongPoint  m_ptFrom;
   LongPoint  m_ptTo;
   LongPoint* m_pPoints;
};


