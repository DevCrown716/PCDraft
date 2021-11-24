// MDRndRectangle.h: interface for the CMDRndRectangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDRNDRECTANGLE_H__617A52F4_8219_11D2_B4E3_B35290854065__INCLUDED_)
#define AFX_MDRNDRECTANGLE_H__617A52F4_8219_11D2_B4E3_B35290854065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDRectangle.h"

struct CRndRectTrackContext : public CRectTrackContext
{
	LongPoint ptCorner;
	Angle rotAngle;
};

class CMDRndRectangle : public CMDRectangle  
{
	DECLARE_SERIAL(CMDRndRectangle)
public:
	enum CornerType
	{
		kCornerElliptical,
		kCornerCircle,
		kCornerProportional
	};

	CMDRndRectangle();
	CMDRndRectangle(CornerType);

	void Serialize(CArchive& ar);

// shape handles
	int GetNumShapeHandles() const { return 9; }
	LongPoint GetShapeHandle(int nIndex) const;
// edges
	int	GetNumShapeEdges() const { return 8; }
	CEdge* GetShapeEdge(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
// unglue
	void Unglue(CMDShapeList* pList) const;
// handles
	bool CanAddPt(CMDShape** ppNewShape) const { return false; }
	bool CanRemovePt() const { return false;}
//	tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// area
	bool HaveArea() const { return true; }
	double GetRawArea(const LongRatio& ratio) const;
// report
	double ReportCalcPerimeter(const LongRatio& ratio) const;
// resize
	const ResizeInfo* GetResizeInfo() const;
	void GetResizeValue(ResizeValue* pValues,int& nHandle);
	void SetResizeValue(ResizeValue* pValues,int nHandle=0);
// dxf
	void WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);


public:
	LongPoint m_lptCornerDelta;
protected:
	int m_nCornerType;

	void AdjustCornerDelta(LongPoint& lptDelta,const LongPoint& lptDiag) const;
	static void CalcRotPoints(const LongPoint pt[], LongPoint ptDelta, Angle nAngle, LongPoint ptRot[]);
};

#endif // !defined(AFX_MDRNDRECTANGLE_H__617A52F4_8219_11D2_B4E3_B35290854065__INCLUDED_)
