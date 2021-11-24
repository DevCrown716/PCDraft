// PCDraftDoc.cpp : implementation of the CPCDraftDoc class
//

#include "stdafx.h"

#include "PCDraft.h"
#include "PCDraftView.h"
#include "DraftContext.h"

#include "Welcome/TemplateWnd.h"
#include "Welcome/TemplateRecentWnd.h"
#include "Welcome/recent_file_list.h"
#include "Welcome/FileDialogExW.h"

#include "ChildFrm.h"
#include "ReportEditFrame.h"
#include "ReportPreFrame.h"
#include "PCDraftDoc.h"
#include "Layer.h"
#include "MDReport.h"
#include "SavedView.h"
#include "ObjectInfoDlg.h"
#include "RotateOptionsDlg.h"
#include "EditLineStyleDlg.h"
#include "EditPatternDlg.h"
#include "ChooseColorDlg.h"
#include "ReportFormatsDlg.h"
#include "ReportOutputDlg.h"
#include "ReportEditView.h"
#include "ReportPreView.h"

#include "MDLineShape.h"
#include "MDRectangle.h"
#include "MDRndRectangle.h"
#include "MDEllipse.h"
#include "MDCircle.h"
#include "MDArcShape.h"
#include "MDBezierShape.h"
#include "MDCubicShape.h"
#include "MDText.h"

#include "SlideShowFrame.h"
#include "SlideShowView.h"

#include "gu.h"
#include "phmath.h"
#include "DXF.h"
#include "DXFUtil.h"
#include "DSShape.h"

#include "RenameDlg.h"
#include "ProgressDlg.h"
#include "GradientDlg.h"
#include "GradientPack.h"

#include <shlwapi.h>
#include <math.h>

//#ifndef _DEMO
//namespace QTML
//{
//#include <QTML.h>
//#include <QuickTimeComponents.h>
//#include "MacGWorld.h"
//}
//#endif


#include "FileDialogEx.h"
void __copy(Gdiplus::RectF & r, LPCRECT lpcrect);
HBITMAP CreateDIB(int x,int y,COLORREF * pdata,int stride,COLORREF ** p = NULL); // Interface Update - Infinisys Ltd.
HBITMAP CreateDIB(Gdiplus::Bitmap * pbitmap, bool bSwap = true); // Interface Update - Infinisys Ltd.
LPWSTR ConvertString(const CString instr);
int GetEncoderClsid(const WCHAR* format,CLSID* pClsid);
CString get_folder_from_path(CString strPath);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define kMaxCoordinate        0x0fff

#define DashingMask        0x0f00
#define kArcMidPt    4     /* OpCode used in setFdPtToArc() */
#define midPt(src1, src2, dst)   \
         (((dst)->x)) = ((src1)->x + (src2)->x) >> 1, \
         (((dst)->y)) = ((src1)->y + (src2)->y) >> 1
#define  MD70_DEFAULT 1

/* Default Palette */
struct
{
   WORD palVersion;
   WORD palNumEntries;
   PALETTEENTRY palEntries[83];
} defaultPalette =
{
   0x300,   83,
   {
      { 255, 255, 255, 0},
      {   0,   0,   0, 0},
      { 127, 127, 127, 0},
      { 221,   8,   6, 0},
      {   0, 128,  17, 0},
      {   0,   0, 212, 0},
      {   2, 171, 234, 0},
      { 242,   8, 132, 0},
      { 252, 243,   5, 0},
      { 255,  27,   0, 0},
      { 255,  55,   0, 0},
      { 255,  83,   0, 0},
      { 255, 111,   0, 0},
      { 255, 139,   0, 0},
      { 255, 167,   0, 0},
      { 255, 195,   0, 0},
      { 255, 223,   0, 0},
      { 255, 251,   0, 0},
      { 232, 255,   0, 0},
      { 204, 255,   0, 0},
      { 176, 255,   0, 0},
      { 148, 255,   0, 0},
      { 121, 255,   0, 0},
      {  93, 255,   0, 0},
      {  65, 255,   0, 0},
      {  37, 255,   0, 0},
      {   9, 255,   0, 0},
      {   0, 255,  18, 0},
      {   0, 255,  46, 0},
      {   0, 255,  74, 0},
      {   0, 255, 102, 0},
      {   0, 255, 130, 0},
      {   0, 255, 158, 0},
      {   0, 255, 186, 0},
      {   0, 255, 214, 0},
      {   0, 255, 242, 0},
      {   0, 242, 255, 0},
      {   0, 214, 255, 0},
      {   0, 186, 255, 0},
      {   0, 158, 255, 0},
      {   0, 130, 255, 0},
      {   0, 102, 255, 0},
      {   0,  74, 255, 0},
      {   0,  46, 255, 0},
      {   0,  18, 255, 0},
      {   9,   0, 255, 0},
      {  37,   0, 255, 0},
      {  65,   0, 255, 0},
      {  93,   0, 255, 0},
      { 120,   0, 255, 0},
      { 148,   0, 255, 0},
      { 176,   0, 255, 0},
      { 204,   0, 255, 0},
      { 232,   0, 255, 0},
      { 255,   0, 251, 0},
      { 255,   0, 223, 0},
      { 255,   0, 195, 0},
      { 255,   0, 167, 0},
      { 255,   0, 139, 0},
      { 255,   0, 111, 0},
      { 255,   0,  83, 0},
      { 255,   0,  55, 0},
      { 255,   0,  27, 0},
      { 255,   0,   0, 0},
      { 195,  21,   0, 0},
      { 195,  42,   0, 0},
      { 195,  63,   0, 0},
      { 195,  85,   0, 0},
      { 195, 106,   0, 0},
      { 195, 127,   0, 0},
      { 195, 149,   0, 0},
      {  58,  58,  58, 0},
      {  83,  83,  83, 0},
      { 109, 109, 109, 0},
      { 134, 134, 134, 0},
      { 160, 160, 160, 0},
      { 185, 185, 185, 0},
      { 210, 210, 210, 0},
      { 234, 234, 234, 0},
      { 204, 204, 204, 0},
      { 153, 153, 153, 0},
      { 135, 135, 135, 0},
      {   0, 255, 255, 0},
   }
};

struct
{
   unsigned char num;
   unsigned char seg[15];
} defaultDashStyles[16]=
{
   { 0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 2,0x06,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 2,0x0C,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 2,0x18,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 2,0x30,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 4,0x06,0x02,0x01,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 4,0x0C,0x02,0x01,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 4,0x18,0x03,0x02,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 4,0x30,0x04,0x02,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 6,0x06,0x02,0x01,0x02,0x01,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 6,0x0C,0x02,0x01,0x02,0x01,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 6,0x18,0x03,0x02,0x02,0x02,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 6,0x30,0x04,0x02,0x02,0x02,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 8,0x0C,0x02,0x01,0x02,0x01,0x02,0x01,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 8,0x18,0x03,0x02,0x02,0x02,0x02,0x02,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
   { 8,0x30,0x04,0x02,0x03,0x02,0x03,0x02,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
};

// 1 English Scales
Ratio EnglishScales[]=
{
   0,26, // number of scales
   10,1, 4,1,  2,1,  1,1,  1,2,  1,4,  1,8,  1,12, 3,48, 1,24,
   3,96, 1,48, 1,64, 1,96, 1,192,   1,120,   1,240,   1,360,   1,480,   1,600,
   1,1200,  1,2400,  1,6000,  1,12000, 1,60000, 0xFFFF,0xFFFF
};

// 2 Metric Scales
Ratio MetricScales[]=
{
   0,23, // number of scales
   10,1, 5,1,  4,1,  5,2,  2,1,  1,1,  1,2,  2,5,  1,5,  1,10,
   1,20, 1,25, 1,50, 1,100,   1,200,   1,500,   1,1000,  1,2000,  1,5000,  1,10000,
   1,1250,  1,2500,  0xFFFF,0xFFFF
};

// 10 Fract GridSnaps
Ratio FractGridSnaps[]=
{
   0,50, // number of scales
   1,1200,  1,600,   1,300,   1,200,   1,192,   1,150,   1,128,   1,120,   1,100,   1,96,
   1,80, 1,64, 1,60, 1,50, 1,48, 1,40, 1,32, 3,32, 1,30, 1,25,
   2,25, 1,24, 1,20, 1,16, 3,16, 1,15, 1,12, 1,10, 1,8,  3,8,
   1,6,  1,5,  1,4,  1,3,  1,2,  2,1,  2,5,  1,36, 1,18, 5,36,
   5,18, 5,32, 5,16, 5,12, 5,24, 5,48, 5,96, 5,240,   19,240,  3,4
};

// 11 Met GridSnaps
Ratio MetGridSnaps[]=
{
   0,18, // number of scales
   1,50, 1,40, 1,20, 1,25, 1,16, 2,25, 1,10, 1,8,  1,5,  1,4,
   2,5,  1,2,  5,8,  4,5,  1,1,  5,4,  2,1,  5,2
};

// 12 Eng Dec Ft Snaps
Ratio EngDecFtSnaps[]=
{
   0,51, // number of scales
   5,2,  5,4,  1,1,  4,5,  5,8,  1,2,  5,12, 2,5,  5,16, 5,18,
   1,4,  5,24, 1,5,  1,6,  1,8,  1,10, 5,32, 5,36, 1,12, 1,16,
   5,48, 5,64, 2,25, 1,20, 1,24, 5,96, 1,30, 5,128,   1,25, 1,32,
   1,36, 5,192,   1,40, 1,48, 1,50, 1,60, 1,64, 1,72, 1,80, 1,96,
   1,100,   1,120,   1,128,   1,160,   1,192,   1,240,   25,96,   1,18, 1,200,   1,3,
   1,15
};

typedef struct
{
   int      fSTRID;
   Ratio *fSCAL;
   short fNumScales;
   GrdEntries  fData[25];
} GrdSInfo;

// 0 English Fract Grids
GrdSInfo EnglishFractGrids=
{
   IDS_ENGLISH_FRAC_GRIDS, FractGridSnaps,   25,
   {
      { 1,0 },
      { 2,0 },
      { 3,5,   {2,17,   3,24, 4,29, 5,33, 6,35} },
      { 4,5,   {2,12,   3,17, 4,24, 5,29, 6,33} },
      { 5,6,   {2,7, 3,12, 4,17, 5,24, 6,29, 7,33} },
      { 6,5,   {3,7, 4,12, 5,17, 6,24, 7,29} },
      { 7,5,   {4,7, 5,12, 6,17, 7,24, 8,29} },
      { 8,7,   {5,10,   6,15, 7,22, 8,27, 9,31, 10,34,   11,35} },
      { 9,7,   {5,7, 6,12, 7,17, 8,24, 9,29, 10,33,   11,30} },
      { 10,7,  {6,10,   7,15, 8,22, 9,27, 10,31,   11,33,   13,35} },
      { 11,7,  {7,12,   8,17, 9,24, 10,29,   11,25,   13,30,   14,50} },
      { 12,7,  {7,10,   8,15, 9,22, 10,27,   11,29,   13,33,   14,35} },
      { 13,7,  {8,12,   9,17, 10,24,   11,18,   12,29,   13,25,   14,30} },
      { 14,7,  {8,10,   9,15, 10,22,   11,24,   12,27,   13,29,   14,33} },
      { 15,6,  {8,5, 9,10, 10,15,   11,17,   13,24,   14,29} },
      { 16,7,  {8,8, 9,13, 10,19,   11,23,   12,26,   13,28,   14,32} },
      { 17,6,  {9,8, 10,13,   11,16,   13,23,   14,28,   15,32} },
      { 18,5,  {11,13,  13,19,   14,26,   16,31,   18,34} },
      { 19,7,  {10,8,   11,11,   13,16,   14,23,   15,28,   17,32,   18,33} },
      { 20,7,  {11,9,   13,14,   14,20,   16,28,   18,32,   19,37,   20,35} },
      { 21,6,  {14,14,  15,20,   16,23,   18,28,   19,32,   20,33} },
      { 22,6,  {15,14,  16,16,   18,23,   19,28,   20,29,   21,33} },
      { 23,5,  {18,14,  19,20,   20,23,   21,28,   22,32} },
      { 24,6,  {19,14,  20,16,   21,23,   22,28,   23,32,   24,33} },
      { 25,5,  {22,14,  23,20,   24,23,   25,28,   26,32} }
   }
};

// 1 English Dec Grids
GrdSInfo EnglishDecGrids=
{
   IDS_ENGLISH_DEC_GRIDS, FractGridSnaps, 25,
   {
      { 1,5,   {1,9, 2,23, 3,28, 4,32, 5,33} },
      { 2,6,   {2,14,   3,20, 4,21, 5,28, 6,32, 8,35} },
      { 3,6,   {2,9, 3,14, 5,23, 6,28, 7,32, 10,35} },
      { 4,5,   {3,9, 6,23, 7,28, 9,32, 10,33} },
      { 5,6,   {3,4, 6,16, 7,23, 9,28, 10,29,   12,33} },
      { 6,5,   {6,11,   7,16, 9,23, 10,24,   12,29} },
      { 7,5,   {7,11,   9,16, 11,23,   12,24,   13,29} },
      { 8,8,   {7,8, 9,13, 10,15,   12,22,   13,27,   14,31,   15,34,   16,35} },
      { 9,7,   {9,11,   10,12,   12,17,   13,24,   14,29,   15,33,   16,30} },
      { 10,7,  {10,10,  12,15,   13,22,   14,27,   15,31,   16,33,   18,35} },
      { 11,7,  {12,12,  13,17,   14,24,   15,29,   16,25,   18,30,   19,50} },
      { 12,7,  {12,10,  13,15,   14,22,   15,27,   16,29,   18,33,   19,35} },
      { 13,7,  {13,12,  14,17,   15,24,   16,18,   17,29,   18,25,   19,30} },
      { 14,7,  {13,10,  14,15,   15,22,   16,24,   17,27,   18,29,   19,33} },
      { 15,6,  {13,5,   14,10,   15,15,   16,17,   18,24,   19,29} },
      { 16,7,  {13,8,   14,13,   15,19,   16,23,   17,26,   18,28,   19,32} },
      { 17,6,  {14,8,   15,13,   16,16,   18,23,   19,28,   20,32} },
      { 18,5,  {16,13,  18,19,   19,26,   21,31,   23,34} },
      { 19,7,  {15,8,   16,11,   18,16,   19,23,   20,28,   22,32,   23,33} },
      { 20,7,  {16,9,   18,14,   19,20,   21,28,   23,32,   24,37,   25,35} },
      { 21,6,  {19,14,  20,20,   21,23,   23,28,   24,32,   25,33} },
      { 22,6,  {20,14,  21,16,   23,23,   24,28,   25,29,   26,33} },
      { 23,5,  {23,14,  24,20,   25,23,   26,28,   27,32} },
      { 24,6,  {24,14,  25,16,   26,23,   27,28,   28,32,   29,33} },
      { 25,5,  {27,14,  28,20,   29,23,   30,28,   31,32} }
   }
};

// 2 Metric Dec Grids
GrdSInfo MetricDecGrids=
{
   IDS_METRIC_GRIDS, MetGridSnaps, 22,
   {
      { 1,5,   {1,2, 2,3,  3,7,  5,9,  7,12} },
      { 2,6,   {2,2, 3,3,  5,7,  6,8,  7,10, 8,12} },
      { 3,8,   {2,1, 3,4,  4,3,  6,7,  7,9,  8,11, 9,12, 11,15} },
      { 4,7,   {3,2, 5,3,  6,5,  7,8,  8,10, 10,12,   11,13} },
      { 5,8,   {3,1, 5,4,  6,3,  7,7,  8,9,  10,11,   11,12,   12,15} },
      { 6,6,   {6,2, 7,3,  8,7,  10,9, 11,10,   12,12} },
      { 7,8,   {14,2,   15,3, 16,7, 18,10,   19,12,   20,15,   21,16,   22,18} },
      { 8,8,   {14,1,   15,4, 16,6, 18,9, 19,11,   20,14,   21,15,   22,17} },
      { 9,7,   {15,1,   16,4, 18,7, 19,9, 20,11,   21,12,   22,15} },
      { 10,6,  {18,3,   19,7, 20,9, 21,10,   22,12,   23,15,   } },
      { 11,7,  {19,3,   20,7, 21,8, 22,10,   23,12,   24,15,   25,16} },
      { 12,5,  {20,6,   21,7, 22,9, 23,11,   24,14} },
      { 13,7,  {26,1,   27,4, 29,7, 30,9, 31,11,   32,12,   33,15} },
      { 14,6,  {29,3,   30,7, 31,9, 32,10,   33,12,   34,15} },
      { 15,7,  {30,3,   31,7, 32,8, 33,10,   34,12,   35,15,   36,16} },
      { 16,8,  {30,1,   31,4, 32,3, 33,7, 34,9, 35,11,   36,12,   37,15} },
      { 17,6,  {31,1,   33,3, 34,7, 35,9, 37,12,   38,15} },
      { 18,7,  {34,3,   35,7, 36,8, 37,10,   38,12,   39,15,   40,16} },
      { 19,6,  {36,3,   37,7, 38,9, 39,11,   40,12,   41,15} },
      { 20,5,  {38,7,   39,9, 40,10,   41,12,   42,15} },
      { 21,0 },
      { 22,0 },
   }
};

// 3 English Dec Ft Grids
GrdSInfo EnglishDecFtGrids=
{
   IDS_ENGLISH_DEC_FT_GRIDS, EngDecFtSnaps, 25,
   {
      { 1,6,   {1,16,   3,11, 4,6,  5,3,  6,2,  8,1} },
      { 2,7,   {1,29,   2,23, 3,16, 4,13, 5,8,  7,4,  8,3} },
      { 3,7,   {1,35,   2,29, 3,24, 4,16, 5,13, 7,8,  8,6} },
      { 4,6,   {1,41,   4,24, 5,16, 7,13, 8,11, 9,6} },
      { 5,5,   {4,33,   5,24, 7,16, 8,15, 9,11} },
      { 6,7,   {4,39,   5,33, 7,24, 8,20, 9,15, 10,11,   11,6} },
      { 7,7,   {4,44,   5,39, 7,33, 8,30, 9,20, 10,15,   12,9} },
      { 8,7,   {5,41,   7,35, 9,24, 10,16,   11,13,   12,11,   13,6} },
      { 9,8,   {7,39,   8,37, 9,30, 10,20,   11,15,   12,17,   13,9, 14,5} },
      { 10,7,  {7,41,   9,33, 10,24,   11,16,   12,15,   13,11,   14,6,} },
      { 11,6,  {9,37,   10,30,   12,22,   13,17,   14,9, 15,5} },
      { 12,6,  {9,39,   10,33,   12,20,   13,15,   14,11,   15,6} },
      { 13,6,  {9,43,   10,37,   12,28,   13,22,   14,17,   15,9} },
      { 14,6,  {10,39,  11,33,   13,20,   14,15,   15,11,   16,6} },
      { 15,6,  {10,44,  11,39,   13,30,   14,20,   15,15,   17,9} },
      { 16,6,  {10,41,  11,35,   13,24,   14,16,   15,13,   17,6} },
      { 17,7,  {10,49,  11,41,   13,33,   14,24,   15,16,   16,13,   17,11} },
      { 18,5,  {13,36,  14,27,   15,51,   17,14,   19,50} },
      { 19,8,  {11,49,  13,39,   14,33,   15,24,   16,16,   17,15,   18,13,   19,11} },
      { 20,6,  {13,41,  14,35,   15,29,   17,16,   19,13,   21,6} },
      { 21,6,  {14,41,  17,24,   19,16,   20,13,   21,11,   22,6} },
      { 22,7,  {14,49,  17,33,   19,24,   20,16,   21,15,   22,11,   23,6} },
      { 23,7,  {17,41,  19,35,   20,29,   21,24,   22,16,   23,13,   24,6} },
      { 24,7,  {19,41,  20,35,   21,33,   22,24,   23,16,   24,11,   25,6} },
      { 25,6,  {22,41,  23,35,   24,24,   25,16,   26,13,   27,6} }
   }
};


#if _FLOAT_ZOOM

// Mac version comments:
// The zoom factor is held on disc and in some data structures as short which is strangely
// formatted to enable old version of MacDraft to read newer data files. It used to be the
// number of places to shift, negative being for a right shift, and positive for a left shift.
// As both 68K and PPC processors interpret their register based shift counts as being modulo 32,
// then the ten bits after the sign bit are ignore ( the sign bit is tested in MacDraft's code to
// determine the direction ). We use the ten bits as fraction bits of the shift, so the combined
// field is the log base 2 of the multiplier to be applied. For negative numbers, the fraction
// bits are inverted so that an exact 1/2, 1/4, 1/8 scale etc read from an old data file will be
// correctly interpreted, as will one read from a new file.

short FixTheFloatingZoomFactor(double zoomFactor)
{
   double f=zoomFactor;
   unsigned short s=(unsigned short)roundtol(f*1024.0);

   short zoom=0;
   for(zoom=0; s; s>>=1) zoom++;
   return zoom-11;
}

double FloatTheFixedZoomFactor ( short zoomFactor )
{
   short s=zoomFactor;
   if(s<0)
   {
      s^=0xFFE0 ; // undo the bit inversion
      s=((s<<10)&0x7C00)|((s>>5)&0x03FF)^0x8000;
   }
   else
      s=((s<<10)&0x7C00)|((s>>5)&0x03FF);
   return pow(2,s/1024.0);
}

short PackZoomFactor(int nZoomFactor,double fZoomFactor)
{
   if(fZoomFactor==0) return nZoomFactor;
   short s=(short)roundtol(log(fZoomFactor)*1024.0/log(2.0));
   short zoom=((s>>10)&0x001F)|((s<<5)&0x7FE0);
   if(s<0) zoom^=0xFFE0;
   return zoom;
}

void UnpackZoomFactor(short nPackedZoom,int& nZoomFactor,double& fZoomFactor)
{
   fZoomFactor=FloatTheFixedZoomFactor(nPackedZoom);
   nZoomFactor=FixTheFloatingZoomFactor(fZoomFactor);
   if(fZoomFactor==FloatTheFixedZoomFactor(nZoomFactor))
      fZoomFactor=0;
}


// MD6.2.1 - Infinisys Ltd.
double PackDoubleZoomFactor(int nZoomFactor,double fZoomFactor)
{
   if(fZoomFactor == 0) return FloatTheFixedZoomFactor(nZoomFactor);
   return fZoomFactor;
}

// MD6.2.1 - Infinisys Ltd.
void UnpackDoubleZoomFactor(double dPackedZoom,int& nZoomFactor,double& fZoomFactor)
{
   fZoomFactor=dPackedZoom;
   nZoomFactor=FixTheFloatingZoomFactor(fZoomFactor);
   if(fZoomFactor == FloatTheFixedZoomFactor(nZoomFactor))
      fZoomFactor=0;
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CPCDraftDoc

IMPLEMENT_DYNCREATE(CPCDraftDoc, CDocument)

BEGIN_MESSAGE_MAP(CPCDraftDoc, CDocument)
   //{{AFX_MSG_MAP(CPCDraftDoc)
   ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageSetup)
   ON_COMMAND(ID_DATA_REPORT_FORMATS, OnDataReportFormats)
   ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateFileRevert)
   ON_COMMAND(ID_FILE_REVERT, OnFileRevert)
   ON_UPDATE_COMMAND_UI(ID_DATA_FIND_REPLACE, OnUpdateDataFindReplace)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_LAYER_SETUP, OnUpdateLayoutLayerSetup)
   //}}AFX_MSG_MAP
   ON_COMMAND(ID_NEW_COLOR,OnNewColor) // New color/New Pattern - Infinisys Ltd
   ON_COMMAND(ID_NEW_PATTERN,OnNewPattern) // New color/New pattern - Infinisys Ltd
   ON_COMMAND(ID_NEW_GRADIENT,OnNewGradient) // Gradient - Infinisys Ltd
   ON_COMMAND_RANGE(ID_EDIT_STYLES_BASE, ID_EDIT_STYLES_BASE+15, OnEditLineStyleRange)
   ON_COMMAND_RANGE(ID_PATTERNS_BASE, ID_PATTERNS_BASE+999, OnEditPatternRange)
   ON_COMMAND_RANGE(ID_DUP_PATTERNS_BASE,ID_DUP_PATTERNS_BASE + 999,OnEditDupPatternRange) // Gradient - Infinisys Ltd
   ON_COMMAND_RANGE(ID_COLORS_BASE,ID_COLORS_BASE + 999,OnEditColorRange)
   ON_COMMAND_RANGE(ID_REPORTS_BASE, ID_REPORTS_BASE+999, OnReportRange)
   ON_COMMAND_RANGE(ID_UPDATE_LINK_BASE, ID_UPDATE_LINK_BASE+999, OnUpdateLinkRange)
   ON_COMMAND_RANGE(ID_BREAK_LINK_BASE, ID_BREAK_LINK_BASE+999, OnBreakLinkRange)
   ON_COMMAND(ID_DATA_PRINTREPORT, OnDataPrintReport)
   ON_COMMAND(ID_DATA_UPDATELINK,OnDataUpdateLink)
   ON_COMMAND(ID_DATA_BREAKLINK,OnDataBreakLink)
END_MESSAGE_MAP()


CVersionDoc::CVersionDoc()
{
   m_numGradients = 0;
   m_numDIBPatterns = 0;
   m_iArchiveVersion = 2;
}

CVersionDoc::~CVersionDoc()
{

}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftDoc construction/destruction

CPCDraftDoc::CPCDraftDoc()
{

   {

      CDC dcMem;
   
      dcMem.CreateCompatibleDC(nullptr);

      //m_bitmapWhite.CreateCompatibleBitmap(&dcMem, 32, 32);
   
      //auto pbitmapOld = dcMem.SelectObject(&m_bitmapWhite);

      dcMem.FillSolidRect(0, 0, 32, 32, RGB(255, 255, 255));
   
      //dcMem.SelectObject(pbitmapOld);

   }

   m_bUtf8Name = true;
   m_bShowGuideLines = true; // Guides - PCDraft 7 - Infinisys Ltd.
   m_bSnapToGuides = true; // Guides - PCDraft 7 - Infinisys Ltd.
   m_bUpdateViews = true; // Library - PCDraft 6 - Infinisys Ltd.
   m_nSlideSeconds=0;
   m_bLayer1AsMaster=false;
   m_bSaveAsMacDraft=false;
   m_bSaveAsStationery=false;
   m_nMaxZoom=CPCDraftApp::gMaxZoom;
   m_bComplete=true;
   m_bShowRulers=false;
   m_nUnit = kUEnglishMask | 0x04;
   m_bEnglishUnits = true;
   m_nDisplayRes = kEngRes;
   m_numDIBPatterns = 71;
   m_numGradients =8; // Gradient - Infinisys Ltd
   m_ratioCustom.fD = 1; // Free Scale - Infinisys Ltd
   m_ratioCustom.fN = 1;
// for(int i=0;i<71;i++) m_DIBPatterns[i]=NULL;
}

CPCDraftDoc::~CPCDraftDoc()
{
}

extern   LPCTSTR  gDimEndMarks;
extern   LPCTSTR  gDimFormats;
extern   LPCTSTR  gDimStandards;
extern   LPCTSTR  gDrawAndPageSetup;
extern   LPCTSTR  gFieldNames;
extern   LPCTSTR  gFillPatternsColors;
extern   LPCTSTR  gHideGridLines;
extern   LPCTSTR  gHidePageBreaks;
extern   LPCTSTR  gLineSettings;
extern   LPCTSTR  gOpenAtCenter;
extern   LPCTSTR  gScaleAndLayerSetup;
extern   LPCTSTR  gShowRulers;
extern   LPCTSTR  gSnapToObject;
extern   LPCTSTR  gTextSettings;

