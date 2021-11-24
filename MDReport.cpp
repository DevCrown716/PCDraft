// MDReport.cpp: implementation of the CMDReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "pcdraftdoc.h"
#include "MDReport.h"
#include "Layer.h"
#include "Excel.h"

#include "CloneUnitsDlg.h"
#include "CloneFtInUnitDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define kMinDefaultColWidth		80
#define kDefaultNumRows			6

IMPLEMENT_SERIAL(CMDReport,CObject,VERSIONABLE_SCHEMA|2)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMDReport::CMDReport(bool bInit)
{
	m_nLinkType=kLinkNone;

	for(int i=0;i<6;i++)
		m_nColumnWidths[i]=kMinDefaultColWidth;
	if(bInit)
	{
		for(int i=0;i<kDefaultNumRows;i++)
			m_Rows.Add(new CMDReportRow);
	}
	
	CFont theFont;
	theFont.CreateStockObject(DEFAULT_GUI_FONT);
	theFont.GetLogFont(&m_LogFont);
	m_LogFont.lfHeight-=2;

	GenerateTag();
}

CMDReport::~CMDReport()
{
	for(int i=0;i<m_Rows.GetSize();i++)
		delete (CMDReportRow*)m_Rows[i];
}

void CMDReport::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_sLinkFile;
		ar >> m_nLinkType;
		ar >> m_nLinkUnits;
		ar >> m_sTitle;
		for(int i=0;i<6;i++)
			ar >> m_nColumnWidths[i];
		ar.Read(&m_LogFont,sizeof(m_LogFont));
		if(ar.m_nObjectSchema>1)
		{
			ar >> m_dwTag;
		}
	}
	else
	{
		ar << m_sLinkFile;
		ar << m_nLinkType;
		ar << m_nLinkUnits;
		ar << m_sTitle;
		for(int i=0;i<6;i++)
			ar << m_nColumnWidths[i];
		ar.Write(&m_LogFont,sizeof(m_LogFont));
      ar << m_dwTag;
	}
	m_Rows.Serialize(ar);
}

void CMDReport::GenerateTag()
{
static DWORD gCounter=0;
	m_dwTag=GetTickCount()+gCounter++;
}

static bool EqualFunctionRowData(CMDReportRow* pCriteria,CMDReportRow* pOldFunction,CMDReportRow* pNewFunction)
{
	for(int i=0;i<6;i++)
	{
		switch(pCriteria->m_nFieldRefNum[i])
		{
			case kNameField:
			case kF2Field:
			case kF3Field:
			case kF4Field:
			case kF5Field:
				if(pOldFunction->m_sRowText[i].CompareNoCase(pNewFunction->m_sRowText[i]))
					return false;
		}
	}
	return true;
}

