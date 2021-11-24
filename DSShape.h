#ifndef __DSSHAPE_H__
#define __DSSHAPE_H__

#include "WrFixed.h"

#pragma pack(1)

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

enum DatumType
{
	kDefaultDatum=-3,
	kCenterDatum=-2,
	kFreeDatum=-1
};

enum Alignment
{
	kInsideLine,
	kCenterLine,
	kOutsideLine,
	kExtraLine
};

enum PenType
{
	kPenInvis = 0,
	kPenColor,			/* rgb color */
	kPenPat,			/* pattern */
	kPenPatEmbed
};

#define	Cap1Mask		0x0f
#define	Cap2Mask		0xf0
#define CapBothMask			(Cap1Mask | Cap2Mask)

#define	HairlineMask	0x80
#define	HairLineCntMask	0x60

#define pmUsePenType	0x01
#define pmUseStyle		0x02
#define pmUseEndCap1	0x04
#define pmUseEndCap2	0x08
#define pmUseEndCaps	0x0C
#define pmUseAlign		0x10
#define pmUseWidth		0x20
#define pmUsePenStyle	0x40
#define pmUseOpacity	   0x80 // Opacity/Transparency - Infinisys Ltd
#define pmAllButCaps	pmUsePenType|pmUseStyle|pmUseAlign|pmUseWidth|pmUsePenStyle|pmUseOpacity
#define pmUseAll		pmAllButCaps|pmUseEndCaps

#define fmAllButOpacity 0x01 // Opacity/Transparency - Infinisys Ltd
#define fmUseOpacity 0x02 // Opacity/Transparency - Infinisys Ltd
#define fmUseAll fmAllButOpacity|fmUseOpacity // Opacity/Transparency - Infinisys Ltd

struct PenModel
{
	BYTE m_nPenType;
	union
	{
		BYTE m_nStyleAlign;
		struct
		{
			BYTE m_nStyle:4;
			BYTE m_nAlignment:2;
			BYTE m_nAccurateWidth:1;
			BYTE m_nSquareEnds:1;
		};
	};
	union
	{
		BYTE m_nEndCaps;
		struct
		{
			BYTE m_nEndCap1:4;
			BYTE m_nEndCap2:4;
		};
	};
	BYTE m_nWidth;	/* 0x80 for hair, otherwise > 1 */
	union
	{
		COLORREF m_crColor;
		HBITMAP m_hDib;
		int m_nIndex;
	};

   // Opacity/Transparency - Infinisys Ltd
   int m_iPenOpacity; 

	PenModel();
	PenModel(const PenModel& pm);
	~PenModel();

	void Modify(const PenModel& pm,int nWhat);
	PenModel& operator=(const PenModel& pm);

	short EndCap1() const { return m_nEndCap1; }
	short EndCap2() const { return m_nEndCap2; }
	bool  AnyCaps() const { return (m_nEndCaps)?true:false; }
	short Dashing() const { return m_nStyle; }
	short HairlineCnt() const { return short((m_nWidth & HairLineCntMask)>>5); }
public:
	void makePenStyleBlade();
	void makePenStyleRound();
	bool isPenStyleRound() const;
};

enum FillType
{
	kNoFill=0,
	kFillColor,
	kFillPat,
	kFillPatEmbed
};

struct FillModel
{
	BYTE  m_nFillType;
	union
	{
		COLORREF m_crColor;
		HBITMAP m_hDib;
		int m_nIndex;
	};
   // Opacity/Transparency - Infinisys Ltd
   int   m_iFillOpacity; // 0 - 255

	FillModel();
	FillModel(const FillModel& fm);
	~FillModel();

	FillModel& operator=(const FillModel& fm);

   void ModifyFillModel(const FillModel & fm, int nWhat); // Opacity/Transparency - Infinisys Ltd

};

enum TextJustify
{
	kAlignLeft,
	kAlignCenter,
	kAlignRight
};

enum TextVAlign
{
	kAlignTop,
	kAlignMiddle,
	kAlignBottom
};

enum TextSpacing
{
	kSingleSpace,
	k1_5Space,
	kDoubleSpace
};

enum TextCase
{
	kCaseUpper,
	kCaseLower,
	kCaseTitle
};

#define tpUseFont		0x01
#define tpUseJust		0x02
#define tpUseVAlign		0x04
#define tpUseSpacing	0x08
#define tpUseCase		0x10
#pragma pack(push, ShapeStructs, 1)
struct TextParams
{
	LOGFONT m_lf;
	char m_nJust;
	char m_nVAlign;
	short m_nSpacing;
	short m_nCase;

	TextParams();
public:
	void Modify(const TextParams& tp,int nWhat);
};
struct ParallelAttrs
{
	short m_nAlign;
	short m_nEndCap;
	short m_nJoint;
	FillModel m_Fill;
	WrFixed m_fWidth;

	ParallelAttrs(BYTE nFillType=kFillColor);
};

struct ShapeAttributes
{
	short m_nFill;
	short m_nPen;
	bool m_bInfo;
	FillModel m_Fill;
	PenModel m_Pen;
	CString m_sInfo[5];

	short m_nPara;
	FillModel m_Para;

	ShapeAttributes();
	ShapeAttributes& operator=(ShapeAttributes& sa);
};
#pragma pack(pop, ShapeStructs)
// additional mfc specific helpers
CArchive& operator<<(CArchive&, const PenModel& );
CArchive& operator>>(CArchive&, PenModel& );
CArchive& operator>>(CArchive&, FillModel& fillModel);
CArchive& operator<<(CArchive&, const FillModel& fillModel);
CArchive& operator>>(CArchive&, ParallelAttrs& attrs);
CArchive& operator<<(CArchive&, const ParallelAttrs& attrs);
CArchive& operator>>(CArchive&, TextParams& params);
CArchive& operator<<(CArchive&, const TextParams& params);

#pragma pack()
#endif
