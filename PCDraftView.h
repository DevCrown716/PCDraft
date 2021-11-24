// PCDraftView.h : interface of the CPCDraftView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCDRAFTVIEW_H__AAE7FC31_44F6_11D2_9842_000000000000__INCLUDED_)
#define AFX_PCDRAFTVIEW_H__AAE7FC31_44F6_11D2_9842_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "lpoint.h"
#include "pointf.h" // Infinisys Ltd. GDI+
#include "pointd.h" // Infinisys Ltd. GDI+
#include "DSShape.h"
#include "MDShape.h"
#include "DocMapper.h"
#include "FillWnd.h"
#include "LineStyleWnd.h"
#include "InplaceEdit.h"
#include "MistakeCounter.h"	// Added by ClassView
#include "PackedDIB.h"
#include "TransparencyBar.h"
#include "LibraryDragDrop.h" // Library - PCDraft 6 - Infinisys Ltd..
#include "imm_client.h" // Infinisys Ltd. - Inline Japanese Support
#include "Welcome/StaticEx.h"  // Infinisys Ltd. 2021 - Unicode File names


struct Snap
{
	LongRatio	fSnapRatio;
	LongPoint	fSnapResidue;
};

enum SnapMode
{
	kNoSnap,kSnapObject,kSnap
};

enum UnselectShapeFlags
{
	usLocked=1,
	usDimStr=2
};

class CMDShape;
class CMDText;
class IShapeSelectedNotify;
struct CRotateContext;

class CTrimContext;
class CChamferContext;
class CFilletContext;
class CAttachContext;
class CAddContext;
class CSubContext;
class CLineDimSketcher;
class CAttribSettingsWnd;

class CPCDraftView: public imm_client
{
protected: // create from serialization only
	CPCDraftView();
	DECLARE_DYNCREATE(CPCDraftView)

// Attributes
public:

   std::unique_ptr<Gdiplus::Bitmap> m_pbitmapDraw; // Infinisys Ltd. 2021

	enum { kPrintDocument,kPrintLayer,kPrintSlides };
	UINT m_nCurrentTool;
	UINT m_nOneTimeTool;
	int m_nPrintOption;
	bool m_bFitToPage;
	bool m_bLayer1AsMaster;

	int m_nRegPolySides;
	short m_nExtendTo;

   CPoint m_pointZoom; // Infinisys Ltd. 2021 - Context Menu - Zoom in/Zoom out

   int m_nStarSides; // Infinisys Ltd
   int m_iStarThickness; // Infinisys Ltd

   std::unique_ptr<Gdiplus::Bitmap> m_pbitmapDrawBackground; // Fast Text Editing - Infinisys Ltd.
   std::unique_ptr<Gdiplus::Bitmap> m_pbitmapDraw1; // Fast Text Editing - Infinisys Ltd.

	UINT m_nPrompt;
	UINT m_nSavedPrompt;
	LongPoint m_lptCursorPos;
	CPoint m_ptCursorPos;
	CPoint m_ptCrossCursorPos;

	bool m_bResizeRel;
	CMDShape* m_pResizeShape;
	ResizeValue m_ResizeValue[8];
	int m_nResizeShapeHandle;
   CMemFile m_ime;

   int m_iEditPasteCounter;

	int m_nDimType;


   CTime       m_timeLastAutoSave; // PC Draft 7 - Auto Save 
   bool        m_bAutoSave; // PC Draft 7 - Auto Save 
   bool        m_bAutoSaving; // PC Draft 7 - Auto Save 
   int         m_iAutoSaveMinutes; // PC Draft 7 - Auto Save 

	CDocMapper m_Mapper;
	CAttachContext* m_pAttachCtx;
	CAddContext* m_pAddCtx;
	CSubContext* m_pSubCtx;
	CTrimContext* m_pTrimCtx;
	CChamferContext* m_pChamferCtx;
	CFilletContext* m_pFilletCtx;

   // Opacity/Translucency - Infinisys Ltd
   CTransparencyBar m_wndTransparency;

   CLibraryDragDrop m_dragdrop; // Library - PCDraft 6 - Infinisys Ltd.

   class CUndoItem
   {
   public:
      int            m_iId;
      CMemFile *     m_pmemfile;

      CUndoItem(int iId,CMemFile * pfile):m_pmemfile(pfile),m_iId(iId) {}
      ~CUndoItem() { delete m_pmemfile; }
   };

