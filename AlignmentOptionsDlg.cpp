// AlignmentOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "AlignmentOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	kAlignTextBitsStrID	20500

enum
{
	kDatumS = 1,
	kMiddleS,
	kTopS,
	kLeftS,
	kBottomS,
	kRightS,
	kHorizontalS,
	kVerticalS,
	kCenterS,
	kCornerS,
	kHandEdgeS,
	kAndS,
	kAlignSelectedS,
	kReferenceS,
	kReferencePtS,
	kToGridS,
	kChooseSelCriteraS,
	kChooseRefCriteraS,
	kInvalidCriteraS,
	kSelectRefObjS,
	kSelectRefPtS
};

/////////////////////////////////////////////////////////////////////////////
// CAlignmentOptionsDlg dialog


CAlignmentOptionsDlg::CAlignmentOptionsDlg(AlignMode mode,AlignMask selMask,AlignMask refMask,CWnd* pParent /*=NULL*/)
	: CDialog(CAlignmentOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlignmentOptionsDlg)
	//}}AFX_DATA_INIT
	m_mode=mode;
	m_selected=selMask;
	m_reference=refMask;
}

void CAlignmentOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlignmentOptionsDlg)
	DDX_Control(pDX, IDC_REFERENCE, m_Ref);
	DDX_Control(pDX, IDC_SELECTED_DATUM, m_SelDatum);
	DDX_Control(pDX, IDC_REFERENCE_DATUM, m_RefDatum);
	DDX_Control(pDX, IDC_FEEDBACK, m_Feedback);
	DDX_Control(pDX, IDC_NOTE, m_Note);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlignmentOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CAlignmentOptionsDlg)
	ON_BN_CLICKED(IDC_ALIGN_TO_OBJ, OnAlignToObj)
	ON_BN_CLICKED(IDC_ALIGN_TO_POINT, OnAlignToPoint)
	ON_BN_CLICKED(IDC_ALIGN_TO_GRID, OnAlignToGrid)
	ON_BN_CLICKED(IDC_REFERENCE_DATUM, OnReferenceDatum)
	ON_BN_CLICKED(IDC_SELECTED_DATUM, OnSelectedDatum)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SELECTED_OPTIONS,OnSelectedOptions)
	ON_BN_CLICKED(IDC_REFERENCE_OPTIONS,OnReferenceOptions)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlignmentOptionsDlg message handlers

BOOL CAlignmentOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SelOpt.SubclassDlgItem(IDC_SELECTED_OPTIONS,this);
	m_SelOpt.BuildMatrix(m_selected);
	m_SelOpt.EnableWindow(m_selected & alignMask_Datum?FALSE:TRUE);
	m_SelDatum.SetCheck(m_selected & alignMask_Datum?1:0);

	m_RefOpt.SubclassDlgItem(IDC_REFERENCE_OPTIONS,this);
	m_RefOpt.BuildMatrix(m_reference);
	m_RefOpt.EnableWindow(m_reference & alignMask_Datum?FALSE:TRUE);
	m_RefDatum.SetCheck(m_reference & alignMask_Datum?1:0);

	SetMode();
	UpdateAlignStatus();

	return TRUE;
}

short CAlignmentOptionsDlg::ValidAlignmentSelectionTest()
{
	if(m_selected==alignMask_None)
		return kChooseSelCriteraS;
	if(m_mode==alignMode_RefObj)
	{
		if(m_reference==alignMask_None)
			return kChooseRefCriteraS;
		//Check for impossible alignment selections
		if((m_selected & ~alignMask_VertCenters) == 0 // Only the Top or Bottom of selected are set
		&& (m_reference & ~alignMask_HorizCenters) == 0) // and Only the Left or Right of reference are set
			return kInvalidCriteraS;

		if ((m_selected & ~alignMask_HorizCenters) == 0 // Only the Left or Right of selected are set
		&& (m_reference & ~alignMask_VertCenters) == 0) // and only the Top or Bottom of reference are set
			return kInvalidCriteraS;
	}
	return 0;
}