BOOL CPCDraftDoc::OnNewDocument()
{
   CPCDraftApp *pApp=(CPCDraftApp*)AfxGetApp();

   if(!pApp->CheckResources() || !CDocument::OnNewDocument())
      return FALSE;

   SetTitle(GetDefaultTitle());

   bool bOverridingDefaults=::GetAsyncKeyState(VK_MENU)<0;

   if (bOverridingDefaults || !pApp->m_bScaleAndLayerSetup || !pApp->LoadDocumentDefault(gScaleAndLayerSetup, this, &CPCDraftDoc::SerializeScaleAndLayerSetup))
   {
      m_nActiveLayer=-1;
      m_bActiveLayerHidden=false;
#ifdef _JAPANESE
      //m_nDefScale=6; // default for Metric
      m_nDefScale=14; // default for Metric  Infinisys Ltd.
#else
      m_nDefScale=4; // default for English
#endif
//    AddLayer();
      CLayer* pLayer=new CLayer;
      CStringW wstr;
      wstr.Format(IDS_LAYERNAME,1);
      pLayer->m_strName = get_cp(wstr,CP_UTF8);
      pLayer->m_nScale=m_nDefScale;
      pLayer->m_nSnap=3;
      m_nActiveLayer=m_Layers.Add(pLayer);

#ifdef _JAPANESE
      //SetupUnits(kUCentimeters,true,kDecimalDegrees,2,1,6);
      SetupUnits(kUCentimeters,true,kDecimalDegrees,2,1,14); // Infinisys Ltd.
#else
      SetupUnits(kUFracFeetInches,false,kDecimalDegrees,2,1,4);
#endif
   }
   else SetupUnits(m_nUnit,m_bHideMetricUnits,GetAngleUnit(),m_nPlaces,m_nAnglePlaces,m_nDefScale, NULL, 0, &m_ratioCustom);

   if (bOverridingDefaults || !pApp->m_bFieldNames || !pApp->LoadDocumentDefault(gFieldNames, this, &CPCDraftDoc::SerializeFieldNames))
   {
      m_sFieldNames[0].LoadString(IDS_FIELD1);
      m_sFieldNames[1].LoadString(IDS_FIELD2);
      m_sFieldNames[2].LoadString(IDS_FIELD3);
      m_sFieldNames[3].LoadString(IDS_FIELD4);
      m_sFieldNames[4].LoadString(IDS_FIELD5);
   }

   if (bOverridingDefaults || !pApp->m_bDimStandards || !pApp->LoadDocumentDefault(gDimStandards, this, &CPCDraftDoc::SerializeDimStandards))
   {
   }

   if (bOverridingDefaults || !pApp->m_bDimFormats || !pApp->LoadDocumentDefault(gDimFormats, this, &CPCDraftDoc::SerializeDimFormats))
   {
   }

   if (bOverridingDefaults || !pApp->m_bDimEndMarks || !pApp->LoadDocumentDefault(gDimEndMarks, this, &CPCDraftDoc::SerializeDimEndmarks))
   {
      m_nDimCap1=6;
      m_nDimCap2=6;
   }

   m_packeditema.RemoveAll(); // Gradient - Infinisys Ltd

   if(bOverridingDefaults || !pApp->m_bFillPatternsColors || !pApp->LoadDocumentDefault(gFillPatternsColors,this,&CPCDraftDoc::SerializeFillPatternsColors))
   {
      m_Palette.CreatePalette((LPLOGPALETTE)&defaultPalette);

      for(int i=0; i<71; i++)
      {
         m_Patterns[i].LoadResource(IDB_PAT01+i);
         HBITMAP hDib = m_Patterns[i].CreateDIBSection();
         m_PatternsHash[i] = CalculateDIBHash(hDib);
         ::DeleteObject(hDib);
         CPackedItem item; // Gradient - Infinisys Ltd
         item.m_etype = packed_type_dib;
         item.m_pdib = &m_Patterns[i];
         m_packeditema.Add(item);
      }

      m_Fill.m_nFillType=kFillColor;
      m_Fill.m_nIndex=PALETTEINDEX(0); // white color

      // Gradient no. 1
      int iGradient = 0;
      {
         double dStep = 1.0 / (9.0 - 1.0);
         double d = 0.0;
         m_Gradients[iGradient].m_dRotation = -90.0;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(106,48,5) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(126,76,33) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(70,40,24) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(138,87,34) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(93,33,0) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(91,35,0) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(73,43,27) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(126,61,18) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(70,40,24) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }

      // Gradient no. 2
      iGradient++;
      {
         m_Gradients[iGradient].m_dRotation = -90.0;
         m_Gradients[iGradient].m_da.Add(0.0);
         m_Gradients[iGradient].m_cra.Add(RGB(222,227,230) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.2);
         m_Gradients[iGradient].m_cra.Add(RGB(255,253,229) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.5);
         m_Gradients[iGradient].m_cra.Add(RGB(214,205,193) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.75);
         m_Gradients[iGradient].m_cra.Add(RGB(237,237,238) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(1.0);
         m_Gradients[iGradient].m_cra.Add(RGB(216,213,187) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }


      // Gradient no. 3
      iGradient++;
      {
         double dStep = 1.0 / (6.0 - 1.0);
         double d = 0.0;
         m_Gradients[iGradient].m_dRotation = -90.0;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(0,168,222) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(51,51,145) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(233,19,136) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(235,45,46) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(253,233,43) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(0,158,84) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }


      // Gradient no. 4
      iGradient++;
      {
         double dStep = 1.0 / (3.0 - 1.0);
         double d = 0.0;
         m_Gradients[iGradient].m_dRotation = 0.0;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(255,244,46) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(165,53,148) | (255 << 24));
         d+= dStep;
         m_Gradients[iGradient].m_da.Add(d);
         m_Gradients[iGradient].m_cra.Add(RGB(218,33,40) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }


      // Gradient no. 5
      iGradient++;
      {
         m_Gradients[iGradient].m_dRotation = -90.0;
         m_Gradients[iGradient].m_da.Add(0);
         m_Gradients[iGradient].m_cra.Add(RGB(176,210,64) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.45);
         m_Gradients[iGradient].m_cra.Add(RGB(255,223,0) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.90);
         m_Gradients[iGradient].m_cra.Add(RGB(0,170,79) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }

      // Gradient no. 6
      iGradient++;
      {
         m_Gradients[iGradient].m_bRadial = true;
         m_Gradients[iGradient].m_da.Add(.25);
         m_Gradients[iGradient].m_cra.Add(RGB(0,186,230) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.60);
         m_Gradients[iGradient].m_cra.Add(RGB(116,220,244) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.90);
         m_Gradients[iGradient].m_cra.Add(RGB(204,245,255) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(1.0);
         m_Gradients[iGradient].m_cra.Add(RGB(255,255,255) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }

      // Gradient no. 7
      iGradient++;
      {
         m_Gradients[iGradient].m_bRadial = true;
         m_Gradients[iGradient].m_dX =-.25;
         m_Gradients[iGradient].m_dY =-.25;
         m_Gradients[iGradient].m_da.Add(.4);
         m_Gradients[iGradient].m_cra.Add(RGB(235,45,46) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.6);
         m_Gradients[iGradient].m_cra.Add(RGB(241,103,104) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.90);
         m_Gradients[iGradient].m_cra.Add(RGB(248,184,184) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(1.0);
         m_Gradients[iGradient].m_cra.Add(RGB(255,255,255) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }

      // Gradient no. 8
      iGradient++;
      {
         m_Gradients[iGradient].m_dRotation = 45.0;
         m_Gradients[iGradient].m_da.Add(.1);
         m_Gradients[iGradient].m_cra.Add(RGB(84,21,19) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.2);
         m_Gradients[iGradient].m_cra.Add(RGB(141,80,39) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.4);
         m_Gradients[iGradient].m_cra.Add(RGB(203,149,82) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(0.55);
         m_Gradients[iGradient].m_cra.Add(RGB(255,242,219) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.75);
         m_Gradients[iGradient].m_cra.Add(RGB(215,177,137) | (255 << 24));
         m_Gradients[iGradient].m_da.Add(.9);
         m_Gradients[iGradient].m_cra.Add(RGB(156,83,13) | (255 << 24));
         CPackedItem item;
         item.m_etype = packed_type_gradient;
         item.m_pgradient = &m_Gradients[iGradient];
         m_packeditema.Add(item);
      }
      iGradient++;
      m_numGradients = iGradient;
   }

   if (bOverridingDefaults || !pApp->m_bLineSettings || !pApp->LoadDocumentDefault(gLineSettings, this, &CPCDraftDoc::SerializeLineSettings))
   {
      memcpy(m_DashStyles,defaultDashStyles,256);
   }

   m_bHideFills=false;
   m_bHideText=false;

   m_bHidePageBreaks=bOverridingDefaults?false:(pApp->m_bHidePageBreaks?true:false);
   m_bHideGridLines=bOverridingDefaults?false:(pApp->m_bHideGridLines?true:false);
   m_bShowRulers=bOverridingDefaults?true:(pApp->m_bShowRulers?true:false);
   m_bStandardRulers=false;
   m_bSnapToObject=bOverridingDefaults?true:(pApp->m_bSnapToObject?true:false);
   m_nZoomFactor=0;
   m_nPrevZoomFactor=0;
   m_fZoomFactor=0;
   m_fPrevZoomFactor=0;
   m_ptPrevZoomScroll.x=0; m_ptPrevZoomScroll.y=0;

   if(bOverridingDefaults || !pApp->m_bDrawAndPageSetup || !pApp->LoadDocumentDefault(gDrawAndPageSetup,this,&CPCDraftDoc::SerializeDrawAndPageSetup))
   {
      if(m_bUpdateViews) // Library - PCDraft 6 - Infinisys Ltd.
      {
         m_PrinterSettings.CopyDefaultMfcPrinter();
         m_PageSize = get_synthesized_printer_page_size();
  //       m_PrinterSettings.CopyDefaultMfcPrinter();
//         m_PageSize=CalcPrinterPageSize();
      }
      else
      {
         m_PageSize=CSize(558,805);
      }
      m_NumPage.x=1;
      m_NumPage.y=1;
      m_nPageNumbering=kPageNumbering1324;
      m_bPrintLastPageFirst=FALSE;
   }

   m_RulerOrigin.Clear();

   if (bOverridingDefaults || !pApp->m_bTextSettings || !pApp->LoadDocumentDefault(gTextSettings, this, &CPCDraftDoc::SerializeTextSettings))
   {
   }

   m_bSaveAsMacDraft=true; //defaulting to MacDraft file format
   SetModifiedFlag(FALSE);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftDoc serialization

void CPCDraftDoc::SerializeScaleAndLayerSetup(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << short(m_nUnit);
      ar << short(1);   // version

      ar << (BYTE)(m_bHideMetricUnits ? 1:0);
      ar << m_nPlaces;
      ar << GetAngleUnit();
      ar << m_snapTable.getSelectedSnap();

      ar << m_nAnglePlaces;
      ar << (BYTE)( m_bEnglishUnits ? 1 : 0);
      ar << m_nDisplayRes;
      ar << m_nDefScale;
      ar << m_nActiveLayer;
      ar << (BYTE)(m_bActiveLayerHidden ? 1 : 0);
   }
   else
   {
      short nVersion;
      ar >> nVersion;
      int tempUnit=nVersion;
      ar >> nVersion;
      if(nVersion>1)
      {
         return;
      }
      m_nUnit=tempUnit;

      BYTE dataGet;
      ar >> dataGet;
      m_bHideMetricUnits = dataGet == TRUE;
      ar >> m_nPlaces;
      int tmp;
      ar >> tmp;
      m_snapTable.setAngleUnit(tmp);
      if(nVersion>0)
      {
         ar >> tmp;
         m_snapTable.selectSnap(tmp);
      }

      ar >> m_nAnglePlaces;
      ar >> dataGet;
      m_bEnglishUnits = dataGet == TRUE;
      ar >> m_nDisplayRes;
      ar >> m_nDefScale;
      ar >> m_nActiveLayer;
      ar >> dataGet;
      m_bActiveLayerHidden = dataGet == TRUE;


   }
   CLayer::bIgnoreShapeList=true;
   m_Layers.Serialize(ar);
   CLayer::bIgnoreShapeList=false;
}
CArchive & save_index (CVersionDoc * pdoc, CArchive & ar,CPackedItem & item)
{

   int iType = item.m_etype;

   ar << iType;

   int i = item.get_index(pdoc);
   
   ar << i;

   return ar;

}

CArchive & load_index(CVersionDoc * pdoc,CArchive & ar,CPackedItem & item)
{

   int iType;

   ar >> iType;

   item.m_etype = (e_packed_type)iType;

   int i = 0;

   ar >> i;

   item.set_index(pdoc, i);

   return ar;

}

void CPCDraftDoc::SerializeFillPatternsColors(CArchive& ar)
{
   int nCount;
   if (ar.IsStoring())
   {
      ar << m_Fill;
      nCount=m_Palette.GetEntryCount();
      ar << nCount;
      while(nCount--)
      {
         PALETTEENTRY palEntry;
         m_Palette.GetPaletteEntries(nCount,1,&palEntry);
         ar.Write(&palEntry,sizeof(palEntry));
      }

      ar << m_numDIBPatterns;
      for(nCount=0; nCount<m_numDIBPatterns; nCount++) ar << m_Patterns[nCount];
      ar << m_numGradients;
      for(nCount=0; nCount<m_numGradients; nCount++) ar << m_Gradients[nCount];
      int iCount = m_packeditema.GetCount();
      ar << iCount;
      for(nCount=0; nCount<m_packeditema.GetCount(); nCount++) save_index(this, ar, m_packeditema[nCount]);
      // WriteDIB(ar,m_DIBPatterns[nCount]);
   }
   else
   {
      ar >> m_Fill;
      LOGPALETTE logPal;
      logPal.palVersion=0x300;
      logPal.palNumEntries=1;
      m_Palette.CreatePalette(&logPal);
      ar >> nCount;
      m_Palette.ResizePalette(nCount);
      while(nCount--)
      {
         PALETTEENTRY palEntry;
         ar.Read(&palEntry,sizeof(palEntry));
         m_Palette.SetPaletteEntries(nCount,1,&palEntry);
      }
      ar >> m_numDIBPatterns;
      for(nCount=0; nCount<m_numDIBPatterns; nCount++)
      {
         ar >> m_Patterns[nCount];
         HBITMAP hDib = m_Patterns[nCount].CreateDIBSection();
         m_PatternsHash[nCount] = CalculateDIBHash(hDib);
         ::DeleteObject(hDib);
         //ReadDIB(ar,m_DIBPatterns[nCount]);
      }
      ar >> m_numGradients;
      for(nCount=0; nCount<m_numGradients; nCount++) ar >> m_Gradients[nCount];
      int iCount = 0;
      ar >> iCount;
      m_packeditema.SetSize(iCount);
      for(nCount=0; nCount<iCount; nCount++) load_index(this, ar, m_packeditema[nCount]);
   }
}

void CPCDraftDoc::SerializeLineSettings(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << m_Pen;
      ar.Write(m_DashStyles,256);
   }
   else
   {
      ar >> m_Pen;
      ar.Read(m_DashStyles,256);
   }
}

void CPCDraftDoc::SerializeDimStandards(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << m_DimStd;
   }
   else
   {
      DimStandard ds;
      ar >> ds;
      if((m_bEnglishUnits?kDsEnglish:kDsMetric)==ds.nType) m_DimStd=ds;
   }
}

void CPCDraftDoc::SerializeDimFormats(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << m_DimFmts[0];
      ar << m_DimFmts[1];
      ar << m_DimFmts[2];
   }
   else
   {
      ar >> m_DimFmts[0];
      ar >> m_DimFmts[1];
      ar >> m_DimFmts[2];
   }
}

void CPCDraftDoc::SerializeDimEndmarks(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << m_nDimCap1 << m_nDimCap2;
   }
   else
   {
      ar >> m_nDimCap1 >> m_nDimCap2;
   }
}

void CPCDraftDoc::SerializeFieldNames(CArchive& ar)
{
   int nCount;
   if (ar.IsStoring())
   {
      for(nCount=0; nCount<5; nCount++)
      {
         ar << m_sFieldNames[nCount];
      }
   }
   else
   {
      for(nCount=0; nCount<5; nCount++)
      {
         ar >> m_sFieldNames[nCount];
      }
   }
}

void CPCDraftDoc::SerializeTextSettings(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << m_TextParams;
   }
   else
   {
      ar >> m_TextParams;
   }
}

void CPCDraftDoc::SerializeDrawAndPageSetup(CArchive& ar)
{
   if (ar.IsStoring())
   {
      ar << m_PageSize;
      ar << m_NumPage;
      ar << m_nPageNumbering;
      ar << m_bPrintLastPageFirst;
   }
   else
   {
      ar >> m_PageSize;
      ar >> m_NumPage;
      ar >> m_nPageNumbering;
      ar >> m_bPrintLastPageFirst;
   }
   m_PrinterSettings.Serialize(ar);
}

void CPCDraftDoc::Serialize(CArchive& ar)
{
   ar.m_pDocument = this;
   int nCount;
// CObject* pObject;

   if (ar.IsStoring())
   {
      ar << short(m_nUnit);
      ar << short(2);   // version
      m_iArchiveVersion = 2;
      ar << (BYTE)(m_bHideMetricUnits ? 1 : 0);
      ar << m_nPlaces;
      ar << GetAngleUnit();
      ar << m_snapTable.getSelectedSnap();

      ar << m_nAnglePlaces;
      ar << (BYTE)(m_bHidePageBreaks ? 1 : 0);
      ar << (BYTE)(m_bHideGridLines ? 1:0);
      ar << (BYTE)(m_bShowRulers ? 1:0);
      ar << (BYTE)(m_bStandardRulers ? 1:0);
      ar << (BYTE)(m_bEnglishUnits ? 1:0);
      ar << m_nDisplayRes;
#ifdef _FLOAT_ZOOM
      int nZoom=PackZoomFactor(m_nZoomFactor,m_fZoomFactor);
      ar << nZoom;
      nZoom=PackZoomFactor(m_nPrevZoomFactor,m_fPrevZoomFactor);
      ar << nZoom;
#else
      ar << m_nZoomFactor;
      ar << m_nPrevZoomFactor;
#endif
      ar << m_ptPrevZoomScroll;
      ar << m_PageSize;
      ar << m_NumPage;
      ar << m_nPageNumbering;
      ar << m_bPrintLastPageFirst;
      ar << m_RulerOrigin;
      ar << m_nDefScale;

      if(m_nDefScale < 0)
      {
         ar << m_ratioCustom.fD;
         ar << m_ratioCustom.fN;
      }

      ar << (BYTE)(m_bHideFills ? 1:0);
      ar << (BYTE)(m_bHideText ? 1:0);
      ar << m_nActiveLayer;
      ar << (BYTE)(m_bActiveLayerHidden ? 1 : 0);

      m_Layers.Serialize(ar);
      m_Reports.Serialize(ar);
      m_SavedViews.Serialize(ar);

      ar << m_Pen;
      ar << m_Fill;
      ar << m_ParallelAttrs;
      ar << m_TextParams;
      ar << m_DimStd;
      ar << m_DimFmts[0];
      ar << m_DimFmts[1];
      ar << m_DimFmts[2];
      ar << m_nDimCap1 << m_nDimCap2;

      for(nCount=0; nCount<5; nCount++)
      {
         ar << m_sFieldNames[nCount];
      }

      nCount=m_Palette.GetEntryCount();
      ar << nCount;
      while(nCount--)
      {
         PALETTEENTRY palEntry;
         m_Palette.GetPaletteEntries(nCount,1,&palEntry);
         ar.Write(&palEntry,sizeof(palEntry));
      }

      int iNumPat; // Infinisys Ltd
      if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
      {

         ar << m_numDIBPatterns;

         iNumPat = m_numDIBPatterns;

      }
      else
      {

         iNumPat = 71;

      }

      for(nCount=0; nCount < iNumPat; nCount++) ar << m_Patterns[nCount];

      if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
      {

         ar << m_numGradients;

         for(nCount=0; nCount < m_numGradients; nCount++) ar << m_Gradients[nCount];

      }


      ar.Write(m_DashStyles,256);

      m_PrinterSettings.Serialize(ar);

      // Infinisys 2021
      if (ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
      {

         int pack = m_packeditema.GetCount();

         ar << pack;

         for (nCount = 0; nCount < pack; nCount++)
         {
            ar << (int &) m_packeditema[nCount].m_bOwn;
            ar << (int &)m_packeditema[nCount].m_etype;
         }

      }

   }
   else
   {


      short nVersion;
      ar >> nVersion;
      m_nUnit=nVersion;
      ar >> nVersion;
      if (nVersion > 2)
         AfxThrowArchiveException(CArchiveException::genericException, "Serialize");
      m_iArchiveVersion = nVersion;
      BYTE dataGet;
      ar >> dataGet;
      m_bHideMetricUnits = dataGet == TRUE;
      ar >> m_nPlaces;
      int tmp;
      ar >> tmp;
      m_snapTable.setAngleUnit(tmp);
      if(nVersion>0)
      {
         ar>>tmp;
         m_snapTable.selectSnap(tmp);
      }

      ar >> m_nAnglePlaces;
      ar >> dataGet;
      m_bHidePageBreaks = dataGet == TRUE;
      ar >> dataGet;
      m_bHideGridLines = dataGet == TRUE;
      ar >> dataGet;
      m_bShowRulers = dataGet == TRUE;
      ar >> dataGet;
      m_bStandardRulers = dataGet == TRUE;
      ar >> dataGet;
      m_bEnglishUnits = dataGet == TRUE;
      ar >> m_nDisplayRes;
      ar >> m_nZoomFactor;
      ar >> m_nPrevZoomFactor;
#ifdef _FLOAT_ZOOM
      UnpackZoomFactor(m_nZoomFactor,m_nZoomFactor,m_fZoomFactor);
      UnpackZoomFactor(m_nPrevZoomFactor,m_nPrevZoomFactor,m_fPrevZoomFactor);
#endif
      ar >> m_ptPrevZoomScroll;
      ar >> m_PageSize;
      ar >> m_NumPage;
      ar >> m_nPageNumbering;
      ar >> m_bPrintLastPageFirst;
      ar >> m_RulerOrigin;
      ar >> m_nDefScale;

      if(m_nDefScale < 0)
      {
         ar >> m_ratioCustom.fD;
         ar >> m_ratioCustom.fN;
      }

      ar >> dataGet;
      m_bHideFills = dataGet == TRUE;
      ar >> dataGet;
      m_bHideText = dataGet == TRUE;
      ar >> m_nActiveLayer;
      ar >> dataGet;
      m_bActiveLayerHidden = dataGet == TRUE;

      m_Layers.Serialize(ar);
      m_Reports.Serialize(ar);
      m_SavedViews.Serialize(ar);

      ar >> m_Pen;
      ar >> m_Fill;
      ar >> m_ParallelAttrs;
      ar >> m_TextParams;
      ar >> m_DimStd;
      ar >> m_DimFmts[0];
      ar >> m_DimFmts[1];
      ar >> m_DimFmts[2];
      ar >> m_nDimCap1 >> m_nDimCap2;

      for(nCount=0; nCount<5; nCount++)
      {
         ar >> m_sFieldNames[nCount];
      }

      LOGPALETTE logPal;
      logPal.palVersion=0x300;
      logPal.palNumEntries=1;
      m_Palette.CreatePalette(&logPal);
      ar >> nCount;
      m_Palette.ResizePalette(nCount);
      while(nCount--)
      {
         PALETTEENTRY palEntry;
         ar.Read(&palEntry,sizeof(palEntry));
         m_Palette.SetPaletteEntries(nCount,1,&palEntry);
      }

      int iNumPat;

      if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
      {

         ar >> m_numDIBPatterns;

         iNumPat = m_numDIBPatterns;

      }
      else
      {

         iNumPat = 71;

      }

      for(nCount=0; nCount<iNumPat; nCount++)
      {
         ar >> m_Patterns[nCount];
         //ReadDIB(ar,m_DIBPatterns[nCount]);
         HBITMAP hDib = m_Patterns[nCount].CreateDIBSection();
         m_PatternsHash[nCount] = CalculateDIBHash(hDib);
         ::DeleteObject(hDib);
      }

// MD6.2.1 - Infinisys Ltd.
      if(ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
      {

         ar >> m_numGradients;

         for(nCount=0; nCount < m_numGradients; nCount++) ar >> m_Gradients[nCount];

      }

      ar.Read(m_DashStyles,256);

      m_PrinterSettings.Serialize(ar);

      m_DimUnit.setDimUnits(m_nUnit,m_bHideMetricUnits,m_nPlaces,m_nDisplayRes,GetAngleUnit(),m_nAnglePlaces);

      // Infinisys 2021
      if (ar.m_pDocument == NULL || ((CVersionDoc *)ar.m_pDocument)->m_iArchiveVersion >= 2) // Infinisys Ltd
      {

         int pack = 0;
         try

         {

            ar >> pack;

         }
         catch (...)
         {

            return;

         }

         m_packeditema.SetSize(pack);


         int iPat = 0;
         int iGrad = 0;

         for (nCount = 0; nCount < pack; nCount++)
         {
            ar >> (int &)m_packeditema[nCount].m_bOwn;
            ar >> (int &)m_packeditema[nCount].m_etype;

            if (m_packeditema[nCount].m_etype == packed_type_dib)
            {
               m_packeditema[nCount].m_pdib = &m_Patterns[iPat];

               iPat++;

            }
            else if (m_packeditema[nCount].m_etype == packed_type_gradient)
            {
               m_packeditema[nCount].m_pgradient = &m_Gradients[iGrad];

               iGrad++;

            }
         }

         

      }

   }
}

/////////////////////////////////////////////////////////////////////////////
// CPCDraftDoc diagnostics

#ifdef _DEBUG
void CPCDraftDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CPCDraftDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPCDraftDoc commands

// Infinisys Ltd. 2021
double CPCDraftDoc::NoZoomToZoom(double d)
{
   if (m_fZoomFactor) return double(d*m_fZoomFactor);
   if (m_nZoomFactor < 0)
      d *= pow(2.0,-m_nZoomFactor);
   else if (m_nZoomFactor > 0)
      d *= pow(2.0, m_nZoomFactor);
   return d;
}


long CPCDraftDoc::NoZoomToZoom(long n)
{
   if(m_fZoomFactor) return long(n*m_fZoomFactor);
   if(m_nZoomFactor<0)
      n>>=-m_nZoomFactor;
   else if(m_nZoomFactor>0)
      n<<=m_nZoomFactor;
   return n;
}

WrFixed CPCDraftDoc::NoZoomToZoom(WrFixed f)
{
   if(m_fZoomFactor) return f*m_fZoomFactor;
   if(m_nZoomFactor<0)
      f>>=-m_nZoomFactor;
   else if(m_nZoomFactor>0)
      f<<=m_nZoomFactor;
   return f;
}

long CPCDraftDoc::ZoomToNoZoom(long n)
{
   if (m_fZoomFactor) return n / m_fZoomFactor;
   if (m_nZoomFactor < 0)
      n <<= -m_nZoomFactor;
   else if (m_nZoomFactor > 0)
      n >>= m_nZoomFactor;
   return n;
}

double CPCDraftDoc::ZoomToNoZoom(double d)
{
   if (m_fZoomFactor) return d / m_fZoomFactor;
   if (m_nZoomFactor < 0)
      d *= pow(2.0, m_nZoomFactor);
   else if (m_nZoomFactor > 0)
      d *= pow(2.0, -m_nZoomFactor);
   return d;
}


WrFixed CPCDraftDoc::ZoomToNoZoom(WrFixed f)
{
   if(m_fZoomFactor) return f/m_fZoomFactor;
   if(m_nZoomFactor<0)
      f<<=-m_nZoomFactor;
   else if(m_nZoomFactor>0)
      f>>=m_nZoomFactor;
   return f;
}

long CPCDraftDoc::LPtoDP(WrFixed f)
{
   if(m_fZoomFactor) return long(f*m_fZoomFactor);
   /* if(m_nZoomFactor<0)
         n>>=-m_nZoomFactor;
      else if(m_nZoomFactor>0)
         n<<=m_nZoomFactor;
   */
// return ((f.GetFixed()/*+0x8000*/)>>(16-m_nZoomFactor));
   return ((f.GetFixed()+(0x7FFF>>m_nZoomFactor))>>(16-m_nZoomFactor));
}

double CPCDraftDoc::LPtoDPF(WrFixed f)
{
   if (m_fZoomFactor) return double(f*m_fZoomFactor);
   /* if(m_nZoomFactor<0)
         n>>=-m_nZoomFactor;
      else if(m_nZoomFactor>0)
         n<<=m_nZoomFactor;
   */
   // return ((f.GetFixed()/*+0x8000*/)>>(16-m_nZoomFactor));
   return ((double)f.GetFixed()) / ((double) (1 << (16 - m_nZoomFactor)));
}

WrFixed CPCDraftDoc::DPtoLP(long n)
{
   if(m_fZoomFactor) return n/m_fZoomFactor;
   /* if(m_nZoomFactor<0)
         n<<=-m_nZoomFactor;
      else if(m_nZoomFactor>0)
         n>>=m_nZoomFactor;
   */
   return WrFixedFromFixed(n<<(16-m_nZoomFactor));
}

Coord CPCDraftDoc::DPtoLP(double d) // Infinisys Ltd. GDI+
{
   
   if (m_fZoomFactor) return d / m_fZoomFactor;

   /* if(m_nZoomFactor<0)
         n<<=-m_nZoomFactor;
      else if(m_nZoomFactor>0)
         n>>=m_nZoomFactor;
   */
   
   int iZoom = 1 << (16 - m_nZoomFactor);
   
   double dZoom = iZoom;

   d = d * dZoom;

   Fixed f = d;

   return WrFixedFromFixed(f);

}

LongPoint CPCDraftDoc::DPtoLP(const pointd& point) // Infinisys Ltd. GDI+
{

   pointd p = point;

   if (m_bShowRulers)
   {
      p.x -= (kLRulerWid + 1);
      p.y -= (kTRulerWid + 1);
      //p.x -= kLRulerWid + 1;
      //p.y -= kTRulerWid + 1;
      //p.x -= kLRulerWid;
      //p.y -= kTRulerWid;
   }

   return LongPoint(DPtoLP(p.x), DPtoLP(p.y));

}


CSize CPCDraftDoc::GetTotalDevSize()
{
   CSize theSize;
   theSize.cx=m_PageSize.cx*m_NumPage.x;
   theSize.cy=m_PageSize.cy*m_NumPage.y;
   if(theSize.cx>kMaxDocH)
      theSize.cx=kMaxDocH;
   if(theSize.cy>kMaxDocV)
      theSize.cy=kMaxDocV;
   theSize.cx=NoZoomToZoom(theSize.cx);
   theSize.cy=NoZoomToZoom(theSize.cy);
   return theSize;
}


void CPCDraftDoc::CenterZoom(double dCenterZoom, int nZoomFactor, CPoint ptScroll) // PC Draft 7
{


   if (nZoomFactor != m_nZoomFactor || dCenterZoom != m_fZoomFactor)
   {
      
      m_fPrevZoomFactor = m_fZoomFactor;
      m_nPrevZoomFactor = m_nZoomFactor;
      m_fZoomFactor = dCenterZoom;
      m_nZoomFactor = nZoomFactor;
      m_ptPrevZoomScroll = ptScroll;

      UpdateAllViews(NULL, kZoomFactorChanged);
      SetModifiedFlag();
   }

}

#ifdef _FLOAT_ZOOM
void CPCDraftDoc::DoZoom(double fZoomShift,int nZoomShift,CPoint ptScroll)
{
   double fPrevZoomFactor=m_fZoomFactor;
   int nPrevZoomFactor=m_nZoomFactor;

   if(m_fZoomFactor) m_fZoomFactor*=fZoomShift;
   else m_fZoomFactor=fZoomShift*FloatTheFixedZoomFactor(m_nZoomFactor);
   m_nZoomFactor+=nZoomShift;
   if(m_fZoomFactor==FloatTheFixedZoomFactor(m_nZoomFactor))
      m_fZoomFactor=0;

   if(m_nZoomFactor>m_nMaxZoom || m_fZoomFactor>FloatTheFixedZoomFactor(m_nMaxZoom))
   {
      m_fZoomFactor=0;
      m_nZoomFactor=m_nMaxZoom;
   }
   else if(m_nZoomFactor<-3 || (m_fZoomFactor && m_fZoomFactor<FloatTheFixedZoomFactor(-3))) // min zoom
   {
      m_fZoomFactor=0;
      m_nZoomFactor=-3;
   }
   if(nPrevZoomFactor!=m_nZoomFactor || fPrevZoomFactor!=m_fZoomFactor)
   {
      m_fPrevZoomFactor=fPrevZoomFactor;
      m_nPrevZoomFactor=nPrevZoomFactor;
      m_ptPrevZoomScroll=ptScroll;

      UpdateAllViews(NULL,kZoomFactorChanged);
      SetModifiedFlag();
   }
}

#else
void CPCDraftDoc::DoZoom(int nZoomShift,CPoint ptScroll)
{
   m_fPrevZoomFactor=m_fZoomFactor;
   m_nPrevZoomFactor=m_nZoomFactor;
   m_ptPrevZoomScroll=ptScroll;
   m_fZoomFactor=0;
   m_nZoomFactor+=nZoomShift;
   UpdateAllViews(NULL,kZoomFactorChanged);
   SetModifiedFlag();
}
#endif

CSize CPCDraftDoc::CalcPrinterPageSize()
{
   //CSize theSize;
   //m_PrinterSettings.SetThisPrinter();
   //PRINTDLG PrintDlg;
   //CDC theDC;
   ////int bListPageSize=1;
   ////if (bListPageSize)
   ////{
   ////   CString strCode;
   ////   for (int i = DMPAPER_FIRST; i <= DMPAPER_LAST; i++)
   ////   {
   ////      m_PrinterSettings.SetPageSize(i);
   ////      m_PrinterSettings.SetThisPrinter();
   ////      PRINTDLG PrintDlg;
   ////      CDC theDC;

   ////      if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintDlg) && AfxGetApp()->CreatePrinterDC(theDC))
   ////      {

   ////         CSize size = CSize(::MulDiv(theDC.GetDeviceCaps(HORZSIZE), 720 * 100, 254 *
   ////            m_PrinterSettings.GetPageScale()), ::MulDiv(theDC.GetDeviceCaps(VERTSIZE), 720 * 100, 254
   ////               * m_PrinterSettings.GetPageScale()));

   ////         CString strW;
   ////         CString strH;
   ////         CString strDm;
   ////         strW.Format("%d", size.cx);
   ////         strH.Format("%d", size.cy);
   ////         strDm.Format("%d", i);
   ////         
   ////         strCode += ("else if(phDocState.fPrintRec.prInfo.rPage.r == " + strW + "\n");
   ////         strCode += ("&& phDocState.fPrintRec.prInfo.rPage.b == "+strH+")\n");
   ////         strCode += ("{\n");
   ////         strCode += ("\n");
   ////         strCode += ("  m_PrinterSettings.SetPageSize(" + strDm + ");\n");
   ////         strCode += (" m_PrinterSettings.SetPageOrientation(DMORIENT_PORTRAIT);bFound = true;\n");
   ////         strCode += ("\n");
   ////         strCode += ("}\n");
   ////         strCode += ("else if(phDocState.fPrintRec.prInfo.rPage.r == " + strH + "\n");
   ////         strCode += ("&& phDocState.fPrintRec.prInfo.rPage.b == " + strW + ")\n");
   ////         strCode += ("{\n");
   ////         strCode += ("\n");
   ////         strCode += ("  m_PrinterSettings.SetPageSize(" + strDm + ");\n");
   ////         strCode += (" m_PrinterSettings.SetPageOrientation(DMORIENT_LANDSCAPE); bFound = true;\n");
   ////         strCode += ("\n");
   ////         strCode += ("}\n");

   ////         //if (size.cx == 575 && size.cy == 819)
   ////         //{
   ////         //   OutputDebugString("Test");
   ////         //}
   ////      }
   ////      m_PrinterSettings.RestorePrinter();
   ////   }
   ////   OutputDebugString(strCode);

   ////}
   ////if(AfxGetApp()->GetPrinterDeviceDefaults(&PrintDlg) && AfxGetApp()->CreatePrinterDC(theDC))
   //if (AfxGetApp()->CreatePrinterDC(theDC))
   //{

   //   int iHorizontal = theDC.GetDeviceCaps(HORZSIZE);
   //   int iVertical = theDC.GetDeviceCaps(VERTSIZE);

   //   int cx = ::MulDiv(iHorizontal, 720 * 100, 254 * m_PrinterSettings.GetPageScale());

   //   int cy = ::MulDiv(iVertical, 720 * 100, 254 * m_PrinterSettings.GetPageScale());
   //    
   //   theSize=CSize(cx,cy);

   //}
   //else theSize=CSize(558,805);
   //m_PrinterSettings.RestorePrinter();
   //return theSize;

   CSize theSize;
   //CSize theCustomPageSize = m_PrinterSettings.GetCustomPageSize();
   //if (theCustomPageSize.cx > 0 && theCustomPageSize.cy > 0)
   //{
   //   m_PrinterSettings.defer_set_page_size(theCustomPageSize.cx, theCustomPageSize.cy);
   //}
   //else
   //{
   //   m_PrinterSettings.SetPageSize(m_PrinterSettings.GetPageSize());
   //}
   m_PrinterSettings.SetThisPrinter();
   PRINTDLG PrintDlg;
   CDC theDC;
#ifdef DEBUG_LIST_PAGE_SIZE
   {
      CString strCode;
      for (int i = DMPAPER_FIRST; i <= DMPAPER_LAST; i++)
      {
         m_PrinterSettings.SetPageSize(i);
         m_PrinterSettings.SetThisPrinter();
         PRINTDLG PrintDlg;
         CDC theDC;

         if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintDlg) && AfxGetApp()->CreatePrinterDC(theDC))
         {

            CSize size = CSize(::MulDiv(theDC.GetDeviceCaps(HORZSIZE), 720 * 100, 254 *
               m_PrinterSettings.GetPageScale()), ::MulDiv(theDC.GetDeviceCaps(VERTSIZE), 720 * 100, 254
                  * m_PrinterSettings.GetPageScale()));

            CString strW;
            CString strH;
            CString strDm;
            strW.Format("%d", size.cx);
            strH.Format("%d", size.cy);
            strDm.Format("%d", i);
            
            strCode += ("else if(phDocState.fPrintRec.prInfo.rPage.r == " + strW + "\n");
            strCode += ("&& phDocState.fPrintRec.prInfo.rPage.b == "+strH+")\n");
            strCode += ("{\n");
            strCode += ("\n");
            strCode += ("  m_PrinterSettings.SetPageSize(" + strDm + ");\n");
            strCode += (" m_PrinterSettings.SetPageOrientation(DMORIENT_PORTRAIT);bFound = true;\n");
            strCode += ("\n");
            strCode += ("}\n");
            strCode += ("else if(phDocState.fPrintRec.prInfo.rPage.r == " + strH + "\n");
            strCode += ("&& phDocState.fPrintRec.prInfo.rPage.b == " + strW + ")\n");
            strCode += ("{\n");
            strCode += ("\n");
            strCode += ("  m_PrinterSettings.SetPageSize(" + strDm + ");\n");
            strCode += (" m_PrinterSettings.SetPageOrientation(DMORIENT_LANDSCAPE); bFound = true;\n");
            strCode += ("\n");
            strCode += ("}\n");

            //if (size.cx == 575 && size.cy == 819)
            //{
            //   OutputDebugString("Test");
            //}
         }
         m_PrinterSettings.RestorePrinter();
      }
      OutputDebugString(strCode);

   }
#endif
   if(AfxGetApp()->GetPrinterDeviceDefaults(&PrintDlg) && AfxGetApp()->CreatePrinterDC(theDC))
//   if (AfxGetApp()->CreatePrinterDC(theDC))
   {

      int iHorizontal = theDC.GetDeviceCaps(HORZSIZE);
      int iVertical = theDC.GetDeviceCaps(VERTSIZE);

      int iPageScale = m_PrinterSettings.GetPageScale();

      int cx = ::MulDiv(iHorizontal, 720 * 100, 254 * iPageScale);

      int cy = ::MulDiv(iVertical, 720 * 100, 254 * iPageScale);

      theSize = CSize(cx, cy);

   }
   else theSize = CSize(558, 805);
   m_PrinterSettings.RestorePrinter();
   return theSize;

}


CSize CPCDraftDoc::get_synthesized_printer_page_size()
{

   CSize theSize;
   
   theSize = m_PrinterSettings.GetCustomPageSize();

   if (theSize.cx <= 0 || theSize.cy <= 0)
   {

      int iPageSize = m_PrinterSettings.GetPageSize();

      int iOrientation = m_PrinterSettings.GetPageOrientation();

      auto ppagesize = page_size::from_index(iPageSize, iOrientation);

      if (ppagesize)
      {

         theSize = ppagesize->get_size();

      }

   }

   return theSize;

}

int CPCDraftDoc::get_synthesized_printer_page_size_index()
{

   int iPageSize = m_PrinterSettings.GetPageSize();

   if (iPageSize == 0 || iPageSize == 256)
   {

      CSize theSize = get_synthesized_printer_page_size();

      auto ppagesize = page_size::from_size(theSize.cx, theSize.cy);

      if (ppagesize)
      {

         iPageSize = ppagesize->m_iPageSize;

      }

   }

   return iPageSize;

}


void CPCDraftDoc::OnFilePageSetup()
{
   if(m_PrinterSettings.PrinterSetup(AfxGetMainWnd()))
   {
      CSize thePageSize=CalcPrinterPageSize();
      if(thePageSize!=m_PageSize)
      {
         m_PageSize=thePageSize;
         CheckDocSize(true);
         UpdateAllViews(NULL,kDocumentSizeChanged);
         SetModifiedFlag();
      }
   }
}

bool CPCDraftDoc::SetupUnits(int nUnit,bool bHideMetricUnits,int nAngleUnit,int nPlaces,int nAnglePlaces,int nScaleIndex,DimStandard* pDimStd,int nAlertID, LongRatio * plongratioCustom)
{
   bool bResult=true;
   int nNewDisplayRes=(nUnit & kUEnglishMask)?(/*(nUnit & kUEnglishFracMask)*/nUnit!=kUDecFeet?kEngRes:kEngDecRes):kMetRes;
   LongRatio lratioCur; // Free Scale - Infinisys Ltd
   LongRatio lratioNew;// Free Scale - Infinisys Ltd

   GetScaleRatio(m_nDefScale,lratioCur);// Free Scale - Infinisys Ltd

   if(nScaleIndex == -1 && plongratioCustom != NULL)// Free Scale - Infinisys Ltd
   {
      lratioNew = *plongratioCustom;
      m_ratioCustom = lratioNew;
   }
   else
   {
      GetScaleRatio(nUnit & kUEnglishMask,nScaleIndex,lratioNew);
   }

   if(nNewDisplayRes == m_nDisplayRes)
   {

      if(nScaleIndex>=-1 && lratioNew != lratioCur)// Free Scale - Infinisys Ltd
      {
         for(int i=0; i<m_Layers.GetSize(); i++)
         {
            CLayer* pLayer=m_Layers[i];
            if(pLayer->m_nScale==m_nDefScale)
            {
               if(pLayer->m_nScale == -1 || nScaleIndex == -1)
               {
                  if(!pLayer->Rescale(-1, this, lratioCur, lratioNew))
                  {
                     if(bResult) AfxMessageBox(IDS_NOT_COMPLETED);
                     bResult=false;
                  }
                  else
                  {
                     pLayer->m_nScale = nScaleIndex;
                  }
               }
               else
               {
                  if(!pLayer->SetScale(nScaleIndex,this))
                  {
                     if(bResult) AfxMessageBox(IDS_NOT_COMPLETED);
                     bResult=false;
                  }
               }
            }
         }
      }
   }
   else
   {
      double resChange=CalcResChangeFactor(nNewDisplayRes);
      for(int i=0; i<m_Layers.GetSize(); i++)
      {
         CLayer* pLayer=m_Layers[i];
         int nScale=pLayer->m_nScale;
         LongRatio srcRatio,dstRatio;
         GetScaleRatio(nScale,srcRatio);
         bool bDefScale=(nScaleIndex>=0 && nScale==m_nDefScale);
         if(MatchScale(nScale,nUnit,m_nUnit))
         {
            if(bDefScale) nScale=nScaleIndex;
            if(nScaleIndex)// Free Scale - Infinisys Ltd
            {
               dstRatio.fN=lratioNew.fN;
               dstRatio.fD=lratioNew.fD;
            }
            else if(nUnit & kUEnglishMask)
            {
               dstRatio.fN=EnglishScales[nScale].fN;
               dstRatio.fD=EnglishScales[nScale].fD;
            }
            else
            {
               dstRatio.fN=MetricScales[nScale].fN;
               dstRatio.fD=MetricScales[nScale].fD;
            }
            if(!pLayer->Rescale(nScale,this,srcRatio,dstRatio,resChange))
            {
               if(bResult) AfxMessageBox(IDS_NOT_COMPLETED);
               bResult=false;
            }
         }
         else
         {
            if(bResult)
            {
               bResult=false;
               if(nAlertID)
               {
                  CString strErr;
                  strErr.Format(nAlertID,pLayer->m_strName);
                  AfxMessageBox(strErr);
               }
            }
         }
      }
      m_nDisplayRes=nNewDisplayRes;
   }

   m_bEnglishUnits=(nUnit & kUEnglishMask)?true:false;
   m_nUnit=nUnit;
   m_bHideMetricUnits=bHideMetricUnits;
   m_nPlaces=nPlaces;
   m_snapTable.setAngleUnit(nAngleUnit);
   m_nAnglePlaces=nAnglePlaces;
   m_nDefScale=nScaleIndex;
   if(pDimStd) m_DimStd=*pDimStd;
   else m_DimStd.FillDefaults(m_bEnglishUnits?kDsEnglish:kDsMetric);
   CLayer* pActiveLayer=GetActiveLayer();
   m_DimUnit.m_ScaleIndex=pActiveLayer?pActiveLayer->m_nScale:m_nDefScale;
   m_DimUnit.setDimUnits(m_nUnit,m_bHideMetricUnits,m_nPlaces,m_nDisplayRes,GetAngleUnit(),m_nAnglePlaces);
   m_DimFmts[kLinear].FillDefaults(kLinear,m_DimStd.nType);
   m_DimFmts[kRadial].FillDefaults(kRadial,m_DimStd.nType);
   m_DimFmts[kAngular].FillDefaults(kAngular,m_DimStd.nType);

   SetModifiedFlag();
   return bResult;
}

void CPCDraftDoc::LayersChanged()
{
   UpdateFrameCounts();
   UpdateMaxZoom();
   UpdateAllViews(NULL,kRefresh);
   SetModifiedFlag();
}

void CPCDraftDoc::RemoveAllLayers()
{
   for(int i=0; i<m_Layers.GetSize(); i++)
   {
      CLayer* pLayer = m_Layers.GetAt(i);
      if (pLayer)
      {
         while ( !pLayer->m_ShapeList.IsEmpty() )
         {
            CMDShape* pShape = pLayer->m_ShapeList.GetTail();

            if (pShape)
            {
               pShape->DeleteLinks(&pLayer->m_LinkVertexList);
               delete pShape;
               pShape = NULL;
            }
            pLayer->m_ShapeList.RemoveTail();
         }
      }
      delete m_Layers.GetAt(i);
   }
   m_Layers.RemoveAll();
   m_nActiveLayer=-1;
}

bool CPCDraftDoc::CheckLayerName(CString& sName)
{
   for(int i=0; i<m_Layers.GetSize(); i++)
   {
      if(m_Layers[i]->m_strName==sName) return false;
   }
   return true;
}

bool CPCDraftDoc::CheckReportName(CString& sName)
{
   for(int i=0; i<m_Reports.GetSize(); i++)
   {
      if(m_Reports[i]->m_sTitle==sName) return false;
   }
   return true;
}

CMDReport* CPCDraftDoc::GetReportByTag(DWORD dwTag)
{
   for(int i=0; i<m_Reports.GetSize(); i++)
   {
      CMDReport* pReport=m_Reports[i];
      if(pReport->GetTag()==dwTag)
         return pReport;
   }
   return NULL;
}

CLayer* CPCDraftDoc::AddLayer(CString* pName)
{
   if(m_nActiveLayer>=0)
   {
//    GetActiveLayer()->m_bGrayed=true;
      if(m_bActiveLayerHidden)
      {
         GetActiveLayer()->m_bHidden=true;
         m_bActiveLayerHidden=false;
      }
   }
   CLayer* pLayer=new CLayer;
   if(pName)
   {
      while(!CheckLayerName(*pName))
      {
         CRenameDlg theDlg(*pName,IDS_LAYER);
         if(theDlg.DoModal()==IDOK) *pName=get_cp(get_uni(theDlg.m_sName, CP_ACP));
         else
         {
            delete pLayer;
            return NULL;
         }
      }
      pLayer->m_strName=*pName;
   }
   else
   {
      CString sName;
      int n=m_Layers.GetSize()+1;
      do
      {
         CStringW wstr;
         wstr.Format(IDS_LAYERNAME,n++);
         sName = get_cp(wstr,CP_UTF8);
      } while(!CheckLayerName(sName));
      pLayer->m_strName=sName;
   }
   pLayer->m_nScale=m_nDefScale;
   pLayer->m_nSnap=3;
   m_nActiveLayer=m_Layers.Add(pLayer);
   UpdateAllViews(NULL,kActivateLayer,pLayer);
   SetModifiedFlag();
   return pLayer;
}

CLayer* CPCDraftDoc::AddLayerSorted(CString* pName)
{
   if(m_nActiveLayer>=0)
   {
//    GetActiveLayer()->m_bGrayed=true;
      if(m_bActiveLayerHidden)
      {
         GetActiveLayer()->m_bHidden=true;
         m_bActiveLayerHidden=false;
      }
   }
   CLayer* pNewLayer=new CLayer;
   pNewLayer->m_strName=*pName;
   int i = 0;
   for(; i<GetLayersCount(); i++)
   {
      CLayer *pLayer = GetLayer(i);
      if(pLayer->m_strName >= *pName) break;
   }
   pNewLayer->m_nScale=m_nDefScale;
   pNewLayer->m_nSnap=3;
   //m_nActiveLayer=
   m_Layers.InsertAt(i, pNewLayer);
   UpdateAllViews(NULL,kActivateLayer,pNewLayer);
   SetModifiedFlag();
   return pNewLayer;
}

void CPCDraftDoc::DeleteLayer(int nIndex)
{
   UpdateAllViews(NULL,kDeactivateTextShape);
// bool bChangeActive=false;
   delete m_Layers.GetAt(nIndex);
   m_Layers.RemoveAt(nIndex);
   if(nIndex<m_nActiveLayer)
   {
      --m_nActiveLayer;
//    bChangeActive=true;
   }
   else if(nIndex==m_nActiveLayer)
   {
      if(m_nActiveLayer==m_Layers.GetSize())
      {
         m_nActiveLayer--;
//       bChangeActive=true;
      }
      CLayer* pLayer=GetActiveLayer();
//    pLayer->m_bGrayed=false;
      if(pLayer->m_bHidden)
      {
         pLayer->m_bHidden=false;
         m_bActiveLayerHidden=true;
      }
      UpdateAllViews(NULL,kActivateLayer,GetActiveLayer());
   }
// if(bChangeActive)
//    UpdateAllViews(NULL,kActivateLayer,GetActiveLayer());
   SetModifiedFlag();
}

void CPCDraftDoc::MoveLayer(int nOldIndex,int nNewIndex)
{
   bool bChangeActive=false;
   if(nOldIndex==m_nActiveLayer)
   {
      m_nActiveLayer=nNewIndex;
      bChangeActive=true;
   }
   else
   {
      if(m_nActiveLayer>nOldIndex)
      {
         if(m_nActiveLayer<=nNewIndex)
         {
            --m_nActiveLayer;
            bChangeActive=true;
         }
      }
      else
      {
         if(m_nActiveLayer>=nNewIndex)
         {
            ++m_nActiveLayer;
            bChangeActive=true;
         }
      }
   }
   CLayer* pLayer=m_Layers.GetAt(nOldIndex);
   m_Layers.RemoveAt(nOldIndex);
   m_Layers.InsertAt(nNewIndex,pLayer);
   if(bChangeActive)
      UpdateAllViews(NULL,kActivateLayer,GetActiveLayer());
   SetModifiedFlag();
}

// Layer - Infinisys Ltd.
void CPCDraftDoc::MoveLayer(CArray < int > & ia,int nNewIndex)
{

   CLayer * playerActive = GetActiveLayer();
   CArray < CLayer * > lptra;
   for(int i = 0; i < ia.GetSize(); i++)
   {
      lptra.Add(m_Layers.ElementAt(ia[i]));
   }
   for(int i = 0; i < lptra.GetSize(); i++)
   {
      for(int j= 0; j < m_Layers.GetSize(); j++)
      {
         if(lptra[i] == m_Layers[j])
         {
            m_Layers.RemoveAt(j);
            if(nNewIndex > j)
            {
               nNewIndex--;
            }
            break;
         }
      }
   }
   for(int i = lptra.GetUpperBound(); i >= 0; i--)
   {
      m_Layers.InsertAt(nNewIndex,lptra[i]);
   }
   for(int i = 0; i < m_Layers.GetCount(); i++)
   {
      if(m_Layers[i] == playerActive)
      {
         m_nActiveLayer = i;
      }
   }
   ia.RemoveAll();
   for(int i = 0; i < lptra.GetSize(); i++)
   {
      for(int j= 0; j < m_Layers.GetSize(); j++)
      {
         if(lptra[i] == m_Layers[j])
         {
            ia.Add(j);
            break;
         }
      }
   }
   UpdateAllViews(NULL,kActivateLayer,GetActiveLayer());
   SetModifiedFlag();
}

void CPCDraftDoc::MergeLayer(int nCount,int* pIndexes)
{
   CLayer* pActiveLayer=GetActiveLayer();
// pActiveLayer->m_bGrayed=true;
   if(m_bActiveLayerHidden)
   {
      pActiveLayer->m_bHidden=true;
      m_bActiveLayerHidden=false;
   }
   ASSERT(nCount>1);
   m_nActiveLayer=pIndexes[0];
   pActiveLayer=GetActiveLayer();
   for(int i=nCount; --i>0;)
   {
      CLayer* pLayer=GetLayer(pIndexes[i]);
      while(!pLayer->m_ShapeList.IsEmpty())
         pActiveLayer->m_ShapeList.AddTail(pLayer->m_ShapeList.RemoveHead());
      delete pLayer;
      m_Layers.RemoveAt(pIndexes[i]);
   }

   if(pActiveLayer->m_bHidden)
   {
      pActiveLayer->m_bHidden = false;
      m_bActiveLayerHidden = true;
   }

   pActiveLayer->m_strName.Format(IDS_MERGEDLAYERNAME,m_nActiveLayer+1);
// pActiveLayer->m_bGrayed=false;
   UpdateAllViews(NULL,kActivateLayer,pActiveLayer);
   SetModifiedFlag();
}

bool CPCDraftDoc::SetActiveLayer(int nIndex)
{
   if(m_nActiveLayer!=nIndex)
   {
      CLayer* pLayer=GetActiveLayer();
//    pLayer->m_bGrayed=true;
      if(m_bActiveLayerHidden)
      {
         pLayer->m_bHidden=true;
         m_bActiveLayerHidden=false;
      }
      m_nActiveLayer=nIndex;
      pLayer=GetActiveLayer();
//    pLayer->m_bGrayed=false;
      if(pLayer->m_bHidden)
      {
         m_bActiveLayerHidden=true;
         pLayer->m_bHidden=false;
      }
      UpdateAllViews(NULL,kActivateLayer,pLayer);
      SetModifiedFlag();
      return true;
   }
   return false;
}

void CPCDraftDoc::DeleteContents()
{
   RemoveAllLayers();
   RemoveAllSavedViews();

   for(int i=0; i<m_Reports.GetSize(); i++)
      delete m_Reports.GetAt(i);
   m_Reports.RemoveAll();

   m_Palette.DeleteObject();

// for(i=0;i<71;i++) if(m_DIBPatterns[i]) DeleteObject(m_DIBPatterns[i]);

   CDocument::DeleteContents();
}

CSize CPCDraftDoc::GetPageDevSize()
{
   return CSize(
          NoZoomToZoom(m_PageSize.cx),
          NoZoomToZoom(m_PageSize.cy));
}

LongPoint CPCDraftDoc::DPtoLP(const CPoint& inPoint)
{
   CPoint pt=inPoint;
   if(m_bShowRulers)
   {
      pt.x-=(kLRulerWid+1);
      pt.y-=(kTRulerWid+1);
      //pt.x-=kLRulerWid+1;
      //pt.y-=kTRulerWid+1;
      //pt.x-=kLRulerWid;
      //pt.y-=kTRulerWid;
   }
   return LongPoint(DPtoLP(pt.x),DPtoLP(pt.y));
}

LongRect CPCDraftDoc::DPtoLP(const CRect& inRect)
{
   CRect rect=inRect;
   if (m_bShowRulers)
   {
      rect.OffsetRect(-kLRulerWid - 1, -kTRulerWid - 1);
      //rect.OffsetRect(-kLRulerWid - 1, -kTRulerWid - 1);
      //rect.OffsetRect(-kLRulerWid, -kTRulerWid);
   }
   return LongRect(DPtoLP(rect.left),DPtoLP(rect.top),DPtoLP(rect.right),DPtoLP(rect.bottom));
}

CPoint CPCDraftDoc::LPtoDP(const LongPoint& inPoint)
{
   CPoint point(LPtoDP(inPoint.x),LPtoDP(inPoint.y));
   if (m_bShowRulers)
   {
      point.Offset(kLRulerWid + 1, kTRulerWid + 1);
      //point.Offset(kLRulerWid+1,kTRulerWid+1);
      //point.Offset(kLRulerWid, kTRulerWid);
   }
   return point;
}

pointd CPCDraftDoc::LPtoDPF(const LongPoint& inPoint)
{
   pointd point(LPtoDPF(inPoint.x), LPtoDPF(inPoint.y));
   if (m_bShowRulers)
   {
      point.Offset(kLRulerWid + 1.0, kTRulerWid + 1.0);
      //point.Offset(kLRulerWid + 1, kTRulerWid + 1);
      //point.Offset(kLRulerWid, kTRulerWid);
   }
   return point;
}

void CPCDraftDoc::LPtoDP(pointd & p, const LongPoint& inPoint)
{
   
   p.x = LPtoDPF(inPoint.x);
   
   p.y = LPtoDPF(inPoint.y);
   
   if (m_bShowRulers)
   {

      p += pointd(kLRulerWid + 1.0, kTRulerWid + 1.0);
      //p += pointd(kLRulerWid + 1, kTRulerWid + 1);
      //p += pointd(kLRulerWid, kTRulerWid);

   }

}

CRect CPCDraftDoc::LPtoDP(const LongRect& inRect)
{
   CRect rect(LPtoDP(inRect.left),LPtoDP(inRect.top),LPtoDP(inRect.right),LPtoDP(inRect.bottom));
   if (m_bShowRulers)
   {
      rect.OffsetRect(kLRulerWid + 1, kTRulerWid + 1);
      //rect.OffsetRect(kLRulerWid + 1, kTRulerWid + 1);
      //rect.OffsetRect(kLRulerWid, kTRulerWid);
   }
   return rect;
}

void CPCDraftDoc::CreateShape(CMDShape* pNewShape,CMDShape* pReplaceShape)
{
   if(pReplaceShape)
   {
      UpdateAllViews(NULL,kRefreshShape,pReplaceShape);
      GetActiveLayer()->ReplaceShape(pReplaceShape,pNewShape);
   }
   else GetActiveLayer()->m_ShapeList.AddHead(pNewShape);
   UpdateAllViews(NULL,kRefreshShape,pNewShape);
   SetModifiedFlag();
}

void CPCDraftDoc::CreateShapes(CMDShapeList* pNewShapes,CMDShape* pReplaceShape)
{
   if(pReplaceShape)
   {
      UpdateAllViews(NULL,kRefreshShape,pReplaceShape);
      GetActiveLayer()->ReplaceShape(pReplaceShape,pNewShapes);
   }
   else GetActiveLayer()->m_ShapeList.AddHead(pNewShapes);
   UpdateAllViews(NULL,kRefreshShapes,pNewShapes);
   SetModifiedFlag();
}

void CPCDraftDoc::DeleteShape(CMDShape *pShape)
{
   CLayer* pLayer=GetActiveLayer();
   POSITION pos=pLayer->m_ShapeList.Find(pShape);
   if(pos)
   {
      pShape->DeleteLinks(&pLayer->m_LinkVertexList);
      pLayer->m_ShapeList.RemoveAt(pos);
      delete pShape;
   }
   SetModifiedFlag();
}

CPCDraftDoc* CPCDraftDoc::GetActiveDocument()
{
   CDocument* pDoc=((CFrameWnd*)AfxGetMainWnd())->GetActiveFrame()->GetActiveDocument();
   return (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CPCDraftDoc)))?(CPCDraftDoc*)pDoc:NULL;
}

void CPCDraftDoc::OnEditLineStyleRange(UINT nID)
{
   CEditLineStyleDlg theDlg(m_DashStyles+(nID-ID_EDIT_STYLES_BASE)*16);
   if(theDlg.DoModal()==IDOK)
   {
      UpdateAllViews(NULL,kRefresh);
      SetModifiedFlag();
   }
}

void CPCDraftDoc::OnNewColor()// New color/New Pattern - Infinisys Ltd
{

   CChooseColorDlg theDlg(RGB(128,128,128),AfxGetMainWnd());
   if(theDlg.DoModal() == IDOK)
   {
      PALETTEENTRY palEntry;
      palEntry.peRed=GetRValue(theDlg.GetColor());
      palEntry.peGreen=GetGValue(theDlg.GetColor());
      palEntry.peBlue=GetBValue(theDlg.GetColor());
      int iNewSize = m_Palette.GetEntryCount() + 1;
      m_Palette.ResizePalette(iNewSize);
      m_Palette.SetPaletteEntries(iNewSize - 1, 1, &palEntry);
      UpdateAllViews(NULL,kRefresh);
      SetModifiedFlag();
   }

}


void CPCDraftDoc::OnNewPattern()// New color/New pattern - Infinisys Ltd
{

   // Gradient - Infinisys Ltd
   CPackedItem item;

   item.m_etype = packed_type_dib;
   item.m_pdib = &m_Patterns[m_numDIBPatterns];
   item.m_pdib->LoadResource(IDB_PAT01);

   HBITMAP hDib=item.m_pdib->CreateDIBSection();

   CEditPatternDlg theDlg(&hDib); //m_DIBPatterns+pattern-1);
   if(theDlg.DoModal() == IDOK)
   {
      item.m_pdib->FromDIBSection(hDib);
      m_PatternsHash[item.m_pdib - m_Patterns] = CalculateDIBHash(hDib);

      m_packeditema.Add(item);

      m_numDIBPatterns++;

      ::DeleteObject(hDib);
      UpdateAllViews(NULL,kRefresh);
      SetModifiedFlag();
   }


}

void CPCDraftDoc::OnNewGradient()// Gradient - Infinisys Ltd
{

   m_Gradients[m_numGradients].reset();
   m_Gradients[m_numGradients].m_da.Add(0.0);
   m_Gradients[m_numGradients].m_cra.Add(RGB(255,255,255));
   m_Gradients[m_numGradients].m_da.Add(1.0);
   m_Gradients[m_numGradients].m_cra.Add(RGB(0,0,0));

   CGradientDlg theDlg(this,&m_Gradients[m_numGradients],false);

   int iResult = theDlg.DoModal();

   if(iResult == IDOK || iResult == ID_APPLY_TO_SELECTED)
   {
      CPackedItem item;
      item.m_etype = packed_type_gradient;
      item.m_pgradient = &m_Gradients[m_numGradients];
      m_packeditema.Add(item);
      m_numGradients++;
      if(iResult == ID_APPLY_TO_SELECTED)
      {
         UpdateAllViews(NULL,kApplyLastPackedItem);
      }
   }
   else
   {

      m_Gradients[m_numGradients].reset();

   }

}

void CPCDraftDoc::OnEditPatternRange(UINT nID)
{
   int pattern=nID-ID_PATTERNS_BASE;
   if(pattern > 2)
   {

      // Gradient - Infinisys Ltd
      CPackedItem & item = m_packeditema[pattern - 1];

      if(item.m_etype == packed_type_dib)
      {
         HBITMAP hDib=item.m_pdib->CreateDIBSection();
         CEditPatternDlg theDlg(&hDib); //m_DIBPatterns+pattern-1);
         if(theDlg.DoModal() == IDOK)
         {
            item.m_pdib->FromDIBSection(hDib);
            m_PatternsHash[item.m_pdib - m_Patterns] = CalculateDIBHash(hDib);
            ::DeleteObject(hDib);
            UpdateAllViews(NULL,kRefresh);
            SetModifiedFlag();
         }
      }
      else if(item.m_etype == packed_type_gradient)
      {
         CGradientDlg theDlg(this, item.m_pgradient, true);
         if(theDlg.DoModal() == IDOK)
         {

         }

      }

   }
   else AfxMessageBox(IDS_MAY_NOT_MODIFY);
}

void CPCDraftDoc::OnEditDupPatternRange(UINT nID) // Gradient - Infinisys Ltd
{
   int pattern=nID - ID_DUP_PATTERNS_BASE;
   if(pattern>2)
   {
      CPackedItem & item = m_packeditema[pattern - 1];

      if(item.m_etype == packed_type_dib)
      {
         CPackedDIB & pattern = *item.m_pdib;
         HBITMAP hDib=pattern.CreateDIBSection();
         CEditPatternDlg theDlg(&hDib); //m_DIBPatterns+pattern-1);
         if(theDlg.DoModal() == IDOK)
         {
            m_Patterns[m_numDIBPatterns].FromDIBSection(hDib);
            m_PatternsHash[m_numDIBPatterns] = CalculateDIBHash(hDib);
            CPackedItem item;
            item.m_etype = packed_type_dib;
            item.m_pdib = &m_Patterns[m_numDIBPatterns];
            m_packeditema.Add(item);

            m_numDIBPatterns++;
            ::DeleteObject(hDib);
            UpdateAllViews(NULL,kRefresh);
            SetModifiedFlag();
         }
      }
      else if(item.m_etype == packed_type_gradient)
      {
         m_Gradients[m_numGradients].reset();

         m_Gradients[m_numGradients] = *item.m_pgradient;

         CGradientDlg theDlg(this,&m_Gradients[m_numGradients],false);

         int iResult = theDlg.DoModal();

         if(iResult == IDOK || iResult == ID_APPLY_TO_SELECTED)
         {
            CPackedItem item;
            item.m_etype = packed_type_gradient;
            item.m_pgradient = &m_Gradients[m_numGradients];
            m_packeditema.Add(item);
            m_numGradients++;
            if(iResult == ID_APPLY_TO_SELECTED)
            {
               UpdateAllViews(NULL,kApplyLastPackedItem);
            }
         }
         else
         {
            m_Gradients[m_numGradients].reset();

         }

      }

   }
   else AfxMessageBox(IDS_MAY_NOT_MODIFY);
}

void CPCDraftDoc::OnEditColorRange(UINT nID)
{
   int color=(nID-ID_COLORS_BASE);
   if(color>2)
   {
      PALETTEENTRY palEntry;
      m_Palette.GetPaletteEntries(color-1,1,&palEntry);
      CChooseColorDlg theDlg(RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue),AfxGetMainWnd());
      if(theDlg.DoModal()==IDOK)
      {
         palEntry.peRed=GetRValue(theDlg.GetColor());
         palEntry.peGreen=GetGValue(theDlg.GetColor());
         palEntry.peBlue=GetBValue(theDlg.GetColor());
         m_Palette.SetPaletteEntries(color-1,1,&palEntry);
         UpdateAllViews(NULL,kRefresh);
         SetModifiedFlag();
      }
   }
   else AfxMessageBox(IDS_MAY_NOT_MODIFY);
}

