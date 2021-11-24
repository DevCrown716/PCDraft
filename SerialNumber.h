#if !defined(SERIALNUMBER_H__INCLUDED_)
#define SERIALNUMBER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const long kUseSNFDefaultResourceBaseID=0;
//Note: This doesn’t mean the base ID is actually zero, it just tells SNF to
//use its internal base ID of 20000

void SetSNFResourceBaseID(const short inBaseID);

// Use this to change the base number of SNF’s internal resources
// Only needs to be called if you don’t want the default value

short SNFToRealResID(const short internalID);

// converts between SNF res id's to real world res id's}

CString CalculateCheckSum(const CString OnThisString);

/*
	{ Given a formatted serial number string, containing 24 alpha numerics and 5 hyphen      }
	{ characters, this routine will calculate a four digit checksum, returning this as       }
	{ the function result.                                                                   }

	{ OnThisString must have the following character setup in order for the correct          }
	{ checksum to be calculated,                                                             }

	{ OnThisString [21] := '0'                                                               }
	{ OnThisString [22] := '1'                                                               }
	{ OnThisString [23] := '2'                                                               }
	{ OnThisString [24] := '3'                                                               }
*/

void FormatTheSerialNumber(CString& ThisSerialNumber);

/*
	{ Given a string containing at least 24 alpha numeric characters, this routine will      }

	{ 1 : Convert all lower case alpha's to upper case                                       }
	{ 2 : Remove any remaining characters not in the range 'A'..'Z', '0'..'9'                }
	{ 3 : Add hyphens such that the resulting string format is AAAA-B000-1111-2222-HHHH-3333 }

	{ AAAA           = product code e.g. 3DWD, MPRR                                          }
	{ B              = revision stage A = Alpha, B = Beta, D = Development, V = Final        }
	{ 000            = Major, Minor and Revision Stage, e.g. 452 is version 4.5.2            }
	{ 1111 & 2222    = Copy number, encoded                                                  }
	{ HHHH           = Hexadecimal checksum                                                  }
	{ 3333           = Protection code and User count, encoded                               }

	{ Characters after the 24th alpha numeric are ignored                                    }
*/

bool IsBadSerialNumber(const CString ThisSerialNumber);

/*
	{ Given a formatted or unformatted serial number string, this routine validates the      }
	{ checksum contained therein and returns true if it is invalid.                          }
	
	{ If the format of ThisSerialNumber cannot be corrected properly, then the checksum      }
	{ will not be valid, and this function will return True.                                 }
*/

bool IsGoodSerialNumber(const CString ThisSerialNumber);

/*
	{ Simply the inverse result of IsBadSerialNumber                                         }
*/

bool GetSerialNumber();

#endif
