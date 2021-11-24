// MDArea.h: interface for the CMDArea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDAREA_H__2646CA31_F632_11D2_A08D_00A0C9A0C319__INCLUDED_)
#define AFX_MDAREA_H__2646CA31_F632_11D2_A08D_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDDimStr.h"

class CMapper;
class CDraftContext;

class CMDArea : public CMDDimStr
{
	DECLARE_SERIAL(CMDArea)
public:
   CMDArea(bool bAttached=true,CMDShape* pOwner=NULL);

	void Serialize(CArchive& ar);

// operations
	void DoShapeRotate(Angle angle, short nAbout, const LongPoint& lptRef) {}
	void DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2);
	void DoShapeFlip(bool bVertical,LongPoint* pLpt=NULL);
// MacDraft
	int SaveMacDraft(PhIOContext& ctx);
// Infinisys Ltd
   virtual int SavePCDraft55(::format55::PhIOContext& ctx);
   // MD6.2.1 - Infinisys Ltd.
   virtual int SaveMacDraft620(::formatMD60::PhIOContext& ctx);
   // MD7.0 - Infinisys Ltd.
   virtual int SaveMacDraft70(::formatMD70::PhIOContext& ctx);
};

#endif // !defined(AFX_MDAREA_H__2646CA31_F632_11D2_A08D_00A0C9A0C319__INCLUDED_)
