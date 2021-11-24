// MDDimStr.h: interface for the CMDDimStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDDIMSTR_H__F1BFC877_10F5_11D3_B61F_BB77D2F1377F__INCLUDED_)
#define AFX_MDDIMSTR_H__F1BFC877_10F5_11D3_B61F_BB77D2F1377F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDFillShape.h"

class CDimUnit;

enum DimStrAlign
{
	kDSCenter,kDSLeft,kDSRight,kDSTop,kDSBottom
};

struct CDimStrTrackContext : public CTrackContext
{
	LongPoint lpt[4];
};

class CMDDimStr : public CMDFillShape  
{
	DECLARE_SERIAL(CMDDimStr)
public:
	
   CMDDimStr(bool bAttached=true,CMDShape* pOwner=NULL);
	virtual ~CMDDimStr() {}
   
   friend class CMDLineDim;
   friend class CMDRadialDim;

	void Serialize(CArchive& ar);


   //CRgn* GetClip(CDraftContext* pDC);
   Gdiplus::GraphicsPath* GetClip(CDraftContext* pDC); // Infinisys Ltd. - GDI+

// extents
	LongRect CalculateBasicExtents() const;
// text params stubs
	bool HaveTextParams(int nWhat) const { return nWhat&tpUseFont; }
	const TextParams& GetTextParams() const;
	void ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC);
// shape points
	int GetNumShapePoints() const { return 1; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint& pt);
// shape handles
	int GetNumShapeHandles() const {return 0; }
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
	void DrawShapeOutlineForFill(CDraftContext* pDC) const { DrawShapeOutline(pDC); }
// hit-testing
	bool PointInFill(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
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

	void GetValues(CString& sValue,CString& sTol1,CString& sTol2);
	void SetValues(CDraftContext* pDC,const CString& sValue,const CString& sTol1="",const CString& sTol2="");
	void SetAlign(int nAlign) { m_nAlign=nAlign; }
	void RecalcExtent(CDraftContext* pDC);
	void CalcCorners();
	void WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;
	void DXFWriteFrame(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;

	bool IsAttached() const { return m_bAttached; }
	void Attach() { m_bAttached=true; }
	void Detach() { m_bAttached=false; }
	CMDShape* GetAttachedShape() const { return (m_bAttached)?m_pOwner:NULL; }
	void SetOwner(CMDShape* pOwner) { m_pOwner=pOwner; }
	CMDShape* GetOwner() { return m_pOwner; }

   LOGFONT & GetLogFont() { return m_lf; }

protected:
	CString m_sValue;
	CString m_sTol1;
	CString m_sTol2;
	int m_nAlign;
	LongPoint m_lptAnchor;
	LongPoint m_lptExtent;
	LongPoint m_lpt[4];
	LOGFONT m_lf;

	bool m_bAttached;
	CMDShape* m_pOwner;
};

#endif // !defined(AFX_MDDIMSTR_H__F1BFC877_10F5_11D3_B61F_BB77D2F1377F__INCLUDED_)
