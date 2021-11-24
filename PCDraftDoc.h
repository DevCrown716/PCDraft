// PCDraftDoc.h : interface of the CPCDraftDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCDRAFTDOC_H__F8C83DFC_0F91_11D2_9806_000000000000__INCLUDED_)
#define AFX_PCDRAFTDOC_H__F8C83DFC_0F91_11D2_9806_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DSShape.h"
#include "DimStructs.h"
#include "lpoint.h"

#include "DXFUtil.h"
#include "DimUnit.h"
#include "PackedDIB.h"
#include "GradientPack.h"
#include "PackedItem.h"

#include "PrinterSettings.h"



#define kEngRes		72
#define kMetRes		28
#define	kEngDecRes	86

#define	kEngFactor		72.0
#define	kMetFactor		71.12
#define	kEngDecFactor	71.666666667

#define	kUpdateBars				1
#define	kZoomFactorChanged		2
#define	kRefresh				3
#define	kDocumentSizeChanged	4
#define kActivateLayer			5
#define kClearSelection			6
#define	kRefreshShape			7
#define	kRefreshShapes			8
#define	kDeactivateTextShape	9
#define	kReportTitleChanged		20
#define	kReportDeleted			21
#define	kSaveRef				50
#define	kResolveRef				51
#define	kLockWindowUpdate			52
#define	kUnlockWindowUpdate			53
#define	kInvalidateSelection				1001 // Opacity/Gradient - Infinisys Ltd
#define	kApplyLastPackedItem				1002 // Opacity/Gradient - Infinisys Ltd

#define kLRulerWid			16		/*	Width of left ruler					*/
#define kTRulerWid			16		/*	Width of top ruler					*/




struct Ratio
{
   unsigned short fN,fD;
};

typedef struct
{
   short	fScaleIndex;
   short	fNumEntries;
   struct	{short str,scal;} entry[10];
} GrdEntries;

enum ePageNumbering
{
   kPageNumbering1324 = 0,
   kPageNumbering1234 = 1
};

class CLayer;
class CMDReport;

// Guides - Infinisys Ltd 2020
class CGuide
{
public:

   bool     m_bVertical;
   Coord    m_coord;


   CGuide() {}
   CGuide(bool bVertical, Coord coord):m_bVertical(bVertical), m_coord(coord) {}

};

class CSnapTable
{
private:
   short m_nAngleSnapsToACircle;
   int m_nSelectedSnap;
   int m_nAngleUnit;
   CStringArray m_strList;

   bool isCustomAngleUnits();
   long calcDivisions();
   void buildSnapTable();
public:
   void selectSnap(int index);

   int getSelectedSnap() { return m_nSelectedSnap; }

   void setAngleUnit(int newVal);
   int getAngleUnit() {return m_nAngleUnit; }

   short getAngleSnaps() {return m_nAngleSnapsToACircle;}
   void setAngleSnaps(short newVal);

   void GetAngleSnaps(CStringArray& strList);

   CSnapTable();

};


class CVersionDoc: public CDocument // Infinisys Ltd
{
public:



   int                                 m_iArchiveVersion;
   //by TSM CPackedDIB m_Patterns[71];
   CPackedDIB m_Patterns[2048];
   int m_PatternsHash[2048];
   int m_numDIBPatterns;

   // Gradient - Infinisys Ltd
   CGradientPack                       m_Gradients[2048];
   int                                 m_numGradients;
   // Gradient - Infinisys Ltd
   CArray < CPackedItem >              m_packeditema;

   CVersionDoc();
   virtual ~CVersionDoc();


   HBITMAP GetPatternBitmap(int nIndex, int iOpacity); // Opacity/Transparency - Infinisys Ltd

};

class CPCDraftDoc: public CVersionDoc
{
protected: // create from serialization only
   DECLARE_DYNCREATE(CPCDraftDoc)

public:
   CPCDraftDoc();

// Attributes
public:
   bool m_bUtf8Name;
   int m_nUnit;
   bool m_bHideMetricUnits;
   int m_nPlaces;
   int m_nAnglePlaces;
   CDimUnit m_DimUnit;
   bool m_bSnapToObject;
   bool m_bSnapToGuides; // Infinisys Ltd. 2020 Guides
   bool m_bShowGuideLines; // Infinisys Ltd. 2020 Guides
   bool m_bHidePageBreaks;
   bool m_bHideGridLines;
   bool m_bShowRulers;
   bool m_bStandardRulers;
   LongPoint m_RulerOrigin;
   bool m_bEnglishUnits;
   int	m_nDisplayRes;
   int m_nZoomFactor;
   int m_nPrevZoomFactor;
#ifdef _FLOAT_ZOOM
   double m_fZoomFactor;
   double m_fPrevZoomFactor;
#endif
   CPoint m_ptPrevZoomScroll;
   CSize m_PageSize;
   CPoint m_NumPage;
   int m_nPageNumbering;
   BOOL m_bPrintLastPageFirst;
   int m_nActiveLayer;
   bool m_bActiveLayerHidden;
   CTypedPtrArray<CObArray,CLayer*> m_Layers;
   CMDReport* m_pActiveReport;
   CTypedPtrArray<CObArray,CMDReport*> m_Reports;
   CObArray m_SavedViews;
   int m_nDefScale;
   bool m_bHideText;