void CPCDraftDoc::OnReportRange(UINT nID)
{
   int report=(nID-ID_REPORTS_BASE);
   if(report<m_Reports.GetSize())
   {
      CReportOutputDlg theDlg;
      if(theDlg.DoModal()==IDOK)
      {
         if(theDlg.m_nTarget==1)
         {
            m_pActiveReport=m_Reports[report];
            ((CPCDraftApp*)AfxGetApp())->m_ReportViewTemplate.CreateView(this,RUNTIME_CLASS(CReportPreView),RUNTIME_CLASS(CReportPreFrame));
         }
         else
         {
            CMDIChildWnd* pNewFrame;
            m_pActiveReport=m_Reports[report];
            if(((CPCDraftApp*)AfxGetApp())->m_ReportViewTemplate.CreateView(this,RUNTIME_CLASS(CReportPreView),RUNTIME_CLASS(CReportPreFrame),FALSE,&pNewFrame))
            {
               pNewFrame->SendMessage(WM_COMMAND,ID_FILE_PRINT);
               pNewFrame->DestroyWindow();
            }
         }
      }
   }
}

void CPCDraftDoc::OnUpdateLinkRange(UINT nID)
{
   int report=(nID-ID_UPDATE_LINK_BASE);
   if(report<m_Reports.GetSize())
   {
      m_Reports[report]->Calculate(this);
      m_Reports[report]->Clone();
   }
}

void CPCDraftDoc::OnBreakLinkRange(UINT nID)
{
   int report=(nID-ID_BREAK_LINK_BASE);
   if(report<m_Reports.GetSize())
   {
      m_Reports[report]->Unlink();
   }
}

void CPCDraftDoc::OnDataReportFormats()
{
   CReportFormatsDlg theDlg(this);
   if(theDlg.DoModal()==IDOK)
   {
      m_pActiveReport=m_Reports[theDlg.m_nSelReport];
      POSITION pos=GetFirstViewPosition();
      while(pos!=NULL)
      {
         CView* pView=GetNextView(pos);
         if(pView->IsKindOf(RUNTIME_CLASS(CReportEditView)) && ((CReportEditView*)pView)->m_pReport==m_pActiveReport)
         {
            pView->GetParentFrame()->ActivateFrame();
            return;
         }
      }
      ((CPCDraftApp*)AfxGetApp())->m_ReportViewTemplate.CreateView(this,RUNTIME_CLASS(CReportEditView),RUNTIME_CLASS(CReportEditFrame));
   }
}

void CPCDraftDoc::OnDataPrintReport()
{
   if(m_Reports.GetSize()>0)
   {
      CMenu theReportMenu;
      theReportMenu.CreatePopupMenu();
      for(int i=0; i<m_Reports.GetSize(); i++)
      {
         CMDReport* pReport=m_Reports[i];
         theReportMenu.AppendMenu(MF_ENABLED | MF_STRING | (i % 20 || 0 == i?0:MF_MENUBARBREAK),ID_REPORTS_BASE + i,pReport->m_sTitle);
      }
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      POSITION pos=GetFirstViewPosition();
      while(pos != NULL)
      {
         CView* pView=GetNextView(pos);
         if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
         {
            int iId = theReportMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y, pView->GetTopLevelFrame());
            if(iId >= 0)
            {
               pView->PostMessage(WM_COMMAND,iId);

            }
            return;
         }
      }
      
   }
}

void CPCDraftDoc::OnDataUpdateLink()
{
   if(m_Reports.GetSize()>0)
   {
      CMenu theReportMenu;
      theReportMenu.CreatePopupMenu();
      for(int i=0; i<m_Reports.GetSize(); i++)
      {
         CMDReport* pReport=m_Reports[i];
         theReportMenu.AppendMenu(MF_ENABLED | MF_STRING | (i % 20 || 0 == i?0:MF_MENUBARBREAK),ID_UPDATE_LINK_BASE + i,pReport->m_sTitle);
      }
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      POSITION pos=GetFirstViewPosition();
      while(pos != NULL)
      {
         CView* pView=GetNextView(pos);
         if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
         {
            int iId = theReportMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,pView->GetTopLevelFrame());
            if(iId >= 0)
            {
               pView->PostMessage(WM_COMMAND,iId);

            }
            return;
         }
      }

   }
}

void CPCDraftDoc::OnDataBreakLink()
{
   if(m_Reports.GetSize()>0)
   {
      CMenu theReportMenu;
      theReportMenu.CreatePopupMenu();
      for(int i=0; i<m_Reports.GetSize(); i++)
      {
         CMDReport* pReport=m_Reports[i];
         theReportMenu.AppendMenu(MF_ENABLED | MF_STRING | (i % 20 || 0 == i?0:MF_MENUBARBREAK),ID_BREAK_LINK_BASE + i,pReport->m_sTitle);
      }
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      POSITION pos=GetFirstViewPosition();
      while(pos != NULL)
      {
         CView* pView=GetNextView(pos);
         if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
         {
            int iId = theReportMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,pView->GetTopLevelFrame());
            if(iId >= 0)
            {
               pView->PostMessage(WM_COMMAND,iId);

            }
            return;
         }
      }

   }
}

CMDReport* CPCDraftDoc::AddReport(CString* pName)
{
   CMDReport* pReport=new CMDReport;
   if(pName)
   {
      while(!CheckReportName(*pName))
      {
         CRenameDlg theDlg(*pName,IDS_REPORT);
         if(theDlg.DoModal()==IDOK) *pName=theDlg.m_sName;
         else
         {
            delete pReport;
            return NULL;
         }
      }
      pReport->m_sTitle=*pName;
   }
   else
   {
      CString sName;
      int n=m_Reports.GetSize()+1;
      do sName.Format(IDS_REPORTNAME,n++); while(!CheckReportName(sName));
      pReport->m_sTitle=sName;
   }
   m_Reports.Add(pReport);

   SetModifiedFlag();
   return pReport;
}

void CPCDraftDoc::DeleteReport(int nIndex)
{
   UpdateAllViews(NULL,kReportDeleted,m_Reports.GetAt(nIndex));
   m_Reports.RemoveAt(nIndex);
   SetModifiedFlag();
}

HBITMAP CVersionDoc::GetPatternBitmap(int nIndex, int iOpacity)
{
   //by TSM
   //ASSERT(nIndex>=0 && nIndex<71);
   //ASSERT(nIndex >= 0 && nIndex < m_numDIBPatterns);

   // Gradient - Infinisys Ltd
   if(nIndex >= 0 && nIndex < m_packeditema.GetCount())
   {
      return m_packeditema[nIndex].CreateBitmap(iOpacity);
   }
   return nullptr;
}

