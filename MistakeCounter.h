// MistakeCounter.h: interface for the CMistakeCounter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISTAKECOUNTER_H__D95EDAE3_F5C9_11D3_98D9_00A0C9A21BD0__INCLUDED_)
#define AFX_MISTAKECOUNTER_H__D95EDAE3_F5C9_11D3_98D9_00A0C9A21BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMistakeCounter  
{
public:
	CMistakeCounter(int nThreshold=3) : m_nCounter(0),m_nThreshold(nThreshold) {}
	virtual ~CMistakeCounter() {}

	void Reset() { m_nCounter=0; }
	void ShowMessage(int nMessageString) { if(((++m_nCounter)%m_nThreshold)==0) AfxMessageBox(nMessageString,MB_OK); else MessageBeep(MB_ICONHAND); }

protected:
	int m_nCounter;
	int m_nThreshold;
};

#endif // !defined(AFX_MISTAKECOUNTER_H__D95EDAE3_F5C9_11D3_98D9_00A0C9A21BD0__INCLUDED_)