   CArray < CUndoItem * >  m_undoitemptra;
   int                     m_iUndo;

// Operations
public:
	CPCDraftDoc* GetDocument() const;
	CPoint GetDeviceScrollPosition(bool bCentered = true) const;
	void SetScrollSizes(SIZE sizeTotal);
	void DoNetSelect(const CPoint& pt, bool bOptionPressed, bool bAdjustCursor);
	void DoPlace(HANDLE hShape, bool bUnscaled);

   using CView::ScreenToClient;
   inline void ScreenToClient(pointd & p) const
   {
      point p1(0, 0);
      ScreenToClient(&p1);
      p.x += (float) p1.x;
      p.y += (float) p1.y;
   }
   inline void ScreenToClient(pointd * p) const { ScreenToClient(*p); }
   inline void ScreenToClient(Gdiplus::RectF &rect) const
   {
      pointd p(0.f, 0.f);
      ScreenToClient(p);
      rect.X = (Gdiplus::REAL)(rect.X +p.x);
      rect.Y = (Gdiplus::REAL)(rect.Y +p.y);
   }
   using CView::ClientToScreen;
   inline void ClientToScreen(pointd & p) const
   {
      point p1(0, 0);
      ClientToScreen(&p1);
      p.x = (float)(p.x + p1.x);
      p.y = (float)(p.y + p1.y);
   }
   inline void ClientToScreen(pointd * p) const { ClientToScreen(*p); }
   inline void ClientToScreen(Gdiplus::RectF &rect) const
   {
      point p1(0, 0);
      ClientToScreen(&p1);
      rect.X += p1.x;
      rect.Y += p1.y;
   }
   void ActivateTextShape(CMDText* pTextShape);
	void DeactivateTextShape();
   void OnChangeSelection();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCDraftView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
   virtual void on_edit_undo() override; // Infinisys Ltd. - Japanese Inline Text
   virtual void on_text_composition(CStringW wstr, enum_composition_state ecompositiontext) override; // Infinisys Ltd. - Japanese Inline Text
   virtual void on_text_composition_done() override;// Infinisys Ltd. - Japanese Inline Text
   virtual void get_text_composition_area(LPRECT lprect) override;// Infinisys Ltd. - Japanese Inline Text
	void CommonChar();
	CLinkVertexList& GetActiveLinkVertexList();
	bool IsSelectedShape(CMDShape* pShape,bool bInsideGroups=false);
	bool CanMoveDimension(CMDShape* pShape);
	void UpdateZoomInfo();
	void DoChamfer();
	BOOL CanReshapeUnsmooth();
	BOOL CanReshapeSmooth();
	void UpdateCursor();
	void UnlockSelection();
	bool IsLinkedShape(const CMDShape* pShape, int nLinkVertex=-1, CMDShape** pLinkedShape=NULL);
	void GetSortedSelection(CMDShapeList& sortedList,bool bRemoveLocked=true);
	void LinkCenterVertexes(CMDShape* pShape,CMDShape* pDimShape);
	void LinkLineVertexes(CMDShape* pDimShape,int nDimVertex,CEdge* pEdge, bool bEdgeSwap);
	void LinkVertexes(const CPoint& pt,bool bLink=true, bool bWarn=false);
	bool SelectLayerFromPoint(const CPoint& pt);
	void DoPan();
	bool m_bInitialized;
	void DropShapes(HANDLE hDrop, CPoint *pt);
	CMistakeCounter m_Mistakes;
	void SnapValue(Coord& value,const LongRatio& snapRatio);
	bool PtInDocument(const LongPoint& lpt);
	bool SnapPtToDocument(LongPoint &lpt);
	void SetArrowTool();
	void ForceResizeUpdate();
	void DoNudge(int dx,int dy);
	virtual ~CPCDraftView();

	void RestoreState(CMemFile* pFile);
	void SaveState(CMemFile* pFile);
	void SaveUndo(int sID,CMemFile* pUndo=NULL);
	void BeginSaveUndo();
	void CommitSaveUndo(int sID);
	void AbortSaveUndo(bool bRestore=true);

   void DoZoomIn(CPoint pt, bool bSingleClick = false);
   void DoZoomOut(CPoint pt, bool bSingleClick = false);


   void ExportImage();