bool CPCDraftDoc::GetActiveGridSnaps(CStringArray &strList)
{
   int strID;
   GrdEntries *entries=NULL;
   GetScaleInfo(GetActiveLayer()->m_nScale,&strID,&entries,NULL);
   if(entries)
   {
      for(int i=0; i<entries->fNumEntries; i++)
      {
         CString str;
         str.LoadString(strID+entries->entry[i].str-1);
         strList.Add(str);
      }
      return true;
   }
   return false;
}

void CPCDraftDoc::GetScaleInfo(int scale, int *strID, GrdEntries **entries, Ratio **snaps)
{
   GrdSInfo *grd;
   *entries = NULL;
   if(m_nUnit & kUEnglishDecMask) grd=&EnglishDecGrids;
   else if(m_nUnit & kUEnglishFracMask) grd=&EnglishFractGrids; else grd=&MetricDecGrids;
   if (m_nUnit==kUDecFeet) grd=&EnglishDecFtGrids;

   if(strID) *strID=grd->fSTRID;
   if(snaps) *snaps=grd->fSCAL;
   if(entries)
   {
      // Free Scale - Infinisys Ltd.
      // for free scale, find immediatelly more precise pre-defined scale
      if(scale == -1)
      {

         LongRatio longRatio = m_ratioCustom;

         double dRatio;

         if(longRatio.fD == 0)
         {

            dRatio = 1.0;

         }
         else
         {

            dRatio = (double) longRatio.fN / (double) longRatio.fD;

         }

         Ratio* ratio;
         if(m_bEnglishUnits)
         {
            ratio=EnglishScales;
         }
         else
         {
            ratio=MetricScales;
         }
         int iCount = ratio->fD;
         for(int i = 1; i <= iCount; i++)
         {
            double dRatioPredefined = (double)ratio[i].fN / (double)ratio[i].fD;
            if(dRatio > dRatioPredefined)
            {
               if(i >= 2)
               {
                  i--;
               }
               scale = i;
               break;
            }
            if(scale < 0)
            {

               scale = grd->fNumScales - 1;

            }
         }
      }
      for(int i=0; i<grd->fNumScales; i++)
      {
         if(grd->fData[i].fScaleIndex==scale)
         {
            *entries=&grd->fData[i];
            return;
         }
      }
   }
}

void CPCDraftDoc::GetSnapRatio(int nSnapIndex,LongRatio& longRatio)
{
   Ratio snapRatio= {0,1};
   GrdEntries *entries;
   Ratio *ratios;
   GetScaleInfo(GetActiveLayer()->m_nScale,NULL,&entries,&ratios);
   if(entries != NULL && nSnapIndex>0 && nSnapIndex<=entries->fNumEntries)
   {
      snapRatio=ratios[entries->entry[nSnapIndex-1].scal];
   }
   longRatio.fN=snapRatio.fN*m_nDisplayRes;
   longRatio.fD=snapRatio.fD;
   // Free Scale - Infinisys Ltd.
   if(GetActiveLayer()->m_nScale == -1)
   {
      Ratio* ratio;
      if(m_bEnglishUnits)
      {
         ratio=EnglishScales;
      }
      else
      {
         ratio=MetricScales;
      }
      longRatio.fN *= m_ratioCustom.fN *ratio[entries->fScaleIndex].fD;
      longRatio.fD *= m_ratioCustom.fD *ratio[entries->fScaleIndex].fN;
   }

   // SMK.SSA
   if ( longRatio.fD || longRatio.fN)
   {
      long gDiv=gcd(longRatio.fN,longRatio.fD);
      longRatio.fN/=gDiv;
      longRatio.fD/=gDiv;
   }
}

void CPCDraftDoc::GetScaleRatio(bool bEnglish,int nScaleIndex,LongRatio& longRatio)
{

   if(nScaleIndex == -1)
   {

      longRatio = m_ratioCustom;

      return;

   }

   Ratio* ratio;
   if(bEnglish) ratio=&EnglishScales[nScaleIndex];
   else ratio=&MetricScales[nScaleIndex];
   longRatio.fN=ratio->fN;
   longRatio.fD=ratio->fD;
}

void CPCDraftDoc::GetScaleRatio(int nScaleIndex,LongRatio& longRatio)
{
   GetScaleRatio(m_bEnglishUnits,nScaleIndex,longRatio);
}

void CPCDraftDoc::GetActiveScaleRatio(LongRatio& longRatio)
{
   GetScaleRatio(GetActiveLayer()->m_nScale,longRatio);
}

LongRect CPCDraftDoc::GetCumulativeExtents(bool bBasic)
{
   LongBoundRect lrcExtent;
   for(int i=0; i<GetLayersCount(); i++)
   {
      CLayer* pLayer=GetLayer(i);
      LongRect lrcBasic=pLayer->GetCumulativeExtents(bBasic);
      if(!lrcBasic.IsEmpty()) lrcExtent|=lrcBasic;
   }
   return lrcExtent;
}

void CPCDraftDoc::Offset(const LongPoint &delta)
{
   POSITION pos;
   for(int i=0; i<GetLayersCount(); i++)
   {
      CLayer* pLayer=GetLayer(i);
      pos=pLayer->m_ShapeList.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos);
         pShape->DoShapeOffset(delta);
      }
   }
   SetModifiedFlag();
}


#ifndef _PERSONAL_EDITION

bool CPCDraftDoc::ExportDXF(const char* szFileName)
{
   CFileException fe;
   CFile* fo=GetFile(szFileName,CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);
   if(fo == NULL)
   {
      ReportSaveLoadException(szFileName, &fe, TRUE, AFX_IDP_INVALID_FILENAME);
      return false;
   }

   TRY
   {
      CWaitCursor wc;

      CString strBuffer;
      short nCode;

      DxfWriteHeader(fo);
      DxfWriteTable(fo);
      DxfWriteBlock(fo);
      DxfWriteEntity(fo);

      nCode = kEntryStart;
      strBuffer.Format("%3d\r\nEOF\r\n", nCode);
      fo->Write(strBuffer, strBuffer.GetLength());
      ReleaseFile(fo, FALSE);
   }
   CATCH_ALL(e)
   {
      ReleaseFile(fo, TRUE);
      TRY
      {
         ReportSaveLoadException(szFileName, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
      }
      END_TRY
//    e->Delete();
      return false;
   }
   END_CATCH_ALL
   return true;
}

void CPCDraftDoc::DxfWriteHeader(CFile* pFile)
{
   CString     strBuffer, strParam1, strParam2;
   short    nCode1, nCode2, nCode3;
   double      fDblVal1, fDblVal2;

   /* Section string */
   findStrFromTab (kHdrSection, strParam1, kHeaderTab);

   /* Header string */
   findStrFromTab (kHdrHeader, strParam2, kHeaderTab);

   nCode1 = kEntryStart;
   nCode2 = kNameString;

   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, strParam1, nCode2, strParam2);
   pFile->Write(strBuffer, strBuffer.GetLength());

   /* Source Application Name and Version */
   nCode1 = kComments;
   strBuffer.Format("%3d\r\n%s\r\n", nCode1, kVerStr);
   pFile->Write(strBuffer, strBuffer.GetLength());

   /* AutoCad Release Version */
   nCode1 = kVarNameId;
   nCode2 = kTextValue;
   findStrFromTab (kACADVER, strParam1, kHeaderTab);
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, strParam1, nCode2, kACVerCode);
   pFile->Write(strBuffer, strBuffer.GetLength());

   /* Default Codepage */
   nCode1 = 9;
   nCode2 = 3;
#ifdef _JAPANESE
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, "$DWGCODEPAGE", nCode2, "dos932");
#else
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, "$DWGCODEPAGE", nCode2, "ANSI_1252");
#endif

   pFile->Write(strBuffer, strBuffer.GetLength());

   /* Current Layer Name */
   nCode1 = kVarNameId;
   nCode2 = kLayerName;
   findStrFromTab (kCLAYER, strParam1, kHeaderTab);

   CLayer* pLayer=GetActiveLayer();
   strParam2=pLayer->m_strName;
   //strParam2.MakeUpper();
   //strParam2.Remove(' ');
   //strParam2.Remove('.');

   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, strParam1, nCode2, strParam2);

   pFile->Write(strBuffer, strBuffer.GetLength());

   /* Find the cumulative extents of all shapes in all layers */
   LongRect rcExtents=GetCumulativeExtents();

   /* Write the Minimum Extent */
   nCode1 = kVarNameId;
   nCode2 = kPrimaryXCoord;
   nCode3 = kPrimaryYCoord;
   fDblVal1 = fDblVal2 = 0.0;
   findStrFromTab (kEXTMIN, strParam1, kHeaderTab);
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", nCode1, strParam1, nCode2, fDblVal1, nCode3, fDblVal2);
   pFile->Write(strBuffer, strBuffer.GetLength());

   /* Write the Maximum Extent -- must be together with Min Extent */
   /* Don't assign new values to nCode1, nCode2 and nCode3 !!!!!!! */
   double curUnitFactor=GetCurUnitFactor();
   fDblVal1 = mtFixToReal(rcExtents.right - rcExtents.left)/curUnitFactor;
   fDblVal2 = mtFixToReal(rcExtents.bottom - rcExtents.top)/curUnitFactor;
   findStrFromTab (kEXTMAX, strParam1, kHeaderTab);
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", nCode1, strParam1, nCode2, fDblVal1, nCode3, fDblVal2);
   pFile->Write(strBuffer, strBuffer.GetLength());

   /* write $FASTZOOM
   vCode1 = kVarNameId;
   vCode2 = kOtherIntValMin;
   vIndex = 0;
   findStrFromTab (kFASTZOOM, (char *)vStr1, kHeaderTab);
   sprintf (vBuf, "%3d\r\n%s\r\n%3d\r\n%d\r\n", vCode1, vStr1, vCode2, vIndex);
   if ((vErrCode = sendBuffer (pFileHdl, vBuf)) == kDxfErr)
      return kDxfErr;

   /* write $FILLMODE - must be together with $FASTZOOM
   /* Don't assign new values to vCode1 and vCode2 !!!!
   vIndex = gFillsOn ? 1 : 0;
   findStrFromTab (kFILLMODE, (char *)vStr1, kHeaderTab);
   sprintf (vBuf, "%3d\r\n%s\r\n%3d\r\n%d\r\n", vCode1, vStr1, vCode2, vIndex);
   if ((vErrCode = sendBuffer (pFileHdl, vBuf)) == kDxfErr)
      return kDxfErr;
    */
   /* End Section string */
   findStrFromTab (kHdrEndSec, strParam1, kHeaderTab);

   nCode1 = kEntryStart;
   strBuffer.Format("%3d\r\n%s\r\n", nCode1, strParam1);

   pFile->Write(strBuffer, strBuffer.GetLength());

}

void CPCDraftDoc::DxfWriteTable(CFile* pFile)
{
   short    nCode1, nCode2;
   CString     strBuffer, strParam1, strParam2;

   /* Section string */
   findStrFromTab (kSECTION, strParam1, kTablesTab);

   /* Header string */
   findStrFromTab (kTABLES, strParam2, kTablesTab);

   nCode1 = kEntryStart;
   nCode2 = kNameString;
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, strParam1, nCode2, strParam2);

   pFile->Write(strBuffer, strBuffer.GetLength());
   /* Build the Line Table - has to preceed Layer Table */

   DxfBuildLineTab (pFile);

   /* Build the Layer Table */
   DxfBuildLayerTab (pFile);

#ifdef _JAPANESE
   /* For japanease version - put default font with BIGFONT SUPPORT */
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", 0, "TABLE", 2, "STYLE");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", 5, "17F09", 100, "AcDbSymbolTable");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%6d\r\n%3d\r\n%s\r\n", 70, 1, 0, "STYLE");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", 5, "17F17", 100, "AcDbSymbolTableRecord");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", 100, "AcDbTextStyleTableRecord", 2, "STANDARD");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%6d\r\n%3d\r\n%s\r\n", 70, 0, 40, "0.0");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", 41, "1.0", 50, "0.0");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%6d\r\n%3d\r\n%s\r\n", 71, 0, 42, "8.0");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", 3, "txt", 4, "BIGFONT");
   pFile->Write(strBuffer, strBuffer.GetLength());
   strBuffer.Format("%3d\r\n%s\r\n", 0, "ENDTAB");
   pFile->Write(strBuffer, strBuffer.GetLength());
#endif

   /* End Section string */
   findStrFromTab (kENDSEC, strParam1, kTablesTab);

   nCode1 = kEntryStart;
   strBuffer.Format("%3d\r\n%s\r\n", nCode1, strParam1);
   pFile->Write(strBuffer,strBuffer.GetLength());
}

void CPCDraftDoc::DxfBuildLineTab(CFile *pFile)
{
   CString        strBuffer, strParam2, strParam1;
   short       nCode1, nCode2, nCode3, nCode4, nCode5, nCode6, nCode7, nCode;
   short       nIndex,nIdx;
   short       nNoSeg, nBitVal;
   double         dblTotal;


   /* Begin Table */
   nCode1 = kEntryStart;
   nCode2 = kNameString;
   nCode3 = kOtherIntValMin;

   findStrFromTab (kTABLE, strParam2, kTablesTab);
   findStrFromTab (kLTYPE, strParam1, kTablesTab);

   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n",
                    nCode1, strParam2, nCode2, strParam1, nCode3, 16);

   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n",
                    nCode1, strParam2, nCode2, strParam1, nCode3, 16);

   pFile->Write(strBuffer, strBuffer.GetLength());

   nCode1 = kEntryStart;
   nCode2 = kNameString;
   nCode3 = kOtherIntValMin;
   nCode4 = kOtherNameMin;
   nCode5 = kHorizAlign;
   nCode6 = kNumOfDashCodes;
   nCode7 = kOtherFloatValMin;
   nCode = kTableLength;

   for (nIndex = 0; nIndex < 16; nIndex++)
   {
      nNoSeg = m_DashStyles[(nIndex)*16];

      dblTotal = 0.0;
      if (nNoSeg)
      {
         for (nIdx = 0; nIdx < nNoSeg; nIdx++)
            dblTotal += m_DashStyles[(nIndex)*16+nIdx+1];
      }

      /* Dash Line Header */
      nBitVal = 64;
      if (nIndex)
      {
         strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s%02d\r\n%3d\r\n%d\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n%3d\r\n%d\r\n%3d\r\n%.6f\r\n",
                          nCode1, strParam1, nCode2, strParam1, nIndex, nCode3, nBitVal, nCode4, kCustom,
                          nCode5, kDxfAlign, nCode6, nNoSeg, nCode7, dblTotal/kEngRes);
      }
      else     /* Solid Line - Line Type should be CONTINUOUS */
      {
         findStrFromTab (kDxfContinuous, strParam2, kLineStyleTab);
         strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n%3d\r\n%d\r\n%3d\r\n%.6f\r\n",
                          nCode1, strParam1, nCode2, strParam2, nCode3, nBitVal, nCode4, kSolidLine,
                          nCode5, kDxfAlign, nCode6, nNoSeg, nCode7, dblTotal/kEngRes);
      }

      pFile->Write(strBuffer, strBuffer.GetLength());

      for (nIdx = 0; nIdx < nNoSeg; nIdx++)
      {
         dblTotal = m_DashStyles[nIndex*16+nIdx+1];
         if ((nIdx+1) % 2)   /* black segment */
            strBuffer.Format("%3d\r\n%.6f\r\n", nCode, dblTotal/kEngRes);
         else  /* white segment */
            strBuffer.Format("%3d\r\n%.6f\r\n", nCode, -dblTotal/kEngRes);

         pFile->Write(strBuffer, strBuffer.GetLength());
      }

   } /* For */

   /* End Table string */
   nCode1 = kEntryStart;
   findStrFromTab (kENDTAB, strParam1, kTablesTab);

   strBuffer.Format("%3d\r\n%s\r\n", nCode1, strParam1);
   pFile->Write(strBuffer, strBuffer.GetLength());
}

void CPCDraftDoc::DxfBuildLayerTab(CFile* pFile)
{
   short    nCode1, nCode2, nCode3, nCode4, nCode5, nColorNo, nBitVal;
   short    nNumLayers=GetLayersCount();
   CString     strBuffer, strParam1, strParam2, strLayerName;

   /* Begin Table */
   nCode1 = kEntryStart;
   nCode2 = kNameString;
   nCode3 = kOtherIntValMin;
   findStrFromTab (kTABLE, strParam1, kTablesTab);
   findStrFromTab (kLAYER, strParam2, kTablesTab);
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n", nCode1, strParam1, nCode2, strParam2, nCode3, nNumLayers);

   pFile->Write(strBuffer, strBuffer.GetLength());

   nCode1 = kEntryStart;
   nCode2 = kNameString;
   nCode3 = kOtherIntValMin;
   nCode4 = kColorNumber;
   nCode5 = kLineTypeName;
   nColorNo = 7;
   findStrFromTab (kDxfContinuous, strParam1, kLineStyleTab);


   for(int i=0; i<GetLayersCount(); i++)
   {
      CLayer* pLayer=GetLayer(i);
      strLayerName=pLayer->m_strName;
//    strLayerName.MakeUpper();
//    strLayerName.Remove(' ');
//    strLayerName.Remove('.');
      // frozen ????

      /*
            if (pLayer->m_bHidden)
               nBitVal = 0;
            else
               nBitVal = 65; // layer disabled ?????
      */

      nBitVal = 0;

      strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n%3d\r\n%d\r\n%3d\r\n%d\r\n%3d\r\n%s\r\n",
                       nCode1, strParam2, nCode2, strLayerName, nCode3, nBitVal, nCode4,
                       nColorNo, nCode5, strParam1);
      pFile->Write(strBuffer, strBuffer.GetLength());
   }

   /* End Table string */
   nCode1 = kEntryStart;
   findStrFromTab (kENDTAB, strParam1, kTablesTab);

   strBuffer.Format("%3d\r\n%s\r\n", nCode1, strParam1);
   pFile->Write(strBuffer, strBuffer.GetLength());

}

void CPCDraftDoc::DxfWriteEntity(CFile* pFile)
{
   CString strBuffer, strParam1, strParam2;
   Coord vDisplayRes;
   short nCode1, nCode2;
   double vUnitFactor;

   if(m_bEnglishUnits)
      vDisplayRes= m_nDisplayRes;
   else
      vDisplayRes = m_nDisplayRes*mtRealToFix(0.1);

   /* Section string */
   findStrFromTab (kDxfSection, strParam1, kEntitiesTab);

   /* Header string */
   findStrFromTab (kDxfEntities, strParam2, kEntitiesTab);

   nCode1 = kEntryStart;
   nCode2 = kNameString;
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, strParam1, nCode2, strParam2);

   pFile->Write(strBuffer, strBuffer.GetLength());
   /* Find the cumulative extents of all shapes in all layers */


   vUnitFactor = GetCurUnitFactor ();

// shapeHdl = vLayerPtr->fFirstShape;


   LongRect rcExtent=GetCumulativeExtents();
   POSITION pos;
   for(int i=0; i<GetLayersCount(); i++)
   {
      CLayer* pLayer=GetLayer(i);
      pos=pLayer->m_ShapeList.GetHeadPosition();
      while(pos)
      {
         CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos);

         pShape->WriteDXF(pFile, pLayer->m_strName, &rcExtent, vUnitFactor, m_nZoomFactor, vDisplayRes, &m_Palette);
      }
   }

   /* End Section string */
   findStrFromTab (kDxfEndSec, strParam1, kEntitiesTab);

   nCode1 = kEntryStart;
   strBuffer.Format("%3d\r\n%s\r\n", nCode1, strParam1);

   pFile->Write(strBuffer, strBuffer.GetLength());
}

void CPCDraftDoc::DxfWriteBlock(CFile *pFile)
{
   CString     strBuffer, strParam1, strParam2;
   short    nCode1, nCode2;

   /* Section string */
   findStrFromTab (kDxfSection, strParam1, kEntitiesTab);

   /* Header string */
   findStrFromTab (kDxfBLOCKS, strParam2, kEntitiesTab);

   nCode1 = kEntryStart;
   nCode2 = kNameString;
   strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", nCode1, strParam1, nCode2, strParam2);

   pFile->Write(strBuffer, strBuffer.GetLength());

   /* End Section string */

   findStrFromTab (kDxfEndSec, strParam1, kEntitiesTab);

   nCode1 = kEntryStart;
   strBuffer.Format("%3d\r\n%s\r\n", nCode1, strParam1);

   pFile->Write(strBuffer, strBuffer.GetLength());
}

#endif

double CPCDraftDoc::GetCurUnitFactor()
{
   double vCurUnitFactor = kEngRes;

   switch ( m_nUnit)
   {
   case kUDecInches:
   case kUDecFeetInches:
   case kUFracInches:
   case kUFracFeetInches:
      vCurUnitFactor = (double)kEngRes;
      break;

   case kUDecFeet:
      vCurUnitFactor = (double)kEngDecRes / 1.2;
      break;

   case kUMillimeters:
   case kUCentimeters:
   case kUDecimeters:
   case kUMeters:
      vCurUnitFactor = (double)kMetRes * 2.5394571;
      break;
   }

   return vCurUnitFactor;
}

bool CPCDraftDoc::ImportDXF(const char* szFileName)
{
   DxfParamStruct dxfParamStruct;
   try
   {

      dxfParamStruct.sShHdl = 0L;

      CFile file(szFileName, CFile::modeRead);
      dxfParamStruct.gRead3DData = false;
      dxfParamStruct.sSectBegin  = false;
      dxfParamStruct.sBufIdx     = 0;
      dxfParamStruct.bFirstLayer = true;
      dxfParamStruct.sSectionId = kHeaderTab;
      dxfParamStruct.sTabBegin = false;
      dxfParamStruct.sShape = 0;
      dxfParamStruct.sShapeID = 0;
      dxfParamStruct.sInsert = false;
      dxfParamStruct.sClosedSh = false;
      dxfParamStruct.sShPtCnt = 0;
      dxfParamStruct.sVertexOk = dxfParamStruct.sIgnoreLayer = false;
      dxfParamStruct.sIgnoreShape = dxfParamStruct.sIgnoreGroup = false;
      dxfParamStruct.sLookAheadForGrp = dxfParamStruct.sLAGrpFound = false;
//    dxfParamStruct.sAddGrpHdl = 0L;
//    dxfParamStruct.sGrShHdl = 0L;
//    dxfParamStruct.sBlkLayerHdl = 0L;
//    dxfParamStruct.sGrpFList = 0L;

      dxfParamStruct.sRdGrpBase = false;
      dxfParamStruct.sGrpBaseXPt = dxfParamStruct.sGrpBaseYPt = 0.0;
      dxfParamStruct.sXScaleFactor = dxfParamStruct.sYScaleFactor = 0.0;

      dxfParamStruct.sExtMaxFound = dxfParamStruct.sExtMinFound = false;
      dxfParamStruct.sExtMinX = dxfParamStruct.sExtMinY = dxfParamStruct.sExtMaxX = dxfParamStruct.sExtMaxY = 0.0;

      dxfParamStruct.sThisLayerStr = false;
      dxfParamStruct.sCurLayerStr    = GetActiveLayer()->m_strName;
      dxfParamStruct.sLayerList   = false;

      dxfParamStruct.sPlTrSolOk = kPLineOk;
      dxfParamStruct.sDefColorIdx = 2;
      dxfParamStruct.sEntityColorIdx = kNilOff;
      dxfParamStruct.sEntityDashIdx = kNilOff;
      dxfParamStruct.sDimensionOk = false;
      dxfParamStruct.sPolyFlag = kPlFg;
      dxfParamStruct.sDefaultDashIdx = 0;
      dxfParamStruct.sLStyleIdx = 0;
      dxfParamStruct.sDashStrIdx = 0;
      dxfParamStruct.sCurUnitFactor = GetCurUnitFactor ();

      long nBytesRead;
      long  nBytesLeft = 0;

      char pBuffer[kDxfBufSize];

      DxfIntialLStyleTab ();  /* Initialize LTYPE Tab */

      dxfParamStruct.sLnWidth = 1;
      dxfParamStruct.sHorizAlign = 0;
      dxfParamStruct.sVertAlign = 0;
      dxfParamStruct.sRotAngle = FixedZero ;
      CProgressDlg dlgProgress;
      dlgProgress.Create();
      dlgProgress.SetRange(0,100);
      dlgProgress.SetStep(10);
      long nTotal,nCurrent=0;
      nTotal=file.GetLength();
      if (!nTotal)
      {
         return false;
      }
      do
      {
         if (dlgProgress.CheckCancelButton())
            return false;
         dlgProgress.SetPos((int) nCurrent*100/nTotal);
         nBytesRead=file.Read(pBuffer, kDxfBufSize);
         nCurrent+=nBytesRead;

         /* Translate the information inside the buffer */
         nBytesLeft = DxfReadBuffer ((char*)pBuffer, nBytesRead, dxfParamStruct);
         nCurrent-=nBytesLeft;

         if (nBytesLeft==kDxfEOF)
            break;

         file.Seek(nBytesLeft, CFile::current);
      }
      while (nBytesRead);

      LongRect extents;
      extents = GetCumulativeExtents();

      Offset(LongPoint(-extents.left,-extents.top));
      m_NumPage.x = (extents.right.GetLong()-extents.left.GetLong()) / m_PageSize.cx + 1;
      m_NumPage.y = (extents.bottom.GetLong()-extents.top.GetLong()) / m_PageSize.cy + 1;
      UpdateAllViews(NULL,kDocumentSizeChanged);
      UpdateFrameCounts();
      UpdateAllViews(NULL,kRefresh);
      SetModifiedFlag();
      return true;

   }
   catch (CFileException* e)
   {
      if (dxfParamStruct.sShHdl)
         delete dxfParamStruct.sShHdl;
      TRACE0("Error: DXF read. File reading error\n");
      e->Delete();
   }
   catch (CUserException* e)
   {
      if (dxfParamStruct.sShHdl)
         delete dxfParamStruct.sShHdl;
      e->Delete();
   }
   catch (CException* e)
   {
      if (dxfParamStruct.sShHdl)
         delete dxfParamStruct.sShHdl;
      TRACE0("Error: DXF read. Error\n");
      e->Delete();
   }
   return false;
}

void CPCDraftDoc::DxfIntialLStyleTab()
{
   short  vIdx;
   DXFTablePtr vTabPtr = 0;

   vTabPtr = (DXFTablePtr)gLineStyleTab;
   vTabPtr++;  /* Skip first item */

   for (vIdx = 1; vIdx < kMaxLineStyleTab; vIdx++, vTabPtr++)
      strcpy((char * )vTabPtr->fVarStr, (char * )"XXXXXXXXXX");

}

long CPCDraftDoc::DxfReadBuffer(char * pBuffer, UINT nBufSize, DxfParamStruct& sParam)
{
   CString strValue;
   short nSrcIdx, nDstIdx;
   short nGrpCode;
   long  nNewOffset;

   UINT   nBufIdx = 0;

   while (nBufIdx < nBufSize)
   {
      strValue="";
      /* Get the Group Code */
      nDstIdx = 0;
      while (nBufIdx < nBufSize && pBuffer[nBufIdx] != kCReturn )
      {
         if (!isspace (pBuffer[nBufIdx]) )
            strValue+=pBuffer[nBufIdx];
         nBufIdx++;
      }

      if (nBufIdx < nBufSize)
         nGrpCode = atoi (strValue);
      else    /* End of the Buffer reached */
      {
         nSrcIdx = nBufSize - 1;
         while ( (nSrcIdx > 0) && !isspace (pBuffer[nSrcIdx]) )
            nSrcIdx--;
         nNewOffset = (long)(nBufSize - 1 - nSrcIdx);

         /* Return offset File Pointer Location */

         return -nNewOffset;
      }

      if (nNewOffset=DxfGetGrpValue (pBuffer, nBufSize, nGrpCode, nBufIdx, sParam))
         return nNewOffset;

      /* Last char in buffer not a space and file pointer is moved back */
   }
   return 0;
}

short CPCDraftDoc::DxfGetGrpValue(char* pBuffer,UINT nBufSize, short nGrpCode, UINT& nBufIdx, DxfParamStruct& sParam)
{
   UINT nBytesLeft=0;
   switch (DxfGrpValueType (nGrpCode))
   {
   case kStringType:
      nBytesLeft = DxfGetString (pBuffer, nBufSize, nBufIdx, nGrpCode, sParam);
      break;
   case kFloatType:
      nBytesLeft = DxfGetFloat (pBuffer, nBufSize, nBufIdx, nGrpCode, sParam);
      break;
   case kIntegerType:
      nBytesLeft = DxfGetInteger (pBuffer, nBufSize, nBufIdx, nGrpCode, sParam);
      break;
   default:
      TRACE0("Error: DXF read error" );
      AfxThrowUserException( );
      break;
   }
   return nBytesLeft;
}

short CPCDraftDoc::DxfGrpValueType(short pGrpCode)
{

   if ( pGrpCode >= kDXF8BitIntLowRangeStart && pGrpCode <= kDXF8BitIntLowRangeEnd )
   {
      // Dealing with an 8 bit int
      return kIntegerType;
   }
   else if ( ( pGrpCode >= kDXF16BitIntLowRangeStart && pGrpCode <= kDXF16BitIntLowRangeEnd )
             || ( pGrpCode >= kDXF16BitIntMidRangeStart && pGrpCode <= kDXF16BitIntMidRangeEnd )
             || ( pGrpCode >= kDXF16BitIntUpperRangeStart && pGrpCode <= kDXF16BitIntUpperRangeEnd ) )
   {
      // Dealing with a short
      return kIntegerType;
   }
   else if ( ( pGrpCode >= kDXF32BitIntLowRangeStart && pGrpCode <= kDXF32BitIntLowRangeEnd )
             || ( pGrpCode == kDXF32BitIntUpperRangeEnd ) )
   {
      // Dealing with a long
      return kIntegerType;
   }
   else if ( ( pGrpCode >= kDXFRealLowRangeStart && pGrpCode <= kDXFRealLowRangeEnd )
             || ( pGrpCode >= kDXFRealMidRangeStart && pGrpCode <= kDXFRealMidRangeEnd )
             || ( pGrpCode >= kDXFRealMidRange2Start && pGrpCode <= kDXFRealMidRange2End )
             || ( pGrpCode >= kDXFRealUpperRangeStart && pGrpCode <= kDXFRealUpperRangeEnd ) )
   {
      // Dealing with a float
      return kFloatType;
   }
   else if ( ( pGrpCode >= kDXFStringLowRangeStart && pGrpCode <= kDXFStringLowRangeEnd )
             || ( pGrpCode >= kDXFStringMidRangeStart && pGrpCode <= kDXFStringMidRangeEnd )
             || ( pGrpCode >= kDXFStringUpperRangeStart && pGrpCode <= kDXFStringUpperRangeEnd )
             || ( pGrpCode == kDXFStringIndivid1 || pGrpCode == kDXFStringIndivid2 || pGrpCode == kDXFStringIndivid3 ) )
   {
      // Dealing with a string
      return kStringType;
   }

   // We can't handle this - or there was an error so treat it like a string!!!!
   return kStringType;

   /*
   if (pGrpCode >= kStrMin1 && pGrpCode <= kStrMax1)
      return kStringType;

   if (pGrpCode >= kStrMin2 && pGrpCode <= kStrMax2)
      return kStringType;

   if (pGrpCode == kComments)
      return kStringType;

   if (pGrpCode >= kFloatMin1 && pGrpCode <= kFloatMax1)
      return kFloatType;

   if (pGrpCode >= kFloatMin2 && pGrpCode <= kFloatMax2)
      return kFloatType;

   if (pGrpCode >= kFloatMin3 && pGrpCode <= kFloatMax3)
      return kFloatType;

   if (pGrpCode >= kIntMin1 && pGrpCode <= kIntMax1)
      return kIntegerType;

   if (pGrpCode >= kIntMin2 && pGrpCode <= kIntMax2)
      return kIntegerType;

   return -1;
   */
}


long CPCDraftDoc::DxfGetString(char* pBuffer,UINT nBufSize, UINT& nBufIdx, short nGrpCode, DxfParamStruct& sParam)
{
   CString strValue="";
   short nIdx = 0, nStrId;
   bool  vStrFound = false;

   /* Get to the Carriage Return that terminated the Group Code */
   while (nBufIdx < nBufSize && pBuffer[nBufIdx] != kCReturn )
      nBufIdx++;

   /* Skip this Carriage Return */
   nBufIdx++;

   /* Get the String */
   while (nBufIdx < nBufSize && !vStrFound)
   {
      if (pBuffer[nBufIdx] != kCReturn)
         strValue += pBuffer[nBufIdx++];
      else
         vStrFound = true;
   }

   if (!vStrFound)
   {
      long  nNewOffset;

      strValue="";
      if (strValue == "EOF")
         return kDxfEOF;

      nIdx = nBufSize - 1;

      /* Beginning of Group Value */
      while (pBuffer[nIdx] != kCReturn && pBuffer[nIdx+1] != kLineFeed)
         nIdx--;
      nIdx--;

      /* Beginning of Group Code */
      while (pBuffer[nIdx] != kCReturn && pBuffer[nIdx+1] != kLineFeed)
         nIdx--;
      nIdx++;

      nNewOffset = (long)(nBufSize - 1 - nIdx);

      return -nNewOffset;
   }
   else  /* Skip the Carriage Return */
      nBufIdx++;

   /* No processing for comments */
   if (nGrpCode == kComments)
      return 0;

   if (strValue.GetLength()!= 0)
   {
      /* Remove the leading white spaces */
      strValue.Remove(kLineFeed);
      strValue.TrimLeft(' ');
   }

   if (!strValue.GetLength())
      return 0;

   switch (sParam.sSectionId)
   {
   case kHeaderTab:
      nStrId = DxfFindIDfromStrName(strValue, kHeaderTab);
      DxfFilterHdrTab(nGrpCode, nStrId, strValue, sParam);
      break;

   case kTablesTab:
      nStrId = DxfFindIDfromStrName (strValue, kTablesTab);
      DxfFilterTablesTab(nGrpCode, nStrId, strValue, sParam);
      break;

   case kBlockTab:
   case kEntitiesTab:
      if ( nGrpCode == kEntryStart )
         nStrId = DxfFindIDfromStrName (strValue, kEntitiesTab);
      else
         nStrId = kNameString;

      DxfFilterTab (nGrpCode, nStrId, strValue, sParam);
      break;

   default:
      if (strValue=="EOF")
         return kDxfEOF;
      break;
   }
   return 0;
}


