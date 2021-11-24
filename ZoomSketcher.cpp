// ZoomSketcher.cpp: implementation of the CZoomSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <float.h>
#include "ZoomSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "PCDraft.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CZoomSketcher::OnDrawSketcher(CDC *pDC)
{
	if(m_bArea)
	{
		pDC->DrawFocusRect(&m_thinRect);
		pDC->DrawFocusRect(&m_thickRect);
	}
}

void CZoomSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_ptFrom=m_ptTo=pt;
	m_bArea=false;
}

void CZoomSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptTo=ptUpdate;
	pointd ptFrom=m_pView->m_Mapper.LPtoDP(m_ptFrom);
	pointd ptTo=m_pView->m_Mapper.LPtoDP(m_ptTo);
	if(abs(ptFrom.x-ptTo.x)>5 || abs(ptFrom.y-ptTo.y)>5)
	{
		CPCDraftDoc* pDoc=m_pView->GetDocument();
		m_bArea=true;
		// Calculate new box
		m_thinRect.SetRect(ptFrom,ptTo);
		m_thinRect.NormalizeRect();
		CRect viewRect;
		m_pView->GetClientRect(&viewRect);
		int viewWidth=viewRect.right-viewRect.left;
		int viewHeight=viewRect.bottom-viewRect.top;
		int rectWidth=m_thinRect.right-m_thinRect.left;
		int rectHeight=m_thinRect.bottom-m_thinRect.top;
		double hRatio=rectWidth?double(viewWidth)/rectWidth:DBL_MAX;
		double vRatio=rectHeight?double(viewHeight)/rectHeight:DBL_MAX;

//#define _FLOAT_ZOOM
#ifdef _FLOAT_ZOOM
		double fZoomFactor=pDoc->m_fZoomFactor?pDoc->m_fZoomFactor:FloatTheFixedZoomFactor(pDoc->m_nZoomFactor);
		m_fZoom=min(hRatio,vRatio);
		if((fZoomFactor*m_fZoom)>FloatTheFixedZoomFactor(pDoc->m_nMaxZoom))
			m_fZoom=FloatTheFixedZoomFactor(pDoc->m_nMaxZoom)/fZoomFactor;
		else
		{
			short oneToOneRes = pDoc->m_nDisplayRes;
			if ( ( oneToOneRes == kEngRes )  // 72 dpi
			|| ( oneToOneRes == kEngDecRes ) // 86 dots per 0.1 ft
			|| ( oneToOneRes == kMetRes ) ) // 28 dots per cm
			{
				// Original IDD grid which always divide up into powers of 2
				// Currently always comes into here
				WrFixed fOneToOneRes = oneToOneRes ;
				WrFixed Res = fOneToOneRes * fZoomFactor * m_fZoom ;
				long l = Res.GetFixed () ; // Get as bit pattern
				short shift ;
				for ( shift = 0 ; l > 10 ; ++shift )
					l >>= 1 ; 
				l <<= shift ;
				Res.SetFromFixed ( l ) ;
				m_fZoom = Res / ( fOneToOneRes * fZoomFactor ) ; // Restrict the zoom so can draw grid neatly
			}
			// else 70 dpi, 90 ( or maybe 100 ) dots per 0.1 ft or 3 dots per mm
		}
		int width=int(viewWidth/m_fZoom);
		int height=int(viewHeight/m_fZoom);
#endif
		m_nZoom=0; // number of powers of 2 to zoom in
		while(hRatio>=2.0 && vRatio>=2.0 && (pDoc->m_nZoomFactor+m_nZoom)<pDoc->m_nMaxZoom)
		{
			hRatio*=0.5;
			vRatio*=0.5;
			++m_nZoom;
		}
#ifndef _FLOAT_ZOOM
		int width=viewWidth>>m_nZoom;
		int height=viewHeight>>m_nZoom;
#endif
		m_thickRect.left=m_thinRect.left+(rectWidth-width)/2 ;
		m_thickRect.top=m_thinRect.top+(rectHeight-height)/2 ;
		m_thickRect.right=m_thickRect.left+width ;
		m_thickRect.bottom=m_thickRect.top+height ;
	}
	else
		m_bArea=false;
}
