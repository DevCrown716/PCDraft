#include "stdafx.h"
#include "pcdraft.h"
#include "SerialNumber.h"
#include "RegistrationDlg.h"
#include "BetaDlg.h"
#include "NetGuard.h"
#include "Welcome/WelcomeWnd.h"
#include <ctype.h>
#include <time.h>

const CString kEncryptString="764219835076421983507642198350764219835076421983507642198350";
const short rbDefaultBaseID=20000;

short gResourceBaseID=0;

char NumToChar(short Num) // Input to this routine must be between 0 & 15
{
	if(Num>9)
		return Num-10+'A';
	else
		return Num+'0';
}

CString HexByteStr(BYTE Num)
{
	char theStr[3];
	theStr[0]=NumToChar((Num>>4)&0x0F);
	theStr[1]=NumToChar(Num&0x0F) ;
	theStr[2]='\0';
	return theStr;
}

CString	HexIntStr(short Num)
{
	return HexByteStr((Num>>8)&0x00FF)+HexByteStr(Num&0x00FF);
}

void SetSNFResourceBaseID(const short inBaseID)
{
	gResourceBaseID=inBaseID;
}

short SNFToRealResID(const short internalID)
{
	short baseID=gResourceBaseID;
	if(baseID==kUseSNFDefaultResourceBaseID)
		baseID=rbDefaultBaseID;
	return baseID+internalID;
}

CString CalculateCheckSum(const CString OnThisString)
{
	short ChkSum=7765 ;
	long length=OnThisString.GetLength();
	for(long Index=1;Index<=length;++Index)
	{
		short xOrWith=(BYTE)OnThisString[Index-1];
		ChkSum^=xOrWith ;
		long finished=ChkSum&0x0000000F;
		for(long Count=1;Count<=finished;++Count)		
			ChkSum=((ChkSum<<1)&0xFFFE)|((ChkSum>>15)&0x0001);
	}
	return HexIntStr(ChkSum);
}

void FormatTheSerialNumber(CString& ThisSerialNumber)
{
	CString Result;
	ThisSerialNumber.MakeUpper();
	for(LPCTSTR p=ThisSerialNumber;*p;++p)
	{
		if(isdigit(*p) || isupper(*p))
			Result+=*p;
	}
	if(Result.GetLength()<29)
		Result+=CString(' ',29-Result.GetLength());
	ThisSerialNumber=Result;
	Result=CString(' ',29);
	short Source=1;
	short Dest=1;
	do
	{
		for(int i=0;i<sizeof(long);i++)
			Result.SetAt(Dest+i-1,ThisSerialNumber.GetAt(Source+i-1));
		Source+=sizeof(long);
		Dest+=sizeof(long);
		if(Dest<29)
			Result.SetAt(Dest-1,'-');
		++Dest;
	}
	while(Dest<=29);
	ThisSerialNumber=Result;
}
/*
PDFT-V505-3543-7632-29EE-1842
PDFT-V500-0946-7642-B4D1-1742
PDFT-V500-0964-7642-987C-1742
PDFT-V500-5452-7632-CDEA-1842

PDPE-V500-3535-7642-A8CB-1842
PDPE-V500-3925-7642-DBDF-1842
PDPE-V500-8452-7642-63D6-1842
PDPE-V500-8451-7642-AD65-1842
PDPE-V500-3008-7641-23DE-1842
*/


bool IsBadSerialNumber(const CString ThisSerialNumber)
{
	CString FormattedSerialNumber;
	CString ToMatchSerialNumber;
	CString CheckText;

	FormattedSerialNumber=ThisSerialNumber ;
	FormatTheSerialNumber(FormattedSerialNumber);
	ToMatchSerialNumber=FormattedSerialNumber;

	ToMatchSerialNumber.SetAt(21-1,'0');
	ToMatchSerialNumber.SetAt(22-1,'1');
	ToMatchSerialNumber.SetAt(23-1,'2');
	ToMatchSerialNumber.SetAt(24-1,'3');

	CheckText=CalculateCheckSum(ToMatchSerialNumber);

	bool checkResult = 	FormattedSerialNumber[21-1]!=CheckText[0] ||
			FormattedSerialNumber[22-1]!=CheckText[1] ||
			FormattedSerialNumber[23-1]!=CheckText[2] ||
			FormattedSerialNumber[24-1]!=CheckText[3] ||
			FormattedSerialNumber[0]!='P' ||
			FormattedSerialNumber[1]!='D' ||
#ifndef _PERSONAL_EDITION
			FormattedSerialNumber[2]!='F' ||
			FormattedSerialNumber[3]!='T' ||
#else
			FormattedSerialNumber[2]!='P' ||
			FormattedSerialNumber[3]!='E' ||
#endif
//			FormattedSerialNumber[5]!='V' ||
			FormattedSerialNumber[6]!='6' ||
#ifdef _PERSONAL_EDITION
			FormattedSerialNumber[7]!='0' ||
#else
			FormattedSerialNumber[7]!='0' ||
#endif
			FormattedSerialNumber[8]!='0';

    if (checkResult)
    {
        // 
    }
    return checkResult;
}

bool IsGoodSerialNumber(const CString ThisSerialNumber)
{
	return !IsBadSerialNumber(ThisSerialNumber);
}