long CPCDraftDoc::DxfGetFloat(char* pBuffer,UINT nBufSize, UINT& nBufIdx, short nGrpCode, DxfParamStruct& sParam)
{
   CString strValue="";
   short nIdx = 0;
   double  vDblValue;
   bool  vStrFound = false;

   /* Get to the Carriage Return that terminated the Group Code */
   while (nBufIdx < nBufSize && pBuffer[nBufIdx] != kCReturn)
      nBufIdx++;
   /* Skip this Carriage Return */
   nBufIdx++;

   /* Get the Float String */
   while (nBufIdx < nBufSize && !vStrFound)
   {
      if (pBuffer[nBufIdx] != kCReturn)
         strValue += pBuffer[nBufIdx++];
      else
         vStrFound = true;
   }

   if (!vStrFound)
   {
      long  nNewOffset;

      nIdx = nBufSize - 1;

      /* Beginning of Group Value */
      while (pBuffer[nIdx] != kCReturn && pBuffer[nIdx+1] != kLineFeed)
         nIdx--;
      nIdx--;

      /* Beginning of Group Code */
      while (pBuffer[nIdx] != kCReturn && pBuffer[nIdx+1] != kLineFeed)
         nIdx--;
      nIdx++;

      nNewOffset = (long)(nBufSize - 1 - nIdx);

      /* Set the File Pointer to New Location */
      return -nNewOffset;
   }
   else  /* Skip the Carriage Return */
      nBufIdx++;

   if (strValue.GetLength() != 0)
   {
      /* Remove the leading white spaces */
      strValue.Remove(kLineFeed);
      strValue.TrimLeft(' ');
   }

   if (!strValue.GetLength() || sParam.sIgnoreShape || sParam.sIgnoreGroup)
      return 0;

   switch (nGrpCode)
   {
   case kPrimaryXCoord:    /* 10 */
   case kOtherXCoordMin:   /* 11 */
   case kOther2XCoord:     /* 12 */
   case kOther3XCoord:     /* 13 */
      if ( sParam.sDimensionOk )
         break;

      if ( sParam.sShape || sParam.sInsert )
      {
         if ( sParam.sShapeID == IDPolygon && !sParam.sVertexOk  && !sParam.sPlTrSolOk )
            break;

         /* The coordinates are in WCS i.e., in Inches */
         vDblValue = atof (strValue) * sParam.sCurUnitFactor;

//          if ( vDblValue > kMaxCoordAllowed ) {
//             TRACE0("Error: DXF read error vDblValue > kMaxCoordAllowed" );
//             AfxThrowUserException( );
//          }

//          else if ( vDblValue < -kMaxCoordAllowed ) {
//             TRACE0("Error: DXF read error vDblValue < -kMaxCoordAllowed" );
//             AfxThrowUserException( );
//          }

//          if ( sParam.sShPtCnt >= sParam.sMaxPtArryCnt ) {

//             sParam.sMaxPtArryCnt += kPtArryIncr;
//             sParam.sPtHdl.SetSize(sParam.sMaxPtArryCnt);
         /* Set the point ptr to the right place. */
//             sParam.sFdPtPtr += (sParam.sMaxPtArryCnt - kPtArryIncr);
//          }

         if ( sParam.sRdGrpBase )
            sParam.sGrpBaseXPt = vDblValue;
         else
         {
            if (sParam.sFdPtPtr == (sParam.sPtHdl.GetSize()-1) )
               sParam.sPtHdl[sParam.sFdPtPtr].x = mtRealToFix (vDblValue);
         }
         /* sShPtCnt++; point count incremented after y coord received */
      }
      else
      {
         vDblValue = atof (strValue) * sParam.sCurUnitFactor;
         if ( sParam.sExtMaxFound )
            sParam.sExtMaxX = vDblValue;
         else if ( sParam.sExtMinFound )
            sParam.sExtMinX = vDblValue;
      }
      break;

   case kPrimaryYCoord:  /* 20 */
   case kOtherYCoordMin: /* 21 */
   case kOther2YCoord:   /* 22 */
   case kOther3YCoord:   /* 23 */
      if ( sParam.sDimensionOk )
         break;

      if ( sParam.sShape || sParam.sInsert )
      {
         if ( sParam.sShape && sParam.sShapeID == IDPolygon && !sParam.sVertexOk && !sParam.sPlTrSolOk )
            break;

         /* The coordinates are in WCS i.e., in Inches */
         vDblValue = -(atof (strValue) * sParam.sCurUnitFactor);

//          if ( vDblValue > kMaxCoordAllowed ) {
//             TRACE0("Error: DXF read error vDblValue > kMaxCoordAllowed" );
//             AfxThrowUserException( );
//          }

//          else if ( vDblValue < -kMaxCoordAllowed ) {
//             TRACE0("Error: DXF read error vDblValue < -kMaxCoordAllowed" );
//             AfxThrowUserException( );
//          }

         if ( sParam.sRdGrpBase )
         {
            sParam.sGrpBaseYPt = vDblValue;
            sParam.sRdGrpBase = false;
         }
         else
         {
            if ( sParam.sFdPtPtr == (sParam.sPtHdl.GetSize()-1))
            {
               sParam.sPtHdl[sParam.sFdPtPtr].y = mtRealToFix (vDblValue);
               sParam.sPtHdl.Add(LongPoint());
               sParam.sFdPtPtr++;
               sParam.sShPtCnt++;
            }
         }
      }
      else
      {
         vDblValue = atof (strValue) * sParam.sCurUnitFactor;
         if ( sParam.sExtMaxFound )
            sParam.sExtMaxY = vDblValue;
         else if ( sParam.sExtMinFound )
            sParam.sExtMinY = vDblValue;

         if (sParam.sExtMaxFound && sParam.sExtMinFound)
         {
            double  vExtX, vExtY, vDblMaxOrd;

            sParam.sExtMaxFound = sParam.sExtMinFound = false;
            if ( (vExtX = sParam.sExtMaxX - sParam.sExtMinX) < 0)
               vExtX = -vExtX;
            if ( (vExtY = sParam.sExtMaxY - sParam.sExtMinY) < 0)
               vExtY = -vExtY;

            vDblMaxOrd = (vExtX > vExtY) ? vExtX : vExtY;
            if (vDblMaxOrd > kMaxCoordinate)   //????
            {
               double         vFactor;
//                short       vIntFac;
//                unsigned char  vNumStr[3];
//                unsigned char  vMsg[255], vMsg2[255];

               vFactor = (double)kMaxCoordinate / vDblMaxOrd;
               sParam.sCurUnitFactor *= vFactor;
               //vIntFac = (short)(vFactor * 100);
               //sprintf((char *)vNumStr, "%d", vIntFac);
               //CtoPstr((char *)vNumStr);
               //GetIndString(vMsg, kDMStrsID, kIdSDXFScale1);
               //GetIndString(vMsg2, kDMStrsID, kIdSDXFScale2);
               //ParamText(vMsg, vNumStr, vMsg2, gNilStr);
               //SetCursor(&qd.arrow);
               //noteAlert(kDXFOKAlertID, cmdKeyFilterUPP);
            }
         }
      }

      break;

   case kTableLength:   /* 49 */

      vDblValue = atof (strValue);
      if ( sParam.sTabBegin && sParam.sLStyleIdx )
      {
         short nIntValue;

         nIntValue = (short) (vDblValue * kEngFactor);
         nIntValue = abs(nIntValue);
         if ( nIntValue == 0 )  nIntValue++;
         DxfCollectDashStr(sParam.sDashStrIdx, nIntValue, sParam.sDashCodeStr);
         sParam.sDashStrIdx++;
      }
      break;

   case kPrimaryZCoord:    /* 30 */
   case kEntityElevation:  /* 38 */
   case kEntityThickness:  /* 39 */
   case kXExtrusion:    /* 210 */
   case kYExtrusion:    /* 220 */
   case kZExtrusion:    /* 230 */
   case kXDataXCoord:      /* 1010 */
   case kXDataYCoord:      /* 1020 */
   case kXDataZCoord:      /* 1030 */
   case kXDataXCoord3DPos: /* 1011 */
   case kXDataYCoord3DPos: /* 1021 */
   case kXDataZCoord3DPos: /* 1031 */
   case kXDataXComp3DDisp: /* 1012 */
   case kXDataYComp3DDisp: /* 1022 */
   case kXDataZComp3DDisp: /* 1032 */
   case kXDataXComp3DDir:  /* 1013 */
   case kXDataYComp3DDir:  /* 1023 */
   case kXDataZComp3DDir:  /* 1033 */
   case kXDataFloatPtVal:  /* 1040 */
   case kXDataDistValue:   /* 1041 */
   case kXDataScaleFactor: /* 1042 */
      break;

   case kOtherFloatValMin: /* 40 */
      vDblValue = atof (strValue) * sParam.sCurUnitFactor;

      if ( sParam.sShape )
      {
         if ( sParam.sShapeID == IDPolygon )
         {
            if ( sParam.sVertexOk )
            {
               DxfLnWStruct   vDxfLnWStruct;
               vDxfLnWStruct.fVertIdx = sParam.sShPtCnt;
               vDxfLnWStruct.fLnW = DxfGetColLnW ((short)vDblValue);
               sParam.sLnWListHdl.Add(vDxfLnWStruct);
            }
            else
               sParam.sLnWidth = DxfGetColLnW ((short)vDblValue);
         }

         switch ( sParam.sShapeID )
         {
         case IDArc:
         case IDCircle:
         case IDText:
            sParam.sPtHdl[sParam.sFdPtPtr].x = mtRealToFix (vDblValue);
            sParam.sPtHdl.Add(LongPoint());
            sParam.sFdPtPtr++;
            sParam.sShPtCnt++;
            break;
         }
      }
      break;

   case kAngleMin:  /* 50 */
      if ( sParam.sDimensionOk ) break;
      vDblValue = atof (strValue);

      if ( sParam.sShape )
      {
         switch ( sParam.sShapeID )
         {
         case IDArc:
            sParam.sPtHdl[sParam.sFdPtPtr].x = mtRealToFix (vDblValue);
            break;
         case IDText:
            sParam.sRotAngle = mtRealToFix (vDblValue);
            break;
         default:
            if ( sParam.sInsert )
            {
               sParam.sPtHdl[sParam.sFdPtPtr].x = mtRealToFix (vDblValue);
               sParam.sPtHdl.Add(LongPoint());
               sParam.sFdPtPtr++;
               sParam.sShPtCnt++;
            }
         }
      }
      else if ( sParam.sInsert )
      {
         if ( sParam.sFdPtPtr == (sParam.sPtHdl.GetSize()-1))
         {
            sParam.sPtHdl[sParam.sFdPtPtr].x = mtRealToFix (vDblValue);
            sParam.sPtHdl.Add(LongPoint());
            sParam.sFdPtPtr++;
            sParam.sShPtCnt++;
         }
      }
      break;

   case kAngleMax:   /* 51 */
      if ( sParam.sDimensionOk ) break;

      if ( sParam.sShape )
      {
         vDblValue = atof (strValue);
         switch ( sParam.sShapeID )
         {
         case IDArc:
            sParam.sPtHdl[sParam.sFdPtPtr].y = mtRealToFix (vDblValue);
            sParam.sPtHdl.Add(LongPoint());
            sParam.sFdPtPtr++;
            sParam.sShPtCnt++;
            break;
         }
      }
      break;

   case kFlatVal2:  /* 41 */
      vDblValue = atof (strValue);
      if ( !sParam.sDimensionOk && sParam.sInsert )
         sParam.sXScaleFactor = vDblValue;
      break;

   case kFlatVal3:  /* 42 */
      vDblValue = atof (strValue);
      if ( !sParam.sDimensionOk && sParam.sInsert )
         sParam.sYScaleFactor = vDblValue;

      /* Group Code 42 for Vertex Bulge tan(Theta/4) */
      if ( sParam.sShape && (sParam.sShapeID == IDPolygon) && sParam.sVertexOk )  //????
      {
         WrFixed        vBulge;
         DxfBulgeStruct vDxfBulgeStruct;

         /* A bulge of 0 indicates a straight segment. */
         if ( vDblValue == 0 ) break;

         vBulge = mtRealToFix (vDblValue);
         //if ( !sBulgeListHdl )
         // sBulgeListHdl = newFList (sizeof (DxfBulgeStruct), 1, 1, 0, sizeof(short));
         vDxfBulgeStruct.fVertIdx = sParam.sShPtCnt;
         vDxfBulgeStruct.fBulge = vBulge;
         sParam.sBulgeListHdl.Add(vDxfBulgeStruct);
      }
      break;

   case kFlatVal4:   /* 43 */
   case kFlatVal5:   /* 44 */
   case kFlatVal6:   /* 45 */
   case kFlatVal7:   /* 46 */
   case kFlatVal8:   /* 47 */
   case kOtherFloatValMax: /* 48 */
      break;

   default:
      break;
   }

   return 0;

}

long CPCDraftDoc::DxfGetInteger(char* pBuffer,UINT nBufSize, UINT& nBufIdx, short nGrpCode, DxfParamStruct& sParam)
{
   CString  strValue="";
   short nIdx = 0, nIntValue;
   long  nLongValue;
   bool  vStrFound = false;

   /* Get to the Carriage Return that terminated the Group Code */
   while (nBufIdx < nBufSize && pBuffer[nBufIdx] != kCReturn)
      nBufIdx++;
   /* Skip this Carriage Return */
   nBufIdx++;

   /* Get the Integer String */
   while (nBufIdx < nBufSize && !vStrFound)
   {

      if (pBuffer[nBufIdx] != kCReturn)
         strValue += pBuffer[nBufIdx++];
      else
         vStrFound = true;
   }

   if (!vStrFound)
   {
      long  nNewOffset;

      nIdx = nBufSize - 1;

      /* Beginning of Group Value */
      while (pBuffer[nIdx] != kCReturn && pBuffer[nIdx+1] != kLineFeed)
         nIdx--;
      nIdx--;

      /* Beginning of Group Code */
      while (pBuffer[nIdx] != kCReturn && pBuffer[nIdx+1] != kLineFeed)
         nIdx--;
      nIdx++;

      nNewOffset = (long)(nBufSize - 1 - nIdx);

      /* Set the File Pointer to New Location */
      return -nNewOffset;
   }
   else  /* Skip the Carriage Return */
      nBufIdx++;

   if (strValue.GetLength()!= 0)
   {
      /* Remove the leading white spaces */
      strValue.Remove(kLineFeed);
      strValue.TrimLeft(' ');
   }

   if (!strValue.GetLength())
      return 0;

   switch (nGrpCode)
   {
   case kColorNumber:
      nIntValue = atoi (strValue);
      switch ( sParam.sSectionId )
      {
      case kTablesTab:
         sParam.sDefColorIdx = DxfGetMacDraftMacColor (nIntValue);

         if ( sParam.sLayerList )
         {

            short       nIdx;

            if ((nIdx = DxfFindLayer (sParam.sCurLayerStr, sParam)) == -1)
            {
               DxfUseCurLayer(sParam.sCurLayerStr,sParam);
               //nIdx = DxfFindLayer(sParam.sCurLayerStr);
            }

            //m_Layers[nIdx].fLayerColor = sParam.sDefColorIdx; ???

         } /* if sLayerList */
         break;
      case kBlockTab:
      case kEntitiesTab:
         sParam.sEntityColorIdx = DxfGetMacDraftMacColor (nIntValue);
      }
      break;

   case kEntitiesFollow:
   case kXData16BitInteger:
      nIntValue = atoi (strValue);
      break;

   case kXData32BitSignLong:
      nLongValue = atol (strValue);
      break;

   case kOtherIntValMin:   /* 70 */
      nLongValue = atol (strValue);
      if ( sParam.sShape && (sParam.sShapeID == IDPolygon) && !sParam.sVertexOk /*&& !sParam.sPlTrSolOk*/ )
      {
         if ( nLongValue == 1 )
            sParam.sClosedSh = nLongValue?true:false;
         else
            sParam.sPolyFlag =  (short)nLongValue;
      }
      else if ((sParam.sPolyFlag == kSplineFitFg) && (nLongValue == 16) && sParam.sVertexOk)
      {
         sParam.sPtHdl.RemoveAt(sParam.sPtHdl.GetSize()-1, 1);
         sParam.sFdPtPtr--;  /* Remove Spline frame control point */
         sParam.sShPtCnt--;  /* Group 70 value 16  */
      }
      break;

   case kHorizAlign: /* 72 */
      sParam.sHorizAlign = atoi (strValue);
      break;

   case kNumOfDashCodes:   /* 73 */
      nIntValue = atoi (strValue);

      if ( (sParam.sShape || sParam.sInsert) && ( sParam.sShapeID == IDText ) )
         sParam.sVertAlign = nIntValue;

      if ( sParam.sTabBegin && sParam.sLStyleIdx )
      {
         if ( nIntValue > 14 )  nIntValue = 14;
         if ( !(nIntValue == 0 && sParam.sLStyleIdx == 1) )
         {
            /* No for First Solid Line */
            DxfCollectDashStr (sParam.sDashStrIdx, nIntValue, sParam.sDashCodeStr);
            sParam.sDashStrIdx++;
         }
         else
            sParam.sLStyleIdx = 0;
      }
      break;

   default:
//       if (nGrpCode >= kOtherIntValMin && nGrpCode <= kOtherIntValMax)
      nIntValue = atoi (strValue);
//       else {
//          TRACE0("Error: DXF read error" );
//          AfxThrowUserException( );
//       }
      break;
   }

   return 0;
}

short CPCDraftDoc::DxfFindIDfromStrName (CString strName, short pWhichTab )
{
   short    nResult, nID = kNilOff, nMaxNum, nIdx;
   DXFTablePtr pTabPtr = 0L;

   switch ( pWhichTab )
   {
   case  kHeaderTab:
      pTabPtr = (DXFTablePtr)gHeaderTab;
      nMaxNum = kMaxHeaderTab;
      break;

   case  kTablesTab:
      pTabPtr = (DXFTablePtr)gTablesTab;
      nMaxNum = kMaxTablesTab;
      break;

   case  kEntitiesTab:
      pTabPtr = (DXFTablePtr)gEntitiesTab;
      nMaxNum = kMaxEntitiesTab;
      break;

   case  kLineStyleTab:
      pTabPtr = (DXFTablePtr)gLineStyleTab;
      nMaxNum = kMaxLineStyleTab;
      break;

   default:
      /* No Tables exist */
      return kNilOff;
      break;
   }

   for (nIdx = 0; nIdx < nMaxNum; nIdx++, pTabPtr++)
   {
      if (!(nResult = strcmp(strName, (char * )pTabPtr->fVarStr)))
      {
         nID = pTabPtr->fVarID;
         break;
      }
   }

   return (nID);
}

short CPCDraftDoc::DxfGetMacDraftMacColor (short   nAcadColor )
{
   short   nColColor;

   switch (nAcadColor)
   {
   case 0:  /* ACAD Black */
      nColColor = 1;
      break;
   case 1:  /* ACAD Red */
      nColColor = 4;
      break;
   case 2:  /* ACAD Yellow */
      nColColor = 9;
      break;
   case 3:  /* ACAD Green */
      nColColor = 20;
      break;
   case 4:  /* ACAD Cyan */
      nColColor = 7;
      break;
   case 5:  /* ACAD Blue */
      nColColor = 6;
      break;
   case 6:  /* ACAD Magenta */
      nColColor = 8;
      break;
   case 7:  /* ACAD White if background is of dark color, else it is black */
      nColColor = 1;
      break;
   case 8:  /* ACAD Dark Red Grey */
      nColColor = 3;
      break;
   case 9:  /* ACAD Dark Red */
      nColColor = 64;
      break;
   case 10: /* ACAD Dark Blue */
      nColColor = 46;
      break;
   case 11: /* ACAD Dark Green */
      nColColor = 5;
      break;
   case 12: /* ACAD Dark Cyan */
      nColColor = 34;
      break;
   case 13: /* ACAD light Blue */
      nColColor = 32;
      break;
   case 14: /* ACAD Dark Magenta */
      nColColor = 59;
      break;
   case 15: /* ACAD light grey */
      nColColor = 3;
      break;
   case 255:/* ACAD White */
      nColColor = 1;
      break;
   default: /* ACAD  */
      nColColor = 1;  /* White */
   }

   return nColColor;
} /* getMacDraftMacColor */


void CPCDraftDoc::DxfCollectDashStr (
short          pIndex,
short          pDashByte,
unsigned char*    pDashCodePtr )
{
   unsigned char  *vDashCodePtr;

   if ( pIndex  < 15 )
   {
      vDashCodePtr = pDashCodePtr + pIndex;
      *vDashCodePtr = (uchar)pDashByte;
   }
} /* collectDashStr */

void CPCDraftDoc::DxfFilterHdrTab (short nGrpCode,short nStrId, CString strName, DxfParamStruct& sParam)
{
   switch (nStrId)
   {
   case kHdrHeader:
      sParam.sSectionId = kHeaderTab;
      break;

   case kTABLES:
      sParam.sSectionId = kTablesTab;
      break;

   case kDxfBLOCKS:
      if (nGrpCode == kNameString)
      {
         sParam.sSectionId = kBlockTab;
         if ( !sParam.sLayerList )
            DxfCreateDefLayerLst (sParam);
      }
      break;

   case kDxfEntities:
      if (nGrpCode == kNameString)
      {
         sParam.sSectionId = kEntitiesTab;
         if ( !sParam.sLayerList )
            DxfCreateDefLayerLst (sParam);
      }
      break;

   case kHdrSection:
      sParam.sSectBegin = true;
      break;

   case kHdrEndSec:
      sParam.sSectBegin = false;
      sParam.sSectionId++;
      break;

   case kEXTMAX:
      sParam.sExtMaxFound = true;
      break;

   case kEXTMIN:
      sParam.sExtMinFound = true;
      break;

   case kCLAYER:
      sParam.sThisLayerStr = true;
      break;

   default:
      if (nGrpCode == kLayerName && sParam.sThisLayerStr)
      {

         /* Current Layer Name */
         sParam.sCurLayerStr=strName;

         /* Make sure it is all in Upper Case */
         //sParam.sCurLayerStr.MakeUpper();
         sParam.sThisLayerStr = false;
      }
      break;
   }

   return;
}

void CPCDraftDoc::DxfCreateDefLayerLst(DxfParamStruct& sParam)
{
   GetActiveLayer()->m_strName = sParam.sCurLayerStr;
   sParam.sLayerList=true;
   sParam.bFirstLayer = false;
}


void CPCDraftDoc::DxfFilterTablesTab (short nGrpCode,short nStrId,CString strName,DxfParamStruct& sParam)
{
   switch (nStrId)
   {
   case kTABLES:
      sParam.sSectionId = kTablesTab;
      break;

   case kSECTION:
      sParam.sSectBegin = true;
      break;

   case kENDSEC:
      sParam.sSectBegin = false;
      sParam.sSectionId++;
      break;

   case kTABLE:
      sParam.sTabBegin = true;
      break;

   case kENDTAB:
      if ( (nGrpCode == kEntryStart) && sParam.sTabBegin &&
            (sParam.sLStyleIdx >= 1 && sParam.sLStyleIdx <= 15) )
      {
         for (int i=0; i<16; i++)
            m_DashStyles[(sParam.sLStyleIdx)*16+i]=sParam.sDashCodeStr[i];
         sParam.sLStyleIdx = 0;
         sParam.sDashStrIdx = 0;
      }
      sParam.sTabBegin = false;
      break;

   case kLAYER:
      if (nGrpCode == kEntryStart && sParam.sTabBegin)
         sParam.sThisLayerStr = true;
      break;

   case kLTYPE:
      if ( (nGrpCode == kEntryStart) && sParam.sTabBegin &&
            (sParam.sLStyleIdx >= 1 && sParam.sLStyleIdx <= 15 ) )
      {
         for (int i=0; i<16; i++)
            m_DashStyles[(sParam.sLStyleIdx)*16+i]=sParam.sDashCodeStr[i];
         sParam.sDashStrIdx = 0;
      }

      if ( nGrpCode == kEntryStart && sParam.sTabBegin )
      {
         sParam.sLStyleIdx++;
         for (int i=0; i<16; i++)
            sParam.sDashCodeStr[i]=0xFF;
      }

      break;

   default:
      if (nGrpCode == kNameString && sParam.sThisLayerStr)
      {
         /* Build the Layer List from the Layer Table */
         //'''if (sParam.sLayerList)
         if (DxfFindLayer (strName, sParam) == -1)
            DxfUseCurLayer(strName, sParam);

         //AddLayer(&strName);
         GetActiveLayer()->m_strName = strName;
         //GetActiveLayer()->m_strName.MakeUpper();
         sParam.sThisLayerStr = false;
         sParam.sLayerList = true;
         sParam.sCurLayerStr = strName;
      }
      else
      {
         if (nGrpCode == kLineTypeName && sParam.sSectionId == kTablesTab &&  sParam.sLayerList)
         {
//             DxfLayerStruct vLayerStruct;
            short nIdx;

            sParam.sDefaultDashIdx = DxfGetColDashIdx(strName);

            if ((nIdx = DxfFindLayer(sParam.sCurLayerStr, sParam)) == -1)
            {
               TRACE0("Error");
               AfxThrowUserException();
            }
            else
            {
               //vLayerStruct.fDashIdx = sParam.sDefaultDashIdx;
               //m_Layers[vIdx].fDasIdx=sParam.sDefaultDashIdx; ???
            }
         }
         else if ( nGrpCode == kNameString && sParam.sTabBegin && sParam.sLStyleIdx )
            DxfResetLStyleTab(strName, sParam.sLStyleIdx);
      }
      break;
   }

   return;
} /* filterTablesTab */

