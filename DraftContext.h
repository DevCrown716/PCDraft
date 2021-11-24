// DraftContext.h: interface for the CDraftContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAFTCONTEXT_H__09EB3C85_D706_11D2_B590_EA06B8112A7F__INCLUDED_)
#define AFX_DRAFTCONTEXT_H__09EB3C85_D706_11D2_B590_EA06B8112A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StyledText.h"
#include "DrawPrim.h"
#include "DSShape.h"	// Added by ClassView
#include "lpoint.h"	// Added by ClassView
#include "pointf.h" // Infinisys Ltd. GDI+
#include "pointd.h" // Infinisys Ltd. GDI+
#include "Edge.h"
#include <gdiplus.h>
#include <map>

enum enum_orientation
{
   e_orientation_none,
   e_orientation_horizontal,
   e_orientation_vertical,
};

enum
{
	kTrackHandleSize = 5
};

struct PenModel;
struct FillModel;
struct ParallelAttrs;
struct DimStandard;
struct LineDimPoints;
struct AngDimPoints;
struct RadialDimPoints;
class LongRect;
class CMapper;
class CStyledText;
class CPCDraftDoc;
class CVersionDoc;
class UsePath;




class CDraftContext
{
	friend class CDrawPrim;
public:

   CDraftContext(CDC* pDC,CMapper* pMapper,CPCDraftDoc* pDoc=NULL, CVersionDoc * pVersionDoc = NULL);
	~CDraftContext();

   CArray<pointf> m_pointa1;
   CArray<pointf> m_pointa2;
   // Flag for using Old Mode Drawing with GDI (not GDI+)
   bool m_bUseGDI; // Infinisys Ltd. GDI+
   bool m_bPrinting;
   bool m_bCheckRectVisible; // Infinisys Ltd. Bitmap Export
   bool m_bOutlineForFill; // Interface Update - Infinisys Ltd.
   bool m_bTransparentBackground; // Interface Update - Infinisys Ltd.
   bool m_bFinalizeDash;
   bool m_bSwapInsideOutside;
   
   enum_composition_state     m_ecompositionstate; // Inline Japanese - Infinisys Ltd.
   int                        m_nCompositionStartChar; // Inline Japanese - Infinisys Ltd.
   int                        m_nCompositionEndChar; // Inline Japanese - Infinisys Ltd.

   CRect m_rcClientActive; // Infinisys Ltd. GDI+
   LongRect * m_prect; // Pattern/Gradient - Infinisys Ltd


   bool shouldUseDash();

	void SetPen(const PenModel& pm);
	void SetGreyPen();
	void SetOutlinePen(bool bReset=true);
   bool IsOutlinePen()const; // Infinisys Ltd. 2021
	const PenModel& GetPen() const { return m_PenModel; }
   Gdiplus::Bitmap * GetPenPattern() { return m_pbitmapPenPattern.get(); }
	void SetGreyMode(bool bEnable) { m_bGreyMode=bEnable; }
	void SetFill(const FillModel& fm);
	void SetTextParams(const TextParams& tp);
	void SetFont(const LOGFONT& lf);
	void MoveTo(const LongPoint& lpt);
	void Line(const LongPoint& lptFrom,const LongPoint& lptTo,short cap1=0,short cap2=0,short border=0);
	void LineTo(const LongPoint& lpt,short cap1=0,short cap2=0,short border=0);
	void OutsideLineTo(const LongPoint &lptTo);
	void ParallelLine(const LongPoint& lptFrom,const LongPoint& lptTo,const ParallelAttrs& pa);
	void ParallelCaps(const LongPoint& lptFrom,const LongPoint& lptTo,const ParallelAttrs& pa,short border);
	void PolyLine(const LongPointArray& pt,int nCount,bool bClosed,short cap1=0,short cap2=0);
	void MetaPoly(const CEdgeList* pEdges,bool bClosed);
	void Rectangle(const LongPoint& lptTopLeft,const LongPoint& lptRightBottom,Angle rotAngle);
	void Arc(const LongPoint &lptCenter,const LongPoint &lptRadii,const LongPoint &lptFrom, const LongPoint &lptTo,int nDirection, Angle rotAngle,short cap1=0,short cap2=0, bool bAdjustAlign=false, 
      bool bClose = false); // Infinisys Ltd. 2021
	void Arc(const LongRect& lrc, const LongPoint& lptFrom, const LongPoint& lptTo, int nDirection);
	void Ellipse(const LongPoint& lptCenter,const LongPoint& lptRadii,Angle rotAngle);
	void RoundRect(const LongPoint& lptTopLeft, const LongPoint& lptBottomRight, const LongPoint& lptDelta, Angle rotAngle);
	void RoundRect(const LongRect& lrc, const LongPoint& lpt);
	void BezierDash(const LongPoint& lpt0, const LongPoint& lpt1, const LongPoint& lpt2, const LongPoint& lpt3, short cap1=0, short cap2=0);
   void BezierDashCaps(const LongPoint& lpt0, const LongPoint& lpt1, const LongPoint& lpt2, const LongPoint& lpt3, short cap1 = 0, short cap2 = 0); // Infinisys Ltd. - GDI+
	void Bezier(const LongPoint& lpt0, const LongPoint& lpt1, const LongPoint& lpt2, const LongPoint& lpt3);
	void BezierTo(const LongPoint& lpt1, const LongPoint& lpt2, const LongPoint& lpt3);
	void DrawText(const CString& sText, const LongRect& lrc,UINT nFormat);
	void DrawAnnot(const LongPoint& lpt, int nID);
	void DrawStyledText(int iOpacity, const CStyledText& st, const LongPoint* lptCorners, bool bAutoAdjustBoundRect, CRgn* pClip=NULL, int nStartChar=-1, int nEndChar=-1);
	void DrawDimText(const CString &sValue,const CString &sTol1,const CString &sTol2, const LongPoint *lptCorners, const LOGFONT& lf);
	void DrawHandle(const LongPoint& lpt,bool bHalftone);
	void DrawDimMarker(const LongPoint& lpt);
	void StretchBlt(const CRect& src, const LongRect& lrc, HBITMAP hDib, bool bUseFillModel=false);
	LongPoint GetTextExtent(const CString& sText,LOGFONT *lf=NULL,bool bTolerance=false);
	CSize GetTextExtent(LPCSTR sText,int nCount);
	BOOL GetTextMetrics(LPTEXTMETRIC lpTm);
	bool SelectFont(const LOGFONT& lf,Angle rotAngle=0,bool bTolerance=false);
	void UnselectFont();
	int CalcLineExtent(const CStyledText & st, int pos, double & lineAscent, double & baseAscent, double & baseDescent, double & width, bool bRelaxedBox, bool bAutoAdjustBoundRect, int nWidth);
	void CalcBoundRect(const CStyledText & st, const TextParams & tp, CRect & rcBound, bool bRelaxedBox, bool bAutoAdjustBoundRect = true, int nWidth = 0);
	bool RectVisible(const LongRect& lrc);
   bool RectActiveClient(const LongRect& lrc);
	void SetDimStandard(const DimStandard* pDimStd) { m_pDimStd=pDimStd; }
	const DimStandard* GetDimStandard() { return m_pDimStd; }

