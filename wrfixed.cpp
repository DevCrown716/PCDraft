#include "stdafx.h"
#include "wrfixed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char WrFixedSizeValidityCheck=1/((sizeof(WrFixed)==4)?1:0);

