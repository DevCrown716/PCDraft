// MDBitmap.h: interface for the CMDBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDBITMAP_H__ABF80823_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_)
#define AFX_MDBITMAP_H__ABF80823_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDFillShape.h"

struct CBitmapTrackContext : public CTrackContext
{
	bool bCrop;
	LongRect lrc;
	CRect rcBounds;
};

class CMDBitmap : public CMDFillShape  
{
	DECLARE_SERIAL(CMDBitmap)

	CMDBitmap();
public:
	CMDBitmap(const CRect& rc,const LongRect& lrc,HBITMAP hDib);
	virtual ~CMDBitmap();

	virtual void Serialize(CArchive& ar);

// flags
	bool IsFilled() const { return true; }
// datum
	void CalcCenterDatum(LongPoint& lpt);
// extents
	LongRect CalculateBasicExtents() const;
// shape points
	LongPoint GetShapePoint(int nIndex) const;
// shape handles
	int GetNumShapeHandles() const { return 8; }
	LongPoint GetShapeHandle(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
//	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
//	int	TestHandles(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeOffset(const LongPoint& delta);
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint lpt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);

// Linked Vertex
	virtual	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);

protected:
//	LongPoint m_ptCorner[4];
	LongRect m_lrc;
	CRect m_rc;
	HBITMAP m_hDib;
};

#endif // !defined(AFX_MDBITMAP_H__ABF80823_E15A_11D2_B5A0_B4ED62CA2E7F__INCLUDED_)
