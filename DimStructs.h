#if !defined(_DIMSTRUCTS_H_INCLUDED_)
#define _DIMSTRUCTS_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

enum {			// DimFormatType
	kLinear,
	kRadial,
	kAngular
};

enum {			// TextDisplayType
	kHorizontal,
	kAlongLine,
	kAbove,
	kBelow
};

enum {			// LeaderType;
	kNoLeader,
	kLeft,
	kRight
};


enum {			// RDSymbolType;
	kNoSymbol,
	kLeading,
	kTrailing
};

enum {			// ToleranceType;
	kNoTolerance,
	kOne,
	kTwo,
	kLimits
};

#define dfUseRounoff		0x01
#define dfUseUseWitness		0x02
#define dfUseArrows			0x04
#define dfUseTextCentered	0x08
#define dfUseTextFramed		0x0C
#define dfUseTextDisplay	0x10
#define dfUseLeader			0x20
#define dfUseRDSymbol		0x40
#define dfUseTolerance		0x80
#define dfUseTolerance1		0xC80
#define dfUseTolerance2		0x100
#define dfUseLabelText		0x200
#define dfUseUseLabel		0x400
#define dfUseStandard		0x8000

struct DimFormat{
	short	nType;				/* Linear, Radial, Angular */
	bool	bTextCentered;
	bool	bTextFramed;
	bool	bArrowsInside;
	bool	bUseLabel;
	bool	bUseWitness;
	short	nTextDisplay;		/* Horizontal, Along Line, Above, Below */
	short	nLeader;			/* None, Left, Right */
	short	nRDSymbol;			/* None, Leading, Trailing */
	short	nTolerance;			/* None, One, Two, Limits */
	double	Tolerance1;
	double	Tolerance2;
	CString	sLabel;
	short	nRoundoff;
public:
	DimFormat();
	void FillDefaults(short type,short dsType);
	short Compare(const DimFormat& df);
	void Modify(const DimFormat& df,short nWhat);
	DimFormat& operator=(DimFormat &inFmt);
};

CArchive& operator<<(CArchive&, const DimFormat& );
CArchive& operator>>(CArchive&, DimFormat& );

enum {
	kDsEnglish,
	kDsMetric
};

struct DimStandard {
	short		nType;						/* ANSI, Metric */
	double		WitnessExt;
	double		WitnessLineGap;
	double		DimTextGap;
	double		LeaderLen;
	double		OutsideLineLen;
	int			nToleranceTextScale;
	int			nToleranceSpaceScale;
	double		CircleCenterLen;
	double		CircleCenterGap;
	double		CircleCenterExt;
public:
	void FillDefaults(short type);
	bool Compare(const DimStandard& ds);
	DimStandard& operator=(DimStandard &inStd);
	void ConvertForUnits(short nDsType);
};

CArchive& operator<<(CArchive&, const DimStandard& );
CArchive& operator>>(CArchive&, DimStandard& );

#endif