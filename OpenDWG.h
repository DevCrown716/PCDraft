// OpenDWG.h: interface for the COpenDWG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENDWG_H__5F961EB4_B574_11D4_AE51_000021E637AD__INCLUDED_)
#define AFX_OPENDWG_H__5F961EB4_B574_11D4_AE51_000021E637AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <ad2.h>

class COpenDWG  
{
public:
	COpenDWG();
	virtual ~COpenDWG();
private:
	static unsigned char *pInitData;
	static int nPos;

	static void* OpenInitFile(const char*);
	static unsigned char ReadInitFileChar(void*);
	static int ReadInitFileChars(void *buf, unsigned int size, unsigned int num, void *f);
//	static int SeekInitFile(void *f, OdaLong offset, int whence);
	static int CloseInitFile(void *f);
};

#endif // !defined(AFX_OPENDWG_H__5F961EB4_B574_11D4_AE51_000021E637AD__INCLUDED_)
