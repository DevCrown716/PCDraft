// DrawPrim.h: interface for the CDrawPrim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWPRIM_H__DACFC5B3_BACF_11D2_BB59_000000000000__INCLUDED_)
#define AFX_DRAWPRIM_H__DACFC5B3_BACF_11D2_BB59_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSShape.h"
#include "gu.h"
#include "Mapper.h"
#include "MDFiloPoly.h"

const WrFixed kFixedPie = WrFixedFromFixed ( 0x32440L ) ; /* Fixed '¹' (3.1416015625) (see IM: IV-64) */
const WrFixed kFixedPie2 = WrFixedFromFixed ( 0x19220L ) ; /* Fixed '¹'/2 */
const WrFixed kFixedPie4 = WrFixedFromFixed ( 0xC910L ) ; /* Fixed '¹'/4 */

//class CMapper;

const short kDimLineOut		= 	12 ;	// from DimStruct.h see also DrawPrim.c /* pixels either side of witness lines with outside dimension */
#define kDimEndmarkOut			64		// from DimStruct.h
#define	kEnd1				0x1000
#define	kEnd2				0x2000
#define	kBothEnd			0x3000

#define	kBezierDraw			0x0001
#define kBezierSelectCheck	0x0002

class CDraftContext;

struct CArcStruct;

class CDrawPrim  
{
public:
	bool isCurrentDashFilled(){
		return (m_DashesIdx&1) == 0;
	}
	enum 
	{
		kMaxDashCodes		= 14,
		kDashCodeEntrySize	= 16, // in bytes
		kMaxDashLines		= 15,
		kDashCodeEntryCount	= (kMaxDashLines + 1),

		kCopyMapByteWidth 	= 4,
		kCopyMapByteShift 	= 2,
		kBitMapSize			= kCopyMapByteWidth * kCopyMapByteWidth * 8
		
	};

	struct EndcapRec 
	{
		short		penWidth;
		short		type;
		short		opcode;
		short		width, height, nose, tail;
		short		offset;
	};

#ifdef DEBUG
   int                  m_iDebug;
   bool                 m_bPrev1; // Infinisys Ltd. 2021 - Debug Provision
#endif
   pointf               m_pfHead;
   pointf               m_pfTail;
   bool                 m_bUsePolyHelperIfDashed; // Infinisys Ltd. 2021
   CMDFiloPoly          m_helperPoly;
	typedef uchar		   DashStyle[kDashCodeEntrySize];
	typedef DashStyle	   DashTable[kDashCodeEntryCount];

	CDrawPrim();
	virtual ~CDrawPrim();

	void	setDashTable(const DashTable* inDashStyles);
	void	dashLine(CDraftContext* pDC,const pointd& startPt,const pointd& endPt, bool bForceDashing=false);
	void dashLineTo(CDraftContext* pDC,const pointd& endPoint);

	void	drawEndcap(CDraftContext* pDC, const LongPoint& inlPt1,	const LongPoint& inlPt2,	short endcap, unsigned short map, const PenModel& penModel, bool isCurve, bool isGrayed, Coord outsideLineLen);
	void	lineCapped (CMapper* pMapper, LongPointPtr p1, LongPointPtr p2, short cap1, short cap2, short whichCap, short pmPenW, LongPointPtr	newP1, LongPointPtr	newP2);
	void	makeButtCaps (short	penWidth, register const pointd& p1, register const pointd& p2, register pointd		*newP1, register pointd	*newP2);
	void	LineBorderPos (CMapper* pMapper, PenModel* penModel, short cap1, short cap2, unsigned short adjustEnd, LongPoint *p1, LongPoint *p2, LongPoint *newP1, LongPoint *newP2);

	void	drawArc ( CDraftContext* pDC, LongPointPtr	pCenter, Coord pHRad, Coord	pVRad, const LongPoint& lptStart, const LongPoint& lptEnd, Angle pRot, bool pIsFine,int nDirection, bool bAdjustAlign,
      bool bClose); // Infinisys Ltdl 2021.

	void	drawEllipse ( CDraftContext* pDC, LongPointPtr pCenter, Coord pHRad, Coord	pVRad, Angle pRot, bool pIsFine);

	void	drawBezier (CDraftContext* pDC, pointd* vBezier);
	bool	checkBezier (const pointd& pPoint1,const pointd& pPoint2, const pointd &pPoint3,const pointd& pPoint4,const pointd& testPt, short penSize);

protected:
	void	doDashLine(CDraftContext* pDC, const pointd& startPt,const pointd& endPt, const uchar *pCode, short penWidth);
//private:
	void drawFilledHalfCircle(CDraftContext* pDC, const LongPoint& lpt, const pointd& ptStart, const pointd& ptEnd);
	void arcSlowOutline(CDraftContext* pDC, CArcStruct& inArc);
	void getDashStr(short index, uchar* dashCodeStr);
	void	arcSlowNew ( CDraftContext* pDC, CArcStruct& inArc, bool bAdjustAlign,
      bool bClose = false);  // Infinisys Ltd. 2021

	void	initDashCodes(const pointd& inStartPt, short dashCode/*, short halfPenWidth*/);
   //void	initDashCodes(const short& inStartPt, short dashCode/*, short halfPenWidth*/);
	short	m_DashesIdx;
	//short	m_PixelCount;
   double m_dDistanceWeNeedToCover;
	short	m_DLSelect;
   pointd	m_CurrentPt;
	const DashTable*	m_DashTable;
   friend class CDraftContext;
};

#endif // !defined(AFX_DRAWPRIM_H__DACFC5B3_BACF_11D2_BB59_000000000000__INCLUDED_)