	void FinishCreateFillet(int nFillet);
	void ResetDupDelta();
	void CancelCurrentTool();
	void SetOneTimeTool(UINT nID, UINT nPrompt=0,int nSnapMode=kNoSnap);
	void SetShowSize(int nIndex,int nType,WrFixed value);
	void ClearShowSize();
	void DoEditClear();
	void DoEditUndo();
   void DoEditRedo(); // Infinisys Ltd
	bool DoShapeRotate(Angle nAngle, short nAbout, const LongPoint& ptOrigin);
	void RotateFeedback(CDraftContext* pDC, CRotateContext* pTrackContext);
	void RestoreSavedView(int nView);
	void ApplyFill(const FillModel& fm,bool bParallel);
	bool ApplyPen(const PenModel &pm, int nWhat);
	void ApplyText(const TextParams& tp, int nWhat);
   void ApplyOpacity(int iOpacity,bool bParallel, bool bDoSaveUndo); // Opacity/Transparency - Infinisys Ltd
	void GetActiveTextParams(TextParams& tp, int nWhat);
	bool HaveTextParamSelection(int nWhat);
	void DoShapeSelect(IShapeSelectedNotify* pTarget);
	void DoAlign(LongPoint *refLpt);
	void UnselectShape(CMDShape* pShape,bool bAttached=true);
	void SetSnap(int index);
	void SetSnapMode(int nSnapMode) { m_nSnapMode=nSnapMode; }
	void UnselectShapes(int nFlags);
	void FlipSelection(bool bVertical);
	void CreateShapesFromHandle(HANDLE hClip,bool bUnscaled=false,CPoint* ptAt=NULL,bool bOffset=false);
	HANDLE SelectionToHandle(bool bByExtent=false);
   // bOffset Parameter - Infinisys Ltd. 2021
	void DoEditPaste(bool bUnscaled=false,CPoint* ptAt=NULL, bool bOffset = false);
	BOOL DoEditCopy();
	bool DoShapesMove(CPoint pt, bool bSingle=false);
	bool DoShapesDrag(CPoint pt, bool bSingle);
	bool SketcherScroll(CPoint pt, BOOL processUpdates = TRUE);
	void EnsureVisible(CMDShape* pShape);
	void SelectNone();
	bool SelectShape(CMDShape* pShape, bool bAddToList, bool bFast=false);
	bool SelectShapesInRect(const LongRect& lrc, bool bInclusive, bool bAddToList);
	void DrawSelection(CDC* pDC);
	void InvalidateShape(CMDShape* pShape, bool bSelected);
	bool SelectShapeFromPoint(CPoint inPoint, bool bAddToList, CMDShape* pStart=NULL);

   // Opacity/Transparency - Infinisys Ltd
   void ShowTransparencyBar(CEdit * pedit,CAttribSettingsWnd * pwnd,e_attrib eattrib);

   void InvalidateSelection();

	void SnapLPt(LongPoint& lpt,bool bSnapToPoint=false);
	void SnapToGrid(LongPoint& lpt) const;
	bool SnapToObject(LongPoint& lpt) const;
   bool SnapToGuides(LongPoint& lpt, int * piGuide =nullptr) const; // Infinisys Ltd. PC Draft 7 Guides
   bool SnapSelectionToGuide(LongPoint& lpt, LongPoint& lptShift, int * piGuide = nullptr); // Infinisys Ltd. PC Draft 7 Guides
	bool SnapToPoint(LongPoint& lpt,int nType) const;
	void SnapLength(Coord& length) const;
	void SnapLPtToLength(const LongPoint& lptFrom, LongPoint& lptTo) const;

	bool NeedMouseTrack();
	void CancelMouseTracking(bool bInSketcher);
	void AdjustCursor(CPoint & pt,bool bInSketcher);

	HCURSOR GetCursor(bool bUseModifiers=true);
	void SetCursorPrompt(UINT nPrompt);
	void ShowCursorPrompt(bool bShow=true);
	void ShowCursorPromptNoLock(bool bShow=true);

/*	void HideCursorPrompt();
	void ShowCursorPrompt(pointd point);
	void AdjustCursorPrompt(pointd point);
*/
	bool NeedCrossCursor() const;
	void ShowCrossCursor(bool bShow=true,bool bSwitch=true);
	void MoveCrossCursor(const CPoint& pt);
	void DrawCrossCursor(CDC* pDC);

   int dpi(int iPixel);

   inline bool IsEditActive() const { return m_bEditActive; } // Infinisys Ltd. - Japanese Input

	void MoveRulersCursor(const CPoint& pt);
	void DrawRulersCursor(CDC* pDC);