void CAlignmentOptionsDlg::ShowHideRefGrid(bool showIt)
{
	UINT nShowCmd=showIt?SW_SHOW:SW_HIDE;
	m_Ref.ShowWindow(nShowCmd);
	m_RefDatum.ShowWindow(nShowCmd);
	m_RefOpt.ShowWindow(nShowCmd);
}

void CAlignmentOptionsDlg::DatumChanged(bool checkIt,AlignMask& nAlignSet)
{
	if(checkIt)
		nAlignSet|=alignMask_Datum;
	else
		nAlignSet&=~alignMask_Datum;
	UpdateAlignStatus();
}

void CAlignmentOptionsDlg::SetMode()
{
	UINT nID=0;
	switch(m_mode)
	{
		case alignMode_None:
			m_mode=alignMode_RefObj;
		case alignMode_RefObj:
			nID=IDC_ALIGN_TO_OBJ;
			break;
		case alignMode_RefPt:
			nID=IDC_ALIGN_TO_POINT;
			break;
		case alignMode_ToGrid:
			nID=IDC_ALIGN_TO_GRID;
			break;
	}
	if(nID)
	{
		((CButton*)GetDlgItem(nID))->SetCheck(1);
		ShowHideRefGrid(m_mode==alignMode_RefObj);
	}
}

void CAlignmentOptionsDlg::CreateTextDescription(CString &text,bool isReferenceGrid)
{
	short strId1=0,strId2=0,strId3=0,specialCase=-2;
	AlignMask nAlignSet=isReferenceGrid ? m_reference : m_selected;
	CAlignmentOptionsWnd* pOpt=isReferenceGrid ? &m_RefOpt : &m_SelOpt;

	// These first two condtions should only be dealt with when isReferenceGrid is true
	if(isReferenceGrid && m_mode==alignMode_RefPt)
		strId1=kReferencePtS;
	else if(isReferenceGrid && m_mode==alignMode_ToGrid)
		strId1=kToGridS;
	else
	{
		specialCase=-1; // From here on in everything relates to either the selected or the reference shape - depending on the current grid
		if(nAlignSet & alignMask_Datum)
			strId1=kDatumS;
		else
		if(pOpt->TestMatrix(1,1))
			strId1=kMiddleS;
		else
		{
			specialCase=0;
			if(pOpt->TestMatrix(0,0))
			{
				strId1=kTopS;
				strId2=kLeftS;
			}
			else if(pOpt->TestMatrix(0,2))
			{
				strId1=kTopS;
				strId2=kRightS;
			}
			else if(pOpt->TestMatrix(2,0))
			{
				strId1=kBottomS;
				strId2=kLeftS;
			}
			else if(pOpt->TestMatrix(2,2))
			{
				strId1=kBottomS;
				strId2=kRightS;
			}
			else
			{
				if(pOpt->TestMatrix(0,1) && pOpt->TestMatrix(2,1))
				{
					strId1=kVerticalS;
					specialCase=1;
				}
				else
				{
					if(pOpt->TestMatrix(0,1))
					{
						strId2=kTopS;
						specialCase=2;
					}
					if(pOpt->TestMatrix(2,1))
					{
						strId2=kBottomS;
						specialCase=2;
					}
				}
				if(pOpt->TestMatrix(1,0) && pOpt->TestMatrix(1,2)) // Can never occur if specialCase is already 1
				{
					strId1=kHorizontalS;
					specialCase+=1;
				}
				else
				{
					if(pOpt->TestMatrix(1,0))
					{
						strId3=kLeftS;
						specialCase+=2;
					}
					if(pOpt->TestMatrix(1,2))
					{
						strId3=kRightS;
						specialCase+=2;
					}
				}
			}
		}
	}
	// There are only ever a maximum of 2 strId's generated above, the following code moves them into
	// strId1 and strId2
	if(strId1==0) // If no StrId1 then use StrId1 for StrId2
	{
		strId1=strId2;
		strId2=0;
		if(strId1==0) // If still no StrId1 then use StrId1 for StrId3
		{
			if(strId3!=0)
				strId1=strId3;
			else
				return; // There is no text to set
		}
	}
	if(strId2==0)
		strId2=strId3;

	const short kMaxTextBits=7;
	short t[kMaxTextBits] = {0,0,0,0,0,0,0};

	t[0]=isReferenceGrid?kReferenceS:kAlignSelectedS;

	t[1]=strId1; // true for all special cases

	switch(specialCase)
	{
		case -2: t[0]=0; break; // remove kReferenceS text
		case -1: break;
		case 0 : t[2]=strId2;t[3]=kCornerS; break;

		case 1 : t[2]=kCenterS; break;
		case 2 : t[2]=kHandEdgeS; break;

		case 3 : t[2]=kCenterS;t[4]=kAndS;t[5]=strId2;t[6]=kHandEdgeS; break;
		case 4 : t[2]=kAndS;t[3]=strId2;t[4]=kHandEdgeS; break;

	}

	// Join all the bits together
	short i;
	for(i=0;i<kMaxTextBits;++i)
	{
		if(t[i])
		{
			CString aStr;
			aStr.LoadString(kAlignTextBitsStrID+t[i]);
			text+=aStr;
		}
	}
}