void CPCDraftDoc::DxfFilterTab (short nGrpCode,short nStrId, CString strName, DxfParamStruct& sParam)
{
   if (sParam.sIgnoreGroup && (nStrId != kDxfEndblk))
   {
      TRACE0("Error : DxfFilterTab \n");
      AfxThrowUserException();
   }

   switch (nStrId)
   {
//    case kDxfClassID :
//       break;
   case kDxfBLOCKS:
      if (nGrpCode == kNameString)
         sParam.sSectionId = kBlockTab;

      if ( !sParam.sLayerList )
         DxfCreateDefLayerLst (sParam);
      break;

   case kDxfEntities:
      if (nGrpCode == kNameString)
      {
         sParam.sSectionId = kEntitiesTab;
         sParam.sLookAheadForGrp = sParam.sLAGrpFound = false;
         if ( !sParam.sLayerList )
            DxfCreateDefLayerLst (sParam);
      }
      break;

   case kDxfSection:
      sParam.sSectBegin = true;
      break;

   case kDxfEndSec:
      sParam.sSectBegin = false;
      sParam.sSectionId++;
//       if (sParam.sSectionId == kEntitiesTab)
      DxfUpdtShape (true, sParam);
//       if ( sParam.sLookAheadForGrp )
//          vErrCode = kDxfStopLookAhead;
      break;

   case kDxfVertex:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//                vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      if ( sParam.sShape )
         sParam.sVertexOk = true;
      else
         sParam.sVertexOk = false;

      break;

   // 3D DXF input - ignore the data, but in future will we handle this????......
   case kDxf3DFace :
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      if (!sParam.gRead3DData)
      {
//          Str255 theString;
//          GetIndString(theString, kDMStrsID, kDXFRead3DStr);
//          ParamText(theString, gNilStr, gNilStr, gNilStr);
//          SetCursor(&qd.arrow);
//          noteAlert(kDXFOKAlertID, cmdKeyFilterUPP);
         sParam.gRead3DData = true;
      }
      break;

   case kDxf3DLine :
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false,sParam);
      if (sParam.sInsert)
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      if (!sParam.gRead3DData)
      {
//          Str255 theString;
//          GetIndString(theString, kDMStrsID, kDXFRead3DStr);
//          ParamText(theString, gNilStr, gNilStr, gNilStr);
//          SetCursor(&qd.arrow);
//          noteAlert(kDXFOKAlertID, cmdKeyFilterUPP);
         sParam.gRead3DData = true;
      }
      DxfCreateLine (sParam);
      return;
      break;

   case kDxfLine:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//                vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);

      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreateLine (sParam);
      return;
      break;

   case kDxfArc:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);

      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreateArc (sParam);
      return;
      break;

   case kDxfCircle:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
            //             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreateCircle (sParam);
      return;
      break;

   case kDxfText:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//                vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreateText (sParam);
      return;
      break;

   case kDxfBlock:  /* Group Begin */
      DxfUpdtShape (false, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreateGroup (sParam);
      return;
      break;

   case kDxfEndblk:  /* Group End */
      if (sParam.sLookAheadForGrp)
      {
         if (sParam.sIgnoreGroup)
            sParam.sIgnoreGroup = false;

         if (!sParam.sLAGrpFound)
            break;
         else
         {
            DxfUpdtShape (true, sParam);
            if ( sParam.sInsert )
            {
               sParam.sInsert = false;
               sParam.sDimensionOk = false;
            }
            sParam.sIgnoreLayer = false;
            DxfUpdtGroup (sParam);

//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      if (sParam.sIgnoreGroup)
      {
         sParam.sIgnoreGroup = false;
         sParam.sIgnoreLayer = false;
         if ( sParam.sInsert )
         {
            sParam.sInsert =  sParam.sDimensionOk = false;
         }
         break;
      }
      DxfUpdtShape (true, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      sParam.sIgnoreLayer = false;
      DxfUpdtGroup (sParam);
      break;

   case kDxfDimension:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      sParam.sDimensionOk = true;
      sParam.sInsert = true;
      break;

   case kDxfInsert:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      sParam.sDimensionOk = false;
      sParam.sInsert = true;
      break;

   case kDxfPolyline:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreatePoly (sParam);
      return;
      break;

   case kDxfTrace :
   case kDxfSolid :
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }

      DxfCreatePoly (sParam);
      if ( nStrId == kDxfTrace )
         sParam.sPlTrSolOk = kTraceOk;
      else if ( nStrId == kDxfSolid)
         sParam.sPlTrSolOk = kSolidOk;
      sParam.sClosedSh = true;
      break;

   case kDxfSeqEnd:
      if (sParam.sLookAheadForGrp)
      {
         if (!sParam.sLAGrpFound)
            break;
         else if (sParam.sSectionId != kBlockTab)
         {
//             vErrCode = kDxfStopLookAhead;
            break;
         }
      }

      DxfUpdtShape (false, sParam);
      if ( sParam.sInsert )
      {
         sParam.sInsert = false;
         sParam.sDimensionOk = false;
      }
      break;

   case kDxfAttdef:
   case kDxfAttrib:
   case kDxfDiameter:
   case kDxfLinear:
   case kDxfOrdinate:
   case kDxfPoint:
   case kDxfRadius:
   case kDxfViewport:
      sParam.sIgnoreShape = true;
      break;

   default:
   {
      if ( sParam.sIgnoreShape ) break;
      switch ( nGrpCode )
      {
      case kEntryStart:
         if (sParam.sLookAheadForGrp)
         {
            if (!sParam.sLAGrpFound)
               break;
            else if (sParam.sSectionId != kBlockTab)
            {
//                vErrCode = kDxfStopLookAhead;
               break;
            }
         }

         DxfUpdtShape (false, sParam);
         if ( sParam.sInsert )
         {
            sParam.sInsert = false;
            sParam.sDimensionOk = false;
         }
         break;

      case kTextValue:
         /* If in the Blocks section and creating a Block
            group code 1 indicates that the Block is an Xref
            Block. Currently this feature is not supported. */
         if ( (sParam.sSectionId == kBlockTab) && (sParam.sShape == kGroupShape) )
         {
//             displayStopAlert (kOKAlertID, kErrorStrListID, kFileHasExternalRef);
            //error
            //return kDxfErr;
         }

         if (sParam.sLookAheadForGrp)
         {
            if (!sParam.sLAGrpFound)
               break;
            else if (sParam.sSectionId != kBlockTab)
            {
//                vErrCode = kDxfStopLookAhead;
               break;
            }
         }

         sParam.sTextStr=strName;
         break;

      case kNameString:
      /*          if ( sParam.sInsert ) {
                     //GroupNameElem   vGrpNameElem;
                     //CString         vName[kMaxGrpNameLn];
                     short       nIndex;
                     //ApplyParams     vApplyParams;
                     //DupParams    vDupParams;

                     //setMem(&vName, kMaxGrpNameLn, 0);
                     //strcpy ((Ptr)&vName, (char * )pStrName);


                     if ((nIndex = DxfFindGroup (sParam, strName)) == -1) {
                        if (sParam.sSectionId ==  kEntitiesTab) {
                           TRACE0("Error: \n");
                           AfxThrowUserException();
                        }
                        else {
                           // The group whose name is vName is not read yet.
                           // We will have to read this group and then come back
                           // here.

                           sParam.sGrpName=strName;

                           if (DxfLookAheadForGroup ()) {
                              if ((nIndex = DxfFindGroup (sParam,strName)) == -1) {
                                 TRACE0("Error : Group not found\n");
                                 AfxThrowUserException();
                              }
                           }
                           else
                              break;
                        }
                     }

                     // duplicate the group

                     vDupParams.fDupDeltas.Clear () ;
                     vDupParams.fScaleFactor.fN = vDupParams.fScaleFactor.fD = 1L;
                     vDupParams.fResChangeDbl = 0.0;

                     vDupParams.fOwnerGroup = 0L;
                     vDupParams.fCurrentMemberNum = 0;

                     vDupParams.fHdlMapList = newRefList (0L);

                     vDupParams.fDrawIt = vDupParams.fAddSelected = false;

                     vDupParams.fInhibitSave = false; **
                                                 * added to fix random disappearance of blocks from document,
                                                 * which was due to this field being left uninitialized     -- 24.Aug.1995.sjr
                                                 **

                     vApplyParams.fSrcLayer = vGrpNameElem.fLayerHdl;
                     vApplyParams.fDestLayer = sActLayerHdl;

                     vApplyParams.fCmd = kDuplicate;
                      sAddGrpHdl = (CMDGroupShape*)groupDuplicate(&vApplyParams, vGrpNameElem.fGrpHdl,
                                                   (DupParams *)&vDupParams);

                     if (sAddGrpHdl == 0L) {
                        vErrCode = kDxfErr;
                        break;
                     }
                     disposeRefList (vDupParams.fHdlMapList, 0L);

                     vGrpNameElem.fShareCount++;
                     updateFList (sGrpFList, vIndex, (char *)&vGrpNameElem);
                  }
                  else {
                     if ( sParam.sShape & kGroupShape ) {
                        short       Index;
                        //GroupNameElem   vGrpNameElem;

                        if ( sParam.sSectionId == kEntitiesTab )
                           break;

                        if (sParam.sLookAheadForGrp) {
      //                    If it is not the group that we are looking for then
      //                      ignore it.

                           if (strName!=sParam.sGrpName)) {
                              sParam.sInsert = false;
                              sParam.sShPtCnt = 0;
                              sParam.sAddGrpHdl = 0L;
                              if (sParam.sGrShHdl)
                                 delete sParam.sGrShHdl;
                              if (sShHdl)
                                 delete sParam.sShHdl;
                              sParam.sShHdl = 0L;
                              sParam.sGrShHdl = 0L;
                              sParam.sShapeID = 0;
                              sParam.sShape = kNoShape;
                              sParam.sDimensionOk = false;
                              sParam.sXScaleFactor = sParam.sYScaleFactor = 0.0;
                              break;
                           }
                           sParam.sLAGrpFound = true;
                        }

                        setMem(&vGrpNameElem, sizeof (GroupNameElem), 0);
                        strcpy ((Ptr)&(vGrpNameElem.fName), (char * )pStrName);

                        if ((vIndex = findKFList (sGrpFList, (char * )(&vGrpNameElem.fName),
                           (char *)&sGroupNameElem)) == -1)
                        {  setMem(&sGroupNameElem, sizeof (GroupNameElem), 0);

                           sGroupNameElem.fGrpHdl = sGrShHdl;
                           sGroupNameElem.fLayerHdl = sActLayerHdl;
                           strcpy ((char * )&(sGroupNameElem.fName), (char * )pStrName);

                           addFList (sGrpFList, (char *)&sGroupNameElem);
                           if (MemError ())
                              vErrCode = kDxfErr;
                        }
                        else {
                           sIgnoreGroup = true;
                           if (sGrShHdl)
                              delete sGrShHdl;
                           sGrShHdl = 0L;
                           sShHdl = 0L;
                           sInsert = sShapeID = 0;
                           sShape = kNoShape;
                           sRdGrpBase = false;
                        }
                     }
                  }
               break;
      */
      case kLayerName:
         if ( sParam.sShape || sParam.sInsert )
         {
            /* Make vStr layer as the active layer */
            //'''if ( sParam.sLayerList )  {
//                DxfLayerStruct vLayerStruct;
//                unsigned char  vCurLayerStr[kDxfLayerNameLen];
            short       nIdx;

            /* Current Layer Name */
//                setMem(vCurLayerStr, kDxfLayerNameLen, 0);
//                strcpy ((char *)vCurLayerStr, (char *)pStrName);

            if ((nIdx = DxfFindLayer (strName, sParam)) == -1)
               DxfUseCurLayer(strName, sParam);


            bool  vSetCurLayer = true;

            nIdx = DxfFindLayer (strName, sParam);
//                if ( sParam.sSectionId == kBlockTab )
//                   if ( !sParam.sBlkLayerHdl ) ???
//                      vSetCurLayer = true;
//                   else
//                      vSetCurLayer = false;

            if ( GetActiveLayer() != m_Layers[nIdx])
            {
               SetActiveLayer(nIdx);
//                   ((CLayer*)m_Layers.GetAt(nIdx))->m_bGrayed=false;
               ((CLayer*)m_Layers.GetAt(nIdx))->m_bHidden=false;
            }
            sParam.sCurLayerStr=strName;

//                   sDefColorIdx = vLayerStruct.fLayerColor;
//                   sDefaultDashIdx = vLayerStruct.fDashIdx;
//          } /* if sLayerList */

            if ( sParam.sGrShHdl.GetCount() && !sParam.sIgnoreLayer )
               sParam.sIgnoreLayer = true;
         }
         break;

      default:
         if ( nGrpCode == kLineTypeName && (sParam.sSectionId == kEntitiesTab || sParam.sSectionId ==kBlockTab) )
            sParam.sEntityDashIdx = DxfGetColDashIdx(strName);
         break;
      } /* inner switch */
      } /* default for outer switch */
   break;
   } /* outer switch */

   return;
}

short CPCDraftDoc::DxfGetColLnW(short pAcadLnW)
{
   short   vColLnW;

   if ( pAcadLnW <= kLnW1 )
      vColLnW = kLnW1;
   else if ( pAcadLnW <= kLnW2 )
      vColLnW = kLnW2;
   else if ( pAcadLnW <= kLnW3 )
      vColLnW = kLnW3;
   else if ( pAcadLnW <= kLnW5 )
      vColLnW = kLnW5;
   else if ( pAcadLnW <= kLnW7 )
      vColLnW = kLnW7;
   else if ( pAcadLnW <= kLnW9 )
      vColLnW = kLnW9;
   else if ( pAcadLnW <= kLnW11 )
      vColLnW = kLnW11;
   else
      vColLnW = kLnW13;

   return vColLnW;
}

void CPCDraftDoc::DxfUseCurLayer (CString    pLayerName, DxfParamStruct& sParam)
{

   AddLayer(&pLayerName);
   if (sParam.bFirstLayer)
   {
      DeleteLayer(0);
      sParam.bFirstLayer = false;
   }
   sParam.sCurLayerStr=pLayerName;
   sParam.sLayerList=true;
   DxfFindLayer(pLayerName, sParam);
}

short CPCDraftDoc::DxfFindLayer(CString strLayerName, DxfParamStruct& sParam)
{
   short i;
   for (i=0; i<m_Layers.GetSize(); i++)
   {
      if (strLayerName==m_Layers[i]->m_strName)
      {
         SetActiveLayer(i);
         sParam.bFirstLayer = false;
         return i;
      }
   }
   return -1;
}

short CPCDraftDoc::DxfGetColDashIdx (CString strLineStyleStr)
{
   short   vColPenTy = 3, vStrId;

   vStrId = DxfFindIDfromStrName (strLineStyleStr, kLineStyleTab);

   switch (vStrId)
   {
   case kDxfContinuous:
      vColPenTy = 0;
      break;
   case kDxfCustomLn01:
      vColPenTy = 1;
      break;
   case kDxfCustomLn02:
      vColPenTy = 2;
      break;
   case kDxfCustomLn03:
      vColPenTy = 3;
      break;
   case kDxfCustomLn04:
      vColPenTy = 4;
      break;
   case kDxfCustomLn05:
      vColPenTy = 5;
      break;
   case kDxfCustomLn06:
      vColPenTy = 6;
      break;
   case kDxfCustomLn07:
      vColPenTy = 7;
      break;
   case kDxfCustomLn08:
      vColPenTy = 8;
      break;
   case kDxfCustomLn09:
      vColPenTy = 9;
      break;
   case kDxfCustomLn10:
      vColPenTy = 10;
      break;
   case kDxfCustomLn11:
      vColPenTy = 11;
      break;
   case kDxfCustomLn12:
      vColPenTy = 12;
      break;
   case kDxfCustomLn13:
      vColPenTy = 13;
      break;
   case kDxfCustomLn14:
      vColPenTy = 14;
      break;
   case kDxfCustomLn15:
      vColPenTy = 15;
      break;

   }

   return vColPenTy;
} /* getColDashIdx */

void CPCDraftDoc::DxfResetLStyleTab (CString strName,short pTabIdx )
{
   DXFTablePtr vTabPtr = 0;
   short vStrId;

   vStrId = DxfFindIDfromStrName (strName, kLineStyleTab);

   if ( (pTabIdx < kMaxLineStyleTab)  &&  (pTabIdx > 0)  && (vStrId == kNilOff) )
   {
      vTabPtr = (DXFTablePtr)gLineStyleTab;
      vTabPtr += pTabIdx;
      strcpy ((char * )vTabPtr->fVarStr, strName);
   }
} /* resetLStyleTab */


void CPCDraftDoc::DxfUpdtShape (bool pUpdtGr, DxfParamStruct& sParam)
{

   if ( sParam.sSectionId != kBlockTab && !sParam.sShHdl && !sParam.sGrShHdl.GetCount() && !sParam.sInsert )
      return;


   sParam.sIgnoreShape = false;
   if ( !sParam.sInsert )
   {
      switch ( sParam.sShapeID )
      {
      case IDLine:
         DxfUpdtLine (sParam);
         break;
      case IDPolygon:
         switch (sParam.sPlTrSolOk)
         {
         case kPLineOk:
            if (sParam.sBulgeListHdl.GetSize())
            {
               short nNumBulge;

               nNumBulge = sParam.sBulgeListHdl.GetSize();
               if ((nNumBulge == 2) && (sParam.sShPtCnt == 2))
               {
                  DxfBulgeStruct vDxfBulgeStruct;

                  // SMK.SSA What this means???
                  //if (sParam.sBulgeListHdl[0].fBulge == kOnePixel) {
                  // if ( sParam.sBulgeListHdl[0].fBulge == kOnePixel )
                  //    sParam.sPolyFlag = kDonutFg;
                  // else
                  //    sParam.sPolyFlag = kCurveFitFg;
                  //}
                  //else
                  // sParam.sPolyFlag = kCurveFitFg;
                  if (sParam.sBulgeListHdl[0].fBulge == kOnePixel)
                     sParam.sPolyFlag = kDonutFg;
                  else
                     sParam.sPolyFlag = kCurveFitFg;


               }
               else if (nNumBulge && (sParam.sShPtCnt > 2))
                  sParam.sPolyFlag = kCurveFitFg;
            }

            if ( sParam.sPolyFlag )
               DxfUpdtAddPolyByFlag (sParam.sPolyFlag, sParam);
            else
               DxfUpdtAddPoly (kPLineOk, sParam);
            break;

         default:
            DxfUpdtAddPoly (sParam.sPlTrSolOk, sParam);
         }

         if ( sParam.sLnWListHdl.GetSize() )
            sParam.sLnWListHdl.RemoveAll();


         if ( sParam.sBulgeListHdl.GetSize() )
            sParam.sBulgeListHdl.RemoveAll();
         sParam.sPlTrSolOk = kPLineOk;
         break;

      case IDArc:
         DxfUpdtArc (sParam);
         break;

      case IDCircle:
         DxfUpdtCircle (sParam);
         break;

      case IDText:
         DxfUpdtText(sParam.sTextStr,sParam);
         sParam.sTextStr="";
         break;
      }
   }

   if (pUpdtGr || sParam.sInsert)
   {
      if (sParam.sInsert && (sParam.sAddGrpHdl != 0L))
      {
         /*       DragByParams   vDragParams;
                  ApplyParams    applyParams;

                  // STEP 1: SCALE/FLIP the group
                  if ( !sDimensionOk && (sXScaleFactor || sYScaleFactor) ) {
                     WrFixed  scaleH, scaleV;

                     if ( sXScaleFactor == 0.0 ) sXScaleFactor = 1.0;
                     if ( sYScaleFactor == 0.0 ) sYScaleFactor = 1.0;
                     scaleH = mtRealToFix (sXScaleFactor);
                     scaleV = mtRealToFix (sYScaleFactor);

                     if ( !(dxfGroupResize (sActLayerHdl, sAddGrpHdl, scaleH, scaleV)) )
                        vErrCode = kDxfErr;

                     sXScaleFactor = 0.0;
                     sYScaleFactor = 0.0;
                  }

                  // STEP 2: ROTATE the group
                  if ( !sDimensionOk && sShPtCnt == 2 ) {
                     RotByParams    vRotParams;

                     if ((vRotParams.fAngle = (sFdPtPtr - 1)->h) != FixedZero )
                     {
                        vRotParams.fRotAbout = kAboutRefPoint;
                        vRotParams.fRefPoint.Clear () ;

                        vRotParams.fSinCos = TSinCos ( vRotParams.fAngle ) ;

                        applyParams.fSrcLayer = sActLayerHdl;
                        applyParams.fDestLayer = 0L;
                        applyParams.fCmd = kRotate;
                        groupRotBy (sActLayerHdl, sAddGrpHdl, &vRotParams);
                     }
                  }


                  // STEP 3: DRAG the group (for inserting the group)
                  // Drag the group sAddGrpHdl by vDragDelta
                  if ( !sDimensionOk && (sShPtCnt >= 1) ) {
                     LongPoint * vFdPtPtr;

                     HUnlock (sPtHdl);
                     HLock (sPtHdl);
                     vFdPtPtr = sFdPtPtr = (LongPointPtr)(*sPtHdl);

                     vDragParams.fDragDeltas.h = vFdPtPtr->h;
                     vDragParams.fDragDeltas.v = vFdPtPtr->v;

                     vDragParams.fDragList = 0L;

                     if ( sAddGrpHdl->cID != IDGroup)
                        return kDxfErr;

                     applyParams.fSrcLayer = sActLayerHdl;
                     applyParams.fDestLayer = 0L;
                     applyParams.fCmd = kDrag;
                     dragByDo (&applyParams, sAddGrpHdl, &vDragParams);

                     sAddGrpHdl->cExtent.Offset ( vDragParams.fDragDeltas ) ;
                  }

                  // Add the group sAddGrpHdl to the current group
                  if ( sShape &= kGroupShape ) {
                     addGrpMem(sAddGrpHdl);
                     sShape = kGroupShape; // Leave only the group shape flag
                     sShapeID = 0;
                  }
                  else {
                     sShape = kNoShape;
                     sGrShHdl = 0L;
                     sShapeID = 0;
                  }

                  sAddGrpHdl = 0L;

                  sShPtCnt = 0;
                  sInsert = false;
                  sShHdl = 0L;
                  // Initialise the pt array pointer
                  HUnlock (sPtHdl);
                  HLock (sPtHdl);
                  sFdPtPtr = (LongPointPtr)(*sPtHdl);
                  if ( sDimensionOk )
                     sDimensionOk = false;*/
      }
   }

   sParam.sXScaleFactor = 0.0;
   sParam.sYScaleFactor = 0.0;

}

void CPCDraftDoc::DxfCreateLine(DxfParamStruct& sParam)
{
   //Size         nShapeSize = sizeof (CMDLineShape);
   //long         total, contig;

   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sVertexOk = false;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sLnWidth = 1;

   sParam.sShape &= kGroupShape; // Leave only the group shape flag

   // Initialise the pt array pointer
   sParam.sPtHdl.RemoveAll();
   sParam.sPtHdl.Add(LongPoint());
   sParam.sFdPtPtr = 0;

   //PurgeSpace (&total, &contig);
   //if (vShapeSize > contig) return kDxfErr;
   if ((sParam.sShHdl = new(CMDLineShape)) == 0L)
   {
      TRACE0("Error : insufficient memory \n");
      AfxThrowUserException();
   }


   //((CMDLineShape*)sShHdl)->IMDLineShape();

   sParam.sShapeID = IDLine;

   // Initialise the statics
   sParam.sShape |= kLineShape; // Add the line shape flag


} /* createLine */

/*---------------------------------------------------------------------*/

/*
   This function updates the attributes of the Line and
   then adds it to the database.

   NOTE: It also updates the static variables.
*/

void CPCDraftDoc::DxfUpdtLine(DxfParamStruct& sParam)
{

   if ( sParam.sShHdl && sParam.sPtHdl.GetSize() >= 2 )
   {

      // Set the shape points.
      sParam.sShHdl->SetShapePoint(0,sParam.sPtHdl.GetAt(0));
      sParam.sShHdl->SetShapePoint(1,sParam.sPtHdl.GetAt(1));

      // Set the default fill and pen.
      DxfSetShPenAndFill(true, sParam);
      PenModel penModel=sParam.sShHdl->GetPenModel();
      penModel.m_nWidth = (uchar)sParam.sLnWidth;

      sParam.sShHdl->SetPenModel(penModel);
      sParam.sShHdl->CalculateExtents();
      DxfAddGrpMem (sParam);

      GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);
   }

   sParam.sShHdl = 0L;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sShapeID = 0;

   sParam.sVertexOk = false;
   sParam.sLnWidth = 1;

   sParam.sShape &= kGroupShape; // Leave only the group shape flag


} /* updtLine */

/*---------------------------------------------------------------------*/

void CPCDraftDoc::DxfCreateArc (DxfParamStruct& sParam)
{

   //Size      vShapeSize = sizeof (CMDArcShape);
   //short     vErrCode = kDxfNoErr;
   //long      total, contig;

   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sLnWidth = 1;

   // Initialise the pt array pointer

   sParam.sFdPtPtr = 0;
   sParam.sPtHdl.RemoveAll();
   sParam.sPtHdl.Add(LongPoint());


   if ( (sParam.sShHdl = new(CMDArcShape)) == 0L)
   {
      TRACE0("Error : insufficient memory \n");
      AfxThrowUserException();
   }
   sParam.sShapeID = IDArc;

   ((CMDArcShape*)sParam.sShHdl)->m_nMode = CMDArcShape::kRadius;

   // Initialise the statics
   sParam.sShape |= kArcShape; // Add the Arc shape flag

} /* createArc */

/*---------------------------------------------------------------------*/

/*
   This function updates the attributes of the Arc and
   then adds it to the database.

   NOTE: It also updates the static variables.
*/
void CPCDraftDoc::DxfUpdtArc (DxfParamStruct& sParam)
{
   Coord    hRad, hRad1;
   LongPoint   centerPt, p1, p2, p3;
   Angle    vStartAng, vEndAng;


   if ( sParam.sShHdl )
   {

      centerPt = sParam.sPtHdl[0];
      hRad = sParam.sPtHdl[1].x;
      hRad1 = sParam.sPtHdl[1].x;
      vStartAng = mtFixAngle(sParam.sPtHdl[2].x);
      vEndAng = mtFixAngle(sParam.sPtHdl[2].y);

      DxfCalcArcPoints (&centerPt, LongPoint ( hRad, hRad1 ), vStartAng, vEndAng, &p1, &p2, &p3);
      sParam.sShHdl->SetShapePoint(0, centerPt);
      sParam.sShHdl->SetShapePoint(1, p1);
      sParam.sShHdl->SetShapePoint(2, p2);
//    sParam.sShHdl->SetShapePoint(3, p3);
      // Set the default fill and pen.

      DxfSetShPenAndFill(false, sParam);
      PenModel penModel=sParam.sShHdl->GetPenModel();
      penModel.m_nWidth = (uchar)sParam.sLnWidth;

      sParam.sShHdl->SetPenModel(penModel);
      sParam.sShHdl->CalculateExtents();
      DxfAddGrpMem (sParam);

      GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);

   }

   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sShapeID = 0;

   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; //Leave only the group shape flag
   sParam.sLnWidth = 1;
}

/*---------------------------------------------------------------------*/

void CPCDraftDoc::DxfCreateCircle (DxfParamStruct& sParam)
{


   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sLnWidth = 1;

   // Initialise the pt array pointer
   sParam.sPtHdl.RemoveAll();
   sParam.sPtHdl.Add(LongPoint());
   sParam.sFdPtPtr = 0;


   if ((sParam.sShHdl = new CMDCircle(CMDCircle::kRadius)) == 0L)
   {
      TRACE0("Error : insufficient memory \n");
      AfxThrowUserException();
   }

   sParam.sShapeID = IDCircle;

   // Initialise the statics
   sParam.sShape |= kCircleShape; // Add the Circle shape flag

} /* createCircle */

/*---------------------------------------------------------------------*/

/*
   This function updates the attributes of the Arc and
   then adds it to the database.

   NOTE: It also updates the static variables.
*/
void CPCDraftDoc::DxfUpdtCircle (DxfParamStruct& sParam)
{

   Coord       hRad;
   LongPoint      centerPt, p1;


   if ( sParam.sShHdl )
   {

      centerPt = sParam.sPtHdl[0];
      hRad = sParam.sPtHdl[1].x;
      p1.x = centerPt.x + hRad;
      p1.y = centerPt.y;

      sParam.sShHdl->SetShapePoint(0, sParam.sPtHdl[0]);
      sParam.sShHdl->SetShapePoint(1, p1);
      //newdbNewLPoint (&vShPtr->cShapeCenter, centerPt, kRealVertex);
      //newdbNewLPoint (&vShPtr->cPoint[0], p1, kRealVertex);

      // Set the default fill and pen.
      DxfSetShPenAndFill(true, sParam);
      PenModel penModel=sParam.sShHdl->GetPenModel();
      penModel.m_nWidth = (uchar)sParam.sLnWidth;

      sParam.sShHdl->SetPenModel(penModel);
      sParam.sShHdl->CalculateExtents();
      DxfAddGrpMem (sParam);

      GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);
   }


   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sShapeID = 0;

   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sLnWidth = 1;

} /* updtCircle */

/*---------------------------------------------------------------------*/

void CPCDraftDoc::DxfCreateText (DxfParamStruct& sParam)
{

   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sLnWidth = 1;

   sParam.sHorizAlign = 0;
   sParam.sVertAlign = 0;
   sParam.sRotAngle = FixedZero ;

   // Initialise the pt array pointer
   sParam.sPtHdl.RemoveAll();
   sParam.sPtHdl.Add(LongPoint());
   sParam.sFdPtPtr = 0;

   if ((sParam.sShHdl = new CMDText(CMDText::kTextUserSize)) == 0L)
   {
      TRACE0("Error : insufficient memory \n");
      AfxThrowUserException();
   }

   sParam.sShape |= kTextShape; // Add the Text shape flag
   sParam.sShapeID = IDText;
} /* createText */

/*---------------------------------------------------------------------*/

/*
   This function updates the attributes of the Text and
   then adds it to the database.

   NOTE: It also updates the static variables.
*/
void CPCDraftDoc::DxfUpdtText ( CString pTxtStr, DxfParamStruct& sParam)
{

   LongPoint lpPoint;

   if ( sParam.sShHdl )
   {
      sParam.sFdPtPtr = 0;

      DxfSetShPenAndFill(true, sParam);
      PenModel penModel=sParam.sShHdl->GetPenModel();
      penModel.m_nWidth = (uchar)sParam.sLnWidth;

      sParam.sShHdl->SetPenModel(penModel);

      TextParams tp;
      tp.m_nJust=kAlignLeft;
      tp.m_nSpacing=kSingleSpace;

      CStyledText st;
      CStyledTextStyle s;

      s.m_LogFont=tp.m_lf;
      s.m_LogFont.lfHeight=(long)mtFixToReal(sParam.sPtHdl[1].x);
      s.m_LogFont.lfWeight=FW_NORMAL;
      s.m_LogFont.lfItalic=FALSE;
      s.m_LogFont.lfUnderline=FALSE;
      s.m_TextColor=penModel.m_crColor;

      CSize size;
      CWindowDC hDC(NULL);
      CFont* pOldFont,*pFont = new CFont();
      pFont->CreateFontIndirect(&s.m_LogFont);
      pOldFont=hDC.SelectObject(pFont);
      size = hDC.GetTextExtent(pTxtStr);
      hDC.SelectObject(pOldFont);
      delete pFont;

      sParam.sShHdl->SetShapePoint(0,sParam.sPtHdl[0]);
      sParam.sShHdl->SetShapePoint(3,sParam.sPtHdl[0]+LongPoint(0,(int)size.cy));
      sParam.sShHdl->SetShapePoint(2,sParam.sPtHdl[0]+LongPoint((int)size.cx,(int)size.cy));
      sParam.sShHdl->SetShapePoint(1,sParam.sPtHdl[0]+LongPoint((int)size.cx,0));
      sParam.sShHdl->SetTextParams(tp);


      for (int i = 0; i < pTxtStr.GetLength(); i++)
         st.Add(pTxtStr.GetAt(i),s);

      ((CMDText*)sParam.sShHdl)->SetText(st);

      if ( sParam.sRotAngle != FixedZero )
         sParam.sShHdl->DoShapeRotate(sParam.sRotAngle,kAboutRefPt,sParam.sPtHdl[0]);

      sParam.sShHdl->CalculateExtents();
      DxfAddGrpMem (sParam);
      GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);

   }
   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sShapeID = 0;
   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sLnWidth = 1;
   sParam.sHorizAlign = 0;
   sParam.sVertAlign = 0;
   sParam.sRotAngle = FixedZero ;
} /* updtText */

/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

void CPCDraftDoc::DxfCreateGroup (DxfParamStruct& sParam)
{
   /*
   short    vErrCode = kDxfNoErr;

   // Initialize the statics
   sGrShHdl = 0L;
   sVertexOk = sShape = sIgnoreLayer = false;
   sClosedSh = false;
   sShPtCnt = 0;
   sLnWidth = 1;

   // Initialise the pt array pointer
   HUnlock (sPtHdl);
   HLock (sPtHdl);
   sFdPtPtr = (LongPointPtr)(*sPtHdl);

   // Create a FList of GroupNameElem, if it does not exist
   if ( !sGrpFList ) {
      if ( !(sGrpFList = newFList (sizeof (GroupNameElem), 1, 1, 0, kMaxGrpNameLn)))
         return kDxfErr;
   }

   setMem(&sGroupNameElem, sizeof (GroupNameElem), 0);

   if (!(sGrShHdl = new(CMDGroupShape)))
      return kDxfErr;

   ((CMDGroupShape*)sGrShHdl)->IMDGroupShape (0);

   // Set header fields.
   sGrShHdl->cOwnerGroup   = 0L;
   sShapeID = sGrShHdl->cID  = IDGroup;

   // Set the default fill and pen.
   setShPenAndFill (sGrShHdl, false);     // setShPenAndFill (sShHdl, false);

   // Initialise the statics
   sRdGrpBase = true;
   sShape |= kGroupShape; // Leave only the group shape flag

   */
} /* createGroup */

/*---------------------------------------------------------------------*/

/*
   This function updates the attributes of the Group and
   then adds it to the database.

   NOTE: It also updates the static variables.
*/

void CPCDraftDoc::DxfUpdtGroup (DxfParamStruct& sParam)
{
   /*
      short          vErrCode = kDxfNoErr;
      short          vNumMem;
      CMDShape       *vLastShape, *aShapeHdl;
      register short    vIdx, vIndex;
      LongPoint         datum;
      DatumCode         datumCode;
      ApplyParams       applyParams;
      CMDGroupShape     *groupShape;


      if ( !sGrShHdl ) goto ERR_EXIT;

      HUnlock (sPtHdl);
      HLock (sPtHdl);
      sFdPtPtr = (LongPointPtr)(*sPtHdl);

      groupShape = (CMDGroupShape*)sGrShHdl;
      vNumMem = groupShape->GetNumGroupMembers();
      if ( !(vNumMem) ) {
         delete sGrShHdl;        // DisposeHandle ((Handle)sGrShHdl);
         sGrShHdl = 0L;
         goto ERR_EXIT;
      }

      if( sGrShHdl )
         groupExtents (sActLayerHdl, sGrShHdl);
      // Sort the group members in the back to front order.

      // Insert group object after the last member.
      vLastShape = groupShape->GetGroupMember(vNumMem - 1);
      dbAddNewCreation (sActLayerHdl, sGrShHdl, vLastShape, kInsAfter);

      // set groupOwner fields of members after adding group
      applyParams.fSrcLayer = sActLayerHdl;
      applyParams.fDestLayer = 0L;
      applyParams.fCmd = kGroup;
      for (vIdx=0 ; vIdx < vNumMem; vIdx++) {
         aShapeHdl = groupShape->GetGroupMember(vIdx);
         shapeApply(&applyParams, aShapeHdl, (uchar *)sGrShHdl, kBackToFront);
      }

      if( sGrShHdl )
         groupExtents (sActLayerHdl, sGrShHdl);
      calcDatum ((CMDShape*)sGrShHdl, &datum, &datumCode, false, true);

      if ( sGrpBaseXPt || sGrpBaseYPt ) {
         DragByParams   vDragParams;

         vDragParams.fDragDeltas.h = -mtRealToFix (sGrpBaseXPt);
         vDragParams.fDragDeltas.v = -mtRealToFix (sGrpBaseYPt);
         vDragParams.fDragList = 0L;

         applyParams.fCmd = kDrag;
         dragByDo (&applyParams, sGrShHdl, &vDragParams);

         sGrShHdl->cExtent.Offset ( vDragParams.fDragDeltas ) ;
      }

      if ((vIndex = findKFList (sGrpFList, (char * )&sGroupNameElem.fName,
                         (char *)&sGroupNameElem)) == -1)
         vErrCode = kDxfErr;

      sGroupNameElem.fGrpHdl = sGrShHdl;
      sGroupNameElem.fLayerHdl = sActLayerHdl;
      updateFList (sGrpFList, vIndex, (char *)&sGroupNameElem);

      // Initialise the statics
   ERR_EXIT:
      sGrShHdl = 0L;
      sClosedSh = false;
      sShPtCnt = 0;
      sShapeID = 0;
      sShape = kNoShape;

      sRdGrpBase = sVertexOk = false;
      sGrpBaseXPt = 0.0;
      sGrpBaseYPt = 0.0;
      sLnWidth = 1;
   */
} /* updtGroup */

/*---------------------------------------------------------------------*/
void CPCDraftDoc::DxfCreatePoly (DxfParamStruct& sParam)
{
   // Initialise the statics
   sParam.sShHdl = 0L;
   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sLnWidth = 1;

   // Initialise the pt array pointer
   sParam.sPtHdl.RemoveAll();
   sParam.sPtHdl.Add(LongPoint());
   sParam.sFdPtPtr = 0;

   if ((sParam.sShHdl = new(CMDPolygonShape)) == 0L)
   {
      TRACE0("Error : insufficient memory \n");
      AfxThrowUserException();
   }

   // Set header fields.
   ((CMDPolygonShape*)sParam.sShHdl)->SetPolygonType(CMDPolygonShape::kPolygon);

   sParam.sShapeID = IDPolygon;

   // Initialize the statics
   sParam.sShape |= kPolygonShape; // Add the Circle shape flag

} /* createPoly */

void CPCDraftDoc::DxfUpdtAddPoly (short pPlTrSolOk, DxfParamStruct& sParam)
{

   short          vIdx, vPtCount;
   bool           vNotForDefa = false, vForMulPoly = false;


   if ( sParam.sShHdl )
   {
      if ( !(vPtCount = sParam.sShPtCnt) )
      {
         delete sParam.sShHdl;
         sParam.sShHdl = 0L;
         sParam.sPtHdl.RemoveAll();
         sParam.sPtHdl.Add(LongPoint());
         sParam.sFdPtPtr = 0;

         goto ERR_EXIT;
      }

      // Calculate the shape points.
      switch ( pPlTrSolOk )
      {
      case kTraceOk: // Special for trace to fix MacDraft Structure
         vNotForDefa = true;
         ((CMDPolygonShape*)sParam.sShHdl)->SetNumShapePoints(4);
         sParam.sShHdl->SetShapePoint(0, sParam.sPtHdl[0]);
         sParam.sShHdl->SetShapePoint(1, sParam.sPtHdl[2]);
         sParam.sShHdl->SetShapePoint(2, sParam.sPtHdl[3]);
         sParam.sShHdl->SetShapePoint(3, sParam.sPtHdl[1]);
         break;

      case k3DFaceOk :
      case kSolidOk:
//       if (vPtCount > 4)
//          goto ERR_EXIT;
         ((CMDPolygonShape*)sParam.sShHdl)->SetNumShapePoints(4);
         vNotForDefa = true;
         sParam.sShHdl->SetShapePoint(0, sParam.sPtHdl[0]);
         sParam.sShHdl->SetShapePoint(1, sParam.sPtHdl[1]);
         sParam.sShHdl->SetShapePoint(3, sParam.sPtHdl[2]);
         sParam.sShHdl->SetShapePoint(2, sParam.sPtHdl[3]);
         break;

      default:
         ((CMDPolygonShape*)sParam.sShHdl)->SetNumShapePoints(vPtCount);

         for (vIdx = 0; vIdx < vPtCount; vIdx++)
            sParam.sShHdl->SetShapePoint(vIdx, sParam.sPtHdl[vIdx]);

      }

      // Set header fields.
      ((CMDPolygonShape*)sParam.sShHdl)->SetClosed(sParam.sClosedSh);
      // Set the default fill and pen.
      DxfSetShPenAndFill(/*vNotForDefa*/ true, sParam);
      PenModel penModel=sParam.sShHdl->GetPenModel();

      // SMK.SSA - Strange if.... What it mean?
      if ( sParam.sLnWidth )
         penModel.m_nWidth = (uchar)sParam.sLnWidth;
      else
         penModel.m_nWidth = (uchar)sParam.sLnWidth;

      sParam.sShHdl->SetPenModel(penModel);
      sParam.sShHdl->CalculateExtents();
      GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);

      DxfAddGrpMem (sParam);
   }
   // Initialise the statics
ERR_EXIT:
   sParam.sShHdl = 0L;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sShapeID = 0;
   sParam.sVertexOk = false;

   sParam.sShape &= kGroupShape; // Leave only the group shape flag
   sParam.sLnWidth = 1;

} /* updtAddPoly */