	void DoApplyAttributes(CMDShape* pShape,ShapeAttributes& attrs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
   void GetSelectionExtents(LPRECT lpcrect); // Library - PCDraft 6 - Infinisys Ltd.
	void GetClientCorner(CPoint& corner);
	void GetClientSize(CSize& size);
   void GetViewRect(CRect & rect); // Infinisys Ltd. - GDI+
	void GetActiveClientRect(CRect& rc);
	CMDShapeList& GetSelection() { return m_Selection; }

	HBITMAP SelectionToBitmap();
	HENHMETAFILE SelectionToMetaFile();

   virtual bool is_text_mode() const; // Infinisys Ltd.

	void DoMoveObject(LongPoint lpt,BOOL bMoveCopy,int nToLayer);
	bool DoCalcMovePt(LongPoint& lpt);
	bool RecalcLinkedVertex(CMDShape* pShape);
	bool RecalcLinkedVertexes(bool bMove=true);
	void ScaleSelection(double nX,double nY,bool bContract);

	void SetTrackingShape(bool bState) { m_bTrackingShape=bState; }

protected:
	IShapeSelectedNotify* m_pShapeSelectedNotifyTarget;
	CScrollBar	m_wndVertBar;
	CScrollBar	m_wndHorzBar;
	CFillWnd m_wndFillInfo;
	CLineStyleWnd m_wndLineInfo;
	//CStatic m_wndPosition;
   StaticEx m_wndPosition;
	CStatic m_wndZoomInfo;
//	CScrollBar m_wndSizeBar;
	CStatic m_wndSizeBar;

	bool m_bEditActive;
	CInplaceEdit m_InplaceEdit;

	CSize	m_totalDev;
	CSize	m_lineDev;
	CSize	m_pageDev;
	CBitmap	m_bmpGridHorz;
	CBitmap	m_bmpGridVert;
	CBitmap	m_bmpGridHorzLarge;
	CBitmap	m_bmpPageHorz;
	CBitmap	m_bmpPageVert;

	CMDShapeList m_Selection;

   CPoint m_ptPaste;

	bool m_bCursorPrompt;
	short m_nPromptVisible;
   int m_iCursorCount;
	CImageList m_Prompt;
	short m_nCrossVisible;

	CMDText* m_pActiveTextShape;

	Snap m_Snap;

   // Infinisys Ltd
	//int	m_nUndoID;
   //CMemFile* m_pUndo;
	//CMemFile* m_pRedo;
	//bool m_bRedo;
	CMemFile* m_pTemp;

	CString m_sShowSize[4];

	bool m_bAdjustDupDelta;
	LongPoint m_lptDupDelta;

	CLineDimSketcher* m_pLineDimSketcher;

	HANDLE m_hPlaceShape;
	bool m_bPlaceUnscaled;

	bool m_bTrackingShape;

	// by TSM
	//int used[2048];
	//int usedCount;
	//CPackedDIB pat_entry;
	//void ProcessShapesForCopy(CPCDraftDoc* pDoc, CMDShape* pShape);
	//void ProcessShapesAfterCopy(CPCDraftDoc* pDoc, CMDShape* pShape);
	// end by TSM

protected:
	void CreateFillModelBitmap(const FillModel& fm,CBitmap& bmp, CSize size);
	bool InRulers(CPoint point);
	void DrawPageBreaks(CDC* pDC, CPoint ptOrg);
	void DrawGridLines(CDC* pDC, CPoint ptOrg);
   void DrawGuides(CDC* pDC, CPoint ptOrg); // Infinisys Ltd. 2020 Guides
   void DrawRulers(CDC* pDC, CRgn* pClip);
	void DrawLayerBar(CDC* pDC);
	void DrawDocument(CDC *pDC);
   void FullDrawSelection(CDC *pDC, bool bTransparentBackground); // Library - PCDraft 6 - Infinisys Ltd.
	void ScrollToDevicePosition(POINT ptDev); // explicit scrolling no checking
	void UpdateBars();
	void GetScrollBarState(CSize sizeClient,CSize& needSb,CSize& sizeRange, CPoint& ptMove);
	void ScrollWindow(int dx,int dy);
	void CalcRulerInfo(int& nRes,WrFixed& nMarkStep,int &nNumMarks,int nMarkLen[]);
	void CalcDocumentDevSize();
	void ConstructGridLines();
	void ConstructPageBreaks();
	bool LayerBarDoClick(int nIndex,CRect &rc,CSize sz,CPoint ptOrigin);
	void InvalidateLayerBar();
   void DoAttribFillColorPatternGradient(bool bParallel); // Interface Update - Infinisys Ltd.
	void DoAttribFillColor(bool bParallel);
	void DoAttribFillPattern(bool bParallel);
	void DoDistribute(bool bByDatum,bool bVert);

// Generated message map functions
protected:
   void ZoomCenter(double fZoomFactor, int nZoomFactor = 0);
#ifdef _FLOAT_ZOOM
	void SmartZoom(double fZoomFactor,int nZoomFactor);
	void SmartZoomIn(double fZoomFactor,int nZoomFactor);
#else
	void SmartZoom(int nZoomFactor);
	void SmartZoomIn(int nZoomFactor);
#endif
	bool HaveUnlockedSelection();
	void OffsetSelection(LongPoint delta, bool bSingle);
	void TrackSelectionWireFrame(CDC* pDC, LongPoint delta, bool bExtentFeedback, bool bSingle);
	void UpdateResizeValue(UINT nID);
	const PenModel& CPCDraftView::GetActivePenModelByLineWidth();

   void DuplicateSelection(bool bWithOffset); // Infinisys Ltd. 2021 - Contextual Menus


   CLibraryItem * LibraryItemFromSelection(); // Library - PCDraft 6 - Infinisys Ltd..
   public:
      Gdiplus::Bitmap * BitmapFromSelection(CRect & rectSelection, double dAlpha = 1.0, int iMargin = 2);// Fix Drag Slow Wire Frame - Infinisys Ltd.
      Gdiplus::Bitmap * BitmapFromView();// Fix Drag Slow Wire Frame - Infinisys Ltd.
      Gdiplus::Bitmap * BitmapFromDrawing();// Infinisys Ltd. 2021
      bool BitmapFromSelection(CRect & rectSelection, CBitmap & bmp, int iMargin = 2);// Fix Drag Slow Wire Frame - Infinisys Ltd.
   bool BitmapFromView(CBitmap & bmp);// Fix Drag Slow Wire Frame - Infinisys Ltd.
   bool BitmapFromDrawing(CBitmap & bmp);// Infinisys Ltd. 2021
   bool DrawSelection(const CRect & rectSelection, Gdiplus::Graphics & g,int x,int y,int iMargin = 2);// MD70 (Big Selection Bug) - Infinisys Ltd.
   protected:

//	afx_msg void OnRequestResizeTextShape(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void OnKillFocusResizePositionX();
	afx_msg void OnKillFocusResizePositionY();
	afx_msg void OnKillFocusResizeValue1();
	afx_msg void OnKillFocusResizeValue2();
	afx_msg void OnKillFocusResizeValue3();
	afx_msg void OnKillFocusResizeValue4();
	afx_msg void OnKillFocusResizeValue5();
	afx_msg void OnKillFocusResizeValue6();
	afx_msg void OnSelChangeDimType();
	afx_msg void OnClickPosition();
	afx_msg void OnClickZoom();
	afx_msg void OnAlign(UINT nID);
	//{{AFX_MSG(CPCDraftView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditClear();
	afx_msg void OnEditSelectNone();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditMove();
	afx_msg void OnEditDupCircular();
	afx_msg void OnEditDupLinear();
   afx_msg void OnViewZoomIn(); // Infinisys Ltd. 2021 - Context Menus
   afx_msg void OnViewZoomOut(); // Infinisys Ltd. 2021 - Context Menus
   afx_msg void OnUpdateViewZoomIn(CCmdUI* pCmdUI); // Infinisys Ltd. 2021 - Context Menus
   afx_msg void OnUpdateViewZoomOut(CCmdUI* pCmdUI); // Infinisys Ltd. 2021 - Context Menus
	afx_msg void OnDataObjectInfo();
	afx_msg void OnToolDimFormat();
	afx_msg void OnUpdateHaveShapes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHaveUnlockedSelection(CCmdUI* pCmdUI);
	afx_msg void OnTextFont();
	afx_msg void OnArrangeRotateOpt();
	afx_msg void OnArrangeAlignOpt();
	afx_msg void OnArrangeRotate();
	afx_msg void OnToolMirror();
	afx_msg void OnArrangeBackward();
	afx_msg void OnArrangeForward();
	afx_msg void OnArrangeToBack();
	afx_msg void OnArrangeToFront();
	afx_msg void OnEditRepeatPaste();
	afx_msg void OnTextHidetext();
	afx_msg void OnUpdateTextHidetext(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryNewPalette();
   afx_msg void OnFileImportImage();
   afx_msg void OnUpdateFileImportImage(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
   afx_msg void OnEditDuplucate();
   afx_msg void OnEditDuplicateWithoutOffset(); // Infinisys Ltd. 2021 - Contextual Menus
	afx_msg void OnArrangeUnlock();
	afx_msg void OnArrangeLock();
	afx_msg void OnUpdateArrangeLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateArrangeUnlock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolAttribPen(CCmdUI* pCmdUI);
   afx_msg void OnToolAttribPenMenu(); // Interface Update - Infinisys Ltd.
	afx_msg void OnToolAttribPenColor();
	afx_msg void OnToolAttribPenPattern();
   afx_msg void OnToolAttribPenTransparency(); // Infinisys Ltd
   afx_msg void OnUpdateToolAttribPenTransparency(CCmdUI* pCmdUI); // Infinisys Ltd
   afx_msg void OnToolAttribPenAlpha(); // Infinisys Ltd
   afx_msg void OnUpdateToolAttribPenAlpha(CCmdUI* pCmdUI); // Infinisys Ltd
   afx_msg void OnUpdateToolAttribFill(CCmdUI* pCmdUI);
   afx_msg void OnToolAttribFillMenu(); // Interface Update - Infinisys Ltd.
	afx_msg void OnToolAttribFillColor();
	afx_msg void OnToolAttribFillPattern();
   afx_msg void OnToolAttribFillTransparency();  // Infinisys Ltd
   afx_msg void OnUpdateToolAttribFillTransparency(CCmdUI* pCmdUI); // Infinisys Ltd
   afx_msg void OnToolAttribFillAlpha();  // Infinisys Ltd
   afx_msg void OnUpdateToolAttribFillAlpha(CCmdUI* pCmdUI); // Infinisys Ltd
   afx_msg void OnUpdateToolAttribPFill(CCmdUI* pCmdUI);
   afx_msg void OnToolAttribPFillMenu(); // Interface Update - Infinisys Ltd.
	afx_msg void OnToolAttribPFillColor();
	afx_msg void OnToolAttribPFillPattern();
   afx_msg void OnToolAttribPFillTransparency();  // Infinisys Ltd
   afx_msg void OnUpdateToolAttribPFillTransparency(CCmdUI* pCmdUI); // Infinisys Ltd
   afx_msg void OnToolAttribPFillAlpha();  // Infinisys Ltd
   afx_msg void OnUpdateToolAttribPFillAlpha(CCmdUI* pCmdUI); // Infinisys Ltd
   afx_msg void OnUpdateToolAttribLineWeigth(CCmdUI* pCmdUI);
	afx_msg void OnToolAttribLineWeigth();
	afx_msg void OnUpdateToolAttribLineStyle(CCmdUI* pCmdUI);
	afx_msg void OnToolAttribLineStyle();
	afx_msg void OnUpdateToolAttribEndMark(CCmdUI* pCmdUI);
	afx_msg void OnToolAttribEndMark();
	afx_msg void OnEditDatum();
	afx_msg void OnArrangeFlipHoriz();
	afx_msg void OnArrangeFlipVert();
	afx_msg void OnArrangeGroup();
	afx_msg void OnUpdateArrangeUngroup(CCmdUI* pCmdUI);
	afx_msg void OnArrangeUngroup();
	afx_msg void OnUpdateArrangeGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolExtend(CCmdUI* pCmdUI);
	afx_msg void OnToolExtend();
	afx_msg void OnToolTrim();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateEditReshapeSmooth(CCmdUI* pCmdUI);
	afx_msg void OnEditReshapeSmooth();
	afx_msg void OnUpdateEditReshapeUnsmooth(CCmdUI* pCmdUI);
	afx_msg void OnEditReshapeUnsmooth();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnToolFillet();
	afx_msg void OnToolChamfer();
	afx_msg void OnUpdateTextSelectalltext(CCmdUI* pCmdUI);
	afx_msg void OnTextSelectalltext();
	afx_msg void OnToolRotate90();
	afx_msg void OnArrangeRotateZero();
	afx_msg void OnDataFieldNames();
	afx_msg void OnEditCopyAttrib();
	afx_msg void OnEditSelectSpecial();
	afx_msg void OnFileLoadLayer();
	afx_msg void OnLayoutDrawingSize();
	afx_msg void OnLayoutPosition();
	afx_msg void OnLayoutSetScaleUnits();
	afx_msg void OnTool1_1();
	afx_msg void OnUpdateToolCombine(CCmdUI* pCmdUI);
	afx_msg void OnToolCombine();
	afx_msg void OnUpdateToolGlue(CCmdUI* pCmdUI);
	afx_msg void OnToolGlue();
	afx_msg void OnViewRestorePrevZoom();
	afx_msg void OnViewSaveView();
	afx_msg void OnFileSetDefaults();
	afx_msg void OnEditAttribOptions();
	afx_msg void OnFilePrintSlides();
	afx_msg void OnViewSlideShow();
	afx_msg void OnViewHomeView();
	afx_msg void OnUpdateViewZoomIn2x(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomIn2x();
	afx_msg void OnUpdateViewZoomIn4x(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomIn4x();
   afx_msg void OnUpdateViewZoomIn8x(CCmdUI* pCmdUI);
   afx_msg void OnViewZoomIn8x();
   afx_msg void OnUpdateViewZoomIn16x(CCmdUI* pCmdUI);
   afx_msg void OnViewZoomIn16x();
   afx_msg void OnUpdateViewZoomIn32x(CCmdUI* pCmdUI);
   afx_msg void OnViewZoomIn32x();
   afx_msg void OnUpdateViewZoomIn64x(CCmdUI* pCmdUI);
   afx_msg void OnViewZoomIn64x();
   afx_msg void OnUpdateViewZoomOut2x(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomOut2x();
	afx_msg void OnUpdateViewZoomOut4x(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomOut4x();
	afx_msg void OnUpdateLayoutShowRulers(CCmdUI* pCmdUI);
	afx_msg void OnLayoutShowRulers();
	afx_msg void OnUpdateLayoutStandardRulers(CCmdUI* pCmdUI);
	afx_msg void OnLayoutStandardRulers();
	afx_msg void OnUpdateLayoutScaleRulers(CCmdUI* pCmdUI);
	afx_msg void OnLayoutScaleRulers();
	afx_msg void OnUpdateLayoutHideGridLines(CCmdUI* pCmdUI);
	afx_msg void OnLayoutHideGridLines();
	afx_msg void OnUpdateLayoutHidePageBreaks(CCmdUI* pCmdUI);
	afx_msg void OnLayoutHidePageBreaks();
	afx_msg void OnUpdateLayoutSnapToObject(CCmdUI* pCmdUI);
	afx_msg void OnLayoutSnapToObject();
   afx_msg void OnUpdateLayoutSnapToGuides(CCmdUI* pCmdUI); 
   afx_msg void OnLayoutSnapToGuides();
   afx_msg void OnUpdateLayoutShowGuideLines(CCmdUI* pCmdUI);
   afx_msg void OnLayoutShowGuideLines();
   afx_msg void OnUpdatePrefHideFills(CCmdUI* pCmdUI);
	afx_msg void OnPrefHideFills();
	afx_msg void OnFilePrintActiveLayer();
	afx_msg void OnUpdateToolUnglue(CCmdUI* pCmdUI);
	afx_msg void OnToolUnglue();
	afx_msg void OnUpdateToolSubtract(CCmdUI* pCmdUI);
	afx_msg void OnToolSubtract();
	afx_msg void OnUpdateArrangeDistribute(CCmdUI* pCmdUI);
	afx_msg void OnArrangeDistribute();
	afx_msg void OnArrangeAlignAgain();
	afx_msg void OnUpdateArrangeAlignAgain(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnUpdateEditDatum(CCmdUI* pCmdUI);
	afx_msg void OnViewMoreViews();
	afx_msg void OnViewDeleteAllViews();
	afx_msg void OnUpdateViewCursorPos(CCmdUI* pCmdUI);
	afx_msg void OnViewCursorPos();
	afx_msg void OnUpdateLayoutPosition(CCmdUI* pCmdUI);
	afx_msg void OnEditApplyAttrib();
	afx_msg void OnUpdateViewShowArea(CCmdUI* pCmdUI);
	afx_msg void OnViewShowArea();
	afx_msg void OnUpdateViewHideArea(CCmdUI* pCmdUI);
	afx_msg void OnViewHideArea();
   afx_msg void OnUpdateViewHideShowArea(CCmdUI* pCmdUI);
   afx_msg void OnViewHideShowArea();
   afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
   afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
   afx_msg void OnEditRedo();
   afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateHaveSelection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopyCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteUnscaled();
	afx_msg void OnUpdateEditPasteUnscaled(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolDimType(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolDimEndmark(CCmdUI* pCmdUI);
	afx_msg void OnToolDimEndmark();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFillInfo();
	afx_msg void OnLineInfo();
	afx_msg void OnToolLink();
	afx_msg void OnToolUnlink();
	afx_msg void OnToolMultiunlink();
	afx_msg void OnPrefToolOptions();
	afx_msg void OnUpdatePrefToolOptions(CCmdUI* pCmdUI);
	afx_msg void OnEditContract();
	afx_msg void OnEditExpand();
	afx_msg void OnFileExportFile();
	afx_msg void OnUpdateFileExportFile(CCmdUI* pCmdUI);
	afx_msg void OnPrefPenstyleBlade();
	afx_msg void OnUpdatePrefPenstyleBlade(CCmdUI* pCmdUI);
	afx_msg void OnPrefPenstyleRound();
	afx_msg void OnUpdatePrefPenstyleRound(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolRotate(CCmdUI* pCmdUI);
	afx_msg void OnToolRotate();
	afx_msg void OnUpdateDisableIfEditDatum(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintSlides(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintActiveLayer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
   afx_msg BOOL DrawBkgnd(CDC* pDC);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnToolsRange(UINT nID);
	afx_msg void OnUpdateToolsRange(CCmdUI* pCmdUI);
	afx_msg void OnBorderPositionRange(UINT nID);
	afx_msg void OnUpdateBorderPositionRange(CCmdUI* pCmdUI);
	afx_msg void OnPenColorsRange(UINT nID);
	afx_msg void OnUpdatePenColorsRange(CCmdUI* pCmdUI);
	afx_msg void OnFillColorsRange(UINT nID);
	afx_msg void OnUpdateFillColorsRange(CCmdUI* pCmdUI);
	afx_msg void OnParaFillColorsRange(UINT nID);
	afx_msg void OnUpdateParaFillColorsRange(CCmdUI* pCmdUI);
	afx_msg void OnPenPatternsRange(UINT nID);
	afx_msg void OnUpdatePenPatternsRange(CCmdUI* pCmdUI);
	afx_msg void OnFillPatternsRange(UINT nID);
	afx_msg void OnUpdateFillPatternsRange(CCmdUI* pCmdUI);
   afx_msg void OnParaFillPatternsRange(UINT nID);
	afx_msg void OnUpdateParaFillPatternsRange(CCmdUI* pCmdUI);
	afx_msg void OnLineWidthRange(UINT nID);
	afx_msg void OnUpdateLineWidthRange(CCmdUI* pCmdUI);
	afx_msg void OnLineStyleRange(UINT nID);
	afx_msg void OnUpdateLineStyleRange(CCmdUI* pCmdUI);
	afx_msg void OnTextJustifyRange(UINT nID);
	afx_msg void OnUpdateTextJustifyRange(CCmdUI* pCmdUI);
	afx_msg void OnTextVAlignRange(UINT nID);
	afx_msg void OnUpdateTextVAlignRange(CCmdUI* pCmdUI);
	afx_msg void OnTextSpacingRange(UINT nID);
	afx_msg void OnUpdateTextSpacingRange(CCmdUI* pCmdUI);
	afx_msg void OnTextCaseRange(UINT nID);
	afx_msg void OnUpdateTextCaseRange(CCmdUI* pCmdUI);
	afx_msg void OnGridRange(UINT nID);
	afx_msg void OnUpdateGridRange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAngleSnapRange(CCmdUI* pCmdUI);
	afx_msg void OnAngleSnapRange(UINT nID);
	afx_msg void OnDeleteViewRange(UINT nID);
	afx_msg void OnSavedViewRange(UINT nID);
	afx_msg void OnSavedViewShortcutRange(UINT nID);
	afx_msg void OnUpdateShowSizeRange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateResizeRange(CCmdUI* pCmdUI);
	afx_msg void OnResizeRange(UINT nID);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnPrefCrossCursor();
	afx_msg void OnIdleUpdateCmdUI();
   afx_msg LRESULT OnDropLibraryItem(WPARAM wParam,LPARAM lParam); // Library - PCDraft 6 - Infinisys Ltd.

#ifndef _UNICODE
	afx_msg LRESULT OnImeChar(WPARAM wParam, LPARAM lParam);
#endif

	DECLARE_MESSAGE_MAP()
protected:
	int m_nSnapMode,m_nSavedSnapMode;
	CImageList m_ImageList;
	bool m_bMouseTracking;
public:
   afx_msg void OnDestroy(); // Library - PCDraft 6 - Infinisys Ltd.
   afx_msg void OnTimer(UINT_PTR nIDEvent); // PC Draft 7 - Auto Save
   afx_msg void OnGuidesAddGuideAtPosition();
   afx_msg void OnGuidesAddhorizontalguide();
   afx_msg void OnGuidesAddverticalguide();
};

#ifndef _DEBUG  // debug version in PCDraftView.cpp
inline CPCDraftDoc* CPCDraftView::GetDocument() const
   { return (CPCDraftDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCDRAFTVIEW_H__AAE7FC31_44F6_11D2_9842_000000000000__INCLUDED_)
