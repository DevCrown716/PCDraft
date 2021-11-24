// RegPolySketcher.h: interface for the CRegPolySketcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGPOLYSKETCHER_H__C476E0AF_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
#define AFX_REGPOLYSKETCHER_H__C476E0AF_D47C_11D2_B58A_E01206F5447F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sketcher.h"

class CRegPolySketcher : public CSketcher  
{
public:
	enum Mode {	kSideSide, kCenterSide, kVertexVertex, kCenterVertex };
	CRegPolySketcher(Mode nMode, int nSides);
	~CRegPolySketcher();
	// CSketcher override
	void OnBeginSketch(CDC* pDC, const LongPoint& pt);
	void OnDrawSketcher(CDC * pDC);
	bool OnCompleteSketch(CDC* pDC, const LongPoint& pt);
	void OnUpdatePt(const LongPoint& ptUpdate);
protected:
	Mode m_nMode;
	int	 m_nSides;
	LongPoint  m_ptFrom;
	LongPoint  m_ptTo;
	LongPoint* m_pPoints;
};

#endif // !defined(AFX_REGPOLYSKETCHER_H__C476E0AF_D47C_11D2_B58A_E01206F5447F__INCLUDED_)