void CPCDraftDoc::DxfUpdtAddPolyByFlag (short pPolyFlag, DxfParamStruct& sParam)
{
   short       vVertIdx = -1;
   short       vLnWIdx = -1;
   short       vKeyIdx;
// CMDShape    *vShHdl = 0L;
   CMDShapeList   vCurvListHdl;
   CMDGroupShape* grShape;
   short vErrCode;


   switch ( pPolyFlag )
   {
   case kDonutFg:
      if ( sParam.sShHdl )
      {
         delete sParam.sShHdl;
         sParam.sShHdl = 0L;


         if ( DxfUpdtAddDonut (true, sParam) != 0)
            vCurvListHdl.AddTail(sParam.sShHdl);

         if (sParam.sShHdl)
            DxfUpdtAddDonut (false, sParam);

         if (sParam.sShHdl)
            vCurvListHdl.AddTail(sParam.sShHdl);

         if (vCurvListHdl.GetCount())
         {
            grShape=new CMDGroupShape(&vCurvListHdl);
            grShape->CalculateExtents();
            GetActiveLayer()->m_ShapeList.AddTail(grShape);
            sParam.sShHdl=grShape;
            DxfAddGrpMem (sParam);
         }
      }
      break;
   case kCurveFitFg:
      if (sParam.sShHdl && sParam.sBulgeListHdl.GetSize())
      {
         short vIdx;
         WrFixed  vBulgeAng;

         delete sParam.sShHdl;
         sParam.sShHdl = 0L;

         for (vIdx = 0; vIdx < sParam.sShPtCnt; vIdx++)
         {
            vKeyIdx = vIdx + 1;
            vVertIdx=-1;
            int i = 0;
            for (; i<sParam.sBulgeListHdl.GetSize(); i++)
               if (vKeyIdx==sParam.sBulgeListHdl[i].fVertIdx)
               {
                  vVertIdx = i;
                  break;
               }

            vLnWIdx=-1;
            for (i=0 ; i<sParam.sLnWListHdl.GetSize(); i++)
               if (vKeyIdx==sParam.sLnWListHdl[vVertIdx].fVertIdx)
               {
                  vLnWIdx = i;
                  break;
               }

            if (vVertIdx != -1)
               vBulgeAng = sParam.sBulgeListHdl[vVertIdx].fBulge;

            if (vLnWIdx != -1)
               sParam.sLnWidth = sParam.sLnWListHdl[vLnWIdx].fLnW;

            if (vIdx == (sParam.sShPtCnt - 1))
            {
               if ( vVertIdx == -1 )
               {
                  if (DxfUpdtAddLine (&sParam.sPtHdl[vIdx], &sParam.sPtHdl[0], sParam) == 0L)
                     break;
               }
               else
               {
                  if (DxfUpdtAddCurveFit (&sParam.sPtHdl[vIdx], &sParam.sPtHdl[0], vBulgeAng, sParam) == 0L)
                     break;
               }
            }
            else
            {
               if (vVertIdx == -1)
               {
                  if (DxfUpdtAddLine (&sParam.sPtHdl[vIdx], &sParam.sPtHdl[vIdx+1], sParam) == 0L)
                     break;
               }
               else
               {
                  if (DxfUpdtAddCurveFit (&sParam.sPtHdl[vIdx], &sParam.sPtHdl[vIdx+1], vBulgeAng, sParam) == 0L)
                     break;
               }
            }

            if (sParam.sShHdl)
               vCurvListHdl.AddTail(sParam.sShHdl);

            if (!sParam.sShHdl)
               break;
            sParam.sShHdl = 0L;
         } /* for vIdx */

         if (sParam.sShHdl && ((grShape=new CMDGroupShape(&vCurvListHdl)) != 0) )
         {
            grShape->CalculateExtents();
            GetActiveLayer()->m_ShapeList.AddTail(grShape);
         }
         else
            vErrCode = kDxfErr;

      }
      else
      {
         if (sParam.sShHdl)
         {
            delete sParam.sShHdl;
            sParam.sShHdl = 0L;
         }
      }
      sParam.sShHdl = 0L;

      if ((vErrCode == kDxfNoErr) && grShape)
      {
         sParam.sShHdl=grShape;
         DxfAddGrpMem (sParam);
      }
      break;
   default:
      DxfUpdtAddPoly (kPLineOk, sParam);
   }

   /* Initialise the statics */
//ERR_EXIT:
   sParam.sPolyFlag = 0;
   sParam.sShHdl = 0L;
   sParam.sClosedSh = false;
   sParam.sShPtCnt = 0;
   sParam.sShapeID = 0;
   sParam.sVertexOk = false;
   sParam.sShape &= kGroupShape; /* Leave only the group shape flag */
   sParam.sLnWidth = 1;

} /* updtAddPolyByFlag */


/*
   This function updates the attributes of the Donuts.
*/

CMDShape* CPCDraftDoc::DxfUpdtAddDonut (bool pDownSide, DxfParamStruct& sParam)
{
// CMDShape*      vShHdl = 0L;
   Coord       hRad;
   LongPoint      centerPt, p1, p2, p3;


   if (sParam.sShHdl)
   {
      delete sParam.sShHdl;
      sParam.sShHdl = 0L;
   }

   if ((sParam.sShHdl = new(CMDArcShape)) == 0L)
      return 0L;


   sParam.sShapeID = IDArc;
   ((CMDArcShape*)sParam.sShHdl)->m_nMode = CMDArcShape::kRadius;

   hRad = Pythag ( sParam.sPtHdl[0] - sParam.sPtHdl[1]) ;
   midPt ( &sParam.sPtHdl[0], &sParam.sPtHdl[1], &centerPt );
   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(0,centerPt);

   //vShPtr->cRadius = hRad ;

   if ( pDownSide )
   {
      p1 = sParam.sPtHdl[0];
      p2 = sParam.sPtHdl[1];
   }
   else
   {
      p1 = sParam.sPtHdl[1];
      p2 = sParam.sPtHdl[0];
   }

   DxfSetFdPtToArc (&centerPt, hRad, hRad, &p2, NULL, kCenterRadius);
   DxfSetFdPtToArc (&centerPt, hRad, hRad, &p1, NULL, kCenterRadius);

   p3 = p2;
   DxfSetFdPtToArc (&centerPt, hRad, hRad, &p1, &p3, kArcMidPt);

   /* Convert angles to Quickdraw angles before saving them in the shape. */
// vShPtr->cStartAng = mtFixAngle (mtAngle ( centerPt , p1 ) - 90 ) ;
// vShPtr->cEndAng = mtFixAngle (mtAngle ( centerPt , p2 ) - 90 ) ;

   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(1, p1);
   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(2, p2);
   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(3, p3);

   /* Set the default fill and pen. */

   DxfSetShPenAndFill(true, sParam);
   PenModel penModel=sParam.sShHdl->GetPenModel();
   penModel.m_nWidth = (uchar)sParam.sLnWidth;
   sParam.sShHdl->SetPenModel(penModel);
   sParam.sShHdl->CalculateExtents();
   DxfAddGrpMem (sParam);

   GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);

   return sParam.sShHdl;

} /* updtAddDonut */

/*
  This function finds the intersection of a circle and a half line passing
  through the center of the circle. The circle is given by pCenter and
  pRadius. The line is given by the two points, pCenter and pScndPt.
  The result is given in pScndPt.
*/
void CPCDraftDoc::DxfSetFdPtToArc (
LongPoint*     pCenter,
Coord          pHRad,
Coord          pVRad,
LongPoint*     pScndPt,
LongPoint*     pThrdPt,
short       pCode )
{
   LongPoint   vVect = *pScndPt;
   double vDist = RawDoublePythag ( *pCenter - vVect ); // ?????
   if ( ! vDist )
      return;

   vVect -= *pCenter ;

   switch ( pCode )
   {
   case kCenterRadius:
      vDist = pHRad / vDist;
      vVect.x *= vDist;
      vVect.y *= vDist;
      *pScndPt = *pCenter + vVect ;
      break;

   case kEllipse:
   {
      Angle vAng = ( mtEATan2 ( vVect, pHRad, pVRad ) ) ;

      vVect.x = mtFixMulDbl ( pHRad, mtCos (vAng) );
      vVect.y = mtFixMulDbl ( pVRad, mtSin (vAng) );
      *pScndPt = *pCenter + vVect ;
   }
   break;

   case kArcMidPt:
   {
      Angle vStartAng = ( mtEATan2 ( vVect, pHRad, pVRad ) ) ;

      vVect = *pThrdPt - *pCenter ;
      Angle vAng = ( mtEATan2 ( vVect, pHRad, pVRad ) ) ;

      if ( vAng < vStartAng )
         vAng += k360;
      vAng = mtFixAngle ((vAng + vStartAng) >> 1);
      vVect.x = mtFixMulDbl ( pHRad, mtCos (vAng) );
      vVect.y = mtFixMulDbl ( pVRad, mtSin (vAng) );
      *pThrdPt = *pCenter + vVect ;
   }
   break;

   } /* switch (pCode) */
} /* setFdPtToArc */

/*---------------------------------------------------------------------*/

CMDShape* CPCDraftDoc::DxfUpdtAddCurveFit (
LongPoint*     pCurFdPtr,
LongPoint*     pNextFdPtr,
Angle       pAng,
DxfParamStruct& sParam  )
{
// CMDShape*      vShHdl;
// Size        vShapeSize = sizeof (CMDArcShape);
   Coord       vRadius;
   LongPoint      centerPt, p1, p2, p3;
   bool        vClockWise = false;


   if (sParam.sShHdl)
   {
      delete sParam.sShHdl;
      sParam.sShHdl = 0L;
   }

   if ((sParam.sShHdl = new(CMDArcShape)) == 0L)
      return 0L;

   sParam.sShapeID = IDArc;
   ((CMDArcShape*)sParam.sShHdl)->m_nMode = CMDArcShape::kElliptic;

   if ( pAng < FixedZero )
   {
      pAng = mtFixAngle (-pAng);
      vClockWise = true;
   }
   pAng = mtRealToFix ((atan (mtFixToReal (pAng)) * kRadToAng * 4 ));

   if (!vClockWise)
   {
      centerPt = DxfGetCentPt (pCurFdPtr, pNextFdPtr, pAng, &vRadius);
      p1 = *pCurFdPtr;
      p2 = *pNextFdPtr;
   }
   else
   {
      centerPt = DxfGetCentPt (pNextFdPtr, pCurFdPtr, pAng, &vRadius);
      p1 = *pNextFdPtr;
      p2 = *pCurFdPtr;
   }

   DxfSetFdPtToArc (&centerPt, vRadius, vRadius, &p2, NULL, kCenterRadius);
   DxfSetFdPtToArc (&centerPt, vRadius, vRadius, &p1, NULL, kCenterRadius);

   p3 = p2;
   DxfSetFdPtToArc (&centerPt, vRadius, vRadius, &p1, &p3, kArcMidPt);

   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(0, centerPt);

   //vShPtr->cRadius = vRadius;

   /* Convert angles to Quickdraw angles before saving them in the shape. */
   //vShPtr->cStartAng = mtFixAngle ( k90 - mtAngle ( centerPt , p1 ) ) ;
   //vShPtr->cEndAng = mtFixAngle ( k90 - mtAngle ( centerPt , p2 ) ) ;
   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(1, p1);
   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(2, p2);
   ((CMDArcShape*)sParam.sShHdl)->SetShapePoint(3, p3);

   /* Set the default fill and pen. */
   DxfSetShPenAndFill(true, sParam);
   PenModel penModel=sParam.sShHdl->GetPenModel();
   penModel.m_nWidth = (uchar)sParam.sLnWidth;
   sParam.sShHdl->SetPenModel(penModel);
   sParam.sShHdl->CalculateExtents();
   DxfAddGrpMem (sParam);

   GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);

   return sParam.sShHdl;
} /* updtAddCurveFit */

LongPoint CPCDraftDoc::DxfGetCentPt (
LongPoint*     pCurFdPtr,
LongPoint*     pNextFdPtr,
Angle       pAng,
Coord       *pRadius)
{
   LongPoint   vCenPt;
   double      vR, vA, vX1, vY1, vX2, vY2, vCx, vCy;
   Angle    vRotAng;

   pAng >>= 1;

   vCx = vX1 = mtFixToReal(pCurFdPtr->x);
   vCy = vY1 = mtFixToReal(pCurFdPtr->y);
   vX2 = mtFixToReal (pNextFdPtr->x);
   vY2 = mtFixToReal (pNextFdPtr->y);

   vA = sqrt ((vX1 - vX2) * (vX1 - vX2) + (vY1 - vY2) * (vY1 - vY2));

   pAng = k360 - pAng;  /* For matching mtSin & mtCos parameters*/

   if ((vR = mtSin (pAng)) != 0)
      vR = vA / (vR * 2);

   vCx += vR * mtSin (pAng);
   vCy -= vR * mtCos (pAng);

   vRotAng = mtAngle ( *pCurFdPtr, *pNextFdPtr ) ;
   vCenPt.x = mtRealToFix (vCx);
   vCenPt.y = mtRealToFix (vCy);

   vCenPt = mtRotPoint ( vCenPt, vRotAng, *pCurFdPtr );

   vR = (vR < 0.0) ? -vR : vR;
   *pRadius = mtRealToFix (vR);

   return vCenPt;
} /* getCentPt */

/*---------------------------------------------------------------------*/

/*
   This function creates and updated the Line for Polygon
*/

CMDShape* CPCDraftDoc::DxfUpdtAddLine (
LongPoint*     pCurFdPtr,
LongPoint*     pNextFdPtr,
DxfParamStruct& sParam  )
{

   if ((sParam.sShHdl = new(CMDLineShape)) == 0L)
      return 0L;

   sParam.sShapeID = IDLine;

   ((CMDLineShape*)sParam.sShHdl)->SetShapePoint(0,*pCurFdPtr);
   ((CMDLineShape*)sParam.sShHdl)->SetShapePoint(0,*pNextFdPtr);

   DxfSetShPenAndFill(true, sParam);
   PenModel penModel=sParam.sShHdl->GetPenModel();
   penModel.m_nWidth = (uchar)sParam.sLnWidth;

   sParam.sShHdl->SetPenModel(penModel);
   sParam.sShHdl->CalculateExtents();
   DxfAddGrpMem (sParam);

   GetActiveLayer()->m_ShapeList.AddTail(sParam.sShHdl);

   return sParam.sShHdl;
} /* updtAddLine */


void CPCDraftDoc::DxfAddGrpMem (DxfParamStruct& sParam)
{

   sParam.sGrShHdl.AddTail(sParam.sShHdl);

   if ( sParam.sAddGrpHdl == sParam.sShHdl )
   {
      sParam.sAddGrpHdl = 0L;
      sParam.sInsert = false;
   }

   return;
} /* addGrpMem */

void CPCDraftDoc::DxfCalcArcPoints ( LongPointPtr  centerPt,
                                     LongPoint            rad,
                                     Angle             startAng,
                                     Angle             endAng,
                                     LongPointPtr         p1,
                                     LongPointPtr         p2,
                                     LongPointPtr         p3)
{
   Angle                midAngle;

   midAngle = mtFixAngle ((mtFixAngle(endAng - startAng) >>1) + startAng);

   p1->x = centerPt->x + mtFixMulDbl (rad.x, mtCos (startAng) );
   p1->y = centerPt->y + mtFixMulDbl (rad.y, mtSin (startAng) );

   p2->x = centerPt->x + mtFixMulDbl (rad.x, mtCos (endAng) );
   p2->y = centerPt->y + mtFixMulDbl (rad.y, mtSin (endAng) );

   p3->x = centerPt->x + mtFixMulDbl (rad.x, mtCos (midAngle) );
   p3->y = centerPt->y + mtFixMulDbl (rad.y, mtSin (midAngle) );
} /* calcArcPoints */

void CPCDraftDoc::DxfSetShPenAndFill (bool pFill,  DxfParamStruct& sParam)
{
   PenModel    aPenModel;
   FillModel      aFillModel;
   short       vIdxColor, vIdxDash;

   if ( sParam.sEntityColorIdx != kNilOff) //  && !sParam.sIgnoreLayer )
      vIdxColor = sParam.sEntityColorIdx;
   else
      vIdxColor = sParam.sDefColorIdx;

   if ( sParam.sEntityDashIdx != kNilOff )
      vIdxDash = sParam.sEntityDashIdx;
   else
      vIdxDash = sParam.sDefaultDashIdx;

   if ( pFill )
   {
      aFillModel.m_nFillType = kFillColor;
      aFillModel.m_nIndex = PALETTEINDEX(0);
      sParam.sShHdl->SetFillModel(aFillModel);
   }

   aPenModel.m_nIndex = PALETTEINDEX(vIdxColor);
   aPenModel.m_nPenType = kPenColor;
   if ( sParam.sEntityDashIdx != kNilOff )
   {
      aPenModel.m_nStyle   = vIdxDash;
      //aPenModel.m_nPenType = ;
   }

   sParam.sShHdl->SetPenModel(aPenModel);

   sParam.sEntityColorIdx = kNilOff;
   sParam.sEntityDashIdx = kNilOff;
} /* setShPenAndFill */

//#define DEBUG_MD60
#undef DEBUG_MD60

BOOL CPCDraftDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
#ifdef _DEMO
   return TRUE;
#else
   if(((CPCDraftApp*)AfxGetApp())->m_bDemoMode) return TRUE;
   UpdateAllViews(NULL,kDeactivateTextShape);

   bool bOldFileExtension = false;

   if(lpszPathName)
   {

      const char * pszDot = lpszPathName == NULL ? NULL : strrchr(lpszPathName,'.');

      if(pszDot != NULL) // format .p55 - Infinisys Ltd
      {

         CString strExtension = pszDot + 1;

         if(strExtension.CompareNoCase("mdd") == 0 || strExtension.CompareNoCase("drf") == 0)
         {

            bOldFileExtension = true;

         }

      }

   }

   if(lpszPathName == NULL || !m_bSaveAsMacDraft || bOldFileExtension)
   {
      CString sTitle=GetTitle();
      int nExt=sTitle.ReverseFind('.');
      if((m_bSaveAsStationery || !m_bSaveAsMacDraft) && nExt>=0)
      {
         sTitle=sTitle.Left(nExt);
         nExt=-1;
      }
      //if(nExt<0) sTitle+=".mdd"; //m_bSaveAsMacDraft?".mdd":".drf";
      // Infinisys Ltd
      //if(nExt<0) sTitle+=".p55"; //m_bSaveAsMacDraft?".mdd":".drf";

#ifdef DEBUG_MD60

      int iFind = sTitle.ReverseFind('.');

      if(iFind < 0) // format .md60 - Infinisys Ltd
      {
         sTitle += ".md60";
      }
      else
      {
         sTitle = sTitle.Left(iFind) + ".md60";
      }

#elif MD70_DEFAULT

      int iFind = sTitle.ReverseFind('.');

      if (iFind < 0) // format .p55 - Infinisys Ltd
      {
         sTitle += ".md70";
      }
      else
      {
         sTitle = sTitle.Left(iFind) + ".md70";
      }

#else

      int iFind = sTitle.ReverseFind('.');

      if(iFind < 0) // format .p55 - Infinisys Ltd
      {
         sTitle += ".p55";
      }
      else
      {
         sTitle = sTitle.Left(iFind) + ".p55";
      }

#endif

      CStringW strFilter;
      strFilter.LoadString(
#ifndef _PERSONAL_EDITION
      IDS_SAVEFILTERS
#else  //PERSONAL_EDITION
      IDS_SAVEFILTERSPE
#endif
      );
      CFileDialogExW theDlg(FALSE,NULL,get_uni(sTitle, m_bUtf8Name ? CP_UTF8 : CP_ACP),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter);

#ifdef DEBUG_MD60
      // override the default extension for DEBUGGING
      theDlg.m_ofn.lpstrDefExt = _T("md60"); // .md60 format - Infinisys Ltd
      theDlg.m_ofn.nFilterIndex = 2;
#endif

      //OPENFILENAME& ofn = theDlg.GetOFN();
      //ofn.lpstrFileTitle = sTitle.GetBuffer(sTitle.GetLength() + 1);
//    theDlg.m_ofn.nFilterIndex=m_bSaveAsMacDraft?3:1;
      if(theDlg.DoModal()==IDOK)
      {
         CStringW thePathName=theDlg.GetPathName();
         m_bUtf8Name = true;
         int iFilterIndex = theDlg.m_ofn.nFilterIndex;
         switch(iFilterIndex)
         {
#ifndef _PERSONAL_EDITION
         case 8: //6 // format .p55 - Infinisys Ltd // 7 // format .md60 - Infinisys Ltd
            if(thePathName.Find('.')<0) thePathName+=".bmp";
            if(SaveBitmap(get_utf8(thePathName)) )return TRUE;
            break;
         case 7: //5 // format .p55 - Infinisys Ltd // 6 // format .md60 - Infinisys Ltd
            if(thePathName.Find('.')<0) thePathName+=".dwg";
            if(ExportDWG(get_utf8(thePathName))) return TRUE; // PCDraft7 XXX
            break;
         case 6: //4 // format .p55 - Infinisys Ltd // 5 // format .md60 - Infinisys Ltd
            if(thePathName.Find('.')<0) thePathName+=".dxf";
            if(ExportDXF(get_utf8(thePathName))) return TRUE;
            break;
#else
         case 6: //4 // format .p55 - Infinisys Ltd // 5 // format .md60 - Infinisys Ltd
            if(thePathName.Find('.')<0) thePathName+=".bmp";
            if(SaveBitmap(thePathName)) return TRUE;
            break;
#endif
         case 3: // md60
            if(thePathName.Find('.')<0) thePathName+=".md60";
            {
               BOOL bResult=SaveMacDraft60(get_utf8(thePathName));
               if(bResult)
               {
                  SetModifiedFlag(FALSE);
                  SetPathName(get_utf8(thePathName));
                  m_bSaveAsMacDraft=true;
                  return TRUE;
               }
            }
            break;
         case 1: // md70
            if(thePathName.Find('.')<0) thePathName+=".md70";
            {
               BOOL bResult=SaveMacDraft70(get_utf8(thePathName));
               if(bResult)
               {
                  SetModifiedFlag(FALSE);
                  SetPathName(get_utf8(thePathName));
                  m_bSaveAsMacDraft=true;
                  return TRUE;
               }
            }
            break;
         case 2: // p55
            if(thePathName.Find('.')<0) thePathName+=".md70";
            {
               BOOL bResult=SavePCDraft55(get_utf8(thePathName));
               if(bResult)
               {
                  SetModifiedFlag(FALSE);
                  SetPathName(get_utf8(thePathName));
                  m_bSaveAsMacDraft=true;
                  return TRUE;
               }
            }
            break;
         case 4:
            if(thePathName.Find('.')<0) thePathName+=".mdd";
            {
               BOOL bResult=SaveMacDraft(get_utf8(thePathName));
               if(bResult)
               {
                  //SetModifiedFlag(FALSE);
                  //SetPathName(thePathName);
                  m_bSaveAsMacDraft=true;
                  return TRUE;
               }
            }
            break;
         case 5:
            if(thePathName.Find('.')<0) thePathName+=".drs";
            for(int i=0; i<m_Layers.GetSize(); i++)
            {
               CLayer* pLayer=m_Layers[i];
               pLayer->MakeSelfContainedShapes(this);
            }
            if(CDocument::DoSave(get_utf8(thePathName),TRUE))
            {
//             m_bSaveAsMacDraft=false;
               m_bSaveAsStationery=true;
               return TRUE;
            }
            break;
            /*       default: // no drf file format
                        if(thePathName.Find('.')<0) thePathName+=".drf";
                        {
                           BOOL bResult=CDocument::DoSave(thePathName,TRUE);
                           if(bResult)
                           {
                              m_bSaveAsMacDraft=false;
                              return TRUE;
                           }
                        }
                        break;
            */
         }

         // be sure to delete the file
         TRY
         {
            CFile::Remove(get_utf8(thePathName));
         }
         CATCH_ALL(e)
         {
            TRACE0("Warning: failed to delete file after failed SaveAs.\n");
//          e->Delete();
         }
         END_CATCH_ALL
      }
      return FALSE;
   }
   if(m_bSaveAsMacDraft && !m_bSaveAsStationery)
   {
      // p55 file format - Infinisys Ltd.
      //BOOL bResult=SaveMacDraft(lpszPathName);
      CString str(lpszPathName);
      BOOL bResult;
      // Infinisys Ltd. - Bugreport #5
      if(str.Right(5).CompareNoCase(".md60") == 0)
      {

         bResult = SaveMacDraft60(lpszPathName);

      }
      else if(str.Right(5).CompareNoCase(".md70") == 0)
      {

         bResult = SaveMacDraft70(lpszPathName);

      }
      else if(str.Right(4).CompareNoCase(".mdd") == 0)
      {

         bResult = SaveMacDraft(lpszPathName);

      }
      else
      {

         bResult = SavePCDraft55(lpszPathName);

      }
      
      if(bResult) SetModifiedFlag(FALSE);
      return bResult;
   }
   return CDocument::DoSave(lpszPathName,TRUE);
#endif
}

void CPCDraftDoc::OnUpdateFileRevert(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!GetPathName().IsEmpty() && IsModified());
}

void CPCDraftDoc::OnFileRevert()
{
   CString text;
   CFileStatus st;
   if(!CFile::GetStatus(GetPathName(),st))
   {
      text.Format(IDS_FILE_NOT_FOUND,(LPCTSTR)GetTitle());
      AfxMessageBox(text);
      return;
   }
   text.Format(IDS_REVERT,(LPCTSTR)GetTitle());
   if(AfxMessageBox(text+st.m_mtime.Format(IDS_LAST_SAVED),MB_OKCANCEL)==IDOK)
   {
      if(!CFile::GetStatus(GetPathName(),st))
      {
         text.Format(IDS_FILE_NOT_FOUND,(LPCTSTR)GetTitle());
         AfxMessageBox(text);
         return;
      }

      UpdateAllViews(NULL,kClearSelection);
      UpdateAllViews(NULL,kSaveRef);
      DeleteContents();
      if(m_bSaveAsMacDraft)
      {
         LoadMacDraft(GetPathName());
         SetModifiedFlag(FALSE);
      }
      else OnOpenDocument(GetPathName());
      UpdateAllViews(NULL,kResolveRef);
      SendInitialUpdate();
   }
}

// Guides PCDraft 7 Infinisys Ltd.
bool CPCDraftDoc::XInDocument(const Coord & coord)
{
   CSize docSize(m_NumPage.x*m_PageSize.cx, m_NumPage.y*m_PageSize.cy);
   return (coord >= 0 && coord.GetLong() <= docSize.cx);
}

CSize CPCDraftDoc::GetDocSize()
{
   CSize docSize(m_NumPage.x*m_PageSize.cx, m_NumPage.y*m_PageSize.cy);
   return docSize;
}

// Guides PCDraft 7 Infinisys Ltd.
bool CPCDraftDoc::YInDocument(const Coord & coord)
{
   CSize docSize(m_NumPage.x*m_PageSize.cx, m_NumPage.y*m_PageSize.cy);
   return (coord >= 0 && coord.GetLong() <= docSize.cy);
}


void CPCDraftDoc::FillModelBitmap(int i,CDC & dc, LPCRECT lpcrect, int iOpacity) // Transparency/Gradient - Infinisys Ltd
{

   if(i < 0 || i >= m_packeditema.GetCount())
   {
      return;
   }
   if(m_packeditema[i].m_etype == packed_type_dib)
   {
      CBitmap pat;
      pat.Attach(GetPatternBitmap(i, iOpacity));
      CBrush brush;
      brush.CreatePatternBrush(&pat);
      dc.FillRect(lpcrect,&brush);

   }
   else if(m_packeditema[i].m_etype == packed_type_gradient)
   {

      Gdiplus::GraphicsPath path;
      Gdiplus::RectF r1;
      __copy(r1, lpcrect);
      path.AddRectangle(r1);
      Gdiplus::Graphics g(dc.GetSafeHdc());

      ::std::unique_ptr < Gdiplus::Brush > brush(m_packeditema[i].m_pgradient->proper_brush(lpcrect, iOpacity));
      g.FillPath(brush.get(), &path);

   }

}

CString CPCDraftDoc::CreateTitle()
{
   CString theTitle,strScale;

   if(GetActiveLayer() != NULL)
   {
      if(GetActiveLayer()->m_nScale == -1)
      {

         strScale.Format("%d : %d",m_ratioCustom.fN,m_ratioCustom.fD);

      }
      else
      {

         VERIFY(strScale.LoadString((m_bEnglishUnits?IDS_ENGLISH_SCALE_FIRST:IDS_METRIC_SCALE_FIRST) + GetActiveLayer()->m_nScale - 1));

      }
#ifndef _PERSONAL_EDITION

      CString strName = GetActiveLayer()->m_strName;
      
      CStringW wstrName = get_uni(strName);

      strName = get_cp(wstrName,CP_OEMCP);

      theTitle.Format("%s - %s - %s",m_strTitle,strName,strScale);
#else
      theTitle.Format("%s - %s",m_strTitle,strScale);
#endif

   }

   return theTitle;
}

CStringW CPCDraftDoc::CreateTitleW()
{
   CStringW theTitle, strScale;

   if (GetActiveLayer() != NULL)
   {
      if (GetActiveLayer()->m_nScale == -1)
      {

         strScale.Format(L"%d : %d", m_ratioCustom.fN, m_ratioCustom.fD);

      }
      else
      {

         VERIFY(strScale.LoadString((m_bEnglishUnits ? IDS_ENGLISH_SCALE_FIRST : IDS_METRIC_SCALE_FIRST) + GetActiveLayer()->m_nScale - 1));

      }
#ifndef _PERSONAL_EDITION

      CString strName = GetActiveLayer()->m_strName;

      CStringW wstrName = get_uni(strName);

      theTitle.Format(L"%s - %s - %s", (const WCHAR *) get_uni(m_strTitle), (const WCHAR *)get_uni(strName), strScale);
#else
      theTitle.Format(L"%s - %s", (const WCHAR *)get_uni(m_strTitle), strScale);
#endif

   }

   return theTitle;
}

double CPCDraftDoc::GetFitFactor(const CSize &szLimit)
{
   LongRect ext=GetCumulativeExtents();
   double xFactor,yFactor;

   xFactor=(double)szLimit.cx/(double)(TruncToShort(ext.right+FixedHalf)+2);
   yFactor=(double)szLimit.cy/(double)(TruncToShort(ext.bottom+FixedHalf)+2);

   return xFactor<yFactor?xFactor:yFactor;
}



Ratio* CPCDraftDoc::GetScaleTableInfo(int nUnits,int& nScales)
{
   if(nUnits & kUEnglishMask)
   {
      nScales=EnglishScales->fD;
      return EnglishScales;
   }
   else
   {
      nScales=MetricScales->fD;
      return MetricScales;
   }
}

bool CPCDraftDoc::MatchScale(int& nScaleIndex,int nNewUnits,int nOldUnits)
{
   Ratio ratio= {1,1};
   if(nScaleIndex>0) ratio=(nOldUnits & kUEnglishMask)?EnglishScales[nScaleIndex]:MetricScales[nScaleIndex];

   int nScales;
   Ratio* pScales=GetScaleTableInfo(nNewUnits,nScales);
   nScaleIndex=(nNewUnits & kUEnglishMask)?4:6;// default
   for(int i=1; i<=nScales; i++)
   {
      if(ratio.fN==pScales[i].fN && ratio.fD==pScales[i].fD)
      {
         nScaleIndex=i;
         return true;
      }
   }
   return false;
}


bool CPCDraftDoc::FindScale(int & nScaleIndex,int nUnits, LongRatio & longRatio) // Free Scale - Infinisys Ltd
{
   int nScales;
   Ratio* pScales=GetScaleTableInfo(nUnits,nScales);
   nScaleIndex=(nUnits & kUEnglishMask)?4:6;// default
   for(int i=1; i <= nScales; i++)
   {
      if(longRatio.fN == pScales[i].fN && longRatio.fD == pScales[i].fD)
      {
         nScaleIndex=i;
         return true;
      }
   }
   return false;
}


int CPCDraftDoc::GetNearestScale(LongRatio longRatio) // Free Scale - Infinisys Ltd
{

   int scale = -1;

   double dRatio;

   if(longRatio.fD == 0)
   {

      dRatio = 1.0;

   }
   else
   {

      dRatio = (double)longRatio.fN / (double)longRatio.fD;

   }

   Ratio* ratio;
   if(m_bEnglishUnits)
   {
      ratio=EnglishScales;
   }
   else
   {
      ratio=MetricScales;
   }
   int iCount = ratio->fD;
   for(int i = 1; i <= iCount; i++)
   {
      double dRatioPredefined = (double)ratio[i].fN / (double)ratio[i].fD;
      if(dRatio >= dRatioPredefined)
      {
         scale = i;
         break;
      }
   }
   return scale;
}

int CPCDraftDoc::GetNearestScale() // Free Scale - Infinisys Ltd
{

   return GetNearestScale(m_ratioCustom);

}


void CPCDraftDoc::OnChangedViewList()
{
   POSITION pos=GetFirstViewPosition();
   while(pos!=NULL)
   {
      CView* pView=GetNextView(pos);
      if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
      {
         CDocument::OnChangedViewList();
         return;
      }
   }
   OnCloseDocument();
}

BOOL CPCDraftDoc::CanCloseFrame(CFrameWnd* pFrame)
{
   if(pFrame->IsKindOf(RUNTIME_CLASS(CChildFrame)))
      return SaveModified();
   return CDocument::CanCloseFrame(pFrame);
}

void CPCDraftDoc::UpdateMaxZoom()
{
   int nMax=Max(m_NumPage.x*m_PageSize.cx,m_NumPage.x*m_PageSize.cx);
   if (nMax < 512) m_nMaxZoom = 6; // PC Draft 7 - Zoom 64
   else if(nMax<1024) m_nMaxZoom=5;
   else if(nMax<2048) m_nMaxZoom=4;
   else if(nMax<4096) m_nMaxZoom=3;
   else if(nMax<8192) m_nMaxZoom=2;
   else if(nMax<16384) m_nMaxZoom=1;
   else m_nMaxZoom=0;
   if(m_nZoomFactor>m_nMaxZoom)
   {
      m_nZoomFactor=m_nMaxZoom;
      m_nPrevZoomFactor=m_nZoomFactor;
      UpdateAllViews(NULL,kZoomFactorChanged);
   }
}

bool CPCDraftDoc::CheckDocSize(bool bExpand)
{
   LongRect lrcExt=GetCumulativeExtents();

   if((lrcExt.left<0 || lrcExt.top<0) && !bExpand) return false;
   bool bExpanded=false;
   if(lrcExt.right>(m_NumPage.x*m_PageSize.cx))
   {
      if(!bExpand) return false;
      int nNewPages=Min(kMaxDocH,lrcExt.right.GetLong())/m_PageSize.cx+1;
      if(nNewPages!=m_NumPage.x)
      {
         m_NumPage.x=nNewPages;
         bExpanded=true;
      }
   }
   if(lrcExt.bottom>(m_NumPage.y*m_PageSize.cy))
   {
      if(!bExpand) return false;
      int nNewPages=Min(kMaxDocV,lrcExt.bottom.GetLong())/m_PageSize.cy+1;
      if(nNewPages!=m_NumPage.y)
      {
         m_NumPage.y=nNewPages;
         bExpanded=true;
      }
   }
   if(bExpanded)
   {
      UpdateMaxZoom();
      UpdateAllViews(NULL,kDocumentSizeChanged);
      SetModifiedFlag();
   }
   return true;
}

