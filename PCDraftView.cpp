// PCDraftView.cpp : implementation of the CPCDraftView class
//

#include "stdafx.h"
#include "resource.h"
#include "mainfrm.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "Layer.h"
#include "OffScreenDC.h"
#include "DraftContext.h"
#include "CustomMenus.h"
#include "FactorMapper.h"

#include "AttachContext.h"
#include "AddContext.h"
#include "SubContext.h"
#include "TrimContext.h"
#include "ChamferContext.h"
#include "FilletContext.h"

#include "SlideShowFrame.h"
#include "SlideShowView.h"

#include "ArcEdge.h"

#include "AddGuideByPositionDlg.h"
#include "AttributeOptionsDlg.h"
#include "ChamferSetupDlg.h"
#include "DrawingSizeDlg.h"
#include "FieldNamesDlg.h"
#include "FilletSetupDlg.h"
#include "PositionDlg.h"
#include "ScaleUnitsDlg.h"
#include "LayerSetupDlg.h"
#include "AttributeOptionsDlg.h"
#include "RotateOptionsDlg.h"
#include "AlignmentOptionsDlg.h"
#include "MoveObjectDlg.h"
#include "LinearDupDlg.h"
#include "CircleDupDlg.h"
#include "ObjectInfoDlg.h"
#include "DimFormatDlg.h"
#include "EditDimFormatDlg.h"
#include "MirrorSetupDlg.h"
#include "TrimSetupDlg.h"
#include "ExtendLineDlg.h"
#include "ParallelLineDlg.h"
#include "PrintSlidesDlg.h"
#include "SelectSpecialDlg.h"
#include "SlideShowDlg.h"
#include "RegularPolyDlg.h"
#include "SelectViewDlg.h"
#include "SaveViewDlg.h"
#include "LoadLayerDlg.h"
#include "RenameDlg.h"
#include "SavedView.h"
#include "StarDialog.h" // Infinisys Ltd

#include "MDRectangle.h"
#include "MDText.h"
#include "MDGroupShape.h"
#include "MDCircle.h"
#include "MDLineShape.h"
#include "MDBitmap.h"
#include "MDCenterDim.h"
#include "MDRndRectangle.h"
#include "MDEllipse.h"
#include "MDArcShape.h"
#include "MDPolygonShape.h"
#include "MDBezierShape.h"
#include "MDCubicShape.h"
#include "MDParallelLine.h"
#include "MDParallelPolygon.h"
#include "MDLineDim.h"
#include "MDRadialDim.h"
#include "MDAngularDim.h"
#include "MDArea.h"

#include "GuideSketcher.h"
#include "LineSketcher.h"
#include "RectSketcher.h"
#include "RndRectSketcher.h"
#include "EllipticSketcher.h"
#include "CircleSketcher.h"
#include "ArcSketcher.h"
#include "FreehandSketcher.h"
#include "PolySketcher.h"
#include "BezierSketcher.h"
#include "CubicSketcher.h"
#include "RegPolySketcher.h"
#include "ParallelLineSketcher.h"
#include "ParallelPolySketcher.h"
#include "OffsetCircleSketcher.h"
#include "OffsetArcSketcher.h"
#include "RubberBandSketcher.h"
#include "RefLineSketcher.h"
#include "LineDimSketcher.h"
#include "RadialDimSketcher.h"
#include "AngularDimSketcher.h"
#include "ConstrLineSketcher.h"
#include "RotateSketcher.h"
#include "SpecialLineSketcher.h"
#include "TextSketcher.h"
#include "DistributeSketcher.h"
#include "ZoomSketcher.h"
#include "ZoomInSketcher.h"
#include "RefPointSketcher.h"
#include "TangentLineSketcher.h"
#include "OriginSketcher.h"
#include "ExtrudeSketcher.h"
#include "StarSketcher.h"

#include "DimUnit.h"
#include "AttachContext.h"
#include "IShapeSelectedNotify.h"

#include "InplaceEdit.h"
#include "tiff.h"

#include "MDPrintDlg.h"
#include "ContractExpandDlg.h"

#include "AttribSettingsWnd.h"
#include "LineWidthWnd.h"
#include "LineStyleWnd.h"

#include "LibraryObject.h"
#include "LibraryPhoto.h"

#include "LayerFrame.h"

#include "DpiAwareness.h"

#include "imm_context.h"

#define USE_CENTER_ZOOM 1

#include <gdiplus.h>
#if 1
#include "FreeImage.h"
#endif
#define AUTO_SAVE_DEBUG 0
CString utf8_to_acp(CString str); // Interface Update - Infinisys Ltd.
HBITMAP CreateDIB(int x,int y, COLORREF * pdata, int stride, COLORREF ** p = NULL); // Interface Update - Infinisys Ltd.
HBITMAP CreateDIB(Gdiplus::Bitmap * pbitmap, bool bSwap = true); // Interface Update - Infinisys Ltd.
HBITMAP dib32_from_freeimage(FIBITMAP *pfibitmap); // Interface Update - Infinisys Ltd.
void SavePNG(CString strPath, Gdiplus::Bitmap * pbitmap);
void SaveJPEG(CString strPath, Gdiplus::Bitmap * pbitmap, int iQuality = 100);
void SaveBMP(CString strPath, Gdiplus::Bitmap * pbitmap);  // Infinisys Ltd. 2021

#ifdef DEBUG
//#define DEBUG_OTHER_DASHES_BUT_NOT_THIS_ONE 
#endif

#define TIMER_ONE_SECOND 1000
// Import Image - Infinisys Ltd
CString get_folder_from_path(CString strPath)
{

   PathRemoveFileSpec(strPath.GetBuffer(0));

   strPath.ReleaseBuffer(-1);

   return strPath;

}

CString get_font_english_name(CString str);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VIEW_MARGIN 50

#define kHORI					1
#define kVERT					2

/* The resolution of horizontal and vertical grid lines is 28 in ZoomFactor 1, 	*/
/* so we will draw 4 points within 28 pixels.									*/

#define	kMatricSkip				7
#define kEngDecFeetSkip			43	/* for determining whether English Dec Feet */

#define	kOriginalEngDecSkip		9	/* for determining whether English Dec */
#define	kNewEngDecSkip			6	/* for 12 dotted points within 72 pixels. */

#define	kInchMrkLen			16		/* same as kRulerWid */
#define	kHalfMrkLen			8
#define kQuarterMrkLen		4
#define	kEighthMrkLen		2

#define	kNumTicksPerInch	8		/* ie, 8 divisions of 1 inch	*/
#define kZoomTicksPerInch	10		/* at -8x and lower				*/
#define kNumTicksPer2Cent	4		/* 2 divisions per centimeter	*/
#define	kTicksPerDecFoot	2		/* only half marks for dec feet	*/

#define	kHOffset1			2		/* off for right side of tick */
#define kHOffset2			-6		/* off for left side of tick */
#define kHOffsetV			0		/* vert offset for number */
#define kVOffset1			-1		/* off for top side of tick */
#define kVOffset2			7		/* off for bottom side of tick */
#define kVOffsetH			0		/* horiz offset for number */

#define	kEng121				4		/* scale index for 1:1	*/
#define	kEngNoInch			10		/* scale index for 1/2" = 1', no more need for inches */
#define	k10X				1
#define	k4X					2
#define	kDecNoInch			16		/* scale index for 1" = 10', no more need for inches */
#define	kMet221				5		/* scale index for 2:1	*/
#define	kMet125				9		/* scale index for 1:5	*/
#define	kMet1250			13		/* scale index for 1:50	*/

/////////////////////////////////////////////////////////////////////////////
// Align parameters
static AlignMode gAlignMode=alignMode_None;
static AlignMask gSelMask=alignMask_None;
static AlignMask gRefMask=alignMask_None;

static bool gRestoreAlignState=false;
static AlignMode gSavedAlignMode=alignMode_None;
static AlignMask gSavedSelMask=alignMask_None;
static AlignMask gSavedRefMask=alignMask_None;


// Import Image - Infinisys Ltd
LPWSTR ConvertString(const CString instr)
{
   // Assumes std::string is encoded in the current Windows ANSI codepage
   int bufferlen = ::MultiByteToWideChar(CP_ACP,0,instr,instr.GetLength(),NULL,0);

   if(bufferlen == 0)
   {
      // Something went wrong. Perhaps, check GetLastError() and log.
      return 0;
   }

   // Allocate new LPWSTR - must deallocate it later
   LPWSTR widestr = new WCHAR[bufferlen + 1];

   ::MultiByteToWideChar(CP_ACP,0,instr,instr.GetLength(),widestr,bufferlen);

   // Ensure wide string is null terminated
   widestr[bufferlen] = 0;

   // Do something with widestr
   return widestr;
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftView

IMPLEMENT_DYNCREATE(CPCDraftView, imm_client)

BEGIN_MESSAGE_MAP(CPCDraftView, imm_client)
   //{{AFX_MSG_MAP(CPCDraftView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_HSCROLL()
   ON_WM_VSCROLL()
   ON_WM_LBUTTONDOWN()
   ON_WM_SETCURSOR()
   ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn) // Infinisys 2021 - ContextMenu
   ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN, OnUpdateViewZoomIn) // Infinisys 2021 - ContextMenu
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT, OnUpdateViewZoomOut)
   ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
   ON_COMMAND(ID_EDIT_SELECT_NONE, OnEditSelectNone)
   ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
   ON_COMMAND(ID_EDIT_MOVE, OnEditMove)
   ON_COMMAND(ID_EDIT_DUP_CIRCULAR, OnEditDupCircular)
   ON_COMMAND(ID_EDIT_DUP_LINEAR, OnEditDupLinear)
   ON_COMMAND(ID_DATA_OBJECT_INFO, OnDataObjectInfo)
   ON_COMMAND(ID_TOOL_DIM_FORMAT, OnToolDimFormat)
   ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_SPECIAL, OnUpdateHaveShapes)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_BACKWARD, OnUpdateHaveUnlockedSelection)
   ON_COMMAND(ID_TEXT_FONT, OnTextFont)
   ON_COMMAND(ID_ARRANGE_ROTATE_OPT, OnArrangeRotateOpt)
   ON_COMMAND(ID_ARRANGE_ALIGN_OPT, OnArrangeAlignOpt)
   ON_COMMAND(ID_ARRANGE_ROTATE, OnArrangeRotate)
   ON_COMMAND(ID_TOOL_MIRROR, OnToolMirror)
   ON_COMMAND(ID_ARRANGE_BACKWARD, OnArrangeBackward)
   ON_COMMAND(ID_ARRANGE_FORWARD, OnArrangeForward)
   ON_COMMAND(ID_ARRANGE_TO_BACK, OnArrangeToBack)
   ON_COMMAND(ID_ARRANGE_TO_FRONT, OnArrangeToFront)
   ON_COMMAND(ID_EDIT_REPEAT_PASTE, OnEditRepeatPaste)
   ON_COMMAND(ID_TEXT_HIDETEXT, OnTextHidetext)
   ON_UPDATE_COMMAND_UI(ID_TEXT_HIDETEXT, OnUpdateTextHidetext)
   ON_WM_QUERYNEWPALETTE()
   ON_COMMAND(ID_FILE_IMPORT_IMAGE,OnFileImportImage)  // Import Image - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TEXT_HIDETEXT,OnUpdateFileImportImage)
   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   ON_COMMAND(ID_EDIT_CUT, OnEditCut)
   ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
   ON_COMMAND(ID_EDIT_DUPLUCATE, OnEditDuplucate)
   ON_COMMAND(ID_EDIT_DUPLICATE_WITHOUT_OFFSET, OnEditDuplicateWithoutOffset) // Infinisys Ltd. 2021 - Contextual Menus
   ON_COMMAND(ID_ARRANGE_UNLOCK, OnArrangeUnlock)
   ON_COMMAND(ID_ARRANGE_LOCK, OnArrangeLock)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_LOCK, OnUpdateArrangeLock)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_UNLOCK, OnUpdateArrangeUnlock)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PEN, OnUpdateToolAttribPen)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PEN_MENU,OnUpdateToolAttribPen)
   ON_COMMAND(ID_TOOL_ATTRIB_PEN_MENU,OnToolAttribPenMenu)
   ON_COMMAND(ID_TOOL_ATTRIB_PEN_COLOR, OnToolAttribPenColor)
   ON_COMMAND(ID_TOOL_ATTRIB_PEN_PATTERN, OnToolAttribPenPattern)
   ON_COMMAND(ID_TOOL_ATTRIB_PEN_TRANSPARENCY,OnToolAttribPenTransparency)  // Transparency - Infinisys Ltd
   ON_COMMAND(ID_TOOL_ATTRIB_PEN_ALPHA,OnToolAttribPenAlpha)  // Interface Update - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PEN_TRANSPARENCY,OnUpdateToolAttribPenTransparency) // Transparency - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PEN_ALPHA,OnUpdateToolAttribPenAlpha) // Interface Update - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_FILL, OnUpdateToolAttribFill)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_FILL_MENU,OnUpdateToolAttribFill) // Interface Update - Infinisys Ltd.
   ON_COMMAND(ID_TOOL_ATTRIB_FILL_MENU,OnToolAttribFillMenu) // Interface Update - Infinisys Ltd.
   ON_COMMAND(ID_TOOL_ATTRIB_FILL_COLOR, OnToolAttribFillColor)
   ON_COMMAND(ID_TOOL_ATTRIB_FILL_PATTERN, OnToolAttribFillPattern)
   ON_COMMAND(ID_TOOL_ATTRIB_FILL_TRANSPARENCY,OnToolAttribFillTransparency) // Transparency - Infinisys Ltd
   ON_COMMAND(ID_TOOL_ATTRIB_FILL_ALPHA,OnToolAttribFillAlpha) // Interface Update - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_FILL_TRANSPARENCY,OnUpdateToolAttribFillTransparency) // Transparency - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_FILL_ALPHA,OnUpdateToolAttribFillAlpha) // Interface Update - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PFILL, OnUpdateToolAttribPFill)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PFILL_MENU,OnUpdateToolAttribPFill) // Interface Update - Infinisys Ltd.
   ON_COMMAND(ID_TOOL_ATTRIB_PFILL_MENU,OnToolAttribPFillMenu) // Interface Update - Infinisys Ltd.
   ON_COMMAND(ID_TOOL_ATTRIB_PFILL_COLOR, OnToolAttribPFillColor)
   ON_COMMAND(ID_TOOL_ATTRIB_PFILL_PATTERN, OnToolAttribPFillPattern)
   ON_COMMAND(ID_TOOL_ATTRIB_PFILL_TRANSPARENCY,OnToolAttribPFillTransparency) // Transparency - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PFILL_TRANSPARENCY,OnUpdateToolAttribPFillTransparency) // Transparency - Infinisys Ltd
   ON_COMMAND(ID_TOOL_ATTRIB_PFILL_ALPHA,OnToolAttribPFillAlpha) // Interface Update - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_PFILL_ALPHA,OnUpdateToolAttribPFillAlpha) // Interface Update - Infinisys Ltd
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_LINE_WEIGTH,OnUpdateToolAttribLineWeigth)
   ON_COMMAND(ID_TOOL_ATTRIB_LINE_WEIGTH, OnToolAttribLineWeigth)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_LINE_STYLE, OnUpdateToolAttribLineStyle)
   ON_COMMAND(ID_TOOL_ATTRIB_LINE_STYLE, OnToolAttribLineStyle)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ATTRIB_END_MARK, OnUpdateToolAttribEndMark)
   ON_COMMAND(ID_TOOL_ATTRIB_END_MARK, OnToolAttribEndMark)
   ON_COMMAND(ID_EDIT_DATUM, OnEditDatum)
   ON_COMMAND(ID_ARRANGE_FLIP_HORIZ, OnArrangeFlipHoriz)
   ON_COMMAND(ID_ARRANGE_FLIP_VERT, OnArrangeFlipVert)
   ON_COMMAND(ID_ARRANGE_GROUP, OnArrangeGroup)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_UNGROUP, OnUpdateArrangeUngroup)
   ON_COMMAND(ID_ARRANGE_UNGROUP, OnArrangeUngroup)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_GROUP, OnUpdateArrangeGroup)
   ON_UPDATE_COMMAND_UI(ID_TOOL_EXTEND, OnUpdateToolExtend)
   ON_COMMAND(ID_TOOL_EXTEND, OnToolExtend)
   ON_COMMAND(ID_TOOL_TRIM, OnToolTrim)
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
   ON_UPDATE_COMMAND_UI(ID_EDIT_RESHAPE_SMOOTH, OnUpdateEditReshapeSmooth)
   ON_COMMAND(ID_EDIT_RESHAPE_SMOOTH, OnEditReshapeSmooth)
   ON_UPDATE_COMMAND_UI(ID_EDIT_RESHAPE_UNSMOOTH, OnUpdateEditReshapeUnsmooth)
   ON_COMMAND(ID_EDIT_RESHAPE_UNSMOOTH, OnEditReshapeUnsmooth)
   ON_WM_MOUSEMOVE()
   ON_COMMAND(ID_TOOL_FILLET, OnToolFillet)
   ON_COMMAND(ID_TOOL_CHAMFER, OnToolChamfer)
   ON_UPDATE_COMMAND_UI(ID_TEXT_SELECTALLTEXT, OnUpdateTextSelectalltext)
   ON_COMMAND(ID_TEXT_SELECTALLTEXT, OnTextSelectalltext)
   ON_COMMAND(ID_TOOL_ROTATE_90, OnToolRotate90)
   ON_COMMAND(ID_ARRANGE_ROTATE_ZERO, OnArrangeRotateZero)
   ON_COMMAND(ID_DATA_FIELD_NAMES, OnDataFieldNames)
   ON_COMMAND(ID_EDIT_COPY_ATTRIB, OnEditCopyAttrib)
   ON_COMMAND(ID_EDIT_SELECT_SPECIAL, OnEditSelectSpecial)
   ON_COMMAND(ID_FILE_LOAD_LAYER, OnFileLoadLayer)
   ON_COMMAND(ID_LAYOUT_DRAWING_SIZE, OnLayoutDrawingSize)
   ON_COMMAND(ID_LAYOUT_POSITION, OnLayoutPosition)
   ON_COMMAND(ID_LAYOUT_SET_SCALE_UNITS, OnLayoutSetScaleUnits)
   ON_COMMAND(ID_TOOL_1_1, OnTool1_1)
   ON_UPDATE_COMMAND_UI(ID_TOOL_COMBINE, OnUpdateToolCombine)
   ON_COMMAND(ID_TOOL_COMBINE, OnToolCombine)
   ON_UPDATE_COMMAND_UI(ID_TOOL_GLUE, OnUpdateToolGlue)
   ON_COMMAND(ID_TOOL_GLUE, OnToolGlue)
   ON_COMMAND(ID_VIEW_RESTORE_PREV_ZOOM, OnViewRestorePrevZoom)
   ON_COMMAND(ID_VIEW_SAVE_VIEW, OnViewSaveView)
   ON_COMMAND(ID_FILE_SET_DEFAULTS, OnFileSetDefaults)
   ON_COMMAND(ID_EDIT_ATTRIB_OPTIONS, OnEditAttribOptions)
   ON_COMMAND(ID_FILE_PRINT_SLIDES, OnFilePrintSlides)
   ON_COMMAND(ID_VIEW_SLIDE_SHOW, OnViewSlideShow)
   ON_COMMAND(ID_VIEW_HOME_VIEW, OnViewHomeView)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_2X, OnUpdateViewZoomIn2x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_2X, OnViewZoomIn2x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_4X, OnUpdateViewZoomIn4x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_4X, OnViewZoomIn4x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_4X, OnUpdateViewZoomIn4x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_4X, OnViewZoomIn4x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_8X, OnUpdateViewZoomIn8x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_8X, OnViewZoomIn8x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_16X, OnUpdateViewZoomIn16x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_16X, OnViewZoomIn16x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_32X, OnUpdateViewZoomIn32x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_32X, OnViewZoomIn32x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN_64X, OnUpdateViewZoomIn64x)
   ON_COMMAND(ID_VIEW_ZOOM_IN_64X, OnViewZoomIn64x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT_2X, OnUpdateViewZoomOut2x)
   ON_COMMAND(ID_VIEW_ZOOM_OUT_2X, OnViewZoomOut2x)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT_4X, OnUpdateViewZoomOut4x)
   ON_COMMAND(ID_VIEW_ZOOM_OUT_4X, OnViewZoomOut4x)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_SHOW_RULERS, OnUpdateLayoutShowRulers)
   ON_COMMAND(ID_LAYOUT_SHOW_RULERS, OnLayoutShowRulers)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_STANDARD_RULERS, OnUpdateLayoutStandardRulers)
   ON_COMMAND(ID_LAYOUT_STANDARD_RULERS, OnLayoutStandardRulers)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_SCALE_RULERS, OnUpdateLayoutScaleRulers)
   ON_COMMAND(ID_LAYOUT_SCALE_RULERS, OnLayoutScaleRulers)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_HIDE_GRID_LINES, OnUpdateLayoutHideGridLines)
   ON_COMMAND(ID_LAYOUT_HIDE_GRID_LINES, OnLayoutHideGridLines)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_HIDE_PAGE_BREAKS, OnUpdateLayoutHidePageBreaks)
   ON_COMMAND(ID_LAYOUT_HIDE_PAGE_BREAKS, OnLayoutHidePageBreaks)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_SNAP_TO_OBJECT, OnUpdateLayoutSnapToObject)
   ON_COMMAND(ID_LAYOUT_SNAP_TO_OBJECT, OnLayoutSnapToObject)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_SNAP_TO_GUIDES, OnUpdateLayoutSnapToGuides)
   ON_COMMAND(ID_LAYOUT_SNAP_TO_GUIDES, OnLayoutSnapToGuides)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_SHOW_GUIDE_LINES, OnUpdateLayoutShowGuideLines)
   ON_COMMAND(ID_LAYOUT_SHOW_GUIDE_LINES, OnLayoutShowGuideLines)
   ON_UPDATE_COMMAND_UI(ID_PREF_HIDE_FILLS, OnUpdatePrefHideFills)
   ON_COMMAND(ID_PREF_HIDE_FILLS, OnPrefHideFills)
   ON_COMMAND(ID_FILE_PRINT_ACTIVE_LAYER, OnFilePrintActiveLayer)
   ON_UPDATE_COMMAND_UI(ID_TOOL_UNGLUE, OnUpdateToolUnglue)
   ON_COMMAND(ID_TOOL_UNGLUE, OnToolUnglue)
   ON_UPDATE_COMMAND_UI(ID_TOOL_SUBTRACT, OnUpdateToolSubtract)
   ON_COMMAND(ID_TOOL_SUBTRACT, OnToolSubtract)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_DISTRIBUTE, OnUpdateArrangeDistribute)
   ON_COMMAND(ID_ARRANGE_DISTRIBUTE, OnArrangeDistribute)
   ON_COMMAND(ID_ARRANGE_ALIGN_AGAIN, OnArrangeAlignAgain)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_ALIGN_AGAIN, OnUpdateArrangeAlignAgain)
   ON_WM_PAINT()
   ON_UPDATE_COMMAND_UI(ID_EDIT_DATUM, OnUpdateEditDatum)
   ON_COMMAND(ID_VIEW_MORE_VIEWS, OnViewMoreViews)
   ON_COMMAND(ID_VIEW_DELETE_ALL_VIEWS, OnViewDeleteAllViews)
   ON_UPDATE_COMMAND_UI(ID_VIEW_CURSOR_POS, OnUpdateViewCursorPos)
   ON_COMMAND(ID_VIEW_CURSOR_POS, OnViewCursorPos)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_POSITION, OnUpdateLayoutPosition)
   ON_COMMAND(ID_EDIT_APPLY_ATTRIB, OnEditApplyAttrib)
   ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_AREA, OnUpdateViewShowArea)
   ON_COMMAND(ID_VIEW_SHOW_AREA, OnViewShowArea)
   ON_UPDATE_COMMAND_UI(ID_VIEW_HIDE_AREA, OnUpdateViewHideArea)
   ON_COMMAND(ID_VIEW_HIDE_AREA, OnViewHideArea)
   ON_UPDATE_COMMAND_UI(ID_VIEW_HIDE_SHOW_AREA,OnUpdateViewHideShowArea)
   ON_COMMAND(ID_VIEW_HIDE_SHOW_AREA,OnViewHideShowArea)
   ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO,OnUpdateEditUndo)
   ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REDO,OnUpdateEditRedo) // Infinisys Ltd
   ON_COMMAND(ID_EDIT_REDO,OnEditRedo) // Infinisys Ltd
   ON_WM_KILLFOCUS()
   ON_WM_SETFOCUS()
   ON_WM_CHAR()
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_ATTRIB, OnUpdateHaveSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopyCut)
   ON_COMMAND(ID_EDIT_PASTE_UNSCALED, OnEditPasteUnscaled)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_UNSCALED, OnUpdateEditPasteUnscaled)
   ON_UPDATE_COMMAND_UI(ID_TOOL_DIM_TYPE, OnUpdateToolDimType)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
   ON_UPDATE_COMMAND_UI(ID_TOOL_DIM_ENDMARK, OnUpdateToolDimEndmark)
   ON_COMMAND(ID_TOOL_DIM_ENDMARK, OnToolDimEndmark)
   ON_WM_RBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_SYSKEYDOWN()
   ON_WM_SYSKEYUP()
   ON_WM_DROPFILES()
   ON_COMMAND(ID_FILLINFO, OnFillInfo)
   ON_COMMAND(ID_LINEINFO, OnLineInfo)
   ON_COMMAND(ID_TOOL_LINK, OnToolLink)
   ON_COMMAND(ID_TOOL_UNLINK, OnToolUnlink)
   ON_COMMAND(ID_TOOL_MULTIUNLINK, OnToolMultiunlink)
   ON_COMMAND(ID_PREF_TOOL_OPTIONS, OnPrefToolOptions)
   ON_UPDATE_COMMAND_UI(ID_PREF_TOOL_OPTIONS, OnUpdatePrefToolOptions)
   ON_COMMAND(ID_EDIT_CONTRACT, OnEditContract)
   ON_COMMAND(ID_EDIT_EXPAND, OnEditExpand)
   ON_COMMAND(ID_FILE_EXPORT_FILE, OnFileExportFile)
   ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_FILE, OnUpdateFileExportFile)
   ON_COMMAND(ID_PREF_PENSTYLE_BLADE, OnPrefPenstyleBlade)
   ON_UPDATE_COMMAND_UI(ID_PREF_PENSTYLE_BLADE, OnUpdatePrefPenstyleBlade)
   ON_COMMAND(ID_PREF_PENSTYLE_ROUND, OnPrefPenstyleRound)
   ON_UPDATE_COMMAND_UI(ID_PREF_PENSTYLE_ROUND, OnUpdatePrefPenstyleRound)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ROTATE, OnUpdateToolRotate)
   ON_COMMAND(ID_TOOL_ROTATE, OnToolRotate)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_LAYER_SETUP, OnUpdateDisableIfEditDatum)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SLIDES, OnUpdateFilePrintSlides)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_ACTIVE_LAYER, OnUpdateFilePrintActiveLayer)
   ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
   ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopyCut)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DUP_CIRCULAR, OnUpdateHaveSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DUP_LINEAR, OnUpdateHaveSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_MOVE, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_FLIP_HORIZ, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_FLIP_VERT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_FORWARD, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLUCATE, OnUpdateHaveSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATE_WITHOUT_OFFSET, OnUpdateHaveSelection) // Infinisys Ltd. 2021 - Contextual Menus
   ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_NONE, OnUpdateHaveSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_TO_BACK, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_TO_FRONT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_ROTATE_OPT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_ROTATE, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_TOOL_ROTATE_90, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REPEAT_PASTE, OnUpdateEditPaste)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_ALIGN_OPT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ARRANGE_ROTATE_ZERO, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_APPLY_ATTRIB, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_DATA_OBJECT_INFO, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateHaveShapes)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_LEFT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_TOP, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_BOTTOM, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_VCENTER, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_HCENTER, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_DISTH_CENTER, OnUpdateArrangeDistribute)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_DISTV_CENTER, OnUpdateArrangeDistribute)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_DISTH_DATUM, OnUpdateArrangeDistribute)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_DISTV_DATUM, OnUpdateArrangeDistribute)
   ON_COMMAND(IDC_POSITION,OnClickPosition)
   ON_COMMAND(IDC_ZOOM,OnClickZoom)
   ON_UPDATE_COMMAND_UI(ID_TOOL_LINK, OnUpdateHaveShapes)
   ON_UPDATE_COMMAND_UI(ID_TOOL_UNLINK, OnUpdateHaveShapes)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MULTIUNLINK, OnUpdateHaveSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CONTRACT, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_EDIT_EXPAND, OnUpdateHaveUnlockedSelection)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_SET_SCALE_UNITS, OnUpdateDisableIfEditDatum)
   //}}AFX_MSG_MAP
   // Standard printing commands
   ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TOOL_RADIO_FIRST, ID_TOOL_RADIO_LAST, OnUpdateToolsRange)
   ON_COMMAND_RANGE(ID_TOOL_RADIO_FIRST, ID_TOOL_RADIO_LAST, OnToolsRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_PREF_BORDER_LINE_INSIDE, ID_PREF_BORDER_LINE_OUTSIDE, OnUpdateBorderPositionRange)
   ON_COMMAND_RANGE(ID_PREF_BORDER_LINE_INSIDE, ID_PREF_BORDER_LINE_OUTSIDE, OnBorderPositionRange)

   ON_UPDATE_COMMAND_UI_RANGE(ID_PEN_COLORS_BASE, ID_PEN_COLORS_BASE+999, OnUpdatePenColorsRange) // Interface Update - Infinisys Ltd.
   ON_COMMAND_RANGE(ID_PEN_COLORS_BASE, ID_PEN_COLORS_BASE+999, OnPenColorsRange) // Interface Update - Infinisys Ltd.
   ON_UPDATE_COMMAND_UI_RANGE(ID_FILL_COLORS_BASE, ID_FILL_COLORS_BASE+999, OnUpdateFillColorsRange) // Interface Update - Infinisys Ltd.
   ON_COMMAND_RANGE(ID_FILL_COLORS_BASE, ID_FILL_COLORS_BASE+999, OnFillColorsRange) // Interface Update - Infinisys Ltd.
   ON_UPDATE_COMMAND_UI_RANGE(ID_PARA_COLORS_BASE, ID_PARA_COLORS_BASE+999, OnUpdateParaFillColorsRange) // Interface Update - Infinisys Ltd.
   ON_COMMAND_RANGE(ID_PARA_COLORS_BASE, ID_PARA_COLORS_BASE+999, OnParaFillColorsRange) // Interface Update - Infinisys Ltd.

   ON_UPDATE_COMMAND_UI_RANGE(ID_PEN_PATTERNS_BASE, ID_PEN_PATTERNS_BASE+999, OnUpdatePenPatternsRange)
   ON_COMMAND_RANGE(ID_PEN_PATTERNS_BASE,ID_PEN_PATTERNS_BASE + 999,OnPenPatternsRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_FILL_PATTERNS_BASE, ID_FILL_PATTERNS_BASE+999, OnUpdateFillPatternsRange)
   ON_COMMAND_RANGE(ID_FILL_PATTERNS_BASE, ID_FILL_PATTERNS_BASE+999, OnFillPatternsRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_PARA_PATTERNS_BASE,ID_PARA_PATTERNS_BASE + 999,OnUpdateParaFillPatternsRange)
   ON_COMMAND_RANGE(ID_PARA_PATTERNS_BASE,ID_PARA_PATTERNS_BASE + 999,OnParaFillPatternsRange)

   ON_UPDATE_COMMAND_UI_RANGE(ID_LINE_WIDTH_BASE, ID_LINE_WIDTH_BASE+10, OnUpdateLineWidthRange)
   ON_COMMAND_RANGE(ID_LINE_WIDTH_BASE, ID_LINE_WIDTH_BASE+10, OnLineWidthRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_STYLES_BASE, ID_STYLES_BASE+15, OnUpdateLineStyleRange)
   ON_COMMAND_RANGE(ID_STYLES_BASE, ID_STYLES_BASE+15, OnLineStyleRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TEXT_JUSTIFY_LEFT, ID_TEXT_JUSTIFY_RIGHT, OnUpdateTextJustifyRange)
   ON_COMMAND_RANGE(ID_TEXT_JUSTIFY_LEFT, ID_TEXT_JUSTIFY_RIGHT, OnTextJustifyRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TEXT_JUSTIFY_TOP, ID_TEXT_JUSTIFY_BOTTOM, OnUpdateTextVAlignRange)
   ON_COMMAND_RANGE(ID_TEXT_JUSTIFY_TOP, ID_TEXT_JUSTIFY_BOTTOM, OnTextVAlignRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TEXT_LINESPACING_SINGLE, ID_TEXT_LINESPACING_DOUBLE, OnUpdateTextSpacingRange)
   ON_COMMAND_RANGE(ID_TEXT_LINESPACING_SINGLE, ID_TEXT_LINESPACING_DOUBLE, OnTextSpacingRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TEXT_UPPERCASE, ID_TEXT_TITLE, OnUpdateTextCaseRange)
   ON_COMMAND_RANGE(ID_TEXT_UPPERCASE, ID_TEXT_TITLE, OnTextCaseRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_GRID_BASE, ID_GRID_BASE+9, OnUpdateGridRange)
   ON_COMMAND_RANGE(ID_GRID_BASE, ID_GRID_BASE+9, OnGridRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_ANGLE_SNAP_BASE, ID_ANGLE_SNAP_BASE+99, OnUpdateAngleSnapRange)
   ON_COMMAND_RANGE(ID_ANGLE_SNAP_BASE, ID_ANGLE_SNAP_BASE+99, OnAngleSnapRange)
   ON_COMMAND_RANGE(ID_VIEWS_BASE, ID_VIEWS_BASE+99, OnDeleteViewRange)
   ON_COMMAND_RANGE(ID_VIEW_SAVED_VIEW1, ID_VIEW_SAVED_VIEW5, OnSavedViewRange)
   ON_COMMAND_RANGE(ID_SAVED_VIEW1, ID_SAVED_VIEW5, OnSavedViewShortcutRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TOOL_SHOWSIZE1, ID_TOOL_SHOWSIZE4, OnUpdateShowSizeRange)
   ON_UPDATE_COMMAND_UI_RANGE(ID_TOOL_RESIZE_ABS, ID_TOOL_RESIZE_APPLY, OnUpdateResizeRange)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_POSITION_X,OnKillFocusResizePositionX)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_POSITION_Y,OnKillFocusResizePositionY)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_VALUE1,OnKillFocusResizeValue1)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_VALUE2,OnKillFocusResizeValue2)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_VALUE3,OnKillFocusResizeValue3)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_VALUE4,OnKillFocusResizeValue4)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_VALUE5,OnKillFocusResizeValue5)
   ON_EN_KILLFOCUS(ID_TOOL_RESIZE_VALUE6,OnKillFocusResizeValue6)
   ON_COMMAND_RANGE(ID_TOOL_RESIZE_ABS, ID_TOOL_RESIZE_APPLY, OnResizeRange)
   ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
   ON_WM_MOUSEWHEEL()
   ON_CBN_SELCHANGE(ID_TOOL_DIM_TYPE,OnSelChangeDimType)
   ON_COMMAND_RANGE(ID_ALIGN_LEFT, ID_ALIGN_DISTV_DATUM, OnAlign)

   ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)

#ifndef _UNICODE
   ON_MESSAGE(WM_IME_CHAR, OnImeChar)
#endif

   ON_MESSAGE(WM_APP + 1024,CPCDraftView::OnDropLibraryItem) // Library - Infinisys Ltd.
   ON_WM_DESTROY()
      ON_WM_TIMER() // PC Draft 7 - Auto Save
      ON_COMMAND(ID_GUIDES_ADDGUIDEATPOSITION, &CPCDraftView::OnGuidesAddGuideAtPosition)
      ON_COMMAND(ID_GUIDES_ADDHORIZONTALGUIDE, &CPCDraftView::OnGuidesAddhorizontalguide)
      ON_COMMAND(ID_GUIDES_ADDVERTICALGUIDE, &CPCDraftView::OnGuidesAddverticalguide)
      END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPCDraftView construction/destruction

CPCDraftView::CPCDraftView()
{

   m_iEditPasteCounter = 0;

   m_iAutoSaveMinutes = 5; // PC Draft 7 - Auto Save
   m_timeLastAutoSave = CTime::GetCurrentTime(); // PC Draft 7 - Auto Save
   m_bAutoSave = false; // PC Draft 7 - Auto Save
   m_bAutoSaving = false; // PC Draft 7 - Auto Save


   m_bInitialized=false;

   m_nCurrentTool=ID_TOOL_ARROW;
   m_nOneTimeTool=0;
   m_nSnapMode=kSnap;
   m_nRegPolySides=6;
   m_nStarSides=5; // Star Tool - Infinisys Ltd
   m_iStarThickness=50; // Star Tool - Infinisys Ltd
   m_ptPaste= CPoint(0,0);
   m_nPrintOption=kPrintDocument;

   m_bMouseTracking=false;
   m_bCursorPrompt=false;
   m_nPromptVisible=0;
   m_nCrossVisible=0;
   m_ptCrossCursorPos= CPoint(-1,-1);

   //m_nUndoID=0;
   //m_bRedo=false;
   //m_pUndo=NULL;
   //m_pRedo=NULL;
   m_pTemp=NULL;
   m_iUndo = 0;

   m_bResizeRel=false;
   m_pResizeShape=NULL;

   m_nDimType=0;

   m_bEditActive=false;

   m_pAttachCtx=new CAttachContext;
   m_pAddCtx=new CAddContext;
   m_pSubCtx=new CSubContext;
   m_pTrimCtx=new CTrimContext;
   m_pChamferCtx=new CChamferContext;
   m_pFilletCtx=new CFilletContext;

   m_hPlaceShape=NULL;
   m_bPlaceUnscaled=false;

   m_bTrackingShape=false;
}

CPCDraftView::~CPCDraftView()
{
   //if(m_pUndo) delete m_pUndo;
   //if(m_pRedo) delete m_pRedo;
   if(m_pTemp) delete m_pTemp;

   // Unlimited Undo/Redo - Infinisys Ltd
   for(int i = 0; i < m_undoitemptra.GetCount(); i++)
   {

      delete m_undoitemptra[i];

   }


   delete m_pAttachCtx;
   delete m_pAddCtx;
   delete m_pSubCtx;
   delete m_pTrimCtx;
   delete m_pChamferCtx;
   delete m_pFilletCtx;

   if(m_hPlaceShape) GlobalFree(m_hPlaceShape);
}

BOOL CPCDraftView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style|=WS_CLIPCHILDREN;
   
   cs.lpszClass=AfxRegisterWndClass(CS_DBLCLKS,LoadCursor(NULL,IDC_ARROW),NULL,NULL);
   if(!CView::PreCreateWindow(cs))
   {
      return FALSE;
   }
   cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftView drawing

void CPCDraftView::OnDraw(CDC* pDC)
{
   if(!m_bInitialized) return;

   CPCDraftDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);


   CRect rc;
   CRgn oldClip;
   pDC->GetClipBox(&rc);
   if(rc.IsRectEmpty()) return;
   else
   {
      HRGN hRgn=::CreateRectRgnIndirect(&rc);
      ::GetClipRgn(pDC->GetSafeHdc(),hRgn);
      oldClip.Attach(hRgn);
   }

   DrawBkgnd(pDC);


   if(pDoc->m_bShowRulers)
   {
      DrawRulers(pDC,&oldClip);
   }

#ifndef _PERSONAL_EDITION
   DrawLayerBar(pDC);
#endif
   // drawing

   CPalette* pPalette=pDC->SelectPalette(&pDoc->m_Palette,FALSE);
   pDC->RealizePalette();

   CRect rectView;
   //GetViewRect(rectView);
   GetActiveClientRect(rectView);
   CRgn theClipRgn;
   theClipRgn.CreateRectRgnIndirect(rectView);
   pDC->SelectClipRgn(&theClipRgn,RGN_AND);

   CPoint ptOrg=GetDeviceScrollPosition();
   pDC->SetViewportOrg(-ptOrg);
   if(!pDoc->m_bHideGridLines)
   {
      DrawGridLines(pDC,ptOrg);
   }
   if(!pDoc->m_bHidePageBreaks)
   {
      DrawPageBreaks(pDC,ptOrg);
   }
   pDC->SelectClipRgn(&oldClip);

   if(pDoc->m_bComplete) // completely opened
   {
      DrawDocument(pDC);
      DrawSelection(pDC);




   }
   pDC->SelectPalette(pPalette,FALSE);
   pDC->SelectClipRgn(&oldClip);
   pDC->SelectClipRgn(NULL);
   if (pDoc->m_bShowGuideLines)
   {
      DrawGuides(pDC, ptOrg);
   }
}

HBITMAP CPCDraftView::SelectionToBitmap()
{
   LongRect lrcExtents;
   if(m_Selection.GetCount()>1)
   {
      LongBoundRect lrcBound;
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         LongRect lrc=pShape->CalculateBasicExtents();
         pShape->ExtendForPen(lrc);
         lrcBound|=lrc;
      }
      lrcExtents=lrcBound;
   }
   else
   {
      CMDShape* pShape=m_Selection.GetHead();
      lrcExtents=pShape->CalculateBasicExtents();
      pShape->ExtendForPen(lrcExtents);
   }

   CRect rcExtents=m_Mapper.LPtoDP(lrcExtents);
   rcExtents.right++;
   rcExtents.bottom++;

   CPCDraftDoc* pDoc=GetDocument();
   CWindowDC theWindowDC(AfxGetMainWnd());
   CBitmap theBmp;
   theBmp.CreateCompatibleBitmap(&theWindowDC,Width(rcExtents),Height(rcExtents));
   {
      CGDIBitmapDC theDC(&theWindowDC,&theBmp);
      theDC.SetViewportOrg(-rcExtents.left,-rcExtents.top);

      CRgn theClipRgn;
      theClipRgn.CreateRectRgnIndirect(&rcExtents);
      theClipRgn.OffsetRgn(-rcExtents.left,-rcExtents.top);
      theDC.SelectClipRgn(&theClipRgn);

      theDC.FillRect(&rcExtents,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
      CDraftContext dc(&theDC,&m_Mapper,pDoc);
      CPalette* pPalette=theDC.SelectPalette(&pDoc->m_Palette,FALSE);
      theDC.RealizePalette();
      CMDShapeList workList;
      workList.AddTail(&m_Selection);
      CLayer* pActiveLayer=pDoc->GetActiveLayer();
      pActiveLayer->SortShapes(workList,false);
      while (workList.GetCount())
      {
         auto pShape = workList.RemoveTail();
         UseRect userect(&dc, pShape);
         pShape->DrawShape(&dc);
      }
      theDC.SelectPalette(pPalette,FALSE);
   }
   return (HBITMAP)theBmp.Detach();
}

HENHMETAFILE CPCDraftView::SelectionToMetaFile()
{
   LongRect lrcExtents;
   if(m_Selection.GetCount()>1)
   {
      LongBoundRect lrcBound;
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         LongRect lrc=pShape->CalculateBasicExtents();
         pShape->ExtendForPen(lrc);
         lrcBound|=lrc;
      }
      lrcExtents=lrcBound;
   }
   else
   {
      CMDShape* pShape=m_Selection.GetHead();
      lrcExtents=pShape->CalculateBasicExtents();
      pShape->ExtendForPen(lrcExtents);
   }

   //CRect rcExtents=m_Mapper.LPtoDP(lrcExtents);
   /*rcExtents.top -=10;
   rcExtents.left -=11;
   rcExtents.right+=11;
   rcExtents.bottom+=11;*/

   CPCDraftDoc* pDoc=GetDocument();
   CWindowDC theWindowDC(AfxGetMainWnd());

   /*	double hr = GetDeviceCaps(HDC(theWindowDC), HORZRES);
   	double vr = GetDeviceCaps(HDC(theWindowDC), VERTRES);*/

   int prevZoom = pDoc->m_nZoomFactor;
   pDoc->m_nZoomFactor = 0;
#ifdef _FLOAT_ZOOM
   double prevZoomFactor = pDoc->m_fZoomFactor;
   if(pDoc->m_fZoomFactor)
      pDoc->m_fZoomFactor = 1;
#endif
   CPCDraftApp* pApp = (CPCDraftApp*)AfxGetApp();
   BOOL prevHLS = pApp->m_bHideLineStyles;
   pApp->m_bHideLineStyles = FALSE;
   BOOL prevZLW = pApp->m_bZoomLineWeights;
   pApp->m_bZoomLineWeights = TRUE;
   bool prevHF = pDoc->m_bHideFills;
   pDoc->m_bHideFills = false;


   CRect rcExtents=m_Mapper.LPtoDP(lrcExtents);
   rcExtents.top -=1;
   rcExtents.left -=1;
   rcExtents.right+=1;
   rcExtents.bottom+=1;

   /*double z1 = Width(rcExtents) / hr;
   double z2 = Height(rcExtents) / vr;
   if(z1 < z2) z1 = z2;
   z2 = z1;
   if(z1 > 1) {
   	while(z1 > 1) {
   		pDoc->m_nZoomFactor--;
   		z1 /= 2;
   	}
   #ifdef _FLOAT_ZOOM
   	pDoc->m_fZoomFactor /= z2;
   #endif
   	rcExtents=m_Mapper.LPtoDP(lrcExtents);
   	rcExtents.right++;
   	rcExtents.bottom++;
   }*/

   CRect rect = rcExtents;

   /*CRgn theClipRgn1;
   theClipRgn1.CreateRectRgnIndirect(&rect);
   theWindowDC.SelectClipRgn(&theClipRgn1);*/

   int iWidthMM = GetDeviceCaps(HDC(theWindowDC), HORZSIZE);
   int iHeightMM = GetDeviceCaps(HDC(theWindowDC), VERTSIZE);
   int iWidthPels = GetDeviceCaps(HDC(theWindowDC), HORZRES);
   int iHeightPels = GetDeviceCaps(HDC(theWindowDC), VERTRES);

   rcExtents.left = (rcExtents.left * iWidthMM * 100)/iWidthPels;
   rcExtents.top = (rcExtents.top * iHeightMM * 100)/iHeightPels;
   rcExtents.right = (rcExtents.right * iWidthMM * 100)/iWidthPels;
   rcExtents.bottom = (rcExtents.bottom * iHeightMM * 100)/iHeightPels;

   HDC hMetaFile = CreateEnhMetaFile(HDC(theWindowDC), NULL, &rcExtents, NULL);//(LPSTR)"PC Draft Drawing\0PC Draft Drawing\0\0");

   CRgn theClipRgn;
   theClipRgn.CreateRectRgnIndirect(&rect);
   SelectClipRgn(hMetaFile, (HRGN)theClipRgn);

   FillRect(hMetaFile, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
   CDraftContext dc( CDC::FromHandle(hMetaFile), &m_Mapper,pDoc);
   HPALETTE pPalette=SelectPalette(hMetaFile, (HPALETTE)pDoc->m_Palette, FALSE);
   RealizePalette(hMetaFile);
   CMDShapeList workList;
   workList.AddTail(&m_Selection);
   CLayer* pActiveLayer=pDoc->GetActiveLayer();
   pActiveLayer->SortShapes(workList,false);
   while (workList.GetCount())
   {
      auto pShape = workList.RemoveTail();
      UseRect userect(&dc, pShape);
      pShape->DrawShape(&dc);
   }
   SelectPalette(hMetaFile, pPalette, FALSE);

   pDoc->m_nZoomFactor = prevZoom;
#ifdef _FLOAT_ZOOM
   pDoc->m_fZoomFactor = prevZoomFactor;
#endif
   pApp->m_bHideLineStyles = prevHLS;
   pApp->m_bZoomLineWeights = prevZLW;
   pDoc->m_bHideFills = prevHF;
   return CloseEnhMetaFile(hMetaFile);
}
void CPCDraftView::GetSelectionExtents(LPRECT lpcrect) // Infinisys Ltd.
{
   CPCDraftDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // PC Draft 7 - Zoom to Center, Better way to get selection extents
   // when no selection
   if (m_Selection.GetCount() <= 0)
   {

      lpcrect->left = 0;
      lpcrect->top = 0;
      lpcrect->right = 0;
      lpcrect->bottom = 0;


      return;

   }

   LongRect r;
   bool bFirst = true;
   POSITION pos=m_Selection.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetPrev(pos);

      LongRect lrc=pShape->GetExtents();
      pShape->ExtendForPen(lrc);
      if(bFirst)
         r = lrc;
      else
         r |= lrc;
      bFirst = false;
   }

   CPoint pt=m_Mapper.LPtoDP(LongPoint(r.left, r.top));
   lpcrect->left = pt.x;
   lpcrect->top = pt.y;
   pt=m_Mapper.LPtoDP(LongPoint(r.right,r.bottom));
   lpcrect->right = pt.x;
   lpcrect->bottom = pt.y;

}


void CPCDraftView::FullDrawSelection(CDC *pDC, bool bTransparentBackground) // Infinisys Ltd.
{
   
   //CPCDraftDoc* pDoc = GetDocument();

   //ASSERT_VALID(pDoc);

   //CDraftContext dc(pDC,&m_Mapper,pDoc);

   //dc.m_bTransparentBackground = false;

   //dc.SetGreyMode(false);

   //CPalette* pPalette=pDC->SelectPalette(&pDoc->m_Palette,FALSE);
   //pDC->RealizePalette();

   //POSITION pos = m_Selection.GetTailPosition();

   //while(pos != NULL)
   //{

   //   CMDShape* pShape = m_Selection.GetPrev(pos);

   //   pShape->DrawShape(&dc);

   //}
   //pDC->SelectPalette(pPalette,FALSE);

}


void CPCDraftView::DrawDocument(CDC *pDC)
{
   CPCDraftDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   CDraftContext dc(pDC,&m_Mapper,pDoc);

   m_Mapper.m_bZoomLineWeights = ((CPCDraftApp*)AfxGetApp())->m_bZoomLineWeights;

   //CRect rcClient;
   GetActiveClientRect(dc.m_rcClientActive);
   CRgn rgn;
   rgn.CreateRectRgnIndirect(&dc.m_rcClientActive);
   pDC->SelectClipRgn(&rgn,RGN_AND);
   dc.m_rcClientActive.OffsetRect(GetDeviceScrollPosition());

   for(int i=0; i<pDoc->m_Layers.GetSize(); i++)
   {
      CLayer* pLayer=pDoc->GetLayer(i);
      if(pLayer->Draw(&dc,pDoc->m_nActiveLayer==i,pDoc->m_bHideText,m_pActiveTextShape))
      {
         break;
      }
   }
   if(m_nOneTimeTool==ID_TOOL_FILLET)
   {
      m_pFilletCtx->DrawFillet(&dc);
   }
   if(m_nCurrentTool==ID_TOOL_EDIT_DATUM && m_Selection.GetCount()>0)
   {
      // draw datum
      CGDIROP rop(pDC,R2_NOT);
      CPoint pt=m_Mapper.LPtoDP(m_Selection.GetHead()->GetShapeDatum());
      pDC->MoveTo(pt+ CPoint(-4,-4));
      pDC->LineTo(pt+ CPoint(5,5));
      pDC->MoveTo(pt+ CPoint(4,-4));
      pDC->LineTo(pt+ CPoint(-5,5));
   }
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftView printing

BOOL CPCDraftView::OnPreparePrinting(CPrintInfo* pInfo)
{
   CPCDraftDoc* pDoc=GetDocument();
   pDoc->m_PrinterSettings.SetThisPrinter();

   /*	delete pInfo->m_pPD;
   	pInfo->m_pPD=new CMDPrintDlg(FALSE);
   */
   pInfo->SetMinPage(1);
   int nPages=pDoc->m_NumPage.x*pDoc->m_NumPage.y;
   if(m_nPrintOption==kPrintSlides)
   {
      int nSlides=pDoc->GetLayersCount();
      if(m_bLayer1AsMaster) nSlides--;
      if(m_bFitToPage) nPages=1;
      pInfo->SetMaxPage(nSlides*nPages);
   }
   else pInfo->SetMaxPage(nPages);

   BOOL bResult=DoPreparePrinting(pInfo);
   if(!bResult) pDoc->m_PrinterSettings.RestorePrinter();
   return bResult;
}

void CPCDraftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CPCDraftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   GetDocument()->m_PrinterSettings.RestorePrinter();
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftView diagnostics

#ifdef _DEBUG
void CPCDraftView::AssertValid() const
{
   CView::AssertValid();
}

void CPCDraftView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CPCDraftDoc* CPCDraftView::GetDocument() const// non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPCDraftDoc)));
   return (CPCDraftDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPCDraftView message handlers

void CPCDraftView::OnInitialUpdate()
{
//	CView::OnInitialUpdate();
   CPCDraftDoc* pDoc=GetDocument();

   m_Mapper.SetDocument(pDoc);
   m_wndFillInfo.SetDoc(pDoc);
   m_wndLineInfo.SetDoc(pDoc);

   CalcDocumentDevSize();
   if(GetDocument()->GetActiveLayer() != NULL)
   {
      SetSnap(GetDocument()->GetActiveLayer()->m_nSnap);
   }
   ResetDupDelta();

   m_pActiveTextShape=NULL;
   m_bEditActive=false;

   UpdateZoomInfo();

   CPCDraftApp* pApp=(CPCDraftApp*)AfxGetApp();
   if(!pApp->m_bShowCursorPosition)
   {
      m_wndPosition.m_wstr = get_uni(pDoc->GetTitle());
      m_wndPosition.Invalidate();
   }

   if(pDoc->GetPathName().IsEmpty() && pApp->m_bOpenAtCenter && (::GetAsyncKeyState(VK_MENU)>=0))
   {
      CPoint ptOrg= CPoint(pDoc->GetTotalDevSize());
      CSize szClientSize;
      GetClientSize(szClientSize);
      ptOrg.x-=szClientSize.cx;
      ptOrg.y-=szClientSize.cy;
      if(ptOrg.x<0) ptOrg.x=0;
      else ptOrg.x/=2.0;
      if(ptOrg.y<0) ptOrg.y=0;
      else ptOrg.y /= 2.0;
      ScrollToDevicePosition(ptOrg);
   }
   m_bInitialized=true;
}


//typedef DPI_AWARENESS_CONTEX TSetThreadDpiAwarenessContext(
//   DPI_AWARENESS_CONTEXT dpiContext
//   );

int CPCDraftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CView::OnCreate(lpCreateStruct) == -1)
      return -1;
   SetTimer(TIMER_ONE_SECOND, 1000, NULL);
   //DWORD isV2 = dpi_awareness_is_per_monitor_v2_window(GetSafeHwnd());
   //TRACE("dpi_awareness_is_per_monitor_v2_window = %d", isV2);
   //DWORD isDpiAware = dpi_awareness_is_per_monitor_window(GetSafeHwnd());
   //TRACE("dpi_awareness_is_per_monitor_window = %d",isDpiAware);
   //pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"GetCurrentPackageFullName");
   CRect rc(0,0,0,0);
   m_wndVertBar.Create(WS_CHILD|WS_VISIBLE|SBS_VERT,rc,this,IDC_VERT_BAR);
   m_wndHorzBar.Create(WS_CHILD|WS_VISIBLE|SBS_HORZ,rc,this,IDC_HORZ_BAR);
   m_wndFillInfo.Create(rc,this,ID_FILLINFO);
   m_wndLineInfo.Create(rc,this,ID_LINEINFO);

   CFont font;
   font.CreateStockObject(DEFAULT_GUI_FONT);
   //m_wndPosition.Create("",WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_NOTIFY,rc,this,IDC_POSITION);
   m_wndPosition.Create(NULL, "", WS_BORDER|WS_VISIBLE | WS_CHILD, rc, this, IDC_POSITION, nullptr);
   m_wndPosition.SetFont(&font,FALSE);
   m_wndZoomInfo.Create("",WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_NOTIFY,rc,this,IDC_ZOOM);
   m_wndZoomInfo.SetFont(&font,FALSE);
   m_wndZoomInfo.SetWindowText("");
//	m_wndSizeBar.Create(WS_CHILD|WS_VISIBLE|SBS_SIZEGRIP,rc,this,IDC_SIZE_BAR);
   m_wndSizeBar.Create("",WS_CHILD|WS_VISIBLE,rc,this,IDC_SIZE_BAR);

   DragAcceptFiles();

   m_dragdrop.Initialize(this);

   return 0;

}


void CPCDraftView::OnSize(UINT nType, int cx, int cy)
{

   if (m_bEditActive)
   {

      m_pbitmapDrawBackground.release();

   }

   CView::OnSize(nType, cx, cy);

   CRect rc;

   int nCyHScroll=::GetSystemMetrics(SM_CYHSCROLL);

   int nCxVScroll=::GetSystemMetrics(SM_CXVSCROLL);

#ifndef _PERSONAL_EDITION

   rc.SetRect(cx-nCxVScroll,57,cx,cy-nCyHScroll);

#else

   rc.SetRect(cx-nCxVScroll,0,cx,cy-nCyHScroll);

#endif

   m_wndVertBar.MoveWindow(&rc);

   rc.SetRect(15*nCxVScroll,cy-nCyHScroll,cx-nCxVScroll,cy);

   m_wndHorzBar.MoveWindow(&rc);

   rc.SetRect(-1,cy-nCyHScroll,nCxVScroll+1,cy+1);

   m_wndFillInfo.MoveWindow(&rc);

   rc.SetRect(nCxVScroll,cy-nCyHScroll,3*nCxVScroll+1,cy+1);

   m_wndLineInfo.MoveWindow(&rc);

   rc.SetRect(3*nCxVScroll,cy-nCyHScroll,13*nCxVScroll+1,cy+1);

   m_wndPosition.MoveWindow(&rc);

   rc.SetRect(13*nCxVScroll,cy-nCyHScroll,15*nCxVScroll,cy+1);

   m_wndZoomInfo.MoveWindow(&rc);

   rc.SetRect(cx-nCxVScroll,cy-nCyHScroll,cx,cy);

   m_wndSizeBar.MoveWindow(&rc);

   UpdateBars();

}


void CPCDraftView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if(pScrollBar==&m_wndHorzBar)
   {
      OnScroll(MAKEWORD(nSBCode,-1),nPos);
   }
   CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPCDraftView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if(pScrollBar==&m_wndVertBar)
   {
      OnScroll(MAKEWORD(-1,nSBCode),nPos);
   }
   CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


#define SB_OFFSET 16 // Infinisys Ltd.

BOOL CPCDraftView::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
{
   ShowCursorPrompt(false);
   ShowCrossCursor(false);
   OnScroll(MAKEWORD(-1,SB_OFFSET),-zDelta/2); // Infinisys Ltd.
   ShowCrossCursor();
   ShowCursorPrompt();
   return true;
}

BOOL CPCDraftView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
   // calc new x position
   int x=m_wndHorzBar.GetScrollPos();
   int xOrig=x;

   switch(LOBYTE(nScrollCode))
   {
   case SB_TOP:
      x=0;
      break;
   case SB_BOTTOM:
      x=INT_MAX;
      break;
   case SB_LINEUP:
      x-=m_lineDev.cx;
      break;
   case SB_LINEDOWN:
      x+=m_lineDev.cx;
      break;
   case SB_PAGEUP:
      x-=m_pageDev.cx;
      break;
   case SB_PAGEDOWN:
      x+=m_pageDev.cx;
      break;
   case SB_THUMBTRACK:
      x=nPos;
      break;
   }

   // calc new y position
   int y=m_wndVertBar.GetScrollPos();
   int yOrig=y;

   switch(HIBYTE(nScrollCode))
   {
   case SB_TOP:
      y=0;
      break;
   case SB_BOTTOM:
      y=INT_MAX;
      break;
   case SB_LINEUP:
      y-=m_lineDev.cy;
      break;
   case SB_LINEDOWN:
      y+=m_lineDev.cy;
      break;
   case SB_PAGEUP:
      y-=m_pageDev.cy;
      break;
   case SB_PAGEDOWN:
      y+=m_pageDev.cy;
      break;
   case SB_OFFSET:
      y+=nPos; // Infinisys Ltd.
      break;
   case SB_THUMBTRACK:
      y=nPos;
      break;
   }

   BOOL bResult=OnScrollBy(CSize(x-xOrig,y-yOrig),bDoScroll);
   if(bResult && bDoScroll)
      UpdateWindow();

   return bResult;
}

BOOL CPCDraftView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
   int xOrig,x;
   int yOrig,y;

   // don't scroll if there is no valid scroll range (ie. no scroll bar)
   if(!m_wndVertBar.IsWindowEnabled())
   {
      // vertical scroll bar not enabled
      sizeScroll.cy=0;
   }
   if(!m_wndHorzBar.IsWindowEnabled())
   {
      // horizontal scroll bar not enabled
      sizeScroll.cx=0;
   }

   // adjust current x position
   xOrig=x=m_wndHorzBar.GetScrollPos();
   int xMax=m_wndHorzBar.GetScrollLimit();
   x+=sizeScroll.cx;
   if(x<0)
      x=0;
   else if(x>xMax)
      x=xMax;

   // adjust current y position
   yOrig=y=m_wndVertBar.GetScrollPos();
   int yMax=m_wndVertBar.GetScrollLimit();
   y+=sizeScroll.cy;
   if(y<0)
      y=0;
   else if(y>yMax)
      y=yMax;

   // did anything change?
   if(x==xOrig && y==yOrig)
      return FALSE;

   if(bDoScroll)
   {
      // do scroll and update scroll positions
      ScrollWindow(-(x-xOrig),-(y-yOrig));
      if(x!=xOrig)
         m_wndHorzBar.SetScrollPos(x);
      if(y!=yOrig)
         m_wndVertBar.SetScrollPos(y);
   }
   return TRUE;
}

CPoint CPCDraftView::GetDeviceScrollPosition(bool bCentered) const
{
   CPoint pt(m_wndHorzBar.GetScrollPos(),m_wndVertBar.GetScrollPos());
   ASSERT(pt.x>=0 && pt.y>=0);
   if(!bCentered)
   {
      return pt;
   }
   // Infinisys
   CRect rectClient;
   GetClientRect(rectClient);
   CSize size = GetDocument()->GetTotalDevSize();
   if(rectClient.Width() > size.cx)
   {
      pt.Offset(-(rectClient.Width() - size.cx) / 2,0);

   }
   else
   {
      pt.Offset(-VIEW_MARGIN,0);
   }
   pt.Offset(0,-VIEW_MARGIN);
   return pt;
}

void CPCDraftView::ScrollToDevicePosition(POINT ptDev)
{
   // Infinisys
   CRect rectClient;
   GetClientRect(rectClient);
   CSize size = GetDocument()->GetTotalDevSize();
   if(rectClient.Width() > size.cx)
   {
      ptDev.x+= -(rectClient.Width() - size.cx) / 2;

   }
   else
   {
      ptDev.x += VIEW_MARGIN;
   }
   ptDev.y+=VIEW_MARGIN;


//   ASSERT(ptDev.x >= -VIEW_MARGIN);
//   ASSERT(ptDev.y >= -VIEW_MARGIN);

   int xOrig,yOrig;
   if(m_wndHorzBar.IsWindowEnabled())
   {
      xOrig=m_wndHorzBar.GetScrollPos();
      if(xOrig!=ptDev.x)
      {
         m_wndHorzBar.SetScrollPos(ptDev.x);
         ptDev.x=m_wndHorzBar.GetScrollPos();
      }
   }
   else
   {
      xOrig = ptDev.x;
   }
   if(m_wndVertBar.IsWindowEnabled())
   {
      yOrig=m_wndVertBar.GetScrollPos();
      if(yOrig!=ptDev.y)
      {
         m_wndVertBar.SetScrollPos(ptDev.y);
         ptDev.y=m_wndVertBar.GetScrollPos();
      }
   }
   else
   {
      yOrig = ptDev.y;
   }
   ScrollWindow(xOrig - ptDev.x,yOrig - ptDev.y);
}

void CPCDraftView::SetScrollSizes(SIZE sizeTotal)
{
   ASSERT(sizeTotal.cx>=0 && sizeTotal.cy>=0);

   m_totalDev=sizeTotal;
//	m_pageDev.cx=m_totalDev.cx/10;
//	m_pageDev.cy=m_totalDev.cy/10;
//	GetClientSize(m_pageDev);
   m_pageDev.cx=360;
   m_pageDev.cy=360;
   m_lineDev.cx=m_pageDev.cx/10;
   m_lineDev.cy=m_pageDev.cy/10;

   if(m_hWnd!=NULL)
   {
      // window has been created, invalidate now
      UpdateBars();
      Invalidate(TRUE);
   }
}

void CPCDraftView::GetClientSize(CSize& size)
{
   CRect rect;
   GetClientRect(&rect);
   ASSERT(rect.top==0 && rect.left==0);
   size.cx=rect.right-GetSystemMetrics(SM_CXVSCROLL);
   size.cy=rect.bottom-GetSystemMetrics(SM_CYHSCROLL);
   if(GetDocument()->m_bShowRulers)
   {
      size.cx-=kLRulerWid;
      size.cy-=kTRulerWid;
   }
}


void CPCDraftView::GetViewRect(CRect& rectView)
{
   
   GetClientRect(&rectView);

   if (m_wndVertBar.IsWindowVisible())
   {
    
      rectView.right -= GetSystemMetrics(SM_CXVSCROLL);

   }

   if (m_wndHorzBar.IsWindowVisible())
   {

      rectView.bottom -= GetSystemMetrics(SM_CYHSCROLL);

   }

   if (GetDocument()->m_bShowRulers)
   {

      rectView.left += kLRulerWid;

      rectView.top += kTRulerWid;

   }

}


void CPCDraftView::UpdateBars()
{

   CSize sizeClient;
   GetClientSize(sizeClient);

   CSize sizeRange;
   CPoint ptMove;
   CSize needSb;

   // get the current scroll bar state given the true client area
   GetScrollBarState(sizeClient,needSb,sizeRange,ptMove);

   // first scroll the window as needed
   ScrollToDevicePosition(ptMove); // will set the scroll bar positions too

   // this structure needed to update the scrollbar page range
   SCROLLINFO info;
   info.fMask=SIF_PAGE|SIF_RANGE;
   info.nMin=0;

   // now update the bars as appropriate
   if(needSb.cx)
   {
      if(!m_wndHorzBar.IsWindowEnabled())
         m_wndHorzBar.EnableWindow(TRUE);
      info.nPage=sizeClient.cx;
      info.nMax=m_totalDev.cx - 1 + VIEW_MARGIN + VIEW_MARGIN;// +100 Infinisys
      m_wndHorzBar.SetScrollInfo(&info,TRUE);
   }
   else if(m_wndHorzBar.IsWindowEnabled())
   {
      m_wndHorzBar.EnableWindow(FALSE);
      // ?????
      CRect rc;
      m_wndHorzBar.GetWindowRect(&rc);
      ScreenToClient(&rc);
      m_wndHorzBar.MoveWindow(&rc);
      // ?????
   }

   if(needSb.cy)
   {
      if(!m_wndVertBar.IsWindowEnabled())
         m_wndVertBar.EnableWindow(TRUE);
      info.nPage=sizeClient.cy;
      info.nMax=m_totalDev.cy - 1 + VIEW_MARGIN + VIEW_MARGIN; // +100 Infinisys
      m_wndVertBar.SetScrollInfo(&info,TRUE);
   }
   else if(m_wndVertBar.IsWindowEnabled())
   {
      m_wndVertBar.EnableWindow(FALSE);
      // ?????
      CRect rc;
      m_wndVertBar.GetWindowRect(&rc);
      ScreenToClient(&rc);
      m_wndVertBar.MoveWindow(&rc);
      // ?????
   }
}

void CPCDraftView::GetScrollBarState(CSize sizeClient,CSize& needSb,CSize& sizeRange, CPoint& ptMove)
{
   // enough room to add scrollbars
   sizeRange=m_totalDev-sizeClient;
   // > 0 => need to scroll
   ptMove=GetDeviceScrollPosition();
   // point to move to (start at current scroll pos)

   BOOL bNeedH=sizeRange.cx>- VIEW_MARGIN - VIEW_MARGIN; // Infinisys
   if(!bNeedH)
      ptMove.x=- VIEW_MARGIN - VIEW_MARGIN;                       // jump back to origin

   BOOL bNeedV=sizeRange.cy>- VIEW_MARGIN - VIEW_MARGIN; // Infinisys
   if(!bNeedV)
      ptMove.y=-100;                       // jump back to origin

   // if current scroll position will be past the limit, scroll to limit
   if(sizeRange.cx>0 && ptMove.x>=sizeRange.cx)
      ptMove.x=sizeRange.cx;
   if(sizeRange.cy>0 && ptMove.y>=sizeRange.cy)
      ptMove.y=sizeRange.cy;

   // now update the bars as appropriate
   needSb.cx=bNeedH;
   needSb.cy=bNeedV;

   // needSb, sizeRange, and ptMove area now all updated
}

void CPCDraftView::ScrollWindow(int dx, int dy)
{
   CRect clientRect;
   CSize clientSize;
   GetClientSize(clientSize);
   clientRect.SetRect(0, 0, clientSize.cx/*+1*/, clientSize.cy/* + 1*/);

   if (dx || dy) DeactivateTextShape();

   if (GetDocument()->m_bShowRulers)
   {
      CRect clipRect;
      //clientRect.OffsetRect(kLRulerWid + 2, kTRulerWid + 2);
      clientRect.OffsetRect(kLRulerWid + 1, kTRulerWid + 1);
      if (dx && dy)
      {
         CView::ScrollWindow(dx, dy, clientRect, clientRect);
         clipRect.SetRect(kLRulerWid + 1, 0, clientRect.right, kTRulerWid + 1);
         CView::ScrollWindow(dx, 0, clipRect, clipRect);
         clipRect.SetRect(0, kTRulerWid + 1, kLRulerWid + 1, clientRect.bottom);
         CView::ScrollWindow(0, dy, clipRect, clipRect);
      }
      else if (dx)
      {
         clipRect.SetRect(kLRulerWid + 1, 0, clientRect.right, clientRect.bottom);
         CView::ScrollWindow(dx, 0, clipRect, clipRect);
      }
      else if (dy)
      {
         clipRect.SetRect(0, kTRulerWid + 1, clientRect.right, clientRect.bottom);
         CView::ScrollWindow(0, dy, clipRect, clipRect);
      }
   }
   else
      CView::ScrollWindow(dx, dy, clientRect, clientRect);
}

void CPCDraftView::DrawRulers(CDC * pDC, CRgn* pClip)
{
   int nRes=GetDocument()->m_nDisplayRes;

   CDimUnit theDimUnits;
   int nScaleIndex=-1,nUnits,nPlaces;
   bool bEnglishUnits;
   LongRatio scaleRatio;
   if(!GetDocument()->m_bStandardRulers)
   {
      if(GetDocument()->GetActiveLayer() != NULL)
      {
         nScaleIndex=GetDocument()->GetActiveLayer()->m_nScale;
      }
      if(nScaleIndex == -1)
      {
         nScaleIndex = GetDocument()->GetNearestScale();
      }
      nUnits=GetDocument()->m_nUnit;
      bEnglishUnits=GetDocument()->m_bEnglishUnits;
      /* set new units */
      if(nUnits==kUDecFeetInches && nScaleIndex>=kEngNoInch)
         nUnits=kUFracFeetInches;
      /* set new places */
      nPlaces=0;
      if(nScaleIndex<(bEnglishUnits?kEng121:kMet221))
      {
         if(nUnits==kUMeters)				nPlaces=3;
         else if(nUnits==kUCentimeters)		nPlaces=1;
         else if(nUnits==kUMillimeters)		nPlaces=0;
         else if(nUnits==kUDecFeet)
         {
            if(nScaleIndex==k10X)			nPlaces=2;
            else if(nScaleIndex==k4X)		nPlaces=3;
            else							nPlaces=2;
         }
         else								nPlaces=2;
      }
      else
      {
         if(nUnits==kUDecimeters && nScaleIndex<kMet125)	nPlaces=1;
         else if(nUnits==kUMeters && nScaleIndex<kMet1250) nPlaces=2;
         else if(nUnits==kUDecFeet)
            if(nScaleIndex<kDecNoInch) nPlaces=1;
      }
      theDimUnits.setDimUnits(nUnits,false,nPlaces,nRes,0,0);
      if(!bEnglishUnits)
         theDimUnits.m_Display=kDimNoUnits;
      if(GetDocument()->GetActiveLayer() != NULL)
      {
         GetDocument()->GetScaleRatio(GetDocument()->GetActiveLayer()->m_nScale,scaleRatio);
      }
   }

   WrFixed nMarkStep;
   int nMinTicks,nNumMarks,nMarkLen[10];
   int hOrgOff=GetDocument()->LPtoDP(GetDocument()->m_RulerOrigin.x);
   int vOrgOff=GetDocument()->LPtoDP(GetDocument()->m_RulerOrigin.y);
   CString thePosStr;
   LPCTSTR c;
   CalcRulerInfo(nRes,nMarkStep,nNumMarks,nMarkLen);
   nMinTicks=nRes/nNumMarks;
   CRect rc;
   GetClientRect(&rc);
   rc.right-=::GetSystemMetrics(SM_CXVSCROLL);
   rc.bottom-=::GetSystemMetrics(SM_CYHSCROLL);

   CRect rcBack;
   rcBack.SetRect(0,0,kLRulerWid+1,kTRulerWid+1);
   
   {

      CGDIBackColor bc(pDC, RGB(255, 255, 255));
      pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rcBack, NULL, 0, NULL);

   }

   CPoint ptOrg=GetDeviceScrollPosition();
   CFont theHFont;
   theHFont.CreateFont(-10,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Geneva");
   CFont theVFont;
   theVFont.CreateFont(-10,0,900,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Geneva");
   // horizontal bar
   CRgn theClipRgn;
   rcBack.SetRect(kLRulerWid+1,0,rc.right+1,kTRulerWid+1);
   theClipRgn.CreateRectRgnIndirect(&rcBack);
   if(NULLREGION!=pDC->SelectClipRgn(&theClipRgn,RGN_AND))
   {
      
      {

         CGDIBackColor bc(pDC, RGB(255, 255, 255));
         pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rcBack, NULL, 0, NULL);

      }

      pDC->SetViewportOrg(-ptOrg.x,0);

      CGDIObject<CFont> thePosFont(pDC,&theHFont);
      CGDITextAlign thePosAlign(pDC,TA_LEFT|TA_TOP|TA_UPDATECP);
      int start=0;
      WrFixed n=0,posN;
      while(start+nRes<ptOrg.x)
      {
         start+=nRes;
         n+=nMarkStep;
      }
      if(hOrgOff)
      {
         while (hOrgOff>=nRes)
         {
            hOrgOff-=nRes;
            n-=nMarkStep;
         }
         start-=nRes-hOrgOff;
         n-=nMarkStep;
      }
      for(int i=start; i<ptOrg.x+rc.right; i+=nRes)
      {
         int	k=0;
         int end=i+nRes;
         for(int j=i; j<end; j+=nMinTicks)	/* draw ticks for one inch */
         {
            pDC->MoveTo(kLRulerWid+1+j,kTRulerWid);
            pDC->LineTo(kLRulerWid+1+j,kTRulerWid-nMarkLen[k]-1);
            k=(k+1)%nNumMarks;
         }
         posN=Abs(n);
         if(GetDocument()->m_bStandardRulers)
         {
            thePosStr.Format("%d",posN.GetFixed());
         }
         else
         {
            thePosStr=theDimUnits.numUser2Str(theDimUnits.numDB2User(scaleRatio,posN),false);
         }
         c=thePosStr;
         int len=thePosStr.GetLength();
         int h;
         if(len>3) h=i+kHOffset2+kHOffset2;
         else if(len>1) h=i+kHOffset2;
         else h=i+kHOffset1;
         pDC->MoveTo(kLRulerWid+2+h,kHOffsetV);
         pDC->TextOut(0,0,c++,1);
         if(len>3)			/* draw 2nd char left of tick */
            pDC->TextOut(0,0,c++,1);
         if(len>1)
            pDC->MoveTo(kLRulerWid+2+i+kHOffset1,kHOffsetV);
         while(*c)
            pDC->TextOut(0,0,c++,1);
         n+=nMarkStep;
      }
      pDC->SetViewportOrg(0,0);
   }
   pDC->SelectClipRgn(pClip);
   // vertical bar
   rcBack.SetRect(0,kTRulerWid+1,kLRulerWid+1,rc.bottom+1);
   theClipRgn.DeleteObject();
   theClipRgn.CreateRectRgnIndirect(&rcBack);
   if(NULLREGION!=pDC->SelectClipRgn(&theClipRgn,RGN_AND))
   {
      
      {

         CGDIBackColor bc(pDC, RGB(255, 255, 255));
         pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rcBack, NULL, 0, NULL);

      }

      pDC->SetViewportOrg(0,-ptOrg.y);

      CGDIObject<CFont> thePosFont(pDC,&theVFont);
      CGDITextAlign thePosAlign(pDC,TA_LEFT|TA_TOP|TA_UPDATECP);
      int start=0;
      WrFixed n=0,posN;
      while(start+nRes<ptOrg.y)
      {
         start+=nRes;
         n+=nMarkStep;
      }
      if(vOrgOff)
      {
         while(vOrgOff>=nRes)
         {
            vOrgOff-=nRes;
            n-=nMarkStep;
         }
         start-=nRes-vOrgOff;
         n-=nMarkStep;
      }
      for(int i=start; i<ptOrg.y+rc.bottom; i+=nRes)
      {
         int	k=0;
         int end=i+nRes;
         for(int j=i; j<end; j+=nMinTicks)	/* draw ticks for one inch */
         {
            pDC->MoveTo(kLRulerWid,kTRulerWid+j+1);
            pDC->LineTo(kLRulerWid-nMarkLen[k]-1,kTRulerWid+j+1);
            k=(k+1)%nNumMarks;
         }
         posN=Abs(n);
         if(GetDocument()->m_bStandardRulers)
         {
            thePosStr.Format("%d",posN.GetFixed());
         }
         else
         {
            thePosStr=theDimUnits.numUser2Str(theDimUnits.numDB2User(scaleRatio,posN),false);
         }
         c=thePosStr;
         int len=thePosStr.GetLength();
         int v;
         if(len>3) v=i+kVOffset2+kVOffset2;
         else if(len>1) v=i+kVOffset2;
         else v=i+kVOffset1;
         pDC->MoveTo(kVOffsetH,kTRulerWid+2+v);
         pDC->TextOut(0,0,c++,1);
         if(len>3)				/* 2nd char below tick */
            pDC->TextOut(0,0,c++,1);
         if(len>1)
            pDC->MoveTo(kVOffsetH,kTRulerWid+1+i+kVOffset1);
         while(*c)
            pDC->TextOut(0,0,c++,1);
         n+=nMarkStep;
      }
      pDC->SetViewportOrg(0,0);
   }
   pDC->SelectClipRgn(pClip);
   pDC->MoveTo(0,kTRulerWid);
   pDC->LineTo(rc.right,kTRulerWid);
   pDC->MoveTo(kLRulerWid,0);
   pDC->LineTo(kTRulerWid,rc.bottom);
}

void CPCDraftView::CalcRulerInfo(int& nRes,WrFixed& nMarkStep,int &nNumMarks,int nMarkLen[])
{
   bool bEnglishUnits=GetDocument()->m_bEnglishUnits;
   int nRealRes=nRes;

   if(bEnglishUnits)
   {
      nMarkStep=FixedMinDelta;	// 1 inch
      if(nRealRes!=kEngDecRes)
      {
         nNumMarks=kNumTicksPerInch;
      }
      else
      {
         /* decimal feet are different */
         nNumMarks=kTicksPerDecFoot;
      }
   }
   else
   {
      nMarkLen[0]=kInchMrkLen;
      nMarkLen[1]=nMarkLen[3]=kQuarterMrkLen;
      nMarkLen[2]=kHalfMrkLen;
      nNumMarks=kNumTicksPer2Cent;
      nMarkStep=FixedMinDeltaTimes2;	// 2 cm
      nRes<<=1;
   }

#if _FLOAT_ZOOM
   CPCDraftDoc* pDoc=GetDocument();
   double fZoomFactor=pDoc->m_fZoomFactor?pDoc->m_fZoomFactor:FloatTheFixedZoomFactor(pDoc->m_nZoomFactor);
   if (fZoomFactor > 0.25)
   {
      if (bEnglishUnits)
      {
         if(nRealRes!=kEngDecRes)
         {
            nMarkLen[0]=kInchMrkLen;
            nMarkLen[1]=kEighthMrkLen;
            nMarkLen[2]=kQuarterMrkLen;
            nMarkLen[3]=kEighthMrkLen;
            nMarkLen[4]=kHalfMrkLen;
            nMarkLen[5]=kEighthMrkLen;
            nMarkLen[6]=kQuarterMrkLen;
            nMarkLen[7]=kEighthMrkLen;
         }
         else
         {
            nMarkLen[0]=kInchMrkLen;
            nMarkLen[1]=kHalfMrkLen;
         }
      }
      if (fZoomFactor > 1.0)
      {
         nRes=int(nRes*fZoomFactor);

         if(!GetDocument()->m_bStandardRulers)
         {
            nMarkStep *= nRealRes;
            nMarkStep <<= 16;
         }
      }
      else
      {
         nRes=int(nRes*fZoomFactor);

         if(!GetDocument()->m_bStandardRulers)
         {
            nMarkStep <<= 16;
            nMarkStep *= nRealRes;
         }
//			if (fZoomFactor == 0.5) nMarkStep<<=1;		/* -2x is the same but change step */
      }
   }
   else
   {
      if(bEnglishUnits)
      {
         if ( fZoomFactor == 0.25 )					/* -4x */
         {
            if(nRealRes!=kEngDecRes)
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=nMarkLen[3]=nMarkLen[5]=nMarkLen[7]=kEighthMrkLen;
               nMarkLen[2]=nMarkLen[4]=nMarkLen[6]=kHalfMrkLen;
            }
            else
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=kHalfMrkLen;
            }
            nMarkStep<<=2;
         }
         else if ( fZoomFactor == 0.125 )			/* -8x */
         {
            if(nRealRes!=kEngDecRes)
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=nMarkLen[2]=nMarkLen[3]=nMarkLen[4]=
                                                   nMarkLen[6]=nMarkLen[7]=nMarkLen[8]=nMarkLen[9]=kEighthMrkLen;
               nMarkLen[5]=kHalfMrkLen;
               nNumMarks=kZoomTicksPerInch;
               nRes=MulDiv(nRes,kZoomTicksPerInch,kNumTicksPerInch);
            }
            else
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=kHalfMrkLen;
            }
            nMarkStep*=short(10);
         }
         else
         {
            // RPH 15/8/2002. On trial
            nRes=int(nRes*fZoomFactor);
            if ( fZoomFactor >= 0.25 )
            {
               nRes *= 2;
            }
            else
            {
               if (nRealRes != kEngDecRes)
               {
                  nMarkStep *= short ( 12 ) ;
                  nRes *= 12 ;
                  nMarkLen[0] = kInchMrkLen;
                  nMarkLen[1] = kHalfMrkLen;
                  nNumMarks = 2;
               }
               else
               {
                  nMarkStep *= short ( 10 ) ;
                  nRes *= 10 ;
                  nMarkLen[0] = kInchMrkLen;
                  nMarkLen[1] = kHalfMrkLen;
                  nNumMarks = 2;
               }
            }
         }
      }
      else
      {
         if ( fZoomFactor == 0.25 )					/* -4x */
         {
            nMarkStep <<= 2;
         }
         else if ( fZoomFactor == 0.125 )					/* -8x */
         {
            nMarkStep *= short ( 10 ) ;
         }
         else
         {
            // RPH 15/8/2002. On trial
            nRes=int(nRes*fZoomFactor);
            if ( fZoomFactor >= 0.25 )
            {
               //nMarkStep <<= 2 ;
               nRes *= 2 ;
            }
            else
            {
               nMarkStep *= short ( 10 ) ;
               nRes *= 10 ;
            }
         }
      }


      if(!GetDocument()->m_bStandardRulers)
      {
         nMarkStep *= nRealRes;
         nMarkStep <<= 16;
      }
   }
#else
   int nZoomFactor=GetDocument()->m_nZoomFactor;
   if(nZoomFactor>=-1)
   {
      if(bEnglishUnits)
      {
         if(nRealRes!=kEngDecRes)
         {
            nMarkLen[0]=kInchMrkLen;
            nMarkLen[1]=kEighthMrkLen;
            nMarkLen[2]=kQuarterMrkLen;
            nMarkLen[3]=kEighthMrkLen;
            nMarkLen[4]=kHalfMrkLen;
            nMarkLen[5]=kEighthMrkLen;
            nMarkLen[6]=kQuarterMrkLen;
            nMarkLen[7]=kEighthMrkLen;
         }
         else
         {
            nMarkLen[0]=kInchMrkLen;
            nMarkLen[1]=kHalfMrkLen;
         }
      }
      if(nZoomFactor>0)
      {
         nRes<<=nZoomFactor;
      }
      if(nZoomFactor==-1)
         nMarkStep*=2;		/* -2x is the same but change step */
   }
   else
   {
      switch(nZoomFactor)
      {
      case -2:			/* -4x */
         if(bEnglishUnits)
         {
            if(nRealRes!=kEngDecRes)
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=nMarkLen[3]=nMarkLen[5]=nMarkLen[7]=kEighthMrkLen;
               nMarkLen[2]=nMarkLen[4]=nMarkLen[6]=kHalfMrkLen;
            }
            else
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=kHalfMrkLen;
            }
         }
         nMarkStep*=4;
         break;
      case -3:			/* -8x */
         if(bEnglishUnits)
         {
            if(nRealRes!=kEngDecRes)
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=nMarkLen[2]=nMarkLen[3]=nMarkLen[4]=
                                                   nMarkLen[6]=nMarkLen[7]=nMarkLen[8]=nMarkLen[9]=kEighthMrkLen;
               nMarkLen[5]=kHalfMrkLen;
               nNumMarks=kZoomTicksPerInch;
               nRes=MulDiv(nRes,kZoomTicksPerInch,kNumTicksPerInch);
            }
            else
            {
               nMarkLen[0]=kInchMrkLen;
               nMarkLen[1]=kHalfMrkLen;
            }
         }
         nMarkStep*=10;
         break;
      }
   }
   if(!GetDocument()->m_bStandardRulers)
   {
      nMarkStep *= nRealRes;
      nMarkStep <<= 16;
   }
#endif
}

void CPCDraftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   CPCDraftDoc* pDoc=GetDocument();
   switch(lHint)
   {
   case kUpdateBars:
      UpdateBars();
      Invalidate(TRUE);
      break;

   case kZoomFactorChanged:
      UpdateZoomInfo();

   case kDocumentSizeChanged:
      CalcDocumentDevSize();
      if(pHint)
         ScrollToDevicePosition(*((CPoint*)pHint));
      break;

   case kActivateLayer:
      DeactivateTextShape();
      m_Selection.RemoveAll();
      SetSnap(((CLayer*)pHint)->m_nSnap);
      Invalidate(TRUE);
      SaveUndo(0);	// discard undo info
      break;

   case kClearSelection:
      m_Selection.RemoveAll();
      break;

   case kDeactivateTextShape:
      DeactivateTextShape();
      break;

   case kRefreshShape:
      InvalidateShape((CMDShape*)pHint,true);
      if(((CPCDraftApp *)AfxGetApp())->m_pLayerFrame != NULL
         && ::IsWindowVisible(((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->GetSafeHwnd()))
      {
         ((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->Refresh();
         ((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->RedrawWindow();

      }
      break;
   case kLockWindowUpdate:
      GetTopWindow()->LockWindowUpdate();
      break;
   case kUnlockWindowUpdate:
      GetTopWindow()->UnlockWindowUpdate();
      break;

   case kRefreshShapes:
   {
      CMDShapeList* pShapes=(CMDShapeList*)pHint;
      POSITION pos=pShapes->GetHeadPosition();
      while(pos)
      {
         InvalidateShape(pShapes->GetNext(pos),true);
      }
   }
   break;

   case kRefresh:
      Invalidate(TRUE);
      m_wndFillInfo.SetFill(pDoc->m_Fill);
      ForceResizeUpdate();
      if(pDoc->GetActiveLayer() != NULL)
      {
         SetSnap(pDoc->GetActiveLayer()->m_nSnap);
      }
      if(((CPCDraftApp *)AfxGetApp())->m_pLayerFrame != NULL
         && ::IsWindowVisible(((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->GetSafeHwnd()))
      {
         ((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->Refresh();
         ((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->RedrawWindow();

      }
      break;
   // Gradient/Transparency - Infinisys Ltd
   case kInvalidateSelection:
      InvalidateSelection();
      break;
   case kApplyLastPackedItem:
   {

      FillModel fm;

      memset(&fm,0,sizeof(fm));

      fm.m_nFillType = kFillPat;

      fm.m_nIndex = GetDocument()->m_packeditema.GetUpperBound();

      fm.m_iFillOpacity = GetDocument()->m_Fill.m_iFillOpacity;

      ApplyFill(fm,false);


   };

//		default:
//			ASSERT(0);
   }
}

void CPCDraftView::CalcDocumentDevSize()
{
   SetScrollSizes(GetDocument()->GetTotalDevSize());
   ConstructGridLines();
   ConstructPageBreaks();
}

#define	kPrevLayer		4
#define	kNextLayer		6
#define	kLayerPopupMenu	2
#define	kFindLayer		0

void CPCDraftView::InvalidateLayerBar()
{
   CRect rc;
   GetClientRect(&rc);
   int cx=rc.right;
   int nCxVScroll=::GetSystemMetrics(SM_CXVSCROLL);
   rc.SetRect(cx-nCxVScroll+1,0,cx,57);
   InvalidateRect(&rc);
}

void CPCDraftView::DrawLayerBar(CDC* pDC)
{
   CRect rc;
   GetClientRect(&rc);
   int cx=rc.right;
   int nCxVScroll=::GetSystemMetrics(SM_CXVSCROLL);
   {
      CGDIBackColor bc(pDC,RGB(255,255,255));
      rc.SetRect(cx-nCxVScroll+1,0,cx,57);
      pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
   }
   {
      CGDIBackColor theBorderColor(pDC,::GetSysColor(COLOR_SCROLLBAR));

   //   rc.SetRect(cx-nCxVScroll,0,cx-nCxVScroll+1,57);
   //   pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);

      rc.SetRect(cx-nCxVScroll+1,20,cx,21);
      pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
      
      rc.SetRect(cx-nCxVScroll+1,37,cx,38);
      pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);

   }

   CImageList* pImage=&((CPCDraftApp*)AfxGetApp())->m_imgLayerBar;
   CPCDraftDoc* pDoc=GetDocument();
   cx=cx-nCxVScroll+1+(nCxVScroll-13)/2;

   pImage->DrawIndirect(pDC,kPrevLayer+(pDoc->m_nActiveLayer==pDoc->GetLastLayerIndex()?1:0), CPoint(cx,0),CSize(13,9), CPoint(0,9));
   pImage->DrawIndirect(pDC,kNextLayer+(pDoc->m_nActiveLayer==pDoc->GetFirstLayerIndex()?1:0), CPoint(cx,10),CSize(13,9), CPoint(0,9));
   bool bOneVisibleLayer=pDoc->GetVisibleLayerCount()==1;
   pImage->DrawIndirect(pDC,kLayerPopupMenu+(bOneVisibleLayer?1:0), CPoint(cx,22),CSize(13,13), CPoint(0,5));
   pImage->DrawIndirect(pDC,kFindLayer+(bOneVisibleLayer?1:0), CPoint(cx,39),CSize(13,18), CPoint(0,0),m_nOneTimeTool==ID_TOOL_SELECT_LAYER?ILD_BLEND50:ILD_NORMAL);
}

/*
	draw a dotted line from [from,to] inclusive with a dot spacing
	specified by spacing
*/

static void dottedLine(CDC* pDC, CPoint from, CPoint to,short spacing,short direct)
{
   int i;
   pDC->SetPixel(from,RGB(0,0,0));
   if(spacing==kOriginalEngDecSkip)	// if skip 9 pixels when English Dec. unit
      spacing=kNewEngDecSkip;			// then skip 6 pixels for drawing 12 dotted points
   if(direct==kHORI)
   {
      i=to.x-from.x;
      do
      {
         from.x+=spacing;
         pDC->SetPixel(from,RGB(0,0,0));
      }
      while((i-=spacing)>=0);
   }
   else
   {
      i=to.y-from.y;
      do
      {
         from.y+=spacing;
         pDC->SetPixel(from,RGB(0,0,0));
      }
      while((i-=spacing)>=0);
   }
}

void CPCDraftView::ConstructGridLines()
{
   if(m_bmpGridHorz.GetSafeHandle())
      m_bmpGridHorz.DeleteObject();
   if(m_bmpGridVert.GetSafeHandle())
      m_bmpGridVert.DeleteObject();
   if(m_bmpGridHorzLarge.GetSafeHandle())
      m_bmpGridHorzLarge.DeleteObject();
   int res=GetDocument()->NoZoomToZoom((long)GetDocument()->m_nDisplayRes);
   short oneToOneRes = GetDocument()->m_nDisplayRes;
   if ( oneToOneRes == kMetRes ) // Double up - otherwise 32x scale fails in metric due to grid being at 2cm intervals
      res *= 2 ;
   m_bmpGridHorz.CreateBitmap(m_totalDev.cx+2*res+1,1,1,1,NULL);
   m_bmpGridVert.CreateBitmap(1,m_totalDev.cy+2*res+1,1,1,NULL);
   m_bmpGridHorzLarge.CreateBitmap(m_totalDev.cx+2*res+1,1,1,1,NULL);
   CWindowDC theWindowDC(this);
   CRect bounds;
   CPoint from,to;

   short mainSkip = res ;
   if ( ( oneToOneRes == kEngRes )  // 72 dpi
         || ( oneToOneRes == kEngDecRes ) // 86 dots per 0.1 ft
         || ( oneToOneRes == kMetRes ) ) // 28 dots per cm
   {
      while (mainSkip > 8 && mainSkip % 2 == 0)	mainSkip /= 2;
   }

// Do the next 3 lines ever do anything ? R.P.H.
   if ( ( oneToOneRes == kEngDecRes )
         && (mainSkip == kOriginalEngDecSkip ) ) /* if skip 9 pixels when English Dec. unit */
      mainSkip	=	kNewEngDecSkip;		/* then skip 6 pixels for drawing 12 dotted points*/

   /******************************************/
   /* horizontal grid constructed			  */
   /******************************************/
   {
      CGDIBitmapDC theBmpDC(&theWindowDC,&m_bmpGridHorz);
      bounds.SetRect(0,0,m_totalDev.cx+2*res+1,1);
      theBmpDC.PatBlt(0,0,bounds.right,bounds.bottom,WHITENESS);
      from=bounds.TopLeft();
      to.y=from.y;
      to.x=bounds.right;
      dottedLine(&theBmpDC,from,to,mainSkip,kHORI);
   }
   /******************************************/
   /* vertical grid constructed			  */
   /******************************************/
   {
      CGDIBitmapDC theBmpDC(&theWindowDC,&m_bmpGridVert);
      bounds.SetRect(0,0,1,m_totalDev.cy+2*res+1);
      theBmpDC.PatBlt(0,0,bounds.right,bounds.bottom,WHITENESS);
      from=bounds.TopLeft();
      to.x=from.x;
      to.y=bounds.bottom;
      dottedLine(&theBmpDC,from,to,mainSkip,kVERT);
   }
   /************************************************/
   /* horizontal large distance grid constructed	*/
   /************************************************/

   short sparseSkip = res ;

   if ( ( oneToOneRes == kEngRes )  // 72 dpi
         || ( oneToOneRes == kEngDecRes ) // 86 dots per 0.1 ft
         || ( oneToOneRes == kMetRes ) ) // 28 dots per cm
   {
      if ( sparseSkip >= mainSkip * 8 )
         sparseSkip /= 4 ; // There are 8 or more dots on main grid lines, can have 4 dots between divisions for sparse grid
      else if ( sparseSkip >= mainSkip * 4 )
         sparseSkip /= 2 ; // There are 4 or more dots on main grid lines, can have 2 dots between divisions for sparse grid
      // else there are too few dots on main grid, omit the sparse grid by leaving sparseSkip at res.h
   }

   CGDIBitmapDC theBmpDC(&theWindowDC,&m_bmpGridHorzLarge);
   bounds.SetRect(0,0,m_totalDev.cx+2*res+1,1);
   theBmpDC.PatBlt(0,0,bounds.right,bounds.bottom,WHITENESS);
   from=bounds.TopLeft();
   to.y=from.y;
   to.x=bounds.right+(sparseSkip*2);
   dottedLine(&theBmpDC,from,to,sparseSkip,kHORI);
}

void CPCDraftView::DrawGridLines(CDC* pDC, CPoint ptOrg)
{
   CSize clientSize;
   GetClientSize(clientSize);
   int nRes=GetDocument()->NoZoomToZoom((long)GetDocument()->m_nDisplayRes);
   /* If units is metric then the distance of between a line			*/
   /*	of Vertical and horizontal is double orginal resource.				*/
   /* For example: kMetRes is 28 in dream 1.1, then drawing among 1".		*/
   /* 				It changed 56 in MacDraft 2.0, then drawing among 2".	*/
   short oneToOneRes = GetDocument()->m_nDisplayRes;
   if( oneToOneRes == kMetRes)
   {
      nRes*=2;
   }
   CPoint ptOffset;
   GetClientCorner(ptOffset);
   int hOrgOff=GetDocument()->LPtoDP(GetDocument()->m_RulerOrigin.x);
   int vOrgOff=GetDocument()->LPtoDP(GetDocument()->m_RulerOrigin.y);
   /**********************************************/
   /* Vertical grid line drawing 				  */
   /**********************************************/
   {
      CGDIBitmapDC theMemDC(pDC,&m_bmpGridVert);
      int start=0;
      if(hOrgOff)
      {
         start-=nRes-(hOrgOff % nRes);
      }
      while(start+nRes<ptOrg.x)
      {
         start+=nRes;
      }
      for(int i=start; i<=m_totalDev.cx; i+=nRes)
      {
         pDC->BitBlt(ptOffset.x+i,ptOffset.y,1,m_totalDev.cy,&theMemDC,0,nRes-vOrgOff%nRes,SRCCOPY);
      }
   }
   /************************************************/
   /* horizontal grid line drawing 			  	*/
   /************************************************/
   {
      CGDIBitmapDC theMemDC(pDC,&m_bmpGridHorz);
      int start=0;
      if(vOrgOff)
      {
         start-=nRes-(vOrgOff % nRes);
      }
      while(start+nRes<ptOrg.y)
      {
         start+=nRes;
      }
      for(int i=start; i<m_totalDev.cy; i+=nRes)
      {
         pDC->BitBlt(ptOffset.x,ptOffset.y+i,m_totalDev.cx+1,1,&theMemDC,nRes-hOrgOff%nRes,0,SRCCOPY);
      }
   }
   /************************************************/
   /* horizontal large spacing grid line drawing 	*/
   /************************************************/
   if( oneToOneRes != kEngDecRes)
   {
#ifdef _FLOAT_ZOOM
      CPCDraftDoc* pDoc=GetDocument();
      if((pDoc->m_fZoomFactor && pDoc->m_fZoomFactor > 0.25) || GetDocument()->m_nZoomFactor > -2)
#else
      if(GetDocument()->m_nZoomFactor > -2)
#endif
      {
         short pitch = nRes ;
         if ( ( oneToOneRes == kEngRes )  // 72 dpi
               || ( oneToOneRes == kEngDecRes ) // 86 dots per 0.1 ft
               || ( oneToOneRes == kMetRes ) ) // 28 dots per cm
         {
            short mainSkip = nRes ;
            while (mainSkip > 8 && mainSkip % 2 == 0)	mainSkip /= 2;
            if ( pitch >= mainSkip * 8 )
               pitch /= 4 ; // There are 8 or more dots on main grid lines, can have 4 dots between divisions for sparse grid
            else if ( pitch >= mainSkip * 4 )
               pitch /= 2 ; // There are 4 or more dots on main grid lines, can have 2 dots between divisions for sparse grid
            // else there are too few dots on main grid, omit the sparse grid by leaving sparseSkip at res.h
         }

         short distanceIndex = nRes / pitch ;	/* the distance among grid line */
         int nStep=nRes / distanceIndex ;
         CGDIBitmapDC theMemDC(pDC,&m_bmpGridHorzLarge);
         int start=0;
         int index=0;
         if(vOrgOff)
         {
            start-=nRes-(vOrgOff % nRes);
         }
         while(start+nStep<ptOrg.y)
         {
            start+=nStep;
            index++;
         }
         for(int i=start; i<m_totalDev.cy; i+=nStep,index++)
            if(index && (index % distanceIndex))
            {
               pDC->BitBlt(ptOffset.x,ptOffset.y+i,m_totalDev.cx+1,1,&theMemDC,nRes-hOrgOff%nRes,0,SRCCOPY);
            }
      }
   }	/* if kEngDecFeet */
}


// Infinisys Ltd. 2020 Guides
void CPCDraftView::DrawGuides(CDC* pDC, CPoint ptOrg)
{

   auto pdocument = GetDocument();

   CRect rectClient;

   GetClientRect(rectClient);

   Gdiplus::Graphics g(pDC->GetSafeHdc());

   g.SetPageUnit(Gdiplus::UnitPixel);

   g.ResetClip();

   Gdiplus::Color color(180, 255, 0, 0);

   Gdiplus::Pen pen(color, 1);

   pDC->SelectObject(&pen);

   CPoint ptOffset;
   GetClientCorner(ptOffset);

   for(int i = 0; i < pdocument->m_guidea.GetSize(); i++)
   {

      auto & guide = pdocument->m_guidea[i];

      bool bHorizontal = !guide.m_bVertical;

      long l = GetDocument()->LPtoDP(guide.m_coord);

      if (bHorizontal)
      {

         l += ptOffset.y;
         g.DrawLine(&pen, (FLOAT) rectClient.left + ptOrg.x + kLRulerWid, (FLOAT)l, (FLOAT)rectClient.right + ptOrg.x, (FLOAT)l);

      }
      else
      {

         l += ptOffset.x;
         g.DrawLine(&pen, (FLOAT)l, (FLOAT)rectClient.top + ptOrg.y + kLRulerWid, (FLOAT)l, (FLOAT)rectClient.bottom + ptOrg.y);

      }

   }

}


void CPCDraftView::ConstructPageBreaks()
{
   if(m_bmpPageHorz.GetSafeHandle())
      m_bmpPageHorz.DeleteObject();
   if(m_bmpPageVert.GetSafeHandle())
      m_bmpPageVert.DeleteObject();
   m_bmpPageHorz.CreateBitmap(m_totalDev.cx+1,1,1,1,NULL);
   m_bmpPageVert.CreateBitmap(1,m_totalDev.cy+1,1,1,NULL);
   CWindowDC theWindowDC(this);
   /******************************************/
   /* horizontal page break constructed	  */
   /******************************************/
   {
      CGDIBitmapDC theBmpDC(&theWindowDC,&m_bmpPageHorz);
      theBmpDC.PatBlt(0,0,m_totalDev.cx+1,1,WHITENESS);
      dottedLine(&theBmpDC, CPoint(0,0), CPoint(m_totalDev.cx+1,0),2,kHORI);
   }
   /******************************************/
   /* vertical page break constructed		  */
   /******************************************/
   {
      CGDIBitmapDC theBmpDC(&theWindowDC,&m_bmpPageVert);
      theBmpDC.PatBlt(0,0,1,m_totalDev.cy+1,WHITENESS);
      dottedLine(&theBmpDC, CPoint(0,0), CPoint(0,m_totalDev.cy+1),2,kVERT);
   }
}

void CPCDraftView::DrawPageBreaks(CDC* pDC, CPoint ptOrg)
{
   CSize theClientSize;
   GetClientSize(theClientSize);
   CRect thePort(ptOrg,theClientSize);
   CPoint theNumPage=GetDocument()->m_NumPage;
   CSize thePageSize=GetDocument()->GetPageDevSize();
   CSize theTotalSize=GetDocument()->GetTotalDevSize();
   int x,y,n;
   CPoint theShift;
   GetClientCorner(theShift);
   {
      CGDIBitmapDC theBmpDC(pDC,&m_bmpPageVert);
      x=0;
      for(n=0; n<=theNumPage.x; n++)
      {
         if(thePort.left<=x && x<=thePort.right && x<=theTotalSize.cx)
            pDC->BitBlt(theShift.x+x,theShift.y,1,m_totalDev.cy+1,&theBmpDC,0,0,SRCCOPY);
         x+=thePageSize.cx;
      }
   }
   {
      CGDIBitmapDC theBmpDC(pDC,&m_bmpPageHorz);
      y=0;
      for(n=0; n<=theNumPage.y; n++)
      {
         if(thePort.top<=y && y<=thePort.bottom && y<theTotalSize.cy)
            pDC->BitBlt(theShift.x,theShift.y+y,m_totalDev.cx+1,1,&theBmpDC,0,0,SRCCOPY);
         y+=thePageSize.cy;
      }
   }
}

void CPCDraftView::GetClientCorner(CPoint& corner)
{
   if(GetDocument()->m_bShowRulers)
   {
      corner.x=kLRulerWid + 1;
      corner.y=kTRulerWid + 1;
   }
   else
   {
      corner.y=0;
      corner.y=0;
   }
}

BOOL CPCDraftView::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

   return DrawBkgnd(pDC);


}


BOOL CPCDraftView::DrawBkgnd(CDC* pDC)
{

   CRect rcClip;
   pDC->GetClipBox(&rcClip);

   CRect rcDoc;
   GetActiveClientRect(rcDoc);

   CRect rc;
   if(rc.IntersectRect(rcDoc,rcClip))
   {
      CGDIBackColor bc(pDC,RGB(255,255,255));
      pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
   }

   CRect rectView;
   GetViewRect(rectView);

   CRect rcClient;

   rcClient = rectView;

   if(rcClient.left < rcDoc.left)
   {

      rcClient.right = rcDoc.left - 1;

      if(rc.IntersectRect(rcClient,rcClip))
      {

         CGDIBackColor bc(pDC,::GetSysColor(COLOR_3DLIGHT));

         pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);

      }
   }

   rcClient = rectView;

   if(rcClient.top < rcDoc.top)
   {

      rcClient.left = max(rcClient.left, rcDoc.left - 1);

      rcClient.right = min(rcClient.right, rcDoc.right + 1);

      rcClient.bottom = rcDoc.top;

      if(rc.IntersectRect(rcClient,rcClip))
      {

         CGDIBackColor bc(pDC,::GetSysColor(COLOR_3DLIGHT));

         pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);

      }

   }

   rcClient = rectView;

   if(rcDoc.right < rcClient.right)
   {

      rcClient.left = rcDoc.right + 1;

      if(rc.IntersectRect(rcClient,rcClip))
      {

         CGDIBackColor bc(pDC,::GetSysColor(COLOR_3DLIGHT));

         pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);

      }

   }

   rcClient = rectView;

   if(rcDoc.bottom < rcClient.bottom)
   {

      rcClient.left = max(rcClient.left, rcDoc.left - 1);

      rcClient.right = min(rcClient.right, rcDoc.right + 1);

      rcClient.top=rcDoc.bottom;

      if(rc.IntersectRect(rcClient,rcClip))
      {

         CGDIBackColor bc(pDC,::GetSysColor(COLOR_3DLIGHT));

         pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);

      }

   }

   CGDIBackColor bc(pDC,RGB(255,255,255));

   COLORREF cr1 = ::GetSysColor(COLOR_3DLIGHT);

   rcDoc.right++;
   rcDoc.bottom++;

   double d = 0.90;
   double dInc = 0.033;

   for(int i = 0; i < 4; i++,d+=dInc)
   {

      cr1 = ::GetSysColor(COLOR_3DLIGHT);

      cr1 = RGB((BYTE)(GetRValue(cr1) * d),(BYTE)(GetGValue(cr1) * d),(BYTE)(GetBValue(cr1) * d));
      rcDoc.InflateRect(1,1);
      pDC->Draw3dRect(rcDoc,cr1,cr1);

   }

   return TRUE;

}


bool CPCDraftView::SelectShapeFromPoint(CPoint inPoint, bool bAddToList,CMDShape* pStart)
{
   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pLayer=pDoc->GetActiveLayer();
   if(pLayer == NULL)
   {
      return false;
   }
   CMDShape* pShape=pLayer->ShapeFromPoint(inPoint,&m_Mapper,pDoc->m_bHideText,false,pStart);
   if(pShape)
   {
      SelectShape(pShape,bAddToList);
      UpdateWindow();
   }
   return pShape!=NULL;
   /*	pDoc->UpdateAllViews(NULL,kRefresh);*/
}

bool CPCDraftView::SelectShapesInRect(const LongRect& lrc, bool bInclusive, bool bAddToList)
{
   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pLayer=pDoc->GetActiveLayer();
   POSITION pos=pLayer->m_ShapeList.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
      if(pDoc->m_bHideText && pShape->IsKindOf(RUNTIME_CLASS(CMDText))) continue;
      LongRect lrcShape=pShape->GetExtents();
      if(bInclusive)
      {
         if((lrc & lrcShape)!=LongRect::kZeroRect)
            SelectShape(pShape,bAddToList);
      }
      else
      {
         if((lrc & lrcShape)==lrcShape)
            SelectShape(pShape,bAddToList);
      }
      bAddToList=true;
   }
   return true;
}

bool CPCDraftView::LayerBarDoClick(int nIndex,CRect &rc,CSize sz,CPoint ptOrigin)
{
   CImageList* pImage=&((CPCDraftApp*)AfxGetApp())->m_imgLayerBar;
   CClientDC dc(this);
   int nCxVScroll=::GetSystemMetrics(SM_CXVSCROLL);
   CPoint pt(rc.left+(nCxVScroll-sz.cx)/2,rc.top);
   SetCapture();
   pImage->DrawIndirect(&dc,nIndex,pt,sz,ptOrigin,ILD_SELECTED);
   bool bSelected=true;
   MSG msg;

   while(1)
   {
      GetMessage(&msg,NULL,0,0);
      ScreenToClient(&msg.pt);
      switch(msg.message)
      {
      case WM_MOUSEMOVE:
         if(rc.PtInRect(msg.pt))
         {
            if(!bSelected)
            {
               pImage->DrawIndirect(&dc,nIndex,pt,sz,ptOrigin,ILD_SELECTED);
               bSelected=true;
            }
         }
         else if(bSelected)
         {
            pImage->DrawIndirect(&dc,nIndex,pt,sz,ptOrigin,ILD_NORMAL);
            bSelected=false;
         }

         break;
      case WM_LBUTTONUP:
         ReleaseCapture();
         if(bSelected)
            pImage->DrawIndirect(&dc,nIndex,pt,sz,ptOrigin,ILD_NORMAL);
         return bSelected;
         break;
      }
   }
   return false;
}

static Angle savedTextAngle;
static LongPoint lptRotCenter;

void CPCDraftView::ExportImage()
{

#ifndef _DEMO

   CString strPath;

   CString strFilter;

   strFilter = "PNG file (*.png)|*.png|JPEG file (*.jpg)|*.jpg|Bitmap file (*.bmp)|*.bmp||";

   CString strTitle;

   strTitle = GetDocument()->GetTitle();

repeat:

   CFileDialog filedlg(FALSE, ".png", strTitle + ".png", OFN_OVERWRITEPROMPT, strFilter, AfxGetApp()->m_pMainWnd);
   TCHAR pathPicture[MAX_PATH];
   memset(pathPicture, 0, sizeof(pathPicture));
   HRESULT hr = SHGetFolderPath(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, pathPicture);

   CString sInitialDirectory = AfxGetApp()->GetProfileString("Settings", "ImageExportFolder", pathPicture);
   // Set sInitialDirectory to whatever you want


   filedlg.m_ofn.lpstrInitialDir = sInitialDirectory;

   if (filedlg.DoModal() != IDOK)
   {

      return;

   }

   strPath = filedlg.GetPathName();

   sInitialDirectory = get_folder_from_path(strPath);

   AfxGetApp()->WriteProfileString("Settings", "ImageExportFolder", sInitialDirectory);

   //LongRect lrcExtents = GetGetCumulativeExtents(false);
   //lrcExtents.Inset(-3, -3);

   //CMapper mapper;
   //CRect rcExtents = mapper.LPtoDP(lrcExtents);
   //rcExtents.right++;
   //rcExtents.bottom++;

   //CRect rcBound(0, 0, Width(rcExtents), Height(rcExtents));

   //CBitmap bitmap;

   //bitmap.Attach(CreateDIB(rcBound.Width(), rcBound.Height(), NULL, 0));

   //int iMargin = 5;

   //{

   //   CDC dcMem;

   //   dcMem.CreateCompatibleDC(NULL);

   //   CBitmap * pOld = dcMem.SelectObject(&bitmap);

   //   dcMem.SetMapMode(MM_TEXT);

   //   dcMem.OffsetViewportOrg(iMargin / 2, iMargin / 2);

   //   CDC* pDC = &dcMem;
   //   pDC->SetViewportOrg(-rcExtents.left, -rcExtents.top);

   //   CRgn theClipRgn;
   //   theClipRgn.CreateRectRgnIndirect(&rcExtents);
   //   theClipRgn.OffsetRgn(-rcExtents.left, -rcExtents.top);
   //   pDC->SelectClipRgn(&theClipRgn);
   //   pDC->FillRect(&rcExtents, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
   //   CDraftContext dc(pDC, &mapper, this);
   //   CPalette* pPalette = pDC->SelectPalette(&m_Palette, FALSE);
   //   pDC->RealizePalette();
   //   for (int i = 0; i < m_Layers.GetSize(); i++) GetLayer(i)->Draw(&dc, true, m_bHideText);
   //   pDC->SelectPalette(pPalette, FALSE);

   //   GdiFlush();


   //}


   //Gdiplus::Bitmap b((HBITMAP)bitmap.GetSafeHandle(), NULL);

   auto pbitmap = BitmapFromDrawing();


   if (strPath.Right(4) == ".png")
   {

      SavePNG(strPath, pbitmap);

   }
   else if (strPath.Right(5) == ".jpeg" || strPath.Right(4) == ".jpg")
   {

      SaveJPEG(strPath, pbitmap);

   }
   else if (strPath.Right(4) == ".bmp") // Infinisys Ltd. 2021
   {

      SaveBMP(strPath, pbitmap); // Infinisys Ltd. 2021

   }
   else
   {


      delete pbitmap;

      AfxMessageBox("Missing file extension. Unknown file type. Please choose either PNG or JPEG.");

      goto repeat;

   }

   delete pbitmap;

#endif
}


void CPCDraftView::ActivateTextShape(CMDText* pTextShape)
{

   SaveUndo(0);
   m_InplaceEdit.ClearUndo();
   ASSERT(m_pActiveTextShape==NULL);
   m_pActiveTextShape=pTextShape;
   UpdateWindow();
   m_pbitmapDrawBackground.reset(BitmapFromView());
   savedTextAngle=m_pActiveTextShape->GetRotation();
   if(savedTextAngle!=0)
   {
      lptRotCenter=Middle(m_pActiveTextShape->GetExtents());
      m_pActiveTextShape->DoShapeRotate(-savedTextAngle,kAboutRefPt,lptRotCenter);
   }

   CRect rc(m_Mapper.LPtoDP(m_pActiveTextShape->GetShapePoint(0)+LongPoint(3,3)),m_Mapper.LPtoDP(m_pActiveTextShape->GetShapePoint(2)-LongPoint(3,3)));
   if(pTextShape->GetTextType()==CMDText::kTextUserSize) m_InplaceEdit.SetBoundRect(rc);
   else
   {
      CPoint pt = rc.TopLeft();
      switch (m_pActiveTextShape->GetTextParams().m_nJust)
      {
      case kAlignLeft: pt.x += 0;
         break;
      case kAlignCenter: pt.x += rc.Width() / 2;
         break;
      case kAlignRight: pt.x += rc.Width();
         break;
      }
      m_InplaceEdit.SetAutoAdjustBoundRect(pt);
   }

   m_InplaceEdit.SetTextParams(m_pActiveTextShape->GetTextParams());
   m_InplaceEdit.GetTextParams(GetDocument()->m_TextParams);
   m_InplaceEdit.SetStyledText(m_pActiveTextShape->GetText());
   m_InplaceEdit.SetSel(0,0,0);
   PenModel pm=m_pActiveTextShape->GetPenModel();
   if(pm.m_nPenType==kPenColor)m_InplaceEdit.SetDefColor(pm.m_crColor);
   FillModel fm=m_pActiveTextShape->GetFillModel();
   m_InplaceEdit.SetBackColor(fm.m_nFillType==kFillColor?fm.m_crColor:((fm.m_nFillType!=kNoFill|| pm.m_nPenType!=kPenColor || pm.m_crColor!=PALETTEINDEX(0))?RGB(255,255,255):RGB(0,0,0)));
   m_InplaceEdit.SetWindow(this);
   m_InplaceEdit.SetMapper(&m_Mapper);
   m_InplaceEdit.SetOrg(GetDeviceScrollPosition());
   m_bEditActive=true;
   m_InplaceEdit.Redraw(false);
   on_text_composition_message(TEXT_COMPOSITION_MESSAGE_UPDATE_CANDIDATE_WINDOW_POSITION);
}

void CPCDraftView::DeactivateTextShape()
{

   if (m_bEditActive) //m_pActiveTextShape)
   {

      m_pbitmapDrawBackground.release();
      m_pbitmapDraw1.release();
      m_InplaceEdit.OnKillFocus();
      imm_context imm(this);
      imm.clear_composition();
      CStyledText theText;
      m_InplaceEdit.GetStyledText(theText);
      if(theText.IsEmpty()) DoEditClear();
      else
      {
         if(!(theText==m_pActiveTextShape->GetText()))
         {
            InvalidateShape(m_pActiveTextShape,true);
            m_pActiveTextShape->SetText(theText);
            LongRect lrc;
            if(m_pActiveTextShape->GetTextType()==CMDText::kTextAutoSize)
            {
               m_InplaceEdit.CalcTrueBoundRect(lrc);
            }
            else
            {
               CRect rc;
               m_InplaceEdit.GetBoundRect(rc);
               lrc=m_Mapper.DPtoLP(rc);
            }
            lrc.Inset(-3,-3);
            for(int i =0; i<4; i++) m_pActiveTextShape->SetShapePoint(i,lrc.GetCorner(i));

            //TextParams tp;
            //m_InplaceEdit.GetTextParams(tp);
            //m_pActiveTextShape->SetTextParams(tp);
            m_pActiveTextShape->CalculateExtents();
            GetDocument()->SetModifiedFlag();

         }

         InvalidateShape(m_pActiveTextShape,true);
         if(savedTextAngle!=0)
            m_pActiveTextShape->DoShapeRotate(savedTextAngle,kAboutRefPt,lptRotCenter);
         InvalidateShape(m_pActiveTextShape,true);
      }
      m_pActiveTextShape=NULL;
      m_bEditActive=false;
   }
}

bool CPCDraftView::InRulers(CPoint point)
{
   CRect corner,top,left;
   CSize size;
   GetClientSize(size);
   corner.SetRect(0,0,kLRulerWid,kTRulerWid);
   top.SetRect(kLRulerWid,0,size.cx+kLRulerWid,kTRulerWid);
   left.SetRect(0,kLRulerWid,kLRulerWid,size.cy+kTRulerWid);
   LongPoint oldOrigin=GetDocument()->m_RulerOrigin;
   LongPoint newOrigin;
   if(corner.PtInRect(point))
   {
      COriginSketcher os;
      if(os.DoSketch(this,point))
      {
         CPoint p;
         GetCursorPos(&p);
         point = p;
         ScreenToClient(&point);
         if(corner.PtInRect(point))
            newOrigin.Clear();
         else
         {
            /*				CPoint origin=GetDeviceScrollPosition();
            				origin.x+=point.x;
            				origin.y+=point.y;
            				if(origin.y<0)
            					origin.y=0;
            				else if(origin.y>m_totalDev.cy)
            					origin.y=m_totalDev.cy;
            				if(origin.x<0)
            					origin.x=0;
            				else if(origin.x>m_totalDev.cx)
            					origin.x=m_totalDev.cx;
            				newOrigin=GetDocument()->DPtoLP(origin);
            				SnapToGrid(newOrigin);
            */				newOrigin=os.m_ptOrigin; // already snapped inside sketcher
         }
      }
      else
         newOrigin=oldOrigin;
   }
   else if(top.PtInRect(point))	// top ruler
   {

      if (GetAsyncKeyState(VK_MENU) < 0)
      {

         CRect rcClient;

         GetActiveClientRect(rcClient);

         BOOL bSketchResult = CGuideSketcher(false).DoSketch(this, point, nullptr, CSketcher::NoSnapToDocument);

      }
      else if(GetAsyncKeyState(VK_COMMAND)<0)
         newOrigin.x=FixedZero;
      else
      {
         CPoint origin=GetDeviceScrollPosition();
         origin.x+=point.x;
         newOrigin=GetDocument()->DPtoLP(origin);
         SnapToGrid(newOrigin);
      }
      newOrigin.y=oldOrigin.y;
   }
   else if(left.PtInRect(point))	// left ruler
   {

      if (GetAsyncKeyState(VK_MENU) < 0)
      {

         CRect rcClient;

         GetActiveClientRect(rcClient);

         BOOL bSketchResult = CGuideSketcher(true).DoSketch(this, point, nullptr, CSketcher::NoSnapToDocument);

      }
      else if(GetAsyncKeyState(VK_COMMAND)<0)
         newOrigin.y=FixedZero;
      else
      {
         CPoint origin=GetDeviceScrollPosition();
         origin.y+=point.y;
         newOrigin=GetDocument()->DPtoLP(origin);
         SnapToGrid(newOrigin);
      }
      newOrigin.x=oldOrigin.x;
   }
   else
      return false;
   if(oldOrigin!=newOrigin)
   {
      GetDocument()->m_RulerOrigin=newOrigin;
      GetDocument()->SetModifiedFlag();
      Invalidate();
      UpdateWindow();
   }
   return true;
}

void CPCDraftView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   CPoint pt=point+GetDeviceScrollPosition();

   if(m_bEditActive)
   {
      CRect rc;
      m_InplaceEdit.GetBoundRect(rc);
      if(rc.PtInRect(pt))
      {
         m_InplaceEdit.DoDblClick(pt);
         return;
      }
   }
}

void CPCDraftView::OnLButtonDown(UINT nFlags, CPoint point)
{

   CPoint pt=point+GetDeviceScrollPosition();
   CPCDraftDoc* pDoc=GetDocument();

   if (GetDocument()->m_bSnapToGuides && GetAsyncKeyState(VK_COMMAND) < 0 && GetAsyncKeyState(VK_MENU) < 0)
   {
      LongPoint lptNew = m_Mapper.DPtoLP(pt);
      auto lptTest = lptNew;
      int iGuide = -1;
      bool bSnap = SnapToGuides(lptTest, &iGuide);

      if (bSnap && iGuide >= 0)
      {
         auto guide = GetDocument()->m_guidea[iGuide];
         GetDocument()->m_guidea.RemoveAt(iGuide);
         RedrawWindow();

         CGuideSketcher(guide.m_bVertical, guide.m_coord).DoSketch(this, pt, nullptr, CSketcher::NoSnapToDocument);

         return;

      }

   }


   if(m_bEditActive)
   {
      CRect rc;
      m_InplaceEdit.GetBoundRect(rc);
      if(rc.PtInRect(pt))
      {
         m_InplaceEdit.DoClick(pt);
         return;
      }
   }
   DeactivateTextShape();

   if(pDoc->m_bShowRulers && InRulers(point))
      return;
#ifndef _PERSONAL_EDITION
   {
      CRect rc;
      GetClientRect(&rc);
      int cx=rc.right;
      int nCxVScroll=::GetSystemMetrics(SM_CXVSCROLL);
      if(point.y<57 && point.x>cx-nCxVScroll)
      {
         rc.left=rc.right-nCxVScroll+1;
         if(point.y<10)
         {
            if(pDoc->m_nActiveLayer==pDoc->GetLastLayerIndex())
               return;
            rc.top=0;
            rc.bottom=9;
            if(LayerBarDoClick(kPrevLayer,rc,CSize(13,9), CPoint(0,9)))
            {
               pDoc->ActivateNextLayer();
               pDoc->LayersChanged();
            }
            return;
         }
         if(point.y<20)
         {
            if(pDoc->m_nActiveLayer==pDoc->GetFirstLayerIndex())
               return;
            rc.top=10;
            rc.bottom=19;
            if(LayerBarDoClick(kNextLayer,rc,CSize(13,9), CPoint(0,9)))
            {
               pDoc->ActivatePrevLayer();
               pDoc->LayersChanged();
            }
            return;
         }
         if(pDoc->GetVisibleLayerCount()==1)
            return;
         if(point.y<38)
         {
            rc.top=22;
            rc.bottom=35;
            {
               CImageList* pImage=&((CPCDraftApp*)AfxGetApp())->m_imgLayerBar;
               CClientDC dc(this);
               CPoint pt(rc.left+(nCxVScroll-13)/2,rc.top);
               pImage->DrawIndirect(&dc,kLayerPopupMenu,pt,CSize(13,13), CPoint(0,5),ILD_SELECTED);
            }
//				if(LayerBarDoClick(kLayerPopupMenu,rc,CSize(13,13),CPoint(0,5)))
            {
               CMenu theMenu;
               theMenu.CreatePopupMenu();
               for(int i=0; i<pDoc->m_Layers.GetSize(); i++)
               {
                  if(!pDoc->m_Layers[i]->m_bHidden)
                  {
                     CString strName;
                     strName = get_cp(get_uni(pDoc->m_Layers[i]->m_strName,CP_UTF8),CP_ACP);
                     theMenu.AppendMenu(MF_STRING | (pDoc->m_nActiveLayer == i?MF_CHECKED:0) | (i % 20 || 0 == i?0:MF_MENUBARBREAK),1 + i,strName);
                  }
               }
               CPoint pt(rc.left,21);
               ClientToScreen(&pt);
               UINT nID=theMenu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD|TPM_NONOTIFY,pt.x,pt.y,this);
               if(nID)
               {
                  pDoc->SetActiveLayer(nID-1);
                  pDoc->LayersChanged();
               }
               theMenu.DestroyMenu();
            }
            InvalidateRect(&rc);
            return;
         }
         rc.top=39;
         rc.bottom=57;
         if(LayerBarDoClick(kFindLayer,rc,CSize(13,18), CPoint(0,0)))
         {
            if(m_nOneTimeTool==ID_TOOL_SELECT_LAYER) CancelCurrentTool();
            else SetOneTimeTool(ID_TOOL_SELECT_LAYER);
         }
         return;
      }
   }
#endif
   CRect rcClient;
   GetActiveClientRect(rcClient);

   if(!rcClient.PtInRect(point)) return;

   if(m_nOneTimeTool)
   {
      switch(m_nOneTimeTool)
      {
      case ID_TOOL_SELECT_HANDLE:
         if(m_pResizeShape)
         {
            m_nResizeShapeHandle=m_pResizeShape->TestResizeHandles(pt,&m_Mapper);
            if(m_nResizeShapeHandle>=0)
            {
               const ResizeInfo* pInfo=m_pResizeShape->GetResizeInfo();
               if(pInfo->nTitle==IDS_EDIT_COORD)
               {
                  m_pResizeShape->GetResizeValue(m_ResizeValue,m_nResizeShapeHandle);
               }
            }
            SetArrowTool();
            return;
         }
         break;

      case ID_TOOL_SELECT_LAYER:
      {
         for(int i=0; i<pDoc->GetLayersCount(); i++)
         {
            CLayer* pLayer=pDoc->GetLayer(i);
            if(!pLayer->m_bHidden && pLayer->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText))
            {
               pDoc->SetActiveLayer(i);
               pDoc->LayersChanged();
            }
         }
      }
      break;

      case ID_TOOL_SELECT_OBJECT:
      {
         CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText);
         if(pShape)
         {
            m_pShapeSelectedNotifyTarget->ShapeSelected(pShape);
         }
         else return;
      }
      break;

      case ID_TOOL_MIRROR:
      {
         CRefLineSketcher refLine;
         if(refLine.DoSketch(this,pt,&rcClient))
         {
            SaveUndo(IDS_MIRROR);
            UnselectShapes(usLocked);
            POSITION pos=m_Selection.GetHeadPosition();
            while(pos)
            {
               CMDShape* pShape=m_Selection.GetNext(pos);
               if(((CPCDraftApp*)AfxGetApp())->m_nMirrorMethod)
               {
                  pShape=pShape->CreateCopy();
                  pDoc->CreateShape(pShape);
               }
               else InvalidateShape(pShape,true);
               pShape->DoShapeMirror(refLine.m_ptFrom,refLine.m_ptTo);
               pShape->ReCalc(&m_Mapper);
               pShape->UpdateDependentShapes(this);
               InvalidateShape(pShape,true);
               RecalcLinkedVertex(pShape);
            }
            pDoc->SetModifiedFlag();
            ForceResizeUpdate();
         }
      }
      break;

      case ID_TOOL_EXTEND:
      {
         CEdge* pEdge=NULL;
         if(m_nExtendTo)
         {
            CRefLineSketcher refLine;
            if(refLine.DoSketch(this,pt,&rcClient))
            {
               pEdge=new CEdge(refLine.m_ptFrom,refLine.m_ptTo);
            }
         }
         else
         {
            CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
            if(pShape)
            {
               pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
               if(pEdge && pEdge->m_nType==CEdge::kLineEdge)
               {
               }
               else return;
            }
            else return;
         }
         if(pEdge)
         {
            BeginSaveUndo();
            POSITION pos=m_Selection.GetHeadPosition();
            while(pos)
            {
               CMDShape* pLine=m_Selection.GetNext(pos);
               if(pLine->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape))
               {
                  LongPoint lpt1,lpt2,lipt;
                  lpt1=pLine->GetShapePoint(0);
                  lpt2=pLine->GetShapePoint(1);
                  if(guLongLnLnPt(lpt1,lpt2,pEdge->m_lptFrom,pEdge->m_lptTo,&lipt))
                  {
                     LongRect r=guPt2Rect(lpt1,lpt2);
                     r.Inset(-FixedMinDelta,-FixedMinDelta);
                     if(!r.Contains(lipt))
                     {
                        double dist1 = RawDoublePythagSqr(lpt1-lipt);
                        double dist2 = RawDoublePythagSqr(lpt2-lipt);
                        InvalidateShape(pLine,true);
                        if(dist1>dist2) pLine->SetShapePoint(1,lipt);
                        else pLine->SetShapePoint(0,lipt);
                        pLine->CalculateExtents();
                        InvalidateShape(pLine,true);
                     }
                  }
               }
            }
            delete pEdge;
            UINT strId;
            if(RecalcLinkedVertexes(false))
            {
               if(pDoc->CheckDocSize(false))
               {
                  CommitSaveUndo(IDS_EXTEND);
                  ForceResizeUpdate();
                  Invalidate(FALSE);
                  CancelCurrentTool();
                  return;
               }
               strId=IDS_CAN_NOT_EXTEND;
            }
            else strId=IDS_UNABLE_COMPLETE_ACTION;
            AbortSaveUndo(true);
            AfxMessageBox(strId);

         }
      }
      break;

      case ID_TOOL_PAN:
         DoPan();
         break;

      case ID_TOOL_DISTRIBUTE:
      {
         BeginSaveUndo();
         UnselectShapes(usLocked/*|usDimStr*/);
         SetSnapMode(kSnap);
         if(CDistributeSketcher(&m_Selection,GetAsyncKeyState(VK_OPTION)<0).DoSketch(this,pt,rcClient))
         {
            if(RecalcLinkedVertexes(false))
            {
               CommitSaveUndo(IDS_DISTRIBUTEONLINE);
               ForceResizeUpdate();
            }
            else
            {
               AbortSaveUndo(true);
               AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
            }
         }
      }
      break;

      case ID_TOOL_SELECT_REF_OBJECT:
      {
         CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,GetAsyncKeyState(VK_OPTION)<0);
         if(pShape)
         {
            LongRect ext=pShape->GetExtents();
            LongPoint ptCenter=Middle(ext);
            LongPoint refPt;
            refPt.x=FixedMin;
            refPt.y=FixedMin;
            if(gRefMask & alignMask_Datum) refPt=pShape->GetShapeDatum();
            else
            {
               if((gRefMask & alignMask_HorizCenters) == alignMask_HorizCenters) refPt.x=ptCenter.x;
               else
               {
                  if(gRefMask & alignMask_Left) refPt.x=ext.left;
                  else if(gRefMask & alignMask_Right) refPt.x=ext.right;
               }
               if((gRefMask & alignMask_VertCenters) == alignMask_VertCenters) refPt.y=ptCenter.y;
               else
               {
                  if(gRefMask & alignMask_Top) refPt.y=ext.top;
                  else if(gRefMask & alignMask_Bottom) refPt.y=ext.bottom;
               }
            }
            DoAlign(&refPt);
//					SetCursorPrompt(0);
         }
         else return;
      }
      break;

      case ID_TOOL_SELECT_REF_POINT:
      {
         LongPoint refPt=m_Mapper.DPtoLP(pt);
         if(GetAsyncKeyState(VK_COMMAND)>=0) SnapLPt(refPt);
         DoAlign(&refPt);
//				SetCursorPrompt(0);
      }
      break;

      case ID_TOOL_FILLET:
      {
         int nFillet=m_pFilletCtx->TestFillet(m_Mapper.DPtoLP(pt));
         if(nFillet<0) return;
         FinishCreateFillet(nFillet);
      }
      break;

      case ID_TOOL_CHAMFER:
      {
         CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText);
         if(pShape && pShape->IsKindOf(RUNTIME_CLASS(CMDLineShape)))
         {
            SelectShape(pShape,true);
            if(m_Selection.GetCount()==1) return;
            OnToolChamfer();
         }
         else return;
      }
      break;

      case ID_TOOL_DIM_NEXT_POINT:
         if(m_pLineDimSketcher->DoSketch(this,pt,&rcClient) && m_nDimType)
         {
            SetOneTimeTool(ID_TOOL_DIM_NEXT_POINT,IDS_NEXTPOINT,kSnap);
            return;
         }
         else
         {
            delete m_pLineDimSketcher;
         }
         break;
      case ID_TOOL_UNLINK:
      case ID_TOOL_LINK:
         SaveUndo(0);
         LinkVertexes(pt,m_nOneTimeTool==ID_TOOL_LINK,true);
         break;
      }
      CancelCurrentTool();
      return;
   }

   m_ptPaste=m_ptCursorPos+GetDeviceScrollPosition();

   bool bDrag = false;
   bool bSketchResult=true;
   switch(m_nCurrentTool)
   {
   case ID_TOOL_ARROW:
   {
#ifndef _PERSONAL_EDITION
      if(GetAsyncKeyState(VK_COMMAND)<0 && GetAsyncKeyState(VK_OPTION)<0 && SelectLayerFromPoint(pt)) break;
#endif
      POSITION pos=m_Selection.GetTailPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetPrev(pos);

         if(GetKeyState(VK_MENU) & 0x8000)
         {
            if(pShape->TestSelect(pt,&m_Mapper))
            {
               bDrag = true;
            }
            continue;
         }


         int nHandle=pShape->TestHandles(pt,&m_Mapper);
         if(nHandle!=-1)
         {
            if(pShape->IsLocked() || (pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && IsLinkedShape(pShape,nHandle))) return;
            if(GetAsyncKeyState(VK_OPTION)<0 && GetAsyncKeyState(VK_SHIFT)>=0)
            {
               BeginSaveUndo();
               if(DoShapesMove(pt))
               {
                  if(RecalcLinkedVertexes())
                  {
                     CommitSaveUndo(IDS_MOVE);
                     ForceResizeUpdate();
                     pDoc->SetModifiedFlag();
                  }
                  else
                  {
                     AbortSaveUndo(true);
                     AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
                  }
               }
               else AbortSaveUndo(false);
            }
            else
            {
               BeginSaveUndo();
               if(pShape->DoTrackShape(nHandle,pt,this,rcClient))
               {
                  if(RecalcLinkedVertex(pShape))
                  {
                     CommitSaveUndo(IDS_RESIZE);
                     ForceResizeUpdate();
                     pDoc->SetModifiedFlag();
                  }
                  else
                  {
                     AbortSaveUndo(true);
                     AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
                  }
               }
               else AbortSaveUndo(false);
            }
            return;
         }
         else if(pShape->TestSelect(pt,&m_Mapper))
         {
            BeginSaveUndo();
            bool bSingle=GetAsyncKeyState(VK_SHIFT)<0;
            if(bSingle)
            {
               m_Selection.RemoveAt(m_Selection.Find(pShape));
               m_Selection.AddTail(pShape);
            }
            if(!pShape->IsLocked() && DoShapesMove(pt,bSingle))
            {
               if(bSingle?RecalcLinkedVertex(pShape):RecalcLinkedVertexes())
               {
                  CommitSaveUndo(IDS_MOVE);
                  pDoc->SetModifiedFlag();
                  ForceResizeUpdate();
               }
               else
               {
                  AbortSaveUndo(true);
                  AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
               }
            }
            else
            {
               AbortSaveUndo(false);
#ifndef _PERSONAL_EDITION
               if(GetAsyncKeyState(VK_OPTION)<0)
               {
                  CObjectInfoDlg theDlg(pDoc,&m_Selection);
                  if(theDlg.DoModal()==IDOK) pDoc->SetModifiedFlag();
                  return;
               }
#endif
               CLayer* pLayer=pDoc->GetActiveLayer();
               CMDShape* pOtherShape=pLayer->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText);
               if(pOtherShape && pOtherShape!=pShape)
                  SelectShape(pOtherShape,(nFlags & MK_SHIFT)?true:false);
               else if(nFlags & MK_SHIFT) UnselectShape(pShape);
               else
               {
                  pOtherShape=pLayer->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,false,pShape);
                  if(pOtherShape && pOtherShape!=pShape) SelectShape(pOtherShape,false);
                  else if(pShape->IsKindOf(RUNTIME_CLASS(CMDText)) && !pShape->IsLocked())
                  {
                     OnToolsRange(ID_TOOL_TEXT);

                     SelectNone();
                     SelectShape(pShape,true);
                     ActivateTextShape((CMDText*)pShape);
//								m_InplaceEdit.DoClick(pt);
//								ActivateTextShape((CMDText*)pShape);
                     m_InplaceEdit.MouseActivate(pt);
                  }
               }
            }
            return;
         }
      }
   }

   if(bDrag)
   {

      CLibraryItem * pitemSel = LibraryItemFromSelection();

      if(pitemSel != NULL)
      {

         m_dragdrop.StartDragging(pitemSel,&point);

      }

      return ;

   }


   if(!SelectShapeFromPoint(pt,(nFlags & MK_SHIFT)?true:false)) DoNetSelect(pt,GetAsyncKeyState(VK_OPTION)<0,true);
   else
   {
      bool bSingle=false;
      if(m_Selection.GetCount()==2)
      {
         CMDArea* pArea=(CMDArea*)m_Selection.GetHead();
         if(pArea->IsKindOf(RUNTIME_CLASS(CMDArea)) && pArea->GetOwner()==m_Selection.GetTail())
         {
            bSingle=GetAsyncKeyState(VK_SHIFT)<0;
         }
         else
         {
            pArea=(CMDArea*)m_Selection.GetTail();
            if(pArea->IsKindOf(RUNTIME_CLASS(CMDArea)) && pArea->GetOwner()==m_Selection.GetHead())
            {
               bSingle=GetAsyncKeyState(VK_SHIFT)<0;
            }
         }
      }

      BeginSaveUndo();
      if(DoShapesMove(pt,bSingle))
      {
         if(RecalcLinkedVertexes())
         {
            CommitSaveUndo(IDS_MOVE);
            ForceResizeUpdate();
//					Invalidate(true);
         }
         else
         {
            AbortSaveUndo(true);
            AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
         }
      }
      else AbortSaveUndo(false);
   }
   break;


   case ID_TOOL_MARQUEE:
   case ID_TOOL_MARQUEE2:
      DoNetSelect(pt,m_nCurrentTool!=ID_TOOL_MARQUEE,false);
      break;

   case ID_TOOL_PAN:
      DoPan();
      break;

   case ID_TOOL_TEXT:
   {
      if(pDoc->m_bHideText) bSketchResult=false;
      else
      {
         CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper);
         if(pShape && pShape->IsKindOf(RUNTIME_CLASS(CMDText)) && !pShape->IsLocked())
         {
            SelectNone();
            SelectShape(pShape,true);
            ActivateTextShape((CMDText*)pShape);
            m_InplaceEdit.DoClick(pt);
         }
         else
         {
            bSketchResult=CTextSketcher().DoSketch(this,pt,&rcClient);
         }
      }
   }
   break;

   case ID_TOOL_CONSTR_LINE:
      bSketchResult=CLineSketcher().DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CONSTR_LINE_5:
      bSketchResult=CConstrLineSketcher(5).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CONSTR_LINE_15:
      bSketchResult=CConstrLineSketcher(15).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CONSTR_LINE_30:
      bSketchResult=CConstrLineSketcher(30).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CONSTR_LINE_45:
      bSketchResult=CConstrLineSketcher(45).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CONSTR_LINE_90:
      bSketchResult=CConstrLineSketcher(90).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_PERP_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         CEdge* pEdge=NULL;
         if(pShape->GetNumShapeEdges()==1) pEdge=pShape->GetShapeEdge(0);
         else pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
         if(pEdge)
         {
            switch(pEdge->m_nType)
            {
            case CEdge::kArcEdge:
               if(((CArcEdge*)pEdge)->m_Radii.x!=((CArcEdge*)pEdge)->m_Radii.y)
               {
                  m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
                  break;
               }
            case CEdge::kLineEdge:
            {
               m_Mistakes.Reset();
               CSpecialLineSketcher sketcher(pEdge,CSpecialLineSketcher::kEdgePerp);
               bSketchResult=sketcher.DoSketch(this,pt,&rcClient);
               SetCursorPrompt(IDS_CLICKLINEARC);
            }
            break;
            default:
               m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
               break;
            }
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
   }
   break;
   case ID_TOOL_MIDPOINT_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
         if(pEdge)
         {
            if(pEdge->m_nType!=CEdge::kBezierEdge)
            {
               CSpecialLineSketcher sketcher(pEdge->MidPoint());
               bSketchResult=sketcher.DoSketch(this,pt,&rcClient);
               SetCursorPrompt(IDS_CLICKLINEARC);
               m_Mistakes.Reset();
               delete pEdge;
            }
            else m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
         }
         else AfxMessageBox(IDS_SELECT_SHAPE_EDGE,MB_OK);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
   }
   break;
   case ID_TOOL_ENDPOINT_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
         if(pEdge)
         {
            m_Mistakes.Reset();
            LongPoint lpt=m_Mapper.DPtoLP(pt);
            LongPoint lptFrom=(Pythag(pEdge->m_lptFrom-lpt)<Pythag(pEdge->m_lptTo-lpt))?pEdge->m_lptFrom:pEdge->m_lptTo;
            CSpecialLineSketcher sketcher(lptFrom);
            bSketchResult=sketcher.DoSketch(this,pt,&rcClient);
            SetCursorPrompt(IDS_CLICKLINEARC);
            delete pEdge;
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_EDGE_ARC);
   }
   break;
   case ID_TOOL_SHAPE_CENTER_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText);
      if(pShape)
      {
         LongPoint lptCenter;
         if(pShape->IsRadial()) lptCenter=pShape->GetCenter();
         else
         {
            LongRect ext=pShape->GetExtents();
            lptCenter.x=(ext.left+ext.right)>>1;
            lptCenter.y=(ext.top+ext.bottom)>>1;
         }
         CSpecialLineSketcher sketcher(lptCenter);
         bSketchResult=sketcher.DoSketch(this,pt,&rcClient);
         SetCursorPrompt(IDS_CLICKOBJECT);
      }
   }
   break;
   case ID_TOOL_TANGENT_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape && pShape->IsRadial())
      {
         LongPoint radii=pShape->GetRadii();
         if(radii.x==radii.y)	// circular only
         {
            m_Mistakes.Reset();
            CSpecialLineSketcher sketcher(CSpecialLineSketcher::kTangent);
            sketcher.m_lpt=pShape->GetCenter();
            sketcher.m_Radius=radii.x;
            bSketchResult=sketcher.DoSketch(this,pt,&rcClient);
            SetCursorPrompt(IDS_CLICKCIRCLEARC);
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
   }
   break;
   case ID_TOOL_CIRCLE_TANGENT_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape && pShape->IsRadial())
      {
         LongPoint radii=pShape->GetRadii();
         if(radii.x==radii.y)	// circular only
         {
            m_Mistakes.Reset();
            CTangentLineSketcher sketcher(pShape);
            bSketchResult=sketcher.DoSketch(this,pt,&rcClient,CSketcher::NoSnap|CSketcher::NoScroll);
            SetCursorPrompt(IDS_CLICK1CIRCLEARC);
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
   }
   break;
   case ID_TOOL_OFFSET_LINE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
         if(pEdge && pEdge->m_nType==CEdge::kLineEdge)
         {
            CSpecialLineSketcher sketcher(pEdge,CSpecialLineSketcher::kOffset);
            bSketchResult=sketcher.DoSketch(this,pt,&rcClient);
            SetCursorPrompt(IDS_CLICKLINEEDGE);
            delete pEdge;
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_LINE);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_LINE);
   }
   break;

   case ID_TOOL_RC_C:
      bSketchResult=CRndRectSketcher(CRndRectSketcher::kCornerCircle).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_RC_E:
      bSketchResult=CRndRectSketcher(CRndRectSketcher::kCornerElliptical).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_RC_P:
      bSketchResult=CRndRectSketcher(CRndRectSketcher::kCornerProportional).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_RECT:
      bSketchResult=CRectSketcher(CRectSketcher::kRectDiagonal).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_RECT_CENTER:
      bSketchResult=CRectSketcher(CRectSketcher::kRectCenter).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_CIRCLE_D:
      bSketchResult=CCircleSketcher(CCircleSketcher::kDiameter).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CIRCLE_R:
      bSketchResult=CCircleSketcher(CCircleSketcher::kRadius).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CIRCLE_3:
      bSketchResult=CCircleSketcher(CCircleSketcher::kThreePoint).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CIRCLE_CIRCLE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape && pShape->IsKindOf(RUNTIME_CLASS(CMDCircle)))
      {
         m_Mistakes.Reset();
         bSketchResult=COffsetCircleSketcher((CMDCircle*)pShape).DoSketch(this,pt,&rcClient);
         SetCursorPrompt(IDS_CLICKCIRCLE);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_CIRCLE);
   }
   break;

   case ID_TOOL_ARC_R:
      bSketchResult=CArcSketcher(CArcSketcher::kRadius).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_ARC_3:
      bSketchResult=CArcSketcher(CArcSketcher::kThreePoint).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_ARC_E:
      bSketchResult=CArcSketcher(CArcSketcher::kElliptic).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_ARC_ARC:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape && pShape->IsKindOf(RUNTIME_CLASS(CMDArcShape)))
      {
         CMDArcShape* pArc=(CMDArcShape*)pShape;
         if(pArc->m_nMode!=CMDArcShape::kElliptic)
         {
            m_Mistakes.Reset();
            bSketchResult=COffsetArcSketcher((CMDArcShape*)pShape).DoSketch(this,pt,&rcClient);
            SetCursorPrompt(IDS_CLICKARC);
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_ARC);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_ARC);
   }
   break;

   case ID_TOOL_ELLIPSE:
      bSketchResult=CEllipticSketcher(CEllipticSketcher::kEllipseDiagonal).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_ELLIPSE_CENTER:
      bSketchResult=CEllipticSketcher(CEllipticSketcher::kEllipseCenter).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_CURVE:
      bSketchResult=CFreehandSketcher(false).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CURVE_CLOSED:
      bSketchResult=CFreehandSketcher(true).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CURVE_BEZIER:
      bSketchResult=CBezierSketcher().DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_CURVE_CUBIC:
      bSketchResult=CCubicSketcher().DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_POLY:
      bSketchResult=CPolySketcher(true).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_POLY_OPEN:
      bSketchResult=CPolySketcher(false).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_REG_POLY_CENTER_SIDE:
      bSketchResult=CRegPolySketcher(CRegPolySketcher::kCenterSide,m_nRegPolySides).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_REG_POLY_CENTER_VERTEX:
      bSketchResult=CRegPolySketcher(CRegPolySketcher::kCenterVertex,m_nRegPolySides).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_REG_POLY_SIDE_SIDE:
      bSketchResult=CRegPolySketcher(CRegPolySketcher::kSideSide,m_nRegPolySides).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_REG_POLY_VERTEX_VERTEX:
      bSketchResult=CRegPolySketcher(CRegPolySketcher::kVertexVertex,m_nRegPolySides).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_PARALLEL_LINE:
      bSketchResult=CParallelLineSketcher(pDoc->m_ParallelAttrs).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_PARALLEL_POLY:
   case ID_TOOL_PARALLEL_POLY_CLOSED:
      bSketchResult=CParallelPolySketcher(pDoc->m_ParallelAttrs,m_nCurrentTool==ID_TOOL_PARALLEL_POLY_CLOSED).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_PARALLEL_LINE_EXTRUDE:
   case ID_TOOL_PARALLEL_POLY_EXTRUDE:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText);
      if(pShape && !pShape->IsRadial())
      {
         if(!pShape->IsParallel())
         {
            CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
            if(pEdge && pEdge->m_nType==CEdge::kLineEdge)
            {
               LongPoint lpt,lipt;
               lpt=m_Mapper.DPtoLP(pt);
               if(GetAsyncKeyState(VK_COMMAND)>=0) SnapLPt(lpt);
               guNearPtLnPt(lpt,pEdge->m_lptFrom,pEdge->m_lptTo,&lipt);
               double halfWidth=pDoc->m_ParallelAttrs.m_fWidth.GetRawDouble()/2;
               if(RawDoublePythag(lipt-pEdge->m_lptFrom)<halfWidth || RawDoublePythag(lipt-pEdge->m_lptTo)<halfWidth)
                  m_Mistakes.ShowMessage(IDS_EXTRUDE_NOT_VALID);
               else
               {
                  m_Mistakes.Reset();
                  bSketchResult=CExtrudeSketcher(pDoc->m_ParallelAttrs,m_nCurrentTool==ID_TOOL_PARALLEL_POLY_EXTRUDE,pEdge->m_lptFrom,pEdge->m_lptTo,pShape).DoSketch(this,pt,&rcClient);
               }
            }
            else m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
            if(pEdge) delete pEdge;
         }
         else
         {
            CDialog theDlg(IDD_BREAK_PARALLEL_SHAPE);
            if(IDOK==theDlg.DoModal())
            {
               SelectShape(pShape, false);
               OnToolUnglue();
            }
         }
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_EXTRUDE);
   }
   break;
   case ID_TOOL_STAR_DIAMETER: // Star Tool - Infinisys Ltd
      bSketchResult=CStarSketcher(CStarSketcher::kDiameter,m_nStarSides, m_iStarThickness).DoSketch(this,pt,&rcClient);
      break;
   case ID_TOOL_STAR_CENTER_VERTEX: // Star Tool - Infinisys Ltd
      bSketchResult=CStarSketcher(CStarSketcher::kCenterVertex,m_nStarSides,m_iStarThickness).DoSketch(this,pt,&rcClient);
      break;

   case ID_TOOL_MARKER:
      if(GetAsyncKeyState(VK_OPTION)<0) bSketchResult=false;
      else
      {
         const Coord kOneEighth = WrFixedFromFixed ( 589824L ) ;		// 1/8 of an inch
         const Coord kOneQuarter = WrFixedFromFixed ( 1179648L ) ;	// 1/4 of an inch
         CMDShapeList list;

         PenModel pm;
         pm.m_nPenType=kPenColor;
         pm.m_nIndex=1;	// black color
         pm.m_nWidth=2;

         FillModel fm;
         fm.m_nFillType=kNoFill;

         LongPoint lpt=m_lptCursorPos;	//m_Mapper.DPtoLP(pt);
         LongPoint lpt1,lpt2;

         CMDShape* pShape=new CMDCircle;
         lpt1=lpt2=lpt;
         pShape->SetShapePoint(0,lpt1);
         lpt2.x+=kOneEighth;
         pShape->SetShapePoint(1,lpt2);
         pShape->SetPenModel(pm);
         pShape->SetFillModel(fm);
         pShape->CalculateExtents();
         pShape->SetShapeDatum(kDefaultDatum);
         list.AddHead(pShape);

         pm.m_nWidth=1;

         pShape=new CMDLineShape;
         lpt1=lpt2=lpt;
         lpt1.x-=kOneQuarter;
         pShape->SetShapePoint(0,lpt1);
         lpt2.x+=kOneQuarter;
         pShape->SetShapePoint(1,lpt2);
         pShape->SetPenModel(pm);
         pShape->CalculateExtents();
         pShape->SetShapeDatum(kDefaultDatum);
         list.AddHead(pShape);

         pShape=new CMDLineShape;
         lpt1=lpt2=lpt;
         lpt1.y-=kOneQuarter;
         pShape->SetShapePoint(0,lpt1);
         lpt2.y+=kOneQuarter;
         pShape->SetShapePoint(1,lpt2);
         pShape->SetPenModel(pm);
         pShape->CalculateExtents();
         pShape->SetShapeDatum(kDefaultDatum);
         list.AddHead(pShape);

         SaveUndo(IDS_CREATION);
         CMDGroupShape* pGroup=new CMDGroupShape(&list);
         pGroup->CalculateExtents();
         pGroup->SetShapeDatum(kDefaultDatum);
         pDoc->CreateShape(pGroup);
         SelectShape(pGroup,false);
      }
      break;

   case ID_TOOL_ATTRIB_DROP:
   {
      bool bIsCopy=GetAsyncKeyState(VK_OPTION)<0;
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         if(bIsCopy) ((CPCDraftApp*)AfxGetApp())->DoCopyAttributes(pShape);
         else
         {
            if(pShape->IsLocked()) SetCursor(AfxGetApp()->LoadCursor(IDC_ATTRIB_DROP_LOCK));
            else
            {
               ShapeAttributes attrib=((CPCDraftApp*)AfxGetApp())->m_ShapeAttributes;

               if(GetAsyncKeyState(VK_SHIFT)<0)
               {
                  CAttributeOptionsDlg theDlg(pDoc,&attrib,true);
                  if(theDlg.DoModal()!=IDOK)
                     break;
               }
               SaveUndo(IDS_EYE_DROPPER);
               DoApplyAttributes(pShape,attrib);
            }
         }
      }
      else
      {
         if(bIsCopy) bSketchResult=false;
         else
         {
            SetCursor(AfxGetApp()->LoadCursor(bIsCopy?IDC_ATTRIB_PICK_QUERY:IDC_ATTRIB_DROP_QUERY));
            Sleep(300);
         }
      }
   }
   break;

   case ID_TOOL_ZOOM:
      if(::GetAsyncKeyState(VK_COMMAND)<0) bSketchResult=false;
      else
      {
         if(::GetAsyncKeyState(VK_OPTION)<0)
         {
            DoZoomOut(point); // Infinisys Ltd 2021 - Context Menu - Moved Zoom Out from inside OnLButtonDown to DoZoomOut
         }
         else
         {
            DoZoomIn(point); // Infinisys Ltd 2021 - Context Menu - Moved Zoom In from inside OnLButtonDown to DoZoomIn
         }
      }
      break;

   case ID_TOOL_DIM_HORIZ:
   {
      m_pLineDimSketcher=new CLineDimSketcher (&pDoc->m_DimStd,&pDoc->m_DimFmts[kLinear],m_nDimType,CLineDimSketcher::kHoriz);
lineDimCommon:
      if(m_nDimType) SetCursorPrompt(IDS_NEXTPOINT);
      else SetCursorPrompt(IDS_2POINT);
      bSketchResult=m_pLineDimSketcher->DoSketch(this,pt,&rcClient);
      if(bSketchResult && m_nDimType)
      {
         SetOneTimeTool(ID_TOOL_DIM_NEXT_POINT,IDS_NEXTPOINT,kSnap);
      }
      else
      {
         if(ID_TOOL_DIM_PERP==m_nCurrentTool)
         {
            m_nSnapMode=kNoSnap;
            SetCursorPrompt(IDS_CLICKLINEEDGE);
         }
         delete m_pLineDimSketcher;
      }
   }
   break;
   case ID_TOOL_DIM_VERT:
   {
      m_pLineDimSketcher=new CLineDimSketcher(&pDoc->m_DimStd,&pDoc->m_DimFmts[kLinear],m_nDimType,CLineDimSketcher::kVert);
      goto lineDimCommon;
   }
   break;
   case ID_TOOL_DIM_SLOPE:
   {
      m_pLineDimSketcher=new CLineDimSketcher(&pDoc->m_DimStd,&pDoc->m_DimFmts[kLinear],m_nDimType,CLineDimSketcher::kSlope);
      goto lineDimCommon;
   }
   break;
   case ID_TOOL_DIM_PERP:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
         if(pEdge && pEdge->m_nType==CEdge::kLineEdge)
         {
            m_pLineDimSketcher=new CLineDimSketcher(&pDoc->m_DimStd,&pDoc->m_DimFmts[kLinear],m_nDimType,CLineDimSketcher::kPerp,pEdge);
            goto lineDimCommon;
         }
      }
      m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
   }
   break;
   case ID_TOOL_DIM_DIAMETER:
   case ID_TOOL_DIM_RADIUS:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape && pShape->IsRadial())
      {
         LongPoint radii=pShape->GetRadii();
//				if(radii.x==radii.y || pShape->GetRotation()==0)
         {
//					m_Mistakes.Reset();
            m_nSnapMode=kSnap;
            bSketchResult=CRadialDimSketcher(&pDoc->m_DimStd,&pDoc->m_DimFmts[kRadial],pShape,m_nCurrentTool==ID_TOOL_DIM_DIAMETER).DoSketch(this,pt,&rcClient);
            m_nSnapMode=kNoSnap;
         }
//				else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ELLIP_ARC);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ELLIP_ARC);
   }

   break;
   case ID_TOOL_DIM_CENTER:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape && pShape->IsRadial())
      {
         SaveUndo(IDS_CREATION);
         m_Mistakes.Reset();
         LongPoint radii=pShape->GetRadii();
         CMDShape* pDimShape=new CMDCenterDim(pDoc->m_DimStd,pDoc->m_DimFmts[kLinear],pShape->GetCenter(),radii);
         if(radii.x!=radii.y) // ellipse - so rotate center dim correspondingly
            pDimShape->SetRotation(pShape->GetRotation());
         pDimShape->ReCalc(&m_Mapper);
         pDimShape->CalculateExtents();
         pDoc->CreateShape(pDimShape);
         SelectShape(pDimShape,false);
         if(((CPCDraftApp*)AfxGetApp())->m_bLinkedDimensions)
            LinkCenterVertexes(pShape,pDimShape);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
   }
   break;
   case ID_TOOL_DIM_ANGULAR:
   {
      CMDShape* pShape=pDoc->GetActiveLayer()->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText,true);
      if(pShape)
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_Mapper);
         if(pEdge && pEdge->m_nType==CEdge::kLineEdge)
         {
            m_Mistakes.Reset();
            CAngularDimSketcher  sketcher(&pDoc->m_DimStd,&pDoc->m_DimFmts[kAngular],pEdge);
            bSketchResult=sketcher.DoSketch(this,pt,&rcClient,CSketcher::NoSnap);
         }
         else m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
      }
      else m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
   }
   break;

   case ID_TOOL_HANDLE:
   {
      if(SelectShapeFromPoint(pt,false))
      {
         CMDShape* pShape=m_Selection.GetHead();
         if(!pShape->IsLocked())
         {
            CLayer* pLayer=pDoc->GetActiveLayer();
            if(GetAsyncKeyState(VK_OPTION)<0)
            {
               if(pShape->CanRemovePt())
               {
                  int nHandle=pShape->TestHandles(pt,&m_Mapper);
                  if(nHandle!=-1)
                  {
                     SaveUndo(IDS_SUBVERTEX);
                     CMDShape* pNewShape=NULL;
                     InvalidateShape(pShape,true);
                     pShape->RemovePt(nHandle,&pNewShape);
                     pLayer->m_LinkVertexList.AdjustForSubVertex(pShape,nHandle);
                     if(pNewShape)
                     {
                        pLayer->m_LinkVertexList.ReplaceShape(pShape,pNewShape);
                        if(pShape->IsAreaShown())
                        {
                           if(pNewShape->HaveArea())
                           {
                              pNewShape->Attach(pShape->Detach());
                           }
                           else pShape->ShowArea(false,this);
                        }
                        m_Selection.RemoveHead();
                        pLayer->ReplaceShape(pShape,pNewShape);
                        m_Selection.AddHead(pNewShape);
                        pNewShape->UpdateDependentShapes(this);
                     }
                     else pShape->UpdateDependentShapes(this);
                  }
               }
            }
            else
            {
               CMDShape* pNewShape=NULL;
               if(pShape->CanAddPt(&pNewShape))
               {
                  SaveUndo(IDS_ADDVERTEX);
                  int nHandle;
                  if(pNewShape) nHandle=pNewShape->AddPt(pt,&m_Mapper);
                  else nHandle=pShape->AddPt(pt,&m_Mapper);
                  pLayer->m_LinkVertexList.AdjustForAddVertex(pShape,nHandle);
                  if(nHandle!=-1)
                  {
                     if(pNewShape)
                     {
                        pLayer->m_LinkVertexList.ReplaceShape(pShape,pNewShape);
                        m_Selection.RemoveHead();
                        if(pShape->IsAreaShown())
                        {
                           pNewShape->Attach(pShape->Detach());
                        }
                        pLayer->ReplaceShape(pShape,pNewShape);
                        m_Selection.AddHead(pNewShape);
                        pShape=pNewShape;
                     }
                     if(GetAsyncKeyState(VK_SHIFT)>=0)
                     {
                        pShape->DoTrackShape(nHandle,pt,this,rcClient);
                     }
                     InvalidateShape(pShape,true);
                     return;
                  }
                  else if(pNewShape) delete pNewShape;
                  pDoc->SetModifiedFlag();
                  ForceResizeUpdate();
               }
            }
         }
      }
   }
   break;

   case ID_TOOL_ROTATE:
   {
      // check if selection contains linked dimensions
      bool bHaveLinkedDims=false;
      LongBoundRect lrcExtent;
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         lrcExtent|=pShape->GetExtents();
         if(pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && CanMoveDimension(pShape))
            bHaveLinkedDims=true;
      }
      LongPoint lptCenter=Middle(lrcExtent);
      CRotateSketcher sketcher(m_Selection.GetCount()==1?m_Selection.GetHead()->GetRotation():0,GetAsyncKeyState(VK_OPTION)>=0,bHaveLinkedDims?&lptCenter:NULL);
      sketcher.DoSketch(this,pt,rcClient,CSketcher::NoSnap|CSketcher::NoScroll);
   }
   break;

   case ID_TOOL_REP_PASTE:
      if(GetAsyncKeyState(VK_OPTION)<0) bSketchResult=false;
      else DoEditPaste(false,&pt);
      break;

   case ID_TOOL_PLACE:
   {
      SaveUndo(0);
      SelectNone();
      CreateShapesFromHandle(m_hPlaceShape,m_bPlaceUnscaled,&pt);
      GetDocument()->CheckDocSize(true);
   }
   break;

   case ID_TOOL_EDIT_DATUM:
      if(GetAsyncKeyState(VK_OPTION)<0 || m_Selection.GetCount()==0) bSketchResult=false;
      else
      {
         CMDShape* pShape=m_Selection.GetHead();
         CPoint ptPos=m_ptCursorPos+GetDeviceScrollPosition();
         LongRect ext=pShape->GetExtents();
         CRect rc=m_Mapper.LPtoDP(ext);
         rc.right++;
         rc.bottom++;
         if(rc.PtInRect(ptPos))
         {
            LongPoint lpt=m_Mapper.DPtoLP(ptPos);
            if(lpt.x<ext.left) lpt.x=ext.left;
            if(lpt.x>ext.right) lpt.x=ext.right;
            if(lpt.y<ext.top) lpt.y=ext.top;
            if(lpt.y>ext.bottom) lpt.y=ext.bottom;

            Coord topLeft=m_Mapper.DPtoLP((long)2), bottomRight=m_Mapper.DPtoLP((long)3);
            LongRect lrcProx(lpt.x-topLeft,lpt.y-topLeft,lpt.x+bottomRight,lpt.y+bottomRight);

            int nDatum=pShape->Snap(&lpt,lrcProx,0);
            if(nDatum<0) pShape->SetShapeDatum(lpt);
            else
            {
               CClientDC cdc(this);
               CPoint ptOrg=GetDeviceScrollPosition();
               cdc.SetViewportOrg(-ptOrg);
               CRect rcClient;
               GetActiveClientRect(rcClient);
               CRgn rgn;
               rgn.CreateRectRgnIndirect(&rcClient);
               cdc.SelectClipRgn(&rgn);

               CGDIROP rop(&cdc,R2_NOT);
               CPoint pt=m_Mapper.LPtoDP(pShape->GetShapeDatum());
               // erase old datum
               cdc.MoveTo(pt+ CPoint(-4,-4));
               cdc.LineTo(pt+ CPoint(5,5));
               cdc.MoveTo(pt+ CPoint(4,-4));
               cdc.LineTo(pt+ CPoint(-5,5));

               pShape->SetShapeDatum(nDatum);
               pt=m_Mapper.LPtoDP(pShape->GetShapeDatum());
               int nCount=4;
               while(nCount--)
               {
                  cdc.MoveTo(pt+ CPoint(-4,-4));
                  cdc.LineTo(pt+ CPoint(5,5));
                  cdc.MoveTo(pt+ CPoint(4,-4));
                  cdc.LineTo(pt+ CPoint(-5,5));
                  Sleep(100);
               }
            }
            ForceResizeUpdate();
            InvalidateShape(pShape,true);
         }
      }
      break;

   default:
      break;
   }
   if(!bSketchResult) SetArrowTool();
}


void CPCDraftView::DoZoomOut(CPoint point, bool bSingleClick)
{
   CPCDraftDoc* pDoc = GetDocument();
   CPoint pt = point + GetDeviceScrollPosition();
   CRect rcClient;
   GetActiveClientRect(rcClient);

   if (!rcClient.PtInRect(point)) return;
   // Zoom out
   if (pDoc->m_nZoomFactor > -3)
   {
      if (bSingleClick || CSketcher().DoSketch(this, pt))
      {
         if (!bSingleClick)
         {
            SaveUndo(0);

         }
         
         LongPoint lpt = m_Mapper.DPtoLP(pt);
#ifdef _FLOAT_ZOOM
         pDoc->DoZoom(0.5, -1, GetDeviceScrollPosition());
#else
         pDoc->DoZoom(-1, GetDeviceScrollPosition());
#endif
         CPoint ptOrg = m_Mapper.LPtoDP(lpt);
         CSize szClientSize;
         GetClientSize(szClientSize);
         ptOrg.x -= szClientSize.cx / 2;
         ptOrg.y -= szClientSize.cy / 2;
         if (ptOrg.x < 0) ptOrg.x = 0;
         if (ptOrg.y < 0) ptOrg.y = 0;
         ScrollToDevicePosition(ptOrg);
      }
   }
   else ::MessageBeep(MB_OK);

}

void CPCDraftView::DoZoomIn(CPoint point, bool bSingleClick)
{
   CPCDraftDoc* pDoc = GetDocument();
   CPoint pt = point + GetDeviceScrollPosition();
   CRect rcClient;
   GetActiveClientRect(rcClient);

   if (!rcClient.PtInRect(point)) return;
   // Zoom in
   if (pDoc->m_nZoomFactor < pDoc->m_nMaxZoom)
   {
      CZoomSketcher theZoomSketcher;
      if (bSingleClick || theZoomSketcher.DoSketch(this, pt, &rcClient, CSketcher::NoSnap | CSketcher::NoScroll))
      {
         if (!bSingleClick)
         {
            SaveUndo(0);

         }
         if (!bSingleClick && theZoomSketcher.m_bArea)
         {
            CPoint ptOrg = theZoomSketcher.m_thickRect.TopLeft();
            LongPoint lptOrg = m_Mapper.DPtoLP(ptOrg);
            pDoc->DoZoom(theZoomSketcher.m_fZoom, theZoomSketcher.m_nZoom, GetDeviceScrollPosition());
            ptOrg = m_Mapper.LPtoDP(lptOrg);
            if (ptOrg.x < 0) ptOrg.x = 0;
            if (ptOrg.y < 0) ptOrg.y = 0;
            ScrollToDevicePosition(ptOrg);
         }
         else
         {
            LongPoint lpt = m_Mapper.DPtoLP(pt);
            pDoc->DoZoom(2, 1, GetDeviceScrollPosition());
            CPoint ptOrg = m_Mapper.LPtoDP(lpt);
            CSize szClientSize;
            GetClientSize(szClientSize);
            ptOrg.x -= szClientSize.cx / 2;
            ptOrg.y -= szClientSize.cy / 2;
            if (ptOrg.x < 0) ptOrg.x = 0;
            if (ptOrg.y < 0) ptOrg.y = 0;
            ScrollToDevicePosition(ptOrg);
         }
         }
      }
   else ::MessageBeep(MB_OK);

}

void CPCDraftView::OnViewZoomIn() // Infinisys Ltd. 2021 - Context Menus
{
   DoZoomIn(m_pointZoom, true);
}


void CPCDraftView::OnViewZoomOut() // Infinisys Ltd. 2021 - Context Menus
{
   DoZoomOut(m_pointZoom, true);
}


void CPCDraftView::OnUpdateViewZoomIn(CCmdUI* pCmdUI) // Infinisys Ltd. 2021 - Context Menus
{

   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor < pDoc->m_nMaxZoom);


}


void CPCDraftView::OnUpdateViewZoomOut(CCmdUI* pCmdUI) // Infinisys Ltd. 2021 - Context Menus
{

   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor > -3);

}


// Infinisys Ltd. 2021 - Context Menus
void CPCDraftView::OnRButtonDown(UINT nFlags, CPoint point)
{
   //if(m_nCurrentTool==ID_TOOL_ARROW && !m_nOneTimeTool)
   //{
   //   CMenu theMenu;
   //   theMenu.LoadMenu(IDR_PCDRAFT_POPUP);
   //   CMenu* pPopup=theMenu.GetSubMenu(0);
   //   ClientToScreen(&point);
   //   pPopup->TrackPopupMenu(TPM_RIGHTBUTTON|TPM_CENTERALIGN,point.x,point.y,AfxGetMainWnd());
   //}
   //else SetArrowTool();

   //CPoint pt = point + GetDeviceScrollPosition();

   m_pointZoom = point;

   enum enum_context_menu
   {
      e_context_menu_no_selection,
      e_context_menu_objects,
      e_context_menu_text,
      e_context_menu_image
   };

   enum_context_menu econtextmenu = e_context_menu_no_selection;

   POSITION pos = m_Selection.GetHeadPosition();
   while (pos)
   {
      CMDShape* pShape = m_Selection.GetNext(pos);
      switch (econtextmenu)
      {
      case e_context_menu_no_selection:
         if (pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
         {
            econtextmenu = e_context_menu_text;
         }
         else if (pShape->IsKindOf(RUNTIME_CLASS(CMDBitmap)))
         {
            econtextmenu = e_context_menu_image;
         }
         else
         {
            econtextmenu = e_context_menu_objects;
         }
         break;
      case e_context_menu_text:
         if (!pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
         {
            econtextmenu = e_context_menu_objects;
         }
         break;
      case e_context_menu_image:
         if (!pShape->IsKindOf(RUNTIME_CLASS(CMDBitmap)))
         {
            econtextmenu = e_context_menu_objects;
         }
         break;
      };
      if (econtextmenu == e_context_menu_objects)
      {

         break;

      }
   }
 
   CMenu theMenu;

   switch (econtextmenu)
   {
   case e_context_menu_no_selection:
      theMenu.LoadMenu(IDR_CONTEXT_NO_SELECTION);
      break;
   case e_context_menu_objects:
      theMenu.LoadMenu(IDR_CONTEXT_OBJECTS);
      break;
   case e_context_menu_image:
      theMenu.LoadMenu(IDR_CONTEXT_IMAGE);
      break;
   case e_context_menu_text:
      theMenu.LoadMenu(IDR_CONTEXT_TEXT);
      break;
   };

   CMenu* pPopup=theMenu.GetSubMenu(0);

   auto pMainFrame = ((CMainFrame*)AfxGetMainWnd());

   CPCDraftDoc* pDoc = GetDocument();

   pMainFrame->CreateAngleSnapSubMenu(pDoc, pPopup);

   pMainFrame->CreateGridSubMenu(pDoc, pPopup);

   if (pPopup != nullptr)
   {

      ClientToScreen(&point);

      pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_CENTERALIGN, point.x, point.y, AfxGetMainWnd());

   }

}


bool CPCDraftView::BitmapFromSelection(CRect & rectSelection,CBitmap & bitmap,int iMargin)
{

   //GetSelectionExtents(&rectSelection);

   CPCDraftDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   LongRect rExtents;
   bool bFirst = true;
   POSITION pos=m_Selection.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetPrev(pos);

      LongRect lrc=pShape->GetExtents();
      pShape->ExtendForPen(lrc);
      if(bFirst)
         rExtents = lrc;
      else
         rExtents |= lrc;
      bFirst = false;
   }
   rExtents.Inset(-3,-3);

   //CPoint pt=m_Mapper.LPtoDP(LongPoint(r.left,r.top));
   //rectSelection.left = pt.x;
   //rectSelection.top = pt.y;
   //pt=m_Mapper.LPtoDP(LongPoint(r.right,r.bottom));
   //rectSelection.right = pt.x;
   //rectSelection.bottom = pt.y;
   
   CMapper & mapper = m_Mapper;
   rectSelection=mapper.LPtoDP(rExtents);
   rectSelection.right++;
   rectSelection.bottom++;

   //rectSelection.InflateRect(iMargin,iMargin);

   COLORREF * pdib = NULL;

   HBITMAP hbitmap = CreateDIB(rectSelection.Width(),rectSelection.Height(),NULL,0,&pdib);

   if(hbitmap == NULL)
   {

      return false;

   }

   bitmap.Attach(hbitmap);

   int w = rectSelection.Width();

   int h = rectSelection.Height();

   int iScan = w * sizeof(COLORREF);

   byte * pb = (byte *) pdib;

   //for(int x = 0; x < iScan; x+=4)
   //{
   //   
   //   for(int y = 0; y < h; y++)
   //   {

   //      pb[x + y * iScan + 0] = 0;
   //      pb[x + y * iScan + 1] = 0;
   //      pb[x + y * iScan + 2] = 0;
   //      pb[x + y * iScan + 3] = 0;

   //   }

   //}

//   LongRect lrcExtents=GetCumulativeExtents(false);
   //lrcExtents.Inset(-3,-3);


   CRect rcBound(0,0,Width(rectSelection),Height(rectSelection));


   {

      CDC dcMem;

      dcMem.CreateCompatibleDC(NULL);


      HGDIOBJ hOld = dcMem.SelectObject(&bitmap);

      dcMem.SetMapMode(MM_TEXT);
      dcMem.OffsetViewportOrg(iMargin / 2,iMargin / 2);

      dcMem.OffsetViewportOrg(-rectSelection.left,-rectSelection.top);

      CRgn theClipRgn;
      theClipRgn.CreateRectRgnIndirect(&rectSelection);
      theClipRgn.OffsetRgn(-rectSelection.left,-rectSelection.top);
      dcMem.SelectClipRgn(&theClipRgn);
      //dcMem.FillRect(&rectSelection,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));


      CPCDraftDoc* pDoc = GetDocument();

      ASSERT_VALID(pDoc);

      CDraftContext dc(&dcMem,&mapper,pDoc);

      dc.m_bTransparentBackground = true;

      dc.SetGreyMode(false);

      CPalette* pPalette=dcMem.SelectPalette(&pDoc->m_Palette,FALSE);
      dcMem.RealizePalette();

      POSITION pos = m_Selection.GetTailPosition();

      while(pos != NULL)
      {

         CMDShape* pShape = m_Selection.GetPrev(pos);

         UseRect userect(&dc, pShape);

         pShape->DrawShape(&dc);

      }
      dcMem.SelectPalette(pPalette,FALSE);


      GdiFlush();

      dcMem.SelectObject(hOld);

   }

//   for(int x = 0; x < iScan; x+=4)
//   {
//
//      for(int y = 0; y < h; y++)
//      {
//
//         //if(pb[x + y * iScan + 3] == 0
//         //   || pb[x + y * iScan + 0] != 0
//         //   || pb[x + y * iScan + 1] != 0
//         //   || pb[x + y * iScan + 2] != 0)
////         if(pb[x + y * iScan + 3] == 0)
//         {
//
//            pb[x + y * iScan + 3] = 255;
//
//         }
//         //else
//         {
//
//         //   ASSERT(pb[x + y * iScan + 0] == 0);
//         //   ASSERT(pb[x + y * iScan + 1] == 0);
//         //   ASSERT(pb[x + y * iScan + 2] == 0);
//            //pb[x + y * iScan + 3] = 0;
//
//         }
//
//      }
//
//   }
//

   return true;

}


bool CPCDraftView::DrawSelection(const CRect & rectSelection,Gdiplus::Graphics & g,int x,int y, int iMargin)
{



   ////GetSelectionExtents(&rectSelection);

   //CPCDraftDoc* pDoc = GetDocument();
   //ASSERT_VALID(pDoc);

   //LongRect rExtents;
   //bool bFirst = true;
   //POSITION pos=m_Selection.GetTailPosition();
   //while(pos)
   //{
   //   CMDShape* pShape=m_Selection.GetPrev(pos);

   //   LongRect lrc=pShape->GetExtents();
   //   pShape->ExtendForPen(lrc);
   //   if(bFirst)
   //      rExtents = lrc;
   //   else
   //      rExtents |= lrc;
   //   bFirst = false;
   //}
   //rExtents.Inset(-3,-3);

   ////CPoint pt=m_Mapper.LPtoDP(LongPoint(r.left,r.top));
   ////rectSelection.left = pt.x;
   ////rectSelection.top = pt.y;
   ////pt=m_Mapper.LPtoDP(LongPoint(r.right,r.bottom));
   ////rectSelection.right = pt.x;
   ////rectSelection.bottom = pt.y;

   CMapper & mapper = m_Mapper;
   //rectSelection=mapper.LPtoDP(rExtents);
   //rectSelection.right++;
   //rectSelection.bottom++;

   //rectSelection.InflateRect(iMargin,iMargin);

   //COLORREF * pdib = NULL;

   //HBITMAP hbitmap = CreateDIB(rectSelection.Width(),rectSelection.Height(),NULL,0,&pdib);

   //if(hbitmap == NULL)
   //{

   //   return false;

   //}

   //bitmap.Attach(hbitmap);

   //int w = rectSelection.Width();

   //int h = rectSelection.Height();

   //int iScan = w * sizeof(COLORREF);

   //byte * pb = (byte *)pdib;

   ////for(int x = 0; x < iScan; x+=4)
   ////{
   ////   
   ////   for(int y = 0; y < h; y++)
   ////   {

   ////      pb[x + y * iScan + 0] = 0;
   ////      pb[x + y * iScan + 1] = 0;
   ////      pb[x + y * iScan + 2] = 0;
   ////      pb[x + y * iScan + 3] = 0;

   ////   }

   ////}

   //   LongRect lrcExtents=GetCumulativeExtents(false);
   //lrcExtents.Inset(-3,-3);


   CRect rcBound(0,0,Width(rectSelection),Height(rectSelection));

   HDC hdc = g.GetHDC();

   try
   {

      CDC dcMem;

      dcMem.Attach(hdc);

      int iSave = dcMem.SaveDC();

      try
      {

         dcMem.SetMapMode(MM_TEXT);
         dcMem.OffsetViewportOrg(iMargin / 2,iMargin / 2);

         dcMem.OffsetViewportOrg(x - rectSelection.left,y - rectSelection.top);

         CRgn theClipRgn;
         theClipRgn.CreateRectRgnIndirect(&rectSelection);
         theClipRgn.OffsetRgn(-rectSelection.left,-rectSelection.top);
         dcMem.SelectClipRgn(&theClipRgn);
         //dcMem.FillRect(&rectSelection,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));


         CPCDraftDoc* pDoc = GetDocument();

         ASSERT_VALID(pDoc);

         CDraftContext dc(&dcMem,&mapper,pDoc);

         dc.m_bTransparentBackground = true;

         dc.SetGreyMode(false);

         CPalette* pPalette=dcMem.SelectPalette(&pDoc->m_Palette,FALSE);
         dcMem.RealizePalette();

         POSITION pos = m_Selection.GetTailPosition();

         while(pos != NULL)
         {

            CMDShape* pShape = m_Selection.GetPrev(pos);

            UseRect userect(&dc, pShape);

            pShape->DrawShape(&dc);

         }
         dcMem.SelectPalette(pPalette,FALSE);


         GdiFlush();

      }
      catch(...)
      {
      }

      dcMem.RestoreDC(iSave);

      dcMem.Detach();

   //   for(int x = 0; x < iScan; x+=4)
   //   {
   //
   //      for(int y = 0; y < h; y++)
   //      {
   //
   //         //if(pb[x + y * iScan + 3] == 0
   //         //   || pb[x + y * iScan + 0] != 0
   //         //   || pb[x + y * iScan + 1] != 0
   //         //   || pb[x + y * iScan + 2] != 0)
   ////         if(pb[x + y * iScan + 3] == 0)
   //         {
   //
   //            pb[x + y * iScan + 3] = 255;
   //
   //         }
   //         //else
   //         {
   //
   //         //   ASSERT(pb[x + y * iScan + 0] == 0);
   //         //   ASSERT(pb[x + y * iScan + 1] == 0);
   //         //   ASSERT(pb[x + y * iScan + 2] == 0);
   //            //pb[x + y * iScan + 3] = 0;
   //
   //         }
   //
   //      }
   //
   //   }
   //
   }
   catch(...)
   {
   }

   g.ReleaseHDC(hdc);

   return true;

}


bool CPCDraftView::BitmapFromView(CBitmap & bitmap)
{
   CRect rView;

   GetClientRect(&rView);

   

   {


      CDC * pdc = GetDC();

      CDC dc;

      dc.CreateCompatibleDC(pdc);

      int iW = pdc->GetViewportExt().cx;
      int iH = pdc->GetViewportExt().cy;

      int iViewW = rView.Width();
      int iViewH = rView.Height();

      if(!bitmap.CreateCompatibleBitmap(pdc,iViewW,iViewH))
      {

         return false;

      }

      //ReleaseDC(pdc);

      HGDIOBJ hOld = dc.SelectObject(&bitmap);

      dc.SetMapMode(MM_TEXT);


      //dc.SetViewportOrg(-GetDeviceScrollPosition());

      dc.BitBlt(0,0,rView.Width(),rView.Height(),pdc,0,0,SRCCOPY);

      //dc.FillSolidRect(rView,RGB(255,255,255));

      //OnEraseBkgnd(&dc);
      //OnPrepareDC(&dc,NULL);
      //OnDraw(&dc);




      GdiFlush();

      ReleaseDC(pdc);

      dc.SelectObject(hOld);

   }

   return true;

}


bool CPCDraftView::BitmapFromDrawing(CBitmap & bitmap)
{
   CPCDraftDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   CRect rectSelection;

   LongRect rExtents = GetDocument()->GetCumulativeExtents(false);

   //CPoint pt=m_Mapper.LPtoDP(LongPoint(r.left,r.top));
   //rectSelection.left = pt.x;
   //rectSelection.top = pt.y;
   //pt=m_Mapper.LPtoDP(LongPoint(r.right,r.bottom));
   //rectSelection.right = pt.x;
   //rectSelection.bottom = pt.y;

   CMapper & mapper = m_Mapper;
   rectSelection = mapper.LPtoDP(rExtents);
   rectSelection.right++;
   rectSelection.bottom++;

   //rectSelection.InflateRect(iMargin,iMargin);

   COLORREF * pdib = NULL;

   HBITMAP hbitmap = CreateDIB(rectSelection.Width(), rectSelection.Height(), NULL, 0, &pdib);

   if (hbitmap == NULL)
   {

      return false;

   }

   bitmap.Attach(hbitmap);

   int w = rectSelection.Width();

   int h = rectSelection.Height();

   int iScan = w * sizeof(COLORREF);

   byte * pb = (byte *)pdib;

   for(int x = 0; x < iScan; x+=4)
   {
      
      for(int y = 0; y < h; y++)
      {

         pb[x + y * iScan + 0] = 255;
         pb[x + y * iScan + 1] = 255;
         pb[x + y * iScan + 2] = 255;
         pb[x + y * iScan + 3] = 255;

      }

   }

//   LongRect lrcExtents=GetCumulativeExtents(false);
   //lrcExtents.Inset(-3,-3);

   int iMargin = 5;


   CRect rcBound(0, 0, Width(rectSelection), Height(rectSelection));


   {

      CDC dcMem;

      dcMem.CreateCompatibleDC(NULL);


      HGDIOBJ hOld = dcMem.SelectObject(&bitmap);

      dcMem.SetMapMode(MM_TEXT);
      dcMem.OffsetViewportOrg(iMargin / 2, iMargin / 2);

      dcMem.OffsetViewportOrg(-rectSelection.left, -rectSelection.top);

      CRgn theClipRgn;
      theClipRgn.CreateRectRgnIndirect(&rectSelection);
      theClipRgn.OffsetRgn(-rectSelection.left, -rectSelection.top);
      dcMem.SelectClipRgn(&theClipRgn);
      //dcMem.FillRect(&rectSelection,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));


      CPCDraftDoc* pDoc = GetDocument();

      ASSERT_VALID(pDoc);

      CDraftContext dc(&dcMem, &mapper, pDoc);
      dc.m_bCheckRectVisible = false;
      dc.m_bTransparentBackground = true;

      dc.SetGreyMode(false);

      CPalette* pPalette = dcMem.SelectPalette(&pDoc->m_Palette, FALSE);
      dcMem.RealizePalette();

      for (int i = 0; i < pDoc->m_Layers.GetSize(); i++)
      {
         CLayer* pLayer = pDoc->GetLayer(i);
         if (pLayer->Draw(&dc, pDoc->m_nActiveLayer == i, pDoc->m_bHideText, m_pActiveTextShape))
         {
            break;
         }
      }



      GdiFlush();

      dcMem.SelectObject(hOld);

   }

   //   for(int x = 0; x < iScan; x+=4)
   //   {
   //
   //      for(int y = 0; y < h; y++)
   //      {
   //
   //         //if(pb[x + y * iScan + 3] == 0
   //         //   || pb[x + y * iScan + 0] != 0
   //         //   || pb[x + y * iScan + 1] != 0
   //         //   || pb[x + y * iScan + 2] != 0)
   ////         if(pb[x + y * iScan + 3] == 0)
   //         {
   //
   //            pb[x + y * iScan + 3] = 255;
   //
   //         }
   //         //else
   //         {
   //
   //         //   ASSERT(pb[x + y * iScan + 0] == 0);
   //         //   ASSERT(pb[x + y * iScan + 1] == 0);
   //         //   ASSERT(pb[x + y * iScan + 2] == 0);
   //            //pb[x + y * iScan + 3] = 0;
   //
   //         }
   //
   //      }
   //
   //   }
   //

   return true;

}

Gdiplus::Bitmap * CPCDraftView::BitmapFromSelection(CRect & rectSelection, double dAlpha, int iMargin)
{

   BYTE bA = (BYTE) (dAlpha * 255.0);

   Gdiplus::Bitmap * pbitmap = NULL;

   CBitmap bmp;

   if(!BitmapFromSelection(rectSelection,bmp,iMargin))
   {

      return NULL;

   }

   BITMAP b;

   bmp.GetBitmap(&b);

#undef new

   pbitmap = new Gdiplus::Bitmap(b.bmWidth,b.bmHeight);
   //pbitmap = new Gdiplus::Bitmap((HBITMAP) bmp.GetSafeHandle(), NULL);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   Gdiplus::Rect r2;

   r2.X        = 0;

   r2.Y        = 0;

   r2.Width    = pbitmap->GetWidth();

   r2.Height   = pbitmap->GetHeight();

   Gdiplus::BitmapData data;

   pbitmap->LockBits(&r2,0,PixelFormat32bppARGB,&data);

   if(bA == 255)
   {

      for(int i = 0;i < r2.Height; i++)
      {

         memcpy(&((byte *)data.Scan0)[data.Stride * i],&((COLORREF*)b.bmBits)[i * r2.Width],r2.Width * sizeof(COLORREF));

      }

   }
   else
   {

      for(int i = 0;i < r2.Height; i++)
      {

         for(int j = 0;j < r2.Width; j++)
         {

            BYTE * ps =(BYTE*)&((COLORREF*)b.bmBits)[i * r2.Width + j];
            BYTE * pd =(BYTE*)&((byte *)data.Scan0)[data.Stride * i + j* sizeof(COLORREF)];
            
            pd[0] =ps[0] * bA / 255;
            pd[1] =ps[1] * bA / 255;
            pd[2] =ps[2] * bA / 255;
            pd[3] =ps[3] * bA / 255;
            //pd[0] =pd[0] * bA / 255;
            //pd[1] =pd[1] * bA / 255;
            //pd[2] =pd[2] * bA / 255;
            //pd[3] =pd[3] * bA / 255;

         }

      }

   }

   pbitmap->UnlockBits(&data);

   return pbitmap;

}


Gdiplus::Bitmap * CPCDraftView::BitmapFromView()
{

   Gdiplus::Bitmap * pbitmap = NULL;

   CBitmap bmp;

   if(!BitmapFromView(bmp))
   {

      return NULL;

   }

   BITMAP b;

   bmp.GetBitmap(&b);

#undef new

   pbitmap = new Gdiplus::Bitmap((HBITMAP) bmp.GetSafeHandle(), NULL);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   //Gdiplus::Rect r2;

   //r2.x        = 0;

   //r2.y        = 0;

   //r2.Width    = pbitmap->GetWidth();

   //r2.Height   = pbitmap->GetHeight();

   //Gdiplus::BitmapData data;

   //pbitmap->LockBits(&r2,0,PixelFormat24bppRGB,&data);

   //for(int i = 0;i < r2.Height; i++)
   //{

   //   memcpy(&((byte *)data.Scan0)[data.Stride * i],&((COLORREF*)b.bmBits)[i * r2.Width],r2.Width * siz);

   //}

   //pbitmap->UnlockBits(&data);

   return pbitmap;

}


Gdiplus::Bitmap * CPCDraftView::BitmapFromDrawing()
{

   Gdiplus::Bitmap * pbitmap = NULL;

   CBitmap bmp;

   if (!BitmapFromDrawing(bmp))
   {

      return NULL;

   }

   BITMAP b;

   bmp.GetBitmap(&b);

#undef new

   pbitmap = new Gdiplus::Bitmap((HBITMAP)bmp.GetSafeHandle(), NULL);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   //Gdiplus::Rect r2;

   //r2.x        = 0;

   //r2.y        = 0;

   //r2.Width    = pbitmap->GetWidth();

   //r2.Height   = pbitmap->GetHeight();

   //Gdiplus::BitmapData data;

   //pbitmap->LockBits(&r2,0,PixelFormat24bppRGB,&data);

   //for(int i = 0;i < r2.Height; i++)
   //{

   //   memcpy(&((byte *)data.Scan0)[data.Stride * i],&((COLORREF*)b.bmBits)[i * r2.Width],r2.Width * siz);

   //}

   //pbitmap->UnlockBits(&data);

   return pbitmap;

}

CLibraryItem * CPCDraftView::LibraryItemFromSelection()
{

   CLibraryItem * pitem = new CLibraryItem();

   CRect & rDrag = pitem->m_rectDraw;

   pitem->m_pbitmapThumbnail = BitmapFromSelection(rDrag);

   int iMargin = 2;

   rDrag.InflateRect(iMargin,iMargin);

   CPCDraftDoc* pDocOld = GetDocument();

   pitem->m_pdoc = new CPCDraftDoc();

   pitem->m_pdoc->OnNewDocument();

   pitem->m_pdoc->m_packeditema.RemoveAll();

   CLayer* pLayer = pDocOld->GetActiveLayer();

   CLayer * player = pitem->m_pdoc->m_Layers[0];

   pitem->m_pdoc->m_nDefScale = pDocOld->m_nDefScale;
   pitem->m_pdoc->m_nDisplayRes = pDocOld->m_nDisplayRes;

   player->m_bGrayed = pLayer->m_bGrayed;
   player->m_bHidden = pLayer->m_bHidden;
   player->m_nScale = pLayer->m_nScale;
   player->m_nSnap = pLayer->m_nSnap;
   player->m_strName = pLayer->m_strName;

   //ar << pDoc->m_nDisplayRes << pLayer->m_nScale;

   LongPoint lptOrigin;

   if(m_Selection.GetCount()>1)
   {

      LongBoundRect lrcExtent;

      POSITION pos=m_Selection.GetHeadPosition();

      while(pos) lrcExtent|=(m_Selection.GetNext(pos))->GetExtents();

      lptOrigin=lrcExtent.GetCorner(0);

   }
   else lptOrigin=m_Selection.GetHead()->GetShapeDatum();

   CLinkVertexList theLinkVertexList;

   POSITION pos=pLayer->m_LinkVertexList.GetHeadPosition();

   while(pos)
   {

      CLinkVertex* pVertex=pLayer->m_LinkVertexList.GetNext(pos);

      if(IsSelectedShape(pVertex->m_pShape,true))
         theLinkVertexList.AddTail(new CLinkVertex(*pVertex));

   }

   theLinkVertexList.RemoveSinglePoints();

   CMDShapeList workList;
   CMDShapeList copyList;
   workList.AddTail(&m_Selection);
   pLayer->SortShapes(workList,false);

   // by TSM
   // Preparing palette entities for copiing using clipboard
   int used[2048];
   int i = 0;
   for(; i<2048; i++) used[i] = -1;

   while(workList.GetCount())
   {
      CMDShape* pShape=workList.RemoveHead();
      CMDShape* pNewShape=pShape->CreateCopy();
      pNewShape->DoShapeOffset(-lptOrigin);
      pNewShape->AdjustLinks(&theLinkVertexList,pShape);
      if(pNewShape->IsKindOf(RUNTIME_CLASS(CMDDimStr))) delete pNewShape;
      else
      {
         pNewShape->MakeSelfContained(pDocOld);
         pNewShape->ProcessShapesForLibraryArchiving(pitem->m_pdoc, pDocOld,&used[0]);
         copyList.AddHead(pNewShape);
      }
   }
   // by TSM
//   int usedCount=0;
//   for(i=0; i<2048; i++)
//      if(used[i]) usedCount++;
////   ar << usedCount;
//   for(i=0; i<2048; i++)
//      if(used[i])
//      {
//         pitem->m_packeditema.Add(pDoc->m_packeditema[i]);
//      }

//   ar << copyList.GetCount();
  // while(copyList.GetCount())
//   {
  //    CMDShape* pShape=copyList.RemoveHead();
    //  ar << pShape;
      //delete pShape;
   //}

   //theLinkVertexList.Serialize(ar);

   //ar.Close();
   //DWORD len=mf.GetLength();
   //BYTE* data=mf.Detach();

   //HANDLE hClip=GlobalAlloc(GMEM_DDESHARE,len);
   //CopyMemory(GlobalLock(hClip),data,len);
   //GlobalUnlock(hClip);
   //free(data);
   //return hClip;

   {

      POSITION pos = copyList.GetHeadPosition();

      while(pos != NULL)
      {

         player->m_ShapeList.AddTail(copyList.GetNext(pos));

      }

   }

   return pitem;

}


HCURSOR CPCDraftView::GetCursor(bool bUseModifiers)
{
   HCURSOR hCur;
   if(m_nOneTimeTool)
   {
      switch(m_nOneTimeTool)
      {
      case ID_TOOL_SELECT_HANDLE:
         hCur=AfxGetApp()->LoadCursor(IDC_ROTATE_ON_PT);
         break;
      case ID_TOOL_LINK:
      case ID_TOOL_UNLINK:
      case ID_TOOL_SELECT_LAYER:
      case ID_TOOL_SELECT_OBJECT:
      case ID_TOOL_FILLET:
         hCur=AfxGetApp()->LoadCursor(IDC_WHICH);
         break;
      case ID_TOOL_CHAMFER:
         hCur=AfxGetApp()->LoadCursor(m_Selection.GetCount()?IDC_SECOND:IDC_FIRST);
         break;
      case ID_TOOL_MIRROR:
      case ID_TOOL_DISTRIBUTE:
         hCur=AfxGetApp()->LoadCursor(IDC_SHAPE_DRAG);
         break;
      case ID_TOOL_EXTEND:
         hCur=AfxGetApp()->LoadCursor(m_nExtendTo?IDC_SHAPE_DRAG:IDC_WHICH);
         break;
      case ID_TOOL_PAN:
         hCur=AfxGetApp()->LoadCursor(IDC_HAND_DRAFT);
         break;
      case ID_TOOL_SELECT_REF_OBJECT:
         hCur=AfxGetApp()->LoadCursor(IDC_POINTER);
         break;
      case ID_TOOL_SELECT_REF_POINT:
         hCur=AfxGetApp()->LoadCursor(IDC_MID_CROSS);
         break;
      case ID_TOOL_DIM_NEXT_POINT:
         hCur=AfxGetApp()->LoadCursor(IDC_ROTATE_ON_PT);
         break;
      }
   }
   else
   {
      switch(m_nCurrentTool)
      {
      case ID_TOOL_ARROW:
      {
         if (m_bTrackingShape)
         {
            hCur = AfxGetApp()->LoadCursor(IDC_SMALL_CROSS);
         }
         else
         {
            //hCur = AfxGetApp()->LoadCursor(IDC_POINTER);
             hCur = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
         }
      }
         break;

      case ID_TOOL_PAN:
         hCur=AfxGetApp()->LoadCursor(IDC_HAND_DRAFT);
         break;

      case ID_TOOL_TEXT:
         hCur=AfxGetApp()->LoadCursor(IDC_TEXT);
         break;

      case ID_TOOL_PERP_LINE:
      case ID_TOOL_MIDPOINT_LINE:
      case ID_TOOL_ENDPOINT_LINE:
      case ID_TOOL_SHAPE_CENTER_LINE:
      case ID_TOOL_TANGENT_LINE:
      case ID_TOOL_CIRCLE_TANGENT_LINE:
      case ID_TOOL_OFFSET_LINE:
      case ID_TOOL_CIRCLE_CIRCLE:
      case ID_TOOL_ARC_ARC:
         hCur=AfxGetApp()->LoadCursor(IDC_MID_CROSS);
         break;

      case ID_TOOL_CONSTR_LINE:
      case ID_TOOL_CONSTR_LINE_5:
      case ID_TOOL_CONSTR_LINE_15:
      case ID_TOOL_CONSTR_LINE_30:
      case ID_TOOL_CONSTR_LINE_45:
      case ID_TOOL_CONSTR_LINE_90:

      case ID_TOOL_RECT:
      case ID_TOOL_RECT_CENTER:

      case ID_TOOL_RC_P:
      case ID_TOOL_RC_C:
      case ID_TOOL_RC_E:

      case ID_TOOL_CIRCLE_R:
      case ID_TOOL_CIRCLE_D:
      case ID_TOOL_CIRCLE_3:

      case ID_TOOL_ARC_R:
      case ID_TOOL_ARC_3:
      case ID_TOOL_ARC_E:

      case ID_TOOL_ELLIPSE:
      case ID_TOOL_ELLIPSE_CENTER:

      case ID_TOOL_CURVE_CUBIC:

      case ID_TOOL_POLY:
      case ID_TOOL_POLY_OPEN:

      case ID_TOOL_REG_POLY_CENTER_SIDE:
      case ID_TOOL_REG_POLY_VERTEX_VERTEX:
      case ID_TOOL_REG_POLY_SIDE_SIDE:
      case ID_TOOL_REG_POLY_CENTER_VERTEX:

      case ID_TOOL_PARALLEL_LINE:
      case ID_TOOL_PARALLEL_POLY:
      case ID_TOOL_PARALLEL_POLY_CLOSED:

      case ID_TOOL_STAR_DIAMETER: // Star Tool - Infinisys Ltd
      case ID_TOOL_STAR_CENTER_VERTEX: // Star Tool - Infinisys Ltd

         if(bUseModifiers)
         {
            if(GetAsyncKeyState(0x43)<0)	// VK_C
            {
               hCur=AfxGetApp()->LoadCursor(IDC_CENTER_SNAP);
               break;
            }
            else if(GetAsyncKeyState(0x45)<0)	// VK_E
            {
               hCur=AfxGetApp()->LoadCursor(IDC_END_SNAP);
               break;
            }
            else if(GetAsyncKeyState(0x4D)<0)	// VK_M
            {
               hCur=AfxGetApp()->LoadCursor(IDC_MID_SNAP);
               break;
            }
         }
         hCur=AfxGetApp()->LoadCursor(IDC_SMALL_CROSS);
         break;

      case ID_TOOL_MARQUEE:
      case ID_TOOL_MARQUEE2:
      case ID_TOOL_MARKER:
      case ID_TOOL_EDIT_DATUM:
         hCur=AfxGetApp()->LoadCursor(IDC_SMALL_CROSS);
         break;

      case ID_TOOL_CURVE:
      case ID_TOOL_CURVE_CLOSED:
      case ID_TOOL_CURVE_BEZIER:
         if(bUseModifiers && GetAsyncKeyState(VK_SHIFT)<0)
         {
            hCur=AfxGetApp()->LoadCursor(IDC_ERASER);
         }
         else
         {
            hCur=AfxGetApp()->LoadCursor(IDC_SMALL_CROSS);
         }
         break;

      case ID_TOOL_PARALLEL_LINE_EXTRUDE:
         hCur=AfxGetApp()->LoadCursor(IDC_EXTRUDE_PLINE);
         break;

      case ID_TOOL_PARALLEL_POLY_EXTRUDE:
         hCur=AfxGetApp()->LoadCursor(IDC_EXTRUDE_PPOLY);
         break;

      case ID_TOOL_ATTRIB_DROP:
         if(GetAsyncKeyState(VK_OPTION)<0) hCur=AfxGetApp()->LoadCursor(IDC_ATTRIB_PICK);
         else hCur=AfxGetApp()->LoadCursor(GetAsyncKeyState(VK_SHIFT)<0?IDC_ATTRIB_DROP_OPT:IDC_ATTRIB_DROP);
         break;

      case ID_TOOL_ZOOM:
         hCur=AfxGetApp()->LoadCursor(GetAsyncKeyState(VK_OPTION)<0?IDC_ZOOMOUT:IDC_ZOOMIN);
         break;

      case ID_TOOL_DIM_HORIZ:
      case ID_TOOL_DIM_VERT:
      case ID_TOOL_DIM_SLOPE:
      case ID_TOOL_DIM_PERP:
      case ID_TOOL_DIM_DIAMETER:
      case ID_TOOL_DIM_RADIUS:
      case ID_TOOL_DIM_CENTER:
      case ID_TOOL_DIM_ANGULAR:
         hCur=AfxGetApp()->LoadCursor(IDC_ROTATE_ON_PT);
         break;

      case ID_TOOL_HANDLE:
         hCur=AfxGetApp()->LoadCursor(GetAsyncKeyState(VK_OPTION)<0?IDC_HANDLE_REMOVE:IDC_HANDLE_SPLIT);
         break;

      case ID_TOOL_ROTATE:
         hCur=AfxGetApp()->LoadCursor(GetAsyncKeyState(VK_OPTION)<0?IDC_ROTATE_ON_PT:IDC_ROTATE);
         break;

      case ID_TOOL_REP_PASTE:
      case ID_TOOL_PLACE:
         hCur=AfxGetApp()->LoadCursor(IDC_REPEAT_PASTE);
         break;

      default:
         hCur=AfxGetApp()->LoadStandardCursor(IDC_NO);
         break;
      }
   }
   return hCur;
}

BOOL CPCDraftView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   if(nHitTest!=HTCLIENT) return CView::OnSetCursor(pWnd, nHitTest, message);

   CRect rcClient;
   GetActiveClientRect(rcClient);
   CPoint pt;
   GetCursorPos(&pt);
   ScreenToClient(&pt);
   if(!rcClient.PtInRect(pt))
   {
      SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
   else SetCursor(GetCursor());
   return TRUE;
}

void CPCDraftView::InvalidateSelection() // Gradient/Transparency - Infinisys Ltd
{

   POSITION pos = m_Selection.GetHeadPosition();

   while(pos != NULL)
   {

      CMDShape* pShape=m_Selection.GetNext(pos);

      InvalidateShape(pShape,true);

   }

}

void CPCDraftView::InvalidateShape(CMDShape *pShape, bool bSelected)
{
   LongRect lrc=pShape->GetExtents();
   pShape->ExtendForPen(lrc);
   lrc.Inset(-13,-13);
   CRect rc=m_Mapper.LPtoDP(lrc);
   if(bSelected)
   {
      rc.InflateRect(
      (kTrackHandleSize+1)>>1,
      (kTrackHandleSize+1)>>1);
   }
   else rc.InflateRect(1,1);
   rc.OffsetRect(-GetDeviceScrollPosition());
   InvalidateRect(&rc);
//	Invalidate(FALSE);
}

void CPCDraftView::DrawSelection(CDC *pDC)
{
   CDraftContext dc(pDC,&m_Mapper,GetDocument());
   POSITION pos=m_Selection.GetHeadPosition();

#undef new
   
   dc.m_PenPlus.reset(new Gdiplus::Pen(Gdiplus::Color::Black, 1.0));
   dc.m_PenBrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color::Black));
   dc.m_BrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color::Black));

   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      pShape->DrawHandles(&dc);
   }
}

void CPCDraftView::EnsureVisible(CMDShape* pShape)
{
   LongRect lrc;
   lrc=pShape->GetExtents();
   CPoint ptOrg=m_Mapper.LPtoDP(Middle(lrc));
   CSize szClientSize;
   GetClientSize(szClientSize);
   ptOrg.x-=szClientSize.cx/2;
   ptOrg.y-=szClientSize.cy/2;
   if(ptOrg.x<0)
      ptOrg.x=0;
   if(ptOrg.y<0)
      ptOrg.y=0;
   ScrollToDevicePosition(ptOrg);
}

bool CPCDraftView::SelectShape(CMDShape* pShape, bool bAddToList, bool bFast)
{
   ASSERT(pShape);
   if(bAddToList)
   {
      if(!bFast) // don't check - assume not selected
      {
         POSITION pos=m_Selection.Find(pShape);
         if(pos)	return false;
      }
   }
   else
   {
      POSITION pos=m_Selection.GetHeadPosition();
      while(!m_Selection.IsEmpty())
         InvalidateShape(m_Selection.RemoveHead(),true);
   }
   m_Selection.AddTail(pShape);
   InvalidateShape(pShape,true);
   if(!bFast) // don't check - assume will be added
   {
      pShape=pShape->GetAttachedShape();
      if(pShape && !m_Selection.Find(pShape))
      {
         m_Selection.AddTail(pShape);
         InvalidateShape(pShape,true);
      }
   }
//	UpdateWindow();
   ResetDupDelta();
   OnChangeSelection(); // Gradient/Transparency - Infinisys Ltd
   return true;
}


void CPCDraftView::UnselectShape(CMDShape *pShape,bool bAttached)
{
   POSITION pos=m_Selection.Find(pShape);
   if(pos)
   {
      m_Selection.RemoveAt(pos);
      InvalidateShape(pShape,true);
      // unselect area if attached
      if(bAttached)
      {
         pShape=pShape->GetAttachedShape();
         if(pShape) UnselectShape(pShape);
      }
      ResetDupDelta();
   }
}

// Changed by TSM - add bool return type
bool CPCDraftView::SketcherScroll(CPoint pt, BOOL processUpdates)
{
   static DWORD dwLastTicks=0;
   static BOOL checkScroll = FALSE;
   DWORD dwTicks=GetTickCount();
   if(!processUpdates) checkScroll = FALSE;
   if(checkScroll)
   {
      checkScroll = FALSE;
      return false;
   }
   if (abs((long)(dwTicks - dwLastTicks))<50)
   {
      checkScroll = TRUE;
      return false;
   }
   if(processUpdates) dwLastTicks=dwTicks;
   CPoint theCorner;
   CSize theSize;
   GetClientCorner(theCorner);
   GetClientSize(theSize);
   CRect rc(theCorner,theSize);

   // calc new x position
   int x=m_wndHorzBar.GetScrollPos();
   int xOrig=x;
   if(pt.x<rc.left)
   {
      x-=m_lineDev.cx;
   }
   else if(pt.x>rc.right)
   {
      x+=m_lineDev.cx;
   }

   // calc new y position
   int y=m_wndVertBar.GetScrollPos();
   int yOrig=y;
   if(pt.y<rc.top)
   {
      y-=m_lineDev.cy;
   }
   else if(pt.y>rc.bottom)
   {
      y+=m_lineDev.cy;
   }

   if(OnScrollBy(CSize(x-xOrig,y-yOrig),processUpdates))
   {
      //if(processUpdates) UpdateWindow();
      if(processUpdates) RedrawWindow();
      return true;
   }
   return false;
}

void CPCDraftView::DoNetSelect(const CPoint& pt, bool bOptionPressed, bool bAdjustCursor)
{
   CFlag cFlag;
   if(!(GetAsyncKeyState(VK_SHIFT)<0)) OnEditSelectNone();
   CRect rc(pt,pt);

   {
      LongRect lrc=m_Mapper.DPtoLP(rc);
      SelectShapesInRect(lrc,bOptionPressed,false);
   }

   CClientDC dc(this);
   CRect rcClient;
   GetActiveClientRect(rcClient);
   CRgn rgn;
   rgn.CreateRectRgnIndirect(&rcClient);
   dc.SelectClipRgn(&rgn);

   CPCDraftDoc* pDoc=GetDocument();
   CSize docSize(pDoc->m_NumPage.x*pDoc->m_PageSize.cx,pDoc->m_NumPage.y*pDoc->m_PageSize.cy);
   docSize.cx=pDoc->NoZoomToZoom(docSize.cx);
   docSize.cy=pDoc->NoZoomToZoom(docSize.cy);

   bool bMouseHasMoved=false;
   SetCapture();
   MSG msg;
   while(1)
   {
      LONG lIdle = 0;
      if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
      {
         lIdle = 0;

         CPoint pt,ptMsg=msg.pt;
         ScreenToClient(&ptMsg);

         switch(msg.message)
         {
         case WM_KEYDOWN:
            if(msg.wParam!=VK_ESCAPE)
               break;
         /* fall */
         case WM_RBUTTONDOWN:
            if(bMouseHasMoved)
            {
               ShowCursorPromptNoLock(false);
               ShowCrossCursor(false);
               CRect r(rc);
               r.NormalizeRect();
               r.right++; r.bottom++;
               dc.DrawFocusRect(&r);
               ShowCrossCursor();
               ShowCursorPromptNoLock();
            }
            ReleaseCapture();
            return;
         case WM_LBUTTONUP:
            if(bAdjustCursor) bOptionPressed=GetKeyState(VK_OPTION)<0;
            if(bMouseHasMoved)
            {
               ShowCursorPromptNoLock(false);
               ShowCrossCursor(false);
               CRect r(rc);
               r.NormalizeRect();
               r.right++; r.bottom++;
               dc.DrawFocusRect(&r);
               ShowCrossCursor();
               ShowCursorPromptNoLock();
            }
            rc.NormalizeRect();
            ReleaseCapture();
            if(bMouseHasMoved)
            {
               LongRect lrc=m_Mapper.DPtoLP(rc);
               SelectShapesInRect(lrc,bOptionPressed,false);
            }
            return;
         case WM_MOUSEMOVE:
            if(bAdjustCursor)
            {
               bOptionPressed=GetKeyState(VK_OPTION)<0;
               SetCursor(AfxGetApp()->LoadCursor(bOptionPressed?IDC_OPTION_HAND:IDC_POINTING_HAND));
            }
            AdjustCursor(ptMsg,true);

            pt=ptMsg+GetDeviceScrollPosition();
            if(pt.x>=docSize.cx) pt.x=docSize.cx-1;
            else if(pt.x<0) pt.x=0;
            // Infinisys Ltd. - Drag Select X was limited by height. (bug: pt.*x* >= docSize.cy)
            if(pt.y>=docSize.cy) pt.y=docSize.cy-1;
            else if(pt.y<0) pt.y=0;

            if(rc.BottomRight()!=pt)
            {
               ShowCursorPromptNoLock(false);
               ShowCrossCursor(false);
               if(bMouseHasMoved)
               {
                  CRect r(rc);
                  r.NormalizeRect();
                  r.right++; r.bottom++;
                  dc.DrawFocusRect(&r);
               }

               SketcherScroll(ptMsg);
               CPoint ptOrg=GetDeviceScrollPosition();
               dc.SetViewportOrg(-ptOrg);

               rc.BottomRight()=pt;

               CRect r(rc);
               r.NormalizeRect();
               r.right++; r.bottom++;
               dc.DrawFocusRect(&r);

               ShowCrossCursor();
               ShowCursorPromptNoLock();

               bMouseHasMoved=true;
            }
            break;
         default:
            DispatchMessage(&msg);
            break;
         }
      }
      else
      {
         // let MFC do its idle processing
         while(AfxGetApp()->OnIdle(lIdle++));

         CPoint pMsg;
         GetCursorPos(&pMsg);
         CPoint ptMsg(pMsg);
         ScreenToClient(&ptMsg);

         CPoint ptClientCorner;
         GetClientCorner(ptClientCorner);
         CSize szClientSize;
         GetClientSize(szClientSize);
         CRect rcClient(ptClientCorner,szClientSize);
         if(!rcClient.PtInRect(ptMsg))
         {
            ShowCursorPromptNoLock(false);
            ShowCrossCursor(false);
            {
               CRect r(rc);
               r.NormalizeRect();
               r.right++; r.bottom++;
               dc.DrawFocusRect(&r);
            }

            SketcherScroll(ptMsg);
            CPoint ptOrg=GetDeviceScrollPosition();
            dc.SetViewportOrg(-ptOrg);

            rc.BottomRight()=ptMsg;

            {
               CRect r(rc);
               r.NormalizeRect();
               r.right++; r.bottom++;
               dc.DrawFocusRect(&r);
            }
            ShowCrossCursor();
            ShowCursorPromptNoLock();

            WPARAM fwKeys=0;
            if(GetAsyncKeyState(VK_CONTROL)<0) fwKeys|=MK_CONTROL;
            if(GetAsyncKeyState(VK_LBUTTON)<0) fwKeys|=MK_LBUTTON;
            if(GetAsyncKeyState(VK_MBUTTON)<0) fwKeys|=MK_MBUTTON;
            if(GetAsyncKeyState(VK_RBUTTON)<0) fwKeys|=MK_RBUTTON;
            if(GetAsyncKeyState(VK_SHIFT)<0) fwKeys|=MK_SHIFT;
            PostMessage(WM_MOUSEMOVE,fwKeys,MAKELONG(ptMsg.x,ptMsg.y));
         }
      }
   }
}

void CPCDraftView::SelectNone()
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(!m_Selection.IsEmpty())
      InvalidateShape(m_Selection.RemoveHead(),true);
}

void CPCDraftView::OnEditSelectNone()
{
   if(m_bEditActive)
   {
      int nCaretChar=m_InplaceEdit.GetCaretPos();
      m_InplaceEdit.SetSel(nCaretChar,nCaretChar);
      m_InplaceEdit.Redraw(false);
   }
   else
   {
      SaveUndo(IDS_RESELECT);
      SelectNone();
   }
}

void CPCDraftView::OnUpdateHaveShapes(CCmdUI* pCmdUI)
{
   if(m_nCurrentTool==ID_TOOL_EDIT_DATUM) pCmdUI->Enable(FALSE);
   else
   {
      CPCDraftDoc* pDoc=GetDocument();
      CLayer* pActiveLayer=pDoc->GetActiveLayer();
      pCmdUI->Enable(pActiveLayer->m_ShapeList.GetCount());
   }
}

void CPCDraftView::OnEditSelectAll()
{
   if(m_bEditActive) m_InplaceEdit.SelectAll();
   else
   {
      SaveUndo(0);
      CPCDraftDoc* pDoc=GetDocument();
      CLayer* pActiveLayer=pDoc->GetActiveLayer();
      m_Selection.RemoveAll(); // fast select none
      POSITION pos=pActiveLayer->m_ShapeList.GetTailPosition();
      while(pos)
      {
         CMDShape *pShape=pActiveLayer->m_ShapeList.GetPrev(pos);
         if(!pDoc->m_bHideText || !pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
            SelectShape(pShape,true,true);
      }
   }
}

void CPCDraftView::DoMoveObject(LongPoint lpt,BOOL bMoveCopy,int nToLayer)
{
   BeginSaveUndo();
   UnselectShapes(usLocked);

   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pLayer=pDoc->GetActiveLayer();

   CLinkVertexList theLinkVertexList;
   POSITION pos=pLayer->m_LinkVertexList.GetHeadPosition();
   while(pos)
   {
      CLinkVertex* pVertex=pLayer->m_LinkVertexList.GetNext(pos);
      if(IsSelectedShape(pVertex->m_pShape,true))
         theLinkVertexList.AddTail(new CLinkVertex(*pVertex));
   }
   theLinkVertexList.RemoveSinglePoints();

   bool bLockedLinksWasDeleted=false;
   CLayer* pToLayer=pDoc->GetLayer(nToLayer);
   pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);

      if(bMoveCopy)
      {
         CMDShape* pNewShape=pShape->CreateCopy();
         pNewShape->AdjustLinks(&theLinkVertexList,pShape);
         pShape=pNewShape;
         pShape->CalculateExtents();
         pToLayer->m_ShapeList.AddHead(pShape);
      }
      else
      {
//				InvalidateShape(pShape,true);
         if(pToLayer!=pLayer)
         {
            UnselectShape(pShape);
            pLayer->m_ShapeList.RemoveAt(pLayer->m_ShapeList.Find(pShape));
            bLockedLinksWasDeleted|=pShape->DeleteLinks(&pLayer->m_LinkVertexList);
            pToLayer->m_ShapeList.AddHead(pShape);
         }
      }
      pShape->DoShapeOffset(lpt);
//			InvalidateShape(pShape,true);
   }

   UINT strId;
   if(bMoveCopy || RecalcLinkedVertexes())
   {
      if(pDoc->CheckDocSize(false))
      {
         if(bMoveCopy || pToLayer!=pLayer)
            pToLayer->m_LinkVertexList.AddTail(&theLinkVertexList);
         if(bLockedLinksWasDeleted)
            AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
         CommitSaveUndo(IDS_MOVE);
         ForceResizeUpdate();
         Invalidate(FALSE);
         return;
      }
      strId=IDS_CANT_MOVE;
   }
   else strId=IDS_UNABLE_COMPLETE_ACTION;
   AbortSaveUndo(true);
   AfxMessageBox(strId);
}

bool CPCDraftView::DoCalcMovePt(LongPoint& lpt)
{
   CPCDraftDoc* pDoc=GetDocument();
   bool bFirst=true;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && !pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)))
      {
         if(bFirst)
         {
            lpt=pShape->GetShapeDatum();
            bFirst=false;
         }
         else
         {
            LongPoint lptDatum=pShape->GetShapeDatum();
            if(lptDatum.x<=lpt.x && lptDatum.y<=lpt.y) lpt=lptDatum;
         }
      }
   }
   if(!bFirst)
   {
      lpt-=pDoc->m_RulerOrigin;
   }
   return !bFirst;
}

void CPCDraftView::OnEditMove()
{
   LongPoint lpt;
   if(!DoCalcMovePt(lpt))
      return;
   CMoveObjectDlg theDlg(this);
   theDlg.m_lptMove=lpt;
   if(theDlg.DoModal()==IDOK)
   {
      DoMoveObject(theDlg.m_lptMove,theDlg.m_bMoveCopy,theDlg.m_nToLayer);
   }
   ForceResizeUpdate();
}

void CPCDraftView::OnUpdateHaveSelection(CCmdUI* pCmdUI)
{
   if(m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool)
   {
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(!pShape->IsKindOf(RUNTIME_CLASS(CMDArea)))
         {
            pCmdUI->Enable();
            return;
         }
      }
   }
   pCmdUI->Enable(FALSE);
}

bool CPCDraftView::HaveUnlockedSelection()
{
   if(m_nOneTimeTool==ID_TOOL_FILLET) return false;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && !pShape->IsKindOf(RUNTIME_CLASS(CMDArea))) return true;
   }
   return false;
}

void CPCDraftView::OnUpdateHaveUnlockedSelection(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_nCurrentTool!=ID_TOOL_EDIT_DATUM && HaveUnlockedSelection());
}

void CPCDraftView::OnEditDupLinear()
{
   CPCDraftDoc* pDoc=GetDocument();
   LongRatio ratio;
   pDoc->GetActiveScaleRatio(ratio);

   CLinearDupDlg theDlg(pDoc);
   if(theDlg.DoModal()==IDOK)
   {
      SaveUndo(IDS_DUPLICATE);

      CMDShapeList workList;
      GetSortedSelection(workList,false);

      POSITION pos=workList.GetTailPosition();
      while(pos)
      {
         CMDShape* pShape=workList.GetPrev(pos);

         short					numHorzCopies, numVertCopies, newShapeCount;
         short					i, j;
         LongPoint				horzDupDeltas, vertDupDeltas;
         Angle angle;


         numHorzCopies = theDlg.m_Dir1Copies;
         numVertCopies = (theDlg.m_nDupType==1) ? theDlg.m_Dir2Copies:1;

         newShapeCount = numHorzCopies * numVertCopies;
         if (!(newShapeCount-1))
            return;

//	calculate the horizontal and vertical dupDeltas from the values in the parameter
//	block.
         if (theDlg.m_nDir1ParamType == 0) //????
         {
            theDlg.m_Dir1Value1.GetValue(horzDupDeltas.x);
            theDlg.m_Dir1Value2.GetValue(angle);
            horzDupDeltas.y = horzDupDeltas.x * mtSin(angle);
            horzDupDeltas.x *= mtCos(angle);
         }
         else if (theDlg.m_nDir1ParamType == 1)
         {
            theDlg.m_Dir1Value1.GetValue(horzDupDeltas.x);
            theDlg.m_Dir1Value2.GetValue(horzDupDeltas.y);
         }

         if (theDlg.m_nDupType==1)   // two direction
         {
            if (theDlg.m_nDir2ParamType == 0)
            {
               theDlg.m_Dir2Value1.GetValue(vertDupDeltas.x);
               theDlg.m_Dir2Value2.GetValue(angle);
               vertDupDeltas.y = vertDupDeltas.x * mtSin(angle);
               vertDupDeltas.x *= mtCos(angle);
            }
            else if (theDlg.m_nDir2ParamType == 1)
            {
               theDlg.m_Dir2Value1.GetValue(vertDupDeltas.x);
               theDlg.m_Dir2Value2.GetValue(vertDupDeltas.y);
            }
         }
         else
         {
            vertDupDeltas.x = 0;
            vertDupDeltas.y = 0;
         }

         for (j = 0; j < numVertCopies; j++)
         {
            for (i = 0; i < numHorzCopies; i++)
            {
               if (i==0 && j==0)
                  continue;
               CMDShape* pNewShape = pShape->CreateCopy();
               pNewShape->SetLocked(false);
               pNewShape->DoShapeOffset(vertDupDeltas*(j) + horzDupDeltas*(i));
               pDoc->CreateShape(pNewShape);
               SelectShape(pNewShape, true);
               InvalidateShape(pNewShape, true);
            }
         }
      }
      GetDocument()->CheckDocSize(true);
   }
}

void CPCDraftView::OnEditDupCircular()
{
   CPCDraftDoc* pDoc=GetDocument();
   LongRatio ratio;
   pDoc->GetActiveScaleRatio(ratio);

   CCircleDupDlg theDlg(pDoc);
   if(theDlg.DoModal()==IDOK)
   {
      LongPoint centerPt;
      if(theDlg.m_bUseMouse)
      {
         CRect rcClient;
         GetActiveClientRect(rcClient);
         CRefPointSketcher sketcher;
         if(!sketcher.DoSketch(this, CPoint(0,0),rcClient)) return;
         centerPt=sketcher.m_lptRef;
      }
      else
      {
         theDlg.m_CentreX.GetValue(centerPt.x);
         theDlg.m_CentreY.GetValue(centerPt.y);
      }

      SaveUndo(IDS_DUPLICATE);

      CMDShapeList workList;
      GetSortedSelection(workList,false);
      POSITION pos=workList.GetTailPosition();
      while(pos)
      {
         CMDShape* pShape=workList.GetPrev(pos);

         short     numCircleCopies, numCircles;
         LongPoint shapePt, deltaPt;
         short     i, j;
         Coord     workingRadius;
         Angle     workingAngle;
         Angle     incrementalAngle;

         numCircleCopies = theDlg.m_nCopies;
         numCircles = theDlg.m_nCircles;
         if (!(numCircleCopies * numCircles-1)) return;


         /* calculate the angle between shapes on the same circle.      */
         theDlg.m_Angle.GetValue(incrementalAngle);
         i = numCircleCopies;
         if ((theDlg.m_nDistribute == 0) && (numCircleCopies > 1))
         {
            if (incrementalAngle != k360) i--;
            incrementalAngle /= i;
         }
         /* calculate the radius and start angle between the circle center and  */
         /* the selected object point (object datum or object center).    */

         Coord radius,shift;
         theDlg.m_Radius.GetValue(radius);
         theDlg.m_Shift.GetValue(shift);


         workingAngle = 0;
         workingRadius = 0;

         deltaPt.x=0;
         deltaPt.y=0;
         for (j = 0; j < numCircles; j++)
         {
            for (i = 0; i < numCircleCopies; i++)
            {
               if (i==0 && j==0)
               {
                  workingAngle+=incrementalAngle;
                  continue;
               }
               /* calculate the next object position. */
               CMDShape* pNewShape = pShape->CreateCopy();
               pNewShape->SetLocked(false);
               LongPoint ptAbout=(theDlg.m_nByObject==1)?pNewShape->GetShapeDatum():Middle(pNewShape->GetExtents());
               LongPoint ptOffset=ptAbout;
               ptAbout=mtRotPoint(ptAbout,workingAngle,centerPt);
               ptOffset=ptAbout-ptOffset;
               pNewShape->DoShapeOffset(ptOffset);

               if(theDlg.m_bRotateCopies)
               {
                  pNewShape->DoShapeRotate(workingAngle,(theDlg.m_nByObject==1)?kAboutDatum:kAboutCenter,ptAbout);
                  pShape->ReCalc(&m_Mapper);
                  pShape->UpdateDependentShapes(this);
               }

               deltaPt.x = (ptAbout.x - centerPt.x)*workingRadius/Pythag(ptAbout - centerPt);
               deltaPt.y = (ptAbout.y - centerPt.y)*workingRadius/Pythag(ptAbout - centerPt);
               pNewShape->DoShapeOffset(deltaPt);

               workingAngle+=incrementalAngle;
               pDoc->CreateShape(pNewShape);
               SelectShape(pNewShape, true);
               InvalidateShape(pNewShape, true);
            } /* end of for each copy in a given circle */
            workingAngle=shift*(j+1);
            workingRadius += radius;
         } /* done with all circles or arcs */
      }
      GetDocument()->CheckDocSize(true);
   }
}

void CPCDraftView::OnDataObjectInfo()
{
   CPCDraftDoc* pDoc=GetDocument();
   CObjectInfoDlg theDlg(pDoc,&m_Selection);
   if(theDlg.DoModal()==IDOK) pDoc->SetModifiedFlag();
}

void CPCDraftView::OnToolDimFormat()
{
   CTypedPtrList<CPtrList,CMDAssocDim*> dims;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && !pShape->IsKindOf(RUNTIME_CLASS(CMDCenterDim)))
      {
         dims.AddTail((CMDAssocDim*)pShape);
      }
   }

   if(dims.GetCount())
   {
      CMDAssocDim* pDim=dims.RemoveHead();
      CEditDimFormatDlg theDlg(&pDim->m_DimFmt,&pDim->m_DimStd,GetDocument());
      BeginSaveUndo();
      if(theDlg.DoModal()==IDOK && theDlg.m_nChanges)
      {
         CommitSaveUndo(IDS_DIMFORMAT);
         InvalidateShape(pDim,true);
         pDim->ReCalc(&m_Mapper);
         pDim->UpdateDependentShapes(this);
         InvalidateShape(pDim,true);
         while(dims.GetCount())
         {
            CMDAssocDim* pOtherDim=dims.RemoveHead();
            if(pDim->m_DimFmt.nType==pOtherDim->m_DimFmt.nType)
            {
               pOtherDim->m_DimFmt.Modify(pDim->m_DimFmt,theDlg.m_nChanges);
               if(theDlg.m_nChanges&dfUseStandard) pOtherDim->m_DimStd=pDim->m_DimStd;
               InvalidateShape(pOtherDim,true);
               pOtherDim->ReCalc(&m_Mapper);
               pOtherDim->UpdateDependentShapes(this);
               InvalidateShape(pOtherDim,true);
            }
         }
      }
      else AbortSaveUndo(false);
      return;
   }

   if(m_Selection.GetCount())
   {
      MessageBeep(MB_ICONHAND);
      return;
   }

   int nType;
   switch(m_nCurrentTool)
   {
   case ID_TOOL_DIM_ANGULAR:
      nType=kAngular;
      break;
   case ID_TOOL_DIM_DIAMETER:
   case ID_TOOL_DIM_RADIUS:
      nType=kRadial;
      break;
   default:
      nType=kLinear;
      break;
   }
   CPCDraftDoc* pDoc=GetDocument();
   CDimFormatDlg(pDoc->m_DimStd.nType,nType,pDoc->m_DimFmts).DoModal();
}

void CPCDraftView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
   if(m_bEditActive) pCmdUI->Enable(m_InplaceEdit.CanCopyCut());
   else OnUpdateHaveUnlockedSelection(pCmdUI);
}

void CPCDraftView::DoEditClear()
{
   UnselectShapes(usLocked);
   while(m_Selection.GetCount())
   {
      CMDShape* pShape=m_Selection.RemoveHead();
      if(pShape->IsKindOf(RUNTIME_CLASS(CMDArea))) ((CMDArea*)pShape)->GetOwner()->Detach();
      else pShape->RemoveArea(this);
      InvalidateShape(pShape,true);
      GetDocument()->DeleteShape(pShape);
   }
}

void CPCDraftView::OnEditClear()
{
   if(m_bEditActive) m_InplaceEdit.DoDelete();
   else
   {
      SaveUndo(IDS_DELETE);
      DoEditClear();
   }
}

void CPCDraftView::OnArrangeBackward()
{
   SaveUndo(IDS_MOVEBACK);
   UnselectShapes(usLocked);
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   POSITION insertAfter=NULL;
   POSITION pos1,pos2=pLayer->m_ShapeList.GetTailPosition();
   while(pos2)
   {
      pos1=pos2;
      CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos2);
      if(m_Selection.Find(pShape))
      {
         if(insertAfter)
         {
            pLayer->m_ShapeList.RemoveAt(pos1);
            if(pos1 < insertAfter)
            {
               insertAfter--;
            }
            pLayer->m_ShapeList.InsertAfter(insertAfter,pShape);
            InvalidateShape(pShape,true);
         }
      }
      else
      {
         insertAfter=pos1;
      }
   }
}

void CPCDraftView::OnArrangeForward()
{
   SaveUndo(IDS_MOVEFORWARD);
   UnselectShapes(usLocked);
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   POSITION insertBefore=NULL;
   POSITION pos1,pos2=pLayer->m_ShapeList.GetHeadPosition();
   while(pos2)
   {
      pos1=pos2;
      CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos2);
      if(m_Selection.Find(pShape))
      {
         if(insertBefore)
         {
            pLayer->m_ShapeList.RemoveAt(pos1);
            if(pos1 < insertBefore)
            {
               insertBefore--;
            }
            pLayer->m_ShapeList.InsertBefore(insertBefore,pShape);
            InvalidateShape(pShape,true);
         }
      }
      else
      {
         insertBefore=pos1;
      }
   }
}

void CPCDraftView::OnArrangeToBack()
{
   SaveUndo(IDS_SENDTOBACK);
   UnselectShapes(usLocked);
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   POSITION insertAfter=NULL;
   POSITION pos1,pos2=pLayer->m_ShapeList.GetTailPosition();
   while(pos2)
   {
      pos1=pos2;
      CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos2);
      if(m_Selection.Find(pShape))
      {
         if(insertAfter)
         {
            pLayer->m_ShapeList.RemoveAt(pos1);
            if(pos1 < insertAfter)
            {
               insertAfter--;
            }
            pLayer->m_ShapeList.InsertAfter(insertAfter,pShape);
            InvalidateShape(pShape,true);
         }
      }
      else
      {
         if(insertAfter==NULL) insertAfter=pos1;
      }
   }
}

void CPCDraftView::OnArrangeToFront()
{
   SaveUndo(IDS_BRINGTOFRONT);
   UnselectShapes(usLocked);
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   POSITION insertBefore=NULL;
   POSITION pos1,pos2=pLayer->m_ShapeList.GetHeadPosition();
   while(pos2)
   {
      pos1=pos2;
      CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos2);
      if(m_Selection.Find(pShape))
      {
         if(insertBefore)
         {
            pLayer->m_ShapeList.RemoveAt(pos1);
            if(pos1 < insertBefore)
            {
               insertBefore--;
            }
            pLayer->m_ShapeList.InsertBefore(insertBefore,pShape);
            InvalidateShape(pShape,true);
         }
      }
      else
      {
         if(insertBefore==NULL) insertBefore=pos1;
      }
   }
}

void CPCDraftView::OnUpdateToolsRange(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(pCmdUI->m_nID!=ID_TOOL_ROTATE || m_nCurrentTool!=ID_TOOL_EDIT_DATUM);
   pCmdUI->SetRadio(pCmdUI->m_nID==m_nCurrentTool || (ID_TOOL_PLACE==m_nCurrentTool && ID_TOOL_REP_PASTE==pCmdUI->m_nID));
}

void CPCDraftView::SetArrowTool()
{
   ShowCursorPrompt(false);
   ShowCrossCursor(false);

   DeactivateTextShape();

   if(m_nCurrentTool==ID_TOOL_EDIT_DATUM && m_Selection.GetCount())
   {
      InvalidateShape(m_Selection.GetHead(),true);
      ForceResizeUpdate();
   }

   CancelCurrentTool();
   m_nCurrentTool=ID_TOOL_ARROW;
   m_nSnapMode=kSnap;
   SetCursorPrompt(0);

   ShowCrossCursor();
   ShowCursorPrompt();
}

void CPCDraftView::CancelCurrentTool()
{
   if(m_nOneTimeTool)
   {
      ShowCursorPrompt(false);
      ShowCrossCursor(false);

      CancelMouseTracking(false);

      if(m_nOneTimeTool==ID_TOOL_FILLET)
      {
         m_pFilletCtx->ClearFillet();
         Invalidate();
      }
      if(m_nOneTimeTool==ID_TOOL_SELECT_LAYER)
      {
         InvalidateLayerBar();
      }
      m_nOneTimeTool=0;
      SetCursorPrompt(m_nSavedPrompt);
      m_nSnapMode=m_nSavedSnapMode;

      ShowCrossCursor();
      ShowCursorPrompt();
   }
}

void CPCDraftView::SetOneTimeTool(UINT nID,UINT nPrompt,int nSnapMode)
{
   ShowCursorPrompt(false);
   ShowCrossCursor(false);

   DeactivateTextShape();
   CancelCurrentTool();

   m_nSavedPrompt=m_nPrompt;
   m_nSavedSnapMode=m_nSnapMode;

   m_nOneTimeTool=nID;
   if(m_nOneTimeTool==ID_TOOL_SELECT_LAYER)
   {
      InvalidateLayerBar();
   }
   SetCursorPrompt(nPrompt);
   m_nSnapMode=nSnapMode;

   ShowCrossCursor();
   ShowCursorPrompt();
}

void CPCDraftView::OnToolsRange(UINT nID)
{
   m_Mistakes.Reset();

   if (nID == ID_TOOL_ARROW && GetAsyncKeyState(VK_OPTION) < 0)
   {
      Invalidate();
      return;
   }

   ShowCursorPrompt(false);
   ShowCrossCursor(false);

   if (nID != ID_TOOL_TEXT)
   {

      DeactivateTextShape();

   }

   if((m_nCurrentTool==ID_TOOL_EDIT_DATUM || nID==ID_TOOL_EDIT_DATUM) && m_Selection.GetCount())
   {
      InvalidateShape(m_Selection.GetHead(),true);
      ForceResizeUpdate();
   }

   CancelCurrentTool();

   m_nCurrentTool=nID;
   m_nSnapMode=kSnap;

   SetCursorPrompt(0);

   switch(nID)
   {
   case ID_TOOL_ARROW:
      if(GetAsyncKeyState(VK_SHIFT)<0) m_nCurrentTool=ID_TOOL_ZOOM;
      break;

   case ID_TOOL_ATTRIB_DROP:
      if(GetAsyncKeyState(VK_OPTION)<0) OnEditAttribOptions();
      break;

   case ID_TOOL_PERP_LINE:
   case ID_TOOL_MIDPOINT_LINE:
   case ID_TOOL_ENDPOINT_LINE:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_CLICKLINEARC);
      break;

   case ID_TOOL_SHAPE_CENTER_LINE:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_CLICKOBJECT);
      break;

   case ID_TOOL_TANGENT_LINE:
   case ID_TOOL_DIM_DIAMETER:
   case ID_TOOL_DIM_RADIUS:
   case ID_TOOL_DIM_CENTER:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_CLICKCIRCLEARC);
      break;

   case ID_TOOL_CIRCLE_TANGENT_LINE:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_1CIRCLEARC);
      break;

   case ID_TOOL_OFFSET_LINE:
   case ID_TOOL_DIM_PERP:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_CLICKLINEEDGE);
      break;

   case ID_TOOL_CIRCLE_CIRCLE:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_CLICKCIRCLE);
      break;

   case ID_TOOL_ARC_ARC:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_CLICKARC);
      break;

   case ID_TOOL_DIM_HORIZ:
   case ID_TOOL_DIM_VERT:
   case ID_TOOL_DIM_SLOPE:
      SetCursorPrompt(IDS_1POINT);
      break;

   case ID_TOOL_DIM_ANGULAR:
      m_nSnapMode=kNoSnap;
      SetCursorPrompt(IDS_1LINEEDGE);
      break;

   case ID_TOOL_PARALLEL_LINE:
   case ID_TOOL_PARALLEL_POLY:
   case ID_TOOL_PARALLEL_POLY_CLOSED:
   case ID_TOOL_PARALLEL_LINE_EXTRUDE:
   case ID_TOOL_PARALLEL_POLY_EXTRUDE:
      if(GetAsyncKeyState(VK_OPTION)<0) OnPrefToolOptions();
      else SetCursorPrompt(IDS_EMPTYPROMPT);
      break;

   case ID_TOOL_REG_POLY_CENTER_SIDE:
   case ID_TOOL_REG_POLY_CENTER_VERTEX:
   case ID_TOOL_REG_POLY_SIDE_SIDE:
   case ID_TOOL_REG_POLY_VERTEX_VERTEX:
      if(GetAsyncKeyState(VK_OPTION)<0) OnPrefToolOptions();
      else SetCursorPrompt(IDS_EMPTYPROMPT);
      break;
   case ID_TOOL_STAR_DIAMETER: // Star Tool - Infinisys Ltd
   case ID_TOOL_STAR_CENTER_VERTEX: // Star Tool - Infinisys Ltd
      if(GetAsyncKeyState(VK_OPTION)<0) OnPrefToolOptions(); // Star Tool - Infinisys Ltd
      else SetCursorPrompt(IDS_EMPTYPROMPT); // Star Tool - Infinisys Ltd
      break;

   case ID_TOOL_MARQUEE:
   case ID_TOOL_MARQUEE2:
      m_nSnapMode=kNoSnap;

   case ID_TOOL_CONSTR_LINE:
   case ID_TOOL_CONSTR_LINE_5:
   case ID_TOOL_CONSTR_LINE_15:
   case ID_TOOL_CONSTR_LINE_30:
   case ID_TOOL_CONSTR_LINE_45:
   case ID_TOOL_CONSTR_LINE_90:

   case ID_TOOL_RECT:
   case ID_TOOL_RECT_CENTER:

   case ID_TOOL_RC_P:
   case ID_TOOL_RC_C:
   case ID_TOOL_RC_E:

   case ID_TOOL_CIRCLE_R:
   case ID_TOOL_CIRCLE_D:
   case ID_TOOL_CIRCLE_3:

   case ID_TOOL_ARC_R:
   case ID_TOOL_ARC_3:
   case ID_TOOL_ARC_E:

   case ID_TOOL_ELLIPSE:
   case ID_TOOL_ELLIPSE_CENTER:

   case ID_TOOL_CURVE:
   case ID_TOOL_CURVE_CLOSED:
   case ID_TOOL_CURVE_BEZIER:
   case ID_TOOL_CURVE_CUBIC:

   case ID_TOOL_POLY:
   case ID_TOOL_POLY_OPEN:

   case ID_TOOL_MARKER:
   case ID_TOOL_EDIT_DATUM:
      SetCursorPrompt(IDS_EMPTYPROMPT);
      break;
   }

   ShowCrossCursor();
   ShowCursorPrompt();
   ((CMainFrame*)AfxGetMainWnd())->UpdatePallettesForCommand(ID_TOOL_PLACE==nID?ID_TOOL_REP_PASTE:nID);
}

void CPCDraftView::OnArrangeRotate()
{
   OnToolsRange(ID_TOOL_ROTATE);
}

void CPCDraftView::OnArrangeRotateOpt()
{
   Angle angle=0;
   bool bSingle=m_Selection.GetCount()==1;
   if(bSingle) angle=m_Selection.GetHead()->GetRotation();
   CRotateOptionsDlg theDlg(angle,GetDocument());
   if(theDlg.DoModal()==IDOK)
   {
      LongPoint lptRef;

      if(theDlg.m_nAbout==kAboutRefPt)
      {
         CPoint ptClientCorner; CSize szClientSize;
         GetClientCorner(ptClientCorner);
         GetClientSize(szClientSize);
         CRect rcClient(ptClientCorner,szClientSize);
         CRefPointSketcher sketcher;
         if(!sketcher.DoSketch(this, CPoint(0,0),rcClient)) return;
         lptRef=sketcher.m_lptRef;
      }

      BeginSaveUndo();
      UnselectShapes(usLocked);

      if(bSingle)	angle=mtFixAngle(theDlg.m_bClockwise?-theDlg.m_Angle:theDlg.m_Angle)-angle;
      else angle=mtFixAngle(theDlg.m_bClockwise?-theDlg.m_Angle:theDlg.m_Angle);

      if(DoShapeRotate(angle,theDlg.m_nAbout,lptRef))
      {
         CommitSaveUndo(IDS_ROTATE);
         ForceResizeUpdate();
      }
      else
      {
         AbortSaveUndo(true);
         AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
      }
   }
}

void CPCDraftView::OnEditRepeatPaste()
{
   OnToolsRange(ID_TOOL_REP_PASTE);
}

void CPCDraftView::OnUpdateBorderPositionRange(CCmdUI* pCmdUI)
{
   BYTE nAlign=(pCmdUI->m_nID-ID_PREF_BORDER_LINE_INSIDE);
   switch(m_Selection.GetCount())
   {
   case 0:
      pCmdUI->SetCheck(GetDocument()->m_Pen.m_nAlignment==nAlign);
      break;
   case 1:
   {
      CMDShape* pShape=m_Selection.GetHead();
      pCmdUI->SetCheck(!pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)) && pShape->GetPenModel().m_nAlignment==nAlign);
   }
   break;
   default:
   {
      CMDShape* pShape = m_Selection.GetHead();
      BYTE nAlignment = pShape->GetPenModel().m_nAlignment;

      POSITION pos = m_Selection.GetHeadPosition();
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         if (nAlignment != pShape->GetPenModel().m_nAlignment)
         {
            nAlignment = GetDocument()->m_Pen.m_nAlignment;
            break;
         }
      }

      pCmdUI->SetCheck(nAlign == nAlignment);
      break;
   }
   }
}

void CPCDraftView::OnBorderPositionRange(UINT nID)
{
   SaveUndo(IDS_PENCHANGE);
   PenModel pm;
   pm.m_nAlignment=(nID-ID_PREF_BORDER_LINE_INSIDE);
   ApplyPen(pm,pmUseAlign);
}

void CPCDraftView::OnUpdateTextHidetext(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bHideText);
}

void CPCDraftView::OnTextHidetext()
{
   SaveUndo(0);
   CPCDraftDoc* pDoc=GetDocument();
   pDoc->m_bHideText=!pDoc->m_bHideText;
   if(pDoc->m_bHideText)
   {
      if(m_bEditActive) DeactivateTextShape();
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         POSITION rpos=pos;
         if(m_Selection.GetNext(pos)->IsKindOf(RUNTIME_CLASS(CMDText)))
         {
            m_Selection.RemoveAt(rpos);
         }
      }
   }
   Invalidate(FALSE);
}

void CPCDraftView::DropShapes(HANDLE hDrop, CPoint *pt)
{
   SaveUndo(IDS_DUPLICATE);
   SelectNone();
   ScreenToClient(pt);
   *pt+=GetDeviceScrollPosition();
   CreateShapesFromHandle(hDrop,GetAsyncKeyState(VK_SHIFT)<0,pt,false);
   GetDocument()->CheckDocSize(true);
}

bool CPCDraftView::DoShapesDrag(CPoint pt, bool bSingle)
{
   CFlag cFlag;
   bool bMouseHasMoved=false;
   bool bCopy=GetAsyncKeyState(VK_OPTION)<0;

   CPoint ptMsg;

   CWnd* pDragWnd=AfxGetMainWnd();
   CClientDC dc(pDragWnd);
   dc.SetROP2(R2_NOT);
   CGDIBackMode bm(&dc,TRANSPARENT);

   POSITION pos=m_Selection.GetTailPosition();
   CMDShape* pShape=m_Selection.GetPrev(pos);
   LongRect lrcExtent=pShape->CalculateBasicExtents();
   if(!bSingle)
   {
      while(pos)
      {
         pShape=m_Selection.GetPrev(pos);
         lrcExtent|=pShape->CalculateBasicExtents();
      }
   }

   SetCapture();
   SetCursor(AfxGetApp()->LoadCursor(IDC_HAND_DRAFT));

   CRect rcDrag=m_Mapper.LPtoDP(lrcExtent);
   rcDrag.InflateRect(0,0,1,1);
   ClientToScreen(&rcDrag);
   pDragWnd->ScreenToClient(&rcDrag);

   CPoint ptNew,ptOld=pt;
   ClientToScreen(&ptOld);
   CPoint p(0, 0);
   pDragWnd->ScreenToClient(&p);
   ptOld += p;
   CPoint ptOrig=ptOld;

   CPCDraftView* pTargetView=NULL;
   while(1)
   {
      MSG msg;
      LONG lIdle = 0;
      if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
      {
         lIdle = 0;
         switch(msg.message)
         {
         case WM_CAPTURECHANGED:
         case WM_CANCELMODE:
         case WM_RBUTTONDOWN:
cancelMove:
            dc.DrawFocusRect(&rcDrag);
            ReleaseCapture();
            SetCursor(AfxGetApp()->LoadCursor(IDC_POINTER));
            return false;
         case WM_LBUTTONUP:
            if(bMouseHasMoved)
            {
               dc.DrawFocusRect(&rcDrag);
               if(pTargetView)
               {
                  if(pTargetView==this)
                  {
                     CPoint ptOffset=ptOld-ptOrig+GetDeviceScrollPosition();
                     LongPoint lptOffset=LongPoint(m_Mapper.DPtoLP(ptOffset.x),m_Mapper.DPtoLP(ptOffset.y));
                     if(bCopy)
                     {
                        HANDLE hDup=SelectionToHandle();
                        SelectNone();
                        CreateShapesFromHandle(hDup,false,&ptOffset,true);
                     }
                     else OffsetSelection(lptOffset,bSingle);
                     if(m_bAdjustDupDelta)
                        m_lptDupDelta+=lptOffset;
                  }
                  else
                  {
                     CPoint ptOffset=rcDrag.TopLeft();
                     CPoint p(0,0);
                     pDragWnd->ClientToScreen(&p);
                     ptOffset += p;
                     pTargetView->DropShapes(SelectionToHandle(true),&ptOffset);
                  }
               }
            }
            ReleaseCapture();
            SetCursor(AfxGetApp()->LoadCursor(IDC_POINTER));
            return bMouseHasMoved;

         case WM_KEYDOWN:
            if(msg.wParam==VK_ESCAPE) goto cancelMove;
            break;
         case WM_MOUSEMOVE:
         {
            ptNew = msg.pt;
            CPoint p(0, 0);
            pDragWnd->ScreenToClient(&p);
            ptNew += p;
            if (ptNew != ptOld)
            {
               if (bMouseHasMoved)
                  dc.DrawFocusRect(&rcDrag);
               rcDrag.OffsetRect(ptNew.x - ptOld.x, ptNew.y - ptOld.y);
               dc.DrawFocusRect(&rcDrag);
               ptOld = ptNew;
               bMouseHasMoved = true;
               CWnd* pWnd = CWnd::WindowFromPoint(msg.pt);
               if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
               {
                  if (pTargetView != (CPCDraftView*)pWnd)
                  {
                     pTargetView = (CPCDraftView*)pWnd;
                  }
               }
               else if (pTargetView)
               {
                  pTargetView = NULL;
               }
            }
         }
            break;
         default:
            break;
         }
      }
      else
      {
         // let MFC do its idle processing
         while(AfxGetApp()->OnIdle(lIdle++));
      }
   }
}

// move selected shapes
bool CPCDraftView::DoShapesMove(CPoint pt, bool bSingle)
{
   CFlag cFlag;
   bool bMouseHasMoved=false;
   bool bExtentFeedback=(GetAsyncKeyState(VK_COMMAND)<0);
   if(bExtentFeedback)
      return DoShapesDrag(pt,bSingle);
   bool bCopy=(bExtentFeedback && GetAsyncKeyState(VK_OPTION)<0);

   CPoint ptMsg;

   CRect rcClient;
   GetActiveClientRect(rcClient);
   CRgn rgn;
   rgn.CreateRectRgnIndirect(&rcClient);
   CClientDC dc(this);
   dc.SelectClipRgn(&rgn);
//   dc.SetROP2(R2_NOT);
   CGDIBackMode bm(&dc,TRANSPARENT);

   LongPoint ptOrig, ptOld, ptNew, ptOffset;
   ptOrig=ptOld=m_Mapper.DPtoLP(pt);
   ptOffset=ptOrig;
   SnapLPt(ptOffset);
   ptOffset-=ptOrig;

//	UnselectShapes(usLocked);

   SetCapture();
   SetCursor(AfxGetApp()->LoadCursor(bExtentFeedback?IDC_HAND_DRAFT:IDC_SHAPE_DRAG));


   CPoint ptOrg=GetDeviceScrollPosition();
   //dc.SetViewportOrg(-ptOrg);
   CBitmap bmp;
	//TrackSelectionWireFrame(&dc,LongPoint(0,0),bExtentFeedback,bSingle);

   int iDrawSelectionMargin = 0;

   CRect rectSelection;
   std::unique_ptr<Gdiplus::Bitmap> pbitmap;
   pbitmap.reset(BitmapFromSelection(rectSelection,0.5,iDrawSelectionMargin));
   std::unique_ptr<Gdiplus::Bitmap> pbitmapView;
   pbitmapView.reset(BitmapFromView());
   std::unique_ptr<Gdiplus::Bitmap> pbitmapDraw;
   pbitmapDraw.reset(BitmapFromView());
   Gdiplus::Graphics gDraw(pbitmapDraw.get());

   CPoint ptShift;
   
   ptShift = pt;
   
   ptShift -= rectSelection.TopLeft();

   int nConstraint=0;
   SetTimer(4321234,100,NULL);
   while(1)
   {
      MSG msg;
      LONG lIdle = 0;
      if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
      {
         lIdle = 0;
         switch(msg.message)
         {
         case WM_CAPTURECHANGED:
         case WM_CANCELMODE:
         case WM_RBUTTONDOWN:
         cancelMove:
         {
            CPoint ptOrg=GetDeviceScrollPosition();
            dc.SetViewportOrg(-ptOrg);

            //TrackSelectionWireFrame(&dc,ptOld-ptOrig,bExtentFeedback,bSingle);
            {
  
               Gdiplus::Graphics g(dc.GetSafeHdc());
               g.DrawImage(pbitmapView.get(),(FLOAT)ptOrg.x,(FLOAT) ptOrg.y);
               //CPoint pt = m_Mapper.LPtoDP(ptOld);
               //g.DrawImage(pbitmap.get(),pt.x - ptShift.x,pt.y - ptShift.y);
            }
            ReleaseCapture();
            SetCursor(AfxGetApp()->LoadCursor(IDC_POINTER));
            ClearShowSize();
         }
            return false;
         case WM_LBUTTONUP:
            if(bMouseHasMoved)
            {
               CPoint ptOrg=GetDeviceScrollPosition();
               dc.SetViewportOrg(-ptOrg);
               //TrackSelectionWireFrame(&dc,ptOld-ptOrig,bExtentFeedback,bSingle);
               {
                  Gdiplus::Graphics g(dc.GetSafeHdc());
                  //g.DrawImage(pbitmapView.get(),0, 0);
                  g.DrawImage(pbitmapDraw.get(), (LONG)ptOrg.x, (LONG)ptOrg.y);
                  //CPoint pt = m_Mapper.LPtoDP(ptOld);
                  //g.DrawImage(pbitmap.get(),pt.x - ptShift.x,pt.y - ptShift.y);
               }
               LongPoint lptOffset=ptOld-ptOrig;
               if(bCopy)
               {
                  HANDLE hDup=SelectionToHandle();
                  SelectNone();
                  CPoint ptOffset=CPoint(m_Mapper.LPtoDP(lptOffset.x),m_Mapper.LPtoDP(lptOffset.y));
                  CreateShapesFromHandle(hDup,true,&ptOffset,true);
               }
               else OffsetSelection(lptOffset,bSingle);
               if(m_bAdjustDupDelta)
                  m_lptDupDelta+=lptOffset;
            }
            ReleaseCapture();
            SetCursor(AfxGetApp()->LoadCursor(IDC_POINTER));
            ClearShowSize();
            return bMouseHasMoved;

         case WM_KEYDOWN:
            if(msg.wParam==VK_ESCAPE) goto cancelMove;
         case WM_KEYUP:
         case WM_MOUSEMOVE:
         case WM_TIMER:
         {
            if(msg.message == WM_TIMER)
            {
               if(msg.wParam != 4321234)
               {
                  break;
               }
            }
            ScreenToClient(&msg.pt);
            ptMsg=msg.pt;
            AdjustCursor(ptMsg,true);
            ptMsg+=GetDeviceScrollPosition();
            ptNew=m_Mapper.DPtoLP(ptMsg);
            if(GetAsyncKeyState(VK_COMMAND) >= 0)
            {
               SnapLPt(ptNew,true);
               ptNew-=ptOffset;
            }
            if(/*ptNew!=ptOld && */PtInDocument(ptNew))
            {


               int nType=0;
               if(GetAsyncKeyState(0x43) < 0) nType=IDC_CENTER_SNAP;		// VK_C
               else if(GetAsyncKeyState(0x45) < 0) nType=IDC_END_SNAP;	// VK_E
               else if(GetAsyncKeyState(0x4D) < 0) nType=IDC_MID_SNAP;	// VK_M
               if(nType) SetCursor(AfxGetApp()->LoadCursor(nType));
               else
               {
                  if(GetAsyncKeyState(VK_SHIFT) < 0)
                  {
                     if(nConstraint == 0)
                     {
                        LongPoint offset=Abs(ptNew - ptOrig);
                        if(offset.x > 0 || offset.y > 0)
                        {
                           nConstraint=offset.x < offset.y?1:2;
                           SetCursor(AfxGetApp()->LoadCursor(nConstraint == 2?IDC_HCONSTRAIN:IDC_VCONSTRAIN));
                        }
                     }
                     switch(nConstraint)
                     {
                     case 1:
                        ptNew.x=ptOrig.x;
                        break;
                     case 2:
                        ptNew.y=ptOrig.y;
                        break;
                     }
                  }
                  else
                  {
                     if(nConstraint) nConstraint=0;
                     SetCursor(AfxGetApp()->LoadCursor(bExtentFeedback?IDC_HAND_DRAFT:IDC_SHAPE_DRAG));
                  }
               }
            }
            ptMsg=m_Mapper.LPtoDP(ptNew);
            CPoint ptMsgOrig=m_Mapper.LPtoDP(ptOrig);
            if(SketcherScroll(msg.pt))
            {
               {
                  GetActiveClientRect(rcClient);
                  CRgn rgn;
                  rgn.CreateRectRgnIndirect(&rcClient);
                  dc.SelectClipRgn(&rgn);

               }
               //CPoint ptOrg = GetDeviceScrollPosition();
               //dc.SetViewportOrg(-ptOrg);
               //UpdateWindow();
               pbitmapView.reset(BitmapFromView());
            }

            //LongPoint lptMove;

            //lptMove = ptNew - ptOrig;

            bool bSnapped = false;

            if (GetDocument()->m_bSnapToGuides)
            {

               bSnapped = SnapSelectionToGuide(ptNew, ptOrig, nullptr);

            }

            if(ptNew != ptOld && PtInDocument(ptNew))
            {
               //if(bMouseHasMoved)TrackSelectionWireFrame(&dc,ptOld-ptOrig,bExtentFeedback,bSingle);
               CPoint ptOrg=GetDeviceScrollPosition();
               dc.SetViewportOrg(-ptOrg);

               //TrackSelectionWireFrame(&dc,ptNew-ptOrig,bExtentFeedback,bSingle);
               {
                  Gdiplus::Graphics g(dc.GetSafeHdc());
                  gDraw.DrawImage(pbitmapView.get(),0,0);
                  CPoint pt = m_Mapper.LPtoDP(ptNew);
                  if(!pbitmap)
                  {
                     //DrawSelection(rectSelection, gDraw,pt.x - ptShift.x - ptOrg.x,pt.y - ptShift.y - ptOrg.y,iDrawSelectionMargin);
                     //DrawSelection(rectSelection, gDraw, pt.x - ptShift.x - ptOrg.x, pt.y - ptShift.y - ptOrg.y, iDrawSelectionMargin);
                     DrawSelection(rectSelection, gDraw, pt.x - ptShift.x - ptOrg.x, pt.y - ptShift.y - ptOrg.y, iDrawSelectionMargin);
                  }
                  else
                  {
                     //gDraw.DrawImage(pbitmap.get(),(LONG) (pt.x - ptShift.x - ptOrg.x),(LONG) (pt.y - ptShift.y - ptOrg.y));
                     gDraw.DrawImage(pbitmap.get(), (LONG)(pt.x - ptShift.x - ptOrg.x), (LONG)(pt.y - ptShift.y - ptOrg.y));
                  }
                  g.DrawImage(pbitmapDraw.get(),(LONG)ptOrg.x,(LONG)ptOrg.y);
                  //g.DrawImage(pbitmapDraw.get(), 0, 0);
                  //g.DrawImage(pbitmap.get(),pt.x - ptShift.x,pt.y - ptShift.y);

               }
               ptOld=ptNew;
               bMouseHasMoved=true;
               SetShowSize(0,1,Pythag(ptNew - ptOrig));
               SetShowSize(1,-1,mtAngle(ptOrig,ptNew));
            }
         }
            break;
         default:
            DispatchMessage(&msg);
            break;
         }
      }
      else
      {
         // let MFC do its idle processing
         while(AfxGetApp()->OnIdle(lIdle++));

  //       CPoint ptMsg;
  //       GetCursorPos(&ptMsg);
  //       ScreenToClient(&ptMsg);

  //       CPoint ptClientCorner;
  //       GetClientCorner(ptClientCorner);
  //       CSize szClientSize;
  //       GetClientSize(szClientSize);
  //       CRect rcClient(ptClientCorner,szClientSize);
  //       if(!rcClient.PtInRect(ptMsg))
  //       {
  ////          TrackSelectionWireFrame(&dc,ptOld-ptOrig,bExtentFeedback,bSingle);
  //          if(SketcherScroll(ptMsg))
  //          {
  //             {
  //                GetActiveClientRect(rcClient);
  //                CRgn rgn;
  //                rgn.CreateRectRgnIndirect(&rcClient);
  //                dc.SelectClipRgn(&rgn);

  //             }

  //             pbitmapView.reset(BitmapFromView());
  //          }
  //          CPoint ptOrg=GetDeviceScrollPosition();
  //          dc.SetViewportOrg(-ptOrg);
  //          //            TrackSelectionWireFrame(&dc,ptOld-ptOrig,bExtentFeedback,bSingle);

  //          {
  //             Gdiplus::Graphics g(dc.GetSafeHdc());
  //             g.DrawImage(pbitmapView.get(),ptOrg.x,ptOrg.y);
  //             CPoint pt = m_Mapper.LPtoDP(ptOld);
  //             g.DrawImage(pbitmap.get(),pt.x - ptShift.x,pt.y - ptShift.y);

  //          }

            WPARAM fwKeys=0;
            if(GetAsyncKeyState(VK_CONTROL)<0) fwKeys|=MK_CONTROL;
            if(GetAsyncKeyState(VK_LBUTTON)<0) fwKeys|=MK_LBUTTON;
            if(GetAsyncKeyState(VK_MBUTTON)<0) fwKeys|=MK_MBUTTON;
            if(GetAsyncKeyState(VK_RBUTTON)<0) fwKeys|=MK_RBUTTON;
            if(GetAsyncKeyState(VK_SHIFT)<0) fwKeys|=MK_SHIFT;
            //PostMessage(WM_MOUSEMOVE,fwKeys,MAKELONG(ptMsg.x,ptMsg.y));
         //}
      }
   }
}
// former DrawSelectionOutline
void CPCDraftView::TrackSelectionWireFrame(CDC *pDC, LongPoint delta, bool bExtentFeedback, bool bSingle)
{
   CDraftContext dc(pDC,&m_Mapper,GetDocument());
   dc.SetOutlinePen(false);

   POSITION pos=m_Selection.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetPrev(pos);
      if(!pShape->IsLocked() && (!pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) || CanMoveDimension(pShape)))
      {
         CPen pen(PS_DOT,1,RGB(0,0,0));
         CGDIObject<CPen> oldPen(pDC,&pen);
         if(bExtentFeedback)
         {
            LongRect lrc=pShape->CalculateBasicExtents();
            lrc.Offset(delta);
            dc.MoveTo(lrc.GetCorner(3));
            for(int i=0; i<4; i++) dc.LineTo(lrc.GetCorner(i));
         }
         else
         {
            CTrackContext* pTrackContext=pShape->TrackCreateContext(kMoveHandle, 0, &m_Mapper);
            ASSERT(pTrackContext);
            pTrackContext->pView=this;
            pShape->TrackFeedback(delta,pTrackContext);
            pShape->TrackWireFrame(&dc, pTrackContext);
            delete pTrackContext;
         }
      }
      if(bSingle) break;
   }
}

void CPCDraftView::OffsetSelection(LongPoint delta, bool bSingle)
{
   POSITION pos=m_Selection.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetPrev(pos);
      if(!pShape->IsLocked() && (!pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) || CanMoveDimension(pShape)))
      {
         InvalidateShape(pShape,true);
         pShape->DoShapeOffset(delta);
         InvalidateShape(pShape,true);
      }
      if(bSingle) break;
   }
   GetDocument()->SetModifiedFlag();
}

BOOL CPCDraftView::OnQueryNewPalette()
{
   CDC *dc=GetDC();
   CPalette *oldPalette=dc->SelectPalette(&GetDocument()->m_Palette,FALSE);
   int numMapped=dc->RealizePalette();
   dc->SelectPalette(oldPalette, FALSE);
   ReleaseDC(dc);
   if(numMapped)
      Invalidate(TRUE);


   return CView::OnQueryNewPalette();
}


FIBITMAP * freeimage_from_hbitmap(HBITMAP hbitmap)
{

   //if (pbitmap == NULL)
   //   return NULL;



   //HBITMAP hbitmap = pbitmap->GetHBITMAP();

   if (hbitmap == NULL)
      return NULL;

   // ...
   // the following code assumes that you have a valid HBITMAP loaded into the memory
   BITMAP bm;
   ::GetObject(hbitmap, sizeof(BITMAP), (char *)&bm);
   if (bm.bmWidth <= 0 || bm.bmHeight <= 0)
      return NULL;
   FIBITMAP * fi;
   //if(bm.bmBitsPixel == 32)
   {
      // fi = FreeImage_AllocateT(bm.bmWidth,bm.bmHeight,bm.bmBitsPixel);
   }
   //else
   {
      fi = FreeImage_Allocate(bm.bmWidth, bm.bmHeight, bm.bmBitsPixel);
   }
   // The GetDIBits function clears the biClrUsed and biClrImportant BITMAPINFO members (dont't know why)
   // So we save these infos below. This is needed for palettized images only.
   int32_t nColors = FreeImage_GetColorsUsed(fi);
   HDC hdc = ::CreateCompatibleDC(NULL);

   GetDIBits(hdc, (HBITMAP)hbitmap, 0, FreeImage_GetHeight(fi), FreeImage_GetBits(fi), FreeImage_GetInfo(fi), DIB_RGB_COLORS);

   ::DeleteDC(hdc);

   ///   pbitmap->ReleaseHBITMAP(hbitmap);

   // restore BITMAPINFO members
   FreeImage_GetInfoHeader(fi)->biClrUsed = nColors;
   FreeImage_GetInfoHeader(fi)->biClrImportant = nColors;
   return fi;


}



HBITMAP dib32_from_freeimage(FIBITMAP *pfibitmap)
{

   if(pfibitmap == NULL)
   {

      return NULL;

   }

   FIBITMAP * pimage32 = FreeImage_ConvertTo32Bits(pfibitmap);

   if(pimage32 == NULL)
   {

      return NULL;

   }

   BITMAPINFO * pbi = FreeImage_GetInfo(pimage32);

   void * pdata = FreeImage_GetBits(pimage32);

   COLORREF * pcolorref = NULL;

   BITMAPINFO info;

   memset(&info,0,sizeof(info));

   info.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
   info.bmiHeader.biWidth         = pbi->bmiHeader.biWidth;
   info.bmiHeader.biHeight        = pbi->bmiHeader.biHeight;
   info.bmiHeader.biPlanes        = 1;
   info.bmiHeader.biBitCount      = 32;
   info.bmiHeader.biCompression   = BI_RGB;
   info.bmiHeader.biSizeImage     = pbi->bmiHeader.biWidth * pbi->bmiHeader.biHeight * 4;


   HBITMAP hDib = ::CreateDIBSection(NULL,&info,DIB_RGB_COLORS,(void **)&pcolorref,NULL,0);

   if(hDib != NULL)
   {
      HDC hdc = ::GetDC(NULL);

      if(pbi->bmiHeader.biHeight != SetDIBits(
            hdc,
            hDib,
            0,
            pbi->bmiHeader.biHeight,
            pdata,
            pbi,
            DIB_RGB_COLORS))
      {

         ASSERT(FALSE);
      }

      ::ReleaseDC(NULL,hdc);

   }

   FreeImage_Unload(pimage32);

   FreeImage_Unload(pfibitmap);

   return hDib;

}

HBITMAP dib32_from_hbitmap(HBITMAP hbitmap)
{

   return dib32_from_freeimage(freeimage_from_hbitmap(hbitmap));

}


// Import Image - Infinisys Ltd
void CPCDraftView::OnFileImportImage()
{

   CString strFilter = "Image files (*.jpg;*.png;*.bmp;*.tiff;*.tif;*.tga;*.psd)|*.jpg; *.png; *.bmp; *.tiff; *.tif; *.tga; *.psd|All Files (*.*)|*.*||";

   CFileDialog filedlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter,GetParentFrame());
   TCHAR pathPicture[MAX_PATH];
   memset(pathPicture,0,sizeof(pathPicture));
   HRESULT hr = SHGetFolderPath(NULL,CSIDL_MYPICTURES,NULL,SHGFP_TYPE_CURRENT,pathPicture);

   CString sInitialDirectory = AfxGetApp()->GetProfileString("Settings", "ImageImportFolder", pathPicture);
   // Set sInitialDirectory to whatever you want


   filedlg.m_ofn.lpstrInitialDir = sInitialDirectory;

   if(filedlg.DoModal() == IDOK)
   {

      HBITMAP hDib = NULL;

      sInitialDirectory = get_folder_from_path(filedlg.GetPathName());

      AfxGetApp()->WriteProfileString("Settings","ImageImportFolder",sInitialDirectory);



#if 1

      FIBITMAP * pfi = NULL;

      FREE_IMAGE_FORMAT    format = FreeImage_GetFIFFromFilename(filedlg.GetPathName());

      if(format != FIF_UNKNOWN)
      {

         pfi = FreeImage_Load(format,filedlg.GetPathName());

      }

      //CDC hdc ;
      //hdc.CreateCompatibleDC(NULL);

      if(pfi != NULL)
      {

         hDib = dib32_from_freeimage(pfi);

      }

#else

      LPWSTR lpwstr = ConvertString(filedlg.GetPathName());

      Gdiplus::Bitmap bitmap(lpwstr);


      delete[] lpwstr;

      Gdiplus::Color clrTransparent(0,0,0,0);

      if(Gdiplus::Ok != bitmap.GetHBITMAP(clrTransparent,&hDib))
      {

         return;

      }

      if(hDib == NULL)
      {

         return;

      }
#endif


      LongPoint lptOffset=m_Mapper.DPtoLP(m_ptPaste);



      BITMAP bm;
      GetObject(hDib,sizeof(BITMAP),&bm);
      CRect rc(0,0,bm.bmWidth,bm.bmHeight);
      LongRect lrc(0,0,(int)bm.bmWidth,(int)bm.bmHeight); //m_Mapper.DPtoLP(rc.right),m_Mapper.DPtoLP(rc.bottom));
      lrc.Offset(lptOffset);
      CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
      pShape->CalculateExtents();
      pShape->SetShapeDatum(kDefaultDatum);
      GetDocument()->CreateShape(pShape);
      SelectShape(pShape,false);
      GetDocument()->CheckDocSize(true);


   }



}


// Import Image - Infinisys Ltd
void CPCDraftView::OnUpdateFileImportImage(CCmdUI * pCmdUI)
{

   pCmdUI->Enable();

}


void CPCDraftView::OnEditCopy()
{
   m_iEditPasteCounter = 0;
   if(m_bEditActive) m_InplaceEdit.DoCopy();
   else DoEditCopy();
}

void CPCDraftView::OnEditCut()
{
   if(m_bEditActive) m_InplaceEdit.DoCut();
   else
   {
      SaveUndo(IDS_CUT);
      if(DoEditCopy())
      {
         DoEditClear();
      }
   }
}

void CPCDraftView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
   if(m_bEditActive) pCmdUI->Enable(m_InplaceEdit.CanPaste());
   else pCmdUI->Enable(!m_nOneTimeTool && IsClipboardFormatAvailable(((CPCDraftApp*)AfxGetApp())->m_cfShapeList) || IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_TEXT) || IsClipboardFormatAvailable(CF_ENHMETAFILE));
}


void CPCDraftView::OnEditPaste()
{

   if (m_bEditActive)
   {

      m_InplaceEdit.DoPaste();

   }
   else
   {

      m_iEditPasteCounter++;

      CPoint point(m_iEditPasteCounter*16, m_iEditPasteCounter*16);
      
      DoEditPaste(false, &point, true);

   }

}


void CPCDraftView::OnUpdateEditPasteUnscaled(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!m_bEditActive && !m_nOneTimeTool && (IsClipboardFormatAvailable(((CPCDraftApp*)AfxGetApp())->m_cfShapeList) || IsClipboardFormatAvailable(CF_DIB)));
}

void CPCDraftView::OnEditPasteUnscaled()
{
   DoEditPaste(true);
}

BOOL CPCDraftView::DoEditCopy()
{
   if(OpenClipboard() && EmptyClipboard())
   {
      HANDLE hClip=SelectionToHandle();
      SetClipboardData(((CPCDraftApp*)AfxGetApp())->m_cfShapeList,hClip);
#ifndef _DEMO
      if(((CPCDraftApp*)AfxGetApp())->m_bDemoMode)
#endif
         SetClipboardData(CF_BITMAP,LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_DEMO_CLIPBOARD)));
#ifndef _DEMO
      else
      {
         //SetClipboardData(CF_BITMAP,SelectionToBitmap());
         SetClipboardData(CF_ENHMETAFILE,SelectionToMetaFile());

      }
#endif
      CloseClipboard();
      return TRUE;
   }
   return FALSE;
}

void CPCDraftView::DoEditPaste(bool bUnscaled, CPoint* ptAt, bool bOffset)
{
   if(OpenClipboard())
   {
      SaveUndo(IDS_PASTE);
      SelectNone();

      HANDLE hClip=::GetClipboardData(((CPCDraftApp*)AfxGetApp())->m_cfShapeList);
      if(hClip)
      {
         CreateShapesFromHandle(hClip,bUnscaled,ptAt?ptAt:&m_ptPaste, bOffset);
         GetDocument()->CheckDocSize(true);
      }
      else
      {
         LongPoint lptOffset=m_Mapper.DPtoLP(ptAt?*ptAt:m_ptPaste);
         SnapToGrid(lptOffset);


         hClip = GetClipboardData(CF_UNICODETEXT);
         CString strClipboardText;
         if (hClip)
         {
            LPCWSTR p = (LPCWSTR)GlobalLock(hClip);
            strClipboardText = get_cp(p, CP_ACP);
            GlobalUnlock(hClip);
         }

         if (strClipboardText.IsEmpty())
         {
            strClipboardText = (LPCTSTR)::GlobalLock(hClip);
            ::GlobalUnlock(hClip);
         }

         if(strClipboardText.GetLength() > 0)
         {
            CRect rc;
            CStyledText st;
            CStyledTextStyle style;
            style.m_TextColor=RGB(0,0,0);
            style.m_LogFont=GetDocument()->m_TextParams.m_lf;
            AdjustFontCharSet(style.m_LogFont);

            CString theText = strClipboardText;

            if(!theText.IsEmpty())
            {
               theText.Replace('\t',' ');
               theText.Replace("\r\n","\n");
               for(int i=0; i<theText.GetLength(); i++)
                  st.Add(theText.GetAt(i),style);
            }
            {
               CClientDC theDC(this);
               CDraftContext dc(&theDC,&m_Mapper,GetDocument());
               dc.CalcBoundRect(st,GetDocument()->m_TextParams,rc, true);
            }
            LongRect lrc=m_Mapper.DPtoLP(rc);
            lrc.Inset(-3,-3);
            lrc.Offset(lptOffset);
            CMDText* pShape=new CMDText(CMDText::kTextAutoSize);
            pShape->SetShapePoint(0,lrc.GetCorner(0));
            pShape->SetShapePoint(1,lrc.GetCorner(1));
            pShape->SetShapePoint(2,lrc.GetCorner(2));
            pShape->SetShapePoint(3,lrc.GetCorner(3));
            pShape->SetText(st);
            pShape->SetTextParams(GetDocument()->m_TextParams);
            pShape->CalculateExtents();
            pShape->SetShapeDatum(kDefaultDatum);
            GetDocument()->CreateShape(pShape);
            SelectShape(pShape,false);
            GetDocument()->CheckDocSize(true);
         }
         else
         {
            hClip=::GetClipboardData(CF_DIB);
            if(hClip)
            {
               CPackedDIB dib;
               dib.FromClipboard(hClip);
               HBITMAP hDib=dib.CreateDIBSection();
               BITMAP bm;
               GetObject(hDib,sizeof(BITMAP),&bm);
               CRect rc(0,0,bm.bmWidth,bm.bmHeight);
               LongRect lrc(0,0,(int)bm.bmWidth,(int)bm.bmHeight); //m_Mapper.DPtoLP(rc.right),m_Mapper.DPtoLP(rc.bottom));
               lrc.Offset(lptOffset);
               CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
               pShape->CalculateExtents();
               pShape->SetShapeDatum(kDefaultDatum);
               GetDocument()->CreateShape(pShape);
               SelectShape(pShape,false);
               GetDocument()->CheckDocSize(true);
            }
            else
            {
               hClip=::GetClipboardData(CF_ENHMETAFILE);
               if(hClip)
               {
                  ENHMETAHEADER hd;
                  GetEnhMetaFileHeader((HENHMETAFILE)hClip,sizeof(hd),&hd);
                  BITMAPINFOHEADER bmih;
                  bmih.biSize=sizeof(BITMAPINFOHEADER);

                  HDC hDC=::GetDC(NULL);
                  bmih.biWidth=MulDiv(hd.rclFrame.right,GetDeviceCaps(hDC,LOGPIXELSX),2540);
                  bmih.biHeight=MulDiv(hd.rclFrame.bottom,GetDeviceCaps(hDC,LOGPIXELSY),2540);
                  ::ReleaseDC(NULL,hDC);

                  bmih.biPlanes=1;
                  bmih.biBitCount=32;
                  bmih.biCompression=BI_RGB;
                  bmih.biSizeImage=0;
                  bmih.biXPelsPerMeter=0;
                  bmih.biYPelsPerMeter=0;
                  bmih.biClrUsed=0;
                  bmih.biClrImportant=0;
                  LPVOID lpBits;
                  HBITMAP hDib=CreateDIBSection(NULL,(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0);
                  CRect rc(0,0,bmih.biWidth,bmih.biHeight);
                  {
                     CWindowDC theWindowDC(AfxGetMainWnd());
                     CGDIBitmapDC theDC(&theWindowDC,CBitmap::FromHandle(hDib));
                     theDC.FillRect(&rc,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
                     rc.InflateRect(0,0,-1,-1);
                     PlayEnhMetaFile(theDC.GetSafeHdc(),(HENHMETAFILE)hClip,&rc);
                  }
                  rc.InflateRect(0,0,1,1);
                  LongRect lrc(0,0,(int)rc.right,(int)rc.bottom); // m_Mapper.DPtoLP(rc.right),m_Mapper.DPtoLP(rc.bottom));
                  lrc.Offset(lptOffset);
                  CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
                  pShape->CalculateExtents();
                  pShape->SetShapeDatum(kDefaultDatum);
                  GetDocument()->CreateShape(pShape);
                  SelectShape(pShape,false);
                  GetDocument()->CheckDocSize(true);
               }
            }
         }
      }
      CloseClipboard();
   }
}

/*void CPCDraftView::ProcessShapesForCopy(CPCDraftDoc* pDoc, CMDShape* pShape)
{
	pShape->ProcessShapesForCopy(pDoc, &used[0]);
}

void CPCDraftView::ProcessShapesAfterCopy(CPCDraftDoc* pDoc, CMDShape* pShape)
{
	pShape->ProcessShapesAfterCopy(pDoc, &used[0]);
}*/

HANDLE CPCDraftView::SelectionToHandle(bool bByExtent)
{
   CMemFile mf(0x40000);
   CArchive ar(&mf,CArchive::store);

   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pLayer=pDoc->GetActiveLayer();
   ar << pDoc->m_nDisplayRes << pLayer->m_nScale;

   LongPoint lptOrigin;
   if(bByExtent || m_Selection.GetCount()>1)
   {
      LongBoundRect lrcExtent;
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos) lrcExtent|=(m_Selection.GetNext(pos))->GetExtents();
      lptOrigin=lrcExtent.GetCorner(0);
   }
   else lptOrigin=m_Selection.GetHead()->GetShapeDatum();
   ar << lptOrigin;

   CLinkVertexList theLinkVertexList;
   POSITION pos=pLayer->m_LinkVertexList.GetHeadPosition();
   while(pos)
   {
      CLinkVertex* pVertex=pLayer->m_LinkVertexList.GetNext(pos);
      if(IsSelectedShape(pVertex->m_pShape,true))
         theLinkVertexList.AddTail(new CLinkVertex(*pVertex));
   }
   theLinkVertexList.RemoveSinglePoints();

   CMDShapeList workList,copyList;
   workList.AddTail(&m_Selection);
   pLayer->SortShapes(workList,false);

   // by TSM
   // Preparing palette entities for copiing using clipboard
   int used[2048];
   int i = 0;
   for(; i<2048; i++) used[i] = 0;

   while(workList.GetCount())
   {
      CMDShape* pShape=workList.RemoveHead();
      CMDShape* pNewShape=pShape->CreateCopy();
      pNewShape->AdjustLinks(&theLinkVertexList,pShape);
      if(pNewShape->IsKindOf(RUNTIME_CLASS(CMDDimStr))) delete pNewShape;
      else
      {
         pNewShape->MakeSelfContained(pDoc);
         pNewShape->ProcessShapesForCopy(pDoc, &used[0]);
         copyList.AddHead(pNewShape);
      }
   }
   
   // by TSM
   
   int usedCount=0;

   for (i = 0; i < 2048; i++)
   {

      if (used[i])
      {

         usedCount++;

      }

   }
   ar << usedCount;
   for(i=0; i<2048; i++)
      if(used[i])
      {
         ar<<i<<pDoc->m_packeditema[i];
      }
   int iCount = copyList.GetCount();
   ar << iCount;
   while(copyList.GetCount())
   {
      CMDShape* pShape=copyList.RemoveHead();
      ar << pShape;
      delete pShape;
   }

   theLinkVertexList.Serialize(ar);

   ar.Close();
   DWORD len=mf.GetLength();
   BYTE* data=mf.Detach();

   HANDLE hClip=GlobalAlloc(GMEM_DDESHARE,len);
   CopyMemory(GlobalLock(hClip),data,len);
   GlobalUnlock(hClip);
   free(data);
   return hClip;
}

void CPCDraftView::CreateShapesFromHandle(HANDLE hClip,bool bUnscaled, CPoint *ptAt,bool bOffset)
{
   CMemFile mf((BYTE*)GlobalLock(hClip),GlobalSize(hClip));
   CArchive ar(&mf,CArchive::load);
   ar.m_pDocument = GetDocument();
   GetDocument()->m_iArchiveVersion = 2;
   int nDisplayRes,nScale;
   ar >> nDisplayRes >> nScale;

   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pLayer=pDoc->GetActiveLayer();
   LongRatio srcRatio,changeRatio;
   pDoc->GetScaleRatio(nDisplayRes!=kMetRes,nScale,srcRatio);
   if(bUnscaled)
   {
      changeRatio.fN=1;
      changeRatio.fD=1;
   }
   else
   {
      LongRatio dstRatio;
      pDoc->GetActiveScaleRatio(dstRatio);

      changeRatio.fN=dstRatio.fN*srcRatio.fD;
      changeRatio.fD=dstRatio.fD*srcRatio.fN;
      long theGCD=gcd(changeRatio.fN,changeRatio.fD);
      changeRatio.fN/=theGCD;
      changeRatio.fD/=theGCD;
   }
   double resChange=pDoc->CalcResChangeFactor(nDisplayRes);
   if(resChange!=0) resChange=1/resChange;

   LongPoint origin;
   ar >> origin;
   origin*=changeRatio;

   LongPoint offset;
   offset.Clear();
   if(ptAt)
   {
      if(bOffset)
      {
         offset.x=m_Mapper.DPtoLP(ptAt->x);
         offset.y=m_Mapper.DPtoLP(ptAt->y);
      }
      else offset=m_Mapper.DPtoLP(*ptAt);
      SnapToGrid(offset);
      if(!bOffset) offset-=origin;
   }

   // by TSM - merging patern palettes
   int usedCount;
   int used[2048];
   int i = 0;
   for(; i<2048; i++) used[i] = 0;

   ar >> usedCount;

   CPackedItem item;
   int j;
   for(i=0; i<usedCount; i++)
   {
      ar >> j;
      ar >> item;

      if(item.m_etype == packed_type_dib)
      {
         ASSERT(0 <= j && j < 2048);

         HBITMAP hDib = item.m_pdib->CreateDIBSection();
         int hash = CalculateDIBHash(hDib);
         int found = -1;
         for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
         {
            if(pDoc->m_packeditema[k].m_etype == packed_type_dib)
            {
               if(hash == pDoc->m_PatternsHash[pDoc->m_packeditema[k].get_index(pDoc)])
               {
                  HBITMAP hDib2 = pDoc->m_Patterns[pDoc->m_packeditema[k].get_index(pDoc)].CreateDIBSection();
                  if(!CompareDIBs(hDib,hDib2))
                  {
                     ::DeleteObject(hDib2);
                     found = k; break;
                  }
                  ::DeleteObject(hDib2);
               }
            }
         }
         if(found>= 0)
         {
            used[j] = found;
            ::DeleteObject(hDib);
         }
         else
         {
            used[j] = pDoc->m_packeditema.GetCount();
            pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
            pDoc->m_Patterns[pDoc->m_numDIBPatterns].FromDIBSection(hDib);
            CPackedItem item;
            item.m_etype = packed_type_dib;
            item.m_pdib = &pDoc->m_Patterns[pDoc->m_numDIBPatterns];
            pDoc->m_numDIBPatterns++;
            pDoc->m_packeditema.Add(item);
            ::DeleteObject(hDib);
         }
      }
      else
      {
         ASSERT(0 <= j && j < 2048);

         int found = -1;
         for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
         {
            if(pDoc->m_packeditema[k].m_etype == packed_type_gradient)
            {
               if(*item.m_pgradient == pDoc->m_Gradients[pDoc->m_packeditema[k].get_index(pDoc)])
               {
                  found = k;
                  break;
               }
            }
         }
         if(found >= 0)
         {
            used[j] = found;
         }
         else
         {
            used[j] = pDoc->m_packeditema.GetCount();
            pDoc->m_Gradients[pDoc->m_numGradients] = *item.m_pgradient;
            CPackedItem item;
            item.m_etype = packed_type_gradient;
            item.m_pgradient = &pDoc->m_Gradients[pDoc->m_numGradients];
            pDoc->m_packeditema.Add(item);
            pDoc->m_numGradients++;
         }
      }
   }
   // end by TSM
   int nCount;
   ar >> nCount;

   bool shownTextRescaleMessage = false;
   bool rescaleTextShapes = false;

   while(nCount--)
   {
      CObject* pObject;
      ar >> pObject;

      CMDShape* pShape=(CMDShape*)pObject;
      // by TSM - applying new patterns to shapes
      pShape->ProcessShapesAfterCopy(pDoc, &used[0]);

      if(!pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
      {
         pShape->DoShapeScale(changeRatio,resChange);
      }
      else
      {
         if(changeRatio.fD!=1.0 ||
               changeRatio.fN!=1.0)
         {
            if(!shownTextRescaleMessage)
            {
               rescaleTextShapes= (IDYES==AfxMessageBox(IDS_INSERT_TEXT_WITH_DIFFERENT_SCALE, MB_YESNO|MB_DEFBUTTON2));
               shownTextRescaleMessage=true;
            }
            if(rescaleTextShapes)
            {
               pShape->DoShapeScale(changeRatio,resChange);
            }
         }
         else
         {
            pShape->DoShapeScale(changeRatio,resChange);
         }
      }

      pDoc->CreateShape(pShape);
      CMDDimStr* pAttached=pShape->Detach();
      if(pAttached)
      {
         pAttached->DoShapeScale(changeRatio,resChange);
         pAttached->DoShapeOffset(offset);
         pAttached->SetLocked(false);
         pDoc->CreateShape(pAttached);
         pShape->Attach(pAttached);
      }

      pShape->SetLocked(false);
      pShape->UpdateDependentShapes(this);
      pShape->CalculateExtents();
      pShape->DoShapeOffset(offset);
      SelectShape(pShape,true);
   }

   TRY
   {
      CLinkVertexList theLinkVertexList;
      theLinkVertexList.Serialize(ar);
      POSITION pos=theLinkVertexList.GetHeadPosition();
      long nNewKey,nOldKey=0;
      while(pos)
      {
         CLinkVertex* pVertex=theLinkVertexList.GetNext(pos);
         if(pVertex->m_nKey!=nOldKey)
         {
            nOldKey=pVertex->m_nKey;
            nNewKey=pLayer->GetVertexKey();
            if(nOldKey<0) nNewKey=-nNewKey;
         }
         pVertex->m_nKey=nNewKey;
         pLayer->m_LinkVertexList.AddTail(pVertex);
      }
   }
   CATCH(CArchiveException,e)
   {
   }
   END_CATCH

   GlobalUnlock(hClip);
}


// Infinisys Ltd. 2021 - Contextual Menus // From OnEditDuplucate
void CPCDraftView::DuplicateSelection(bool bWithOffset)
{

      SaveUndo(IDS_DUPLICATE);
   if (GetAsyncKeyState(VK_OPTION) < 0)
   {
      UnselectShapes(usLocked); // duplicate unlocked
      if (m_Selection.GetCount() == 0)
      {
         MessageBeep(MB_ICONHAND);
         return;
      }
   }
   HANDLE hDup = SelectionToHandle();
   LongPoint lptDupDelta = m_lptDupDelta;	// save delta
   SelectNone();
   CPoint ptDupDelta;
   if (GetAsyncKeyState(VK_SHIFT) < 0 || !bWithOffset) ptDupDelta = CPoint(0,0); // duplicate on top
   else ptDupDelta = CPoint(m_Mapper.LPtoDP(m_lptDupDelta.x),m_Mapper.LPtoDP(m_lptDupDelta.y));
   CreateShapesFromHandle(hDup,true,&ptDupDelta,true);
   UnlockSelection();
   GetDocument()->CheckDocSize(true);
   m_lptDupDelta = lptDupDelta;		// restore delta
   m_bAdjustDupDelta = true;

}


void CPCDraftView::OnEditDuplucate()
{
   
   // Infinisys Ltd. 2021 - Contextual Menus
   DuplicateSelection(true);

}


// Infinisys Ltd. 2021 - Contextual Menus
void CPCDraftView::OnEditDuplicateWithoutOffset()
{

   DuplicateSelection(false);

}


void CPCDraftView::OnUpdateArrangeLock(CCmdUI* pCmdUI)
{
   if(m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool)
   {
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(!pShape->IsLocked())
         {
            pCmdUI->Enable();
            return;
         }
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnArrangeLock()
{
   SaveUndo(IDS_LOCK);
   DeactivateTextShape();
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked())
      {
         pShape->SetLocked(true);
         InvalidateShape(pShape,true);
      }
   }
}

void CPCDraftView::OnUpdateArrangeUnlock(CCmdUI* pCmdUI)
{
   if(m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool)
   {
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(pShape->IsLocked())
         {
            pCmdUI->Enable();
            return;
         }
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnArrangeUnlock()
{
   SaveUndo(IDS_UNLOCK);
   UnlockSelection();
}

void CPCDraftView::ApplyFill(const FillModel &fm, bool bParallel)
{
   CPCDraftDoc* pDoc=GetDocument();
   bool bFound=false;
   if(bParallel)
   {
      SaveUndo(IDS_PARAFILLCHANGE);
      if(m_Selection.GetCount())
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(pShape->IsParallel() && !pShape->IsLocked())
            {
               ParallelAttrs attrs=pShape->GetParallelAttrs();
               attrs.m_Fill.ModifyFillModel(fm, fmAllButOpacity);
               pShape->SetParallelAttrs(attrs);
               InvalidateShape(pShape,true);
               bFound=true;
            }
         }
      }
      if(!bFound)
      {
         pDoc->m_ParallelAttrs.m_Fill=fm;
      }
   }
   else
   {
      SaveUndo(IDS_FILLCHANGE);
      if(m_Selection.GetCount())
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(!pShape->IsLocked())
            {
               pShape->ModifyFillModel(fm, fmAllButOpacity);
               InvalidateShape(pShape,true);
               bFound=true;
            }
         }
      }
      if(!bFound)
      {
         pDoc->m_Fill.ModifyFillModel(fm, fmAllButOpacity);
         m_wndFillInfo.SetFill(pDoc->m_Fill);
      }
   }
   pDoc->SetModifiedFlag();

   //if (m_pbitmapDrawBackground)
   //{
   //   m_pbitmapDrawBackground.release();
   //   RedrawWindow();
   //   m_pbitmapDrawBackground.reset(BitmapFromView());
   //}
   //else
   //{
   //   UpdateWindow();
   //}

}


// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::ApplyOpacity(int iOpacity,bool bParallel, bool bDoSaveUndo)
{
   CPCDraftDoc* pDoc=GetDocument();
   bool bFound=false;
   if(bParallel)
   {
      if(bDoSaveUndo)
      {
         SaveUndo(IDS_PARAFILLCHANGE);
      }
      if(m_Selection.GetCount())
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(pShape->IsParallel() && !pShape->IsLocked())
            {
               ParallelAttrs attrs=pShape->GetParallelAttrs();
               attrs.m_Fill.m_iFillOpacity = iOpacity;
               pShape->SetParallelAttrs(attrs);
               InvalidateShape(pShape,true);
               bFound=true;
            }
         }
      }
      if(!bFound)
      {
         pDoc->m_ParallelAttrs.m_Fill.m_iFillOpacity = iOpacity;
      }
   }
   else
   {
      if(bDoSaveUndo)
      {
         SaveUndo(IDS_FILLCHANGE);
      }
      if(m_Selection.GetCount())
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(!pShape->IsLocked())
            {
               pShape->SetFillOpacity(iOpacity);
               InvalidateShape(pShape,true);
               bFound=true;
            }
         }
      }
      if(!bFound)
      {
         pDoc->m_Fill.m_iFillOpacity = iOpacity;
         m_wndFillInfo.SetFill(pDoc->m_Fill);
      }
   }
   pDoc->SetModifiedFlag();
}

void CPCDraftView::CreateFillModelBitmap(const FillModel& fm,CBitmap& bmp, CSize size)
{
   CClientDC cdc(NULL);
   ASSERT(bmp.GetSafeHandle()==NULL);
   bmp.CreateCompatibleBitmap(&cdc,size.cx,size.cy);

   CGDIBitmapDC dc(&cdc,&bmp);
   CPCDraftDoc* pDoc=GetDocument();
   CPalette* pPalette=dc.SelectPalette(&pDoc->m_Palette,FALSE);
   dc.RealizePalette();
   CBrush brush;
   CBitmap pat;
   CRect rc(0,0,size.cx,size.cy);
   dc.FillSolidRect(&rc,RGB(255,255,255));
   switch(fm.m_nFillType)
   {
   case kFillColor:
      brush.CreateSolidBrush(fm.m_crColor);
      break;
   case kFillPat:
      pDoc->FillModelBitmap(fm.m_nIndex,dc, rc, fm.m_iFillOpacity);
      break;
   case kPenPatEmbed:
   {
      DIBSECTION ds;
      GetObject(fm.m_hDib,sizeof(DIBSECTION),&ds);
      if(ds.dsBmih.biBitCount==1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
      HBITMAP hBmp=CreateDIBitmap(dc.GetSafeHdc(),&ds.dsBmih,CBM_INIT,ds.dsBm.bmBits,(BITMAPINFO*)&ds.dsBmih,DIB_RGB_COLORS);
      pat.Attach(hBmp);
   }
   brush.CreatePatternBrush(&pat);
   break;
   default:
      ASSERT(FALSE);
      break;
   }
   if(fm.m_nFillType != kFillPat)
   {
      dc.FillRect(&rc,&brush);
   }
   dc.SelectPalette(pPalette,FALSE);
}

void CPCDraftView::OnUpdateToolAttribFill(CCmdUI* pCmdUI)
{

   BOOL bFromSelection;

   if (m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const FillModel& fmTmp = pShape->GetFillModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const FillModel& fmTmpNext = pShape->GetFillModel();
         if (fmTmpNext.m_nFillType != fmTmp.m_nFillType || fmTmpNext.m_nIndex != fmTmp.m_nIndex)
         {
            bFromSelection = FALSE;
            break;
         }
      }
   }

   const FillModel& fm = bFromSelection ? m_Selection.GetTail()->GetFillModel():GetDocument()->m_Fill;
   if(fm.m_nFillType == kNoFill)
   {
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,0,NULL);
   }
   else
   {
      CBitmap bmp;
      CRect r(0, 0, 16, 16);
      if(!pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_INFO,0,(LPARAM)&r))
      {
         pCmdUI->m_pOther->GetClientRect(r);
      }
      CreateFillModelBitmap(fm,bmp, r.Size());
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,1,(LPARAM)bmp.Detach());
   }
}

// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnUpdateToolAttribFillTransparency(CCmdUI* pCmdUI)
{

   BOOL bFromSelection;

   if(m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const FillModel& fmTmp = pShape->GetFillModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while(pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const FillModel& fmTmpNext = pShape->GetFillModel();
         if(fmTmpNext.m_iFillOpacity != fmTmp.m_iFillOpacity)
         {
            pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,-1);
            return;

         }
      }
   }

   const FillModel& fm = bFromSelection ? m_Selection.GetTail()->GetFillModel():GetDocument()->m_Fill;

   if(fm.m_nFillType == kNoFill)
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,-1,NULL);
   else
   {
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,fm.m_iFillOpacity);
   }
}
void CPCDraftView::OnUpdateToolAttribFillAlpha(CCmdUI* pCmdUI)
{
   //OnUpdateToolAttribFillTransparency(pCmdUI);
}

void CPCDraftView::OnUpdateFillColorsRange(CCmdUI* pCmdUI)
{
   FillModel fm;
   switch(m_Selection.GetCount())
   {
   case 0:
      fm=GetDocument()->m_Fill;
      break;
   case 1:
      fm=(m_Selection.GetHead())->GetFillModel();
      break;
   default:
      pCmdUI->SetCheck(0);
      return;
   }
   int nColor=pCmdUI->m_nID-ID_FILL_COLORS_BASE-1;
   pCmdUI->SetCheck(fm.m_nFillType==kFillColor && fm.m_crColor==PALETTEINDEX(nColor));
   pCmdUI->Enable(nColor < GetDocument()->m_Palette.GetEntryCount()); // Infinisys Ltd.
}

void CPCDraftView::OnFillColorsRange(UINT nID)
{
   FillModel fm;
   if(nID==ID_FILL_COLORS_BASE) fm.m_nFillType=kNoFill;
   else
   {
      fm.m_nFillType=kFillColor;
      fm.m_crColor=PALETTEINDEX(nID-ID_FILL_COLORS_BASE-1);
   }
   //if (m_bEditActive)
   //{
   //   m_pActiveTextShape->SetFillModel(fm);
   //   InvalidateRect(nullptr, false);
   //}
   //else
   //{
      ApplyFill(fm, false);

//   }
}

void CPCDraftView::OnUpdateFillPatternsRange(CCmdUI* pCmdUI)
{
   FillModel fm;
   switch(m_Selection.GetCount())
   {
   case 0:
      fm=GetDocument()->m_Fill;
      break;
   case 1:
      fm=(m_Selection.GetHead())->GetFillModel();
      break;
   default:
      pCmdUI->SetCheck(0);
      return;
   }
   int nPattern=pCmdUI->m_nID-ID_FILL_PATTERNS_BASE-1;
   pCmdUI->SetCheck(fm.m_nFillType==kFillPat && fm.m_nIndex==nPattern);
   
}

void CPCDraftView::OnFillPatternsRange(UINT nID)
{
   FillModel fm;
   if(nID==ID_FILL_PATTERNS_BASE) fm.m_nFillType=kNoFill;
   else
   {
      fm.m_nFillType=kFillPat;
      fm.m_nIndex=nID-ID_FILL_PATTERNS_BASE-1;
   }
   ApplyFill(fm,false);
}


void CPCDraftView::OnUpdateToolAttribPFill(CCmdUI* pCmdUI)
{
   BOOL bFromSelection;

   if (m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const FillModel& fmTmp = pShape->GetParallelAttrs().m_Fill;

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const FillModel& fmTmpNext = pShape->GetParallelAttrs().m_Fill;
         if (fmTmpNext.m_nFillType != fmTmp.m_nFillType || fmTmpNext.m_nIndex != fmTmp.m_nIndex)
         {
            bFromSelection = FALSE;
            break;
         }
      }
   }
   const FillModel& fm= bFromSelection ? m_Selection.GetTail()->GetParallelAttrs().m_Fill:GetDocument()->m_ParallelAttrs.m_Fill;

   if(fm.m_nFillType==kNoFill)
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,0,NULL);
   else
   {
      CBitmap bmp;
      CRect r;
      pCmdUI->m_pOther->GetClientRect(r);
      CreateFillModelBitmap(fm,bmp,r.Size());
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,1,(LPARAM)bmp.Detach());
   }
}


// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnUpdateToolAttribPFillTransparency(CCmdUI* pCmdUI)
{
   BOOL bFromSelection;

   if(m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const FillModel& fmTmp = pShape->GetParallelAttrs().m_Fill;

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while(pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const FillModel& fmTmpNext = pShape->GetParallelAttrs().m_Fill;
         if(fmTmpNext.m_iFillOpacity != fmTmp.m_iFillOpacity)
         {
            pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,-1);
            return;

         }
      }
   }
   const FillModel& fm= bFromSelection ? m_Selection.GetTail()->GetParallelAttrs().m_Fill:GetDocument()->m_ParallelAttrs.m_Fill;

   if(fm.m_nFillType == kNoFill)
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,-1,NULL);
   else
   {
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,fm.m_iFillOpacity);
   }
}
void CPCDraftView::OnUpdateToolAttribPFillAlpha(CCmdUI* pCmdUI)
{
   //OnUpdateToolAttribPFillTransparency(pCmdUI);
}


void CPCDraftView::OnUpdateParaFillColorsRange(CCmdUI* pCmdUI)
{
   FillModel fm;
   switch(m_Selection.GetCount())
   {
   case 0:
      fm=GetDocument()->m_ParallelAttrs.m_Fill;
      break;
   case 1:
   {
      CMDShape* pShape=m_Selection.GetHead();
      if(pShape->IsParallel())
      {
         fm=pShape->GetParallelAttrs().m_Fill;
         break;
      }
   }
   // fall through
   default:
      pCmdUI->SetCheck(0);
      return;
   }
   int nColor=pCmdUI->m_nID-ID_PARA_COLORS_BASE-1;
   pCmdUI->SetCheck(fm.m_nFillType==kFillColor && fm.m_crColor==PALETTEINDEX(nColor));
   pCmdUI->Enable(nColor < GetDocument()->m_Palette.GetEntryCount()); // Infinisys Ltd.
}

void CPCDraftView::OnParaFillColorsRange(UINT nID)
{
   FillModel fm;
   if(nID==ID_PARA_COLORS_BASE) fm.m_nFillType=kNoFill;
   else
   {
      fm.m_nFillType=kFillColor;
      fm.m_crColor=PALETTEINDEX(nID-ID_PARA_COLORS_BASE-1);
   }
   ApplyFill(fm,true);
}

void CPCDraftView::DoAttribFillColor(bool bParallel)
{
   CColorMenu theMenu;
   theMenu.Create(&GetDocument()->m_Palette,bParallel?IDS_PARALLELCOLOR:IDS_FILLCOLOR,bParallel?ID_PARA_COLORS_BASE:ID_FILL_COLORS_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

void CPCDraftView::DoAttribFillColorPatternGradient(bool bParallel)
{
   CColorPatternGradientMenu theMenu;
   theMenu.Create(&GetDocument()->m_Palette,
      bParallel?IDS_PARALLELCOLOR:IDS_FILLCOLOR,
      bParallel?ID_PARA_COLORS_BASE:ID_FILL_COLORS_BASE,
      GetDocument()->m_packeditema.GetData(),
      GetDocument()->m_packeditema.GetCount(),
      bParallel?IDS_PARALLELPATTERN:IDS_FILLPATTERN,
      bParallel?ID_PARA_PATTERNS_BASE:ID_FILL_PATTERNS_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

void CPCDraftView::OnToolAttribFillMenu()
{
   DoAttribFillColorPatternGradient(GetAsyncKeyState(VK_OPTION)<0);
}


void CPCDraftView::OnToolAttribFillColor()
{
   DoAttribFillColor(GetAsyncKeyState(VK_OPTION)<0);
}


void CPCDraftView::OnToolAttribPFillMenu()
{
   DoAttribFillColorPatternGradient(true);
}

void CPCDraftView::OnToolAttribPFillColor()
{
   DoAttribFillColor(true);
}

void CPCDraftView::OnUpdateParaFillPatternsRange(CCmdUI* pCmdUI)
{
   FillModel fm;
   switch(m_Selection.GetCount())
   {
   case 0:
      fm=GetDocument()->m_ParallelAttrs.m_Fill;
      break;
   case 1:
   {
      CMDShape* pShape=m_Selection.GetHead();
      if(pShape->IsParallel())
      {
         fm=pShape->GetParallelAttrs().m_Fill;
         break;
      }
   }
   // fall through
   default:
      pCmdUI->SetCheck(0);
      return;
   }
   int nPattern=pCmdUI->m_nID-ID_PARA_PATTERNS_BASE-1;
   pCmdUI->SetCheck(fm.m_nFillType==kFillPat && fm.m_nIndex==nPattern);
}

void CPCDraftView::OnParaFillPatternsRange(UINT nID)
{
   FillModel fm;
   if(nID==ID_PARA_PATTERNS_BASE) fm.m_nFillType=kNoFill;
   else
   {
      fm.m_nFillType=kFillPat;
      fm.m_nIndex=nID-ID_PARA_PATTERNS_BASE-1;
   }
   ApplyFill(fm,true);
}

void CPCDraftView::DoAttribFillPattern(bool bParallel)
{
   CPatternMenu theMenu;
   theMenu.Create(GetDocument()->m_packeditema.GetData(),GetDocument()->m_packeditema.GetCount(), bParallel?IDS_PARALLELPATTERN:IDS_FILLPATTERN,bParallel?ID_PARA_PATTERNS_BASE:ID_FILL_PATTERNS_BASE); // Gradient - Infinisys Ltd
   theMenu.CmdTrack(AfxGetMainWnd());
}

void CPCDraftView::OnToolAttribFillPattern()
{
   DoAttribFillPattern(GetAsyncKeyState(VK_OPTION)<0);
}

// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::ShowTransparencyBar(CEdit * pedit, CAttribSettingsWnd * pwnd,e_attrib eattrib)
{

   if(!IsWindow(m_wndTransparency))
   {

      m_wndTransparency.Create(IDD_TRANSPARENCY_BAR);

   }

   CRect rc;

   if(pedit != NULL)
   {

      pedit->GetWindowRect(rc);

   }
   else
   {

      pwnd->GetWindowRect(rc);

   }

   m_wndTransparency.m_pedit = pedit;

   m_wndTransparency.m_eattrib = eattrib;

   CString str;

   if(pedit != NULL)
   {

      m_wndTransparency.m_pedit->GetWindowText(str);

   }
   else
   {
      str = pwnd->m_str;

   }

   m_wndTransparency.m_slider.SetPos(atoi(str));

   m_wndTransparency.m_pview  = this;

   m_wndTransparency.SetWindowPos(&CWnd::wndTopMost,rc.left + 18,rc.top + 18,0,0,SWP_NOSIZE);

   m_wndTransparency.ShowWindow(SW_SHOW);

}


// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnToolAttribFillTransparency()
{

   CMainFrame * pframe = (CMainFrame *)GetTopLevelFrame();

   ShowTransparencyBar(NULL,&pframe->m_RibbonBar.m_properties.m_wndFillTransparency,attrib_fill);

}

void CPCDraftView::OnToolAttribFillAlpha()
{

   CMainFrame * pframe = (CMainFrame *)GetTopLevelFrame();

   ShowTransparencyBar(NULL,&pframe->m_RibbonBar.m_properties.m_wndFillTransparency,attrib_fill);

}

void CPCDraftView::OnToolAttribPFillPattern()
{
   DoAttribFillPattern(true);
}
// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnToolAttribPFillTransparency()
{

   CMainFrame * pframe = (CMainFrame *)GetTopLevelFrame();

   ShowTransparencyBar(NULL,&pframe->m_RibbonBar.m_properties.m_wndPFillTransparency,attrib_pfill);

}

void CPCDraftView::OnToolAttribPFillAlpha()
{

   CMainFrame * pframe = (CMainFrame *)GetTopLevelFrame();

   ShowTransparencyBar(NULL,&pframe->m_RibbonBar.m_properties.m_wndPFillTransparency,attrib_pfill);

}


bool CPCDraftView::ApplyPen(const PenModel &pm, int nWhat)
{
   CPCDraftDoc* pDoc=GetDocument();
   bool bFound=false;
   bool bResult=true;

   if(m_Selection.GetCount())
   {
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(!pShape->IsLocked())
         {
            InvalidateShape(pShape,true);
            bResult&=pShape->ModifyPenModel(pm,nWhat);
            InvalidateShape(pShape,true);
            bFound=true;
         }
      }
   }
   if(!bFound)
   {
      pDoc->m_Pen.Modify(pm,nWhat);
   }
   pDoc->SetModifiedFlag();
   return bResult;
}

void CPCDraftView::OnUpdateToolAttribPen(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc=GetDocument();

   BOOL bFromSelection;

   if (m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const PenModel& fmTmp = pShape->GetPenModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const PenModel& fmTmpNext = pShape->GetPenModel();
         if (fmTmpNext.m_nPenType != fmTmp.m_nPenType || fmTmpNext.m_nIndex != fmTmp.m_nIndex)
         {
            bFromSelection = FALSE;
            break;
         }
      }
   }

   const PenModel& pm = bFromSelection ? m_Selection.GetTail()->GetPenModel():pDoc->m_Pen;

   if(pm.m_nPenType==kPenInvis)
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,0,NULL);
   else
   {
      CRect rc(0,0,32,32);
      pCmdUI->m_pOther->GetClientRect(&rc);
      CBitmap bmp;
      CClientDC cdc(NULL);
      bmp.CreateCompatibleBitmap(&cdc,rc.Width(),rc.Height());
      {
         CGDIBitmapDC dc(&cdc,&bmp);
         CPalette* pPalette=dc.SelectPalette(&pDoc->m_Palette,FALSE);
         dc.RealizePalette();
         dc.FillSolidRect(rc,RGB(255,255,255));
         CBrush brush;
         CBitmap pat;

         switch(pm.m_nPenType)
         {
         case kPenColor:
            brush.CreateSolidBrush(pm.m_crColor);
            break;
         case kPenPat:
            pDoc->FillModelBitmap(pm.m_nIndex,dc,&rc, pm.m_iPenOpacity);
            //pDoc->GetPatternBitmap(pm.m_nIndex,&pat);
            //brush.CreatePatternBrush(&pat);
            break;
         case kPenPatEmbed:
         {
            DIBSECTION ds;
            GetObject(pm.m_hDib,sizeof(DIBSECTION),&ds);
            if(ds.dsBmih.biBitCount==1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
            HBITMAP hBmp=CreateDIBitmap(dc.GetSafeHdc(),&ds.dsBmih,CBM_INIT,ds.dsBm.bmBits,(BITMAPINFO*)&ds.dsBmih,DIB_RGB_COLORS);
            pat.Attach(hBmp);
         }
         brush.CreatePatternBrush(&pat);
         break;
         default:
            ASSERT(FALSE);
            break;
         }
         if(pm.m_nPenType != kPenPat)
         {
            dc.FillRect(&rc,&brush);
         }
         dc.SelectPalette(pPalette,FALSE);
      }
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,(pm.m_nPenType>kPenColor)?2:1,(LPARAM)bmp.Detach());
   }
}

// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnUpdateToolAttribPenTransparency(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc=GetDocument();

   BOOL bFromSelection;

   if(m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const PenModel& fmTmp = pShape->GetPenModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while(pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const PenModel& fmTmpNext = pShape->GetPenModel();
         if(fmTmpNext.m_iPenOpacity != fmTmp.m_iPenOpacity)
         {
            pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,-1);
            return;
         }
      }
   }

   const PenModel& pm = bFromSelection ? m_Selection.GetTail()->GetPenModel():pDoc->m_Pen;

   if(pm.m_nPenType == kPenInvis)
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,-1);
   else
   {
      pCmdUI->m_pOther->SendMessage(UM_ATTRIBSETTINGS_UPDATE,pm.m_iPenOpacity);
   }
}

void CPCDraftView::OnUpdateToolAttribPenAlpha(CCmdUI* pCmdUI)
{
   //OnUpdateToolAttribPenTransparency(pCmdUI);
}


void CPCDraftView::OnUpdatePenColorsRange(CCmdUI* pCmdUI)
{
   PenModel pm;
   switch(m_Selection.GetCount())
   {
   case 0:
      pm=GetDocument()->m_Pen;
      break;
   case 1:
      pm=(m_Selection.GetHead())->GetPenModel();
      break;
   default:
      pCmdUI->SetCheck(0);
      return;
   }
   int nColor=pCmdUI->m_nID-ID_PEN_COLORS_BASE-1;
   pCmdUI->SetCheck(pm.m_nPenType==kPenColor && pm.m_crColor==PALETTEINDEX(nColor));
   pCmdUI->Enable(nColor < GetDocument()->m_Palette.GetEntryCount()); // Infinisys Ltd.
}

void CPCDraftView::OnPenColorsRange(UINT nID)
{
   SaveUndo(IDS_PENCHANGE);
   PenModel pm;
   if(nID==ID_PEN_COLORS_BASE) pm.m_nPenType=kPenInvis;
   else
   {
      pm.m_nPenType=kPenColor;
      pm.m_crColor=PALETTEINDEX(nID-ID_PEN_COLORS_BASE-1);
   }
   if(m_bEditActive)
   {
      if(pm.m_nPenType==kPenColor)
      {
         int nStart,nEnd;
         m_InplaceEdit.GetSel(nStart,nEnd);
         if(nStart==nEnd) m_InplaceEdit.SetDefColor(pm.m_crColor);
         else m_InplaceEdit.ReplaceSelColor(pm.m_crColor);
      }
   }
   else ApplyPen(pm,pmUsePenType);
}

void CPCDraftView::OnToolAttribPenMenu()
{
   CColorPatternGradientMenu theMenu;
   theMenu.Create(
      &GetDocument()->m_Palette,
      IDS_PENCOLOR,
      ID_PEN_COLORS_BASE,
      GetDocument()->m_packeditema.GetData(),
      GetDocument()->m_packeditema.GetCount(),
      IDS_PENPATTERN,
      ID_PEN_PATTERNS_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

void CPCDraftView::OnToolAttribPenColor()
{
   CColorMenu theMenu;
   theMenu.Create(&GetDocument()->m_Palette,IDS_PENCOLOR,ID_PEN_COLORS_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

void CPCDraftView::OnUpdatePenPatternsRange(CCmdUI* pCmdUI)
{
   PenModel pm;
   switch(m_Selection.GetCount())
   {
   case 0:
      pm=GetDocument()->m_Pen;
      break;
   case 1:
      pm=(m_Selection.GetHead())->GetPenModel();
      break;
   default:
      pCmdUI->SetCheck(0);
      return;
   }
   BYTE nPattern=pCmdUI->m_nID-ID_PEN_PATTERNS_BASE-1;
   pCmdUI->SetCheck(pm.m_nPenType==kPenPat && pm.m_nIndex==nPattern);
}

void CPCDraftView::OnPenPatternsRange(UINT nID)
{
   SaveUndo(IDS_PENCHANGE);
   PenModel pm;
   if(nID==ID_PEN_PATTERNS_BASE) pm.m_nPenType=kPenInvis;
   else
   {
      pm.m_nPenType=kPenPat;
      pm.m_nIndex=nID-ID_PEN_PATTERNS_BASE-1;
   }
   if(m_bEditActive)
   {
      /*		if(pm.m_nPenType==kPenPat)
      		{
      			int nStart,nEnd;
      			m_InplaceEdit.GetSel(nStart,nEnd);
      			if(nStart==nEnd) m_InplaceEdit.SetDefColor(pm.m_crColor);
      			else m_InplaceEdit.ReplaceSelColor(pm.m_crColor);
      		}
      */
   }
   else ApplyPen(pm,pmUsePenType);
}

void CPCDraftView::OnToolAttribPenPattern()
{
   CPatternMenu theMenu;
   theMenu.Create(GetDocument()->m_packeditema.GetData(),GetDocument()->m_packeditema.GetCount(),IDS_PENPATTERN,ID_PEN_PATTERNS_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnToolAttribPenTransparency()
{

   CMainFrame * pframe = (CMainFrame *)GetTopLevelFrame();

   ShowTransparencyBar(NULL,&pframe->m_RibbonBar.m_properties.m_wndPenTransparency,attrib_pen);

}

// Opacity/Transparency - Infinisys Ltd
void CPCDraftView::OnToolAttribPenAlpha()
{

   CMainFrame * pframe = (CMainFrame *)GetTopLevelFrame();

   ShowTransparencyBar(NULL,&pframe->m_RibbonBar.m_properties.m_wndPenTransparency,attrib_pen);

}



static BYTE lineWeigths[]= {0x81,0xA1,0xC1,1,2,3,5,7,9,11,13};

void CPCDraftView::OnUpdateLineWidthRange(CCmdUI* pCmdUI)
{
   BYTE nWidth=lineWeigths[pCmdUI->m_nID-ID_LINE_WIDTH_BASE];
   switch(m_Selection.GetCount())
   {
   case 0:
      pCmdUI->SetCheck(GetDocument()->m_Pen.m_nWidth==nWidth);
      break;
   case 1:
      pCmdUI->SetCheck((m_Selection.GetHead())->GetPenModel().m_nWidth==nWidth);
      break;
   default:
      pCmdUI->SetCheck(0);
      break;
   }
}

void CPCDraftView::OnLineWidthRange(UINT nID)
{
   SaveUndo(IDS_PENCHANGE);
   PenModel pm;
   pm.m_nWidth=lineWeigths[nID-ID_LINE_WIDTH_BASE];
   ApplyPen(pm,pmUseWidth);
}

const PenModel& CPCDraftView::GetActivePenModelByLineWidth()
{
   BOOL bFromSelection;
   if(m_Selection.GetCount())
   {
      CMDShape *pShape = m_Selection.GetHead();
      const PenModel& fmTmp = pShape->GetPenModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const PenModel& fmTmpNext = pShape->GetPenModel();
         if (fmTmpNext.m_nWidth != fmTmp.m_nWidth)
         {
            bFromSelection = FALSE;
            break;
         }
      }

   }
   else
   {
      bFromSelection = FALSE;
   }

   return bFromSelection ? m_Selection.GetTail()->GetPenModel() : GetDocument()->m_Pen;
}

void CPCDraftView::OnUpdateToolAttribLineWeigth(CCmdUI* pCmdUI)
{
   const PenModel& pm = GetActivePenModelByLineWidth();
   pCmdUI->m_pOther->SendMessage(UM_LINEWIDTH_UPDATE,(pm.m_nPenType==kPenInvis || pm.m_nIndex==-1)?true:false,(short)pm.m_nWidth);
}

void CPCDraftView::OnIdleUpdateCmdUI()
{
   m_wndLineInfo.SetPen(GetActivePenModelByLineWidth());
}

void CPCDraftView::OnToolAttribLineWeigth()
{
   CBitmapMenu theMenu(IDB_WIDTH_MENU,11,ID_LINE_WIDTH_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

void CPCDraftView::OnUpdateLineStyleRange(CCmdUI* pCmdUI)
{
   BYTE nStyle=(pCmdUI->m_nID-ID_STYLES_BASE);
   switch(m_Selection.GetCount())
   {
   case 0:
      pCmdUI->SetCheck(GetDocument()->m_Pen.m_nStyle==nStyle);
      break;
   case 1:
      pCmdUI->SetCheck((m_Selection.GetHead())->GetPenModel().m_nStyle==nStyle);
      break;
   default:
      pCmdUI->SetCheck(0);
      break;
   }
}

void CPCDraftView::OnLineStyleRange(UINT nID)
{
   SaveUndo(IDS_LINESTYLECHANGE);
   PenModel pm;
   pm.m_nStyle=(nID-ID_STYLES_BASE);
   ApplyPen(pm,pmUseStyle);
}

void CPCDraftView::OnUpdateToolAttribLineStyle(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc=GetDocument();
   PenModel pm;


   BOOL bFromSelection;
   if(m_Selection.GetCount())
   {
      CMDShape *pShape = m_Selection.GetHead();
      const PenModel& fmTmp = pShape->GetPenModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const PenModel& fmTmpNext = pShape->GetPenModel();
         if (fmTmpNext.m_nStyle != fmTmp.m_nStyle)
         {
            bFromSelection = FALSE;
            break;
         }
      }
   }
   else
      bFromSelection = FALSE;

   pm.m_nStyle = bFromSelection ? m_Selection.GetTail()->GetPenModel().m_nStyle : pDoc->m_Pen.m_nStyle;
   pm.m_nWidth = 5;

   CBitmap bmp;
   CClientDC cdc(NULL);
   bmp.CreateCompatibleBitmap(&cdc,48,3);
   {
      CGDIBitmapDC dc(&cdc,&bmp);
      dc.MoveTo(0,0);

      dc.FillSolidRect(0, 0, 48, 3, RGB(255, 255, 255));

      CGDIBackColor bc(&dc,RGB(255,255,255));
      CGDITextColor tc(&dc,RGB(0,0,0));

      CDrawPrim drawprim;
      drawprim.setDashTable((const CDrawPrim::DashTable*)pDoc->m_DashStyles);
      CDraftContext draftcontext(&dc,&m_Mapper,pDoc);
      draftcontext.SetPen(pm);

#ifndef DEBUG_OTHER_DASHES_BUT_NOT_THIS_ONE
      drawprim.dashLine(&draftcontext,pointd(0,1), pointd(48,1),true);

      if (drawprim.m_helperPoly.has_points())
      {

         drawprim.m_helperPoly.PushPoints(drawprim.m_pfHead, drawprim.m_pfTail, &draftcontext);

         drawprim.m_helperPoly.DrawNow(&draftcontext);

         drawprim.m_helperPoly.Reset();

      }
#endif
   }

   pCmdUI->m_pOther->SendMessage(UM_SHOWBMP_UPDATE,1,LPARAM(bmp.Detach()));
}

void CPCDraftView::OnToolAttribLineStyle()
{
   CLineStyleMenu theMenu;
   theMenu.Create(GetDocument()->m_DashStyles,ID_STYLES_BASE);
   theMenu.CmdTrack(AfxGetMainWnd());
}

extern UINT UM_SETCAPS;

void CPCDraftView::OnUpdateToolAttribEndMark(CCmdUI* pCmdUI)
{
   short nCap1,nCap2;

   BOOL bFromSelection;
   if(m_Selection.GetCount())
   {
      CMDShape *pShape = m_Selection.GetHead();
      const PenModel& fmTmp = pShape->GetPenModel();
      nCap1 = fmTmp.EndCap1();
      nCap2 = fmTmp.EndCap2();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while (pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const PenModel& fmTmpNext = pShape->GetPenModel();
         if (fmTmpNext.m_nEndCaps != fmTmp.m_nEndCaps)
         {
            bFromSelection = FALSE;
            break;
         }
      }

   }
   else
      bFromSelection = FALSE;

   const PenModel& pm = bFromSelection ? m_Selection.GetTail()->GetPenModel() : GetDocument()->m_Pen;
   nCap1 = pm.EndCap1();
   nCap2 = pm.EndCap2();
   pCmdUI->m_pOther->SendMessage(UM_SETCAPS,MAKEWPARAM(nCap1,nCap2));
}

void CPCDraftView::OnToolAttribEndMark()
{

   CPoint p;
   GetCursorPos(&p);
   CPoint pt(p);

   CEndmarksMenu theMenu;
   int cmd=theMenu.Track(this);
   if(cmd>=0)
   {
      SaveUndo(IDS_ENDMARK);
      short value=cmd/3;
      cmd%=3;

      PenModel pm;
      pm.m_nEndCaps=(value<<4)|value;
      int useCap=0;
      if(cmd<2) useCap|=pmUseEndCap1;
      if(cmd>0) useCap|=pmUseEndCap2;
      if(!ApplyPen(pm,useCap)) MessageBeep(MB_ICONHAND);
   }
}

void CPCDraftView::OnUpdateEditDatum(CCmdUI* pCmdUI)
{
#ifndef _PERSONAL_EDITION
   CString strText;
   if(m_nCurrentTool==ID_TOOL_EDIT_DATUM)
   {
      strText.LoadString(IDS_CENTERDATUM);
      pCmdUI->SetText(strText);
      pCmdUI->Enable(m_Selection.GetCount()>0);
   }
   else
   {
      strText.LoadString(IDS_EDITDATUM);
      pCmdUI->SetText(strText);
      pCmdUI->Enable(m_Selection.GetCount()==1 && !m_Selection.GetHead()->IsLocked() && !m_Selection.GetHead()->IsKindOf(RUNTIME_CLASS(CMDArea)));
   }
#endif
}

void CPCDraftView::OnEditDatum()
{
#ifndef _PERSONAL_EDITION
   if(m_nCurrentTool==ID_TOOL_EDIT_DATUM) m_Selection.GetHead()->SetShapeDatum(kCenterDatum);
   else
   {
      SaveUndo(0);
      OnToolsRange(ID_TOOL_EDIT_DATUM);
   }
   if(m_Selection.GetCount()) InvalidateShape(m_Selection.GetHead(),true);
#endif
}

void CPCDraftView::OnArrangeFlipHoriz()
{
   CancelCurrentTool();
   FlipSelection(false);
}

void CPCDraftView::OnArrangeFlipVert()
{
   CancelCurrentTool();
   FlipSelection(true);
}

void CPCDraftView::FlipSelection(bool bVertical)
{
   BeginSaveUndo();
   UnselectShapes(usLocked);
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->IsLocked()) continue;
      pShape->DoShapeFlip(bVertical);
      pShape->ReCalc(&m_Mapper);
      pShape->UpdateDependentShapes(this);
      InvalidateShape(pShape,true);
   }
   if(RecalcLinkedVertexes())
   {
      CommitSaveUndo(IDS_FLIP);
      ForceResizeUpdate();
      GetDocument()->SetModifiedFlag();
   }
   else
   {
      AbortSaveUndo(true);
      AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
   }
}

void CPCDraftView::UnselectShapes(int nFlags)
{
   POSITION pos,posn=m_Selection.GetHeadPosition();
   while(posn)
   {
      pos=posn;
      CMDShape* pShape=m_Selection.GetNext(posn);
      if((nFlags&usLocked && pShape->IsLocked()) || (nFlags&usDimStr && pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr))))
      {
         m_Selection.RemoveAt(pos);
         InvalidateShape(pShape,true);
      }
   }
}

void CPCDraftView::OnUpdateArrangeGroup(CCmdUI* pCmdUI)
{
   switch(m_Selection.GetCount())
   {
   case 1:
      if(!m_Selection.GetHead()->IsKindOf(RUNTIME_CLASS(CMDGroupShape))) break;
   case 0:
      pCmdUI->Enable(FALSE);
      return;
   }
   pCmdUI->Enable(HaveUnlockedSelection() && m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool);
}

void CPCDraftView::OnArrangeGroup()
{
   SaveUndo(IDS_GROUP);

   if(m_Selection.GetCount()==2)
   {
      CMDArea* pArea=(CMDArea*)m_Selection.GetHead();
      if(pArea->IsKindOf(RUNTIME_CLASS(CMDArea)) && pArea->GetOwner()==m_Selection.GetTail())
      {
         pArea->Attach();
         return;
      }
      pArea=(CMDArea*)m_Selection.GetTail();
      if(pArea->IsKindOf(RUNTIME_CLASS(CMDArea)) && pArea->GetOwner()==m_Selection.GetHead())
      {
         pArea->Attach();
         return;
      }
   }

   UnselectShapes(usLocked);
   CMDShapeList groupList;
   CPCDraftDoc* pDoc=GetDocument();
   CLayer *pLayer=pDoc->GetActiveLayer();
   CMDShapeArray& layerList=pLayer->m_ShapeList;

   POSITION ipos=NULL;
   POSITION pos=layerList.GetHeadPosition();
   while(pos && m_Selection.GetCount())
   {
      POSITION rpos=pos;
      CMDShape* pShape=layerList.GetNext(pos);
      POSITION spos=m_Selection.Find(pShape);
      if(spos)
      {
         m_Selection.RemoveAt(spos);
         if(groupList.GetCount())
         {
            layerList.RemoveAt(rpos); // don't remove top shape - will be replaced on create
            pos--;
         }
         else ipos=rpos;
         groupList.AddTail(pShape);
      }
   }

   CMDGroupShape* pShape=new CMDGroupShape(&groupList);
   pShape->CalculateExtents();
   pShape->SetShapeDatum(kDefaultDatum);

//	pDoc->CreateShape(...)
   pDoc->GetActiveLayer()->m_ShapeList.InsertBefore(ipos,pShape);
   ipos++;
   layerList.RemoveAt(ipos);
   pDoc->UpdateAllViews(NULL,kRefreshShape,pShape);
   pDoc->SetModifiedFlag();

   SelectShape(pShape,true);
}

void CPCDraftView::OnUpdateArrangeUngroup(CCmdUI* pCmdUI)
{
   if(m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool)
   {
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(pShape->IsLocked()) continue;
         if(pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
         {
            pCmdUI->Enable();
            return;
         }
         else
         {
            if(!pShape->IsKindOf(RUNTIME_CLASS(CMDArea)) && pShape->GetAttachedShape())
            {
               pCmdUI->Enable();
               return;
            }
         }
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnArrangeUngroup()
{
   SaveUndo(IDS_UNGROUP);

   UnselectShapes(usLocked);
   CMDShapeList shapeList;
   shapeList.AddTail(&m_Selection);	// make a copy

   m_Selection.RemoveAll();
   while(shapeList.GetCount())
   {
      CMDShape* pShape=shapeList.RemoveHead();
      if(pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
      {
         CMDShapeList pList;
         ((CMDGroupShape*)pShape)->Ungroup(&pList);
         GetDocument()->CreateShapes(&pList,pShape);
      }
      else
      {
         if(pShape->IsAreaShown())
         {
            CMDArea* pArea=(CMDArea*)pShape->GetAttachedShape();
            pArea->Detach();
            InvalidateShape(pArea,true);
            InvalidateShape(pShape,true);
         }
      }
   }
}

void CPCDraftView::OnUpdateToolExtend(CCmdUI* pCmdUI)
{
   // enable if any lines selected
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape))
      {
         pCmdUI->Enable();
         return;
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnToolExtend()
{
   CancelCurrentTool();
   if(m_Selection.GetCount()==2)
   {
      CMDShape* pLine1=m_Selection.GetHead();
      CMDShape* pLine2=m_Selection.GetTail();
      if(!pLine1->IsLocked() && !pLine2->IsLocked()
            && pLine1->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape)
            && pLine2->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape)
            && GetAsyncKeyState(VK_OPTION)>=0)
      {
         LongPoint lpt1,lpt2,lpt3,lpt4,lipt;
         lpt1=pLine1->GetShapePoint(0);
         lpt2=pLine1->GetShapePoint(1);
         lpt3=pLine2->GetShapePoint(0);
         lpt4=pLine2->GetShapePoint(1);
         if(guLongLnLnPt(lpt1,lpt2,lpt3,lpt4,&lipt))
         {
            BeginSaveUndo();
            LongRect r=guPt2Rect(lpt1,lpt2);
            r.Inset(-FixedMinDelta,-FixedMinDelta);
            if(!r.Contains(lipt))
            {
               double dist1 = RawDoublePythagSqr(lpt1-lipt);
               double dist2 = RawDoublePythagSqr(lpt2-lipt);
               InvalidateShape(pLine1,true);
               if(dist1>dist2) pLine1->SetShapePoint(1,lipt);
               else pLine1->SetShapePoint(0,lipt);
               pLine1->CalculateExtents();
               InvalidateShape(pLine1,true);
            }
            r=guPt2Rect(lpt3,lpt4);
            r.Inset(-FixedMinDelta,-FixedMinDelta);
            if(!r.Contains(lipt))
            {
               double dist3 = RawDoublePythagSqr(lpt3-lipt);
               double dist4 = RawDoublePythagSqr(lpt4-lipt);
               InvalidateShape(pLine2,true);
               if(dist3>dist4) pLine2->SetShapePoint(1,lipt);
               else pLine2->SetShapePoint(0,lipt);
               pLine2->CalculateExtents();
               InvalidateShape(pLine2,true);
            }
            GetDocument()->SetModifiedFlag();

            UINT strId;
            if(RecalcLinkedVertexes(false))
            {
               if(GetDocument()->CheckDocSize(false))
               {
                  CommitSaveUndo(IDS_EXTEND);
                  ForceResizeUpdate();
                  Invalidate(FALSE);
                  return;
               }
               strId=IDS_CAN_NOT_EXTEND;
            }
            else strId=IDS_UNABLE_COMPLETE_ACTION;
            AbortSaveUndo(true);
            AfxMessageBox(strId);
         }
         else AfxMessageBox(IDS_LINES_DO_NOT_INTERSECT);
         return;
      }
   }
   CExtendLineDlg theDlg;
   if(theDlg.DoModal()==IDOK)
   {
      m_nExtendTo=theDlg.m_nExtendTo;
   }
   else return;
   SetOneTimeTool(ID_TOOL_EXTEND);
}

void CPCDraftView::OnToolTrim()
{
   CancelCurrentTool();
   if(GetAsyncKeyState(VK_OPTION)<0)
   {
      CTrimSetupDlg theDlg(m_pTrimCtx);
      theDlg.DoModal();
   }
   else
   {
      if(m_pTrimCtx->CanTrim(&m_Selection))
      {
         SaveUndo(IDS_TRIM);
         CLinkVertexList* pList=&GetActiveLinkVertexList();
         m_pTrimCtx->m_pLines[0]->DeleteLinks(pList);
         m_pTrimCtx->m_pLines[1]->DeleteLinks(pList);
         m_pTrimCtx->Trim();
         if(m_pTrimCtx->m_pNewLines[0])
         {
            GetDocument()->CreateShape(m_pTrimCtx->m_pNewLines[0]);
            SelectShape(m_pTrimCtx->m_pNewLines[0],true);
         }
         if(m_pTrimCtx->m_pNewLines[1])
         {
            GetDocument()->CreateShape(m_pTrimCtx->m_pNewLines[1]);
            SelectShape(m_pTrimCtx->m_pNewLines[1],true);
         }
         Invalidate(FALSE);
         GetDocument()->SetModifiedFlag();
      }
      else AfxMessageBox(IDS_TRIM_2LINES_ERR);
   }
}

void CPCDraftView::OnToolFillet()
{
   CancelCurrentTool();
   if(GetAsyncKeyState(VK_OPTION)<0)
   {
      CFilletSetupDlg theDlg(GetDocument(),m_pFilletCtx);
      theDlg.DoModal();
   }
   else
   {
      if(m_pFilletCtx->CanFillet(&m_Selection))
      {
         int nFillet=m_pFilletCtx->CreateFillet();
         if(nFillet)
         {
            if(nFillet>0) FinishCreateFillet(nFillet-1);
            else
            {
               SetOneTimeTool(ID_TOOL_FILLET);
               Invalidate(FALSE);
            }
         }
      }
      else AfxMessageBox(IDS_FILLET_2LINES_ERR);
   }
}

void CPCDraftView::FinishCreateFillet(int nFillet)
{
   BeginSaveUndo();
   InvalidateShape(m_pFilletCtx->m_pLines[0],true);
   InvalidateShape(m_pFilletCtx->m_pLines[1],true);
   CLinkVertexList* pList=&GetActiveLinkVertexList();
   m_pFilletCtx->m_pLines[0]->DeleteLinks(pList);
   m_pFilletCtx->m_pLines[1]->DeleteLinks(pList);
   CMDShape* pFillet=m_pFilletCtx->FinishCreateFillet(nFillet);
   GetDocument()->CreateShape(pFillet);
   SelectShape(pFillet,true);

   if(m_pFilletCtx->m_pNewLines[0])
   {
      GetDocument()->CreateShape(m_pFilletCtx->m_pNewLines[0]);
      SelectShape(m_pFilletCtx->m_pNewLines[0],true);
   }
   else InvalidateShape(m_pFilletCtx->m_pLines[0],true);
   if(m_pFilletCtx->m_pNewLines[1])
   {
      GetDocument()->CreateShape(m_pFilletCtx->m_pNewLines[1]);
      SelectShape(m_pFilletCtx->m_pNewLines[1],true);
   }
   else InvalidateShape(m_pFilletCtx->m_pLines[1],true);
   if(GetDocument()->CheckDocSize(false)) CommitSaveUndo(IDS_FILLET);
   else
   {
      AbortSaveUndo();
      AfxMessageBox(IDS_CAN_NOT_MAKE_FILLET);
   }
}

void CPCDraftView::OnToolChamfer()
{
   CancelCurrentTool();
   if(GetAsyncKeyState(VK_OPTION)<0)
   {
      CChamferSetupDlg theDlg(GetDocument(),m_pChamferCtx);
      theDlg.DoModal();
   }
   else
   {
      if(m_Selection.GetCount()==0) SetOneTimeTool(ID_TOOL_CHAMFER);
      else if(m_pChamferCtx->CanChamfer(&m_Selection)) DoChamfer();
      else AfxMessageBox(IDS_CHAMFER_2LINES_ERR);
   }
}

void CPCDraftView::OnToolMirror()
{
   if(GetAsyncKeyState(VK_OPTION)<0)
   {
      CMirrorSetupDlg theDlg;
      theDlg.m_nMirror=((CPCDraftApp*)AfxGetApp())->m_nMirrorMethod;
      if(theDlg.DoModal()==IDOK)
      {
         ((CPCDraftApp*)AfxGetApp())->m_nMirrorMethod=theDlg.m_nMirror;
      }
   }
   else
   {
      if(HaveUnlockedSelection()) SetOneTimeTool(ID_TOOL_MIRROR);
      else AfxMessageBox(IDS_SELECTION_INCOMPATIBLE);
   }
}

void CPCDraftView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_TAB:
      if(!m_bEditActive && m_pResizeShape)
      {
         ((CMainFrame*)AfxGetMainWnd())->DoActivateResizePalette();
      }
      break;
   case VK_UP:
      if(m_bEditActive) m_InplaceEdit.GoUp();
      else DoNudge(0,-1);
      break;

   case VK_DOWN:
      if(m_bEditActive) m_InplaceEdit.GoDown();
      else DoNudge(0,1);
      break;

   case VK_LEFT:
      if(m_bEditActive) m_InplaceEdit.GoLeft();
      else DoNudge(-1,0);
      break;

   case VK_RIGHT:
      if(m_bEditActive) m_InplaceEdit.GoRight();
      else DoNudge(1,0);
      break;

   case VK_HOME:
      if(m_bEditActive) m_InplaceEdit.GoHome();
      break;

   case VK_END:
      if(m_bEditActive) m_InplaceEdit.GoEnd();
      break;

   case VK_DELETE:
      if(m_bEditActive) m_InplaceEdit.DoDelete();
      else if(m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool) OnEditClear();
      break;

   case VK_BACK:
      if(!m_bEditActive && (m_nCurrentTool!=ID_TOOL_EDIT_DATUM && !m_nOneTimeTool)) OnEditClear();
      break;

   case VK_ESCAPE:
      if(!m_bEditActive) CancelCurrentTool();
      break;

   case VK_SPACE:
      if(!m_bEditActive && m_nCurrentTool==ID_TOOL_ARROW) OnToolsRange(ID_TOOL_PAN); //SetOneTimeTool(ID_TOOL_PAN);
      break;

   case VK_SHIFT:
   case VK_COMMAND:
      if(!m_bEditActive && (nFlags&0x4000)==0) UpdateCursor();
      break;
   }
}

void CPCDraftView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_SHIFT:
   case VK_COMMAND:
      if(!m_bEditActive) UpdateCursor();
      break;
   }

//	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

bool CPCDraftView::HaveTextParamSelection(int nWhat)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && pShape->HaveTextParams(nWhat))
      {
         return true;
      }
   }
   return false;
}

void CPCDraftView::GetActiveTextParams(TextParams &tp, int nWhat)
{
   if(m_bEditActive) m_InplaceEdit.GetCaretTextParams(tp);
   else
   {
      int nFound=0;
      if(m_Selection.GetCount())
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(!pShape->IsLocked() && pShape->HaveTextParams(nWhat))
            {
               if(!nFound) tp=pShape->GetTextParams();
               nFound++;
            }
         }
      }
      if(nFound!=1) tp=GetDocument()->m_TextParams;
   }
}

void CPCDraftView::ApplyText(const TextParams& tp, int nWhat)
{
   CPCDraftDoc* pDoc=GetDocument();
   bool bFound=false;

   if(m_Selection.GetCount())
   {
      CClientDC cdc(this);
      CDraftContext dc(&cdc,&m_Mapper);
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(!pShape->IsLocked() && pShape->HaveTextParams(nWhat))
         {
            InvalidateShape(pShape,true);
            pShape->ModifyTextParams(tp,nWhat,&dc);
            pShape->UpdateDependentShapes(this);
            pShape->CalculateExtents();
            InvalidateShape(pShape,true);
            bFound=true;
         }
      }
   }
   if(!bFound) pDoc->m_TextParams.Modify(tp,nWhat);
   pDoc->SetModifiedFlag();
}

void CPCDraftView::OnTextFont()
{
   TextParams tp;
   GetActiveTextParams(tp,tpUseFont);
   CString strFont = get_font_english_name(tp.m_lf.lfFaceName);
   strncpy(tp.m_lf.lfFaceName, strFont, sizeof(tp.m_lf.lfFaceName));
   CFontDialog theDlg(&tp.m_lf,CF_SCREENFONTS|CF_TTONLY|CF_EFFECTS|CF_ENABLETEMPLATE);
   theDlg.m_cf.lpTemplateName=MAKEINTRESOURCE(IDD_CHOOSE_FONT);
   theDlg.m_cf.hInstance=AfxGetResourceHandle();
   if(theDlg.DoModal()==IDOK)
   {
      CStringW wstr = get_uni(tp.m_lf.lfFaceName, CP_ACP);
      theDlg.GetCurrentFont(&tp.m_lf);
      CString strFont = get_font_english_name(tp.m_lf.lfFaceName);
      strncpy(tp.m_lf.lfFaceName, strFont, sizeof(tp.m_lf.lfFaceName));
      if(m_bEditActive)
      {
         int nStart,nEnd;
         m_InplaceEdit.GetSel(nStart,nEnd);
         if(nStart==nEnd) m_InplaceEdit.SetDefFont(tp.m_lf);
         else m_InplaceEdit.ReplaceSelFont(tp.m_lf);
      }
      else ApplyText(tp,tpUseFont);
   }
}

void CPCDraftView::OnUpdateTextJustifyRange(CCmdUI* pCmdUI)
{
   TextParams tp;
   GetActiveTextParams(tp,tpUseJust);
   pCmdUI->SetCheck(tp.m_nJust==int(pCmdUI->m_nID-ID_TEXT_JUSTIFY_LEFT));
}

void CPCDraftView::OnTextJustifyRange(UINT nID)
{
   int nAlign=(nID-ID_TEXT_JUSTIFY_LEFT);
   if(m_bEditActive)
   {
      m_InplaceEdit.SetTextAlign(nAlign);
   }
   else
   {
      SaveUndo(IDS_TEXTJUST);
      TextParams tp;
      tp.m_nJust=nAlign;
      ApplyText(tp,tpUseJust);
   }
}

void CPCDraftView::OnUpdateTextVAlignRange(CCmdUI* pCmdUI)
{
   TextParams tp;
   GetActiveTextParams(tp,tpUseVAlign);
   pCmdUI->SetCheck(tp.m_nVAlign==int(pCmdUI->m_nID-ID_TEXT_JUSTIFY_TOP));
   pCmdUI->Enable(!m_bEditActive);
}

void CPCDraftView::OnTextVAlignRange(UINT nID)
{
   int nVAlign=(nID-ID_TEXT_JUSTIFY_TOP);
   SaveUndo(IDS_TEXTJUST);
   TextParams tp;
   tp.m_nVAlign=nVAlign;
   ApplyText(tp,tpUseVAlign);
}

void CPCDraftView::OnUpdateTextSpacingRange(CCmdUI* pCmdUI)
{
   TextParams tp;
   GetActiveTextParams(tp,tpUseSpacing);
   pCmdUI->SetCheck(tp.m_nSpacing==int(pCmdUI->m_nID-ID_TEXT_LINESPACING_SINGLE));
}

void CPCDraftView::OnTextSpacingRange(UINT nID)
{
   int nSpacing=(nID-ID_TEXT_LINESPACING_SINGLE);
   if(m_bEditActive)
   {
      m_InplaceEdit.SetLineSpace(nSpacing);
   }
   else
   {
      SaveUndo(IDS_TEXTSPACE);
      TextParams tp;
      tp.m_nSpacing=nSpacing;
      ApplyText(tp,tpUseSpacing);
   }
}

void CPCDraftView::OnUpdateTextCaseRange(CCmdUI* pCmdUI)
{
   if(GetDocument()->m_bHideText) pCmdUI->Enable(FALSE);
   else
   {
      if(m_bEditActive) pCmdUI->Enable();
      else
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(!pShape->IsLocked() && pShape->HaveTextParams(tpUseCase))
            {
               pCmdUI->Enable();
               return;
            }
         }
         pCmdUI->Enable(FALSE);
      }
   }
}

void CPCDraftView::OnTextCaseRange(UINT nID)
{
   int nCase=(nID-ID_TEXT_UPPERCASE);
   if(m_bEditActive)
   {
      m_InplaceEdit.SetCase(nCase);
   }
   else
   {
      SaveUndo(IDS_TEXTCASE);
      TextParams tp;
      tp.m_nCase=nCase;
      ApplyText(tp,tpUseCase);
   }
}

void CPCDraftView::OnUpdateTextSelectalltext(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc=GetDocument();
   if(pDoc->m_bHideText || m_bEditActive) pCmdUI->Enable(FALSE);
   else
   {
      CLayer* pActiveLayer=pDoc->GetActiveLayer();
      POSITION pos=pActiveLayer->m_ShapeList.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=pActiveLayer->m_ShapeList.GetNext(pos);
         if(pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
         {
            pCmdUI->Enable();
            return;
         }
      }
      pCmdUI->Enable(FALSE);
   }
}

void CPCDraftView::OnTextSelectalltext()
{
   SaveUndo(0);
   SelectNone();
   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pActiveLayer=pDoc->GetActiveLayer();
   POSITION pos=pActiveLayer->m_ShapeList.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=pActiveLayer->m_ShapeList.GetNext(pos);
      if(pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
      {
         SelectShape(pShape,true);
         InvalidateShape(pShape,true);
      }
   }
}

void CPCDraftView::OnUpdateEditReshapeSmooth(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(CanReshapeSmooth());
}

void CPCDraftView::OnEditReshapeSmooth()
{
   SaveUndo(IDS_SMOOTH);
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->CanSmooth() && !pShape->IsLocked())
      {
         InvalidateShape(pShape,true);
         pShape->Smooth();
         pShape->CalculateExtents();
         pShape->UpdateDependentShapes(this);
         InvalidateShape(pShape,true);
      }
   }
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateEditReshapeUnsmooth(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(CanReshapeUnsmooth());
}

void CPCDraftView::OnEditReshapeUnsmooth()
{
   SaveUndo(IDS_UNSMOOTH);
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->CanUnsmooth() && !pShape->IsLocked())
      {
         InvalidateShape(pShape,true);
         pShape->Unsmooth();
         pShape->CalculateExtents();
         pShape->UpdateDependentShapes(this);
         InvalidateShape(pShape,true);
      }
   }
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnToolRotate90()
{
   DeactivateTextShape();
   BeginSaveUndo();
   CancelCurrentTool();
   UnselectShapes(usLocked);

   bool bShowAlert=true;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsKindOf(RUNTIME_CLASS(CMDBitmap)))
      {
         bShowAlert=false;
         break;
      }
   }

   if(bShowAlert)
   {
      AbortSaveUndo(true);
      AfxMessageBox(IDS_SELECTION_INCOMPATIBLE);
      return;
   }
   if(!DoShapeRotate(GetAsyncKeyState(VK_SHIFT)<0?-90:90,GetAsyncKeyState(VK_OPTION)<0?kAboutDatum:kAboutCenter,LongPoint::kZeroPt))
   {
      AbortSaveUndo(true);
      AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
      return;
   }
   CommitSaveUndo(IDS_ROTATE);
   ForceResizeUpdate();
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnArrangeRotateZero()
{
   BeginSaveUndo();
   UnselectShapes(usLocked);
   bool bLinkedSelection=false;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      CMDShape* pLinkedShape;
      if(IsLinkedShape(pShape,-1,&pLinkedShape) && IsSelectedShape(pLinkedShape,true))
      {
         bLinkedSelection=true;
         break;
      }
      InvalidateShape(pShape,true);
      pShape->DoShapeRotate(-pShape->GetRotation(),kAboutCenter,LongPoint::kZeroPt);
      pShape->ReCalc(&m_Mapper);
      pShape->UpdateDependentShapes(this);
      InvalidateShape(pShape,true);
   }
   if(!bLinkedSelection && RecalcLinkedVertexes())
   {
      CommitSaveUndo(IDS_ROTATETOZERO);
      ForceResizeUpdate();
      GetDocument()->SetModifiedFlag();
   }
   else
   {
      AbortSaveUndo(true);
      if(bLinkedSelection) MessageBeep(MB_ICONHAND);
      else AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
   }
}

void CPCDraftView::OnDataFieldNames()
{
   CFieldNamesDlg theDlg;
   theDlg.m_sName1=GetDocument()->m_sFieldNames[0];
   theDlg.m_sName2=GetDocument()->m_sFieldNames[1];
   theDlg.m_sName3=GetDocument()->m_sFieldNames[2];
   theDlg.m_sName4=GetDocument()->m_sFieldNames[3];
   theDlg.m_sName5=GetDocument()->m_sFieldNames[4];
   if(theDlg.DoModal()==IDOK)
   {
      GetDocument()->m_sFieldNames[0]=theDlg.m_sName1;
      GetDocument()->m_sFieldNames[1]=theDlg.m_sName2;
      GetDocument()->m_sFieldNames[2]=theDlg.m_sName3;
      GetDocument()->m_sFieldNames[3]=theDlg.m_sName4;
      GetDocument()->m_sFieldNames[4]=theDlg.m_sName5;
      GetDocument()->SetModifiedFlag();
   };
}

void CPCDraftView::OnEditSelectSpecial()
{
   CSelectSpecialDlg theDlg;
   if(theDlg.DoModal()==IDOK)
   {
      SaveUndo(0);
      SelectNone();
      CPCDraftDoc* pDoc=GetDocument();
      CLayer* pLayer=pDoc->GetActiveLayer();
      POSITION pos=pLayer->m_ShapeList.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos);
         bool bOk=false;
         switch(theDlg.m_nObjectClass)
         {
         case CSelectSpecialDlg::kAllClass:
            bOk=true;
            break;
         case CSelectSpecialDlg::kText:
            bOk=pShape->IsKindOf(RUNTIME_CLASS(CMDText))?true:false;
            break;
         case CSelectSpecialDlg::kGeneralShape:
            switch(theDlg.m_nObjectType)
            {
            case CSelectSpecialDlg::kAllGeneralShape:
               bOk=pShape->IsKindOf(RUNTIME_CLASS(CMDLineShape))?true:false;
               bOk|=pShape->IsKindOf(RUNTIME_CLASS(CMDRectangle))?true:false;
               bOk|=pShape->IsKindOf(RUNTIME_CLASS(CMDCircle))?true:false;
               bOk|=pShape->IsKindOf(RUNTIME_CLASS(CMDArcShape))?true:false;
               bOk|=pShape->IsKindOf(RUNTIME_CLASS(CMDPolygonShape))?true:false;
               break;
            case CSelectSpecialDlg::kLines:
               bOk=(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineShape));
               break;
            case CSelectSpecialDlg::kRectangles:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDRectangle);
               break;
            case CSelectSpecialDlg::kRoundedRect:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDRndRectangle);
               break;
            case CSelectSpecialDlg::kCircles:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDCircle);
               break;
            case CSelectSpecialDlg::kCircularArcs:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDArcShape))
               {
                  bOk=((CMDArcShape*)pShape)->m_nMode!=CMDArcShape::kElliptic;
               }
               break;
            case CSelectSpecialDlg::kEllipticalArcs:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDArcShape))
               {
                  bOk=((CMDArcShape*)pShape)->m_nMode==CMDArcShape::kElliptic;
               }
               break;
            case CSelectSpecialDlg::kOvals:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDEllipse);
               break;
            case CSelectSpecialDlg::kBezierCurves:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDBezierShape);
               break;
            case CSelectSpecialDlg::kSplineCurves:
               if(pShape->IsKindOf(RUNTIME_CLASS(CMDPolygonShape)))
               {
                  int nType=((CMDPolygonShape*)pShape)->GetPolygonType();
                  bOk=(nType==CMDPolygonShape::kFreeSpline || nType==CMDPolygonShape::kPolySpline || nType==CMDPolygonShape::kCubic);
               }
               break;
            case CSelectSpecialDlg::kPolygons:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDPolygonShape))
               {
                  bOk=((CMDPolygonShape*)pShape)->GetPolygonType()==CMDPolygonShape::kPolygon && pShape->IsClosed();
               }
               break;
            case CSelectSpecialDlg::kPolylines:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDPolygonShape))
               {
                  int nType=((CMDPolygonShape*)pShape)->GetPolygonType();
                  bOk=(nType==CMDPolygonShape::kFreehand || (nType==CMDPolygonShape::kPolygon && !pShape->IsClosed()));
               }
               break;
            case CSelectSpecialDlg::kParallelLines:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDParallelLine);
               break;
            case CSelectSpecialDlg::kParallelPolygons:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDParallelPolygon) && pShape->IsClosed();
               break;
            case CSelectSpecialDlg::kParallelPolylines:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDParallelPolygon) && !pShape->IsClosed();
               break;
            default:
               ASSERT(FALSE);
            }
            break;
         case CSelectSpecialDlg::kDimension:
            switch(theDlg.m_nObjectType)
            {
            case CSelectSpecialDlg::kAllDimension:
               bOk=pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim))?true:false;
               break;
            case CSelectSpecialDlg::kHorizontal:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineDim))
               {
                  bOk=((CMDLineDim*)pShape)->m_nType==CMDLineDim::kHoriz;
               }
               break;
            case CSelectSpecialDlg::kVertical:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineDim))
               {
                  bOk=((CMDLineDim*)pShape)->m_nType==CMDLineDim::kVert;
               }
               break;
            case CSelectSpecialDlg::kSlope:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineDim))
               {
                  bOk=((CMDLineDim*)pShape)->m_nType==CMDLineDim::kSlope;
               }
               break;
            case CSelectSpecialDlg::kPerpendicular:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDLineDim))
               {
                  bOk=((CMDLineDim*)pShape)->m_nType==CMDLineDim::kPerp;
               }
               break;
            case CSelectSpecialDlg::kDiameter:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDRadialDim))
               {
                  bOk=((CMDRadialDim*)pShape)->m_bDiameter;
               }
               break;
            case CSelectSpecialDlg::kRadius:
               if(pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDRadialDim))
               {
                  bOk=!((CMDRadialDim*)pShape)->m_bDiameter;
               }
               break;
            case CSelectSpecialDlg::kCircleCenter:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDCenterDim);
               break;
            case CSelectSpecialDlg::kAngular:
               bOk=pShape->GetRuntimeClass()==RUNTIME_CLASS(CMDAngularDim);
               break;
            default:
               ASSERT(FALSE);
            }
            break;
         default:
            ASSERT(FALSE);
         }
         if(bOk && pShape->IsKindOf(RUNTIME_CLASS(CMDText)) && pDoc->m_bHideText)
         {
            bOk=false;
         }
         bool bGroup=pShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape))?true:false;
         if(bOk)
         {
            const PenModel& pm=pShape->GetPenModel();
            if(bOk && theDlg.m_bUsePen)
            {
               switch(pm.m_nPenType)
               {
               case kPenInvis:
                  bOk=(theDlg.m_nPenValue<0);
                  break;
               case kPenColor:
                  bOk=(theDlg.m_nPenKind==0)&&(pm.m_crColor==PALETTEINDEX(theDlg.m_nPenValue));
                  break;
               case kPenPat:
                  bOk=(theDlg.m_nPenKind==1)&&(pm.m_nIndex==theDlg.m_nPenValue);
                  break;
               default:
                  bOk=false;
               }
               if(bGroup) bOk=false;  // groups do not have pens
            }
            if(bOk && theDlg.m_bUseLineWeight)
            {
               if(bGroup || pm.m_nWidth!=theDlg.m_nLineWidthValue)
                  bOk=false;
            }
            if(bOk && theDlg.m_bUseLineStyle)
            {
               if(bGroup || pm.m_nStyle!=theDlg.m_nLineStyleValue)
                  bOk=false;
            }
         }
         if(bOk && theDlg.m_bUseFill)
         {
            const FillModel& fm=pShape->GetFillModel();
            switch(fm.m_nFillType)
            {
            case kNoFill:
               bOk=(theDlg.m_nFillValue<0);
               break;
            case kFillColor:
               bOk=(theDlg.m_nFillKind==0)&&(fm.m_crColor==PALETTEINDEX(theDlg.m_nFillValue));
               break;
            case kFillPat:
               bOk=(theDlg.m_nFillKind==1)&&(fm.m_nIndex==theDlg.m_nFillValue);
               break;
            default:
               bOk=false;
            }
            if(bGroup) bOk=false; // groups do not have fill
         }
         if(bOk && (theDlg.m_bUse[0] || theDlg.m_bUse[1] || theDlg.m_bUse[2] || theDlg.m_bUse[3] || theDlg.m_bUse[4]))
         {
            CString sValues[5];
            pShape->GetValues(sValues);
            for(int i=0; i<5; i++)
               if(theDlg.m_bUse[i] && theDlg.m_sValues[i]!=sValues[i])
               {
                  bOk=false;
                  break;
               }
         }
         if(bOk)
            SelectShape(pShape,true);
      }
      if(!m_Selection.GetCount()) MessageBeep(MB_ICONHAND);
   }
}

void CPCDraftView::OnFileLoadLayer()
{
   CPCDraftDoc* pDoc=GetDocument();
   CString strFilter;
   strFilter.LoadString(
#ifndef _PERSONAL_EDITION
   IDS_LOADLAYERFILTERS
#else  //PERSONAL_EDITION	
   IDS_LOADLAYERFILTERSPE
#endif
   );
   CFileDialog theDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,strFilter);
   /*
   #ifndef _PERSONAL_EDITION
   static gFilterIndex=pDoc->m_bSaveAsMacDraft?3:1;
   	theDlg.m_ofn.nFilterIndex=gFilterIndex;
   #endif
   */
   if(theDlg.DoModal()==IDOK)
   {
      /*
      #ifndef _PERSONAL_EDITION
      		gFilterIndex=theDlg.m_ofn.nFilterIndex;
      #endif
      */
      if(theDlg.GetFileExt().CompareNoCase("bmp")==0)
      {
         HBITMAP hDib=(HBITMAP)::LoadImage(NULL,theDlg.GetPathName(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTCOLOR|LR_CREATEDIBSECTION);
         if(hDib)
         {
#ifdef _PERSONAL_EDITION
            CLayer* pLayer=pDoc->GetActiveLayer();
#else
            CLayer* pLayer=pDoc->AddLayer(&theDlg.GetFileName());
#endif
            if(pLayer)
            {
               BITMAP bmp;
               ::GetObject(hDib,sizeof(BITMAP),&bmp);
               CRect rc(0,0,bmp.bmWidth,bmp.bmHeight);
               LongRect lrc(0,0,m_Mapper.DPtoLP(bmp.bmWidth),m_Mapper.DPtoLP(bmp.bmHeight));
               CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
               pShape->CalculateExtents();
               pShape->SetShapeDatum(kDefaultDatum);
               pDoc->CreateShape(pShape);
               SelectShape(pShape,false);
               pDoc->LayersChanged();
               pDoc->CheckDocSize(true);
            }
            else DeleteObject(hDib);
         }
      }
      else if (theDlg.GetFileExt().CompareNoCase("tif")==0 || theDlg.GetFileExt().CompareNoCase("tiff")==0)
      {
         HBITMAP hBitmap;
         int width,height;
         if (readTiff((LPCSTR)theDlg.GetPathName(),hBitmap,width, height))
         {
#ifdef _PERSONAL_EDITION
            CLayer* pLayer=pDoc->GetActiveLayer();
#else
            CLayer* pLayer=pDoc->AddLayer(&theDlg.GetFileName());
#endif
            if(pLayer)
            {
               CRect rc(0,0,width,height);
               LongRect lrc(0,0,width,height);
               CMDBitmap* pShape=new CMDBitmap(rc,lrc,hBitmap);
               pShape->CalculateExtents();
               pShape->SetShapeDatum(kDefaultDatum);
               pDoc->CreateShape(pShape);
               SelectShape(pShape,false);
               pDoc->LayersChanged();
               pDoc->CheckDocSize(true);
            }
            else DeleteObject(hBitmap);
         }
         else AfxMessageBox(IDS_INCOMPATIBLE_TIFF);
      }
#ifndef _PERSONAL_EDITION
      else
      {
         CPCDraftDoc* pNewDoc=new CPCDraftDoc;
         BOOL bResult;
         if(theDlg.GetFileExt().CompareNoCase("drf")==0 || theDlg.GetFileExt().CompareNoCase("drs")==0)
            bResult=pNewDoc->OnOpenDocument(theDlg.GetPathName());
         else	// assume MacDraft
            bResult=pNewDoc->OnNewDocument() && pNewDoc->LoadMacDraft(theDlg.GetPathName());
         if(bResult)
         {
            pNewDoc->SetPathName(theDlg.GetPathName(),FALSE);
            CLoadLayerDlg theLayerDlg;
            theLayerDlg.m_pDoc=pNewDoc;
            //if(theLayerDlg.DoModal()==IDOK && pNewDoc->SetupUnits(pDoc->m_nUnit,pDoc->m_bHideMetricUnits,pDoc->GetAngleUnit(),pDoc->m_nPlaces,pDoc->m_nAnglePlaces,-1,NULL,IDS_CANT_LOAD_LAYER))
            if(theLayerDlg.DoModal() == IDOK && pNewDoc->SetupUnits(pDoc->m_nUnit,pDoc->m_bHideMetricUnits,pDoc->GetAngleUnit(),pDoc->m_nPlaces,pDoc->m_nAnglePlaces,pDoc->m_nDefScale,NULL,IDS_CANT_LOAD_LAYER, &pDoc->m_ratioCustom)) // Infinisys Ltd.
            {
               CLayer* pNewLayer=pNewDoc->GetLayer(theLayerDlg.m_nLayerIndex);
               CLayer* pLayer=pDoc->AddLayer(&pNewLayer->m_strName);
               if(pLayer)
               {
                  CMemFile mf(0x40000);
                  {
                     CArchive ar(&mf,CArchive::store);
                     pNewLayer->Serialize(ar);
                  }
                  mf.SeekToBegin();
                  {
                     CArchive ar(&mf,CArchive::load);
                     pLayer->Serialize(ar);
                  }
                  pLayer->m_bHidden=false;
                  pLayer->m_bGrayed=true;
                  pLayer->UpdateDependentShapes(this);
                  pDoc->LayersChanged();
                  pDoc->CheckDocSize(true);
               }
            }
         }
         pNewDoc->OnCloseDocument();
      }
#endif
   }
}

void CPCDraftView::OnLayoutDrawingSize()
{
   CDrawingSizeDlg theDlg;
   CPCDraftDoc* pDoc=GetDocument();
   theDlg.m_wndDrawingSize.m_PageSize=pDoc->m_PageSize;
   LongRect lrcExtent=pDoc->GetCumulativeExtents();
   if(lrcExtent.IsEmpty()) theDlg.m_wndDrawingSize.m_MinNumPage= CPoint(1,1);
   else
   {
      theDlg.m_wndDrawingSize.m_MinNumPage.x=(lrcExtent.right.GetLong()/pDoc->m_PageSize.cx)+1;
      theDlg.m_wndDrawingSize.m_MinNumPage.y=(lrcExtent.bottom.GetLong()/pDoc->m_PageSize.cy)+1;
   }
   theDlg.m_wndDrawingSize.m_NumPage=pDoc->m_NumPage;
   theDlg.m_nPageNum=pDoc->m_nPageNumbering;
   theDlg.m_bLastPageFirst=pDoc->m_bPrintLastPageFirst;
   theDlg.m_bRegistrationMarks=pDoc->m_bRegistrationMarks;
   if(theDlg.DoModal()==IDOK)
   {
      pDoc->m_nPageNumbering=theDlg.m_nPageNum;
      pDoc->m_bPrintLastPageFirst=theDlg.m_bLastPageFirst;
      pDoc->m_bRegistrationMarks=theDlg.m_bRegistrationMarks;
      pDoc->m_NumPage=theDlg.m_wndDrawingSize.m_NumPage;
      pDoc->UpdateMaxZoom();
      pDoc->UpdateAllViews(NULL,kDocumentSizeChanged);
      pDoc->SetModifiedFlag();
   }
}

void CPCDraftView::OnUpdateLayoutPosition(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!GetDocument()->GetCumulativeExtents().IsEmpty());
}

void CPCDraftView::OnLayoutPosition()
{
   CPositionDlg theDlg(GetDocument());
   if(theDlg.DoModal()==IDOK)
   {
      SaveUndo(IDS_POSITION);
      if(!theDlg.m_Delta.IsZero()) GetDocument()->Offset(theDlg.m_Delta);
      GetDocument()->UpdateAllViews(NULL,kRefresh);
      GetDocument()->SetModifiedFlag();
   }
}

void CPCDraftView::OnLayoutSetScaleUnits()
{
   CScaleUnitsDlg theDlg(GetDocument());
   BeginSaveUndo();
   if(theDlg.DoModal()==IDOK)
   {
      CPCDraftDoc* pDoc=GetDocument();
      pDoc->UpdateFrameCounts();
      pDoc->UpdateAllViews(NULL,kDocumentSizeChanged);
      pDoc->SetModifiedFlag();
      SetSnap(pDoc->GetActiveLayer()->m_nSnap);
      for(int i=0; i<pDoc->GetLayersCount(); i++)
      {
         pDoc->GetLayer(i)->UpdateDependentShapes(this);
      }
      ForceResizeUpdate();
      AbortSaveUndo(false);
      SaveUndo(0);
   }
   else AbortSaveUndo();
}

void CPCDraftView::OnTool1_1()
{
//	OnViewHomeView();
#ifdef _FLOAT_ZOOM
   SmartZoom(0,-GetDocument()->m_nZoomFactor);
#else
   SmartZoom(-GetDocument()->m_nZoomFactor);
#endif
}

void CPCDraftView::OnUpdateToolGlue(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_pAttachCtx->CanAttach(&m_Selection)?TRUE:FALSE);
}

void CPCDraftView::OnToolGlue()
{
   CancelCurrentTool();
//	if(m_pAttachCtx->CanAttach(&m_Selection))
   {
      SaveUndo(IDS_ATTACH);
      UnselectShapes(usLocked);
      CMDShape* pShape=m_pAttachCtx->Attach();

      while(m_Selection.GetCount())
      {
         CMDShape* pShape=m_Selection.RemoveHead();
         InvalidateShape(pShape,true);
         GetDocument()->DeleteShape(pShape);
      }

      pShape->SetFillModel(GetDocument()->m_Fill);
      pShape->SetPenModel(GetDocument()->m_Pen);
      GetDocument()->CreateShape(pShape);
      SelectShape(pShape,false);
   }
}

void CPCDraftView::OnUpdateToolUnglue(CCmdUI* pCmdUI)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && pShape->CanUnglue() && !pShape->IsAreaShown())
      {
         pCmdUI->Enable();
         return;
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnToolUnglue()
{
   CancelCurrentTool();
   SaveUndo(IDS_BREAK);
   UnselectShapes(usLocked);
   CMDShapeList newSelection;
   CPCDraftDoc* pDoc=GetDocument();
   while(m_Selection.GetCount())
   {
      CMDShape* pShape=m_Selection.GetHead();
      UnselectShape(pShape);
      if(pShape->CanUnglue() && !pShape->IsAreaShown())
      {
         CMDShapeList shapeList;
         pShape->Unglue(&shapeList);
         pDoc->CreateShapes(&shapeList,pShape);
         newSelection.AddTail(&shapeList);
      }
   }
   while(newSelection.GetCount())
   {
      CMDShape* pShape=newSelection.RemoveHead();
      pShape->UpdateDependentShapes(this);
      pShape->CalculateExtents();
      SelectShape(pShape,true);
   }
}

void CPCDraftView::OnUpdateToolCombine(CCmdUI* pCmdUI)
{
   BOOL bEnable=FALSE;
   CMDShapeList workList;
//	workList.AddTail(&m_Selection);
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr))) workList.AddTail(pShape);
   }
   POSITION posAdder=GetDocument()->GetActiveLayer()->TopmostShape(&workList);
   if(posAdder)
   {
      CMDShape* pAdder=workList.GetAt(posAdder);
      workList.RemoveAt(posAdder);
      if(m_pAddCtx->CanAdd(pAdder,&workList)) bEnable=TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CPCDraftView::OnToolCombine()
{
   CancelCurrentTool();
//	if(m_AddCtx->CanAttach(&m_Selection))
   {
      BeginSaveUndo();
      UnselectShapes(usLocked);
      OnViewHideArea(); // hide areas
      CMDShapeList workList,newList;
      workList.AddTail(&m_Selection);

      POSITION posAdder=GetDocument()->GetActiveLayer()->TopmostShape(&workList);
      CMDShape* pAdder=workList.GetAt(posAdder);
      workList.RemoveAt(posAdder);
      m_pAddCtx->Add(pAdder->CreateCopy(),&workList,&newList);
      if(newList.GetCount())
      {
         CommitSaveUndo(IDS_COMBINE);
         SelectNone();
         while(workList.GetCount())
         {
            CMDShape* pShape=workList.RemoveHead();
            if(pShape->IsAreaShown()) pShape->ShowArea(false,this);
            InvalidateShape(pShape,true);
            GetDocument()->DeleteShape(pShape);
         }

         if(pAdder->IsAreaShown()) pAdder->ShowArea(false,this);
         GetDocument()->CreateShapes(&newList,pAdder);
         while(newList.GetCount())
         {
            CMDShape* pShape=newList.RemoveHead();
            pShape->CalculateExtents();
            SelectShape(pShape,true);
         }
      }
      else AbortSaveUndo(true);
   }
}

void CPCDraftView::OnUpdateToolSubtract(CCmdUI* pCmdUI)
{
   BOOL bEnable=FALSE;
   CMDShapeList workList;
   GetSortedSelection(workList);
   if(workList.GetCount())
   {
      CMDShape* pSuber=workList.RemoveHead();
      if(m_pSubCtx->CanSub(pSuber,&workList)) bEnable=TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CPCDraftView::OnToolSubtract()
{
   CancelCurrentTool();
//	if(m_pSubCtx->CanSub(&m_Selection))
   {
      BeginSaveUndo();
      UnselectShapes(usLocked);
      CMDShapeList newList;
      GetSortedSelection(m_Selection,false);
      CMDShape* pSuber=m_Selection.RemoveHead();
      UnselectShape(pSuber);
      m_pSubCtx->Sub(pSuber,&m_Selection,&newList);
      if(newList.GetCount())
      {
         CommitSaveUndo(IDS_SUBTRACT);
         GetDocument()->CreateShapes(&newList,m_Selection.RemoveHead());

         while(m_Selection.GetCount())
         {
            CMDShape* pShape=m_Selection.RemoveHead();
            InvalidateShape(pShape,true);
            GetDocument()->DeleteShape(pShape);
         }

         while(newList.GetCount())
         {
            CMDShape* pShape=newList.RemoveHead();
            pShape->CalculateExtents();
            SelectShape(pShape,true);
         }
         SelectShape(pSuber,true);
      }
      else AbortSaveUndo();
   }
}

void CPCDraftView::OnViewRestorePrevZoom()
{
   CPCDraftDoc* pDoc=GetDocument();
   CPoint ptScroll=pDoc->m_ptPrevZoomScroll;
#ifdef _FLOAT_ZOOM
   pDoc->DoZoom(pDoc->m_fPrevZoomFactor/(pDoc->m_fZoomFactor?pDoc->m_fZoomFactor:FloatTheFixedZoomFactor(pDoc->m_nZoomFactor)),-pDoc->m_nZoomFactor+pDoc->m_nPrevZoomFactor,GetDeviceScrollPosition());
#else
   pDoc->DoZoom(-pDoc->m_nZoomFactor+pDoc->m_nPrevZoomFactor,GetDeviceScrollPosition());
#endif
   ScrollToDevicePosition(ptScroll);
}

void CPCDraftView::OnViewSaveView()
{
   CPCDraftDoc* pDoc=GetDocument();
   CSaveViewDlg theDlg(pDoc);
   if(theDlg.DoModal()==IDOK)
   {
      CSavedView* pView=new CSavedView;
      pView->m_sTitle=theDlg.m_sTitle;
      pView->m_ptOrg=GetDeviceScrollPosition();
#ifdef _FLOAT_ZOOM
      pView->m_nZoom=PackZoomFactor(pDoc->m_nZoomFactor,pDoc->m_fZoomFactor);
#else
      pView->m_nZoom=GetDocument()->m_nZoomFactor;
#endif
      pView->m_nShortcut=theDlg.m_bUseKey?theDlg.m_nShortcut:0;
      if(pDoc->m_SavedViews.GetSize() && theDlg.m_bUseKey)
      {
         int i = 0;
         for(; i<pDoc->m_SavedViews.GetSize(); i++)
         {
            int nShortcut=((CSavedView*)pDoc->m_SavedViews.GetAt(i))->m_nShortcut;
            if(!nShortcut || nShortcut>pView->m_nShortcut) break;
         }
         pDoc->m_SavedViews.InsertAt(i,pView);
      }
      else pDoc->m_SavedViews.Add(pView);
      pDoc->SetModifiedFlag();
   }
}

void CPCDraftView::OnFileSetDefaults()
{
   CPCDraftApp *pApp=(CPCDraftApp*)AfxGetApp();
   pApp->DocumentSetDefaults(GetDocument());
}

void CPCDraftView::DoApplyAttributes(CMDShape *pShape,ShapeAttributes& attr)
{
   if(attr.m_nFill>0)
   {
      pShape->SetFillModel(attr.m_Fill);
   }
   if(attr.m_nPen>0)
   {
      InvalidateShape(pShape,true);
      pShape->SetPenModel(attr.m_Pen);
   }
   if(attr.m_bInfo)
   {
      pShape->SetValues(attr.m_sInfo);
   }
   if(attr.m_nPara>0 && pShape->IsParallel())
   {
      ParallelAttrs pa=pShape->GetParallelAttrs();
      pa.m_Fill=attr.m_Para;
      pShape->SetParallelAttrs(pa);
   }
   InvalidateShape(pShape,true);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnEditCopyAttrib()
{
   ((CPCDraftApp*)AfxGetApp())->DoCopyAttributes(m_Selection.GetHead());
}

void CPCDraftView::OnEditApplyAttrib()
{
   ShapeAttributes attrib=((CPCDraftApp*)AfxGetApp())->m_ShapeAttributes;

   if(GetAsyncKeyState(VK_SHIFT)<0)
   {
      CAttributeOptionsDlg theDlg(GetDocument(),&attrib,true);
      if(theDlg.DoModal()!=IDOK)
         return;
   }
   UnselectShapes(usLocked);
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      DoApplyAttributes(pShape,attrib);
   }
}

void CPCDraftView::OnEditAttribOptions()
{
   CAttributeOptionsDlg theDlg(GetDocument(),&((CPCDraftApp*)AfxGetApp())->m_ShapeAttributes);
   if(theDlg.DoModal()==IDOK)
   {
   }
}

void CPCDraftView::OnFilePrintSlides()
{
#ifndef	_DEMO
   if(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode)
   {
      CPrintSlidesDlg theDlg(GetDocument());
      if(theDlg.DoModal()==IDOK)
      {
         m_bFitToPage=(theDlg.m_nFitFull==0);
         m_bLayer1AsMaster=theDlg.m_bLayer1AsMaster?true:false;
         m_nPrintOption=kPrintSlides;
         CView::OnFilePrint();
         m_nPrintOption=kPrintDocument;
      }
   }
#endif
}

void CPCDraftView::OnViewSlideShow()
{
   CSlideShowDlg theDlg(GetDocument());
   if(theDlg.DoModal()==IDOK)
   {
      CRuntimeClass* pFrameClass=RUNTIME_CLASS(CSlideShowFrame);
      CSlideShowFrame* pFrame=(CSlideShowFrame*)pFrameClass->CreateObject();
      CCreateContext cc;
      memset(&cc,0,sizeof(cc));
      cc.m_pNewViewClass=RUNTIME_CLASS(CSlideShowView);
      cc.m_pCurrentDoc=GetDocument();
      pFrame->Create(NULL,_T("Slide Show"),WS_CAPTION | WS_POPUP | WS_VISIBLE, CFrameWnd::rectDefault, AfxGetMainWnd(), NULL, 0, &cc);
      pFrame->InitialUpdateFrame(GetDocument(),TRUE);
      pFrame->ShowWindow(SW_SHOWMAXIMIZED);
//		GetDocument()->DoSlideShow();
   };
}

#ifdef _FLOAT_ZOOM
void CPCDraftView::SmartZoom(double fZoomFactor,int nZoomFactor)
#else
void CPCDraftView::SmartZoom(int nZoomFactor)
#endif
{
   CPoint pt,ptScroll;
   CSize szClientSize;
   GetClientSize(szClientSize);
   if(szClientSize.cx>m_totalDev.cx) szClientSize.cx=m_totalDev.cx;
   if(szClientSize.cy>m_totalDev.cy) szClientSize.cy=m_totalDev.cy;
   pt=ptScroll=GetDeviceScrollPosition();
   pt.x+=szClientSize.cx/2;
   pt.y+=szClientSize.cy/2;
   if(pt.x>m_totalDev.cx) pt.x=m_totalDev.cx;
   if(pt.y>m_totalDev.cy) pt.y=m_totalDev.cy;
   LongPoint lpt=m_Mapper.DPtoLP(pt);
#ifdef _FLOAT_ZOOM
   GetDocument()->DoZoom(fZoomFactor,nZoomFactor,ptScroll);
#else
   GetDocument()->DoZoom(nZoomFactor,ptScroll);
#endif
   CPoint ptOrg=m_Mapper.LPtoDP(lpt);
   ptOrg.x-=szClientSize.cx/2;
   ptOrg.y-=szClientSize.cy/2;
   if(ptOrg.x<0 || szClientSize.cx>=m_totalDev.cx) ptOrg.x=0;
   if(ptOrg.y<0 || szClientSize.cy>=m_totalDev.cy) ptOrg.y=0;
   ScrollToDevicePosition(ptOrg);
//	UpdateBars();
}

// PC Draft 7 - Zoom Center
void CPCDraftView::ZoomCenter(double fZoomFactor, int nZoomFactor)
{

   if (fZoomFactor != 0.0 && nZoomFactor == 0)
   {

      if (fZoomFactor == 64.0)
      {
         nZoomFactor = 6;
      }
      else if (fZoomFactor == 32.0)
      {
         nZoomFactor = 5;
      }
      else if (fZoomFactor == 16.0)
      {
         nZoomFactor = 4;
      }
      else if (fZoomFactor == 8.0)
      {
         nZoomFactor = 3;
      }
      else if (fZoomFactor == 4.0)
      {
         nZoomFactor = 2;
      }
      else if (fZoomFactor == 2.0)
      {
         nZoomFactor = 1;
      }
      else if (fZoomFactor == 1.0)
      {
         nZoomFactor = 0;
      }
      else if (fZoomFactor == 0.5)
      {
         nZoomFactor = -1;
      }
      else if (fZoomFactor == 0.25)
      {
         nZoomFactor = -2;
      }

   }

   CPoint ptScroll;

   ptScroll = GetDeviceScrollPosition();

   GetDocument()->CenterZoom(0.0f, nZoomFactor, ptScroll);

   CPoint pt;

   RECT rect = {};
   GetSelectionExtents(&rect);

   CSize szClientSize = {};
   GetClientSize(szClientSize);
   if (szClientSize.cx > m_totalDev.cx) szClientSize.cx = m_totalDev.cx;
   if (szClientSize.cy > m_totalDev.cy) szClientSize.cy = m_totalDev.cy;

   if (rect.right <= rect.left || rect.bottom <= rect.top)
   {

      // Zoom Center of Document

      pt.x = m_totalDev.cx / 2;
      pt.y = m_totalDev.cy / 2;

   }
   else
   {

      // Zoom Center of Selected Objects

      pt.x = (rect.right + rect.left) / 2;
      pt.y = (rect.bottom + rect.top) / 2;

   }

   pt.x -= szClientSize.cx / 2;
   pt.y -= szClientSize.cy / 2;

   ScrollToDevicePosition(pt);

}

#ifdef _FLOAT_ZOOM
void CPCDraftView::SmartZoomIn(double fZoomFactor,int nZoomFactor)
#else
void CPCDraftView::SmartZoomIn(int nZoomFactor)
#endif
{
   CPoint p;
   ::GetCursorPos(&p);
   CPoint pt(p);
   ScreenToClient(&pt);
   CPoint ptScroll=GetDeviceScrollPosition();
   pt+=ptScroll;
   CRect rcClient;
   GetActiveClientRect(rcClient);

   CZoomInSketcher theSketcher(nZoomFactor);
   if(theSketcher.DoSketch(this,pt,&rcClient,CSketcher::NoSnap|CSketcher::NoScroll))
   {
      CPoint ptClientCorner;
      GetClientCorner(ptClientCorner);
      pt.x=theSketcher.m_Rect.left-ptClientCorner.x;
      pt.y=theSketcher.m_Rect.top-ptClientCorner.y;
      if(pt.x<ptClientCorner.x) pt.x=ptClientCorner.x;
      if(pt.y<ptClientCorner.y) pt.y=ptClientCorner.y;
      LongPoint lpt=m_Mapper.DPtoLP(pt);
#ifdef _FLOAT_ZOOM
      GetDocument()->DoZoom(fZoomFactor,nZoomFactor,ptScroll);
#else
      GetDocument()->DoZoom(nZoomFactor,ptScroll);
#endif
      CPoint ptOrg=m_Mapper.LPtoDP(lpt);
      ScrollToDevicePosition(ptOrg);
   }
}

void CPCDraftView::OnViewHomeView()
{
#ifdef _FLOAT_ZOOM
   SmartZoom(0,-GetDocument()->m_nZoomFactor);
#else
   SmartZoom(-GetDocument()->m_nZoomFactor);
#endif
   ScrollToDevicePosition(CPoint(0,0));
}


#if USE_CENTER_ZOOM

void CPCDraftView::OnUpdateViewZoomIn2x(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor != 1);
}

void CPCDraftView::OnViewZoomIn2x()
{

   ZoomCenter(2.0);

}


void CPCDraftView::OnUpdateViewZoomIn4x(CCmdUI* pCmdUI)
{
   
   CPCDraftDoc* pDoc=GetDocument();

   pCmdUI->Enable(pDoc->m_nZoomFactor != 2);

}

void CPCDraftView::OnViewZoomIn4x()
{

   // PC Draft 7 - Zoom Center
   ZoomCenter(4.0);

}

void CPCDraftView::OnUpdateViewZoomIn8x(CCmdUI* pCmdUI)
{

   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor != 3);

}


void CPCDraftView::OnViewZoomIn8x()
{

   // PC Draft 7 - Zoom Center
   ZoomCenter(8.0);

}



void CPCDraftView::OnUpdateViewZoomIn16x(CCmdUI* pCmdUI)
{

   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor != 4);

}


void CPCDraftView::OnViewZoomIn16x()
{
   
   // PC Draft 7 - Zoom Center
   ZoomCenter(16.0);

}


void CPCDraftView::OnUpdateViewZoomIn32x(CCmdUI* pCmdUI)
{

   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor != 5);

}


void CPCDraftView::OnViewZoomIn32x()
{

   // PC Draft 7 - Zoom Center
   ZoomCenter(32.0);

}


void CPCDraftView::OnUpdateViewZoomIn64x(CCmdUI* pCmdUI)
{

   CPCDraftDoc* pDoc = GetDocument();

   pCmdUI->Enable(pDoc->m_nZoomFactor != 6);

}


void CPCDraftView::OnViewZoomIn64x()
{

   // PC Draft 7 - Zoom Center
   ZoomCenter(64.0);

}


void CPCDraftView::OnUpdateViewZoomOut2x(CCmdUI* pCmdUI)
{

   pCmdUI->Enable(GetDocument()->m_nZoomFactor != -1);

}

void CPCDraftView::OnViewZoomOut2x()
{
   
   // PC Draft 7 - Zoom Center
   ZoomCenter(0.5);

}


void CPCDraftView::OnUpdateViewZoomOut4x(CCmdUI* pCmdUI)
{

   pCmdUI->Enable(GetDocument()->m_nZoomFactor != -2);

}


void CPCDraftView::OnViewZoomOut4x()
{

   // PC Draft 7 - Zoom Center
   ZoomCenter(0.25);

}


#else

void CPCDraftView::OnUpdateViewZoomIn4x(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor + 2 <= pDoc->m_nMaxZoom);
}

void CPCDraftView::OnViewZoomIn4x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoomIn(4,2);
   #else
      SmartZoomIn(2);
   #endif
}

void CPCDraftView::OnUpdateViewZoomIn8x(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor + 3 <= pDoc->m_nMaxZoom);
}

void CPCDraftView::OnViewZoomIn8x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoomIn(8, 3);
   #else
      SmartZoomIn(3);
   #endif
}

void CPCDraftView::OnUpdateViewZoomIn16x(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor + 4 <= pDoc->m_nMaxZoom);
}


void CPCDraftView::OnViewZoomIn16x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoomIn(16, 4);
   #else
      SmartZoomIn(4);
   #endif
}

void CPCDraftView::OnUpdateViewZoomIn32x(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor + 5 <= pDoc->m_nMaxZoom);
}

void CPCDraftView::OnViewZoomIn32x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoomIn(32, 5);
   #else
      SmartZoomIn(5);
   #endif
}

void CPCDraftView::OnUpdateViewZoomIn64x(CCmdUI* pCmdUI)
{
   CPCDraftDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->m_nZoomFactor + 6 <= pDoc->m_nMaxZoom);
}

void CPCDraftView::OnViewZoomIn64x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoomIn(64, 6);
   #else
      SmartZoomIn(6);
   #endif
}

void CPCDraftView::OnUpdateViewZoomOut2x(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->m_nZoomFactor - 1 >= -3);
}

void CPCDraftView::OnViewZoomOut2x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoom(0.5,-1);
   #else
      SmartZoom(-1);
   #endif
}

void CPCDraftView::OnUpdateViewZoomOut4x(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->m_nZoomFactor - 2 >= -3);
}

void CPCDraftView::OnViewZoomOut4x()
{
   #ifdef _FLOAT_ZOOM
      SmartZoom(0.25,-2);
   #else
      SmartZoom(-2);
   #endif
}

#endif

void CPCDraftView::OnUpdateLayoutShowRulers(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bShowRulers);
}

void CPCDraftView::OnLayoutShowRulers()
{
   GetDocument()->m_bShowRulers=!GetDocument()->m_bShowRulers;
   GetDocument()->UpdateAllViews(NULL,kUpdateBars);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateLayoutStandardRulers(CCmdUI* pCmdUI)
{
   pCmdUI->SetRadio(GetDocument()->m_bStandardRulers);
}

void CPCDraftView::OnLayoutStandardRulers()
{
   GetDocument()->m_bStandardRulers=true;
   GetDocument()->UpdateAllViews(NULL,kUpdateBars);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateLayoutScaleRulers(CCmdUI* pCmdUI)
{
   pCmdUI->SetRadio(!GetDocument()->m_bStandardRulers);
}

void CPCDraftView::OnLayoutScaleRulers()
{
   GetDocument()->m_bStandardRulers=false;
   GetDocument()->UpdateAllViews(NULL,kUpdateBars);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateLayoutHideGridLines(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bHideGridLines);
}

void CPCDraftView::OnLayoutHideGridLines()
{
   GetDocument()->m_bHideGridLines=!GetDocument()->m_bHideGridLines;
   GetDocument()->UpdateAllViews(NULL,kRefresh);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateLayoutHidePageBreaks(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bHidePageBreaks);
}

void CPCDraftView::OnLayoutHidePageBreaks()
{
   GetDocument()->m_bHidePageBreaks=!GetDocument()->m_bHidePageBreaks;
   GetDocument()->UpdateAllViews(NULL,kRefresh);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateLayoutSnapToObject(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bSnapToObject);
}

void CPCDraftView::OnLayoutSnapToObject()
{
   GetDocument()->m_bSnapToObject=!GetDocument()->m_bSnapToObject;
   GetDocument()->UpdateAllViews(NULL,kRefresh);
   GetDocument()->SetModifiedFlag();
}


void CPCDraftView::OnUpdateLayoutSnapToGuides(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bSnapToGuides);
}

void CPCDraftView::OnLayoutSnapToGuides()
{
   GetDocument()->m_bSnapToGuides = !GetDocument()->m_bSnapToGuides;
   GetDocument()->UpdateAllViews(NULL, kRefresh);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateLayoutShowGuideLines(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bShowGuideLines);
}

void CPCDraftView::OnLayoutShowGuideLines()
{
   GetDocument()->m_bShowGuideLines = !GetDocument()->m_bShowGuideLines;
   GetDocument()->UpdateAllViews(NULL, kRefresh);
   GetDocument()->SetModifiedFlag();
}
void CPCDraftView::OnUpdatePrefHideFills(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_bHideFills);
}

void CPCDraftView::OnPrefHideFills()
{
   GetDocument()->m_bHideFills=!GetDocument()->m_bHideFills;
   GetDocument()->UpdateAllViews(NULL,kRefresh);
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
   CPCDraftDoc* pDoc=GetDocument();
   CPalette* pPalette=pDC->SelectPalette(&pDoc->m_Palette,FALSE);
   pDC->RealizePalette();

   double factor;
   CSize ext=pDoc->GetPageDevSize();
   if(m_nPrintOption==kPrintSlides && m_bFitToPage)
   {
      double xFactor,yFactor;
      xFactor=(double)pDC->GetDeviceCaps(HORZRES)/(double)(pDoc->m_NumPage.x*pDoc->m_PageSize.cx);
      yFactor=(double)pDC->GetDeviceCaps(VERTRES)/(double)(pDoc->m_NumPage.y*pDoc->m_PageSize.cy);
      factor=xFactor<yFactor?xFactor:yFactor;
   }
   else
   {
      factor=(double)pDC->GetDeviceCaps(LOGPIXELSX)/(pDoc->m_bEnglishUnits?72.0:71.12);
//		factor=(double)pDC->GetDeviceCaps(HORZRES)/(double)pDoc->m_PageSize.cx;
   }

   LongPoint lptOrg;
   int nSlideNum=(pInfo->m_nCurPage-1);
   if(m_nPrintOption!=kPrintSlides || !m_bFitToPage)
   {
      int nPages=pDoc->m_NumPage.x*pDoc->m_NumPage.y;
      int nPage=nSlideNum%nPages;
      if(pDoc->m_bPrintLastPageFirst) nPage=nPages-nPage-1;
      if(pDoc->m_nPageNumbering==kPageNumbering1234)
      {
         lptOrg.x=pDoc->m_PageSize.cx*(nPage%pDoc->m_NumPage.x);
         lptOrg.y=pDoc->m_PageSize.cy*(nPage/pDoc->m_NumPage.x);
      }
      else
      {
         lptOrg.x=pDoc->m_PageSize.cx*(nPage/pDoc->m_NumPage.y);
         lptOrg.y=pDoc->m_PageSize.cy*(nPage%pDoc->m_NumPage.y);
      }
      nSlideNum/=nPages;
   }

   if(pDoc->m_bRegistrationMarks)
   {
      int tickSize=pDC->GetDeviceCaps(LOGPIXELSX)/25;

      // SMK.SSA - don't need this
      // CPCDraftDoc* pDoc=GetDocument();
      int sizeX = pDC->GetDeviceCaps(HORZRES)-1;
      int sizeY = pDC->GetDeviceCaps(VERTRES)-1;

      pDC->MoveTo(CPoint(0,tickSize));
      pDC->LineTo(CPoint(0, 0) );
      pDC->LineTo(CPoint(tickSize+1,0));

      pDC->MoveTo(CPoint(sizeX-tickSize, 0));
      pDC->LineTo(CPoint(sizeX, 0));
      pDC->LineTo(CPoint(sizeX, tickSize+1));

      pDC->MoveTo(CPoint(0, sizeY-tickSize));
      pDC->LineTo(CPoint(0, sizeY));
      pDC->LineTo(CPoint(tickSize+1, sizeY));

      pDC->MoveTo(CPoint(sizeX-tickSize, sizeY));
      pDC->LineTo(CPoint(sizeX, sizeY));
      pDC->LineTo(CPoint(sizeX, sizeY-(tickSize+1)));
   }

   CFactorMapper mapper(factor,lptOrg);
   mapper.SetDocument(pDoc);
   CDraftContext dc(pDC,&mapper,pDoc);
   dc.m_bPrinting = true;
   switch(m_nPrintOption)
   {
   case kPrintDocument:
   {
      for(int i=0; i<pDoc->m_Layers.GetSize(); i++)	pDoc->GetLayer(i)->Draw(&dc,true);
   }
   break;

   case kPrintLayer:
      pDoc->GetLayer(pDoc->m_nActiveLayer)->Draw(&dc,true);
      break;

   case kPrintSlides:
   {
      if(m_bLayer1AsMaster)
      {
         pDoc->GetLayer(0)->Draw(&dc,true);
         nSlideNum++;
      }
      pDoc->GetLayer(nSlideNum)->Draw(&dc,true);
   }
   break;
   }
   pDC->SelectPalette(pPalette,FALSE);
}

void CPCDraftView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
   CView::OnPrepareDC(pDC, pInfo);
   if(pDC->IsPrinting()) pDC->SetMapMode(MM_TEXT);
}

void CPCDraftView::OnFilePrintActiveLayer()
{
#ifndef	_DEMO
   if(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode)
   {
      m_nPrintOption=kPrintLayer;
      CView::OnFilePrint();
      m_nPrintOption=kPrintDocument;
   }
#endif
}

void CPCDraftView::SnapToGrid(LongPoint &lpt) const
{
   unsigned long vFN, vFD;
   long adjOrdinate, halfSnap;
   long newOrdinate, divMod, residue;
   bool bSign;

   if(!(vFN=m_Snap.fSnapRatio.fN)) return;
   if(!(vFD=m_Snap.fSnapRatio.fD)) return;

   halfSnap=0x00008000 * vFN;
   halfSnap/=vFD;

   /* SNAP THE X-ORDINATE FIRST */
   adjOrdinate=lpt.x.GetFixed() ;
   if(adjOrdinate<0)
   {
      bSign=true;
      adjOrdinate=-adjOrdinate;
   }
   else bSign=false;

   residue=m_Snap.fSnapResidue.x.GetFixed();

   /*	if(adjOrdinate<residue)
   		adjOrdinate = 0L;
   	else
   */
   adjOrdinate-=residue;

   adjOrdinate+=halfSnap;

   /* test for overflow conditions */

   if( adjOrdinate >= 0x7fffffffL / (long)vFD )
   {
      divMod = adjOrdinate / vFN;
      divMod *= vFD;

      newOrdinate = (divMod & 0xffff0000) / vFD;
      newOrdinate *= vFN;
   }
   else
   {
      divMod=adjOrdinate*vFD;	/* divide by snap */
      divMod/=vFN;

      newOrdinate=(divMod & 0xffff0000)*vFN;
      newOrdinate/=vFD;
   }

   /* newOrdinate = dividend * snap is multiple of snap */
   /* add the residue back */

   newOrdinate+=residue;

   if(bSign) newOrdinate=-newOrdinate;
   lpt.x.SetFromFixed ( newOrdinate ) ;

   /* NOW, SNAP THE Y-ORDINATE */

   adjOrdinate = lpt.y.GetFixed () ;
   if(adjOrdinate<0)
   {
      bSign=true;
      adjOrdinate=-adjOrdinate;
   }
   else bSign=false;

   residue = m_Snap.fSnapResidue.y.GetFixed () ;

   /*	if ( adjOrdinate < residue )
   		adjOrdinate = 0L;
   	else*/
   adjOrdinate -= residue;

   adjOrdinate += halfSnap;

   /* test for overflow conditions */

   if ( adjOrdinate >= 0x7fffffffL / (long)vFD )
   {
      divMod = adjOrdinate / vFN;
      divMod *= vFD;

      newOrdinate = (divMod & 0xffff0000) / vFD;
      newOrdinate *= vFN;

   }
   else
   {
      divMod = adjOrdinate  * vFD;	/* divide by snap */
      divMod /= vFN;

      newOrdinate = (divMod & 0xffff0000) * vFN;
      newOrdinate /= vFD;

   }

   /* newOrdinate = dividend * snap is multiple of snap */

   /* add the residue back */

   newOrdinate += residue;

   if(bSign) newOrdinate=-newOrdinate;
   lpt.y.SetFromFixed ( newOrdinate ) ;
}

bool CPCDraftView::SnapToGuides(LongPoint &lpt,int * piGuide) const
{

   ASSERT(GetDocument()->m_bSnapToGuides);

   LongPoint lptSnap;
   Coord topLeft=m_Mapper.DPtoLP((long)10), bottomRight=m_Mapper.DPtoLP((long)10);
   LongRect lrcProx(lpt.x-topLeft,lpt.y-topLeft,lpt.x+bottomRight,lpt.y+bottomRight);

   bool bVert = false;
   bool bHorz = false;

   for (int i = 0; i < GetDocument()->m_guidea.GetCount(); i++)
   {

      auto & guide = GetDocument()->m_guidea[i];

      if (!bVert && guide.m_bVertical)
      {

         if (guide.m_coord >= lrcProx.left && guide.m_coord <= lrcProx.right)
         {

            lpt.x = guide.m_coord;

            bVert = true;

         }

      }
      else if(!bHorz)
      {

         if (guide.m_coord >= lrcProx.top && guide.m_coord <= lrcProx.bottom)
         {

            lpt.y = guide.m_coord;

            bHorz = true;

         }

      }

      if (piGuide)
      {

         if (bVert || bHorz)
         {

            *piGuide = i;

            return true;

         }

      }

      if (bVert && bHorz)
      {

         return true;

      }

   }

   return bVert || bHorz;

}


bool CPCDraftView::SnapSelectionToGuide(LongPoint &lpt, LongPoint &lptShift, int * piGuide)
{

   ASSERT(GetDocument()->m_bSnapToGuides);

   LongPoint lptSnap;
   Coord topLeft = m_Mapper.DPtoLP((long)10), bottomRight = m_Mapper.DPtoLP((long)10);

   bool bVert = false;
   bool bHorz = false;

   bool bFound = false;
   POSITION pos = m_Selection.GetHeadPosition();
   while (pos && !bFound)
   {
      CMDShape* pShape = m_Selection.GetNext(pos);


      int c = pShape->GetNumShapePoints();

      for (int iPoint = 0; iPoint < c; iPoint++)
      {

         auto p = pShape->GetShapePoint(iPoint);


         LongPoint lptMove = lpt - lptShift;


         p += lptMove;

         LongRect lrcProx(p.x - topLeft, p.y - topLeft, p.x + bottomRight, p.y + bottomRight);

         for (int i = 0; i < GetDocument()->m_guidea.GetCount(); i++)
         {

            auto & guide = GetDocument()->m_guidea[i];

            if (!bVert && guide.m_bVertical)
            {

               if (guide.m_coord >= lrcProx.left && guide.m_coord <= lrcProx.right)
               {

                  lpt.x = lpt.x+guide.m_coord - p.x;

                  bVert = true;

               }

            }
            else if (!bHorz)
            {

               if (guide.m_coord >= lrcProx.top && guide.m_coord <= lrcProx.bottom)
               {

                  lpt.y = lpt.y + guide.m_coord - p.y;

                  bHorz = true;

               }

            }

            if (piGuide)
            {

               if (bVert || bHorz)
               {

                  *piGuide = i;

                  return true;

               }

            }

            if (bVert && bHorz)
            {

               return true;

            }

         }

      }

   }

   return false;

}



bool CPCDraftView::SnapToObject(LongPoint &lpt) const
{
   LongPoint lptSnap;
   Coord topLeft = m_Mapper.DPtoLP((long)4), bottomRight = m_Mapper.DPtoLP((long)5);
   LongRect lrcProx(lpt.x - topLeft, lpt.y - topLeft, lpt.x + bottomRight, lpt.y + bottomRight);
   CLayer* pLayer = GetDocument()->GetActiveLayer();
   if (pLayer == NULL)
   {
      return false;
   }
   POSITION pos = pLayer->m_ShapeList.GetHeadPosition();
   while (pos)
   {
      CMDShape* pShape = pLayer->m_ShapeList.GetNext(pos);
      if (pShape->Snap(&lptSnap, lrcProx, 1) >= 0)
      {
         lpt = lptSnap;
         return true;
      }
   }
   return false;
}

bool CPCDraftView::SnapToPoint(LongPoint& lpt,int nType) const
{
   bool bFound=false;
   LongPoint lptSnap;
   WrFixed dist;
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   POSITION pos=pLayer->m_ShapeList.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos);
      switch(nType)
      {
      case 1:	// center
      {
         LongRect lrc=pShape->GetExtents();
         lrc.Inset(-FixedMinDelta,-FixedMinDelta);
         if(lrc.Contains(lpt))
         {
            LongPoint lptMiddle=Middle(lrc);
            WrFixed dist2=Pythag(lptMiddle-lpt);
            if(!bFound || dist2<dist)
            {
               lptSnap=lptMiddle;
               dist=dist2;
               bFound=true;
            }
         }
      }
      break;
      case 2:
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(m_Mapper.LPtoDP(lpt),&m_Mapper);
         if(pEdge)
         {
            if(Pythag(pEdge->m_lptFrom-lpt)>Pythag(pEdge->m_lptTo-lpt)) lpt=pEdge->m_lptTo;
            else lpt=pEdge->m_lptFrom;
            return true;
         }
      }
      break;
      case 3:
      {
         CEdge* pEdge=pShape->GetShapeEdgeFromPt(m_Mapper.LPtoDP(lpt),&m_Mapper);
         if(pEdge && pEdge->m_nType!=CEdge::kBezierEdge)
         {
            lpt=pEdge->MidPoint();
            return true;
         }
      }
      break;
      }
   }
   if(bFound) lpt=lptSnap;
   return bFound;
}

void CPCDraftView::SnapLPt(LongPoint &lpt,bool bSnapToPoint)
{
   bool wasSnapped=false;

   if(bSnapToPoint)
   {
      int nType=0;
      if(GetAsyncKeyState(0x43)<0) nType=1;		// VK_C
      else if(GetAsyncKeyState(0x45)<0) nType=2;	// VK_E
      else if(GetAsyncKeyState(0x4D)<0) nType=3;	// VK_M
      if(nType)
         wasSnapped=SnapToPoint(lpt,nType);
   }
   if(!wasSnapped && GetDocument()->m_bSnapToGuides) wasSnapped=SnapToGuides(lpt);
   if(!wasSnapped && GetDocument()->m_bSnapToObject) wasSnapped=SnapToObject(lpt);
   if(!wasSnapped && m_nSnapMode==kSnap) SnapToGrid(lpt);
}

void CPCDraftView::SnapLength(Coord& length) const
{
   long adjLength, halfSnap;
   long newLength, divMod;
//	bool bSign;

   if(!m_Snap.fSnapRatio.fN) return;

   halfSnap=MulDiv(0x00008000,m_Snap.fSnapRatio.fN,m_Snap.fSnapRatio.fD);

   adjLength=length.GetFixed();
   /*	if(adjLength<0)
   	{
   		bSign=true;
   		adjLength=-adjLength;
   	}
   	else bSign=false;
   */
   adjLength+=halfSnap;

   /* test for overflow conditions */
   if( adjLength >= 0x7fffffffL / (long)m_Snap.fSnapRatio.fD)
   {
      divMod=adjLength/m_Snap.fSnapRatio.fN;
      divMod*=m_Snap.fSnapRatio.fD;

      newLength=(divMod & 0xffff0000)/m_Snap.fSnapRatio.fD;
      newLength*=m_Snap.fSnapRatio.fN;

   }
   else
   {
      divMod=adjLength*m_Snap.fSnapRatio.fD;	/* divide by snap */
      divMod/=m_Snap.fSnapRatio.fN;

      newLength=(divMod & 0xffff0000)*m_Snap.fSnapRatio.fN;
      newLength/=m_Snap.fSnapRatio.fD;

   }
//	if(bSign) newLength=-newLength;
   length=WrFixedFromFixed(newLength);
}

void CPCDraftView::SnapLPtToLength(const LongPoint& lptFrom, LongPoint& lptTo) const
{
   Angle angle=mtAngle(lptFrom,lptTo);
   Coord dist=Pythag(lptFrom-lptTo);
   SnapLength(dist);
   lptTo.x.SetFromRawDoubleTruncated(lptFrom.x.GetRawDouble()+(double)(mtCos(angle)* dist.GetRawDouble())+0.5);
   lptTo.y.SetFromRawDoubleTruncated(lptFrom.y.GetRawDouble()+(double)(mtSin(angle)* dist.GetRawDouble())+0.5);
}

void CPCDraftView::OnUpdateGridRange(CCmdUI* pCmdUI)
{
   BYTE nSnap=(pCmdUI->m_nID-ID_GRID_BASE);
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   pCmdUI->SetCheck(nSnap==pLayer->m_nSnap);
}

void CPCDraftView::OnGridRange(UINT nID)
{
   BYTE nSnap=(nID-ID_GRID_BASE);
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   pLayer->m_nSnap=nSnap;
   SetSnap(nSnap);
   ResetDupDelta();
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::SetSnap(int index)
{
   CPCDraftDoc* pDoc=GetDocument();
   pDoc->GetSnapRatio(index,m_Snap.fSnapRatio);

   LongPoint snappedOrigin,rulerOrigin,longRes;
   rulerOrigin = pDoc->m_RulerOrigin;

   /*	Derive proper snap residue so that origin is exactly
   	on snap.

   	origin.h = n * snapSize + snapResidue;
   */

   m_Snap.fSnapResidue.Clear () ;
   snappedOrigin=rulerOrigin;
   SnapLPt(snappedOrigin);

   Coord diff;
   if( (diff = rulerOrigin.x - snappedOrigin.x) < FixedZero )
      diff += mtRealToFix ((double)m_Snap.fSnapRatio.fN /
                           (double)m_Snap.fSnapRatio.fD );
   m_Snap.fSnapResidue.x = diff;

   if( (diff = rulerOrigin.y - snappedOrigin.y) < FixedZero )
      diff += mtRealToFix ((double)m_Snap.fSnapRatio.fN /
                           (double)m_Snap.fSnapRatio.fD );
   m_Snap.fSnapResidue.y=diff;
}

void CPCDraftView::DoAlign(LongPoint *pLpt)
{
   BeginSaveUndo();
   UnselectShapes(usLocked);

   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      LongPoint objPt;
      objPt.x=FixedMin;
      objPt.y=FixedMin;
      if(gSelMask & alignMask_Datum) objPt=pShape->GetShapeDatum();
      else
      {
         LongRect ext=pShape->GetExtents();
         LongPoint ptCenter=Middle(ext);
//			if((gSelMask & alignMask_Left?true:false) == (gSelMask & alignMask_Right?true:false)) objPt.x=ptCenter.x;
         if((gSelMask & alignMask_HorizCenters) == alignMask_HorizCenters) objPt.x=ptCenter.x;
         else
         {
            if(gSelMask & alignMask_Left) objPt.x=ext.left;
            else if(gSelMask & alignMask_Right) objPt.x=ext.right;
         }
//			if((gSelMask & alignMask_Top?true:false) == (gSelMask & alignMask_Bottom?true:false)) objPt.y=ptCenter.y;
         if((gSelMask & alignMask_VertCenters) == alignMask_VertCenters) objPt.y=ptCenter.y;
         else
         {
            if(gSelMask & alignMask_Top) objPt.y=ext.top;
            else if(gSelMask & alignMask_Bottom) objPt.y=ext.bottom;
         }
      }

      LongPoint refPt;
      if(pLpt) refPt=*pLpt;
      else
      {
         refPt=objPt;
         SnapToGrid(refPt);
      }

      LongPoint delta;
      delta.x=(refPt.x==FixedMin || objPt.x==FixedMin)? FixedZero : refPt.x-objPt.x;
      delta.y=(refPt.y==FixedMin || objPt.y==FixedMin)? FixedZero : refPt.y-objPt.y;

      InvalidateShape(pShape,true);
      pShape->DoShapeOffset(delta);
      InvalidateShape(pShape,true);
      GetDocument()->SetModifiedFlag();
   }
   if(RecalcLinkedVertexes())
   {
      CommitSaveUndo(IDS_ALIGN);
      ForceResizeUpdate();
   }
   else
   {
      AbortSaveUndo(true);
      AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
   }
   if(gRestoreAlignState)
   {
      gAlignMode=gSavedAlignMode;
      gSelMask=gSavedSelMask;
      gRefMask=gSavedRefMask;
      gRestoreAlignState=false;
   }
}

void CPCDraftView::OnUpdateArrangeAlignAgain(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(gAlignMode!=alignMode_None && HaveUnlockedSelection());
}

void CPCDraftView::OnArrangeAlignAgain()
{
   switch(gAlignMode)
   {
   case alignMode_RefObj:
      SetOneTimeTool(ID_TOOL_SELECT_REF_OBJECT,IDS_CLICKREFERENCEOBJECT);
      break;
   case alignMode_RefPt:
      SetOneTimeTool(ID_TOOL_SELECT_REF_POINT,IDS_CLICKREFERENCEPOINT,kSnap);
      break;
   case alignMode_ToGrid:
      DoAlign(NULL);
      break;
   }
}

void CPCDraftView::OnArrangeAlignOpt()
{
#ifndef _PERSONAL_EDITION
   ShowCrossCursor(false);
   CAlignmentOptionsDlg theDlg(gAlignMode,gSelMask,gRefMask);
   if(theDlg.DoModal()==IDOK)
   {
      gAlignMode=theDlg.m_mode;
      gSelMask=theDlg.m_selected;
      gRefMask=theDlg.m_reference;
      OnArrangeAlignAgain();
   };
   ShowCrossCursor();
#endif
}

void CPCDraftView::OnUpdateArrangeDistribute(CCmdUI* pCmdUI)
{
   POSITION pos=m_Selection.GetHeadPosition();
   int count=0;
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && !pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)) && (!pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) || !IsLinkedShape(pShape,-1))) count++;
   }
   pCmdUI->Enable(count>1);
}

void CPCDraftView::OnArrangeDistribute()
{
   SetOneTimeTool(ID_TOOL_DISTRIBUTE);
}

void CPCDraftView::OnActivateView(BOOL bActivate,CView* pActivateView,CView* pDeactiveView)
{
   if(!bActivate)
   {
      DeactivateTextShape();
   }
   else
   {
      if(((CPCDraftApp *)AfxGetApp())->m_pLayerFrame != NULL
         && ::IsWindowVisible(((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->GetSafeHwnd()))
      {
         ((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->Refresh();
         ((CPCDraftApp *)AfxGetApp())->m_pLayerFrame->RedrawWindow();

      }
   }
   //((CMainFrame*)AfxGetMainWnd())->m_wndAttribPalette.m_pview = this;
   CView::OnActivateView(bActivate,pActivateView,pDeactiveView);

}


void CPCDraftView::OnPaint()
{

   CPCDraftDoc* pDoc = GetDocument();

   ASSERT_VALID(pDoc);

   if (m_pbitmapDrawBackground)
   {

      CPaintDC dcPaint(this);

      m_pbitmapDraw1.reset(m_pbitmapDrawBackground->Clone(0, 0, m_pbitmapDrawBackground->GetWidth(), m_pbitmapDrawBackground->GetHeight(), PixelFormat32bppARGB));

      Gdiplus::RectF r(0, 0, m_pbitmapDrawBackground->GetWidth(), m_pbitmapDrawBackground->GetHeight());

      {

         Gdiplus::Graphics g(m_pbitmapDraw1.get());

         CDC dc;

         dc.Attach(g.GetHDC());

         CPoint ptOrg = GetDeviceScrollPosition();
         dc.SetViewportOrg(-ptOrg);

         if (m_bEditActive)
         {
            CDraftContext theContext(&dc, &m_Mapper, pDoc);
            TextParams tp;
            m_InplaceEdit.GetTextParams(tp);
            theContext.SetTextParams(tp);
            auto & PenModel = m_pActiveTextShape->GetPenModel();
            theContext.SetPen(PenModel);
            m_InplaceEdit.Draw(&theContext);
         }

         g.ReleaseHDC(dc.Detach());

      }

      Gdiplus::Graphics g2(dcPaint.GetSafeHdc());

      g2.DrawImage(m_pbitmapDraw1.get(), r, r, Gdiplus::UnitPixel);

      return;

   }

   ShowCursorPrompt(false);
   ShowCrossCursor(false);

   CPaintDC dcPaint(this);

   CRect rView;

   GetClientRect(&rView);

   auto pbitmap = new ::Gdiplus::Bitmap(
      rView.Width(),
      rView.Height(), 
      rView.Width() * 4,
      PixelFormat32bppARGB,
      nullptr);

   m_pbitmapDraw.reset(pbitmap);

   Gdiplus::RectF r(0, 0, rView.Width(), rView.Height());

   {

      Gdiplus::Graphics g(m_pbitmapDraw.get());

      CDC dc;

      dc.Attach(g.GetHDC());

      OnEraseBkgnd(&dc);
      OnPrepareDC(&dc, NULL);
      OnDraw(&dc);

      HDC hdc = dc.Detach();

      g.ReleaseHDC(hdc);

   }

   Gdiplus::Graphics g2(dcPaint.GetSafeHdc());

   g2.DrawImage(m_pbitmapDraw.get(), r, r, Gdiplus::UnitPixel);

   ShowCrossCursor();
   ShowCursorPrompt();

}


void CPCDraftView::RestoreSavedView(int nView)
{
   CPCDraftDoc* pDoc=GetDocument();
   ASSERT(nView>=0 && nView<pDoc->m_SavedViews.GetSize());
   CSavedView* pView=(CSavedView*)pDoc->m_SavedViews.GetAt(nView);
#ifdef _FLOAT_ZOOM
   int nZoomFactor;
   double fZoomFactor;
   UnpackZoomFactor(pView->m_nZoom,nZoomFactor,fZoomFactor);
   pDoc->DoZoom(fZoomFactor/(pDoc->m_fZoomFactor?pDoc->m_fZoomFactor:FloatTheFixedZoomFactor(pDoc->m_nZoomFactor)),nZoomFactor-pDoc->m_nZoomFactor,GetDeviceScrollPosition());
#else
   pDoc->DoZoom(pView->m_nZoom-pDoc->m_nZoomFactor,GetDeviceScrollPosition());
#endif
   ScrollToDevicePosition(pView->m_ptOrg);
   SaveUndo(0);
}

void CPCDraftView::OnViewMoreViews()
{
   CSelectViewDlg theDlg(GetDocument());
   if(theDlg.DoModal()==IDOK)
   {
      RestoreSavedView(theDlg.m_nView);
   }
}

void CPCDraftView::OnDeleteViewRange(UINT nID)
{
   BYTE nView=(nID-ID_VIEWS_BASE);
   CSavedView* pView=(CSavedView*)GetDocument()->m_SavedViews.GetAt(nView);
   GetDocument()->m_SavedViews.RemoveAt(nView);
   GetDocument()->SetModifiedFlag();
   delete pView;
}

void CPCDraftView::OnSavedViewRange(UINT nID)
{
   RestoreSavedView(nID-ID_VIEW_SAVED_VIEW1);
}

void CPCDraftView::OnSavedViewShortcutRange(UINT nID)
{
   int nShortcut=nID-ID_SAVED_VIEW1+1;
   CPCDraftDoc* pDoc=GetDocument();
   for(int i=0; i<pDoc->m_SavedViews.GetSize(); i++)
   {
      CSavedView* pView=(CSavedView*)pDoc->m_SavedViews[i];
      if(pView->m_nShortcut==nShortcut)
      {
         RestoreSavedView(i);
         return;
      }
   }
}

void CPCDraftView::OnViewDeleteAllViews()
{
   if(AfxMessageBox(IDS_DELETE_ALL_VIEWS,MB_YESNO|MB_DEFBUTTON2|MB_ICONWARNING)==IDYES)
   {
      GetDocument()->RemoveAllSavedViews();
      GetDocument()->SetModifiedFlag();
   }
}

void CPCDraftView::DoShapeSelect(IShapeSelectedNotify* pTarget)
{
   ASSERT(pTarget!=NULL);
   m_pShapeSelectedNotifyTarget=pTarget;
   SetOneTimeTool(ID_TOOL_SELECT_OBJECT);
}

void CPCDraftView::OnUpdateViewCursorPos(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(((CPCDraftApp*)AfxGetApp())->m_bShowCursorPosition);
}

void CPCDraftView::OnViewCursorPos()
{
   CPCDraftApp* pApp=(CPCDraftApp*)AfxGetApp();
   pApp->m_bShowCursorPosition=!pApp->m_bShowCursorPosition;
   if(!pApp->m_bShowCursorPosition)
   {
      //m_wndPosition.SetWindowText(GetDocument()->GetTitle());
      m_wndPosition.m_wstr = get_uni(GetDocument()->GetTitle());
   }
}

void CPCDraftView::RotateFeedback(CDraftContext *pDC, CRotateContext* pRotateContext)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=(CMDShape*)m_Selection.GetNext(pos);
      if(!pShape->IsLocked() && (!pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) || CanMoveDimension(pShape)))
      {
         CTrackContext* pTrackContext=pShape->TrackCreateContext(-2, (LPARAM)pRotateContext, &m_Mapper);
         if(pTrackContext)
         {
            pTrackContext->pView=this;
            pShape->TrackWireFrame(pDC, pTrackContext);
            delete pTrackContext;
         }
      }
   }
}

bool CPCDraftView::DoShapeRotate(Angle nAngle, short nAbout, const LongPoint &ptOrigin)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=(CMDShape*)m_Selection.GetNext(pos);

      if(!pShape->IsLocked() && (!pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) || CanMoveDimension(pShape)))
      {
         InvalidateShape(pShape,true);
         pShape->DoShapeRotate(nAngle,nAbout,ptOrigin);
         pShape->ReCalc(&m_Mapper);
         pShape->UpdateDependentShapes(this);
         InvalidateShape(pShape,true);
         GetDocument()->SetModifiedFlag();
      }
   }
   _CrtDumpMemoryLeaks();
   return RecalcLinkedVertexes(false);
}

void CPCDraftView::OnUpdateViewHideShowArea(CCmdUI* pCmdUI)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->HaveArea() || pShape->IsAreaShown())
      {
         pCmdUI->Enable();
         return;
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnViewHideShowArea()
{
   bool bShouldShow = false;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetAt(pos);
      if(pShape->HaveArea() && !pShape->IsAreaShown())
      {
         bShouldShow = true;
      }
      m_Selection.GetNext(pos);
   }
   restart:
   pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(bShouldShow)
      {
         if(pShape->HaveArea() && !pShape->IsAreaShown())
         {
            pShape->ShowArea(true,this);
         }
      }
      else
      {
         if(pShape->IsAreaShown())
         {
            pShape->ShowArea(false,this);
            goto restart;
         }
      }
   }
   GetDocument()->SetModifiedFlag();
}


void CPCDraftView::OnUpdateViewShowArea(CCmdUI* pCmdUI)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->HaveArea() && !pShape->IsAreaShown())
      {
         pCmdUI->Enable();
         return;
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnViewShowArea()
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->HaveArea() && !pShape->IsAreaShown())
      {
         pShape->ShowArea(true,this);
      }
   }
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateViewHideArea(CCmdUI* pCmdUI)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->IsAreaShown())
      {
         pCmdUI->Enable();
         return;
      }
   }
   pCmdUI->Enable(FALSE);
}

void CPCDraftView::OnViewHideArea()
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetAt(pos);
      if(pShape->IsAreaShown())
      {
         pShape->ShowArea(false,this);
      }
      m_Selection.GetNext(pos);
   }
   GetDocument()->SetModifiedFlag();
}

void CPCDraftView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
   if(m_bEditActive)
   {
      int nID=m_InplaceEdit.CanUndoRedo();
      CString text;
      text.LoadString(nID?nID:IDS_CANTUNDO);
      pCmdUI->SetText(text);
      pCmdUI->Enable(nID!=0);
   }
   else
   {
      CString text;
      // Infinisys Ltd
      //text.LoadString(m_bRedo?IDS_REDO:IDS_UNDO);
      text.LoadString(IDS_UNDO);
      if(m_iUndo > 0 && m_nOneTimeTool!=ID_TOOL_FILLET)
      {
         if(m_undoitemptra[m_iUndo-1]->m_iId==IDS_RESELECT) text.LoadString(IDS_RESELECT);
         else
         {
            CString text2;
            text2.LoadString(m_undoitemptra[m_iUndo - 1]->m_iId);
            text+=text2;
         }
      }
      else text.LoadString(IDS_CANTUNDO);

      text+="\tCtrl+Z";
      pCmdUI->SetText(text);
      pCmdUI->Enable(m_iUndo > 0 && m_undoitemptra[m_iUndo - 1]->m_iId>0 && m_nOneTimeTool != ID_TOOL_FILLET);
   }
}

// Unlimited Undo/Redo - Infinisys Ltd
void CPCDraftView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
   if(m_bEditActive)
   {
      CString text;
      text.LoadString(IDS_CANTREDO);
      pCmdUI->SetText(text);
      pCmdUI->Enable(FALSE);
   }
   else
   {
      CString text;
      text.LoadString(IDS_REDO);
      if(m_iUndo < m_undoitemptra.GetSize() - 1 && m_undoitemptra[m_iUndo]->m_iId > 0 && m_nOneTimeTool != ID_TOOL_FILLET)
      {
         if(m_undoitemptra[m_iUndo]->m_iId == IDS_RESELECT) text.LoadString(IDS_RESELECT);
         else
         {
            CString text2;
            text2.LoadString(m_undoitemptra[m_iUndo]->m_iId);
            text+=text2;
         }
      }
      else text.LoadString(IDS_CANTREDO);

      text+="\tCtrl+Shift+Z";
      pCmdUI->SetText(text);
      pCmdUI->Enable(m_iUndo < m_undoitemptra.GetSize() && m_undoitemptra[m_iUndo]->m_iId>0 && m_nOneTimeTool != ID_TOOL_FILLET);
   }
}

void CPCDraftView::DoEditUndo()
{

   if(m_iUndo <= 0)
   {

      return;

   }


   if(m_iUndo == m_undoitemptra.GetSize())
   {

      CMemFile * pUndo = new CMemFile(0x40000);

      SaveState(pUndo);

      m_undoitemptra.Add(new CUndoItem(m_undoitemptra[m_iUndo-1]->m_iId, pUndo));

   }

   DeactivateTextShape();

   GetDocument()->UpdateAllViews(NULL,kSaveRef);

   m_iUndo--;

   RestoreState(m_undoitemptra[m_iUndo]->m_pmemfile);

   GetDocument()->UpdateAllViews(NULL,kZoomFactorChanged);

   GetDocument()->UpdateAllViews(NULL,kRefresh);

}

// Unlimited Undo/Redo - Infinisys Ltd
void CPCDraftView::DoEditRedo()
{

   if(m_iUndo >= m_undoitemptra.GetSize() - 1)
   {

      return;

   }

   DeactivateTextShape();

   GetDocument()->UpdateAllViews(NULL,kSaveRef);

   m_iUndo++;

   RestoreState(m_undoitemptra[m_iUndo]->m_pmemfile);

   GetDocument()->UpdateAllViews(NULL,kZoomFactorChanged);

   GetDocument()->UpdateAllViews(NULL,kRefresh);

}

void CPCDraftView::OnEditUndo()
{
   if(m_bEditActive)
      m_InplaceEdit.DoUndoRedo();
   else
      DoEditUndo();
}


// Unlimited Undo/Redo - Infinisys Ltd
void CPCDraftView::OnEditRedo()
{
   if(!m_bEditActive)
      DoEditRedo();
}


// Unlimited Undo/Redo - Infinisys Ltd
void CPCDraftView::SaveUndo(int sID, CMemFile * pUndo)
{

   while(m_undoitemptra.GetSize() > m_iUndo)
   {

      delete m_undoitemptra[m_undoitemptra.GetUpperBound()];

      m_undoitemptra.RemoveAt(m_undoitemptra.GetUpperBound());

   }


   if(sID)
   {

      if(!pUndo)
      {

         pUndo = new CMemFile(0x40000);

         SaveState(pUndo);

      }

      m_undoitemptra.Add(new CUndoItem(sID, pUndo));

      m_iUndo = m_undoitemptra.GetSize();

   }

}

void CPCDraftView::BeginSaveUndo()
{
   if(m_pTemp==NULL)
   {
      GetDocument()->UpdateAllViews(NULL,kSaveRef);
      m_pTemp=new CMemFile(0x40000);
      SaveState(m_pTemp);
   }
}

void CPCDraftView::CommitSaveUndo(int sID)
{
   ASSERT(m_pTemp);
   SaveUndo(sID,m_pTemp);
   m_pTemp=NULL;
}

void CPCDraftView::AbortSaveUndo(bool bRestore)
{
   if(m_pTemp)
   {
      if(bRestore) RestoreState(m_pTemp);
      delete m_pTemp;
      m_pTemp=NULL;
   }
}

void CPCDraftView::SaveState(CMemFile *pFile)
{
   CArchive ar(pFile,CArchive::store);
   GetDocument()->Serialize(ar);
   m_Selection.Serialize(ar);
   ar.Close();
}

void CPCDraftView::RestoreState(CMemFile *pFile)
{
   CPCDraftDoc* pDoc=GetDocument();
   // save values that shuld not be affected by undo
   bool bHideFills=pDoc->m_bHideFills;
   int nZoomFactor=pDoc->m_nZoomFactor;
#ifdef _FLOAT_ZOOM
   double fZoomFactor=pDoc->m_fZoomFactor;
#endif
   bool bShowRulers=pDoc->m_bShowRulers;

   m_Selection.RemoveAll();
   pDoc->DeleteContents();
   pFile->SeekToBegin();
   CArchive ar(pFile,CArchive::load);
   pDoc->Serialize(ar);
   m_Selection.Serialize(ar);
   ar.Close();
   pDoc->UpdateAllViews(NULL,kResolveRef);

   // restore saved
   pDoc->m_bShowRulers=bShowRulers;
   pDoc->m_nZoomFactor=nZoomFactor;
#ifdef _FLOAT_ZOOM
   pDoc->m_fZoomFactor=fZoomFactor;
#endif
   pDoc->m_bHideFills=bHideFills;
}

void CPCDraftView::OnUpdateShowSizeRange(CCmdUI* pCmdUI)
{
   BYTE nParam=(pCmdUI->m_nID-ID_TOOL_SHOWSIZE1);
   pCmdUI->SetText(m_sShowSize[nParam]);
}

void CPCDraftView::ClearShowSize()
{
   m_sShowSize[0]="";
   m_sShowSize[1]="";
   m_sShowSize[2]="";
   m_sShowSize[3]="";
}

void CPCDraftView::SetShowSize(int nIndex, int nType, WrFixed value)
{
   CPCDraftDoc* pDoc=GetDocument();
   m_sShowSize[nIndex].LoadString(IDS_SHOWSIZE_BASE+nType);
   LongRatio ratio;
   pDoc->GetActiveScaleRatio(ratio);
   if(nType>0)	m_sShowSize[nIndex]+=pDoc->GetDimUnit()->numUser2Str(pDoc->GetDimUnit()->numDB2User(ratio,value),false);
   else m_sShowSize[nIndex]+=pDoc->GetDimUnit()->makeADistStr(value,false);
}

void CPCDraftView::OnUpdateResizeRange(CCmdUI* pCmdUI)
{
   const ResizeInfo* pInfo=NULL;
   CMDShape* pResizeShape=NULL;
   if(m_nOneTimeTool!=ID_TOOL_FILLET)
   {
      POSITION pos=m_Selection.GetTailPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetPrev(pos);
         if(pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr))) continue;
         if(!pShape->IsLocked())
         {
            pInfo=pShape->GetResizeInfo();
            if(pInfo) pResizeShape=pShape;
         }
         break;
      }

   }
   if(pResizeShape)
   {
      if(m_pResizeShape!=pResizeShape)
      {
         m_pResizeShape=pResizeShape;
         m_nResizeShapeHandle=-1;
         m_pResizeShape->GetResizeValue(m_ResizeValue,m_nResizeShapeHandle);
      }
   }
   else m_pResizeShape=NULL;

   CString text="";
   switch(pCmdUI->m_nID)
   {
   case ID_TOOL_RESIZE_SELECTPT:
      pCmdUI->Enable(pInfo!=NULL /* && pInfo->nTitle==IDS_EDIT_COORD */);
      break;
   case ID_TOOL_RESIZE_ABS:
      pCmdUI->SetCheck(!m_bResizeRel);
      break;
   case ID_TOOL_RESIZE_REL:
      pCmdUI->SetCheck(m_bResizeRel);
      break;
   case ID_TOOL_RESIZE_TITLE:
      if(pInfo) text.LoadString(pInfo->nTitle);
      pCmdUI->SetText(text);
      break;
   case ID_TOOL_RESIZE_LABEL1:
   case ID_TOOL_RESIZE_LABEL2:
   case ID_TOOL_RESIZE_LABEL3:
   case ID_TOOL_RESIZE_LABEL4:
   case ID_TOOL_RESIZE_LABEL5:
   case ID_TOOL_RESIZE_LABEL6:
      if(pInfo && int(pCmdUI->m_nID-ID_TOOL_RESIZE_LABEL1)<pInfo->nParams)
      {
         CString str;
         str.LoadString(pInfo->params[pCmdUI->m_nID-ID_TOOL_RESIZE_LABEL1].nName);
         pCmdUI->SetText(str);
         pCmdUI->m_pOther->ShowWindow(SW_SHOW);
      }
      else
      {
         pCmdUI->m_pOther->ShowWindow(SW_HIDE);
      }
      break;
   case ID_TOOL_RESIZE_VALUE1:
   case ID_TOOL_RESIZE_VALUE2:
   case ID_TOOL_RESIZE_VALUE3:
   case ID_TOOL_RESIZE_VALUE4:
   case ID_TOOL_RESIZE_VALUE5:
   case ID_TOOL_RESIZE_VALUE6:
      if(pInfo && int(pCmdUI->m_nID-ID_TOOL_RESIZE_VALUE1)<pInfo->nParams)
      {
         pCmdUI->m_pOther->ShowWindow(SW_SHOW);
         if(pInfo->nTitle==IDS_EDIT_COORD && m_nResizeShapeHandle==-1) //m_ResizeValue[2].m_Long==-1)
         {
            pCmdUI->SetText("");
            break;
         }
         if(::GetFocus()!=pCmdUI->m_pOther->GetSafeHwnd())
         {
            int n=pCmdUI->m_nID-ID_TOOL_RESIZE_VALUE1;
            CString theValue;
            switch(pInfo->params[n].nType)
            {
            case kUnitlessValue:
            {
               WrFixed value=m_ResizeValue[n].m_Unitless;
               if(m_bResizeRel)
               {
                  ResizeValue theOldValue[8];
                  if(pInfo->nTitle==IDS_EDIT_COORD)
                  {
                     theOldValue[2].m_Long=m_ResizeValue[2].m_Long;
                  }
                  m_pResizeShape->GetResizeValue(theOldValue,m_nResizeShapeHandle);
                  value-=theOldValue[n].m_Unitless;
               }
               LongRatio ratio;
               CPCDraftDoc* pDoc=GetDocument();
               pDoc->GetActiveScaleRatio(ratio);
               theValue=pDoc->GetDimUnit()->numUser2Str(pDoc->GetDimUnit()->numDB2User(ratio,value),false);
            }
            break;
            case kAngleValue:
            {
               Angle value=m_ResizeValue[n].m_Angle;
               if(m_bResizeRel)
               {
                  ResizeValue theOldValue[8];
                  if(pInfo->nTitle==IDS_EDIT_COORD)
                  {
                     theOldValue[2].m_Long=m_ResizeValue[2].m_Long;
                  }
                  m_pResizeShape->GetResizeValue(theOldValue,m_nResizeShapeHandle);
                  value-=theOldValue[n].m_Angle;
               }
               theValue=GetDocument()->GetDimUnit()->makeADistStr(value,false);
            }
            break;
            case kDoubleValue:
               theValue.Format("%g",m_ResizeValue[n].m_Double);
               break;
            case kLongValue:
               theValue.Format("%d",m_ResizeValue[n].m_Long);
               break;
            }
            pCmdUI->SetText(theValue);
         }
      }
      else
         pCmdUI->m_pOther->ShowWindow(SW_HIDE);
      break;
   case ID_TOOL_RESIZE_POSITION_X:
   case ID_TOOL_RESIZE_POSITION_Y:
      if(::GetFocus()!=pCmdUI->m_pOther->GetSafeHwnd())
      {
         if(m_pResizeShape)
         {
            int n=pCmdUI->m_nID==ID_TOOL_RESIZE_POSITION_X?6:7;
            WrFixed value=m_ResizeValue[n].m_Unitless;
            if(m_bResizeRel)
            {
               ResizeValue theOldValue[8];
               if(pInfo->nTitle==IDS_EDIT_COORD)
               {
                  theOldValue[2].m_Long=m_ResizeValue[2].m_Long;
               }
               m_pResizeShape->GetResizeValue(theOldValue,m_nResizeShapeHandle);
               value-=theOldValue[n].m_Unitless;
            }
            LongRatio ratio;
            CPCDraftDoc* pDoc=GetDocument();
            pDoc->GetActiveScaleRatio(ratio);
            pCmdUI->SetText(pDoc->GetDimUnit()->numUser2Str(pDoc->GetDimUnit()->numDB2User(ratio,value),false));
         }
         else
         {
            pCmdUI->SetText("");
         }
      }
      break;
   }
}

void CPCDraftView::UpdateResizeValue(UINT nID)
{
   if(m_pResizeShape)
   {
      const ResizeInfo* pInfo=m_pResizeShape->GetResizeInfo();
      CWnd* pEditWnd=CWnd::FromHandle((HWND)CWnd::GetCurrentMessage()->lParam);
      CString sValue;
      pEditWnd->GetWindowText(sValue);
      CPCDraftDoc* pDoc=GetDocument();
      switch(nID)
      {
      case ID_TOOL_RESIZE_POSITION_X:
      case ID_TOOL_RESIZE_POSITION_Y:
      {
         double userValue;
         if(pDoc->GetDimUnit()->numStr2User(sValue,userValue))
         {
            LongRatio ratio;
            pDoc->GetActiveScaleRatio(ratio);
            WrFixed value=pDoc->GetDimUnit()->numUser2DB(ratio,userValue);
            int n=nID==ID_TOOL_RESIZE_POSITION_X?6:7;
            if(m_bResizeRel)
            {
               ResizeValue theOldValue[8];
               if(pInfo->nTitle==IDS_EDIT_COORD)
               {
                  theOldValue[2].m_Long=m_ResizeValue[2].m_Long;
               }
               m_pResizeShape->GetResizeValue(theOldValue,m_nResizeShapeHandle);
               value+=theOldValue[n].m_Unitless;
            }
            m_ResizeValue[n].m_Unitless=value;
         }
      }
      break;
      case ID_TOOL_RESIZE_VALUE1:
      case ID_TOOL_RESIZE_VALUE2:
      case ID_TOOL_RESIZE_VALUE3:
      case ID_TOOL_RESIZE_VALUE4:
      case ID_TOOL_RESIZE_VALUE5:
      case ID_TOOL_RESIZE_VALUE6:
      {
         int n=nID-ID_TOOL_RESIZE_VALUE1;
         const ResizeInfo* pInfo=m_pResizeShape->GetResizeInfo();
         switch(pInfo->params[n].nType)
         {
         case kUnitlessValue:
         {
            double userValue;
            if(pDoc->GetDimUnit()->numStr2User(sValue,userValue))
            {
               LongRatio ratio;
               pDoc->GetActiveScaleRatio(ratio);
               WrFixed value=pDoc->GetDimUnit()->numUser2DB(ratio,userValue);
               if(m_bResizeRel)
               {
                  ResizeValue theOldValue[8];
                  if(pInfo->nTitle==IDS_EDIT_COORD)
                  {
                     theOldValue[2].m_Long=m_ResizeValue[2].m_Long;
                  }
                  m_pResizeShape->GetResizeValue(theOldValue,m_nResizeShapeHandle);
                  value+=theOldValue[n].m_Unitless;
               }
               m_ResizeValue[n].m_Unitless=value;
            }
         }
         break;
         case kAngleValue:
         {
            Angle value;
            if(pDoc->GetDimUnit()->numStr2Angle(sValue,value))
            {
               if(m_bResizeRel)
               {
                  ResizeValue theOldValue[8];
                  if(pInfo->nTitle==IDS_EDIT_COORD)
                  {
                     theOldValue[2].m_Long=m_ResizeValue[2].m_Long;
                  }
                  m_pResizeShape->GetResizeValue(theOldValue,m_nResizeShapeHandle);
                  value+=theOldValue[n].m_Angle;
               }
               m_ResizeValue[n].m_Angle=value;
            }
         }
         break;
         case kDoubleValue:
            m_ResizeValue[n].m_Double=atof(sValue);
            break;
         case kLongValue:
            m_ResizeValue[n].m_Long=atol(sValue);
            break;
         }
      }
      break;
      }
   }
}

void CPCDraftView::OnKillFocusResizePositionX()
{
   UpdateResizeValue(ID_TOOL_RESIZE_POSITION_X);
}

void CPCDraftView::OnKillFocusResizePositionY()
{
   UpdateResizeValue(ID_TOOL_RESIZE_POSITION_Y);
}

void CPCDraftView::OnKillFocusResizeValue1()
{
   UpdateResizeValue(ID_TOOL_RESIZE_VALUE1);
}

void CPCDraftView::OnKillFocusResizeValue2()
{
   UpdateResizeValue(ID_TOOL_RESIZE_VALUE2);
}

void CPCDraftView::OnKillFocusResizeValue3()
{
   UpdateResizeValue(ID_TOOL_RESIZE_VALUE3);
}

void CPCDraftView::OnKillFocusResizeValue4()
{
   UpdateResizeValue(ID_TOOL_RESIZE_VALUE4);
}

void CPCDraftView::OnKillFocusResizeValue5()
{
   UpdateResizeValue(ID_TOOL_RESIZE_VALUE5);
}

void CPCDraftView::OnKillFocusResizeValue6()
{
   UpdateResizeValue(ID_TOOL_RESIZE_VALUE6);
}

void CPCDraftView::OnResizeRange(UINT nID)
{
   switch(nID)
   {
   case ID_TOOL_RESIZE_ABS:
      m_bResizeRel=false;
      break;
   case ID_TOOL_RESIZE_REL:
      m_bResizeRel=true;
      break;
   case ID_TOOL_RESIZE_APPLY:
      if(m_pResizeShape)
      {
         if((m_pResizeShape->IsKindOf(RUNTIME_CLASS(CMDRadialDim)) || m_pResizeShape->IsKindOf(RUNTIME_CLASS(CMDAngularDim))) && IsLinkedShape(m_pResizeShape))
         {
            ResizeValue resizeValue[8];
            m_pResizeShape->GetResizeValue(resizeValue,m_nResizeShapeHandle);
            m_ResizeValue[0]=resizeValue[0];
            m_ResizeValue[1]=resizeValue[1];
         }
         CPCDraftDoc* pDoc=GetDocument();
         BeginSaveUndo();
         InvalidateShape(m_pResizeShape,true);
         m_pResizeShape->SetResizeValue(m_ResizeValue,m_nResizeShapeHandle);
         m_pResizeShape->ProcessDeletedShapes(&GetActiveLinkVertexList());
         m_pResizeShape->ReCalc(&m_Mapper);
         m_pResizeShape->UpdateDependentShapes(this);
         pDoc->SetModifiedFlag();

         UINT strId=0;
         if(RecalcLinkedVertex(m_pResizeShape))
         {
            if(pDoc->CheckDocSize(false))
            {
               CommitSaveUndo(IDS_RESIZE);
               InvalidateShape(m_pResizeShape,true);
               const ResizeInfo* pInfo=m_pResizeShape->GetResizeInfo();
               if(pInfo->nTitle==IDS_EDIT_COORD)
                  m_ResizeValue[2].m_Long=-1;
            }
            else strId=IDS_CANT_RESIZE;
         }
         else strId=IDS_UNABLE_COMPLETE_ACTION;
         m_pResizeShape=NULL;	// will be assigned on next update
         if(strId)
         {
            AbortSaveUndo(true);
            AfxMessageBox(strId);
         }
      }
      break;
   case ID_TOOL_RESIZE_SELECTPT:
      SetOneTimeTool(ID_TOOL_SELECT_HANDLE,IDS_CLICKPOINT,kSnapObject);
      break;
   }
}

void CPCDraftView::ForceResizeUpdate()
{
   m_pResizeShape=NULL;
}

void CPCDraftView::OnKillFocus(CWnd* pNewWnd)
{
//	CView::OnKillFocus(pNewWnd);
   if(m_bEditActive) m_InplaceEdit.OnKillFocus();
}

void CPCDraftView::OnSetFocus(CWnd* pOldWnd)
{
//	CView::OnSetFocus(pOldWnd);
   if(m_bEditActive) m_InplaceEdit.OnSetFocus();
}

int tool_palette_get_shortcut_command(int iShortcut);

void CPCDraftView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   CPCDraftDoc* pDoc=GetDocument();
   //if(nChar>0x20 && !m_bEditActive && GetAsyncKeyState(VK_COMMAND)>=0 && GetAsyncKeyState(VK_OPTION)>=0 && (nFlags&0x4000)==0 && m_Selection.GetCount()==1 && !pDoc->m_bHideText)
   //   CommonChar();
   if (nChar == 27)
   {
      PostMessage(WM_COMMAND, ID_TOOL_ARROW);
      return;
   }
   if (m_bEditActive)
   {
      switch (nChar)
      {
      case 0x08: m_InplaceEdit.DoBackSpace(); break;
      case 0x0D: m_InplaceEdit.DoReturn(); break;
      default:
         m_InplaceEdit.DoTyping(nChar);
      }
      if (!m_InplaceEdit.m_bImeComposition)
      {
         m_InplaceEdit.SaveState(&m_ime);
      }
   }
   else if (GetAsyncKeyState(VK_SHIFT) >= 0)
   {
      int iCommand = tool_palette_get_shortcut_command(nChar);
      if (iCommand > 0)
      {
         PostMessage(WM_COMMAND, iCommand);
         return;
      }
   }
   else CView::OnChar(nChar, nRepCnt, nFlags);
}


bool CPCDraftView::is_text_mode() const
{

   return m_nCurrentTool == ID_TOOL_TEXT;

}


#ifndef _UNICODE

LRESULT CPCDraftView::OnImeChar(WPARAM wParam, LPARAM lParam)
{
   CPCDraftDoc* pDoc=GetDocument();
   if(!m_bEditActive && GetAsyncKeyState(VK_COMMAND)>=0 && GetAsyncKeyState(VK_OPTION)>=0 && ((UINT)lParam&0x4000)==0 && m_Selection.GetCount()==1 && !pDoc->m_bHideText)
      CommonChar();


   if(m_bEditActive)
   {
      char pChar[2];
      pChar[0] = (char)(wParam >> 8);
      pChar[1] = (char)(wParam);
      if (_istlead(pChar[0]))
         m_InplaceEdit.DoImeTyping(CString(pChar, 2));
      else
         m_InplaceEdit.DoTyping(pChar[1]);
      return TRUE;
   }
   return FALSE;
}
#endif


void CPCDraftView::OnUpdateEditCopyCut(CCmdUI* pCmdUI)
{
   if(m_bEditActive) pCmdUI->Enable(m_InplaceEdit.CanCopyCut());
   else if(ID_EDIT_COPY==pCmdUI->m_nID) OnUpdateHaveSelection(pCmdUI);
   else OnUpdateHaveUnlockedSelection(pCmdUI);
}

void CPCDraftView::OnUpdateToolDimType(CCmdUI* pCmdUI)
{
   if(pCmdUI->m_pOther->SendMessage(CB_GETDROPPEDSTATE,m_nDimType,0)==0
         && pCmdUI->m_pOther->SendMessage(CB_GETCURSEL,0,0)!=m_nDimType)
   {
      pCmdUI->m_pOther->SendMessage(CB_SETCURSEL,m_nDimType,0);
   }
}

void CPCDraftView::OnSelChangeDimType()
{
   CWnd* pCBWnd=CWnd::FromHandle((HWND)CWnd::GetCurrentMessage()->lParam);
   int nDimType=pCBWnd->SendMessage(CB_GETCURSEL,0,0);
   if(nDimType!=m_nDimType)
   {
      if(m_nOneTimeTool==ID_TOOL_DIM_NEXT_POINT)
         delete m_pLineDimSketcher;
      CancelCurrentTool();
      m_nDimType=nDimType;
   }
}

void CPCDraftView::OnMouseMove(UINT nFlags, CPoint point)
{

   CView::OnMouseMove(nFlags, point);

   if (GetDocument()->m_bSnapToGuides && GetAsyncKeyState(VK_COMMAND) < 0 && GetAsyncKeyState(VK_MENU) < 0)
   {
      CPoint pt = point + GetDeviceScrollPosition();
      LongPoint lptNew = m_Mapper.DPtoLP(pt);
      auto lptTest = lptNew;
      int iGuide = -1;
      bool bSnap = SnapToGuides(lptTest, &iGuide);

      if (bSnap && iGuide >= 0)
      {
         auto guide = GetDocument()->m_guidea[iGuide];

         if (guide.m_bVertical)
         {

            SetCursor(AfxGetApp()->LoadCursor(IDC_SPLIT));

         }
         else
         {

            SetCursor(AfxGetApp()->LoadCursor(IDC_SPLITH));

         }


         return;

      }

   }

   AdjustCursor(point,false);

}


LRESULT CPCDraftView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
   if(m_bMouseTracking)
   {
      m_bMouseTracking=false;
      ShowCursorPrompt(false);
      ShowCrossCursor(false,false);
   }
   return FALSE;
}

bool CPCDraftView::NeedMouseTrack()
{
   return (m_bCursorPrompt /*&& m_bPromptVisible*/) || (NeedCrossCursor()/* && m_bCrossVisible*/ || GetDocument()->m_bShowRulers);
}

void CPCDraftView::CancelMouseTracking(bool bInSketcher)
{
   if(m_bMouseTracking)
   {
      TRACKMOUSEEVENT tme;
      tme.cbSize = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags=TME_CANCEL|TME_LEAVE;
      tme.hwndTrack=GetSafeHwnd();
      ::_TrackMouseEvent(&tme);
      m_bMouseTracking=false;

      ShowCursorPrompt(false);
      ShowCrossCursor(false,false);
      if(bInSketcher)
         SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
}

void CPCDraftView::AdjustCursor(CPoint &pt,bool bInSketcher)
{
   if(bInSketcher)
   {
      CPoint sPt=pt;
      ClientToScreen(&sPt);
      bInSketcher=CWnd::WindowFromPoint(sPt)!=this;
   }

   CRect rcClient;
   GetActiveClientRect(rcClient);
   if(rcClient.PtInRect(pt) && !bInSketcher)
   {
      LongPoint lptNew=m_Mapper.DPtoLP(pt+GetDeviceScrollPosition());
      if(m_nSnapMode>kNoSnap && GetAsyncKeyState(VK_COMMAND)>=0) SnapLPt(lptNew);
      if(lptNew!=m_lptCursorPos || !m_bMouseTracking)
      {
         m_lptCursorPos=lptNew;

         CString text;
         CPCDraftDoc* pDoc=GetDocument();
         if(((CPCDraftApp*)AfxGetApp())->m_bShowCursorPosition)
         {
            LongPoint lpt=m_lptCursorPos-pDoc->m_RulerOrigin;
            LongRatio ratio;
            pDoc->GetActiveScaleRatio(ratio);
            text.Format("%s , %s",pDoc->GetDimUnit()->numUser2Str(pDoc->GetDimUnit()->numDB2User(ratio,lpt.x),false),pDoc->GetDimUnit()->numUser2Str(pDoc->GetDimUnit()->numDB2User(ratio,lpt.y),false));

         }
         else text=pDoc->GetTitle();
         //AfxSetWindowText(m_wndPosition.GetSafeHwnd(),text);
         m_wndPosition.m_wstr = get_uni(text);

         CPoint ptNew=m_Mapper.LPtoDP(lptNew)-GetDeviceScrollPosition();
         if(rcClient.PtInRect(ptNew))
         {
            m_ptCursorPos=ptNew;
            if(NeedMouseTrack())
            {
               if(m_bMouseTracking)
               {
                  if(m_nPromptVisible>0) m_Prompt.DragShowNolock(FALSE);
                  if(m_nCrossVisible>0) MoveCrossCursor(ptNew);
                  else MoveRulersCursor(ptNew);
                  if(m_nPromptVisible>0)
                  {
                     m_Prompt.DragShowNolock(TRUE);
                     m_Prompt.DragMove(ptNew);
                  }
               }
               else
               {
                  TRACKMOUSEEVENT tme;
                  tme.cbSize = sizeof(TRACKMOUSEEVENT);
                  tme.dwFlags = TME_LEAVE;
                  tme.hwndTrack=GetSafeHwnd();
                  if (::_TrackMouseEvent(&tme))
                  {
                     m_bMouseTracking=true;
                     m_ptCrossCursorPos=ptNew;
                     ShowCrossCursor(true,false);
                     ShowCursorPrompt();
                  }
               }
            }
            return;
         }
      }
      return;
   }
//	else
   {
      CancelMouseTracking(bInSketcher);
   }
}

void CPCDraftView::SetCursorPrompt(UINT nPrompt)
{
   ShowCursorPrompt(false);
   ShowCrossCursor(false);

   // clean up previous prompt if any
   if(m_bCursorPrompt)
   {
      m_Prompt.DeleteImageList();
      m_bCursorPrompt=false;
   }
   m_nPrompt=nPrompt;
   if(nPrompt)
   {
      CString sPrompt;
      sPrompt.LoadString(nPrompt);

      CClientDC dc(NULL);
      CBitmap bmp;

      LOGFONT lf;
      GetObject(GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONT),&lf);
      lf.lfWeight=FW_BOLD;

      CFont font;
      font.CreateFontIndirect(&lf);

      CRect rc(0,0,0,0);
      {
         {
            CGDIObject<CFont> oldFont(&dc,&font);
//			dc.DrawText(sPrompt,&rc,DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
            rc.SetRect(CPoint(0,0), CPoint(dc.GetTextExtent(sPrompt)));
            rc.right+=32;
            if(rc.bottom<32) rc.bottom=32;
         }
      }
      bmp.CreateCompatibleBitmap(&dc,/*32+*/rc.Width(),/*32*/rc.Height());
      {
         {
            CDC dcMem;
            dcMem.CreateCompatibleDC(&dc);
            CGDIObject<CBitmap> oldBitmap(&dcMem,&bmp);
            CGDIObject<CFont> oldFont(&dcMem,&font);
            dcMem.SetTextColor(RGB(0,0,0));
            dcMem.SetBkColor(RGB(255,0,0));
            dcMem.PatBlt(0,0,/*32+*/rc.Width(),/*32*/rc.Height(),WHITENESS);

//			rc.OffsetRect(24,8);
//			dcMem.DrawText(sPrompt,&rc,DT_NOPREFIX | DT_SINGLELINE);
            dcMem.ExtTextOut(24,8,ETO_OPAQUE|ETO_CLIPPED,&rc,sPrompt,NULL);

            HCURSOR hCur=GetCursor(false);
            dcMem.DrawIcon(0,0,hCur);
         }
      }

      VERIFY(m_Prompt.Create(/*32+*/rc.Width(),/*32*/rc.Height(),ILC_COLOR | ILC_MASK,1,1));
      VERIFY(m_Prompt.Add(&bmp,RGB(255,0,0))!=-1);
//		VERIFY(m_Prompt.Add(GetCursor())!=-1);
//		m_Prompt.GetImageCount();
//		m_Prompt.SetDragCursorImage(0,CPoint(0,0));
//		m_Prompt.BeginDrag(0,CPoint(-16,10));
//		m_Prompt.SetDragCursorImage(0,CPoint(0,0));
      m_bCursorPrompt=true;
   }
   ShowCrossCursor();
   ShowCursorPrompt();
}

void CPCDraftView::ShowCursorPromptNoLock(bool bShow)
{
   if(m_bCursorPrompt && m_nPromptVisible>0) m_Prompt.DragShowNolock(bShow);
}


void CPCDraftView::ShowCursorPrompt(bool bShow)
{

   m_nPromptVisible += bShow ? 1 : -1;

   if(m_bCursorPrompt)
   {
      
      if(bShow)
      {
         
         if(m_nPromptVisible==1)
         {
            
            //ShowCursor(FALSE); // Infinisys Ltd. PCDraft 6.0.1

            //UINT ui = ::GetDpiForWindow(GetSafeHwnd());

            int i15Pixel = dpi(15);

            //int i15Pixel = 15;

            m_Prompt.BeginDrag(0, CPoint(i15Pixel,i15Pixel));

            m_Prompt.SetDragCursorImage(0, CPoint(0,0));

            m_Prompt.DragEnter(this,m_ptCursorPos);

         }

      }
      else
      {

         if(m_nPromptVisible==0)
         {

            m_Prompt.DragLeave(this);

            m_Prompt.EndDrag();

            //ShowCursor(TRUE); // Infinisys Ltd. PCDraft 6.0.1

         }

      }

   }

}


bool CPCDraftView::NeedCrossCursor() const
{
   if((m_nOneTimeTool && m_nOneTimeTool!=ID_TOOL_DIM_NEXT_POINT) ||
         (m_nCurrentTool==ID_TOOL_ARROW && !m_bTrackingShape) ||
         (m_nCurrentTool>ID_TOOL_ARROW && m_nCurrentTool<ID_TOOL_MARQUEE) ||
         (m_nCurrentTool>=ID_TOOL_PERP_LINE && m_nCurrentTool<=ID_TOOL_OFFSET_LINE) ||
         m_nCurrentTool==ID_TOOL_CIRCLE_CIRCLE ||
         m_nCurrentTool==ID_TOOL_ARC_ARC ||
         (m_nCurrentTool>ID_TOOL_MARKER && m_nCurrentTool!=ID_TOOL_EDIT_DATUM)
     ) return false;
   return ((CPCDraftApp*)AfxGetApp())->m_bCrossCursor?true:false;
}

void CPCDraftView::ShowCrossCursor(bool bShow,bool bSwitch)
{
   m_nCrossVisible+=bShow?1:-1;

   if(m_nCrossVisible==(bShow?1:0))
   {
      if(NeedCrossCursor())
      {
         if(bShow && bSwitch) DrawRulersCursor(NULL);
         DrawCrossCursor(NULL);
         if(!bShow && bSwitch) DrawRulersCursor(NULL);
      }
      else if(!bSwitch) DrawRulersCursor(NULL);
   }
}

void CPCDraftView::MoveCrossCursor(const CPoint& pt)
{
   if(NeedCrossCursor())
   {
      CClientDC dc(this);
      DrawCrossCursor(&dc);
      m_ptCrossCursorPos=pt;
      DrawCrossCursor(&dc);
   }
   else
      MoveRulersCursor(pt);
}

void CPCDraftView::MoveRulersCursor(const CPoint& pt)
{
   if(GetDocument()->m_bShowRulers)
   {
      CClientDC dc(this);
      DrawRulersCursor(&dc);
      m_ptCrossCursorPos=pt;
      DrawRulersCursor(&dc);
   }
   else
      m_ptCrossCursorPos=pt;
}

void CPCDraftView::DrawRulersCursor(CDC *pDC)
{
   if(GetDocument()->m_bShowRulers)
   {
      CRect rc;
      GetActiveClientRect(rc);
      CPoint pt=m_ptCrossCursorPos;
      if(rc.PtInRect(pt))
      {
         CDC* pWinDC=NULL;
         if(pDC==NULL)
            pDC=pWinDC=GetDC();

         pDC->DPtoLP(&pt);
         pDC->DPtoLP(&rc);

         CGDIROP oldRop(pDC,R2_XORPEN);
         CPen pen(PS_DOT,0,COLORREF(0));
         CGDIObject<CPen> oldPen(pDC,&pen);

         pDC->MoveTo(0,pt.y);
         pDC->LineTo(kLRulerWid,pt.y);

         pDC->MoveTo(pt.x,0);
         pDC->LineTo(pt.x,kTRulerWid);

         if(pWinDC)
            ReleaseDC(pWinDC);
      }
   }
}
int CPCDraftView::dpi(int iPixel)
{

   return iPixel * window_dpi(GetSafeHwnd()) / 96;
}
void CPCDraftView::DrawCrossCursor(CDC *pDC)
{
   CRect rc;
   GetActiveClientRect(rc);
   CPoint pt=m_ptCrossCursorPos;
   if(rc.PtInRect(pt))
   {
      if(GetDocument()->m_bShowRulers)
      {
         rc.left=0;
         rc.top=0;
      }

      CDC* pWinDC=NULL;
      if(pDC==NULL)
         pDC=pWinDC=GetDC();

      pDC->DPtoLP(&pt);
      pDC->DPtoLP(&rc);

      CGDIROP oldRop(pDC,R2_XORPEN);
      CPen pen(PS_DOT,0,COLORREF(0));
      CGDIObject<CPen> oldPen(pDC,&pen);

      pDC->MoveTo(rc.left,pt.y);
      pDC->LineTo(rc.right,pt.y);

      pDC->MoveTo(pt.x,rc.top);
      pDC->LineTo(pt.x,rc.bottom);

      if(pWinDC)
         ReleaseDC(pWinDC);
   }
}

void CPCDraftView::GetActiveClientRect(CRect &rc)
{
   CPoint ptClientCorner;
   GetClientCorner(ptClientCorner);
   CSize szClientSize;
   GetClientSize(szClientSize);
   CRect rcClient(ptClientCorner,szClientSize);
   CRect rcZoomDoc(-GetDeviceScrollPosition()+ptClientCorner,m_totalDev);
   rc.IntersectRect(rcClient,rcZoomDoc);
}

void CPCDraftView::ResetDupDelta()
{
   m_bAdjustDupDelta=false;
   int nSnap;
   if(m_Snap.fSnapRatio.fN==0 || m_Snap.fSnapRatio.fD==0) nSnap=1;
   else nSnap=m_Snap.fSnapRatio.fN/m_Snap.fSnapRatio.fD;
   m_lptDupDelta.x=m_lptDupDelta.y=WrFixed(nSnap); //m_Mapper.DPtoLP(nSnap);
}

void CPCDraftView::OnUpdateToolDimEndmark(CCmdUI* pCmdUI)
{
   short nCap1,nCap2;
   CPCDraftDoc* pDoc=GetDocument();
   nCap1=pDoc->m_nDimCap1;
   nCap2=pDoc->m_nDimCap2;
   pCmdUI->m_pOther->SendMessage(UM_SETCAPS,MAKEWPARAM(nCap1,nCap2));
}

void CPCDraftView::OnToolDimEndmark()
{
   CPoint pt;
   GetCursorPos(&pt);

   CEndmarksMenu theMenu;
   int cmd=theMenu.Track(this);
   if(cmd>=0)
   {
      SaveUndo(IDS_ENDMARK);
      BYTE value=cmd/3;
      cmd%=3;

      bool bFound=false;
      if(m_Selection.GetCount())
      {
         POSITION pos=m_Selection.GetHeadPosition();
         while(pos)
         {
            CMDShape* pShape=m_Selection.GetNext(pos);
            if(!pShape->IsLocked() && pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)))
            {
               CMDAssocDim* pDim=(CMDAssocDim*)pShape;
               if(cmd<2) pDim->m_nDimCap1=value;
               if(cmd>0) pDim->m_nDimCap2=value;
               InvalidateShape(pDim,true);
               bFound=true;
            }
         }
      }
      if(!bFound)
      {
         CPCDraftDoc* pDoc=GetDocument();
         if(cmd<2) pDoc->m_nDimCap1=value;
         if(cmd>0) pDoc->m_nDimCap2=value;
      }
      GetDocument()->SetModifiedFlag();
   }
}

void CPCDraftView::DoNudge(int dx, int dy)
{
   if(HaveUnlockedSelection())
   {
      BeginSaveUndo();
      UnselectShapes(usLocked);
      WrFixed incr;
      if(GetAsyncKeyState(VK_COMMAND)<0 || m_Snap.fSnapRatio.fN==0) incr=m_Mapper.DPtoLP((long)1);
      else incr=WrFixedFromFixed((0x00010000*m_Snap.fSnapRatio.fN)/m_Snap.fSnapRatio.fD);
      OffsetSelection(LongPoint(dx*incr,dy*incr),false);
      if(RecalcLinkedVertexes())
      {
         CommitSaveUndo(IDS_MOVE);
         ForceResizeUpdate();
      }
      else
      {
         AbortSaveUndo(true);
         AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
      }
   }
}

void CPCDraftView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_OPTION:
      if(!m_bEditActive && (nFlags&0x4000)==0) UpdateCursor();
      break; // was commented and now uncommened again as requested

   default:
      CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
      break;
   }
}

void CPCDraftView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_OPTION:
      if(!m_bEditActive) UpdateCursor();
      break; // was commented and now uncommened again as requested

   default:
      CView::OnSysKeyUp(nChar, nRepCnt, nFlags);
      break;
   }
}

void CPCDraftView::OnDropFiles(HDROP hDropInfo)
{
   GetParentFrame()->ActivateFrame();
   CPoint ptDrop;
   if(::DragQueryPoint(hDropInfo,&ptDrop))
   {
      LongPoint lptDrop=m_Mapper.DPtoLP(ptDrop+GetDeviceScrollPosition());
      SnapToGrid(lptDrop);
      UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
      CPCDraftDoc* pDoc=GetDocument();
      for(UINT iFile = 0; iFile < nFiles; iFile++)
      {
         TCHAR szFileName[_MAX_PATH];
         ::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
         LPCTSTR p=strrchr(szFileName,'.');
         if(p)
         {
            CString theExt(p+1);
            if(theExt.CompareNoCase("bmp")==0)
            {
               HBITMAP hDib=(HBITMAP)::LoadImage(NULL,szFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTCOLOR|LR_CREATEDIBSECTION);
               if(hDib)
               {
                  BITMAP bmp;
                  ::GetObject(hDib,sizeof(BITMAP),&bmp);
                  CRect rc(0,0,bmp.bmWidth,bmp.bmHeight);
                  LongRect lrc(0,0,m_Mapper.DPtoLP(bmp.bmWidth),m_Mapper.DPtoLP(bmp.bmHeight));
                  lrc.Offset(lptDrop);
                  CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
                  pShape->CalculateExtents();
                  pShape->SetShapeDatum(kDefaultDatum);
                  pDoc->CreateShape(pShape);
                  SelectShape(pShape,false);
                  pDoc->SetModifiedFlag();
               }
            }
            else if(theExt.CompareNoCase("tif")==0 || theExt.CompareNoCase("tiff")==0)
            {
               HBITMAP hBitmap;
               int width,height;
               if(readTiff((LPCSTR)szFileName,hBitmap,width, height))
               {
                  CRect rc(0,0,width,height);
                  LongRect lrc(0,0,width,height);
                  lrc.Offset(lptDrop);
                  CMDBitmap* pShape=new CMDBitmap(rc,lrc,hBitmap);
                  pShape->CalculateExtents();
                  pShape->SetShapeDatum(kDefaultDatum);
                  pDoc->CreateShape(pShape);
                  SelectShape(pShape,false);
                  pDoc->SetModifiedFlag();
               }
               else AfxMessageBox(IDS_INCOMPATIBLE_TIFF);
            }
         }
      }
      pDoc->CheckDocSize(true);
      ::DragFinish(hDropInfo);
   }
}

bool CPCDraftView::PtInDocument(const LongPoint &lpt)
{
   CPCDraftDoc* pDoc=GetDocument();
   CSize docSize(pDoc->m_NumPage.x*pDoc->m_PageSize.cx,pDoc->m_NumPage.y*pDoc->m_PageSize.cy);
   return (lpt.x>=0 && lpt.x.GetLong()<=docSize.cx && lpt.y>=0 && lpt.y.GetLong()<=docSize.cy);
}


bool CPCDraftView::SnapPtToDocument(LongPoint &lpt)
{
   bool bSnapped=false;
   CPCDraftDoc* pDoc=GetDocument();
   CSize docSize(pDoc->m_NumPage.x*pDoc->m_PageSize.cx,pDoc->m_NumPage.y*pDoc->m_PageSize.cy);
   if(lpt.x>docSize.cx)
   {
      lpt.x=docSize.cx;
      bSnapped=true;
   }
   else if(lpt.x<0)
   {
      lpt.x=0;
      bSnapped=true;
   }
   if(lpt.y>docSize.cy)
   {
      lpt.y=docSize.cy;
      bSnapped=true;
   }
   else if(lpt.y<0)
   {
      lpt.y=0;
      bSnapped=true;
   }
   return bSnapped;
}

void CPCDraftView::OnFillInfo()
{
   if(GetAsyncKeyState(VK_COMMAND)<0)
   {
      if(GetAsyncKeyState(VK_SHIFT)<0) OnToolAttribPenPattern();
      else DoAttribFillPattern(GetAsyncKeyState(VK_OPTION)<0);
   }
   else
   {
      if(GetAsyncKeyState(VK_SHIFT)<0) OnToolAttribPenColor();
      else DoAttribFillColor(GetAsyncKeyState(VK_OPTION)<0);
   }
   /*	if(HaveUnlockedSelection())
   	{
   		FillModel fm;
   		fm.m_nFillType=kNoFill;
   		ApplyFill(fm,false);
   	}*/
}

void CPCDraftView::OnLineInfo()
{
   if(GetAsyncKeyState(VK_OPTION)<0) OnToolAttribLineStyle();
   else OnToolAttribLineWeigth();
}

void CPCDraftView::SnapValue(Coord &value, const LongRatio &snapRatio)
{
   if(!snapRatio.fN) return;

   long halfSnap=MulDiv(0x00008000,snapRatio.fN,snapRatio.fD);
   long adjValue=value.GetFixed();
   bool bSign;
   if(adjValue<0)
   {
      bSign=true;
      adjValue=-adjValue;
   }
   else bSign=false;
   adjValue+=halfSnap;

   /* test for overflow conditions */
   long divMod,newValue;
   if( adjValue >= 0x7fffffffL / (long)snapRatio.fD)
   {
      divMod=adjValue/snapRatio.fN;
      divMod*=m_Snap.fSnapRatio.fD;

      newValue=(divMod & 0xffff0000)/snapRatio.fD;
      newValue*=snapRatio.fN;
   }
   else
   {
      divMod=adjValue*snapRatio.fD;	/* divide by snap */
      divMod/=snapRatio.fN;

      newValue=(divMod & 0xffff0000)*snapRatio.fN;
      newValue/=snapRatio.fD;

   }
   if(bSign) newValue=-newValue;
   value=WrFixedFromFixed(newValue);
}

void CPCDraftView::DoPlace(HANDLE hShape, bool bUnscaled)
{
   GetParentFrame()->ActivateFrame();
   if(m_hPlaceShape) GlobalFree(m_hPlaceShape);
   m_hPlaceShape=hShape;
   m_bPlaceUnscaled=bUnscaled;
   OnToolsRange(ID_TOOL_PLACE);
}

void CPCDraftView::OnClickPosition()
{
   CMenu theMenu;
   if(theMenu.LoadMenu(IDR_PCDRAFT_POPUP))
   {
      CRect rc;
      m_wndPosition.GetWindowRect(&rc);
      theMenu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,rc.right,rc.top,AfxGetMainWnd());
   }
}

void CPCDraftView::OnClickZoom()
{
   CMenu theMenu;
   theMenu.CreatePopupMenu();
   CPCDraftDoc* pDoc=GetDocument();
   for(int nZoom=-3; nZoom<=pDoc->m_nMaxZoom; nZoom++)
   {
      CString theZoomText;
      theZoomText.Format(nZoom<0?IDS_ZOOMDIVIDE:IDS_ZOOMMULTIPLY,1<<abs(nZoom));
#ifdef _FLOAT_ZOOM
      theMenu.AppendMenu(pDoc->m_nZoomFactor==nZoom?MF_STRING|MF_CHECKED:MF_STRING,100+nZoom,theZoomText);
#else
      theMenu.AppendMenu(pDoc->m_nZoomFactor==nZoom?MF_STRING|MF_CHECKED:MF_STRING,100+ nZoom,theZoomText);
#endif
   }
   CRect rc;
   m_wndZoomInfo.GetWindowRect(&rc);
   int nID=theMenu.TrackPopupMenu(TPM_NONOTIFY|TPM_RETURNCMD|TPM_LEFTBUTTON|TPM_LEFTALIGN,rc.right,rc.top,AfxGetMainWnd());
   if(nID)
   {
      // PC Draft 7
      ZoomCenter(0.0, nID - 100);

//#ifdef _FLOAT_ZOOM
//      SmartZoom(0,100-nID);
//#else
//      SmartZoom(100-nID);
//#endif
   }
}

void CPCDraftView::DoDistribute(bool bByDatum,bool bVert)
{
   ShowCursorPromptNoLock(false);
   ShowCrossCursor(false);

   BeginSaveUndo();

   LongBoundRect lrcDist;
   POSITION pos=m_Selection.GetHeadPosition();
   int count=-1;
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->IsLocked() || pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)) || (pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && IsLinkedShape(pShape,-1))) continue;
      LongRect ext=pShape->GetExtents();
      lrcDist|=bByDatum?pShape->GetShapeDatum():Middle(ext);
      count++;
   }
   LongPoint ptTo,ptFrom;
   if(bVert)
   {
      ptFrom.y=lrcDist.top;
      ptTo.y=lrcDist.bottom;
      ptTo.x=ptFrom.x=(lrcDist.left+lrcDist.right)>>1;
   }
   else
   {
      ptFrom.x=lrcDist.left;
      ptTo.x=lrcDist.right;
      ptTo.y=ptFrom.y=(lrcDist.top+lrcDist.bottom)>>1;
   }
   double distH = ( ptTo.x.GetRawDouble () - ptFrom.x.GetRawDouble () ) / count;
   double distV = ( ptTo.y.GetRawDouble () - ptFrom.y.GetRawDouble () ) / count;
   pos=m_Selection.GetHeadPosition();
   int i=0;
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->IsLocked() || pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)) || (pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && IsLinkedShape(pShape,-1))) continue;
      LongRect ext=pShape->GetExtents();
      LongPoint objPt=bByDatum?pShape->GetShapeDatum():Middle(ext);
      InvalidateShape(pShape,true);
      pShape->DoShapeOffset(LongPoint(ptFrom.x-objPt.x+WrFixedFromRawDoubleTruncated (i * distH),ptFrom.y-objPt.y+WrFixedFromRawDoubleTruncated(i*distV)));
      InvalidateShape(pShape,true);
      i++;
   }
   if(RecalcLinkedVertexes(false))
   {
      CommitSaveUndo(IDS_DISTRIBUTEONLINE);
      ForceResizeUpdate();
   }
   else
   {
      AbortSaveUndo(true);
      AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
   }

   UpdateWindow();
   ShowCrossCursor();
   ShowCursorPromptNoLock();
}

void CPCDraftView::OnAlign(UINT nID)
{
   if(nID<ID_ALIGN_DISTH_CENTER)
   {
      gSavedAlignMode=gAlignMode;
      gSavedSelMask=gSelMask;
      gSavedRefMask=gRefMask;
      gRestoreAlignState=true;
   }

   switch(nID)
   {
   case ID_ALIGN_LEFT:
      gAlignMode=alignMode_RefObj;
      gSelMask=alignMask_Left;
      gRefMask=alignMask_Left;
      OnArrangeAlignAgain();
      break;
   case ID_ALIGN_RIGHT:
      gAlignMode=alignMode_RefObj;
      gSelMask=alignMask_Right;
      gRefMask=alignMask_Right;
      OnArrangeAlignAgain();
      break;
   case ID_ALIGN_TOP:
      gAlignMode=alignMode_RefObj;
      gSelMask=alignMask_Top;
      gRefMask=alignMask_Top;
      OnArrangeAlignAgain();
      break;
   case ID_ALIGN_BOTTOM:
      gAlignMode=alignMode_RefObj;
      gSelMask=alignMask_Bottom;
      gRefMask=alignMask_Bottom;
      OnArrangeAlignAgain();
      break;
   case ID_ALIGN_VCENTER:
      gAlignMode=alignMode_RefObj;
      gSelMask=alignMask_VertCenters;
      gRefMask=alignMask_VertCenters;
      OnArrangeAlignAgain();
      break;
   case ID_ALIGN_HCENTER:
      gAlignMode=alignMode_RefObj;
      gSelMask=alignMask_HorizCenters;
      gRefMask=alignMask_HorizCenters;
      OnArrangeAlignAgain();
      break;
   case ID_ALIGN_DISTH_CENTER:
      DoDistribute(false,false);
      break;
   case ID_ALIGN_DISTV_CENTER:
      DoDistribute(false,true);
      break;
   case ID_ALIGN_DISTH_DATUM:
      DoDistribute(true,false);
      break;
   case ID_ALIGN_DISTV_DATUM:
      DoDistribute(true,true);
      break;
   }
}

void CPCDraftView::DoPan()
{
   MSG msg;
   SetCapture();
   CPoint ptOrg;
   GetCursorPos(&ptOrg);
   while(1)
   {
      GetMessage(&msg,NULL,0,0);
      switch(msg.message)
      {
      case WM_LBUTTONUP:
         ReleaseCapture();
         if(GetAsyncKeyState(VK_SPACE)>=0) CancelCurrentTool();
         return;
      case WM_MOUSEMOVE:
      {
         CSize scrollSize=ptOrg-msg.pt;
         ShowCrossCursor(false);
         OnScrollBy(scrollSize);
         ShowCrossCursor();
         ptOrg=msg.pt;
      }
      break;
      default:
         DispatchMessage(&msg);
         break;
      }
   }
}

void CPCDraftView::OnToolLink()
{
   SetOneTimeTool(ID_TOOL_LINK);
}

void CPCDraftView::OnToolUnlink()
{
   SetOneTimeTool(ID_TOOL_UNLINK);
}

bool CPCDraftView::RecalcLinkedVertex(CMDShape* pShape)
{
   bool bResult;
   TRY
   {
      CLinkVertexContext theContext;
      theContext.m_pView=this;
      theContext.m_pList=&GetActiveLinkVertexList();
      pShape->AddToLinkContext(&theContext);
      pShape->RecalcLinkedVertex(&theContext);
      bResult=true;
   }
   CATCH(CException,e)
   {
      bResult=false;
   }
   END_CATCH
   return bResult;
}

bool CPCDraftView::RecalcLinkedVertexes(bool bMove)
{
   bool bResult;
   TRY
   {
      CLinkVertexList* pList=&GetActiveLinkVertexList();
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(pShape->IsLinked(*pList,-1,NULL))
         {
            CLinkVertexContext theContext;
            theContext.m_pView=this;
            theContext.m_pList=pList;

            POSITION pos2=m_Selection.GetHeadPosition();
            POSITION endPos=bMove?NULL:pos;
            while(pos2!=endPos)
            {
               CMDShape* pShape=m_Selection.GetNext(pos2);
               //				if(!pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim))) // add all except dimensions
               pShape->AddToLinkContext(&theContext);
            }
            pShape->RecalcLinkedVertex(&theContext);
         }
      }
      bResult=true;
   }
   CATCH(CException,e)
   {
      bResult=false;
   }
   END_CATCH
   return bResult;
}

void CPCDraftView::OnToolMultiunlink()
{
   CDialog theDlg(IDD_MULTIPLE_UNLINK);
   MessageBeep(MB_ICONHAND);
   if(GetAsyncKeyState(VK_OPTION)<0 || IDOK==theDlg.DoModal())
   {
      SaveUndo(0); // clear previous undo
      CancelCurrentTool();
      CLinkVertexList* pList=&GetActiveLinkVertexList();
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
         m_Selection.GetNext(pos)->DeleteLinks(pList);
   }
}

void CPCDraftView::OnPrefToolOptions()
{
   switch(m_nCurrentTool)
   {
   case ID_TOOL_ATTRIB_DROP:
      OnEditAttribOptions();
      break;
   case ID_TOOL_PARALLEL_LINE:
   case ID_TOOL_PARALLEL_POLY:
   case ID_TOOL_PARALLEL_POLY_CLOSED:
   case ID_TOOL_PARALLEL_LINE_EXTRUDE:
   case ID_TOOL_PARALLEL_POLY_EXTRUDE:
   {
      CMDShapeList workList;
      POSITION pos=m_Selection.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=m_Selection.GetNext(pos);
         if(pShape->IsParallel() && !pShape->IsLocked()) workList.AddTail(pShape);
      }
      if(workList.GetCount())
      {
         ParallelAttrs attrs;
         pos=workList.GetHeadPosition();
         attrs=workList.GetNext(pos)->GetParallelAttrs();
         while(pos)
         {
            ParallelAttrs pa=workList.GetNext(pos)->GetParallelAttrs();
            if(attrs.m_nAlign!=pa.m_nAlign) attrs.m_nAlign=-1;
            if(attrs.m_nEndCap!=pa.m_nEndCap) attrs.m_nEndCap=-1;
            if(attrs.m_nJoint!=pa.m_nJoint) attrs.m_nJoint=-1;
            if(attrs.m_fWidth!=pa.m_fWidth) attrs.m_fWidth=-1;
         }
         CParallelLineDlg theDlg(&attrs,GetDocument());
         if(theDlg.DoModal()==IDOK)
         {
            SaveUndo(IDS_PARAATTRCHANGE);
            pos=workList.GetHeadPosition();
            while(pos)
            {
               CMDShape* pShape=workList.GetNext(pos);
               InvalidateShape(pShape,true);
               ParallelAttrs pa=pShape->GetParallelAttrs();
               if(attrs.m_nAlign!=-1) pa.m_nAlign=attrs.m_nAlign;
               if(attrs.m_nEndCap!=-1) pa.m_nEndCap=attrs.m_nEndCap;
               if(attrs.m_nJoint!=-1) pa.m_nJoint=attrs.m_nJoint;
               if(attrs.m_fWidth!=-1) pa.m_fWidth=attrs.m_fWidth;
               pShape->SetParallelAttrs(pa);
               pShape->CalculateExtents();
               InvalidateShape(pShape,true);
            }

         }
      }
      else
      {
         SaveUndo(0);
         CParallelLineDlg theDlg(&GetDocument()->m_ParallelAttrs,GetDocument());
         theDlg.DoModal();
      }
   }
   break;
   case ID_TOOL_REG_POLY_CENTER_SIDE:
   case ID_TOOL_REG_POLY_CENTER_VERTEX:
   case ID_TOOL_REG_POLY_SIDE_SIDE:
   case ID_TOOL_REG_POLY_VERTEX_VERTEX:
   {
      CRegularPolyDlg theDlg;
      theDlg.m_nSides=m_nRegPolySides;
      if(theDlg.DoModal()==IDOK)
         m_nRegPolySides=theDlg.m_nSides;
   }
   break;
   // Star Tool -  Infinisys Ltd
   case ID_TOOL_STAR_DIAMETER:
   case ID_TOOL_STAR_CENTER_VERTEX:
   {
      CStarDialog theDlg;
      theDlg.m_nSides=m_nStarSides;
      theDlg.m_iThickness = m_iStarThickness;
      if(theDlg.DoModal() == IDOK)
      {
         m_nStarSides=theDlg.m_nSides;
         m_iStarThickness=theDlg.m_iThickness;
      }
   }
   break;
   }
}

void CPCDraftView::OnUpdatePrefToolOptions(CCmdUI* pCmdUI)
{
   BOOL bEnable;
   switch(m_nCurrentTool)
   {
   case ID_TOOL_ATTRIB_DROP:
   case ID_TOOL_PARALLEL_LINE:
   case ID_TOOL_PARALLEL_POLY:
   case ID_TOOL_PARALLEL_POLY_CLOSED:
   case ID_TOOL_PARALLEL_LINE_EXTRUDE:
   case ID_TOOL_PARALLEL_POLY_EXTRUDE:
   case ID_TOOL_REG_POLY_CENTER_SIDE:
   case ID_TOOL_REG_POLY_CENTER_VERTEX:
   case ID_TOOL_REG_POLY_SIDE_SIDE:
   case ID_TOOL_REG_POLY_VERTEX_VERTEX:
   case ID_TOOL_STAR_DIAMETER:       // Star Tool -  Infinisys Ltd
   case ID_TOOL_STAR_CENTER_VERTEX:       // Star Tool -  Infinisys Ltd
      bEnable=TRUE;
      break;
   default:
      bEnable=FALSE;
   }
   pCmdUI->Enable(bEnable);
}

void CPCDraftView::ScaleSelection(double nX,double nY,bool bContract)
{
   BeginSaveUndo();
   LongBoundRect lrcFrom;
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked())
         lrcFrom|=pShape->GetExtents();
   }
   LongPoint ptCenter=Middle(lrcFrom);
   LongRect lrcTo=lrcFrom;
   lrcTo.Offset(-ptCenter.x,-ptCenter.y);
   if(bContract)
   {
      lrcTo.left/=nX;
      lrcTo.right/=nX;
      lrcTo.top/=nY;
      lrcTo.bottom/=nY;
   }
   else
   {
      lrcTo.left*=nX;
      lrcTo.right*=nX;
      lrcTo.top*=nY;
      lrcTo.bottom*=nY;
   }
   lrcTo.Offset(ptCenter.x,ptCenter.y);

   CPCDraftDoc* pDoc=GetDocument();
   CLayer* pLayer=pDoc->GetActiveLayer();
   pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      POSITION opos=pos;
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(!pShape->IsLocked())
      {
         CMDShape* pNewShape=NULL;
         InvalidateShape(pShape,true);
         pShape->DoShapeScale(lrcFrom,lrcTo,&pNewShape);
         pShape->ProcessDeletedShapes(&pLayer->m_LinkVertexList);
         if(pNewShape)
         {
            m_Selection.InsertAfter(opos,pNewShape);
            m_Selection.RemoveAt(opos);
            pLayer->ReplaceShape(pShape,pNewShape);
            pNewShape->UpdateDependentShapes(this);
            InvalidateShape(pNewShape,true);
         }
         else
         {
            pShape->UpdateDependentShapes(this);
            InvalidateShape(pShape,true);
         }
      }
   }

   UINT strId;
   if(RecalcLinkedVertexes())
   {
      if(pDoc->CheckDocSize(false))
      {
         CommitSaveUndo(bContract?IDS_CONTRACT:IDS_EXPAND);
         ForceResizeUpdate();
         pDoc->SetModifiedFlag();
         Invalidate(FALSE);
         return;
      }
      strId=IDS_CANT_RESIZE;
   }
   else strId=IDS_UNABLE_COMPLETE_ACTION;
   AbortSaveUndo(true);
   AfxMessageBox(strId);
}

void CPCDraftView::OnEditContract()
{
   CContractExpandDlg theDlg;
   theDlg.m_bContract=true;
   if(theDlg.DoModal()==IDOK)
      ScaleSelection(theDlg.m_nX,theDlg.m_nY,theDlg.m_bContract);
}

void CPCDraftView::OnEditExpand()
{
   CContractExpandDlg theDlg;
   theDlg.m_bContract=false;
   if(theDlg.DoModal()==IDOK && theDlg.m_nX && theDlg.m_nY)
      ScaleSelection(theDlg.m_nX,theDlg.m_nY,theDlg.m_bContract);
}

bool CPCDraftView::SelectLayerFromPoint(const CPoint &pt)
{
   CPCDraftDoc* pDoc=GetDocument();
   for(int i=0; i<pDoc->GetLayersCount(); i++)
   {
      CLayer* pLayer=pDoc->GetLayer(i);
      if(!pLayer->m_bHidden && pLayer->ShapeFromPoint(pt,&m_Mapper,pDoc->m_bHideText))
      {
         if(pDoc->SetActiveLayer(i))
         {
            pDoc->LayersChanged();
            return true;
         }
         break;
      }
   }
   return false;
}


void CPCDraftView::OnUpdateFileExportFile(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(((CPCDraftApp*)AfxGetApp())->CanExportFile());
}

void CPCDraftView::OnFileExportFile()
{
   //CPCDraftDoc *pDoc = GetDocument();
   //if(pDoc)
   //{
      //pDoc->ExportQT();
     // pDoc->ExportImage(); // Infinisys Ltd.
   ExportImage(); // Infinisys Ltd. 2021

   //}
}

void CPCDraftView::OnPrefPenstyleBlade()
{
   SaveUndo(IDS_PENSTYLE);
   PenModel pm;
   pm.makePenStyleBlade();
   ApplyPen(pm,pmUsePenStyle);
}

void CPCDraftView::OnUpdatePrefPenstyleBlade(CCmdUI* pCmdUI)
{
   PenModel pm;
   switch(m_Selection.GetCount())
   {
   case 0:
      pm=GetDocument()->m_Pen;
      break;
   case 1:
      pm=(m_Selection.GetHead())->GetPenModel();
      break;
   default:
   {
      CMDShape* pShape = m_Selection.GetHead();
      bool bIsRound = pShape->GetPenModel().isPenStyleRound();


      POSITION pos = m_Selection.GetHeadPosition();
      while (pos != NULL)
      {
         CMDShape* pShape = m_Selection.GetNext(pos);
         if (pShape->GetPenModel().isPenStyleRound() != bIsRound)
         {
            // There are shapes of different types
            pCmdUI->SetCheck(!GetDocument()->m_Pen.isPenStyleRound());
            return;
         }
      }

      // All have the same type
      pCmdUI->SetCheck(!bIsRound);
      return;
   }
   }
   pCmdUI->SetCheck(!pm.isPenStyleRound());
}

void CPCDraftView::OnPrefPenstyleRound()
{
   SaveUndo(IDS_PENSTYLE);
   PenModel pm;
   pm.makePenStyleRound();
   ApplyPen(pm,pmUsePenStyle);
}

void CPCDraftView::OnUpdatePrefPenstyleRound(CCmdUI* pCmdUI)
{
   PenModel pm;
   switch(m_Selection.GetCount())
   {
   case 0:
      pm=GetDocument()->m_Pen;
      break;
   case 1:
      pm=(m_Selection.GetHead())->GetPenModel();
      break;
   default:
   {
      CMDShape* pShape = m_Selection.GetHead();
      bool bIsRound = pShape->GetPenModel().isPenStyleRound();


      POSITION pos = m_Selection.GetHeadPosition();
      while (pos != NULL)
      {
         CMDShape* pShape = m_Selection.GetNext(pos);
         if (bIsRound != pShape->GetPenModel().isPenStyleRound())
         {
            // There are shapes of different types
            pCmdUI->SetCheck(GetDocument()->m_Pen.isPenStyleRound());
            return;
         }
      }

      // All have the same type
      pCmdUI->SetCheck(bIsRound);
      return;
   }
   }
   pCmdUI->SetCheck(pm.isPenStyleRound());
}

void CPCDraftView::LinkLineVertexes(CMDShape* pDimShape,int nDimVertex,CEdge* pEdge, bool bEdgeSwap)
{
#ifndef _PERSONAL_EDITION
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   int nLineVertex1,nLineVertex2;
   CMDShape* pShape=pLayer->ShapeFromEdge(pEdge,nLineVertex1,nLineVertex2);
   if(pShape)
   {
      if(guAlmostEqualPt(pDimShape->GetShapePoint(nDimVertex),pShape->GetShapePoint(bEdgeSwap?nLineVertex1:nLineVertex2)))
      {
         int nTemp=nLineVertex1;
         nLineVertex1=nLineVertex2;
         nLineVertex2=nTemp;
      }
      long nKey=-pLayer->GetVertexKey();
      CLinkVertex* pLinkVertex=new CLinkVertex;
      pLinkVertex->m_nKey=nKey;
      pLinkVertex->m_pShape=pDimShape;
      pLinkVertex->m_nVertex=nDimVertex;
      pLinkVertex->m_bResolved=true;
      pLayer->m_LinkVertexList.AddTail(pLinkVertex);
      pLinkVertex=new CLinkVertex;
      pLinkVertex->m_nKey=nKey;
      pLinkVertex->m_pShape=pShape;
      pLinkVertex->m_nVertex=bEdgeSwap?nLineVertex2:nLineVertex1;
      pLinkVertex->m_bResolved=true;
      pLayer->m_LinkVertexList.AddTail(pLinkVertex);
      pLinkVertex=new CLinkVertex;
      pLinkVertex->m_nKey=nKey;
      pLinkVertex->m_pShape=pShape;
      pLinkVertex->m_nVertex=bEdgeSwap?nLineVertex1:nLineVertex2;
      pLinkVertex->m_bResolved=true;
      pLayer->m_LinkVertexList.AddTail(pLinkVertex);
   }
#endif
}

void CPCDraftView::LinkCenterVertexes(CMDShape* pShape,CMDShape* pDimShape)
{
#ifndef _PERSONAL_EDITION
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   long nKey=pLayer->GetVertexKey();
   CLinkVertex* pLinkVertex=new CLinkVertex;
   pLinkVertex->m_nKey=nKey;
   pLinkVertex->m_pShape=pShape;
   pLinkVertex->m_nVertex=kCenterHandle;
   pLinkVertex->m_bResolved=true;
   pLayer->m_LinkVertexList.AddTail(pLinkVertex);
   pLinkVertex=new CLinkVertex;
   pLinkVertex->m_nKey=nKey;
   pLinkVertex->m_pShape=pDimShape;
   pLinkVertex->m_nVertex=kCenterHandle;
   pLinkVertex->m_bResolved=true;
   pLayer->m_LinkVertexList.AddTail(pLinkVertex);
#endif
}

void CPCDraftView::LinkVertexes(const CPoint &pt, bool bLink, bool bWarn)
{
#ifndef _PERSONAL_EDITION
   CArray<CMDShape*,CMDShape*> theShapes;
   CArray<int,int> theHandles;
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   POSITION pos=pLayer->m_ShapeList.GetTailPosition();
   while(pos)
   {
      CMDShape* pShape=pLayer->m_ShapeList.GetPrev(pos);
      if(pShape->IsKindOf(RUNTIME_CLASS(CMDAngularDim))) continue;
      CUIntArray vertexes;
      CMDShapeArray shapes;
      pShape->GetVertexesFromPt(vertexes,shapes,pt,&m_Mapper);
      for(int i=0; i<vertexes.GetSize(); i++)
      {
         int nHandle=vertexes.GetAt(i);
         CMDShape* pShape=shapes.GetAt(i);
         pLayer->m_LinkVertexList.DeleteAllByShapeAndVertex(pShape,nHandle,false);
         theShapes.Add(pShape);
         theHandles.Add(nHandle);
      }
   }
   if(theShapes.GetSize()>1)
   {
      if(bLink)
      {
         long nKey=pLayer->GetVertexKey();
         for(int i=theShapes.GetSize(); --i>=0;)
         {
            CLinkVertex* pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=nKey;
            pLinkVertex->m_pShape=theShapes.GetAt(i);
            pLinkVertex->m_nVertex=theHandles.GetAt(i);
            pLinkVertex->m_bResolved=true;
            pLayer->m_LinkVertexList.AddTail(pLinkVertex);
         }
      }
   }
   else if(bWarn) MessageBeep(MB_ICONHAND);
#endif
}

void CPCDraftView::OnUpdateToolRotate(CCmdUI* pCmdUI)
{
   bool bEnable=HaveUnlockedSelection();
   if(!bEnable && m_nCurrentTool==ID_TOOL_ROTATE) SetArrowTool();
   pCmdUI->Enable(bEnable && m_nCurrentTool!=ID_TOOL_EDIT_DATUM);
   pCmdUI->SetRadio(m_nCurrentTool==ID_TOOL_ROTATE);
}

void CPCDraftView::OnToolRotate()
{
   if(GetAsyncKeyState(VK_OPTION)<0) OnArrangeRotateOpt();
   else OnToolsRange(ID_TOOL_ROTATE);
}

void CPCDraftView::GetSortedSelection(CMDShapeList &sortedList,bool bRemoveLocked)
{
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   CMDShapeList workList;
   workList.AddTail(&m_Selection);
   pLayer->SortShapes(workList,bRemoveLocked);
   sortedList.RemoveAll();
   sortedList.AddTail(&workList);
}

bool CPCDraftView::IsLinkedShape(const CMDShape *pShape, int nLinkVertex, CMDShape** pLinkedShape)
{
   CLayer* pLayer=GetDocument()->GetActiveLayer();
   return pLayer->m_LinkVertexList.IsLinkedShape(pShape,nLinkVertex,pLinkedShape);
}

void CPCDraftView::UnlockSelection()
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->IsLocked())
      {
         pShape->SetLocked(false);
         InvalidateShape(pShape,true);
      }
   }
}

void CPCDraftView::UpdateCursor()
{
   CPoint pt;
   GetCursorPos(&pt);
   if(WindowFromPoint(pt)==this)
   {
      OnSetCursor(this,HTCLIENT,WM_MOUSEMOVE); // is this correct?

      ScreenToClient(&pt);
      AdjustCursor(pt,false);
   }
}

BOOL CPCDraftView::CanReshapeSmooth()
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->CanSmooth() && !pShape->IsLocked())
         return TRUE;
   }
   return FALSE;
}

BOOL CPCDraftView::CanReshapeUnsmooth()
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pShape=m_Selection.GetNext(pos);
      if(pShape->CanUnsmooth() && !pShape->IsLocked())
         return TRUE;
   }
   return FALSE;
}

void CPCDraftView::DoChamfer()
{
   BeginSaveUndo();
   CPCDraftDoc* pDoc=GetDocument();
   InvalidateShape(m_pChamferCtx->m_pLines[0],true);
   InvalidateShape(m_pChamferCtx->m_pLines[1],true);
   CMDLineShape* pChamfer=m_pChamferCtx->CreateChamfer();
   if(pChamfer)
   {
      CLinkVertexList* pList=&GetActiveLinkVertexList();
      m_pChamferCtx->m_pLines[0]->DeleteLinks(pList);
      m_pChamferCtx->m_pLines[1]->DeleteLinks(pList);

      pDoc->CreateShape(pChamfer);
      SelectShape(pChamfer,true);
      if(m_pChamferCtx->m_pNewLines[0])
      {
         pDoc->CreateShape(m_pChamferCtx->m_pNewLines[0]);
         SelectShape(m_pChamferCtx->m_pNewLines[0],true);
      }
      else InvalidateShape(m_pChamferCtx->m_pLines[0],true);
      if(m_pChamferCtx->m_pNewLines[1])
      {
         pDoc->CreateShape(m_pChamferCtx->m_pNewLines[1]);
         SelectShape(m_pChamferCtx->m_pNewLines[1],true);
      }
      else InvalidateShape(m_pChamferCtx->m_pLines[1],true);
   }
   if(pDoc->CheckDocSize(false)) CommitSaveUndo(IDS_CHAMFER);
   else
   {
      AbortSaveUndo();
      AfxMessageBox(IDS_NO_COMMON_INTERSECTION);
   }
}

void CPCDraftView::OnUpdateDisableIfEditDatum(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_nCurrentTool!=ID_TOOL_EDIT_DATUM);
}

void CPCDraftView::OnUpdateFilePrintSlides(CCmdUI* pCmdUI)
{
#ifdef	_DEMO
   pCmdUI->Enable(FALSE);
#else
   pCmdUI->Enable(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode);
#endif
}

void CPCDraftView::OnUpdateFilePrintActiveLayer(CCmdUI* pCmdUI)
{
#ifdef	_DEMO
   pCmdUI->Enable(FALSE);
#else
   pCmdUI->Enable(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode);
#endif
}

void CPCDraftView::OnUpdateFilePrint(CCmdUI* pCmdUI)
{
#ifdef	_DEMO
   pCmdUI->Enable(FALSE);
#else
   pCmdUI->Enable(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode);
#endif
}

void CPCDraftView::OnFilePrint()
{
#ifndef	_DEMO
   if(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode)
      CView::OnFilePrint();
#endif
}

void CPCDraftView::UpdateZoomInfo()
{
   CPCDraftDoc* pDoc=GetDocument();
   CString theZoomText;
#ifdef _FLOAT_ZOOM
   if(pDoc->m_fZoomFactor)
   {
      if(pDoc->m_fZoomFactor>1)
         theZoomText.Format("%.1f",pDoc->m_fZoomFactor);
      else
         theZoomText.Format("-%.1f",1/pDoc->m_fZoomFactor);
   }
   else
#endif
   {
      theZoomText.Format("%d",1<<abs(pDoc->m_nZoomFactor));
      if(pDoc->m_nZoomFactor<0) theZoomText="-"+theZoomText;
   }
   CString strSymbol;
   strSymbol.LoadString(IDS_ZOOMSYMBOL);
   theZoomText+=strSymbol;
   AfxSetWindowText(m_wndZoomInfo.GetSafeHwnd(),theZoomText);
}

bool CPCDraftView::CanMoveDimension(CMDShape *pShape)
{
   CMDShape* pLinkedShape;
   return (!IsLinkedShape(pShape,-1,&pLinkedShape) || IsSelectedShape(pLinkedShape,true));
}

void CPCDraftView::OnUpdateAngleSnapRange(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(GetDocument()->m_snapTable.getSelectedSnap()==int(pCmdUI->m_nID-ID_ANGLE_SNAP_BASE-1));
}

void CPCDraftView::OnAngleSnapRange(UINT nID)
{
   GetDocument()->m_snapTable.selectSnap(nID-ID_ANGLE_SNAP_BASE-1);
}

bool CPCDraftView::IsSelectedShape(CMDShape *pShape, bool bInsideGroups)
{
   POSITION pos=m_Selection.GetHeadPosition();
   while(pos)
   {
      CMDShape* pCurrentShape=m_Selection.GetNext(pos);
      if(bInsideGroups && pCurrentShape->IsKindOf(RUNTIME_CLASS(CMDGroupShape)))
      {
         CMDGroupShape* pGroup=(CMDGroupShape*)pCurrentShape;
         if(pGroup->IsMember(pShape,true)) return true;
      }
      else
      {
         if(pShape==pCurrentShape) return true;
      }
   }
   return false;
}

CLinkVertexList& CPCDraftView::GetActiveLinkVertexList()
{
   return GetDocument()->GetActiveLayer()->m_LinkVertexList;
}

void CPCDraftView::CommonChar()
{
   CPCDraftDoc* pDoc=GetDocument();
   CMDShape* pShape=m_Selection.GetHead();
   if(pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
   {
      if(pShape->IsLocked()) return;
      OnToolsRange(ID_TOOL_TEXT);
      ActivateTextShape((CMDText*)pShape);
      m_InplaceEdit.GoEnd();
   }
   else
   {
      SaveUndo(IDS_CREATION);
      CMDText* pTextShape;
      if(pShape->IsKindOf(RUNTIME_CLASS(CMDRectangle)))
      {
         pTextShape=new CMDText(CMDText::kTextUserSize);
         for(int i=0; i<4; i++) pTextShape->SetShapePoint(i,pShape->GetShapePoint(i));
         pTextShape->SetRotation(pShape->GetRotation());
      }
      else
      {
         LongRect lrc=pShape->GetExtents();
         if(Height(lrc)<12 || Width(lrc)<1)
         {
            pTextShape=new CMDText(CMDText::kTextAutoSize);
            for(int i=0; i<4; i++) pTextShape->SetShapePoint(i,lrc.GetCorner(0));
         }
         else
         {
            pTextShape=new CMDText(CMDText::kTextUserSize);
            for(int i=0; i<4; i++) pTextShape->SetShapePoint(i,lrc.GetCorner(i));
         }
      }
      pTextShape->SetPenModel(pDoc->m_Pen);
      pTextShape->SetTextParams(pDoc->m_TextParams);
      pTextShape->CalculateExtents();
      pTextShape->SetShapeDatum(kDefaultDatum);
      GetDocument()->CreateShape(pTextShape);
      SelectShape(pTextShape,false);
#ifndef _PERSONAL_EDITION
      if(pShape->IsKindOf(RUNTIME_CLASS(CMDRectangle)))
      {
         CLayer* pLayer=pDoc->GetActiveLayer();
         for(int i=0; i<4; i++)
         {
            long nKey=pLayer->GetVertexKey();
            CLinkVertex* pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=nKey;
            pLinkVertex->m_pShape=pTextShape;
            pLinkVertex->m_nVertex=i;
            pLinkVertex->m_bResolved=true;
            pLayer->m_LinkVertexList.AddTail(pLinkVertex);
            pLinkVertex=new CLinkVertex;
            pLinkVertex->m_nKey=nKey;
            pLinkVertex->m_pShape=pShape;
            pLinkVertex->m_nVertex=i;
            pLinkVertex->m_bResolved=true;
            pLayer->m_LinkVertexList.AddTail(pLinkVertex);
         }
      }
#endif
      OnToolsRange(ID_TOOL_TEXT);
      ActivateTextShape(pTextShape);
      m_InplaceEdit.GoEnd();
   }
}

// Gradient/Transparency Tool -  Infinisys Ltd
void CPCDraftView::OnChangeSelection()
{

   //POSITION pos=m_Selection.GetHeadPosition();

   //int iFillOpacityCount = 0;
   //
   //int iOpacity;
   //
   //int iFillOpacity = -1;

   //while(pos)
   //{

   //   CMDShape* pShape=m_Selection.GetNext(pos);

   //   if(pShape->GetFillOpacity(iOpacity))
   //   {
   //
   //      if(iFillOpacityCount == 0)
   //      {

   //         iFillOpacity = iOpacity;

   //      }
   //      else
   //      {

   //         if(iFillOpacity != iOpacity)
   //         {
   //
   //            iFillOpacity = -1;

   //            break;

   //         }

   //      }

   //      iFillOpacityCount++;

   //   }

   //}
   BOOL bFromSelection;

   if(m_Selection.GetCount() == 0)
   {
      bFromSelection = FALSE;
   }
   else
   {
      CMDShape *pShape = m_Selection.GetHead();
      const FillModel& fmTmp = pShape->GetFillModel();

      POSITION pos = m_Selection.GetHeadPosition();
      bFromSelection = TRUE;
      while(pos != NULL)
      {
         pShape = m_Selection.GetNext(pos);
         const FillModel& fmTmpNext = pShape->GetFillModel();
         if(fmTmpNext.m_nFillType != fmTmp.m_nFillType || fmTmpNext.m_nIndex != fmTmp.m_nIndex)
         {
            bFromSelection = FALSE;
            break;
         }
      }
   }

   

      const FillModel& fm = bFromSelection ? m_Selection.GetTail()->GetFillModel():GetDocument()->m_Fill;
   CMainFrame * pframe = (CMainFrame*)GetTopLevelFrame(); // Interface Update - Infinisys Ltd.
   if(fm.m_nFillType == kNoFill)
   {
      pframe->m_RibbonBar.m_properties.m_wndFill.SendMessage(UM_ATTRIBSETTINGS_UPDATE,0,NULL);
   }
   else
   {
      CBitmap bmp;
      CRect r;
      pframe->m_RibbonBar.m_properties.m_wndFill.GetClientRect(r);
      CreateFillModelBitmap(fm,bmp,r.Size());
      pframe->m_RibbonBar.m_properties.m_wndFill.SendMessage(UM_ATTRIBSETTINGS_UPDATE,1,(LPARAM)bmp.Detach());
   }

   //if(m_Selection.GetCount() > 0 && pframe->m_RibbonBar.m_iTabSel != 8)
   //{

   //   pframe->m_RibbonBar.set_cur_tab(9);


   //}

   //if(iFillOpacity < 0)
   //{

   //   ((CMainFrame*)AfxGetMainWnd())->m_wndAttribPalette.m_editFill.SetWindowTextA("");

   //}
   //else
   //{

   //   CString strFormat;

   //   strFormat.Format("%d", (int) (iFillOpacity * 100.0 / 255.0));

   //   ((CMainFrame*)AfxGetMainWnd())->m_wndAttribPalette.m_editFill.SetWindowTextA(strFormat);

   //}

}




LRESULT CPCDraftView::OnDropLibraryItem(WPARAM wparam,LPARAM lparam)
{

   CPoint pt((LPARAM)wparam);
   CLibraryItem * pitem = (CLibraryItem *)lparam;
   pitem->load_bk();
   bool shownTextRescaleMessage = false;
   bool rescaleTextShapes = false;
   if(pitem->m_pphoto != NULL)
   {


      LongPoint lptOffset=m_Mapper.DPtoLP(pt + GetDeviceScrollPosition());


      HBITMAP hDib = CreateDIB(pitem->m_pphoto->m_pbitmap);
      BITMAP bm;
      GetObject(hDib,sizeof(BITMAP),&bm);
      CRect rc(0,0,bm.bmWidth,bm.bmHeight);
      LongRect lrc(0,0,(int)bm.bmWidth,(int)bm.bmHeight); //m_Mapper.DPtoLP(rc.right),m_Mapper.DPtoLP(rc.bottom));
      lrc.Offset(lptOffset);
      CMDBitmap* pShape=new CMDBitmap(rc,lrc,hDib);
      pShape->CalculateExtents();
      pShape->SetShapeDatum(kDefaultDatum);
      GetDocument()->CreateShape(pShape);
      SelectShape(pShape,false);
      GetDocument()->CheckDocSize(true);



   }
   else
   {

      LongPoint lptOffset=m_Mapper.DPtoLP(pt + GetDeviceScrollPosition());
      CPCDraftDoc* pDoc=pitem->m_pdoc;
      int nDisplayRes = pDoc->m_nDisplayRes;
      int nScale = pDoc->m_Layers[0]->m_nScale;

      LongRatio changeRatio;
      if(GetKeyState(VK_OPTION) & 0x8000)
      {
         changeRatio.fN=1;
         changeRatio.fD=1;
      }
      else
      {
         LongRatio srcRatio;
         pDoc->GetScaleRatio(nDisplayRes != kMetRes,nScale,srcRatio);
         LongRatio dstRatio;
         GetDocument()->GetActiveScaleRatio(dstRatio);

         changeRatio.fN=dstRatio.fN*srcRatio.fD;
         changeRatio.fD=dstRatio.fD*srcRatio.fN;
         long theGCD=gcd(changeRatio.fN,changeRatio.fD);
         changeRatio.fN/=theGCD;
         changeRatio.fD/=theGCD;
      }
      double resChange=GetDocument()->CalcResChangeFactor(nDisplayRes);
      if(resChange != 0) resChange=1 / resChange;


      CLayer * player = pitem->m_pdoc->m_Layers[0];
      POSITION pos = player->m_ShapeList.GetHeadPosition();
      while(pos != NULL)
      {
         CMDShape* pShape=player->m_ShapeList.GetNext(pos)->CreateCopy();
         // by TSM - applying new patterns to shapes
         pShape->ProcessShapesFromLibraryArchiving(GetDocument(),pitem->m_pdoc);
         if(!pShape->IsKindOf(RUNTIME_CLASS(CMDText)))
         {
            CString stra[5];
            pShape->GetValues(stra);
            CString str = pitem->m_strName;
            CStringW wstr = get_uni(str);
            str = get_cp(wstr,CP_ACP);
            stra[0] = str;
            pShape->SetValues(stra);
            pShape->DoShapeScale(changeRatio,resChange);
         }
         else
         {
            if(changeRatio.fD != 1.0 ||
               changeRatio.fN != 1.0)
            {
               if(!shownTextRescaleMessage)
               {
                  rescaleTextShapes= (IDYES == AfxMessageBox(IDS_INSERT_TEXT_WITH_DIFFERENT_SCALE,MB_YESNO | MB_DEFBUTTON2));
                  shownTextRescaleMessage=true;
               }
               if(rescaleTextShapes)
               {
                  pShape->DoShapeScale(changeRatio,resChange);
               }
            }
            else
            {
               pShape->DoShapeScale(changeRatio,resChange);
            }
         }

         GetDocument()->CreateShape(pShape);
         //CMDDimStr* pAttached=pShape->Detach();
         //if(pAttached)
         //{
         //   pAttached->DoShapeScale(changeRatio,resChange);
         //   pAttached->DoShapeOffset(offset);
         //   pAttached->SetLocked(false);
         //   pDoc->CreateShape(pAttached);
         //   pShape->Attach(pAttached);
         //}

         pShape->SetLocked(false);
         pShape->UpdateDependentShapes(this);
         pShape->CalculateExtents();
         pShape->DoShapeOffset(lptOffset);
         SelectShape(pShape,true);
      }

      GetDocument()->CheckDocSize(true);

      delete pitem;

      RedrawWindow();

      SetFocus();

      GetTopLevelFrame()->SetActiveWindow();

   }

   return 0;

}



void CPCDraftView::OnDestroy()
{

   m_dragdrop.Revoke();

   CView::OnDestroy();

   // TODO: Add your message handler code here
}


// PC Draft 7 - Auto Save
void CPCDraftView::OnTimer(UINT_PTR nIDEvent)
{
   CView::OnTimer(nIDEvent);

   if (nIDEvent == TIMER_ONE_SECOND)
   {

      CPCDraftApp* papp = (CPCDraftApp*)AfxGetApp();

      if (m_iAutoSaveMinutes != papp->m_iAutoSaveMinutes)
      {

         m_iAutoSaveMinutes = papp->m_iAutoSaveMinutes;

      }
      
      if (m_bAutoSave != papp->m_bAutoSave)
      {

         if (m_bAutoSave)
         {

            m_timeLastAutoSave = CTime::GetCurrentTime();

         }

         m_bAutoSave = papp->m_bAutoSave;

      }

      if (m_bAutoSave)
      {

         CTimeSpan span = CTime::GetCurrentTime() - m_timeLastAutoSave;

         if (m_iAutoSaveMinutes <= 0)
         {

            m_iAutoSaveMinutes = 1;

         }

#if AUTO_SAVE_DEBUG
         if (span.GetTotalSeconds() >= 15 && !m_bAutoSaving)
#else
         if (span.GetTotalMinutes() >= m_iAutoSaveMinutes && !m_bAutoSaving)
#endif
         {

            if (GetDocument()->IsModified())
            {
               m_bAutoSaving = true;

               try
               {

                  SendMessage(WM_COMMAND, ID_FILE_SAVE);

               }
               catch (...)
               {


               }

               m_bAutoSaving = false;

            }

            m_timeLastAutoSave = CTime::GetCurrentTime();



         }


      }

   }
}


void CPCDraftView::OnGuidesAddGuideAtPosition()
{
   CGuide guide;
   AddGuideByPositionDlg dialog(GetDocument(), guide);
   if (dialog.DoModal() == IDOK)
   {

      GetDocument()->m_guidea.Add(guide);

   }

   RedrawWindow();
}


void CPCDraftView::OnGuidesAddhorizontalguide()
{

   CGuide guide;

   guide.m_bVertical = false;

   guide.m_coord = GetDocument()->m_PageSize.cy*GetDocument()->m_NumPage.y / 2;

   GetDocument()->m_guidea.Add(guide);

   RedrawWindow();

}


void CPCDraftView::get_text_composition_area(LPRECT lprect)
{

   CRect r;

   m_InplaceEdit.GetBoundRect(r);

   r.top = m_InplaceEdit.m_rectCaret.top;
   r.bottom = m_InplaceEdit.m_rectCaret.bottom;
   r.OffsetRect(m_InplaceEdit.m_rectCaret.left - r.left, 0);
   //CPoint ptOffset;
   //GetClientCorner(ptOffset);

   //CRect rectView;

   //GetActiveClientRect(rectView);

   //r.OffsetRect(rectView.TopLeft());

   //r.OffsetRect(kLRulerWid, kLRulerWid);
   //CPoint ptOrg = GetDeviceScrollPosition();

   //r.OffsetRect(ptOrg);

   //ClientToScreen(r);

   *lprect = r;

}


void CPCDraftView::on_text_composition(CStringW wstr, enum_composition_state ecompositionstate)
{
   if (m_InplaceEdit.m_bImeComposition)
   {
      m_InplaceEdit.RestoreState(&m_ime);
   }
   else
   {
      m_ime.SetLength(0);
      m_InplaceEdit.SaveState(&m_ime);
      m_InplaceEdit.m_bImeComposition = true;
   }
   CString str;
   str = get_cp(wstr, CP_ACP);
   m_InplaceEdit.ReplaceSel(str, ecompositionstate);
    
   if (m_InplaceEdit.m_bImeComposition)
   {

      RedrawWindow(nullptr);
   }
}

void CPCDraftView::on_text_composition_done()
{
   m_InplaceEdit.m_bImeComposition = false;
   m_InplaceEdit.m_ecompositionstate = e_composition_state_none;
   imm_client::on_text_composition_done();
}

void CPCDraftView::OnGuidesAddverticalguide()
{
   
   CGuide guide;

   guide.m_bVertical = true;

   guide.m_coord = GetDocument()->m_PageSize.cx*GetDocument()->m_NumPage.x / 2;

   GetDocument()->m_guidea.Add(guide);

   RedrawWindow();

}


void CPCDraftView::on_edit_undo()
{

   m_InplaceEdit.DoUndoRedo();

}