void CAlignmentOptionsDlg::UpdateAlignStatus()
{
	short noteId=-1;
	CString theFeedBackText,theNoteText;
	short errStrId=ValidAlignmentSelectionTest();
	if(errStrId==0) // No ErrStr - do normal feedback
	{
		theFeedBackText.Empty();
		CreateTextDescription(theFeedBackText,false);
		CreateTextDescription(theFeedBackText,true);
		if(m_mode==alignMode_RefObj)
			noteId=kSelectRefObjS;
		else if(m_mode==alignMode_RefPt)
			noteId=kSelectRefPtS;
	}
	else
	{ // Report Err as Feedback
		theFeedBackText.LoadString(kAlignTextBitsStrID+errStrId);
	}

	AfxSetWindowText(m_Feedback.GetSafeHwnd(),theFeedBackText);

	if(noteId>0)
		theNoteText.LoadString(kAlignTextBitsStrID+noteId);
	AfxSetWindowText(m_Note.GetSafeHwnd(),theNoteText);

	GetDlgItem(IDOK)->EnableWindow(errStrId==0);
}

void CAlignmentOptionsDlg::ModeChanged(AlignMode newMode)
{
	m_mode=newMode;
	ShowHideRefGrid(m_mode==alignMode_RefObj);
	UpdateAlignStatus(); // Mode changes always effect the reference grid text
}

void CAlignmentOptionsDlg::OnSelectedOptions() 
{
	m_SelOpt.BuildAlignSet(m_selected);
	UpdateAlignStatus();
}

void CAlignmentOptionsDlg::OnReferenceOptions() 
{
	m_RefOpt.BuildAlignSet(m_reference);
	UpdateAlignStatus();
}

void CAlignmentOptionsDlg::OnAlignToObj() 
{
	if(IsDlgButtonChecked(IDC_ALIGN_TO_OBJ))
		ModeChanged(alignMode_RefObj);
}

void CAlignmentOptionsDlg::OnAlignToPoint() 
{
	if(IsDlgButtonChecked(IDC_ALIGN_TO_POINT))
		ModeChanged(alignMode_RefPt);
}

void CAlignmentOptionsDlg::OnAlignToGrid() 
{
	if(IsDlgButtonChecked(IDC_ALIGN_TO_GRID))
		ModeChanged(alignMode_ToGrid);
}

void CAlignmentOptionsDlg::OnReferenceDatum() 
{
	DatumChanged(m_RefDatum.GetCheck()?true:false,m_reference);
	m_RefOpt.EnableWindow(m_reference & alignMask_Datum?FALSE:TRUE);
}

void CAlignmentOptionsDlg::OnSelectedDatum() 
{
	DatumChanged(m_SelDatum.GetCheck()?true:false,m_selected);
	m_SelOpt.EnableWindow(m_selected & alignMask_Datum?FALSE:TRUE);
}