   PenModel m_Pen;
   FillModel m_Fill;
   ParallelAttrs m_ParallelAttrs;
   TextParams m_TextParams;
   DimStandard m_DimStd;
   DimFormat m_DimFmts[3];
   BYTE m_nDimCap1,m_nDimCap2;


   CString m_sFieldNames[5];
   bool m_bHideFills;
   CPalette m_Palette;
//	HBITMAP m_DIBPatterns[71];
   unsigned char m_DashStyles[256];

   // Guides - Infinisys Ltd 2020
   CArray < CGuide >    m_guidea;
   


   CPrinterSettings m_PrinterSettings;
// version 5.0
   BOOL m_bRegistrationMarks;

// custom scale
   LongRatio    m_ratioCustom;  // Free Scale - Infinisys Ltd
   //bool     m_bCustomRatio;=  m_nDefScale == -1

//	int m_nAngleUnit;
   CSnapTable m_snapTable;

// not serialized
   int m_nSlideSeconds;
   bool m_bLayer1AsMaster;
   bool m_bSaveAsMacDraft;
   bool m_bSaveAsStationery;
   int m_nMaxZoom;
   bool m_bComplete;
   
   
   bool m_bUpdateViews; // Library - PCDraft 6 - Infinisys Ltd.



// Operations
public:
   bool XInDocument(const Coord & coord); // Guides PCDraft 7 Infinisys Ltd.
   bool YInDocument(const Coord & coord); // Guides PCDraft 7 Infinisys Ltd.
   CSize GetDocSize(); // Guides PCDraft 7 Infinisys Ltd.

   void FillModelBitmap(int i,CDC & dc, LPCRECT lpcrect, int iOpacity); // Opacity/Transparency - Infinisys Ltd
   CString CreateTitle();
   CStringW CreateTitleW();
   CSize GetTotalDevSize();
   CSize GetPageDevSize();
   bool SetupUnits(int nUnit,bool bHideMetricUnits,int nAngleUnit,int nPlaces,int nAnglePlaces,int nScale,DimStandard* pDimStd=NULL,int nAlertID=0,LongRatio * plongratioCustom = NULL);
   bool CheckLayerName(CString& sName);
   bool CheckReportName(CString& sName);
   CMDReport* GetReportByTag(DWORD dwTag);
   CLayer* AddLayer(CString* pName=NULL);
   CLayer* AddLayerSorted(CString* pName);
   void MoveLayer(CArray < int > & ia,int nNewIndex); // Layer - PCDraft 6.0 - Infinisys Ltd.
   void MoveLayer(int nOldIndex,int nNewIndex);
   void DeleteLayer(int nIndex);
   void MergeLayer(int nCount,int* pIndexes);
   CLayer* GetLayer(int nIndex) { return m_Layers[nIndex]; }
   CLayer* GetActiveLayer() { return (m_nActiveLayer<0)?NULL:GetLayer(m_nActiveLayer); }
   bool SetActiveLayer(int nIndex);
   int GetLayersCount() { return (int) m_Layers.GetSize(); }
   int GetFirstLayerIndex();
   int GetLastLayerIndex();
   void ActivateNextLayer();
   void ActivatePrevLayer();
   int GetVisibleLayerCount();

