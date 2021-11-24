/*
K32EXP.C -- Get32ProcAddress
Win32 code to import by ordinal from KERNEL32.DLL in Windows 95

After Andrew Schulman wrote Unauthorized Windows 95 (IDG Books, 1994), KERNEL32.DLL
stopped exporting undocumented Win32 functions such as VxDCall() and
GetpWin16Lock() by name. The functions discussed in *Unauthorized*
continue to be exported by ordinal (for example, VxDCall is
KERNEL32.1 and GetpWin16Lock is KERNEL.93). However, KERNEL32 does
not allow imports by ordinal (Message from debug version:
"GetProcAddress: kernel32 by id not supported").

This module provides GetK32ProcAddress() to support import by ordinal
from KERNEL32. There's nothing undocumented in here, except for the
ordinal numbers themselves. GetModuleHandle() returns the address of
the executable image (see Matt Pietrek in *Microsoft Systems Journal*,
September 1995, p. 20), and the image is documented in the PE (Portable
Executable) file format.
*/ 
#include "stdafx.h" 
#include <windows.h>
#include "k32exp.h"

#define ENEWHDR     0x003CL         /* offset of new EXE header */
#define EMAGIC      0x5A4D          /* old EXE magic id:  'MZ'  */
#define PEMAGIC     0x4550          /* NT portable executable */

#define GET_DIR(x)  (hdr->OptionalHeader.DataDirectory[x].VirtualAddress)
   
DWORD  WINAPI GetK32ProcAddress(int ord)
{
    static HANDLE hmod = 0;
    IMAGE_NT_HEADERS *hdr;
    IMAGE_EXPORT_DIRECTORY *exp;
    DWORD *AddrFunc;
    WORD enewhdr, *pw;
    int did_load = 0;
    BYTE *moddb;

    if (hmod == 0)      // one-time static init
        hmod = GetModuleHandle("KERNEL32");
    if (hmod == 0)      // still
        return 0;
    
    moddb = (BYTE *) hmod;
    pw = (WORD *) &moddb[0];
    if (*pw != EMAGIC)              
        return 0;
    pw = (WORD *) &moddb[ENEWHDR];
    enewhdr = *pw;
    pw = (WORD *) &moddb[enewhdr];
    if (*pw != PEMAGIC)             
        return 0;
    hdr = (IMAGE_NT_HEADERS *) pw;
    
    // Note: offset from moddb, *NOT* from hdr!
    exp = (IMAGE_EXPORT_DIRECTORY *) (((DWORD) moddb) +
        ((DWORD) GET_DIR(IMAGE_DIRECTORY_ENTRY_EXPORT)));
    AddrFunc = (DWORD *) (moddb + (DWORD) exp->AddressOfFunctions);

    // should verify that e.g.:
    // GetProcAddress(hmod, "VirtualAlloc") == GetK32ProcAddress(710);
    
    ord--;  // table is 0-based, ordinals are 1-based
    if (ord < (int) exp->NumberOfFunctions)
        return ((DWORD) (moddb + AddrFunc[ord]));
    else
        return 0;
}

