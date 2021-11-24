// MDReport.h: interface for the CMDReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDREPORT_H__B9B27296_B7A7_11D2_B545_E15F3229EE7E__INCLUDED_)
#define AFX_MDREPORT_H__B9B27296_B7A7_11D2_B545_E15F3229EE7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define kEmptyRow			0

#define	kTextRow			1
#define kCriteriaRow		2
#define	kFunctionRow		3

enum RowFunction
{
	kFunctArea = 1,
	kFunctCount,
	kFunctHeight,
	kFunctLength,
	kFunctPerimeter,
	kFunctWidth,
	kFunctXDim,
	kFunctYDim,
	kFunctTotalName,
	kFunctTotalF2,
	kFunctTotalF3,
	kFunctTotalF4,
	kFunctTotalF5
};

#define kFirstFunction	kFunctArea
#define kLastFunction	kFunctTotalF5
#define kNumFunctions	13

enum FieldRefNum
{
	kUnDefined = 0,
	kNameField,
	kF2Field,
	kF3Field,
	kF4Field,
	kF5Field,
	kFunctionNameField,
	kResultField = 128
};

enum FieldJust
{
	kJustLeft	= DT_LEFT,
	kJustRight	= DT_RIGHT,
	kJustCenter	= DT_CENTER
};

enum LinkType
{
	kLinkNone = 0,
	kLinkResults,
	kLinkAll
};

class CMDReportRow : public CObject
{
	DECLARE_SERIAL(CMDReportRow)
public:
	CMDReportRow();
	virtual ~CMDReportRow();

	virtual void	Serialize(CArchive& ar);

	short	m_nRowType;
	short	m_nFunction;
	short	m_nFieldRefNum[6];
	BYTE	m_nJust[6];
	CString	m_sRowText[6];
};

class CPCDraftDoc;

class CMDReport : public CObject  
{
	DECLARE_SERIAL(CMDReport)
public:
	CMDReport(bool bInit=true);
	virtual ~CMDReport();

	virtual void	Serialize(CArchive& ar);

	bool Calculate(CPCDraftDoc* pDoc, bool bClone=false);
	void Clone(int nCopyType=-1);
	void Unlink();
	bool IsLinked() const { return m_nLinkType!=kLinkNone; }
	DWORD GetTag() { return m_dwTag; }
	void GenerateTag();

	CString m_sTitle;
	int	m_nColumnWidths[6];
	LOGFONT m_LogFont;
	CObArray m_Rows;
	CString	m_sLinkFile;
	int m_nLinkType;
	int m_nLinkUnits;
	DWORD m_dwTag;
};

#endif // !defined(AFX_MDREPORT_H__B9B27296_B7A7_11D2_B545_E15F3229EE7E__INCLUDED_)