   double NoZoomToZoom(double d); // Infinisys Ltd. 2021
   long NoZoomToZoom(long n);
   WrFixed	NoZoomToZoom(WrFixed f);
   double ZoomToNoZoom(double d); // Infinisys Ltd. 2021
   long ZoomToNoZoom(long n); // Infinisys Ltd. 2021
   WrFixed ZoomToNoZoom(WrFixed f);
   long LPtoDP(WrFixed n);
   double LPtoDPF(WrFixed n);// Infinisys Ltd. GDI+
   pointd LPtoDPF(const LongPoint& inPoint);// Infinisys Ltd. GDI+
   CPoint LPtoDP(const LongPoint& inPoint);
   CRect LPtoDP(const LongRect& inRect);
   void LPtoDP(pointd & p, const LongPoint& inPoint); // Infinisys Ltd. GDI+
   WrFixed DPtoLP(long n);
   LongPoint DPtoLP(const CPoint& inPoint);
   LongRect DPtoLP(const CRect& inRect);
   Coord DPtoLP(double d); // Infinisys Ltd. GDI+
   LongPoint DPtoLP(const pointd& lpt); // Infinisys Ltd. GDI+
   using ::CDocument::GetFile;
   CFile* GetFile(LPCTSTR lpszFileName,UINT nOpenFlags,CFileException* pError) override;


   void CenterZoom(double dCenterZoom, int nZoomFactor, CPoint ptScroll); // PC Draft 7
#ifdef _FLOAT_ZOOM
   void DoZoom(double fZoomShift,int nZoomShift,CPoint ptScroll);
#else
   void DoZoom(int nZoomShift,CPoint ptScroll);
#endif
   void CreateShape(CMDShape* pNewShape,CMDShape* pReplaceShape=NULL);
   void CreateShapes(CMDShapeList* pNewShapes,CMDShape* pReplaceShape=NULL);
   void DeleteShape(CMDShape* pShape);
   CMDReport* AddReport(CString* pName=NULL);
   void DeleteReport(int nIndex);
   //void ExportQT();
   //void ExportImage();

   void SerializeFieldNames(CArchive& ar);
   void SerializeDimStandards(CArchive& ar);
   void SerializeDimFormats(CArchive& ar);
   void SerializeDimEndmarks(CArchive& ar);
   void SerializeDrawAndPageSetup(CArchive& ar);
   void SerializeFillPatternsColors(CArchive& ar);
   void SerializeLineSettings(CArchive& ar);
   void SerializeScaleAndLayerSetup(CArchive& ar);
   void SerializeTextSettings(CArchive& ar);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPCDraftDoc)
public:
   virtual BOOL OnNewDocument();
   virtual void Serialize(CArchive& ar);
   virtual void DeleteContents();
   virtual void OnChangedViewList();
   virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
public:
   virtual BOOL SaveModified();
   //}}AFX_VIRTUAL

// Implementation
public:
   void SetPathName(LPCTSTR lpszPathName,BOOL bAddToMRU = TRUE);
   void AutomaticScale(double dValue);
   int GetAngleUnit();
   void SnapToAngle(LongPoint& ptTo, LongPoint& ptFrom, bool bShift);
   void GetAngleSnaps(CStringArray& strList);
   float fNoZoomToZoom(float penf);
   int GetObjectCount(bool bOverall);
   double CalcResChangeFactor(int nNewDisplayRes);
   void UpdateMaxZoom();
   COLORREF translate_color(COLORREF cr);
   bool IsLegalDocSize(const LongPoint& lrc) const;
   bool CheckDocSize(bool bExpand);
   // modified return type and extra notify parameter
   // Infinisys Ltd. - Japanese Input
   bool ExpandDoc(const LongRect& lrcBounds, bool bUpdate = true); 
   bool MatchScale(int& nScaleIndex,int nNewUnits,int nOldUnits);
   bool FindScale(int& nScaleIndex, int nUnits, LongRatio & longRatio); // Free Scale - Infinisys Ltd
   int GetNearestScale(LongRatio longRatio); // MD60 - Infinisys Ltd
   int GetNearestScale(); // Free Scale - Infinisys Ltd
   double GetFitFactor(const CSize& szLimit);
   CDimUnit* GetDimUnit() { return &m_DimUnit; }
   bool LoadMacDraft(const char* szPath);
   bool SaveMacDraft(const char* szPath);
   bool LoadPCDraft55(const char *szPath); // format .p55 - Infinisys Ltd
   bool SavePCDraft55(const char* szPath); // format .p55 - Infinisys Ltd
   bool LoadMacDraft60(const char *szPath, bool bUtf8Name = false); // format .md60 - Infinisys Ltd
   bool SaveMacDraft60(const char *szPath,bool bUtf8Name = false); // format .md60 - Infinisys Ltd
   bool LoadMacDraft70(const char *szPath,bool bUtf8Name = false); // format .md70 - Infinisys Ltd
   bool SaveMacDraft70(const char *szPath,bool bUtf8Name = false); // format .md70 - Infinisys Ltd
   bool ImportWinDraft(const char* szPath);
   bool ExportDXF(const char* szPath);
   bool ExportDWG(const char* szPath);
   bool ImportDXF(const char* szPath);
   int ImportDWG(const char* szPath, double sUnits, bool bMetric);
   bool SaveBitmap(const char* szPath);
   double GetCurUnitFactor();
   void Offset(const LongPoint& delta);
   LongRect GetCumulativeExtents(bool bBasic=true);
   void GetSnapRatio(int nSnapIndex,LongRatio& longRatio);
   void GetScaleRatio(bool bEnglish,int nScaleIndex,LongRatio& longRatio);
   void GetScaleRatio(int nScaleIndex,LongRatio& longRatio);
   void GetActiveScaleRatio(LongRatio& longRatio);
   void GetScaleInfo(int scale,int* strID,GrdEntries** entry,Ratio** snaps);
   bool GetActiveGridSnaps(CStringArray& strList);
   void LayersChanged();
   void RemoveAllSavedViews();
   virtual ~CPCDraftDoc();
   static CPCDraftDoc* GetActiveDocument();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   Ratio* GetScaleTableInfo(int nUnits,int& nScales);
   CSize CalcPrinterPageSize();
   CSize get_synthesized_printer_page_size();
   int get_synthesized_printer_page_size_index();
   void RemoveAllLayers();