bool CPCDraftDoc::ExpandDoc(const LongRect& lrcExt , bool bUpdate)
{
   bool bExpanded=false;
   if(lrcExt.right>(m_NumPage.x*m_PageSize.cx))
   {
      int nNewPages=Min(kMaxDocH,lrcExt.right.GetLong())/m_PageSize.cx+1;
      if(nNewPages!=m_NumPage.x)
      {
         m_NumPage.x=nNewPages;
         bExpanded=true;
      }
   }
   if(lrcExt.bottom>(m_NumPage.y*m_PageSize.cy))
   {
      int nNewPages=Min(kMaxDocV,lrcExt.bottom.GetLong())/m_PageSize.cy+1;
      if(nNewPages!=m_NumPage.y)
      {
         m_NumPage.y=nNewPages;
         bExpanded=true;
      }
   }
   if(bExpanded && bUpdate)
   {
      UpdateMaxZoom();
      UpdateAllViews(NULL,kDocumentSizeChanged);
      SetModifiedFlag();
   }
   return bExpanded;
}

bool CPCDraftDoc::IsLegalDocSize(const LongPoint& lpt) const
{
   return ((m_PageSize.cx*(lpt.x.GetLong()/m_PageSize.cx+1))<=kMaxCoordinate
           && (m_PageSize.cy*(lpt.y.GetLong()/m_PageSize.cy+1))<=kMaxCoordinate);
}


COLORREF CPCDraftDoc::translate_color(COLORREF crColor)
{

   if (crColor & PALETTEINDEX(0))
   {

      PALETTEENTRY pe;

      m_Palette.GetPaletteEntries(crColor & 0xFF, 1, &pe);

      crColor = RGB(pe.peRed, pe.peGreen, pe.peBlue);

   }

   return crColor;

}


int CPCDraftDoc::GetFirstLayerIndex()
{
   for(int i=0; i<m_Layers.GetSize(); i++)
      if(!GetLayer(i)->m_bHidden)
         return i;
   if(m_bActiveLayerHidden) return m_nActiveLayer;
   ASSERT(FALSE);
   return -1;
}

int CPCDraftDoc::GetLastLayerIndex()
{
   for(int i=m_Layers.GetUpperBound(); i>=0; i--)
      if(!GetLayer(i)->m_bHidden)
         return i;
   if(m_bActiveLayerHidden) return m_nActiveLayer;
   ASSERT(FALSE);
   return -1;
}

void CPCDraftDoc::ActivateNextLayer()
{
   int nLast=GetLastLayerIndex();
   for(int i=m_nActiveLayer+1; i<=nLast; i++)
      if(!GetLayer(i)->m_bHidden)
      {
         SetActiveLayer(i);
         return;
      }
   ASSERT(FALSE);
}

void CPCDraftDoc::ActivatePrevLayer()
{
   int nFirst=GetFirstLayerIndex();
   for(int i=m_nActiveLayer-1; nFirst<=i; i--)
      if(!GetLayer(i)->m_bHidden)
      {
         SetActiveLayer(i);
         return;
      }
   ASSERT(FALSE);
}

int CPCDraftDoc::GetVisibleLayerCount()
{
   int nCount=0;
   for(int i=0; i<m_Layers.GetSize(); i++)
      if(!GetLayer(i)->m_bHidden)
         nCount++;
   return nCount;
}

bool CPCDraftDoc::SaveBitmap(const char *szPath)
{
   CFileException fe;
   CFile* fo=NULL;
   fo=GetFile(szPath,CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);
   if(fo == NULL)
   {
      ReportSaveLoadException(szPath, &fe, TRUE, AFX_IDP_INVALID_FILENAME);
      return false;
   }
   TRY
   {
      CWaitCursor wait;

      LongRect lrcExtents=GetCumulativeExtents(false);
      lrcExtents.Inset(-3,-3);
      CRect rcExtents=LPtoDP(lrcExtents);
      rcExtents.right++;
      rcExtents.bottom++;
      CWindowDC theWindowDC(AfxGetMainWnd());
      CBitmap theBmp;

      BITMAPINFOHEADER bmih;
      bmih.biSize=sizeof(BITMAPINFOHEADER);
      bmih.biWidth=Width(rcExtents);
      bmih.biHeight=Height(rcExtents);
      bmih.biPlanes=1;
      bmih.biBitCount=24;
      bmih.biCompression=BI_RGB;
      bmih.biSizeImage=0;
      bmih.biXPelsPerMeter=0;
      bmih.biYPelsPerMeter=0;
      bmih.biClrUsed=0;
      bmih.biClrImportant=0;

      LPVOID lpBits;
      theBmp.Attach(CreateDIBSection(theWindowDC.GetSafeHdc(),(LPBITMAPINFO)&bmih,DIB_RGB_COLORS,&lpBits,NULL,0));

      {
         CGDIBitmapDC theDC(&theWindowDC,&theBmp);
         theDC.SetViewportOrg(-rcExtents.left,-rcExtents.top);

         CRgn theClipRgn;
         theClipRgn.CreateRectRgnIndirect(&rcExtents);
         theClipRgn.OffsetRgn(-rcExtents.left,-rcExtents.top);
         theDC.SelectClipRgn(&theClipRgn);

         theDC.FillRect(&rcExtents,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
         CDocMapper mapper(this);
         CDraftContext dc(&theDC,&mapper,this);
         CPalette* pPalette=theDC.SelectPalette(&m_Palette,FALSE);
         theDC.RealizePalette();
         for(int i=0; i<m_Layers.GetSize(); i++) GetLayer(i)->Draw(&dc,true,m_bHideText);
         theDC.SelectPalette(pPalette,FALSE);
      }

      DIBSECTION ds;
      GetObject(theBmp.GetSafeHandle(),sizeof(DIBSECTION),&ds);

      BITMAPFILEHEADER bmfh;
      bmfh.bfType=0x4d42;
      bmfh.bfSize=sizeof(BITMAPFILEHEADER)+ds.dsBmih.biSize+ds.dsBmih.biSizeImage;
      bmfh.bfReserved1=0;
      bmfh.bfReserved2=0;
      bmfh.bfOffBits=sizeof(BITMAPFILEHEADER)+ds.dsBmih.biSize;

      fo->Write(&bmfh,sizeof(BITMAPFILEHEADER));
      fo->Write(&ds.dsBmih,ds.dsBmih.biSize);
      fo->Write(ds.dsBm.bmBits,ds.dsBmih.biSizeImage);
      ReleaseFile(fo, FALSE);
   }
   CATCH_ALL(e)
   {
      ReleaseFile(fo, TRUE);
      TRY
      {
         ReportSaveLoadException(szPath, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
      }
      END_TRY
//    e->Delete();
      return false;
   }
   END_CATCH_ALL

   return true;
}

double CPCDraftDoc::CalcResChangeFactor(int nNewDisplayRes)
{
   if(nNewDisplayRes!=m_nDisplayRes)
   {
      double   n,d;
      switch(m_nDisplayRes)
      {
      case kEngRes:     d = kEngFactor;      break;
      case kEngDecRes:  d = kEngDecFactor;   break;
      case kMetRes:     d = kMetFactor;      break;
      default:       d = 1.0;
      }
      switch(nNewDisplayRes)
      {
      case kEngRes:     n = kEngFactor;      break;
      case kEngDecRes:  n = kEngDecFactor;   break;
      case kMetRes:     n = kMetFactor;      break;
      default:       n = 1.0;
      }
      return n / d;
   }
   return 0;
}

void CPCDraftDoc::OnUpdateDataFindReplace(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void CPCDraftDoc::OnUpdateLayoutLayerSetup(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void CPCDraftDoc::RemoveAllSavedViews()
{
   for(int i=0; i<m_SavedViews.GetSize(); i++)
      delete m_SavedViews.GetAt(i);
   m_SavedViews.RemoveAll();
}

BOOL CPCDraftDoc::SaveModified()
{
#ifdef _DEMO
   return TRUE;
#else
   if(((CPCDraftApp*)AfxGetApp())->m_bDemoMode) return TRUE;
   UpdateAllViews(NULL,kDeactivateTextShape);
   return CDocument::SaveModified();
#endif
}

int CPCDraftDoc::GetObjectCount(bool bOverall)
{
   return GetActiveLayer()->GetObjectCount(bOverall);
}

float CPCDraftDoc::fNoZoomToZoom(float penf)
{
   return   (float)ldexp(1.0f, m_nZoomFactor)*penf;
}

void CPCDraftDoc::GetAngleSnaps(CStringArray& strList)
{
   m_snapTable.GetAngleSnaps(strList);
}

double round(double val)
{
   return floor(val+0.5);
}


void CPCDraftDoc::SnapToAngle(LongPoint &ptTo, LongPoint& ptFrom, bool bShift)
{
   long snapsToACircle = 0;

   // The modifier key usage may seem strange, but a lot of thought and experimentation has gone into it.
   // Spend at least a couple of hours experimenting before you decide to change it permanently.
   // I expect you will decide to cancel your checkout. RPH
   // N.B. the command key gets used in the grid snapping code to turn grid snapping off, so
   // holding down command and shift is a very useful combination to turn both snappings off.


   if ( m_snapTable.getAngleSnaps() )
   {
      // Angle snapping
      if (bShift)
         snapsToACircle = 0 ; // Shift key toggles angle snapping off
      else
         snapsToACircle = m_snapTable.getAngleSnaps();
   }
   else
   {
      // No snapping of any kind
      if (bShift)
         snapsToACircle = 8 ; // Shift key gives 45 degree snapping
      else
         snapsToACircle = 0 ;
   }


   if ( snapsToACircle )
   {
      double snap = mtFixToReal ( mtAngle ( ptFrom, ptTo ) ) ;
      snap = round( snap * snapsToACircle / 360.0 ) ; // Find nearest snap
      Angle longAngle = WrFixed ( snap * 360.0 / snapsToACircle ) ;
      if ( ( longAngle % k45 ) == FixedZero )
      {
         if ( ( longAngle % k180 ) == FixedZero )
            ptTo.y= ptFrom.y;
         else if ( ( longAngle % k90 ) == FixedZero )
            ptTo.x= ptFrom.x;
         guGetHV45LPt (&ptTo, &ptFrom);
      }
      else
      {
         long double m =  mtSin (longAngle) / mtCos (longAngle);
         ptTo.y.SetFromRawDoubleTruncated ( m * ptTo.x.GetRawDouble () +
                                            ptFrom.y.GetRawDouble () - m * ptFrom.x.GetRawDouble () ) ;
      }
   }

}

//void CPCDraftDoc::ExportQT()
//{
////#ifndef _DEMO
////   LongRect lrcExtents=GetCumulativeExtents(false);
////   lrcExtents.Inset(-3,-3);
////
////   CMapper mapper;
////   CRect rcExtents=mapper.LPtoDP(lrcExtents);
////   rcExtents.right++;
////   rcExtents.bottom++;
////
////   using namespace QTML;
////
////   Rect rcBound;
////   MacSetRect(&rcBound,0,0, Width(rcExtents), Height(rcExtents));
////   CMacGWorld gWorld(&rcBound);
////   gWorld.BeginDrawing();
////   {
////      CDC* pDC=gWorld.GetDC();
////      pDC->SetViewportOrg(-rcExtents.left,-rcExtents.top);
////
////      CRgn theClipRgn;
////      theClipRgn.CreateRectRgnIndirect(&rcExtents);
////      theClipRgn.OffsetRgn(-rcExtents.left,-rcExtents.top);
////      pDC->SelectClipRgn(&theClipRgn);
////
////      pDC->FillRect(&rcExtents,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
////      CDraftContext dc(pDC,&mapper,this);
////      CPalette* pPalette=pDC->SelectPalette(&m_Palette,FALSE);
////      pDC->RealizePalette();
////      for(int i=0; i<m_Layers.GetSize(); i++) GetLayer(i)->Draw(&dc,true,m_bHideText);
////      pDC->SelectPalette(pPalette,FALSE);
////   }
////   gWorld.EndDrawing();
////
////   GraphicsImportComponent gi;
////   ComponentResult   err;
////   Handle handle=NewHandleClear(512);
////   err=MemError();
////   if(!err)
////   {
////      PicHandle hPict=gWorld.GetPicture();
////      err=HandAndHand((Handle)hPict, handle);
////      KillPicture(hPict);
////      if(!err)
////      {
////         err=OpenADefaultComponent(GraphicsImporterComponentType, kQTFileTypePicture, &gi);
////         if(!err)
////         {
////            err=GraphicsImportSetDataHandle(gi, handle);
////            if(!err)
////            {
////               CString strName=GetTitle();
////               FSSpec aFileSpec;
////               NativePathNameToFSSpec(strName.LockBuffer(),&aFileSpec,0);
////               strName.UnlockBuffer();
////               CreatePortAssociation(AfxGetMainWnd()->m_hWnd, NULL, 0);
////               err=GraphicsImportDoExportImageFileDialog(gi, &aFileSpec, NULL, NULL, NULL, NULL, NULL);
////               DestroyPortAssociation((CGrafPtr)GetHWNDPort(AfxGetMainWnd()->m_hWnd));
////            }
////            CloseComponent(gi);
////         }
////      }
////   }
////   if(err && err!=userCanceledErr) AfxMessageBox("Error exporting image");
////#endif
//}

//void CPCDraftDoc::ExportImage()
//{
//#ifndef _DEMO
//   CString strPath;
//
//   CString strFilter;
//   
//   strFilter = "PNG file (*.png)|*.png|JPEG file (*.jpg)|*.jpg||";
//
//   CString strTitle;
//
//   strTitle = m_strTitle;
//
//   repeat:
//
//   CFileDialog filedlg(FALSE,".png",strTitle + ".png",OFN_OVERWRITEPROMPT,strFilter,AfxGetApp()->m_pMainWnd);
//   TCHAR pathPicture[MAX_PATH];
//   memset(pathPicture,0,sizeof(pathPicture));
//   HRESULT hr = SHGetFolderPath(NULL,CSIDL_MYPICTURES,NULL,SHGFP_TYPE_CURRENT,pathPicture);
//
//   CString sInitialDirectory = AfxGetApp()->GetProfileString("Settings","ImageExportFolder",pathPicture);
//   // Set sInitialDirectory to whatever you want
//
//
//   filedlg.m_ofn.lpstrInitialDir = sInitialDirectory;
//
//   if(filedlg.DoModal() != IDOK)
//   {
//
//      return;
//
//   }
//
//   strPath = filedlg.GetPathName();
//
//   sInitialDirectory = get_folder_from_path(strPath);
//
//   AfxGetApp()->WriteProfileString("Settings","ImageExportFolder",sInitialDirectory);
//
//   LongRect lrcExtents=GetCumulativeExtents(false);
//   lrcExtents.Inset(-3,-3);
//
//   CMapper mapper;
//   CRect rcExtents=mapper.LPtoDP(lrcExtents);
//   rcExtents.right++;
//   rcExtents.bottom++;
//
//   CRect rcBound(0,0,Width(rcExtents),Height(rcExtents));
//
//   CBitmap bitmap;
//
//   bitmap.Attach(CreateDIB(rcBound.Width(),rcBound.Height(),NULL,0));
//
//   int iMargin = 5;
//
//   {
//
//      CDC dcMem;
//
//      dcMem.CreateCompatibleDC(NULL);
//
//      CBitmap * pOld = dcMem.SelectObject(&bitmap);
//
//      dcMem.SetMapMode(MM_TEXT);
//
//      dcMem.OffsetViewportOrg(iMargin / 2,iMargin / 2);
//
//      CDC* pDC=&dcMem;
//      pDC->SetViewportOrg(-rcExtents.left,-rcExtents.top);
//
//      CRgn theClipRgn;
//      theClipRgn.CreateRectRgnIndirect(&rcExtents);
//      theClipRgn.OffsetRgn(-rcExtents.left,-rcExtents.top);
//      pDC->SelectClipRgn(&theClipRgn);
//      pDC->FillRect(&rcExtents,CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
//      CDraftContext dc(pDC,&mapper,this);
//      CPalette* pPalette=pDC->SelectPalette(&m_Palette,FALSE);
//      pDC->RealizePalette();
//      for(int i=0; i < m_Layers.GetSize(); i++) GetLayer(i)->Draw(&dc,true,m_bHideText);
//      pDC->SelectPalette(pPalette,FALSE);
//
//      GdiFlush();
//
//
//   }
//
//
//   Gdiplus::Bitmap b((HBITMAP)bitmap.GetSafeHandle(),NULL);
//
//
//   if(strPath.Right(4) == ".png")
//   {
//
//      SavePNG(strPath,&b);
//
//   }
//   else if(strPath.Right(5) == ".jpeg" || strPath.Right(4) == ".jpg")
//   {
//
//      SaveJPEG(strPath,&b);
//
//   }
//   else
//   {
//
//      AfxMessageBox("Missing file extension. Unknown file type. Please choose either PNG or JPEG.");
//
//      goto repeat;
//
//   }
//
//#endif
//}

CSnapTable::CSnapTable():
   m_nAngleSnapsToACircle(0),
   m_nSelectedSnap(-1),m_nAngleUnit(0)
{
   buildSnapTable();
}

void CSnapTable::GetAngleSnaps(CStringArray &strList)
{
   for(int i=0; i<m_strList.GetSize(); i++)
   {
      strList.Add(m_strList.GetAt(i));
   }
}

void CSnapTable::selectSnap(int index)
{
   if(m_nSelectedSnap!=index)
   {
      m_nSelectedSnap=index;

      if (m_nSelectedSnap<0)
      {
         m_nAngleSnapsToACircle = 0 ;
      }
      else
      {
         CString s=m_strList.GetAt(m_nSelectedSnap);
         double d;
         if ( (s.GetLength() > 2) &&
               (s.GetAt(0) == '1') &&
               (s.GetAt(1) == '/')
            )
         {
            s=s.Mid(2);
            d = 1.0 / atof( s ) ;
         }
         else
         {
            d = atof( s ) ;
         }

         m_nAngleSnapsToACircle = (short)round( calcDivisions()/d );
      }
   }
}

void CSnapTable::buildSnapTable()
{
   m_strList.RemoveAll();
   long divisions = calcDivisions();

   long numer = 0 ;
   long denom = 0 ;


   if(m_nAngleSnapsToACircle)
   {
      numer = divisions / m_nAngleSnapsToACircle ;
      if ( numer == 0 )
         denom = m_nAngleSnapsToACircle / divisions ;
   }

   long maxSnap = divisions ;
   int i = 8;
   for ( ; i > 1 ; --i )
   {
      if ( ( maxSnap % i ) == 0 )
      {
         maxSnap /= i ;
         break ;
      }
   }

   // First check for binary

   i = divisions ;
   while ( ( i & 1 ) == 0 )
   {
      i >>= 1 ;
   }

   bool recip = false ;
   bool check = false ;

   CString theString;
   if ( i == 1 )
   {
      // Binary
      for (i = 0; i < 9; i++)
      {
         check = false ;
         theString .Format("%d", maxSnap );
         if ( recip )
         {
            check = maxSnap == denom ;
            theString = "1/"  + theString ;
            maxSnap <<= 1 ;
         }
         else
         {
            check = maxSnap == numer ;
            recip = maxSnap == 1 ;
            if ( recip )
               maxSnap = 2 ;
            else
               maxSnap >>= 1 ;
         }
         if(check)
         {
            m_nSelectedSnap=i;
         }
         m_strList.Add(theString);
      }
   }
   else
   {
      // Now check for decimal

      i = divisions ;
      while ( ( i % 10 ) == 0 )
      {
         i /= 10 ;
      }

      if ( i == 1 )
      {
         // Decimal

         for (i = 0; i <= 10 ; i++)
         {
            check = false ;
            theString.Format("%d", maxSnap );
            long j = maxSnap ;
            while ( ( j % 10 ) == 0 )
            {
               j /= 10 ;
            }

            if ( recip )
            {
               check = (maxSnap == denom);

               theString = "1/"  + theString ;
               if ( j == 1 )
               {
                  maxSnap *= 2 ;
               }
               else if ( j == 5 )
               {
                  maxSnap *= 2 ;
               }
               else
               {
                  maxSnap = maxSnap * 5 / 2 ;
               }
            }
            else
            {
               check = (maxSnap == numer);
               recip = (maxSnap == 1) ;
               if ( recip )
               {
                  maxSnap = 2 ;
               }
               else if ( j == 1 )
               {
                  maxSnap /= 2 ;
               }
               else
               {
                  if ( j == 5 )
                  {
                     maxSnap = maxSnap * 2 / 5 ;
                  }
                  else
                  {
                     maxSnap /= 2 ;
                  }
               }
            }
            if(check)
            {
               m_nSelectedSnap=i;
            }
            m_strList.Add(theString);
         }
      }
      else
      {
         // probably 360 but maybe something whacky
         CString strDegree;
         strDegree.LoadString(IDS_DEGREE_SYMBOL);

         for (i = 0 ; i < 20 ; i++)
         {
            check = false ;
            if ( maxSnap == 1 )
               break ;

            if ( ( i != 0 ) || ( maxSnap == divisions ) )
            {
               --maxSnap ;
               while ( divisions % maxSnap )
               {
                  --maxSnap ;
               }
            }

            if(maxSnap == numer)
            {
               m_nSelectedSnap=i;
            }
            theString.Format("%d", maxSnap ) ;
            if ( !isCustomAngleUnits())
               theString = theString + strDegree;
            m_strList.Add(theString);
         }
      }
   }
}

bool CSnapTable::isCustomAngleUnits()
{
   return m_nAngleUnit<0;
}

int CPCDraftDoc::GetAngleUnit()
{
   return m_snapTable.getAngleUnit();
}

void CSnapTable::setAngleUnit(int newVal)
{
   if(newVal!=m_nAngleUnit)
   {
      m_nAngleUnit=newVal;
      selectSnap(-1);
      buildSnapTable();
   }
}

void CSnapTable::setAngleSnaps(short newVal)
{
   if(newVal!=m_nAngleSnapsToACircle)
   {
      m_nAngleSnapsToACircle=newVal;
      m_nSelectedSnap=-1;
      buildSnapTable();
   }
}

long CSnapTable::calcDivisions()
{
   return ( isCustomAngleUnits() ? -m_nAngleUnit : 360 );
}

BOOL CPCDraftDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
   // Infinisys Ltd.
   OnNewDocument();

   if (!CDocument::OnOpenDocument(lpszPathName))
      return FALSE;

   if(((CPCDraftApp*)AfxGetApp())->m_bRemoveLinks)
   {
      for(int i=0; i<GetLayersCount(); i++)
         GetLayer(i)->m_LinkVertexList.DeleteAll();
   }

   return TRUE;
}


void CPCDraftDoc::AutomaticScale(double dExtent)
{
   int nScales;
   Ratio* pScales=GetScaleTableInfo(m_nUnit,nScales);
   nScales--;
   int nScaleIndex=(m_nUnit & kUEnglishMask)?4:6;// default
   double bestSize = -1;
   for(int i=1; i<=nScales; i++)
   {
      double extent = ( dExtent * pScales[ i ].fN ) / ( pScales[ i ].fD ) ;

      if ( extent <= 4095.0 && extent > bestSize )
      {
         bestSize = extent ;
         nScaleIndex=i;
      }
   }
   m_nDefScale=nScaleIndex;
   GetActiveLayer()->m_nScale=nScaleIndex;
   m_DimUnit.m_ScaleIndex=nScaleIndex;
}


void SaveJPEG(CString strPath, Gdiplus::Bitmap * pbitmap, int iQuality)
{

   if(iQuality < 0)
   {

      iQuality = 0;

   }
   else if(iQuality > 100)
   {

      iQuality = 100;

   }

   CLSID clsid;

   int result = GetEncoderClsid(L"image/jpeg",&clsid);

   if(result < 0)
   {

      AfxMessageBox("The JPEG encoder is not installed.", MB_ICONEXCLAMATION);

   }

   LPWSTR pwsz = ConvertString(strPath);

   Gdiplus::EncoderParameters encoderParameters;

   ULONG             quality;

   // Before we call Image::Save, we must initialize an
   // EncoderParameters object. The EncoderParameters object
   // has an array of EncoderParameter objects. In this
   // case, there is only one EncoderParameter object in the array.
   // The one EncoderParameter object has an array of values.
   // In this case, there is only one value (of type ULONG)
   // in the array. We will let this value vary from 0 to 100.

   encoderParameters.Count = 1;
   encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
   encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
   encoderParameters.Parameter[0].NumberOfValues = 1;

   // Save the image as a JPEG with quality level 100.
   quality = iQuality;
   encoderParameters.Parameter[0].Value = &quality;

   pbitmap->Save(pwsz,&clsid,&encoderParameters);

   delete[] pwsz;

}
Gdiplus::Bitmap * LoadPNG(const CArray<unsigned char> & data)
{

   Gdiplus::Bitmap * p =nullptr;

   DWORD imageSize = data.GetSize();
   if(!imageSize)
      return p;

   IStream* pStream = ::SHCreateMemStream(data.GetData(),data.GetSize());
   if(!pStream)
      return p;

   p = Gdiplus::Bitmap::FromStream(pStream);
   pStream->Release();
   if(p)
   {
      if(p->GetLastStatus() != Gdiplus::Ok)
      {
         delete p;
         return nullptr;

      }
   }
   return p;
}

bool SavePNG(CArray<unsigned char> & data,Gdiplus::Bitmap * pbitmap)
{
   CLSID clsid;

   int result = GetEncoderClsid(L"image/png",&clsid);

   if(result < 0)
   {

      AfxMessageBox("The PNG encoder is not installed.",MB_ICONEXCLAMATION);

   }

   auto stream = SHCreateMemStream(NULL, 0);
   pbitmap->RotateFlip(Gdiplus::RotateNoneFlipY);

   if(pbitmap->Save(stream,&clsid,NULL) == Gdiplus::Ok)
   {
      ULARGE_INTEGER ui;
      if(SUCCEEDED(IStream_Size(stream,&ui)) && ui.HighPart == 0)
      {

         data.SetSize(ui.LowPart);
         LARGE_INTEGER li;
         li.u.HighPart = 0;
         li.u.LowPart = 0;
         if(SUCCEEDED(stream->Seek(li,STREAM_SEEK_SET,NULL)))
         {

            if(SUCCEEDED(IStream_Read(stream,data.GetData(),data.GetSize())))
            {
               stream->Release();
               pbitmap->RotateFlip(Gdiplus::RotateNoneFlipY);
               return true;
            }
         }
         stream->Release();
         pbitmap->RotateFlip(Gdiplus::RotateNoneFlipY);

      }

   }

   return false;

}
   
void SavePNG(CString strPath,Gdiplus::Bitmap * pbitmap)
{

   CLSID clsid;

   int result = GetEncoderClsid(L"image/png",&clsid);

   if(result < 0)
   {

      AfxMessageBox("The PNG encoder is not installed.",MB_ICONEXCLAMATION);

   }

   LPWSTR pwsz = ConvertString(strPath);

   //Gdiplus::EncoderParameters encoderParameters;

   pbitmap->Save(pwsz,&clsid,NULL);

   delete[] pwsz;

}

void SaveBMP(CString strPath, Gdiplus::Bitmap * pbitmap)  // Infinisys Ltd. 2021
{

   CLSID clsid;

   int result = GetEncoderClsid(L"image/bmp", &clsid);

   if (result < 0)
   {

      AfxMessageBox("The BMP encoder is not installed.", MB_ICONEXCLAMATION);

   }

   LPWSTR pwsz = ConvertString(strPath);

   //Gdiplus::EncoderParameters encoderParameters;

   pbitmap->Save(pwsz, &clsid, NULL);

   delete[] pwsz;

}

UINT AFXAPI AfxGetFileName2(LPCWSTR lpszPathName, _Out_writes_opt_(nMax) LPWSTR lpszTitle, UINT nMax)
{
   ASSERT(lpszTitle == NULL ||
      AfxIsValidAddress(lpszTitle, nMax));
   ASSERT(AfxIsValidString(lpszPathName));

   ENSURE_ARG(lpszPathName != NULL);

   // always capture the complete file name including extension (if present)
   LPWSTR lpszTemp = ::PathFindFileNameW(lpszPathName);

   // lpszTitle can be NULL which just returns the number of bytes
   if (lpszTitle == NULL)
      return static_cast<UINT>(wcslen(lpszTemp)) + 1;

   // otherwise copy it into the buffer provided
   Checked::wcsncpy_s(lpszTitle, nMax, lpszTemp, _TRUNCATE);
   return 0;
}

UINT AFXAPI AfxGetFileTitle2(LPCWSTR lpszPathName, _Out_writes_(nMax) LPWSTR lpszTitle, UINT nMax)
{
   ASSERT(lpszTitle == NULL ||
      AfxIsValidAddress(lpszTitle, _MAX_FNAME));
   ASSERT(AfxIsValidString(lpszPathName));

   // use a temporary to avoid bugs in ::GetFileTitle when lpszTitle is NULL
   WCHAR szTemp[_MAX_PATH];
   LPWSTR lpszTemp = lpszTitle;
   if (lpszTemp == NULL)
   {
      lpszTemp = szTemp;
      nMax = _countof(szTemp);
   }
   if (::GetFileTitleW(lpszPathName, lpszTemp, (WORD)nMax) != 0)
   {
      // when ::GetFileTitle fails, use cheap imitation
      return AfxGetFileName2(lpszPathName, lpszTitle, nMax);
   }
   return lpszTitle == NULL ? static_cast<UINT>(wcslen(lpszTemp)) + 1 : 0;
}

BOOL AFXAPI AfxFullPath(_Pre_notnull_ _Post_z_ LPWSTR lpszPathOut, LPCWSTR lpszFileIn);
BOOL AFXAPI AfxFullPath(_Pre_notnull_ _Post_z_ LPTSTR lpszPathOut,LPCTSTR lpszFileIn);

UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName,_Out_writes_(nMax) LPTSTR lpszTitle,UINT nMax);

HRESULT _AfxSHCreateItemFromParsingName(__in PCWSTR pszPath,__in IBindCtx *pbc,__in REFIID riid,__out void **ppv);

void CPCDraftDoc::SetPathName(LPCTSTR lpszPathName,BOOL bAddToMRU)
{

   if (m_bUtf8Name)
   {
      CStringW wstrPathName(get_uni(lpszPathName));
      // store the path fully qualified
      WCHAR szFullPath[_MAX_PATH];
      ENSURE(lpszPathName);
      if (wcslen(wstrPathName) >= _MAX_PATH)
      {
         ASSERT(FALSE);
         // MFC requires paths with length < _MAX_PATH
         // No other way to handle the error from a void function
         AfxThrowFileException(CFileException::badPath);
      }

      if (AfxFullPath(szFullPath, wstrPathName) == FALSE)
      {
         ASSERT(FALSE);
         // MFC requires paths with length < _MAX_PATH
         // No other way to handle the error from a void function
         AfxThrowFileException(CFileException::badPath);
      }

      m_strPathName = get_utf8(szFullPath);
      ASSERT(!m_strPathName.IsEmpty());       // must be set to something
      m_bEmbedded = FALSE;
      ASSERT_VALID(this);

      // set the document title based on path name
      WCHAR szTitle[_MAX_FNAME];
      if (AfxGetFileTitle2(szFullPath, szTitle, _MAX_FNAME) == 0)
         SetTitle(get_cp(szTitle, CP_UTF8));

      // add it to the file MRU list
      if (bAddToMRU)
      {

         auto papplication = ((CPCDraftApp *)AfxGetApp());

         CString strAppID = papplication->m_pszAppID;

         ASSERT(AfxIsValidString(lpszPathName));

         papplication->recent_file_list()->Add(lpszPathName);

         if (papplication->m_pTemplateWnd)
         {

            papplication->m_pTemplateWnd->m_recentwnd.m_precentfilelist->Add(wstrPathName);

         }

         HRESULT hr = S_OK;
         CComPtr<IShellItem> psi = NULL;

         CStringW wstr;

         wstr = get_uni(m_strPathName, CP_UTF8);

         hr = _AfxSHCreateItemFromParsingName(wstr, NULL, IID_IShellItem, reinterpret_cast<void**>(&psi));

         ENSURE(SUCCEEDED(hr));

         ((CPCDraftApp *)AfxGetApp())->recent_file_list()->Add(psi, strAppID);


      }

      ASSERT_VALID(this);
   }
   else
   {
      // store the path fully qualified
      TCHAR szFullPath[_MAX_PATH];
      ENSURE(lpszPathName);
      if (_tcslen(lpszPathName) >= _MAX_PATH)
      {
         ASSERT(FALSE);
         // MFC requires paths with length < _MAX_PATH
         // No other way to handle the error from a void function
         AfxThrowFileException(CFileException::badPath);
      }

      if (AfxFullPath(szFullPath, lpszPathName) == FALSE)
      {
         ASSERT(FALSE);
         // MFC requires paths with length < _MAX_PATH
         // No other way to handle the error from a void function
         AfxThrowFileException(CFileException::badPath);
      }

      m_strPathName = szFullPath;
      ASSERT(!m_strPathName.IsEmpty());       // must be set to something
      m_bEmbedded = FALSE;
      ASSERT_VALID(this);

      // set the document title based on path name
      TCHAR szTitle[_MAX_FNAME];
      if (AfxGetFileTitle(szFullPath, szTitle, _MAX_FNAME) == 0)
         SetTitle(szTitle);

      // add it to the file MRU list
      if (bAddToMRU)
      {
         CWinApp* pApp = AfxGetApp();

         CString strAppID = pApp->m_pszAppID;

         ASSERT(AfxIsValidString(lpszPathName));

         ((CPCDraftApp *)AfxGetApp())->recent_file_list()->Add(lpszPathName);

         ((CPCDraftApp *)AfxGetApp())->m_pTemplateWnd->m_recentwnd.m_precentfilelist->Add(get_uni(lpszPathName, CP_ACP));

         HRESULT hr = S_OK;
         CComPtr<IShellItem> psi = NULL;

         CStringW wstr;

         wstr = get_uni(m_strPathName, CP_ACP);

         hr = _AfxSHCreateItemFromParsingName(wstr, NULL, IID_IShellItem, reinterpret_cast<void**>(&psi));

         ENSURE(SUCCEEDED(hr));

         ((CPCDraftApp *)AfxGetApp())->recent_file_list()->Add(psi, strAppID);


      }

      ASSERT_VALID(this);

   }

}