bool CMDReport::Calculate(CPCDraftDoc* pDoc, bool bClone)
{
	CMDReportRow* pLastCriteriaRow=NULL;
	CMDReportRow* pLastFunctionRow=NULL;
	CMDShapeList theMatchingShapes;
	CWordArray theMatchingLayers;
	CDimUnit du=*pDoc->GetDimUnit();
	du.m_bHideMetricUnits=false; // force not to hide metric units
	if(bClone)
	{
		if(du.m_Display==kDimIn)
		{
			CCloneUnitsDlg theDlg;
			if(theDlg.DoModal()==IDOK)
			{
				if(theDlg.m_nUnits==1)
				{
					du.switchUnits(kUDecInches,kEngRes);
				}
			}
			else return false;
		}
		else
		{
			if(du.m_Display==kDimFt_In)
			{
				CCloneFtInUnitDlg theDlg;
				if(theDlg.DoModal()==IDOK)
				{
					switch(theDlg.m_nUnits)
					{
					case 1:
						du.switchUnits(kUDecInches,kEngRes);
						break;
					case 2:
						du.switchUnits(kUDecFeet,kEngRes);
						break;
					}
				}
				else return false;
			}
		}
	}
	for(int i=0;i<m_Rows.GetSize();i++)
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_Rows[i];
		if(pReportRow->m_nRowType==kCriteriaRow)
		{
			pLastCriteriaRow=pReportRow;
		}
		else
		{
			if(pLastCriteriaRow!=NULL && pReportRow->m_nRowType==kFunctionRow)
			{
				if(pLastFunctionRow==NULL || !EqualFunctionRowData(pLastCriteriaRow,pLastFunctionRow,pReportRow))
				{
					theMatchingShapes.RemoveAll();
					theMatchingLayers.RemoveAll();
					for(int n=0;n<pDoc->GetLayersCount();n++)
					{
						CLayer* pLayer=pDoc->GetLayer(n);
						POSITION pos=pLayer->m_ShapeList.GetHeadPosition();
						while(pos)
						{
							CMDShape* pShape=pLayer->m_ShapeList.GetNext(pos);
							if(pShape->ReportFitsCriteria(pLastCriteriaRow,pReportRow))
							{
								theMatchingShapes.AddHead(pShape);
								theMatchingLayers.Add(n);
							}
						}
					}
				}
				CString sResult;
				switch(pReportRow->m_nFunction)
				{
					case kFunctCount:
						sResult.Format("%d",theMatchingShapes.GetCount());
						break;
					case kFunctArea:
					case kFunctHeight:
					case kFunctLength:
					case kFunctPerimeter:
					case kFunctWidth:
					case kFunctXDim:
					case kFunctYDim:
						{
							LongRatio ratio;
							double dResult=0.0;
							POSITION pos=theMatchingShapes.GetHeadPosition();
							int n=0;
							while(pos)
							{
								pDoc->GetScaleRatio(pDoc->GetLayer(theMatchingLayers[n++])->m_nScale,ratio);
								CMDShape* pShape=theMatchingShapes.GetNext(pos);
								double shapeResult=0.0;
								switch(pReportRow->m_nFunction)
								{
									case kFunctArea: shapeResult=pShape->ReportCalcArea(ratio); break;
									case kFunctHeight: shapeResult=pShape->ReportCalcHeight(ratio); break;
									case kFunctLength: shapeResult=pShape->ReportCalcLength(ratio); break;
									case kFunctPerimeter: shapeResult=pShape->ReportCalcPerimeter(ratio); break;
									case kFunctWidth: shapeResult=pShape->ReportCalcWidth(ratio); break;
									case kFunctXDim: shapeResult=pShape->ReportCalcXDim(ratio); break;
									case kFunctYDim: shapeResult=pShape->ReportCalcYDim(ratio); break;
								}
								dResult+=shapeResult;
							}
							switch(pReportRow->m_nFunction)
							{
								case kFunctArea:
									{
										sResult=du.calcAreaStr(dResult);
										if(bClone) sResult=sResult.Left(sResult.ReverseFind(' '));
									}
									break;
								default:
									sResult=du.calcDimStr(dResult,bClone);
									break;
							}
						}
						break;
					case kFunctTotalName:
					case kFunctTotalF2:
					case kFunctTotalF3:
					case kFunctTotalF4:
					case kFunctTotalF5:
						{
							long lResult=0;
							POSITION pos=theMatchingShapes.GetHeadPosition();
							while(pos)
							{
								CMDShape* pShape=theMatchingShapes.GetNext(pos);
								lResult+=pShape->ReportCalcFieldTotal(pReportRow->m_nFunction);
							}
							sResult.Format("%d",lResult);
						}
						break;
				}
				for(int n=0;n<6;n++)
					if(pLastCriteriaRow->m_nFieldRefNum[n]==kResultField)
						pReportRow->m_sRowText[n]=sResult;
			}
		}
	}
	return true;
}

IMPLEMENT_SERIAL(CMDReportRow,CObject,1)

CMDReportRow::CMDReportRow()
{
	m_nRowType=kTextRow;
	m_nFunction=0;
	for(int i=0;i<6;i++)
	{
		m_nJust[i]=kJustLeft;
		m_sRowText[i].Empty();
	}
}

CMDReportRow::~CMDReportRow()
{

}

void CMDReportRow::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_nRowType;
		ar >> m_nFunction;
		if(m_nRowType==kCriteriaRow)
		{
			for(int i=0;i<6;i++)
				ar >> m_nFieldRefNum[i];
		}
		else
		{
			for(int i=0;i<6;i++)
			{
				ar >> m_sRowText[i];
				ar >> m_nJust[i];
			}
		}
	}
	else
	{
		ar << m_nRowType;
		ar << m_nFunction;
		if(m_nRowType==kCriteriaRow)
		{
			for(int i=0;i<6;i++)
				ar << m_nFieldRefNum[i];
		}
		else
		{
			for(int i=0;i<6;i++)
			{
				ar << m_sRowText[i];
				ar << m_nJust[i];
			}
		}
	}
}