// Generated message map functions
protected:
   virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
   //{{AFX_MSG(CPCDraftDoc)
   afx_msg void OnFilePageSetup();
   afx_msg void OnDataReportFormats();
   afx_msg void OnUpdateFileRevert(CCmdUI* pCmdUI);
   afx_msg void OnFileRevert();
   afx_msg void OnUpdateDataFindReplace(CCmdUI* pCmdUI);
   afx_msg void OnUpdateLayoutLayerSetup(CCmdUI* pCmdUI);
   //}}AFX_MSG
   afx_msg void OnDataPrintReport();// Interface Update - Infinisys Ltd
   afx_msg void OnDataUpdateLink();// Interface Update - Infinisys Ltd
   afx_msg void OnDataBreakLink();// Interface Update - Infinisys Ltd
   afx_msg void OnNewColor(); // Gradient - Infinisys Ltd
   afx_msg void OnNewPattern(); // Gradient - Infinisys Ltd
   afx_msg void OnNewGradient(); // Gradient - Infinisys Ltd
   afx_msg void OnEditLineStyleRange(UINT nID);
   afx_msg void OnEditPatternRange(UINT nID);
   afx_msg void OnEditDupPatternRange(UINT nID); // Gradient - Infinisys Ltd
   afx_msg void OnEditColorRange(UINT nID);
   afx_msg void OnReportRange(UINT nID);
   afx_msg void OnUpdateLinkRange(UINT nID);
   afx_msg void OnBreakLinkRange(UINT nID);
   DECLARE_MESSAGE_MAP()
private:
   void DxfSetShPenAndFill (bool pFill,  DxfParamStruct& sParam);
   CMDShape* DxfUpdtAddLine (LongPoint* pCurFdPtr, LongPoint* pNextFdPtr, DxfParamStruct& sParam);
   LongPoint DxfGetCentPt (LongPoint*	pCurFdPtr, LongPoint* pNextFdPtr,Angle pAng, Coord *pRadius);
   CMDShape* DxfUpdtAddCurveFit (LongPoint* pCurFdPtr,LongPoint* pNextFdPtr,Angle pAng, DxfParamStruct& sParam);
   void DxfSetFdPtToArc (LongPoint* pCenter, Coord pHRad, Coord pVRad, LongPoint* pScndPt, LongPoint* pThrdPt,short pCode);
   CMDShape* DxfUpdtAddDonut (bool pDownSide, DxfParamStruct& sParam);
   void DxfUpdtAddPolyByFlag (short pPolyFlag, DxfParamStruct& sParam);
   void DxfUpdtAddPoly (short pPlTrSolOk, DxfParamStruct& sParam);
   void DxfCalcArcPoints (LongPointPtr	centerPt, LongPoint rad, Angle startAng, Angle endAng,LongPointPtr p1, LongPointPtr p2,LongPointPtr p3);
   void DxfAddGrpMem (DxfParamStruct& sParam);
   void DxfUpdtShape (bool pUpdtGr, DxfParamStruct& sParam);
   void DxfCreateLine(DxfParamStruct& sParam);
   void DxfUpdtLine(DxfParamStruct& sParam);
   void DxfCreateArc (DxfParamStruct& sParam);
   void DxfUpdtArc (DxfParamStruct& sParam);
   void DxfCreateCircle (DxfParamStruct& sParam);
   void DxfUpdtCircle (DxfParamStruct& sParam);
   void DxfCreateText (DxfParamStruct& sParam);
   void DxfUpdtText ( CString pTxtStr, DxfParamStruct& sParam);
   void DxfCreateGroup (DxfParamStruct& sParam);
   void DxfUpdtGroup (DxfParamStruct& sParam);
   void DxfCreatePoly (DxfParamStruct& sParam);

   void DxfResetLStyleTab (CString strName,short pTabIdx );
   short DxfGetColDashIdx (CString 	strLineStyleStr );
