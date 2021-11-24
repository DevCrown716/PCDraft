// MDText.h: interface for the CMDText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDTEXT_H__CDB88B9A_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
#define AFX_MDTEXT_H__CDB88B9A_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDFillShape.h"
#include "StyledText.h"

#include "text_format_rtf/_text_format_rtf.h"

struct CTextTrackContext : public CTrackContext
{
	LongPoint pt[4];
};

const Coord kTopLeftMargin	= short ( 3 ) ;		/* top left margin for view rect */
const Coord kBotRightMargin	= short ( 3 ) ;		/* bottom right margin for view rect */
const Coord kDestRectMargin 	= short ( 2 ) ;		/* padding for destination rect */



class CMDText : public CMDFillShape
{
	DECLARE_SERIAL(CMDText)
public:
	enum TextType
	{
		kTextAutoSize,
		kTextUserSize
	};
	
	CMDText();
	CMDText(TextType nType);

	void Serialize(CArchive& ar);

// pen model
	bool ModifyPenModel(const PenModel &penModel, int nWhat);
// text params
	bool HaveTextParams(int nWhat) const { return true; }
	const TextParams& GetTextParams() const { return m_TextParams; }
	void SetTextParams(const TextParams& tp);
	void ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC);
// shape points
	int	 GetNumShapePoints() const { return 4; }
	LongPoint GetShapePoint(int nIndex) const;
	void SetShapePoint(int nIndex, const LongPoint &pt);
// shape handles
	int GetNumShapeHandles() const { return m_nType==kTextAutoSize?4:8; }
	LongPoint GetShapeHandle(int nIndex) const;
// drawing
	void DrawShape(CDraftContext* pDC) const;
	void DrawHandles(CDraftContext* pDC) const;
	void DrawShapeOutline(CDraftContext* pDC) const;
// hit-testing
	bool PointInShape(const pointd& pt, CMapper* pMapper) const;
// operations
	void DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape);
	void DoShapeScale(const LongRatio& ratio,double factor=0);
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
	void MakeSelfContained(CPCDraftDoc* pDoc,bool CopyDIB=false);
// tracking
	CTrackContext* TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper);
	void TrackFeedback(LongPoint pt, CTrackContext* pTrackContext);
	void TrackShowSize(CTrackContext* pTrackContext);
	void TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext);
	void TrackComplete(CTrackContext* pTrackContext);
// report
	double ReportCalcWidth(const LongRatio& ratio) const { return 0; }
	double ReportCalcHeight(const LongRatio& ratio) const { return 0; }
	double ReportCalcLength(const LongRatio& ratio) const { return 0; }
	double ReportCalcPerimeter(const LongRatio& ratio) const { return 0; }
	double ReportCalcXDim(const LongRatio& ratio) const { return 0; }
	double ReportCalcYDim(const LongRatio& ratio) const { return 0; }
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);


	TextType GetTextType() { return m_nType; }
	void SetText(CStyledText& st) { m_StyledText.Copy(st); }
	CStyledText& GetText() { return m_StyledText; }
// Linked Vertex
	void OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex);
	void GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper);
// DXF
	void WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const;



protected:
	TextParams m_TextParams;
	TextType m_nType;
	LongPoint m_ptCorner[4];
	CStyledText m_StyledText;
};

#endif // !defined(AFX_MDTEXT_H__CDB88B9A_81F2_11D2_B4E2_DAC3AA148C6E__INCLUDED_)