static void UpdateCell(CMDReportRow* pReportRow,_Worksheet& theWorksheet,int i,int n,bool bNumber)
{
	COleVariant theOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	Range theRange;
	CString theName;
	theName.Format("PCDraft_R%d_C%d",i+1,n+1);
	TRY
	{
		theRange=theWorksheet.GetRange(COleVariant(theName),theOptional);
	}
	CATCH_ALL(e)
	{
		CString theCell;
		theCell.Format("%c%d",'A'+n,i+1);
		theRange=theWorksheet.GetRange(COleVariant(theCell),theOptional);
		theRange.SetName(COleVariant(theName));
	}
	END_CATCH_ALL
	theRange.SetNumberFormat(COleVariant("@"));
	theRange.SetValue(COleVariant(pReportRow->m_sRowText[n]));
//	if(bNumber) theRange.SetNumberFormat(COleVariant("0"));
}

void CMDReport::Clone(int nCopyType)
{
#ifndef _DEMO
	if(((CPCDraftApp*)AfxGetApp())->m_bDemoMode) return;

	COleVariant theOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	_Application theApplication;
	Worksheets theWorksheets;
	_Worksheet theWorksheet;
	Workbooks theWorkbooks;
	_Workbook theWorkbook;
	bool bUpdate;
	TRY
	{
		if(nCopyType>=0)
		{
			m_nLinkType=kLinkAll;
		}
		// Instantiate Excel and start a new workbook.
		if(theApplication.GetDispatch("Excel.Application"))
		{
			bUpdate=true;
		}
		else if(theApplication.CreateDispatch("Excel.Application"))
		{
			bUpdate=false;
		}
		else
		{
			AfxMessageBox(IDS_EXCEL_NOT_FOUND,MB_OK);
			m_nLinkType=kLinkNone;
			return;
		}
		theWorkbooks=theApplication.GetWorkbooks();
		if(theWorkbooks.GetCount()==0) bUpdate=false;
		if(bUpdate)
		{
			theWorkbook=theApplication.GetActiveWorkbook();
		}
		else
		{
			theWorkbook=theWorkbooks.Add(theOptional);
		}
		theWorksheets=theWorkbook.GetWorksheets();
		if(bUpdate)
		{
			TRY
			{
				theWorksheet=theWorksheets.GetItem(COleVariant(m_sTitle));
			}
			CATCH(CException,e)
			{
				theWorksheet=theWorksheets.Add(theOptional,theOptional,theOptional,theOptional);
				theWorksheet.SetName(m_sTitle);
			}
			END_CATCH
		}
		else
		{
			theWorksheet=theWorksheets.GetItem(COleVariant((short)1));
			theWorksheet.SetName(m_sTitle);
		}
		CMDReportRow* pLastCriteriaRow=NULL;
		for(int i=0;i<m_Rows.GetSize();i++)
		{
			CMDReportRow* pReportRow=(CMDReportRow*)m_Rows[i];
			if(pReportRow->m_nRowType==kCriteriaRow)
			{
				pLastCriteriaRow=pReportRow;
			}
			else
			{
				if(m_nLinkType==kLinkResults && bUpdate)
				{
					if(pLastCriteriaRow!=NULL && pReportRow->m_nRowType==kFunctionRow)
					{
						for(int n=0;n<6;n++)
							if(pLastCriteriaRow->m_nFieldRefNum[n]==kResultField)
							{
								UpdateCell(pReportRow,theWorksheet,i,n,true);
							}
					}
				}
				else
				{
					for(int n=0;n<6;n++)
					{
						UpdateCell(pReportRow,theWorksheet,i,n,pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[n]==kResultField);
					}
				}
			}
		}
		if(nCopyType>=0)
		{
			m_nLinkType=nCopyType;
		}
		// Return control of Excel to the user.
		theApplication.SetVisible(TRUE);
		theApplication.SetUserControl(TRUE);
	}
	CATCH(CException,e)
	{
		m_nLinkType=kLinkNone;
		e->ReportError();
	}
	END_CATCH
#endif
}

void CMDReport::Unlink()
{
	m_nLinkType=kLinkNone;
	m_sLinkFile="";
	m_nLinkUnits=0;
}