//	static short DxfAddLayer (DxfParamStruct& sParam, CString strLayerName );
   short DxfFindLayer(CString strLayerName, DxfParamStruct& sParam);
   void DxfUseCurLayer (CString 	pLayerName, DxfParamStruct& sParam);
   short DxfGetColLnW(short pAcadLnW);
   void DxfCreateDefLayerLst(DxfParamStruct& sParam);
   void DxfFilterTab (short nGrpCode,short nStrId, CString strName, DxfParamStruct& sParam);
   void DxfFilterTablesTab (short nGrpCode,short nStrId,CString strName,DxfParamStruct& sParam);
   void DxfFilterHdrTab (short nGrpCode,short nStrId, CString strName, DxfParamStruct& sParam);
   void  DxfCollectDashStr (short pIndex,short pDashByte,unsigned char* pDashCodePtr);
   short DxfGetMacDraftMacColor ( short	pAcadColor );
   short DxfFindIDfromStrName (CString strName, short pWhichTab );
   long DxfGetInteger(char* pBuffer,UINT nBufSize, UINT& nBufIdx, short nGrpCode, DxfParamStruct& sParam);
   long DxfGetFloat(char* pBuffer,UINT nBufSize, UINT& nBufIdx, short nGrpCode, DxfParamStruct& sParam);
   long DxfGetString(char* pBuffer,UINT nBufSize, UINT& nBufIdx, short nGrpCode, DxfParamStruct& sParam);
   short DxfGetGrpValue (char* pBuffer,UINT bufSize, short nGrpCode, UINT& nBufIfx, DxfParamStruct& sParam);
   short DxfGrpValueType (short pGrpCode );
   long DxfReadBuffer(char * pBuffer, UINT nBufSize, DxfParamStruct& sParam);
   void DxfIntialLStyleTab();
   void DxfWriteBlock(CFile* pFile);
   void DxfWriteEntity(CFile* pFile);
   void DxfBuildLayerTab(CFile* pFile);
   void DxfBuildLineTab(CFile* pFile);
   void DxfWriteTable(CFile* pFile);
   void DxfWriteHeader(CFile* pFile);

};

#ifdef _FLOAT_ZOOM
short FixTheFloatingZoomFactor (double zoomFactor);
double FloatTheFixedZoomFactor(short zoomFactor);
short PackZoomFactor(int nZoomFactor,double fZoomFactor);
double PackDoubleZoomFactor(int nZoomFactor,double fZoomFactor); // MD6.2.1 - Infinisys Ltd.
void UnpackZoomFactor(short nPackedZoom,int& nZoomFactor,double& fZoomFactor);
void UnpackDoubleZoomFactor(double dPackedZoom,int& nZoomFactor,double& fZoomFactor); // MD6.2.1 - Infinisys Ltd.
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCDRAFTDOC_H__F8C83DFC_0F91_11D2_9806_000000000000__INCLUDED_)


class CFILE
{
public:

   FILE * m_pf;

   CFILE(const char *psz,const WCHAR * pwszMode,int iShare)
   {
      m_pf = NULL;
      open(get_uni(psz), pwszMode, iShare);
   }

   CFILE(const WCHAR * pwsz,const WCHAR * pwszMode,int iShare)
   {
      m_pf = NULL;
      open(pwsz,pwszMode,iShare);
   }


   CFILE(FILE * pf)
   {
      m_pf = pf;
   }
   ~CFILE()
   {
      if(m_pf != NULL)
      {
         fclose(m_pf);
      }
   }

   bool open(const WCHAR * pwsz,const WCHAR * pwszMode,int iShare)
   {
      if(m_pf != NULL)
      {
         fclose(m_pf);
      }
      m_pf = _wfsopen(pwsz,pwszMode,iShare);
      return m_pf != NULL;
   }


   operator FILE * ()
   {
      return m_pf;
   }




};