	bool BeginPath();
	bool EndPath(CRgn* pRgn);
	bool IsCreatingPath() { return m_bCreatingPath; }
	bool IsOutline() { return m_bOutline; }
   //bool IsPenPatternFillEx();
   void PaintPath();
	//void PaintRgn(CRgn* rgn);
   void PaintPenPath();// Infinisys Ltd. GDI+
  // void PaintPenRgn(CRgn *pRgn);
//   void PaintPenRgn(Gdiplus::Region *pRgn); // Bug fixes - Interface Update - Infinisys Ltd.

	void ResetJoint();
	void OpenJoint(const LongPoint& lpt,pointd* pt);
	void CloseJoint(const LongPoint& lpt, pointd* pt=NULL);

   // Infinisys Ltd - GDI+ CRgn -> Gdiplus::GraphicsPath
	void DrawLineDim(const LineDimPoints& ldp,short cap1=0,short cap2=0, Gdiplus::GraphicsPath* pClip=NULL); 
	void DrawAngDim(const AngDimPoints& adp,short cap1=0,short cap2=0, Gdiplus::GraphicsPath* pClip=NULL);
	void DrawRadialDim(const RadialDimPoints& rdp, short cap1=0, short cap2=0, Gdiplus::GraphicsPath* pClip=NULL);

	void DrawLine(const LongPoint &lpt1,const LongPoint &lpt2);
	CMapper* GetMapper();
	CDC* GetCDC() { return m_pDC; }

	LongPoint& GetCurrentPos() { return m_lptCurrent; }
	void SetCurrentPos(const LongPoint& lpt) { m_lptCurrent=lpt; }
   Gdiplus::GraphicsPath * path() { return m_ppath; }
   Gdiplus::Font * font() { return m_pfont.get(); }
   Gdiplus::Graphics * graphics() { return m_pgraphics; }


   void graphics_on();
   void graphics_off();
   void graphics_high();
   void graphics_joint();
   
