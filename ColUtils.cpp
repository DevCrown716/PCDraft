/******************************************************

	ColUtils.c

	Copyright © 1992-98 by Microspot Ltd. All Rights Reserved.

	Created					: 02/18/92

******************************************************/

#include "stdafx.h"

#include "ColUtils.h"

/*---------------------------------------------------------------------------------*/

Angle m360Ang ( Angle fdAngle)
{
  while ( fdAngle < FixedZero )
    fdAngle += k360;
  while ( fdAngle >= k360 )
    fdAngle -= k360;

  return ( fdAngle );
}/* m360Ang */