short ExtractUserCount(const CString FromThisSerialNumber)
{
	char SupplementaryString[4];
	SupplementaryString[0]=FromThisSerialNumber[28-1];
	SupplementaryString[1]=FromThisSerialNumber[29-1];
	SupplementaryString[2]=FromThisSerialNumber[26-1];
	SupplementaryString[3]=FromThisSerialNumber[27-1];

	char ResultString[]="xxxx";
	for(int Index=0;Index<4;Index++)
	{
		for(int Count=0;Count<10;Count++)
		{
			if(SupplementaryString[Index]==kEncryptString[2+Index+Count])
				ResultString[Index]='0'+Count;
		}
	}

	return atoi(ResultString)%316;
}

bool GetSerialNumber1()
{
	CRegistrationDlg theDlg;
	theDlg.m_sName=AfxGetApp()->GetProfileString("Registration","Name");
	theDlg.m_sOrganization=AfxGetApp()->GetProfileString("Registration","Organization");
	theDlg.m_sSerialNumber=AfxGetApp()->GetProfileString("Registration","SerialNumber");
	BOOL bForceDialog=AfxGetApp()->GetProfileInt("Registration","ForceDialog",0);
	if(IsBadSerialNumber(theDlg.m_sSerialNumber) || bForceDialog || GetAsyncKeyState(VK_COMMAND)<0)
	{
		// If we were started up for DDE restore the Show state
		if (AfxGetApp()->m_pCmdInfo != NULL)
		{
			AfxGetApp()->m_nCmdShow = (int)AfxGetApp()->m_pCmdInfo;
			AfxGetApp()->m_pCmdInfo = NULL;
			AfxGetApp()->m_pMainWnd->ShowWindow(AfxGetApp()->m_nCmdShow);
		}
		if(theDlg.DoModal()==IDOK && !IsBadSerialNumber(theDlg.m_sSerialNumber))
		{
			AfxGetApp()->WriteProfileString("Registration","Name",theDlg.m_sName);
			if(!theDlg.m_sOrganization.IsEmpty()) AfxGetApp()->WriteProfileString("Registration","Organization",theDlg.m_sOrganization);
			FormatTheSerialNumber(theDlg.m_sSerialNumber);
			AfxGetApp()->WriteProfileString("Registration","SerialNumber",theDlg.m_sSerialNumber);
			if(bForceDialog) AfxGetApp()->WriteProfileInt("Registration","ForceDialog",0);
		}
		else return false;
	}
	else FormatTheSerialNumber(theDlg.m_sSerialNumber);

	if(theDlg.m_sSerialNumber[5]!='V' && time(NULL)>1014854400) // beta expiration about 2/28/2002
	{
		CBetaDlg theDlg;
		theDlg.DoModal();
		return false;
	}
/* GK-: Use CNetGuard interface AskCopies
	CString theNetAsk;
	theNetAsk.Format("?%s",theDlg.m_sSerialNumber);
	((CPCDraftApp*)AfxGetApp())->m_pNetGuard->SendBroadcast(theNetAsk,ExtractUserCount(theDlg.m_sSerialNumber));
*/
	return true;
}



bool GetSerialNumber()
{
   WelcomeWnd theDlg;
   theDlg.m_strName = AfxGetApp()->GetProfileString("Registration", "Name");
   theDlg.m_strOrganization = AfxGetApp()->GetProfileString("Registration", "Organization");
   theDlg.m_strSerialNumber = AfxGetApp()->GetProfileString("Registration", "SerialNumber");
   BOOL bForceDialog = AfxGetApp()->GetProfileInt("Registration", "ForceDialog", 0);
   if (IsBadSerialNumber(theDlg.m_strSerialNumber) || bForceDialog || GetAsyncKeyState(VK_COMMAND) < 0)
   {
      // If we were started up for DDE restore the Show state
      if (AfxGetApp()->m_pCmdInfo != NULL)
      {
         AfxGetApp()->m_nCmdShow = (int)AfxGetApp()->m_pCmdInfo;
         AfxGetApp()->m_pCmdInfo = NULL;
         AfxGetApp()->m_pMainWnd->ShowWindow(AfxGetApp()->m_nCmdShow);
      }

      bool bOk = theDlg.DoModal() == IDOK;

      if (bOk)
      {

         bOk = !IsBadSerialNumber(theDlg.m_strSerialNumber);

      }

      if (bOk)
      {
         AfxGetApp()->WriteProfileString("Registration", "Name", theDlg.m_strName);
         if (!theDlg.m_strOrganization.IsEmpty()) AfxGetApp()->WriteProfileString("Registration", "Organization", theDlg.m_strOrganization);
         FormatTheSerialNumber(theDlg.m_strSerialNumber);
         AfxGetApp()->WriteProfileString("Registration", "SerialNumber", theDlg.m_strSerialNumber);
         if (bForceDialog) AfxGetApp()->WriteProfileInt("Registration", "ForceDialog", 0);
      }
      else return false;
   }
   else FormatTheSerialNumber(theDlg.m_strSerialNumber);

   if (theDlg.m_strSerialNumber[5] != 'V' && time(NULL) > 1014854400) // beta expiration about 2/28/2002
   {
      CBetaDlg theDlg;
      theDlg.DoModal();
      return false;
   }
   /* GK-: Use CNetGuard interface AskCopies
      CString theNetAsk;
      theNetAsk.Format("?%s",theDlg.m_sSerialNumber);
      ((CPCDraftApp*)AfxGetApp())->m_pNetGuard->SendBroadcast(theNetAsk,ExtractUserCount(theDlg.m_sSerialNumber));
   */
   return true;
}