   void path_new(bool bAccumul, Gdiplus::FillMode fillMode = Gdiplus::FillModeAlternate);
   bool path_destroy(bool bCloseAccumul);
   Gdiplus::GraphicsPath * path_detach();

protected:
	void SaveClip(CRgn& oldClip);
	bool canUsePath();
   void AdjustOutside(LongRect& lrc, enum_orientation eorientation); // Infinisys Ltd. 2021
   void AdjustInside(LongRect& lrc, enum_orientation eorientation); // Infinisys Ltd. 2021
	void AdjustBorder(LongRect& lrc,LongPoint* lptCorner=NULL);
   void AdjustBounds(LongRect& lrc,LongPoint* lptCorner=NULL); // Pattern/Gradient - Infinisys Ltd
	void DrawLineCaps(const LongPoint &lpt,short cap1=0,short cap2=0,short border=0);
	void DrawArc(const LongPoint &lptCenter,const LongPoint &lptRadii,const LongPoint &lptFrom, const LongPoint &lptTo,int nDirection, Angle rotAngle,short cap1,short cap2,bool bAdjustAlign, short nDimEndmarkOut=0,bool bForceGDI=false,
   bool bClose = false);  // Infinisys Ltd. 2021
   //void PatternFill(CRgn *pRgn,Gdiplus::Brush * pbrush); // GDI+ Opacity/Transparency - Infinisys Ltd
   //void PatternFill(Gdiplus::Brush * pbrush); // GDI+ Opacity/Transparency - Infinisys Ltd
   //void BrushFill(CRgn *pRgn,Gdiplus::Brush* pBrush,int iOpacity); // GDI+ Opacity/Transparency - Infinisys Ltd

   void BrushFill(Gdiplus::Brush* pBrush); // GDI+ Opacity/Transparency - Infinisys Ltd

   //void PatternFill(Gdiplus::Region *pRgn,CBitmap* pPattern,int iPackedFill,int iOpacity); // Opacity/Transparency - Infinisys Ltd
   void BrushFill(Gdiplus::Region *pRgn,Gdiplus::Brush* pBrush,int iOpacity); // Opacity/Transparency - Infinisys Ltd

   Gdiplus::Graphics * m_pgraphics;

	PenModel m_PenModel;
	//CBitmap m_PenPattern;
   ::std::unique_ptr < Gdiplus::Bitmap > m_pbitmapPenPattern;
	FillModel m_FillModel;
	//CBitmap m_FillPattern;
   ::std::unique_ptr < Gdiplus::Bitmap > m_pbitmapFillPattern;
	TextParams m_TextParams;
	CDC* m_pDC;
	CMapper* m_pMapper;
   public:
	CDrawPrim	m_DrawPrim;
   protected:
	//CPen m_Pen;
	//CBrush m_PenBrush;
	//CBrush m_Brush;
   int m_iPackedItemPenFill; // Pattern/Gradient - Infinisys Ltd
   int m_iPackedItemFill; // Pattern/Gradient - Infinisys Ltd
	CPen* m_oldPen;
	CFont m_Font;
	LongPoint m_lptCurrent;
	CFont* m_pOldFont;
	COLORREF m_TextColor;
   int m_iPathAccumul;
   Gdiplus::GraphicsPath * m_ppath;
   Gdiplus::GraphicsPath * m_ppathAccumul;
   ::std::unique_ptr < Gdiplus::Font >  m_pfont;
   ::std::unique_ptr < CFont >  m_pgdifont;
   int m_iPathCount;

   friend class ::UsePath;
   public:
   ::std::unique_ptr < Gdiplus::Pen > m_PenPlus;
   ::std::unique_ptr < Gdiplus::Brush > m_PenBrushPlus;
   ::std::unique_ptr < Gdiplus::Brush > m_BrushPlus;
   Gdiplus::Color m_colorPen;
   CPCDraftDoc * m_pDoc;
   protected:
   CVersionDoc * m_pVersionDoc;
	const DimStandard* m_pDimStd;
	bool m_bCreatingPath;
	bool m_bOutline;

	bool m_bGreyMode;

	pointd m_ptJoint[6];
	LongPoint m_lptJoint[2];
	bool m_bJoint;
};


// GDI+
class UsePath
{
public:

   CDraftContext * m_pdc;

   UsePath(CDraftContext * pdc)
   {
      m_pdc = pdc;

      if (m_pdc->m_iPathCount == 0)
      {

         m_pdc->m_ppath = new Gdiplus::GraphicsPath();
         m_pdc->m_bCreatingPath = true;

      }
      m_pdc->m_iPathCount++;
   }

   ~UsePath()
   {

      m_pdc->m_iPathCount--;
      if (m_pdc->m_iPathCount == 0)
      {
         delete m_pdc->m_ppath;
         m_pdc->m_ppath = nullptr;
         m_pdc->m_bCreatingPath = false;

      }

   }


};


#include "MDShape.h"


// GDI+
class UseRect
{
public:

   
   CDraftContext *      m_pdc;
   LongRect             m_rect;
   LongRect *           m_prectOld;

   UseRect(CDraftContext * pdc, const LongRect & lr)
   {

      m_pdc = pdc;

      m_rect = lr;

      m_prectOld = m_pdc->m_prect;

      m_pdc->m_prect = &m_rect;

   }

   UseRect(CDraftContext * pdc, CMDShape * pshape)
   {
      
      m_pdc = pdc;

      m_rect = pshape->GetExtents();

      m_prectOld = m_pdc->m_prect;
      
      m_pdc->m_prect = &m_rect;

   }


   ~UseRect()
   {

      m_pdc->m_prect = m_prectOld;

   }


};


#endif // !defined(AFX_DRAFTCONTEXT_H__09EB3C85_D706_11D2_B590_EA06B8112A7F__INCLUDED_)
